////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "AreaTrigger.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SpellAuras.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "Containers.h"
#include "tomb_of_sargeras.h"

enum eLavaStalkerTactic
{
    LavaStalkerTopLeftTactic = 0,
    LavaStalkerTopRightTactic,
    LavaStalkerBottomLeftTactic,
    LavaStalkerBottomRightTactic,
    LavaStalkerMaxTactic
};

enum eSpells
{
    BurningArmor                 = 231363,
    MeltedArmor                  = 234264,
    BurningEruption              = 231395,
    CrashingComet                = 232249,
    CrashingCometTriggered       = 230345,
    InfernalSpikeSummon          = 233055,
    InfernalSpike                = 233019,
    InfernalAura                 = 230439,
    ShatteringStar               = 233272,
    ShatteringStarAT             = 233279,
    ShatteringStarDamage         = 233281,
    ShatteringStarLastDamage     = 233283,
    StarBurn                     = 236329,
    InfernalBurning              = 233062,
    InfernalSpikeProtection      = 234475,
    GorothEnergy                 = 237333,
    ///< Heroic
    FelEruption                  = 234346,
    FelEruptionTelegraph         = 234366,
    FelPoolAreaTrigger           = 234330,
    FelPoolAura                  = 230348,
    FelPeriodicTrigger           = 234380,
    ///< Mythic
    RainofBrimstoneEffect        = 233285, ///< only for addons
    RainofBrimstone              = 238587,
    RainofBrimstoneSummon        = 233266,
    FelFireAura                  = 241455, ///< Aura for Brimstone Infernal
    InfernalDetonation           = 233900 ///< Inflicts damage when destroy Infernal Spike
};

enum eEvents
{
    EventBurningArmor = 1,
    EventCrashingComet,
    EventInfernalSpikeSummon,
    EventShatteringStar,
    EventInfernalBurning,
    EventRainofBrimstone,
    EventCheckDestroySpike,
    EventActionLavaStalker
};

enum eMisc
{
    NpcInfernalSpike                 = 116976,
    SpellvisualInfernalSpikeDestroy  = 66119,
    GoInfernalSpike                  = 266938,
    NpcLavaStalker                   = 117931,
    NpcBrimstoneInfernal             = 119950
};

enum eTalks
{
    TALK_AGGRO = 1,
    TALK_AGGRO2,
    TALK_AGGRO3,
    TALK_DIED,
    EMOTE_INFERNALBURNING,
    TALK_INFERNALBURNING,
    TALK_SHATERINGSTAR,
    TALK_SHATERINGSTAR2,
    TALK_SHATERINGSTAR3,
    TALK_SLAY,
    TALK_SLAY2,
    TALK_SLAY3,
    TALK_WIPE,
    TALK_WIPE2
};

enum eAchievGoroth
{
    AchievementFelTurkey    = 11724,
    AchievementMythicGoroth = 11767
};

const uint32 shatteringMythicTimers [5] = { 34, 60, 60, 60, 32 };

class EventLavaStalker: public BasicEvent
{
    public:

    explicit EventLavaStalker(Creature* p_Creature) : pLavaStalker(p_Creature)
    {
    }

    bool Execute(uint64 /*p_CurrTime*/, uint32 /*p_Diff*/)
    {
        if (pLavaStalker)
        {
            pLavaStalker->CastSpell(pLavaStalker, eSpells::FelEruptionTelegraph, true);
            pLavaStalker->CastSpell(pLavaStalker, eSpells::FelEruption, true);
            pLavaStalker->CastSpell(pLavaStalker, eSpells::FelPeriodicTrigger, true);
        }

        return true;
    }

private:
    Creature* pLavaStalker;
};

class boss_goroth : public CreatureScript
{
    public:
        boss_goroth() : CreatureScript("boss_goroth") { }

        struct boss_gorothAI : public BossAI
        {
            boss_gorothAI(Creature* creature) : BossAI(creature, eData::DataGoroth)
            {
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_STUN, true);
            }

            void InitializeAI() override
            {
                if (!instance || static_cast<InstanceMap*>(me->GetMap())->GetScriptId() != sObjectMgr->GetScriptId(TOSScriptName))
                    me->IsAIEnabled = false;
                else if (!me->isDead())
                    Reset();
            }

            void ClearCount()
            {
                _shatteringStarTargetGUID = 0;
                _countArmor = 0;
                _countComet = 0;
                _countShatter = 0;
                _countSpike = 0;
                _countRain = 0;
                _tacticLavaStalker = urand(LavaStalkerTopLeftTactic, LavaStalkerBottomRightTactic);
                _countInfernalChaosbringer = 0;
                _countDataFelTurkey = 0;
                _prevtacticLavaStalker = _tacticLavaStalker;
            }

