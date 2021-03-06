// © 2021 Matthew Barham. All Rights Reserved.

#include "Core/BGPlayerState.h"


#include "Core/BGGameInstance.h"
#include "Core/BGGameModeBase.h"
#include "Engine/DemoNetDriver.h"
#include "Kismet/KismetSystemLibrary.h"

ABGPlayerState::ABGPlayerState()
{
}

void ABGPlayerState::BeginPlay()
{
	Super::BeginPlay();

	auto const GameInstance = GetGameInstance<UBGGameInstance>();
	if (GameInstance)
	{
		GameInstance->OnSaveGame.AddUniqueDynamic(this, &ABGPlayerState::ClientSetPlayerInfo);
	}
}

void ABGPlayerState::ClientToggleReady_Implementation()
{
	PlayerInfo.bReady = !PlayerInfo.bReady;
	ServerSetPlayerInfo(PlayerInfo);
}

void ABGPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABGPlayerState, PlayerInfo)
}

bool ABGPlayerState::CheckForExistingSaveData(FBGPlayerInfo& OutPlayerInfo)
{
	auto const GameInstance = GetGameInstance<UBGGameInstance>();
	if (GameInstance && GameInstance->DoesPlayerSaveExist())
	{
		if (GameInstance->LoadPlayerInfo(PlayerInfo))
			return true;
	}
	return false;
}

void ABGPlayerState::ClientSetPlayerInfo_Implementation(FBGPlayerInfo const& InPlayerInfo)
{
	PlayerInfo = InPlayerInfo;
	PlayerInfo.PlayerID = GetPlayerId();
	// PlayerInfo.PlayerName = FText::FromString(FString::Printf(TEXT("Player %d"), GetPlayerId()));
	if (UKismetSystemLibrary::IsServer(this))
	{
		PlayerInfo.bReady = true;
		PlayerInfo.bGameMasterPermissions = true;
	}
	UE_LOG(LogTemp, Warning, TEXT("PlayerState: PlayerID = %d"), GetPlayerId())

	ServerSetPlayerInfo(PlayerInfo);
}

void ABGPlayerState::ServerSetPlayerInfo_Implementation(FBGPlayerInfo const& InPlayerInfo)
{
	PlayerInfo = InPlayerInfo;

	auto const World = GetWorld();
	if (World)
	{
		auto const LobbyGameMode = World->GetAuthGameMode<ABGGameModeBase>();
		if (LobbyGameMode)
		{
			LobbyGameMode->UpdatePlayerInfoInArray(PlayerInfo);
			UE_LOG(LogTemp, Warning, TEXT("PlayerState: Updating Player Info Array"))
		}
	}
}

void ABGPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	UE_LOG(LogTemp, Warning, TEXT("PlayerState: CopyProperties()"))

	auto CastPlayerState = Cast<ABGPlayerState>(PlayerState);
	if (CastPlayerState)
	{
		CastPlayerState->PlayerInfo = PlayerInfo;
		ServerSetPlayerInfo(PlayerInfo);
	}
}

void ABGPlayerState::OverrideWith(APlayerState* PlayerState)
{
	Super::OverrideWith(PlayerState);

	UE_LOG(LogTemp, Warning, TEXT("PlayerState: OverrideWith()"))

	auto const CastPlayerState = Cast<ABGPlayerState>(PlayerState);
	if (CastPlayerState)
	{
		PlayerInfo = CastPlayerState->GetPlayerInfo();
		PlayerInfo.PlayerID = GetPlayerId();

		ClientSetPlayerInfo(PlayerInfo);
	}
}
