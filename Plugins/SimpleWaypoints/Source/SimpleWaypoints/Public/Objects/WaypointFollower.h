// Copyright 2025 Crippling Depression Ind. all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "WaypointFollower.generated.h"

/**
*	This component needs to be attached to either AIController 
*	or controlled ACharacter. 
*	
*	It's goal is to hold the current waypoint reference and to 
*	govern the logic for waypoint selection.
* 
*	@see AWaypoint
*	@see AWaypointGraph
*/

DECLARE_LOG_CATEGORY_EXTERN(LogWaypointFollower, Log, All);

class AWaypointGraph;
class AWaypoint;
class UBehaviorTree;
class ACharacter;
class AAIController;

UCLASS(meta = (BlueprintSpawnableComponent))
class SIMPLEWAYPOINTS_API UWaypointFollower : public UActorComponent
{
	GENERATED_UCLASS_BODY()

public:
//====================================================================
// PUBLIC OVERRIDES
//====================================================================

	/** Used to reserve memory for IgnoredWaypoints array; to setup demo BT and to set debug config */
	virtual void BeginPlay() override;
	/** Manages waypoints cooldowns and debugs */
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

//====================================================================
// PUBLIC FUNCTIONS
//====================================================================

	/**/
	UFUNCTION(BlueprintCallable, Category = "WaypointFollower")
	static UWaypointFollower* GetWaypointFollower(AActor* Owner);
	/**/
	UFUNCTION(BlueprintCallable, Category = "WaypointFollower")
	void SetWaypointGraph(AWaypointGraph* Graph) { WaypointGraph = Graph; }
	/** Adds current waypoint to the history and sets dynamic behavior if it has one */
	UFUNCTION(BlueprintCallable, Category = "WaypointFollower")
	virtual void ReachWaypoint();
	/** Tries to set a new CurrentWaypoint from available ones and returns reference to it */
	UFUNCTION(BlueprintCallable, Category = "WaypointFollower")
	virtual AWaypoint* SelectWaypoint();
	/** Makes waypoint temporarily unavailable for the owner (as a result of failed movement by default) */
	UFUNCTION(BlueprintCallable, Category = "WaypointFollower")
	virtual void IgnoreWaypoint(AWaypoint* Waypoint);

	// WP Getters 

	UFUNCTION(BlueprintPure, Category = "WaypointFollower")
	const AWaypoint* GetCurrentWaypoint() const { return CurrentWaypoint.Get(); }
	const TArray<AWaypoint*>& GetVisitedWaypoints() const { return VisitedWaypoints; }
	const FGameplayTag GetInjectTag() const { return DynamicBehaviorTag; }

//====================================================================
// PROTECTED FUNCTIONS
//====================================================================

protected:

	// Protected Setters 

	/** Sets CurrentWaypoint and additionally Calls ReleaseWaypoint() and OccupyWaypoint() */
	UFUNCTION(BlueprintCallable, Category = "WaypointFollower")
	void SetCurrentWaypoint(AWaypoint* Waypoint);

	// Getters

	ACharacter* GetOwnerCharacter() const;
	AAIController* GetOwnerController() const;

	// Filtering 

	/** Checks availability of destinations */
	void FilterDestinations(TMap<AWaypoint*, uint8>& Waypoints) const;
	/** Checks waypoint's Conditions array */
	bool DoesMeetConditions(AWaypoint* Waypoint) const;
	/** Checks whether waypoint is in the IgnoredWaypoints TMap */
	bool IsOnCooldown(AWaypoint* Waypoint) const;
	/** Checks if waypoint's max users number was reached */
	bool IsOccupied(AWaypoint* Waypoint) const;
	/** Checks whether waypoint is in the VisitedWaypoints TArray */
	bool WasVisited(AWaypoint* Waypoint) const;
	AWaypoint* GetRandomWaypoint(TMap<AWaypoint*, uint8>& Destinations) const;

	// Other

	void AddToHistory(AWaypoint* Waypoint);
	void TickCooldowns(float DeltaTime);
	bool IsWaypointReached(AWaypoint* Waypoint) const;
	void SetWaypointBehaviorParameters(AWaypoint* Waypoint);

	// Debug
#if !UE_BUILD_SHIPPING
	void DebugLog(FString Message) const;
	void DebugLogWaypoint(AWaypoint* Waypoint, FString Message) const;
#endif

//====================================================================
// PROTECTED PROPERTIES
//====================================================================

	/** Max number of stored visited waypoints */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WaypointFollower|Config")
	uint8 HistoryLimit = 5;
	/** If true and no destination is available, owner will pick the first one from WaypointGraph */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WaypointFollower|Config")
	uint8 bLoopPath : 1;
	/** If true and multiple destinations are available, owner will skip the ones in VisitedWaypoints */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WaypointFollower|Config")
	uint8 bAvoidVisited : 1;
	/** If reached waypoint contains dynamic behavior, it will be injected to owner's AIController with this tag */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WaypointFollower|Config")
	FGameplayTag DynamicBehaviorTag;

	/** DEMO
	*	these properties are added strictly for the demo. As a rule of thumb it is advised to use spawners
	*	for AI characters and I would recommend setting up both WaypointGraph and BT in there. */

	/** Source of waypoints. Exposed to setup for the purpose of demo */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WaypointFollower|Config|DEMO")
	AWaypointGraph* WaypointGraph;
	/** Behavior Tree to run in owner's AIController. Exposed to setup for the purpose of demo */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WaypointFollower|Config|DEMO")
	UBehaviorTree* BTOverride;

	/** Enables visual helpers and logging */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WaypointFollower|Debug")
	uint8 bEnableDebug : 1;

private:

//====================================================================
// PRIVATE PROPERTIES
//====================================================================

	UPROPERTY(VisibleInstanceOnly)
	TArray<AWaypoint*> VisitedWaypoints;
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<AWaypoint> CurrentWaypoint;
	UPROPERTY(VisibleInstanceOnly)
	TMap<AWaypoint*, float> IgnoredWaypoints;

	mutable TWeakObjectPtr<AAIController> OwnerController;
	mutable TWeakObjectPtr<ACharacter> OwnerCharacter;
};
