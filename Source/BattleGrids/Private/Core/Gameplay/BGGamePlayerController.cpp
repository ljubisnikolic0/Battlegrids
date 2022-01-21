// © 2021 Matthew Barham. All Rights Reserved.

#include "Core/Gameplay/BGGamePlayerController.h"

#include "Actors/BGBoard.h"
#include "Actors/BGSplineStructure.h"
#include "Actors/BGStructure.h"
#include "Actors/BGTile.h"
#include "Actors/BGToken.h"
#include "Actors/Structures/BGDoor.h"
#include "Components/SplineComponent.h"
#include "Core/BGPlayerState.h"
#include "Core/Gameplay/BGGameplayGameModeBase.h"
#include "Engine/DemoNetDriver.h"
#include "Kismet/GameplayStatics.h"

void ABGGamePlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	OutlineObject();

	switch (GrabbedObject)
	{
	case EBGObjectType::None:
		// OutlineObject();
		break;
	case EBGObjectType::Token:
		// HandleTokenSelection();
		break;
	case EBGObjectType::Structure:
		GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel2), true, LastHitResult);
		HandleSplineStructureSelection();
		break;
	case EBGObjectType::Board:
		HandleBoardSelection();
		break;
	default: ;
	}
}

void ABGGamePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Handle Select (Default: Left Click w/ Mouse)
	InputComponent->BindAction("Select", IE_Pressed, this, &ABGGamePlayerController::SelectObject);
	// InputComponent->BindAction("Select", IE_Released, this, &ABGGamePlayerController::ReleaseObject);

	// Token Movement Handling
	InputComponent->BindAxis("RotateToken", this, &ABGGamePlayerController::RotateToken);
}

void ABGGamePlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABGGamePlayerController, ControlMode)
	DOREPLIFETIME(ABGGamePlayerController, GrabbedObject)
	DOREPLIFETIME(ABGGamePlayerController, TokenNames)
	DOREPLIFETIME(ABGGamePlayerController, StructureNames)
}

void ABGGamePlayerController::GetRowNamesOfObjectTypeFromGameMode(EBGObjectType const& ObjectType)
{
	GetRowNamesOfObjectTypeFromGameMode_Server(ObjectType);
}

void ABGGamePlayerController::GetRowNamesOfObjectTypeFromGameMode_Server_Implementation(
	EBGObjectType const& ObjectType)
{
	switch (ObjectType)
	{
	case EBGObjectType::None: break;
	case EBGObjectType::Token:
		TokenNames = Cast<ABGGameplayGameModeBase>(UGameplayStatics::GetGameMode(this))->GetRowNamesOfType(ObjectType);
		break;
	case EBGObjectType::Structure:
		StructureNames = Cast<ABGGameplayGameModeBase>(UGameplayStatics::GetGameMode(this))->
			GetRowNamesOfType(ObjectType);
		break;
	case EBGObjectType::Board: break;
	default: ;
	}
}

void ABGGamePlayerController::SelectObject()
{
	GetHitResultUnderCursorByChannel(
		UEngineTypes::ConvertToTraceType(GrabbedObject == EBGObjectType::None
			                                 ? ECC_GameTraceChannel6
			                                 : ECC_GameTraceChannel2), true,
		LastHitResult);

	switch (GrabbedObject)
	{
	case EBGObjectType::None:
		if (LastHitResult.bBlockingHit && LastHitResult.GetActor()->IsValidLowLevel())
		{
			LastClickedActor = LastHitResult.GetActor();
			UE_LOG(LogTemp, Warning, TEXT("LastClickedActor: %s"), *LastClickedActor->GetName())

			if ((GrabbedToken = Cast<ABGToken>(LastClickedActor))->IsValidLowLevel())
			{
				GrabbedObject = EBGObjectType::Token;
				return;
			}

			if ((GrabbedStructure = Cast<ABGSplineStructure>(LastClickedActor))->IsValidLowLevel())
			{
				GrabbedObject = EBGObjectType::Structure;
				return;
			}

			if ((GrabbedBoard = Cast<ABGBoard>(LastClickedActor))->IsValidLowLevel())
			{
				GrabbedObject = EBGObjectType::Board;
			}
		}
		break;
	case EBGObjectType::Token:
		if (LastHitResult.bBlockingHit && LastHitResult.GetActor()->IsValidLowLevel() && LastHitResult.GetActor()->IsA(
			ABGTile::StaticClass()))
		{
			LastClickedActor = LastHitResult.GetActor();
			MoveTokenToLocation(false);
			ReleaseObject();
		}
		break;
	case EBGObjectType::Structure:
		if (LastHitResult.bBlockingHit && LastHitResult.GetActor()->IsValidLowLevel() && LastHitResult.GetActor()->IsA(
			ABGTile::StaticClass()))
		{
			LastClickedActor = LastHitResult.GetActor();
			HandleSplineStructureSelection();
			ReleaseObject();
		}
		break;
	case EBGObjectType::Board:
		HandleBoardSelection();
		break;
	default: ;
	}
}

