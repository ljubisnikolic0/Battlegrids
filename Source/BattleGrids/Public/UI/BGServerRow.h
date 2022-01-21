// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BGServerRow.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class BATTLEGRIDS_API UBGServerRow : public UUserWidget
{
	GENERATED_BODY()

public:

	UTextBlock* GetServerName() const { return ServerName; }

	void SetServerName(FString const& InString) const;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UTextBlock* ServerName;
};
