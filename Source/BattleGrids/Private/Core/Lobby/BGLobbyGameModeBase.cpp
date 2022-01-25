// © 2021 Matthew Barham. All Rights Reserved.


#include "Core/Lobby/BGLobbyGameModeBase.h"

#include "Core/BGGameInstance.h"
#include "Core/BGPlayerState.h"
#include "Core/Lobby/BGLobbyPlayerController.h"
#include "UI/BGLobbyMenu.h"

void ABGLobbyGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if (!GameInstance)
	{
		GameInstance = Cast<UBGGameInstance>(GetGameInstance());
	}

	ServerData = GameInstance->GetInitialServerData();
	ServerData.CurrentPlayers = ConnectedPlayers.Num();

	ServerReady = true;
}

void ABGLobbyGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	while (!ServerReady) {}

	ConnectedPlayers.AddUnique(NewPlayer);

	if (!GameInstance)
	{
		GameInstance = Cast<UBGGameInstance>(GetGameInstance());
	}

	ServerData.CurrentPlayers++;
	UpdateAllConnectedPlayers();
}

void ABGLobbyGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	UE_LOG(LogTemp, Warning, TEXT("Player Left"))

	for (int i{}; i < ConnectedPlayers.Num(); ++i)
	{
		if (ConnectedPlayers[i] == Exiting)
		{
			ConnectedPlayers.RemoveAt(i);
			UE_LOG(LogTemp, Warning, TEXT("Removed Player from Connected Players Array"))
		}
	}

	if (!GameInstance)
	{
		GameInstance = Cast<UBGGameInstance>(GetGameInstance());
	}

	ServerData.CurrentPlayers--;
	UpdateAllConnectedPlayers();
}

void ABGLobbyGameModeBase::UpdateAllConnectedPlayers()
{
	for (auto PC : ConnectedPlayers)
	{
		if (auto LobbyPC = Cast<ABGLobbyPlayerController>(PC))
		{
			LobbyPC->UpdateServerData(ServerData);
		}
	}
}
