// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/BGSaveGame.h"
#include "Core/BGTypes.h"

#include "BGPlayerSave.generated.h"

/**
 * BattleGrids Player Save
 */
UCLASS()
class BATTLEGRIDS_API UBGPlayerSave : public UBGSaveGame
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "BGPlayerSave|SaveData")
	void Save(FBGPlayerInfo const& NewPlayerInfo);

	FBGPlayerInfo GetPlayerInfo() const { return PlayerInfo; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BGPlayerSave|Config")
	FBGPlayerInfo PlayerInfo;
};