// Copyright 2025 Crippling Depression Ind. all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Conditions/BaseCondition.h"
#include "Waypoint.generated.h"

/**
*	Base class for waypoint actor
* 
*	Waypoints should be attached to a WaypointGraph in order to create
*	a path. Each of them should have at least one other waypoint set in
*	Destinations array, without that user may either stop following or
*	select first waypoint from the graph.
* 
*	@see AWaypointGraph
*	@see UWaypointFollower
*/

class UBehaviorTree;
class UBBValueProvider_Base;

UCLASS()
class SIMPLEWAYPOINTS_API AWaypoint : public AActor
{
	GENERATED_UCLASS_BODY()

public:
//~=============================================================================
// PUBLIC FUNCTIONS	

	/** Called by WaypointFollower upon selection (if is the new one) */
	void OccupyWaypoint();
	/** Called by WaypointFollower upon selection (if is the previous one) */
	void ReleaseWaypoint();

	// Getters

	/** Returns destinations */
	void GetDestinations(TMap<AWaypoint*, uint8>& OutDestinations) const { OutDestinations = Destinations; }
	/** Returns copy of destinations */
	TMap<AWaypoint*, uint8> GetDestinationsCopy() const { return Destinations; }
	/**/
	float GetCooldown() const { return Cooldown; }
	/**/
	void SetPointEnabled(bool bNewEnabled);
	/**/
	bool IsPointEnabled() const { return bIsEnabled; }
	/**/
	bool IsPointOccupied() const { return CurrentUsers >= MaxUsers; }
	/**/
	bool CheckConditions(AActor* User);
	/**/
	bool HasConditions() const { return !UseConditions.IsEmpty(); }
	/** Get behavior tree meant to be injected after reaching this waypoint */
	UBehaviorTree* GetDynamicBehavior() const { return Behavior; }
	/**/
	const TArray<UBBValueProvider_Base*>& GetBehaviorParams() const { return BehaviorParams; }

	// Debug

#if WITH_EDITOR
	/** Meant to be called in WaypointGraphComponent::TickComponent */
	void DrawDestinationsArrows();
#endif

protected:

//~=============================================================================
// PROTECTED FUNCTIONS

	/** Calls UpdateDebugText() */
	virtual void PostLoad() override;
#if WITH_EDITOR
	/** Reacts to bIsEnabled change */
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	/** Updates info about being enabled and current users count */
	void UpdateDebugText();
#endif

//~=============================================================================
// PROTECTED PROPERTIES

	// Components

	UPROPERTY(EditAnywhere, Category = "Waypoint")
	class UStaticMeshComponent* Mesh;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Waypoint")
	class UTextRenderComponent* Text;
	UPROPERTY(EditAnywhere, Category = "Waypoint")
	class UArrowComponent* Arrow;
#endif

	// Variables

	/** Possible destinations [Destination|Weight] where weight specifies chance for being selected (0 doesn't mean never) */
	UPROPERTY(EditInstanceOnly, Category = "Waypoint")
	TMap<AWaypoint*, uint8> Destinations;
	/** For how long in seconds waypoint won't be accessible for single user after failed movement. Lower/equal 0 means no cooldown */
	UPROPERTY(EditAnywhere, Category = "Waypoint")
	float Cooldown = -1.f;
	/** Only enabled waypoints will be considered in selection */
	UPROPERTY(EditAnywhere, Category = "Waypoint")
	uint8 bIsEnabled : 1;
	/** This WP can be selected only by this number of users at the time */
	UPROPERTY(EditAnywhere, Category = "Waypoint")
	uint8 MaxUsers = 1;
	/** Whether AI Controller should be injected with dynamic BT upon reaching this WP */
	UPROPERTY(EditAnywhere, Category = "Waypoint")
	bool bPerformBehavior = false;
	/** Dynamic Behavior to inject */
	UPROPERTY(EditAnywhere, Category = "Waypoint", meta = (EditCondition = "bPerformBehavior"))
	UBehaviorTree* Behavior;
	/** Parameters passed to injected Behavior */
	UPROPERTY(EditAnywhere, Category = "Waypoint", meta = (EditCondition = "bPerformBehavior"), Instanced)
	TArray<UBBValueProvider_Base*> BehaviorParams;
	/** Match type for conditions */
	UPROPERTY(EditAnywhere, Category = "Waypoint|Conditions")
	EConditionMatchType MatchType;
	/** Conditions required to select this waypoint */
	UPROPERTY(EditAnywhere, Category = "Waypoint|Conditions", Instanced, meta = (NoElementDuplicate, TitleProperty = "ConditionName"))
	TArray<UBaseCondition*> UseConditions;

private:
	uint8 CurrentUsers = 0;
};
