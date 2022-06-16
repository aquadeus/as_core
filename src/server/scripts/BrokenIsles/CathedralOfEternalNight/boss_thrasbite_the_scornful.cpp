#include "cathedral_of_eternal_night.hpp"

enum eSpells
{
    SpellPulverizingCudgel  = 237276,
    SpellHeaveCudgel        = 243124,
    SpellHeaveCudgelDisarm  = 243153,
    SpellDisarm             = 243158,
    SpellDestructiveRampage = 240948,
    SpellScornfulGaze       = 237726,
    SpellScornfulCharge     = 237891,
    SpellScornfulChargeMove = 238462,
    SpellScornfulChargeDmg  = 238469,
    SpellJumpDest           = 187564,

};

enum eEvents
{
    EventPulverizingCudgel  = 1,
    EventScornfulGaze,
    EventHeaveCudgel,
};

enum eTalks
{
    TalkIntro,
    TalkAggro,
    TalkCudgel,
    TalkGaze,
    TalkKill,
    TalkWipe,
    TalkDied,
};

enum ePoints
{
    PointJumpPos    = 10,
    PointWeapon,
};

enum eActions
{
    ActionWeaponDespawn = 30,
};

enum eScornfulData
{
    DataMarkedPlayer = 30,
    DataBookCase,
};

/// Trashbite the Scornful - 117194
class boss_trashbite_the_scornful : public CreatureScript
{
    public:
        boss_trashbite_the_scornful() : CreatureScript("boss_trashbite_the_scornful")
        {}

        struct boss_trashbite_the_scornful_AI : public BossAI
        {
            public:

                explicit boss_trashbite_the_scornful_AI(Creature* p_Me) : BossAI(p_Me, eData::DataTrashbiteTheScornful)
                {
                    m_Intro = false;
                    m_MarkedPlayer = 0;
                }

                void SetGUID(uint64 p_Guid, int32 p_Id) override
                {
                    if (p_Id == eScornfulData::DataMarkedPlayer)
                        m_MarkedPlayer = p_Guid;
                    else if (p_Id == eScornfulData::DataBookCase)
                        m_BookCaseGuid = p_Guid;
                }

                uint64 GetGUID(int32 p_Id) override
                {
                    if (p_Id == eScornfulData::DataMarkedPlayer)
                        return m_MarkedPlayer;

                    return 0;
                }

