// © 2021 Matthew Barham. All Rights Reserved.


#include "UI/BGInGamePlayerRow.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Core/BGPlayerState.h"
#include "UI/BGInGamePlayerList.h"

bool UBGInGamePlayerRow::Initialize()
{
	bool const bSuccess = Super::Initialize();
	if (!bSuccess) return false;

	if (!ensure(GameMasterPermissionsButton)) return false;
	GameMasterPermissionsButton->OnClicked.AddDynamic(this, &UBGInGamePlayerRow::OnGameMasterPermissionsClicked);

	return true;
}

void UBGInGamePlayerRow::Setup(UBGInGamePlayerList* InParent, uint32 const InIndex)
{
	Parent = InParent;
	Index = InIndex;

	// Only show Permissions button if Owning player of this UI is a Game Master
	auto const PlayerState = Parent->GetOwningPlayerState<ABGPlayerState>();
	if (PlayerState && PlayerState->GetPlayerInfo().bGameMasterPermissions)
	{
		GameMasterPermissionsButton->SetVisibility(ESlateVisibility::Visible);
		GameMasterPermissionsButton->SetIsEnabled(true);

	}
	else
	{
		GameMasterPermissionsButton->SetVisibility(ESlateVisibility::Hidden);
		GameMasterPermissionsButton->SetIsEnabled(false);
	}

	// Change Player Text based on the PlayerInfo this specific Row is assigned
	if (GameMasterPlayerText)
	{
		if (PlayerInfo.bGameMasterPermissions)
		{
			GameMasterPlayerText->SetText(FText::FromString("Game Master"));
		}
		else
		{
			GameMasterPlayerText->SetText(FText::FromString("Player"));
		}
	}
}

void UBGInGamePlayerRow::SetPlayerInfo(FBGPlayerInfo const& InPlayerInfo)
{
	PlayerInfo = InPlayerInfo;
}

void UBGInGamePlayerRow::OnGameMasterPermissionsClicked()
{
	if (Parent)
	{
		Parent->SelectIndex(Index);
	}
}