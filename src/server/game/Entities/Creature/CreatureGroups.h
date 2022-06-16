////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _FORMATIONS_H
#define _FORMATIONS_H

#include "Define.h"
#include "Common.h"

class Creature;
class CreatureGroup;

struct FormationInfo
{
    uint32 leaderGUID;
    float follow_dist;
    float follow_angle;
    uint8 groupAI;
    uint16 point_1;
    uint16 point_2;
};

typedef std::unordered_map<uint32/*memberDBGUID*/, FormationInfo*>   CreatureGroupInfoType;

class FormationMgr
{
    friend class ACE_Singleton<FormationMgr, ACE_Null_Mutex>;
    public:
        FormationMgr() { }
        ~FormationMgr();
        void AddCreatureToGroup(uint32 group_id, Creature* creature);
        void RemoveCreatureFromGroup(CreatureGroup* group, Creature* creature);
        void LoadCreatureFormations();
        CreatureGroupInfoType CreatureGroupMap;
        FormationInfo* CreateCustomFormation(Creature* p_Creature);
        void AddMemberInLeaderGroup(Creature* p_Leader, Creature* p_Member, float p_FollowAngle, float p_FollowDistance);
        FormationInfo* GetDefaultLeaderFormation(Creature* p_Leader);

};

class CreatureGroup
{
    private:
        uint64 m_LeaderGuid;
        typedef std::map<uint64, FormationInfo*>  CreatureGroupMemberType;
        CreatureGroupMemberType m_members;

        uint32 m_groupID;
        bool m_Formed;

    public:
        // Group cannot be created empty
        explicit CreatureGroup(uint32 id) : m_LeaderGuid(0), m_groupID(id), m_Formed(false) {}
        ~CreatureGroup() {}

        Creature* getLeader() const { return sObjectAccessor->FindCreature(m_LeaderGuid); }
        uint32 GetId() const { return m_groupID; }
        bool isEmpty() const { return m_members.empty(); }
        bool isFormed() const { return m_Formed; }
        CreatureGroupMemberType GetMembers() const { return m_members; }

        void AddMember(Creature* member, FormationInfo* p_Formation = nullptr);
        void RemoveMember(Creature* member);
        void FormationReset(bool dismiss);

        void LeaderMoveTo(float x, float y, float z);
        void MemberAttackStart(Creature* member, Unit* target);
};

#define sFormationMgr ACE_Singleton<FormationMgr, ACE_Null_Mutex>::instance()

#endif
