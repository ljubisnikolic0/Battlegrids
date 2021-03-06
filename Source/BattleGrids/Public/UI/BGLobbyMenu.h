// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "UI/BGMenuWidget.h"
#include "BGLobbyMenu.generated.h"

struct FBGPlayerInfo;

class ABGLobbyPlayerController;
class ABGPlayerState;
class UPanelWidget;
class UButton;
class UTextBlock;

/**
 * Lobby Widget for BattleGrids
 */
UCLASS()
class BATTLEGRIDS_API UBGLobbyMenu : public UBGMenuWidget
{
	GENERATED_BODY()

public:

	virtual bool Initialize() override;

	void SetPlayerList(TArray<FBGPlayerInfo> ConnectedPlayersInformation);

	void UpdatePlayerInformationAtIndex(uint32 const Index, FBGPlayerInfo const& InPlayerInfo);
	
protected:
	explicit UBGLobbyMenu(FObjectInitializer const& ObjectInitializer);

	void UpdateReadyButtonColorAndText(bool const bReady);

private:

	// Callback Functions
	UFUNCTION()
	void StartGame();

	UFUNCTION()
	void ToggleReady();

	UFUNCTION()
	void OpenSettingsMenu();

	UFUNCTION()
	void OpenColorMenu();

	UFUNCTION()
	void LeaveLobby();
	
	// Lobby Widgets

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UTextBlock* ServerName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UTextBlock* CurrentPlayerCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UPanelWidget* PlayerList;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UTextBlock* ReadyButtonText;

	// Buttons
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UButton* StartGameButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UButton* ReadyButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UButton* GameSettingsButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UButton* SelectColorButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UButton* LeaveLobbyButton;

	// Class References
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true),
		Category = "UBGMainMenu|Config")
	TSubclassOf<UUserWidget> ConnectedPlayerWidgetClass;

	// Variables
	TOptional<uint32> SelectedIndex;
};
