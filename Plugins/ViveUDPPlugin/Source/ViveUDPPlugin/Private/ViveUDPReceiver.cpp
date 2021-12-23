#include "ViveUDPReceiver.h"
#include "ViveUDPSocketObject.h"
#include "ViveLog.h"

FViveUDPReceiver::FViveUDPReceiver( UViveUDPSocketObject* InSocketObject )
    : Stopping( false )
{
    SocketObject = InSocketObject;

    auto threadName = FString::Printf( TEXT( "ViveUDPReceiverThread_%s" ), *(FGuid::NewGuid().ToString()) );
    Thread = MakeShareable( FRunnableThread::Create( this, *threadName, 128 * 1024, EThreadPriority::TPri_AboveNormal, FPlatformAffinity::GetPoolThreadMask() ) );
}

FViveUDPReceiver::~FViveUDPReceiver()
{
    Stop();

    if ( Thread.IsValid() ) {
        Thread->Kill( true );
        Thread = nullptr;
    }
}

void FViveUDPReceiver::Stop()
{
    Stopping = true;

    if ( Thread.IsValid() )
        Thread->WaitForCompletion();
}

uint32 FViveUDPReceiver::Run()
{
    if ( !SocketObject->IsRunning() ) {
        VIVELOG( Error, TEXT( "#### UDP socket is not running. ####" ) );
        return 0;
    }

    const FTimespan socketTimeout( FTimespan::FromMilliseconds( SocketObject->GetWaitTimeMS() ) );

    while ( !Stopping ) {
        if ( SocketObject->IsRunning() && SocketObject->Wait( socketTimeout ) ) {

            uint32 pendingDataSize = 0;
            while( SocketObject->HasPendingData( pendingDataSize ) ) {
                FArrayReaderPtr arrayReader = MakeShared<FArrayReader, ESPMode::ThreadSafe>( true );
                arrayReader->SetNumUninitialized( FMath::Min(pendingDataSize, (uint32)SocketObject->GetMaxPacketSize() ) );

                int32 receivedDataSize = 0;
                FIPv4Endpoint endpoint;
                auto recvResult = false;
                if ( SocketObject->GetUseReceiveFrom() ) {
                    auto sender = SocketObject->GetSocketSubsystem()->CreateInternetAddr();
                    recvResult = SocketObject->ReceiveFrom( arrayReader->GetData(), arrayReader->Num(), receivedDataSize, *sender );
                    if ( recvResult ) {
                        sender->GetIp( endpoint.Address.Value );
                        endpoint.Port = (uint16)sender->GetPort();
                    }
                }
                else {
                    recvResult = SocketObject->Receive( arrayReader->GetData(), arrayReader->Num(), receivedDataSize );
                }

                if ( recvResult ) {
                    ensure( receivedDataSize < SocketObject->GetMaxPacketSize() );
                    arrayReader->RemoveAt( receivedDataSize, arrayReader->Num() - receivedDataSize, false );
                    SocketObject->Receive( arrayReader, endpoint );
                }
            }
        }
    }

    return 0;
}
