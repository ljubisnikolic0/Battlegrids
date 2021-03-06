// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/BGPlayerController.h"
#include "BGMenuPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEGRIDS_API ABGMenuPlayerController : public ABGPlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
};
