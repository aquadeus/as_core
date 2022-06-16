////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef CROSS
#include <ace/Message_Block.h>
#include <ace/OS_NS_string.h>
#include <ace/OS_NS_unistd.h>
#include <ace/os_include/arpa/os_inet.h>
#include <ace/os_include/netinet/os_tcp.h>
#include <ace/os_include/sys/os_types.h>
#include <ace/os_include/sys/os_socket.h>
#include <ace/OS_NS_string.h>
#include <ace/Reactor.h>
#include <ace/Auto_Ptr.h>

#include "WorldSocket.h"
#include "Common.h"

#include "Util.h"
#include "World.h"
#include "WorldPacket.h"
#include "SharedDefines.h"
#include "ByteBuffer.h"
#include "Opcodes.h"
#include "DatabaseEnv.h"
#include "BigNumber.h"
#include "SHA1.h"
#include "WorldSession.h"
#include "RealmSocketMgr.h"
#include "InstanceSocketMgr.h"
#include "Log.h"
#include "PacketLog.h"
#include "ScriptMgr.h"
#include "AccountMgr.h"
#include "ObjectMgr.h"
#include "zlib.h"
#include "HMACSHA1.h"
#include "WardenKeyGeneration.h"
#include "Util.h"
#include "AuthenticationPackets.h"
#include "CharacterPackets.h"
#include "CollectionMgr.hpp"

std::string const WorldSocket::m_ServerConnectionInitialize("WORLD OF WARCRAFT CONNECTION - SERVER TO CLIENT");
std::string const WorldSocket::m_ClientConnectionInitialize("WORLD OF WARCRAFT CONNECTION - CLIENT TO SERVER");
uint8 const ContinuedSessionSeed[16] = { 0x16, 0xAD, 0x0C, 0xD4, 0x46, 0xF9, 0x4F, 0xB2, 0xEF, 0x7D, 0xEA, 0x2A, 0x17, 0x66, 0x4D, 0x2F };
uint32 const g_SizeOfClientHeader[2] = { 0, 6 };

WorldSocket::WorldSocket(void) : WorldHandler(),
m_LastPingTime(ACE_Time_Value::zero), m_OverSpeedPings(0), m_Session(0), m_RecvWPct(0),
m_RecvPct(), m_HeaderBuffer(g_SizeOfClientHeader[0]), m_PacketBuffer(4096), m_OutBuffer(0),
m_OutBufferSize(65536), m_OutActive(false), m_Seed(static_cast<uint32> (rand32())), m_Initialized(false),
m_CompressionStream(nullptr), m_CryptKey(nullptr), m_DelayedCloseSocket(false)
{
    m_Seed.SetRand(8 * 16);

    reference_counting_policy().value(ACE_Event_Handler::Reference_Counting_Policy::ENABLED);

    msg_queue()->high_water_mark(8 * 1024 * 1024);
    msg_queue()->low_water_mark(8 * 1024 * 1024);

    _type = CONNECTION_TYPE_REALM;
    _key = 0;

    m_QueryCallbackMgr = std::make_shared<QueryCallbackManager>();
}

WorldSocket::~WorldSocket (void)
{
    delete m_RecvWPct;
    delete m_CryptKey;

    if (m_OutBuffer)
        m_OutBuffer->release();

    closing_ = true;

    peer().close();

    if (m_CompressionStream)
    {
        deflateEnd(m_CompressionStream);
        delete m_CompressionStream;
    }

    /// Session not yet added to the list
    if (m_Session && !m_Crypt.IsInitialized())
        delete m_Session;

    WorldPacket* l_Packet = nullptr;
    while (m_SendingPacketQueue.next(l_Packet))
    {
        if (!(l_Packet->m_SafeReferenceCount--))
            delete l_Packet;
    }
}

bool WorldSocket::IsClosed (void) const
{
    return closing_;
}

void WorldSocket::CloseSocket (void)
{
    {
        ACE_GUARD(LockType, Guard, m_SessionLock);
        m_Session = nullptr;
    }

    {
        ACE_GUARD (LockType, Guard, m_OutBufferLock);

        if (closing_)
            return;

        closing_ = true;
        peer().close_writer();
    }
}

const std::string& WorldSocket::GetRemoteAddress (void) const
{
    return m_Address;
}

int WorldSocket::SendPacket(WorldPacket const& pct)
{
    if (closing_)
        return -1;

    if (pct.GetOpcode() == 0)
        return 0;

    WorldPacket const* pkt = &pct;
    const_cast<WorldPacket*>(pkt)->FlushBits();

    // Dump outgoing packet
# ifdef WIN32
    if (sPacketLog->CanLogPacket())
        sPacketLog->LogPacket(pct, SERVER_TO_CLIENT);

    if (sWorld->getBoolConfig(CONFIG_LOG_PACKETS))
    {
        switch (pct.GetOpcode())
        {
            case SMSG_MONSTER_MOVE:
            case SMSG_THREAT_CLEAR:
            case SMSG_THREAT_REMOVE:
            case SMSG_THREAT_UPDATE:
            case SMSG_HIGHEST_THREAT_UPDATE:
            case SMSG_ATTACKER_STATE_UPDATE:
                break;

            default:
                printf("Send packet %s\n", GetOpcodeNameForLogging(pkt->GetOpcode(), WOW_SERVER_TO_CLIENT).c_str());
        }
    }
# endif

    m_SendingPacketQueue.add(new WorldPacket(*pkt));

    return 0;
}

void WorldSocket::SendPackets(std::vector<WorldPacket*>& p_Packets)
{
    if (closing_)
        return;

# ifdef WIN32
    for (WorldPacket* l_Packet : p_Packets)
    {
        if (sPacketLog->CanLogPacket())
            sPacketLog->LogPacket(*l_Packet, SERVER_TO_CLIENT);

        if (sWorld->getBoolConfig(CONFIG_LOG_PACKETS))
        {
            switch (l_Packet->GetOpcode())
            {
                case SMSG_MONSTER_MOVE:
                case SMSG_THREAT_CLEAR:
                case SMSG_THREAT_REMOVE:
                case SMSG_THREAT_UPDATE:
                case SMSG_HIGHEST_THREAT_UPDATE:
                case SMSG_ATTACKER_STATE_UPDATE:
                    break;
                default:
                    printf("Send packet %s\n", GetOpcodeNameForLogging(l_Packet->GetOpcode(), WOW_SERVER_TO_CLIENT).c_str());
            }
        }
    }
# endif

    for (WorldPacket* l_Packet : p_Packets)
        m_SendingPacketQueue.add(l_Packet);
}

uint32 const g_MinSizeForCompression = 0x400;

