// Copyright 2025 Crippling Depression Ind. all rights reserved.


#include "Objects/Waypoint.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/TextRenderComponent.h"
#include "Components/ArrowComponent.h"


AWaypoint::AWaypoint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetGenerateOverlapEvents(false);
	Mesh->PrimaryComponentTick.bStartWithTickEnabled = false;
#if WITH_EDITOR
	static ConstructorHelpers::FObjectFinder<UStaticMesh> FloorMarker(TEXT("/SimpleWaypoints/SM/Flag.Flag"));
	if (FloorMarker.Succeeded())
	{
		Mesh->SetStaticMesh(FloorMarker.Object);
	}
#endif
	RootComponent = Mesh;

	PrimaryActorTick.bCanEverTick = false;
	SetHidden(true);
	bIsEnabled = true;

#if WITH_EDITOR
	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->PrimaryComponentTick.bStartWithTickEnabled = false;
	Arrow->SetupAttachment(GetRootComponent());

	Text = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Text"));
	Text->bIsEditorOnly = true;
	Text->PrimaryComponentTick.bStartWithTickEnabled = false;
	Text->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	Text->SetRelativeLocation(FVector(0.f, 0.f, 150.f));
	Text->SetupAttachment(GetRootComponent());
	UpdateDebugText();
#endif
}

void AWaypoint::OccupyWaypoint()
{
	CurrentUsers++;
#if WITH_EDITOR
	UpdateDebugText();
#endif
}

void AWaypoint::ReleaseWaypoint()
{
	CurrentUsers--;
#if WITH_EDITOR
	UpdateDebugText();
#endif
}

void AWaypoint::SetPointEnabled(bool bNewEnabled)
{
	bIsEnabled = bNewEnabled;
#if WITH_EDITOR
	UpdateDebugText();
#endif
}

bool AWaypoint::CheckConditions(AActor* User)
{
	if (UseConditions.IsEmpty())
	{
		return true;
	}

	for (int i = UseConditions.Num() - 1; i >= 0; --i)
	{
		if (UseConditions[i]->CheckCondition(User))
		{
			if (MatchType == EConditionMatchType::ANY)
			{
				return true;
			}
		}
		else
		{
			if (MatchType == EConditionMatchType::ALL)
			{
				return false;
			}
		}
	}

	return MatchType == EConditionMatchType::ALL;
}

void AWaypoint::PostLoad()
{
	Super::PostLoad();
#if WITH_EDITOR
	UpdateDebugText();
#endif
}

#if WITH_EDITOR
void AWaypoint::DrawDestinationsArrows()
{
	for (const auto& Destination : Destinations)
	{
		if (Destination.Key)
		{
			DrawDebugDirectionalArrow(GetWorld(),
			GetActorLocation(),
			Destination.Key->GetActorLocation(),
			50.f,
			Destination.Key->HasConditions() ? FColor::Blue : (Destination.Key->IsPointEnabled() ? FColor::Green : FColor::Red),
			false,
			0.f,
			SDPG_Foreground,
			Destination.Value + 1.f);
		}
	}
}

void AWaypoint::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (!PropertyChangedEvent.Property)
	{
		return;
	}
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(AWaypoint, bIsEnabled) ||
		PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(AWaypoint, MaxUsers))
	{
		UpdateDebugText();
	}
}

void AWaypoint::UpdateDebugText()
{
	if (Text)
	{
		FString OutputText = "";
		if (bIsEnabled)
		{
			OutputText = "Enabled\n";
			Text->SetTextRenderColor(FColor::Green);
		}
		else
		{
			OutputText = "Disabled\n";
			Text->SetTextRenderColor(FColor::Red);
		}
		OutputText += FString::Printf(TEXT("Users: %u / %u"), CurrentUsers, MaxUsers);
		Text->SetText(FText::FromString(OutputText));
	}
}
#endif