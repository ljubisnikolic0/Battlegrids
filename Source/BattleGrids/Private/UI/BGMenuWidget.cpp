// © 2021 Matthew Barham. All Rights Reserved.


#include "UI/BGMenuWidget.h"

#include "UI/BGThinkingPopup.h"

void UBGMenuWidget::Setup()
{
	this->AddToViewport();

	// set input mode to UI only and show mouse cursor
	auto const World = GetWorld();
	if (!ensure(World)) return;
	auto PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController)) return;

	this->SetOwningPlayer(PlayerController);

	FInputModeGameAndUI InputModeData;
	InputModeData.SetWidgetToFocus(this->TakeWidget());
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PlayerController->SetInputMode(InputModeData);

	PlayerController->bShowMouseCursor = true;
}

void UBGMenuWidget::Teardown()
{
	this->RemoveFromViewport();

	auto const World = GetWorld();
	if (!ensure(World)) return;
	auto PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController)) return;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PlayerController->SetInputMode(InputModeData);
}

void UBGMenuWidget::SetMenuInterface(IBGMenuInterface* NewMenuInterface)
{
	MenuInterface = NewMenuInterface;
}