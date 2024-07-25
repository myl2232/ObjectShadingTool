#pragma once
#include "CoreMinimal.h"

class UShadingManager;

class FWidgetManager
{
public:
	FWidgetManager();

	/** Handles the UI tab creation when requested */
	TSharedRef<SDockTab> OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs);

protected:
	
	UShadingManager* ShadingManger;

};

