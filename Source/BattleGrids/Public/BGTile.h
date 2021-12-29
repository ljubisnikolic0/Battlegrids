// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BGTile.generated.h"

UCLASS()
class BATTLEGRIDS_API ABGTile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABGTile();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "BGTile|Functions")
	void ToggleTileVisibility(bool bHide);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UStaticMeshComponent* StaticMeshComponent;

};
