// © 2021 Matthew Barham. All Rights Reserved.


#include "UI/BGGameHUD.h"

bool UBGGameHUD::Initialize()
{
	bool const bSuccess = Super::Initialize();
	if (!bSuccess) return false;
	
	return true;
}