ACE_Message_Block* WorldSocket::WritePacketToBuffer(WorldPacket const& p_Packet)
{
    uint32 l_PacketSize = p_Packet.size();

    if (l_PacketSize > g_MinSizeForCompression && m_CompressionStream)
        l_PacketSize = compressBound(l_PacketSize) + sizeof(CompressedWorldPacket);

    uint32 l_SizeOfHeader = g_SizeOfClientHeader[m_Initialized];
    uint32 l_TotalSize = l_PacketSize + l_SizeOfHeader;
    uint32 l_Opcode = p_Packet.GetOpcode();
    ACE_Message_Block* l_Buffer;

    // If there is free space in the output buffer insert it instead of enqueing the packet
    if (m_OutBuffer->space() >= l_TotalSize && msg_queue()->is_empty())
        l_Buffer = m_OutBuffer;
    else
        ACE_NEW_RETURN(l_Buffer, ACE_Message_Block(l_TotalSize), nullptr);

    CompressedWorldPacket l_CompressedPacket;
    PktHeader* l_HeaderPointer = reinterpret_cast<PktHeader*>(l_Buffer->wr_ptr());
    l_Buffer->wr_ptr(l_SizeOfHeader);

    if (p_Packet.size() > g_MinSizeForCompression && m_CompressionStream)
    {
        l_CompressedPacket.m_UncompressedSize = p_Packet.size() + 2;
        l_CompressedPacket.m_UncompressedAdler = adler32(adler32(0x9827D8F1, (Bytef*)&l_Opcode, 2), p_Packet.contents(), p_Packet.size());

        /// Reserve space for compression info - uncompressed size and checksums
        char* l_CompressionInfo = l_Buffer->wr_ptr();
        l_Buffer->wr_ptr(sizeof(CompressedWorldPacket));

        uint32 l_CompressedSize = CompressPacket(l_Buffer->wr_ptr(), p_Packet);
        l_CompressedPacket.m_CompressedAdler = adler32(0x9827D8F1, (const Bytef*)l_Buffer->wr_ptr(), l_CompressedSize);

        l_Buffer->wr_ptr(l_CompressedSize);
        memcpy(l_CompressionInfo, &l_CompressedPacket, sizeof(CompressedWorldPacket));

        l_PacketSize = l_CompressedSize + sizeof(CompressedWorldPacket);
        l_Opcode = SMSG_COMPRESSED_PACKET;
    }
    else if (l_PacketSize)
    {
        if (l_Buffer->copy((char*)(p_Packet.contents()), l_PacketSize) == -1)
            ACE_ASSERT(false);
    }

    l_HeaderPointer->m_Size = l_PacketSize + sizeof(uint16);                          ///< Including opcopde
    l_HeaderPointer->m_Command = l_Opcode;

    m_Crypt.EncryptSend((uint8*)l_HeaderPointer, sizeof(l_HeaderPointer->m_Size));    ///< Only size in Legion*/

    return l_Buffer;
}

uint32 WorldSocket::CompressPacket(char* p_Buffer, WorldPacket const& p_Packet)
{
    uint16 l_Opcode = p_Packet.GetOpcode();
    uint32 l_BufferSize = deflateBound(m_CompressionStream, p_Packet.size() + sizeof(l_Opcode));

    m_CompressionStream->next_out = (Bytef*)p_Buffer;
    m_CompressionStream->avail_out = l_BufferSize;
    m_CompressionStream->next_in = (Bytef*)&l_Opcode;
    m_CompressionStream->avail_in = sizeof(l_Opcode);

    int32 z_res = deflate(m_CompressionStream, Z_NO_FLUSH);
    if (z_res != Z_OK)
    {
        sLog->outAshran("Can't compress packet opcode (zlib: deflate) Error code: %i (%s, msg: %s)", z_res, zError(z_res), m_CompressionStream->msg);
        return 0;
    }

    m_CompressionStream->next_in = (Bytef*)p_Packet.contents();
    m_CompressionStream->avail_in = p_Packet.size();

    z_res = deflate(m_CompressionStream, Z_SYNC_FLUSH);
    if (z_res != Z_OK)
    {
        sLog->outAshran("Can't compress packet data (zlib: deflate) Error code: %i (%s, msg: %s)", z_res, zError(z_res), m_CompressionStream->msg);
        return 0;
    }

    return l_BufferSize - m_CompressionStream->avail_out;
}

long WorldSocket::AddReference (void)
{
    return static_cast<long> (add_reference());
}

long WorldSocket::RemoveReference (void)
{
    return static_cast<long> (remove_reference());
}

int WorldSocket::open (void *a)
{
    ACE_UNUSED_ARG (a);

    // Prevent double call to this func.
    if (m_OutBuffer)
        return -1;

    ACE_INET_Addr local_addr;
    if (peer().get_local_addr(local_addr) == -1)
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "WorldSocket::open: peer().get_remote_addr errno = %s", ACE_OS::strerror(errno));
        return -1;
    }

    // This will also prevent the socket from being Updated
    // while we are initializing it.
    m_OutActive = true;

    // Hook for the manager.
    if (local_addr.get_port_number() == sWorld->getIntConfig(CONFIG_PORT_WORLD))
    {
        if (sRealmSocketMgr->OnSocketOpen(this) == -1)
            return -1;
    }

    if (local_addr.get_port_number() == sWorld->getIntConfig(CONFIG_PORT_INSTANCE))
    {
        if (sInstanceSocketMgr->OnSocketOpen(this) == -1)
            return -1;
    }

    // Allocate the buffer.
    ACE_NEW_RETURN (m_OutBuffer, ACE_Message_Block (m_OutBufferSize), -1);

    // Store peer address.
    ACE_INET_Addr remote_addr;

    if (peer().get_remote_addr(remote_addr) == -1)
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "WorldSocket::open: peer().get_remote_addr errno = %s", ACE_OS::strerror (errno));
        return -1;
    }

    m_Address = remote_addr.get_host_addr();

    // Send handshake string
    uint8 l_Terminator = '\n';

    if (m_OutBuffer->copy(m_ServerConnectionInitialize.c_str(), m_ServerConnectionInitialize.size()) == -1)
        ACE_ASSERT(false);

    if (m_OutBuffer->copy((const char*)&l_Terminator, sizeof(l_Terminator)) == -1)
        ACE_ASSERT(false);

    // Register with ACE Reactor
    if (reactor()->register_handler(this, ACE_Event_Handler::READ_MASK | ACE_Event_Handler::WRITE_MASK) == -1)
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "WorldSocket::open: unable to register client handler errno = %s", ACE_OS::strerror (errno));
        return -1;
    }

    // reactor takes care of the socket from now on
    remove_reference();

    return 0;
}

int WorldSocket::close (u_long)
{
    shutdown();

    closing_ = true;

    remove_reference();

    return 0;
}

int WorldSocket::handle_input (ACE_HANDLE)
{
    if (closing_)
        return -1;

    switch (HandleNewPacket())
    {
    case -1 :
        {
            if ((errno == EWOULDBLOCK) ||
                (errno == EAGAIN))
            {
                return Update();                           // interesting line, isn't it ?
            }

            sLog->outDebug(LOG_FILTER_NETWORKIO, "WorldSocket::handle_input: Peer error closing connection errno = %s", ACE_OS::strerror (errno));

            errno = ECONNRESET;
            return -1;
        }
    case 0:
        {
            sLog->outDebug(LOG_FILTER_NETWORKIO, "WorldSocket::handle_input: Peer has closed connection");

            errno = ECONNRESET;
            return -1;
        }
    case 1:
        return 1;
    default:
        return Update();                               // another interesting line ;)
    }

    ACE_NOTREACHED(return -1);
}

int WorldSocket::handle_output (ACE_HANDLE)
{
    ACE_GUARD_RETURN (LockType, Guard, m_OutBufferLock, -1);

    if (closing_)
        return -1;

    size_t send_len = m_OutBuffer->length();

    if (send_len == 0)
        return handle_output_queue(Guard);

#ifdef MSG_NOSIGNAL
    ssize_t n = peer().send (m_OutBuffer->rd_ptr(), send_len, MSG_NOSIGNAL);
#else
    ssize_t n = peer().send (m_OutBuffer->rd_ptr(), send_len);
#endif // MSG_NOSIGNAL

    if (n == 0)
        return -1;
    else if (n == -1)
    {
        if (errno == EWOULDBLOCK || errno == EAGAIN)
            return schedule_wakeup_output (Guard);

        return -1;
    }
    else if (n < (ssize_t)send_len) //now n > 0
    {
        m_OutBuffer->rd_ptr (static_cast<size_t> (n));

        // move the data to the base of the buffer
        m_OutBuffer->crunch();

        return schedule_wakeup_output (Guard);
    }
    else //now n == send_len
    {
        m_OutBuffer->reset();

        return handle_output_queue (Guard);
    }

    ACE_NOTREACHED (return 0);
}

