// Copyright 2025 Crippling Depression Ind. all rights reserved.


#include "Objects/WaypointFollower.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "Objects/WaypointGraph.h"
#include "Objects/Waypoint.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BBValueProvider/BBValueProvider_Base.h"

DEFINE_LOG_CATEGORY(LogWaypointFollower);

UWaypointFollower::UWaypointFollower(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = bTickInEditor = false;
	bLoopPath = true;
}

void UWaypointFollower::BeginPlay()
{
	Super::BeginPlay();

	VisitedWaypoints.Reserve(HistoryLimit);

	// DEMO
	if (BTOverride)
	{
		if (AAIController* AI = GetOwnerController())
		{
			AI->RunBehaviorTree(BTOverride);
		}
	}

#if !UE_BUILD_SHIPPING
	// We want to keep tick enabled for debug purposes
	if (bEnableDebug)
	{
		SetComponentTickEnabled(true);
	}
#endif
}

void UWaypointFollower::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TickCooldowns(DeltaTime);

#if !UE_BUILD_SHIPPING
	if (bEnableDebug)
	{
		if (CurrentWaypoint)
		{
			DrawDebugDirectionalArrow(GetWorld(), OwnerController.Get()->GetPawn()->GetActorLocation(), CurrentWaypoint->GetActorLocation(), 10.f, FColor::Green, false, -1.f, ESceneDepthPriorityGroup::SDPG_World, 1.f);
		}
	}
#endif
}

UWaypointFollower* UWaypointFollower::GetWaypointFollower(AActor* Owner)
{
	if (AController* Controller = Cast<AController>(Owner))
	{
		if (UWaypointFollower* WPFollower = Controller->GetComponentByClass<UWaypointFollower>())
		{
			return WPFollower;
		}
		else
		{
			if (APawn* Pawn = Controller->GetPawn())
			{
				return Pawn->GetComponentByClass<UWaypointFollower>();
			}
		}
	}

	return Owner->GetComponentByClass<UWaypointFollower>();
}

void UWaypointFollower::ReachWaypoint()
{
	AddToHistory(CurrentWaypoint);

	if (AAIController* AIC = OwnerController.Get())
	{
		if (UBehaviorTreeComponent* BTComp = AIC->GetComponentByClass<UBehaviorTreeComponent>())
		{
			if (CurrentWaypoint->GetDynamicBehavior())
			{
				BTComp->SetDynamicSubtree(DynamicBehaviorTag, CurrentWaypoint->GetDynamicBehavior());
				if (!CurrentWaypoint->GetBehaviorParams().IsEmpty())
				{
					SetWaypointBehaviorParameters(CurrentWaypoint);
				}
			}
		}
	}
}

AWaypoint* UWaypointFollower::SelectWaypoint()
{
	if (!WaypointGraph)
	{
#if !UE_BUILD_SHIPPING
		DebugLog("No action graph");
#endif
		return nullptr;
	}

	if (!CurrentWaypoint)
	{
		SetCurrentWaypoint(WaypointGraph->GetNearestPoint(GetOwner()->GetActorLocation()));
#if !UE_BUILD_SHIPPING
		DebugLogWaypoint(CurrentWaypoint, "Picked nearest waypoint");
#endif
		return CurrentWaypoint;
	}

	if (!IsWaypointReached(CurrentWaypoint))
	{
		return CurrentWaypoint;
	}

	TMap<AWaypoint*, uint8> Destinations = CurrentWaypoint->GetDestinationsCopy();
	FilterDestinations(Destinations);
	if (AWaypoint* SelectedWaypoint = GetRandomWaypoint(Destinations))
	{
#if !UE_BUILD_SHIPPING
		DebugLogWaypoint(SelectedWaypoint,"Picked randomly from destinations");
#endif
		SetCurrentWaypoint(SelectedWaypoint);
	}
	else if (bLoopPath)
	{
#if !UE_BUILD_SHIPPING
		DebugLog("No destination found, choosing first graph point");
#endif
		SetCurrentWaypoint(WaypointGraph->GetFirstPoint());
	}
	else
	{
#if !UE_BUILD_SHIPPING
		DebugLog("No WP available");
#endif
		SetCurrentWaypoint(nullptr);
	}

	return CurrentWaypoint;
}

void UWaypointFollower::IgnoreWaypoint(AWaypoint* Waypoint)
{
	IgnoredWaypoints.Add(Waypoint, Waypoint->GetCooldown());
	SetComponentTickEnabled(true);
}

void UWaypointFollower::SetCurrentWaypoint(AWaypoint* Waypoint)
{
	if (CurrentWaypoint)
	{
		CurrentWaypoint->ReleaseWaypoint();
	}
	CurrentWaypoint = Waypoint;
	CurrentWaypoint->OccupyWaypoint();
}

ACharacter* UWaypointFollower::GetOwnerCharacter() const
{
	if (!OwnerCharacter.IsValid())
	{
		if (!OwnerController.IsValid())
		{
			UE_LOG(LogWaypointFollower, Error, TEXT("%s: Controller not found"), *GetOwner()->GetName());
			return nullptr;
		}
		OwnerCharacter = Cast<ACharacter>(OwnerController.Get()->GetPawn());
	}
	return OwnerCharacter.Get();
}

AAIController* UWaypointFollower::GetOwnerController() const
{
	if (OwnerController.IsValid())
	{
		return OwnerController.Get();
	}
	else
	{
		if (GetOwner())
		{
			OwnerController = UAIBlueprintHelperLibrary::GetAIController(GetOwner());
			if (OwnerController.IsValid())
			{
				return OwnerController.Get();
			}
		}
	}
	UE_LOG(LogWaypointFollower, Error, TEXT("%s: Controller not found"), *GetOwner()->GetName());
	return nullptr;
}

