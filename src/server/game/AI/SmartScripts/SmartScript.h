////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef TRINITY_SMARTSCRIPT_H
#define TRINITY_SMARTSCRIPT_H

#include "Common.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "Unit.h"
#include "ConditionMgr.h"
#include "Spell.h"
#include "GridNotifiers.h"

#include "SmartScriptMgr.h"

class SmartScript
{
    public:
        SmartScript();
        ~SmartScript();

        void OnInitialize(WorldObject* obj, AreaTriggerEntry const* at = nullptr, Quest const* p_Quest = nullptr);
        void GetScript();
        void FillScript(SmartAIEventList e, WorldObject* obj, AreaTriggerEntry const* at, Quest const* quest = nullptr);

        void ProcessEventsFor(SMART_EVENT e, Unit* unit = nullptr, uint32 var0 = 0, uint32 var1 = 0, bool bvar = false, const SpellInfo* spell = nullptr, GameObject* gob = nullptr);
        void ProcessEvent(SmartScriptHolder& e, Unit* unit = nullptr, uint32 var0 = 0, uint32 var1 = 0, bool bvar = false, const SpellInfo* spell = nullptr, GameObject* gob = nullptr);
        bool CheckTimer(SmartScriptHolder const& e) const;
        void RecalcTimer(SmartScriptHolder& e, uint32 min, uint32 max);
        void UpdateTimer(SmartScriptHolder& e, uint32 const diff);
        void InitTimer(SmartScriptHolder& e);
        void ProcessAction(SmartScriptHolder& e, Unit* unit = nullptr, uint32 var0 = 0, uint32 var1 = 0, bool bvar = false, const SpellInfo* spell = nullptr, GameObject* gob = nullptr);
        void ProcessTimedAction(SmartScriptHolder& e, uint32 const& min, uint32 const& max, Unit* unit = nullptr, uint32 var0 = 0, uint32 var1 = 0, bool bvar = false, const SpellInfo* spell = nullptr, GameObject* gob = nullptr);
        ObjectList* GetTargets(SmartScriptHolder const& e, Unit* invoker = nullptr);
        ObjectList* GetWorldObjectsInDist(float dist);
        void InstallTemplate(SmartScriptHolder const& e);
        SmartScriptHolder CreateEvent(SMART_EVENT e, uint32 event_flags, uint32 event_param1, uint32 event_param2, uint32 event_param3, uint32 event_param4, uint32 event_param5, SMART_ACTION action, uint32 action_param1, uint32 action_param2, uint32 action_param3, uint32 action_param4, uint32 action_param5, uint32 action_param6, SMARTAI_TARGETS t, uint32 target_param1, uint32 target_param2, uint32 target_param3, uint32 phaseMask = 0);
        void AddEvent(SMART_EVENT e, uint32 event_flags, uint32 event_param1, uint32 event_param2, uint32 event_param3, uint32 event_param4, uint32 event_param5, SMART_ACTION action, uint32 action_param1, uint32 action_param2, uint32 action_param3, uint32 action_param4, uint32 action_param5, uint32 action_param6, SMARTAI_TARGETS t, uint32 target_param1, uint32 target_param2, uint32 target_param3, uint32 phaseMask = 0);
        void SetPathId(uint32 id) { mPathId = id; }
        uint32 GetPathId() const { return mPathId; }
        WorldObject* GetBaseObject()
        {
            WorldObject* obj = nullptr;
            if (me)
                obj = me;
            else if (go)
                obj = go;
            else if (player)
                obj = player;
            else if (m_Event)
                obj = m_Event;
            return obj;
        }

        bool IsUnit(WorldObject* obj)
        {
            return obj && (obj->GetTypeId() == TYPEID_UNIT || obj->IsPlayer());
        }

        bool IsPlayer(WorldObject* obj)
        {
            return obj && obj->IsPlayer();
        }

        bool IsCreature(WorldObject* obj)
        {
            return obj && obj->GetTypeId() == TYPEID_UNIT;
        }

