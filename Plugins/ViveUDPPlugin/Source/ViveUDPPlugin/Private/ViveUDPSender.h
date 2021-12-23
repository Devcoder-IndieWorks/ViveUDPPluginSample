// Copyright ViveStudios. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "HAL/ThreadSafeBool.h"
#include "UObject/WeakObjectPtrTemplates.h"

class FViveUDPSender : public FRunnable
{
public:
    FViveUDPSender( class UViveUDPSocketObject* InSocketObject );
    ~FViveUDPSender();

    virtual uint32 Run() override;
    virtual void Stop() override;

    void SendMessage( const FString& InIP, int32 InPort, const FString& InMessage, const TArray<uint8>& InByteArray );

private:
    void SendBytes( TSharedRef<FInternetAddr>& InAddress, const TArray<uint8>& InByteArray, int32& OutSent );
    void PauseThread( bool InPause );

private:
    FThreadSafeBool Stopping;
    FThreadSafeBool Paused;
    FString SendToIP;
    int32 SendToPort;

    TWeakObjectPtr<class UViveUDPSocketObject> SocketObject;
    TSharedPtr<FRunnableThread> Thread;

    TQueue<FString> MessageQueue;
    TQueue<TArray<uint8>> ByteArrayQueue;
};
