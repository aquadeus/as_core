////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef TRINITY_GRIDNOTIFIERS_H
#define TRINITY_GRIDNOTIFIERS_H

#include "ObjectGridLoader.h"
#include "UpdateData.h"
#include <iostream>

#include "Corpse.h"
#include "Object.h"
#include "AreaTrigger.h"
#include "DynamicObject.h"
#include "GameObject.h"
#include "Player.h"
#include "Unit.h"
#include "CreatureAI.h"
#include "GameObjectAI.h"
#include "Spell.h"
#include "MapManager.h"

class Player;
//class Map;

namespace JadeCore
{
    struct VisibleNotifier
    {
        Player &i_player;
        std::set<Unit*> i_visibleNow;
        GuidUnorderedSet vis_guids;
        UpdateData& i_data;

        VisibleNotifier(Player &player, UpdateData& data) : i_player(player), vis_guids(player.m_clientGUIDs), i_data(data) { }
        template<class T> void Visit(GridRefManager<T> &m);
        void Visit(EventObjectMapType &m);
        void SendToSelf(void);
    };

    struct VisibleChangesNotifier
    {
        WorldObject &i_object;

        explicit VisibleChangesNotifier(WorldObject &object) : i_object(object) {}
        template<class T> void Visit(GridRefManager<T> &) {}
        void Visit(PlayerMapType &);
        void Visit(CreatureMapType &);
        void Visit(DynamicObjectMapType &);
        void Visit(ConversationMapType &);
    };

    struct AIRelocationNotifier
    {
        Unit &i_unit;
        bool isCreature;
        explicit AIRelocationNotifier(Unit &unit) : i_unit(unit), isCreature(unit.GetTypeId() == TYPEID_UNIT)  {}

        template<class T> void Visit(GridRefManager<T> &) {}
        void Visit(CreatureMapType &);
        void Visit(EventObjectMapType &);
        void Visit(GameObjectMapType &);
    };

    struct GridUpdater
    {
        GridType &i_grid;
        uint32 i_timeDiff;
        GridUpdater(GridType &grid, uint32 diff) : i_grid(grid), i_timeDiff(diff) {}

        template<class T> void updateObjects(GridRefManager<T> &m)
        {
            for (typename GridRefManager<T>::iterator iter = m.begin(); iter != m.end(); ++iter)
                iter->getSource()->Update(i_timeDiff);
        }

        void Visit(CreatureMapType &m)
        {
            for (GridRefManager<Creature>::iterator iter = m.begin(); iter != m.end(); ++iter)
                iter->getSource()->Update(i_timeDiff);
        }

        void Visit(GameObjectMapType &m)
        {
            for (GridRefManager<GameObject>::iterator iter = m.begin(); iter != m.end(); ++iter)
            {
                if (!iter->getSource()->IsTransport())
                    iter->getSource()->Update(i_timeDiff);
            }
        }

        void Visit(PlayerMapType &m)        { updateObjects<Player>(m); }
        void Visit(DynamicObjectMapType &m) { updateObjects<DynamicObject>(m); }
        void Visit(CorpseMapType &m)        { updateObjects<Corpse>(m); }
        void Visit(AreaTriggerMapType &m)   { updateObjects<AreaTrigger>(m); }
        void Visit(ConversationMapType &m) { updateObjects<Conversation>(m); }
    };

    struct AnyReceiverInRange
    {
        WorldObject*               Source;
        float                      RangeSQ;
        std::vector<WorldObject*>* Receivers;

        AnyReceiverInRange(WorldObject* p_Source, float p_Range, std::vector<WorldObject*>& p_Receivers)
        {
            Source    = p_Source;
            RangeSQ   = p_Range * p_Range;
            Receivers = &p_Receivers;
        }

        void Visit(PlayerMapType& p_Players);
        void Visit(CreatureMapType& p_Creatures);
        void Visit(DynamicObjectMapType& p_DynamicObjects);
        template<class SKIP> void Visit(GridRefManager<SKIP>&) {}
    };

    struct MessageDistDeliverer
    {
        WorldObject*       m_Source;
        WorldPacket const* m_Message;
        float              m_RangeSQ;

        MessageDistDeliverer(WorldObject* p_Source, WorldPacket const* p_Message, float p_Range)
        {
            m_Source   = p_Source;
            m_Message  = p_Message;
            m_RangeSQ  = p_Range * p_Range;
        }

        void Visit(PlayerMapType& p_Players);
        void Visit(CreatureMapType& p_Creatures);
        void Visit(DynamicObjectMapType& p_DynamicObjects);
        template<class SKIP> void Visit(GridRefManager<SKIP>&) {}

        void SendPacket(Player* p_Target)
        {
            /// never send packet to self
            if (p_Target == m_Source)
                return;

            WorldSession* l_Session = p_Target->GetSession();
            if (!l_Session)
                return;

            switch (m_Message->GetOpcode())
            {
                case SMSG_SPELL_NON_MELEE_DAMAGE_LOG:
                    break;
                case SMSG_MOVE_UPDATE:
                    if (m_Source == p_Target->m_mover)
                        return;
                    /// no break on purpose
                default:
                {
                    if (!p_Target->HaveAtClient(m_Source))
                        return;

                    break;
                }
            }

            l_Session->SendPacket(m_Message);
        }
    };

    struct ObjectUpdater
    {
        uint32 i_timeDiff;
        explicit ObjectUpdater(const uint32 diff) : i_timeDiff(diff) {}
        template<class T> void Visit(GridRefManager<T> &m);
        void Visit(PlayerMapType &) {}
        void Visit(CorpseMapType &) {}
        void Visit(CreatureMapType &);
        void Visit(GameObjectMapType &);
    };

    // SEARCHERS & LIST SEARCHERS & WORKERS

    // WorldObject searchers & workers

    template<class Check>
    struct WorldObjectSearcher
    {
        uint32 i_mapTypeMask;
        WorldObject const* m_Searcher;
        WorldObject* &i_object;
        Check &i_check;

        WorldObjectSearcher(WorldObject const* searcher, WorldObject* & result, Check& check, uint32 mapTypeMask = GRID_MAP_TYPE_MASK_ALL)
            : i_mapTypeMask(mapTypeMask), m_Searcher(searcher), i_object(result), i_check(check) {}

        void Visit(GameObjectMapType &m);
        void Visit(PlayerMapType &m);
        void Visit(CreatureMapType &m);
        void Visit(CorpseMapType &m);
        void Visit(DynamicObjectMapType &m);
        void Visit(AreaTriggerMapType &m);
        void Visit(ConversationMapType &m);
        void Visit(EventObjectMapType &m);

        template<class NOT_INTERESTED> void Visit(GridRefManager<NOT_INTERESTED> &) {}
    };

    template<class Check>
    struct WorldObjectLastSearcher
    {
        uint32 i_mapTypeMask;
        WorldObject const* m_Searcher;
        WorldObject* &i_object;
        Check &i_check;
        std::set<WorldObject*> i_visited;

        WorldObjectLastSearcher(WorldObject const* searcher, WorldObject* & result, Check& check, uint32 mapTypeMask = GRID_MAP_TYPE_MASK_ALL)
            :  i_mapTypeMask(mapTypeMask), m_Searcher(searcher), i_object(result), i_check(check) {}

        void Visit(GameObjectMapType &m);
        void Visit(PlayerMapType &m);
        void Visit(CreatureMapType &m);
        void Visit(CorpseMapType &m);
        void Visit(DynamicObjectMapType &m);
        void Visit(AreaTriggerMapType &m);
        void Visit(ConversationMapType &m);
        void Visit(EventObjectMapType &m);

        void VisitWithNearbies(GameObjectMapType &m);
        void VisitWithNearbies(PlayerMapType &m);
        void VisitWithNearbies(CreatureMapType &m);
        void VisitWithNearbies(CorpseMapType &m);
        void VisitWithNearbies(DynamicObjectMapType &m);
        void VisitWithNearbies(AreaTriggerMapType &m);
        void VisitWithNearbies(ConversationMapType &m);
        void VisitWithNearbies(EventObjectMapType &m);

        template<class NOT_INTERESTED> void Visit(GridRefManager<NOT_INTERESTED> &) {}
        template<class NOT_INTERESTED> void VisitWithNearbies(GridRefManager<NOT_INTERESTED> &) {}
    };

    template<class Check>
    struct WorldObjectListSearcher
    {
        uint32 i_mapTypeMask;
        WorldObject const* m_Searcher;
        std::list<WorldObject*> &i_objects;
        Check& i_check;
        std::set<WorldObject*> i_visited;

        WorldObjectListSearcher(WorldObject const* searcher, std::list<WorldObject*> &objects, Check & check, uint32 mapTypeMask = GRID_MAP_TYPE_MASK_ALL)
            : i_mapTypeMask(mapTypeMask), m_Searcher(searcher), i_objects(objects), i_check(check) {}

        void Visit(PlayerMapType &m);
        void Visit(CreatureMapType &m);
        void Visit(CorpseMapType &m);
        void Visit(GameObjectMapType &m);
        void Visit(DynamicObjectMapType &m);
        void Visit(AreaTriggerMapType &m);
        void Visit(ConversationMapType &m);
        void Visit(EventObjectMapType &m);

        void VisitWithNearbies(GameObjectMapType &m);
        void VisitWithNearbies(PlayerMapType &m);
        void VisitWithNearbies(CreatureMapType &m);
        void VisitWithNearbies(CorpseMapType &m);
        void VisitWithNearbies(DynamicObjectMapType &m);
        void VisitWithNearbies(AreaTriggerMapType &m);
        void VisitWithNearbies(ConversationMapType &m);
        void VisitWithNearbies(EventObjectMapType &m);

        template<class NOT_INTERESTED> void Visit(GridRefManager<NOT_INTERESTED> &) {}
        template<class NOT_INTERESTED> void VisitWithNearbies(GridRefManager<NOT_INTERESTED> &) {}
    };

    template<class Do>
    struct WorldObjectWorker
    {
        uint32 i_mapTypeMask;
        WorldObject const* m_Searcher;
        Do const& i_do;

        WorldObjectWorker(WorldObject const* searcher, Do const& _do, uint32 mapTypeMask = GRID_MAP_TYPE_MASK_ALL)
            : i_mapTypeMask(mapTypeMask), m_Searcher(searcher), i_do(_do) {}