        bool IsGameObject(WorldObject* obj)
        {
            return obj && obj->GetTypeId() == TYPEID_GAMEOBJECT;
        }

        void OnUpdate(const uint32 diff);
        void OnMoveInLineOfSight(Unit* who);

        Unit* DoSelectLowestHpFriendly(float range, uint32 MinHPDiff);
        void DoFindFriendlyCC(std::list<Creature*>& _list, float range);
        void DoFindFriendlyMissingBuff(std::list<Creature*>& list, float range, uint32 spellid);
        Unit* DoFindClosestFriendlyInRange(float range, bool playerOnly);

        void StoreTargetList(ObjectList* targets, uint32 id)
        {
            if (!targets)
                return;

            if (mTargetStorage.find(id) != mTargetStorage.end())
                mTargetStorage[id].clear();

            for (WorldObject* l_Object : *targets)
                mTargetStorage[id].push_back(l_Object->GetGUID());
        }

        bool IsSmart(Creature* c = nullptr)
        {
            bool smart = true;
            if (c && c->GetAIName() != "SmartAI")
                smart = false;

            if (!me || me->GetAIName() != "SmartAI")
                smart = false;

            if (!smart)
                sLog->outError(LOG_FILTER_SQL, "SmartScript: Action target Creature(entry: %u) is not using SmartAI, action skipped to prevent crash.", c ? c->GetEntry() : (me ? me->GetEntry() : 0));

            return smart;
        }

        bool IsSmartGO(GameObject* g = nullptr)
        {
            bool smart = true;
            if (g && g->GetAIName() != "SmartGameObjectAI")
                smart = false;

            if (!go || go->GetAIName() != "SmartGameObjectAI")
                smart = false;
            if (!smart)
                sLog->outError(LOG_FILTER_SQL, "SmartScript: Action target GameObject(entry: %u) is not using SmartGameObjectAI, action skipped to prevent crash.", g ? g->GetEntry() : (go ? go->GetEntry() : 0));

            return smart;
        }

        ObjectGuidList* GetTargetList(uint32 id)
        {
            ObjectGuidListMap::iterator itr = mTargetStorage.find(id);
            if (itr != mTargetStorage.end())
                return &(*itr).second;

            return nullptr;
        }

        void StoreCounter(uint32 id, uint32 value, uint32 reset)
        {
            CounterMap::const_iterator itr = mCounterList.find(id);

            if (itr != mCounterList.end())
            {
                if (reset == 0)
                    value += GetCounterValue(id);

                mCounterList.erase(id);
            }

            mCounterList.insert(std::make_pair(id, value));
            ProcessEventsFor(SMART_EVENT_COUNTER_SET);
        }

        uint32 GetCounterId(uint32 id)
        {
            CounterMap::iterator itr = mCounterList.find(id);

            if (itr != mCounterList.end())
                return itr->first;

            return 0;
        }

        uint32 GetCounterValue(uint32 id)
        {
            CounterMap::iterator itr = mCounterList.find(id);

            if (itr != mCounterList.end())
                return itr->second;

            return 0;
        }

        GameObject* FindGameObjectNear(WorldObject* searchObject, uint32 guid) const
        {
            GameObject* gameObject = nullptr;

            CellCoord p(JadeCore::ComputeCellCoord(searchObject->GetPositionX(), searchObject->GetPositionY()));
            Cell cell(p);

            JadeCore::GameObjectWithDbGUIDCheck goCheck(*searchObject, guid);
            JadeCore::GameObjectSearcher<JadeCore::GameObjectWithDbGUIDCheck> checker(searchObject, gameObject, goCheck);

            TypeContainerVisitor<JadeCore::GameObjectSearcher<JadeCore::GameObjectWithDbGUIDCheck>, GridTypeMapContainer > objectChecker(checker);
            cell.Visit(p, objectChecker, *searchObject->GetMap(), *searchObject, searchObject->GetGridActivationRange());

            return gameObject;
        }

