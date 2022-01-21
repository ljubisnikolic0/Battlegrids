// © 2021 Matthew Barham. All Rights Reserved.


#include "UI/BGServerRow.h"

#include "Components/TextBlock.h"

void UBGServerRow::SetServerName(FString const& InString) const
{
	ServerName->SetText(FText::FromString(InString));
}
