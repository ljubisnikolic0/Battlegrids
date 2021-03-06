// © 2021 Matthew Barham. All Rights Reserved.


#include "UI/BGContextMenu.h"

#include "Actors/BGActor.h"
#include "Characters/BGCharacter.h"

void UBGContextMenu::SetHitResult(FHitResult const& InHitResult)
{
	HitResult = InHitResult;
}
