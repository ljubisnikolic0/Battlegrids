// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/BGPlayerState.h"
#include "BGLobbyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEGRIDS_API ABGLobbyPlayerState : public ABGPlayerState
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
};