void ABGGamePlayerController::ReleaseObject()
{
	GrabbedObject = EBGObjectType::None;

	if (GrabbedStructure)
	{
		SetSplineStructurePhysicsAndCollision(GrabbedStructure, true,
		                                      ECollisionEnabled::Type::QueryAndPhysics);
	}

	GrabbedToken = nullptr;
	GrabbedStructure = nullptr;
	GrabbedBoard = nullptr;
	LastClickedActor = nullptr;
	LastHitResult.Reset();
	NearestIndexToClick = -1;
}

void ABGGamePlayerController::MoveTokenToLocation(bool const bHolding)
{
	if (GrabbedToken && LastClickedActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("MoveTokenToLocation(): LastClickedActor = %s"), *LastClickedActor->GetName())

		FRotator const Rotation = FRotator(0.f, GrabbedToken->GetActorRotation().Yaw, 0.f);

		float ZedValue;
		bHolding ? ZedValue = 100.f : ZedValue = 50.f;

		FVector Location;

		if (Cast<ABGSplineStructure>(LastClickedActor))
		{
			// Don't move tokens on Bases
			if (LastHitResult.GetComponent()->GetName().Contains("Base"))
			{
				return;
			}

			Location = LastHitResult.GetComponent()->Bounds.Origin + FVector(
				0.f, 0.f, LastHitResult.GetComponent()->Bounds.BoxExtent.Z + ZedValue);
		}
		else
		{
			FVector ActorOrigin{};
			FVector ActorBoxExtent{};

			LastClickedActor->GetActorBounds(false, ActorOrigin, ActorBoxExtent, false);

			Location.X = ActorOrigin.X;
			Location.Y = ActorOrigin.Y;
			Location.Z = ZedValue + ActorOrigin.Z + ActorBoxExtent.Z;
		}


		MoveTokenToLocation_Server(GrabbedToken, Location, Rotation);

		// if (HasAuthority())
		// {
		// 	GrabbedToken->MoveToken(Location);
		// 	GrabbedToken->SetActorRotation(Rotation, ETeleportType::ResetPhysics);
		// }
		//
		// if (!HasAuthority())
		// {
		// 	// Make a server call to ask the GameMode to move the token
		// }
	}
}

void ABGGamePlayerController::HandleSplineStructureSelection()
{
	// TODO: Prevent accessing this function if we aren't GameMaster or if the Structure is locked
	if (!GetGameMasterPermissions() || GrabbedStructure->GetLocked())
		return;

	switch (ControlMode)
	{
	case EBGControlMode::Build:
		if (GetInputAnalogKeyState(EKeys::LeftAlt) == 1)
		{
			AddSplinePointToSplineStructure();
			break;
		}
		ModifySplineStructureLength();
		break;
	case EBGControlMode::Edit: break;
	case EBGControlMode::Move:
		MoveSplineStructure();
		break;
	default: ;
	}
}

void ABGGamePlayerController::SpawnSplineStructureAtLocation(FVector const& Location, FName const& WallStaticMeshName,
                                                             FName const& WallMaskedMaterialInstanceName,
                                                             FName const& CornerStaticMeshName,
                                                             FName const& CornerMaskedMaterialInstanceName,
                                                             FName const& BaseStaticMeshName,
                                                             FName const& BaseMaterialInstanceName)
{
	SpawnSplineStructureAtLocation_Server(Location, WallStaticMeshName, WallMaskedMaterialInstanceName,
	                                      CornerStaticMeshName, CornerMaskedMaterialInstanceName, BaseStaticMeshName,
	                                      BaseMaterialInstanceName);
}

void ABGGamePlayerController::SetSplineStructurePhysicsAndCollision(ABGSplineStructure* StructureToModify,
                                                                    bool const bGravityOn,
                                                                    ECollisionEnabled::Type const CollisionType)
{
	if (StructureToModify)
	{
		if (!HasAuthority())
		{
			StructureToModify->SetStructurePhysicsAndCollision(bGravityOn, CollisionType);
		}

		SetSplineStructurePhysicsAndCollision_Server(StructureToModify, bGravityOn, CollisionType);
	}
}

