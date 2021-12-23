#include "ViveUDPSocketBPLibrary.h"
#include "ViveUDPSocketBPAPIImpl.h"

UViveUDPSocketBPLibrary::UViveUDPSocketBPLibrary( const FObjectInitializer& ObjectInitializer )
    : Super( ObjectInitializer )
{
}

void UViveUDPSocketBPLibrary::GetAPI( TScriptInterface<IViveUDPSocketBPAPI>& OutAPI )
{
    static UViveUDPSocketBPAPIImpl* objInstance = NewObject<UViveUDPSocketBPAPIImpl>( GetTransientPackage(), NAME_None, RF_MarkAsRootSet );
    OutAPI = objInstance;
}
