#include "tomb_of_sargeras.h"

enum eSpells
{
    SpellQuietus            = 236595,
    SpellQuietusAoE         = 242620,
    SpellQuietusDmg         = 236507,
    SpellBoundEssence       = 236351,
    SpellBoundEssenceTrigger= 238580,

    SpellSpiritualBarrierSpirit = 235620,
    SpellSpiritualBarrierPhysic = 235113,

    SpellDissonanceSpirit       = 239006,
    SpellDissonanceCorporeal    = 239007,

    /// Engine of Souls
    SpellBoneShards         = 236142,
    SpellCollapsingFissure  = 235885,
    SpellCollapsingFissureAT= 235890,
    SpellRupturingSlam      = 235927,
    SpellGraspingDarkness   = 235968,
    SpellBoneCageArmor      = 236513,
    SpellSpearOfAnguishAOE  = 235923,
    SpellSpearOfAnguishAbsorb   = 238442,
    SpellTormentedCries     = 238570,
    SpellTormentedCriesAOE  = 235988,
    SpellTormentedCriesDmg  = 235989,
    SpellTormentedCriesDot  = 236011,

    /// Soul Queen Dehjanna
    SpellShatteringScream   = 235969,
    SpellShatteringScreamDmg= 236772,
    SpellSpiritChains       = 236361,
    SpellCrushMind          = 236340,
    SpellWither             = 236131,
    SpellWailingSouls       = 236072,

    SpellSoulbind           = 236449,
    SpellSoulbindCorporealSearcher = 236455,
    SpellSoulbindSpiritSearcher = 236454,
    SpellSoulbindBuff       = 236459,
    SpellSoulbindDmg        = 236464,
    SpellSoulbindBuffSearcher= 236465,
    SpellSoulRot            = 237588,
    SpellSoulRotAT          = 236235,
    SpellSoulRotDmg         = 236241,
    SpellSoulEruption       = 236158,
    SpellUnreleting         = 203950,

    /// Desolate Host
    SpellSunderingDoom      = 236542,
    SpellDoomedSummering    = 236544,
    SpellTorment            = 236548,

    /// Achievement
    SpellDiminishingSoulAOE     = 240358,
    SpellDiminishingSoulMinAOE  = 240317,
    SpellDiminishingSoulMissile = 240326,
    SpellDiminishingSoulBuff    = 240359,
    SpellDiminishingSoulPrevent = 240325,
};

enum eEvents
{
    /// Engine of Souls
    EventBoneShards = 1,
    EventCollapsingFissure,
    EventTormentedCries,
    EventSpearOfAnguish,
    EventQuietus,

    /// Soul Queen Dehjana
    EventCrushMind,
    EventWailingSouls,
    EventWither,
    EventSoulbind,

    /// Desolate Host
    EventSunderingDoom,
    EventDommedSummering,

    EventSummonAdds,
};

enum eDesolateHostData
{
    DataRupturingSlam,
    DataVisibility,
};

enum eOrientations
{
    North,
    South,
    East,
    West
};

enum eDesolateHostActions
{
    ActionCancelSummonAdds = 10,
};

static const Position g_DesolateSummon = { 6447.458f, -1099.681f, 2881.605f, 2.491447f };
static const Position g_CenterSummonAdds = { 6424.443f, -1080.127f, 2881.528f, 5.682911f };
static const Position g_DiminishingSoulPos = { 6465.686f, -1095.425f, 2881.538f };

const Position g_CorporealRealmInitPos[4] =
{
    { 6444.92f, -1101.77f, 2881.61f, 5.80206f },
    { 6448.42f, -1097.31f, 2881.56f, 4.09861f },
    { 6441.27f, -1096.42f, 2881.61f, 5.69617f },
    { 6438.02f, -1103.45f, 2881.61f, 6.07203f }
};

const Position g_SpiritRealmInitPos[5] =
{
    { 6456.28f, -1089.77f, 2881.61f, 0.320368f },
    { 6460.06f, -1082.43f, 2881.61f, 5.07256f  },
    { 6462.84f, -1076.22f, 2881.56f, 2.59168f  },
    { 6445.54f, -1094.06f, 2881.56f, 0.854889f },
    { 6455.06f, -1090.62f, 2881.56f, 5.33937f  },
};

const Position g_SoulFontsPos[] =
{
    /// Soul Font - 118701
    { 6418.5f, -1059.29f,   2881.99f, 0 },
    { -546.959f,  2447.5f,  103.941f, 3.54669f },
    { 6407.38f, -1109.63f,  2881.99f, 0 },
    { 6446.13f, -1054.33f,  2881.99f, 0 },
    { 6468.97f, -1070.34f,  2881.99f, 0 },
    { 6430.19f, -1125.69f,  2881.99f, 0 },
    { 6402.56f, -1082.19f,  2881.99f, 0 },

    /// Soul Font - 118699
    { 6418.5f, -1059.29f, 2881.99f, 0 },
    { 6446.13f,-1054.33f, 2881.99f, 0 },
    { 6407.38f,-1109.63f, 2881.99f, 0 },
    { 6402.56f,-1082.19f, 2881.99f, 0 },
    { 6430.19f,-1125.69f, 2881.99f, 0 },
    { 6468.97f,-1070.34f, 2881.99f, 0 },
};

/// Engine of Souls - 118460
class boss_engine_of_souls : public CreatureScript
{
    public:
        boss_engine_of_souls() : CreatureScript("boss_engine_of_souls")
        {}

        enum eTalks
        {
            TalkAggro,
            TalkDesolate,
            TalkTormentedCries,
            TalkKill,
        };

        struct boss_engine_of_souls_AI : public BossAI
        {
            explicit boss_engine_of_souls_AI(Creature* p_Me) : BossAI(p_Me, eData::DataDesolateHost)
            {}

            void InitializeAI() override
            {
                BossAI::InitializeAI();
                SpawnAdds();
                RespawnFonts();
            }

            bool CanBeAttacked(Unit const* p_Target, SpellInfo const* /**/) const override
            {
                return !p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
            }

            void AddBarriersToPlayers()
            {
                Map::PlayerList const& l_Players = instance->instance->GetPlayers();

                for (Map::PlayerList::const_iterator l_Itr = l_Players.begin(); l_Itr != l_Players.end(); ++l_Itr)
                {
                    if (Player* l_Target = l_Itr->getSource())
                    {
                        if (!l_Target->HasAura(eSharedSpells::SpellSpiritRealm))
                            l_Target->CastSpell(l_Target, eSharedSpells::SpellCorporealRealm, true);
                    }
                }
            }

            void JustRespawned() override
            {
                BossAI::JustRespawned();
                SpawnAdds();
                RespawnFonts();
            }

            void SpawnAdds()
            {
                for (uint8 l_Itr = 0; l_Itr < 2; ++l_Itr)
                    DoSummon(eCreatures::NpcReanimatedTemplar, g_CorporealRealmInitPos[l_Itr], 60800 * IN_MILLISECONDS);

                for (uint8 l_Itr = 2; l_Itr < 4; ++l_Itr)
                    DoSummon(eCreatures::NpcGhastlyBonewarden, g_CorporealRealmInitPos[l_Itr], 60800 * IN_MILLISECONDS);
            }

            void RespawnFonts()
            {
                if (IsMythic())
                {
                    std::list<Creature*> l_Fonts;

                    me->GetCreatureListWithEntryInGridAppend(l_Fonts, eCreatures::NpcSoulFont, 100.f);
                    me->GetCreatureListWithEntryInGridAppend(l_Fonts, eCreatures::NpcSoulFont2, 100.f);

                    for (Creature* l_Itr : l_Fonts)
                    {
                        if (l_Itr == nullptr)
                            continue;

                        l_Itr->DespawnOrUnsummon();
                    }

                    l_Fonts.clear();

                    for (uint8 l_Itr = 0; l_Itr < 7; ++l_Itr)
                        DoSummon(eCreatures::NpcSoulFont, g_SoulFontsPos[l_Itr], 60800 * IN_MILLISECONDS, TEMPSUMMON_CORPSE_TIMED_DESPAWN);

                    for (uint8 l_Itr = 7; l_Itr < 13; ++l_Itr)
                        DoSummon(eCreatures::NpcSoulFont2, g_SoulFontsPos[l_Itr], 60800 * IN_MILLISECONDS, TEMPSUMMON_CORPSE_TIMED_DESPAWN);
                }
            }

            void AreaTriggerCreated(AreaTrigger* p_At) override
            {
                if (p_At == nullptr)
                    return;

                if (p_At->GetSpellId() == SpellCollapsingFissureAT)
                    p_At->SetPhaseMask(1, true);
            }

            void EnterEvadeMode() override
            {
                BossAI::EnterEvadeMode();

                Creature* l_Desolate = sObjectAccessor->GetCreature(*me, instance->GetData64(eCreatures::BossDesolateHost));

                if (l_Desolate == nullptr)
                    return;

                if (l_Desolate->isDead())
                {
                    l_Desolate->NearTeleportTo(l_Desolate->GetHomePosition());
                    l_Desolate->Respawn(true, true, 30 * TimeConstants::IN_MILLISECONDS);
                }
            }

            void EnterCombat(Unit* /**/) override
            {
                Talk(eTalks::TalkAggro);

                _EnterCombat();

                if (Creature* l_Queen = sObjectAccessor->GetCreature(*me, instance->GetData64(eCreatures::BossSoulQueenDejahna)))
                    DoZoneInCombat(l_Queen, 250.f);

                UpdateVisibilityStartCombat();

                DoCast(me, eSpells::SpellQuietus, true);

                events.ScheduleEvent(eEvents::EventBoneShards, IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventTormentedCries, 120 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventSummonAdds, 60 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventCollapsingFissure, 6 * IN_MILLISECONDS);

                if (IsHeroicOrMythic())
                    events.ScheduleEvent(eEvents::EventSpearOfAnguish, 20 * IN_MILLISECONDS);

                if (!IsLFR())
                    me->CastSpell(g_DiminishingSoulPos, eSpells::SpellDiminishingSoulMinAOE, true);

                AddBarriersToPlayers();
            }