void ABGGamePlayerController::ModifySplineStructureLength()
{
	if (GrabbedStructure && LastHitResult.GetActor()->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Warning, TEXT("Cursor Hit: %s"), *LastHitResult.GetActor()->GetName())

		if (LastHitResult.GetActor()->IsA(ABGTile::StaticClass()) && !Cast<ABGTile>(LastHitResult.GetActor())->
		                                                              GetStaticMeshComponent()->GetVisibleFlag())
		{
			return;
		}

		// Snap the Target Location to grid, and match the Z axis to the current SplineStructure Z value
		auto GridSnappedIntersection = LastHitResult.GetActor()->GetActorLocation().GridSnap(100.f);
		GridSnappedIntersection.Z = GrabbedStructure->GetActorLocation().Z;

		if (NearestIndexToClick < 0)
		{
			if (GrabbedStructure->GetInstancedStaticMeshComponentByString("WallInstance")->GetInstanceCount() < 2)
			{
				NearestIndexToClick = 1;
			}
			else
			{
				NearestIndexToClick = FMath::RoundToInt(
					GrabbedStructure->GetSplineComponent()->FindInputKeyClosestToWorldLocation(
						GridSnappedIntersection));
			}
		}

		if (!HasAuthority())
		{
			// UE_LOG(LogTemp, Warning, TEXT("NearestIndexToClick: %i"), NearestIndexToClick)
			GrabbedStructure->SetLocationOfSplinePoint(NearestIndexToClick, GridSnappedIntersection);
			GrabbedStructure->UpdateSplineStructureMesh();
		}

		ModifyStructureLength_Server(GrabbedStructure, NearestIndexToClick, GridSnappedIntersection);
	}
}

void ABGGamePlayerController::AddSplinePointToSplineStructure()
{
	if (GrabbedStructure)
	{
		if (NearestIndexToClick == -1)
		{
			FVector WorldPosition;
			FVector WorldDirection;
			DeprojectMousePositionToWorld(WorldPosition, WorldDirection);

			// Get intersection of mouse position at XY plane from GrabbedStructure's origin
			auto Intersection = FMath::LinePlaneIntersection(
				WorldPosition,
				WorldDirection * 2000.f + WorldPosition,
				GrabbedStructure->GetActorLocation(),
				FVector(0.f, 0.f, 1.f)).GridSnap(100.f);

			Intersection.Z = GrabbedStructure->GetActorLocation().Z;

			auto const ClosestKey = GrabbedStructure->GetSplineComponent()->FindInputKeyClosestToWorldLocation(
				Intersection);

			const auto TotalSplinePoints = GrabbedStructure->GetSplineComponent()->GetNumberOfSplinePoints();

			double IntegralPart;

			double const DecimalPart = modf(ClosestKey, &IntegralPart);

			// within 0.5f, plus or minus 0.15f
			if (DecimalPart >= 0.35f && DecimalPart <= 0.65f)
			{
				NearestIndexToClick = ++IntegralPart;
			}

			else if (DecimalPart < 0.35f && IntegralPart == 0)
			{
				NearestIndexToClick = IntegralPart;
			}

			else if ((DecimalPart > 0.65f || DecimalPart == 0.f) && IntegralPart == TotalSplinePoints - 1)
			{
				NearestIndexToClick = TotalSplinePoints;
			}

			if (NearestIndexToClick >= 0)
			{
				AddSplinePointToSplineStructure_Server(GrabbedStructure, Intersection, NearestIndexToClick);
			}
		}

		// Delay necessary for Spline Points to be updated/shifted so that new index is grabbed for movement, and not old one
		FTimerHandle SplinePointUpdateTimer;
		GetWorldTimerManager().SetTimer(SplinePointUpdateTimer, this,
		                                &ABGGamePlayerController::ModifySplineStructureLength,
		                                0.2f, false);
	}
}

void ABGGamePlayerController::MoveSplineStructure()
{
	if (GrabbedStructure)
	{
		SetSplineStructurePhysicsAndCollision(GrabbedStructure, false, ECollisionEnabled::Type::PhysicsOnly);
		FVector Location{};

		UE_LOG(LogTemp, Warning, TEXT("Moving Structure"))

		LastHitResult.Reset();
		LastClickedActor = nullptr;
		if (GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel4), true,
		                                     LastHitResult))
		{
			if (LastHitResult.GetActor()->IsA(ABGTile::StaticClass()))
			{
				UE_LOG(LogTemp, Warning, TEXT("Cursor Hit: %s"), *LastHitResult.GetActor()->GetName())

				if (!Cast<ABGTile>(LastHitResult.GetActor())->GetStaticMeshComponent()->GetVisibleFlag())
					return;

				LastClickedActor = LastHitResult.GetActor();

				FVector ActorOrigin{};
				FVector ActorBoxExtent{};

				LastClickedActor->GetActorBounds(false, ActorOrigin, ActorBoxExtent, false);

				Location.X = LastClickedActor->GetActorLocation().X;
				Location.Y = LastClickedActor->GetActorLocation().Y;
				Location.Z = LastClickedActor->GetActorLocation().Z + ActorOrigin.Z + ActorBoxExtent.Z;
			}
			else
			{
				return;
			}
		}

		if (!HasAuthority())
		{
			GrabbedStructure->SetActorLocation(Location);
		}
		MoveSplineStructure_Server(GrabbedStructure, Location);
	}
}

