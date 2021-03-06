// © 2021 Matthew Barham. All Rights Reserved.


#include "Actors/BGSplineStructure.h"


#include "Actors/BGPawn.h"
#include "Actors/BGStructure.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABGSplineStructure::ABGSplineStructure()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	// Need to have Scene Component as root, so that everything moves properly!
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneComponent;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline Component"));
	SplineComponent->SetupAttachment(RootComponent);
	SplineComponent->SetIsReplicated(true);
	SplineComponent->SetLocationAtSplinePoint(1, FVector(50.f, 0.f, 0.f), ESplineCoordinateSpace::Local, true);
	SplineComponent->SetSplinePointType(0, ESplinePointType::Linear);
	SplineComponent->SetSplinePointType(1, ESplinePointType::Linear);

	bLocked = false;
}

void ABGSplineStructure::RestoreInstanceMeshAtIndex_Implementation(int const& Index, FTransform const& Transform,
                                                                   FString const& InstanceName)
{
	auto Instance = GetInstancedStaticMeshComponentByString(InstanceName);

	if (Instance)
	{
		auto const NewTransform = Transform.GetRelativeTransform(this->GetActorTransform());
		Instance->AddInstance(NewTransform);

		// Turn BoxCollision back on
		CollisionBoxes[Index]->SetCollisionProfileName("StructureBoxCollision");
		CollisionBoxes[Index]->SetCanEverAffectNavigation(true);
		DisabledCollisionBoxes.RemoveSingle(CollisionBoxes[Index]);
	}
}

void ABGSplineStructure::SpawnStructureActorAtIndex_Implementation(int const& Index,
                                                                   TSubclassOf<ABGStructure>
                                                                   StructureClassToSpawn,
                                                                   FTransform const& SpawnTransform,
                                                                   FString const& OldInstanceName)
{
	if (StructureClassToSpawn)
	{
		// Prepare a Structure to spawn using the passed parameter
		ABGStructure* StructureToSpawn = GetWorld()->SpawnActorDeferred<ABGStructure>(
			StructureClassToSpawn->GetDefaultObject()->GetClass(), SpawnTransform);
		StructureToSpawn->SetParentSplineStructure(this);
		StructureToSpawn->SetIndexOnSplineStructure(Index);

		// Spawn the new Structure and add it to the SplineStructure's array
		if (auto Structure = Cast<ABGStructure>(UGameplayStatics::FinishSpawningActor(StructureToSpawn, SpawnTransform))
		)
		{
			SpawnedStructures.Add(Structure);
			Structure->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		}
	}
}

void ABGSplineStructure::ModifyInstanceMeshAtIndex_Implementation(int const& Index,
                                                                  FString const& NewInstanceName,
                                                                  UStaticMesh* StaticMesh,
                                                                  UMaterialInstance* MaterialInstance,
                                                                  FString const& OldInstanceName)
{
	if (StaticMesh && MaterialInstance)
	{
		FTransform SavedTransform;

		auto const OldInstance = GetInstancedStaticMeshComponentByString(OldInstanceName);

		// Find the Old Instance (such as "WallInstance"), save its transform and then remove it
		if (OldInstance)
		{
			OldInstance->GetInstanceTransform(Index, SavedTransform, false);
			SavedTransform.SetLocation(SavedTransform.GetLocation() + FVector(0.f, 0.f, 0.1f));

			RemoveInstanceMeshAtIndex(Index);

			// CollisionBoxes[Index]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		// Check if we already have an InstancedStaticMeshComponent with the new name (such as "DoorInstance")
		// If we do, add to that one instead of making a new one
		for (auto InstanceComponent : GetInstanceComponents())
		{
			if (InstanceComponent && InstanceComponent->GetName().Equals(NewInstanceName))
			{
				if (auto CastInstance = Cast<UInstancedStaticMeshComponent>(InstanceComponent))
				{
					CastInstance->AddInstance(SavedTransform);
					return;
				}
			}
		}

		// Otherwise...make a new InstancedStaticMeshComponent with the new InstanceName
		auto NewInstance = CreateInstancedStaticMeshComponent(NewInstanceName, StaticMesh,
		                                                      MaterialInstance);
		NewInstance->AddInstance(SavedTransform);
	}
}

void ABGSplineStructure::SetStructurePhysicsAndCollision_Implementation(bool const bGravityOn,
                                                                        ECollisionEnabled::Type const CollisionType)
{
	auto CurrentInstanceComponents = this->GetInstanceComponents();
	if (CurrentInstanceComponents.Num() > 0)
	{
		for (auto Instance : CurrentInstanceComponents)
		{
			if (auto InstancedCast = Cast<UInstancedStaticMeshComponent>(Instance))
			{
				InstancedCast->SetEnableGravity(bGravityOn);
				InstancedCast->SetCollisionEnabled(CollisionType);
				CollisionType == ECollisionEnabled::Type::QueryAndPhysics
					? InstancedCast->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block)
					: InstancedCast->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
			}
		}
	}
}

