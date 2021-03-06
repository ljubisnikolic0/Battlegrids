// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "UI/BGMenuWidget.h"

#include "CoreMinimal.h"
#include "BGGameHUD.generated.h"

class UMenuAnchor;

/**
 * In-Game HUD for BattleGrids
 */
UCLASS()
class BATTLEGRIDS_API UBGGameHUD : public UBGMenuWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	UMenuAnchor* GetContextMenuAnchor() const { return ContextMenuAnchor; }
	
	UFUNCTION()
	void SetContextMenuHitResult(FHitResult const& InHitResult) { ContextMenuHitResult = InHitResult; }
	
	UFUNCTION(BlueprintCallable, Category = "BGGameHUD|Functions")
	void ToggleContextMenu() const;

	UFUNCTION(BlueprintCallable, Category = "BGGameHUD|Functions")
	void CloseContextMenu() const;

protected:
	virtual bool Initialize() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UMenuAnchor* ContextMenuAnchor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FHitResult ContextMenuHitResult;
};
