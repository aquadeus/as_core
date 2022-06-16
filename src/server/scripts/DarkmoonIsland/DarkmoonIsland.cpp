////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedEscortAI.h"
#include "SpellScript.h"
#include "ScriptPCH.h"
#include "DarkmoonIsland.h"
#include "AreaTrigger.h"
#include "SpellAuras.h"
#include "SpellMgr.h"
#include "Containers.h"

/// NPC 55402 - Korgol
class npc_darkmoon_faire_korgol : public CreatureScript
{
public:
    npc_darkmoon_faire_korgol() : CreatureScript("npc_darkmoon_faire_korgol") { }


    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_darkmoon_faire_korgolAI(p_Creature);
    }

    struct npc_darkmoon_faire_korgolAI : public ScriptedAI
    {
        npc_darkmoon_faire_korgolAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        void DoAction(int32 const p_Action) override
        {
            switch (p_Action)
            {
                case Actions::SpawnDeathmatchChest:
                {
                    std::list<GameObject*> l_GameObjectList;
                    GetGameObjectListWithEntryInGrid(l_GameObjectList, me, eGameObject::DarkmoonChest, 500.0f);
                    for (std::list<GameObject*>::iterator iter = l_GameObjectList.begin(); iter != l_GameObjectList.end(); ++iter)
                    {
                        (*iter)->Respawn();
                        (*iter)->SetPhaseMask(1, true);
                    }
                }
                default:
                    break;
            }
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            if (!UpdateVictim())
                return;
        }
    };
};

/// NPC 3500035 - Controler Chest (Spawn Chest via event spawn)
class npc_darkmoon_controler_chest : public CreatureScript
{
public:
    npc_darkmoon_controler_chest() : CreatureScript("npc_darkmoon_controler_chest")
    {

    }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_darkmoon_controler_chestAI(creature);
    }

    struct npc_darkmoon_controler_chestAI : public ScriptedAI
    {
        npc_darkmoon_controler_chestAI(Creature *creature) : ScriptedAI(creature)
        {
        }

        void Reset() override
        {
            if(Creature* l_Korgol = me->FindNearestCreature(eCreature::Korgol, 500.0f))
                l_Korgol->GetAI()->DoAction(Actions::SpawnDeathmatchChest);
        }

        void UpdateAI(const uint32 diff) override
        {
            if (!UpdateVictim())
                return;
        }
    };
};

/// NPC 3500036 - Controler Launh Concert
class npc_darkmoon_controller_timer_L70ETC : public CreatureScript
{
public:
    npc_darkmoon_controller_timer_L70ETC() : CreatureScript("npc_darkmoon_controller_timer_L70ETC")
    {

    }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_darkmoon_controler_chestAI(creature);
    }

    struct npc_darkmoon_controler_chestAI : public ScriptedAI
    {
        npc_darkmoon_controler_chestAI(Creature *creature) : ScriptedAI(creature)
        {
        }

        void Reset() override
        {
            if(Creature* l_ControllerConcert = me->FindNearestCreature(eCreature::ControllerConcert, 500.0f))
                l_ControllerConcert->GetAI()->DoAction(Actions::Warn30MinutesLeft);
        }

        void UpdateAI(const uint32 diff) override
        {
            if (!UpdateVictim())
                return;
        }
    };
};


/// NPC 23830 - Controler Concert
class npc_darkmoon_faire_chieftains_controller : public CreatureScript
{
public:
    npc_darkmoon_faire_chieftains_controller() : CreatureScript("npc_darkmoon_faire_chieftains_controller") { }

    struct npc_darkmoon_faire_chieftains_controllerAI : public ScriptedAI
    {
        npc_darkmoon_faire_chieftains_controllerAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        EventMap m_Events;

        void Reset () override
        {
            m_Events.Reset();
        }

        void DoAction(const int32 action) override
        {
            switch(action)
            {
                case Actions::Warn30MinutesLeft:
                {
                    if(Creature* l_Monitor = me->FindNearestCreature(eCreature::MichaelSchweitzer, 500.0f))
                        l_Monitor->AI()->ZoneTalk(eTalk::MonitorWarn30);
                    m_Events.ScheduleEvent(Events::Warn15MinutesLeft, 900 * IN_MILLISECONDS);
                    break;
                }
            }
        }

        void sGossipHello(Player* p_Player) override
        {
            m_Events.ScheduleEvent(Events::ConcertChieftainsBegin, 5000);
        }

