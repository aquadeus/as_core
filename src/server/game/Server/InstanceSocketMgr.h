////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/** \addtogroup u2w User to World Communication
 *  @{
 *  \file InstancemSocketMgr.h
 *  \author Derex <derex101@gmail.com>
 */

#ifndef CROSS
#ifndef __INSTANCESOCKETMGR_H
#define __INSTANCESOCKETMGR_H

#include "Common.h"

class WorldSocket;
class ReactorRunnable;
class ACE_Event_Handler;

/// Manages all sockets connected to peers and network threads
class InstanceSocketMgr
{
public:
    friend class WorldSocket;
    friend class ACE_Singleton<InstanceSocketMgr, ACE_Thread_Mutex>;

    /// Start network, listen at address:port .
    int StartNetwork(ACE_UINT16 port, const char* address);

    /// Stops all network threads, It will wait for all running threads .
    void StopNetwork();

    /// Wait untill all network threads have "joined" .
    void Wait();

private:

    int OnSocketOpen(WorldSocket* sock);
    int StartReactiveIO(ACE_UINT16 port, const char* address);

private:
    InstanceSocketMgr();
    virtual ~InstanceSocketMgr();

    ReactorRunnable* m_NetThreads;
    size_t m_NetThreadsCount;

    int m_SockOutKBuff;
    int m_SockOutUBuff;
    bool m_UseNoDelay;

    class WorldSocketAcceptor* m_Acceptor;
    class WorldSocketAcceptor* m_InstanceAcceptor;
};

#define sInstanceSocketMgr ACE_Singleton<InstanceSocketMgr, ACE_Thread_Mutex>::instance()

#endif
/// @}
#endif