            void UpdateVisibilityStartCombat()
            {
                std::list<Creature*> l_Mobs;

                me->GetCreatureListWithEntryInGridAppend(l_Mobs, eCreatures::NpcFallenPriestess, 150.f);
                me->GetCreatureListWithEntryInGridAppend(l_Mobs, eCreatures::NpcSoulResidue, 150.f);

                if (IsMythic())
                {
                    me->GetCreatureListWithEntryInGridAppend(l_Mobs, eCreatures::NpcReanimatedTemplar, 150.f);
                    me->GetCreatureListWithEntryInGridAppend(l_Mobs, eCreatures::NpcGhastlyBonewarden, 150.f);
                }

                l_Mobs.push_back(me);

                for (auto l_Mob : l_Mobs)
                {
                    Map::PlayerList const& l_PlayerList = me->GetMap()->GetPlayers();
                    for (Map::PlayerList::const_iterator l_Itr = l_PlayerList.begin(); l_Itr != l_PlayerList.end(); ++l_Itr)
                    {
                        Player* l_Target = l_Itr->getSource();

                        if (l_Target == nullptr)
                            continue;

                        l_Mob->SendUpdateToPlayer(l_Target);
                    }
                }
            }

            void UpdateVisibility(Player* p_Player)
            {
                if (p_Player == nullptr)
                    return;

                if (p_Player->isDead())
                    return;

                std::list<Creature*> l_Mobs;

                me->GetCreatureListWithEntryInGridAppend(l_Mobs, eCreatures::NpcReanimatedTemplar, 500.f);
                me->GetCreatureListWithEntryInGridAppend(l_Mobs, eCreatures::NpcGhastlyBonewarden, 500.f);

                if (IsMythic())
                {
                    me->GetCreatureListWithEntryInGridAppend(l_Mobs, eCreatures::NpcFallenPriestess, 500.f);
                    me->GetCreatureListWithEntryInGridAppend(l_Mobs, eCreatures::NpcSoulResidue, 500.f);
                }

                l_Mobs.push_back(me);

                for (Creature* l_Itr : l_Mobs)
                {
                    if (l_Itr == nullptr)
                        continue;

                    l_Itr->SendUpdateToPlayer(p_Player);
                }

                Map::PlayerList const& l_PlayerList = instance->instance->GetPlayers();
                if (l_PlayerList.isEmpty())
                    return;

                for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
                {
                    Player* l_Player = l_Iter->getSource();
                    if (!p_Player || l_Player == p_Player)
                        continue;

                    p_Player->SendUpdateToPlayer(l_Player);
                    l_Player->SendUpdateToPlayer(p_Player);
                }
            }

            void SetGUID(uint64 p_Guid, int32 p_Id) override
            {
                if (p_Id == eDesolateHostData::DataVisibility)
                {
                    Player* l_Player = sObjectAccessor->GetPlayer(*me, p_Guid);
                    UpdateVisibility(l_Player);
                }
            }

            void DestroySoulFonts()
            {
                std::list<Creature*> l_Fonts;

                me->GetCreatureListWithEntryInGridAppend(l_Fonts, eCreatures::NpcSoulFont, 100.f);
                me->GetCreatureListWithEntryInGridAppend(l_Fonts, eCreatures::NpcSoulFont2, 100.f);

                for (Creature* l_Itr : l_Fonts)
                {
                    if (l_Itr == nullptr)
                        continue;

                    l_Itr->DespawnOrUnsummon();
                }
            }

            void DamageTaken(Unit* /**/, uint32 & p_Dmg, SpellInfo const* /**/) override
            {
                if (HealthBelowPct(30) && !m_Desolate && !IsLFR())
                {
                    m_Desolate = true;
                    Talk(eTalks::TalkDesolate);

                    if (Creature* l_Host = sObjectAccessor->GetCreature(*me, instance->GetData64(eCreatures::BossDesolateHost)))
                    {
                        l_Host->SetReactState(REACT_AGGRESSIVE);
                        l_Host->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_SELECTABLE);
                        l_Host->SetVisible(true);
                    }

                    if (Unit* l_Queen = sObjectAccessor->GetUnit(*me, instance->GetData64(eCreatures::BossSoulQueenDejahna)))
                    {
                        if (l_Queen->IsAIEnabled)
                            l_Queen->GetAI()->DoAction(eDesolateHostActions::ActionCancelSummonAdds);
                    }

                    if (IsMythic())
                        DestroySoulFonts();

                    events.CancelEvent(eEvents::EventSummonAdds);
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                BossAI::JustSummoned(p_Summon);
                p_Summon->CastSpell(p_Summon, eSharedSpells::SpellCorporealRealm, true);
            }

            bool CanBeSelectedAsTarget(Unit const* p_Target, SpellInfo const* p_Spell) const override
            {
                if (p_Target == nullptr)
                    return false;

                if (p_Spell == nullptr)
                    return true;

                switch (p_Spell->Id)
                {
                    case eSpells::SpellQuietusAoE:
                    case eSpells::SpellQuietusDmg:
                    {
                        return true;
                    }

                    default: break;
                }

                if (p_Target->IsFriendlyTo(me))
                    return true;

                return !p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
            }

            void KilledUnit(Unit* p_Victim) override
            {
                if (p_Victim == nullptr)
                    return;

                if (p_Victim->IsPlayer())
                    Talk(eTalks::TalkKill);
            }

            void Reset() override
            {
                _Reset();

                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                me->AddUnitState(UNIT_STATE_ROOT);
                me->DespawnAreaTriggersInArea(eSpells::SpellTormentedCriesDmg, 150.f);
                me->DespawnCreaturesInArea(eCreatures::NpcGhastlyBonewarden, 250.f);
                me->DespawnCreaturesInArea(eCreatures::NpcReanimatedTemplar, 250.f);
                m_Desolate = false;
            }

