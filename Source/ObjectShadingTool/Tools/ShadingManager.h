#pragma once

#include "CoreMinimal.h"
#include "ISementicInterface.h"
#include "TextureBackupManager.h"
#include "Common/PathUtils.h"
#include "Common/ShadingData.h"
#include "Engine/World.h"
#include "Widgets/WidgetManager.h"

#include "ShadingManager.generated.h"

UCLASS()
class UShadingManager : public UObject, public ISementicInterface
{
	GENERATED_BODY()

	friend FWidgetManager;
	friend FSemanticClassesWidgetManager;
	
public:
	UShadingManager();
	~UShadingManager();
	
	void BindEvents();
	
protected:

	void Initialize();
	
	void OnLevelChanged(ERHIFeatureLevel::Type type);
	
	/** Handles adding a new actor to the level */
	void OnLevelActorAdded(AActor* Actor);

	/** Handles removing actor references from the manager */
	void OnLevelActorDeleted(AActor* Actor);

	/** Handles editor closing, making sure original mesh colors are selected */
	void OnEditorClose();
	
	/** Sets a semantic class to the actor */
	void SetSemanticClassToActor(AActor* Actor,	const FString& ClassName, const bool bForceDisplaySemanticClass = false, const bool bDelayAddingDescriptors = false);
	
	/** Set active actor texture style to original or semantic color */
	void CheckoutActorTexture(AActor* Actor, const ETextureStyle NewTextureStyle);
	
	/** Update mesh materials to show requested texture styles */
	void CheckoutTextureStyle(const ETextureStyle NewTextureStyle);

	/** Generates the semantic class material if needed and returns it */
	UMaterialInstanceConstant* GetSemanticClassMaterial(FShadingSemantic& SemanticClass);	
		
	/** Applies desired class to all selected actors */
	void ApplySemanticClassToSelectedActors(const FString& ClassName);

	virtual void RegistActorClassPair(AActor* Actor, FString OldClassName,  FString ClassName) override;	
		
private:
	
	/** Actor to semantic class name bindings */
	UPROPERTY(EditAnywhere, Category = "Actor Data")
	TMap<FGuid, FString> ActorClassPairs;
	
	UPROPERTY()
	UTextureBackupManager* TextureBackupManager = nullptr;
	
	/** Marks if events have already been bounded */
	bool bEventsBound;
		
	/** Currently selected texture style */
	ETextureStyle CurrentTextureStyle;
	
	/** Plain color material used for semantic mesh coloring */
	UPROPERTY()
	UMaterial* PlainColorMaterial = nullptr;

	UPROPERTY()
	UObjectShadingSet* ObjectShadings = nullptr;

	FDelegateHandle FHandle_Add;
	FDelegateHandle FHandle_Remove;
	FDelegateHandle FHandle_EditorClose;
	FDelegateHandle FHandle_LevelChange;
};
