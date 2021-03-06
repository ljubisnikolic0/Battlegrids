// © 2021 Matthew Barham. All Rights Reserved.

#include "Actors/Structures/BGDoor.h"

#include "Net/UnrealNetwork.h"

ABGDoor::ABGDoor()
{
	PrimaryActorTick.bCanEverTick = true;

	DoorFrameStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Frame Static Mesh"));
	RootComponent = DoorFrameStaticMeshComponent;

	DoorStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Static Mesh"));
	DoorStaticMeshComponent->SetupAttachment(DoorFrameStaticMeshComponent);
	DoorStaticMeshComponent->SetRelativeLocation(FVector(45.f, 5.f, 10.f));

	bOpen = false;
}

void ABGDoor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	DoorRotation = DoorStaticMeshComponent->GetRelativeRotation();

	// if the Lerp is within 1.f just snap it open/close and return
	if (FMath::IsNearlyEqual(DoorRotation.Yaw, RotateValue, 1.f))
	{
		DoorStaticMeshComponent->SetRelativeRotation(FQuat(FRotator(0.0f, bOpen ? RotateValue : 0.0f, 0.0f)));
		return;
	}

	if (bOpen)
	{
		DoorStaticMeshComponent->SetRelativeRotation(
			FMath::Lerp(FQuat(DoorRotation), FQuat(FRotator(0.0f, RotateValue, 0.0f)), 0.05f));
	}
	else
	{
		DoorStaticMeshComponent->SetRelativeRotation(
			FMath::Lerp(FQuat(DoorRotation), FQuat(FRotator(0.0f, 0.0f, 0.0f)), 0.05f));
	}
}

void ABGDoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABGDoor, bOpen)
	DOREPLIFETIME(ABGDoor, RotateValue)
	DOREPLIFETIME(ABGDoor, DoorRotation)
}

void ABGDoor::ToggleOpenClose_Implementation()
{
	bOpen ? RotateValue = 90.f : RotateValue = -90.f;
	bOpen = !bOpen;

	// disable the collision and AffectNavigation if the Door is open for both the frame and door meshes
	bOpen
		? DoorStaticMeshComponent->SetCanEverAffectNavigation(false)
		: DoorStaticMeshComponent->SetCanEverAffectNavigation(true);
	bOpen
		? DoorStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly)
		: DoorStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	bOpen
		? DoorFrameStaticMeshComponent->SetCanEverAffectNavigation(false)
		: DoorFrameStaticMeshComponent->SetCanEverAffectNavigation(true);
	bOpen
		? DoorFrameStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly)
		: DoorFrameStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}