void ABGSplineStructure::UpdateSplineStructureMesh_Implementation(int const& PointIndex)
{
	CleanupInstanceComponents(PointIndex);

	if (StructureData.WallStaticMesh && StructureData.WallMaskedMaterialInstance)
	{
		UpdateWallMesh();
	}

	if (StructureData.CornerStaticMesh && StructureData.CornerMaskedMaterialInstance)
	{
		UpdateCornerMesh();
	}

	UpdateComponentTransforms();

	// SetStructurePhysicsAndCollision(true, ECollisionEnabled::QueryAndPhysics);
}

void ABGSplineStructure::SetLocationOfSplinePoint_Implementation(int const& PointIndex, FVector const& NewLocation)
{
	SplineComponent->SetLocationAtSplinePoint(PointIndex, NewLocation, ESplineCoordinateSpace::World, true);
}

void ABGSplineStructure::AddPointToSplineAtIndex_Implementation(FVector const& Location, int const& Index)
{
	SplineComponent->AddSplinePointAtIndex(Location, Index, ESplineCoordinateSpace::World, true);
	SplineComponent->SetSplinePointType(Index, ESplinePointType::Linear, true);
}

// Called when the game starts or when spawned
void ABGSplineStructure::BeginPlay()
{
	Super::BeginPlay();

	SetReplicatingMovement(true);
}

void ABGSplineStructure::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	UpdateSplineStructureMesh();
}

void ABGSplineStructure::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABGSplineStructure, StructureData)
	DOREPLIFETIME(ABGSplineStructure, SpawnedStructures)
	DOREPLIFETIME(ABGSplineStructure, UniqueCollisionNumber)
	DOREPLIFETIME(ABGSplineStructure, bLocked)
}

void ABGSplineStructure::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                        const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap Begin!"))

	if (OtherActor->IsA(ABGPawn::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Overlap w/ ABGPawn!"))

		if (auto InstancedComponent = GetInstancedStaticMeshComponentByString("WallInstance"))
		{
			InstancedComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
		}

		if (OverlappedComp)
		{
			OverlappedComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
		}
	}
}

void ABGSplineStructure::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap End!"))

	if (OtherActor->IsA(ABGPawn::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("End Overlap w/ ABGPawn!"))

		if (auto InstancedComponent = GetInstancedStaticMeshComponentByString("WallInstance"))
		{
			InstancedComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		}

		if (OverlappedComponent && !DisabledCollisionBoxes.Contains(Cast<UBoxComponent>(OverlappedComponent)))
		{
			OverlappedComponent->SetCollisionProfileName("StructureBoxCollision");
		}
	}
}

