#include "ShadingManager.h"

#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "Kismet/GameplayStatics.h"

#if PLATFORM_WINDOWS
#include "FileHelpers.h"
#endif


FShadingManager::FShadingManager()
{
	bEventsBound = false;

	CurrentTextureStyle = ETextureStyle::COLOR;
	
	TextureBackupManager = NewObject<UTextureBackupManager>();

	PlainColorMaterial = DuplicateObject<UMaterial>(LoadObject<UMaterial>(nullptr, *FPathUtils::PlainColorMaterialPath()), nullptr);

	Initialize();
}

FShadingManager::~FShadingManager()
{
	
}

void FShadingManager::Initialize()
{	
	BindEvents();
}

void FShadingManager::BindEvents()
{
	// Bind event to OnLevelActorDeleted event to remove the actor from our buffers
	if (!bEventsBound)
	{
		GEngine->OnLevelActorAdded().AddUObject(this, &FShadingManager::OnLevelActorAdded);
		GEngine->OnLevelActorDeleted().AddUObject(this, &FShadingManager::OnLevelActorDeleted);
		GEngine->OnEditorClose().AddUObject(this, &FShadingManager::OnEditorClose);
		GWorld->OnLevelsChanged().AddUObject(this, &FShadingManager::OnLevelChanged);
	
		bEventsBound = true;
	}
}

void FShadingManager::OnLevelChanged()
{
	ActorClassPairs.Empty();
}

void FShadingManager::OnLevelActorAdded(AActor* Actor)
{
	UE_LOG(LogShadingTool, Log, TEXT("%s: Adding actor '%s'"), *FString(__FUNCTION__), *Actor->GetName())

	// Preemptively assign the undefined semantic class to the new actor
	// In the case of the semantic mode being selected, assigned class will be immediately displayed
	const bool bForceDisplaySemanticClass = false;
	const bool bDelayAddingDescriptors = true;
	SetSemanticClassToActor(Actor, UndefinedSemanticClassName, bForceDisplaySemanticClass, bDelayAddingDescriptors);
}

void FShadingManager::OnLevelActorDeleted(AActor* Actor)
{
	UE_LOG(LogShadingTool, Log, TEXT("%s: Removing actor '%s'"), *FString(__FUNCTION__), *Actor->GetName())
	ActorClassPairs.Remove(Actor->GetActorGuid());
	TextureBackupManager->RemoveActor(Actor);
}

#if PLATFORM_WINDOWS
void FShadingManager::OnEditorClose()
{
	UE_LOG(LogShadingTool, Log, TEXT("%s: Making sure original mesh colors are selected"), *FString(__FUNCTION__))
	CheckoutTextureStyle(ETextureStyle::COLOR);
	// Make level dirty and save it
	ULevel* Level = GWorld->GetCurrentLevel();
	Level->MarkPackageDirty();
	FEditorFileUtils::SaveLevel(Level);
}
#endif

void FShadingManager::SetSemanticClassToActor(AActor* Actor, const FString& ClassName, const bool bForceDisplaySemanticClass, const bool bDelayAddingDescriptors)
{
	// Remove class if already assigned
	ActorClassPairs.Remove(Actor->GetActorGuid());

	// Set the new class
	ActorClassPairs.Add(Actor->GetActorGuid(), ClassName);

	/*// Immediately display the change when in the semantic mode
	if (CurrentTextureStyle == ETextureStyle::SEMANTIC)
	{
		if (bDelayAddingDescriptors)
		{
			// Adding with a delay is requested, add the actor to the delay buffer, start the timer,
			// stop the current execution and the callback will handle the rest
			DelayActorBuffer.Add(Actor);
			const float DelaySeconds = 0.2f;
			const bool bLoop = false;
			GEditor->GetEditorWorldContext().World()->GetTimerManager().SetTimer(
				DelayActorTimerHandle,
				this,
				&UTextureStyleManager::ProcessDelayActorBuffer,
				DelaySeconds,
				bLoop);
			return;
		}
	}*/

	// No delay is requested, checkout the semantic color
	if (bForceDisplaySemanticClass || CurrentTextureStyle == ETextureStyle::SEMANTIC)
	{
		CheckoutActorTexture(Actor, ETextureStyle::SEMANTIC);
	}

	// No need to save the TextureMappingAsset for every actor, the caller will do it
}

