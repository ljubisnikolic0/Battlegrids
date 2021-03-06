// © 2021 Matthew Barham. All Rights Reserved.


#include "UI/BGInGamePlayerList.h"

#include "Components/PanelWidget.h"
#include "UI/BGInGamePlayerRow.h"

void UBGInGamePlayerList::SetPlayerList(TArray<FBGPlayerInfo> const& InPlayerInfo)
{
	auto const World = this->GetWorld();
	if (World && PlayerList)
	{
		PlayerList->ClearChildren();

		uint32 i = 0;
		for (FBGPlayerInfo const& PlayerInfo : InPlayerInfo)
		{
			UBGInGamePlayerRow* PlayerRow = CreateWidget<UBGInGamePlayerRow>(World, PlayerRowClass);
			if (!PlayerRow) return;

			PlayerRow->SetPlayerInfo(PlayerInfo);
			PlayerRow->Setup(this, i);
			++i;

			PlayerList->AddChild(PlayerRow);
		}
	}
}

void UBGInGamePlayerList::SelectIndex(uint32 const Index)
{
	SelectedIndex = Index;
	UpdateChildren();
}

bool UBGInGamePlayerList::Initialize()
{
	bool const bSuccess = Super::Initialize();
	if (!bSuccess) return false;

	return true;
}

void UBGInGamePlayerList::UpdateChildren()
{
}
