// Copyright 2025 Crippling Depression Ind. all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "MoveToWaypoint.generated.h"

/**
 *  This MoveTo puts calls ReachWaypoint in case of success
 *  and puts waypoint on cooldown in case of failure.
 * 
 *	@see UWaypointFollower
 */
UCLASS()
class SIMPLEWAYPOINTS_API UMoveToWaypoint : public UBTTask_MoveTo
{
	GENERATED_BODY()

public:
	UMoveToWaypoint();
	
protected:
	virtual EBTNodeResult::Type PerformMoveTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
