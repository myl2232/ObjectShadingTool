#include "WidgetManager.h"
#include "Tools/ShadingManager.h"

FWidgetManager::FWidgetManager()
{
	ShadingManger = NewObject<UShadingManager>();
}

TSharedRef<SDockTab> FWidgetManager::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	// Bind events now that the editor has finished starting up
	if (ShadingManger)
	{
		ShadingManger->BindEvents();
	}
	
	return SNew(SDockTab);
}