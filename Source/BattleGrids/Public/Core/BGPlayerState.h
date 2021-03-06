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

	UFUNCTION(Client, Reliable, BlueprintCallable, Category = "BGPlayerState|Functions")
	void ClientToggleReady();

	FBGPlayerInfo GetPlayerInfo() const { return PlayerInfo; }

	UFUNCTION(Client, Reliable, BlueprintCallable, Category = "BGPlayerState|Functions")
	void ClientSetPlayerInfo(FBGPlayerInfo const& InPlayerInfo);

protected:

	ABGPlayerState();

	virtual void BeginPlay() override;

	// Receives NewPlayerState during seamless travel
	virtual void CopyProperties(APlayerState* PlayerState) override;

	// Receives OldPlayerState during a reconnect
	virtual void OverrideWith(APlayerState* PlayerState) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	bool CheckForExistingSaveData(FBGPlayerInfo& OutPlayerInfo);

	// Update PlayerInfo on the server so that it replicates to clients
	UFUNCTION(Server, Reliable)
	void ServerSetPlayerInfo(FBGPlayerInfo const& InPlayerInfo);

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "BGPlayerState|Config")
	FBGPlayerInfo PlayerInfo;
};