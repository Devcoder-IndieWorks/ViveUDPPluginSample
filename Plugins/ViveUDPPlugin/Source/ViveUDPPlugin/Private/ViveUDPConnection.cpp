#include "ViveUDPConnection.h"
#include "ViveUDPSocketObject.h"
#include "ViveLog.h"

FViveUDPConnection::FViveUDPConnection( UViveUDPSocketObject* InSocketObject )
{
    SocketObject = InSocketObject;
    Socket = nullptr;
    SocketSubsystem = nullptr;

    auto threadName = FString::Printf( TEXT( "ViveUDPConnectionThread_%s" ), *(FGuid::NewGuid().ToString()) );
    Thread = MakeShareable( FRunnableThread::Create( this, *threadName, 128 * 1024, EThreadPriority::TPri_AboveNormal, FPlatformAffinity::GetPoolThreadMask() ) );
}

FViveUDPConnection::~FViveUDPConnection()
{
    if ( Thread.IsValid() ) {
        Thread->Kill( true );
        Thread = nullptr;
    }

    if ( Socket != nullptr ) {
        Socket->Close();
        SocketSubsystem->DestroySocket( Socket );
        Socket = nullptr;
        SocketSubsystem = nullptr;
    }
}

uint32 FViveUDPConnection::Run()
{
    if ( Socket == nullptr ) {
        SocketSubsystem = ISocketSubsystem::Get( PLATFORM_SOCKETSUBSYSTEM );

        if ( ensure( SocketSubsystem != nullptr ) ) {
            FIPv4Endpoint deviceEndpoint;
            FIPv4Address::Parse( SocketObject->GetIP(), deviceEndpoint.Address );
            deviceEndpoint.Port = (uint16)SocketObject->GetPort();

            if ( SocketObject->GetUseBroadcast() ) {
                Socket = FUdpSocketBuilder( TEXT( "ViveUDPSocketWithBroadcast" ) )
                    .AsNonBlocking()
                    .AsReusable()
                    .WithBroadcast()
                    .BoundToEndpoint( deviceEndpoint )
                    .WithReceiveBufferSize( SocketObject->GetMaxPacketSize() );
            }
            else {
                Socket = FUdpSocketBuilder( TEXT( "ViveUDPSocket" ) )
                    .AsNonBlocking()
                    .AsReusable()
                    .BoundToEndpoint( deviceEndpoint )
                    .WithReceiveBufferSize( SocketObject->GetMaxPacketSize() );
            }

            if ( (Socket != nullptr) && (Socket->GetSocketType() == SOCKTYPE_Datagram) ) {
                VIVELOG( Log, TEXT( "#### Opened UDP socket with IP address: [%s] ####" ), *(deviceEndpoint.ToString()) );

                SocketObject->SetSocket( Socket );
                SocketObject->SetSocketSubsystem( SocketSubsystem );
                SocketObject->SetRunning( true );
                SocketObject->StartReceiver();
                SocketObject->StartSender();

                TWeakObjectPtr<UViveUDPSocketObject> socket = SocketObject;
                AsyncTask( ENamedThreads::GameThread, [socket]{
                    if ( socket.IsValid() )
                        socket->ConnectionCallback( true, FString::Printf( TEXT( "Init UDP connection OK. IP: [%s]." ), *(socket->GetIP()) ), 
                            socket->GetConnectionID() );
                } );
            }
            else {
                VIVELOG( Error, TEXT( "#### Failed to open UDP socket with IP address: [%s] ####" ), *(deviceEndpoint.ToString()) );
            }
        }
    }

    return 0;
}
