// © 2021 Matthew Barham. All Rights Reserved.


#include "Core/BGSaveGame.h"

void UBGSaveGame::SavePlayerInfo(FBGPlayerInfo const& NewPlayerInfo)
{
	PlayerInfo = NewPlayerInfo;
}
