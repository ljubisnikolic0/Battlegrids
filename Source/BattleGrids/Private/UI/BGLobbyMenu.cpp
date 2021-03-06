// © 2021 Matthew Barham. All Rights Reserved.


#include "UI/BGLobbyMenu.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Core/BGGameInstance.h"
#include "Core/BGPlayerState.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UI/BGConnectedPlayer.h"

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

UBGLobbyMenu::UBGLobbyMenu(FObjectInitializer const& ObjectInitializer)
{
	if (ConnectedPlayerWidgetClass->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Warning, TEXT("Found class %s"), *ConnectedPlayerWidgetClass->GetName());
	}
}

void UBGLobbyMenu::UpdateReadyButtonColorAndText(bool const bReady)
{
	if (!ensure(ReadyButtonText)) return;

	bReady ? ReadyButton->SetBackgroundColor(FLinearColor::Green) : ReadyButton->SetBackgroundColor(FLinearColor::Red);
	bReady
		? ReadyButtonText->SetText(FText::FromString("Ready"))
		: ReadyButtonText->SetText(FText::FromString("Not Ready"));
}

void UBGLobbyMenu::SetPlayerList(TArray<FBGPlayerInfo> ConnectedPlayersInformation)
{
	UE_LOG(LogTemp, Warning, TEXT("LobbyMenu Array Length: %d"), ConnectedPlayersInformation.Num())

	auto const World = this->GetWorld();
	if (!ensure(World)) return;

	PlayerList->ClearChildren();

	uint32 i = 0;
	for (FBGPlayerInfo const& PlayerInfo : ConnectedPlayersInformation)
	{
		UBGConnectedPlayer* ConnectedPlayer = CreateWidget<UBGConnectedPlayer>(World, ConnectedPlayerWidgetClass);
		if (!ensure(ConnectedPlayer)) return;

		ConnectedPlayer->SetPlayerInfo(PlayerInfo);
		ConnectedPlayer->Setup(this, i);
		++i;

		PlayerList->AddChild(ConnectedPlayer);
	}
}

void UBGLobbyMenu::UpdatePlayerInformationAtIndex(uint32 const Index, FBGPlayerInfo const& InPlayerInfo)
{
	SelectedIndex = Index;
	MenuInterface->UpdatePlayerInfo(Index, InPlayerInfo);
}

void UBGLobbyMenu::StartGame()
{
	if (MenuInterface)
	{
		MenuInterface->StartGame();
	}
}

void UBGLobbyMenu::ToggleReady()
{
	auto CastPlayerState = GetOwningPlayerState<ABGPlayerState>();
	if (CastPlayerState)
	{
		CastPlayerState->ClientToggleReady();
		UpdateReadyButtonColorAndText(CastPlayerState->GetPlayerInfo().bReady);
	}
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
