// © 2021 Matthew Barham. All Rights Reserved.


#include "Core/Gameplay/BGGamePlayerState.h"

#include "Core/BGGameInstance.h"
#include "Core/Save/BGPlayerSave.h"

void ABGGamePlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner())
	{
		auto const GameInstance = GetGameInstance<UBGGameInstance>();
		if (GameInstance && GameInstance->GetLoading())
		{
			auto const PlayerSave = GameInstance->GetPlayerSave();
			if (PlayerSave)
			{
				ClientSetPlayerInfo(PlayerInfo = PlayerSave->GetPlayerInfo());
			}
		}
		else
		{
			ClientSetPlayerInfo(GetPlayerInfo());
		}
	}
}