// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "UI/BGMenuWidget.h"

#include "CoreMinimal.h"
#include "BGInGameMenu.generated.h"

class UButton;

/**
 * 
 */
UCLASS()
class BATTLEGRIDS_API UBGInGameMenu : public UBGMenuWidget
{
	GENERATED_BODY()

public:

	virtual bool Initialize() override;
	
private:

	// Callback Functions
	UFUNCTION()
	void ResumePressed();

	UFUNCTION()
	void SavePressed();

	UFUNCTION()
	void LeaveGamePressed();

	UFUNCTION()
	void QuitToDesktopPressed();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UButton* ResumeButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UButton* SaveGameButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UButton* LeaveGameButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UButton* QuitToDesktopButton;
};