int WorldSocket::handle_output_queue (GuardType& g)
{
    if (msg_queue()->is_empty())
        return cancel_wakeup_output(g);

    ACE_Message_Block* mblk;

    if (msg_queue()->dequeue_head(mblk, (ACE_Time_Value*)&ACE_Time_Value::zero) == -1)
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "WorldSocket::handle_output_queue dequeue_head");
        return -1;
    }

    const size_t send_len = mblk->length();

#ifdef MSG_NOSIGNAL
    ssize_t n = peer().send (mblk->rd_ptr(), send_len, MSG_NOSIGNAL);
#else
    ssize_t n = peer().send (mblk->rd_ptr(), send_len);
#endif // MSG_NOSIGNAL

    if (n == 0)
    {
        mblk->release();

        return -1;
    }
    else if (n == -1)
    {
        if (errno == EWOULDBLOCK || errno == EAGAIN)
        {
            msg_queue()->enqueue_head(mblk, (ACE_Time_Value*) &ACE_Time_Value::zero);
            return schedule_wakeup_output (g);
        }

        mblk->release();
        return -1;
    }
    else if (n < (ssize_t)send_len) //now n > 0
    {
        mblk->rd_ptr(static_cast<size_t> (n));

        if (msg_queue()->enqueue_head(mblk, (ACE_Time_Value*) &ACE_Time_Value::zero) == -1)
        {
            sLog->outError(LOG_FILTER_NETWORKIO, "WorldSocket::handle_output_queue enqueue_head");
            mblk->release();
            return -1;
        }

        return schedule_wakeup_output (g);
    }
    else //now n == send_len
    {
        mblk->release();

        return msg_queue()->is_empty() ? cancel_wakeup_output(g) : ACE_Event_Handler::WRITE_MASK;
    }

    ACE_NOTREACHED(return -1);
}

int WorldSocket::handle_close (ACE_HANDLE h, ACE_Reactor_Mask)
{
    // Critical section
    {
        ACE_GUARD_RETURN (LockType, Guard, m_OutBufferLock, -1);

        closing_ = true;

        if (h == ACE_INVALID_HANDLE)
            peer().close_writer();
    }

    // Critical section
    {
        ACE_GUARD_RETURN (LockType, Guard, m_SessionLock, -1);

        if (m_Session)
            m_Session->SetforceExit();

        m_Session = NULL;
    }

    reactor()->remove_handler(this, ACE_Event_Handler::DONT_CALL | ACE_Event_Handler::ALL_EVENTS_MASK);
    return 0;
}

int WorldSocket::Update (void)
{
    if (closing_)
        return -1;

    if (m_OutActive)
        return 0;

    m_QueryCallbackMgr->Update();

    /// Move world packet from the intermediate lock-free queue to the ACE message queue
    {
        WorldPacket* l_Packet = nullptr;

        WorldPacket l_MultiplePackets(SMSG_MULTIPLE_PACKETS);

        while (m_SendingPacketQueue.next(l_Packet))
        {
            /// Packet is too big, compress and send it right away (size of one packet in SMSG_MULTIPLE_PACKETS is uint16)
            if (l_Packet->size() > 0x7FFF || !sWorld->getBoolConfig(CONFIG_ENABLE_MULTIPLE_PACKETS))
            {
                ACE_Message_Block* l_Buffer = WritePacketToBuffer(*l_Packet);

                if (l_Buffer != m_OutBuffer && l_Buffer != nullptr)
                {
                    if (msg_queue()->enqueue_tail(l_Buffer, (ACE_Time_Value*)&ACE_Time_Value::zero) == -1)
                    {
                        sLog->outError(LOG_FILTER_NETWORKIO, "WorldSocket::Update enqueue_tail failed");
                        l_Buffer->release();
                        return -1;
                    }
                }
            }
            else
            {
                l_MultiplePackets << uint16(l_Packet->size());
                l_MultiplePackets << uint16(l_Packet->GetOpcode());

                if (l_Packet->size())
                    l_MultiplePackets.append(l_Packet->contents(), l_Packet->size());
            }

            if (l_Packet->m_SafeReferenceCount.fetch_sub(1) == 1)
                delete l_Packet;
        }

        if (l_MultiplePackets.size())
        {
            ACE_Message_Block* l_Buffer = WritePacketToBuffer(l_MultiplePackets);

            if (l_Buffer != m_OutBuffer && l_Buffer != nullptr)
            {
                if (msg_queue()->enqueue_tail(l_Buffer, (ACE_Time_Value*)&ACE_Time_Value::zero) == -1)
                {
                    sLog->outError(LOG_FILTER_NETWORKIO, "WorldSocket::Update enqueue_tail failed");
                    l_Buffer->release();
                    return -1;
                }
            }
        }
    }

    {
        ACE_GUARD_RETURN(LockType, Guard, m_OutBufferLock, 0);
        if (m_OutBuffer->length() == 0 && msg_queue()->is_empty())
            return 0;
    }

    int ret;
    do
    ret = handle_output(get_handle());
    while (ret > 0);

    if (m_DelayedCloseSocket)
        return -1;

    return ret;
}

int WorldSocket::ReadPacketHeader()
{
    ACE_ASSERT(m_RecvWPct == nullptr && m_HeaderBuffer.size() == g_SizeOfClientHeader[m_Initialized]);
    m_Crypt.DecryptRecv(reinterpret_cast<uint8*>(m_HeaderBuffer.rd_ptr()), sizeof(uint32)); ///< Only size in Legion

    uint32 l_Opcode;
    uint32 l_Size;

    ExtractOpcodeAndSize(reinterpret_cast<PktHeader*>(m_HeaderBuffer.rd_ptr()), l_Opcode, l_Size);

#ifdef WIN32

    if (m_Initialized && sWorld->getBoolConfig(CONFIG_LOG_PACKETS))
    {
        std::string l_OopcodeName = GetOpcodeNameForLogging((Opcodes)l_Opcode, WOW_CLIENT_TO_SERVER);
        printf("Receive opcode %s 0x%08.8X size : %u \n", l_OopcodeName.c_str(), l_Opcode, l_Size);
    }
#endif

    if (m_Initialized && !PktHeader::IsValidOpcode(l_Opcode))
    {
        Player* _player = m_Session ? m_Session->GetPlayer() : NULL;
        sLog->outError(LOG_FILTER_NETWORKIO, "WorldSocket::ReadPacketHeader(): client (account: %u, char [GUID: %u, name: %s]) sent malformed packet (size: %d, cmd: %d)",
            m_Session ? m_Session->GetAccountId() : 0,
            _player ? _player->GetGUIDLow() : 0,
            _player ? _player->GetName() : "<none>",
            l_Size, l_Opcode);

        errno = EINVAL;
        return -1;
    }

    ACE_NEW_RETURN(m_RecvWPct, WorldPacket((Opcodes)l_Opcode, l_Size), -1);
    m_RecvWPct->resize(l_Size);

    if (l_Size != 0)
        m_RecvPct.base((char*)m_RecvWPct->contents(), l_Size);

    return 0;
}

void WorldSocket::ExtractOpcodeAndSize(PktHeader const* p_Header, uint32& p_Opcode, uint32& p_Size) const
{
    p_Opcode = p_Header->m_Command;
    p_Size = p_Header->m_Size - sizeof(p_Header->m_Command);
}

