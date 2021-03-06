// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Core/BGTypes.h"
#include "GameFramework/Character.h"
#include "BGCharacter.generated.h"

class AAIController;
class ABGPlayerState;
class UCapsuleComponent;
class UCharacterMovementComponent;
class UStaticMeshComponent;
class UStaticMesh;
class UWidgetComponent;
class UBGContextMenu;

/**
* BattleGrids Character class to be used for miniatures and models
*/
UCLASS()
class BATTLEGRIDS_API ABGCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABGCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Call when spawning Token via DeferredActorSpawn function to set the mesh and material
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "BGToken|Functions")
	void InitializeMeshAndMaterial(UStaticMesh* ModelStaticMesh, UMaterialInstance* MaterialInstance,
								UStaticMesh* BaseStaticMesh) const;

	// Multicast, toggles whether or not the token position and rotation is locked.
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "BGToken|Functions")
	void ToggleLockTokenInPlace(bool const bLock);

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

	UStaticMeshComponent* GetTokenModelStaticMeshComponent() const { return CharacterModelStaticMeshComponent; }
	UStaticMeshComponent* GetTokenBaseStaticMeshComponent() const { return CharacterBaseStaticMeshComponent; }

	AAIController* GetCharacterAIController() const { return CharacterAIController; }

	TArray<ABGPlayerState*> GetPlayerPermissions() const { return PlayerPermissions; }

	FBGCharacterModelSaveInfo GetCharacterModelSaveInfo() const { return CharacterModelSaveInfo; }

	//////////////////////
	/// Setters
	
	void SetCharacterModelSaveInfo(FBGCharacterModelSaveInfo const& InSaveInfo);

	void UpdateCharacterSaveLocation();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void FellOutOfWorld(const UDamageType& dmgType) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* CharacterBaseStaticMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* CharacterModelStaticMeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BGCharacter|Config")
	FText CharacterName;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "BGCharacter|Config")
	TArray<ABGPlayerState*> PlayerPermissions;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config")
	AAIController* CharacterAIController;

	/** Character Save Data */
	UPROPERTY(Replicated)
	FBGCharacterModelSaveInfo CharacterModelSaveInfo;
};