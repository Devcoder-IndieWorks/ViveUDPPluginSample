// Copyright ViveStudios. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ViveUDPReceiveFilterType.h"
#include "ViveUDPSocketBPAPI.generated.h"

DECLARE_DYNAMIC_DELEGATE_ThreeParams( FViveUDPConnectionDelegate, bool, Success, FString, Message, FString, ConnectionID );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams( FViveUDPConnectionDelegateGroup, bool, Success, FString, Message, FString, ConnectionID );
DECLARE_DYNAMIC_DELEGATE( FViveUDPDisconnectionDelegate );
DECLARE_DYNAMIC_MULTICAST_DELEGATE( FViveUDPDisconnectionDelegateGroup );
DECLARE_DYNAMIC_DELEGATE_ThreeParams( FViveUDPReceiveDelegate, FString, Message, const TArray<uint8>&, ByteArray, FString, ConnectionID );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams( FViveUDPReceiveDelegateGroup, FString, Message, const TArray<uint8>&, ByteArray, FString, ConnectionID );
DECLARE_DYNAMIC_DELEGATE_FiveParams( FViveUDPReceiveFromDelegate, FString, Message, const TArray<uint8>&, ByteArray, FString, IpFromSender, 
    int32, PortFromSender, FString, ConnectionID );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams( FViveUDPReceiveFromDelegateGroup, FString, Message, const TArray<uint8>&, ByteArray, FString, IpFromSender, 
    int32, PortFromSender, FString, ConnectionID );

//-----------------------------------------------------------------------------

UINTERFACE(meta=(CannotImplementInterfaceInBlueprint))
class VIVEUDPPLUGIN_API UViveUDPSocketBPAPI : public UInterface
{
    GENERATED_BODY()
};

//-----------------------------------------------------------------------------

class VIVEUDPPLUGIN_API IViveUDPSocketBPAPI
{
    GENERATED_BODY()
public:
    UFUNCTION( BlueprintCallable, Category="ViveUDPSocket" )
    virtual void AddOnUDPConnectionDelegate( const FViveUDPConnectionDelegate& InDelegate ) {}
    UFUNCTION( BlueprintCallable, Category="ViveUDPSocket" )
    virtual void RemoveOnUDPConnectionDelegate( const FViveUDPConnectionDelegate& InDelegate ) {}

    UFUNCTION( BlueprintCallable, Category="ViveUDPSocket" )
    virtual void AddOnUDPDisconnectionDelegate( const FViveUDPDisconnectionDelegate& InDelegate ) {}
    UFUNCTION( BlueprintCallable, Category="ViveUDPSocket" )
    virtual void RemoveOnUDPDisconnectionDelegate( const FViveUDPDisconnectionDelegate& InDelegate ) {}

    UFUNCTION( BlueprintCallable, Category="ViveUDPSocket" )
    virtual void AddOnUDPReceiveDelegate( const FViveUDPReceiveDelegate& InDelegate ) {}
    UFUNCTION( BlueprintCallable, Category="ViveUDPSocket" )
    virtual void RemoveOnUDPReceiveDelegate( const FViveUDPReceiveDelegate& InDelegate ) {}

    UFUNCTION( BlueprintCallable, Category="ViveUDPSocket" )
    virtual void AddOnUDPReceiveFromDelegate( const FViveUDPReceiveFromDelegate& InDelegate ) {}
    UFUNCTION( BlueprintCallable, Category="ViveUDPSocket" )
    virtual void RemoveOnUDPReceiveFromDelegate( const FViveUDPReceiveFromDelegate& InDelegate ) {}

    //-------------------------------------------------------------------------

    UFUNCTION( BlueprintCallable, Category="ViveUDPSocket" )
    virtual void InitUDPSocket( FString& OutConnectionID, const FString& InIP, int32 InPort, int32 InMaxPacketSize = 65507, 
        int32 InWaitTimeMS = 100, bool InUseSender = true, bool InUseReceiveFrom = false, bool InUseBroadcast = false,
        EViveUDPReceiveFilterType InReceiveFilter = EViveUDPReceiveFilterType::E_SAB ) {}

    UFUNCTION( BlueprintCallable, Category="ViveUDPSocket" )
    virtual void CloseUDPSocket( FString InConnectionID ) {}

    UFUNCTION( BlueprintCallable, Category="ViveUDPSocket" )
    virtual void SendTo( FString InIP, int32 InPort, FString InMessage, TArray<uint8> InByteArray, 
        bool InAddLineBreak = true, FString InConnectionID = TEXT( "" ) ) {}
};
