// Copyright 2025 Crippling Depression Ind. all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "Conditions/BaseCondition.h"
#include "BTDecorator_CheckConditions.generated.h"

/**
 *	Allows execution flow when provided conditions are met for the possesed Pawn.
 */

UCLASS()
class EXTRALOGIC_API UBTDecorator_CheckConditions : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_CheckConditions();
	
protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual FString GetStaticDescription() const override;

	UPROPERTY(EditAnywhere, Category = "CheckConditions")
	EConditionMatchType MatchType;
	UPROPERTY(EditAnywhere, Category = "CheckConditions", Instanced)
	TArray<UBaseCondition*> Conditions;
};