        void UpdateAI(uint32 diff) override
        {
            m_Events.Update(diff);

            while (uint32 eventId = m_Events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case Events::Warn15MinutesLeft:
                    {
                        if(Creature* l_Monitor = me->FindNearestCreature(eCreature::MichaelSchweitzer, 500.0f))
                            l_Monitor->AI()->ZoneTalk(eTalk::MonitorWarn15);
                        m_Events.ScheduleEvent(Events::Warn5MinutesLeft, 600 * IN_MILLISECONDS);
                        break;
                    }
                    case Events::Warn5MinutesLeft:
                    {
                        if(Creature* l_Monitor = me->FindNearestCreature(eCreature::MichaelSchweitzer, 500.0f))
                            l_Monitor->AI()->ZoneTalk(eTalk::MonitorWarn5);
                        m_Events.ScheduleEvent(Events::ConcertChieftainsBegin, 300 * IN_MILLISECONDS);
                        break;
                    }
                    case Events::ConcertChieftainsBegin:
                    {
                        if(Creature* l_Monitor = me->FindNearestCreature(eCreature::MichaelSchweitzer, 500.0f))
                            l_Monitor->AI()->ZoneTalk(eTalk::MonitorBegin);
                        if(Creature* l_SamuroCtrl = me->FindNearestCreature(eCreature::SamuroCtrlr, 500.0f))
                            l_SamuroCtrl->CastSpell(l_SamuroCtrl, eSpellDarkmoonFaire::SummonEffect, true);
                        m_Events.ScheduleEvent(Events::SummonSamuro, 1000);
                        m_Events.ScheduleEvent(Events::EndOfConcert, 180 * IN_MILLISECONDS);
                        break;
                    }
                    case Events::SummonSamuro:
                    {
                        me->PlayDirectMusic(eMusic::PowerOfTheHorde);
                        if(Creature* l_Samuro = me->FindNearestCreature(eCreature::Samuro, 500.0f))
                            l_Samuro->SetPhaseMask(1, true);
                        if(Creature* l_MayKilCtrl = me->FindNearestCreature(eCreature::MaiKylCtrlr, 500.0f))
                        {
                            l_MayKilCtrl->CastSpell(l_MayKilCtrl, eSpellDarkmoonFaire::SummonEffect, true);
                            std::list<Creature*> l_HordeAndAllianceCitizen;
                            me->GetCreatureListWithEntryInGrid(l_HordeAndAllianceCitizen, eCreature::AllianceCitizen, 40.0f);
                            me->GetCreatureListWithEntryInGrid(l_HordeAndAllianceCitizen, eCreature::HordeCitizen, 40.0f);

                            if(!l_HordeAndAllianceCitizen.empty())
                            {
                                for (auto citizens : l_HordeAndAllianceCitizen)
                                {
                                    citizens->SetFacingToObject(l_MayKilCtrl);
                                    citizens->HandleEmoteCommand(EMOTE_STATE_DANCE);
                                }
                            }
                        }
                        DoCast(eSpellDarkmoonFaire::LightningCloud);
                        m_Events.ScheduleEvent(Events::SummonMayKil, 2000);
                        break;
                    }
                    case Events::SummonMayKil:
                    {
                        if(Creature* l_MaiKyl = me->FindNearestCreature(eCreature::MaiKyl, 500.0f))
                            l_MaiKyl->SetPhaseMask(1, true);

                        if(Creature* l_SigCtrl = me->FindNearestCreature(eCreature::SigNiciousCtrlr, 500.0f))
                            l_SigCtrl->CastSpell(l_SigCtrl, eSpellDarkmoonFaire::SummonEffect, true);

                        DoCast(eSpellDarkmoonFaire::EarthQuakeVisual);
                        m_Events.ScheduleEvent(Events::SummonSig, 2000);
                        break;
                    }
                    case Events::SummonSig:
                    {
                        if(Creature* l_Sig = me->FindNearestCreature(eCreature::SigNicious, 500.0f))
                            l_Sig->SetPhaseMask(1, true);

                        if(Creature* l_BergrCtrl = me->FindNearestCreature(eCreature::BergrisstCtrlr, 500.0f))
                            l_BergrCtrl->CastSpell(l_BergrCtrl, eSpellDarkmoonFaire::SummonEffect, true);

                        m_Events.ScheduleEvent(Events::SummonBergrisst, 2000);
                        break;
                    }
                    case Events::SummonBergrisst:
                    {
                        if(Creature* l_Bergrisst = me->FindNearestCreature(eCreature::Bergrisst, 500.0f))
                            l_Bergrisst->SetPhaseMask(1, true);

                        if(Creature* l_ThunderChief = me->FindNearestCreature(eCreature::ChiefThunderSkinsCtrlr, 500.0f))
                            l_ThunderChief->CastSpell(l_ThunderChief, eSpellDarkmoonFaire::SummonEffect, true);

                        DoCast(eSpellDarkmoonFaire::RainOfFire);
                        m_Events.ScheduleEvent(Events::SummonThunderChief, 2000);
                        break;
                    }
                    case Events::SummonThunderChief:
                    {
                        if(Creature* l_ThunderChief = me->FindNearestCreature(eCreature::ChiefThunderSkins, 500.0f))
                            l_ThunderChief->SetPhaseMask(1, true);

                        m_Events.ScheduleEvent(Events::SamCustom2, 17000);
                        break;
                    }
                    case Events::SamCustom2:
                    {
                        if(Creature* l_Samuro = me->FindNearestCreature(eCreature::Samuro, 500.0f))
                            l_Samuro->HandleEmoteCommand(EMOTE_ONESHOT_CUSTOM_SPELL_02);

                        m_Events.ScheduleEvent(Events::BergrCustom1, 5000);
                        break;
                    }
                    case Events::BergrCustom1:
                    {
                        if(Creature* l_Bergrisst = me->FindNearestCreature(eCreature::Bergrisst, 500.0f))
                            l_Bergrisst->HandleEmoteCommand(EMOTE_ONESHOT_CUSTOM_SPELL_01);
                        DoCast(eSpellDarkmoonFaire::EarthQuakeVisual);
                        DoCast(eSpellDarkmoonFaire::LightningCloud);
                        DoCast(eSpellDarkmoonFaire::RainOfFire);
                        m_Events.ScheduleEvent(Events::SigCustom2, 3000);
                        m_Events.ScheduleEvent(Events::MayKylCustom2, 2000);
                        break;
                    }
                    case Events::SigCustom2:
                    {
                        if(Creature* l_Sig = me->FindNearestCreature(eCreature::SigNicious, 500.0f))
                            l_Sig->HandleEmoteCommand(EMOTE_ONESHOT_CUSTOM_SPELL_02);
                        break;
                    }
                    case Events::MayKylCustom2:
                    {
                        if(Creature* l_MaiKyl = me->FindNearestCreature(eCreature::MaiKyl, 500.0f))
                            l_MaiKyl->HandleEmoteCommand(EMOTE_ONESHOT_CUSTOM_SPELL_02);
                        m_Events.ScheduleEvent(Events::SamuroCustom3, 15000);
                        break;
                    }
                    case Events::SamuroCustom3:
                    {
                        if(Creature* l_Samuro = me->FindNearestCreature(eCreature::Samuro, 500.0f))
                            l_Samuro->HandleEmoteCommand(EMOTE_ONESHOT_CUSTOM_SPELL_03);
                        m_Events.ScheduleEvent(Events::MaykylCustom3, 3000);
                        break;
                    }
                    case Events::MaykylCustom3:
                    {
                        if(Creature* l_MaiKyl = me->FindNearestCreature(eCreature::MaiKyl, 500.0f))
                            l_MaiKyl->HandleEmoteCommand(EMOTE_ONESHOT_CUSTOM_SPELL_03);

                        m_Events.ScheduleEvent(Events::ChiefCustom3, 4000);
                        break;
                    }
                    case Events::ChiefCustom3:
                    {
                        if(Creature* l_ThunderChief = me->FindNearestCreature(eCreature::ChiefThunderSkins, 500.0f))
                            l_ThunderChief->HandleEmoteCommand(EMOTE_ONESHOT_CUSTOM_SPELL_03);
                        DoCast(eSpellDarkmoonFaire::EarthQuakeVisual);
                        DoCast(eSpellDarkmoonFaire::LightningCloud);
                        DoCast(eSpellDarkmoonFaire::RainOfFire);
                        m_Events.ScheduleEvent(Events::SamCustom2, 4000);
                        break;
                    }
                    case Events::EndOfConcert:
                    {
                        m_Events.Reset();
                        std::list<Creature*> l_CreaturesList;
                        me->GetCreatureListWithEntryInGrid(l_CreaturesList, eCreature::MaiKyl, 500.0f);
                        me->GetCreatureListWithEntryInGrid(l_CreaturesList, eCreature::SigNicious, 500.0f);
                        me->GetCreatureListWithEntryInGrid(l_CreaturesList, eCreature::Samuro, 500.0f);
                        me->GetCreatureListWithEntryInGrid(l_CreaturesList, eCreature::Bergrisst, 500.0f);
                        me->GetCreatureListWithEntryInGrid(l_CreaturesList, eCreature::ChiefThunderSkins, 500.0f);
                        if(!l_CreaturesList.empty())
                        {
                            for (auto concertNpc : l_CreaturesList)
                            {
                                concertNpc->SetPhaseMask(2, true);
                                concertNpc->HandleEmoteCommand(EMOTE_ONESHOT_NONE);
                            }
                        }
                        std::list<Creature*> l_HordeAndAllianceCitizenEnd;
                        me->GetCreatureListWithEntryInGrid(l_HordeAndAllianceCitizenEnd, eCreature::AllianceCitizen, 40.0f);
                        me->GetCreatureListWithEntryInGrid(l_HordeAndAllianceCitizenEnd, eCreature::HordeCitizen, 40.0f);
                        if(!l_HordeAndAllianceCitizenEnd.empty())
                            for (auto citizens : l_HordeAndAllianceCitizenEnd)
                                citizens->DespawnOrUnsummon();
                        break;
                    }
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_darkmoon_faire_chieftains_controllerAI(p_Creature);
    }
};

/// NPC 3500023 - Shot mini game controller
class npc_darkmoon_faire_shot_controller : public CreatureScript
{
public:
    npc_darkmoon_faire_shot_controller() : CreatureScript("npc_darkmoon_faire_shot_controller") { }

