// Copyright 2025 Crippling Depression Ind. all rights reserved.


#include "BehaviorTree/MoveToWaypoint.h"
#include "AIController.h"
#include "Objects/WaypointFollower.h"
#include "Objects/Waypoint.h"

UMoveToWaypoint::UMoveToWaypoint()
{
	NodeName = "Move To Waypoint";
}

EBTNodeResult::Type UMoveToWaypoint::PerformMoveTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::PerformMoveTask(OwnerComp, NodeMemory);
	if (UWaypointFollower* WPFollower = UWaypointFollower::GetWaypointFollower(OwnerComp.GetOwner()))
	{
		if (AWaypoint* WP = Cast<AWaypoint>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BlackboardKey.SelectedKeyName)))
		{
			if (Result == EBTNodeResult::Failed)
			{
				WPFollower->IgnoreWaypoint(WP);
			}
			else if (Result == EBTNodeResult::Succeeded)
			{
				WPFollower->ReachWaypoint();
			}
		}
	}
	return Result;
}
