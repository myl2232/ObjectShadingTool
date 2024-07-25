#pragma once
#include "CoreMinimal.h"
#include "Materials/MaterialInstanceConstant.h"

/** Enum representing mesh texture styles */
UENUM()
enum class ETextureStyle : uint8
{
	COLOR = 0 UMETA(DisplayName = "COLOR"),
	SEMANTIC = 1 UMETA(DisplayName = "SEMANTIC"),
};

UENUM()
enum EFilterType : uint8
{	
	EFilter_String,
	EFilter_Object,
	EFilter_Property,	
};

USTRUCT(BlueprintType)
struct FShadingFilter
{
	GENERATED_BODY()
	
	EFilterType FilterType;
};

USTRUCT(BlueprintType)
struct FStringShadingFilter : public FShadingFilter
{
	GENERATED_BODY()
	
	FString FolderPath;
};

USTRUCT(BlueprintType)
struct FObjectShadingFilter : public FShadingFilter
{
	GENERATED_BODY()
	
	TWeakObjectPtr<UObject> ParentObject;
};

USTRUCT(BlueprintType)
struct FPropertyShadingFilter : public FShadingFilter
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FShadingSemantic
{
	UPROPERTY(EditAnywhere, Category= "Semantic Class")
	FString Name;
	UPROPERTY(EditAnywhere, Category= "Semantic Class")
	FColor Color;	
	UPROPERTY(EditAnywhere, Category= "Semantic Class")
	TArray<FShadingFilter> Filters;
	/** Reference to the plain color material instance */
	UPROPERTY(EditAnywhere, Category = "Semantic Class")
	UMaterialInstanceConstant* PlainColorMaterialInstance;
	
	
};