    struct npc_darkmoon_faire_shot_controllerAI : public ScriptedAI
    {
        npc_darkmoon_faire_shot_controllerAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        EventMap m_Events;
        std::list<Creature*> l_TriggerList;

        void Reset () override
        {
            m_Events.ScheduleEvent(Events::LaunchTargetIndicator, 7000);
        }

        void GetTrigger()
        {
            l_TriggerList.clear();
            me->GetCreatureListWithEntryInGrid(l_TriggerList, eCreature::BunnyTarget1, 30.0f);
            JadeCore::RandomResizeList(l_TriggerList, 1);
        }

        void DoAction(int32 const p_Action) override
        {
            switch (p_Action)
            {
                case Actions::LaunchAnotherTarget:
                {
                    m_Events.Reset();
                    m_Events.ScheduleEvent(Events::LaunchTargetIndicator, 90);
                    break;
                }
                default:
                    break;
            }
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            m_Events.Update(p_Diff);

            switch (m_Events.ExecuteEvent())
            {
                case Events::LaunchTargetIndicator:
                {
                    if(!l_TriggerList.empty())
                        l_TriggerList.front()->RemoveAurasDueToSpell(eSpellDarkmoonFaire::TargetIndicator);
                    GetTrigger();

                    if(!l_TriggerList.empty())
                    {
                        l_TriggerList.front()->CastSpell(l_TriggerList.front(), eSpellDarkmoonFaire::TargetIndicator, true);
                        l_TriggerList.front()->AI()->DoAction(LaunchQuickshotAchievement);
                    }

                    m_Events.ScheduleEvent(Events::LaunchTargetIndicator, 7000);
                    break;
                }
                default:
                    break;
            }
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_darkmoon_faire_shot_controllerAI(p_Creature);
    }
};

/// NPC 24171 - Shot mini game target
class npc_darkmoon_faire_shot_target : public CreatureScript
{
public:
    npc_darkmoon_faire_shot_target() : CreatureScript("npc_darkmoon_faire_shot_target") { }

    struct npc_darkmoon_faire_shot_targetAI : public ScriptedAI
    {
        npc_darkmoon_faire_shot_targetAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        EventMap m_Events;
        bool m_canGiveAchievement = false;

        void DoAction(int32 const p_Action) override
        {
            switch (p_Action)
            {
                case Actions::LaunchQuickshotAchievement:
                {
                    m_canGiveAchievement = true;
                    m_Events.ScheduleEvent(Events::AbortQuickshotAchievement, 1500);
                    break;
                }
                default:
                    break;
            }
        }

        void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
        {
            Player* l_Player = p_Caster->ToPlayer();
            if (!l_Player)
                return;

            switch (p_Spell->Id)
            {
                case eSpellDarkmoonFaire::ShotOverride:
                {
                    if(m_canGiveAchievement)
                        if(!l_Player->HasAchieved(eAchievement::QuickShot))
                           if (AchievementEntry const* l_AchievementEntry = sAchievementStore.LookupEntry(eAchievement::QuickShot))
                                l_Player->CompletedAchievement(l_AchievementEntry);

                    if(me->HasAura(eSpellDarkmoonFaire::TargetIndicator))
                    {
                        if(Creature* l_ShotController = me->FindNearestCreature(eCreature::BunnyControllerShot, 30.0f, true))
                            l_ShotController->AI()->DoAction(Actions::LaunchAnotherTarget);

                        l_Player->CastSpell(l_Player, eSpellDarkmoonFaire::ShotCredit, true);
                    }

                    break;
                }
                default:
                    break;
            }

        }

        void UpdateAI(uint32 const p_Diff) override
        {
            m_Events.Update(p_Diff);

            switch (m_Events.ExecuteEvent())
            {
                case Events::AbortQuickshotAchievement:
                {
                    m_canGiveAchievement = false;
                    break;
                }
                default:
                    break;
            }
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_darkmoon_faire_shot_targetAI(p_Creature);
    }
};

class npc_darkmoon_moonfang_controller : public CreatureScript
{
public:
    npc_darkmoon_moonfang_controller() : CreatureScript("npc_darkmoon_moonfang_controller")
    {

    }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_darkmoon_moonfang_controllerAI(creature);
    }

    struct npc_darkmoon_moonfang_controllerAI : public ScriptedAI
    {
        npc_darkmoon_moonfang_controllerAI(Creature *creature) : ScriptedAI(creature)
        {
        }
        uint32 m_WolvesSummonned = 0;

        void Reset() override
        {
            SummonMoonfangWolves();
            m_WolvesSummonned = 0;
        }

        void SummonMoonfangWolves()
        {
            std::list<Creature*> l_MoonfangSnarlers;
            std::list<Creature*> l_MoonfangSnarlersToSummon;
            me->GetCreatureListWithEntryInGrid(l_MoonfangSnarlers, eCreature::MoonfangSnarler, 500.0f);

            for (auto moonfang : l_MoonfangSnarlers)
            {
                if(moonfang->isDead())
                    continue;
                if(moonfang->GetPhaseMask() == 1)
                    continue;
                l_MoonfangSnarlersToSummon.push_back(moonfang);
            }

            if(l_MoonfangSnarlersToSummon.empty())
                return;
            JadeCore::RandomResizeList(l_MoonfangSnarlersToSummon, 1);
            l_MoonfangSnarlersToSummon.front()->SetPhaseMask(1, true);
        }

        void DoAction(int32 const p_Action) override
        {
            switch (p_Action)
            {
                case Actions::SummonNewWolf:
                {
                    m_WolvesSummonned >= 13 ? m_WolvesSummonned = 0 : m_WolvesSummonned++;

                    if(m_WolvesSummonned < 11)
                        SummonMoonfangWolves();
                    else if(m_WolvesSummonned == 11)
                    {
                        if(Creature* l_MoonfangDeadhowl = me->FindNearestCreature(eCreature::MoonfangDreadhowl, 500.0f, true))
                        {
                            l_MoonfangDeadhowl->SetPhaseMask(1, true);
                            l_MoonfangDeadhowl->AI()->DoAction(DreadhowlMoonfangSpawn);
                        }
                    }
                    else if (m_WolvesSummonned == 12)
                    {
                        if (Creature* l_MoonfangMother= me->FindNearestCreature(eCreature::MoonfangMother, 500.0f, true))
                        {
                            l_MoonfangMother->SetPhaseMask(1, true);
                            l_MoonfangMother->AI()->ZoneTalk(1);
                            me->DespawnOrUnsummon();
                        }
                    }
                    break;
                }
                default:
                    break;
            }
        }

