// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "BGToken.generated.h"

class AAIController;
class ABGPlayerState;
class UCapsuleComponent;
class UCharacterMovementComponent;
class UStaticMeshComponent;
class UStaticMesh;

UCLASS()
class BATTLEGRIDS_API ABGToken : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABGToken();

	// Call when spawning Token via DeferredActorSpawn function to set the mesh and material
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "BGToken|Functions")
	void InitializeMeshAndMaterial(UStaticMesh* ModelStaticMesh, UMaterialInstance* MaterialInstance,
	                               UStaticMesh* BaseStaticMesh) const;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Multicast, toggles whether or not the token position and rotation is locked.
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "BGToken|Functions")
	void ToggleLockTokenInPlace(bool bLock);

	// Returns the locked state of the token.
	UFUNCTION(BlueprintCallable, Category = "BGToken|Functions")
	bool GetIsTokenLocked() const;

	// Checks whether or not a PlayerState exists in the Token's permissions array
	UFUNCTION(BlueprintCallable, Category = "BGToken|Functions")
	bool PlayerHasPermissions(ABGPlayerState const* PlayerStateToCheck);

	UFUNCTION(BlueprintCallable, Category = "BGToken|Functions")
	bool AddPlayerToPermissionsArray(ABGPlayerState* PlayerStateToAdd);

	UFUNCTION(BlueprintCallable, Category = "BGToken|Functions")
	bool RemovePlayerFromPermissionsArray(ABGPlayerState* PlayerStateToRemove);

	//////////////////////
	/// Getters

	UStaticMeshComponent* GetTokenModelStaticMeshComponent() const { return TokenModelStaticMeshComponent; }
	UStaticMeshComponent* GetTokenBaseStaticMeshComponent() const { return TokenBaseStaticMeshComponent; }

	AAIController* GetCharacterAIController() const { return CharacterAIController; }

	TArray<ABGPlayerState*> GetPlayerPermissions() const { return PlayerPermissions; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void FellOutOfWorld(const UDamageType& dmgType) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* TokenBaseStaticMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* TokenModelStaticMeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BGToken|Config")
	FText TokenName;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "BGToken|Config")
	TArray<ABGPlayerState*> PlayerPermissions;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config")
	AAIController* CharacterAIController;
};