void ABGGamePlayerController::ModifySplineStructureInstanceMeshAtIndex(ABGSplineStructure* StructureToModify,
                                                                       int const& Index,
                                                                       FString const& NewInstanceName,
                                                                       UStaticMesh* StaticMesh,
                                                                       UMaterialInstance* MaterialInstance,
                                                                       FString const& OldInstanceName)
{
	if (StructureToModify)
	{
		// Just go straight to server call
		ModifyInstanceMeshAtIndex_Server(StructureToModify, Index, NewInstanceName, StaticMesh,
		                                 MaterialInstance, OldInstanceName);
	}
}

void ABGGamePlayerController::SpawnStructureActorAtSplineStructureIndex(ABGSplineStructure* StructureToModify,
                                                                        int const& Index,
                                                                        TSubclassOf<ABGStructure> StructureClassToSpawn,
                                                                        FString const& OldInstanceName)
{
	if (StructureToModify && StructureClassToSpawn)
	{
		SpawnStructureActorAtSplineStructureIndex_Server(StructureToModify, Index, StructureClassToSpawn,
		                                                 OldInstanceName);
	}
}

void ABGGamePlayerController::RemoveInstanceAtIndexOnSplineStructure(ABGSplineStructure* StructureToModify,
                                                                     int const& Index, FString const& InstanceName)
{
	if (StructureToModify)
	{
		RemoveInstanceAtIndexOnSplineStructure_Server(StructureToModify, Index, InstanceName);
	}
}

void ABGGamePlayerController::RestoreInstanceAtIndexOnSplineStructure(ABGSplineStructure* StructureToModify,
                                                                      int const& Index,
                                                                      FTransform const& NewInstanceTransform,
                                                                      FString const& InstanceName)
{
	if (StructureToModify)
	{
		RestoreInstanceAtIndexOnSplineStructure_Server(StructureToModify, Index, NewInstanceTransform, InstanceName);
	}
}

void ABGGamePlayerController::DestroyStructureActor(ABGStructure* StructureToRemove)
{
	if (StructureToRemove)
	{
		DestroyStructureActor_Server(StructureToRemove);
	}
}

void ABGGamePlayerController::ToggleLockSplineStructureInPlace(ABGSplineStructure* SplineStructureToLock,
                                                               bool const bNewLocked)
{
	if (SplineStructureToLock)
	{
		ToggleLockSplineStructureInPlace_Server(SplineStructureToLock, bNewLocked);
	}
}

void ABGGamePlayerController::ToggleLockStructure(ABGStructure* StructureToLock, bool const bNewLocked)
{
	if (StructureToLock)
	{
		ToggleLockStructure_Server(StructureToLock, bNewLocked);
	}
}

void ABGGamePlayerController::ToggleDoorOpenClose(ABGDoor* DoorToToggle)
{
	if (DoorToToggle)
	{
		ToggleDoorOpenClose_Server(DoorToToggle);
	}
}

void ABGGamePlayerController::ResetSplineStructure(ABGSplineStructure* SplineStructureToReset) const
{
	if (SplineStructureToReset)
	{
		if (!HasAuthority())
		{
			if (SplineStructureToReset->GetSplineComponent()->GetNumberOfSplinePoints() > 2)
			{
				// clear all spline points except for indices 0 and 1
				for (int i{SplineStructureToReset->GetSplineComponent()->GetNumberOfSplinePoints()}; i > 2; --i)
				{
					SplineStructureToReset->GetSplineComponent()->RemoveSplinePoint(i - 1);
				}
			}

			// reset Spline Point 0 to actor's origin
			SplineStructureToReset->GetSplineComponent()->SetLocationAtSplinePoint(
				0, SplineStructureToReset->GetActorLocation(), ESplineCoordinateSpace::World, true);

			// reset Spline Point 1 to 105.f away the origin
			auto Location = SplineStructureToReset->GetActorLocation();
			Location.X += 50.f;

			SplineStructureToReset->GetSplineComponent()->SetLocationAtSplinePoint(
				1, Location, ESplineCoordinateSpace::World, true);

			SplineStructureToReset->UpdateSplineStructureMesh();
		}
	}
}