        void UpdateAI(const uint32 diff) override
        {
            if (!UpdateVictim())
                return;
        }
    };
};


class npc_darkmoon_moonfang_wolves : public CreatureScript
{
public:
    npc_darkmoon_moonfang_wolves() : CreatureScript("npc_darkmoon_moonfang_wolves") { }

    struct npc_darkmoon_moonfang_wolvesAI : public ScriptedAI
    {
        npc_darkmoon_moonfang_wolvesAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        EventMap m_Events;

        void Reset () override
        {
            m_Events.Reset();
        }

        void EnterCombat(Unit* /**/) override
        {
            events.ScheduleEvent(Events::HorrificHowlEvent, 20 * IN_MILLISECONDS);
            events.ScheduleEvent(Events::CripplingBiteEvent, urand(3, 5) * IN_MILLISECONDS);
            events.ScheduleEvent(Events::LeapForTheKillEvent, urand(10, 14) * IN_MILLISECONDS);
        }

        void DoAction(int32 const p_Action) override
        {
            switch (p_Action)
            {
                case Actions::DreadhowlMoonfangSpawn:
                {
                    ZoneTalk(2);
                    break;
                }
                default:
                    break;
            }
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            if (!UpdateVictim())
                return;
            m_Events.Update(p_Diff);

            switch (m_Events.ExecuteEvent())
            {
                case Events::HorrificHowlEvent:
                {
                    DoCast(me, eSpellDarkmoonFaire::DreadfulHowl);
                    m_Events.ScheduleEvent(Events::HorrificHowlEvent, 20 * IN_MILLISECONDS);
                    break;
                }
                case Events::CripplingBiteEvent:
                {
                    if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.f, true))
                            DoCast(l_Target, eSpellDarkmoonFaire::CripplingBite);
                    events.ScheduleEvent(Events::CripplingBiteEvent, urand(3, 5) * IN_MILLISECONDS);
                    break;
                }
                case Events::LeapForTheKillEvent:
                {
                    if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.f, true))
                            DoCast(l_Target, eSpellDarkmoonFaire::LeapForTheKill);
                    events.ScheduleEvent(Events::LeapForTheKillEvent, urand(10, 14) * IN_MILLISECONDS);
                    break;
                }
                default:
                    break;
            }
            DoMeleeAttackIfReady();
        }

        void JustDied(Unit* /**/) override
        {
            m_Events.Reset();
            Talk(1);
            if(Creature* l_Creature = me->FindNearestCreature(eCreature::BrendonPaulson, 500.0f, true))
                l_Creature->AI()->DoAction(Actions::SummonNewWolf);
        }

    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_darkmoon_moonfang_wolvesAI(p_Creature);
    }
};


/// Darkmoon Adventurer & Darkmoon Trophy
class playerscript_darkmoon_adventurer_guide: public PlayerScript
{
    public:
        playerscript_darkmoon_adventurer_guide() : PlayerScript("playerscript_darkmoon_adventurer_guide") {}

        void OnPVPKill(Player* p_Killer, Player* p_Killed) override
        {
            if (p_Killer->GetMap()->IsBattleground())
                if (roll_chance_i(1) && p_Killer->HasItemCount(eItems::DarkmoonFairGuide) && p_Killer->HasAura(eSpellDarkmoonFaire::DarkmoonAdventurerAura) && !p_Killer->HasAchieved(eAchievement::DarkmoonDefender))
                    p_Killer->AddItem(RewardPVPItemID[urand(0, 2)], 1);
        }

        void OnKill(Player* p_Killer, Unit* p_Killed, bool p_BeforeAurasRemoved = false) override
        {
            if (p_Killer->GetMap()->IsDungeon() || p_Killer->GetMap()->IsRaid())
                if (p_Killed->getLevelForTarget(p_Killer) >= p_Killer->getLevel())
                    if (roll_chance_i(1) && p_Killer->HasItemCount(eItems::DarkmoonFairGuide) && p_Killer->HasAura(eSpellDarkmoonFaire::DarkmoonAdventurerAura) && !p_Killer->HasAchieved(eAchievement::DarkmoonDungeoneer))
                        p_Killer->AddItem(RewardPVEItemID[urand(0, 4)], 1);

            if (p_Killed->getLevelForTarget(p_Killer) >= p_Killer->getLevel() && p_Killer->GetQuestStatus(eQuests::TestYourStrengh) == QUEST_STATUS_INCOMPLETE)
                    if (roll_chance_i(10) && p_Killer->HasAura(eSpellDarkmoonFaire::DarkmoonAdventurerAura))
                        p_Killer->AddItem(eItems::GrislyTrophy, urand(1, 2));

            if (p_Killer->HasAchieved(eAchievement::DarkmoonDungeoneer) && p_Killer->HasAchieved(eAchievement::DarkmoonDefender) && !p_Killer->HasAchieved(eAchievement::DarkmoonDespoiler) && p_Killer->HasAura(eSpellDarkmoonFaire::DarkmoonAdventurerAura))
                if (p_Killed->getLevelForTarget(p_Killer) >= p_Killer->getLevel())
                    if (roll_chance_i(10))
                        p_Killer->AddItem(eItems::SoothsayersRunes, 1);
        }
};

/// Darkmoon Race Phase
class PlayerScript_darkmoon_race_quest : public PlayerScript
{
    public:
        PlayerScript_darkmoon_race_quest() :PlayerScript("PlayerScript_darkmoon_race_quest") {}

        void OnQuestAccept(Player* p_Player, Quest const* p_Quest) override
        {
            switch (p_Quest->GetQuestId())
            {
                case eQuests::ShortRaceQuestOne:
                case eQuests::ShortRaceQuestTwo:
                case eQuests::ShortRaceQuestThree:
                case eQuests::LongRaceQuestOne:
                case eQuests::LongRaceQuestTwo:
                {
                    if(!p_Player->HasAura(eSpellDarkmoonFaire::Wanderluster) && !p_Player->HasAura(eSpellDarkmoonFaire::Powermonger) && !p_Player->HasAura(eSpellDarkmoonFaire::Rocketeer) && !p_Player->HasAura(eSpellDarkmoonFaire::RacingStrider))
                        p_Player->CastSpell(p_Player, eSpellDarkmoonFaire::RacingStrider, true);

                    p_Player->SetInPhase(74536, true, true);
                    break;
                }
                default:
                    break;
            }
        }

        void OnQuestAbandon(Player* p_Player, const Quest* p_Quest) override
        {
            switch (p_Quest->GetQuestId())
            {
                case eQuests::ShortRaceQuestOne:
                case eQuests::ShortRaceQuestTwo:
                case eQuests::ShortRaceQuestThree:
                case eQuests::LongRaceQuestOne:
                case eQuests::LongRaceQuestTwo:
                {
                    p_Player->SetInPhase(74536, true, false);
                    p_Player->SetInPhase(74372, true, false);
                    p_Player->SetInPhase(74365, true, false);
                    p_Player->SetInPhase(74368, true, false);
                    p_Player->SetInPhase(74366, true, false);
                    p_Player->SetInPhase(74369, true, false);
                    p_Player->CastSpell(p_Player, eSpellDarkmoonFaire::RaceCancelled, true);
                    std::list<Creature*> l_TriggerList;
                    p_Player->GetCreatureListWithEntryInGrid(l_TriggerList, eCreature::BigRaceBunny, 500.0f);
                    p_Player->GetCreatureListWithEntryInGrid(l_TriggerList, eCreature::ShortRaceBunny, 500.0f);

                    for (auto trigger : l_TriggerList)
                    {
                        if(Unit* l_Owner = trigger->ToTempSummon()->GetSummoner())
                            if(l_Owner->GetGUID() == p_Player->GetGUID())
                                trigger->DespawnOrUnsummon();
                    }
                    break;
                }
                default:
                    break;
            }
        }

