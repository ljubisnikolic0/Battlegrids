// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "BGMenuWidget.h"
#include "Blueprint/UserWidget.h"
#include "BGInGamePlayerList.generated.h"

struct FBGPlayerInfo;

class UPanelWidget;

/**
 * In-Game UI which shows the list of Players and offers options to the Game Master for those players
 */
UCLASS()
class BATTLEGRIDS_API UBGInGamePlayerList : public UBGMenuWidget
{
	GENERATED_BODY()

public:
	void SetPlayerList(TArray<FBGPlayerInfo> const& InPlayerInfo);

	void SelectIndex(uint32 const Index);

private:

	virtual bool Initialize() override;

	void UpdateChildren();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UPanelWidget* PlayerList;

	// Class References
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true),
		Category = "BGInGamePlayerList|Config")
	TSubclassOf<UUserWidget> PlayerRowClass;

	// unexposed variables
	TOptional<uint32> SelectedIndex;
};
