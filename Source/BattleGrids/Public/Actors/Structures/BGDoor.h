// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/BGStructure.h"
#include "BGDoor.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEGRIDS_API ABGDoor : public ABGStructure
{
	GENERATED_BODY()
public:

	ABGDoor();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "BGDoor|Network")
	void ToggleOpenClose();

protected:

	virtual void Tick(float DeltaSeconds) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* DoorFrameStaticMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* DoorStaticMeshComponent;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "BGDoor|Config")
	uint8 bOpen : 1;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "BGDoor|Config")
	float RotateValue {90.f};

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "BGDoor|Config")
	FRotator DoorRotation{};
};