        void OnLogout(Player* p_Player) override
        {
            for (uint8 i = 0; i < 5; ++i)
                if(p_Player->GetQuestStatus(RaceQuestId[i]) == QUEST_STATUS_INCOMPLETE)
                    p_Player->FailQuest(RaceQuestId[i]);
        }
};

class PlayerScript_darkmoon_item_quest : public PlayerScript
{
    public:
        PlayerScript_darkmoon_item_quest() : PlayerScript("PlayerScript_darkmoon_item_quest") { }

        void OnQuestAccept(Player* p_Player, Quest const* p_Quest) override
        {
            switch (p_Quest->GetQuestId())
            {
                case eQuests::ACapturedBanner:
                {
                    if (p_Player->HasItemCount(eItems::BannerOfTheFallen, 2))
                        p_Player->DestroyItemCount(eItems::BannerOfTheFallen, 1, false);
                    break;
                }
                case eQuests::ACuriousCrystal:
                {
                    if (p_Player->HasItemCount(eItems::ImbuedCrystal, 2))
                        p_Player->DestroyItemCount(eItems::ImbuedCrystal, 1, false);
                    break;
                }
                case eQuests::AnExoticEgg:
                {
                    if (p_Player->HasItemCount(eItems::MonstrousEgg, 2))
                        p_Player->DestroyItemCount(eItems::MonstrousEgg, 1, false);
                    break;
                }
                case eQuests::AWondrousWeapon:
                {
                    if (p_Player->HasItemCount(eItems::OrnateWeapon, 2))
                        p_Player->DestroyItemCount(eItems::OrnateWeapon, 1, false);
                    break;
                }
                case eQuests::TheCapturedJournal:
                {
                    if (p_Player->HasItemCount(eItems::FallenAdventurerJournal, 2))
                        p_Player->DestroyItemCount(eItems::FallenAdventurerJournal, 1, false);
                    break;
                }
                case eQuests::TheEnemyInsigna:
                {
                    if (p_Player->HasItemCount(eItems::CapturedInsigna, 2))
                        p_Player->DestroyItemCount(eItems::CapturedInsigna, 1, false);
                    break;
                }
                case eQuests::TheMasterStrategist:
                {
                    if (p_Player->HasItemCount(eItems::ATreatiseOnStrategy, 2))
                        p_Player->DestroyItemCount(eItems::ATreatiseOnStrategy, 1, false);
                    break;
                }
                default:
                    break;
            }
        }
};

/// Wings of Flame trigerred - 170820
class spell_darkmoon_firebird_challenge_check_trigger : public SpellScriptLoader
{
    public:
        spell_darkmoon_firebird_challenge_check_trigger() : SpellScriptLoader("spell_darkmoon_firebird_challenge_check_trigger") { }

        class spell_darkmoon_firebird_challenge_allow_fly_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_darkmoon_firebird_challenge_allow_fly_AuraScript);

            void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                std::list<AreaTrigger*> l_AreatriggerList;
                l_Caster->GetAreaTriggerList(l_AreatriggerList, eSpellDarkmoonFaire::SummonRingFirebird);

                for (AreaTrigger* l_Areatrigger : l_AreatriggerList)
                    l_Areatrigger->Despawn();
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_darkmoon_firebird_challenge_allow_fly_AuraScript::HandleRemove, EFFECT_1, SPELL_AURA_MOD_INCREASE_VEHICLE_FLIGHT_SPEED, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_darkmoon_firebird_challenge_allow_fly_AuraScript();
        }
};

/// Darkmoon Race Mount - 179750 179751 179752 179256
class spell_darkmoon_mount_race : public SpellScriptLoader
{
    public:
        spell_darkmoon_mount_race() : SpellScriptLoader("spell_darkmoon_mount_race") { }

        class  spell_darkmoon_mount_race_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_darkmoon_mount_race_AuraScript);



            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Target = GetTarget();

                if (!l_Target)
                    return;

                if (Player* p_Player = l_Target->ToPlayer())
                {
                    for (uint8 i = 0; i < 5; ++i)
                        if (p_Player->GetQuestStatus(RaceQuestId[i]) == QUEST_STATUS_INCOMPLETE)
                            p_Player->FailQuest(RaceQuestId[i]);

                    std::list<Creature*> triggerList;
                    p_Player->GetCreatureListWithEntryInGrid(triggerList, eCreature::BigRaceBunny, 500.0f);
                    p_Player->GetCreatureListWithEntryInGrid(triggerList, eCreature::ShortRaceBunny, 500.0f);

                    for (auto trigger : triggerList)
                    {
                        if (Unit* l_Owner = trigger->ToTempSummon()->GetSummoner())
                            if (l_Owner->GetGUID() == p_Player->GetGUID())
                                trigger->DespawnOrUnsummon();
                    }
                }
            }

            void Register() override
            {
                switch (m_scriptSpellId)
                {
                    case eSpellDarkmoonFaire::Wanderluster:
                        AfterEffectRemove += AuraEffectRemoveFn(spell_darkmoon_mount_race_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_MOUNTED, AURA_EFFECT_HANDLE_REAL);
                        break;
                    case eSpellDarkmoonFaire::Powermonger:
                    case eSpellDarkmoonFaire::Rocketeer:
                    case eSpellDarkmoonFaire::RacingStrider:
                        AfterEffectRemove += AuraEffectRemoveFn(spell_darkmoon_mount_race_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOUNTED, AURA_EFFECT_HANDLE_REAL);
                        break;
                    default:
                        break;
                }
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_darkmoon_mount_race_AuraScript();
        }
};

/// Firebird Challenge - Wings of flame (trigger) 170819
class spell_darkmoon_firebird_challenge : public SpellScriptLoader
{
public:
    spell_darkmoon_firebird_challenge() : SpellScriptLoader("spell_darkmoon_firebird_challenge") { }

    class spell_darkmoon_firebird_challenge_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_darkmoon_firebird_challenge_AuraScript);


        void OnTick(AuraEffect const* p_AurEff)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            if (p_AurEff->GetTickNumber() == 1)
            {
                for (uint8 i = 1; i < 56; ++i)
                    l_Caster->CastSpell(g_ATFireRingChallengePos[i], eSpellDarkmoonFaire::SummonRingFirebird, true, nullptr, nullptr, l_Caster->GetGUID());
            }
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_darkmoon_firebird_challenge_AuraScript::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_darkmoon_firebird_challenge_AuraScript();
    }
};

