#include "ViveUDPSender.h"
#include "ViveUDPSocketObject.h"
#include "ViveLog.h"

FViveUDPSender::FViveUDPSender( UViveUDPSocketObject* InSocketObject )
    : Stopping( false )
    , Paused( false )
{
    SocketObject = InSocketObject;
    SendToPort = 0;

    auto threadName = FString::Printf( TEXT( "ViveUDPSenderThread_%s" ), *(FGuid::NewGuid().ToString()) );
    Thread = MakeShareable( FRunnableThread::Create( this, *threadName, 128 * 1024, EThreadPriority::TPri_AboveNormal, FPlatformAffinity::GetPoolThreadMask() ) );
}

FViveUDPSender::~FViveUDPSender()
{
    Stop();

    if ( Thread.IsValid() ) {
        Thread->Kill( true );
        Thread = nullptr;
    }
}

void FViveUDPSender::Stop()
{
    Stopping = true;
    PauseThread( false );

    if ( Thread.IsValid() )
        Thread->WaitForCompletion();
}

uint32 FViveUDPSender::Run()
{
    if ( !SocketObject->IsRunning() ) {
        VIVELOG( Error, TEXT( "#### UDP socket is not running. ####" ) );
        return 0;
    }

    while( !Stopping ) {
        if ( SocketObject->IsRunning() && (!MessageQueue.IsEmpty() || !ByteArrayQueue.IsEmpty()) ) {
            auto peerAddress = SocketObject->GetSocketSubsystem()->CreateInternetAddr();
            bool isValid = false;
            peerAddress->SetIp( *SendToIP, isValid );
            peerAddress->SetPort( SendToPort );
            CVIVELOG( !isValid, Error, TEXT( "#### Can't sent to [%s:%i]. ####" ), *SendToIP, SendToPort );

            if ( isValid ) {
                int32 bytesSent = 0;
                while ( !MessageQueue.IsEmpty() ) {
                    FString msg;
                    MessageQueue.Dequeue( msg );
                    FTCHARToUTF8 data( *msg );
                    TArray<uint8> byteArray;
                    byteArray.Append( (uint8*)data.Get(), data.Length() );
                    SendBytes( peerAddress, byteArray, bytesSent );
                }
                
                while ( !ByteArrayQueue.IsEmpty() ) {
                    TArray<uint8> byteArray;
                    ByteArrayQueue.Dequeue( byteArray );
                    SendBytes( peerAddress, byteArray, bytesSent );
                }
                VIVELOG( Log, TEXT( "#### Bytes sent: [%i] ####" ), bytesSent );
            }
        }

        if ( SocketObject->IsRunning() ) {
            PauseThread( true );
            while ( Paused && SocketObject->IsRunning() )
                FPlatformProcess::Sleep( 0.01 );
        }
    }

    return 0;
}

void FViveUDPSender::SendBytes(  TSharedRef<FInternetAddr>& InAddress, const TArray<uint8>& InByteArray, int32& OutSent )
{
    OutSent = 0;

    if ( InByteArray.Num() > SocketObject->GetMaxPacketSize() ) {
        TArray<uint8> byteArrayTemp;

        for ( int32 i = 0; i < InByteArray.Num(); i++ ) {
            byteArrayTemp.Add( InByteArray[i] );
            if ( byteArrayTemp.Num() == SocketObject->GetMaxPacketSize() ) {
                OutSent += SocketObject->SendTo( InAddress, byteArrayTemp );
                byteArrayTemp.Empty();
            }
        }

        if ( byteArrayTemp.Num() > 0 )
            OutSent += SocketObject->SendTo( InAddress, byteArrayTemp );
    }
    else {
        OutSent = SocketObject->SendTo( InAddress, InByteArray );
    }
}

void FViveUDPSender::SendMessage( const FString& InIP, int32 InPort, const FString& InMessage, const TArray<uint8>& InByteArray )
{
    SendToIP = InIP;
    SendToPort = InPort;

    if ( InMessage.Len() > 0 )
        MessageQueue.Enqueue( InMessage );

    if ( InByteArray.Num() > 0 )
        ByteArrayQueue.Enqueue( InByteArray );

    PauseThread( false );
}

void FViveUDPSender::PauseThread( bool InPause )
{
    if ( Paused != InPause ) {
        Paused = InPause;
        if ( Thread.IsValid() )
            Thread->Suspend( InPause );
    }
}
