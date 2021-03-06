// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "BGTypes.h"
#include "GameFramework/GameStateBase.h"
#include "BGGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEGRIDS_API ABGGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:

	ABGGameStateBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "BGGameStateBase|Config")
	TArray<FBGPlayerInfo> ConnectedPlayersInfo;
};