void ABGSplineStructure::CleanupInstanceComponents(int const& PointIndex)
{
	// Can we remove ONLY the parts of the wall that need updating? The code below is semi-functional but has problems.

	// if (PointIndex > 0)
	// {
	// 	auto WallInstance = GetInstancedStaticMeshComponentByString("WallInstance");
	// 	auto WallBaseInstance = GetInstancedStaticMeshComponentByString("WallBaseInstance");
	// 	auto CornerInstance = GetInstancedStaticMeshComponentByString("CornerInstance");
	// 	auto CornerBaseInstance = GetInstancedStaticMeshComponentByString("CornerBaseInstance");
	// 	if (WallInstance->IsValidLowLevel() && WallBaseInstance->IsValidLowLevel())
	// 	{
	// 		auto const LocationOne = SplineComponent->GetLocationAtSplinePoint(
	// 			PointIndex, ESplineCoordinateSpace::Local);
	// 		auto const LocationTwo = SplineComponent->GetLocationAtSplinePoint(
	// 			PointIndex - 1, ESplineCoordinateSpace::Local);
	// 		auto const Distance = FVector::Dist(LocationOne, LocationTwo);
	// 		int const TotalInstancesBetweenPoints = FMath::TruncToInt(Distance / 100.f) - 1;
	// 		for (int i{WallInstance->GetInstanceCount()}; i > TotalInstancesBetweenPoints; --i)
	// 		{
	// 			UE_LOG(LogTemp, Warning, TEXT("Removing wall instance %d"), i)
	// 			WallInstance->RemoveInstance(i);
	// 			WallBaseInstance->RemoveInstance(i);
	// 		}
	// 		for (int i{CornerInstance->GetInstanceCount()}; i > CornerInstance->GetInstanceCount() - PointIndex; --i)
	// 		{
	// 			UE_LOG(LogTemp, Warning, TEXT("Removing corner instance %d"), i)
	// 			CornerInstance->RemoveInstance(i);
	// 			CornerBaseInstance->RemoveInstance(i);
	// 		}
	// 	}
	// }

	auto CurrentInstanceComponents = GetInstanceComponents();

	UE_LOG(LogTemp, Warning, TEXT("Removing all instances"))

	if (CurrentInstanceComponents.Num() > 0)
	{
		for (auto Instance : CurrentInstanceComponents)
		{
			if (Instance && !Instance->IsPendingKillOrUnreachable())
				Instance->DestroyComponent();
		}
		CurrentInstanceComponents.Empty();
	}

	if (CollisionBoxes.Num() > 0)
	{
		for (auto Box : CollisionBoxes)
		{
			if (Box && !Box->IsPendingKillOrUnreachable())
			{
				Box->DestroyComponent();
			}
		}
		CollisionBoxes.Empty();
	}

	if (DisabledCollisionBoxes.Num() > 0)
	{
		for (auto Box : DisabledCollisionBoxes)
		{
			if (Box && !Box->IsPendingKillOrUnreachable())
			{
				Box->DestroyComponent();
			}
		}
		DisabledCollisionBoxes.Empty();
	}

	if (SpawnedStructures.Num() > 0)
	{
		for (auto Structure : SpawnedStructures)
		{
			if (Structure && !Structure->IsPendingKillOrUnreachable())
			{
				Structure->Destroy();
			}
		}
		SpawnedStructures.Empty();
	}
}

