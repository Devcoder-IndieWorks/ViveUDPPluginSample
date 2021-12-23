#include "ViveUDPSocketObject.h"
#include "ViveUDPConnection.h"
#include "ViveUDPSender.h"
#include "ViveUDPReceiver.h"
#include "ViveLog.h"

UViveUDPSocketObject::UViveUDPSocketObject( const FObjectInitializer& ObjectInitializer )
    : Super( ObjectInitializer )
{
    Running = false;
    Port = 0;
    MaxPacketSize = 0;
    WaitTimeMS = 0;
    UseSender = false;
    UseReceiveFrom = false;
    ReceiveFilter = EViveUDPReceiveFilterType::E_SAB;
    Socket = nullptr;
    SocketSubsystem = nullptr;
}

void UViveUDPSocketObject::Setup( const FString& InIP, int32 InPort, const FString InConnectionID, int32 InMaxPacketSize, int32 InWaitTimeMS, 
    bool InUseSender, bool InUseReceiveFrom, bool InUseBroadcast, EViveUDPReceiveFilterType InReceiveFilter )
{
    IP = InIP;
    Port = InPort;
    ConnectionID = InConnectionID;
    WaitTimeMS = InWaitTimeMS;
    UseSender = InUseSender;
    UseReceiveFrom = InUseReceiveFrom;
    UseBroadcast = InUseBroadcast;
    ReceiveFilter = InReceiveFilter;

    if ( InMaxPacketSize >= 1 && InMaxPacketSize <= 65507 )
        MaxPacketSize = InMaxPacketSize;
}

void UViveUDPSocketObject::Connection()
{
    if ( !UDPConnection.IsValid() )
        UDPConnection = MakeShared<FViveUDPConnection>( this );
}

void UViveUDPSocketObject::Disconnection()
{
    if ( Running ) {
        Running = false;

        if ( UDPSender.IsValid() ) {
            UDPSender->Stop();
            UDPSender = nullptr;
        }

        if ( UDPReceiver.IsValid() ) {
            UDPReceiver->Stop();
            UDPReceiver = nullptr;
        }

        if ( UDPConnection.IsValid() )
            UDPConnection = nullptr;

        Socket = nullptr;
        SocketSubsystem = nullptr;

        TWeakObjectPtr<UViveUDPSocketObject> thisPtr = this;
        AsyncTask( ENamedThreads::GameThread, [thisPtr]{
            if ( thisPtr.IsValid() )
                thisPtr->DisconnectionCallback();
        } );
    }
}

void UViveUDPSocketObject::StartSender()
{
    if ( UseSender ) {
        if ( !UDPSender.IsValid() )
            UDPSender = MakeShared<FViveUDPSender>( this );
    }
}

void UViveUDPSocketObject::StartReceiver()
{
    if ( !UDPReceiver.IsValid() )
        UDPReceiver = MakeShared<FViveUDPReceiver>( this );
}

void UViveUDPSocketObject::SetConnectionCallback( TFunction<void( bool, const FString&, const FString& )>&& InCallback )
{
    OnConnection = MoveTemp( InCallback );
}

void UViveUDPSocketObject::SetDisconnectionCallback( TFunction<void()>&& InCallback )
{
    OnDisconnection = MoveTemp( InCallback );
}

void UViveUDPSocketObject::SetReceiveCallback( TFunction<void( const FString&, const TArray<uint8>&, const FString& )>&& InCallback )
{
    OnReceive = MoveTemp( InCallback );
}

void UViveUDPSocketObject::SetReceiveFromCallback( TFunction<void( const FString&, const TArray<uint8>&, const FString&, int32, const FString& )>&& InCallback )
{
    OnReceiveFrom = MoveTemp( InCallback );
}

void UViveUDPSocketObject::ConnectionCallback( bool InSuccess, const FString& InMessage, const FString& InConnectionID )
{
    if ( OnConnection ) {
        OnConnection( InSuccess, InMessage, InConnectionID );
    }
}

void UViveUDPSocketObject::DisconnectionCallback()
{
    if ( OnDisconnection ) {
        OnDisconnection();
    }
}

void UViveUDPSocketObject::ReceiveCallback( const FString& InMessage, const TArray<uint8>& InByteArray, const FString& InConnectionID )
{
    if ( !UseReceiveFrom && OnReceive ) {
        OnReceive( InMessage, InByteArray, InConnectionID );
    }
}

void UViveUDPSocketObject::ReceiveFromCallback( const FString& InMessage, const TArray<uint8>& InByteArray, const FString& InIP, int32 InPort, const FString& InConnectionID )
{
    if ( UseReceiveFrom && OnReceiveFrom ) {
        OnReceiveFrom( InMessage, InByteArray, InIP, InPort, InConnectionID );
    }
}