int WorldSocket::HandlePacketData()
{
    // set errno properly here on error !!!
    // now have a header and payload

    ACE_ASSERT(m_RecvPct.space() == 0);
    ACE_ASSERT(m_HeaderBuffer.space() == 0);
    ACE_ASSERT(m_RecvWPct != nullptr);

    const int l_ReturnValue = ProcessIncoming(m_RecvWPct);

    m_RecvPct.base(nullptr, 0);
    m_RecvPct.reset();
    m_RecvWPct = nullptr;

    m_HeaderBuffer.reset();

    if (l_ReturnValue == -1)
        errno = EINVAL;

    return l_ReturnValue;
}

int WorldSocket::HandleNewPacket()
{
    m_PacketBuffer.reset();

    const size_t l_SizeOfBuffer = m_PacketBuffer.space();
    const ssize_t l_ReadSize = peer().recv(m_PacketBuffer.wr_ptr(), l_SizeOfBuffer);

    if (l_ReadSize <= 0)
        return int(l_ReadSize);

     m_PacketBuffer.wr_ptr(l_ReadSize);

    while (m_PacketBuffer.length() > 0)
    {
        if (m_PacketBuffer.rd_ptr()[0] == '\n' && !m_Crypt.IsInitialized() && m_PacketBuffer.length() == 1)
        {
            m_PacketBuffer.rd_ptr(1);
            continue;
        }

        if (m_Initialized)
        {
            // Receive header first
            if (m_HeaderBuffer.space() > 0)
            {
                const size_t l_HeaderReadSize = std::min(m_PacketBuffer.length(),  m_HeaderBuffer.space());
                m_HeaderBuffer.copy(m_PacketBuffer.rd_ptr(), l_HeaderReadSize);
                m_PacketBuffer.rd_ptr(l_HeaderReadSize);

                if (m_HeaderBuffer.space() > 0)
                {
                    ACE_ASSERT(m_PacketBuffer.length() == 0);
                    // Couldn't receive the whole header this time.
                    errno = EWOULDBLOCK;
                    return -1;
                }

                // We just received nice new header
                if (ReadPacketHeader() == -1)
                    return -1;
            }
        }
        // Its possible on some error situations that this happens
        // for example on closing when epoll receives more chunked data and stuff
        // hope this is not hack, as proper m_RecvWPct is asserted around
        if (m_Initialized && !m_RecvWPct)
        {
            sLog->outError(LOG_FILTER_NETWORKIO, "Forcing close on input m_RecvWPct = NULL");
            errno = EINVAL;
            return -1;
        }

        if (!m_Initialized)
        {
            m_RecvWPct = new WorldPacket((Opcodes)0x0000, m_PacketBuffer.length());
            m_RecvWPct->resize(m_PacketBuffer.length());
            m_RecvPct.base((char*)m_RecvWPct->contents(), m_PacketBuffer.length());
        }

        // We have full read header, now check the data payload
        if (m_RecvPct.space() > 0)
        {
            //need more data in the payload
            const size_t l_PacketRecvSize = std::min(m_PacketBuffer.length(), m_RecvPct.space());
            m_RecvPct.copy(m_PacketBuffer.rd_ptr(), l_PacketRecvSize);
            m_PacketBuffer.rd_ptr(l_PacketRecvSize);

            if (m_RecvPct.space() > 0)
            {
                ACE_ASSERT(m_PacketBuffer.length() == 0);
                // Couldn't receive the whole data this time.
                errno = EWOULDBLOCK;
                return -1;
            }
        }

        //just received fresh new payload
        if (HandlePacketData() == -1)
            return -1;
    }

    return size_t(l_ReadSize) == l_SizeOfBuffer ? 1 : 2;
}

int WorldSocket::cancel_wakeup_output (GuardType& g)
{
    if (!m_OutActive)
        return 0;

    m_OutActive = false;

    g.release();

    if (reactor()->cancel_wakeup
        (this, ACE_Event_Handler::WRITE_MASK) == -1)
    {
        // would be good to store errno from reactor with errno guard
        sLog->outError(LOG_FILTER_NETWORKIO, "WorldSocket::cancel_wakeup_output");
        return -1;
    }

    return 0;
}

int WorldSocket::schedule_wakeup_output (GuardType& g)
{
    if (m_OutActive)
        return 0;

    m_OutActive = true;

    g.release();

    if (reactor()->schedule_wakeup
        (this, ACE_Event_Handler::WRITE_MASK) == -1)
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "WorldSocket::schedule_wakeup_output");
        return -1;
    }

    return 0;
}

