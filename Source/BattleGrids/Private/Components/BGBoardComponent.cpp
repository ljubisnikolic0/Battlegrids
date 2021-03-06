// © 2021 Matthew Barham. All Rights Reserved.


#include "Components/BGBoardComponent.h"

#include "Actors/BGActor.h"
#include "Components/BGTileComponent.h"
#include "Core/Gameplay/BGGameplayGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UBGBoardComponent::UBGBoardComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

// Called when the game starts
void UBGBoardComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBGBoardComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBGBoardComponent, BoardTiles)
	DOREPLIFETIME(UBGBoardComponent, BoardSize)
	DOREPLIFETIME(UBGBoardComponent, ZedLevel)
	DOREPLIFETIME(UBGBoardComponent, TileStaticMesh)
}

void UBGBoardComponent::BuildBoard_Implementation(FVector const& CenteredLocation, int const& X, int const& Y)
{
	// Do not execute if BoardTiles contains Tiles already
	if (BoardTiles.Num() == 0)
	{
		BoardSize.X = X;
		BoardSize.Y = Y;

		// 2D Grid Execution Macro, rewritten in C++
		for (auto OuterIndex{0}; OuterIndex <= Y - 1; ++OuterIndex)
			for (auto InnerIndex{0}; InnerIndex <= X - 1; ++InnerIndex)
			{
				float const SectorSize = 100.f;
				// Prepare a fresh Transform
				FVector SpawnLocation =
					FVector(static_cast<float>(InnerIndex) - static_cast<float>(X) / 2.f,
					        static_cast<float>(OuterIndex) - static_cast<float>(Y) / 2.f,
					        0.f)
					* SectorSize
					* 1.f
					+ CenteredLocation;

				FTransform SpawnTransform;
				SpawnTransform.SetLocation(SpawnLocation);

				// Prepare a Tile to spawn
				auto TileToSpawn = Cast<ABGGameplayGameModeBase>(
						UGameplayStatics::GetGameMode(this))->
					CreateNewActor(EBGActorType::Tile);

				auto TileComponent = Cast<UBGTileComponent>(TileToSpawn->
					GetComponentByClass(UBGTileComponent::StaticClass()));
				
				if (TileToSpawn && TileComponent)
				{
					TileComponent->SetBoardReference(Cast<ABGActor>(GetOwner()));
					TileComponent->SetTileInfo(FBGTileInfo(InnerIndex, OuterIndex, 0));
					if (TileStaticMesh)
					{
						TileComponent->SetTileStaticMesh(TileStaticMesh);
					}
					TileToSpawn->FinishSpawning(SpawnTransform);
				}

				// Spawn the new Tile and add it to the board's array
				BoardTiles.Add(TileToSpawn);
				TileToSpawn->AttachToActor(GetOwner(), FAttachmentTransformRules::KeepWorldTransform);
			}
	}
}

void UBGBoardComponent::GrowBoard_Implementation(int const& X, int const& Y)
{
	// TODO: implement GrowBoard
}

void UBGBoardComponent::ShrinkBoard_Implementation(int const& X, int const& Y)
{
	// TODO: implement ShrinkBoard
}