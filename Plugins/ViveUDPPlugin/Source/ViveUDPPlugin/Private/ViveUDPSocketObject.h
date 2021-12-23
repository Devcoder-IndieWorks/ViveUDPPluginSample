// Copyright ViveStudios. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Sockets.h"
#include "Networking/Public/Networking.h"
#include "UObject/Object.h"
#include "Misc/Timespan.h"
#include "ViveUDPReceiveFilterType.h"
#include "ViveUDPSocketObject.generated.h"

UCLASS()
class UViveUDPSocketObject : public UObject
{
    GENERATED_UCLASS_BODY()
public:
    void Setup( const FString& InIP, int32 InPort, const FString InConnectionID, int32 InMaxPacketSize = 65507, int32 InWaitTimeMS = 100, 
        bool InUseSender = true, bool InUseReceiveFrom = false, bool InUseBroadcast = false, 
        EViveUDPReceiveFilterType InReceiveFilter =  EViveUDPReceiveFilterType::E_SAB );
    void Connection();
    void Disconnection();
    void StartSender();
    void StartReceiver();

    //-------------------------------------------------------------------------

    void SetConnectionCallback( TFunction<void( bool, const FString&, const FString& )>&& InCallback );
    void SetDisconnectionCallback( TFunction<void()>&& InCallback );
    void SetReceiveCallback( TFunction<void( const FString&, const TArray<uint8>&, const FString& )>&& InCallback );
    void SetReceiveFromCallback( TFunction<void( const FString&, const TArray<uint8>&, const FString&, int32, const FString& )>&& InCallback );

    //-------------------------------------------------------------------------

    void ConnectionCallback( bool InSuccess, const FString& InMessage, const FString& InConnectionID );
    void DisconnectionCallback();
    void ReceiveCallback( const FString& InMessage, const TArray<uint8>& InByteArray, const FString& InConnectionIO );
    void ReceiveFromCallback( const FString& InMessage, const TArray<uint8>& InByteArray, const FString& InIP, int32 InPort, const FString& InConnectionID );

    //-------------------------------------------------------------------------

    void SendTo( const FString& InIP, int32 InPort, const FString& InMessage, const TArray<uint8>& InByteArray );
    int32 SendTo( TSharedRef<FInternetAddr>& InAddress, const TArray<uint8>& InByteArray );

    void Receive( const FArrayReaderPtr& InArrayReaderPtr, const FIPv4Endpoint& InEndPoint );
    bool Receive( uint8* OutData, int32 InBufferSize, int32& OutBytesRead );
    bool ReceiveFrom( uint8* OutData, int32 InBufferSize, int32& OutBytesRead, FInternetAddr& OutSender );

    bool Wait( const FTimespan& InWaitTime );
    bool HasPendingData( uint32& OutPendingDataSize );

    //-------------------------------------------------------------------------

    bool IsRunning() const;
    void SetRunning( bool InRunning );

    FSocket* GetSocket() const;
    void SetSocket( FSocket* InSocket );

    ISocketSubsystem* GetSocketSubsystem() const;
    void SetSocketSubsystem( ISocketSubsystem* InSocketSubsystem );

    FString GetIP() const;
    int32 GetPort() const;

    FString GetConnectionID() const;
    int32 GetMaxPacketSize() const;
    int32 GetWaitTimeMS() const;
    bool GetUseReceiveFrom() const;
    bool GetUseBroadcast() const;
    EViveUDPReceiveFilterType GetReceiveFilter() const;

private:
    bool Running;
    FString ConnectionID;
    FString IP;
    int32 Port;
    int32 MaxPacketSize;
    int32 WaitTimeMS;
    bool UseSender;
    bool UseReceiveFrom;
    bool UseBroadcast;

    EViveUDPReceiveFilterType ReceiveFilter;

    //-------------------------------------------------------------------------

    TFunction<void( bool, const FString&, const FString& )> OnConnection;
    TFunction<void()> OnDisconnection;
    TFunction<void( const FString&, const TArray<uint8>&, const FString& )> OnReceive;
    TFunction<void( const FString&, const TArray<uint8>&, const FString&, int32, const FString& )> OnReceiveFrom;

    //-------------------------------------------------------------------------

    FSocket* Socket;
    ISocketSubsystem* SocketSubsystem;

    //-------------------------------------------------------------------------

    TSharedPtr<class FViveUDPConnection> UDPConnection;
    TSharedPtr<class FViveUDPSender> UDPSender;
    TSharedPtr<class FViveUDPReceiver> UDPReceiver;
};
