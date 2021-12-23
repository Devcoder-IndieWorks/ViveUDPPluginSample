// Copyright ViveStudios. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "UObject/WeakObjectPtrTemplates.h"

class FViveUDPConnection : public FRunnable
{
public:
    FViveUDPConnection( class UViveUDPSocketObject* InSocketObject );
    ~FViveUDPConnection();

    virtual uint32 Run() override;

private:
    TWeakObjectPtr<class UViveUDPSocketObject> SocketObject;
    TSharedPtr<FRunnableThread> Thread;
    class FSocket* Socket;
    class ISocketSubsystem* SocketSubsystem;
};