int WorldSocket::ProcessIncoming(WorldPacket* p_NewPacket)
{
    ACE_ASSERT(p_NewPacket);

    // manage memory ;)
    ACE_Auto_Ptr<WorldPacket> aptr(p_NewPacket);

    if (closing_)
        return -1;

    if (m_Initialized)
    {
        // Dump received packet.
        if (sPacketLog->CanLogPacket())
            sPacketLog->LogPacket(*p_NewPacket, CLIENT_TO_SERVER);

        try
        {
            switch (p_NewPacket->GetOpcode())
            {
                case CMSG_PING:
                    return HandlePing(*p_NewPacket);
                case CMSG_AUTH_SESSION:
                {
                    if (m_Session)
                    {
                        sLog->outError(LOG_FILTER_NETWORKIO, "WorldSocket::ProcessIncoming: received duplicate CMSG_AUTH_SESSION from %s", m_Session->GetPlayerName(false).c_str());
                        return -1;
                    }

                    sScriptMgr->OnPacketReceive(WorldPacket(*p_NewPacket));
                    return HandleAuthSession(*p_NewPacket);
                }
                case CMSG_KEEP_ALIVE:
                {
                    sLog->outDebug(LOG_FILTER_NETWORKIO, "%s", GetOpcodeNameForLogging(p_NewPacket->GetOpcode(), WOW_CLIENT_TO_SERVER).c_str());
                    sScriptMgr->OnPacketReceive(WorldPacket(*p_NewPacket));
                    return 0;
                }
                case CMSG_LOG_DISCONNECT:
                {
                    sLog->outAshran("Received Disconnect reason %u\n", p_NewPacket->read<uint32>());

                    #ifdef WIN32
                        printf("Received Disconnect reason %u\n", p_NewPacket->read<uint32>());
                    #endif

                    ACE_GUARD_RETURN(LockType, Guard, m_SessionLock, -1);
                    if (m_Session)
                        m_Session->SetforceExit();

                    sScriptMgr->OnPacketReceive(WorldPacket(*p_NewPacket));
                    return 0;
                }
                case CMSG_CRYPT_ENABLED:
                {
                    sScriptMgr->OnPacketReceive(WorldPacket(*p_NewPacket));
                    ACE_GUARD_RETURN(LockType, Guard, m_SessionLock, -1);
                    return HandleCryptEnabled(*p_NewPacket);
                }
                case CMSG_AUTH_CONTINUED_SESSION:
                {
                    sScriptMgr->OnPacketReceive(WorldPacket(*p_NewPacket));
                    return HandleAuthContinuedSession(*p_NewPacket);
                }
                case CMSG_CONNECT_TO_FAILED:
                {
                    ACE_GUARD_RETURN(LockType, Guard, m_SessionLock, -1);
                    WorldPackets::Auth::ConnectToFailed connectToFailed(*p_NewPacket);
                    connectToFailed.Read();
                    HandleConnectToFailed(connectToFailed);
                    return 0;
                }
                default:
                {
                    ACE_GUARD_RETURN(LockType, Guard, m_SessionLock, -1);
                    if (!m_Session)
                    {
                        sLog->outError(LOG_FILTER_OPCODES, "ProcessIncoming: Client not authed opcode = %u", uint32(p_NewPacket->GetOpcode()));
                        return -1;
                    }

                    OpcodeHandler* handler = g_OpcodeTable[WOW_CLIENT_TO_SERVER][p_NewPacket->GetOpcode()];
                    if (!handler || handler->status == STATUS_UNHANDLED)
                    {
                        //sLog->outError(LOG_FILTER_OPCODES, "No defined handler for opcode %s sent by %s", GetOpcodeNameForLogging(p_NewPacket->GetOpcode(), WOW_CLIENT_TO_SERVER).c_str(), m_Session->GetPlayerName(false).c_str());
                        return 0;
                    }

                    // Our Idle timer will reset on any non PING opcodes.
                    // Catches people idling on the login screen and any lingering ingame connections.
                    m_Session->ResetTimeOutTime();

                    // OK, give the packet to WorldSession
                    aptr.release();
                    // WARNING here we call it with locks held.
                    // Its possible to cause deadlock if QueuePacket calls back
                    if (handler->forwardToIR == PROCESS_DISTANT_IF_NEED && m_Session->GetInterRealmBG())
                    {
                        Player* player = m_Session->GetPlayer();
                        if (!player)
                            return 0;

                        sWorld->GetInterRealmSession()->SendTunneledPacket(player->GetGUID(), p_NewPacket);
                    }
                    else
                        m_Session->QueuePacket(p_NewPacket);

                    return 0;
                }
            }
        }
        catch (ByteBufferException &)
        {
            sLog->outError(LOG_FILTER_NETWORKIO, "WorldSocket::ProcessIncoming ByteBufferException occured while parsing an instant handled packet %s from client %s, accountid=%i. Disconnected client.",
                GetOpcodeNameForLogging(p_NewPacket->GetOpcode(), WOW_CLIENT_TO_SERVER).c_str(), GetRemoteAddress().c_str(), m_Session ? int32(m_Session->GetAccountId()) : -1);
            p_NewPacket->hexlike();
            return -1;
        }
    }
    else
    {
        if (std::string(reinterpret_cast<char const*>(m_RecvPct.rd_ptr()), std::min(m_RecvPct.size(), m_ClientConnectionInitialize.length())) != m_ClientConnectionInitialize)
            return -1;

        if (sWorld->getIntConfig(CONFIG_COMPRESSION))
        {
            m_CompressionStream = new z_stream();
            m_CompressionStream->zalloc = (alloc_func)NULL;
            m_CompressionStream->zfree = (free_func)NULL;
            m_CompressionStream->opaque = (voidpf)NULL;
            m_CompressionStream->avail_in = 0;
            m_CompressionStream->next_in = NULL;

            int32 z_res = deflateInit2(m_CompressionStream, sWorld->getIntConfig(CONFIG_COMPRESSION), Z_DEFLATED, -15, 8, Z_DEFAULT_STRATEGY);
            if (z_res != Z_OK)
            {
                sLog->outAshran("Can't initialize packet compression (zlib: deflateInit) Error code: %i (%s)", z_res, zError(z_res));
                return false;
            }
        }

        m_Initialized = true;
        m_HeaderBuffer.size(g_SizeOfClientHeader[1]);

        return HandleSendAuthSession();
    }

    ACE_NOTREACHED (return 0);
}

int WorldSocket::HandleSendAuthSession()
{
    _encryptSeed.SetRand(16 * 8);
    _decryptSeed.SetRand(16 * 8);

    WorldPackets::Auth::AuthChallenge challenge;
    memcpy(challenge.Challenge.data(), m_Seed.AsByteArray(16), 16);
    memcpy(&challenge.DosChallenge[0], _encryptSeed.AsByteArray(16), 16);
    memcpy(&challenge.DosChallenge[4], _decryptSeed.AsByteArray(16), 16);
    challenge.DosZeroBits = 1;

    SendPacket(*challenge.Write());
    return 0;
}

enum AuthSessionQueryIndex
{
    AUTH_SESSION_QUERY_GMLEVEL,
    AUTH_SESSION_QUERY_BANNED,
    AUTH_SESSION_QUERY_PREMIUM,
    AUTH_SESSION_QUERY_TRUSTED,
    AUTH_SESSION_QUERY_WARDROBES,
    AUTH_SESSION_QUERY_WARDROBE_FAVORITES,
    MAX_AUTH_SESSION_QUERY
};

class AuthSessionQueryHolder : public SQLQueryHolder
{
    private:
        uint32 m_AccountId;
        std::string m_IP;

    public:
        AuthSessionQueryHolder(uint32 p_AccountId, std::string p_IP) : m_AccountId(p_AccountId), m_IP(p_IP){ }

        bool Initialize()
        {
            SetSize(MAX_AUTH_SESSION_QUERY);

            bool l_Result = true;

            PreparedStatement* l_Statement = nullptr;

            l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_SEL_AUTH_SESSION_GMLEVEL);
            l_Statement->setUInt32(0, m_AccountId);
            l_Statement->setUInt32(1, g_RealmID);
            l_Result &= SetPreparedQuery(AUTH_SESSION_QUERY_GMLEVEL, l_Statement);

            l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_SEL_AUTH_SESSION_BANNED);
            l_Statement->setUInt32(0, m_AccountId);
            l_Statement->setString(1, m_IP);
            l_Result &= SetPreparedQuery(AUTH_SESSION_QUERY_BANNED, l_Statement);

            l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_SEL_AUTH_SESSION_PREMIUM);
            l_Statement->setUInt32(0, m_AccountId);
            l_Result &= SetPreparedQuery(AUTH_SESSION_QUERY_PREMIUM, l_Statement);

            l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_SEL_AUTH_SESSION_TRUSTED);
            l_Statement->setUInt32(0, m_AccountId);
            l_Result &= SetPreparedQuery(AUTH_SESSION_QUERY_TRUSTED, l_Statement);

            l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_WARDROBES);
            l_Statement->setUInt32(0, m_AccountId);
            l_Result &= SetPreparedQuery(AUTH_SESSION_QUERY_WARDROBES, l_Statement);

            l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_WARDROBE_FAVORITES);
            l_Statement->setUInt32(0, m_AccountId);
            l_Result &= SetPreparedQuery(AUTH_SESSION_QUERY_WARDROBE_FAVORITES, l_Statement);

            return l_Result;
        }
};