            void CleanUp()
            {
                me->SummonCreatureGroupDespawn(CREATURE_SUMMON_GROUP_RESET);
                me->SummonCreatureGroupDespawn(CREATURE_SUMMON_GROUP_COMBAT);
                me->RemoveAllAreasTrigger();

                events.Reset();
                summons.DespawnAll();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_END, me);
                }
            }

            void JustDied(Unit* /**/) override
            {
                if (Creature* l_Dehjanna = me->FindNearestCreature(eCreatures::BossSoulQueenDejahna, 250.f))
                    me->Kill(l_Dehjanna);

                if (IsLFR())
                    _JustDied();
                else
                {
                    me->ClearLootContainers();

                    Creature* l_Host = me->FindNearestCreature(eCreatures::BossDesolateHost, 200.f, false);

                    if (l_Host != nullptr)
                    {
                        l_Host->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        _JustDied();

                        std::vector<uint32> l_Creatures =
                        {
                            eCreatures::NpcReanimatedTemplar,
                            eCreatures::NpcSoulResidue,
                            eCreatures::NpcGhastlyBonewarden,
                            eCreatures::NpcFallenPriestess,
                        };

                        me->DespawnCreaturesInArea(l_Creatures, 250.f);
                    }
                }
            }

            void ExecuteEvent(uint32 const p_EventId) override
            {
                switch (p_EventId)
                {
                    case eEvents::EventBoneShards:
                    {
                        DoCastVictim(eSpells::SpellBoneShards);
                        events.ScheduleEvent(eEvents::EventBoneShards, 3500);
                        break;
                    }

                    case eEvents::EventCollapsingFissure:
                    {
                        DoCastAOE(eSpells::SpellCollapsingFissure);
                        events.ScheduleEvent(eEvents::EventCollapsingFissure, 30 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventTormentedCries:
                    {
                        Talk(eTalks::TalkTormentedCries);
                        DoCast(me, eSpells::SpellTormentedCries);
                        events.ScheduleEvent(eEvents::EventTormentedCries, 120 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventSpearOfAnguish:
                    {
                        DoCastAOE(eSpells::SpellSpearOfAnguishAOE);
                        events.ScheduleEvent(eEvents::EventSpearOfAnguish, 22 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventSummonAdds:
                    {
                        Position l_Pos;

                        for (uint8 l_Itr = 0; l_Itr < 2; ++l_Itr)
                        {
                            me->GetRandomPoint(g_CenterSummonAdds, frand(7.5f, 12.f), l_Pos);
                            me->SummonCreature(eCreatures::NpcGhastlyBonewarden, l_Pos, TEMPSUMMON_MANUAL_DESPAWN);
                        }

                        me->GetRandomPoint(g_CenterSummonAdds, frand(7.5f, 12.f), l_Pos);
                        me->SummonCreature(eCreatures::NpcReanimatedTemplar, l_Pos, TEMPSUMMON_MANUAL_DESPAWN);

                        events.ScheduleEvent(eEvents::EventSummonAdds, 120 * IN_MILLISECONDS);

                        break;
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (!me->HasAura(eSpells::SpellQuietus))
                    DoCast(me, eSpells::SpellQuietus, true);

                while (uint32 const l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);
            }

            private:
                bool m_Desolate;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new boss_engine_of_souls_AI(p_Me);
        }
};

/// Desolate Host - 119072
class boss_desolate_host : public CreatureScript
{
    public:
        boss_desolate_host() : CreatureScript("boss_desolate_host")
        {}

        enum eTalks
        {
            TalkDead = 4,
        };

        enum eVisualKit
        {
            SpellSpawnVisualKit = 82097,
        };

        struct boss_desolate_host_AI : public BossAI
        {
            explicit boss_desolate_host_AI(Creature* p_Me) : BossAI(p_Me, eData::DataDesolateHost)
            {}

            void Reset() override
            {
                me->SetVisible(false);
                me->RemoveAllAuras();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_SELECTABLE);
                me->SetReactState(REACT_PASSIVE);

                me->SummonCreatureGroupDespawn(CREATURE_SUMMON_GROUP_COMBAT);
                me->RemoveAllAreasTrigger();

                me->ResetLootMode();
                events.Reset();
                summons.DespawnAll();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_END, me);
                }

                me->SummonCreatureGroup(CREATURE_SUMMON_GROUP_RESET);
            }

            bool CheckAchievement()
            {
                Map::PlayerList const& l_Players = me->GetMap()->GetPlayers();
                for (Map::PlayerList::const_iterator l_Itr = l_Players.begin(); l_Itr != l_Players.end(); ++l_Itr)
                {
                    Unit* l_Target = l_Itr->getSource();

                    if (l_Target == nullptr || l_Target->isDead())
                        continue;

                    if (l_Target->HasAura(eSpells::SpellDiminishingSoulBuff))
                        return true;
                }

                return false;
            }

            void CleanUp()
            {
                me->SummonCreatureGroupDespawn(CREATURE_SUMMON_GROUP_RESET);
                me->SummonCreatureGroupDespawn(CREATURE_SUMMON_GROUP_COMBAT);
                me->RemoveAllAreasTrigger();

                events.Reset();
                summons.DespawnAll();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_END, me);
                }
            }

            void JustDied(Unit* /**/) override
            {
                Talk(eTalks::TalkDead);

                if (CheckAchievement())
                    instance->DoCompleteAchievement(eAchievements::GreatSoulGreatPurpose);

                if (Creature* l_Boss = sObjectAccessor->GetCreature(*me, instance->GetData64(eCreatures::BossEngineOfSouls)))
                {
                    if (l_Boss->isDead())
                    {
                        _JustDied();
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                        std::vector<uint32> l_Creatures =
                        {
                            eCreatures::NpcReanimatedTemplar,
                            eCreatures::NpcSoulResidue,
                            eCreatures::NpcGhastlyBonewarden,
                            eCreatures::NpcFallenPriestess,
                        };

                        me->DespawnCreaturesInArea(l_Creatures, 250.f);

                        instance->UpdateEncounterState(EncounterCreditType::ENCOUNTER_CREDIT_KILL_CREATURE, l_Boss->GetEntry(), l_Boss);
                        instance->DoAddBossLootedOnPlayers(me);

                        std::list<Creature*> l_Fonts;
                        me->GetCreatureListWithEntryInGridAppend(l_Fonts, eCreatures::NpcSoulFont, 150.f);
                        me->GetCreatureListWithEntryInGridAppend(l_Fonts, eCreatures::NpcSoulFont2, 150.f);

                        for (Creature* l_Itr : l_Fonts)
                        {
                            if (l_Itr == nullptr)
                                continue;

                            l_Itr->SetVisible(true);
                        }
                    }
                    else
                    {
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        CleanUp();
                    }
                }
            }

            void JustReachedHome() override
            {
                _JustReachedHome();
            }

            void EnterEvadeMode() override
            {
                BossAI::EnterEvadeMode();

                Creature* l_Queen = sObjectAccessor->GetCreature(*me, instance->GetData64(eCreatures::BossSoulQueenDejahna));
                Creature* l_Engine = sObjectAccessor->GetCreature(*me, instance->GetData64(eCreatures::BossEngineOfSouls));

                instance->RemoveFromDamageManager(me, 1);

                if (l_Queen && l_Queen->isDead())
                {
                    if (l_Queen->IsAIEnabled)
                        l_Queen->AI()->EnterEvadeMode();
                }

                if (l_Engine && l_Engine->isDead())
                {
                    if (l_Engine->IsAIEnabled)
                        l_Engine->AI()->EnterEvadeMode();
                }
            }

            void EnterCombat(Unit* /**/) override
            {
                instance->AddToDamageManager(me, 1);

                AddTimedDelayedOperation(IN_MILLISECONDS + 500, [this] () -> void
                {
                    me->SendPlaySpellVisualKit(eVisualKit::SpellSpawnVisualKit, 0);
                    me->HandleEmoteCommand(EMOTE_ONESHOT_BATTLEROAR);

                    me->setActive(true);
                    DoZoneInCombat();
                    if (instance)
                    {
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_START, me);
                    }

                    me->SummonCreatureGroup(CREATURE_SUMMON_GROUP_COMBAT);

                    events.ScheduleEvent(eEvents::EventSunderingDoom, 13 * IN_MILLISECONDS);
                    events.ScheduleEvent(eEvents::EventDommedSummering, 23 * IN_MILLISECONDS);

                });
            }

            void ExecuteEvent(uint32 const p_EventId) override
            {
                switch (p_EventId)
                {
                    case eEvents::EventSunderingDoom:
                    {
                        DoCast(me, eSpells::SpellSunderingDoom);
                        events.ScheduleEvent(eEvents::EventSunderingDoom, 25 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventDommedSummering:
                    {
                        DoCast(me, eSpells::SpellDoomedSummering);
                        events.ScheduleEvent(eEvents::EventDommedSummering, 25 * IN_MILLISECONDS);
                        break;
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                BossAI::UpdateAI(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new boss_desolate_host_AI(p_Me);
        }
};

/// Soul Quuen Dehjanna - 118462
class boss_soul_queen_dehjanna : public CreatureScript
{
    public:
        boss_soul_queen_dehjanna() : CreatureScript("boss_soul_queen_dehjanna")
        {}

        enum eTalks
        {
            TalkAggro,
            TalkSoulbind,
            TalkWailingSouls,
            TalkKilled,
            TalkDead
        };

        struct boss_soul_queen_dehjanna_AI : public BossAI
        {
            explicit boss_soul_queen_dehjanna_AI(Creature* p_Me) : BossAI(p_Me, eData::DataDesolateHost)
            {}

            void Reset() override
            {
                _Reset();

                me->AddUnitState(UNIT_STATE_ROOT);
                DoCast(me, eSharedSpells::SpellSpiritRealm, true);

                me->DespawnCreaturesInArea(eCreatures::NpcFallenPriestess, 250.f);
                me->DespawnCreaturesInArea(eCreatures::NpcSoulResidue, 250.f);
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eDesolateHostActions::ActionCancelSummonAdds)
                    events.CancelEvent(eEvents::EventSummonAdds);
            }

            void UpdateVisibility(Player* p_Player)
            {
                if (p_Player == nullptr)
                    return;

                if (p_Player->isDead())
                    return;

                std::list<Creature*> l_Mobs;

                me->GetCreatureListWithEntryInGridAppend(l_Mobs, eCreatures::NpcFallenPriestess, 150.f);
                me->GetCreatureListWithEntryInGridAppend(l_Mobs, eCreatures::NpcSoulResidue, 150.f);

                if (IsMythic())
                {
                    me->GetCreatureListWithEntryInGridAppend(l_Mobs, eCreatures::NpcReanimatedTemplar, 150.f);
                    me->GetCreatureListWithEntryInGridAppend(l_Mobs, eCreatures::NpcGhastlyBonewarden, 150.f);
                }

                l_Mobs.push_back(me);

                for (Creature* l_Itr : l_Mobs)
                    l_Itr->SendUpdateToPlayer(p_Player);

                Map::PlayerList const& l_PlayerList = instance->instance->GetPlayers();
                if (l_PlayerList.isEmpty())
                    return;

                for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
                {
                    Player* l_Player = l_Iter->getSource();
                    if (!p_Player || l_Player == p_Player)
                        continue;

                    p_Player->SendUpdateToPlayer(l_Player);
                    l_Player->SendUpdateToPlayer(p_Player);
                }
            }

            void SetGUID(uint64 p_Guid, int32 p_Id) override
            {
                if (p_Id == eDesolateHostData::DataVisibility)
                {
                    Player* l_Player = sObjectAccessor->GetPlayer(*me, p_Guid);
                    UpdateVisibility(l_Player);
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                BossAI::JustSummoned(p_Summon);

                if (p_Summon == nullptr)
                    return;

                p_Summon->CastSpell(p_Summon, eSharedSpells::SpellSpiritRealm, true);
                p_Summon->CastSpell(p_Summon, eSharedSpells::SpellSpiritRealmVisual, true);
            }

            bool CanBeAttacked(Unit const* p_Target, SpellInfo const* /**/) const override
            {
                return p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
            }

            void CleanUp()
            {
                me->SummonCreatureGroupDespawn(CREATURE_SUMMON_GROUP_RESET);
                me->SummonCreatureGroupDespawn(CREATURE_SUMMON_GROUP_COMBAT);
                me->RemoveAllAreasTrigger();

                events.Reset();
                summons.DespawnAll();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_END, me);
                }
            }

            void SpawnAdds()
            {
                for (uint8 l_Itr = 0; l_Itr < 2; ++l_Itr)
                    DoSummon(eCreatures::NpcFallenPriestess, g_SpiritRealmInitPos[l_Itr], 60800 * IN_MILLISECONDS);

                for (uint8 l_Itr = 2; l_Itr < 5; ++l_Itr)
                    DoSummon(eCreatures::NpcSoulResidue, g_SpiritRealmInitPos[l_Itr], 60800 * IN_MILLISECONDS);
            }

            void InitializeAI() override
            {
                BossAI::InitializeAI();
                SpawnAdds();
            }

            void JustRespawned() override
            {
                BossAI::JustRespawned();
                SpawnAdds();
            }

            void KilledUnit(Unit* p_Victim) override
            {
                if (p_Victim == nullptr)
                    return;

                if (p_Victim->IsPlayer())
                    Talk(eTalks::TalkKilled);
            }

            void JustDied(Unit* /**/) override
            {
                Talk(eTalks::TalkDead);

                if (IsLFR())
                    _JustDied();
                else
                {
                    CleanUp();
                    me->ClearLootContainers();
                }
            }

            void UpdateVisibilityStartCombat()
            {
                std::list<Creature*> l_Mobs;

                me->GetCreatureListWithEntryInGridAppend(l_Mobs, eCreatures::NpcFallenPriestess, 150.f);
                me->GetCreatureListWithEntryInGridAppend(l_Mobs, eCreatures::NpcSoulResidue, 150.f);

                if (IsMythic())
                {
                    me->GetCreatureListWithEntryInGridAppend(l_Mobs, eCreatures::NpcReanimatedTemplar, 150.f);
                    me->GetCreatureListWithEntryInGridAppend(l_Mobs, eCreatures::NpcGhastlyBonewarden, 150.f);
                }

                l_Mobs.push_back(me);

                for (auto l_Mob : l_Mobs)
                {
                    Map::PlayerList const& l_PlayerList = me->GetMap()->GetPlayers();
                    for (Map::PlayerList::const_iterator l_Itr = l_PlayerList.begin(); l_Itr != l_PlayerList.end(); ++l_Itr)
                    {
                        Player* l_Target = l_Itr->getSource();

                        if (l_Target == nullptr)
                            continue;

                        l_Mob->SendUpdateToPlayer(l_Target);
                    }
                }
            }

            void EnterCombat(Unit* /**/) override
            {
                Talk(eTalks::TalkAggro);

                _EnterCombat();

                m_Desolate = false;

                if (Creature* l_Engine = sObjectAccessor->GetCreature(*me, instance->GetData64(eCreatures::BossEngineOfSouls)))
                    DoZoneInCombat(l_Engine, 250.f);

                std::list<Creature*> l_Adds;

                me->GetCreatureListWithEntryInGridAppend(l_Adds, eCreatures::NpcFallenPriestess, 150.f);
                me->GetCreatureListWithEntryInGridAppend(l_Adds, eCreatures::NpcSoulResidue, 150.f);

                for (Creature* l_Itr : l_Adds)
                {
                    if (l_Itr == nullptr)
                        continue;

                    DoZoneInCombat(l_Itr, 150.f);
                }

                UpdateVisibilityStartCombat();

                DoCast(me, eSpells::SpellQuietus, true);

                events.ScheduleEvent(eEvents::EventCrushMind, 3 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventSoulbind, 14 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventSummonAdds, 120 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventWailingSouls, 60 * IN_MILLISECONDS);

                if (IsHeroicOrMythic())
                    events.ScheduleEvent(eEvents::EventWither, 25 * IN_MILLISECONDS);
            }

            void DamageTaken(Unit* /**/, uint32 & p_Dmg, SpellInfo const* /**/) override
            {
                if (HealthBelowPct(30) && !m_Desolate)
                {
                    m_Desolate = true;

                    Creature* l_Engine = sObjectAccessor->GetCreature(*me, instance->GetData64(eCreatures::BossEngineOfSouls));

                    if (l_Engine && l_Engine->IsAIEnabled)
                        l_Engine->GetAI()->DamageTaken(nullptr, p_Dmg, nullptr);

                    events.CancelEvent(eEvents::EventSummonAdds);
                }
            }

            void ExecuteEvent(uint32 const p_EventId) override
            {
                switch (p_EventId)
                {
                    case eEvents::EventSoulbind:
                    {
                        Talk(eTalks::TalkSoulbind);
                        DoCast(me, eSpells::SpellSoulbind);
                        events.ScheduleEvent(eEvents::EventSoulbind, 25 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventWailingSouls:
                    {
                        Talk(eTalks::TalkWailingSouls);
                        DoCast(me, eSpells::SpellWailingSouls);
                        events.ScheduleEvent(eEvents::EventWailingSouls, 120 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventCrushMind:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_TOPAGGRO, 0, 0.f, true))
                            DoCast(l_Target, eSpells::SpellCrushMind);

                        events.ScheduleEvent(eEvents::EventCrushMind, 3 * IN_MILLISECONDS);

                        break;
                    }

                    case eEvents::EventWither:
                    {
                        Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.f, true);

                        if (l_Target == nullptr)
                            l_Target = SelectTarget(SELECT_TARGET_TOPAGGRO, 0, 0.f, true);

                        if (l_Target != nullptr)
                            DoCast(l_Target, eSpells::SpellWither);

                        events.ScheduleEvent(eEvents::EventWither, urand(25, 50) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventSummonAdds:
                    {
                        Position l_Pos;

                        for (uint8 l_Itr = 0; l_Itr < 2; ++l_Itr)
                        {
                            me->GetRandomPoint(g_CenterSummonAdds, frand(7.5f, 12.f), l_Pos);
                            me->SummonCreature(eCreatures::NpcFallenPriestess, l_Pos, TEMPSUMMON_MANUAL_DESPAWN);
                        }

                        for (uint8 l_Itr = 0; l_Itr < 3; ++l_Itr)
                        {
                            me->GetRandomPoint(g_CenterSummonAdds, frand(7.5f, 12.f), l_Pos);
                            me->SummonCreature(eCreatures::NpcSoulResidue, l_Pos, TEMPSUMMON_MANUAL_DESPAWN);
                        }

                        events.ScheduleEvent(eEvents::EventSummonAdds, 60 * IN_MILLISECONDS);


                        break;
                    }
                }
            }

            bool CanBeSelectedAsTarget(Unit const* p_Target, SpellInfo const* p_Spell) const override
            {
                if (p_Target == nullptr)
                    return false;

                if (p_Spell == nullptr)
                    return true;

                if (!p_Target->IsPlayer())
                    return true;

                switch (p_Spell->Id)
                {
                    case eSpells::SpellSoulbindCorporealSearcher:
                    case eSpells::SpellSoulbindSpiritSearcher:
                    case eSpells::SpellQuietusAoE:
                    case eSpells::SpellQuietusDmg:
                    {
                        return true;
                    }

                    default: break;
                }

                if (p_Target->IsFriendlyTo(me))
                    return true;

                return p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (!me->HasAura(eSpells::SpellQuietus))
                    DoCast(me, eSpells::SpellQuietus, true);

                while (uint32 const l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);
            }

            bool m_Desolate;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new boss_soul_queen_dehjanna_AI(p_Me);
        }
};

/// Reanimated Templar - 118715
class npc_desolate_host_reanimate_templar : public CreatureScript
{
    public:
        npc_desolate_host_reanimate_templar() : CreatureScript("npc_desolate_host_reanimate_templar")
        {}

        enum eEvents
        {
            EventRupturingSlam  = 1,
        };

        struct npc_desolate_host_reanimate_templar_AI : public ScriptedAI
        {
            explicit npc_desolate_host_reanimate_templar_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                m_ToSpiritRealm = false;
            }

            bool CanAIAttack(Unit const* p_Target) const override
            {
                if (IsMythic())
                {
                    if (me->HasAura(eSpells::SpellSpiritualBarrierPhysic))
                        return !p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
                    else
                        return p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
                }
                else
                    return !p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
            }

            bool CanBeSelectedAsTarget(Unit const* p_Target, SpellInfo const* p_Spell) const override
            {
                if (p_Target == nullptr)
                    return false;

                if (p_Spell == nullptr)
                    return true;

                if (p_Target->IsFriendlyTo(me))
                    return true;

                if (IsMythic())
                {
                    if (me->HasAura(eSpells::SpellSpiritualBarrierPhysic))
                        return !p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
                    else
                        return p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
                }
                else
                    return !p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
            }

            bool CanBeAttacked(Unit const* p_Target, SpellInfo const* /**/) const override
            {
                if (p_Target == nullptr)
                    return false;

                if (!p_Target->IsPlayer())
                    return true;

                if (IsMythic())
                {
                    if (me->HasAura(eSpells::SpellSpiritualBarrierPhysic))
                        return !p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
                    else
                        return p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
                }
                else
                    return !p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
            }

            void DamageTaken(Unit* /**/, uint32 & p_Dmg, SpellInfo const* /**/) override
            {
                if (HealthBelowPct(50) && !m_Shield && IsHeroicOrMythic())
                {
                    m_Shield = true;
                    DoCast(me, eSpells::SpellBoneCageArmor, true);
                }
            }

            void SetData(uint32 p_Type, uint32 p_State) override
            {
                if (p_Type == eDesolateHostData::DataVisibility)
                {
                    if (p_State == ePhasesMasks::PhaseSpiritRealm)
                    {
                        m_ToSpiritRealm = true;
                        me->RemoveAllAuras();
                        DoCast(me, eSharedSpells::SpellSpiritRealm, true);
                        DoCast(me, eSpells::SpellSpiritualBarrierSpirit, true);
                    }
                }
            }

            uint32 GetData(uint32 p_Type) override
            {
                if (p_Type == eDesolateHostData::DataVisibility)
                    return m_ToSpiritRealm ? 1 : 2;
                else if (p_Type == eDesolateHostData::DataRupturingSlam)
                {
                    eOrientations l_Ret = m_SlamOri;
                    m_SlamOri = eOrientations(m_SlamOri + 1);
                    return l_Ret;
                }

                return 0;
            }

            void EnterCombat(Unit* /**/) override
            {
                events.ScheduleEvent(eEvents::EventRupturingSlam, 10 * IN_MILLISECONDS);
            }

            void Reset() override
            {
                events.Reset();
                m_Shield = false;
                summons.DespawnAll();

                if (IsMythic())
                {
                    if (!m_ToSpiritRealm)
                        DoCast(me, eSpells::SpellBoundEssence, true);
                    else
                    {
                        me->RemoveAllAuras();
                        DoCast(me, eSharedSpells::SpellSpiritRealm, true);
                    }
                }
            }

            void JustDied(Unit* /**/) override
            {
                events.Reset();
                summons.DespawnAll();
            }

            void ExecuteEvent(uint32 const p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventRupturingSlam:
                    {
                        m_SlamOri = eOrientations::North;
                        DoCast(me, eSpells::SpellRupturingSlam);
                        events.ScheduleEvent(eEvents::EventRupturingSlam, 25 * IN_MILLISECONDS);
                        break;
                    }

                    default: break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                ExecuteEvent(events.ExecuteEvent());

                DoMeleeAttackIfReady();
            }

            private:
                eOrientations m_SlamOri;
                bool m_Shield;
                bool m_ToSpiritRealm;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_desolate_host_reanimate_templar_AI(p_Me);
        }
};

/// Ghastly Bonewarden - 118728
class npc_desolate_host_ghastly_bonewarden : public CreatureScript
{
    public:
        npc_desolate_host_ghastly_bonewarden() : CreatureScript("npc_desolate_host_ghastly_bonewarden")
        {}

        enum eEvents
        {
            EventGraspingDarkness   = 1,
        };

        struct npc_desolate_host_ghastly_bonewarden_AI : public ScriptedAI
        {
            explicit npc_desolate_host_ghastly_bonewarden_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                m_ToSpiritRealm = false;
            }

            bool CanAIAttack(Unit const* p_Target) const override
            {
                if (IsMythic())
                {
                    if (me->HasAura(eSpells::SpellSpiritualBarrierPhysic))
                        return !p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
                    else
                        return p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
                }
                else
                    return !p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
            }

            bool CanBeSelectedAsTarget(Unit const* p_Target, SpellInfo const* p_Spell) const override
            {
                if (p_Target == nullptr)
                    return false;

                if (p_Spell == nullptr)
                    return true;

                if (p_Target->IsFriendlyTo(me))
                    return true;

                if (IsMythic())
                {
                    if (me->HasAura(eSpells::SpellSpiritualBarrierPhysic))
                        return !p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
                    else
                        return p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
                }
                else
                    return !p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
            }

            bool CanBeAttacked(Unit const* p_Target, SpellInfo const* /**/) const override
            {
                if (p_Target == nullptr)
                    return false;

                if (!p_Target->IsPlayer())
                    return true;

                if (IsMythic())
                {
                    if (me->HasAura(eSpells::SpellSpiritualBarrierPhysic))
                        return !p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
                    else
                        return p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
                }
                else
                    return !p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
            }

            void SetData(uint32 p_Type, uint32 p_State) override
            {
                if (p_Type == eDesolateHostData::DataVisibility)
                {
                    if (p_State == ePhasesMasks::PhaseSpiritRealm)
                    {
                        m_ToSpiritRealm = true;
                        me->RemoveAllAuras();
                        DoCast(me, eSharedSpells::SpellSpiritRealm, true);
                    }
                }
            }

            uint32 GetData(uint32 p_Type) override
            {
                if (p_Type == eDesolateHostData::DataVisibility)
                    return m_ToSpiritRealm ? 1 : 2;

                return 0;
            }

            void Reset() override
            {
                m_Shield = false;
                events.Reset();

                if (IsMythic())
                {
                    if (!m_ToSpiritRealm)
                        DoCast(me, eSpells::SpellBoundEssence, true);
                    else
                    {
                        me->RemoveAllAuras();
                        DoCast(me, eSharedSpells::SpellSpiritRealm, true);
                    }
                }
            }

            void DamageTaken(Unit* /**/, uint32 & p_Dmg, SpellInfo const* /**/) override
            {
                if (HealthBelowPct(50) && !m_Shield && IsHeroicOrMythic())
                {
                    m_Shield = true;
                    DoCast(me, eSpells::SpellBoneCageArmor, true);
                }
            }

            void JustDied(Unit* /**/) override
            {
                events.Reset();
            }

            void EnterCombat(Unit* /**/) override
            {
                DoZoneInCombat(nullptr, 250.f);
                events.ScheduleEvent(eEvents::EventGraspingDarkness, urand(10, 20) * IN_MILLISECONDS);
            }

            void ExecuteEvent(uint32 const p_EventId)
            {
                if (p_EventId == eEvents::EventGraspingDarkness)
                {
                    if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                        DoCast(l_Target, eSpells::SpellGraspingDarkness);
                    else
                        DoCastVictim(eSpells::SpellGraspingDarkness);

                    events.ScheduleEvent(eEvents::EventGraspingDarkness, urand(10, 20) * IN_MILLISECONDS);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                ExecuteEvent(events.ExecuteEvent());

                DoMeleeAttackIfReady();
            }

            private:
                bool m_Shield;
                bool m_ToSpiritRealm;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_desolate_host_ghastly_bonewarden_AI(p_Me);
        }
};

/// Soul Residue - 118730
class npc_desolate_host_soul_residue : public CreatureScript
{
    public:
        npc_desolate_host_soul_residue() : CreatureScript("npc_desolate_host_soul_residue")
        {}

        enum eEvents
        {
            EventSoulRot    = 1,
        };

        struct npc_desolate_host_soul_residue_AI : public ScriptedAI
        {
            explicit npc_desolate_host_soul_residue_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                m_ToCorporeal = false;
            }

            bool CanAIAttack(Unit const* p_Target) const override
            {
                if (IsMythic())
                {
                    if (me->HasAura(eSpells::SpellSpiritualBarrierPhysic))
                        return !p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
                    else
                        return p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
                }
                else
                    return p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
            }

            void ExecuteEvent(uint32 const p_EventId)
            {
                if (p_EventId == eEvents::EventSoulRot)
                {
                    DoCast(me, eSpells::SpellSoulRot, true);
                    DoCast(me, eSpells::SpellSoulRotAT, true);
                    events.ScheduleEvent(eEvents::EventSoulRot, 1500);
                }
            }

            void SetData(uint32 p_Type, uint32 p_State) override
            {
                if (p_Type == eDesolateHostData::DataVisibility)
                {
                    if (p_State == ePhasesMasks::PhaseCorporealRealm)
                    {
                        m_ToCorporeal = true;
                        me->RemoveAllAuras();
                        DoCast(me, eSpells::SpellSpiritualBarrierPhysic, true);
                    }
                }
            }

            uint32 GetData(uint32 p_Type) override
            {
                if (p_Type == eDesolateHostData::DataVisibility)
                    return m_ToCorporeal ? 1 : 2;

                return 0;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 const l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }

            bool CanBeSelectedAsTarget(Unit const* p_Target, SpellInfo const* p_Spell) const override
            {
                if (p_Target == nullptr)
                    return false;

                if (p_Spell == nullptr)
                    return true;

                if (p_Target->IsFriendlyTo(me))
                    return true;

                if (IsMythic())
                {
                    if (me->HasAura(eSpells::SpellSpiritualBarrierSpirit))
                        return p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
                    else
                        return !p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
                }
                else
                    return p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
            }

            bool CanBeAttacked(Unit const* p_Target, SpellInfo const* /**/) const override
            {
                if (p_Target == nullptr)
                    return false;

                if (!p_Target->IsPlayer())
                    return true;

                if (IsMythic())
                {
                    if (me->HasAura(eSpells::SpellSpiritualBarrierSpirit))
                        return p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
                    else
                        return !p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
                }
                else
                    return p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
            }

            void Reset() override
            {
                ScriptedAI::Reset();

                events.Reset();

                DoCast(me, eSharedSpells::SpellSpiritRealm, true);

                if (IsMythic())
                {
                    if (!m_ToCorporeal)
                        DoCast(me, eSpells::SpellBoundEssence, true);
                    else
                    {
                        me->RemoveAllAuras();
                        DoCast(me, eSpells::SpellSpiritualBarrierPhysic, true);
                    }
                }
            }

            void EnterCombat(Unit* /**/) override
            {
                DoZoneInCombat(me, 250.f);
                events.ScheduleEvent(eEvents::EventSoulRot, 500);

                if (IsMythic())
                    DoCast(me, eSpells::SpellUnreleting, true);
            }

            void JustDied(Unit* /**/) override
            {
                events.Reset();

                DoCast(me, eSpells::SpellSoulEruption, true);
            }

            bool m_ToCorporeal;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_desolate_host_soul_residue_AI(p_Me);
        }
};

/// Fallen Priestess - 118729
class npc_desolate_host_fallen_priestess : public CreatureScript
{
    public:
        npc_desolate_host_fallen_priestess() : CreatureScript("npc_desolate_host_fallen_priestess")
        {}

        enum eEvents
        {
            EventShatteringScream = 1,
            EventSpiritChains,
        };

        struct npc_desolate_host_fallen_priestess_AI : public ScriptedAI
        {
            explicit npc_desolate_host_fallen_priestess_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                m_ToCorporeal = false;
            }

            bool CanAIAttack(Unit const* p_Target) const override
            {
                if (IsMythic())
                {
                    if (me->HasAura(eSpells::SpellSpiritualBarrierPhysic))
                        return !p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
                    else
                        return p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
                }
                else
                    return p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
            }

            void Reset() override
            {
                events.Reset();

                DoCast(me, eSharedSpells::SpellSpiritRealm, true);

                if (IsMythic())
                {
                    if (!m_ToCorporeal)
                        DoCast(me, eSpells::SpellBoundEssence, true);
                    else
                    {
                        me->RemoveAllAuras();
                        DoCast(me, eSpells::SpellSpiritualBarrierPhysic, true);
                    }
                }
            }

            void JustDied(Unit* /**/) override
            {
                events.Reset();
            }

            void SetData(uint32 p_Type, uint32 p_State) override
            {
                if (p_Type == eDesolateHostData::DataVisibility)
                {
                    if (p_State == ePhasesMasks::PhaseCorporealRealm)
                    {
                        m_ToCorporeal = true;
                        me->RemoveAllAuras();
                        DoCast(me, eSpells::SpellSpiritualBarrierPhysic, true);
                    }
                }
            }

            uint32 GetData(uint32 p_Type) override
            {
                if (p_Type == eDesolateHostData::DataVisibility)
                    return m_ToCorporeal ? 1 : 2;

                return 0;
            }

            void EnterCombat(Unit* /**/) override
            {
                DoZoneInCombat(nullptr, 250.f);

                events.ScheduleEvent(eEvents::EventShatteringScream, urand(8, 10) * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventSpiritChains, urand(12, 16) * IN_MILLISECONDS);
            }

            void ExecuteEvent(uint32 const p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventShatteringScream:
                    {
                        Unit* l_Target = SelectRangedTarget(true);

                        if (l_Target == nullptr)
                            l_Target = SelectMeleeTarget(false);

                        if (l_Target == nullptr)
                            l_Target = me->getVictim();

                        if (l_Target != nullptr)
                            DoCast(l_Target, eSpells::SpellShatteringScream);

                        events.ScheduleEvent(eEvents::EventShatteringScream, urand(8, 10) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventSpiritChains:
                    {
                        Unit* l_Target = SelectRangedTarget(true);

                        if (l_Target == nullptr)
                            l_Target = SelectMeleeTarget(false);

                        if (l_Target == nullptr)
                            l_Target = me->getVictim();

                        if (l_Target != nullptr)
                            DoCast(l_Target, eSpells::SpellSpiritChains);

                        events.ScheduleEvent(eEvents::EventSpiritChains, urand(12, 16) * IN_MILLISECONDS);
                        break;
                    }
                    default: break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 const l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }

            bool CanBeSelectedAsTarget(Unit const* p_Target, SpellInfo const* p_Spell) const override
            {
                if (p_Target == nullptr)
                    return false;

                if (p_Spell == nullptr)
                    return true;

                if (p_Target->IsFriendlyTo(me))
                    return true;

                if (IsMythic())
                {
                    if (me->HasAura(eSpells::SpellSpiritualBarrierSpirit))
                        return p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
                    else
                        return !p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
                }
                else
                    return p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
            }

            bool CanBeAttacked(Unit const* p_Target, SpellInfo const* /**/) const override
            {
                if (p_Target == nullptr)
                    return false;

                if (!p_Target->IsPlayer())
                    return true;

                if (IsMythic())
                {
                    if (me->HasAura(eSpells::SpellSpiritualBarrierSpirit))
                        return p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
                    else
                        return !p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
                }
                else
                    return p_Target->HasAura(eSharedSpells::SpellSpiritRealm);
            }

            bool m_ToCorporeal;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_desolate_host_fallen_priestess_AI(p_Me);
        }
};

/// Soul Font - 118701, 118699
class npc_desolate_host_soul_font : public CreatureScript
{
    public:
        npc_desolate_host_soul_font() : CreatureScript("npc_desolate_host_soul_font")
        {}

        struct npc_desolate_host_soul_font_AI : public ScriptedAI
        {
            explicit npc_desolate_host_soul_font_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void OnSpellClick(Unit* p_Clicker) override
            {
                if (IsMythic())
                {
                    if (p_Clicker->HealthAbovePct(50) && p_Clicker->isInCombat())
                        return;
                }

                if (p_Clicker->HasAura(eSpells::SpellSpearOfAnguishAbsorb) || p_Clicker->HasAura(eSpells::SpellSpiritChains))
                    return;

                if (p_Clicker->HasAura(eSharedSpells::SpellSpiritRealm))
                    p_Clicker->RemoveAurasDueToSpell(eSharedSpells::SpellSpiritRealm);
                else
                    p_Clicker->CastSpell(p_Clicker, eSharedSpells::SpellSpiritRealm, true);
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_desolate_host_soul_font_AI(p_Me);
        }
};

/// Tormented Cries Dummy - 118924
class npc_desolate_host_tormented_cries_dummy : public CreatureScript
{
    public:
        npc_desolate_host_tormented_cries_dummy() : CreatureScript("npc_desolate_host_tormented_cries_dummy")
        {}

        struct npc_desolate_host_tormented_cries_dummy_AI : public ScriptedAI
        {
            explicit npc_desolate_host_tormented_cries_dummy_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                me->SetVisible(false);
                me->SetReactState(REACT_PASSIVE);
            }

            void Reset() override
            {
                DoCastAOE(eSpells::SpellTormentedCriesAOE);
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_desolate_host_tormented_cries_dummy_AI(p_Me);
        }
};

/// Spirit Realm - 235621
class spell_desolate_host_spirit_realm : public SpellScriptLoader
{
    public:
        spell_desolate_host_spirit_realm() : SpellScriptLoader("spell_desolate_host_spirit_realm")
        {}

        class spell_desolate_host_spirit_realm_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_desolate_host_spirit_realm_AuraScript);

            enum eSpells
            {
                SpiritRealm     = 235620,
                CorporateRealm  = 235113
            };

            void UpdateVisibilityPlayers(Player* p_Owner)
            {
                if (InstanceScript* l_Instance = p_Owner->GetInstanceScript())
                {
                    Creature* l_Boss = sObjectAccessor->GetCreature(*p_Owner, l_Instance->GetData64(eCreatures::BossEngineOfSouls));

                    if (l_Boss != nullptr)
                    {
                        if (l_Boss->IsAIEnabled)
                            l_Boss->GetAI()->SetGUID(p_Owner->GetGUID(), eDesolateHostData::DataVisibility);
                    }

                    l_Boss = sObjectAccessor->GetCreature(*p_Owner, l_Instance->GetData64(eCreatures::BossSoulQueenDejahna));

                    if (l_Boss != nullptr)
                    {
                        if (l_Boss->IsAIEnabled)
                            l_Boss->GetAI()->SetGUID(p_Owner->GetGUID(), eDesolateHostData::DataVisibility);
                    }
                }
            }

            void HandleAfterApply(AuraEffect const* /**/, AuraEffectHandleModes /**/)
            {
                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner)
                    return;

                if (Player* l_Player = l_Owner->ToPlayer())
                {
                    l_Player->SetPhaseMask(ePhasesMasks::PhaseSpiritRealm, true);

                    l_Player->RemoveAura(eSharedSpells::SpellCorporealRealm);

                    UpdateVisibilityPlayers(l_Player);

                    l_Player->AddDelayedEvent([l_Player]() -> void
                    {
                        l_Player->CastSpell(l_Player, eSharedSpells::SpellSpiritRealmVisual, true);
                    }, 100);
                }
            }

            void HandleAfterRemove(AuraEffect const* /**/, AuraEffectHandleModes /**/)
            {
                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner)
                    return;

                if (Player* l_Player = l_Owner->ToPlayer())
                {
                    GetUnitOwner()->SetPhaseMask(PHASEMASK_NORMAL, true);

                    l_Player->RemoveAura(eSharedSpells::SpellSpiritRealmVisual);

                    UpdateVisibilityPlayers(l_Player);

                    l_Player->CastSpell(l_Player, eSharedSpells::SpellCorporealRealm, true);
                }
                else
                    GetUnitOwner()->SetPhaseMask(ePhasesMasks::PhaseCorporealRealm, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_desolate_host_spirit_realm_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectApply += AuraEffectRemoveFn(spell_desolate_host_spirit_realm_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_desolate_host_spirit_realm_AuraScript();
        }
};

/// Doomed Sundering - 236567
/// Sundering Doom - 236563
class spell_desolate_host_doom_spells : public SpellScriptLoader
{
    public:
        spell_desolate_host_doom_spells() : SpellScriptLoader("spell_desolate_host_doom_spells")
        {}

        class spell_desolate_host_doom_spells_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_desolate_host_doom_spells_SpellScript);

            bool Load() override
            {
                m_TargetCount = 0;
                return true;
            }

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                m_TargetCount = p_Targets.size();
            }

            void HandleAfterCast()
            {
                if (GetCaster() == nullptr)
                    return;

                if (m_TargetCount == 0)
                {
                    if (Aura* l_Buff = GetCaster()->GetAura(eSpells::SpellTorment))
                        l_Buff->ModStackAmount(5);
                    else
                        GetCaster()->CastCustomSpell(eSpells::SpellTorment, SpellValueMod::SPELLVALUE_AURA_STACK, 5, GetCaster());
                }
                else
                    GetCaster()->CastSpell(GetCaster(), eSpells::SpellTorment, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_desolate_host_doom_spells_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                AfterCast += SpellCastFn(spell_desolate_host_doom_spells_SpellScript::HandleAfterCast);
            }

            size_t m_TargetCount;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_desolate_host_doom_spells_SpellScript();
        }
};

///< Doomed Sundering Corporeal - 236568
///< Sundering Doom Spirit - 236564
class spell_desolate_host_other_realm_spells : public SpellScriptLoader
{
    public:
        spell_desolate_host_other_realm_spells() : SpellScriptLoader("spell_desolate_host_other_realm_spells")
        {}

        class spell_desolate_host_other_realm_spells_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_desolate_host_other_realm_spells_SpellScript);

            void HandleOnHit(SpellEffIndex /**/)
            {
                if (GetHitUnit() == nullptr || GetCaster() == nullptr)
                    return;

                float l_Dist = std::min(std::max(1.0f, GetCaster()->GetDistance2d(GetHitUnit()) - 10.f), 18.0f);

                if (l_Dist > 1.0f)
                    SetHitDamage(GetHitDamage() * (1.f - (5 * l_Dist / 100.f)));
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_desolate_host_other_realm_spells_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_desolate_host_other_realm_spells_SpellScript();
        }
};

/// Quietus - 236673, 242620
class spell_desolate_host_quietus : public SpellScriptLoader
{
    public:
        spell_desolate_host_quietus() : SpellScriptLoader("spell_desolate_host_quietus")
        {}

        class spell_desolate_host_quietus_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_desolate_host_quietus_SpellScript);

            SpellCastResult HandleCheckCast()
            {
                if (GetCaster() == nullptr)
                    return SpellCastResult::SPELL_FAILED_DONT_REPORT;

                Unit* l_Caster = GetCaster();

                std::list<HostileReference*>& l_Targets = l_Caster->getThreatManager().getThreatList();

                bool l_Result = true;

                if (l_Caster->GetEntry() == eCreatures::BossSoulQueenDejahna)
                {
                    for (HostileReference* l_Itr : l_Targets)
                    {
                        Unit* l_Target = l_Itr->getTarget();

                        if (l_Target == nullptr)
                            continue;

                        if (l_Target->HasAura(eSharedSpells::SpellSpiritRealm))
                        {
                            l_Result = false;
                            break;
                        }
                    }
                }
                else if (l_Caster->GetEntry() == eCreatures::BossEngineOfSouls)
                {
                    for (HostileReference* l_Itr : l_Targets)
                    {
                        Unit* l_Target = l_Itr->getTarget();

                        if (l_Target == nullptr)
                            continue;

                        if (!l_Target->HasAura(eSharedSpells::SpellSpiritRealm))
                        {
                            l_Result = false;
                            break;
                        }
                    }
                }
                else
                    l_Result = false;

                if (l_Result)
                    return SpellCastResult::SPELL_CAST_OK;

                return SpellCastResult::SPELL_FAILED_DONT_REPORT;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_desolate_host_quietus_SpellScript::HandleCheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_desolate_host_quietus_SpellScript();
        }
};

/// Shattering Scream - 236772
class spell_desolate_host_shattering_scream : public SpellScriptLoader
{
    public:
        spell_desolate_host_shattering_scream() : SpellScriptLoader("spell_desolate_host_shattering_scream")
        {}

        class spell_desolate_host_shattering_scream_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_desolate_host_shattering_scream_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if([] (WorldObject*& p_Itr) -> bool
                {
                    return p_Itr->IsPlayer();
                });
            }

            void HandleOnHit(SpellEffIndex /**/)
            {
                if (GetHitUnit() == nullptr)
                    return;

                GetHitUnit()->RemoveAurasDueToSpell(eSpells::SpellBoneCageArmor);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_desolate_host_shattering_scream_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_desolate_host_shattering_scream_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_desolate_host_shattering_scream_SpellScript();
        }
};