/// Whack! Summon Aura - 101994
class spell_darkmoon_gnoll_summon_aura : public SpellScriptLoader
{
    public:
        spell_darkmoon_gnoll_summon_aura() : SpellScriptLoader("spell_darkmoon_gnoll_summon_aura") { }

        class spell_darkmoon_gnoll_summon_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_darkmoon_gnoll_summon_aura_AuraScript);

            std::list<Creature*> l_barrelList;

            void OnTick(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster)
                    return;
                GetBarrels();

                if(l_barrelList.empty())
                    return;

                if(l_barrelList.size() >= 4)
                    JadeCore::RandomResizeList(l_barrelList, urand(1, 3));

                for (auto barrel : l_barrelList)
                {
                    if (Unit* l_Creature = sObjectAccessor->GetUnit(*l_Caster, barrel->GetGUID()))
                    {
                        uint8 m_Rand = urand(0, 2);
                        uint64 m_CreatureToSpawn = BasicGnoll;

                        if(m_Rand == 2)
                            m_CreatureToSpawn = BabyGnoll;
                        else if(m_Rand == 1)
                            m_CreatureToSpawn = BossGnoll;
                        std::list<Creature*> babyList;
                        l_Caster->GetCreatureListWithEntryInGrid(babyList, BabyGnoll, 50.0f);

                        if(babyList.size() >= 2)
                            m_CreatureToSpawn = BasicGnoll;
                        l_Caster->SummonCreature(m_CreatureToSpawn, l_Creature->GetPosition(), TempSummonType::TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 8000);
                    }
                }

            }

            void GetBarrels()
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster)
                    return;
                l_barrelList.clear();
                std::list<Creature*> l_Templist;
                l_Caster->GetCreatureListWithEntryInGrid(l_Templist, GnollBarrel, 50.0f);

                for (auto barrel : l_Templist)
                {
                    if(Creature* basicGnoll = barrel->FindNearestCreature(eCreature::BasicGnoll, 2.0f, true))
                        if(Unit* l_Owner = basicGnoll->ToTempSummon()->GetSummoner())
                            if(l_Owner->GetGUID() == l_Caster->GetGUID())
                                return;

                    if(Creature* BossGnoll = barrel->FindNearestCreature(eCreature::BossGnoll, 2.0f, true))
                        if(Unit* l_Owner = BossGnoll->ToTempSummon()->GetSummoner())
                            if(l_Owner->GetGUID() == l_Caster->GetGUID())
                                return;

                    if(Creature* BabyGnoll = barrel->FindNearestCreature(eCreature::BabyGnoll, 2.0f, true))
                        if(Unit* l_Owner = BabyGnoll->ToTempSummon()->GetSummoner())
                            if(l_Owner->GetGUID() == l_Caster->GetGUID())
                                return;
                    l_barrelList.push_back(barrel);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_darkmoon_gnoll_summon_aura_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_darkmoon_gnoll_summon_aura_AuraScript();
        }
};

/// Darkmoon Deathmatch - 108941
class spell_darkmoon_deathmatch_teleport: public SpellScriptLoader
{
    public:
        spell_darkmoon_deathmatch_teleport() : SpellScriptLoader("spell_darkmoon_deathmatch_teleport") { }

        class spell_darkmoon_deathmatch_teleport_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_darkmoon_deathmatch_teleport_SpellScript);

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster)
                    return;
                l_Caster->CastSpell(l_Caster, l_Caster->GetAreaId() == eArea::DarkmoonDeathmatch ? eSpellDarkmoonFaire::ExitDeathmatch:eSpellDarkmoonFaire::EnterDeathmatch, false);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_darkmoon_deathmatch_teleport_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_darkmoon_deathmatch_teleport_SpellScript();
        }
};

/// Jump To Banner - 179748
/// Jump To Hazard - 153397
class spell_darkmoon_jump_to_checkpoint: public SpellScriptLoader
{
    public:
        spell_darkmoon_jump_to_checkpoint() : SpellScriptLoader("spell_darkmoon_jump_to_checkpoint") { }

        class spell_darkmoon_jump_to_checkpoint_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_darkmoon_jump_to_checkpoint_SpellScript);

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                std::list<Creature*> checkpointList;
                std::list<Creature*> GoodcheckpointList;
                l_Caster->GetCreatureListWithEntryInGrid(checkpointList, eCreature::BunnyCheckpoint, 100.0f);

                if(checkpointList.empty())
                    return;

                for (auto checkpoint : checkpointList)
                {
                    if(l_Caster->isInFront(checkpoint))
                        GoodcheckpointList.push_back(checkpoint);
                }

                if(GoodcheckpointList.empty())
                    return;
                GoodcheckpointList.sort(JadeCore::ObjectDistanceOrderPred(l_Caster));
                l_Caster->CastSpell(GoodcheckpointList.front(), eSpellDarkmoonFaire::HazardImmunity, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_darkmoon_jump_to_checkpoint_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_darkmoon_jump_to_checkpoint_SpellScript();
        }
};

/// Jump To PowerUp - 152725
class spell_darkmoon_jump_to_powerup: public SpellScriptLoader
{
    public:
        spell_darkmoon_jump_to_powerup() : SpellScriptLoader("spell_darkmoon_jump_to_powerup") { }

        class spell_darkmoon_jump_to_powerup_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_darkmoon_jump_to_powerup_SpellScript);

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                std::list<Creature*> goodPowerUpLootList;
                std::list<Creature*> powerUpLootList;
                l_Caster->GetCreatureListWithEntryInGrid(powerUpLootList, eCreature::PowerUpLoot, 100.0f);

                if(powerUpLootList.empty())
                    return;

                for (auto powerUpLoot : goodPowerUpLootList)
                {
                    if(l_Caster->isInFront(powerUpLoot))
                        goodPowerUpLootList.push_back(powerUpLoot);
                }

                if(goodPowerUpLootList.empty())
                    return;

                goodPowerUpLootList.sort(JadeCore::ObjectDistanceOrderPred(l_Caster));
                l_Caster->CastSpell(goodPowerUpLootList.front(), eSpellDarkmoonFaire::HazardImmunity, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_darkmoon_jump_to_powerup_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_darkmoon_jump_to_powerup_SpellScript();
        }
};

/// Jump To Racer - 179804
class spell_darkmoon_jump_to_racer: public SpellScriptLoader
{
    public:
        spell_darkmoon_jump_to_racer() : SpellScriptLoader("spell_darkmoon_jump_to_racer") { }

        class spell_darkmoon_jump_to_racer_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_darkmoon_jump_to_racer_SpellScript);

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                std::list<Player*> playerList;
                std::list<Player*> GoodplayerList;
                l_Caster->GetPlayerListInGrid(playerList, 100.0f);

                if(playerList.empty())
                    return;

                for (auto plr : playerList)
                {
                    if(plr->HasAura(eSpellDarkmoonFaire::Rocketeer))
                        if(l_Caster->isInFront(plr))
                            GoodplayerList.push_back(plr);
                }

                if(GoodplayerList.empty())
                    return;
                GoodplayerList.sort(JadeCore::ObjectDistanceOrderPred(l_Caster));
                l_Caster->CastSpell(GoodplayerList.front(), eSpellDarkmoonFaire::HazardImmunity, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_darkmoon_jump_to_racer_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_darkmoon_jump_to_racer_SpellScript();
        }
};

