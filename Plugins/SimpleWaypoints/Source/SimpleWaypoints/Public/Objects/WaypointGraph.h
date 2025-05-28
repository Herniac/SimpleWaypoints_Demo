// Copyright 2025 Crippling Depression Ind. all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaypointGraph.generated.h"

/**
*	Waypoint graph, as an attach parent for waypoints, defines the scope
*	of path selection available for the user.
* 
*	@see AWaypoint
*	@see UWaypointFollower
*/

class AWaypoint;
class UTextRenderComponent;
class UBillboardComponent;

UCLASS(HideCategories = (Materials, Rendering, Lighting, Physics, Collision, LOD))
class SIMPLEWAYPOINTS_API AWaypointGraph : public AActor
{
	GENERATED_BODY()
	
public:	
	AWaypointGraph();

	//~====================================================================
	// PUBLIC FUNCTIONS

	// Getters

	UFUNCTION(BlueprintPure, Category = "WaypointGraph")
	int32 GetWaypointCount() const { return Waypoints.Num(); }
	UFUNCTION(BlueprintPure, Category = "WaypointGraph")
	void GetWaypoints(TArray<AWaypoint*>& OutWaypoints) const { OutWaypoints = Waypoints; }
	UFUNCTION(BlueprintPure, Category = "WaypointGraph|PointSelection")
	AWaypoint* GetFirstPoint() const { return Waypoints[0]; }
	UFUNCTION(BlueprintPure, Category = "WaypointGraph|PointSelection")
	AWaypoint* GetRandomPoint() const;
	UFUNCTION(BlueprintPure, Category = "WaypointGraph|PointSelection")
	AWaypoint* GetFarthestPoint(const FVector& ToLocation) const;
	UFUNCTION(BlueprintPure, Category = "WaypointGraph|PointSelection")
	AWaypoint* GetNearestPoint(const FVector& ToLocation) const;

	// Waypoint management

	void AddWaypoint(AWaypoint* NewWaypoint);
	void RemoveWaypoint(AWaypoint* Waypoint);

protected:

	//~====================================================================
	// PROTECTED OVERRIDES

	/** Disables GraphComponent's tick */
	virtual void BeginPlay() override;
	/** Clears invalid Waypoints array entries */
	virtual void PostLoad() override;
#if WITH_EDITOR
	/** Tracks actor name changes for debug text */
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	//~====================================================================
	// PROTECTED FUNCTIONS

	/** Adds a new waypoint of class specified in DefaultWaypointClass */
	UFUNCTION(BlueprintCallable, Category = "WaypointGraph", CallInEditor)
	void CreateWaypoint();

public:

	//~====================================================================
	// PUBLIC PROPERTIES

	/**	Variables */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WaypointGraph")
	TArray<AWaypoint*> Waypoints;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WaypointGraph")
	TArray<APawn*> GraphUsers;

	/** Components */

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	UWaypointGraphComponent* GraphComponent;
#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	UTextRenderComponent* TextComponent;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	UBillboardComponent* BillboardComponent;
#endif

protected:

	//~====================================================================
	// PROTECTED PROPERTIES

	/** Class for CreateWaypoint() */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WaypointGraph")
	TSubclassOf<AWaypoint> DefaultWaypointClass;
};

/**
*	This Component's main purposes are to:
*	- Notify the owner (AWaypointGraph) about Attach/Detach events, 
*	- Draw debug arrows to visualize destinations
*/

UCLASS()
class SIMPLEWAYPOINTS_API UWaypointGraphComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UWaypointGraphComponent();

protected:
	virtual void OnChildAttached(USceneComponent* ChildComponent) override;
	virtual void OnChildDetached(USceneComponent* ChildComponent) override;
	virtual void OnRegister() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#if WITH_EDITOR
	void DrawArrows();
#endif // with editor

	TWeakObjectPtr<AWaypointGraph> OwnerGraph;
};