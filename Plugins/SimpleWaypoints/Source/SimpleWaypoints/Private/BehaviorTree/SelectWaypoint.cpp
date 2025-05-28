// Copyright 2025 Crippling Depression Ind. all rights reserved.


#include "BehaviorTree/SelectWaypoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Objects/WaypointFollower.h"
#include "Objects/Waypoint.h"

USelectWaypoint::USelectWaypoint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "Select Waypoint";
	bNotifyOnSearch = false;
	bNotifyTick = false;
	bNotifyCeaseRelevant = false;
	bNotifyBecomeRelevant = true;

	Waypoint.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(USelectWaypoint, Waypoint), AActor::StaticClass());
}

void USelectWaypoint::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		Waypoint.ResolveSelectedKey(*BBAsset);
	}
}

void USelectWaypoint::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	if (OwnerComp.GetOwner())
	{
		if (UWaypointFollower* WPFollower = UWaypointFollower::GetWaypointFollower(OwnerComp.GetOwner()))
		{
			if (UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent())
			{
				BBComp->SetValueAsObject(Waypoint.SelectedKeyName, WPFollower->SelectWaypoint());
			}
		}
	}
}

FString USelectWaypoint::GetStaticDescription() const
{
	return "Selects Waypoint on activation";
}

#if WITH_EDITOR
FName USelectWaypoint::GetNodeIconName() const
{
	return FName("BTEditor.Graph.BTNode.Composite.Selector.Icon");
}
#endif
