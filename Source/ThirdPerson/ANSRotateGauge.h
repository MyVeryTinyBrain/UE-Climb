// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotifyStateEx.h"
#include "ANSRotateGauge.generated.h"

USTRUCT(BlueprintType)
struct FRotateGaugeData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Slot = 0;
};

/**
 * 
 */
UCLASS()
class THIRDPERSON_API UANSRotateGauge : public UAnimNotifyStateEx
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	FRotateGaugeData data;

public:
	const FRotateGaugeData& GetData() const { return data; }
};
