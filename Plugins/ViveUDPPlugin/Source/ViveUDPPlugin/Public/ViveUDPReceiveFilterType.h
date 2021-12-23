// Copyright ViveStudios. All Rights Reserved.
#pragma once
#include "UObject/ObjectMacros.h"
#include "ViveUDPReceiveFilterType.generated.h"

UENUM( BlueprintType )
enum class EViveUDPReceiveFilterType : uint8
{
    E_SAB UMETA(DisplayName="Message And Bytes"),
    E_S   UMETA(DisplayName="Message"),
    E_B   UMETA(DisplayName="Bytes")
};
