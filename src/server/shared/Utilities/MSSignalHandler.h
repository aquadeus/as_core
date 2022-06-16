////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2014 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __MS_SIGNAL_HANDLER_H
#define __MS_SIGNAL_HANDLER_H

#include "Common.h"

namespace MS
{
    namespace SignalHandler
    {
        static thread_local uint32 gFailureCountLimit = 10;                      ///< How many signal can be receive by the signal handler before shutdown the server

        static thread_local bool gThrowExceptionAtFailure = false;              ///< Store per thread if the current context can be rescued
        static uint32 gFailureCounter = 0;                                      ///< Counter of signal receive by the signal handler

        void EnableThrowExceptionAtFailure();
        void DisableThrowExceptionAtFailure();
        void OnSignalReceive(int p_Signal);
        void SetFailureCountLimit(uint32 p_Limit);
    }
}

#endif
