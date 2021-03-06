// © 2021 Matthew Barham. All Rights Reserved.


#include "Core/Save/BGGameSave.h"

void UBGGameSave::SaveServerData(FBGServerData const InServerData)
{
	SavedServerData = InServerData;
}

void UBGGameSave::SaveCharacterModelInfo(FBGCharacterModelSaveInfo const InCharacterInfo)
{
	SavedCharacterModelInfo.AddUnique(InCharacterInfo);
}

void UBGGameSave::SaveSplineWallInfo(FBGWallSplineSaveInfo const InWallSplineInfo)
{
	SavedSplineWallInfo.AddUnique(InWallSplineInfo);
}