/// Shattering Scream Dot - 236515
class spell_desolate_host_shattering_scream_dot : public SpellScriptLoader
{
    public:
        spell_desolate_host_shattering_scream_dot() : SpellScriptLoader("spell_desolate_host_shattering_scream_dot")
        {}

        class spell_desolate_host_shattering_scream_dot_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_desolate_host_shattering_scream_dot_AuraScript);

            void HandleOnPeriodic(AuraEffect const* /**/)
            {
                if (GetUnitOwner() == nullptr)
                    return;

                if (GetStackAmount() > 4)
                {
                    GetUnitOwner()->CastSpell(GetUnitOwner(), eSpells::SpellShatteringScreamDmg, true);
                    Remove();
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_desolate_host_shattering_scream_dot_AuraScript::HandleOnPeriodic, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_desolate_host_shattering_scream_dot_AuraScript();
        }
};

/// Soulbind Buff - 236459
class spell_desolate_host_soulbind_buff : public SpellScriptLoader
{
    public:
        spell_desolate_host_soulbind_buff() : SpellScriptLoader("spell_desolate_host_soulbind_buff")
        {}

        class spell_desolate_host_soulbind_buff_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_desolate_host_soulbind_buff_AuraScript);

            void HandleAfterApply(AuraEffect const* /**/, AuraEffectHandleModes /**/)
            {
                if (GetUnitOwner() == nullptr)
                    return;

                GetUnitOwner()->SetPhaseMask(ePhasesMasks::PhaseCorporealRealm, true);
            }

            void HandleAfterRemove(AuraEffect const* /**/, AuraEffectHandleModes /**/)
            {
                if (GetUnitOwner() == nullptr)
                    return;

                GetUnitOwner()->SetPhaseMask(GetUnitOwner()->HasAura(eSharedSpells::SpellSpiritRealm) ? PhaseSpiritRealm : PHASEMASK_NORMAL, true);
            }

            void HandleOnPeriodic(AuraEffect const* /**/)
            {
                if (GetUnitOwner() == nullptr)
                    return;

                GetUnitOwner()->CastSpell(GetUnitOwner(), eSpells::SpellSoulbindDmg, true);
                GetUnitOwner()->CastSpell(GetUnitOwner(), eSpells::SpellSoulbindBuffSearcher, true);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_desolate_host_soulbind_buff_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_desolate_host_soulbind_buff_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_desolate_host_soulbind_buff_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_desolate_host_soulbind_buff_AuraScript();
        }
};

