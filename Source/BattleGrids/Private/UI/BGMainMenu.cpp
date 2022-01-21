// © 2021 Matthew Barham. All Rights Reserved.


#include "UI/BGMainMenu.h"
#include "UI/BGServerRow.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"

void UBGMainMenu::SetServerList(TArray<FString> ServerNames)
{
	auto const World = this->GetWorld();
	if (!ensure(World)) return;

	ServerList->ClearChildren();
	
	for (auto ServerName : ServerNames)
	{
		ServerRow = CreateWidget<UBGServerRow>(World, ServerRowClass);
		if (!ensure(ServerRow)) return;

		ServerRow->SetServerName(ServerName);

		ServerList->AddChild(ServerRow);
	}
}

UBGMainMenu::UBGMainMenu(FObjectInitializer const& ObjectInitializer)
{
	if (ServerRowClass->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Warning, TEXT("Found class %s"), *ServerRowClass->GetName());
	}
}

bool UBGMainMenu::Initialize()
{
	bool const bSuccess = Super::Initialize();
	if (!bSuccess) return false;

	// Initialize Main Menu widgets and bind Callbacks
	if (!ensure(HostButton)) return false;
	HostButton->OnClicked.AddDynamic(this, &UBGMainMenu::HostServer);

	if (!ensure(JoinButton)) return false;
	JoinButton->OnClicked.AddDynamic(this, &UBGMainMenu::OpenJoinMenu);

	if (!ensure(QuitButton)) return false;
	QuitButton->OnClicked.AddDynamic(this, &UBGMainMenu::QuitPressed);

	// Initialize Join Menu widgets and bind Callbacks
	if (!ensure(CancelJoinMenuButton)) return false;
	CancelJoinMenuButton->OnClicked.AddDynamic(this, &UBGMainMenu::OpenMainMenu);

	if (!ensure(ConfirmJoinMenuButton)) return false;
	ConfirmJoinMenuButton->OnClicked.AddDynamic(this, &UBGMainMenu::JoinServer);

	return true;
}

void UBGMainMenu::HostServer()
{
	if (MenuInterface)
	{
		MenuInterface->Host();
	}
}

void UBGMainMenu::JoinServer()
{
	if (MenuInterface)
	{
		// if (!ensure(IPAddressField)) return;
		// auto const Address = IPAddressField->GetText().ToString();
		MenuInterface->Join("");
	}
}

void UBGMainMenu::OpenJoinMenu()
{
	if (!ensure(MenuSwitcher)) return;
	if (!ensure(JoinMenu)) return;
	MenuSwitcher->SetActiveWidget(JoinMenu);

	if (MenuInterface)
	{
		MenuInterface->RefreshServerList();
	}
}

void UBGMainMenu::OpenMainMenu()
{
	if (!ensure(MenuSwitcher)) return;
	if (!ensure(MainMenu)) return;
	MenuSwitcher->SetActiveWidget(MainMenu);
}

void UBGMainMenu::QuitPressed()
{
	auto const World = GetWorld();
	if (!ensure(World)) return;

	auto PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController)) return;

	PlayerController->ConsoleCommand("quit");
}