        Creature* FindCreatureNear(WorldObject* searchObject, uint32 guid) const
        {
            Creature* creature = nullptr;
            CellCoord p(JadeCore::ComputeCellCoord(searchObject->GetPositionX(), searchObject->GetPositionY()));
            Cell cell(p);

            JadeCore::CreatureWithDbGUIDCheck target_check(searchObject, guid);
            JadeCore::CreatureSearcher<JadeCore::CreatureWithDbGUIDCheck> checker(searchObject, creature, target_check);

            TypeContainerVisitor<JadeCore::CreatureSearcher <JadeCore::CreatureWithDbGUIDCheck>, GridTypeMapContainer > unit_checker(checker);
            cell.Visit(p, unit_checker, *searchObject->GetMap(), *searchObject, searchObject->GetGridActivationRange());

            return creature;
        }

        ObjectGuidListMap mTargetStorage;

        void OnReset();
        void ResetBaseObject()
        {
            if (meOrigGUID)
            {
                if (Creature* m = HashMapHolder<Creature>::Find(meOrigGUID))
                {
                    me = m;
                    go = nullptr;
                }
            }
            if (goOrigGUID)
            {
                if (GameObject* o = HashMapHolder<GameObject>::Find(goOrigGUID))
                {
                    me = nullptr;
                    go = o;
                }
            }
            goOrigGUID = 0;
            meOrigGUID = 0;
        }

        //TIMED_ACTIONLIST (script type 9 aka script9)
        void SetScript9(SmartScriptHolder& e, uint32 entry);
        Unit* GetLastInvoker();
        uint64 mLastInvoker;
        typedef std::unordered_map<uint32, uint32> CounterMap;
        CounterMap mCounterList;

    private:
        void IncPhase(int32 p = 1)
        {
            if (p >= 0)
                mEventPhase += (uint32)p;
            else
                DecPhase(abs(p));
        }

        void DecPhase(int32 p = 1) { mEventPhase  -= (mEventPhase < (uint32)p ? (uint32)p - mEventPhase : (uint32)p); }
        bool IsInPhase(uint32 p) const { return (1 << (mEventPhase - 1)) & p; }
        void SetPhase(uint32 p = 0) { mEventPhase = p; }

        SmartAIEventList mEvents;
        SmartAIEventList mInstallEvents;
        SmartAIEventList mTimedActionList;
        Creature* me{};
        uint64 meOrigGUID;
        GameObject* go{};
        uint64 goOrigGUID;
        EventObject* m_Event{};
        Player* player{};
        AreaTriggerEntry const* trigger{};
        Quest const* quest{};
        SmartScriptType mScriptType;
        uint32 mEventPhase{};

        std::unordered_map<int32, int32> mStoredDecimals;
        uint32 mPathId{};
        SmartAIEventList mStoredEvents;
        std::list<uint32>mRemIDs;

        uint32 mTextTimer{};
        uint32 mLastTextID{};
        uint64 mTextGUID{};
        uint32 mTalkerEntry{};
        bool mUseTextTimer{};
        bool isProcessingTimedActionList{};

        SMARTAI_TEMPLATE mTemplate;
        void InstallEvents();

        void RemoveStoredEvent (uint32 id)
        {
            if (!mStoredEvents.empty())
            {
                for (SmartAIEventList::iterator i = mStoredEvents.begin(); i != mStoredEvents.end(); ++i)
                {
                    if (i->event_id == id)
                    {
                        mStoredEvents.erase(i);
                        return;
                    }
                }
            }
        }
        SmartScriptHolder FindLinkedEvent (uint32 link)
        {
            if (!mEvents.empty())
            {
                for (SmartAIEventList::iterator i = mEvents.begin(); i != mEvents.end(); ++i)
                {
                    if (i->event_id == link)
                    {
                        return (*i);
                    }
                }
            }
            SmartScriptHolder s;
            return s;
        }
};

#endif