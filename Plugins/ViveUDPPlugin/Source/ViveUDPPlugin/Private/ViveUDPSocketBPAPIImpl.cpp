#include "ViveUDPSocketBPAPIImpl.h"
#include "ViveUDPSocketObject.h"
#include "ViveLog.h"

UViveUDPSocketBPAPIImpl::UViveUDPSocketBPAPIImpl( const FObjectInitializer& ObjectInitializer )
    : Super( ObjectInitializer )
{
}

void UViveUDPSocketBPAPIImpl::AddOnUDPConnectionDelegate( const FViveUDPConnectionDelegate& InDelegate )
{
    OnUDPConnectionDelegateGroup.Add( InDelegate );
}

void UViveUDPSocketBPAPIImpl::RemoveOnUDPConnectionDelegate( const FViveUDPConnectionDelegate& InDelegate )
{
    OnUDPConnectionDelegateGroup.Remove( InDelegate );
}

void UViveUDPSocketBPAPIImpl::AddOnUDPDisconnectionDelegate( const FViveUDPDisconnectionDelegate& InDelegate )
{
    OnUDPDisconnectionDelegateGroup.Add( InDelegate );
}

void UViveUDPSocketBPAPIImpl::RemoveOnUDPDisconnectionDelegate( const FViveUDPDisconnectionDelegate& InDelegate )
{
    OnUDPDisconnectionDelegateGroup.Remove( InDelegate );
}

void UViveUDPSocketBPAPIImpl::AddOnUDPReceiveDelegate( const FViveUDPReceiveDelegate& InDelegate )
{
    OnUDPReceiveDelegateGroup.Add( InDelegate );
}

void UViveUDPSocketBPAPIImpl::RemoveOnUDPReceiveDelegate( const FViveUDPReceiveDelegate& InDelegate )
{
    OnUDPConnectionDelegateGroup.Remove( InDelegate );
}

void UViveUDPSocketBPAPIImpl::AddOnUDPReceiveFromDelegate( const FViveUDPReceiveFromDelegate& InDelegate )
{
    OnUDPReceiveFromDelegateGroup.Add( InDelegate );
}

void UViveUDPSocketBPAPIImpl::RemoveOnUDPReceiveFromDelegate( const FViveUDPReceiveFromDelegate& InDelegate )
{
    OnUDPReceiveFromDelegateGroup.Remove( InDelegate );
}

void UViveUDPSocketBPAPIImpl::InitUDPSocket( FString& OutConnectionID, const FString& InIP, int32 InPort, int32 InMaxPacketSize, 
    int32 InWaitTimeMS, bool InUseSender, bool InUseReceiveFrom, bool InUseBrodcast, EViveUDPReceiveFilterType InReceiveFilter )
{
    auto key = InIP + FString::FromInt( InPort );
    OutConnectionID = key;

    auto findObj = UDPSocketObjects.Find( key );
    if ( findObj != nullptr ) {
        auto socketObj = *findObj;
        socketObj->ConnectionCallback( true, FString::Printf( TEXT( "Connection already present: IP[%s], Port[%d]" ), *InIP, InPort ), OutConnectionID );
    }
    else if ( ensure( OnUDPConnectionDelegateGroup.IsBound() ) && 
         ensure( OnUDPDisconnectionDelegateGroup.IsBound() )  && 
         ensure( OnUDPReceiveDelegateGroup.IsBound() || OnUDPReceiveFromDelegateGroup.IsBound() ) ) {

        auto socketObj = NewObject<UViveUDPSocketObject>( this, NAME_None );
        socketObj->SetConnectionCallback( [this]( const bool success, FString msg, FString id ){
            if ( OnUDPConnectionDelegateGroup.IsBound() )
                OnUDPConnectionDelegateGroup.Broadcast( success, msg, id );
        } );
        socketObj->SetDisconnectionCallback( [this]{
            if ( OnUDPDisconnectionDelegateGroup.IsBound() )
                OnUDPDisconnectionDelegateGroup.Broadcast();
        } );
        socketObj->SetReceiveCallback( [this]( const FString& msg, const TArray<uint8>& byteArr, const FString& id ){
            if ( OnUDPReceiveDelegateGroup.IsBound() )
                OnUDPReceiveDelegateGroup.Broadcast( msg, byteArr, id );
        } );
        socketObj->SetReceiveFromCallback( [this]( const FString& msg, const TArray<uint8>& byteArr, const FString& ip, int32 port, const FString& id ){
            if ( OnUDPReceiveFromDelegateGroup.IsBound() )
                OnUDPReceiveFromDelegateGroup.Broadcast( msg, byteArr, ip, port, id );
        } );

        socketObj->Setup( InIP, InPort, OutConnectionID, InMaxPacketSize, InWaitTimeMS, InUseSender, InUseReceiveFrom, InUseBrodcast, 
            InReceiveFilter );
        socketObj->Connection();

        UDPSocketObjects.Add( key, socketObj );
    }
}

void UViveUDPSocketBPAPIImpl::CloseUDPSocket( FString InConnectionID )
{
    CVIVELOG( InConnectionID.IsEmpty(), Warning, TEXT( "#### Connection ID is empty. ####" ) );

    if ( !InConnectionID.IsEmpty() ) {
        auto findObj = UDPSocketObjects.Find( InConnectionID );
        CVIVELOG( (findObj == nullptr), Warning, TEXT( "#### Connection not found. [%s] ####" ), *InConnectionID );

        if ( findObj != nullptr ) {
            auto socketObj = *findObj;
            socketObj->Disconnection();

            UDPSocketObjects.Remove( InConnectionID );
        }
    }
}

void UViveUDPSocketBPAPIImpl::SendTo( FString InIP, int32 InPort, FString InMessage, TArray<uint8> InByteArray, bool InAddLineBreak, 
    FString InConnectionID )
{
    CVIVELOG( InConnectionID.IsEmpty(), Error, TEXT( "#### Connection not found. ####" ) );

    if ( !InConnectionID.IsEmpty() ) {
        if ( (InMessage.Len() > 0) && InAddLineBreak )
            InMessage.Append( TEXT( "\r\n" ) );

        auto findObj = UDPSocketObjects.Find( InConnectionID );
        if ( ensure( findObj != nullptr ) ) {
            auto socketObject = *findObj;
            socketObject->SendTo( InIP, InPort, InMessage, InByteArray );
        }
    }
}
