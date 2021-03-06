// © 2021 Matthew Barham. All Rights Reserved.

#include "UI/BGGameHUD.h"

#include "Components/MenuAnchor.h"

void UBGGameHUD::ToggleContextMenu() const
{
	if (!ensure(ContextMenuAnchor)) return;

	ContextMenuAnchor->ToggleOpen(true);
}

void UBGGameHUD::CloseContextMenu() const
{
	if (!ensure(ContextMenuAnchor)) return;

	ContextMenuAnchor->Close();
}

bool UBGGameHUD::Initialize()
{
	bool const bSuccess = Super::Initialize();
	if (!bSuccess) return false;
	
	return true;
}
