// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/BGSaveGame.h"
#include "Core/BGTypes.h"

#include "BGGameSave.generated.h"

/**
 * BattleGrids Game Save
 */
UCLASS()
class BATTLEGRIDS_API UBGGameSave : public UBGSaveGame
{
	GENERATED_BODY()

public:

	void SaveServerData(FBGServerData const InServerData);
	void SaveCharacterModelInfo(FBGCharacterModelSaveInfo const InCharacterInfo);
	void SaveSplineWallInfo(FBGWallSplineSaveInfo const InWallSplineInfo);

	FBGServerData GetSavedServerData() const { return SavedServerData; }
	TArray<FBGCharacterModelSaveInfo> GetSavedCharacterModelInfoArray() const { return SavedCharacterModelInfo; }
	TArray<FBGWallSplineSaveInfo> GetSavedSplineWallInfoArray() const { return SavedSplineWallInfo; }

protected:

	UPROPERTY()
	FBGServerData SavedServerData;
	
	UPROPERTY()
	TArray<FBGCharacterModelSaveInfo> SavedCharacterModelInfo;

	UPROPERTY()
	TArray<FBGWallSplineSaveInfo> SavedSplineWallInfo;

	
};