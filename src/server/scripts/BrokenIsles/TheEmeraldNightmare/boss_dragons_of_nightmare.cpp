////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "the_emerald_nightmare.hpp"

enum Says
{
    SAY_AGGRO,
    SAY_CALL,
    SAY_WARNING,
    SAY_SLAY_PLAYER,
    SAY_WIPE,
    SAY_DEATH,
    SAY_HINTERLANDS_PORTAL,
    SAY_ASHENVALE_PORTAL,
    SAY_FERALAS_PORTAL
};

enum Spells
{
    SPELL_EMPTY_ENERGY              = 205283, ///< 224200, 203909, 204719?
    SPELL_ENERGIZE_UP               = 205284,
    SPELL_REMOVE_MARK_TAERAR        = 224627,
    SPELL_REMOVE_MARK_LETHON        = 224626,
    SPELL_REMOVE_MARK_EMERISS       = 224625,
    SPELL_NIGHTMARE_BOND            = 203966, ///< Share damage. 203969
    SPELL_CORRUPTED_BREATH          = 203028,
    SPELL_ENERGIZE_DRAGONS          = 205281,
    SPELL_SLUMBERING_NIGHTMARE      = 203110,
    SPELL_CORRUPTING_POISON         = 225067,
    SPELL_SPIRIT_AURA               = 203950,

    /// Ysondre - 102679
    SPELL_MARK_OF_YSONDRE           = 203083,
    SPELL_MARK_OF_YSONDRE_AURA      = 203102,
    SPELL_ENERGIZE_YSONDRE          = 205282,
    SPELL_NIGHTMARE_BLAST           = 203147,
    SPELL_NIGHTMARE_BLAST_MISSILE   = 203151,
    SPELL_CALL_DEFILED_SPIRIT       = 207573,
    SPELL_CALL_DEFILED_SPIRIT_SUM   = 203766,

    /// Taerar - 102681
    SPELL_MARK_OF_TAERAR            = 203085,
    SPELL_SHADES_OF_TAERAR          = 204100,
    SPELL_SEEPING_FOG               = 205331,
    SPELL_SEEPING_FOG_MISSILE       = 205332,
    SPELL_BELLOWING_ROAR_AURA       = 205172, ///< Heroic+
    SPELL_BELLOWING_ROAR            = 204078,

    /// Lethon - 102682
    SPELL_MARK_OF_LETHON            = 203086,
    SPELL_SIPHON_SPIRIT             = 203888,
    SPELL_SIPHON_SPIRIT_SUM         = 203837,
    SPELL_GLOOM_AURA                = 206758,
    SPELL_SHADOW_BURST              = 204030, ///< Heroic+

    /// Emeriss - 102683
    SPELL_MARK_OF_EMERISS           = 203084,
    SPELL_NIGHTMARE_ENERGY          = 224200,
    SPELL_GROUNDED                  = 204719,
    SPELL_VOLATILE_INFECTION        = 203787,
    SPELL_ESSENCE_OF_CORRUPTION     = 205298,

    /// Nightmare Bloom - 102804
    SPELL_NIGHTMARE_BLOOM_VISUAL    = 203672,
    SPELL_NIGHTMARE_BLOOM_DUMMY     = 205945,
    SPELL_NIGHTMARE_BLOOM_DUMMY_2   = 207681,
    SPELL_NIGHTMARE_BLOOM_DUMMY_3   = 220938,
    SPELL_NIGHTMARE_BLOOM_DUMMY_4   = 211497,
    SPELL_NIGHTMARE_BLOOM_AT        = 203687,
    SPELL_NIGHTMARE_BLOOM_PERIODIC  = 205278,

    /// Defiled Druid Spirit - 103080
    SPELL_DEFILED_SPIRIT_ROOT       = 203768,
    SPELL_DEFILED_ERUPTION          = 203771,

    /// Essence of Corruption - 103691
    SPELL_CORRUPTION                = 205300,

    /// Shade of Taerar - 103145
    SPELL_NIGHTMARE_VISAGE          = 204084,
    SPELL_CORRUPTED_BREATH_2        = 204767,

    /// Dread Horror - 103044
    SPELL_WASTING_DREAD_AT          = 204729, ///< Heroic

    /// Corrupted Mushroom
    SPELL_CORRUPTED_BURST_4         = 203817,
    SPELL_CORRUPTED_BURST_7         = 203827,
    SPELL_CORRUPTED_BURST_10        = 203828,

    /// Mythic abilities
    SPELL_TAIL_LASH                 = 204122,
    SPELL_NIGHTMARE_SOULS_COSMETIC  = 214497,
    SPELL_DEVOUR_NIGHTMARE          = 214488,
    SPELL_CRIMSON_MOON              = 214513,
    SPELL_COLLAPSING_NIGHTMARE      = 214540
};

enum eEvents
{
    EVENT_CORRUPTED_BREATH = 1,

    /// Ysondre
    EVENT_SWITCH_DRAGONS,
    EVENT_NIGHTMARE_BLAST,
    EVENT_CALL_DEFILED_SPIRIT,

    /// Taerar
    EVENT_SHADES_OF_TAERAR,
    EVENT_SEEPING_FOG,

    /// Emeriss
    EVENT_VOLATILE_INFECTION,
    EVENT_ESSENCE_OF_CORRUPTION,

    /// Lethon
    EVENT_SIPHON_SPIRIT,
    EVENT_SHADOW_BURST, ///< Heroic+

    /// Mythic
    EVENT_TAIL_LASH,
    EVENT_COLLAPSING_NIGHTMARE
};

enum ePortals
{
    Hinterlands,
    Ashenvale,
    Feralas,

    MaxPortals
};

std::array<uint32, ePortals::MaxPortals> g_PortalEntries =
{
    {
        NPC_DISEASED_RIFT,
        NPC_NIGHTMARE_RIFT,
        NPC_SHADOWY_RIFT
    }
};

Position const g_DragonPos[6] =
{
    { 617.51f, -12807.00f, 4.840f, 3.39f },
    { 674.78f, -12875.50f, 42.21f, 2.91f },
    { 623.48f, -12845.71f, 4.130f, 2.47f },
    /// Mythic
    { 573.08f, -12874.01f, 7.692f, 2.15f },
    { 681.84f, -12869.25f, 43.62f, 2.74f },
    { 662.45f, -12893.78f, 43.62f, 2.31f }
};

/// Ysondre - 102679
class boss_dragon_ysondre : public CreatureScript
{
    public:
        boss_dragon_ysondre() : CreatureScript("boss_dragon_ysondre") { }

        struct boss_dragon_ysondreAI : public BossAI
        {
            boss_dragon_ysondreAI(Creature* p_Creature) : BossAI(p_Creature, DATA_DRAGON_NIGHTMARE)
            {
                m_GroupMember = sFormationMgr->CreateCustomFormation(me);

                uint64 l_Dragons = sWorld->getWorldState(WS_DRAGONS_OF_NIGHTMARE_WEEKLY_ROTATION);

                if (IsMythic())
                    m_HelperList = { NPC_EMERISS, NPC_TAERAR, NPC_LETHON };
                else if (l_Dragons)
                {
                    uint32 l_FirstDragon = l_Dragons & 0xFFFFFFFF;
                    uint32 l_SecondDragon = (l_Dragons >> 32) & 0xFFFFFFFF;

                    m_HelperList.push_back(l_FirstDragon);
                    m_HelperList.push_back(l_SecondDragon);
                }
                else
                {
                    m_HelperList = { NPC_TAERAR, NPC_LETHON, NPC_EMERISS };

                    JadeCore::RandomResizeList(m_HelperList, 2);
                }

                m_IntroDone = false;

                m_Instance = p_Creature->GetInstanceScript();
            }

            InstanceScript* m_Instance;

            std::vector<uint32> m_HelperList;
            FormationInfo* m_GroupMember;
            uint8 m_EventPhaseHP;
            uint8 m_PortalPhasePct;
            uint8 m_NextPortal;
            bool m_IntroDone;