void FShadingManager::CheckoutActorTexture(AActor* Actor, const ETextureStyle NewTextureStyle)
{
	// Check if the actor has a semantic class assigned
	const FGuid& ActorGuid = Actor->GetActorGuid();
	if (!ActorClassPairs.Contains(ActorGuid))
	{
		if (NewTextureStyle == ETextureStyle::SEMANTIC)
		{
			// If semantic view is being selected, assign the default class to the actor
			// This method will be recalled by the following method
			const bool bForceDisplaySemanticClass = true;
			SetSemanticClassToActor(Actor, UndefinedSemanticClassName, bForceDisplaySemanticClass);
		}
		return;
	}

	// Get the name of the semantic class assigned to the actor
	const FString& ClassName = ActorClassPairs[ActorGuid];

	// Make sure the semantic class name is valid
	if (!SemanticClasses.Contains(ClassName))
	{
		UE_LOG(LogShadingTool, Error, TEXT("%s: Uknown class name '%s'"), *FString(__FUNCTION__), *ClassName)
		return;
	}

	// Check whether the actor currently has its original materials active
	// It it does, the original materials will be backed up
	const bool bOriginalTextureActive = !TextureBackupManager->ContainsActor(Actor);

	// If the actor has original texture and is changing to the same one, ignore the actor
	if (bOriginalTextureActive && NewTextureStyle == ETextureStyle::COLOR)
	{
		return;
	}

	// Update the actor texture
	const bool bDoAdd = bOriginalTextureActive;
	const bool bDoPaint = true;
	UMaterialInstanceConstant* Material = nullptr;
	if (NewTextureStyle == ETextureStyle::SEMANTIC)
	{
		Material = GetSemanticClassMaterial(SemanticClasses[ClassName]);
	}
	TextureBackupManager->AddAndPaint(Actor, bDoAdd, bDoPaint, Material);
}

void FShadingManager::CheckoutTextureStyle(const ETextureStyle NewTextureStyle)
{
	UE_LOG(LogShadingTool, Log, TEXT("%s: New texture style: %d"), *FString(__FUNCTION__), NewTextureStyle)

	if (NewTextureStyle == CurrentTextureStyle)
	{
		// Return if the desired style is already selected
		UE_LOG(LogShadingTool, Log, TEXT("%s: TextureStyle %d already selected"), *FString(__FUNCTION__), NewTextureStyle)
		return;
	}

	// Apply materials to all actors
	TArray<AActor*> LevelActors;
	UGameplayStatics::GetAllActorsOfClass(GWorld, AActor::StaticClass(), LevelActors);
	for (AActor* Actor : LevelActors)
	{
		CheckoutActorTexture(Actor, NewTextureStyle);
	}

	/*// Make sure any changes to the TextureMappingAsset are changed
	SaveTextureMappingAsset();*/

	CurrentTextureStyle = NewTextureStyle;
}

UMaterialInstanceConstant* FShadingManager::GetSemanticClassMaterial(FShadingSemantic& SemanticClass)
{
	// If the plain color material is null, create it
	if (SemanticClass.PlainColorMaterialInstance == nullptr)
	{
		UMaterialInstanceConstantFactoryNew* Factory = NewObject<UMaterialInstanceConstantFactoryNew>();
		Factory->InitialParent = PlainColorMaterial;

		const FString PackageFileName = FString::Printf(TEXT("M_%s"), *(SemanticClass.Name));
		const FString PackagePath =
			FPathUtils::ProjectPluginContentDir() / FString(TEXT("ConstMaterials")) / PackageFileName;
		UPackage* Package = CreatePackage(*PackagePath);
		SemanticClass.PlainColorMaterialInstance = Cast<UMaterialInstanceConstant>(Factory->FactoryCreateNew(
			UMaterialInstanceConstant::StaticClass(),
			Package,
			*PackageFileName,
			RF_Public | RF_Transient,
			NULL,
			GWarn));

		if (SemanticClass.PlainColorMaterialInstance == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("%s: Could not create the plain color material instance"),
				*FString(__FUNCTION__))
			check(SemanticClass.PlainColorMaterialInstance)
		}
		SemanticClass.PlainColorMaterialInstance->SetVectorParameterValueEditorOnly(
			*SemanticColorParameter,
			SemanticClass.Color);
	}

	return SemanticClass.PlainColorMaterialInstance;
}