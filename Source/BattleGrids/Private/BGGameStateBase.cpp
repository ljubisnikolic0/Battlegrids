// © 2021 Matthew Barham. All Rights Reserved.


#include "BGGameStateBase.h"

#include "Engine/DemoNetDriver.h"

ABGGameStateBase::ABGGameStateBase()
{
	
}

void ABGGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABGGameStateBase, ConnectedPlayersInfo)
}
