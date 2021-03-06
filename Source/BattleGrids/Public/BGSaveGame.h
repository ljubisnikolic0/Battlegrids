// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "BGTypes.h"
#include "GameFramework/SaveGame.h"
#include "BGSaveGame.generated.h"

/**
 * BattleGrids save game object
 */
UCLASS()
class BATTLEGRIDS_API UBGSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "BGSaveGame|SaveData")
	void SavePlayerInfo(FBGPlayerInfo const& NewPlayerInfo);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BGSaveGame|Config")
	FBGPlayerInfo PlayerInfo;
};