void UViveUDPSocketObject::SendTo(  const FString& InIP, int32 InPort, const FString& InMessage, const TArray<uint8>& InByteArray )
{
    if ( UseSender && UDPSender.IsValid() )
        UDPSender->SendMessage( InIP, InPort, InMessage, InByteArray );
}

int32 UViveUDPSocketObject::SendTo( TSharedRef<FInternetAddr>& InAddress, const TArray<uint8>& InByteArray )
{
    int32 bytesSent = 0;
    Socket->SendTo( InByteArray.GetData(), InByteArray.Num(), bytesSent, *InAddress );
    return bytesSent;
}

void UViveUDPSocketObject::Receive( const FArrayReaderPtr& InArrayReaderPtr, const FIPv4Endpoint& InEndPoint )
{
    FString ip;
    int32 port = 0;
    if ( UseReceiveFrom ) {
        auto peerAddress = InEndPoint.ToInternetAddr();
        ip = peerAddress->ToString( false );
        port = peerAddress->GetPort();
    }

    FString receiveMessage;
    if ( ReceiveFilter == EViveUDPReceiveFilterType::E_SAB || ReceiveFilter == EViveUDPReceiveFilterType::E_S ) {
        auto data = (char*)InArrayReaderPtr->GetData();
        data[ InArrayReaderPtr->Num() ] = '\0';
        receiveMessage = FString( UTF8_TO_TCHAR( data ) );
    }

    TArray<uint8> byteArray;
    if ( ReceiveFilter == EViveUDPReceiveFilterType::E_SAB || ReceiveFilter == EViveUDPReceiveFilterType::E_B ) {
        byteArray.Append( InArrayReaderPtr->GetData(), InArrayReaderPtr->Num() );
    }

    TWeakObjectPtr<UViveUDPSocketObject> thisPtr = this;
    AsyncTask( ENamedThreads::GameThread, [ip, port, receiveMessage, byteArray, thisPtr] {
        if ( thisPtr.IsValid() ) {
            if ( !thisPtr->IsRunning() )
                return;

            if ( thisPtr->GetUseReceiveFrom() )
                thisPtr->ReceiveFromCallback( receiveMessage, byteArray, ip, port, thisPtr->GetConnectionID() );
            else
                thisPtr->ReceiveCallback( receiveMessage, byteArray, thisPtr->GetConnectionID() );
        }
    } );
}

bool UViveUDPSocketObject::Receive( uint8* OutData, int32 InBufferSize, int32& OutBytesRead )
{
    if ( Socket != nullptr ) {
        OutBytesRead = 0;
        return Socket->Recv( OutData, InBufferSize, OutBytesRead );
    }

    return false;
}

bool UViveUDPSocketObject::ReceiveFrom( uint8* OutData, int32 InBufferSize, int32& OutBytesRead, FInternetAddr& OutSender )
{
    if ( Socket != nullptr ) {
        OutBytesRead = 0;
        return Socket->RecvFrom( OutData, InBufferSize, OutBytesRead, OutSender );
    }

    return false;
}

bool UViveUDPSocketObject::Wait( const FTimespan& InWaitTime )
{
    if ( Socket != nullptr )
        return Socket->Wait( ESocketWaitConditions::WaitForRead, InWaitTime );

    return false;
}

bool UViveUDPSocketObject::HasPendingData( uint32& OutPendingDataSize )
{
    OutPendingDataSize = 0;

    if ( Socket != nullptr )
        return Socket->HasPendingData( OutPendingDataSize );

    return false;
}

bool UViveUDPSocketObject::IsRunning() const
{
    return Running;
}

void UViveUDPSocketObject::SetRunning( bool InRunning )
{
    Running = InRunning;
}

FSocket* UViveUDPSocketObject::GetSocket() const
{
    return Socket;
}

void UViveUDPSocketObject::SetSocket( FSocket* InSocket )
{
    Socket = InSocket;
}

ISocketSubsystem* UViveUDPSocketObject::GetSocketSubsystem() const
{
    return SocketSubsystem;
}

void UViveUDPSocketObject::SetSocketSubsystem( ISocketSubsystem* InSocketSubsystem )
{
    SocketSubsystem = InSocketSubsystem;
}

FString UViveUDPSocketObject::GetIP() const
{
    return IP;
}

int32 UViveUDPSocketObject::GetPort() const
{
    return Port;
}

FString UViveUDPSocketObject::GetConnectionID() const
{
    return ConnectionID;
}

int32 UViveUDPSocketObject::GetMaxPacketSize() const
{
    return MaxPacketSize;
}

int32 UViveUDPSocketObject::GetWaitTimeMS() const
{
    return WaitTimeMS;
}

bool UViveUDPSocketObject::GetUseReceiveFrom() const
{
    return UseReceiveFrom;
}

bool UViveUDPSocketObject::GetUseBroadcast() const
{
    return UseBroadcast;
}

EViveUDPReceiveFilterType UViveUDPSocketObject::GetReceiveFilter() const
{
    return ReceiveFilter;
}
