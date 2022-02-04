// © 2021 Matthew Barham. All Rights Reserved.


#include "Core/BGGameStateBase.h"

#include "Actors/BGBoard.h"
#include "Core/BGGameModeBase.h"
#include "Net/UnrealNetwork.h"

void ABGGameStateBase::BeginPlay()
{
	Super::BeginPlay();

}

void ABGGameStateBase::AddPlayerInfoToArray(FBGPlayerInfo const& InPlayerInfo)
{
	ConnectedPlayersInfo.Add(InPlayerInfo);
}

void ABGGameStateBase::RemovePlayerInfoFromArray(int const& Index)
{
	ConnectedPlayersInfo.RemoveAt(Index);
}

void ABGGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABGGameStateBase, ConnectedPlayersInfo)
}