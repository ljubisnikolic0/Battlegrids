// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/BGTypes.h"

#include "BGInGamePlayerRow.generated.h"

class UBGInGamePlayerList;
class UButton;
class UTextBlock;

/**
 * A UI row that displays a Player's name and information, and available game options to modify the Player
 */
UCLASS()
class BATTLEGRIDS_API UBGInGamePlayerRow : public UUserWidget
{
	GENERATED_BODY()

public:
	void Setup(UBGInGamePlayerList* InParent, uint32 const InIndex);

	FBGPlayerInfo GetPlayerInfo() const { return PlayerInfo; }

	void SetPlayerInfo(FBGPlayerInfo const& InPlayerInfo);

protected:

	UFUNCTION()
	void OnGameMasterPermissionsClicked();
	
private:

	virtual bool Initialize() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UTextBlock* PlayerName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UTextBlock* GameMasterPlayerText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UButton* GameMasterPermissionsButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UBGInGamePlayerList* Parent;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FBGPlayerInfo PlayerInfo;

	uint32 Index;
};