int WorldSocket::HandleAuthSession(WorldPacket& p_RecvPacket)
{
    uint64 l_DosResponse = 0;

    uint32 l_BattlegroupID              = 0;
    uint32 l_RegionID                   = 0;
    uint32 l_SiteID                     = 0;
    uint32 l_RealmID                    = 0;
    uint32 l_AccountNameLenght          = 0;

    uint16 l_ClientBuild = 0;

    uint8 l_ClientAuthChallenge[24];
    uint8 l_ClientSeed[16];

    uint8 l_LoginServerType = 0;
    uint8 l_BuildType       = 0;

    bool l_UseIpV6 = false;

    std::string l_AccountName;

    //////////////////////////////////////////////////////////////////////////
    p_RecvPacket >> l_DosResponse;                      ///< uint64
    p_RecvPacket >> l_ClientBuild;                      ///< uint16
    p_RecvPacket >> l_BuildType;                        ///< uint8

    p_RecvPacket >> l_RegionID;                         ///< uint32
    p_RecvPacket >> l_BattlegroupID;                    ///< uint32
    p_RecvPacket >> l_RealmID;                          ///< uint32

    p_RecvPacket.read(l_ClientSeed, 16);
    p_RecvPacket.read(l_ClientAuthChallenge, 24);

    l_UseIpV6 = p_RecvPacket.ReadBit();

    p_RecvPacket >> l_AccountNameLenght;
    l_AccountName = p_RecvPacket.ReadString(l_AccountNameLenght);

    //////////////////////////////////////////////////////////////////////////

    WorldPacket l_AuthResponsePacket;

    if (sWorld->IsClosed())
    {
        WorldSession::WriteAuthResponse(l_AuthResponsePacket, BattlenetRpcErrorCode::ERROR_DENIED, false, 0);
        SendPacket(l_AuthResponsePacket);
        sLog->outError(LOG_FILTER_NETWORKIO, "WorldSocket::HandleAuthSession: World closed, denying client (%s).", GetRemoteAddress().c_str());
        return -1;
    }

    std::string l_EscapedAccountName = l_AccountName;
    LoginDatabase.EscapeString(l_EscapedAccountName);

    PreparedStatement* l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_SEL_AUTH_SESSION_ACCOUNT);
    l_Statement->setString(0, l_EscapedAccountName);
    LoginDatabase.AsyncQuery(l_Statement, m_QueryCallbackMgr, [this, l_ClientSeed, l_ClientAuthChallenge, l_ClientBuild](PreparedQueryResult p_Result)->void
    {
        WorldPacket             l_AuthResponsePacket;
        LocaleConstant          l_AccountLocale;
        std::array<uint8, 64>   l_KeyData;

        /// Stop if the account is not found
        if (!p_Result)
        {
            WorldSession::WriteAuthResponse(l_AuthResponsePacket, BattlenetRpcErrorCode::ERROR_NO_ACCOUNT_REGISTERED, false, 0);
            SendPacket(l_AuthResponsePacket);
            sLog->outError(LOG_FILTER_NETWORKIO, "WorldSocket::HandleAuthSession: Sent Auth Response (unknown account).");
            DelayedCloseSocket();
            return;
        }

        Field * l_Fields = p_Result->Fetch();

        std::string l_AccountName = l_Fields[11].GetString();
        std::string l_EscapedAccountName = l_AccountName;
        LoginDatabase.EscapeString(l_EscapedAccountName);

        uint32 l_AccountID         = l_Fields[0].GetUInt32();
        uint32 l_AccountExpansion  = l_Fields[6].GetUInt8();
        uint32 l_ServerExpansion   = sWorld->getIntConfig(CONFIG_EXPANSION);
        uint64 l_JoinDateTimestamp = l_Fields[12].GetUInt64();
        uint32 l_ServiceFlags      = l_Fields[13].GetUInt32();
        uint32 l_CustomFlags       = l_Fields[14].GetUInt32();
        uint32 l_MacAddr           = l_Fields[15].GetUInt32();

        if (l_AccountExpansion > l_ServerExpansion)
            l_AccountExpansion = l_ServerExpansion;

        sLog->outDebug(LOG_FILTER_NETWORKIO, "WorldSocket::HandleAuthSession: (s,v) check s: %s v: %s", l_Fields[5].GetCString(), l_Fields[4].GetCString());

        /// Re-check ip locking (same check as in auth server).
        if (l_Fields[3].GetUInt8() == 1) // if ip is locked
        {
            if (strcmp(l_Fields[2].GetCString(), GetRemoteAddress().c_str()))
            {
                WorldSession::WriteAuthResponse(l_AuthResponsePacket, BattlenetRpcErrorCode::ERROR_GAME_ACCOUNT_LOCKED, false, 0);
                SendPacket(l_AuthResponsePacket);
                sLog->outDebug(LOG_FILTER_NETWORKIO, "WorldSocket::HandleAuthSession: Sent Auth Response (Account IP differs).");
                DelayedCloseSocket();
                return;
            }
        }

        int64  l_MuteTime  = l_Fields[7].GetInt64();
        HexStrToByteArray(l_Fields[1].GetString(), l_KeyData.data());

        /// Negative mute time indicates amount of seconds to be muted effective on next login - which is now.
        if (l_MuteTime < 0)
        {
            l_MuteTime = time(NULL) + llabs(l_MuteTime);

            PreparedStatement* l_Stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_MUTE_TIME);
            l_Stmt->setInt64(0, l_MuteTime);
            l_Stmt->setUInt32(1, l_AccountID);
            LoginDatabase.Execute(l_Stmt);
        }

        l_AccountLocale = LocaleConstant(l_Fields[8].GetUInt8());

        if (l_AccountLocale >= TOTAL_LOCALES)
            l_AccountLocale = LOCALE_enUS;

        sWorld->SetAccountLocale(l_AccountID, l_AccountLocale);

        uint32 l_Recruiter = l_Fields[9].GetUInt32();
        m_OS = l_Fields[10].GetString();

        /// Check that Key and account name are the same on client and server
        uint8 l_AuthCheckSeed[16] = { 0xC5, 0xC6, 0x98, 0x95, 0x76, 0x3F, 0x1D, 0xCD, 0xB6, 0xA1, 0x37, 0x28, 0xB3, 0x12, 0xFF, 0x8A };

        uint8 const ClientTypeSeed_Win[16] = { 0x2A, 0xAC, 0x82, 0xC8, 0x0E, 0x82, 0x9E, 0x2C, 0xA9, 0x02, 0xD7, 0x0C, 0xFA, 0x1A, 0x83, 0x3A };
        uint8 const ClientTypeSeed_Wn64[16] = { 0x59, 0xA5, 0x3F, 0x30, 0x72, 0x88, 0x45, 0x4B, 0x41, 0x9B, 0x13, 0xE6, 0x94, 0xDF, 0x50, 0x3C };
        uint8 const ClientTypeSeed_Mc64[16] = { 0xDB, 0xE7, 0xF8, 0x60, 0x27, 0x6D, 0x6B, 0x40, 0x0A, 0xAA, 0x86, 0xB3, 0x5D, 0x51, 0xA4, 0x17 };

        SHA256Hash l_DigestKeyHash;
        l_DigestKeyHash.UpdateData(l_KeyData.data(), l_KeyData.size());
        if (m_OS == "Win")
            l_DigestKeyHash.UpdateData(reinterpret_cast<uint8 const*>(ClientTypeSeed_Win), 16);
        else if (m_OS == "Wn64")
            l_DigestKeyHash.UpdateData(reinterpret_cast<uint8 const*>(ClientTypeSeed_Wn64), 16);
        else if (m_OS == "Mc64")
            l_DigestKeyHash.UpdateData(reinterpret_cast<uint8 const*>(ClientTypeSeed_Mc64), 16);

        l_DigestKeyHash.Finalize();

        HmacHash256 l_ServerAuthChallenge(l_DigestKeyHash.GetLength(), l_DigestKeyHash.GetDigest());
        l_ServerAuthChallenge.UpdateData(l_ClientSeed, 16);
        l_ServerAuthChallenge.UpdateData(m_Seed.AsByteArray(16), 16);
        l_ServerAuthChallenge.UpdateData(l_AuthCheckSeed, 16);
        l_ServerAuthChallenge.Finalize();

        std::string l_SessionIP = GetRemoteAddress();

        if (memcmp(l_ServerAuthChallenge.GetDigest(), l_ClientAuthChallenge, sizeof(l_ClientAuthChallenge)))
        {
            WorldSession::WriteAuthResponse(l_AuthResponsePacket, BattlenetRpcErrorCode::ERROR_SERVICE_FAILURE_AUTH, false, 0);
            SendPacket(l_AuthResponsePacket);
            sLog->outError(LOG_FILTER_NETWORKIO, "WorldSocket::HandleAuthSession: Authentication failed for account: %u ('%s') address: %s", l_AccountID, l_AccountName.c_str(), l_SessionIP.c_str());
            DelayedCloseSocket();
            return;
        }

        sLog->outDebug(LOG_FILTER_NETWORKIO, "WorldSocket::HandleAuthSession: Client '%s' authenticated successfully from %s.", l_AccountName.c_str(), l_SessionIP.c_str());

        bool l_AccountIsRecruiter = false;

        /// Update the last_ip in the database
        /// No SQL injection, username escaped.
        LoginDatabase.EscapeString(l_SessionIP);
        LoginDatabase.PExecute("UPDATE account SET last_ip = '%s' WHERE username = '%s'", l_SessionIP.c_str(), l_EscapedAccountName.c_str());

        AuthSessionQueryHolder* l_QueryHolder = new AuthSessionQueryHolder(l_AccountID, GetRemoteAddress());
        if (!l_QueryHolder->Initialize())
        {
            delete l_QueryHolder;
            DelayedCloseSocket();
            return;
        }

        QueryResultHolderFuture l_QueryHolderFuture = LoginDatabase.DelayQueryHolder(l_QueryHolder);

        m_QueryCallbackMgr->AddQueryHolderCallback(QueryHolderCallback(l_QueryHolderFuture, [=](SQLQueryHolder* p_QueryHolder) -> void
        {
            if (!p_QueryHolder)
                return;

            WorldPacket l_AuthResponsePacket;
            uint32 l_AccountGMLevel = 0;
            bool l_IsAnimator = false;
            if (auto l_Result = p_QueryHolder->GetPreparedResult(AUTH_SESSION_QUERY_GMLEVEL))
            {
                Field* l_Fields = l_Result->Fetch();
                l_AccountGMLevel = l_Fields[1].GetUInt8();
                l_IsAnimator = l_Fields[2].GetBool();
            }

            /// if account banned
            if (PreparedQueryResult l_BanResult = p_QueryHolder->GetPreparedResult(AUTH_SESSION_QUERY_BANNED))
            {
                WorldSession::WriteAuthResponse(l_AuthResponsePacket, BattlenetRpcErrorCode::ERROR_GAME_ACCOUNT_BANNED, false, 0);
                SendPacket(l_AuthResponsePacket);
                sLog->outError(LOG_FILTER_NETWORKIO, "WorldSocket::HandleAuthSession: Sent Auth Response (Account banned).");
                DelayedCloseSocket();
                return;
            }

            uint8 l_AccountPremiumType = 0;
            bool  l_AccountIsPremium = false;

            if (auto l_PremiumResult = p_QueryHolder->GetPreparedResult(AUTH_SESSION_QUERY_PREMIUM))
            {
                Field * l_PremiumFields = l_PremiumResult->Fetch();

                l_AccountIsPremium = true;
                l_AccountPremiumType = l_PremiumFields->GetUInt8();
            }

            /// Check locked state for server
            AccountTypes l_AllowedAccountType = sWorld->GetPlayerSecurityLimit();
            sLog->outDebug(LOG_FILTER_NETWORKIO, "Allowed Level: %u Player Level %u", l_AllowedAccountType, AccountTypes(l_AccountGMLevel));

            if (l_AllowedAccountType > SEC_PLAYER && AccountTypes(l_AccountGMLevel) < l_AllowedAccountType)
            {
                WorldSession::WriteAuthResponse(l_AuthResponsePacket, BattlenetRpcErrorCode::ERROR_GAME_ACCOUNT_LOCKED, false, 0);
                SendPacket(l_AuthResponsePacket);
                sLog->outInfo(LOG_FILTER_NETWORKIO, "WorldSocket::HandleAuthSession: User tries to login but his security level is not enough");
                DelayedCloseSocket();
                return;
            }

            m_Session = new WorldSession(l_AccountID, this, AccountTypes(l_AccountGMLevel), l_IsAnimator, l_AccountIsPremium, l_AccountPremiumType, l_AccountExpansion, l_MuteTime, l_AccountLocale, l_Recruiter, l_AccountIsRecruiter, l_ServiceFlags, l_CustomFlags, l_MacAddr);

            uint8 SessionKeySeed[16] = { 0x58, 0xCB, 0xCF, 0x40, 0xFE, 0x2E, 0xCE, 0xA6, 0x5A, 0x90, 0xB8, 0x01, 0x68, 0x6C, 0x28, 0x0B };

            SHA256Hash l_ShaKeyData;
            l_ShaKeyData.UpdateData(l_KeyData.data(), l_KeyData.size());
            l_ShaKeyData.Finalize();

            HmacHash256 l_SessionKeyHmac(l_ShaKeyData.GetLength(), l_ShaKeyData.GetDigest());
            l_SessionKeyHmac.UpdateData(m_Seed.AsByteArray(16), 16);
            l_SessionKeyHmac.UpdateData(l_ClientSeed, 16);
            l_SessionKeyHmac.UpdateData(SessionKeySeed, 16);
            l_SessionKeyHmac.Finalize();

            uint8 l_GeneratedKey[40];
            SHA1Randx sessionKeyGenerator(l_SessionKeyHmac.GetDigest(), l_SessionKeyHmac.GetLength());
            sessionKeyGenerator.Generate(l_GeneratedKey, 40);

            m_CryptKey = new BigNumber();
            m_CryptKey->SetBinary(l_GeneratedKey, 40);

            LoginDatabase.PExecute("UPDATE account SET sessionkey = '%s' WHERE id = '%u'", m_CryptKey->AsHexStr(), l_AccountID);

            m_Session->LoadLoyaltyData();
            m_Session->SetClientBuild(l_ClientBuild);
            m_Session->SetAccountJoinDate(l_JoinDateTimestamp);
            m_Session->GetCollectionMgr()->LoadAccountItemAppearances(p_QueryHolder->GetPreparedResult(AUTH_SESSION_QUERY_WARDROBES), p_QueryHolder->GetPreparedResult(AUTH_SESSION_QUERY_WARDROBE_FAVORITES));

            if (auto l_TrustedResult = p_QueryHolder->GetPreparedResult(AUTH_SESSION_QUERY_TRUSTED))
                m_Session->SetTrustedState();

            delete p_QueryHolder;

            PreparedStatement* l_Statement = SessionRealmDatabase.GetPreparedStatement(CHAR_SEL_ACCOUNT_DATA);
            l_Statement->setUInt32(0, l_AccountID);
            SessionRealmDatabase.AsyncQuery(l_Statement, m_QueryCallbackMgr, [this, l_AccountID](PreparedQueryResult p_Result)->void
            {
                if (m_Session)
                    m_Session->LoadAccountData(p_Result, GLOBAL_CACHE_MASK);

                PreparedStatement* l_Statement = SessionRealmDatabase.GetPreparedStatement(CHAR_SEL_TUTORIALS);
                l_Statement->setUInt32(0, l_AccountID);
                SessionRealmDatabase.AsyncQuery(l_Statement, m_QueryCallbackMgr, [this, l_AccountID](PreparedQueryResult p_Result)->void
                {
                    if (m_Session)
                        m_Session->LoadTutorialsData(p_Result);

                    SendCryptEnable();
                });
            });
        }));
    });

    return 0;
}

