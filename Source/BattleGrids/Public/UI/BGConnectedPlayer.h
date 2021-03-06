// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/BGTypes.h"

#include "BGConnectedPlayer.generated.h"

class UBGLobbyMenu;
class UTextBlock;
class UButton;
class UImage;
class UVerticalBox;

/**
 * 
 */
UCLASS()
class BATTLEGRIDS_API UBGConnectedPlayer : public UUserWidget
{
	GENERATED_BODY()

public:
	void Setup(UBGLobbyMenu* InParent, uint32 const InIndex);

	FBGPlayerInfo GetPlayerInfo() const { return PlayerInfo; }

	void SetPlayerInfo(FBGPlayerInfo const& InPlayerInfo) { PlayerInfo = InPlayerInfo; }

	void UpdateConnectedPlayerButtonColor(bool const bReady) const;

	void UpdateGameMasterPrivilegeButton(bool const bGameMasterPermissions);

protected:
	virtual bool Initialize() override;

	/////////////////////
	/// Button Callbacks

	UFUNCTION()
	void TogglePlayerSettingsDropDown();

	UFUNCTION()
	void OnGameMasterButtonClicked();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UButton* ConnectedPlayerButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* PlayerName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UImage* PlayerColor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* PlayerSettingsDropDown;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UButton* GameMasterPrivilegeButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UImage* GameMasterButtonImage;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* PlayerIDText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBGLobbyMenu* Parent;

	UPROPERTY(BlueprintReadOnly)
	FBGPlayerInfo PlayerInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BGConnectedPlayer|Config")
	UTexture2D* CrossTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BGConnectedPlayer|Config")
	UTexture2D* CheckmarkTexture;

	uint32 Index;
};
