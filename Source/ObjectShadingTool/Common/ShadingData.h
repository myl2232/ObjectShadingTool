#pragma once
#include "CoreMinimal.h"
#include "Materials/MaterialInstanceConstant.h"
#include "ShadingData.generated.h"

/** Enum representing mesh texture styles */
UENUM()
enum class ETextureStyle : uint8
{
	COLOR = 0 UMETA(DisplayName = "COLOR"),
	SEMANTIC = 1 UMETA(DisplayName = "SEMANTIC"),
};

UENUM()
enum class EFilterType : uint8
{	
	EFilter_String,
	EFilter_Property,
	EFilter_Actor,
	EFilter_Level,
};

UCLASS(Blueprintable)
class UPropertySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category= "Semantic Class")
	TMap<FString,FString> PropertySet;
};

USTRUCT(BlueprintType)
struct FShadingFilter
{
	GENERATED_BODY()

	//UPROPERTY(EditAnywhere, Category= "Semantic Class")
	EFilterType FilterType = EFilterType::EFilter_String;
};

USTRUCT(BlueprintType)
struct FStringShadingFilter : public FShadingFilter
{
	GENERATED_BODY()

	FStringShadingFilter()
	{
		FilterType = EFilterType::EFilter_String;
	}
	
	UPROPERTY(EditAnywhere, Category= "Semantic Class")
	FString FolderPath;
};

USTRUCT(BlueprintType)
struct FPropertyShadingFilter : public FShadingFilter
{
	GENERATED_BODY()
	FPropertyShadingFilter()
	{
		FilterType = EFilterType::EFilter_Property;
	}

	UPROPERTY(EditAnywhere, Category= "Semantic Class")
	FSoftObjectPath TargetActorPath;
	UPROPERTY()
	UObject* TargetActor;
	UPROPERTY(EditAnywhere, Category= "Semantic Class")
	FString PropertyName;
	UPROPERTY(EditAnywhere, Category= "Semantic Class")
	FString PropertyValueText;
};

USTRUCT(BlueprintType)
struct FActorShadingFilter : public FShadingFilter
{
	GENERATED_BODY()
public:
	FActorShadingFilter()
	{
		FilterType = EFilterType::EFilter_Actor;
	}
	
	UPROPERTY(EditAnywhere, Category= "Semantic Class")
	FSoftObjectPath ParentActorPath;

	UPROPERTY()
	UObject* ParentActor;
};

USTRUCT(BlueprintType)
struct FShadingSemanticItem
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category= "Semantic Class")
	EFilterType FilterType;
	UPROPERTY(EditAnywhere, Category= "Semantic Class", meta=(EditCondition="FilterType == EFilterType::EFilter_String", EditConditionHides))
	FStringShadingFilter FolderFilter;
	UPROPERTY(EditAnywhere, Category= "Semantic Class", meta=(EditCondition="FilterType == EFilterType::EFilter_Property", EditConditionHides))
	FPropertyShadingFilter PropertyFilter;
	UPROPERTY(EditAnywhere, Category= "Semantic Class", meta=(EditCondition="FilterType == EFilterType::EFilter_Actor", EditConditionHides))
	FActorShadingFilter ActorFilter;
	UPROPERTY(EditAnywhere, Category= "Semantic Class", meta=(EditCondition="FilterType == EFilterType::EFilter_Level", EditConditionHides))
	FString LevelFilter;
};


USTRUCT(BlueprintType)
struct FShadingSemantic
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category= "Semantic Class")
	FString Name;
	UPROPERTY(EditAnywhere, Category= "Semantic Class")
	FColor Color;
	/** Reference to the plain color material instance */
	UPROPERTY(EditAnywhere, Category = "Semantic Class", meta=(EditConditionHides))
	UMaterialInstanceConstant* PlainColorMaterialInstance;
	UPROPERTY(EditAnywhere, Category= "Semantic Class")
	TArray<FShadingSemanticItem> Filters;
};

UCLASS(Blueprintable)
class UObjectShadingSet : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	void FlushObjectProperties();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ShadingTools")
	TArray<FShadingSemantic> Semantics;
};
