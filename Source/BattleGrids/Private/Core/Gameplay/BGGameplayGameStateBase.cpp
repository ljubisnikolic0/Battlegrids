// © 2021 Matthew Barham. All Rights Reserved.


#include "Core/Gameplay/BGGameplayGameStateBase.h"

#include "Actors/BGBoard.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void ABGGameplayGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	// populate AllBoards array with all ABGBoard actors in the world
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(this, ABGBoard::StaticClass(), Actors);

	// Spawn initial boards (to be improved)
	for (auto Actor : Actors)
	{
		auto Board = Cast<ABGBoard>(Actor);
		if (Board->GetZedLevel() == 0)
		{
			Board->BuildBoard(Board->GetActorLocation(), 10, 10);
		}

		AllBoards.AddUnique(Board);
	}
}

void ABGGameplayGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABGGameplayGameStateBase, AllBoards)
}