/// Lasso Powerup - 152943
/// Lasso Powerup - 150288
class spell_darkmoon_lasso_powerup : public SpellScriptLoader
{
public:
    spell_darkmoon_lasso_powerup() : SpellScriptLoader("spell_darkmoon_lasso_powerup") { }

    class spell_darkmoon_lasso_powerup_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_darkmoon_lasso_powerup_SpellScript);

        void FilterTarget(std::list<WorldObject*>& p_Targets)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster || p_Targets.empty())
                return;

            p_Targets.sort(JadeCore::ObjectDistanceOrderPred(l_Caster));
            p_Targets.resize(1);
        }

        void HandleOnHit()
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            if (Unit* l_Target = GetHitUnit())
                if (Creature* l_PowerUpHit = l_Target->ToCreature())
                    l_Caster->SummonCreature(l_PowerUpHit->GetEntry(), l_Caster->GetPosition(), TempSummonType::TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 2000, 0, l_Caster->GetGUID());
        }

        void Register() override
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_darkmoon_lasso_powerup_SpellScript::FilterTarget, EFFECT_0, TARGET_UNIT_CONE_ENTRY);
            OnHit += SpellHitFn(spell_darkmoon_lasso_powerup_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_darkmoon_lasso_powerup_SpellScript();
    }
};

/// Ring Toss - 181458 (triggered)
class spell_darkmoon_ring_toss: public SpellScriptLoader
{
    public:
        spell_darkmoon_ring_toss() : SpellScriptLoader("spell_darkmoon_ring_toss") { }

        class spell_darkmoon_ring_toss_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_darkmoon_ring_toss_SpellScript);

            void HandleOnHit(SpellEffIndex)
            {
                WorldLocation const* l_Loc = GetExplTargetDest();
                if (!GetCaster() || !l_Loc)
                    return;

                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    if(Creature* l_Creature = l_Player->FindNearestCreature(eCreature::Dubenko, 50.0f))
                    {
                        if (l_Creature->GetExactDist2d(l_Loc) < 7.0f)
                        {
                            l_Player->CastSpell(l_Player, RingTossCredit, true);
                            l_Creature->CastSpell(l_Creature, l_Creature->HasAura(VisualOneRing)? VisualTwoRing:VisualOneRing, true);
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_darkmoon_ring_toss_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_darkmoon_ring_toss_SpellScript();
        }
};

/// HUGE, SHARP TEETH! - 114080
class spell_darkmoon_huge_sharp_teeth : public SpellScriptLoader
{
    public:
        spell_darkmoon_huge_sharp_teeth() : SpellScriptLoader("spell_darkmoon_huge_sharp_teeth") { }

        class spell_darkmoon_huge_sharp_teeth_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_darkmoon_huge_sharp_teeth_AuraScript);

            void OnApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if(Creature* l_Creature = l_Caster->FindNearestCreature(eCreature::DarkmoonRabbit, 2.0f, true))
                {
                    l_Creature->NearTeleportTo(l_Caster->GetPosition());
                    l_Creature->CastSpell(l_Caster, eSpellDarkmoonFaire::HugeSharpTeeth, false);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_darkmoon_huge_sharp_teeth_AuraScript::OnApply, EFFECT_0, SPELL_AURA_SET_VEHICLE_ID, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_darkmoon_huge_sharp_teeth_AuraScript();
        }
};

/// Draw - 170978
class spell_darkmoon_faire_draw_toy: public SpellScriptLoader
{
    public:
        spell_darkmoon_faire_draw_toy() : SpellScriptLoader("spell_darkmoon_faire_draw_toy") { }

        class spell_darkmoon_faire_draw_toy_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_darkmoon_faire_draw_toy_SpellScript);

            void HandleOnHit(SpellEffIndex)
            {
                if (!GetCaster())
                    return;

                if (Player* l_Player = GetCaster()->ToPlayer())
                    l_Player->CastSpell(l_Player, DeckOfNemelexToySpell[urand(0, 9)], true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_darkmoon_faire_draw_toy_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_darkmoon_faire_draw_toy_SpellScript();
        }
};

/// Fire-Eater's Vial - 179950
class spell_darkmoon_fire_eater_vial_toy: public SpellScriptLoader
{
    public:
        spell_darkmoon_fire_eater_vial_toy() : SpellScriptLoader("spell_darkmoon_fire_eater_vial_toy") { }

        class spell_darkmoon_fire_eater_vial_toy_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_darkmoon_fire_eater_vial_toy_SpellScript);

            void HandleOnHit(SpellEffIndex)
            {
                if (!GetCaster())
                    return;

                if (Player* l_Player = GetCaster()->ToPlayer())
                    l_Player->CastSpell(l_Player, urand(0, 1) == 0 ? eSpellDarkmoonFaire::FireBreathingPurple : eSpellDarkmoonFaire::FireBreathingGreen, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_darkmoon_fire_eater_vial_toy_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_darkmoon_fire_eater_vial_toy_SpellScript();
        }
};

/// Fling Rings - 179880
class spell_darkmoon_faire_fling_ring_toy: public SpellScriptLoader
{
    public:
        spell_darkmoon_faire_fling_ring_toy() : SpellScriptLoader("spell_darkmoon_faire_fling_ring_toy") { }

        class spell_darkmoon_faire_fling_ring_toy_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_darkmoon_faire_fling_ring_toy_SpellScript);

            void HandleOnHit(SpellEffIndex)
            {
                if (!GetCaster())
                    return;

                if (Player* l_Player = GetCaster()->ToPlayer())
                    for (uint8 i = 1; i < 20; ++i)
                        l_Player->CastSpell(l_Player, eSpellDarkmoonFaire::FlingRings, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_darkmoon_faire_fling_ring_toy_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_darkmoon_faire_fling_ring_toy_SpellScript();
        }
};

/// Citizen Costume - 73814
class spell_darkmoon_citizen_costume : public SpellScriptLoader
{
    public:
        spell_darkmoon_citizen_costume() : SpellScriptLoader("spell_darkmoon_citizen_costume") { }

        class  spell_darkmoon_citizen_costume_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_darkmoon_citizen_costume_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (!GetTarget())
                    return;

                if(Creature* l_Creature = GetTarget()->ToCreature())
                    l_Creature->SetDisplayId(l_Creature->GetEntry() == AllianceCitizen ? AllianceModels[urand(0, 14)] : HordeModels[urand(0, 11)]);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectRemoveFn(spell_darkmoon_citizen_costume_AuraScript::OnApply, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_darkmoon_citizen_costume_AuraScript();
        }
};

/// Darkmoon Cannon - Cannonball! - 62244
class spell_darkmoon_cannonball_landing : public SpellScriptLoader
{
public:
    spell_darkmoon_cannonball_landing() : SpellScriptLoader("spell_darkmoon_cannonball_landing") { }

