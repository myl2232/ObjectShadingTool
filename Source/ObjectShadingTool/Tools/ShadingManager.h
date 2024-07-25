#pragma once
#include "TextureBackupManager.h"
#include "Common/PathUtils.h"
#include "Common/ShadingData.h"
#include "Engine/World.h"

class FShadingManager
{
public:
	FShadingManager();
	~FShadingManager();
	
protected:
	void Initialize();
	
	void BindEvents();

	void OnLevelChanged();
	
	/** Handles adding a new actor to the level */
	void OnLevelActorAdded(AActor* Actor);

	/** Handles removing actor references from the manager */
	void OnLevelActorDeleted(AActor* Actor);

#if PLATFORM_WINDOWS
	/** Handles editor closing, making sure original mesh colors are selected */
	void OnEditorClose();
#endif
	
	/** Sets a semantic class to the actor */
	void SetSemanticClassToActor(AActor* Actor,	const FString& ClassName, const bool bForceDisplaySemanticClass = false, const bool bDelayAddingDescriptors = false);
	
	/** Set active actor texture style to original or semantic color */
	void CheckoutActorTexture(AActor* Actor, const ETextureStyle NewTextureStyle);
	
	/** Update mesh materials to show requested texture styles */
	void CheckoutTextureStyle(const ETextureStyle NewTextureStyle);

	/** Generates the semantic class material if needed and returns it */
	UMaterialInstanceConstant* GetSemanticClassMaterial(FShadingSemantic& SemanticClass);
	
private:
	TMap<FString, FShadingSemantic> SemanticClasses;
	
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

	//FWorldDelegates::FOnLevelChanged LevelChangedToWorld;
	
	/** The name of the semantic color material parameter */
	static const FString SemanticColorParameter;

	/** The name of the Undefined semantic class */
	static const FString UndefinedSemanticClassName;
};
