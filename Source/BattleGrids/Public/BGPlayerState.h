// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "BGTypes.h"
#include "GameFramework/PlayerState.h"
#include "BGPlayerState.generated.h"

/**
 * BattleGrids game player state class
 */
UCLASS()
class BATTLEGRIDS_API ABGPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FBGPlayerInfo GetPlayerInfo() const { return PlayerInfo; }

protected:

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "BGPlayerState|Config")
	FBGPlayerInfo PlayerInfo;
};
