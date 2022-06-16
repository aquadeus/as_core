////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/*
 * Interaction between core and LFGScripts
 */

#include "Common.h"
#include "SharedDefines.h"
#include "ScriptMgr.h"

class Player;
class Group;
using GroupPtr = std::shared_ptr<Group>;

class LFGPlayerScript : public PlayerScript
{
    public:
        LFGPlayerScript();

        /// Player Hooks
        void OnLevelChanged(Player* p_Player, uint8 p_OldLevel);
        void OnLogout(Player* p_Player);
        void OnLogin(Player* p_Player);
        void OnBindToInstance(Player* p_Player, Difficulty p_Difficulty, uint32 p_MapID, bool p_Permanent, uint8 p_ExtendState);
};

class LFGGroupScript : public GroupScript
{
    public:
        LFGGroupScript();

        /// Group Hooks
        void OnAddMember(GroupPtr p_Group, uint64 p_Guid);
        void OnRemoveMember(GroupPtr p_Group, uint64 p_Guid, RemoveMethod p_Method, uint64 p_KickerGuid, char const* p_Reason);
        void OnDisband(GroupPtr p_Group);
        void OnChangeLeader(GroupPtr p_Group, uint64 p_NewLeaderGuid, uint64 p_OldLeaderGuid);
        void OnInviteMember(GroupPtr p_Group, uint64 p_Guid);
};
