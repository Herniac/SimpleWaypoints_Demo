// Copyright 2025 Crippling Depression Ind. all rights reserved.


#include "BehaviorTree/ContainsDynamicBehavior.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Objects/WaypointFollower.h"
#include "Objects/Waypoint.h"

UContainsDynamicBehavior::UContainsDynamicBehavior()
{
    NodeName = "Contains Dynamic Behavior?";
    bNotifyDeactivation = true;
}

bool UContainsDynamicBehavior::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    if (AActor* Owner = OwnerComp.GetOwner())
    {
        if (UWaypointFollower* WPFollower = UWaypointFollower::GetWaypointFollower(Owner))
        {
            if (const AWaypoint* WP = WPFollower->GetCurrentWaypoint())
            {
                if (WP->GetDynamicBehavior())
                {
                    return true;
                }
            }
        }
    }
    return false;
}

void UContainsDynamicBehavior::OnNodeDeactivation(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type NodeResult)
{
	if (AActor* Owner = SearchData.OwnerComp.GetOwner())
	{
		if (UWaypointFollower* WPFollower = UWaypointFollower::GetWaypointFollower(Owner))
		{
			SearchData.OwnerComp.SetDynamicSubtree(WPFollower->GetInjectTag(), nullptr);
		}
	}

}

#if WITH_EDITOR
FName UContainsDynamicBehavior::GetNodeIconName() const
{
    return FName("BTEditor.Graph.BTNode.Task.RunBehavior.Icon");
}
#endif