int WorldSocket::HandleCryptEnabled(WorldPacket& p_RecvPacket)
{
    if (m_Crypt.IsInitialized())
    {
        sLog->outAshran("WorldSocket::HandleCryptEnabled: m_Crypt.IsInitialized");
        return -1;
    }

    if (_type == ConnectionType::CONNECTION_TYPE_REALM)
    {
        if (!m_Session)
        {
            sLog->outAshran("WorldSocket::HandleCryptEnabled: m_Session == nullptr");
            return -1;
        }

        m_Crypt.Init(m_CryptKey);

        /// Initialize Warden system only if it is enabled by config
        if (sWorld->getBoolConfig(CONFIG_WARDEN_ENABLED))
            m_Session->InitWarden(m_CryptKey, m_OS);

        /// Sleep this Network thread for
        uint32 l_SleepTime = sWorld->getIntConfig(CONFIG_SESSION_ADD_DELAY);
        ACE_OS::sleep(ACE_Time_Value(0, l_SleepTime));

        sWorld->AddSession(m_Session);
    }
    else
    {
        m_Crypt.Init(m_CryptKey, _encryptSeed.AsByteArray(), _decryptSeed.AsByteArray());
        AddReference();
        sWorld->AddInstanceSocket(this, _key);
    }
    return 0;
}