    class spell_darkmoon_cannonball_landing_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_darkmoon_cannonball_landing_AuraScript);

        void OnTick(AuraEffect const* p_AurEff)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            if(l_Caster->IsInWater())
            {
                float l_DistanceToCenter = l_Caster->GetDistance(-4478.05f, 6220.95f, 0.093f);
                uint32 m_creditCount = 1;

                if (l_DistanceToCenter <= 3.0f)
                {
                    if(Player* l_Player = l_Caster->ToPlayer())
                        if(!l_Player->HasAchieved(eAchievement::BlastenheimmerBullseye))
                            if (AchievementEntry const* l_AchievementEntry = sAchievementStore.LookupEntry(eAchievement::BlastenheimmerBullseye))
                                l_Player->CompletedAchievement(l_AchievementEntry);
                    l_Caster->AddAura(eSpellDarkmoonFaire::Bullseye, l_Caster);
                    m_creditCount = 5;
                }
                else if (l_DistanceToCenter >= 3.1f && l_DistanceToCenter <= 5.0f)
                {
                    l_Caster->AddAura(eSpellDarkmoonFaire::GreatShot, l_Caster);
                    m_creditCount = 3;
                }
                else
                    l_Caster->AddAura(eSpellDarkmoonFaire::PoorShot, l_Caster);

                for (uint8 i = 0; i < m_creditCount; ++i)
                    l_Caster->CastSpell(l_Caster, eSpellDarkmoonFaire::CannonCredit, true);
                l_Caster->RemoveAurasDueToSpell(eSpellDarkmoonFaire::CannonBlastUIBar);
                l_Caster->RemoveAurasDueToSpell(eSpellDarkmoonFaire::Cannonblast);
            }
        }

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_darkmoon_cannonball_landing_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_darkmoon_cannonball_landing_AuraScript();
    }
};

/// 7344
class AreaTrigger_at_darkmoon_faire_gnoll : public AreaTriggerScript
{
    public:
        AreaTrigger_at_darkmoon_faire_gnoll() : AreaTriggerScript("at_darkmoon_faire_gnoll") { }

        bool OnTrigger(Player* p_Player, AreaTriggerEntry const* /*areaTrigger*/) override
        {
            if (!p_Player->HasAura(eSpellDarkmoonFaire::PowerBarGnoll))
            {
                if(Creature* l_Mola = p_Player->FindNearestCreature(eCreature::Mola, 60.0f, true))
                    if(!p_Player->HasAura(eSpellDarkmoonFaire::StayOutGnoll))
                        l_Mola->Whisper(eTrinityString::OutsideGamingArea, p_Player->GetGUID(), true);
                p_Player->CastSpell(p_Player, eSpellDarkmoonFaire::StayOutGnoll, true);
            }

            return true;
        }
};

/// 7338
class AreaTrigger_at_darkmoon_faire_toss : public AreaTriggerScript
{
    public:
        AreaTrigger_at_darkmoon_faire_toss() : AreaTriggerScript("at_darkmoon_faire_toss") { }

        bool OnTrigger(Player* p_Player, AreaTriggerEntry const* /*areaTrigger*/) override
        {
            if(Creature* l_JessRog = p_Player->FindNearestCreature(eCreature::JessRog, 60.0f, true))
                if(!p_Player->HasAura(eSpellDarkmoonFaire::StayOutToss))
                    l_JessRog->Whisper(eTrinityString::OutsideGamingArea, p_Player->GetGUID(), true);
            p_Player->CastSpell(p_Player, eSpellDarkmoonFaire::StayOutToss, true);
            return true;
        }
};

/// 7340
class AreaTrigger_at_darkmoon_faire_tonk : public AreaTriggerScript
{
    public:
        AreaTrigger_at_darkmoon_faire_tonk() : AreaTriggerScript("at_darkmoon_faire_tonk") { }

        bool OnTrigger(Player* p_Player, AreaTriggerEntry const* /*areaTrigger*/) override
        {
            if (!p_Player->HasAura(eSpellDarkmoonFaire::PowerBarTonk))
            {
                if(Creature* l_Finlay = p_Player->FindNearestCreature(eCreature::Finlay, 60.0f, true))
                    if(!p_Player->HasAura(eSpellDarkmoonFaire::StayOutTonk))
                        l_Finlay->Whisper(eTrinityString::OutsideGamingArea, p_Player->GetGUID(), true);
                p_Player->CastSpell(p_Player, eSpellDarkmoonFaire::StayOutTonk, true);

            }

            return true;
        }
};

/// 8725
class AreaTrigger_at_darkmoon_faire_carousel : public AreaTriggerScript
{
    public:
        AreaTrigger_at_darkmoon_faire_carousel() : AreaTriggerScript("at_darkmoon_faire_carousel") { }

        bool OnTrigger(Player* p_Player, AreaTriggerEntry const* /*areaTrigger*/) override
        {
            if (!p_Player->HasItemCount(81055))
            {
                if(Creature* l_KaeTi = p_Player->FindNearestCreature(eCreature::KaeTi, 60.0f, true))
                    if(!p_Player->HasAura(eSpellDarkmoonFaire::StayOutCarousel))
                        l_KaeTi->Whisper(eTrinityString::OutsideCarouselArea, p_Player->GetGUID(), true);

                p_Player->CastSpell(p_Player, eSpellDarkmoonFaire::StayOutCarousel, true);
            }

            return true;
        }
};

#ifndef __clang_analyzer__
void AddSC_darkmoon_island()
{
    /// Creatures
    new npc_darkmoon_faire_korgol();
    new npc_darkmoon_controler_chest();
    new npc_darkmoon_controller_timer_L70ETC();
    new npc_darkmoon_faire_chieftains_controller();
    new npc_darkmoon_faire_shot_controller();
    new npc_darkmoon_faire_shot_target();
    new npc_darkmoon_moonfang_controller();
    new npc_darkmoon_moonfang_wolves();
    /// Playerscript
    new playerscript_darkmoon_adventurer_guide();
    new PlayerScript_darkmoon_race_quest();
    new PlayerScript_darkmoon_item_quest();
    /// Spells
    new spell_darkmoon_firebird_challenge_check_trigger();
    new spell_darkmoon_mount_race();
    new spell_darkmoon_firebird_challenge();
    new spell_darkmoon_gnoll_summon_aura();
    new spell_darkmoon_deathmatch_teleport();
    new spell_darkmoon_jump_to_racer();
    new spell_darkmoon_jump_to_checkpoint();
    new spell_darkmoon_jump_to_powerup();
    new spell_darkmoon_lasso_powerup();
    new spell_darkmoon_ring_toss();
    new spell_darkmoon_huge_sharp_teeth();
    new spell_darkmoon_faire_draw_toy();
    new spell_darkmoon_fire_eater_vial_toy();
    new spell_darkmoon_faire_fling_ring_toy();
    new spell_darkmoon_citizen_costume();
    new spell_darkmoon_cannonball_landing();
    /// AreaTrigger
    new AreaTrigger_at_darkmoon_faire_gnoll();
    new AreaTrigger_at_darkmoon_faire_toss();
    new AreaTrigger_at_darkmoon_faire_tonk();
    new AreaTrigger_at_darkmoon_faire_carousel();
}
#endif
