// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Core/BGTypes.h"
#include "GameFramework/Actor.h"
#include "BGActor.generated.h"

class UWidgetComponent;

/**
 * BattleGrands custom Actor from which all in-game objects inherit
 */
UCLASS()
class BATTLEGRIDS_API ABGActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABGActor();

	//////////////////////
	/// Getters

	EBGActorType GetActorType() const { return ActorType; }

	bool GetIsLocked() const { return bIsLocked; }

	//////////////////////
	/// Setters

	void SetActorType(EBGActorType const& InActorType);

	void SetupComponentSlot(UActorComponent* InComponent);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "BGActor|Functions")
	void ToggleLocked(bool const bSetLocked);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* RootSceneComponent;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Components")
	UActorComponent* ComponentSlot;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "BGActor|Config")
	EBGActorType ActorType;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "BGActor|Config")
	uint8 bIsLocked: 1;
};