        void Visit(GameObjectMapType &m)
        {
            if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_GAMEOBJECT))
                return;
            for (GameObjectMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
                if (itr->getSource()->InSamePhase(m_Searcher))
                    i_do(itr->getSource());
        }

        void Visit(PlayerMapType &m)
        {
            if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_PLAYER))
                return;
            for (PlayerMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
                if (itr->getSource()->InSamePhase(m_Searcher))
                    i_do(itr->getSource());
        }
        void Visit(CreatureMapType &m)
        {
            if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_CREATURE))
                return;
            for (CreatureMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
                if (itr->getSource()->InSamePhase(m_Searcher))
                    i_do(itr->getSource());
        }

        void Visit(CorpseMapType &m)
        {
            if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_CORPSE))
                return;
            for (CorpseMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
                if (itr->getSource()->InSamePhase(m_Searcher))
                    i_do(itr->getSource());
        }

        void Visit(DynamicObjectMapType &m)
        {
            if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_DYNAMICOBJECT))
                return;
            for (DynamicObjectMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
                if (itr->getSource()->InSamePhase(m_Searcher))
                    i_do(itr->getSource());
        }

        void Visit(AreaTriggerMapType &m)
        {
            if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_AREATRIGGER))
                return;
            for (AreaTriggerMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
                if (itr->getSource()->InSamePhase(m_Searcher))
                    i_do(itr->getSource());
        }

        void Visit(ConversationMapType &m)
        {
            if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_CONVERSATION))
                return;

            for (ConversationMapType::iterator l_Iter = m.begin(); l_Iter != m.end(); ++l_Iter)
            {
                if (l_Iter->getSource()->InSamePhase(m_Searcher))
                    i_do(l_Iter->getSource());
            }
        }

        void Visit(EventObjectMapType &m)
        {
            if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_EVENTOBJECT))
                return;

            for (EventObjectMapType::iterator l_Iter = m.begin(); l_Iter != m.end(); ++l_Iter)
            {
                if (l_Iter->getSource()->InSamePhase(m_Searcher))
                    i_do(l_Iter->getSource());
            }
        }

        template<class NOT_INTERESTED> void Visit(GridRefManager<NOT_INTERESTED> &) {}
    };

    /// AreaTriggers searchers
    template<class Check>
    struct AreaTriggerListSearcher
    {
        WorldObject const* m_Searcher;
        std::list<AreaTrigger*> &m_AreaTriggers;
        Check& m_Check;

        AreaTriggerListSearcher(WorldObject const* p_Searcher, std::list<AreaTrigger*>& p_AreaTriggers, Check& p_Check)
            : m_Searcher(p_Searcher), m_AreaTriggers(p_AreaTriggers), m_Check(p_Check) {}

        void Visit(AreaTriggerMapType& p_AreaTriggerMap);

        template<class NOT_INTERESTED> void Visit(GridRefManager<NOT_INTERESTED> &) {}
    };

    template<class Check>
    struct AreaTriggerSearcher
    {
        WorldObject const* m_Searcher;
        AreaTrigger* &i_object;
        Check & i_check;

        AreaTriggerSearcher(WorldObject const* searcher, AreaTrigger* & result, Check & check)
            : m_Searcher(searcher), i_object(result), i_check(check) {}

        void Visit(AreaTriggerMapType &m);

        template<class NOT_INTERESTED> void Visit(GridRefManager<NOT_INTERESTED> &) {}
    };

    /// Conversation searchers
    template<class Check>
    struct ConversationSearcher
    {
        WorldObject const* m_Searcher;
        Conversation* &i_object;
        Check & i_check;

        ConversationSearcher(WorldObject const* searcher, Conversation* & result, Check & check)
            : m_Searcher(searcher), i_object(result), i_check(check) { }

        void Visit(ConversationMapType& p_ConversationMap);

        template<class NOT_INTERESTED> void Visit(GridRefManager<NOT_INTERESTED> &) { }
    };

    template<class Check>
    struct ConversationListSearcher
    {
        WorldObject const* m_Searcher;
        std::list<Conversation*>& m_Conversations;
        Check& m_Check;

        ConversationListSearcher(WorldObject const* p_Searcher, std::list<Conversation*>& p_Conversation, Check& p_Check)
            : m_Searcher(p_Searcher), m_Conversations(p_Conversation), m_Check(p_Check) { }

        void Visit(ConversationMapType& p_ConversationMap);

        template<class NOT_INTERESTED> void Visit(GridRefManager<NOT_INTERESTED> &) { }
    };

    /// Gameobject searchers
    template<class Check>
    struct GameObjectSearcher
    {
        WorldObject const* m_Searcher;
        GameObject* &i_object;
        Check &i_check;

        GameObjectSearcher(WorldObject const* searcher, GameObject* & result, Check& check)
            : m_Searcher(searcher), i_object(result), i_check(check) {}

        void Visit(GameObjectMapType &m);

        template<class NOT_INTERESTED> void Visit(GridRefManager<NOT_INTERESTED> &) {}
    };

    // Last accepted by Check GO if any (Check can change requirements at each call)
    template<class Check>
    struct GameObjectLastSearcher
    {
        WorldObject const* m_Searcher;
        GameObject* &i_object;
        Check& i_check;

        GameObjectLastSearcher(WorldObject const* searcher, GameObject* & result, Check& check)
            : m_Searcher(searcher), i_object(result), i_check(check) {}

        void Visit(GameObjectMapType &m);

        template<class NOT_INTERESTED> void Visit(GridRefManager<NOT_INTERESTED> &) {}
    };

    template<class Check>
    struct GameObjectListSearcher
    {
        WorldObject const* m_Searcher;
        std::list<GameObject*> &i_objects;
        Check& i_check;

        GameObjectListSearcher(WorldObject const* searcher, std::list<GameObject*> &objects, Check & check)
            : m_Searcher(searcher), i_objects(objects), i_check(check) {}

        void Visit(GameObjectMapType &m);

        template<class NOT_INTERESTED> void Visit(GridRefManager<NOT_INTERESTED> &) {}
    };

    template<class Functor>
    struct GameObjectWorker
    {
        GameObjectWorker(WorldObject const* searcher, Functor& func)
            : _func(func), m_Searcher(searcher) {}

        void Visit(GameObjectMapType& m)
        {
            for (GameObjectMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
                if (itr->getSource()->InSamePhase(m_Searcher))
                    _func(itr->getSource());
        }

        template<class NOT_INTERESTED> void Visit(GridRefManager<NOT_INTERESTED> &) {}

    private:
        Functor& _func;
        WorldObject const* m_Searcher;
    };

    // Unit searchers

    // First accepted by Check Unit if any
    template<class Check>
    struct UnitSearcher
    {
        WorldObject const* m_Searcher;
        Unit* &i_object;
        Check & i_check;

        UnitSearcher(WorldObject const* searcher, Unit* & result, Check & check)
            : m_Searcher(searcher), i_object(result), i_check(check) {}

        void Visit(CreatureMapType &m);
        void Visit(PlayerMapType &m);

        template<class NOT_INTERESTED> void Visit(GridRefManager<NOT_INTERESTED> &) {}
    };

    // Last accepted by Check Unit if any (Check can change requirements at each call)
    template<class Check>
    struct UnitLastSearcher
    {
        WorldObject const* m_Searcher;
        Unit* &i_object;
        Check & i_check;

        UnitLastSearcher(WorldObject const* searcher, Unit* & result, Check & check)
            : m_Searcher(searcher), i_object(result), i_check(check) {}

        void Visit(CreatureMapType &m);
        void Visit(PlayerMapType &m);

        template<class NOT_INTERESTED> void Visit(GridRefManager<NOT_INTERESTED> &) {}
    };

    // All accepted by Check units if any
    template<class Check>
    struct UnitListSearcher
    {
        WorldObject const* m_Searcher;
        std::list<Unit*> &i_objects;
        Check& i_check;

        UnitListSearcher(WorldObject const* searcher, std::list<Unit*> &objects, Check & check)
            : m_Searcher(searcher), i_objects(objects), i_check(check)
        {
            if (!IS_AREATRIGGER(m_Searcher->GetGUID()))
                return;

            if (Unit::IsAlreadyReportedAreatrigger(m_Searcher->GetEntry()))
                return;

            uint32 l_Entry = m_Searcher->GetEntry();

            if (AreaTrigger const* l_AT = m_Searcher->ToAreaTrigger())
            {
                if (!l_Entry)
                    l_Entry = l_AT->GetAreaTriggerInfo().customEntry;
            }

            ///sLog->outExtChat("#jarvis-spells", "danger", false, "Areatrigger (entry : %u) has malformed searcher", l_Entry);
            ///Unit::AddReportedAreatrigger(m_Searcher->GetEntry());
        }

        void Visit(PlayerMapType &m);
        void Visit(CreatureMapType &m);

        template<class NOT_INTERESTED> void Visit(GridRefManager<NOT_INTERESTED> &) {}
    };

    // Creature searchers

    template<class Check>
    struct CreatureSearcher
    {
        WorldObject const* m_Searcher;
        Creature* &i_object;
        Check & i_check;

        CreatureSearcher(WorldObject const* searcher, Creature* & result, Check & check)
            : m_Searcher(searcher), i_object(result), i_check(check) {}

        void Visit(CreatureMapType &m);

        template<class NOT_INTERESTED> void Visit(GridRefManager<NOT_INTERESTED> &) {}
    };

    // Last accepted by Check Creature if any (Check can change requirements at each call)
    template<class Check>
    struct CreatureLastSearcher
    {
        WorldObject const* m_Searcher;
        Creature* &i_object;
        Check & i_check;

        CreatureLastSearcher(WorldObject const* searcher, Creature* & result, Check & check)
            : m_Searcher(searcher), i_object(result), i_check(check) {}

        void Visit(CreatureMapType &m);

        template<class NOT_INTERESTED> void Visit(GridRefManager<NOT_INTERESTED> &) {}
    };

    template<class Check>
    struct CreatureListSearcher
    {
        WorldObject const* m_Searcher;
        std::list<Creature*> &i_objects;
        Check& i_check;

        CreatureListSearcher(WorldObject const* searcher, std::list<Creature*> &objects, Check & check)
            : m_Searcher(searcher), i_objects(objects), i_check(check) {}

        void Visit(CreatureMapType &m);

        template<class NOT_INTERESTED> void Visit(GridRefManager<NOT_INTERESTED> &) {}
    };

    template<class Do>
    struct CreatureWorker
    {
        WorldObject const* m_Searcher;
        Do& i_do;

        CreatureWorker(WorldObject const* searcher, Do& _do)
            : m_Searcher(searcher), i_do(_do) {}

        void Visit(CreatureMapType &m)
        {
            for (CreatureMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
                if (itr->getSource()->InSamePhase(m_Searcher))
                    i_do(itr->getSource());
        }

        template<class NOT_INTERESTED> void Visit(GridRefManager<NOT_INTERESTED> &) {}
    };

    // Player searchers

    template<class Check>
    struct PlayerSearcher
    {
        WorldObject const* m_Searcher;
        Player* &i_object;
        Check & i_check;

        PlayerSearcher(WorldObject const* searcher, Player* & result, Check & check)
            : m_Searcher(searcher), i_object(result), i_check(check) {}

        void Visit(PlayerMapType &m);

        template<class NOT_INTERESTED> void Visit(GridRefManager<NOT_INTERESTED> &) {}
    };

    template<class Check>
    struct PlayerListSearcher
    {
        WorldObject const* m_Searcher;
        std::list<Player*> &i_objects;
        Check& i_check;

        PlayerListSearcher(WorldObject const* searcher, std::list<Player*> &objects, Check & check)
            : m_Searcher(searcher), i_objects(objects), i_check(check) {}

        void Visit(PlayerMapType &m);

        template<class NOT_INTERESTED> void Visit(GridRefManager<NOT_INTERESTED> &) {}
    };

    template<class Check>
    struct PlayerLastSearcher
    {
        WorldObject const* m_Searcher;
        Player* &i_object;
        Check& i_check;

        PlayerLastSearcher(WorldObject const* searcher, Player*& result, Check& check) : m_Searcher(searcher), i_object(result), i_check(check)
        {
        }

        void Visit(PlayerMapType& m);

        template<class NOT_INTERESTED> void Visit(GridRefManager<NOT_INTERESTED> &) {}
    };

    template<class Do>
    struct PlayerDistWorker
    {
        WorldObject const* i_searcher;
        float i_dist;
        Do& i_do;

        PlayerDistWorker(WorldObject const* searcher, float _dist, Do& _do)
            : i_searcher(searcher), i_dist(_dist), i_do(_do) {}

        void Visit(PlayerMapType &m)
        {
            for (PlayerMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
                if (itr->getSource()->InSamePhase(i_searcher) && itr->getSource()->IsWithinDist(i_searcher, i_dist))
                    i_do(itr->getSource());
        }

        template<class NOT_INTERESTED> void Visit(GridRefManager<NOT_INTERESTED> &) {}
    };

    template<class Do>
    struct PlayerChatDistWorker
    {
        WorldObject const* i_searcher;
        float i_dist;
        Do& i_do;

        PlayerChatDistWorker(WorldObject const* searcher, float _dist, Do& _do)
            : i_searcher(searcher), i_dist(_dist), i_do(_do) {}

        void Visit(PlayerMapType &m)
        {
            for (PlayerMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
                if (itr->getSource()->canSeeOrDetect(i_searcher) && itr->getSource()->IsWithinDist(i_searcher, i_dist))
                    i_do(itr->getSource());
        }

        template<class NOT_INTERESTED> void Visit(GridRefManager<NOT_INTERESTED> &) {}
    };

    // CHECKS && DO classes

    // WorldObject check classes

    class AnyDeadUnitObjectInRangeCheck
    {
        public:
            AnyDeadUnitObjectInRangeCheck(Unit* searchObj, float range) : i_searchObj(searchObj), i_range(range) {}
            bool operator()(Player* u);
            bool operator()(Corpse* u);
            bool operator()(Creature* u);
            template<class NOT_INTERESTED> bool operator()(NOT_INTERESTED*) { return false; }
        protected:
            Unit const* const i_searchObj;
            float i_range;
    };

    class AnyDeadUnitSpellTargetInRangeCheck : public AnyDeadUnitObjectInRangeCheck
    {
        public:
            AnyDeadUnitSpellTargetInRangeCheck(Unit* searchObj, float range, SpellInfo const* spellInfo, SpellTargetCheckTypes check)
                : AnyDeadUnitObjectInRangeCheck(searchObj, range), i_spellInfo(spellInfo), i_check(searchObj, searchObj, spellInfo, check, NULL)
            {}
            bool operator()(Player* u);
            bool operator()(Corpse* u);
            bool operator()(Creature* u);
            template<class NOT_INTERESTED> bool operator()(NOT_INTERESTED*) { return false; }
        protected:
            SpellInfo const* i_spellInfo;
            WorldObjectSpellTargetCheck i_check;
    };

    // WorldObject do classes

    class RespawnDo
    {
        public:
            RespawnDo() {}
            void operator()(Creature* u) const { u->Respawn(); }
            void operator()(GameObject* u) const { u->Respawn(); }
            void operator()(WorldObject*) const {}
            void operator()(Corpse*) const {}
    };

    // GameObject checks

    class GameObjectFocusCheck
    {
        public:
            GameObjectFocusCheck(Unit const* unit, uint32 focusId) : i_unit(unit), i_focusId(focusId) {}
            bool operator()(GameObject* go) const
            {
                if (go->GetGOInfo()->type != GAMEOBJECT_TYPE_SPELL_FOCUS)
                    return false;

                if (go->GetGOInfo()->spellFocus.spellFocusType != i_focusId)
                    return false;

                float dist = (float)((go->GetGOInfo()->spellFocus.radius)/2);

                return go->IsWithinDistInMap(i_unit, dist);
            }
        private:
            Unit const* i_unit;
            uint32 i_focusId;
    };

    // Find the nearest Fishing hole and return true only if source object is in range of hole
    class NearestGameObjectFishingHole
    {
        public:
            NearestGameObjectFishingHole(WorldObject const& obj, float range) : i_obj(obj), i_range(range) {}
            bool operator()(GameObject* go)
            {
                if (go->GetGOInfo()->type == GAMEOBJECT_TYPE_FISHINGHOLE && go->isSpawned() && i_obj.IsWithinDistInMap(go, i_range) && i_obj.IsWithinDistInMap(go, (float)go->GetGOInfo()->fishingHole.radius))
                {
                    i_range = i_obj.GetDistance(go);
                    return true;
                }
                return false;
            }
            float GetLastRange() const { return i_range; }
        private:
            WorldObject const& i_obj;
            float  i_range;

            // prevent clone
            NearestGameObjectFishingHole(NearestGameObjectFishingHole const&);
    };

    class NearestGameObjectCheck
    {
        public:
            NearestGameObjectCheck(WorldObject const& obj) : i_obj(obj), i_range(999) {}
            bool operator()(GameObject* go)
            {
                if (i_obj.IsWithinDistInMap(go, i_range))
                {
                    i_range = i_obj.GetDistance(go);        // use found GO range as new range limit for next check
                    return true;
                }
                return false;
            }
            float GetLastRange() const { return i_range; }
        private:
            WorldObject const& i_obj;
            float i_range;

            // prevent clone this object
            NearestGameObjectCheck(NearestGameObjectCheck const&);
    };

    // Success at unit in range, range update for next check (this can be use with GameobjectLastSearcher to find nearest GO)
    class NearestGameObjectEntryInObjectRangeCheck
    {
        public:
            NearestGameObjectEntryInObjectRangeCheck(WorldObject const& obj, uint32 entry, float range) : i_obj(obj), i_entry(entry), i_range(range) {}
            bool operator()(GameObject* go)
            {
                if (go->GetEntry() == i_entry && i_obj.IsWithinDistInMap(go, i_range))
                {
                    i_range = i_obj.GetDistance(go);        // use found GO range as new range limit for next check
                    return true;
                }
                return false;
            }
            float GetLastRange() const { return i_range; }
        private:
            WorldObject const& i_obj;
            uint32 i_entry;
            float  i_range;

            // prevent clone this object
            NearestGameObjectEntryInObjectRangeCheck(NearestGameObjectEntryInObjectRangeCheck const&);
    };

    /// Success at unit in range, range update for next check (this can be use with GameobjectLastSearcher to find nearest GO)
    class NearestGameObjectInObjectRangeCheck
    {
        public:
            NearestGameObjectInObjectRangeCheck(WorldObject const& p_Object, float p_Range) : m_Object(p_Object), m_Range(p_Range) { }

            bool operator()(GameObject* p_GameObject)
            {
                if (m_Object.IsWithinDistInMap(p_GameObject, m_Range))
                {
                    m_Range = m_Object.GetDistance(p_GameObject);        // use found GO range as new range limit for next check
                    return true;
                }
                return false;
            }

            float GetLastRange() const { return m_Range; }

        private:
            WorldObject const& m_Object;
            float  m_Range;

            // prevent clone this object
            NearestGameObjectInObjectRangeCheck(NearestGameObjectInObjectRangeCheck const&);
    };

    // Success at unit in range, range update for next check (this can be use with GameobjectLastSearcher to find nearest GO with a certain type)
    class NearestGameObjectTypeInObjectRangeCheck
    {
    public:
        NearestGameObjectTypeInObjectRangeCheck(WorldObject const& obj, GameobjectTypes type, float range) : i_obj(obj), i_type(type), i_range(range) {}
        bool operator()(GameObject* go)
        {
            if (go->GetGoType() == i_type && i_obj.IsWithinDistInMap(go, i_range))
            {
                i_range = i_obj.GetDistance(go);        // use found GO range as new range limit for next check
                return true;
            }
            return false;
        }
        float GetLastRange() const { return i_range; }
    private:
        WorldObject const& i_obj;
        GameobjectTypes i_type;
        float  i_range;

        // prevent clone this object
        NearestGameObjectTypeInObjectRangeCheck(NearestGameObjectTypeInObjectRangeCheck const&);

    };

    class GameObjectWithDbGUIDCheck
    {
        public:
            GameObjectWithDbGUIDCheck(WorldObject const& /*obj*/, uint32 db_guid) : i_db_guid(db_guid) {}
            bool operator()(GameObject const* go) const
            {
                return go->GetDBTableGUIDLow() == i_db_guid;
            }
        private:
            uint32 i_db_guid;
    };

    // Unit checks

    class MostHPMissingInRange
    {
        public:
            MostHPMissingInRange(Unit const* obj, float range, uint32 hp) : i_obj(obj), i_range(range), i_hp(hp) {}
            bool operator()(Unit* u)
            {
                if (u->isAlive() && u->isInCombat() && !i_obj->IsHostileTo(u) && i_obj->IsWithinDistInMap(u, i_range) && u->GetMaxHealth() - u->GetHealth() > i_hp)
                {
                    i_hp = u->GetMaxHealth() - u->GetHealth();
                    return true;
                }
                return false;
            }
        private:
            Unit const* i_obj;
            float i_range;
            uint32 i_hp;
    };

    class FriendlyCCedInRange
    {
        public:
            FriendlyCCedInRange(Unit const* obj, float range) : i_obj(obj), i_range(range) {}
            bool operator()(Unit* u)
            {
                if (u->isAlive() && u->isInCombat() && !i_obj->IsHostileTo(u) && i_obj->IsWithinDistInMap(u, i_range) &&
                    (u->isFeared() || u->isCharmed() || u->isFrozen() || u->HasUnitState(UNIT_STATE_STUNNED) || u->HasUnitState(UNIT_STATE_CONFUSED)))
                {
                    return true;
                }
                return false;
            }
        private:
            Unit const* i_obj;
            float i_range;
    };

    class FriendlyMissingBuffInRange
    {
        public:
            FriendlyMissingBuffInRange(Unit const* obj, float range, uint32 spellid) : i_obj(obj), i_range(range), i_spell(spellid) {}
            bool operator()(Unit* u)
            {
                if (u->isAlive() && u->isInCombat() && !i_obj->IsHostileTo(u) && i_obj->IsWithinDistInMap(u, i_range) &&
                    !(u->HasAura(i_spell)))
                {
                    return true;
                }
                return false;
            }
        private:
            Unit const* i_obj;
            float i_range;
            uint32 i_spell;
    };

    class AnyUnfriendlyUnitInObjectRangeCheck
    {
        public:
            AnyUnfriendlyUnitInObjectRangeCheck(WorldObject const* obj, Unit const* funit, float range) : i_obj(obj), i_funit(funit), i_range(range) {}
            bool operator()(Unit* u)
            {
                if (u->isAlive() && i_obj->IsWithinDistInMap(u, i_range) && !i_funit->IsFriendlyTo(u))
                    return true;
                else
                    return false;
            }
        private:
            WorldObject const* i_obj;
            Unit const* i_funit;
            float i_range;
    };

    class AnyUnfriendlyNoTotemUnitInObjectRangeCheck
    {
        public:
            AnyUnfriendlyNoTotemUnitInObjectRangeCheck(WorldObject const* obj, Unit const* funit, float range) : i_obj(obj), i_funit(funit), i_range(range) {}
            bool operator()(Unit* u)
            {
                if (!u->isAlive())
                    return false;

                if (u->GetCreatureType() == CREATURE_TYPE_NON_COMBAT_PET)
                    return false;

                if (u->GetTypeId() == TYPEID_UNIT && ((Creature*)u)->isTotem())
                    return false;

                if (!u->isTargetableForAttack(false))
                    return false;

                return i_obj->IsWithinDistInMap(u, i_range) && i_funit->IsValidAttackTarget(u);
            }
        private:
            WorldObject const* i_obj;
            Unit const* i_funit;
            float i_range;
    };

    class AnyUnfriendlyNoTotemUnitInMeleeRangeCheck
    {
        public:
            AnyUnfriendlyNoTotemUnitInMeleeRangeCheck(Unit const* p_Caster) : m_Caster(p_Caster) {}
            bool operator()(Unit* u)
            {
                if (!u->isAlive())
                    return false;

                if (u->GetCreatureType() == CREATURE_TYPE_NON_COMBAT_PET)
                    return false;

                if (u->GetTypeId() == TYPEID_UNIT && ((Creature*)u)->isTotem())
                    return false;

                if (!u->isTargetableForAttack(false))
                    return false;

                return m_Caster->IsWithinMeleeRange(u) && m_Caster->IsValidAttackTarget(u);
            }
        private:
            Unit const* m_Caster;
    };

    class AnyUnfriendlyAttackableVisibleUnitInObjectRangeCheck
    {
        public:
            AnyUnfriendlyAttackableVisibleUnitInObjectRangeCheck(Unit const* funit, float range)
                : i_funit(funit), i_range(range) {}

            bool operator()(const Unit* u)
            {
                return u->isAlive()
                    && i_funit->IsWithinDistInMap(u, i_range)
                    && !i_funit->IsFriendlyTo(u)
                    && i_funit->IsValidAttackTarget(u)
                    && u->GetCreatureType() != CREATURE_TYPE_CRITTER
                    && i_funit->canSeeOrDetect(u);
            }
        private:
            Unit const* i_funit;
            float i_range;
    };

    class CreatureWithDbGUIDCheck
    {
        public:
            CreatureWithDbGUIDCheck(WorldObject const* /*obj*/, uint32 lowguid) : i_lowguid(lowguid) {}
            bool operator()(Creature* u)
            {
                return u->GetDBTableGUIDLow() == i_lowguid;
            }
        private:
            uint32 i_lowguid;
    };

    class AnyFriendlyUnitInObjectRangeCheck
    {
        public:
            AnyFriendlyUnitInObjectRangeCheck(WorldObject const* obj, Unit const* funit, float range) : i_obj(obj), i_funit(funit), i_range(range) {}
            bool operator()(Unit* u)
            {
                if (u->isAlive() && i_obj->IsWithinDistInMap(u, i_range) && i_funit->IsFriendlyTo(u))
                    return true;
                else
                    return false;
            }
        private:
            WorldObject const* i_obj;
            Unit const* i_funit;
            float i_range;
    };

    class AnyFriendlyUnitInObjectRangeCheckTC
    {
        public:
            AnyFriendlyUnitInObjectRangeCheckTC(WorldObject const* obj, Unit const* funit, float range, bool playerOnly = false) : i_obj(obj), i_funit(funit), i_range(range), i_playerOnly(playerOnly) { }
            bool operator()(Unit* u)
            {
                if (u->isAlive() && i_obj->IsWithinDistInMap(u, i_range) && i_funit->IsFriendlyTo(u) && (!i_playerOnly || u->GetTypeId() == TYPEID_PLAYER))
                    return true;
                else
                    return false;
            }
        private:
            WorldObject const* i_obj;
            Unit const* i_funit;
            float i_range;
            bool i_playerOnly;
    };

    class AnyUnitHavingBuffInObjectRangeCheck
    {
        public:
            AnyUnitHavingBuffInObjectRangeCheck(WorldObject const* obj, Unit const* funit, float range, uint32 spellid, bool isfriendly)
                : i_obj(obj), i_funit(funit), i_range(range), i_friendly(isfriendly), i_spellid(spellid) {}
            bool operator()(Unit* u)
            {
                if (u->isAlive() && i_obj->IsWithinDistInMap(u, i_range) && i_funit->IsFriendlyTo(u) == i_friendly && u->HasAura(i_spellid, i_obj->GetGUID()))
                    return true;
                else
                    return false;
            }
        private:
            WorldObject const* i_obj;
            Unit const* i_funit;
            float i_range;
            bool i_friendly;
            uint32 i_spellid;
    };

    class AnyGroupedPlayerInObjectRangeCheck
    {
        public:
            AnyGroupedPlayerInObjectRangeCheck(WorldObject const* obj, Unit const* funit, float range, bool raid) : _source(obj), _refUnit(funit), _range(range), _raid(raid) {}
            bool operator()(Unit* u)
            {
                if (!u->ToPlayer())
                    return false;

                if (G3D::fuzzyEq(_range, 0))
                    return false;

                if (_raid)
                {
                    if (!_refUnit->IsInRaidWith(u))
                        return false;
                }
                else if (!_refUnit->IsInPartyWith(u))
                    return false;

                return !_refUnit->IsHostileTo(u) && u->isAlive() && _source->IsWithinDistInMap(u, _range);
            }

        private:
            WorldObject const* _source;
            Unit const* _refUnit;
            float _range;
            bool _raid;
    };

    class AnyGroupedUnitInObjectRangeCheck
    {
        public:
            AnyGroupedUnitInObjectRangeCheck(WorldObject const* obj, Unit const* funit, float range, bool raid) : _source(obj), _refUnit(funit), _range(range), _raid(raid) {}
            bool operator()(Unit* u)
            {
                if (G3D::fuzzyEq(_range, 0))
                    return false;

                if (_raid)
                {
                    if (!_refUnit->IsInRaidWith(u))
                        return false;
                }
                else if (!_refUnit->IsInPartyWith(u))
                    return false;

                return !_refUnit->IsHostileTo(u) && u->isAlive() && _source->IsWithinDistInMap(u, _range);
            }

        private:
            WorldObject const* _source;
            Unit const* _refUnit;
            float _range;
            bool _raid;
    };

    class AnyUnitInObjectRangeCheck
    {
        public:
            AnyUnitInObjectRangeCheck(WorldObject const* obj, float range, bool p_ObjectSize = true) : i_obj(obj), i_range(range), m_ObjectSize(p_ObjectSize) { }
            bool operator()(Unit* u)
            {
                if (!u->isAlive())
                    return false;

                if (m_ObjectSize && i_obj->IsWithinDistInMap(u, i_range))
                    return true;
                else if (!m_ObjectSize && i_obj->GetDistance(*u) <= i_range)
                    return true;

                return false;
            }
        private:
            WorldObject const* i_obj;
            float i_range;
            bool m_ObjectSize;
    };

    class AnyUnitInPositionRangeCheck
    {
        public:
            AnyUnitInPositionRangeCheck(Position const p_Position, WorldObject const* p_WorldObject, float p_Range, bool p_ObjectSize = true, bool p_AliveCheck = true) : m_Position(p_Position), m_WorldObject(p_WorldObject), m_Range(p_Range), m_ObjectSize(p_ObjectSize), m_AliveCheck(p_AliveCheck) { }
            bool operator()(Unit* u)
            {
                if (m_AliveCheck && !u->isAlive())
                    return false;

                if (!m_WorldObject->IsInMap(u) || !m_WorldObject->InSamePhase(u))
                    return false;

                if (m_ObjectSize && u->IsWithinDist3d(&m_Position, m_Range, m_WorldObject, false, true))
                    return true;
                else if (!m_ObjectSize)
                    return u->GetExactDist(&m_Position) < m_Range;

                return false;
            }
        private:
            WorldObject const* m_WorldObject;
            Position const m_Position;
            float m_Range;
            bool m_ObjectSize;
            bool m_AliveCheck;
    };

    class IdenticOverlappingAreatrigger
    {
        public:
            IdenticOverlappingAreatrigger(AreaTrigger const* p_AreaTrigger) :
                m_AreaTrigger(p_AreaTrigger) {}
            bool operator() (AreaTrigger* p_AreaTrigger)
            {
                return (p_AreaTrigger != m_AreaTrigger) && (p_AreaTrigger->GetSpellId() == m_AreaTrigger->GetSpellId()) && p_AreaTrigger->GetEntry() == m_AreaTrigger->GetEntry()
                    && (p_AreaTrigger->InSamePhase(m_AreaTrigger))
                    && (p_AreaTrigger->GetExactDist2d(m_AreaTrigger) < ((p_AreaTrigger->GetRadius() + m_AreaTrigger->GetRadius()) * 2.0f));
            }

        private:
            AreaTrigger const* m_AreaTrigger;
    };

    class NearestAttackableUnitInPositionRangeCheck
    {
        public:
            NearestAttackableUnitInPositionRangeCheck(WorldObject const* obj, Unit const* funit, float range) : i_obj(obj), i_funit(funit), i_range(range) {}
            bool operator()(Unit* u)
            {
                if (i_funit->IsValidAttackTarget(u) && i_obj->IsInMap(u) && i_funit->InSamePhase(u) && i_obj->IsWithinDist(u, i_range))
                {
                    i_range = i_obj->GetDistance(u);        // use found unit range as new range limit for next check
                    return true;
                }

                return false;
            }
        private:
            WorldObject const* i_obj;
            Unit const* i_funit;
            float i_range;

            // prevent clone this object
            NearestAttackableUnitInPositionRangeCheck(NearestAttackableUnitInPositionRangeCheck const&);
    };

    class AnyPlayerInPositionRangeCheck
    {
        public:
            AnyPlayerInPositionRangeCheck(WorldObject const* obj, WorldObject const* p_Caster, float range, bool reqAlive = true, bool p_Self = false) : _obj(obj), _caster(p_Caster), _range(range), _reqAlive(reqAlive), m_Self(p_Self) { }
            bool operator()(Player* u)
            {
                if (_reqAlive && !u->isAlive())
                    return false;

                if (!(_obj && _obj->IsInMap(u) && _caster->InSamePhase(u) && _obj->IsWithinDist(u, _range)))
                    return false;

                /// Don't add self
                if (!m_Self && _obj->GetTypeId() == TypeID::TYPEID_PLAYER && _obj->GetGUID() == u->GetGUID())
                    return false;

                return true;
            }

        private:
            WorldObject const* _obj;
            WorldObject const* _caster;
            float _range;
            bool _reqAlive;
            bool m_Self;
    };

    class AnyAreatriggerInObjectRangeCheck
    {
        public:
            AnyAreatriggerInObjectRangeCheck(WorldObject const* p_Object, float p_Range) : m_Object(p_Object), m_Range(p_Range) {}
            bool operator()(AreaTrigger* p_AreaTrigger)
            {
                if (m_Object->IsWithinDistInMap(p_AreaTrigger, m_Range))
                    return true;

                return false;
            }
        private:
            WorldObject const* m_Object;
            float m_Range;
    };

    class AnyConversationInObjectRangeCheck
    {
        public:
            AnyConversationInObjectRangeCheck(WorldObject const* p_Object, float p_Range) : m_Object(p_Object), m_Range(p_Range) { }
            bool operator()(Conversation* p_Conversation)
            {
                if (m_Object->IsWithinDistInMap(p_Conversation, m_Range))
                    return true;

                return false;
            }
        private:
            WorldObject const* m_Object;
            float m_Range;
    };

    class NearestFriendlyUnitInObjectRangeCheck
    {
        public:
            NearestFriendlyUnitInObjectRangeCheck(WorldObject const* p_Obj, Unit const* p_Unit, float p_Range) : m_Obj(p_Obj), m_Unit(p_Unit), m_Range(p_Range) {}
            bool operator()(Unit* u)
            {
                if (u->isAlive() && m_Obj->IsWithinDistInMap(u, m_Range) && m_Unit->IsFriendlyTo(u) && u != m_Unit)
                {
                    m_Range = m_Obj->GetDistance(u);        // use found unit range as new range limit for next check
                    return true;
                }

                return false;
            }
        private:
            WorldObject const* m_Obj;
            Unit const* m_Unit;
            float m_Range;
    };

    // Success at unit in range, range update for next check (this can be use with UnitLastSearcher to find nearest unit)
    class NearestAttackableUnitInObjectRangeCheck
    {
        public:
            NearestAttackableUnitInObjectRangeCheck(WorldObject const* obj, Unit const* funit, float range) : i_obj(obj), i_funit(funit), i_range(range) {}
            bool operator()(Unit* u)
            {
                if (u->isTargetableForAttack() && i_obj->IsWithinDistInMap(u, i_range) &&
                    !i_funit->IsFriendlyTo(u))
                {
                    i_range = i_obj->GetDistance(u);        // use found unit range as new range limit for next check
                    return true;
                }

                return false;
            }
        private:
            WorldObject const* i_obj;
            Unit const* i_funit;
            float i_range;

            // prevent clone this object
            NearestAttackableUnitInObjectRangeCheck(NearestAttackableUnitInObjectRangeCheck const&);
    };

    // Success at unit in range, range update for next check (this can be use with UnitLastSearcher to find nearest unit)
    class NearestAttackableNoCCUnitInObjectRangeCheck
    {
        public:
            NearestAttackableNoCCUnitInObjectRangeCheck(WorldObject const* obj, Unit const* funit, float range) : i_obj(obj), i_funit(funit), i_range(range) {}
            bool operator()(Unit* u)
            {
                if (u->isTargetableForAttack() && i_obj->IsWithinDistInMap(u, i_range) &&
                    !i_funit->IsFriendlyTo(u) && !u->HasBreakableByDamageCrowdControlAura() && !u->HasAuraType(SPELL_AURA_MOD_CONFUSE))
                {
                    i_range = i_obj->GetDistance(u);        // use found unit range as new range limit for next check
                    return true;
                }

                return false;
            }
        private:
            WorldObject const* i_obj;
            Unit const* i_funit;
            float i_range;

            // prevent clone this object
            NearestAttackableNoCCUnitInObjectRangeCheck(NearestAttackableNoCCUnitInObjectRangeCheck const&);
    };

    class AnyAoETargetUnitInObjectRangeCheck
    {
        public:
            AnyAoETargetUnitInObjectRangeCheck(WorldObject const* obj, Unit const* funit, float range)
                : i_obj(obj), i_funit(funit), _spellInfo(NULL), i_range(range)
            {
                Unit const* check = i_funit;
                Unit const* owner = i_funit->GetOwner();
                if (owner)
                    check = owner;
                i_targetForPlayer = (check->IsPlayer());
                if (i_obj->GetTypeId() == TYPEID_DYNAMICOBJECT)
                    _spellInfo = sSpellMgr->GetSpellInfo(((DynamicObject*)i_obj)->GetSpellId());
            }
            bool operator()(Unit* u)
            {
                // Check contains checks for: live, non-selectable, non-attackable flags, flight check and GM check, ignore totems
                if (u->GetTypeId() == TYPEID_UNIT && ((Creature*)u)->isTotem())
                    return false;

                if (i_funit->_IsValidAttackTarget(u, _spellInfo,i_obj->GetTypeId() == TYPEID_DYNAMICOBJECT ? i_obj : NULL) && i_obj->IsWithinDistInMap(u, i_range))
                    return true;

                return false;
            }
        private:
            bool i_targetForPlayer;
            WorldObject const* i_obj;
            Unit const* i_funit;
            SpellInfo const* _spellInfo;
            float i_range;
    };

    // do attack at call of help to friendly crearture
    class CallOfHelpCreatureInRangeDo
    {
        public:
            CallOfHelpCreatureInRangeDo(Unit* funit, Unit* enemy, float range)
                : i_funit(funit), i_enemy(enemy), i_range(range)
            {}
            void operator()(Creature* u)
            {
                if (u == i_funit)
                    return;

                if (!u->CanAssistTo(i_funit, i_enemy, false))
                    return;

                // too far
                if (!u->IsWithinDistInMap(i_funit, i_range))
                    return;

                // only if see assisted creature's enemy
                if (!u->IsWithinLOSInMap(i_enemy))
                    return;

                if (u->AI())
                    u->AI()->AttackStart(i_enemy);
            }
        private:
            Unit* const i_funit;
            Unit* const i_enemy;
            float i_range;
    };

    struct AnyDeadUnitCheck
    {
        bool operator()(Unit* u) { return !u->isAlive(); }
    };

    /*
    struct AnyStealthedCheck
    {
        bool operator()(Unit* u) { return u->GetVisibility() == VISIBILITY_GROUP_STEALTH; }
    };
    */

    // Creature checks

    class NearestHostileUnitCheck
    {
        public:
            explicit NearestHostileUnitCheck(Creature const* creature, float dist = 0) : me(creature)
            {
                m_range = (dist == 0 ? 9999 : dist);
            }
            bool operator()(Unit* u)
            {
                if (!me->IsWithinDistInMap(u, m_range))
                    return false;

                if (!me->IsValidAttackTarget(u))
                    return false;

                m_range = me->GetDistance(u);   // use found unit range as new range limit for next check
                return true;
            }

    private:
            Creature const* me;
            float m_range;
            NearestHostileUnitCheck(NearestHostileUnitCheck const&);
    };

    class NearestHostileNoCCUnitCheck
    {
    public:
        explicit NearestHostileNoCCUnitCheck(Creature const* creature, float dist = 0) : me(creature)
        {
            m_range = (dist == 0 ? 9999 : dist);
        }
        bool operator()(Unit* u)
        {
            if (!me->IsWithinDistInMap(u, m_range))
                return false;

            if (!me->IsValidAttackTarget(u))
                return false;

            if (u->HasBreakableByDamageCrowdControlAura())
                return false;

            if (u->HasAuraType(SPELL_AURA_MOD_CONFUSE))
                return false;

            m_range = me->GetDistance(u);   // use found unit range as new range limit for next check
            return true;
        }

    private:
        Creature const *me;
        float m_range;
        NearestHostileNoCCUnitCheck(NearestHostileNoCCUnitCheck const&);
    };

    class NearestHostileUnitInAttackDistanceCheck
    {
        public:
            explicit NearestHostileUnitInAttackDistanceCheck(Creature const* creature, float dist = 0) : me(creature)
            {
                m_range = (dist == 0 ? 9999 : dist);
                m_force = (dist == 0 ? false : true);
            }
            bool operator()(Unit* u)
            {
                if (!me->IsWithinDistInMap(u, m_range))
                    return false;

                if (!me->canSeeOrDetect(u))
                    return false;

                if (m_force)
                {
                    if (!me->IsValidAttackTarget(u))
                        return false;
                }
                else if (!me->canStartAttack(u, false))
                    return false;

                m_range = me->GetDistance(u);   // use found unit range as new range limit for next check
                return true;
            }
            float GetLastRange() const { return m_range; }
        private:
            Creature const* me;
            float m_range;
            bool m_force;
            NearestHostileUnitInAttackDistanceCheck(NearestHostileUnitInAttackDistanceCheck const&);
    };

    class NearestHostileUnitInAggroRangeCheck
    {
        public:
            explicit NearestHostileUnitInAggroRangeCheck(Creature const* creature, bool useLOS = false) : _me(creature), _useLOS(useLOS)
            {
            }
            bool operator()(Unit* u)
            {
                if (!u->IsHostileTo(_me))
                    return false;

                if (!u->IsWithinDistInMap(_me, _me->GetAggroRange(u)))
                    return false;

                if (!_me->IsValidAttackTarget(u))
                    return false;

                if (_useLOS && !u->IsWithinLOSInMap(_me))
                    return false;

                return true;
            }

    private:
            Creature const* _me;
            bool _useLOS;
            NearestHostileUnitInAggroRangeCheck(NearestHostileUnitInAggroRangeCheck const&);
    };

    class AnyAssistCreatureInRangeCheck
    {
        public:
            AnyAssistCreatureInRangeCheck(Unit* funit, Unit* enemy, float range, bool p_CheckFaction = true)
                : i_funit(funit), i_enemy(enemy), i_range(range), i_CheckFaction(p_CheckFaction)
            {
            }
            bool operator()(Creature* u)
            {
                if (u == i_funit)
                    return false;

                if (!u->CanAssistTo(i_funit, i_enemy, i_CheckFaction))
                    return false;

                // too far
                if (!i_funit->IsWithinDistInMap(u, i_range))
                    return false;

                // only if see assisted creature
                if (!i_funit->IsWithinLOSInMap(u))
                    return false;

                return true;
            }
        private:
            Unit* const i_funit;
            Unit* const i_enemy;
            float i_range;
            bool i_CheckFaction;
    };

    class NearestAssistCreatureInCreatureRangeCheck
    {
        public:
            NearestAssistCreatureInCreatureRangeCheck(Creature* obj, Unit* enemy, float range)
                : i_obj(obj), i_enemy(enemy), i_range(range) {}

            bool operator()(Creature* u)
            {
                if (u == i_obj)
                    return false;
                if (!u->CanAssistTo(i_obj, i_enemy))
                    return false;

                if (!i_obj->IsWithinDistInMap(u, i_range))
                    return false;

                if (!i_obj->IsWithinLOSInMap(u))
                    return false;

                i_range = i_obj->GetDistance(u);            // use found unit range as new range limit for next check
                return true;
            }
            float GetLastRange() const { return i_range; }
        private:
            Creature* const i_obj;
            Unit* const i_enemy;
            float  i_range;

            // prevent clone this object
            NearestAssistCreatureInCreatureRangeCheck(NearestAssistCreatureInCreatureRangeCheck const&);
    };

    // Success at unit in range, range update for next check (this can be use with CreatureLastSearcher to find nearest creature)
    class NearestCreatureEntryWithLiveStateInObjectRangeCheck
    {
        public:
            NearestCreatureEntryWithLiveStateInObjectRangeCheck(WorldObject const& obj, uint32 entry, bool alive, float range)
                : i_obj(obj), i_entry(entry), i_alive(alive), i_range(range) {}

            bool operator()(Creature* u)
            {
                if (u->GetEntry() == i_entry && u->isAlive() == i_alive && i_obj.IsWithinDistInMap(u, i_range))
                {
                    i_range = i_obj.GetDistance(u);         // use found unit range as new range limit for next check
                    return true;
                }
                return false;
            }
            float GetLastRange() const { return i_range; }
        private:
            WorldObject const& i_obj;
            uint32 i_entry;
            bool   i_alive;
            float  i_range;

            // prevent clone this object
            NearestCreatureEntryWithLiveStateInObjectRangeCheck(NearestCreatureEntryWithLiveStateInObjectRangeCheck const&);
    };

    // Success at unit in range and visible, range update for next check (this can be use with CreatureLastSearcher to find nearest creature)
    class NearestCreatureEntryWithLiveStateInObjectRangeCanBeSeenCheck
    {
        public:
            NearestCreatureEntryWithLiveStateInObjectRangeCanBeSeenCheck(WorldObject const& obj, uint32 entry, bool alive, float range)
                : i_obj(obj), i_entry(entry), i_alive(alive), i_range(range) {}

            bool operator()(Creature* u)
            {
                if (u->GetEntry() == i_entry && u->isAlive() == i_alive && i_obj.IsWithinDistInMap(u, i_range) && i_obj.canSeeOrDetect(u))
                {
                    i_range = i_obj.GetDistance(u);         // use found unit range as new range limit for next check
                    return true;
                }
                return false;
            }
            float GetLastRange() const { return i_range; }
        private:
            WorldObject const& i_obj;
            uint32 i_entry;
            bool   i_alive;
            float  i_range;

            // prevent clone this object
            NearestCreatureEntryWithLiveStateInObjectRangeCanBeSeenCheck(NearestCreatureEntryWithLiveStateInObjectRangeCanBeSeenCheck const&);
    };

    /// Success at areatrigger in range, range update for next check (this can be use with CreatureLastSearcher to find nearest creature)
    class NearestAreaTriggerWithIDInObjectRangeCheck
    {
        public:
            NearestAreaTriggerWithIDInObjectRangeCheck(WorldObject const& p_Object, uint32 p_SpellID, float p_Range)
                : m_Object(p_Object), m_SpellID(p_SpellID), m_Range(p_Range) { }

            bool operator()(AreaTrigger* p_AreaTrigger)
            {
                if (p_AreaTrigger->GetSpellId() == m_SpellID && m_Object.IsWithinDistInMap(p_AreaTrigger, m_Range))
                {
                    /// Use found areatrigger range as new range limit for next check
                    m_Range = m_Object.GetDistance(p_AreaTrigger);
                    return true;
                }

                return false;
            }

            float GetLastRange() const { return m_Range; }

        private:
            WorldObject const& m_Object;
            uint32 m_SpellID;
            float m_Range;

            // prevent clone this object
            NearestAreaTriggerWithIDInObjectRangeCheck(NearestAreaTriggerWithIDInObjectRangeCheck const&);
    };

    class AnyPlayerInObjectRangeCheck
    {
        public:
            AnyPlayerInObjectRangeCheck(WorldObject const* obj, float range, bool reqAlive = true, bool p_Self = false) : _obj(obj), _range(range), _reqAlive(reqAlive), m_Self(p_Self) { }
            bool operator()(Player* u)
            {
                if (_reqAlive && !u->isAlive())
                    return false;

                if (!_obj->IsWithinDistInMap(u, _range))
                    return false;

                /// Don't add self
                if (!m_Self && _obj->GetTypeId() == TypeID::TYPEID_PLAYER && _obj->GetGUID() == u->GetGUID())
                    return false;

                return true;
            }

        private:
            WorldObject const* _obj;
            float _range;
            bool _reqAlive;
            bool m_Self;
    };

    class AnyDeadPlayerInObjectRangeCheck
    {
        public:
            AnyDeadPlayerInObjectRangeCheck(WorldObject const* p_Object, float p_Range, bool p_Self = false) : m_WorldObject(p_Object), m_Range(p_Range), m_Self(p_Self) { }
            bool operator()(Player* p_Player)
            {
                if (p_Player->isAlive())
                    return false;

                if (!m_WorldObject->IsWithinDistInMap(p_Player, m_Range))
                    return false;

                /// Don't add self
                if (!m_Self && m_WorldObject->IsPlayer() && m_WorldObject->GetGUID() == p_Player->GetGUID())
                    return false;

                return true;
            }

        private:
            WorldObject const* m_WorldObject;
            float m_Range;
            bool m_Self;
    };

    class NearestPlayerInObjectRangeCheck
    {
        public:
            NearestPlayerInObjectRangeCheck(WorldObject const* obj, float range) : i_obj(obj), i_range(range)
            {
            }

            bool operator()(Player* u)
            {
                if (i_obj == u)
                    return false;

                if (u->isAlive() && i_obj->IsWithinDistInMap(u, i_range))
                {
                    i_range = i_obj->GetDistance(u);
                    return true;
                }

                return false;
            }
        private:
            WorldObject const* i_obj;
            float i_range;

            NearestPlayerInObjectRangeCheck(NearestPlayerInObjectRangeCheck const&);
    };

    class NearestDeadPlayerInObjectRangeCheck
    {
        public:
            NearestDeadPlayerInObjectRangeCheck(WorldObject const* obj, float range) : i_obj(obj), i_range(range)
            {
            }

            bool operator()(Player* u)
            {
                if (!u->isAlive() && i_obj->IsWithinDistInMap(u, i_range))
                {
                    i_range = i_obj->GetDistance(u);
                    return true;
                }

                return false;
            }
        private:
            WorldObject const* i_obj;
            float i_range;

            NearestDeadPlayerInObjectRangeCheck(NearestDeadPlayerInObjectRangeCheck const&);
    };

    class NearestPlayerNotGMInObjectRangeCheck
    {
        public:
            NearestPlayerNotGMInObjectRangeCheck(WorldObject const* obj, float range) : i_obj(obj), i_range(range)
            {
            }

            bool operator()(Player* u)
            {
                if (!u->isGameMaster() && u->isAlive() && i_obj->IsWithinDistInMap(u, i_range))
                {
                    i_range = i_obj->GetDistance(u);
                    return true;
                }

                return false;
            }
        private:
            WorldObject const* i_obj;
            float i_range;

            NearestPlayerNotGMInObjectRangeCheck(NearestPlayerNotGMInObjectRangeCheck const&);
    };

    class AllFriendlyCreaturesInGrid
    {
    public:
        AllFriendlyCreaturesInGrid(Unit const* obj) : unit(obj) {}
        bool operator() (Unit* u)
        {
            if (u->isAlive() && u->IsVisible() && u->IsFriendlyTo(unit))
                return true;

            return false;
        }
    private:
        Unit const* unit;
    };

    class AllGameObjectsWithEntryInRange
    {
    public:
        AllGameObjectsWithEntryInRange(const WorldObject* object, uint32 entry, float maxRange) : m_pObject(object), m_uiEntry(entry), m_fRange(maxRange) {}
        bool operator() (GameObject* go)
        {
            if (go->GetEntry() == m_uiEntry && m_pObject->IsWithinDist(go, m_fRange, false))
                return true;

            return false;
        }
    private:
        const WorldObject* m_pObject;
        uint32 m_uiEntry;
        float m_fRange;
    };

    class AllCreaturesOfEntryInRange
    {
        public:
            AllCreaturesOfEntryInRange(const WorldObject* object, uint32 entry, float maxRange) : m_pObject(object), m_uiEntry(entry), m_fRange(maxRange) {}
            bool operator() (Unit* unit)
            {
                if (unit->GetEntry() == m_uiEntry && m_pObject->IsWithinDist(unit, m_fRange, false))
                    return true;

                return false;
            }

        private:
            const WorldObject* m_pObject;
            uint32 m_uiEntry;
            float m_fRange;
    };

    class AllCreaturesInRange
    {
    public:
        AllCreaturesInRange(const WorldObject* object, float maxRange) : m_pObject(object), m_fRange(maxRange) {}
        bool operator() (Unit* unit)
        {
            if (m_pObject->IsWithinDist(unit, m_fRange, false))
                return true;

            return false;
        }

    private:
        const WorldObject* m_pObject;
        float m_fRange;
    };

    class AllDeadCreaturesInRange
    {
        public:
            AllDeadCreaturesInRange(const WorldObject* object, float maxRange, uint64 excludeGUID) : m_pObject(object), m_excludeGUID(excludeGUID), m_fRange(maxRange) {}
            bool operator() (Unit* unit)
            {
                if (unit->GetTypeId() == TYPEID_UNIT && unit->GetGUID() != m_excludeGUID && !unit->isAlive() && m_pObject->IsWithinDist(unit, m_fRange, false))
                    return true;

                return false;
            }

        private:
            const WorldObject* m_pObject;
            uint64 m_excludeGUID;
            float m_fRange;
    };

    class PlayerAtMinimumRangeAway
    {
    public:
        PlayerAtMinimumRangeAway(Unit const* unit, float fMinRange) : unit(unit), fRange(fMinRange) {}
        bool operator() (Player* player)
        {
            //No threat list check, must be done explicit if expected to be in combat with creature
            if (!player->isGameMaster() && player->isAlive() && !unit->IsWithinDist(player, fRange, false))
                return true;

            return false;
        }

    private:
        Unit const* unit;
        float fRange;
    };

    class GameObjectInRangeCheck
    {
    public:
        GameObjectInRangeCheck(float _x, float _y, float _z, float _range, uint32 _entry = 0) :
          x(_x), y(_y), z(_z), range(_range), entry(_entry) {}
        bool operator() (GameObject* go)
        {
            if (!entry || (go->GetGOInfo() && go->GetGOInfo()->entry == entry))
                return go->IsInRange(x, y, z, range);
            else return false;
        }
    private:
        float x, y, z, range;
        uint32 entry;
    };

    class AllAreaTriggersInRangeCheck
    {
    public:
        AllAreaTriggersInRangeCheck(const WorldObject* object, float maxRange) : m_pObject(object), m_fRange(maxRange) {}
        bool operator() (AreaTrigger* p_AreaTrigger)
        {
            return m_pObject->IsWithinDist(p_AreaTrigger, m_fRange, false) && m_pObject->InSamePhase(p_AreaTrigger);
        }
    private:
        const WorldObject* m_pObject;
        float m_fRange;
    };

    class AllWorldObjectsInRange
    {
        public:
            AllWorldObjectsInRange(const WorldObject* object, float maxRange) : m_pObject(object), m_fRange(maxRange)
            {
                m_Timestamp = getMSTime();
            }

            bool operator() (WorldObject* go)
            {
                if (m_pObject->GetEntry() == 118089 && GetMSTimeDiffToNow(m_Timestamp) > 10)
                    sLog->outExtChat("#jarvis", "danger", true, "AllWorldObjectsInRange from 118089 smartai freeze => %u ALED", go->GetEntry());

                return m_pObject->IsWithinDist(go, m_fRange, false) && m_pObject->InSamePhase(go);
            }
        private:
            const WorldObject* m_pObject;
            float m_fRange;
            uint32 m_Timestamp;
    };

    class ObjectTypeIdCheck
    {
        public:
            ObjectTypeIdCheck(TypeID typeId, bool equals) : _typeId(typeId), _equals(equals) {}
            bool operator()(WorldObject* object)
            {
                return (object->GetTypeId() == _typeId) == _equals;
            }

        private:
            TypeID _typeId;
            bool _equals;
    };

    class ObjectGUIDCheck
    {
        public:
            ObjectGUIDCheck(uint64 GUID) : _GUID(GUID) {}
            bool operator()(WorldObject* object)
            {
                return object->GetGUID() == _GUID;
            }

        private:
            uint64 _GUID;
    };

    class UnitAuraCheck
    {
        public:
            UnitAuraCheck(bool present, uint32 spellId, uint64 casterGUID = 0) : _present(present), _spellId(spellId), _casterGUID(casterGUID) {}
            bool operator()(Unit* unit) const
            {
                return unit->HasAura(_spellId, _casterGUID) == _present;
            }

            bool operator()(WorldObject* object) const
            {
                return object->ToUnit() && object->ToUnit()->HasAura(_spellId, _casterGUID) == _present;
            }

        private:
            bool _present;
            uint32 _spellId;
            uint64 _casterGUID;
    };

    class UnitAuraTypeCheck
    {
        public:
            UnitAuraTypeCheck(bool present, AuraType type) : _present(present), _type(type) {}
            bool operator()(Unit* unit) const
            {
                return unit->HasAuraType(_type) == _present;
            }

            bool operator()(WorldObject* object) const
            {
                return object->ToUnit() && object->ToUnit()->HasAuraType(_type) == _present;
            }

        private:
            bool _present;
            AuraType _type;
    };

    // Player checks and do

    // Prepare using Builder localized packets with caching and send to player
    template<class Builder>
    class LocalizedPacketDo
    {
        public:
            explicit LocalizedPacketDo(Builder& builder) : i_builder(builder) {}

            ~LocalizedPacketDo()
            {
                for (size_t i = 0; i < i_data_cache.size(); ++i)
                    delete i_data_cache[i];
            }
            void operator()(Player* p);

        private:
            Builder& i_builder;
            std::vector<WorldPacket*> i_data_cache;         // 0 = default, i => i-1 locale index
    };

    // Prepare using Builder localized packets with caching and send to player
    template<class Builder>
    class LocalizedPacketListDo
    {
        public:
            typedef std::vector<WorldPacket*> WorldPacketList;
            explicit LocalizedPacketListDo(Builder& builder) : i_builder(builder) {}

            ~LocalizedPacketListDo()
            {
                for (size_t i = 0; i < i_data_cache.size(); ++i)
                    for (size_t j = 0; j < i_data_cache[i].size(); ++j)
                        delete i_data_cache[i][j];
            }
            void operator()(Player* p);

        private:
            Builder& i_builder;
            std::vector<WorldPacketList> i_data_cache;
                                                            // 0 = default, i => i-1 locale index
    };

    class SummonTimerOrderPred
    {
        public:
            SummonTimerOrderPred(bool ascending = true) : m_ascending(ascending) {}
            bool operator() (const Unit* a, const Unit* b) const
            {
                if (!a->isSummon() || !b->isSummon())
                    return (urand(0, 1) ? false : true);

                uint32 rA = ((TempSummon*)a)->GetTimer();
                uint32 rB = ((TempSummon*)b)->GetTimer();
                return m_ascending ? rA < rB : rA > rB;
            }
        private:
            const bool m_ascending;
    };

    class UnitHealthState
    {
        public:
            UnitHealthState(bool sortlow) : _sortlow(sortlow) {}
            bool operator()(Unit* unitA, Unit* unitB) const
            {
                return (unitA->GetHealthPct() < unitB->GetHealthPct()) == _sortlow;
            }

            bool operator()(WorldObject* objectA, WorldObject* objectB) const
            {
                return objectA->ToUnit() && objectB->ToUnit() && (objectA->ToUnit()->GetHealthPct() < objectB->ToUnit()->GetHealthPct()) == _sortlow;
            }

        private:
            bool _sortlow;
    };

    class UnitAuraAndCheck
    {
        public:
            UnitAuraAndCheck(int32 aura1, int32 aura2 = 0, int32 aura3 = 0, uint64 casterGUID = 0) : _aura1(aura1), _aura2(aura2), _aura3(aura3), _casterGUID(casterGUID) {}
            bool operator()(Unit* unit) const
            {
                if (_aura1 < 0 && unit->HasAura(abs(_aura1), _casterGUID))
                    return true;
                else if (_aura1 > 0 && !unit->HasAura(_aura1, _casterGUID))
                    return true;

                if (_aura2 < 0 && unit->HasAura(abs(_aura2), _casterGUID))
                    return true;
                else if (_aura2 > 0 && !unit->HasAura(_aura2, _casterGUID))
                    return true;

                if (_aura3 < 0 && unit->HasAura(abs(_aura3), _casterGUID))
                    return true;
                else if (_aura3 > 0 && !unit->HasAura(_aura3, _casterGUID))
                    return true;

                return false;
            }

            bool operator()(WorldObject* object) const
            {
                if (Unit* unit = object->ToUnit())
                {
                    if (_aura1 < 0 && unit->HasAura(abs(_aura1), _casterGUID))
                        return true;
                    else if (_aura1 > 0 && !unit->HasAura(_aura1, _casterGUID))
                        return true;

                    if (_aura2 < 0 && unit->HasAura(abs(_aura2), _casterGUID))
                        return true;
                    else if (_aura2 > 0 && !unit->HasAura(_aura2, _casterGUID))
                        return true;

                    if (_aura3 < 0 && unit->HasAura(abs(_aura3), _casterGUID))
                        return true;
                    else if (_aura3 > 0 && !unit->HasAura(_aura3, _casterGUID))
                        return true;

                    return false;
                }
                else
                    return true;
            }

        private:
            int32 _aura1;
            int32 _aura2;
            int32 _aura3;
            uint64 _casterGUID;
    };

    class UnitDistanceCheck
    {
        public:
            UnitDistanceCheck(bool checkin, Unit* caster, float dist) : _checkin(checkin), _caster(caster), _dist(dist) {}
            bool operator()(Unit* unit) const
            {
                return (_caster->GetExactDist2d(unit) > _dist) == _checkin;
            }

            bool operator()(WorldObject* object) const
            {
                return (_caster->GetExactDist2d(object) > _dist) == _checkin;
            }

        private:
            bool _checkin;
            Unit* _caster;
            float _dist;
    };

    class UnitTypeCheck
    {
        public:
            UnitTypeCheck(bool checkin, uint32 typeMask) : _checkin(checkin), _typeMask(typeMask) {}
            bool operator()(Unit* unit) const
            {
                return bool(_typeMask & (1 << unit->GetTypeId())) == _checkin;
            }

            bool operator()(WorldObject* object) const
            {
                return bool(_typeMask & (1 << object->GetTypeId())) == _checkin;
            }

        private:
            bool _checkin;
            uint32 _typeMask;
    };

    class UnitSortDistance
    {
        public:
            UnitSortDistance(bool sortlow, Unit* caster) : _sortlow(sortlow), _caster(caster) {}
            bool operator()(Unit* unitA, Unit* unitB) const
            {
                return (_caster->GetExactDist2d(unitA) < _caster->GetExactDist2d(unitB)) == _sortlow;
            }

            bool operator()(WorldObject* objectA, WorldObject* objectB) const
            {
                return (_caster->GetExactDist2d(objectA) < _caster->GetExactDist2d(objectB)) == _sortlow;
            }

        private:
            bool _sortlow;
            Unit* _caster;
    };

    class UnitFriendlyCheck
    {
        public:
            UnitFriendlyCheck(bool present, Unit* caster) : _present(present), _caster(caster) {}
            bool operator() (Unit* unit)
            {
                return unit->IsFriendlyTo(_caster) == _present;
            }
            bool operator() (WorldObject* object)
            {
                return object->ToUnit() && object->ToUnit()->IsFriendlyTo(_caster) == _present;
            }
        private:
            bool _present;
            Unit* _caster;
    };

    class UnitRaidCheck
    {
        public:
            UnitRaidCheck(bool present, Unit* caster) : _present(present), _caster(caster) {}
            bool operator() (Unit* unit)
            {
                return unit->IsInRaidWith(_caster) == _present;
            }
            bool operator() (WorldObject* object)
            {
                return object->ToUnit() && object->ToUnit()->IsInRaidWith(_caster) == _present;
            }
        private:
            bool _present;
            Unit* _caster;
    };

    class UnitPartyCheck
    {
        public:
            UnitPartyCheck(bool present, Unit* caster) : _present(present), _caster(caster) {}
            bool operator() (Unit* unit)
            {
                return unit->IsInPartyWith(_caster) == _present;
            }
            bool operator() (WorldObject* object)
            {
                return object->ToUnit() && object->ToUnit()->IsInPartyWith(_caster) == _present;
            }
        private:
            bool _present;
            Unit* _caster;
    };

    class UnitCheckInLos
    {
        public:
            UnitCheckInLos(bool present, Unit* caster) : _present(present), _caster(caster) {}
            bool operator() (WorldObject* object)
            {
                return object->IsWithinLOSInMap(_caster) == _present;
            }
        private:
            bool _present;
            Unit* _caster;
    };

    class UnitCheckInBetween
    {
        public:
            UnitCheckInBetween(bool present, Unit* caster, Unit* target, float size) : _present(present), _caster(caster), _target(target), _size(size) {}
            bool operator() (WorldObject* object)
            {
                return object->IsInBetween(_caster, _target, _size) == _present;
            }
        private:
            bool _present;
            Unit* _caster;
            Unit* _target;
            float _size;
    };

    class UnitCheckInBetweenShift
    {
        public:
            UnitCheckInBetweenShift(bool present, Unit* caster, Unit* target, float size, float shift, float angleShift)
            : _present(present), _caster(caster), _target(target), _size(size), _shift(shift), _angleShift(angleShift) {}

            bool operator() (WorldObject* object)
            {
                return object->IsInBetweenShift(_caster, _target, _size, _shift, _angleShift) == _present;
            }
        private:
            bool _present;
            Unit* _caster;
            Unit* _target;
            float _size;
            float _shift;
            float _angleShift;
    };

    class UnitAuraOrCheck
    {
        public:
            UnitAuraOrCheck(int32 aura1, int32 aura2, int32 aura3, uint64 casterGUID = 0) : _aura1(aura1), _aura2(aura2), _aura3(aura3), _casterGUID(casterGUID) {}
            bool operator()(Unit* unit) const
            {
                if (_aura1 < 0 && !unit->HasAura(abs(_aura1), _casterGUID))
                    return false;
                else if (_aura1 > 0 && unit->HasAura(_aura1, _casterGUID))
                    return false;

                if (_aura2 < 0 && !unit->HasAura(abs(_aura2), _casterGUID))
                    return false;
                else if (_aura2 > 0 && unit->HasAura(_aura2, _casterGUID))
                    return false;

                if (_aura3 < 0 && !unit->HasAura(abs(_aura3), _casterGUID))
                    return false;
                else if (_aura3 > 0 && unit->HasAura(_aura3, _casterGUID))
                    return false;

                return true;
            }

            bool operator()(WorldObject* object) const
            {
                if (Unit* unit = object->ToUnit())
                {
                    if (_aura1 < 0 && !unit->HasAura(abs(_aura1), _casterGUID))
                        return false;
                    else if (_aura1 > 0 && unit->HasAura(_aura1, _casterGUID))
                        return false;

                    if (_aura2 < 0 && !unit->HasAura(abs(_aura2), _casterGUID))
                        return false;
                    else if (_aura2 > 0 && unit->HasAura(_aura2, _casterGUID))
                        return false;

                    if (_aura3 < 0 && !unit->HasAura(abs(_aura3), _casterGUID))
                        return false;
                    else if (_aura3 > 0 && unit->HasAura(_aura3, _casterGUID))
                        return false;

                    return true;
                }
                else
                    return true;
            }

        private:
            int32 _aura1;
            int32 _aura2;
            int32 _aura3;
            uint64 _casterGUID;
    };

    class UnitEntryCheck
    {
        public:
            UnitEntryCheck (int32 entry1, int32 entry2, int32 entry3) : _entry1(entry1), _entry2(entry2), _entry3(entry3) {}
            bool operator()(WorldObject* object) const
            {
                if (_entry1 > 0)
                {
                    if (_entry1 > 0 && object->GetEntry() == _entry1)
                        return false;
                    if (_entry2 > 0 && object->GetEntry() == _entry2)
                        return false;
                    if (_entry3 > 0 && object->GetEntry() == _entry3)
                        return false;

                    return true;
                }
                if (_entry1 < 0)
                {
                    if (_entry1 < 0 && object->GetEntry() == abs(_entry1))
                        return true;
                    if (_entry2 < 0 && object->GetEntry() == abs(_entry2))
                        return true;
                    if (_entry3 < 0 && object->GetEntry() == abs(_entry3))
                        return true;

                    return false;
                }

                return true;
            }

        private:
            int32 _entry1;
            int32 _entry2;
            int32 _entry3;
    };

    class UnitAttackableCheck
    {
        public:
            UnitAttackableCheck(bool present, Unit* caster) : _present(present), _caster(caster) {}
            bool operator() (Unit* unit)
            {
                return _caster->IsValidAttackTarget(unit) == _present;
            }
            bool operator() (WorldObject* object)
            {
                return object->ToUnit() && _caster->IsValidAttackTarget(object->ToUnit()) == _present;
            }
        private:
            bool _present;
            Unit* _caster;
    };

    class DestDistanceCheck
    {
    public:
        DestDistanceCheck(bool checkin, Position* dest, float dist) : _checkin(checkin), _dest(dest), _dist(dist) {}
        bool operator()(Unit* unit) const
        {
            return (_dest->GetExactDist2d(unit) > _dist) == _checkin;
        }
        bool operator()(WorldObject* object) const
        {
            return (_dest->GetExactDist2d(object) > _dist) == _checkin;
        }
    private:
        bool _checkin;
        Position* _dest;
        float _dist;
    };

    class UnitOwnerCheck
    {
    public:
        UnitOwnerCheck(bool present, Unit* caster) : _present(present), _caster(caster) {}
        bool operator()(Unit* unit)
        {
            return unit->IsOwnerOrSelf(_caster) == _present;
        }
        bool operator()(WorldObject* object)
        {
            return object->ToUnit() && object->ToUnit()->IsOwnerOrSelf(_caster) == _present;
        }
    private:
        bool _present;
        Unit* _caster;
    };
}
#endif
