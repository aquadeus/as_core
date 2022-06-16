////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "Creature.h"
#include "CreatureGroups.h"
#include "ObjectMgr.h"

#include "CreatureAI.h"

#define MAX_DESYNC 5.0f

FormationMgr::~FormationMgr()
{
    for (CreatureGroupInfoType::iterator itr = CreatureGroupMap.begin(); itr != CreatureGroupMap.end(); ++itr)
        delete itr->second;
}

void FormationMgr::AddCreatureToGroup(uint32 groupId, Creature* member)
{
    Map* map = member->GetMap();
    if (!map)
        return;

    CreatureGroupHolderType::iterator itr = map->CreatureGroupHolder.find(groupId);

    // Add member to an existing group
    if (itr != map->CreatureGroupHolder.end())
    {
        sLog->outDebug(LOG_FILTER_UNITS, "Group found: %u, inserting creature GUID: %u, Group InstanceID %u", groupId, member->GetGUIDLow(), member->GetInstanceId());
        itr->second->AddMember(member);
    }
    // Create new group
    else
    {
        sLog->outDebug(LOG_FILTER_UNITS, "Group not found: %u. Creating new group.", groupId);
        CreatureGroup* group = new CreatureGroup(groupId);
        map->CreatureGroupHolder[groupId] = group;
        group->AddMember(member);
    }
}

void FormationMgr::RemoveCreatureFromGroup(CreatureGroup* group, Creature* member)
{
    sLog->outDebug(LOG_FILTER_UNITS, "Deleting member pointer to GUID: %u from group %u", group->GetId(), member->GetDBTableGUIDLow());
    group->RemoveMember(member);

    if (group->isEmpty())
    {
        Map* map = member->GetMap();
        if (!map)
            return;

        sLog->outDebug(LOG_FILTER_UNITS, "Deleting group with InstanceID %u", member->GetInstanceId());
        map->CreatureGroupHolder.erase(group->GetId());
        delete group;
    }
}

