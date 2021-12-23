// Copyright ViveStudios. All Rights Reserved.
#pragma once
#include "ViveUDPSocketBPAPI.h"
#include "ViveUDPSocketBPAPIImpl.generated.h"

UCLASS()
class UViveUDPSocketBPAPIImpl : public UObject, public IViveUDPSocketBPAPI
{
    GENERATED_UCLASS_BODY()
public:
    UFUNCTION( BlueprintCallable, Category="ViveUDPSocket" )
    void AddOnUDPConnectionDelegate( const FViveUDPConnectionDelegate& InDelegate ) override;
    UFUNCTION( BlueprintCallable, Category="ViveUDPSocket" )
    void RemoveOnUDPConnectionDelegate( const FViveUDPConnectionDelegate& InDelegate ) override;

    UFUNCTION( BlueprintCallable, Category="ViveUDPSocket" )
    void AddOnUDPDisconnectionDelegate( const FViveUDPDisconnectionDelegate& InDelegate ) override;
    UFUNCTION( BlueprintCallable, Category="ViveUDPSocket" )
    void RemoveOnUDPDisconnectionDelegate( const FViveUDPDisconnectionDelegate& InDelegate ) override;

    UFUNCTION( BlueprintCallable, Category="ViveUDPSocket" )
    void AddOnUDPReceiveDelegate( const FViveUDPReceiveDelegate& InDelegate ) override;
    UFUNCTION( BlueprintCallable, Category="ViveUDPSocket" )
    void RemoveOnUDPReceiveDelegate( const FViveUDPReceiveDelegate& InDelegate ) override;

    UFUNCTION( BlueprintCallable, Category="ViveUDPSocket" )
    void AddOnUDPReceiveFromDelegate( const FViveUDPReceiveFromDelegate& InDelegate ) override;
    UFUNCTION( BlueprintCallable, Category="ViveUDPSocket" )
    void RemoveOnUDPReceiveFromDelegate( const FViveUDPReceiveFromDelegate& InDelegate ) override;

    //-------------------------------------------------------------------------

    UFUNCTION( BlueprintCallable, Category="ViveUDPSocket" )
    void InitUDPSocket( FString& OutConnectionID, const FString& InIP, int32 InPort, int32 InMaxPacketSize = 65507, 
        int32 InWaitTimeMS = 100, bool InUseSender = true, bool InUseReceiveFrom = false, bool InUseBroadcast = false,
        EViveUDPReceiveFilterType InReceiveFilter = EViveUDPReceiveFilterType::E_SAB ) override;

    UFUNCTION( BlueprintCallable, Category="ViveUDPSocket" )
    void CloseUDPSocket( FString InConnectionID ) override;

    UFUNCTION( BlueprintCallable, Category="ViveUDPSocket" )
    void SendTo( FString InIP, int32 InPort, FString InMessage, TArray<uint8> InByteArray, 
        bool InAddLineBreak = true, FString InConnectionID = TEXT( "" ) ) override;

private:
    FViveUDPConnectionDelegateGroup OnUDPConnectionDelegateGroup;
    FViveUDPDisconnectionDelegateGroup OnUDPDisconnectionDelegateGroup;
    FViveUDPReceiveDelegateGroup OnUDPReceiveDelegateGroup;
    FViveUDPReceiveFromDelegateGroup OnUDPReceiveFromDelegateGroup;

    //-------------------------------------------------------------------------

    UPROPERTY()
    TMap<FString, class UViveUDPSocketObject*> UDPSocketObjects;
};
