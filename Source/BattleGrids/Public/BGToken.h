// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GeometryCollection/GeometryCollectionSimulationTypes.h"

#include "BGToken.generated.h"

UCLASS()
class BATTLEGRIDS_API ABGToken : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABGToken();

	// Multicast, toggles whether or not the token position and rotation is locked.
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "BGToken|Functions")
	void ToggleLockTokenInPlace(bool bLock);

	// Returns the locked state of the token.
	UFUNCTION(BlueprintCallable, Category = "BGToken|Functions")
	bool GetIsTokenLocked() const;

	// Multicast, toggles physics and collision on the token
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "BGToken|Functions")
	void ToggleTokenPhysicsAndCollision(bool const bPhysicsOn, bool const bGravityOn,
	                                    ECollisionEnabled::Type const CollisionType);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BGToken|Config")
	FText TokenName;
};