void ABGSplineStructure::UpdateWallMesh()
{
	auto WallInstancedComponent = GetInstancedStaticMeshComponentByString("WallInstance");

	if (!WallInstancedComponent->IsValidLowLevel() || WallInstancedComponent->IsPendingKillOrUnreachable())
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateWallMesh(): Creating new CornerInstance"))

		WallInstancedComponent = CreateInstancedStaticMeshComponent(
			"WallInstance", StructureData.WallStaticMesh,
			StructureData.WallMaskedMaterialInstance);
		WallInstancedComponent->SetCollisionProfileName("Structure");
	}

	auto BaseInstancedComponent = GetInstancedStaticMeshComponentByString("WallBaseInstance");

	if (!BaseInstancedComponent->IsValidLowLevel() || WallInstancedComponent->IsPendingKillOrUnreachable())
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateWallMesh(): Creating new WallBaseInstance"))

		BaseInstancedComponent = CreateInstancedStaticMeshComponent(
			"WallBaseInstance", StructureData.BaseStaticMesh,
			StructureData.BaseMaterialInstance);
		BaseInstancedComponent->SetCollisionProfileName("Base");
	}

	int LoopCount{1};

	for (int i{}; i <= SplineComponent->GetNumberOfSplinePoints() - 2; ++i)
	{
		auto LocationOne = SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		auto LocationTwo = SplineComponent->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);

		auto const Distance = FVector::Dist(LocationOne, LocationTwo);
		// don't hardcode repeat-width (eventually remove and replace with globally set variable)
		auto const RepeatWidth{100.f};

		int const Integral = FMath::TruncToInt(Distance / 100.f) - 1;
		UE_LOG(LogTemp, Warning, TEXT("Integral: %i"), Integral)

		// Builds base meshes between Spline Points
		for (int j{}; j <= Integral; ++j)
		{
			FTransform BaseTransform{};

			const auto SplineLocation = SplineComponent->GetLocationAtDistanceAlongSpline(
				LoopCount * RepeatWidth, ESplineCoordinateSpace::Local);
			const auto SplineRotation = SplineComponent->GetRotationAtDistanceAlongSpline(
				LoopCount * RepeatWidth, ESplineCoordinateSpace::Local);

			FVector BaseLocation{};
			BaseLocation.X = SplineLocation.X;
			BaseLocation.Y = SplineLocation.Y;
			BaseLocation.Z = SplineLocation.Z;

			FRotator BaseRotation{};
			BaseRotation.Pitch = SplineRotation.Pitch + StructureData.Transform.GetRotation().Rotator().Pitch;
			BaseRotation.Yaw = SplineRotation.Yaw + StructureData.Transform.GetRotation().Rotator().Yaw;
			BaseRotation.Roll = SplineRotation.Roll + StructureData.Transform.GetRotation().Rotator().Roll;

			BaseTransform.SetLocation(BaseLocation);
			BaseTransform.SetRotation(FQuat(BaseRotation));
			BaseTransform.SetScale3D(StructureData.Transform.GetScale3D());


			++LoopCount;
			BaseInstancedComponent->AddInstance(BaseTransform);

			if (BaseTransform.GetLocation() == LocationOne || BaseTransform.GetLocation() == LocationTwo)
			{
				UE_LOG(LogTemp, Warning, TEXT("Corner Point, Skipping Wall Mesh"))
				continue;
			}

			FTransform StructureTransform{BaseTransform};

			int RandomNumber = FMath::RandRange(0, 10);
			StructureTransform.SetRotation(
				FQuat(StructureTransform.Rotator().Add(0.f, RandomNumber < 4 ? 0.f : 180.f, 0.f)));
			StructureTransform.SetLocation(StructureTransform.GetLocation() + FVector(0.f, 0.f, 0.1f));

			WallInstancedComponent->AddInstance(StructureTransform);

			// Create a Box Collision for each instance
			auto BoxCollision = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass(),
			                                             FName(TEXT(
				                                             "Collision" + FString::FromInt(UniqueCollisionNumber++))),
			                                             RF_Transient);
			BoxCollision->RegisterComponent();
			BoxCollision->SetCollisionProfileName("StructureBoxCollision");
			BoxCollision->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
			BoxCollision->SetRelativeLocation(BaseTransform.GetLocation() + FVector(
				0.f, 0.f, WallInstancedComponent->GetStaticMesh()->
				                                  GetBounds().BoxExtent.Z));
			BoxCollision->SetRelativeRotation(BaseRotation);
			FVector Bounds = WallInstancedComponent->GetStaticMesh()->GetBounds().Origin
				+ WallInstancedComponent->GetStaticMesh()->GetBounds().BoxExtent
				+ FVector(0.5f, 0.5f, 0.05f);
			Bounds.Z /= 2;
			BoxCollision->SetBoxExtent(Bounds);
			CollisionBoxes.AddUnique(BoxCollision);

			BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ABGSplineStructure::OnOverlapBegin);
			BoxCollision->OnComponentEndOverlap.AddDynamic(this, &ABGSplineStructure::OnOverlapEnd);
		}
	}
}