/// Soulbind Searcher - 236465
class spell_desolate_host_soulbind_searcher : public SpellScriptLoader
{
    public:
        spell_desolate_host_soulbind_searcher() : SpellScriptLoader("spell_desolate_host_soulbind_searcher")
        {}

        class spell_desolate_host_soulbind_searcher_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_desolate_host_soulbind_searcher_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty() || GetCaster() == nullptr)
                    return;

                p_Targets.remove_if([] (WorldObject*& p_Itr) -> bool
                {
                    if (Unit* l_Target = p_Itr->ToUnit())
                        return !l_Target->HasAura(eSpells::SpellSoulbindBuff);

                    return !p_Itr->IsPlayer();
                });

                if (!p_Targets.empty())
                    GetCaster()->RemoveAurasDueToSpell(eSpells::SpellSoulbindBuff);
            }

            void HandleOnHit(SpellEffIndex /**/)
            {
                if (!GetHitUnit())
                    return;

                GetHitUnit()->RemoveAurasDueToSpell(eSpells::SpellSoulbindBuff);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_desolate_host_soulbind_searcher_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnEffectHitTarget += SpellEffectFn(spell_desolate_host_soulbind_searcher_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_desolate_host_soulbind_searcher_SpellScript();
        }
};

/// Bound Essence - 236351
class spell_desolate_host_bound_essence : public SpellScriptLoader
{
    public:
        spell_desolate_host_bound_essence() : SpellScriptLoader("spell_desolate_host_bound_essence")
        {}

