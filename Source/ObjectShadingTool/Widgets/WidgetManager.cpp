#include "WidgetManager.h"

#include "Chaos/ChaosPerfTest.h"
#include "Tools/ShadingManager.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Text/STextBlock.h"

const FString FWidgetManager::TextureStyleColorName(TEXT("Original color textures"));
const FString FWidgetManager::TextureStyleSemanticName(TEXT("Semantic color textures"));

#define LOCTEXT_NAMESPACE "FWidgetManager"

FWidgetManager::FWidgetManager()
{
	ShadingManger = NewObject<UShadingManager>();

	ShadingManger->AddToRoot();
	// Register the semantic classes updated callback
	ShadingManger->OnSemanticClassesUpdated().AddRaw(this, &FWidgetManager::OnSemanticClassesUpdated);
	
	// Prepare content of the texture style checkout combo box
	TextureStyleNames.Add(MakeShared<FString>(TextureStyleColorName));
	TextureStyleNames.Add(MakeShared<FString>(TextureStyleSemanticName));

	SemanticsWidget.SetShadingManager(ShadingManger);
}

TSharedRef<SDockTab> FWidgetManager::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	// Bind events now that the editor has finished starting up
	if (ShadingManger)
	{
		ShadingManger->BindEvents();
	}
	
	// Update combo box semantic class names
	OnSemanticClassesUpdated();	

	// Generate the UI
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		.ContentPadding(2)
		[
			SNew(SScrollBox)			
			+SScrollBox::Slot()
			.Padding(2)
			[
				SNew(SButton)
				.OnClicked_Raw(&SemanticsWidget, &FSemanticClassesWidgetManager::OnManageSemanticClassesClicked)
				.Content()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("ManageSemanticClassesButtonText", "Manage Semantic Classes"))
				]
			]
			+SScrollBox::Slot()
			.Padding(2)
			[
				SAssignNew(SemanticClassComboBox, SComboBox<TSharedPtr<FString>>)
				.OptionsSource(&SemanticClassNames)
				.ContentPadding(2)
				.OnGenerateWidget_Lambda(
					[](TSharedPtr<FString> StringItem)
					{ return SNew(STextBlock).Text(FText::FromString(*StringItem)); })
				.OnSelectionChanged_Raw(this, &FWidgetManager::OnSemanticClassComboBoxSelectionChanged)
				.Content()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("PickSemanticClassComboBoxText", "Pick a semantic class"))
				]
			]
			+SScrollBox::Slot()
			.Padding(2)
			[
				SNew(SComboBox<TSharedPtr<FString>>)
				.OptionsSource(&TextureStyleNames)
				.ContentPadding(2)
				.OnGenerateWidget_Lambda(
					[](TSharedPtr<FString> StringItem)
					{ return SNew(STextBlock).Text(FText::FromString(*StringItem)); })
				.OnSelectionChanged_Raw(this, &FWidgetManager::OnTextureStyleComboBoxSelectionChanged)
				.Content()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("PickMeshTextureStyleComboBoxText", "Pick a mesh texture style"))
				]
			]	
		];
}

void FWidgetManager::OnSemanticClassComboBoxSelectionChanged(
	TSharedPtr<FString> StringItem,
	ESelectInfo::Type SelectInfo)
{
	if (StringItem.IsValid())
	{
		UE_LOG(LogShadingTool, Log, TEXT("%s: Semantic class selected: %s"), *FString(__FUNCTION__), **StringItem)
		ShadingManger->ApplySemanticClassToSelectedActors(*StringItem);
		SemanticClassComboBox->ClearSelection();
	}
}

void FWidgetManager::OnTextureStyleComboBoxSelectionChanged(TSharedPtr<FString> StringItem, ESelectInfo::Type SelectInfo)
{
	if (StringItem.IsValid())
	{
		UE_LOG(LogShadingTool, Log, TEXT("%s: Texture style selected: %s"), *FString(__FUNCTION__), **StringItem)
		if (*StringItem == TextureStyleColorName)
		{
			ShadingManger->CheckoutTextureStyle(ETextureStyle::COLOR);
		}
		else if (*StringItem == TextureStyleSemanticName)
		{
			ShadingManger->CheckoutTextureStyle(ETextureStyle::SEMANTIC);
		}
		else
		{
			UE_LOG(LogShadingTool, Error, TEXT("%s: Got unexpected texture style: %s"),
				*FString(__FUNCTION__), **StringItem);
		}
	}
}

void FWidgetManager::OnSemanticClassesUpdated()
{
	// Refresh the list of semantic classes
	SemanticClassNames.Reset();
	TArray<FString> ClassNames = ShadingManger->SemanticClassNames();
	for (const FString& ClassName : ClassNames)
	{
		SemanticClassNames.Add(MakeShared<FString>(ClassName));
	}

	// Refresh the combo box
	if (SemanticClassComboBox.IsValid())
	{
		SemanticClassComboBox->RefreshOptions();
	}
	else
	{
		UE_LOG(LogShadingTool, Error, TEXT("%s: Semantic class picker is invalid, could not refresh"),
			*FString(__FUNCTION__));
	}
}