void FormationMgr::LoadCreatureFormations()
{
    uint32 oldMSTime = getMSTime();

    for (CreatureGroupInfoType::iterator itr = CreatureGroupMap.begin(); itr != CreatureGroupMap.end(); ++itr) // for reload case
        delete itr->second;
    CreatureGroupMap.clear();

    // Get group data                                   0              1            2               3         4      5      6           7       8
    QueryResult result = WorldDatabase.Query("SELECT leaderGuid, memberGuid, leaderLinkedId, memberLinkedId, dist, angle, groupAI, point_1, point_2 FROM creature_formations ORDER BY leaderLinkedId");

    if (!result)
    {
        sLog->outError(LOG_FILTER_SERVER_LOADING, ">>  Loaded 0 creatures in formations. DB table `creature_formations` is empty!");
        return;
    }

    uint32 count = 0;
    Field* fields;
    FormationInfo* group_member;

    do
    {
        fields = result->Fetch();

        // Load group member data
        group_member                        = new FormationInfo();
        uint32 leaderGuid                   = fields[0].GetUInt32();
        uint32 memberGuid                   = fields[1].GetUInt32();
        std::string leaderLinkedId          = fields[2].GetString();
        group_member->leaderGUID            = leaderGuid ? leaderGuid : sObjectMgr->GetGUIDByLinkedId(leaderLinkedId);
        std::string memberLinkedId          = fields[3].GetString();
        uint32 memberGUID                   = memberGuid ? memberGuid : sObjectMgr->GetGUIDByLinkedId(memberLinkedId);
        group_member->groupAI               = fields[6].GetUInt32();
        group_member->point_1               = fields[7].GetUInt16();
        group_member->point_2               = fields[8].GetUInt16();
        // If creature is group leader we may skip loading of dist/angle
        if (group_member->leaderGUID != memberGUID)
        {
            group_member->follow_dist       = fields[4].GetFloat();
            group_member->follow_angle      = fields[5].GetFloat() * M_PI / 180;
        }
        else
        {
            group_member->follow_dist       = 0;
            group_member->follow_angle      = 0;
        }

        // Check data correctness
        {
            if (!sObjectMgr->GetCreatureData(group_member->leaderGUID))
            {
                sLog->outError(LOG_FILTER_SQL, "creature_formations table leader guid %u incorrect (not exist)", group_member->leaderGUID);
                delete group_member;
                continue;
            }

            if (!sObjectMgr->GetCreatureData(memberGUID))
            {
                sLog->outError(LOG_FILTER_SQL, "creature_formations table member guid %u incorrect (not exist)", memberGUID);
                delete group_member;
                continue;
            }
        }

        CreatureGroupMap[memberGUID] = group_member;
        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u creatures in formations in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

FormationInfo* FormationMgr::CreateCustomFormation(Creature* p_Creature)
{
    FormationInfo* group_member         = new FormationInfo();
    group_member->leaderGUID            = p_Creature->GetGUIDLow();
    group_member->groupAI               = 0;
    group_member->follow_dist           = 1.0f;
    group_member->follow_angle          = 45 * M_PI / 180;
    sFormationMgr->AddCreatureToGroup(p_Creature->GetGUIDLow(), p_Creature);
    if (CreatureGroup* f = p_Creature->GetFormation())
        f->AddMember(p_Creature, group_member);
    return group_member;
}

void FormationMgr::AddMemberInLeaderGroup(Creature* p_Leader, Creature* p_Member, float p_FollowAngle, float p_FollowDistance)
{
    if (!p_Leader->GetFormation())
    {
        sFormationMgr->AddCreatureToGroup(p_Leader->GetGUIDLow(), p_Leader);
    }

    if (CreatureGroup* l_CreatureGroup = p_Leader->GetFormation())
    {
        FormationInfo* l_FormationInfo = new FormationInfo();
        l_FormationInfo->leaderGUID            = p_Leader->GetGUIDLow();
        l_FormationInfo->groupAI               = 0;
        l_FormationInfo->follow_dist           = p_FollowDistance;
        l_FormationInfo->follow_angle          = p_FollowAngle * M_PI / 180;

        l_CreatureGroup->AddMember(p_Member, l_FormationInfo);
    }
}

FormationInfo* FormationMgr::GetDefaultLeaderFormation(Creature* p_Leader)
{
    FormationInfo* l_FormationInfo = new FormationInfo();
    l_FormationInfo->leaderGUID            = p_Leader->GetGUIDLow();
    l_FormationInfo->groupAI               = 0;
    l_FormationInfo->follow_dist           = 0.0f;
    l_FormationInfo->follow_angle          = 0.0f;
    return l_FormationInfo;
}

void CreatureGroup::AddMember(Creature* member, FormationInfo* p_Formation /*= nullptr*/)
{
    sLog->outDebug(LOG_FILTER_UNITS, "CreatureGroup::AddMember: Adding unit GUID: %u.", member->GetGUIDLow());

    // Check if it is a leader
    if (member->GetDBTableGUIDLow() == m_groupID || (member->ToTempSummon() && member->GetGUIDLow() == m_groupID))
    {
        sLog->outDebug(LOG_FILTER_UNITS, "Unit GUID: %u is formation leader. Adding group.", member->GetGUIDLow());
        m_LeaderGuid = member->GetGUID();
    }

    CreatureGroupInfoType::iterator itr = sFormationMgr->CreatureGroupMap.find(member->GetDBTableGUIDLow());
    if (itr != sFormationMgr->CreatureGroupMap.end())
        m_members[member->GetGUID()] = itr->second;
    else if (p_Formation != nullptr)
        m_members[member->GetGUID()] = p_Formation;
    else if (p_Formation == nullptr && member->ToTempSummon() && member->GetGUIDLow() == m_groupID)
        m_members[member->GetGUID()] = sFormationMgr->GetDefaultLeaderFormation(member);

    member->SetFormation(this);
}

void CreatureGroup::RemoveMember(Creature* member)
{
    if (m_LeaderGuid == member->GetGUID())
        m_LeaderGuid = 0;

    if (m_members.find(member->GetGUID()) != m_members.end())
        m_members.erase(member->GetGUID());

    member->SetFormation(NULL);
}

void CreatureGroup::MemberAttackStart(Creature* member, Unit* target)
{
    uint8 groupAI = 0;
    if (member->GetDBTableGUIDLow())
    {
        CreatureGroupInfoType::iterator itr = sFormationMgr->CreatureGroupMap.find(member->GetDBTableGUIDLow());
        if (itr != sFormationMgr->CreatureGroupMap.end())
            groupAI = itr->second->groupAI;
        else
            groupAI = 2;
    }
    else
        groupAI = 2;

    if (groupAI == 1 && member->GetGUID() != m_LeaderGuid)
        return;

    CreatureGroupMemberType l_Members = m_members;
    for (CreatureGroupMemberType::iterator itr = l_Members.begin(); itr != l_Members.end(); ++itr)
    {
        if (Creature* l_Leader = sObjectAccessor->FindCreature(m_LeaderGuid))
            sLog->outDebug(LOG_FILTER_UNITS, "GROUP ATTACK: group instance id %u calls member instid %u", l_Leader->GetInstanceId(), member->GetInstanceId());

        // Skip one check
        if (itr->first == member->GetGUID())
            continue;

        Creature* l_CreatureIter = sObjectAccessor->FindCreature(itr->first);
        if (l_CreatureIter == nullptr)
            continue;

        if (!l_CreatureIter->isAlive())
            continue;

        if (l_CreatureIter->getVictim())
            continue;

        if (l_CreatureIter->IsValidAttackTarget(target) && l_CreatureIter->IsAIEnabled)
            l_CreatureIter->AI()->AttackStart(target);
    }
}

void CreatureGroup::FormationReset(bool dismiss)
{
    for (CreatureGroupMemberType::iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
    {
        Creature* l_CreatureIter = sObjectAccessor->FindCreature(itr->first);
        if (l_CreatureIter == nullptr)
            continue;

        if (itr->first != m_LeaderGuid && l_CreatureIter->isAlive())
        {
            if (dismiss)
                l_CreatureIter->GetMotionMaster()->Initialize();
            else
                l_CreatureIter->GetMotionMaster()->MoveIdle();

            sLog->outDebug(LOG_FILTER_UNITS, "Set %s movement for member GUID: %u", dismiss ? "default" : "idle", l_CreatureIter->GetGUIDLow());
        }
    }

    m_Formed = !dismiss;
}

void CreatureGroup::LeaderMoveTo(float x, float y, float z)
{
    // @TODO : This should probably get its own movement generator or use WaypointMovementGenerator.
    // If the leader's path is known, member's path can be plotted as well using formation offsets.
    if (!m_LeaderGuid)
        return;

    Creature* l_Leader = sObjectAccessor->FindCreature(m_LeaderGuid);
    if (l_Leader == nullptr)
        return;

    float pathangle = std::atan2(l_Leader->GetPositionY() - y, l_Leader->GetPositionX() - x);

    for (CreatureGroupMemberType::iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
    {
        Creature* member = sObjectAccessor->FindCreature(itr->first);
        if (member == l_Leader || !member->isAlive() || member->getVictim())
            continue;

        if (itr->second->point_1)
        {
            if (l_Leader->GetCurrentWaypointID() == itr->second->point_1)
                itr->second->follow_angle = itr->second->follow_angle + M_PI;
            if (l_Leader->GetCurrentWaypointID() == itr->second->point_2 &&
                l_Leader->GetCurrentWaypointID() != l_Leader->GetPrevWaypointID())
                itr->second->follow_angle = itr->second->follow_angle - M_PI;
        }
        float angle = itr->second->follow_angle;
        float dist = itr->second->follow_dist;

        float dx = x +  std::cos(angle + pathangle) * dist;
        float dy = y +  std::sin(angle + pathangle) * dist;
        float dz = z;

        JadeCore::NormalizeMapCoord(dx);
        JadeCore::NormalizeMapCoord(dy);

        if (!member->IsFlying())
            member->UpdateGroundPositionZ(dx, dy, dz);

        if (member->IsWithinDist(l_Leader, dist + MAX_DESYNC))
            member->SetUnitMovementFlags(l_Leader->GetUnitMovementFlags());
        else
            member->SetWalk(false);

        member->GetMotionMaster()->MovePoint(0, dx, dy, dz);
        member->SetHomePosition(dx, dy, dz, pathangle);
    }
}
