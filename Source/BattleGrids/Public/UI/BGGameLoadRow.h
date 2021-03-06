// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/Save/BGGameSave.h"

#include "BGGameLoadRow.generated.h"

class UButton;
class UBGMainMenu;
class UTextBlock;
/**
 * BattleGrids game load row
 */
UCLASS()
class BATTLEGRIDS_API UBGGameLoadRow : public UUserWidget
{
	GENERATED_BODY()

public:
	void Setup(UBGMainMenu* InParent, uint32 const InIndex);

	UBGGameSave* GetGameSave() const { return GameSave; }
	bool GetIsSelected() const { return bIsSelected; }

	void SetGameSave(UBGGameSave* InGameSave);
	void SetRowIsSelected(bool const InSelected);

protected:
	UFUNCTION()
	void OnRowClicked();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UButton* RowButton;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UTextBlock* LoadName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UBGMainMenu* Parent;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UBGGameSave* GameSave;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool bIsSelected{false};

	uint32 Index;
};
