////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _GROUPMGR_H
#define _GROUPMGR_H

#include "Group.h"

class GroupMgr
{
    friend class ACE_Singleton<GroupMgr, ACE_Null_Mutex>;
private:
    GroupMgr();
    ~GroupMgr();

public:
    typedef ACE_Based::LockedMap<uint32, GroupPtr> GroupContainer;
    typedef std::vector<GroupPtr>      GroupDbContainer;

    GroupPtr GetGroupByGUID(uint32 guid) const;

    uint32 GenerateNewGroupDbStoreId();
    void   RegisterGroupDbStoreId(uint32 storageId, GroupPtr group);
    void   FreeGroupDbStoreId(GroupPtr group);
    void   SetNextGroupDbStoreId(uint32 storageId) { NextGroupDbStoreId = storageId; };
    GroupPtr GetGroupByDbStoreId(uint32 storageId) const;
    void   SetGroupDbStoreSize(uint32 newSize) { GroupDbStore.resize(newSize); }

#ifndef CROSS
    void   LoadGroups();
#endif /* not CROSS */
    uint32 GenerateGroupId();
    void   AddGroup(GroupPtr group);
    void   RemoveGroup(GroupPtr group);


protected:
    uint32           NextGroupId;
    uint32           NextGroupDbStoreId;
    GroupContainer   GroupStore;
    GroupDbContainer GroupDbStore;
};

#define sGroupMgr ACE_Singleton<GroupMgr, ACE_Null_Mutex>::instance()

#endif