        class spell_desolate_host_bound_essence_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_desolate_host_bound_essence_AuraScript);

            void HandleAfterRemove(AuraEffect const* /**/, AuraEffectHandleModes /**/)
            {
                if (GetUnitOwner() == nullptr)
                    return;

                if (AuraApplication const* l_App = GetTargetApplication())
                    if (l_App->GetRemoveMode() != AuraRemoveMode::AURA_REMOVE_BY_DEATH)
                        return;

                Unit* l_Owner = GetUnitOwner();

                if (InstanceScript* l_Instance = l_Owner->GetInstanceScript())
                {
                    if (l_Instance->GetBossState(eData::DataDesolateHost) != IN_PROGRESS)
                        return;

                    GetUnitOwner()->CastSpell(GetUnitOwner(), eSpells::SpellBoundEssenceTrigger, true);
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_desolate_host_bound_essence_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_desolate_host_bound_essence_AuraScript();
        }
};

/// Bound Essence Trigger - 238585
class spell_desolate_host_bound_essence_trigger : public SpellScriptLoader
{
    public:
        spell_desolate_host_bound_essence_trigger() : SpellScriptLoader("spell_desolate_host_bound_essence_trigger")
        {}

        class spell_desolate_host_bound_essence_trigger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_desolate_host_bound_essence_trigger_SpellScript);

            void HandleOnHit(SpellEffIndex /**/)
            {
                if (GetCaster() == nullptr || GetHitDest() == nullptr)
                    return;

                Unit* l_Owner = GetCaster();
                WorldLocation* l_Dest = GetHitDest();

                uint32 l_Entry = l_Owner->GetEntry();
                Creature* l_Summon = nullptr;
                uint32 l_Phase = 0;

                if (l_Owner->IsAIEnabled)
                {
                    if (l_Owner->GetAI()->GetData(eDesolateHostData::DataVisibility) == 1)
                        return;
                }

                if (l_Entry == eCreatures::NpcFallenPriestess ||
                    l_Entry == eCreatures::NpcSoulResidue)
                    l_Phase = ePhasesMasks::PhaseCorporealRealm;
                else if (l_Entry == eCreatures::NpcReanimatedTemplar ||
                    l_Entry == eCreatures::NpcGhastlyBonewarden)
                    l_Phase = ePhasesMasks::PhaseSpiritRealm;

                InstanceScript* l_Instance = l_Owner->GetInstanceScript();

                if (l_Instance == nullptr)
                    return;

                if (l_Phase == ePhasesMasks::PhaseSpiritRealm)
                {
                    Creature* l_Boss = sObjectAccessor->GetCreature(*l_Owner, l_Instance->GetData64(eCreatures::BossSoulQueenDejahna));

                    if (l_Boss)
                        l_Summon = l_Boss->SummonCreature(l_Owner->GetEntry(), *l_Dest, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                }
                else
                {
                    Creature* l_Boss = sObjectAccessor->GetCreature(*l_Owner, l_Instance->GetData64(eCreatures::BossEngineOfSouls));

                    if (l_Boss)
                        l_Summon = l_Boss->SummonCreature(l_Owner->GetEntry(), *l_Dest, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                }

                if (l_Summon != nullptr)
                {
                    l_Summon->SetHomePosition(g_DesolateCenterRoom);

                    if (l_Summon->IsAIEnabled)
                        l_Summon->GetAI()->SetData(eDesolateHostData::DataVisibility, l_Phase);
                }
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_desolate_host_bound_essence_trigger_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_desolate_host_bound_essence_trigger_SpellScript();
        }
};

/// Tormented Cries Mark - 238018
class spell_desolate_host_tormented_cries_mark : public SpellScriptLoader
{
    public:
        spell_desolate_host_tormented_cries_mark() : SpellScriptLoader("spell_desolate_host_tormented_cries_mark")
        {}

        class spell_desolate_host_tormented_cries_mark_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_desolate_host_tormented_cries_mark_AuraScript);

            void HandleAfterRemove(AuraEffect const* /**/, AuraEffectHandleModes /**/)
            {
                if (GetUnitOwner() == nullptr || GetCaster() == nullptr)
                    return;

                GetCaster()->SetFacingToObject(GetUnitOwner());
                GetCaster()->CastSpell(GetUnitOwner(), eSpells::SpellTormentedCriesDmg, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_desolate_host_tormented_cries_mark_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_desolate_host_tormented_cries_mark_AuraScript();
        }
};

/// Tormented Cries AOE - 235988
class spell_desolate_host_tormented_cries_aoe : public SpellScriptLoader
{
    public:
        spell_desolate_host_tormented_cries_aoe() : SpellScriptLoader("spell_desolate_host_tormented_cries_aoe")
        {}

        class spell_desolate_host_tormented_cries_aoe_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_desolate_host_tormented_cries_aoe_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if(JadeCore::UnitAuraCheck(true, eSharedSpells::SpellSpiritRealm));
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_desolate_host_tormented_cries_aoe_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_desolate_host_tormented_cries_aoe_SpellScript();
        }
};

