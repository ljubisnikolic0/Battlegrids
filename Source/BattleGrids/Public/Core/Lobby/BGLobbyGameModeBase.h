// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Core/BGGameModeBase.h"

#include "BGLobbyGameModeBase.generated.h"


/**
 * Game Mode for BattleGrids Lobby
 */
UCLASS()
class BATTLEGRIDS_API ABGLobbyGameModeBase : public ABGGameModeBase
{
	GENERATED_BODY()

protected:

	ABGLobbyGameModeBase();

	virtual void BeginPlay() override;
};