void WorldSocket::HandleConnectToFailed(WorldPackets::Auth::ConnectToFailed& connectToFailed)
{
    if (m_Session)
    {
        if (m_Session->PlayerLoading())
        {
            switch (connectToFailed.Serial)
            {
                case WorldPackets::Auth::ConnectToSerial::WorldAttempt1:
                    m_Session->SendConnectToInstance(WorldPackets::Auth::ConnectToSerial::WorldAttempt2);
                    break;
                case WorldPackets::Auth::ConnectToSerial::WorldAttempt2:
                    m_Session->SendConnectToInstance(WorldPackets::Auth::ConnectToSerial::WorldAttempt3);
                    break;
                case WorldPackets::Auth::ConnectToSerial::WorldAttempt3:
                    m_Session->SendConnectToInstance(WorldPackets::Auth::ConnectToSerial::WorldAttempt4);
                    break;
                case WorldPackets::Auth::ConnectToSerial::WorldAttempt4:
                    m_Session->SendConnectToInstance(WorldPackets::Auth::ConnectToSerial::WorldAttempt5);
                    break;
                case WorldPackets::Auth::ConnectToSerial::WorldAttempt5:
                {
                    sLog->outTrace(LOG_FILTER_NETWORKIO, "%s failed to connect 5 times to world socket, aborting login", m_Session->GetPlayerName().c_str());
                    m_Session->AbortLogin(WorldPackets::Character::LoginFailureReason::NoWorld);
                    break;
                }
                default:
                    return;
            }
        }
    }
}

int WorldSocket::HandleAuthContinuedSession(WorldPacket& p_Packet)
{
    std::shared_ptr<WorldPackets::Auth::AuthContinuedSession> authSession = std::make_shared<WorldPackets::Auth::AuthContinuedSession>(p_Packet);
    authSession->Read();

    WorldSession::ConnectToKey key;
    _key = key.Raw = authSession->Key;

    _type = ConnectionType(key.Fields.ConnectionType);

    if (_type != CONNECTION_TYPE_INSTANCE)
    {
        WorldPacket l_AuthResponsePacket;
        WorldSession::WriteAuthResponse(l_AuthResponsePacket, BattlenetRpcErrorCode::ERROR_DENIED, false, 0);
        SendPacket(l_AuthResponsePacket);

        sLog->outError(LOG_FILTER_NETWORKIO, "WorldSocket::HandleAuthContinuedSession: Client %s !_type != CONNECTION_TYPE_INSTANCE", GetRemoteAddress().c_str());
        DelayedCloseSocket();
        return -1;
    }

    PreparedStatement* l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_SESSIONKEY);
    l_Statement->setUInt32(0, key.Fields.AccountId);
    LoginDatabase.AsyncQuery(l_Statement, m_QueryCallbackMgr, [this, key, authSession](PreparedQueryResult p_Result)
    {
        if (!p_Result)
        {
            sLog->outAshran("WorldSocket::HandleAuthContinuedSession: no result for account id %u", key.Fields.AccountId);
            DelayedCloseSocket();
            return;
        }

        Field * l_Fields = p_Result->Fetch();

        m_CryptKey = new BigNumber();
        m_CryptKey->SetHexStr(l_Fields[0].GetCString());

        HmacHash256 hmac(40, m_CryptKey->AsByteArray(40));
        hmac.UpdateData(reinterpret_cast<uint8 const*>(&authSession->Key), sizeof(authSession->Key));
        hmac.UpdateData(authSession->LocalChallenge.data(), authSession->LocalChallenge.size());
        hmac.UpdateData(m_Seed.AsByteArray(16), 16);
        hmac.UpdateData(ContinuedSessionSeed, 16);
        hmac.Finalize();

        if (memcmp(hmac.GetDigest(), authSession->Digest.data(), authSession->Digest.size()))
        {
            sLog->outError(LOG_FILTER_NETWORKIO, "WorldSocket::HandleAuthContinuedSession: Authentication failed for account: %u address: %s",
                uint32(key.Fields.AccountId), GetRemoteAddress().c_str());

            DelayedCloseSocket();
            return;
        }

        SendCryptEnable();
    });

    return 0;
}

void WorldSocket::SendCryptEnable()
{
    WorldPacket l_Data(SMSG_CRYPT_ENABLE, 0);
    SendPacket(l_Data);
}

int WorldSocket::HandlePing(WorldPacket& recvPacket)
{
    uint32 ping;
    uint32 latency;

    // Get the ping packet content
    recvPacket >> ping;
    recvPacket >> latency;

    if (m_LastPingTime == ACE_Time_Value::zero)
        m_LastPingTime = ACE_OS::gettimeofday(); // for 1st ping
    else
    {
        ACE_Time_Value cur_time = ACE_OS::gettimeofday();
        ACE_Time_Value diff_time(cur_time);
        diff_time -= m_LastPingTime;
        m_LastPingTime = cur_time;

        if (diff_time < ACE_Time_Value(27))
        {
            ++m_OverSpeedPings;

            uint32 max_count = sWorld->getIntConfig(CONFIG_MAX_OVERSPEED_PINGS);

            if (max_count && m_OverSpeedPings > max_count)
            {
                ACE_GUARD_RETURN(LockType, Guard, m_SessionLock, -1);

                if (m_Session && AccountMgr::IsPlayerAccount(m_Session->GetSecurity()))
                {
                    sLog->outError(LOG_FILTER_NETWORKIO, "WorldSocket::HandlePing: %s kicked for over-speed pings (address: %s)",
                        m_Session->GetPlayerName(false).c_str(), GetRemoteAddress().c_str());

                    return -1;
                }
            }
        }
        else
            m_OverSpeedPings = 0;
    }

    // critical section
    {
        ACE_GUARD_RETURN(LockType, Guard, m_SessionLock, -1);

        if (m_Session)
        {
            m_Session->SetLatency(latency);
            m_Session->ResetClientTimeDelay();
        }
        else
        {
            sLog->outError(LOG_FILTER_NETWORKIO, "WorldSocket::HandlePing: peer sent CMSG_PING, "
                "but is not authenticated or got recently kicked, "
                " address = %s",
                GetRemoteAddress().c_str());
            return -1;
        }
    }

    WorldPacket packet(SMSG_PONG, 4);
    packet << ping;
    return SendPacket(packet);
}

void WorldSocket::DelayedCloseSocket()
{
    m_DelayedCloseSocket = true;
}

void WorldSocket::SetWorldSession(WorldSession* session)
{
    ACE_GUARD(LockType, Guard, m_SessionLock);
    m_Session = session;
}

#endif
