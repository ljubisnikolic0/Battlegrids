// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/BGActor.h"

#include "BGLight.generated.h"

class UStaticMeshComponent;
class UBGContextMenu;

/**
 * BattleGrids movable light class
 */
UCLASS()
class BATTLEGRIDS_API ABGLight : public ABGActor
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BGLight|Config")
	TSubclassOf<UBGContextMenu> LightContextMenuClass;
};