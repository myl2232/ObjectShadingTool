// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Widgets/WidgetManager.h"

class FObjectShadingToolModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
protected:
	void PluginButtonClicked();
	void RegisterMenus();
private:
	TSharedPtr<class FUICommandList> PluginCommands;
	FWidgetManager WidgetManager;
};
