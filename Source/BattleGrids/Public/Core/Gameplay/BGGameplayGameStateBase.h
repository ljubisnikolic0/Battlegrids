// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Core/BGGameStateBase.h"
#include "Core/BGTypes.h"

#include "BGGameplayGameStateBase.generated.h"

class ABGCharacter;
class ABGActor;

/**
 * GameStateBase child class for Gameplay levels in BattleGrids
 */
UCLASS()
class BATTLEGRIDS_API ABGGameplayGameStateBase : public ABGGameStateBase
{
	GENERATED_BODY()

public:
	ABGGameplayGameStateBase();

	UFUNCTION(BlueprintCallable, Category = "BGGameplayGameStateBase|Functions")
	TArray<ABGActor*> GetSpawnedActorsOfType(EBGActorType const& ActorType);

	UFUNCTION(BlueprintCallable, Category = "BGGameplayGameStateBase|Functions")
	TArray<ABGCharacter*> GetSpawnedCharacters();

	void AddSpawnedEntityToArray(ABGActor* ActorToAdd);
	void AddSpawnedEntityToArray(ABGCharacter* CharacterToAdd);

	UFUNCTION(Server, Reliable)
	void RefreshCurrentWorldState();

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "BGGameplayGameStateBase|Config",
		meta = (AllowPrivateAccess = true))
	TArray<ABGActor*> AllBGActors;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "BGGameplayGameStateBase|Config",
		meta = (AllowPrivateAccess = true))
	TArray<ABGCharacter*> AllBGCharacters;
};
