// Copyright 2025 Crippling Depression Ind. all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "SelectWaypoint.generated.h"

/**
 *	This service selects new Waypoint via owner's WaypointFollowerComponent 
 *	and sets it as BB value for further use in MoveToWaypoint.
 */
UCLASS(HideCategories = (Service))
class SIMPLEWAYPOINTS_API USelectWaypoint : public UBTService
{
	GENERATED_UCLASS_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector Waypoint;

	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

#if WITH_EDITOR
	virtual FName GetNodeIconName() const override;
#endif // WITH_EDITOR

};