///< Soulbind - 236454, 236455
class spell_desolate_host_soulbind : public SpellScriptLoader
{
    public:
        spell_desolate_host_soulbind() : SpellScriptLoader("spell_desolate_host_soulbind")
        {}

        class spell_desolate_host_soulbind_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_desolate_host_soulbind_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                if (GetSpellInfo()->Id == eSpells::SpellSoulbindSpiritSearcher)
                    p_Targets.remove_if(JadeCore::UnitAuraCheck(false, eSharedSpells::SpellSpiritRealm));
                else if (GetSpellInfo()->Id == eSpells::SpellSoulbindCorporealSearcher)
                    p_Targets.remove_if(JadeCore::UnitAuraCheck(true, eSharedSpells::SpellSpiritRealm));

                p_Targets.sort(SearcherCmp());

                if (p_Targets.size() > 1)
                    p_Targets.resize(1);
            }

            void HandleOnHit(SpellEffIndex /**/)
            {
                if (GetHitUnit() == nullptr)
                    return;

                if (GetHitUnit()->GetGUID() == GetCaster()->GetGUID())
                    return;

                GetHitUnit()->CastSpell(GetHitUnit(), eSpells::SpellSoulbindBuff, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_desolate_host_soulbind_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnEffectHitTarget += SpellEffectFn(spell_desolate_host_soulbind_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_desolate_host_soulbind_SpellScript();
        }
};

///< Dissonance - 239006, 239007
class spell_desolate_host_dissonance : public SpellScriptLoader
{
    public:
        spell_desolate_host_dissonance() : SpellScriptLoader("spell_desolate_host_dissonance")
        {}

        class spell_desolate_host_dissonance_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_desolate_host_dissonance_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty() || GetCaster() == nullptr)
                    return;

                bool l_IsCorporeal = GetSpellInfo()->Id == eSpells::SpellDissonanceCorporeal ? false : true;

                p_Targets.remove_if([&l_IsCorporeal] (WorldObject*& p_Itr) -> bool
                {
                    if (!p_Itr->IsPlayer())
                        return true;

                    Unit* l_Target = p_Itr->ToUnit();

                    if (l_Target == nullptr)
                        return true;

                    bool l_IsOnSpirit = l_Target->HasAura(eSharedSpells::SpellSpiritRealm);

                    if (l_IsCorporeal)
                        return l_IsOnSpirit == true;
                    else
                        return l_IsOnSpirit == false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_desolate_host_dissonance_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_desolate_host_dissonance_SpellScript();
        }
};

///< Spiritual Barrier - 235113, 235620, 235732, 235734
class spell_desolate_host_spiritual_barrier : public SpellScriptLoader
{
    public:
        spell_desolate_host_spiritual_barrier() : SpellScriptLoader("spell_desolate_host_spiritual_barrier")
        {}

        class spell_desolate_host_spiritual_barrier_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_desolate_host_spiritual_barrier_AuraScript);

            bool Load() override
            {
                m_Timer = 0;
                return true;
            }

            void HandleOnUpdate(uint32 const p_Diff)
            {
                if (GetUnitOwner() == nullptr)
                    return;

                if (!GetUnitOwner()->isInCombat())
                    return;

                if (GetUnitOwner()->GetMap() == nullptr || !GetUnitOwner()->GetMap()->IsHeroicOrMythic())
                    return;

                m_Timer += p_Diff;

                if (m_Timer < 3 * IN_MILLISECONDS)
                    return;

                m_Timer = 0;

                if (GetUnitOwner()->HasAura(eSharedSpells::SpellSpiritRealm))
                    GetUnitOwner()->CastSpell(GetUnitOwner(), eSpells::SpellDissonanceSpirit, true);
                else
                    GetUnitOwner()->CastSpell(GetUnitOwner(), eSpells::SpellDissonanceCorporeal, true);
            }

            void Register() override
            {
                OnAuraUpdate += AuraUpdateFn(spell_desolate_host_spiritual_barrier_AuraScript::HandleOnUpdate);
            }

            uint32 m_Timer;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_desolate_host_spiritual_barrier_AuraScript();
        }
};