void ABGSplineStructure::UpdateCornerMesh()
{
	auto CornerInstancedComponent = GetInstancedStaticMeshComponentByString("CornerInstance");

	if (!CornerInstancedComponent->IsValidLowLevel() || CornerInstancedComponent->IsPendingKillOrUnreachable())
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateCornerMesh(): Creating new CornerInstance"))
		CornerInstancedComponent = CreateInstancedStaticMeshComponent(
			"CornerInstance", StructureData.CornerStaticMesh,
			StructureData.CornerMaskedMaterialInstance);
		CornerInstancedComponent->SetCollisionProfileName("Structure");
	}

	auto BaseInstancedComponent = GetInstancedStaticMeshComponentByString("CornerBaseInstance");

	if (!BaseInstancedComponent->IsValidLowLevel() || BaseInstancedComponent->IsPendingKillOrUnreachable())
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateCornerMesh(): Creating new CornerBaseInstance"))
		BaseInstancedComponent = CreateInstancedStaticMeshComponent(
			"CornerBaseInstance", StructureData.BaseStaticMesh,
			StructureData.BaseMaterialInstance);
		BaseInstancedComponent->SetCollisionProfileName("Base");
	}

	for (int i{}; i < SplineComponent->GetNumberOfSplinePoints(); ++i)
	{
		auto SplinePointTransform = SplineComponent->GetTransformAtSplinePoint(i, ESplineCoordinateSpace::Local, false);

		CornerInstancedComponent->AddInstance(SplinePointTransform);

		BaseInstancedComponent->AddInstance(SplinePointTransform);
	}
}

UInstancedStaticMeshComponent* ABGSplineStructure::CreateInstancedStaticMeshComponent(
	FString const& InstanceName, UStaticMesh* StaticMesh,
	UMaterialInstance* MaterialInstance)
{
	auto NewInstancedStaticMeshComponent = NewObject<UInstancedStaticMeshComponent>(this, *InstanceName);
	NewInstancedStaticMeshComponent->RegisterComponent();
	NewInstancedStaticMeshComponent->SetIsReplicated(true);
	NewInstancedStaticMeshComponent->AttachToComponent(SplineComponent,
	                                                   FAttachmentTransformRules::SnapToTargetIncludingScale);
	NewInstancedStaticMeshComponent->SetFlags(RF_Transactional);

	if (StaticMesh)
	{
		NewInstancedStaticMeshComponent->SetStaticMesh(StaticMesh);
	}
	if (MaterialInstance)
	{
		NewInstancedStaticMeshComponent->SetMaterial(0, MaterialInstance);
	}

	this->AddInstanceComponent(NewInstancedStaticMeshComponent);

	return NewInstancedStaticMeshComponent;
}

void ABGSplineStructure::ToggleLockStructureInPlace_Implementation(bool const bNewLocked)
{
	SetLocked(bNewLocked);
}

void ABGSplineStructure::AddPointToSpline_Implementation(FVector const& Location, float const& Key)
{
	FSplinePoint NewSplinePoint;
	NewSplinePoint.Type = ESplinePointType::Linear;
	NewSplinePoint.Position = Location;
	NewSplinePoint.ArriveTangent = FVector::ZeroVector;
	NewSplinePoint.LeaveTangent = FVector::ZeroVector;
	NewSplinePoint.Rotation = FRotator::ZeroRotator;
	NewSplinePoint.InputKey = Key;
	SplineComponent->AddPoint(NewSplinePoint);
	UpdateSplineStructureMesh();
}

void ABGSplineStructure::RemoveInstanceMeshAtIndex_Implementation(int const& Index,
                                                                  FString const& InstanceName)
{
	UE_LOG(LogTemp, Warning, TEXT("Removing Instance at Index: %i"), Index);

	auto Instance = GetInstancedStaticMeshComponentByString(InstanceName);

	if (Instance)
	{
		Instance->RemoveInstance(Index);
		CollisionBoxes[Index]->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
		CollisionBoxes[Index]->SetCanEverAffectNavigation(false);
		DisabledCollisionBoxes.Add(CollisionBoxes[Index]);
	}
}

void ABGSplineStructure::RemoveStructureFromArray(ABGStructure* StructureToRemove)
{
	SpawnedStructures.RemoveSingle(StructureToRemove);
}

void ABGSplineStructure::SetStructureInfo(FBGStructureInfo const& NewStructureInfo)
{
	StructureData = NewStructureInfo;

	UpdateSplineStructureMesh();
}

UInstancedStaticMeshComponent* ABGSplineStructure::GetInstancedStaticMeshComponentByString(
	FString const& InstanceName) const
{
	for (auto Instance : GetInstanceComponents())
	{
		if (Instance && Instance->GetName().Equals(InstanceName))
		{
			if (auto const CastInstance = Cast<UInstancedStaticMeshComponent>(Instance))
			{
				return CastInstance;
			}
		}
	}
	return nullptr;
}
