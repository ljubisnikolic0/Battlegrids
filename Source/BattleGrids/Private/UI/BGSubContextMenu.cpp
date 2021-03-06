// © 2021 Matthew Barham. All Rights Reserved.


#include "UI/BGSubContextMenu.h"

#include "Core/BGGameInstance.h"

bool UBGSubContextMenu::Initialize()
{
	bool const bSuccess = Super::Initialize();
	if (!bSuccess) return false;

	auto const CastGameInstance = GetGameInstance<UBGGameInstance>();
	if (CastGameInstance)
	{
		GameHUDReference = GetGameInstance<UBGGameInstance>()->GetGameHUD();
	}
	
	return true;
}