/** Filtering */

void UWaypointFollower::FilterDestinations(TMap<AWaypoint*, uint8>& Destinations) const
{
	if (Destinations.IsEmpty())
	{
		return;
	}

	// First iteration pass
	for (auto It = Destinations.CreateIterator(); It; ++It)
	{
		AWaypoint* Wp = It.Key();

		if (IsOnCooldown(Wp))
		{
#if !UE_BUILD_SHIPPING
			DebugLogWaypoint(Wp, "On cooldown");
#endif
			It.RemoveCurrent();
			continue;
		}

		if (IsOccupied(Wp))
		{
#if !UE_BUILD_SHIPPING
			DebugLogWaypoint(Wp, "Is occupied");
#endif
			It.RemoveCurrent();
			continue;
		}

		if (!DoesMeetConditions(Wp))
		{
#if !UE_BUILD_SHIPPING
			DebugLogWaypoint(Wp, "Conditions mismatch");
#endif
			It.RemoveCurrent();
			continue;
		}
	}

	// Second iteration pass
	if (bAvoidVisited && Destinations.Num() > 1)
	{
		for (auto It = Destinations.CreateIterator(); It; ++It)
		{
			AWaypoint* Wp = It.Key();
			if (WasVisited(Wp))
			{
#if !UE_BUILD_SHIPPING
				DebugLogWaypoint(Wp, "Already visited");
#endif
				It.RemoveCurrent();
				if (Destinations.Num() <= 1)
				{
					break;
				}
			}
		}
	}
}

bool UWaypointFollower::DoesMeetConditions(AWaypoint* Waypoint) const
{
	return Waypoint->CheckConditions(GetOwner());
}

bool UWaypointFollower::IsOnCooldown(AWaypoint* Waypoint) const
{
	if (IgnoredWaypoints.Contains(Waypoint))
	{
		return true;
	}
	
	return false;
}

bool UWaypointFollower::IsOccupied(AWaypoint* Waypoint) const
{
	if (Waypoint->IsPointOccupied())
	{
		return true;
	}

	return false;
}

bool UWaypointFollower::WasVisited(AWaypoint* Waypoint) const
{
	if (VisitedWaypoints.Contains(Waypoint))
	{
		return true;
	}

	return false;
}

AWaypoint* UWaypointFollower::GetRandomWaypoint(TMap<AWaypoint*, uint8>& Destinations) const
{
	if (!Destinations.Num())
	{
		return nullptr;
	}

	int32 TotalWeight = 0;

	for (const auto& Destination : Destinations)
	{
		TotalWeight += Destination.Value;
	}

	int32 RandomWeight = FMath::RandRange(0, TotalWeight);
	TotalWeight = 0;

	for (const auto& Destination : Destinations)
	{
		TotalWeight += Destination.Value;
		if (TotalWeight >= RandomWeight)
		{
			return Destination.Key;
		}
	}

	ensure(false); // Something went wrong
	return nullptr;
}

/** History */

void UWaypointFollower::AddToHistory(AWaypoint* Waypoint)
{
	if (VisitedWaypoints.Num() == (HistoryLimit))
	{
		VisitedWaypoints.RemoveAt(0, EAllowShrinking::No);
	}
	VisitedWaypoints.Add(Waypoint);
}

/** Cooldown */

void UWaypointFollower::TickCooldowns(float DeltaTime)
{
	for (TMap<AWaypoint*, float>::TIterator It(IgnoredWaypoints); It; ++It)
	{
		It->Value -= DeltaTime;
		if (It->Value <= 0.f)
		{
			It.RemoveCurrent();
		}
	}

#if !UE_BUILD_SHIPPING
	// We want to keep tick enabled for debug purposes
	if (bEnableDebug)
	{
		return;
	}
#endif

	if (IgnoredWaypoints.IsEmpty())
	{
		SetComponentTickEnabled(false);
	}
}

bool UWaypointFollower::IsWaypointReached(AWaypoint* Waypoint) const
{
	if (VisitedWaypoints.IsEmpty())
	{
		return false;
	}
	else
	{
		if (VisitedWaypoints.Last() == CurrentWaypoint)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

void UWaypointFollower::SetWaypointBehaviorParameters(AWaypoint* Waypoint)
{
	if (OwnerController.IsValid())
	{
		if (UBlackboardComponent* BB = OwnerController.Get()->GetBlackboardComponent())
		{
			TArray<UBBValueProvider_Base*> Params = Waypoint->GetBehaviorParams();
			if (!Params.IsEmpty())
			{
				for (const auto& Param : Waypoint->GetBehaviorParams())
				{
					Param->SetBlackboardValue(BB);
				}
			}
		}
	}
}

#if !UE_BUILD_SHIPPING
void UWaypointFollower::DebugLog(FString Message) const
{
	if (bEnableDebug)
	{
		UE_LOG(LogWaypointFollower, Log, TEXT("%s: %s"), *GetOwner()->GetActorLabel(), *Message);
	}
}
void UWaypointFollower::DebugLogWaypoint(AWaypoint* Waypoint, FString Message) const
{
	if (bEnableDebug)
	{
		UE_LOG(LogWaypointFollower, Log, TEXT("%s (%s): %s"), *GetOwner()->GetActorLabel(), *Waypoint->GetActorLabel(), *Message);
	}
}
#endif