void ABGGamePlayerController::DestroySplineStructure(ABGSplineStructure* SplineStructureToDestroy)
{
	if (SplineStructureToDestroy)
	{
		if (!HasAuthority())
		{
			SplineStructureToDestroy->Destroy();
		}
		DestroySplineStructure_Server(SplineStructureToDestroy);
	}
}

void ABGGamePlayerController::HandleBoardSelection()
{
	if (GrabbedBoard)
	{
		if (!GetGameMasterPermissions())
		{
			return;
		}

		FVector WorldPosition;
		FVector WorldDirection;

		DeprojectMousePositionToWorld(WorldPosition, WorldDirection);

		auto const GridSnappedIntersection = FMath::LinePlaneIntersection(WorldPosition,
		                                                                  WorldDirection * 2000.f + WorldPosition,
		                                                                  GrabbedBoard->GetActorLocation(),
		                                                                  FVector(0.f, 0.f, 1.f)).GridSnap(105.f);

		MoveBoardToLocation(GridSnappedIntersection);
	}
}

void ABGGamePlayerController::MoveBoardToLocation(FVector const& Location)
{
	if (GrabbedBoard)
	{
		if (!HasAuthority())
		{
			GrabbedBoard->SetActorLocation(Location);
		}
		MoveBoardToLocation_Server(GrabbedBoard, Location);
	}
}

void ABGGamePlayerController::ToggleTileVisibility(ABGTile* TileToToggle)
{
	if (TileToToggle)
	{
		if (!HasAuthority())
		{
			TileToToggle->ToggleTileVisibility(TileToToggle->GetStaticMeshComponent()->GetVisibleFlag());
		}
		ToggleTileVisibility_Server(TileToToggle);
	}
}

void ABGGamePlayerController::ShrinkBoard(ABGBoard* BoardToShrink)
{
	if (BoardToShrink)
	{
		if (!HasAuthority())
		{
			BoardToShrink->ShrinkBoard(BoardToShrink->GetBoardSize().X - 1, BoardToShrink->GetBoardSize().Y - 1);
		}
		ShrinkBoard_Server(BoardToShrink);
	}
}

void ABGGamePlayerController::GrowBoard(ABGBoard* BoardToGrow)
{
	if (BoardToGrow)
	{
		if (!HasAuthority())
		{
			BoardToGrow->GrowBoard(BoardToGrow->GetBoardSize().X + 1, BoardToGrow->GetBoardSize().Y + 1);
		}
		GrowBoard_Server(BoardToGrow);
	}
}

void ABGGamePlayerController::ToggleDoorOpenClose_Server_Implementation(ABGDoor* DoorToToggle)
{
	if (HasAuthority() && DoorToToggle)
	{
		DoorToToggle->ToggleOpenClose();
	}
}

void ABGGamePlayerController::ToggleLockStructure_Server_Implementation(
	ABGStructure* StructureToLock, bool const bNewLocked)
{
	if (HasAuthority() && StructureToLock)
	{
		StructureToLock->ToggleStructureLock(bNewLocked);
	}
}

void ABGGamePlayerController::RestoreInstanceAtIndexOnSplineStructure_Server_Implementation(
	ABGSplineStructure* StructureToModify, int const& Index, FTransform const& NewInstanceTransform,
	FString const& InstanceName)
{
	if (HasAuthority() && StructureToModify)
	{
		StructureToModify->RestoreInstanceMeshAtIndex(Index, NewInstanceTransform, InstanceName);
	}
}

void ABGGamePlayerController::RemoveInstanceAtIndexOnSplineStructure_Server_Implementation(
	ABGSplineStructure* StructureToModify, int const& Index, FString const& InstanceName)
{
	if (HasAuthority() && StructureToModify)
	{
		StructureToModify->RemoveInstanceMeshAtIndex(Index, InstanceName);
	}
}

void ABGGamePlayerController::DestroyStructureActor_Server_Implementation(
	ABGStructure* StructureToRemove)
{
	if (HasAuthority() && StructureToRemove)
	{
		ABGGameplayGameModeBase::DestroyStructureActor(StructureToRemove);
	}
}

void ABGGamePlayerController::SpawnStructureActorAtSplineStructureIndex_Server_Implementation(
	ABGSplineStructure* StructureToModify, int const& Index, TSubclassOf<ABGStructure> StructureClassToSpawn,
	FString const& OldInstanceName)
{
	if (HasAuthority() && StructureToModify && StructureClassToSpawn)
	{
		ABGGameplayGameModeBase::SpawnStructureActorAtSplineStructureIndex(
			StructureToModify, Index, StructureClassToSpawn, OldInstanceName);
	}
}

