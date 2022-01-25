// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "Core/BGTypes.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BGServerRow.generated.h"

class UButton;
class UTextBlock;
class UBGMainMenu;

/**
 * 
 */
UCLASS()
class BATTLEGRIDS_API UBGServerRow : public UUserWidget
{
	GENERATED_BODY()

public:
	void Setup(UBGMainMenu* InParent, uint32 const InIndex);

	FBGServerData GetServerData() const { return ServerData; }
	UTextBlock* GetServerName() const { return ServerName; }
	UTextBlock* GetHostUser() const { return HostUser; }
	UTextBlock* GetConnectionFraction() const { return ConnectionFraction; }
	bool GetIsSelected() const { return bSelected; }

	void SetServerData(FBGServerData const& InServerData);
	void SetRowIsSelected(bool const InSelected);

protected:

	UFUNCTION()
	void OnRowClicked();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* ServerName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* HostUser;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* ConnectionFraction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UButton* RowButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBGMainMenu* Parent;

	UPROPERTY(BlueprintReadOnly)
	bool bSelected {false};

	UPROPERTY(BlueprintReadOnly)
	FBGServerData ServerData;

	uint32 Index;
};
