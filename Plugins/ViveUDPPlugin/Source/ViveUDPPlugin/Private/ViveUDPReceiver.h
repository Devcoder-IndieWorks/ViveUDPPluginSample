// Copyright ViveStudios. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "HAL/ThreadSafeBool.h"
#include "UObject/WeakObjectPtrTemplates.h"

class FViveUDPReceiver : public FRunnable
{
public:
    FViveUDPReceiver( class UViveUDPSocketObject* InSocketObject );
    ~FViveUDPReceiver();

    virtual uint32 Run() override;
    virtual void Stop() override;

private:
    FThreadSafeBool Stopping;

    TWeakObjectPtr<class UViveUDPSocketObject> SocketObject;
    TSharedPtr<FRunnableThread> Thread;
};
