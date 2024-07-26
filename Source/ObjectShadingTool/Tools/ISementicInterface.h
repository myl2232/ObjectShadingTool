#pragma once
#include "Common/ShadingData.h"
#include "Common/ShadingCommon.h"

struct ISementicInterface
{
public:
	virtual ~ISementicInterface(){};
	
	virtual void RegistActorClassPair(AActor* Actor, FString OldClassName, FString ClassName){};
	
	/** Create new semantic class */
	bool NewSemanticClass(const FString& ClassName,	const FColor& ClassColor);
	/** Gets the class color if it exists */
	FColor ClassColor(const FString& ClassName);

	/** Update the name of a semantic class */
	bool UpdateClassName(const FString& OldClassName, const FString& NewClassName);

	/** Update the color of a semantic class */
	bool UpdateClassColor(const FString& ClassName, const FColor& NewClassColor);

	/** Remove a semantic class */
	bool RemoveSemanticClass(const FString& ClassName);

	/** Remove all semantic classes except for the default one */
	void RemoveAllSemanticClasses();
	
	/** Returns names of existing semantic classes */
	TArray<FString> SemanticClassNames() const;
	
	/** Returns array of const pointers to semantic classes */
	TArray<const FShadingSemantic*> GetSemanticClasses() const;
	
	/** Delegate type used to broadcast the semantic classes updated event */	
	DECLARE_EVENT(UTextureStyleManager, FSemanticClassesUpdatedEvent);
	/** Returns a reference to the event for others to bind */	
	FSemanticClassesUpdatedEvent& OnSemanticClassesUpdated() { return SemanticClassesUpdatedEvent; }
	
	TMap<FString, FShadingSemantic> SemanticClasses;
	
	/** Semantic classes updated event dispatcher */
	FSemanticClassesUpdatedEvent SemanticClassesUpdatedEvent;
};