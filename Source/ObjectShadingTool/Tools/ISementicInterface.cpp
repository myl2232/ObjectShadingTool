#include "ISementicInterface.h"

#include "Kismet/GameplayStatics.h"

bool ISementicInterface::NewSemanticClass(const FString& ClassName,	const FColor& ClassColor)
{
	if (ClassName.Len() == 0)
	{
		UE_LOG(LogShadingTool, Warning, TEXT("%s: Cannot create a class with the name ''"), *FString(__FUNCTION__));
		return false;
	}

	// Check collisions with existing classes
	for (auto& Element : SemanticClasses)
	{
		const FShadingSemantic& SemanticClass = Element.Value;
		if (SemanticClass.Name == ClassName || SemanticClass.Color == ClassColor)
		{
			UE_LOG(LogShadingTool, Warning, TEXT("%s: New semantic class (%s, (%d %d %d)) colliding with existing (%s, (%d %d %d))"),
				*FString(__FUNCTION__),
				*ClassName, ClassColor.R, ClassColor.G, ClassColor.B,
				*SemanticClass.Name, SemanticClass.Color.R, SemanticClass.Color.G, SemanticClass.Color.B);
			return false;
		}
	}

	// Crate the new class
	FShadingSemantic& NewSemanticClass = SemanticClasses.Add(ClassName);
	NewSemanticClass.Name = ClassName;
	NewSemanticClass.Color = ClassColor;
	// The semantic class material instance will be created when it's needed
	
	// Broadcast the semantic classes change
	SemanticClassesUpdatedEvent.Broadcast();
	
	return true;
}

/** Gets the class color if it exists */
FColor ISementicInterface::ClassColor(const FString& ClassName)
{
	if (SemanticClasses.Contains(ClassName))
	{
		return SemanticClasses[ClassName].Color;
	}
	return FColor::White;
}

/** Update the name of a semantic class */
bool ISementicInterface::UpdateClassName(const FString& OldClassName, const FString& NewClassName)
{
	if (OldClassName == NewClassName)
	{
		return true;
	}

	if (!SemanticClasses.Contains(OldClassName))
	{
		UE_LOG(LogShadingTool, Log, TEXT("%s: Previous semantic class '%s' not found"),
			*FString(__FUNCTION__), *OldClassName);
		return false;
	}

	if (SemanticClasses.Contains(NewClassName))
	{
		UE_LOG(LogShadingTool, Log, TEXT("%s: New semantic class '%s' already exists"),
			*FString(__FUNCTION__), *NewClassName);
		return false;
	}

	if (NewClassName.Len() == 0)
	{
		UE_LOG(LogShadingTool, Warning, TEXT("%s: Cannot update class name to ''"), *FString(__FUNCTION__));
		return false;
	}

	const FColor ClassColor = SemanticClasses[OldClassName].Color;

	// Remove the existing class
	SemanticClasses.Remove(OldClassName);
	// Add new class with the same color
	NewSemanticClass(NewClassName, ClassColor);
	// Update actor mappings to the new semantic class name
	TArray<AActor*> LevelActors;
	UGameplayStatics::GetAllActorsOfClass(GWorld/*GEditor->GetEditorWorldContext().World()*/, AActor::StaticClass(), LevelActors);
	for (AActor* Actor : LevelActors)
	{
		RegistActorClassPair(Actor, OldClassName, NewClassName);
		/*if (ActorClassPairs.Contains(Actor->GetActorGuid()) &&
			ActorClassPairs[Actor->GetActorGuid()] == OldClassName)
		{
			ActorClassPairs[Actor->GetActorGuid()] = NewClassName;
		}*/
	}
	// No action regarding actor materials necessary


	return true;
}

/** Update the color of a semantic class */
bool ISementicInterface::UpdateClassColor(const FString& ClassName, const FColor& NewClassColor)
{
	if (!SemanticClasses.Contains(ClassName))
	{
		UE_LOG(LogShadingTool, Log, TEXT("%s: Requested semantic class '%s' not found"),
			*FString(__FUNCTION__), *ClassName);
		return false;
	}

	if (SemanticClasses[ClassName].Color == NewClassColor)
	{
		return true;
	}

	// Check if color is already in use
	for (auto& Element : SemanticClasses)
	{
		const FShadingSemantic& SemanticClass = Element.Value;
		if (SemanticClass.Color == NewClassColor)
		{
			UE_LOG(LogShadingTool, Warning, TEXT("%s: Requested color (%d %d %d) already used by %s"),
				*FString(__FUNCTION__), NewClassColor.R, NewClassColor.G, NewClassColor.B, *SemanticClass.Name);
			return false;
		}
	}

	// Update the class color
	SemanticClasses[ClassName].Color = NewClassColor;
	// Invalidate the material instance
	SemanticClasses[ClassName].PlainColorMaterialInstance = nullptr;
	// Update each actor color immediately in case of the semantic view mode
	TArray<AActor*> LevelActors;
	UGameplayStatics::GetAllActorsOfClass(GEditor->GetEditorWorldContext().World(), AActor::StaticClass(), LevelActors);
	for (AActor* Actor : LevelActors)
	{
		RegistActorClassPair(Actor, ClassName, ClassName);		
	}

	return true;
}

/** Remove a semantic class */
bool ISementicInterface::RemoveSemanticClass(const FString& ClassName)
{
	if (!SemanticClasses.Contains(ClassName))
	{
		UE_LOG(LogShadingTool, Log, TEXT("%s: Requested semantic class '%s' not found"),
			*FString(__FUNCTION__), *ClassName);
		return false;
	}

	if (ClassName == UndefinedSemanticClassName)
	{
		// Just ignore removing the necessary undefined semantic class
		return true;
	}

	// Reset all actor to the undefined class
	TArray<AActor*> LevelActors;
	UGameplayStatics::GetAllActorsOfClass(GEditor->GetEditorWorldContext().World(), AActor::StaticClass(), LevelActors);
	for (AActor* Actor : LevelActors)
	{
		RegistActorClassPair(Actor, ClassName, UndefinedSemanticClassName);
	}

	// Remove the class
	SemanticClasses.Remove(ClassName);
	
	// Broadcast the semantic classes change
	SemanticClassesUpdatedEvent.Broadcast();
	
	return true;
}

/** Remove all semantic classes except for the default one */
void ISementicInterface::RemoveAllSemanticClasses()
{
	for (auto& Element : SemanticClasses)
	{
		const FString& ClassName = Element.Key;
		// Skip the default undefined semantic class
		if (ClassName != UndefinedSemanticClassName)
		{
			RemoveSemanticClass(ClassName);
		}
	}
}

TArray<FString> ISementicInterface::SemanticClassNames() const
{
	TArray<FString> SemanticClassNames;
	for (auto& Element : SemanticClasses)
	{
		SemanticClassNames.Add(Element.Key);
	}
	return SemanticClassNames;
}

TArray<const FShadingSemantic*> ISementicInterface::GetSemanticClasses() const
{
	TArray<const FShadingSemantic*> RetSemanticClasses;
	for (auto& Element : SemanticClasses)
	{
		RetSemanticClasses.Add(&Element.Value);
	}
	return RetSemanticClasses;
}