/// Spear of Anguish AOE - 235923
class spell_desolate_host_spear_of_anguish_aoe : public SpellScriptLoader
{
    public:
        spell_desolate_host_spear_of_anguish_aoe() : SpellScriptLoader("spell_desolate_host_spear_of_anguish_aoe")
        {}
        
        class spell_desolate_host_spear_of_anguish_aoe_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_desolate_host_spear_of_anguish_aoe_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.sort(SearcherCmp());

                if (p_Targets.size() > 1)
                    p_Targets.resize(1);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_desolate_host_spear_of_anguish_aoe_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_desolate_host_spear_of_anguish_aoe_SpellScript();
        }
};

///< Spear of Anguish - 235933, 242796
class spell_desolate_host_spear_of_anguish : public SpellScriptLoader
{
    public:
        spell_desolate_host_spear_of_anguish() : SpellScriptLoader("spell_desolate_host_spear_of_anguish")
        {}

        class spell_desolate_host_spear_of_anguish_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_desolate_host_spear_of_anguish_SpellScript);

            void HandleOnHit(SpellEffIndex /**/)
            {
                if (GetHitUnit() == nullptr)
                    return;

                GetHitUnit()->CastSpell(GetHitUnit(), eSharedSpells::SpellSpiritRealm, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_desolate_host_spear_of_anguish_SpellScript::HandleOnHit, EFFECT_2, SPELL_EFFECT_FORCE_CAST);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_desolate_host_spear_of_anguish_SpellScript();
        }
};

///< Diminishing Soul - 240359
class spell_desolate_host_diminishing_soul : public SpellScriptLoader
{
    public:
        spell_desolate_host_diminishing_soul() : SpellScriptLoader("spell_desolate_host_diminishing_soul")
        {}

        class spell_desolate_host_diminishing_soul_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_desolate_host_diminishing_soul_AuraScript);

            void HandleAfterRemove(AuraEffect const* /**/, AuraEffectHandleModes /**/)
            {
                if (GetUnitOwner() == nullptr)
                    return;

                if (!GetUnitOwner()->isInCombat() || GetUnitOwner()->isDead())
                    return;

                Unit* l_Owner = GetUnitOwner();

                if (InstanceScript* l_Ins = l_Owner->GetInstanceScript())
                {
                    if (l_Ins->GetBossState(eData::DataDesolateHost) != IN_PROGRESS)
                        return;
                }

                l_Owner->CastSpell(l_Owner, eSpells::SpellDiminishingSoulMissile, true);
                l_Owner->CastSpell(l_Owner, eSpells::SpellDiminishingSoulPrevent, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_desolate_host_diminishing_soul_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_desolate_host_diminishing_soul_AuraScript();
        }
};

///< Diminishing Soul AOE - 240356, 240358
class spell_desolate_host_diminishing_soul_aoe : public SpellScriptLoader
{
    public:
        spell_desolate_host_diminishing_soul_aoe() : SpellScriptLoader("spell_desolate_host_diminishing_soul_aoe")
        {}

        class spell_desolate_host_diminishing_soul_aoe_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_desolate_host_diminishing_soul_aoe_SpellScript);

            bool Load() override
            {
                return GetCaster() && GetCaster()->isInCombat();
            }

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if([] (WorldObject*& p_Itr) -> bool
                {
                    if (!p_Itr->IsPlayer())
                        return true;

                    if (Unit* l_Target = p_Itr->ToUnit())
                        return l_Target->HasAura(eSpells::SpellDiminishingSoulPrevent);

                    return false;
                });
            }

            void HandleOnHit(SpellEffIndex /**/)
            {
                if (GetHitUnit() == nullptr)
                    return;

                if (InstanceScript* l_Instance = GetHitUnit()->GetInstanceScript())
                {
                    if (l_Instance->GetBossState(eData::DataDesolateHost) != IN_PROGRESS)
                        return;
                }

                GetHitUnit()->CastSpell(GetHitUnit(), eSpells::SpellDiminishingSoulBuff, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_desolate_host_diminishing_soul_aoe_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_desolate_host_diminishing_soul_aoe_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_desolate_host_diminishing_soul_aoe_SpellScript();
        }
};

/// Rupturing Slam - 235927
class at_desolate_host_rupturing_slam : public AreaTriggerEntityScript
{
    public:
        at_desolate_host_rupturing_slam() : AreaTriggerEntityScript("at_desolate_host_rupturing_slam")
        {}

        void OnSetCreatePosition(AreaTrigger* p_At, Unit* p_Caster, Position& p_Src, Position& p_Tgt, std::vector<G3D::Vector3>& /**/) override
        {
            p_At->SetDuration(10000);
            p_At->SetTimeToTarget(10000);
            p_At->SetMoveType(AreaTriggerMoveType::AT_MOVE_TYPE_THROUGH_WALLS);

            if (p_Caster != nullptr && p_Caster->IsAIEnabled)
            {
                switch (p_Caster->GetAI()->GetData(eDesolateHostData::DataRupturingSlam))
                {
                    case eOrientations::West:
                    {
                        p_Src.SetOrientation(p_Src.GetOrientation() + float(M_PI) / 2.f);
                        break;
                    }

                    case eOrientations::East:
                    {
                        p_Src.SetOrientation(p_Src.GetOrientation() - float(M_PI) / 2.f);
                        break;
                    }

                    case eOrientations::South:
                    {
                        p_Src.SetOrientation(p_Src.GetOrientation() + float(M_PI));
                        break;
                    }

                    default: break;
                }
            }

            p_Tgt.SetOrientation(p_Src.GetOrientation());

            p_At->GetFirstCollisionPosition(p_Tgt, 250.f, 0);
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_desolate_host_rupturing_slam();
        }
};

///< Soul Rot - 236235
class at_desolate_host_soul_rot : public AreaTriggerEntityScript
{
    public:
        at_desolate_host_soul_rot() : AreaTriggerEntityScript("at_desolate_host_soul_rot")
        {}

        void OnUpdate(AreaTrigger* p_At, uint32 p_Diff) override
        {
            if (p_At == nullptr || p_At->GetCaster() == nullptr)
                return;

            GuidList* l_Targets = p_At->GetAffectedPlayers();

            for (auto & l_Itr : *l_Targets)
            {
                Unit* l_Target = sObjectAccessor->GetUnit(*p_At, l_Itr);

                if (l_Target == nullptr || !l_Target->IsPlayer())
                    continue;

                if (!l_Target->HasAura(eSpells::SpellSoulRotDmg))
                    p_At->GetCaster()->AddAura(eSpells::SpellSoulRotDmg, l_Target);
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_desolate_host_soul_rot();
        }
};

///< Tormented Cries - 235989
class at_desolate_host_tormented_cries : public AreaTriggerEntityScript
{
    public:
        at_desolate_host_tormented_cries() : AreaTriggerEntityScript("at_desolate_host_tormented_cries")
        {}

        void OnUpdate(AreaTrigger* p_At, uint32 const p_Diff) override
        {
            if (p_At == nullptr)
                return;

            GuidList* l_Targets = p_At->GetAffectedPlayers();

            if (l_Targets == nullptr)
                return;

            for (auto & l_Itr : *l_Targets)
            {
                Unit* l_Target = sObjectAccessor->GetUnit(*p_At, l_Itr);

                if (l_Target == nullptr || !l_Target->IsPlayer())
                    continue;

                if (!l_Target->HasAura(eSpells::SpellTormentedCriesDot))
                    l_Target->AddAura(eSpells::SpellTormentedCriesDot, l_Target);
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_desolate_host_tormented_cries();
        }
};

void AddSC_boss_desolate_host()
{
    /// Creatures
    new boss_engine_of_souls();
    new boss_soul_queen_dehjanna();
    new boss_desolate_host();
    new npc_desolate_host_fallen_priestess();
    new npc_desolate_host_ghastly_bonewarden();
    new npc_desolate_host_reanimate_templar();
    new npc_desolate_host_soul_residue();
    new npc_desolate_host_soul_font();
    new npc_desolate_host_tormented_cries_dummy();

    /// Spells
    new spell_desolate_host_spear_of_anguish_aoe();
    new spell_desolate_host_spear_of_anguish();
    new spell_desolate_host_spirit_realm();
    new spell_desolate_host_doom_spells();
    new spell_desolate_host_other_realm_spells();
    new spell_desolate_host_spiritual_barrier();
    new spell_desolate_host_dissonance();
    new spell_desolate_host_bound_essence();
    new spell_desolate_host_bound_essence_trigger();
    new spell_desolate_host_quietus();
    new spell_desolate_host_shattering_scream();
    new spell_desolate_host_shattering_scream_dot();
    new spell_desolate_host_tormented_cries_aoe();
    new spell_desolate_host_soulbind_buff();
    new spell_desolate_host_soulbind_searcher();
    new spell_desolate_host_tormented_cries_mark();
    new spell_desolate_host_diminishing_soul();
    new spell_desolate_host_soulbind();
    new spell_desolate_host_diminishing_soul_aoe();

    /// Areatriggers
    new at_desolate_host_rupturing_slam();
    new at_desolate_host_soul_rot();
    new at_desolate_host_tormented_cries();
}
