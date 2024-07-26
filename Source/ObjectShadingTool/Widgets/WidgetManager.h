#pragma once
#include "CoreMinimal.h"
#include "SemanticClassesWidgetManager.h"

class UShadingManager;

class FWidgetManager
{
public:
	FWidgetManager();

	/** Handles the UI tab creation when requested */
	TSharedRef<SDockTab> OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs);
		
protected:
	
	/** Callback function handling the choosing of the semantic class inside the combo box */
	void OnSemanticClassComboBoxSelectionChanged(TSharedPtr<FString> StringItem, ESelectInfo::Type SelectInfo);
	/** Callback function handling the choosing of the texture style inside the combo box */
	void OnTextureStyleComboBoxSelectionChanged(TSharedPtr<FString> StringItem, ESelectInfo::Type SelectInfo);
	
	void OnSemanticClassesUpdated();
	
private:
	
	UShadingManager* ShadingManger;
	
	/** The name of the texture style representing original colors */
	static const FString TextureStyleColorName;

	/** The name of the texture style representing semantic colors */
	static const FString TextureStyleSemanticName;
	
	/** Manager that handles semantic class widget */
	FSemanticClassesWidgetManager SemanticsWidget;
	
	/** FStrings texture style names referenced by the combo box */
	TArray<TSharedPtr<FString>> TextureStyleNames;
	
	/** Semantic class combo box */
	TSharedPtr<SComboBox<TSharedPtr<FString>>> SemanticClassComboBox;
	
	/** FStrings semantic class names referenced by the combo box */
	TArray<TSharedPtr<FString>> SemanticClassNames;
};