                void CleanUp()
                {
                    me->RemoveAllAreasTrigger();
                    m_DamageSaved.fill(0);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSharedSpells::SpellMidlyAmused);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSharedSpells::SpellModeratelyStirred);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSharedSpells::SpellFairlyExcited);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSharedSpells::SpellTotallyTantalized);
                }

                void Reset() override
                {
                    _Reset();

                    CleanUp();

                    if (IsMythic())
                        me->UpdateStatsForLevel();

                    AddTimedDelayedOperation(500, [this]() -> void
                    {
                        CheckIntro();
                        RestoreBookCases();
                    });
                }

                void GetMythicHealthMod(float& p_HealthMod) override
                {
                    p_HealthMod = 2.0312f;
                }

                void JustSummoned(Creature* p_Summon) override
                {
                    BossAI::JustSummoned(p_Summon);
                    DoZoneInCombat(p_Summon, 100.f);
                }

                void CheckIntro()
                {
                    InstanceScript* l_Instance = me->GetInstanceScript();

                    if (l_Instance->GetData(me->GetEntry()) == EncounterState::DONE)
                    {
                        m_Intro = true;
                        me->SetVisible(true);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                        me->NearTeleportTo(g_ScornfulJumpPos);
                        me->SetHomePosition(g_ScornfulJumpPos);
                    }
                    else
                    {
                        m_Intro = false;
                        me->SetVisible(false);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                    }
                }

                void RestoreBookCases()
                {
                    for (uint32 l_Itr = eGameObjects::GoBookCase01; l_Itr <= eGameObjects::GoBookCase04; ++l_Itr)
                    {
                        GameObject* l_BookCase = me->FindNearestGameObject(l_Itr, 150.f);

                        if (l_BookCase != nullptr)
                            l_BookCase->SetGoState(GO_STATE_READY);
                    }
                }

                void DoAction(int32 const p_Action) override
                {
                    if (p_Action == eSharedActions::ActionBossIntro)
                    {
                        AddTimedDelayedOperation(2500, [this] () -> void
                        {
                            me->SetVisible(true);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                            me->CastSpell(g_ScornfulJumpPos, eSpells::SpellJumpDest, true);
                            me->SetHomePosition(g_ScornfulJumpPos);
                        });
                    }
                    else if (p_Action == eActions::ActionWeaponDespawn)
                    {
                        SetEquipmentSlots(true);
                        me->UpdateDamagePhysical(WeaponAttackType::BaseAttack);
                        events.ScheduleEvent(eEvents::EventScornfulGaze, IN_MILLISECONDS);
                    }
                }

                void SummonBooks()
                {
                    std::array<uint32, 3> l_Books =
                    {
                        {
                            eCreatures::NpcSatiricalBook,
                            eCreatures::NpcAnimatedBook,
                            eCreatures::NpcBiographyBook
                        }
                    };

                    for (uint8 l_Itr = 0; l_Itr < 2; ++l_Itr)
                        me->SummonCreature(l_Books.at(urand(0, 2)), me->GetPosition(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                }

                void MovementInform(uint32 p_Type, uint32 p_Id) override
                {
                    if (p_Id == eSpells::SpellJumpDest)
                    {
                        Talk(eTalks::TalkIntro);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                    }
                    else if (p_Id == 1003)
                    {
                        GameObject* l_BookCase = sObjectAccessor->GetGameObject(*me, m_BookCaseGuid);

                        if (l_BookCase != nullptr)
                        {
                            l_BookCase->SetGoState(GOState::GO_STATE_ACTIVE);

                            if (IsMythic())
                                SummonBooks();
                        }
                    }
                }

                void JustReachedHome() override
                {
                    Talk(eTalks::TalkWipe);
                    RestoreBookCases();
                    _JustReachedHome();
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                }

                void EnterCombat(Unit* /**/) override
                {
                    Talk(eTalks::TalkAggro);
                    _EnterCombat();
                    events.ScheduleEvent(eEvents::EventPulverizingCudgel, 6 * IN_MILLISECONDS);
                    events.ScheduleEvent(eEvents::EventHeaveCudgel, 17 * IN_MILLISECONDS);
                }

                void KilledUnit(Unit* p_Victim) override
                {
                    if (p_Victim == nullptr)
                        return;

                    if (p_Victim->IsPlayer())
                        Talk(eTalks::TalkKill);
                }

                void CheckAchievement()
                {
                    Map::PlayerList const& l_PlayerList = me->GetMap()->GetPlayers();
                    bool l_Result = false;

                    for (Map::PlayerList::const_iterator l_Iter = l_PlayerList.begin(); l_Iter != l_PlayerList.end(); ++l_Iter)
                    {
                        Player* l_Player = l_Iter->getSource();

                        if (l_Player == nullptr)
                            continue;

                        l_Result = l_Player->HasAura(eSharedSpells::SpellTotallyTantalized);
                    }

                    if (l_Result && IsMythic() && l_PlayerList.getSize() >= 5)
                        instance->DoCompleteAchievement(eAchievements::SteamyRomanceSaga);
                }

                void JustDied(Unit* /**/) override
                {
                    CheckAchievement();
                    Talk(eTalks::TalkDied);
                    me->RemoveAllAreasTrigger();
                    _JustDied();
                    CleanUp();
                }

                void ClearWeapons()
                {
                    SetEquipmentSlots(false, EQUIP_UNEQUIP, EQUIP_UNEQUIP, EQUIP_NO_CHANGE);
                    me->UpdateDamagePhysical(WeaponAttackType::BaseAttack);
                }

                void OnSpellCasted(SpellInfo const* p_Spell) override
                {
                    if (p_Spell == nullptr)
                        return;

                    switch (p_Spell->Id)
                    {
                        case eSpells::SpellHeaveCudgel:
                        {
                            ClearWeapons();
                            break;
                        }

                        default: break;
                    }
                }

                void ExecuteEvent(uint32 const p_EventId) override
                {
                    switch (p_EventId)
                    {
                        case eEvents::EventHeaveCudgel:
                        {
                            const Position l_Pos = { g_WeaponCirclePath.front().x, g_WeaponCirclePath.front().y, g_WeaponCirclePath.front().z };
                            me->CastSpell(l_Pos, eSpells::SpellHeaveCudgel, false);
                            events.ScheduleEvent(eEvents::EventHeaveCudgel, 38 * IN_MILLISECONDS);
                            break;
                        }

                        case eEvents::EventPulverizingCudgel:
                        {
                            Talk(eTalks::TalkCudgel);
                            DoCastAOE(eSpells::SpellPulverizingCudgel);
                            events.ScheduleEvent(eEvents::EventPulverizingCudgel, 38 * IN_MILLISECONDS);
                            break;
                        }

                        case eEvents::EventScornfulGaze:
                        {
                            m_BookCaseGuid = 0;
                            m_MarkedPlayer = 0;
                            Talk(eTalks::TalkGaze);
                            DoCastAOE(eSpells::SpellScornfulGaze);
                            events.ScheduleEvent(eEvents::EventScornfulGaze, 38 * IN_MILLISECONDS);
                            break;
                        }

                        default: break;
                    }
                }

                void UpdateAI(uint32 const p_Diff) override
                {
                    UpdateOperations(p_Diff);
                    BossAI::UpdateAI(p_Diff);
                }

            private:
                bool m_Intro;
                uint64 m_MarkedPlayer, m_BookCaseGuid;
                std::array<uint32, 5> m_DamageSaved;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new boss_trashbite_the_scornful_AI(p_Me);
        }
};