            void Reset() override
            {
                _Reset();
                DoCast(me, SPELL_EMPTY_ENERGY, true);

                m_EventPhaseHP = 70;
                m_PortalPhasePct = 90;
                m_NextPortal = ePortals::Hinterlands;

                AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    if (Creature* l_Dragon = me->SummonCreature(m_HelperList[0], g_DragonPos[IsMythic() ? 3 : 0]))
                    {
                        if (CreatureGroup* l_Formation = me->GetFormation())
                            l_Formation->AddMember(l_Dragon, m_GroupMember);
                    }

                    if (IsMythic())
                    {
                        for (uint8 l_I = 0; l_I < 2; ++l_I)
                        {
                            if (Creature* l_Dragon = me->SummonCreature(m_HelperList[1 + l_I], g_DragonPos[4 + l_I]))
                            {
                                l_Dragon->SetReactState(REACT_PASSIVE);

                                if (l_Dragon->IsAIEnabled)
                                    l_Dragon->AI()->SetFlyMode(true);

                                l_Dragon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_ATTACKABLE_1);
                            }
                        }
                    }
                    else
                    {
                        if (Creature* l_Dragon = me->SummonCreature(m_HelperList[1], g_DragonPos[1]))
                        {
                            l_Dragon->SetReactState(REACT_PASSIVE);

                            if (l_Dragon->IsAIEnabled)
                                l_Dragon->AI()->SetFlyMode(true);

                            l_Dragon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_ATTACKABLE_1);
                        }
                    }
                });

                DespawnTrash();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                _EnterCombat();

                DoCast(me, SPELL_ENERGIZE_YSONDRE, true);
                DoCast(me, SPELL_MARK_OF_YSONDRE, true);

                events.ScheduleEvent(EVENT_CORRUPTED_BREATH, 15500);
                events.ScheduleEvent(EVENT_SWITCH_DRAGONS, 1000);
                events.ScheduleEvent(EVENT_NIGHTMARE_BLAST, 40000);

                if (IsMythic())
                    events.ScheduleEvent(EVENT_TAIL_LASH, 1000);

                EntryCheckPredicate l_Pred(m_HelperList[0]);
                summons.DoAction(ACTION_3, l_Pred);

                /// Handle flying attacks if needed
                {
                    l_Pred = EntryCheckPredicate(m_HelperList[1]);
                    summons.DoAction(ACTION_5, l_Pred);

                    if (IsMythic())
                    {
                        l_Pred = EntryCheckPredicate(m_HelperList[2]);
                        summons.DoAction(ACTION_5, l_Pred);
                    }
                }

                Talk(SAY_AGGRO);
            }

            void EnterEvadeMode() override
            {
                Talk(SAY_WIPE);

                CreatureAI::EnterEvadeMode();

                summons.DespawnAll();

                ClearDelayedOperations();

                me->NearTeleportTo(me->GetHomePosition());
                me->Respawn(true, true, 30 * TimeConstants::IN_MILLISECONDS);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SetBossState(DATA_DRAGON_NIGHTMARE, EncounterState::FAIL);
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (!p_Who->IsPlayer())
                    return;

                if (!m_IntroDone && me->IsWithinDistInMap(p_Who, 150.0f))
                {
                    m_IntroDone = true;
                    Conversation* l_Conversation = new Conversation;
                    if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), 3608, p_Who, nullptr, *p_Who))
                        delete l_Conversation;
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                _JustDied();
                DespawnTrash();
                Talk(SAY_DEATH);
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (!IsMythic())
                    return;

                if (!m_PortalPhasePct || m_NextPortal >= ePortals::MaxPortals)
                    return;

                if (me->HealthBelowPctDamaged(m_PortalPhasePct, p_Damage) && m_Instance)
                {
                    m_PortalPhasePct -= 30;

                    Talk(SAY_HINTERLANDS_PORTAL + m_NextPortal);

                    for (uint32 l_Entry : g_PortalEntries)
                    {
                        if (l_Entry == g_PortalEntries[m_NextPortal])
                            continue;

                        if (Creature* l_Portal = Creature::GetCreature(*me, m_Instance->GetData64(l_Entry)))
                            l_Portal->RemoveAura(SPELL_NIGHTMARE_SOULS_COSMETIC);
                    }

                    if (Creature* l_Portal = Creature::GetCreature(*me, m_Instance->GetData64(g_PortalEntries[m_NextPortal])))
                        l_Portal->CastSpell(l_Portal, SPELL_NIGHTMARE_SOULS_COSMETIC, true);

                    ++m_NextPortal;
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_TAIL_LASH)
                    me->ClearUnitState(UNIT_STATE_CANNOT_TURN);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_NIGHTMARE_BLAST:
                    {
                        DoCast(p_Target, SPELL_NIGHTMARE_BLAST_MISSILE, true);
                        break;
                    }
                    case SPELL_CALL_DEFILED_SPIRIT:
                    {
                        DoCast(p_Target, SPELL_CALL_DEFILED_SPIRIT_SUM, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void KilledUnit(Unit* p_Killed) override
            {
                if (p_Killed->IsPlayer())
                    Talk(SAY_SLAY_PLAYER);
            }

            void DespawnTrash()
            {
                for (uint32 l_Entry : g_PortalEntries)
                {
                    if (Creature* l_Portal = Creature::GetCreature(*me, m_Instance->GetData64(l_Entry)))
                    {
                        if (l_Portal->IsAIEnabled)
                            l_Portal->AI()->DoAction(0);
                    }
                }

                std::list<Creature*> l_CreatureList;
                GetCreatureListWithEntryInGrid(l_CreatureList, me, NPC_NIGHTMARE_BLOOM, 120.0f);
                GetCreatureListWithEntryInGrid(l_CreatureList, me, NPC_ESSENCE_OF_CORRUPTION, 120.0f);
                GetCreatureListWithEntryInGrid(l_CreatureList, me, NPC_SHADE_OF_TAERAR, 120.0f);
                GetCreatureListWithEntryInGrid(l_CreatureList, me, NPC_SEEPING_FOG, 120.0f);
                GetCreatureListWithEntryInGrid(l_CreatureList, me, NPC_SPIRIT_SHADE, 120.0f);
                GetCreatureListWithEntryInGrid(l_CreatureList, me, NPC_DREAD_HORROR, 120.0f);

                for (Creature* l_Creature : l_CreatureList)
                {
                    if (l_Creature && l_Creature->IsInWorld())
                        l_Creature->DespawnOrUnsummon();
                }
            }

            void DoAction(int32 const p_Action, Creature* p_Caller) override
            {
                switch (p_Action)
                {
                    case ACTION_1:
                    {
                        if (CreatureGroup* l_Formation = me->GetFormation())
                            l_Formation->AddMember(p_Caller, m_GroupMember);
                        break;
                    }
                    case ACTION_2:
                    {
                        if (CreatureGroup* l_Formation = me->GetFormation())
                            l_Formation->RemoveMember(p_Caller);
                        break;
                    }
                    case ACTION_4: ///< Special Attack
                    {
                        events.ScheduleEvent(EVENT_CALL_DEFILED_SPIRIT, 100);
                        break;
                    }
                    default:
                        break;
                }
            }

            uint32 GetData(uint32 p_ID /*= 0*/) override
            {
                return m_NextPortal;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                if (me->GetDistance(me->GetHomePosition()) >= 130.0f)
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_CORRUPTED_BREATH:
                    {
                        DoCastVictim(SPELL_CORRUPTED_BREATH);
                        events.ScheduleEvent(EVENT_CORRUPTED_BREATH, 27000);
                        break;
                    }
                    case EVENT_SWITCH_DRAGONS:
                    {
                        if (me->HealthBelowPct(m_EventPhaseHP))
                        {
                            if (m_EventPhaseHP > 40)
                            {
                                m_EventPhaseHP = 40;

                                EntryCheckPredicate l_Pred0(m_HelperList[0]);
                                summons.DoAction(ACTION_1, l_Pred0);

                                EntryCheckPredicate l_Pred1(m_HelperList[1]);
                                summons.DoAction(ACTION_2, l_Pred1);
                            }
                            else
                            {
                                m_EventPhaseHP = 0;

                                if (IsMythic())
                                {
                                    EntryCheckPredicate l_Pred0(m_HelperList[1]);
                                    summons.DoAction(ACTION_1, l_Pred0);

                                    EntryCheckPredicate l_Pred1(m_HelperList[2]);
                                    summons.DoAction(ACTION_2, l_Pred1);
                                }
                                else
                                {
                                    EntryCheckPredicate l_Pred0(m_HelperList[0]);
                                    summons.DoAction(ACTION_2, l_Pred0);

                                    EntryCheckPredicate l_Pred1(m_HelperList[1]);
                                    summons.DoAction(ACTION_1, l_Pred1);
                                }

                                break;
                            }
                        }

                        events.ScheduleEvent(EVENT_SWITCH_DRAGONS, 1000);
                        break;
                    }
                    case EVENT_NIGHTMARE_BLAST:
                    {
                        DoCast(SPELL_NIGHTMARE_BLAST);
                        events.ScheduleEvent(EVENT_NIGHTMARE_BLAST, 16000);
                        break;
                    }
                    case EVENT_CALL_DEFILED_SPIRIT:
                    {
                        Talk(SAY_CALL);
                        DoCast(SPELL_CALL_DEFILED_SPIRIT);
                        break;
                    }
                    case EVENT_TAIL_LASH:
                    {
                        std::list<Player*> l_PlayerList;
                        me->GetPlayerListInGrid(l_PlayerList, 10.0f);

                        if (!l_PlayerList.empty())
                        {
                            l_PlayerList.remove_if([this](Player* p_Player) -> bool
                            {
                                if (!me->isInBack(p_Player, (50.0f * M_PI / 180.0f)))
                                    return true;

                                return false;
                            });

                            if (!l_PlayerList.empty())
                            {
                                me->AddUnitState(UNIT_STATE_CANNOT_TURN);

                                DoCast(me, SPELL_TAIL_LASH);
                            }
                        }

                        events.ScheduleEvent(EVENT_TAIL_LASH, 2000);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

            void RemoveEncounterAuras()
            {
                if (m_Instance == nullptr)
                    return;

                m_Instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VOLATILE_INFECTION);
                m_Instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MARK_OF_EMERISS);
                m_Instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MARK_OF_TAERAR);
                m_Instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MARK_OF_LETHON);
                m_Instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MARK_OF_YSONDRE);
                m_Instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CORRUPTING_POISON);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_dragon_ysondreAI(p_Creature);
        }
};

