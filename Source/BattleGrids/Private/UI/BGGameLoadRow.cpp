// © 2021 Matthew Barham. All Rights Reserved.


#include "UI/BGGameLoadRow.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "UI/BGMainMenu.h"

void UBGGameLoadRow::Setup(UBGMainMenu* InParent, uint32 const InIndex)
{
	Parent = InParent;
	Index = InIndex;
	if (!ensure(RowButton)) return;
	RowButton->OnClicked.AddDynamic(this, &UBGGameLoadRow::OnRowClicked);
}

void UBGGameLoadRow::SetGameSave(UBGGameSave* InGameSave)
{
	GameSave = InGameSave;
	LoadName->SetText(FText::FromString("Debug Name"));
}

void UBGGameLoadRow::SetRowIsSelected(bool const InSelected)
{
	bIsSelected = InSelected;
}

void UBGGameLoadRow::OnRowClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Clicked Load Game Row"))
	Parent->SelectLoadGameRowIndex(Index);
}