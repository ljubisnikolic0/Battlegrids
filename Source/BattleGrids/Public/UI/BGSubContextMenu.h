// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "BGGameHUD.h"
#include "Blueprint/UserWidget.h"
#include "BGSubContextMenu.generated.h"

class UMenuAnchor;

/**
 * 
 */
UCLASS()
class BATTLEGRIDS_API UBGSubContextMenu : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual bool Initialize() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BGSubContextMenu|Config")
	UBGGameHUD* GameHUDReference;
};