/// Taerar <Nightmare of Ysondre> - 102681
class boss_dragon_taerar : public CreatureScript
{
    public:
        boss_dragon_taerar() : CreatureScript("boss_dragon_taerar") { }

        struct boss_dragon_taerarAI : public ScriptedAI
        {
            boss_dragon_taerarAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = me->GetInstanceScript();
                me->SetSpeed(MOVE_FLIGHT, 2.0f);
            }

            InstanceScript* m_Instance;

            void Reset() override
            {
                me->SetMaxPower(POWER_ENERGY, 100);
                DoCast(me, SPELL_EMPTY_ENERGY, true);
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                DoZoneInCombat();

                DoCast(me, SPELL_ENERGIZE_UP, true);
                Talk(SAY_AGGRO);
            }

            void DefaultEvents()
            {
                if (m_Instance)
                    m_Instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                me->RemoveAurasDueToSpell(SPELL_BELLOWING_ROAR_AURA);

                DoCast(me, SPELL_MARK_OF_TAERAR, true);
                DoCast(me, SPELL_ENERGIZE_DRAGONS, true);

                events.ScheduleEvent(EVENT_CORRUPTED_BREATH, 16000);
                events.ScheduleEvent(EVENT_SEEPING_FOG, 26000);

                if (IsMythic())
                    events.ScheduleEvent(EVENT_TAIL_LASH, 1000);
            }

            void JustDied(Unit* /*p_Killer*/) override { }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case ACTION_1:
                    {
                        if (Creature* l_Summoner = me->GetAnyOwner()->ToCreature())
                        {
                            if (l_Summoner->IsAIEnabled)
                                l_Summoner->AI()->DoAction(ACTION_2, me); ///< Leave group
                        }
                        events.Reset();
                        DoStopAttack();
                        SetFlyMode(true);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_ATTACKABLE_1);
                        me->SetReactState(REACT_PASSIVE);
                        me->RemoveAurasDueToSpell(SPELL_MARK_OF_TAERAR);
                        me->RemoveAurasDueToSpell(SPELL_ENERGIZE_DRAGONS);
                        me->GetMotionMaster()->MovePoint(1, g_DragonPos[1].GetPositionX(), g_DragonPos[1].GetPositionY(), g_DragonPos[1].GetPositionZ(), false);
                        if (m_Instance)
                            m_Instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                        break;
                    }
                    case ACTION_2:
                    {
                        me->GetMotionMaster()->MovePoint(2, g_DragonPos[2].GetPositionX(), g_DragonPos[2].GetPositionY(), g_DragonPos[2].GetPositionZ(), false);
                        break;
                    }
                    case ACTION_3:
                    {
                        DefaultEvents();
                        break;
                    }
                    case ACTION_4: ///< Special Attack
                    {
                        events.ScheduleEvent(EVENT_SHADES_OF_TAERAR, 100);
                        break;
                    }
                    case ACTION_5:  ///< Flying attack
                    {
                        if (IsHeroicOrMythic())
                            DoCast(me, SPELL_BELLOWING_ROAR_AURA, true);

                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != POINT_MOTION_TYPE)
                    return;

                if (p_ID == 1)
                {
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_ATTACKABLE_1);
                    me->SetFacingTo(2.47f);
                }
                else if (p_ID == 2)
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_ATTACKABLE_1);

                    SetFlyMode(false);
                    me->SetReactState(REACT_AGGRESSIVE);
                    DoZoneInCombat(me, 150.0f);

                    if (Creature* l_Summoner = me->GetAnyOwner()->ToCreature())
                    {
                        me->SetHealth(l_Summoner->GetHealth());

                        if (l_Summoner->IsAIEnabled)
                            l_Summoner->AI()->DoAction(ACTION_1, me); ///< Invite group
                    }

                    DefaultEvents();
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_TAIL_LASH)
                    me->ClearUnitState(UNIT_STATE_CANNOT_TURN);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_SEEPING_FOG)
                    DoCast(p_Target, SPELL_SEEPING_FOG_MISSILE, true);
            }

            void JustSummoned(Creature* p_Summon) override
            {
                ScriptedAI::JustSummoned(p_Summon);

                if (p_Summon->GetEntry() == NPC_SEEPING_FOG)
                {
                    p_Summon->GetMotionMaster()->Clear();
                    p_Summon->GetMotionMaster()->MoveRandom(30.0f);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_CORRUPTED_BREATH:
                    {
                        DoCastVictim(SPELL_CORRUPTED_BREATH);
                        events.ScheduleEvent(EVENT_CORRUPTED_BREATH, 27000);
                        break;
                    }
                    case EVENT_SHADES_OF_TAERAR:
                    {
                        DoCast(SPELL_SHADES_OF_TAERAR);
                        Talk(SAY_CALL);
                        break;
                    }
                    case EVENT_SEEPING_FOG:
                    {
                        DoCast(me, SPELL_SEEPING_FOG, true);
                        events.ScheduleEvent(EVENT_SEEPING_FOG, 16000);
                        break;
                    }
                    case EVENT_TAIL_LASH:
                    {
                        std::list<Player*> l_PlayerList;
                        me->GetPlayerListInGrid(l_PlayerList, 10.0f);

                        if (!l_PlayerList.empty())
                        {
                            l_PlayerList.remove_if([this](Player* p_Player) -> bool
                            {
                                if (!me->isInBack(p_Player, (50.0f * M_PI / 180.0f)))
                                    return true;

                                return false;
                            });

                            if (!l_PlayerList.empty())
                            {
                                me->AddUnitState(UNIT_STATE_CANNOT_TURN);

                                DoCast(me, SPELL_TAIL_LASH);
                            }
                        }

                        events.ScheduleEvent(EVENT_TAIL_LASH, 2000);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_dragon_taerarAI(p_Creature);
        }
};

/// Lethon <Nightmare of Ysondre> - 102682
class boss_dragon_lethon : public CreatureScript
{
    public:
        boss_dragon_lethon() : CreatureScript("boss_dragon_lethon") { }

        struct boss_dragon_lethonAI : public ScriptedAI
        {
            boss_dragon_lethonAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = me->GetInstanceScript();
                me->SetSpeed(MOVE_FLIGHT, 2.0f);
            }

            InstanceScript* m_Instance;

            EventMap m_FlyingEvents;

            void Reset() override
            {
                me->SetMaxPower(POWER_ENERGY, 100);
                DoCast(me, SPELL_EMPTY_ENERGY, true);
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                DoZoneInCombat();

                DoCast(me, SPELL_ENERGIZE_UP, true);
                Talk(SAY_AGGRO);
            }

            void DefaultEvents()
            {
                if (m_Instance)
                    m_Instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                DoCast(me, SPELL_MARK_OF_LETHON, true);
                DoCast(me, SPELL_ENERGIZE_DRAGONS, true);
                DoCast(me, SPELL_GLOOM_AURA, true);

                events.ScheduleEvent(EVENT_CORRUPTED_BREATH, 16000);

                m_FlyingEvents.Reset();

                if (IsMythic())
                    events.ScheduleEvent(EVENT_TAIL_LASH, 1000);
            }

            void JustDied(Unit* /*p_Killer*/) override { }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case ACTION_1:
                    {
                        if (Creature* l_Summoner = me->GetAnyOwner()->ToCreature())
                        {
                            if (l_Summoner->IsAIEnabled)
                                l_Summoner->AI()->DoAction(ACTION_2, me); ///< Leave group
                        }
                        events.Reset();
                        DoStopAttack();
                        SetFlyMode(true);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_ATTACKABLE_1);
                        me->SetReactState(REACT_PASSIVE);
                        me->RemoveAurasDueToSpell(SPELL_MARK_OF_LETHON);
                        me->RemoveAurasDueToSpell(SPELL_ENERGIZE_DRAGONS);
                        me->RemoveAurasDueToSpell(SPELL_GLOOM_AURA);
                        me->GetMotionMaster()->MovePoint(1, g_DragonPos[1].GetPositionX(), g_DragonPos[1].GetPositionY(), g_DragonPos[1].GetPositionZ(), false);

                        if (m_Instance)
                            m_Instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                        break;
                    }
                    case ACTION_2:
                    {
                        me->GetMotionMaster()->MovePoint(2, g_DragonPos[2].GetPositionX(), g_DragonPos[2].GetPositionY(), g_DragonPos[2].GetPositionZ(), false);
                        break;
                    }
                    case ACTION_3:
                    {
                        DefaultEvents();
                        break;
                    }
                    case ACTION_4: ///< Special Attack
                    {
                        events.ScheduleEvent(EVENT_SIPHON_SPIRIT, 100);
                        break;
                    }
                    case ACTION_5:  ///< Flying attack
                    {
                        if (IsHeroicOrMythic())
                            m_FlyingEvents.ScheduleEvent(EVENT_SHADOW_BURST, 15000);

                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo)  override
            {
                if (p_SpellInfo->Id == SPELL_SIPHON_SPIRIT)
                    p_Target->CastSpell(p_Target, SPELL_SIPHON_SPIRIT_SUM, true);
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != POINT_MOTION_TYPE)
                    return;

                if (p_ID == 1)
                {
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_ATTACKABLE_1);
                    me->SetFacingTo(2.47f);
                }
                else if (p_ID == 2)
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_ATTACKABLE_1);

                    SetFlyMode(false);
                    me->SetReactState(REACT_AGGRESSIVE);
                    DoZoneInCombat(me, 150.0f);

                    if (Creature* l_Summoner = me->GetAnyOwner()->ToCreature())
                    {
                        me->SetHealth(l_Summoner->GetHealth());

                        if (l_Summoner->IsAIEnabled)
                            l_Summoner->AI()->DoAction(ACTION_1, me); ///< Invite group
                    }

                    DefaultEvents();
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_TAIL_LASH)
                    me->ClearUnitState(UNIT_STATE_CANNOT_TURN);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                m_FlyingEvents.Update(p_Diff);

                if (m_FlyingEvents.ExecuteEvent() == EVENT_SHADOW_BURST)
                {
                    DoCast(me, SPELL_SHADOW_BURST, true);
                    m_FlyingEvents.ScheduleEvent(EVENT_SHADOW_BURST, 14500);
                }

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_CORRUPTED_BREATH:
                    {
                        DoCastVictim(SPELL_CORRUPTED_BREATH);
                        events.ScheduleEvent(EVENT_CORRUPTED_BREATH, 27000);
                        break;
                    }
                    case EVENT_SIPHON_SPIRIT:
                    {
                        DoCast(SPELL_SIPHON_SPIRIT);
                        Talk(SAY_CALL);
                        Talk(SAY_WARNING);
                        break;
                    }
                    case EVENT_TAIL_LASH:
                    {
                        std::list<Player*> l_PlayerList;
                        me->GetPlayerListInGrid(l_PlayerList, 10.0f);

                        if (!l_PlayerList.empty())
                        {
                            l_PlayerList.remove_if([this](Player* p_Player) -> bool
                            {
                                if (!me->isInBack(p_Player, (50.0f * M_PI / 180.0f)))
                                    return true;

                                return false;
                            });

                            if (!l_PlayerList.empty())
                            {
                                me->AddUnitState(UNIT_STATE_CANNOT_TURN);

                                DoCast(me, SPELL_TAIL_LASH);
                            }
                        }

                        events.ScheduleEvent(EVENT_TAIL_LASH, 2000);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_dragon_lethonAI(p_Creature);
        }
};