/// Heavecudgel - 121710
class npc_trashbite_scornful_heavecudgel : public CreatureScript
{
    public:
        npc_trashbite_scornful_heavecudgel() : CreatureScript("npc_trashbite_scornful_heavecudgel")
        {}

        struct npc_trashbite_scornful_heavecudgel_AI : public ScriptedAI
        {
            explicit npc_trashbite_scornful_heavecudgel_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetSpeed(MOVE_RUN, 4.0f, true);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            void JustDespawned() override
            {
                InstanceScript* l_Instance = me->GetInstanceScript();

                if (l_Instance != nullptr)
                {
                    Unit* l_Boss = sObjectAccessor->GetUnit(*me, l_Instance->GetData64(eCreatures::BossTrashBiteTheScornful));

                    if (l_Boss != nullptr)
                    {
                        l_Boss->RemoveAurasDueToSpell(eSpells::SpellDisarm);

                        if (l_Boss->IsAIEnabled)
                            l_Boss->GetAI()->DoAction(eActions::ActionWeaponDespawn);
                    }
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_Id) override
            {
                if (p_Id == ePoints::PointWeapon)
                    me->GetMotionMaster()->MoveSmoothPath(ePoints::PointWeapon, g_WeaponCirclePath.data(), g_WeaponCirclePath.size(), false);
            }

            void Reset() override
            {
                AddTimedDelayedOperation(100, [this]() -> void
                {
                    DoCast(me, eSpells::SpellDestructiveRampage, true);
                    me->GetMotionMaster()->MoveSmoothPath(ePoints::PointWeapon, g_WeaponCirclePath.data(), g_WeaponCirclePath.size(), false);
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_trashbite_scornful_heavecudgel_AI(p_Me);
        }
};

/// Books - 121392, 121384, 121364
class npc_trashbite_scornful_book : public CreatureScript
{
    public:
        npc_trashbite_scornful_book() : CreatureScript("npc_trashbite_scornful_book")
        {}

        enum eSpells
        {
            SpellArcaneBolt         = 242170,
            SpellStiflingSatire     = 238678,
            SpellFetteringFiction   = 238480,
            SpellBeguilingBiography = 238483,
        };

        enum eEvents
        {
            EventArcaneBolt = 1,
            EventStilflingSatire,
            EventFetteringFiction,
            EventBeguilingBiography,
        };

        struct npc_trashbite_scornful_book_AI : public ScriptedAI
        {
            explicit npc_trashbite_scornful_book_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void GetMythicHealthMod(float& p_Mod) override
            {
                p_Mod = 1.0628f;
            }

            void EnterCombat(Unit* /**/) override
            {
                events.ScheduleEvent(eEvents::EventArcaneBolt, urand(6, 12) * IN_MILLISECONDS);
                ScheduleTasks();
            }

            void JustDied(Unit* /**/) override
            {
                events.Reset();
            }

            void Reset() override
            {
                events.Reset();

                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void ScheduleTasks()
            {
                switch (me->GetEntry())
                {
                    case eCreatures::NpcAnimatedBook:
                    {
                        events.ScheduleEvent(eEvents::EventFetteringFiction, urand(4, 10) * IN_MILLISECONDS);
                        break;
                    }

                    case eCreatures::NpcSatiricalBook:
                    {
                        events.ScheduleEvent(eEvents::EventStilflingSatire, urand(8, 12) * IN_MILLISECONDS);
                        break;
                    }

                    case eCreatures::NpcBiographyBook:
                    {
                        events.ScheduleEvent(eEvents::EventBeguilingBiography, urand(10, 12) * IN_MILLISECONDS);
                        break;
                    }

                    default: break;
               }
            }

            void ExecuteEvent(uint32 const p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventArcaneBolt:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                            DoCast(l_Target, eSpells::SpellArcaneBolt);

                        events.ScheduleEvent(eEvents::EventArcaneBolt, urand(4, 10) * IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventStilflingSatire:
                    {
                        DoCastAOE(eSpells::SpellStiflingSatire);
                        events.ScheduleEvent(eEvents::EventStilflingSatire, urand(15, 20) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventBeguilingBiography:
                    {
                        DoCastAOE(eSpells::SpellBeguilingBiography);
                        events.ScheduleEvent(eEvents::EventBeguilingBiography, urand(10, 20) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventFetteringFiction:
                    {
                        DoCastAOE(eSpells::SpellFetteringFiction);
                        events.ScheduleEvent(eEvents::EventFetteringFiction, urand(12, 24) * IN_MILLISECONDS);
                        break;
                    }

                    default: break;
                }

            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_trashbite_scornful_book_AI(p_Me);
        }
};

/// Scornful Gaze - 237726
class spell_trashbite_scornful_gaze : public SpellScriptLoader
{
    public:
        spell_trashbite_scornful_gaze() : SpellScriptLoader("spell_trashbite_scornful_gaze")
        {}

        class spell_trashbite_scornful_gaze_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_trashbite_scornful_gaze_AuraScript);

            void HandleOnRemove(AuraEffect const* /**/, AuraEffectHandleModes /**/)
            {
                if (GetCaster() == nullptr || GetUnitOwner() == nullptr)
                    return;

                Unit* l_Caster = GetCaster();
                Unit* l_Owner = GetUnitOwner();

                AuraRemoveMode removeMode = GetTargetApplication()->GetRemoveMode();
                if (removeMode != AURA_REMOVE_BY_EXPIRE)
                    return;

                if (l_Caster->IsAIEnabled)
                    l_Caster->GetAI()->SetGUID(l_Owner->GetGUID(), eScornfulData::DataMarkedPlayer);

                l_Caster->CastSpell(l_Owner, eSpells::SpellScornfulChargeMove, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_trashbite_scornful_gaze_AuraScript::HandleOnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_trashbite_scornful_gaze_AuraScript();
        }
};

/// Scornful Charge Move - 238462
class spell_trashbite_scornful_charge : public SpellScriptLoader
{
    public:
        spell_trashbite_scornful_charge() : SpellScriptLoader("spell_trashbite_scornful_charge")
        {}

        class spell_trashbite_scornful_charge_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_trashbite_scornful_charge_SpellScript);

            bool Load() override
            {
                m_BookCaseGuid = 0;
                m_NovelId = 0;

                m_BooksAchievement =
                {
                    {
                        std::make_pair(eGameObjects::GoBookCase03, NpcSteamyRomanceSaga1),
                        std::make_pair(eGameObjects::GoBookCase04, NpcSteamyRomanceSaga2),
                        std::make_pair(eGameObjects::GoBookCase01, NpcSteamyRomanceSaga3),
                        std::make_pair(eGameObjects::GoBookCase02, NpcSteamyRomanceSaga4)
                    }
                };

                return true;
            }

            void HandleBeforeCast()
            {
                if (GetCaster() == nullptr || GetExplTargetDest() == nullptr)
                    return;

                Unit* l_Caster = GetCaster();

                if (!l_Caster->IsAIEnabled)
                    return;

                Unit* l_Target = sObjectAccessor->GetUnit(*l_Caster, l_Caster->GetAI()->GetGUID(eScornfulData::DataMarkedPlayer));

                if (l_Target == nullptr)
                    return;

                for (uint32 l_Itr = eGameObjects::GoBookCase01; l_Itr <= eGameObjects::GoBookCase04; ++l_Itr)
                {
                    GameObject* l_BookCase = l_Caster->FindNearestGameObject(l_Itr, 150.f);

                    if (l_BookCase == nullptr)
                        continue;

                    if (l_BookCase->isInFront(l_Target))
                    {
                        m_BookCaseGuid = l_BookCase->GetGUID();

                        if (l_Caster->GetMap() && l_Caster->GetMap()->IsMythic())
                        {
                            auto l_Itr = std::find_if(m_BooksAchievement.begin(), m_BooksAchievement.end(), [&l_BookCase](auto& p_Itr) -> bool
                            {
                                return l_BookCase->GetEntry() == p_Itr.first;
                            });

                            if (l_Itr != m_BooksAchievement.end())
                                m_NovelId = l_Itr->second;
                        }

                        if (l_Caster->IsAIEnabled)
                            l_Caster->GetAI()->SetGUID(m_BookCaseGuid, eScornfulData::DataBookCase);

                        WorldLocation l_NewDest;

                        l_NewDest.Relocate(l_BookCase->GetPosition());

                        SetExplTargetDest(l_NewDest);

                        return;
                    }
                }
            }

            void HandleOnHit(SpellEffIndex /**/)
            {
                if (GetHitDest() == nullptr)
                    return;

                WorldLocation* l_Dest = GetHitDest();

                if (m_BookCaseGuid == 0)
                    GetCaster()->CastSpell(l_Dest, eSpells::SpellScornfulChargeDmg, true);
                else if (m_NovelId != 0)
                    GetCaster()->SummonCreature(m_NovelId, *l_Dest);
            }

            void Register() override
            {
                BeforeCast      += SpellCastFn(spell_trashbite_scornful_charge_SpellScript::HandleBeforeCast);
                OnEffectHit     += SpellEffectFn(spell_trashbite_scornful_charge_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_CHARGE_DEST);
            }

            uint64 m_BookCaseGuid;
            uint32 m_NovelId;
            std::array<std::pair<uint32, uint32>, 4> m_BooksAchievement;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_trashbite_scornful_charge_SpellScript();
        }
};

/// Beguiling Biography - 238484
class spell_trashbite_beguiling_biography : public SpellScriptLoader
{
    public:
        spell_trashbite_beguiling_biography() : SpellScriptLoader("spell_trashbite_beguiling_biography")
        {}

        class spell_trashbite_beguiling_biography_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_trashbite_beguiling_biography_AuraScript);

            void HandlePeriodic(AuraEffect const* /**/)
            {
                if (GetUnitOwner() == nullptr)
                    return;

                if (GetUnitOwner()->HealthBelowPct(30))
                    Remove();
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_trashbite_beguiling_biography_AuraScript::HandlePeriodic, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_trashbite_beguiling_biography_AuraScript();
        }
};

/// Heave Cudgel - 243124
class spell_trashbite_scornful_heave_cudgel : public SpellScriptLoader
{
    public:
        spell_trashbite_scornful_heave_cudgel() : SpellScriptLoader("spell_trashbite_scornful_heave_cudgel")
        {}

        class spell_trashbite_scornful_heave_cudgel_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_trashbite_scornful_heave_cudgel_SpellScript);

            void HandleBeforeCast()
            {
                if (GetCaster() == nullptr)
                    return;

                Position l_Pos;
                Unit* l_Caster = GetCaster();

                WorldLocation l_Loc(l_Caster->GetMapId(), g_WeaponCirclePath.front().x, g_WeaponCirclePath.front().y, g_WeaponCirclePath.front().z);
                SetExplTargetDest(l_Loc);
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_trashbite_scornful_heave_cudgel_SpellScript::HandleBeforeCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_trashbite_scornful_heave_cudgel_SpellScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_trashbite_the_scornful()
{
    new boss_trashbite_the_scornful();
    new npc_trashbite_scornful_heavecudgel();
    new npc_trashbite_scornful_book();
    new spell_trashbite_scornful_gaze();
    new spell_trashbite_scornful_charge();
    new spell_trashbite_beguiling_biography();
    new spell_trashbite_scornful_heave_cudgel();
}
#endif