void ABGGamePlayerController::ToggleLockSplineStructureInPlace_Server_Implementation(
	ABGSplineStructure* SplineStructureToLock,
	bool const bNewLocked)
{
	if (HasAuthority() && SplineStructureToLock)
	{
		SplineStructureToLock->ToggleLockStructureInPlace(bNewLocked);
	}
}

void ABGGamePlayerController::ModifyInstanceMeshAtIndex_Server_Implementation(
	ABGSplineStructure* StructureToModify, int const& Index, FString const& NewInstanceName, UStaticMesh* StaticMesh,
	UMaterialInstance* MaterialInstance, FString const& OldInstanceName)
{
	if (HasAuthority() && StructureToModify)
	{
		ABGGameplayGameModeBase::ModifySplineStructureInstanceMeshAtIndex(StructureToModify, Index, NewInstanceName,
		                                                                  StaticMesh,
		                                                                  MaterialInstance, OldInstanceName);
	}
}

void ABGGamePlayerController::MoveSplineStructure_Server_Implementation(ABGSplineStructure* StructureToMove,
                                                                        FVector const& Location)
{
	if (HasAuthority() && StructureToMove)
	{
		ABGGameplayGameModeBase::MoveSplineStructure(StructureToMove, Location);
	}
}

void ABGGamePlayerController::ResetSplineStructure_Server_Implementation(ABGSplineStructure* StructureToReset)
{
	if (HasAuthority() && StructureToReset)
	{
		ABGGameplayGameModeBase::ResetSplineStructure(StructureToReset);
	}
}

void ABGGamePlayerController::ShrinkBoard_Server_Implementation(ABGBoard* BoardToShrink)
{
	if (HasAuthority() && BoardToShrink)
	{
		ABGGameplayGameModeBase::ShrinkBoard(BoardToShrink);
	}
}

void ABGGamePlayerController::GrowBoard_Server_Implementation(ABGBoard* BoardToGrow)
{
	if (HasAuthority() && BoardToGrow)
	{
		ABGGameplayGameModeBase::GrowBoard(BoardToGrow);
	}
}

void ABGGamePlayerController::SpawnSplineStructureAtLocation_Server_Implementation(
	FVector const& Location, FName const& WallStaticMeshName,
	FName const& WallMaskedMaterialInstanceName,
	FName const& CornerStaticMeshName,
	FName const& CornerMaskedMaterialInstanceName,
	FName const& BaseStaticMeshName,
	FName const& BaseMaterialInstanceName)
{
	if (HasAuthority())
	{
		Cast<ABGGameplayGameModeBase>(UGameplayStatics::GetGameMode(this))->SpawnSplineStructureAtLocation(
			Location, WallStaticMeshName, WallMaskedMaterialInstanceName, CornerStaticMeshName,
			CornerMaskedMaterialInstanceName, BaseStaticMeshName,
			BaseMaterialInstanceName);
		UE_LOG(LogTemp, Warning, TEXT("Spawning Structure At Location (server)"))
	}
}

void ABGGamePlayerController::SpawnNewBoard_Server_Implementation(int const& Zed, int const& X, int const& Y)
{
	if (HasAuthority())
	{
		Cast<ABGGameplayGameModeBase>(UGameplayStatics::GetGameMode(this))->SpawnNewBoard(Zed, X, Y);
	}
}

void ABGGamePlayerController::DestroyToken_Server_Implementation(ABGToken* TokenToDestroy)
{
	if (HasAuthority() && TokenToDestroy)
	{
		ABGGameplayGameModeBase::DestroyToken(TokenToDestroy);
	}
}

void ABGGamePlayerController::ToggleTokenPermissionsForPlayer_Server_Implementation(ABGPlayerState* PlayerStateToToggle,
	ABGToken* TokenToToggle)
{
	if (HasAuthority() && PlayerStateToToggle && TokenToToggle)
	{
		ABGGameplayGameModeBase::ToggleTokenPermissionsForPlayer(PlayerStateToToggle, TokenToToggle);
	}
}

void ABGGamePlayerController::RotateToken_Server_Implementation(ABGToken* TokenToRotate, FRotator const& NewRotation)
{
	if (HasAuthority() && TokenToRotate)
	{
		TokenToRotate->SetActorRotation(NewRotation);
	}
}

void ABGGamePlayerController::ResetTokenRotation_Server_Implementation(ABGToken* TokenToReset)
{
	if (HasAuthority() && TokenToReset)
	{
		TokenToReset->SetActorRotation(FRotator::ZeroRotator);
	}
}

