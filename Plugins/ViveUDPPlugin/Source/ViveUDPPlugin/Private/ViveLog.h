// Copyright ViveStudios. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN( LogViveUDP, Log, All )

#define VIVELOG_CALLINFO ( FString( TEXT( "[" ) ) + FString( __FUNCTION__ ) + TEXT( "(" ) + FString::FromInt( __LINE__ ) + TEXT( ")" ) + FString( TEXT( "]" ) ) )

#define VIVELOG_CALLONLY( Verbosity ) UE_LOG( LogViveUDP, Verbosity, TEXT( "%s" ), *VIVELOG_CALLINFO )
#define VIVELOG( Verbosity, Format, ... ) UE_LOG( LogViveUDP, Verbosity, TEXT( "%s LOG: %s" ), *VIVELOG_CALLINFO, *FString::Printf( Format, ##__VA_ARGS__ ) )
#define CVIVELOG( Condition, Verbosity, Format, ... ) UE_CLOG( Condition, LogViveUDP, Verbosity, TEXT( "%s LOG: %s" ), *VIVELOG_CALLINFO, *FString::Printf( Format, ##__VA_ARGS__ ) )
