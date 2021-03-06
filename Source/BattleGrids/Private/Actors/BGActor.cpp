// © 2021 Matthew Barham. All Rights Reserved.


#include "Actors/BGActor.h"

#include "Components/BGSplineWallComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "UI/BGContextMenu.h"

// Sets default values
ABGActor::ABGActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	if (HasAuthority())
	{
		bReplicates = true;
	}

	bIsLocked = false;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Scene Component"));
	RootComponent = RootSceneComponent;
}

// Called when the game starts or when spawned
void ABGActor::BeginPlay()
{
	Super::BeginPlay();

	SetReplicatingMovement(true);
}

void ABGActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABGActor, ComponentSlot)
	DOREPLIFETIME(ABGActor, ActorType)
	DOREPLIFETIME(ABGActor, bIsLocked)
}

void ABGActor::SetActorType(EBGActorType const& InActorType)
{
	ActorType = InActorType;
}

void ABGActor::SetupComponentSlot(UActorComponent* InComponent)
{
	ComponentSlot = InComponent;
	RegisterAllComponents();
}

void ABGActor::ToggleLocked_Implementation(bool const bSetLocked)
{
	bIsLocked = bSetLocked;
}