            void ClearOther()
            {
                me->RemoveAurasDueToSpell(eSpells::GorothEnergy);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::BurningArmor);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::MeltedArmor);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::CrashingComet);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::ShatteringStar);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::StarBurn);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::InfernalBurning);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::InfernalSpikeProtection);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::FelPoolAura);

                std::vector<uint32> p_DespawnEntrys = { eMisc::NpcInfernalSpike, eMisc::NpcBrimstoneInfernal };
                me->DespawnCreaturesInArea(p_DespawnEntrys, 200.f);

                std::list<Creature*> l_LavaStalkers;
                GetCreatureListWithEntryInGrid(l_LavaStalkers, me, eMisc::NpcLavaStalker, 500.0f);
                if (!l_LavaStalkers.empty())
                {
                    for (Creature* lavaStalker : l_LavaStalkers)
                    {
                       lavaStalker->m_Events.KillAllEvents(true);
                       lavaStalker->RemoveAurasDueToSpell(eSpells::FelEruptionTelegraph);
                       lavaStalker->RemoveAurasDueToSpell(eSpells::FelEruption);
                       lavaStalker->RemoveAurasDueToSpell(eSpells::FelPeriodicTrigger);
                    }
                }
                me->DespawnAreaTriggersInArea(eSpells::FelPoolAreaTrigger, 200.f);
                me->SetPower(POWER_ENERGY, 0);
            }

            void Reset() override
            {
                _Reset();
                events.Reset();
                ClearCount();
                ClearOther();
                if (instance->GetData(eData::DataPreEventGoroth) == DONE)
                {
                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                    me->SetVisible(true);
                }
                me->SetPower(POWER_ENERGY, 0);
                me->SetCanFly(false);
                me->SetDisableGravity(false);
            }

            bool CanAIAttack(Unit const* l_Target) const override
            {
                return l_Target && !l_Target->HasAura(eMiscSpells::SpellSpiritOfRedemptionAura);
            }

            bool IgnoreFlyingMovementFlagsUpdate() override
            {
                return true;
            }

            bool CanFly() override
            {
                return false;
            }

            void EnterCombat(Unit* /*attacker*/) override
            {
                Talk(urand(eTalks::TALK_AGGRO, eTalks::TALK_AGGRO3));
                _EnterCombat();
                ClearCount();

                events.ScheduleEvent(eEvents::EventCheckDestroySpike, 1 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventBurningArmor, 10 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventCrashingComet, 8500);
                events.ScheduleEvent(eEvents::EventInfernalSpikeSummon, 4800);

                uint32 timeShattering = (IsMythic() ? 34 : 24) * IN_MILLISECONDS;
                _countShatter++;
                events.ScheduleEvent(eEvents::EventShatteringStar, timeShattering);
                events.ScheduleEvent(eEvents::EventInfernalBurning, 54 * IN_MILLISECONDS);
                if (IsMythic())
                {
                    events.ScheduleEvent(eEvents::EventRainofBrimstone, 12 * IN_MILLISECONDS);
                    _countRain++;
                }

                if (IsHeroic() || IsMythic())
                    events.ScheduleEvent(eEvents::EventActionLavaStalker, 40 * IN_MILLISECONDS);

                me->RemoveAurasDueToSpell(eSpells::GorothEnergy);
                me->AddAura(eSpells::GorothEnergy, me);
            }

            uint8 GetLavaStalkerTactics(Creature* pCreature)
            {
                if (pCreature->GetOrientation() == 3.06299f)
                    return LavaStalkerTopRightTactic;

                if ((pCreature->GetPositionX() == 6190.58f && pCreature->GetPositionY() == -760.286f) ||
                    (pCreature->GetPositionX() == 6145.28f && pCreature->GetPositionY() == -737.465f) ||
                    (pCreature->GetPositionX() == 6175.45f && pCreature->GetPositionY() == -743.946f) ||
                    (pCreature->GetPositionX() == 6184.66f && pCreature->GetPositionY() == -752.774f) ||
                    (pCreature->GetPositionX() == 6127.87f && pCreature->GetPositionY() == -736.67f)  ||
                    (pCreature->GetPositionX() == 6160.73f && pCreature->GetPositionY() == -739.231f) ||
                    (pCreature->GetPositionX() == 6133.76f && pCreature->GetPositionY() == -737.195f))
                     return LavaStalkerTopLeftTactic;

                if ((pCreature->GetPositionX() == 6066.89f && pCreature->GetPositionY() == -735.167f) ||
                    (pCreature->GetPositionX() == 6075.59f && pCreature->GetPositionY() == -736.146f) ||
                    (pCreature->GetPositionX() == 6041.21f && pCreature->GetPositionY() == -736.642f) ||
                    (pCreature->GetPositionX() == 6053.99f && pCreature->GetPositionY() == -736.132f) ||
                    (pCreature->GetPositionX() == 6027.95f && pCreature->GetPositionY() == -741.924f) ||
                    (pCreature->GetPositionX() == 6012.29f && pCreature->GetPositionY() == -758.318f) ||
                    (pCreature->GetPositionX() == 6019.32f && pCreature->GetPositionY() == -750.609f) ||
                    (pCreature->GetPositionX() == 6006.95f && pCreature->GetPositionY() == -768.491f))
                     return LavaStalkerBottomLeftTactic;

                if ((pCreature->GetPositionX() == 6075.56f && pCreature->GetPositionY() == -854.344f) ||
                    (pCreature->GetPositionX() == 6067.87f && pCreature->GetPositionY() == -854.542f) ||
                    (pCreature->GetPositionX() == 6060.89f && pCreature->GetPositionY() == -854.802f) ||
                    (pCreature->GetPositionX() == 6041.87f && pCreature->GetPositionY() == -854.049f) ||
                    (pCreature->GetPositionX() == 6053.f    && pCreature->GetPositionY() == -854.846f) ||
                    (pCreature->GetPositionX() == 6025.13f && pCreature->GetPositionY() == -848.597f) ||
                    (pCreature->GetPositionX() == 6012.99f && pCreature->GetPositionY() == -836.207f) ||
                    (pCreature->GetPositionX() == 6004.9f  && pCreature->GetPositionY() == -820.528f))
                     return LavaStalkerBottomRightTactic;

                return LavaStalkerMaxTactic;
            }

            void ActionLavaStalkerTactics()
            {
                std::list<Creature*> l_LavaStalkers;
                GetCreatureListWithEntryInGrid(l_LavaStalkers, me, eMisc::NpcLavaStalker, 500.0f);
                if (l_LavaStalkers.empty())
                    return;

                for (Creature* lavaStalker : l_LavaStalkers)
                {
                     if (_tacticLavaStalker == GetLavaStalkerTactics(lavaStalker))
                     {
                         lavaStalker->m_Events.KillAllEvents(true);
                         lavaStalker->RemoveAurasDueToSpell(eSpells::FelEruptionTelegraph);
                         lavaStalker->RemoveAurasDueToSpell(eSpells::FelEruption);
                         lavaStalker->RemoveAurasDueToSpell(eSpells::FelPeriodicTrigger);

                         lavaStalker->CastSpell(lavaStalker, eSpells::FelEruptionTelegraph, true);
                         lavaStalker->CastSpell(lavaStalker, eSpells::FelEruption, true);
                         lavaStalker->CastSpell(lavaStalker, eSpells::FelPeriodicTrigger, true);
                         lavaStalker->m_Events.AddEvent(new EventLavaStalker(lavaStalker), lavaStalker->m_Events.CalculateTime(10100));
                         lavaStalker->m_Events.AddEvent(new EventLavaStalker(lavaStalker), lavaStalker->m_Events.CalculateTime(20100));
                         lavaStalker->m_Events.AddEvent(new EventLavaStalker(lavaStalker), lavaStalker->m_Events.CalculateTime(30100));
                         lavaStalker->m_Events.AddEvent(new EventLavaStalker(lavaStalker), lavaStalker->m_Events.CalculateTime(40100));
                     }
                }

                _prevtacticLavaStalker = _tacticLavaStalker;
                _tacticLavaStalker = urand(LavaStalkerTopLeftTactic, LavaStalkerBottomRightTactic);
                ///< Calculate Dynamic new tactic
                if (_prevtacticLavaStalker == _tacticLavaStalker)
                    _tacticLavaStalker++;

                if (_tacticLavaStalker >= LavaStalkerMaxTactic)
                    _tacticLavaStalker = urand(LavaStalkerTopLeftTactic, LavaStalkerBottomLeftTactic);

                if (_prevtacticLavaStalker == _tacticLavaStalker)
                    _tacticLavaStalker++;
            }

            void JustReachedHome() override
            {
                 _JustReachedHome();
                 Talk(urand(eTalks::TALK_WIPE, eTalks::TALK_WIPE2));
            }

            void EnterEvadeMode() override
            {
                ClearOther();
                BossAI::EnterEvadeMode();
            }

            void KilledUnit(Unit* p_Who) override
            {
                if (p_Who->IsPlayer())
                    Talk(urand(eTalks::TALK_SLAY, eTalks::TALK_SLAY3));
            }
            
            uint32 GetData(uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case 0:
                        return (uint32)_countDataFelTurkey;
                    default:
                        break;
                }

                return 0;
            }

            void SetData(uint32 p_ID, uint32 /*p_Value*/) override
            {
                switch (p_ID)
                {
                    case 0:
                    {
                        ++_countDataFelTurkey;
                        break;
                    }
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                _JustDied();

                if (!IsLFR() && GetData(0) >= 30)
                    instance->DoCompleteAchievement(eAchievGoroth::AchievementFelTurkey);

                if (IsMythic())
                    instance->DoCompleteAchievement(eAchievGoroth::AchievementMythicGoroth);

                Talk(eTalks::TALK_DIED);
                ClearOther();
            }

            bool CheckRoom()
            {
                float min_x = 6003.837f;
                float max_x = 6200.81f;
                float min_y = -864.71f;
                float max_y = -717.52f;
                return !(
                    me->GetPositionX() < min_x ||
                    me->GetPositionX() > max_x ||
                    me->GetPositionY() < min_y ||
                    me->GetPositionY() > max_y
                    );
            }

            void DoAction(int32 const p_Action) override
            {
                _countInfernalChaosbringer++;
                if (_countInfernalChaosbringer == 3)
                {
                    if (instance->GetData(eData::DataPreEventGoroth) == DONE)
                    {
                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                        me->SetVisible(true);
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                if (!CheckRoom())
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eEvents::EventBurningArmor:
                        {
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                DoCast(target, eSpells::BurningArmor);

                            _countArmor++;

                            uint32 timerBurning = (IsLFR() ? (_countArmor == 3 ? 33 * IN_MILLISECONDS : _countArmor == 8 ? 29 * IN_MILLISECONDS : 24300) : IsMythic() ? 24500 : ((_countArmor > 3 && _countArmor % 2 == 0) ? 35 * IN_MILLISECONDS : 25 * IN_MILLISECONDS));
                            events.ScheduleEvent(eEvents::EventBurningArmor, timerBurning);
                            break;
                        }
                        case eEvents::EventCrashingComet:
                        {
                            DoCast(eSpells::CrashingComet, true);
                            _countComet++;

                            uint32 timerComet = (_countComet == 7 ? 20 * IN_MILLISECONDS : _countComet == 10 ? 25 * IN_MILLISECONDS : 18300);
                            events.ScheduleEvent(eEvents::EventCrashingComet, timerComet);
                            break;
                        }
                        case eEvents::EventInfernalSpikeSummon:
                        {
                            std::list<Player*> l_PlayerList;
                            me->GetPlayerListInGrid(l_PlayerList, 200.0f);

                            if (Unit* I_Victim = me->getVictim())
                                if (Player* I_Player = I_Victim->ToPlayer())
                                    l_PlayerList.remove(I_Player);

                            l_PlayerList.remove_if(CheckDeadTarget());
                            l_PlayerList.sort(JadeCore::DistanceCompareOrderPred(me, false)); ///< first check of top range players

                            if (l_PlayerList.size() > 3)
                                l_PlayerList.resize(3);

                            for (Player* player : l_PlayerList)
                                DoCast(player, eSpells::InfernalSpikeSummon);

                            _countSpike++;

                            uint32 timerInfernalSpike = (IsLFR() ? (_countSpike == 4 ? 26 * IN_MILLISECONDS : 16600) : (_countSpike == 4 ? 22 * IN_MILLISECONDS : _countSpike == 11 ? 19500 : 17 * IN_MILLISECONDS));
                            events.ScheduleEvent(eEvents::EventInfernalSpikeSummon, timerInfernalSpike);
                            break;
                        }
                        case eEvents::EventShatteringStar:
                        {
                            std::list<Player*> l_PlayerList;
                            me->GetPlayerListInGrid(l_PlayerList, 200.0f);

                            if (Unit* I_Victim = me->getVictim())
                                if (Player* I_Player = I_Victim->ToPlayer())
                                    l_PlayerList.remove(I_Player);

                            l_PlayerList.remove_if(JadeCore::UnitAuraCheck(true, eSpells::BurningArmor));
                            l_PlayerList.remove_if(CheckDeadTarget());

                            l_PlayerList.remove_if([](Player* p_Player) -> bool
                            {
                                if (!p_Player || p_Player->IsActiveSpecTankSpec())
                                    return true;

                                return false;
                            });

                            if (l_PlayerList.size() > 1)
                                JadeCore::RandomResizeList(l_PlayerList, 1);

                            for (Player* player : l_PlayerList)
                            {
                                Talk(urand(eTalks::TALK_SHATERINGSTAR, eTalks::TALK_SHATERINGSTAR3));
                                _shatteringStarTargetGUID = player->GetGUID();
                                DoCast(player, eSpells::ShatteringStar, true);
                            }

                            if (_countShatter > 4) /// reset count
                                _countShatter = 0;

                            uint32 timeShattering = (IsMythic() ? shatteringMythicTimers[_countShatter] : 60) * IN_MILLISECONDS;
                            events.ScheduleEvent(eEvents::EventShatteringStar, timeShattering);
                            _countShatter++;
                            break;
                        }
                        case eEvents::EventInfernalBurning:
                        {
                            Talk(eTalks::EMOTE_INFERNALBURNING);
                            DoCast(eSpells::InfernalBurning);
                            Talk(eTalks::TALK_INFERNALBURNING);
                            events.ScheduleEvent(eEvents::EventInfernalBurning, IsLFR() ? 64400 : 60500);
                            break;
                        }
                        case eEvents::EventRainofBrimstone:
                        {
                            DoCast(me, eSpells::RainofBrimstoneEffect);
                            std::list<Player*> l_PlayerList;
                            me->GetPlayerListInGrid(l_PlayerList, 200.0f);

                            if (Unit* I_Victim = me->getVictim())
                                if (Player* I_Player = I_Victim->ToPlayer())
                                    l_PlayerList.remove(I_Player);

                            l_PlayerList.remove_if(JadeCore::UnitAuraCheck(true, eMiscSpells::SpellSpiritOfRedemptionAura));
                            l_PlayerList.sort(JadeCore::DistanceCompareOrderPred(me, false)); ///< first check of top range players

                            if (l_PlayerList.size() > 3)
                                l_PlayerList.resize(3);

                            for (Player* player : l_PlayerList)
                                DoCast(player->GetPosition(), eSpells::RainofBrimstone);

                            _countRain++;
                            events.ScheduleEvent(eEvents::EventRainofBrimstone, _countRain == 5 ? 67200 : 60 * IN_MILLISECONDS);
                            break;
                        }
                        case eEvents::EventCheckDestroySpike:
                        {
                            std::list<Creature*> l_InfernalSpikes;
                            GetCreatureListWithEntryInGrid(l_InfernalSpikes, me, eMisc::NpcInfernalSpike, 6.0f);
                            if (!l_InfernalSpikes.empty())
                            {
                                for (Creature* infernalSpike : l_InfernalSpikes)
                                {
                                    if (!infernalSpike || !infernalSpike->HasAura(eSpells::InfernalAura))
                                        continue;

                                    if (me->GetExactDist(infernalSpike) > 6.f)
                                        continue;

                                    if (infernalSpike->AI())
                                        infernalSpike->AI()->DoAction(0);
                                    infernalSpike->SendPlaySpellVisual(eMisc::SpellvisualInfernalSpikeDestroy, infernalSpike, 0.f, 0.f, infernalSpike->GetPosition());
                                    infernalSpike->DespawnOrUnsummon(4000);
                                }
                            }
                            events.ScheduleEvent(eEvents::EventCheckDestroySpike, 1 * IN_MILLISECONDS);
                            break;
                        }
                        case eEvents::EventActionLavaStalker:
                        {
                             ActionLavaStalkerTactics();
                             events.ScheduleEvent(eEvents::EventActionLavaStalker, 50 * IN_MILLISECONDS);
                             break;
                        }
                        default:
                            break;
                    }
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                DoMeleeAttackIfReady();
            }

            uint64 GetGUID(int32 /*p_ID*/ = 0) override
            {
                return _shatteringStarTargetGUID;
            }

            void SetGUID(uint64 p_Guid, int32 p_ID = 0) override
            {
                _shatteringStarTargetGUID = p_Guid;
            }

        private:
            uint64 _shatteringStarTargetGUID;
            uint8 _countArmor;
            uint8 _countComet;
            uint8 _countShatter;
            uint8 _countSpike;
            uint8 _countRain;
            uint8 _tacticLavaStalker;
            uint8 _countInfernalChaosbringer;
            uint8 _countDataFelTurkey;
            uint8 _prevtacticLavaStalker;
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_gorothAI(p_Creature);
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Burning Armor - 231363
class spell_goroth_burning_armor : public SpellScriptLoader
{
    public:
        spell_goroth_burning_armor() : SpellScriptLoader("spell_goroth_burning_armor") { }

        class spell_goroth_burning_armor_Aurascript : public AuraScript
        {
            PrepareAuraScript(spell_goroth_burning_armor_Aurascript);

            void HandleRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* I_Target = GetTarget())
                {
                    I_Target->CastSpell(I_Target, eSpells::BurningEruption, true);
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_goroth_burning_armor_Aurascript::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_goroth_burning_armor_Aurascript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Crashing Comet - 232249
class spell_goroth_crashing_comet : public SpellScriptLoader
{
    public:
        spell_goroth_crashing_comet() : SpellScriptLoader("spell_goroth_crashing_comet") { }

        class spell_goroth_crashing_comet_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_goroth_crashing_comet_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Victim = l_Caster->getVictim())
                        p_Targets.remove(l_Victim);
                }

                p_Targets.remove_if(JadeCore::UnitAuraCheck(true, eSpells::BurningArmor));
                p_Targets.remove_if(CheckDeadTarget());

                p_Targets.remove_if([](WorldObject* p_Target) -> bool
                {
                    if (!p_Target->IsPlayer() || p_Target->ToPlayer()->IsActiveSpecTankSpec())
                        return true;

                    return false;
                });

                if (p_Targets.size() > 3)
                    JadeCore::RandomResizeList(p_Targets, 3);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_goroth_crashing_comet_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        class spell_goroth_crashing_comet_Aurascript : public AuraScript
        {
            PrepareAuraScript(spell_goroth_crashing_comet_Aurascript);

            void HandleRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* I_Caster = GetCaster())
                    if (Unit* I_Target = GetTarget())
                        I_Caster->CastSpell(I_Target, eSpells::CrashingCometTriggered, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_goroth_crashing_comet_Aurascript::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_goroth_crashing_comet_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_goroth_crashing_comet_Aurascript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Crashing Comet - 230345
class spell_goroth_crashing_comet_triggered : public SpellScriptLoader
{
public:
    spell_goroth_crashing_comet_triggered() : SpellScriptLoader("spell_goroth_crashing_comet_triggered") { }

    class spell_goroth_crashing_comet_triggered_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_goroth_crashing_comet_triggered_SpellScript);

        void HandleHit(SpellEffIndex /*effIndex*/)
        {
            Unit* I_Target = GetHitUnit();
            SpellInfo const* l_SpellInfo = GetSpellInfo();
            if (!I_Target || !l_SpellInfo)
                return;

            std::list<Creature*> l_InfernalSpikes;
            I_Target->GetCreatureListWithEntryInGrid(l_InfernalSpikes, eMisc::NpcInfernalSpike, l_SpellInfo->Effects[EFFECT_0].CalcRadius());

            for (Creature* infernalSpike : l_InfernalSpikes)
            {
                if (!infernalSpike->HasAura(eSpells::InfernalAura))
                     continue;

                if (infernalSpike->AI())
                    infernalSpike->AI()->DoAction(0);
                infernalSpike->SendPlaySpellVisual(eMisc::SpellvisualInfernalSpikeDestroy, infernalSpike, 0.f, 0.f, infernalSpike->GetPosition());
                infernalSpike->DespawnOrUnsummon(4000);
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_goroth_crashing_comet_triggered_SpellScript::HandleHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_goroth_crashing_comet_triggered_SpellScript();
    }
};

/// Last Update 7.3.2 Build 25549
/// Called by Infernal Buring - 233062
class spell_goroth_infernal_burning : public SpellScriptLoader
{
public:
    spell_goroth_infernal_burning() : SpellScriptLoader("spell_goroth_infernal_burning") { }

    class spell_goroth_infernal_burning_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_goroth_infernal_burning_SpellScript);

        void HandleBeforeCast()
        {
            Unit* I_Caster = GetCaster();
            if (!I_Caster)
                return;

            Creature* I_Creature = I_Caster->ToCreature();
            if (!I_Creature)
                return;

            std::list<Creature*> l_InfernalSpikes;
            I_Creature->GetCreatureListWithEntryInGrid(l_InfernalSpikes, eMisc::NpcInfernalSpike, 200.0f);
            std::list<Player*> l_PlayerList;
            I_Caster->GetPlayerListInGrid(l_PlayerList, 200.0f);

            for (Player* l_Player : l_PlayerList)
                for (Creature* I_Spike : l_InfernalSpikes)
                    if (I_Spike->IsInBetween(I_Caster, l_Player, 3.0f))
                        l_Player->AddAura(eSpells::InfernalSpikeProtection, l_Player);
        }

        void HandleAfterCast()
        {
            Unit* I_Caster = GetCaster();
            if (!I_Caster)
                return;

            I_Caster->SetPower(POWER_ENERGY, 0);
            I_Caster->RemoveAurasDueToSpell(eSpells::GorothEnergy);
            I_Caster->AddAura(eSpells::GorothEnergy, I_Caster);

            std::list<Player*> l_PlayerList;
            I_Caster->GetPlayerListInGrid(l_PlayerList, 200.0f);
            for (Player* l_Player : l_PlayerList)
                 l_Player->RemoveAurasDueToSpell(eSpells::InfernalSpikeProtection);
        }

        void Register() override
        {
            BeforeCast += SpellCastFn(spell_goroth_infernal_burning_SpellScript::HandleBeforeCast);
            AfterCast  += SpellCastFn(spell_goroth_infernal_burning_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_goroth_infernal_burning_SpellScript();
    }
};

/// Last Update 7.3.2 Build 25549
/// Called by Infernal Buring - 233078
class spell_goroth_infernal_burning_triggered : public SpellScriptLoader
{
public:
    spell_goroth_infernal_burning_triggered() : SpellScriptLoader("spell_goroth_infernal_burning_triggered") { }

    class spell_goroth_infernal_burning_triggered_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_goroth_infernal_burning_triggered_SpellScript);

        void HandleOnCast()
        {
            Unit* I_Caster = GetCaster();
            SpellInfo const* l_SpellInfo = GetSpellInfo();
            if (!I_Caster || !l_SpellInfo)
                return;

            std::list<Creature*> l_InfernalSpikes;
            I_Caster->GetCreatureListWithEntryInGrid(l_InfernalSpikes, eMisc::NpcInfernalSpike, l_SpellInfo->GetMaxRange());

            for (Creature* infernalSpike : l_InfernalSpikes)
            {
                if (!infernalSpike->HasAura(eSpells::InfernalAura))
                    continue;

                if (infernalSpike->AI())
                    infernalSpike->AI()->DoAction(0);
                infernalSpike->SendPlaySpellVisual(eMisc::SpellvisualInfernalSpikeDestroy, I_Caster, 0.f, 0.f, I_Caster->GetPosition());
                infernalSpike->DespawnOrUnsummon(4000);
            }
        }

        void Register() override
        {
            OnCast += SpellCastFn(spell_goroth_infernal_burning_triggered_SpellScript::HandleOnCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_goroth_infernal_burning_triggered_SpellScript();
    }
};

/// Last Update 7.3.2 Build 25549
/// Called by Shattering Star - 233279
class spell_at_goroth_shattering_star : public AreaTriggerEntityScript
{
    public:
        spell_at_goroth_shattering_star() : AreaTriggerEntityScript("spell_at_goroth_shattering_star")
        {
            m_Targets.clear();
            m_infernalSpike.clear();
        }

        std::set<uint64> m_Targets;
        std::set<uint64> m_infernalSpike;
        float l_Radius = 5.5f;
        void OnUpdate(AreaTrigger* p_At, uint32 /*p_Time*/) override
        {
            Unit* I_Caster = p_At->GetCaster();
            if (!I_Caster)
                return;

            std::list<Player*> l_PlayerList;
            JadeCore::AnyPlayerInObjectRangeCheck l_Check(p_At, l_Radius);
            JadeCore::PlayerListSearcher<JadeCore::AnyPlayerInObjectRangeCheck> l_Searcher(p_At, l_PlayerList, l_Check);
            p_At->VisitNearbyObject(l_Radius, l_Searcher);
            if (!l_PlayerList.empty())
            {
                l_PlayerList.remove_if(CheckDeadTarget());
                for (Player* l_Player : l_PlayerList)
                {
                    if (!l_Player || m_Targets.find(l_Player->GetGUID()) != m_Targets.end())
                        continue;

                    I_Caster->CastSpell(l_Player, eSpells::ShatteringStarDamage, true);
                    m_Targets.insert(l_Player->GetGUID());
                }
            }

            std::list<Creature*> l_InfernalSpikes;
            JadeCore::AllCreaturesOfEntryInRange checker(p_At, eMisc::NpcInfernalSpike, l_Radius);
            JadeCore::CreatureListSearcher<JadeCore::AllCreaturesOfEntryInRange> searcher(p_At, l_InfernalSpikes, checker);
            p_At->VisitNearbyObject(l_Radius, searcher);
            if (l_InfernalSpikes.empty())
                return;

            for (Creature* infernalSpike : l_InfernalSpikes)
            {
                if (!infernalSpike || m_infernalSpike.find(infernalSpike->GetGUID()) != m_infernalSpike.end())
                    continue;

                if (!infernalSpike->HasAura(eSpells::InfernalAura))
                    continue;

                m_infernalSpike.insert(infernalSpike->GetGUID());
                if (infernalSpike->AI())
                    infernalSpike->AI()->DoAction(0);
                infernalSpike->SendPlaySpellVisual(eMisc::SpellvisualInfernalSpikeDestroy, infernalSpike, 0.f, 0.f, infernalSpike->GetPosition());
                infernalSpike->DespawnOrUnsummon(4000);

                Creature* l_Creature = I_Caster->ToCreature();
                if (l_Creature && l_Creature->IsAIEnabled)
                    l_Creature->AI()->SetData(0, 0);
            }
        }

        void OnDestinationReached(AreaTrigger* p_At) override
        {
            Unit* I_Caster = p_At->GetCaster();
            if (!I_Caster)
                return;

            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::ShatteringStarLastDamage, I_Caster->GetMap()->GetDifficultyID());
            if (!l_SpellInfo)
                return;

            if (m_infernalSpike.empty())
            {
                Creature* l_Creature = I_Caster->ToCreature();
                if (!l_Creature || !l_Creature->IsAIEnabled)
                    return;

                Unit* l_Target = ObjectAccessor::GetUnit(*l_Creature, l_Creature->AI()->GetGUID(0));
                if (!l_Target)
                    return;

                if (I_Caster->IsValidAttackTarget(l_Target))
                    I_Caster->CastSpell(l_Target, eSpells::StarBurn, true);
            }

            uint32 countInfernalSpike = m_infernalSpike.size() + 1;
            I_Caster->CastCustomSpell(eSpells::ShatteringStarLastDamage, SPELLVALUE_BASE_POINT0, l_SpellInfo->Effects[EFFECT_0].BasePoints / countInfernalSpike, nullptr, true);
            p_At->Remove();
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_goroth_shattering_star();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Shattering Star - 233274
class spell_goroth_shattering_star : public SpellScriptLoader
{
public:
    spell_goroth_shattering_star() : SpellScriptLoader("spell_goroth_shattering_star") { }

    class spell_goroth_shattering_star_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_goroth_shattering_star_SpellScript);

        void HandleHit(SpellEffIndex /*effIndex*/)
        {
            if (Unit* I_Caster = GetCaster())
                if (Unit* I_Target = GetHitUnit())
                   if (Creature* I_Goroth = I_Target->ToCreature())
                       if (I_Goroth->AI())
                           I_Goroth->CastSpell(I_Caster, eSpells::ShatteringStarAT, true);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_goroth_shattering_star_SpellScript::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_goroth_shattering_star_SpellScript();
    }
};

/// Last Update 7.3.2 Build 25549
/// Called by Rain of Brimstone - 238588
class spell_goroth_rain_of_brimstone_triggered : public SpellScriptLoader
{
    public:
        spell_goroth_rain_of_brimstone_triggered() : SpellScriptLoader("spell_goroth_rain_of_brimstone_triggered") { }

        class spell_goroth_rain_of_brimstone_triggered_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_goroth_rain_of_brimstone_triggered_SpellScript);

            void CountTargets(std::list<WorldObject*>& p_Targets)
            {
                m_CountTargets = int32(p_Targets.size());
            }

            void HandleDamage(SpellEffIndex)
            {
                if (m_CountTargets)
                    SetHitDamage(GetHitDamage() / m_CountTargets);
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (m_CountTargets || !l_Caster || !GetExplTargetDest())
                    return;

                l_Caster->CastSpell(*GetExplTargetDest(), eSpells::RainofBrimstoneSummon, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_goroth_rain_of_brimstone_triggered_SpellScript::CountTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_goroth_rain_of_brimstone_triggered_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                AfterCast += SpellCastFn(spell_goroth_rain_of_brimstone_triggered_SpellScript::HandleAfterCast);
            }

        private:
            int32 m_CountTargets;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_goroth_rain_of_brimstone_triggered_SpellScript();
        }
};

/// NPC 116976 - Infernal Spike
class npc_goroth_infernal_spike : public CreatureScript
{
    public:
        npc_goroth_infernal_spike() : CreatureScript("npc_goroth_infernal_spike") { }

        struct npc_goroth_infernal_spikeAI : public ScriptedAI
        {
            npc_goroth_infernal_spikeAI(Creature* p_Creature) : ScriptedAI(p_Creature) {}

            enum eNpcEvents
            {
                EventCastInfernalSpike = 1,
                EventActivatePillar
            };

            uint64 m_GoInfernalSpikeGUID;
            EventMap m_Events;

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->AddUnitState(UNIT_STATE_ROOT);
                m_GoInfernalSpikeGUID = 0;
                m_Events.Reset();
                m_Events.ScheduleEvent(eNpcEvents::EventCastInfernalSpike, 500);
                m_Events.ScheduleEvent(eNpcEvents::EventActivatePillar, 4700);
            }

            void SummonedCreatureDespawn(Creature* /*summon*/) override
            {
                if (GameObject* l_GoInfernalSpike = GameObject::GetGameObject(*me, m_GoInfernalSpikeGUID))
                    l_GoInfernalSpike->Delete();
            }

            void DoAction(int32 const p_Action) override
            {
                if (IsMythic())
                    me->CastSpell(me, eSpells::InfernalDetonation, true);

                if (GameObject* l_GoInfernalSpike = GameObject::GetGameObject(*me, m_GoInfernalSpikeGUID))
                    l_GoInfernalSpike->Delete();
                me->RemoveAurasDueToSpell(eSpells::InfernalSpike);
                me->RemoveAurasDueToSpell(eSpells::InfernalAura);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                m_Events.Update(p_Diff);

                while (uint32 eventId = m_Events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eNpcEvents::EventCastInfernalSpike:
                        {
                            me->CastSpell(me, eSpells::InfernalSpike, false);
                            break;
                        }
                        case eNpcEvents::EventActivatePillar:
                        {
                            if (GameObject* l_GoInfernalSpike = me->SummonGameObject(eMisc::GoInfernalSpike, *me, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, nullptr, 100, 0, false, true))
                                m_GoInfernalSpikeGUID = l_GoInfernalSpike->GetGUID();
                            break;
                        }
                        default:
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new npc_goroth_infernal_spikeAI(p_Creature);
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Feel Pool - 234330
class spell_at_goroth_fel_pool : public AreaTriggerEntityScript
{
    public:
        spell_at_goroth_fel_pool() : AreaTriggerEntityScript("spell_at_goroth_fel_pool") { }

        std::list<uint64> m_Targets;
        uint32 m_CheckTimer = 500;

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (m_CheckTimer <= p_Time)
            {
                m_CheckTimer = 500;
                if (!p_AreaTrigger)
                     return;

                auto l_AreaTriggerCaster = p_AreaTrigger->GetCaster();
                if (!l_AreaTriggerCaster)
                    return;

                SpellInfo const* p_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::FelPoolAura);
                if (!p_SpellInfo)
                    return;

                std::list<Player*> l_PlayerList;
                float l_Radius = 5.0f; /// From sniff

                std::list<uint64> l_Targets(m_Targets);
                for (uint64 l_TargetGuid : l_Targets)
                {
                    Player* l_Player = ObjectAccessor::GetPlayer(*l_AreaTriggerCaster, l_TargetGuid);
                    if (!l_Player)
                        continue;

                    if (l_Player->HasAura(eSpells::FelPoolAura) && l_Player->GetDistance(p_AreaTrigger) <= l_Radius)
                        continue;

                    if (l_Player->HasAura(eSpells::FelPoolAura))
                        l_Player->RemoveAurasDueToSpell(eSpells::FelPoolAura);

                    m_Targets.remove(l_TargetGuid);
                }

                JadeCore::AnyPlayerInObjectRangeCheck l_Check(p_AreaTrigger, l_Radius);
                JadeCore::PlayerListSearcher<JadeCore::AnyPlayerInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_PlayerList, l_Check);
                p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);
                l_PlayerList.remove_if(CheckDeadTarget());

                if (!l_PlayerList.empty())
                {
                    for (Player* l_Player : l_PlayerList)
                    {
                        for (Player* l_Player : l_PlayerList)
                        {
                            if (!l_Player || l_Player->IsImmunedToSpell(p_SpellInfo) || l_Player->HasAura(eSpells::FelPoolAura))
                                continue;

                            l_Player->CastSpell(l_Player, eSpells::FelPoolAura, true);
                            m_Targets.push_back(l_Player->GetGUID());
                        }
                    }
                }

                std::list<Creature*> l_InfernalSpikes;
                JadeCore::AllCreaturesOfEntryInRange checker(p_AreaTrigger, eMisc::NpcInfernalSpike, l_Radius);
                JadeCore::CreatureListSearcher<JadeCore::AllCreaturesOfEntryInRange> searcher(p_AreaTrigger, l_InfernalSpikes, checker);
                p_AreaTrigger->VisitNearbyObject(l_Radius, searcher);
                if (l_InfernalSpikes.empty())
                    return;

                for (Creature* infernalSpike : l_InfernalSpikes)
                {
                    if (!infernalSpike->HasAura(eSpells::InfernalAura))
                        continue;

                    if (infernalSpike->AI())
                        infernalSpike->AI()->DoAction(0);
                    infernalSpike->SendPlaySpellVisual(eMisc::SpellvisualInfernalSpikeDestroy, infernalSpike, 0.f, 0.f, infernalSpike->GetPosition());
                    infernalSpike->DespawnOrUnsummon(4000);
                }
            }
            else
               m_CheckTimer -= p_Time;
        }

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
        {
            if (!p_AreaTrigger)
                 return;

            auto l_AreaTriggerCaster = p_AreaTrigger->GetCaster();
            if (!l_AreaTriggerCaster)
                return;

            std::list<uint64> l_Targets = m_Targets;
            for (uint64 l_TargetGuid : l_Targets)
            {
                Player* l_Player = ObjectAccessor::GetPlayer(*l_AreaTriggerCaster, l_TargetGuid);
                if (!l_Player)
                    continue;

                if (l_Player->HasAura(eSpells::FelPoolAura))
                    l_Player->RemoveAurasDueToSpell(eSpells::FelPoolAura);

                m_Targets.remove(l_TargetGuid);
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_goroth_fel_pool();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Fel Eruption 234368
class spell_goroth_fel_eruption_triggered : public SpellScriptLoader
{
    public:
        spell_goroth_fel_eruption_triggered() : SpellScriptLoader("spell_goroth_fel_eruption_triggered") { }

        class spell_goroth_fel_eruption_triggered_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_goroth_fel_eruption_triggered_SpellScript);

            void HandleHit(SpellEffIndex)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                std::list<Player*> l_PlayerList;
                l_Caster->GetPlayerListInGrid(l_PlayerList, 50.0f);
                if (l_PlayerList.empty())
                {
                    if (WorldLocation const* l_Loc = GetExplTargetDest())
                        l_Caster->CastSpell(l_Loc, eSpells::FelPoolAreaTrigger, true);

                    return;
                }

                if (WorldLocation const* l_Loc = GetExplTargetDest())
                    l_Caster->CastSpell(l_Loc, eSpells::FelPoolAreaTrigger, true);

                l_PlayerList.resize(1);
                for (Player* l_Player : l_PlayerList)
                {
                    std::list<AreaTrigger*> l_AreatriggerList;
                    l_Player->GetAreaTriggerListWithSpellIDInRange(l_AreatriggerList, eSpells::FelPoolAreaTrigger, 15.f);
                    if (l_AreatriggerList.empty())
                        l_Caster->CastSpell(l_Player->GetPosition(), eSpells::FelPoolAreaTrigger, true);
                }
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_goroth_fel_eruption_triggered_SpellScript::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_goroth_fel_eruption_triggered_SpellScript;
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Burning Eruption - 231395
class spell_goroth_burning_eruption : public SpellScriptLoader
{
    public:
        spell_goroth_burning_eruption() : SpellScriptLoader("spell_goroth_burning_eruption") { }

        class spell_goroth_burning_eruption_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_goroth_burning_eruption_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->GetMap() || l_Caster->GetMap()->IsMythic())
                    return;

                float l_Dist = 15.f;
                p_Targets.remove_if([this, l_Caster, l_Dist](WorldObject* p_Target) -> bool
                {
                    if (!p_Target || !p_Target->ToUnit())
                        return true;

                    if (l_Caster->GetDistance(p_Target) > l_Dist)
                        return true;

                    return false;
                });
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::MeltedArmor, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_goroth_burning_eruption_SpellScript::HandleAfterCast);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_goroth_burning_eruption_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_goroth_burning_eruption_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Melted Armor - 234264
class spell_goroth_melted_armor : public SpellScriptLoader
{
    public:
        spell_goroth_melted_armor() : SpellScriptLoader("spell_goroth_melted_armor") { }

        class spell_goroth_melted_armor_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_goroth_melted_armor_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->GetMap() || l_Caster->GetMap()->IsMythic())
                    return;

                float l_Dist = 15.f;
                p_Targets.remove_if([this, l_Caster, l_Dist](WorldObject* p_Target) -> bool
                {
                    if (!p_Target || !p_Target->ToUnit())
                        return true;

                    if (l_Caster->GetDistance(p_Target) > l_Dist)
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_goroth_melted_armor_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_goroth_melted_armor_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Goroth Energize - 237333
class spell_goroth_energize: public SpellScriptLoader
{
    public:
        spell_goroth_energize() : SpellScriptLoader("spell_goroth_energize") { }

        class spell_goroth_energize_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_goroth_energize_AuraScript);

            float l_Energy;

            bool Load()
            {
                l_Energy = 0.f;
                return true;
            }

            void HandleOnPeriodic(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || l_Caster->getPowerType() != POWER_ENERGY)
                    return;

                l_Energy += 2.f;
                l_Caster->SetPower(POWER_ENERGY, int32(l_Energy));
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_goroth_energize_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_goroth_energize_AuraScript();
        }
};

/// NPC 119950 - Brimstone Infernal
class npc_goroth_brimstone_infernal : public CreatureScript
{
    public:
        npc_goroth_brimstone_infernal() : CreatureScript("npc_goroth_brimstone_infernal") { }

        struct npc_goroth_brimstone_infernalAI : public ScriptedAI
        {
            npc_goroth_brimstone_infernalAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
            }

            enum eNpcEvents
            {
                EventCastFelFire = 1
            };

            EventMap m_Events;

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoZoneInCombat(me, 250.0f);
                me->AddUnitState(UNIT_STATE_ROOT);
                me->AttackStop();
                me->StopMoving();
                DoCast(eSpells::FelFireAura);
                m_Events.Reset();
                m_Events.ScheduleEvent(eNpcEvents::EventCastFelFire, 8 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (!me->HasAura(eSpells::FelFireAura) && me->HasUnitState(UNIT_STATE_ROOT))
                    me->ClearUnitState(UNIT_STATE_ROOT);

                while (uint32 eventId = m_Events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eNpcEvents::EventCastFelFire:
                        {
                            me->AddUnitState(UNIT_STATE_ROOT);
                            me->AttackStop();
                            me->StopMoving();
                            DoCast(me, eSpells::FelFireAura);
                            m_Events.ScheduleEvent(eNpcEvents::EventCastFelFire, 8 * IN_MILLISECONDS);
                            break;
                        }
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new npc_goroth_brimstone_infernalAI(p_Creature);
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_goroth()
{
    new boss_goroth();
    new spell_goroth_burning_armor();
    new spell_goroth_crashing_comet();
    new spell_goroth_crashing_comet_triggered();
    new spell_goroth_infernal_burning();
    new spell_goroth_infernal_burning_triggered();
    new spell_at_goroth_shattering_star();
    new spell_goroth_shattering_star();
    new spell_goroth_rain_of_brimstone_triggered();
    new npc_goroth_infernal_spike();
    new spell_at_goroth_fel_pool();
    new spell_goroth_fel_eruption_triggered();
    new spell_goroth_burning_eruption();
    new spell_goroth_melted_armor();
    new spell_goroth_energize();
    new npc_goroth_brimstone_infernal();
}
#endif