/// Emeriss <Nightmare of Ysondre> - 102683
class boss_dragon_emeriss : public CreatureScript
{
    public:
        boss_dragon_emeriss() : CreatureScript("boss_dragon_emeriss") { }

        struct boss_dragon_emerissAI : public ScriptedAI
        {
            boss_dragon_emerissAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = me->GetInstanceScript();
                me->SetSpeed(MOVE_FLIGHT, 2.0f);
            }

            InstanceScript* m_Instance;

            void Reset() override
            {
                me->SetMaxPower(POWER_ENERGY, 100);
                DoCast(me, SPELL_EMPTY_ENERGY, true);
                DoCast(me, SPELL_NIGHTMARE_ENERGY, true);
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                DoZoneInCombat();

                DoCast(me, SPELL_ENERGIZE_UP, true);
                Talk(SAY_AGGRO);
            }

            void DefaultEvents()
            {
                if (m_Instance)
                    m_Instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                DoCast(me, SPELL_MARK_OF_EMERISS, true);
                DoCast(me, SPELL_ENERGIZE_DRAGONS, true);

                events.ScheduleEvent(EVENT_CORRUPTED_BREATH, 15500);
                events.ScheduleEvent(EVENT_ESSENCE_OF_CORRUPTION, 30000);

                if (IsMythic())
                    events.ScheduleEvent(EVENT_TAIL_LASH, 1000);
            }

            void JustDied(Unit* /*p_Killer*/) override { }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case ACTION_1:
                    {
                        if (Creature* l_Summoner = me->GetAnyOwner()->ToCreature())
                        {
                            if (l_Summoner->IsAIEnabled)
                                l_Summoner->AI()->DoAction(ACTION_2, me); ///< Leave group
                        }
                        events.Reset();
                        DoStopAttack();
                        SetFlyMode(true);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_ATTACKABLE_1);
                        me->SetReactState(REACT_PASSIVE);
                        me->RemoveAurasDueToSpell(SPELL_MARK_OF_EMERISS);
                        me->RemoveAurasDueToSpell(SPELL_ENERGIZE_DRAGONS);
                        me->GetMotionMaster()->MovePoint(1, g_DragonPos[1].GetPositionX(), g_DragonPos[1].GetPositionY(), g_DragonPos[1].GetPositionZ(), false);
                        if (m_Instance)
                            m_Instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                        break;
                    }
                    case ACTION_2:
                    {
                        me->GetMotionMaster()->MovePoint(2, g_DragonPos[2].GetPositionX(), g_DragonPos[2].GetPositionY(), g_DragonPos[2].GetPositionZ(), false);
                        break;
                    }
                    case ACTION_3:
                    {
                        DefaultEvents();
                        break;
                    }
                    case ACTION_4: ///< Special Attack
                    {
                        events.ScheduleEvent(EVENT_VOLATILE_INFECTION, 100);
                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != POINT_MOTION_TYPE)
                    return;

                if (p_ID == 1)
                {
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_ATTACKABLE_1);
                    me->SetFacingTo(2.47f);
                }
                else if (p_ID == 2)
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_ATTACKABLE_1);

                    SetFlyMode(false);
                    me->SetReactState(REACT_AGGRESSIVE);
                    DoZoneInCombat(me, 150.0f);

                    if (Creature* l_Summoner = me->GetAnyOwner()->ToCreature())
                    {
                        me->SetHealth(l_Summoner->GetHealth());

                        if (l_Summoner->IsAIEnabled)
                            l_Summoner->AI()->DoAction(ACTION_1, me); ///< Invite group
                    }

                    DefaultEvents();
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_TAIL_LASH)
                    me->ClearUnitState(UNIT_STATE_CANNOT_TURN);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_CORRUPTED_BREATH:
                    {
                        DoCastVictim(SPELL_CORRUPTED_BREATH);
                        events.ScheduleEvent(EVENT_CORRUPTED_BREATH, 27000);
                        break;
                    }
                    case EVENT_VOLATILE_INFECTION:
                    {
                        DoCast(me, SPELL_VOLATILE_INFECTION, true);
                        break;
                    }
                    case EVENT_ESSENCE_OF_CORRUPTION:
                    {
                        Talk(SAY_CALL);
                        Talk(SAY_WARNING);
                        DoCast(me, SPELL_ESSENCE_OF_CORRUPTION, true);
                        events.ScheduleEvent(EVENT_ESSENCE_OF_CORRUPTION, 30000);
                        break;
                    }
                    case EVENT_TAIL_LASH:
                    {
                        std::list<Player*> l_PlayerList;
                        me->GetPlayerListInGrid(l_PlayerList, 10.0f);

                        if (!l_PlayerList.empty())
                        {
                            l_PlayerList.remove_if([this](Player* p_Player) -> bool
                            {
                                if (!me->isInBack(p_Player, (50.0f * M_PI / 180.0f)))
                                    return true;

                                return false;
                            });

                            if (!l_PlayerList.empty())
                            {
                                me->AddUnitState(UNIT_STATE_CANNOT_TURN);

                                DoCast(me, SPELL_TAIL_LASH);
                            }
                        }

                        events.ScheduleEvent(EVENT_TAIL_LASH, 2000);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_dragon_emerissAI(p_Creature);
        }
};

