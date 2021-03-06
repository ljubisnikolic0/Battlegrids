// © 2021 Matthew Barham. All Rights Reserved.

#include "Characters/BGCharacter.h"

#include "Net/UnrealNetwork.h"
#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DemoNetDriver.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UI/BGContextMenu.h"

ABGCharacter::ABGCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	// AI Controller is NECESSARY in order for AddMovementInput() to work!!!
	AutoPossessAI = EAutoPossessAI::Spawned;

	auto Capsule = GetCapsuleComponent();

	// Capsule->SetIsReplicated(true);
	Capsule->bDynamicObstacle = true;
	Capsule->InitCapsuleSize(20.f, 60.f);
	Capsule->SetCollisionProfileName("TokenCapsule");

	CharacterBaseStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Token Base Mesh"));
	CharacterBaseStaticMeshComponent->SetupAttachment(Capsule);
	CharacterBaseStaticMeshComponent->SetRelativeLocation(FVector(0.f, 0.f, -60.f));
	CharacterBaseStaticMeshComponent->SetCollisionProfileName("Token");
	CharacterBaseStaticMeshComponent->SetIsReplicated(true);

	CharacterModelStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Token Model Mesh"));
	CharacterModelStaticMeshComponent->SetCollisionProfileName("Token");
	CharacterModelStaticMeshComponent->SetIsReplicated(true);
}

void ABGCharacter::SetCharacterModelSaveInfo(FBGCharacterModelSaveInfo const& InSaveInfo)
{
	CharacterModelSaveInfo = InSaveInfo;
}

void ABGCharacter::UpdateCharacterSaveLocation()
{
	CharacterModelSaveInfo.CharacterLocation = GetActorLocation();
}

void ABGCharacter::BeginPlay()
{
	Super::BeginPlay();

	CharacterAIController = Cast<AAIController>(GetController());
	if (CharacterAIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("PawnAIController Successfully Initialized"))
	}

	SetReplicatingMovement(true);
	CharacterModelStaticMeshComponent->SetSimulatePhysics(false);
	CharacterModelStaticMeshComponent->AttachToComponent(CharacterBaseStaticMeshComponent,
	                                                 FAttachmentTransformRules(EAttachmentRule::KeepRelative, true),
	                                                 TEXT("ModelRoot"));
}

void ABGCharacter::FellOutOfWorld(const UDamageType& dmgType)
{
	Super::FellOutOfWorld(dmgType);

	Destroy();
}

void ABGCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABGCharacter, PlayerPermissions)
	DOREPLIFETIME(ABGCharacter, CharacterModelSaveInfo)
}

void ABGCharacter::ToggleLockTokenInPlace_Implementation(bool const bLock)
{
	bLock ? GetCharacterMovement()->DisableMovement() : GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void ABGCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

bool ABGCharacter::GetIsTokenLocked() const
{
	if (GetCharacterMovement()->MovementMode == MOVE_None)
		return true;
	return false;
}

bool ABGCharacter::PlayerHasPermissions(ABGPlayerState const* PlayerStateToCheck)
{
	for (auto It : PlayerPermissions)
		if (It == PlayerStateToCheck)
			return true;

	return false;
}

bool ABGCharacter::AddPlayerToPermissionsArray(ABGPlayerState* PlayerStateToAdd)
{
	if (PlayerPermissions.IsValidIndex(PlayerPermissions.AddUnique(PlayerStateToAdd)))
	{
		return true;
	}
	return false;
}

bool ABGCharacter::RemovePlayerFromPermissionsArray(ABGPlayerState* PlayerStateToRemove)
{
	if (PlayerPermissions.Contains(PlayerStateToRemove))
	{
		if (PlayerPermissions.Remove(PlayerStateToRemove) != 0)
		{
			return true;
		}
	}
	return false;
}

void ABGCharacter::InitializeMeshAndMaterial_Implementation(UStaticMesh* ModelStaticMesh,
                                                            UMaterialInstance* MaterialInstance,
                                                            UStaticMesh* BaseStaticMesh) const
{
	if (BaseStaticMesh && ModelStaticMesh && MaterialInstance)
	{
		CharacterBaseStaticMeshComponent->SetStaticMesh(BaseStaticMesh);
		CharacterBaseStaticMeshComponent->SetMaterial(0, MaterialInstance);
		CharacterModelStaticMeshComponent->SetStaticMesh(ModelStaticMesh);
		CharacterModelStaticMeshComponent->SetMaterial(0, MaterialInstance);
	}
}
