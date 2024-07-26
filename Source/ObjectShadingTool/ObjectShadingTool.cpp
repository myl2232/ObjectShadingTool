// Copyright Epic Games, Inc. All Rights Reserved.

#include "ObjectShadingTool.h"
#include "Widgets/ShadingToolCommands.h"
#include "Widgets/ShadingStyle.h"
#include "ToolMenus.h"

#define LOCTEXT_NAMESPACE "FObjectShadingToolModule"

static const FName ShadingToolTabName("ShadingTool");

void FObjectShadingToolModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FShadingStyle::Initialize();
	FShadingStyle::ReloadTextures();

	FShadingToolCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FShadingToolCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FObjectShadingToolModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FObjectShadingToolModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ShadingToolTabName, FOnSpawnTab::CreateRaw(&WidgetManager, &FWidgetManager::OnSpawnPluginTab))
			.SetDisplayName(LOCTEXT("ShadingToolTabTitle", "ShadingTool"))
			.SetMenuType(ETabSpawnerMenuType::Hidden);	
}

void FObjectShadingToolModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

void FObjectShadingToolModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(ShadingToolTabName);
}

void FObjectShadingToolModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FShadingToolCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FShadingToolCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FObjectShadingToolModule, ObjectShadingTool)