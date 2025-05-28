// Copyright 2025 Crippling Depression Ind. all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "ContainsDynamicBehavior.generated.h"

/**
 * Checks whether current Waypoint contains dynamic behavior to inject on activation
 * Clears this dynamic behavior on deactivation.
 */
UCLASS()
class SIMPLEWAYPOINTS_API UContainsDynamicBehavior : public UBTDecorator
{
	GENERATED_BODY()

	UContainsDynamicBehavior();
	
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual void OnNodeDeactivation(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type NodeResult) override;

#if WITH_EDITOR
	virtual FName GetNodeIconName() const;
#endif
};
