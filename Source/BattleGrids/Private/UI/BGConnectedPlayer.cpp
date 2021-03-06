// © 2021 Matthew Barham. All Rights Reserved.


#include "UI/BGConnectedPlayer.h"

#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UI/BGLobbyMenu.h"

bool UBGConnectedPlayer::Initialize()
{
	bool const bSuccess = Super::Initialize();
	if (!bSuccess) return false;

	if (!ensure(ConnectedPlayerButton)) return false;
	ConnectedPlayerButton->OnClicked.AddDynamic(this, &UBGConnectedPlayer::TogglePlayerSettingsDropDown);

	if (!ensure(GameMasterPrivilegeButton)) return false;
	GameMasterPrivilegeButton->OnClicked.AddDynamic(this, &UBGConnectedPlayer::OnGameMasterButtonClicked);

	// Only let Server Host change Game Master Permissions
	if (UKismetSystemLibrary::IsServer(GetOwningPlayer()))
	{
		GameMasterPrivilegeButton->SetIsEnabled(true);
	}
	else
	{
		GameMasterPrivilegeButton->SetIsEnabled(false);
	}

	return true;
}

void UBGConnectedPlayer::TogglePlayerSettingsDropDown()
{
	switch (PlayerSettingsDropDown->GetVisibility())
	{
	case ESlateVisibility::Visible:
		PlayerSettingsDropDown->SetVisibility(ESlateVisibility::Collapsed);
		break;
	case ESlateVisibility::Collapsed:
		PlayerSettingsDropDown->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		break;
	case ESlateVisibility::Hidden:
		PlayerSettingsDropDown->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		break;
	case ESlateVisibility::HitTestInvisible:
		PlayerSettingsDropDown->SetVisibility(ESlateVisibility::Collapsed);
		break;
	case ESlateVisibility::SelfHitTestInvisible:
		PlayerSettingsDropDown->SetVisibility(ESlateVisibility::Collapsed);
		break;
	default: ;
	}
}

void UBGConnectedPlayer::OnGameMasterButtonClicked()
{
	PlayerInfo.bGameMasterPermissions = !PlayerInfo.bGameMasterPermissions;

	Parent->UpdatePlayerInformationAtIndex(Index, PlayerInfo);
}

void UBGConnectedPlayer::Setup(UBGLobbyMenu* InParent, uint32 const InIndex)
{
	Parent = InParent;
	Index = InIndex;

	UpdateConnectedPlayerButtonColor(PlayerInfo.bReady);
	UpdateGameMasterPrivilegeButton(PlayerInfo.bGameMasterPermissions);

	if (!ensure(PlayerIDText)) return;
	PlayerIDText->SetText(FText::FromString(FString::FromInt(PlayerInfo.PlayerID)));
}

void UBGConnectedPlayer::UpdateConnectedPlayerButtonColor(bool const bReady) const
{
	bReady
		? ConnectedPlayerButton->SetBackgroundColor(FLinearColor::Green)
		: ConnectedPlayerButton->SetBackgroundColor(FLinearColor::Red);
}

void UBGConnectedPlayer::UpdateGameMasterPrivilegeButton(bool const bGameMasterPermissions)
{
	bGameMasterPermissions
		? GameMasterPrivilegeButton->SetBackgroundColor(FLinearColor::Green)
		: GameMasterPrivilegeButton->SetBackgroundColor(FLinearColor::Red);

	bGameMasterPermissions
		? GameMasterButtonImage->SetBrushFromTexture(CheckmarkTexture, false)
		: GameMasterButtonImage->SetBrushFromTexture(CrossTexture, false);
}
