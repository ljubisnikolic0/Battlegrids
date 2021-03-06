// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "BGActor.h"

#include "BGStructure.generated.h"

class UBGSplineWallComponent;
class UStaticMeshComponent;

UCLASS()
class BATTLEGRIDS_API ABGStructure : public ABGActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABGStructure();

	/////////////////
	/// Getters

	UBGSplineWallComponent* GetParentSplineWallComponent() const { return ParentSplineWallComponent; }

	int GetIndexOnSplineStructure() const { return IndexOnSplineStructure; }

	/////////////////
	/// Setters

	void SetParentSplineWallComponent(UBGSplineWallComponent* NewParentSplineWallComponent);

	void SetIndexOnSplineStructure(int const Index) { IndexOnSplineStructure = Index; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "BGStructure|Config")
	UBGSplineWallComponent* ParentSplineWallComponent{nullptr};

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "BGStructure|Config")
	int IndexOnSplineStructure{-1};
};