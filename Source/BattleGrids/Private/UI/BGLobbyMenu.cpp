// © 2021 Matthew Barham. All Rights Reserved.


#include "UI/BGLobbyMenu.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Core/BGGameInstance.h"
#include "Core/BGPlayerState.h"
#include "Kismet/KismetSystemLibrary.h"

bool UBGLobbyMenu::Initialize()
{
	bool const bSuccess = Super::Initialize();
	if (!bSuccess) return false;

	// Bind Callbacks
	if (!ensure(StartGameButton)) return false;
	StartGameButton->OnClicked.AddDynamic(this, &UBGLobbyMenu::StartGame);

	if (!ensure(ReadyButton)) return false;
	ReadyButton->OnClicked.AddDynamic(this, &UBGLobbyMenu::ToggleReady);

	if (!ensure(GameSettingsButton)) return false;
	GameSettingsButton->OnClicked.AddDynamic(this, &UBGLobbyMenu::OpenSettingsMenu);

	if (!ensure(SelectColorButton)) return false;
	SelectColorButton->OnClicked.AddDynamic(this, &UBGLobbyMenu::OpenColorMenu);

	if (!ensure(LeaveLobbyButton)) return false;
	LeaveLobbyButton->OnClicked.AddDynamic(this, &UBGLobbyMenu::LeaveLobby);

	// Only show the Start button for the Host, and the Ready button for the Clients
	if (UKismetSystemLibrary::IsServer(GetOwningPlayer()))
	{
		ReadyButton->SetVisibility(ESlateVisibility::Hidden);
		ReadyButton->SetIsEnabled(false);
	}
	else
	{
		StartGameButton->SetVisibility(ESlateVisibility::Hidden);
		StartGameButton->SetIsEnabled(false);
	}

	return true;
}

void UBGLobbyMenu::UpdateLobbyInfo(FBGServerData const ServerData) const
{
	ServerName->SetText(FText::FromString(ServerData.Name));
	CurrentPlayerCount->SetText(FText::FromString(FString::FromInt(ServerData.CurrentPlayers)));
}

void UBGLobbyMenu::StartGame()
{
}

void UBGLobbyMenu::ToggleReady()
{
}

void UBGLobbyMenu::OpenSettingsMenu()
{
}

void UBGLobbyMenu::OpenColorMenu()
{
}

void UBGLobbyMenu::LeaveLobby()
{
	if (MenuInterface)
	{
		Teardown();
		MenuInterface->LoadMainMenu();
	}
}
