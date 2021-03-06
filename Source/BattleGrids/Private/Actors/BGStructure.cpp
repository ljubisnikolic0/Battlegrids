// © 2021 Matthew Barham. All Rights Reserved.


#include "Actors/BGStructure.h"

#include "Components/BGSplineWallComponent.h"
#include "Net/UnrealNetwork.h"
#include "UI/BGContextMenu.h"

// Sets default values
ABGStructure::ABGStructure()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

// Called when the game starts or when spawned
void ABGStructure::BeginPlay()
{
	Super::BeginPlay();

	SetReplicatingMovement(true);
}

void ABGStructure::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABGStructure, ParentSplineWallComponent)
	DOREPLIFETIME(ABGStructure, IndexOnSplineStructure)
}

void ABGStructure::SetParentSplineWallComponent(UBGSplineWallComponent* NewParentSplineWallComponent)
{
	ParentSplineWallComponent = NewParentSplineWallComponent;
}