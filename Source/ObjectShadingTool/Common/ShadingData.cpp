#include "ShadingData.h"
#include "ShadingCommon.h"

/*
 * UObjectShadingSet
 */

void UObjectShadingSet::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FlushObjectProperties();
}

void UObjectShadingSet::FlushObjectProperties()
{
	for(auto Iter = Semantics.CreateIterator(); Iter; ++Iter)
	{
		for(auto IterFilter = Iter->Filters.CreateIterator(); IterFilter; ++IterFilter)
		{
			if(IterFilter->FilterType == EFilterType::EFilter_Actor)
			{
				UObject* ParentObject = IterFilter->ActorFilter.ParentActor;
				if(ParentObject)
				{
					//FString str = ParentObject->GetPathName();
					//UE_LOG(LogTemp, Warning, TEXT("---Original ParentActor: %s"), *ParentObject->GetPathName());

					if(ParentObject->GetPathName().Equals(*(IterFilter->ActorFilter.ParentActorPath.ToString())))
						continue;
				}
				
				if(auto ParentActor = StaticLoadObject(UObject::StaticClass(),nullptr, *(IterFilter->ActorFilter.ParentActorPath.ToString())))
				{
					IterFilter->ActorFilter.ParentActor = ParentActor;

					UE_LOG(LogShadingTool, Verbose, TEXT("---SetParentActor: %s"), *ParentActor->GetName());
				}
			}
			else if(IterFilter->FilterType == EFilterType::EFilter_Property)
			{
				UObject* TargetObject = IterFilter->PropertyFilter.TargetActor;
				if(TargetObject)
				{
					if(TargetObject->GetPathName().Equals(*(IterFilter->PropertyFilter.TargetActorPath.ToString())))
						continue;
				}
            				
				if(auto TargetActor = StaticLoadObject(UObject::StaticClass(),nullptr, *(IterFilter->PropertyFilter.TargetActorPath.ToString())))
				{
					IterFilter->PropertyFilter.TargetActor = TargetActor;
					
					UE_LOG(LogShadingTool, Verbose, TEXT("---SetTargetActor: %s"), *TargetActor->GetName());
				}
			}
		}
	}
}