/// Nightmare Bloom - 102804
class npc_ysondre_nightmare_bloom : public CreatureScript
{
    public:
        npc_ysondre_nightmare_bloom() : CreatureScript("npc_ysondre_nightmare_bloom") { }

        struct npc_ysondre_nightmare_bloomAI : public ScriptedAI
        {
            npc_ysondre_nightmare_bloomAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);

                m_IsSoaking = false;
                m_SoakingTime = 0;
            }

            bool m_IsSoaking;
            uint32 m_SoakingTime;

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoCast(me, SPELL_NIGHTMARE_BLOOM_VISUAL, true);
                DoCast(me, SPELL_NIGHTMARE_BLOOM_DUMMY, true);
                DoCast(me, SPELL_NIGHTMARE_BLOOM_AT, true);

                AddTimedDelayedOperation(6 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    DoCast(me, SPELL_NIGHTMARE_BLOOM_PERIODIC, true);
                });
            }

            void SetData(uint32 /*p_ID*/, uint32 p_Value) override
            {
                m_IsSoaking = p_Value != 0;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!m_IsSoaking)
                    return;

                m_SoakingTime += p_Diff;

                if (m_SoakingTime >= 30 * TimeConstants::IN_MILLISECONDS)
                {
                    m_IsSoaking = false;
                    me->DespawnOrUnsummon(10);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_ysondre_nightmare_bloomAI(p_Creature);
        }
};

/// Defiled Druid Spirit - 103080
class npc_ysondre_defiled_druid_spirit : public CreatureScript
{
    public:
        npc_ysondre_defiled_druid_spirit() : CreatureScript("npc_ysondre_defiled_druid_spirit") { }

        struct npc_ysondre_defiled_druid_spiritAI : public ScriptedAI
        {
            npc_ysondre_defiled_druid_spiritAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            uint16 m_ExplodeTimer;

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                m_ExplodeTimer = 500;
                DoCast(me, SPELL_DEFILED_SPIRIT_ROOT, true);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (m_ExplodeTimer)
                {
                    if (m_ExplodeTimer <= p_Diff)
                    {
                        m_ExplodeTimer = 0;
                        me->DespawnOrUnsummon(8000);
                        DoCast(SPELL_DEFILED_ERUPTION);
                    }
                    else
                        m_ExplodeTimer -= p_Diff;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_ysondre_defiled_druid_spiritAI(p_Creature);
        }
};

/// Shade of Taerar - 103145
class npc_ysondre_shade_of_taerar : public CreatureScript
{
    public:
        npc_ysondre_shade_of_taerar() : CreatureScript("npc_ysondre_shade_of_taerar") { }

        struct npc_ysondre_shade_of_taerarAI : public ScriptedAI
        {
            npc_ysondre_shade_of_taerarAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoCast(me, SPELL_NIGHTMARE_VISAGE, true); ///< Scale
                DoZoneInCombat(me, 150.0f);
                events.ScheduleEvent(EVENT_1, 6000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        DoCastVictim(SPELL_CORRUPTED_BREATH_2);
                        events.ScheduleEvent(EVENT_1, 18 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_ysondre_shade_of_taerarAI(p_Creature);
        }
};

/// Spirit Shade - 103100
class npc_ysondre_spirit_shade : public CreatureScript
{
    public:
        npc_ysondre_spirit_shade() : CreatureScript("npc_ysondre_spirit_shade") { }

        struct npc_ysondre_spirit_shadeAI : public ScriptedAI
        {
            npc_ysondre_spirit_shadeAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = me->GetInstanceScript();
                me->SetSpeed(MOVE_RUN, 0.5f);
                me->SetReactState(REACT_PASSIVE);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
            }

            InstanceScript* m_Instance;
            bool m_Despawn;
            uint16 m_MoveTimer;
            uint16 m_CheckBossStateTimer;

            void Reset() override
            {
                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                /// Safety check, I don't know why but sometimes, Ysondre gets a copy of herself
                if (!p_Summoner->IsPlayer())
                {
                    me->DespawnOrUnsummon(10);
                    return;
                }

                m_Despawn = false;
                m_CheckBossStateTimer = 1000;
                m_MoveTimer = 500;

                p_Summoner->CastSpell(me, 203840, true); ///< Clone Player

                if (IsMythic())
                    DoCast(me, SPELL_SPIRIT_AURA, true); ///< Mod Scale
            }

            void GetMythicHealthMod(float& p_Mod) override
            {
                p_Mod = 2.6632f;
            }