void ABGGamePlayerController::ToggleTokenLockInPlace_Server_Implementation(ABGToken* TokenToToggle, bool bLock)
{
	if (HasAuthority() && TokenToToggle)
	{
		ABGGameplayGameModeBase::ToggleTokenLockInPlace(TokenToToggle, bLock);
	}
}

void ABGGamePlayerController::SpawnTokenAtLocation_Server_Implementation(
	FVector const& Location, FName const& MeshName, FName const& MaterialName)
{
	if (HasAuthority())
	{
		Cast<ABGGameplayGameModeBase>(UGameplayStatics::GetGameMode(this))->SpawnTokenAtLocation(
			Location, MeshName, MaterialName);

		UE_LOG(LogTemp, Warning, TEXT("Spawning Token At Location (server)"))
	}
}

void ABGGamePlayerController::MoveTokenToLocation_Server_Implementation(ABGToken* TokenToMove, FVector const& Location,
                                                                        FRotator const TokenRotation)
{
	if (HasAuthority() && TokenToMove)
	{
		UE_LOG(LogTemp, Warning, TEXT("Moving Token To Location (server)"))
		ABGGameplayGameModeBase::MoveTokenToLocation(TokenToMove, Location, TokenRotation);
	}
}

void ABGGamePlayerController::ModifyStructureLength_Server_Implementation(
	ABGSplineStructure* StructureToModify, int const& PointIndex,
	FVector const& NewLocation)
{
	if (HasAuthority() && StructureToModify)
	{
		ABGGameplayGameModeBase::ModifySplineStructureLength(StructureToModify, PointIndex, NewLocation);
	}
}

void ABGGamePlayerController::AddSplinePointToSplineStructure_Server_Implementation(
	ABGSplineStructure* StructureToModify,
	FVector const& ClickLocation, int const& Index)
{
	if (HasAuthority() && StructureToModify)
	{
		ABGGameplayGameModeBase::AddSplinePointToSplineStructure(StructureToModify, ClickLocation,
		                                                         Index);
	}
}

void ABGGamePlayerController::RotateToken(float Value)
{
	if (Value != 0)
		if (GrabbedToken)
		{
			auto const OriginalRotation = GrabbedToken->GetActorRotation().GetDenormalized();
			float const Remainder = FMath::Fmod(OriginalRotation.Yaw, 45.f);

			// If we have a Yaw that is greater than or equal to 360 degrees, use 0 instead
			int Quotient = (OriginalRotation.Yaw > 337.5f ? 0 : OriginalRotation.Yaw) / 45;

			UE_LOG(LogTemp, Warning, TEXT("Incoming Yaw: %f"), OriginalRotation.Yaw)
			UE_LOG(LogTemp, Warning, TEXT("Remainder: %f"), Remainder)
			UE_LOG(LogTemp, Warning, TEXT("Quotient: %d"), Quotient)

			// if our Yaw is close to 360 then don't upgrade the Quotient (lest we shoot past 45 and go to 90)
			if (Remainder >= 22.5f && OriginalRotation.Yaw < 337.5f)
				++Quotient;

			Quotient *= 45;

			if (Value > 0)
				Quotient += 45;
			else if (Value < 0)
			{
				Quotient -= 45;

				Quotient < 0 ? Quotient += 360 : Quotient;
			}

			UE_LOG(LogTemp, Warning, TEXT("New Angle: %d"), Quotient)

			auto NewRotation = FRotator(0.f, Quotient, 0.f);
			NewRotation.Normalize();

			if (!HasAuthority())
			{
				GrabbedToken->SetActorRotation(NewRotation);
			}
			RotateToken_Server(GrabbedToken, NewRotation);
		}
}

void ABGGamePlayerController::ResetTokenRotation(ABGToken* TokenToReset)
{
	if (TokenToReset)
	{
		if (!HasAuthority())
		{
			TokenToReset->SetActorRotation(FRotator::ZeroRotator);
		}
		ResetTokenRotation_Server(TokenToReset);
	}
}

void ABGGamePlayerController::ToggleTokenLockInPlace(ABGToken* TokenToToggle, bool bLock)
{
	if (TokenToToggle)
	{
		if (!HasAuthority())
		{
			TokenToToggle->ToggleLockTokenInPlace(bLock);
		}
		ToggleTokenLockInPlace_Server(TokenToToggle, bLock);
	}
}

