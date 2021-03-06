// © 2021 Matthew Barham. All Rights Reserved.


#include "UI/BGServerRow.h"
#include "UI/BGMainMenu.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UBGServerRow::Setup(UBGMainMenu* InParent, uint32 const InIndex)
{
	Parent = InParent;
	Index = InIndex;
	if (!ensure(RowButton)) return;
	RowButton->OnClicked.AddDynamic(this, &UBGServerRow::OnRowClicked);
}

void UBGServerRow::SetServerData(FBGServerData const& InServerData)
{
	ServerData = InServerData;
	ServerName->SetText(FText::FromString(ServerData.Name));
	HostUser->SetText(FText::FromString(ServerData.HostUsername));
	ConnectionFraction->SetText(
		FText::FromString(FString::Printf(TEXT("%d/%d"), ServerData.CurrentPlayers, ServerData.MaxPlayers)));
}

void UBGServerRow::SetRowIsSelected(bool const InSelected)
{
	bIsSelected = InSelected;
}

void UBGServerRow::OnRowClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Clicked Server Row"))
	Parent->SelectServerRowIndex(Index);
}