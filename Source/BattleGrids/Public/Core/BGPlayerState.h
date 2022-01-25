// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Core/BGTypes.h"
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

	FBGServerData GetServerData() const { return ServerData; }

	void SetPlayerInfo(FBGPlayerInfo const& InPlayerInfo);

	void SetServerData(FBGServerData const& InServerData);

	void IncrementPlayerCount() { ServerData.CurrentPlayers++; }
	void DecrementPlayerCount() { ServerData.CurrentPlayers--; }

protected:

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "BGPlayerState|Config")
	FBGPlayerInfo PlayerInfo;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "BGPlayerState|Config")
	FBGServerData ServerData;
};
