// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "UI/BGMenuWidget.h"

#include "CoreMinimal.h"
#include "BGGameHUD.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class BATTLEGRIDS_API UBGGameHUD : public UBGMenuWidget
{
	GENERATED_BODY()

protected:
	virtual bool Initialize() override;

};