            void OnCalculateMoveSpeed(float& p_Velocity) override
            {
                p_Velocity = baseMoveSpeed[MOVE_WALK] * 0.7f;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (m_CheckBossStateTimer)
                {
                    if (m_CheckBossStateTimer <= p_Diff)
                    {
                        m_CheckBossStateTimer = 0;

                        if (!m_Instance || m_Instance->GetBossState(DATA_DRAGON_NIGHTMARE) != IN_PROGRESS)
                        {
                            m_Despawn = true;
                            me->DespawnOrUnsummon(100);
                            return;
                        }
                    }
                    else
                        m_CheckBossStateTimer -= p_Diff;
                }

                if (m_MoveTimer)
                {
                    if (m_MoveTimer <= p_Diff)
                    {
                        m_MoveTimer = 200;

                        if (!me->HasUnitState(UNIT_STATE_STUNNED) && m_Instance)
                        {
                            if (Creature* l_Lethon = Creature::GetCreature(*me, m_Instance->GetData64(NPC_LETHON)))
                            {
                                me->SetWalk(true);
                                me->GetMotionMaster()->MovePoint(0, *l_Lethon);
                            }
                        }
                    }
                    else
                        m_MoveTimer -= p_Diff;
                }

                if (m_Instance && !m_Despawn)
                {
                    if (Creature* l_Lethon = Creature::GetCreature(*me, m_Instance->GetData64(NPC_LETHON)))
                    {
                        if (me->GetDistance(*l_Lethon) <= 5.0f)
                        {
                            m_Despawn = true;
                            m_MoveTimer = 0;
                            m_CheckBossStateTimer = 0;
                            DoCast(me, 203897, true); ///< Heal
                            me->GetMotionMaster()->Clear();
                            me->DespawnOrUnsummon(100);
                        }
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_ysondre_spirit_shadeAI(p_Creature);
        }
};

/// Dread Horror - 103044
class npc_ysondre_dread_horror : public CreatureScript
{
    public:
        npc_ysondre_dread_horror() : CreatureScript("npc_ysondre_dread_horror") { }

        struct npc_ysondre_dread_horrorAI : public ScriptedAI
        {
            npc_ysondre_dread_horrorAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void GetMythicHealthMod(float& p_Mod) override
            {
                p_Mod = 2.223f;
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                if (IsMythic())
                    DoCast(me, SPELL_SPIRIT_AURA, true); ///< Mod Scale

                if (IsHeroicOrMythic())
                    DoCast(me, SPELL_WASTING_DREAD_AT, true);

                DoZoneInCombat(me, 100.0f);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_ysondre_dread_horrorAI(p_Creature);
        }
};

/// Corrupted Mushroom - 103095, 103096, 103097
class npc_ysondre_corrupted_mushroom : public CreatureScript
{
    public:
        npc_ysondre_corrupted_mushroom() : CreatureScript("npc_ysondre_corrupted_mushroom") { }

        struct npc_ysondre_corrupted_mushroomAI : public ScriptedAI
        {
            npc_ysondre_corrupted_mushroomAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC);
            }

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                switch (me->GetEntry())
                {
                    case NPC_CORRUPTED_MUSHROOM_SMALL:
                        events.ScheduleEvent(EVENT_1, 500);
                        break;
                    case NPC_CORRUPTED_MUSHROOM_MEDIUM:
                        events.ScheduleEvent(EVENT_2, 500);
                        break;
                    case NPC_CORRUPTED_MUSHROOM_BIG:
                        events.ScheduleEvent(EVENT_3, 500);
                        break;
                }

                events.ScheduleEvent(EVENT_4, 5000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                        DoCast(me, SPELL_CORRUPTED_BURST_4, false);
                        break;
                    case EVENT_2:
                        DoCast(me, SPELL_CORRUPTED_BURST_7, false);
                        break;
                    case EVENT_3:
                        DoCast(me, SPELL_CORRUPTED_BURST_10, false);
                        break;
                    case EVENT_4:
                        me->DespawnOrUnsummon(100);
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_ysondre_corrupted_mushroomAI(p_Creature);
        }
};

/// Diseased Rift - 103378
/// Nightmare Rift - 103395
/// Shadowy Rift - 103395
class npc_ysondre_rift_controller : public CreatureScript
{
    public:
        npc_ysondre_rift_controller() : CreatureScript("npc_ysondre_rift_controller") { }

        struct npc_ysondre_rift_controllerAI : public ScriptedAI
        {
            npc_ysondre_rift_controllerAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
                m_IsPortalBack = p_Creature->ToTempSummon() != nullptr;
            }

            enum eLocationsIn
            {
                HinterlandsLocID    = 5520,
                AshenvaleLocID      = 5522,
                FeralasLocID        = 5521
            };

            enum eLocationsBack
            {
                FromHinterlandsLocID    = 5526,
                FromAshenvaleLocID      = 5525,
                FromFeralasLocID        = 5524
            };

            std::array<uint32, ePortals::MaxPortals> m_PortalLocIn =
            {
                {
                    eLocationsIn::HinterlandsLocID,
                    eLocationsIn::AshenvaleLocID,
                    eLocationsIn::FeralasLocID
                }
            };

            std::array<Position const, ePortals::MaxPortals> m_LumberingSpawnPos =
            {
                {
                    { -2741.14f, 2439.25f, 93.906f, 2.307f },    ///< Hinterlands
                    { -2774.24f, 3657.78f, 110.94f, 1.337f },    ///< Ashenvale
                    { -4149.84f, 135.766f, 29.827f, 1.000f }     ///< Feralas
                }
            };

            std::array<Position const, ePortals::MaxPortals> m_BackPortalSpawnPos =
            {
                {
                    { -2648.32f, 2526.73f, 110.88f, 3.4504f },
                    { -2620.14f, 3673.33f, 138.83f, 2.8770f },
                    { -4024.75f, 139.937f, 53.042f, 2.6846f }
                }
            };

            InstanceScript* m_Instance;

            uint32 m_Location;

            bool m_IsPortalBack;

            uint32 m_SafetyTimer = 0;
            uint64 m_LumberingGuid;

            void Reset() override
            {
                if (m_IsPortalBack)
                {
                    switch (me->GetEntry())
                    {
                        case NPC_DISEASED_RIFT:
                        {
                            m_Location = eLocationsBack::FromHinterlandsLocID;
                            break;
                        }
                        case NPC_NIGHTMARE_RIFT:
                        {
                            m_Location = eLocationsBack::FromAshenvaleLocID;
                            break;
                        }
                        case NPC_SHADOWY_RIFT:
                        {
                            m_Location = eLocationsBack::FromFeralasLocID;
                            break;
                        }
                        default:
                            break;
                    }

                    events.ScheduleEvent(EVENT_1, 1);
                    return;
                }

                m_Location = 0;
                m_LumberingGuid = 0;

                events.Reset();

                switch (me->GetEntry())
                {
                    case NPC_DISEASED_RIFT:
                    {
                        me->SummonCreature(me->GetEntry(), m_BackPortalSpawnPos[ePortals::Hinterlands]);
                        break;
                    }
                    case NPC_NIGHTMARE_RIFT:
                    {
                        me->SummonCreature(me->GetEntry(), m_BackPortalSpawnPos[ePortals::Ashenvale]);
                        break;
                    }
                    case NPC_SHADOWY_RIFT:
                    {
                        me->SummonCreature(me->GetEntry(), m_BackPortalSpawnPos[ePortals::Feralas]);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHit(Unit* /*p_Caster*/, SpellInfo const* p_SpellInfo) override
            {
                if (!m_IsPortalBack && m_Instance && p_SpellInfo->Id == SPELL_NIGHTMARE_SOULS_COSMETIC)
                {
                    if (Creature* l_Ysondre = Creature::GetCreature(*me, m_Instance->GetData64(NPC_YSONDRE)))
                    {
                        if (!l_Ysondre->IsAIEnabled)
                            return;

                        uint32 l_Portal = l_Ysondre->AI()->GetData();
                        if (l_Portal >= ePortals::MaxPortals)
                            return;

                        m_Location = m_PortalLocIn[l_Portal];

                        events.ScheduleEvent(EVENT_1, 1);

                        if (Creature* l_Lumbering = me->SummonCreature(NPC_LUMBERING_MINDGORGER, m_LumberingSpawnPos[l_Portal], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 15 * TimeConstants::IN_MILLISECONDS))
                        {
                            m_SafetyTimer = 90 * TimeConstants::IN_MILLISECONDS;

                            m_LumberingGuid = l_Lumbering->GetGUID();

                            if (l_Lumbering->IsAIEnabled)
                                l_Lumbering->AI()->SetData(0, l_Portal);
                        }
                    }
                }
            }

            void SummonedCreatureDies(Creature* p_Summon, Unit* /*p_Killer*/) override
            {
                if (!m_IsPortalBack && p_Summon->GetEntry() == NPC_LUMBERING_MINDGORGER)
                {
                    m_SafetyTimer = 0;

                    events.Reset();

                    me->RemoveAura(SPELL_NIGHTMARE_SOULS_COSMETIC);

                    m_Location = 0;
                }
            }

            void DoAction(int32 const p_Action) override
            {
                me->RemoveAura(SPELL_NIGHTMARE_SOULS_COSMETIC);

                if (Creature* l_Lumbering = Creature::GetCreature(*me, m_LumberingGuid))
                    l_Lumbering->DespawnOrUnsummon();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (m_SafetyTimer)
                {
                    if (m_SafetyTimer <= p_Diff)
                    {
                        m_SafetyTimer = 0;

                        AddTimedDelayedOperation(p_Diff, [this]() -> void
                        {
                            Map::PlayerList const& l_PlayerList = me->GetMap()->GetPlayers();
                            for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
                            {
                                if (Player* l_Player = l_Iter->getSource())
                                {
                                    if (!l_Player->isGameMaster() && l_Player->isAlive())
                                        me->Kill(l_Player);
                                }
                            }
                        });
                    }
                    else
                        m_SafetyTimer -= p_Diff;
                }

                if (!m_Location)
                    return;

                /// Allow players to go back in case of wipe/reset
                if (!m_IsPortalBack && m_Instance && m_Instance->GetBossState(DATA_DRAGON_NIGHTMARE) != IN_PROGRESS)
                    return;

                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        std::list<Player*> l_PlayerList;
                        me->GetPlayerListInGrid(l_PlayerList, 8.0f);

                        for (Player* l_Player : l_PlayerList)
                            l_Player->TeleportTo(m_Location);

                        events.ScheduleEvent(EVENT_1, 500);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_ysondre_rift_controllerAI(p_Creature);
        }
};

/// Lumbering Mindgorger - 108065
class npc_ysondre_lumbering_mindgorger : public CreatureScript
{
    public:
        npc_ysondre_lumbering_mindgorger() : CreatureScript("npc_ysondre_lumbering_mindgorger") { }

        struct npc_ysondre_lumbering_mindgorgerAI : public ScriptedAI
        {
            npc_ysondre_lumbering_mindgorgerAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance          = p_Creature->GetInstanceScript();
                m_EnergizeTimer     = 0;
                m_CrimsonMoonTimer  = 0;
                m_Init              = false;
                m_Location          = 0;
            }

            InstanceScript* m_Instance;

            uint32 m_EnergizeCounter;
            uint32 m_EnergizeTimer;
            uint32 m_CrimsonMoonTimer;

            bool m_Init;

            uint32 m_Location;

            std::array<Position const, ePortals::MaxPortals> m_LumberingMovePos =
            {
                {
                    { -2782.26f, 2485.98f, 92.933f, 0.301f },    ///< Hinterlands
                    { -2756.52f, 3707.35f, 113.33f, 6.070f },    ///< Ashenvale
                    { -4135.26f, 201.862f, 28.383f, 5.803f }     ///< Feralas
                }
            };

            void Reset() override
            {
                if (!m_Init)
                {
                    m_Init = true;

                    me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_REGENERATE_POWER);

                    DoCast(me, SPELL_EMPTY_ENERGY, true);

                    events.Reset();

                    m_EnergizeCounter = 0;
                    m_EnergizeTimer = 1000;
                }
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                me->SetWalk(false);

                if (m_Instance != nullptr)
                    m_Instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                events.ScheduleEvent(EVENT_COLLAPSING_NIGHTMARE, 3000);

                m_CrimsonMoonTimer = 5000;
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (m_Instance != nullptr)
                    m_Instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_COLLAPSING_NIGHTMARE)
                    events.ScheduleEvent(EVENT_COLLAPSING_NIGHTMARE, 1000);
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_DEVOUR_NIGHTMARE)
                {
                    AddTimedDelayedOperation(100, [this]() -> void
                    {
                        Map::PlayerList const& l_PlayerList = me->GetMap()->GetPlayers();
                        for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
                        {
                            if (Player* l_Player = l_Iter->getSource())
                            {
                                if (!l_Player->isGameMaster() && l_Player->isAlive())
                                    me->Kill(l_Player);
                            }
                        }
                    });
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != POINT_MOTION_TYPE)
                    return;

                if (p_ID == 1)
                {
                    AddTimedDelayedOperation(10, [this]() -> void
                    {
                        me->NearTeleportTo(m_LumberingMovePos[m_Location]);
                    });

                    AddTimedDelayedOperation(300, [this]() -> void
                    {
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveRandom(10.0f);
                    });
                }
            }

            void SetData(uint32 /*p_ID*/, uint32 p_Value) override
            {
                m_Location = p_Value;

                AddTimedDelayedOperation(10, [this]() -> void
                {
                    if (m_Location >= ePortals::MaxPortals)
                        return;

                    me->SetWalk(true);

                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(1, m_LumberingMovePos[m_Location], false);
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (m_EnergizeTimer <= p_Diff)
                {
                    ++m_EnergizeCounter;

                    me->ModifyPower(POWER_ENERGY, m_EnergizeCounter % 4 ? 1 : 2);

                    if (me->GetPower(POWER_ENERGY) >= 100)
                        DoCast(me, SPELL_DEVOUR_NIGHTMARE);

                    m_EnergizeTimer = 1000;
                }
                else
                    m_EnergizeTimer -= p_Diff;

                if (!UpdateVictim())
                    return;

                if (m_CrimsonMoonTimer <= p_Diff)
                {
                    DoCast(me, SPELL_CRIMSON_MOON, true);

                    m_CrimsonMoonTimer = 5000;
                }
                else
                    m_CrimsonMoonTimer -= p_Diff;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_STUNNED))
                    return;

                /// Lumbering Mindgorger can auto-attack while casting
                if (me->HasUnitState(UNIT_STATE_CASTING))
                {
                    Unit* l_Victim = me->getVictim();

                    if (!me->IsWithinMeleeRange(l_Victim))
                        return;

                    /// Make sure our attack is ready and we aren't currently casting before checking distance
                    if (me->isAttackReady())
                    {
                        me->ClearUnitState(UNIT_STATE_CANNOT_AUTOATTACK);
                        me->AttackerStateUpdate(l_Victim);
                        me->resetAttackTimer();
                    }

                    return;
                }

                switch (events.ExecuteEvent())
                {
                    case EVENT_COLLAPSING_NIGHTMARE:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM))
                            DoCast(l_Target, SPELL_COLLAPSING_NIGHTMARE);

                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_ysondre_lumbering_mindgorgerAI(p_Creature);
        }
};

/// Essence of Corruption - 103691
class npc_ysondre_essence_of_corruption : public CreatureScript
{
    public:
        npc_ysondre_essence_of_corruption() : CreatureScript("npc_ysondre_essence_of_corruption") { }

