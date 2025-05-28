// Copyright 2025 Crippling Depression Ind. all rights reserved.


#include "Objects/WaypointGraph.h"
#include "Objects/Waypoint.h"
#include "Components/BillboardComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/LineBatchComponent.h"

AWaypointGraph::AWaypointGraph()
{
	GraphComponent = CreateDefaultSubobject<UWaypointGraphComponent>(FName("GraphComponent"));
	RootComponent = GraphComponent;
	DefaultWaypointClass = AWaypoint::StaticClass();

#if WITH_EDITORONLY_DATA
	// Setup billboard
	BillboardComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>("Billboard");
	if (BillboardComponent)
	{
		BillboardComponent->SetSprite(LoadObject<UTexture2D>(NULL, TEXT("/Engine/EditorResources/S_VectorFieldVol.S_VectorFieldVol")));
		BillboardComponent->SetEditorScale(0.75f);
		BillboardComponent->SetupAttachment(RootComponent);
	}

	// Setup Title
	TextComponent = CreateEditorOnlyDefaultSubobject<UTextRenderComponent>(FName("TitleComp"));
	if (TextComponent)
	{
		TextComponent->SetText(FText::FromString(GetActorLabel()));
		TextComponent->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
		TextComponent->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
		TextComponent->SetupAttachment(RootComponent);
	}
#endif

	// Tick setup
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void AWaypointGraph::CreateWaypoint()
{
	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AWaypoint* NewWaypoint = GetWorld()->SpawnActor<AWaypoint>(DefaultWaypointClass, GetActorLocation(), FRotator(), Params);
	NewWaypoint->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false));
}

AWaypoint* AWaypointGraph::GetRandomPoint() const
{
	return Waypoints[FMath::RandRange(0, Waypoints.Num() - 1)];
}

AWaypoint* AWaypointGraph::GetFarthestPoint(const FVector& ToLocation) const
{
	TArray<AWaypoint*> SortedPoints = Waypoints;
	SortedPoints.Sort([ToLocation](AWaypoint& a, AWaypoint& b)
		{
			float DistanceA = (a.GetActorLocation() - ToLocation).SizeSquared();
			float DistanceB = (b.GetActorLocation() - ToLocation).SizeSquared();
			return DistanceA > DistanceB;
		});

	return SortedPoints[0];
}

AWaypoint* AWaypointGraph::GetNearestPoint(const FVector& ToLocation) const
{
	TArray<AWaypoint*> SortedPoints = Waypoints;
	SortedPoints.Sort([&ToLocation](AWaypoint& a, AWaypoint& b)
		{
			float DistanceA = (a.GetActorLocation() - ToLocation).SizeSquared();
			float DistanceB = (b.GetActorLocation() - ToLocation).SizeSquared();
			return DistanceA < DistanceB;
		});

	return SortedPoints[0];
}

void AWaypointGraph::AddWaypoint(AWaypoint* NewWaypoint)
{
	if (NewWaypoint)
	{
		Waypoints.AddUnique(NewWaypoint);
	}
}

void AWaypointGraph::RemoveWaypoint(AWaypoint* Waypoint)
{
	if (Waypoint)
	{
		Waypoints.Remove(Waypoint);
	}
}

void AWaypointGraph::BeginPlay()
{
	Super::BeginPlay();

	GraphComponent->SetComponentTickEnabled(false);
}

void AWaypointGraph::PostLoad()
{
	Super::PostLoad();

	Waypoints.RemoveAll([](AWaypoint* Waypoint)
	{
		return !Waypoint;
	});
}

#if WITH_EDITOR
void AWaypointGraph::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

#if WITH_EDITORONLY_DATA
	if (TextComponent)
	{
		TextComponent->SetText(FText::FromString(GetActorLabel()));
	}
#endif // With editor
}
#endif // With Editoronly data

/**	Action Graph Component */

UWaypointGraphComponent::UWaypointGraphComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	bTickInEditor = true;
}

void UWaypointGraphComponent::OnChildAttached(USceneComponent* ChildComponent)
{
	if (AWaypointGraph* Graph = OwnerGraph.Get())
	{
		if (AWaypoint* Waypoint = Cast<AWaypoint>(ChildComponent->GetOwner()))
		{
			Graph->AddWaypoint(Waypoint);
			GEditor->SelectActor(Graph, false, true);
			GEditor->SelectActor(Waypoint, true, true);
		}
	}
}

void UWaypointGraphComponent::OnChildDetached(USceneComponent* ChildComponent)
{
	if (AWaypointGraph* Graph = OwnerGraph.Get())
	{
		if (AWaypoint* Waypoint = Cast<AWaypoint>(ChildComponent->GetOwner()))
		{
			Graph->RemoveWaypoint(Waypoint);
			if (Graph)
			{
				GEditor->SelectActor(Graph, true, true);
			}
		}
	}
}

void UWaypointGraphComponent::OnRegister()
{
	Super::OnRegister();

	if (GetOwner())
	{
		OwnerGraph = Cast<AWaypointGraph>(GetOwner());
	}
}

void UWaypointGraphComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

#if WITH_EDITOR
	DrawArrows();
#endif
}

void UWaypointGraphComponent::DrawArrows()
{
	if (!GetWorld())
	{
		return;
	}
	if (AWaypointGraph* Graph = OwnerGraph.Get())
	{
		// Draw all destinations if waypoint graph is selected
		if (Graph->IsSelected())
		{
			for (const auto& Waypoint : Graph->Waypoints)
			{
				Waypoint->DrawDestinationsArrows();
			}
		}
		// Draw destinations only for selected waypoints
		else
		{
			for (const auto& Waypoint : Graph->Waypoints)
			{
				if (Waypoint->IsSelected())
				{
					Waypoint->DrawDestinationsArrows();
				}
			}
		}
		
	}
}