void ABGGamePlayerController::ToggleTokenPermissionsForPlayer(ABGPlayerState* PlayerStateToToggle,
                                                              ABGToken* TokenToToggle)
{
	if (PlayerStateToToggle && TokenToToggle)
	{
		if (!HasAuthority())
		{
			TokenToToggle->PlayerHasPermissions(PlayerStateToToggle)
				? TokenToToggle->RemovePlayerFromPermissionsArray(PlayerStateToToggle)
				: TokenToToggle->AddPlayerToPermissionsArray(PlayerStateToToggle);
		}

		ToggleTokenPermissionsForPlayer_Server(PlayerStateToToggle, TokenToToggle);
	}
}

void ABGGamePlayerController::DestroyToken(ABGToken* TokenToDestroy)
{
	if (TokenToDestroy)
	{
		DestroyToken_Server(TokenToDestroy);
	}
}

void ABGGamePlayerController::MoveBoardToLocation_Server_Implementation(ABGBoard* BoardToMove,
                                                                        FVector const& NewLocation)
{
	if (BoardToMove)
	{
		BoardToMove->SetActorLocation(NewLocation);
	}
}

void ABGGamePlayerController::ToggleTileVisibility_Server_Implementation(ABGTile* TileToToggle)
{
	if (TileToToggle)
	{
		ABGGameplayGameModeBase::ToggleTileVisibility(TileToToggle);
	}
}

bool ABGGamePlayerController::GetGameMasterPermissions() const
{
	if (auto const PS = GetPlayerState<ABGPlayerState>())
		if (PS->GetPlayerInfo().bGameMasterPermissions)
			return true;
	return false;
}

void ABGGamePlayerController::OutlineObject()
{
	FHitResult HitResult;
	if (GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel6), true, HitResult))
	{
		if (HitResult.GetActor()->IsValidLowLevel())
		{
			TArray<UStaticMeshComponent*> StaticMeshComponents;

			// Do we have a previous CurrentOutlinedActor, and is it not equal to the newly hit Actor?
			if (CurrentOutlinedActor && CurrentOutlinedActor != HitResult.GetActor())
			{
				// Turn off the RenderCustomDepth on all Static Mesh Components of the previous CurrentOutlinedActor
				CurrentOutlinedActor->GetComponents<UStaticMeshComponent>(StaticMeshComponents);

				if (StaticMeshComponents.Num() > 0)
				{
					for (auto Comp : StaticMeshComponents)
					{
						// UE_LOG(LogTemp, Warning, TEXT("Setting Render Custom Depth : False (1)"))
						Comp->SetRenderCustomDepth(false);
					}
				}
			}

			// Clear the array
			StaticMeshComponents.Empty();

			// set the pointer to the newly hit Actor
			CurrentOutlinedActor = HitResult.GetActor();

			// Fill it again with the new hit Actor's Static Mesh Components
			HitResult.GetActor()->GetComponents<UStaticMeshComponent>(StaticMeshComponents);

			// Turn the RenderCustomDepth on for the new hit Actor
			if (StaticMeshComponents.Num() > 0)
			{
				for (auto Comp : StaticMeshComponents)
				{
					// UE_LOG(LogTemp, Warning, TEXT("Setting Render Custom Depth : True"))
					Comp->SetRenderCustomDepth(true);
				}
			}
		}
	}
	else
	{
		// Do we have a previous CurrentOutlinedActor
		if (CurrentOutlinedActor)
		{
			// Turn off the RenderCustomDepth on all Static Mesh Components of the previous CurrentOutlinedActor
			TArray<UStaticMeshComponent*> StaticMeshComponents;
			CurrentOutlinedActor->GetComponents<UStaticMeshComponent>(StaticMeshComponents);

			if (StaticMeshComponents.Num() > 0)
			{
				for (auto Comp : StaticMeshComponents)
				{
					// UE_LOG(LogTemp, Warning, TEXT("Setting Render Custom Depth : False (2)"))
					Comp->SetRenderCustomDepth(false);
				}
			}
		}
	}
}

void ABGGamePlayerController::UpdateTransformOnServer_Implementation(FTransform NewTransform)
{
	GetPawn()->SetActorTransform(NewTransform);
}

void ABGGamePlayerController::SetSplineStructurePhysicsAndCollision_Server_Implementation(
	ABGSplineStructure* StructureToModify,
	bool const bGravityOn,
	ECollisionEnabled::Type const CollisionType)
{
	if (StructureToModify)
	{
		ABGGameplayGameModeBase::SetSplineStructurePhysicsAndCollision(StructureToModify, bGravityOn,
		                                                               CollisionType);
	}
}

void ABGGamePlayerController::DestroySplineStructure_Server_Implementation(ABGSplineStructure* StructureToDestroy)
{
	if (StructureToDestroy)
	{
		ABGGameplayGameModeBase::DestroySplineStructure(StructureToDestroy);
	}
}