        struct npc_ysondre_essence_of_corruptionAI : public ScriptedAI
        {
            npc_ysondre_essence_of_corruptionAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void GetMythicHealthMod(float& p_Mod) override
            {
                p_Mod = 2.9315f;
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                events.ScheduleEvent(EVENT_1, 10000);

                DoZoneInCombat(me, 100.0f);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (events.ExecuteEvent() == EVENT_1)
                {
                    DoCast(me, SPELL_CORRUPTION);

                    events.ScheduleEvent(EVENT_1, 15000);
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_ysondre_essence_of_corruptionAI(p_Creature);
        }
};

/// Energize (Long, DNT) - 205281
/// Energize (Short, DNT) - 205282
class spell_ysondre_periodic_energize : public SpellScriptLoader
{
    public:
        spell_ysondre_periodic_energize() : SpellScriptLoader("spell_ysondre_periodic_energize") { }

        class spell_ysondre_periodic_energize_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ysondre_periodic_energize_AuraScript);

            bool m_FullPower = false;
            int32 m_PowerCount = 0;
            uint32 m_ApplyTime = 0;

            bool Load() override
            {
                m_ApplyTime = uint32(time(nullptr));

                if (Unit* l_Caster = GetCaster())
                {
                    /// Emeriss starts at 56 energy, that's about ~25 seconds of advance
                    if (l_Caster->GetEntry() == NPC_EMERISS)
                        m_ApplyTime -= 25;
                }

                return true;
            }

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (GetCaster() == nullptr)
                    return;

                Creature* l_Caster = GetCaster()->ToCreature();
                if (!l_Caster || !l_Caster->isInCombat())
                    return;

                m_PowerCount = l_Caster->GetPower(POWER_ENERGY);

                if (!m_PowerCount && m_FullPower)
                    m_ApplyTime = uint32(time(nullptr));

                if (m_PowerCount < 100)
                {
                    switch (l_Caster->GetEntry())
                    {
                        /// Ysondre reaches 100 energy every 34 seconds
                        case NPC_YSONDRE:
                        {
                            uint32 l_CurrTime   = uint32(time(nullptr));
                            int32 l_CurPower    = l_Caster->GetPower(POWER_ENERGY);
                            int32 l_NewPower    = std::min<uint32>((l_CurrTime - m_ApplyTime) * 100 / 34, 100);

                            if (l_NewPower <= l_CurPower)
                                break;

                            l_Caster->EnergizeBySpell(l_Caster, GetSpellInfo()->Id, l_NewPower - l_CurPower, POWER_ENERGY);
                            break;
                        }
                        /// Lethon and Taerar reach 100 energy every 50 seconds
                        case NPC_LETHON:
                        case NPC_TAERAR:
                        {
                            l_Caster->EnergizeBySpell(l_Caster, GetSpellInfo()->Id, 2, POWER_ENERGY);
                            break;
                        }
                        /// Emeris reaches 100 energy every 45 seconds
                        default:
                        {
                            uint32 l_CurrTime   = uint32(time(nullptr));
                            int32 l_CurPower    = l_Caster->GetPower(POWER_ENERGY);
                            int32 l_NewPower    = std::min<uint32>((l_CurrTime - m_ApplyTime) * 100 / 45, 100);

                            if (l_NewPower <= l_CurPower)
                                break;

                            l_Caster->EnergizeBySpell(l_Caster, GetSpellInfo()->Id, l_NewPower - l_CurPower, POWER_ENERGY);
                            break;
                        }
                    }

                    if (m_FullPower)
                        m_FullPower = false;
                }
                else
                {
                    if (!m_FullPower)
                    {
                        m_FullPower = true;

                        if (l_Caster->IsAIEnabled)
                        {
                            if (l_Caster->GetEntry() == NPC_YSONDRE)
                                l_Caster->AI()->DoAction(ACTION_4, nullptr);
                            else
                                l_Caster->AI()->DoAction(ACTION_4);
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_ysondre_periodic_energize_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_ysondre_periodic_energize_AuraScript();
        }
};

/// Mark of Ysondre - 203102
/// Mark of Taerar - 203121
/// Mark of Lethon - 203124
/// Mark of Emeriss - 203125
class spell_ysondre_marks : public SpellScriptLoader
{
    public:
        spell_ysondre_marks() : SpellScriptLoader("spell_ysondre_marks") { }

        class spell_ysondre_marks_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ysondre_marks_AuraScript);

            void OnApply(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                if (!GetTarget())
                    return;

                if (Aura* l_Aura = p_AurEff->GetBase())
                {
                    if (l_Aura->GetStackAmount() == 10)
                        GetTarget()->CastSpell(GetTarget(), SPELL_SLUMBERING_NIGHTMARE, true);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_ysondre_marks_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAPPLY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_ysondre_marks_AuraScript();
        }
};

/// Slumbering Nightmare - 203110
class spell_ysondre_slumbering_nightmare : public SpellScriptLoader
{
    public:
        spell_ysondre_slumbering_nightmare() : SpellScriptLoader("spell_ysondre_slumbering_nightmare") { }

        class spell_ysondre_slumbering_nightmare_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ysondre_slumbering_nightmare_AuraScript);

            void CalculateMaxDuration(int32& p_Duration)
            {
                if (!GetCaster())
                    return;

                if (GetCaster()->GetMap()->IsLFR())
                    p_Duration = 10000;
                else
                    p_Duration = 30000;
            }

            void Register() override
            {
                DoCalcMaxDuration += AuraCalcMaxDurationFn(spell_ysondre_slumbering_nightmare_AuraScript::CalculateMaxDuration);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_ysondre_slumbering_nightmare_AuraScript();
        }
};

/// Nightmare Bloom - 203686
class spell_ysondre_nightmare_bloom : public SpellScriptLoader
{
    public:
        spell_ysondre_nightmare_bloom() : SpellScriptLoader("spell_ysondre_nightmare_bloom") { }

