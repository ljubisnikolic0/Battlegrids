// © 2021 Matthew Barham. All Rights Reserved.

#include "Core/Menu/BGMenuPlayerController.h"
#include "Core/BGGameInstance.h"

void ABGMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	auto GameInstance = GetGameInstance<UBGGameInstance>();

	if (bLoading && GameInstance)
	{
		GameInstance->ToggleLoadingScreen(true);
	}
}

void ABGMenuPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bLoading && GetWorld()->GetGameState() && PlayerState)
	{
		auto GameInstance = GetGameInstance<UBGGameInstance>();
		GameInstance->LoadMainMenuWidget();
		ToggleLoadingState(false);
		GameInstance->ToggleLoadingScreen(false);
	}
}