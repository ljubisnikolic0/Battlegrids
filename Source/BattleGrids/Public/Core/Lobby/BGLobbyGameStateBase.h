// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/BGGameStateBase.h"

#include "BGLobbyGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEGRIDS_API ABGLobbyGameStateBase : public ABGGameStateBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

};