        class spell_ysondre_nightmare_bloom_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ysondre_nightmare_bloom_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty() && GetCaster())
                {
                    if (Unit* l_Summoner = GetCaster()->GetAnyOwner())
                        GetCaster()->CastSpell(GetCaster(), 203667, true, 0, 0, l_Summoner->GetGUID());
                }
            }

            void HandleOnHit()
            {
                if (GetCaster() && GetHitUnit())
                    GetCaster()->CastSpell(GetHitUnit(), 203690, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ysondre_nightmare_bloom_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnHit += SpellHitFn(spell_ysondre_nightmare_bloom_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_ysondre_nightmare_bloom_SpellScript();
        }
};

/// Shadow Burst - 204040, 204044
class spell_ysondre_shadow_burst : public SpellScriptLoader
{
    public:
        spell_ysondre_shadow_burst() : SpellScriptLoader("spell_ysondre_shadow_burst") { }

        enum eSpells
        {
            SpellShadowBurstAoE = 204044
        };

        class spell_ysondre_shadow_burst_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ysondre_shadow_burst_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (GetCaster() && GetSpellInfo()->Id == 204044 && !p_Targets.empty())
                    p_Targets.sort(JadeCore::UnitSortDistance(true, GetCaster()));
            }

            void Register() override
            {
                if (m_scriptSpellId != eSpells::SpellShadowBurstAoE)
                    return;

                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ysondre_shadow_burst_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ysondre_shadow_burst_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ALLY);
            }
        };

        class spell_ysondre_shadow_burst_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ysondre_shadow_burst_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (!GetCaster() || !GetTarget() || GetId() != 204040)
                    return;

                if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
                    GetTarget()->CastSpell(GetTarget(), 204044, true, 0, 0, GetCaster()->GetGUID());
            }

            void Register() override
            {
                if (m_scriptSpellId == eSpells::SpellShadowBurstAoE)
                    return;

                AfterEffectRemove += AuraEffectRemoveFn(spell_ysondre_shadow_burst_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_ysondre_shadow_burst_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_ysondre_shadow_burst_AuraScript();
        }
};

/// Bellowing Roar - 205172
class spell_ysondre_bellowing_roar : public SpellScriptLoader
{
    public:
        spell_ysondre_bellowing_roar() : SpellScriptLoader("spell_ysondre_bellowing_roar") { }

        class spell_ysondre_bellowing_roar_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ysondre_bellowing_roar_AuraScript);

            void OnPeriodic(AuraEffect const* /*p_AurEff*/)
            {
                if (!GetCaster())
                    return;

                GetCaster()->CastSpell(GetCaster(), SPELL_BELLOWING_ROAR);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_ysondre_bellowing_roar_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_ysondre_bellowing_roar_AuraScript();
        }
};

/// Siphon Spirit - 203888
class spell_ysondre_siphon_spirit : public SpellScriptLoader
{
    public:
        spell_ysondre_siphon_spirit() : SpellScriptLoader("spell_ysondre_siphon_spirit") { }

        class spell_ysondre_siphon_spirit_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ysondre_siphon_spirit_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                Creature* l_Lethon = GetCaster()->ToCreature();
                if (l_Lethon == nullptr)
                    return;

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(SPELL_MARK_OF_YSONDRE_AURA, l_Lethon->GetMap()->GetDifficultyID());
                if (l_SpellInfo == nullptr)
                    return;

                Creature* l_Ysondre = nullptr;
                if (InstanceScript* l_Instance = l_Lethon->GetInstanceScript())
                    l_Ysondre = Creature::GetCreature(*l_Lethon, l_Instance->GetData64(NPC_YSONDRE));

                if (l_Ysondre == nullptr)
                    return;

                float l_Radius = l_SpellInfo->Effects[EFFECT_0].CalcRadius(l_Ysondre);

                /// Lethon will rip the souls out of 3 players who are within range of Ysondre's Mark of Ysondre debuff, across the room from him
                p_Targets.remove_if([=](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr || !p_Object->IsPlayer())
                        return true;

                    if (p_Object->GetDistance(l_Ysondre) > l_Radius)
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ysondre_siphon_spirit_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_ysondre_siphon_spirit_SpellScript();
        }
};

/// Nightmare Bloom (Visual) - 225699
class spell_ysondre_nightmare_bloom_visual : public SpellScriptLoader
{
    public:
        spell_ysondre_nightmare_bloom_visual() : SpellScriptLoader("spell_ysondre_nightmare_bloom_visual") { }

        class spell_ysondre_nightmare_bloom_visual_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ysondre_nightmare_bloom_visual_AuraScript);

            void AfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (!GetTarget())
                    return;

                if (Creature* l_Target = GetTarget()->ToCreature())
                {
                    if (l_Target->IsAIEnabled)
                        l_Target->AI()->SetData(0, 0);
                }
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (!GetTarget())
                    return;

                if (Creature* l_Target = GetTarget()->ToCreature())
                {
                    if (l_Target->IsAIEnabled)
                        l_Target->AI()->SetData(0, 1);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_ysondre_nightmare_bloom_visual_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_ysondre_nightmare_bloom_visual_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_ysondre_nightmare_bloom_visual_AuraScript();
        }
};

/// Volatile Infection - 203787
class spell_ysondre_volatile_infection : public SpellScriptLoader
{
    public:
        spell_ysondre_volatile_infection() : SpellScriptLoader("spell_ysondre_volatile_infection") { }

        class spell_ysondre_volatile_infection_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ysondre_volatile_infection_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if([](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr || !p_Object->IsPlayer())
                        return true;

                    if (p_Object->ToPlayer()->GetRoleForGroup() == Roles::ROLE_TANK)
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ysondre_volatile_infection_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_ysondre_volatile_infection_SpellScript();
        }
};

/// Nightmare Bloom - 203687
class areatrigger_ysondre_nightmare_bloom : public AreaTriggerEntityScript
{
    public:
        areatrigger_ysondre_nightmare_bloom() : AreaTriggerEntityScript("areatrigger_ysondre_nightmare_bloom") { }

        std::set<uint64> m_Targets;

        bool OnAddTarget(AreaTrigger* p_AreaTrigger, Unit* p_Target) override
        {
            m_Targets.insert(p_Target->GetGUID());
            return false;
        }

        bool OnRemoveTarget(AreaTrigger* p_AreaTrigger, Unit* p_Target) override
        {
            m_Targets.erase(p_Target->GetGUID());

            /// If a player is still soaking the Blossom, don't reapply the visual
            if (!m_Targets.empty())
                return true;

            return false;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_ysondre_nightmare_bloom();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_dragons_of_nightmare()
{
    /// Bosses
    new boss_dragon_ysondre();
    new boss_dragon_taerar();
    new boss_dragon_lethon();
    new boss_dragon_emeriss();

    /// Creatures
    new npc_ysondre_nightmare_bloom();
    new npc_ysondre_defiled_druid_spirit();
    new npc_ysondre_shade_of_taerar();
    new npc_ysondre_spirit_shade();
    new npc_ysondre_dread_horror();
    new npc_ysondre_corrupted_mushroom();
    new npc_ysondre_rift_controller();
    new npc_ysondre_lumbering_mindgorger();
    new npc_ysondre_essence_of_corruption();

    /// Spells
    new spell_ysondre_shadow_burst();
    new spell_ysondre_periodic_energize();
    new spell_ysondre_marks();
    new spell_ysondre_slumbering_nightmare();
    new spell_ysondre_nightmare_bloom();
    new spell_ysondre_bellowing_roar();
    new spell_ysondre_siphon_spirit();
    new spell_ysondre_nightmare_bloom_visual();
    new spell_ysondre_volatile_infection();

    /// AreaTrigger
    new areatrigger_ysondre_nightmare_bloom();
}
#endif
