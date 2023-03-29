// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EngineMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(ThirdPerson, Log, All);

#define LOG(x, ...) UE_LOG(ThirdPerson, Log, TEXT(x), __VA_ARGS__)
#define LOG_WARNING(x, ...) UE_LOG(ThirdPerson, Warning, TEXT(x), __VA_ARGS__)
#define LOG_ERROR(x, ...) UE_LOG(ThirdPerson, Error, TEXT(x), __VA_ARGS__)

#define Abstract

#define EditAnywhere
#define VisibleAnywhere
#define BlueprintReadOnly
#define BlueprintReadWrite
#define Category
#define DisplayName
#define BlueprintType

// Meta

#define AllowPrivateAccess
#define EditCondition
#define ClampMin
#define ClampMax
#define UIMin
#define UIMax