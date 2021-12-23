// Copyright ViveStudios. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ViveUDPSocketBPLibrary.generated.h"

UCLASS() 
class VIVEUDPPLUGIN_API UViveUDPSocketBPLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_UCLASS_BODY()
public:
    UFUNCTION( BlueprintPure, meta=(DisplayName="Vive UDPSocket Module"), Category="ViveUDPSocket" )
    static void GetAPI( TScriptInterface<class IViveUDPSocketBPAPI>& OutAPI );
};
