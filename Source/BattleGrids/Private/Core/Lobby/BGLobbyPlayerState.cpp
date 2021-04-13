// © 2021 Matthew Barham. All Rights Reserved.


#include "Core/Lobby/BGLobbyPlayerState.h"


void ABGLobbyPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner())
	{
		CheckForExistingSaveData(PlayerInfo);
		ClientSetPlayerInfo(PlayerInfo);
	}
}