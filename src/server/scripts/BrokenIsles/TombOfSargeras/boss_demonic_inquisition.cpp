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
#include "InstanceScript.h"

///< Npc Ids:
/*
    Battle with Boss:
    116691 - Belac
    116689 - Atrigan
    117957 - Tormented Soul

    Trash:
    121399 - Inquisitor Sebilus
    121400 - Accusator Gnazh
    121473 - Coercitor Nixa
*/

enum eSpells
{
    ///< Atrigan
    BoneScythe                    = 233423, ///< Aura Proc
    BoneScytheEnergy              = 233420, ///< proc from 233423
    ScytheSweep                   = 233426, /// Only Tank
    BoneSaw                       = 233441, ///< Aura Proc and periodic trigger spell
    BoneSawTakenDamage            = 234550, ///< Proc from 233441
    BoneSawFixate                 = 248741,
    Berserk                       = 64238,
    /// Atrigan - Heroic
    CalcifiedQuills               = 233431,
    CalcifiedQuillsTriggered      = 233432,
    CalcifiedQuillsDamage         = 233435,
    ///< Belac
    PangsofGuilt                  = 239401,
    EchoingAnguish                = 233983,
    EchoingAnguishTriggered       = 239358,
    TormentingBurst               = 234015,
    FelSquall                     = 235230,
    FelSquallEnergy               = 235237,
    ///< Belac - Heroic
    SuffocatingDark               = 233895,
    SuffocatingDarkAT             = 233894,
    SuffocatingDarkDamage         = 233901,
    ///< Tormented Soul
    SoulCorruption                = 248713,
    TheLegionsPrisoner            = 235305,
    TormentedEruption             = 239135,
    RemnantofHopeSummon           = 235301,
    RemnantofHopeAT               = 241604,
    ///< Mythic
    AdherentFragment              = 241505, ///< Summon Fragment
    HarrowingReconstitution       = 241521, ///< count Fragment
    ExplosiveAnguish              = 241524, ///< After Death NpcTormentedFragment
    SpontaneousFragmentationFixate= 241713,
    ///< Other: For Players
    Torment                       = 233103, ///< Aura added Atrigan for Alt power All players
    TormentTrigger                = 233104, /// Triggered 233103
    Confess                       = 234364, /// Triggered 233103
    ConfessTeleport               = 233679,
    ConfessButton                 = 233652,
    RemnantofHopeAddedButton      = 243560,
    RemnantofHopeButton           = 235293,
    BelacPrisoner                 = 236283, ///< Cast when player in Prison
    UnbearableTorment             = 233430, ///< Cast spell when 100% torment
    ///< Generic
    FueledbyTorment               = 246455
};

enum eEvents
{
    ///< Atrigan
    EventScytheSweep = 1,
    EventCalcifiedQuills,
    EventBoneSaw,
    EventBerserk,
    EventCheckRoot,
    ///< Belac
    EventPangsofGuilt,
    EventEchoingAnguish,
    EventTormentingBurst,
    EventFelSquall,
    EventSuffocatingDark,
    ///< Tormented Soul
    EventSoulCorruption,
    EventTormentedEruption
};

enum eMisc
{
    NpcRemnantofHope  = 118562,
    NpcTormentedFragment = 121138
};

enum eTalks
{
    TALK_ATRIGAN_DIALOG = 0,
    TALK_BELAC_DIALOG = 0,
    TALK_ATRIGAN_DIALOG2 = 1,
    TALK_BELAC_DIALOG2 = 1,
    TALK_AGGRO = 2,
    TALK_AGGRO2,
    TALK_AGGRO3,
    TALK_DIED,
    TALK_SLAY,
    TALK_SLAY2,
    TALK_SLAY3,
    TALK_WIPE,
    TALK_WIPE2
};

enum eAchievDemonicInquisition
{
    AchievementGrinandBearIt            = 11696,
    AchievementMythicDemonicInquisition = 11774
};

bool CheckPlayerInRoom(Player* player)
{
   return (player->isAlive() && !player->isGameMaster() && player->GetDistance2d(6401.90f, -796.01f) <= 60.f); ///< Center Room
}

static const Position l_CenterRoom = { 6371.84f, -795.59f, 3011.62f };

struct ResetAggroSelector : public std::unary_function<Unit*, bool>
{
    public:

        ResetAggroSelector(uint64 _guid) : casterGUID(_guid) {}

        bool operator()(Unit const* target) const
        {
            if (target->GetGUID() == casterGUID)
                return true;

            return false;
        }
private:
    uint64 casterGUID;
};

class EventDialogBeforeBattle: public BasicEvent
{
    public:

    explicit EventDialogBeforeBattle(Creature* p_Creature, uint8 p_Count) : m_Creature(p_Creature), m_Count(p_Count)
    {
    }

    bool Execute(uint64 /*p_CurrTime*/, uint32 /*p_Diff*/)
    {
        if (m_Creature)
        {
            if (m_Creature->GetEntry() == eCreatures::NpcAtrigan)
            {
                if (m_Creature->AI())
                    m_Creature->AI()->Talk(eTalks::TALK_ATRIGAN_DIALOG2);
            }
            else if (m_Creature->GetEntry() == eCreatures::NpcBelac)
            {
                if (m_Creature->AI())
                    m_Creature->AI()->Talk(m_Count);
            }
        }

        return true;
    }

private:
    Creature* m_Creature;
    uint8 m_Count;
};

class EventCastCalcifiedQuills: public BasicEvent
{
    public:

    explicit EventCastCalcifiedQuills(Unit* p_Caster, float p_x, float p_y, float p_z) : m_Caster(p_Caster), m_x(p_x), m_y(p_y), m_z(p_z)
    {
    }

    bool Execute(uint64 /*p_CurrTime*/, uint32 /*p_Diff*/)
    {
        if (m_Caster)
            m_Caster->CastSpell(m_x, m_y, m_z, eSpells::CalcifiedQuillsTriggered, true);

        return true;
    }

private:
    Unit* m_Caster;
    float m_x, m_y, m_z;
};

class boss_atrigan : public CreatureScript
{
    public:
        boss_atrigan() : CreatureScript("boss_atrigan") { }

        struct boss_atriganAI : public BossAI
        {
            boss_atriganAI(Creature* creature) : BossAI(creature, eData::DataDemonicInquisition)
            {
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_STUN, true);
            }

            std::set<uint64> l_CalcifiedQuillsTargets;

            void SetCalcifiedQuillsTarget(uint64 pGuid)
            {
                l_CalcifiedQuillsTargets.insert(pGuid);
            }

            bool HasCalcifiedQuillsTarget(uint64 pGuid)
            {
                return (l_CalcifiedQuillsTargets.find(pGuid) != l_CalcifiedQuillsTargets.end());
            }

            void InitializeAI() override
            {
                if (!instance || static_cast<InstanceMap*>(me->GetMap())->GetScriptId() != sObjectMgr->GetScriptId(TOSScriptName))
                    me->IsAIEnabled = false;
                else if (!me->isDead())
                    Reset();
            }

            void ClearOther()
            {
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::BoneSawFixate);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::EchoingAnguish);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SuffocatingDarkDamage);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::TormentTrigger);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::Confess);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::ConfessButton);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::RemnantofHopeAddedButton);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::UnbearableTorment);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpontaneousFragmentationFixate);
                me->RemoveAurasDueToSpell(eSpells::FueledbyTorment);
                me->DespawnAreaTriggersInArea(eSpells::SuffocatingDarkAT, 500.f);
                me->DespawnAreaTriggersInArea(eSpells::RemnantofHopeAT, 500.f);
                me->DespawnCreaturesInArea(eMisc::NpcRemnantofHope, 500.f);

                me->SetPower(POWER_ENERGY, 0);
                if (Creature* l_Belac = ObjectAccessor::GetCreature(*me, instance->GetData64(eCreatures::NpcBelac)))
                {
                    l_Belac->SetPower(POWER_ENERGY, 0);
                    l_Belac->RemoveAurasDueToSpell(eSpells::FueledbyTorment);
                }

                if (IsMythic())
                    me->DespawnCreaturesInArea(eMisc::NpcTormentedFragment, 200.f);
            }

            bool IsEasyDifficulty()
            {
                return (IsNormal() || IsLFR());
            }

            void Reset() override
            {
                me->SetPower(POWER_ENERGY, 0);
                _Reset();
                events.Reset();
                ClearOther();
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::BelacPrisoner);
                me->CastSpell(me, eSpells::BoneScythe, true);
                me->SetCanFly(false);
                me->SetDisableGravity(false);
            }

            bool BypassXPDistanceIfNeeded(WorldObject* /*p_Object*/) override
            {
                return true;
            }

            bool IgnoreFlyingMovementFlagsUpdate() override
            {
                return true;
            }

            bool CanFly() override
            {
                return false;
            }

            void DamageTaken(Unit* attacker, uint32 &damage, SpellInfo const* p_SpellInfo) override
            {
                if (!me || !me->isAlive() || !attacker)
                    return;

                if (Aura* I_BoneSawAura = me->GetAura(eSpells::BoneSaw))
                {
                    if (p_SpellInfo && p_SpellInfo->Id == 148859) ///< doesn't proc from Shadowy Apparition (Level 110)
                        return;

                    if (p_SpellInfo && p_SpellInfo->IsPeriodic())
                    {
                        if (Aura* I_SpellInfoAura = me->GetAura(p_SpellInfo->Id, attacker->GetGUID()))
                        {
                            if (I_BoneSawAura->GetApplyTime() > I_SpellInfoAura->GetApplyTime())
                                return;
                        }
                    }

                    if (attacker->IsPlayer())
                        me->CastSpell(attacker, eSpells::BoneSawTakenDamage, true);
                    else if (attacker->isPet())
                    {
                        if (Unit* l_Attaker = attacker->GetOwner())
                            me->CastSpell(l_Attaker, eSpells::BoneSawTakenDamage, true);
                    }
                }
            }

            void EnterCombat(Unit* attacker) override
            {
                //Talk(urand(eTalks::TALK_AGGRO, eTalks::TALK_AGGRO3));
                me->CastSpell(me, eSpells::Torment, true);
                me->CastSpell(me, eSpells::FueledbyTorment, true);

                _EnterCombat();

                if (Creature* l_Belac = ObjectAccessor::GetCreature(*me, instance->GetData64(eCreatures::NpcBelac)))
                {
                    int32 l_bp = 2;
                    l_Belac->CastCustomSpell(l_Belac, eSpells::FueledbyTorment, &l_bp, NULL, NULL, true);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, l_Belac);
                    l_Belac->SetInCombatWithZone();
                    if (!l_Belac->isInCombat() && !l_Belac->IsInEvadeMode())
                        l_Belac->AI()->EnterCombat(attacker);
                }

                events.ScheduleEvent(eEvents::EventScytheSweep, 5200);
                events.ScheduleEvent(eEvents::EventBoneSaw, 60 * IN_MILLISECONDS);

                if (!IsEasyDifficulty())
                    events.ScheduleEvent(eEvents::EventCalcifiedQuills, 8500);

                if (!IsLFR())
                    events.ScheduleEvent(eEvents::EventBerserk, 720 * IN_MILLISECONDS);
            }

            void JustReachedHome() override
            {
                 _JustReachedHome();
                 //Talk(urand(eTalks::TALK_WIPE, eTalks::TALK_WIPE2));
                 CheckWipe();
                 ClearOther();
                 instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::BelacPrisoner);
                 me->DeleteThreatList();
                 me->CombatStop(true);
            }

            void CheckWipe()
            {
                Map* map = me->GetMap();
                if (!map->IsDungeon())
                    return;

                Map::PlayerList const &PlayerList = map->GetPlayers();
                if (PlayerList.isEmpty())
                    return;

                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                {
                    if (!i->getSource()->isGameMaster() && i->getSource()->HasAura(eSpells::BelacPrisoner) && i->getSource()->GetDistance2d(6401.90f, -796.01f) > 60.f) ///< Center Room
                    {
                        i->getSource()->NearTeleportTo(l_CenterRoom);
                        i->getSource()->RemoveAurasDueToSpell(eSpells::BelacPrisoner);
                    }
                }
            }

            void EnterEvadeMode() override
            {
                ClearOther();
                CheckWipe();
                if (Creature* l_Belac = ObjectAccessor::GetCreature(*me, instance->GetData64(eCreatures::NpcBelac)))
                {
                    if (l_Belac->AI())
                    {
                        l_Belac->AI()->DoAction(0);
                        l_Belac->AI()->EnterEvadeMode();
                    }
                }
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::BelacPrisoner);

                BossAI::EnterEvadeMode();
                me->DeleteThreatList();
                me->CombatStop(true);
            }

            void KilledUnit(Unit* p_Who) override
            {
                //if (p_Who->IsPlayer())
                    //Talk(urand(eTalks::TALK_SLAY, eTalks::TALK_SLAY3));
            }

            Player* SelectRandomPlayer()
            {
                Map* map = me->GetMap();
                if (!map->IsDungeon())
                    return nullptr;

                Map::PlayerList const &PlayerList = map->GetPlayers();
                if (PlayerList.isEmpty())
                    return nullptr;

                std::list<Player*> temp;
                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    if (i->getSource()->isAlive() && !i->getSource()->isGameMaster())
                        temp.push_back(i->getSource());

                if (!temp.empty())
                {
                    std::list<Player*>::const_iterator j = temp.begin();
                    advance(j, rand()%temp.size());
                    return (*j);
                }
                return nullptr;
            }

            bool CheckAchievementGrinandBearIt()
            {
                Map* map = me->GetMap();
                if (!map->IsDungeon())
                    return false;

                Map::PlayerList const &PlayerList = map->GetPlayers();
                if (PlayerList.isEmpty())
                    return false;

                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    if (i->getSource()->isAlive() && !i->getSource()->isGameMaster() && !i->getSource()->HasAura(eSpells::UnbearableTorment))
                        return false;

                return true;
            }

            void JustDied(Unit* p_Killer) override
            {
                _JustDied();
                if (Creature* l_Belac = ObjectAccessor::GetCreature(*me, instance->GetData64(eCreatures::NpcBelac)))
                    if (l_Belac->isAlive())
                        if (Player* player = SelectRandomPlayer())
                            player->Kill(l_Belac);

                CheckWipe();
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::BelacPrisoner);
                if (!IsLFR() && CheckAchievementGrinandBearIt())
                    instance->DoCompleteAchievement(eAchievDemonicInquisition::AchievementGrinandBearIt);

                if (IsMythic())
                    instance->DoCompleteAchievement(eAchievDemonicInquisition::AchievementMythicDemonicInquisition);

                //Talk(eTalks::TALK_DIED);
                ClearOther();
            }

            bool CheckRoom()
            {
                if (!me->isInCombat())
                    return false;

                if (me->GetDistance2d(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY()) > 75)
                {
                    EnterEvadeMode();
                    return false;
                }

                return true;
            }

            void DoMeleeAttackIfReady()
            {
                if (me->HasUnitState(UNIT_STATE_CASTING) || me->HasUnitState(UNIT_STATE_ROOT))
                    return;

                Unit* l_Victim = me->getVictim();

                if (l_Victim != nullptr && !l_Victim->isAlive())
                    return;

                if (!me->IsWithinMeleeRange(l_Victim))
                    return;

                if (me->isAttackReady())
                {
                    me->AttackerStateUpdate(l_Victim);
                    me->resetAttackTimer();
                    DoCastVictim(eSpells::BoneScytheEnergy);
                }

                if (me->haveOffhandWeapon() && me->isAttackReady(WeaponAttackType::OffAttack))
                {
                    me->AttackerStateUpdate(l_Victim, WeaponAttackType::OffAttack);
                    me->resetAttackTimer(WeaponAttackType::OffAttack);
                    DoCastVictim(eSpells::BoneScytheEnergy);
                }
            }

            bool UpdateVictim()
            {
                if (!me->isInCombat())
                    return false;

                if (!me->HasReactState(REACT_PASSIVE))
                {
                    if (Unit* l_Victim = me->SelectVictim())
                    {
                        if (l_Victim->GetDistance2d(6401.90f, -796.01f) > 60.f)
                        {
                            me->RemoveAurasByType(SPELL_AURA_MOD_TAUNT, l_Victim->GetGUID());
                            me->getThreatManager().resetAggro(ResetAggroSelector(l_Victim->GetGUID()));
                            if (Creature* l_Belac = ObjectAccessor::GetCreature(*me, instance->GetData64(eCreatures::NpcBelac)))
                            {
                                l_Belac->RemoveAurasByType(SPELL_AURA_MOD_TAUNT, l_Victim->GetGUID());
                                l_Belac->getThreatManager().resetAggro(ResetAggroSelector(l_Victim->GetGUID()));
                            }

                            std::list<HostileReference*> l_ThreatList = me->getThreatManager().getThreatList();
                            if (l_ThreatList.empty())
                            {
                                EnterEvadeMode();
                                return false;
                            }

                            l_ThreatList.remove_if([this](HostileReference* p_HostileRef) -> bool
                            {
                                Unit* l_Target = p_HostileRef->getTarget();
                                if (!l_Target || !l_Target->isAlive() || !me->IsValidAttackTarget(l_Target) || l_Target->GetDistance2d(6401.90f, -796.01f) > 60.f)
                                    return true;

                                return false;
                            });

                            if (l_ThreatList.empty())
                            {
                                EnterEvadeMode();
                                return false;
                            }

                            l_ThreatList.sort(JadeCore::ThreatOrderPred());

                            if (me->HasUnitState(UNIT_STATE_ROOT))
                            {
                                me->GetMotionMaster()->Clear();
                                me->AttackStop();
                                me->StopMoving();
                                return true;
                            }

                            if (Unit* l_NewTarget = l_ThreatList.front()->getTarget())
                            {
                                AttackStart(l_NewTarget);
                                me->GetMotionMaster()->Clear();
                                me->GetMotionMaster()->MoveChase(l_NewTarget);
                            }

                            return true;
                        }
                        else
                        {
                            if (me->HasUnitState(UNIT_STATE_ROOT))
                            {
                                me->GetMotionMaster()->Clear();
                                me->AttackStop();
                                me->StopMoving();
                                return true;
                            }

                            AttackStart(l_Victim);
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveChase(l_Victim);
                        }
                    }
                    return me->getVictim();
                }
                else if (me->getThreatManager().isThreatListEmpty())
                {
                    EnterEvadeMode();
                    return false;
                }

                return true;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (me->SelectNearestPlayerNotGM(75.0f) && instance && instance->GetData(eData::DataPreEventDemonicInquisition) == NOT_STARTED)
                {
                    instance->SetData(eData::DataPreEventDemonicInquisition, DONE);
                    Talk(eTalks::TALK_ATRIGAN_DIALOG);
                    me->m_Events.AddEvent(new EventDialogBeforeBattle(me, uint8(eTalks::TALK_ATRIGAN_DIALOG2)), me->m_Events.CalculateTime(20 * TimeConstants::IN_MILLISECONDS));
                    if (Creature* l_Belac = ObjectAccessor::GetCreature(*me, instance->GetData64(eCreatures::NpcBelac)))
                    {
                        l_Belac->m_Events.AddEvent(new EventDialogBeforeBattle(l_Belac, uint8(eTalks::TALK_BELAC_DIALOG)), l_Belac->m_Events.CalculateTime(8 * TimeConstants::IN_MILLISECONDS));
                        l_Belac->m_Events.AddEvent(new EventDialogBeforeBattle(l_Belac, uint8(eTalks::TALK_BELAC_DIALOG2)), l_Belac->m_Events.CalculateTime(32 * TimeConstants::IN_MILLISECONDS));
                    }
                }

                if (me->isInCombat() && instance)
                {
                    if (Creature* l_Belac = ObjectAccessor::GetCreature(*me, instance->GetData64(eCreatures::NpcBelac)))
                    {
                        if (l_Belac->GetHealth() > me->GetHealth())
                            l_Belac->SetHealth(me->GetHealth());
                        else if (l_Belac->GetHealth() < me->GetHealth())
                            me->SetHealth(l_Belac->GetHealth());
                    }
                }

                if (!CheckRoom() || !UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eEvents::EventScytheSweep:
                        {
                            DoCast(eSpells::ScytheSweep);
                            events.ScheduleEvent(eEvents::EventScytheSweep, 23 * IN_MILLISECONDS);
                            break;
                        }
                        case eEvents::EventCalcifiedQuills:
                        {
                            me->AddUnitState(UNIT_STATE_ROOT);
                            me->AttackStop();
                            me->StopMoving();
                            l_CalcifiedQuillsTargets.clear();
                            DoCast(eSpells::CalcifiedQuills, true);
                            events.ScheduleEvent(eEvents::EventCalcifiedQuills, 20200);
                            events.ScheduleEvent(eEvents::EventCheckRoot, 5500);
                            break;
                        }
                        case eEvents::EventBoneSaw:
                        {
                            DoCast(me, eSpells::BoneSaw);
                            events.ScheduleEvent(eEvents::EventBoneSaw, 65 * IN_MILLISECONDS);
                            break;
                        }
                        case eEvents::EventBerserk:
                        {
                            DoCast(me, eSpells::Berserk);
                            break;
                        }
                        case eEvents::EventCheckRoot:
                        {
                            me->ClearUnitState(UNIT_STATE_ROOT);
                            break;
                        }
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_atriganAI(p_Creature);
        }
};

class boss_belac : public CreatureScript
{
    public:
        boss_belac() : CreatureScript("boss_belac") { }

        struct boss_belacAI : public BossAI
        {
            boss_belacAI(Creature* creature) : BossAI(creature, eData::DataDemonicInquisition)
            {
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_STUN, true);
            }

            bool l_Evade;
            std::set<uint64> l_TeleportCenterRoomTargets;

            void SetTeleportCenterRoomTarget(uint64 pGuid)
            {
                l_TeleportCenterRoomTargets.insert(pGuid);
            }

            void InitializeAI() override
            {
                if (!instance || static_cast<InstanceMap*>(me->GetMap())->GetScriptId() != sObjectMgr->GetScriptId(TOSScriptName))
                    me->IsAIEnabled = false;
                else if (!me->isDead())
                    Reset();
            }

            void Reset() override
            {
                me->SetPower(POWER_ENERGY, 0);
                l_Evade = false;
                me->AddUnitState(UNIT_STATE_ROOT);
                events.Reset();
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                l_TeleportCenterRoomTargets.clear();
            }

            void EnterEvadeMode() override
            {
                if (!l_Evade)
                    return;
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                BossAI::EnterEvadeMode();
                l_Evade = false;
                me->DeleteThreatList();
                me->CombatStop(true);
            }

            void JustReachedHome() override
            {
                 me->DeleteThreatList();
                 me->CombatStop(true);
            }

            void DoAction(int32 const p_Action) override
            {
                l_Evade = true;
            }

            bool IsEasyDifficulty()
            {
                return (IsNormal() || IsLFR());
            }

            void DamageTaken(Unit* attacker, uint32 &damage, SpellInfo const* p_SpellInfo) override
            {
                if (!me || !me->isAlive() || !instance || !attacker)
                    return;

                if (Aura* I_FelSquallAura = me->GetAura(eSpells::FelSquall))
                {
                    if (p_SpellInfo && p_SpellInfo->Id == 148859) ///< doesn't proc from Shadowy Apparition (Level 110)
                        return;

                    if (p_SpellInfo && p_SpellInfo->IsPeriodic())
                    {
                        if (Aura* I_SpellInfoAura = me->GetAura(p_SpellInfo->Id, attacker->GetGUID()))
                        {
                            if (I_FelSquallAura->GetApplyTime() > I_SpellInfoAura->GetApplyTime())
                                return;
                        }
                    }

                    if (attacker->IsPlayer())
                        me->CastSpell(attacker, eSpells::FelSquallEnergy, true);
                    else if (attacker->isPet())
                    {
                        if (Unit* l_Attaker = attacker->GetOwner())
                            me->CastSpell(l_Attaker, eSpells::FelSquallEnergy, true);
                    }
                }
            }

            void EnterCombat(Unit* attacker) override
            {
                if (instance)
                {
                    if (Creature* l_Atrigan = ObjectAccessor::GetCreature(*me, instance->GetData64(eCreatures::NpcAtrigan)))
                    {
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                        l_Atrigan->SetInCombatWithZone();
                        if (!l_Atrigan->isInCombat() && !l_Atrigan->IsInEvadeMode())
                            l_Atrigan->AI()->EnterCombat(attacker);
                    }
                }

                events.ScheduleEvent(eEvents::EventPangsofGuilt, 500);
                events.ScheduleEvent(eEvents::EventEchoingAnguish, urand(6 * IN_MILLISECONDS, 20 * IN_MILLISECONDS));
                events.ScheduleEvent(eEvents::EventTormentingBurst, 15 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventFelSquall, 35 * IN_MILLISECONDS);

                if (!IsEasyDifficulty())
                    events.ScheduleEvent(eEvents::EventSuffocatingDark, urand(13 * IN_MILLISECONDS, 48 * IN_MILLISECONDS));

                if (!IsLFR())
                    events.ScheduleEvent(eEvents::EventBerserk, 720 * IN_MILLISECONDS);
            }

            Player* SelectRandomPlayer()
            {
                Map* map = me->GetMap();
                if (!map->IsDungeon())
                    return nullptr;

                Map::PlayerList const &PlayerList = map->GetPlayers();
                if (PlayerList.isEmpty())
                    return nullptr;

                std::list<Player*> temp;
                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    if (i->getSource()->isAlive() && !i->getSource()->isGameMaster())
                        temp.push_back(i->getSource());

                if (!temp.empty())
                {
                    std::list<Player*>::const_iterator j = temp.begin();
                    advance(j, rand()%temp.size());
                    return (*j);
                }
                return nullptr;
            }

            void JustDied(Unit* p_Killer) override
            {
                if (Creature* l_Atrigan = ObjectAccessor::GetCreature(*me, instance->GetData64(eCreatures::NpcAtrigan)))
                    if (l_Atrigan->isAlive())
                        if (Player* player = SelectRandomPlayer())
                            player->Kill(l_Atrigan);

                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void OnInterruptCast(Unit* /*p_Caster*/, SpellInfo const* /*p_SpellInfo*/, SpellInfo const* p_CurrSpellInfo, uint32 /*p_SchoolMask*/) override
            {
                if (p_CurrSpellInfo->Id == eSpells::PangsofGuilt)
                    events.RescheduleEvent(eEvents::EventPangsofGuilt, 6 * IN_MILLISECONDS);
            }

            void CheckHitResult(MeleeHitOutcome& /*p_MeleeResult*/, SpellMissInfo& /*p_SpellResult*/, Unit* p_Attacker, SpellInfo const* const p_SpellInfo ) override
            {
                if (!p_SpellInfo || !p_SpellInfo->HasAura(SPELL_AURA_MOD_SILENCE)) ///< 8/24/2018 fixed issue with Silience
                    return;

                for (uint32 i = CURRENT_MELEE_SPELL; i < CURRENT_MAX_SPELL; ++i)
                {
                    if (Spell* spell = me->GetCurrentSpell(CurrentSpellTypes(i)))
                    {
                        if (spell->m_spellInfo->Id == eSpells::PangsofGuilt)
                        {
                            me->InterruptSpell(CurrentSpellTypes(i), false);
                            events.RescheduleEvent(eEvents::EventPangsofGuilt, 6 * IN_MILLISECONDS);
                            break;
                        }
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!l_TeleportCenterRoomTargets.empty())
                {
                    for (std::set<uint64>::const_iterator tar = l_TeleportCenterRoomTargets.begin(); tar != l_TeleportCenterRoomTargets.end();)
                    {
                        if (Player* l_Player = ObjectAccessor::GetPlayer(*me, *tar))
                        {
                            if (l_Player->IsBeingTeleportedFar() || !l_Player->GetSession() || l_Player->GetSession()->PlayerLoading() || l_Player->GetDistance2d(me) > 5.f)
                            {
                                ++tar;
                                continue;
                            }

                            tar = l_TeleportCenterRoomTargets.erase(tar);
                            l_Player->GetMotionMaster()->MoveJump(6371.84f, -795.59f, 3011.62f, 50.f, 20.f, 10.f);
                            if (IsMythic())
                                me->CastSpell(me, eSpells::AdherentFragment, true);
                        }
                        else
                            ++tar;
                    }
                }

                if (!UpdateVictim())
                    return;

                if (Creature* l_Atrigan = ObjectAccessor::GetCreature(*me, instance->GetData64(eCreatures::NpcAtrigan)))
                {
                    if (l_Atrigan->GetHealth() > me->GetHealth())
                        l_Atrigan->SetHealth(me->GetHealth());
                    else if (l_Atrigan->GetHealth() < me->GetHealth())
                        me->SetHealth(l_Atrigan->GetHealth());
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING) || me->HasAura(eSpells::FelSquall))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eEvents::EventPangsofGuilt:
                        {
                            DoCast(eSpells::PangsofGuilt);
                            events.ScheduleEvent(eEvents::EventPangsofGuilt, urand(3 * IN_MILLISECONDS, 5 * IN_MILLISECONDS));
                            break;
                        }
                        case eEvents::EventEchoingAnguish:
                        {
                            DoCast(eSpells::EchoingAnguish);
                            events.ScheduleEvent(eEvents::EventEchoingAnguish, 22900);
                            break;
                        }
                        case eEvents::EventTormentingBurst:
                        {
                            DoCast(eSpells::TormentingBurst);
                            events.ScheduleEvent(eEvents::EventTormentingBurst, 17 * IN_MILLISECONDS);
                            break;
                        }
                        case eEvents::EventFelSquall:
                        {
                            me->CastSpell(me, eSpells::FelSquall, true);
                            me->SetPower(POWER_ENERGY, 0);
                            me->RemoveAurasDueToSpell(eSpells::FueledbyTorment);
                            me->CastSpell(me, eSpells::FueledbyTorment, true);
                            events.ScheduleEvent(eEvents::EventFelSquall, 60 * IN_MILLISECONDS);
                            break;
                        }
                        case eEvents::EventSuffocatingDark:
                        {
                            DoCast(eSpells::SuffocatingDark);
                            events.ScheduleEvent(eEvents::EventSuffocatingDark, 24 * IN_MILLISECONDS);
                            break;
                        }
                        case eEvents::EventBerserk:
                        {
                            DoCast(eSpells::Berserk);
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
            return new boss_belacAI(p_Creature);
        }
};

class npc_inquisition_tormented_soul : public CreatureScript
{
    public:
        npc_inquisition_tormented_soul() : CreatureScript("npc_inquisition_tormented_soul") { }

        struct npc_inquisition_tormented_soulAI : public ScriptedAI
        {
            npc_inquisition_tormented_soulAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
            }

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();
                if (!me->HasAura(eSpells::TheLegionsPrisoner))
                    me->CastSpell(me, eSpells::TheLegionsPrisoner, true);
            }

            void DamageTaken(Unit* attacker, uint32 &damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (me->GetHealthPct() <= 60)
                    me->SetHealth(me->GetMaxHealth());
            }

            void EnterCombat(Unit* /*attacker*/) override
            {
                m_Events.ScheduleEvent(eEvents::EventSoulCorruption, 1500);
                m_Events.ScheduleEvent(eEvents::EventTormentedEruption, 500);
                if (!me->HasAura(eSpells::TheLegionsPrisoner))
                    me->CastSpell(me, eSpells::TheLegionsPrisoner, true);
            }

            void EnterEvadeMode() override
            {
                me->RemoveAurasByType(SPELL_AURA_MOD_TAUNT);
                me->DespawnAreaTriggersInArea(eSpells::RemnantofHopeAT, 100.f);
                me->DespawnCreaturesInArea(eMisc::NpcRemnantofHope, 100.f);
                ScriptedAI::EnterEvadeMode();
            }

            bool CheckPlayers()
            {
                std::list<Player*> playerList;
                GetPlayerListInGrid(playerList, me, 50.0f);

                for (auto player : playerList)
                     if (player->isAlive() && player->HasAura(eSpells::BelacPrisoner))
                         return true;

                return false;
            }
            
            bool UpdateVictim()
            {
                if (!me->isInCombat())
                    return false;

                if (!me->HasReactState(REACT_PASSIVE))
                {
                    if (Unit* l_Victim = me->SelectVictim())
                    {
                        if (l_Victim->GetDistance2d(6401.90f, -796.01f) <= 60.f)
                        {
                            me->RemoveAurasByType(SPELL_AURA_MOD_TAUNT, l_Victim->GetGUID());
                            me->getThreatManager().resetAggro(ResetAggroSelector(l_Victim->GetGUID()));

                            std::list<HostileReference*> l_ThreatList = me->getThreatManager().getThreatList();
                            if (l_ThreatList.empty())
                            {
                                EnterEvadeMode();
                                return false;
                            }

                            l_ThreatList.remove_if([this](HostileReference* p_HostileRef) -> bool
                            {
                                Unit* l_Target = p_HostileRef->getTarget();
                                if (!l_Target || !l_Target->isAlive() || !me->IsValidAttackTarget(l_Target) || l_Target->GetDistance2d(6401.90f, -796.01f) <= 60.f)
                                    return true;

                                return false;
                            });

                            if (l_ThreatList.empty())
                            {
                                EnterEvadeMode();
                                return false;
                            }

                            l_ThreatList.sort(JadeCore::ThreatOrderPred());

                            if (me->HasUnitState(UNIT_STATE_ROOT))
                            {
                                me->GetMotionMaster()->Clear();
                                me->AttackStop();
                                me->StopMoving();
                                return true;
                            }

                            if (Unit* l_NewTarget = l_ThreatList.front()->getTarget())
                            {
                                AttackStart(l_NewTarget);
                                me->GetMotionMaster()->Clear();
                                me->GetMotionMaster()->MoveChase(l_NewTarget);
                            }

                            return true;
                        }
                        else
                        {
                            if (me->HasUnitState(UNIT_STATE_ROOT))
                            {
                                me->GetMotionMaster()->Clear();
                                me->AttackStop();
                                me->StopMoving();
                                return true;
                            }

                            AttackStart(l_Victim);
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveChase(l_Victim);
                        }
                    }
                    return me->getVictim();
                }
                else if (me->getThreatManager().isThreatListEmpty())
                {
                    EnterEvadeMode();
                    return false;
                }

                return true;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                if (!CheckPlayers())
                {
                    EnterEvadeMode();
                    return;
                }

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = m_Events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eEvents::EventSoulCorruption:
                        {
                            DoCast(eSpells::SoulCorruption);
                            m_Events.ScheduleEvent(eEvents::EventSoulCorruption, urand(8 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
                            break;
                        }
                        case eEvents::EventTormentedEruption:
                        {
                            DoCast(eSpells::TormentedEruption);
                            m_Events.ScheduleEvent(eEvents::EventTormentedEruption, urand(12 * IN_MILLISECONDS, 30 * IN_MILLISECONDS));
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
            return new npc_inquisition_tormented_soulAI(p_Creature);
        }
};

/// Last Update: 7.3.5 26365
/// Called by Bone Saw - 233441
class spell_inquisition_bone_saw : public SpellScriptLoader
{
    public:
        spell_inquisition_bone_saw() : SpellScriptLoader("spell_inquisition_bone_saw") { }

        class spell_inquisition_bone_saw_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_inquisition_bone_saw_AuraScript);

            bool HandleCheckProc(ProcEventInfo& p_ProcInfo)
            {
                return false;
            }

            void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::BoneSawFixate, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_inquisition_bone_saw_AuraScript::HandleCheckProc);
                AfterEffectApply += AuraEffectApplyFn(spell_inquisition_bone_saw_AuraScript::AfterApply, EFFECT_1, SPELL_AURA_MOD_DECREASE_SPEED, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_inquisition_bone_saw_AuraScript();
        }
};

/// Last Update: 7.3.5 26365
/// Called by Bone Saw - 248741
class spell_inquisition_bone_saw_fixate : public SpellScriptLoader
{
    public:
        spell_inquisition_bone_saw_fixate() : SpellScriptLoader("spell_inquisition_bone_saw_fixate") { }

        class spell_inquisition_bone_saw_fixate_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_inquisition_bone_saw_fixate_SpellScript);

            void SelectTarget(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* I_Victim = l_Caster->getVictim();
                if (!I_Victim)
                    return;

                p_Targets.clear();
                p_Targets.push_back(I_Victim);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_inquisition_bone_saw_fixate_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_inquisition_bone_saw_fixate_SpellScript();
        }
};

/// Last Update: 7.3.5 26365
/// Called by Calcified Quills - 233431
class spell_inquisition_calcified_quills : public SpellScriptLoader
{
    public:
        spell_inquisition_calcified_quills() : SpellScriptLoader("spell_inquisition_calcified_quills") { }

        class spell_inquisition_calcified_quills_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_inquisition_calcified_quills_SpellScript);

            void SelectTarget(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (InstanceScript* p_instance = l_Caster->GetInstanceScript())
                {
                    if (Unit* l_Atrigan = ObjectAccessor::GetUnit(*l_Caster, p_instance->GetData64(eCreatures::NpcAtrigan)))
                    {
                        if (Unit* I_Victim = l_Atrigan->getVictim())
                            p_Targets.remove(I_Victim);
                    }
                }

                p_Targets.remove_if([](WorldObject* p_Target) -> bool
                {
                    if (!p_Target->IsPlayer() || p_Target->ToPlayer()->IsActiveSpecTankSpec())
                        return true;

                    return false;
                });

                JadeCore::RandomResizeList(p_Targets, 1);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_inquisition_calcified_quills_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        class spell_inquisition_calcified_quills_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_inquisition_calcified_quills_AuraScript);

            void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->AttackStop();
                l_Caster->StopMoving();
                l_Caster->SetFacingToObject(l_Target);
                l_Caster->SetOrientation(l_Caster->GetAngle(l_Target));
                for (uint8 i = 1; i <= 100; ++i)
                {
                    ///< Front
                    Position posCenter;
                    l_Caster->GetNearPosition(posCenter, 3.0f * i, 0.f);
                    l_Caster->m_Events.AddEvent(new EventCastCalcifiedQuills(l_Caster, posCenter.GetPositionX(), posCenter.GetPositionY(), posCenter.GetPositionZ()), l_Caster->m_Events.CalculateTime(5 * i));
                    ///< Front Right
                    Position posCenterRight;
                    l_Caster->GetNearPosition(posCenterRight, 3.0f * i, -0.1f);
                    l_Caster->m_Events.AddEvent(new EventCastCalcifiedQuills(l_Caster, posCenterRight.GetPositionX(), posCenterRight.GetPositionY(), posCenterRight.GetPositionZ()), l_Caster->m_Events.CalculateTime(5 * i));
                    ///< Front Left
                    Position posCenterLeft;
                    l_Caster->GetNearPosition(posCenterLeft, 3.0f * i, 0.1f);
                    l_Caster->m_Events.AddEvent(new EventCastCalcifiedQuills(l_Caster, posCenterLeft.GetPositionX(), posCenterLeft.GetPositionY(), posCenterLeft.GetPositionZ()), l_Caster->m_Events.CalculateTime(5 * i));
                }

                l_Caster->ClearUnitState(UNIT_STATE_ROOT);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_inquisition_calcified_quills_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_inquisition_calcified_quills_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_inquisition_calcified_quills_AuraScript();
        }
};

/// Last Update: 7.3.5 26365
/// Called by Echoing Anguish - 233983
class spell_inquisition_echoing_anguis: public SpellScriptLoader
{
    public:
        spell_inquisition_echoing_anguis() : SpellScriptLoader("spell_inquisition_echoing_anguis") { }

        class spell_inquisition_echoing_anguis_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_inquisition_echoing_anguis_SpellScript);

            void SelectTarget(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                p_Targets.remove_if([](WorldObject* p_Object) -> bool
                {
                    if (!p_Object || !p_Object->IsPlayer())
                        return true;

                    return false;
                });

                if (InstanceScript* p_instance = l_Caster->GetInstanceScript())
                {
                    if (Unit* l_Atrigan = ObjectAccessor::GetUnit(*l_Caster, p_instance->GetData64(eCreatures::NpcAtrigan)))
                    {
                        if (Unit* I_Victim = l_Atrigan->getVictim())
                            p_Targets.remove(I_Victim);
                    }
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_inquisition_echoing_anguis_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_inquisition_echoing_anguis_SpellScript::SelectTarget, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_inquisition_echoing_anguis_SpellScript::SelectTarget, EFFECT_2, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        class spell_inquisition_echoing_anguis_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_inquisition_echoing_anguis_AuraScript);

            void HandleDispel(DispelInfo* dispelInfo)
            {
                Unit* l_Target = GetUnitOwner();
                if (!l_Target)
                    return;

                l_Target->CastSpell(l_Target, eSpells::EchoingAnguishTriggered, true);
            }

            void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Target = GetUnitOwner();
                if (!l_Target)
                    return;

                l_Target->CastSpell(l_Target, eSpells::EchoingAnguishTriggered, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_inquisition_echoing_anguis_AuraScript::HandleRemove, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
                AfterDispel += AuraDispelFn(spell_inquisition_echoing_anguis_AuraScript::HandleDispel);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_inquisition_echoing_anguis_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_inquisition_echoing_anguis_AuraScript();
        }
};

/// Last Update: 7.3.5 26365
/// Called by Suffocating Dark - 233895
class spell_inquisition_suffocating_dark : public SpellScriptLoader
{
    public:
        spell_inquisition_suffocating_dark() : SpellScriptLoader("spell_inquisition_suffocating_dark") { }

        class spell_inquisition_suffocating_dark_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_inquisition_suffocating_dark_SpellScript);

            void SelectTarget(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                ///< fixed 09/30/2018 report from forum
                p_Targets.remove_if([](WorldObject* p_Target) -> bool
                {
                    if (!p_Target || !p_Target->IsPlayer())
                        return true;

                    if (p_Target->ToPlayer()->GetRoleForGroup() == Roles::ROLE_DAMAGE || p_Target->ToPlayer()->GetRoleForGroup() == Roles::ROLE_TANK)
                        return true;

                    return false;
                });

                if (p_Targets.empty())
                    return;

                bool CheckAtrigan = false;
                if (InstanceScript* p_instance = l_Caster->GetInstanceScript())
                {
                    if (Unit* l_Atrigan = ObjectAccessor::GetUnit(*l_Caster, p_instance->GetData64(eCreatures::NpcAtrigan)))
                    {
                        p_Targets.sort(JadeCore::DistanceCompareOrderPred(l_Atrigan, false)); ///< fixed 09/20/2018 report from wowcircle
                        CheckAtrigan = true;
                        if (Unit* I_Victim = l_Atrigan->getVictim())
                            p_Targets.remove(I_Victim);
                    }
                }

                if (p_Targets.empty())
                    return;

                if (!CheckAtrigan)
                    p_Targets.sort(JadeCore::DistanceCompareOrderPred(l_Caster, false)); ///< fixed 09/20/2018 report from wowcircle

                if (p_Targets.size() > 3)
                    p_Targets.resize(3);
            }

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_Target = GetHitUnit();
                if (!l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::SuffocatingDarkAT, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_inquisition_suffocating_dark_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                AfterHit += SpellHitFn(spell_inquisition_suffocating_dark_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_inquisition_suffocating_dark_SpellScript();
        }
};

/// Last Update: 7.3.5 26365
/// Called by SuffocatingDark - 233894
class spell_at_inquisition_suffocating_dark : public AreaTriggerEntityScript
{
    public:
        spell_at_inquisition_suffocating_dark() : AreaTriggerEntityScript("spell_at_inquisition_suffocating_dark") {}

        std::list<uint64> m_Targets;
        uint32 m_CheckTimer = 500;
        float l_Radius = 9.0f;

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

                SpellInfo const* p_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::SuffocatingDarkDamage);
                if (!p_SpellInfo)
                    return;

                std::list<Player*> l_PlayerList;
                std::list<uint64> l_Targets(m_Targets);
                for (uint64 l_TargetGuid : l_Targets)
                {
                    Player* l_Player = ObjectAccessor::GetPlayer(*l_AreaTriggerCaster, l_TargetGuid);
                    if (!l_Player)
                        continue;

                    if (l_Player->HasAura(eSpells::SuffocatingDarkDamage) && l_Player->GetDistance(p_AreaTrigger) <= l_Radius)
                        continue;

                    if (l_Player->HasAura(eSpells::SuffocatingDarkDamage))
                        l_Player->RemoveAurasDueToSpell(eSpells::SuffocatingDarkDamage);

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
                            if (!l_Player || l_Player->IsImmunedToSpell(p_SpellInfo) || l_Player->HasAura(eSpells::SuffocatingDarkDamage))
                                continue;

                            l_Player->CastSpell(l_Player, eSpells::SuffocatingDarkDamage, true);
                            m_Targets.push_back(l_Player->GetGUID());
                        }
                    }
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

                if (l_Player->HasAura(eSpells::SuffocatingDarkDamage))
                    l_Player->RemoveAurasDueToSpell(eSpells::SuffocatingDarkDamage);

                m_Targets.remove(l_TargetGuid);
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_inquisition_suffocating_dark();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by The Legions Prisoner - 235305
class spell_inquisition_legions_prisoner: public SpellScriptLoader
{
    public:
        spell_inquisition_legions_prisoner() : SpellScriptLoader("spell_inquisition_legions_prisoner") { }

        class spell_inquisition_legions_prisoner_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_inquisition_legions_prisoner_AuraScript);

            void CalculateAmount(AuraEffect const* /*auraEffect*/, int32& amount, bool& /*canBeRecalculated*/)
            {
                amount = -1;
            }

            void Absorb(AuraEffect* /*auraEffect*/, DamageInfo& /*dmgInfo*/, uint32& /*absorbAmount*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                if (roll_chance_i(25))
                    l_Target->CastSpell(l_Target, eSpells::RemnantofHopeSummon, true);
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_inquisition_legions_prisoner_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_inquisition_legions_prisoner_AuraScript::Absorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_inquisition_legions_prisoner_AuraScript();
        }
};

class npc_inquisition_remnant_of_hope : public CreatureScript
{
    public:
        npc_inquisition_remnant_of_hope() : CreatureScript("npc_inquisition_remnant_of_hope") { }

        struct npc_inquisition_remnant_of_hopeAI : public ScriptedAI
        {
            npc_inquisition_remnant_of_hopeAI(Creature* p_Creature) : ScriptedAI(p_Creature) {}

            enum eNpcEvents
            {
                EventRemnantofHope = 1
            };

            EventMap m_Events;

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->AddUnitState(UNIT_STATE_ROOT);
                m_Events.Reset();
                m_Events.ScheduleEvent(eNpcEvents::EventRemnantofHope, 500);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                m_Events.Update(p_Diff);

                while (uint32 eventId = m_Events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eNpcEvents::EventRemnantofHope:
                        {
                            me->CastSpell(me, eSpells::RemnantofHopeAT, true);
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
            return new npc_inquisition_remnant_of_hopeAI(p_Creature);
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Remnant of Hope AreaTrigger - 241604
class spell_at_remnant_of_hope : public AreaTriggerEntityScript
{
    public:
        spell_at_remnant_of_hope() : AreaTriggerEntityScript("spell_at_remnant_of_hope") {}

        uint32 m_Timer = 500;
        float l_Radius = 1.5f;

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            {
                if (m_Timer <= p_Time)
                {
                    m_Timer = 500;

                    std::list<Player*> l_PlayerList;
                    JadeCore::AnyPlayerInObjectRangeCheck l_Check(p_AreaTrigger, l_Radius);
                    JadeCore::PlayerListSearcher<JadeCore::AnyPlayerInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_PlayerList, l_Check);
                    p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);

                    if (!l_PlayerList.empty())
                    {
                        l_PlayerList.remove_if(CheckDeadTarget());
                        for (Player* l_Player : l_PlayerList)
                        {
                            if (!l_Player)
                                continue;

                            int32 powerValue = irand(-50, -35);
                            l_Player->ModifyPower(POWER_ALTERNATE_POWER, powerValue);
                            p_AreaTrigger->Remove();
                            if (l_Caster->ToCreature())
                                l_Caster->ToCreature()->DespawnOrUnsummon();
                        }
                    }
                }
                else
                    m_Timer -= p_Time;
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_remnant_of_hope();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Torment - 233104
class spell_inquisition_torment : public SpellScriptLoader
{
    public:
        spell_inquisition_torment() : SpellScriptLoader("spell_inquisition_torment") { }

        class spell_inquisition_torment_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_inquisition_torment_SpellScript);

            void SelectTarget(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if(JadeCore::UnitAuraCheck(true, eSpells::TormentTrigger));
                p_Targets.remove_if([](WorldObject* p_Object) -> bool
                {
                    if (!p_Object || !p_Object->IsPlayer() || p_Object->ToPlayer()->IsBeingTeleportedFar() || !p_Object->ToPlayer()->GetSession() || p_Object->ToPlayer()->GetSession()->PlayerLoading())
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_inquisition_torment_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_inquisition_torment_SpellScript::SelectTarget, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        class spell_inquisition_torment_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_inquisition_torment_AuraScript);

            void PeriodicTick(AuraEffect const* /*aurEff*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                Player* l_Player = l_Target->ToPlayer();
                if (!l_Player)
                    return;

                int32 powerValue = l_Player->GetPower(POWER_ALTERNATE_POWER);
                if (powerValue == 0 && l_Player->HasAura(eSpells::BelacPrisoner))
                {
                    if (CheckPlayerInRoom(l_Player))
                    {
                        l_Player->RemoveAurasDueToSpell(eSpells::BelacPrisoner);
                        return;
                    }

                    if (!l_Player->HasAura(eSpells::RemnantofHopeAddedButton))
                        l_Player->CastSpell(l_Player, eSpells::RemnantofHopeAddedButton, true);
                }
                else if (powerValue >= 100 && CheckPlayerInRoom(l_Player) && !l_Player->HasAura(eSpells::UnbearableTorment) && !l_Player->HasAura(eSpells::BelacPrisoner))
                         l_Player->CastSpell(l_Player, eSpells::UnbearableTorment, true);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_inquisition_torment_AuraScript::PeriodicTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_inquisition_torment_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_inquisition_torment_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Confess - 234364
class spell_inquisition_confess : public SpellScriptLoader
{
    public:
        spell_inquisition_confess() : SpellScriptLoader("spell_inquisition_confess") { }

        class spell_inquisition_confess_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_inquisition_confess_SpellScript);

            void SelectTarget(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if(JadeCore::UnitAuraCheck(true, eSpells::Confess));
                p_Targets.remove_if(JadeCore::UnitAuraCheck(true, eSpells::RemnantofHopeAddedButton));
                p_Targets.remove_if([](WorldObject* p_Object) -> bool
                {
                    if (!p_Object || !p_Object->IsPlayer() || p_Object->ToPlayer()->IsBeingTeleportedFar() || !p_Object->ToPlayer()->GetSession() || p_Object->ToPlayer()->GetSession()->PlayerLoading() || !CheckPlayerInRoom(p_Object->ToPlayer()))
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_inquisition_confess_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_inquisition_confess_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by ConfessButton - 233652
class spell_inquisition_confess_button : public SpellScriptLoader
{
    public:
        spell_inquisition_confess_button() : SpellScriptLoader("spell_inquisition_confess_button") { }

        class spell_inquisition_confess_button_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_inquisition_confess_button_SpellScript);

            void HandleBeforeCast()
            {
                if (Unit* I_Caster = GetCaster())
                {
                    I_Caster->RemoveAurasDueToSpell(eSpells::Confess);
                    I_Caster->RemoveAurasDueToSpell(eSpells::SpontaneousFragmentationFixate);
                    I_Caster->RemoveAurasDueToSpell(eSpells::ConfessButton);
                    I_Caster->RemoveAurasDueToSpell(eSpells::UnbearableTorment);
                    I_Caster->RemoveAurasDueToSpell(eSpells::EchoingAnguish);
                    I_Caster->RemoveAurasDueToSpell(eSpells::SuffocatingDarkDamage);
                }
            }

            void HandleAfterCast()
            {
                if (Unit* I_Caster = GetCaster())
                {
                    I_Caster->RemoveAurasDueToSpell(eSpells::Confess);
                    I_Caster->RemoveAurasDueToSpell(eSpells::SpontaneousFragmentationFixate);
                    I_Caster->RemoveAurasDueToSpell(eSpells::ConfessButton);
                    I_Caster->RemoveAurasDueToSpell(eSpells::UnbearableTorment);
                    I_Caster->RemoveAurasDueToSpell(eSpells::EchoingAnguish);
                    I_Caster->RemoveAurasDueToSpell(eSpells::SuffocatingDarkDamage);

                    float _x, _y, _z;
                    InstanceScript* p_instance = I_Caster->GetInstanceScript();
                    if (!p_instance)
                        return;

                    if (Unit* l_Belac = ObjectAccessor::GetUnit(*I_Caster, p_instance->GetData64(eCreatures::NpcBelac)))
                    {
                        l_Belac->GetClosePoint(_x, _y, _z, 2.f, 4.f);
                        float speedZ = 20.f;
                        float speedXY = 50.f;

                        I_Caster->GetMotionMaster()->MoveJump(_x, _y, _z+10.f, speedXY, speedZ, 10.f, eSpells::ConfessButton, eSpells::ConfessTeleport);
                    }
                }
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_inquisition_confess_button_SpellScript::HandleBeforeCast);
                AfterCast += SpellCastFn(spell_inquisition_confess_button_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_inquisition_confess_button_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by The Legion's Prisoner - 247174
class spell_inquisition_legions_belac_prisoner : public SpellScriptLoader
{
    public:
        spell_inquisition_legions_belac_prisoner() : SpellScriptLoader("spell_inquisition_legions_belac_prisoner") { }

        class spell_inquisition_legions_belac_prisoner_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_inquisition_legions_belac_prisoner_SpellScript);

            void SelectTarget(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                float l_Dist = 50.f;
                p_Targets.remove_if([l_Caster, l_Dist](WorldObject* p_Target) -> bool
                {
                    if (!p_Target || !p_Target->IsPlayer() || p_Target->ToPlayer()->IsBeingTeleportedFar() || !p_Target->ToPlayer()->GetSession() || p_Target->ToPlayer()->GetSession()->PlayerLoading())
                        return true;

                    if (l_Caster->GetDistance(p_Target) > l_Dist)
                        return true;

                    return false;
                });

                p_Targets.remove_if(JadeCore::UnitAuraCheck(true, eSpells::BelacPrisoner));
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_inquisition_legions_belac_prisoner_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_inquisition_legions_belac_prisoner_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by ConfessTeleport - 233679
class spell_inquisition_confess_teleport : public SpellScriptLoader
{
    public:
        spell_inquisition_confess_teleport() : SpellScriptLoader("spell_inquisition_confess_teleport") { }

        class spell_inquisition_confess_teleport_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_inquisition_confess_teleport_SpellScript);

            void HandleBeforeCast()
            {
                if (Unit* I_Caster = GetCaster())
                {
                    I_Caster->RemoveAurasDueToSpell(eSpells::ConfessButton);
                    I_Caster->RemoveAurasDueToSpell(eSpells::SpontaneousFragmentationFixate);
                    I_Caster->CastSpell(I_Caster, eSpells::BelacPrisoner, true);
                    I_Caster->RemoveAurasDueToSpell(eSpells::UnbearableTorment);
                    I_Caster->RemoveAurasDueToSpell(eSpells::Confess);
                    I_Caster->RemoveAurasDueToSpell(eSpells::EchoingAnguish);
                    I_Caster->RemoveAurasDueToSpell(eSpells::SuffocatingDarkDamage);
                }
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_inquisition_confess_teleport_SpellScript::HandleBeforeCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_inquisition_confess_teleport_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Soul Corruption - 248713
class spell_inquisition_soul_corruption : public SpellScriptLoader
{
    public:
        spell_inquisition_soul_corruption() : SpellScriptLoader("spell_inquisition_soul_corruption") { }

        class spell_inquisition_soul_corruption_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_inquisition_soul_corruption_SpellScript);

            void SelectTarget(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                float l_Dist = 50.f;
                p_Targets.remove_if([l_Caster, l_Dist](WorldObject* p_Target) -> bool
                {
                    if (!p_Target || !p_Target->ToUnit())
                        return true;

                    if (l_Caster->GetDistance(p_Target) > l_Dist)
                        return true;

                    return false;
                });

                p_Targets.remove_if([](WorldObject* p_Object) -> bool
                {
                    if (!p_Object || !p_Object->IsPlayer() || p_Object->ToPlayer()->IsBeingTeleportedFar() || !p_Object->ToPlayer()->GetSession() || p_Object->ToPlayer()->GetSession()->PlayerLoading())
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_inquisition_soul_corruption_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_inquisition_soul_corruption_SpellScript::SelectTarget, EFFECT_1, TARGET_UNIT_DEST_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_inquisition_soul_corruption_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by RemnantofHopeButton - 235293
class spell_inquisition_remnant_of_hope_button : public SpellScriptLoader
{
    public:
        spell_inquisition_remnant_of_hope_button() : SpellScriptLoader("spell_inquisition_remnant_of_hope_button") { }

        class spell_inquisition_remnant_of_hope_button_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_inquisition_remnant_of_hope_button_SpellScript);

            void HandleBeforeCast()
            {
                if (Unit* I_Caster = GetCaster())
                {
                    I_Caster->RemoveAurasDueToSpell(eSpells::Confess);
                    I_Caster->RemoveAurasDueToSpell(eSpells::ConfessButton);
                    I_Caster->RemoveAurasDueToSpell(eSpells::BelacPrisoner);
                }
            }

            void HandleAfterCast()
            {
                if (Unit* I_Caster = GetCaster())
                {
                    I_Caster->RemoveAurasDueToSpell(eSpells::Confess);
                    I_Caster->RemoveAurasDueToSpell(eSpells::ConfessButton);
                    I_Caster->RemoveAurasDueToSpell(eSpells::BelacPrisoner);

                    InstanceScript* p_instance = I_Caster->GetInstanceScript();
                    if (!p_instance)
                        return;

                    if (Creature* l_Belac = ObjectAccessor::GetCreature(*I_Caster, p_instance->GetData64(eCreatures::NpcBelac)))
                        if (boss_belac::boss_belacAI* l_LinkAI = CAST_AI(boss_belac::boss_belacAI, l_Belac->GetAI()))
                            l_LinkAI->SetTeleportCenterRoomTarget(I_Caster->GetGUID());
                }
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_inquisition_remnant_of_hope_button_SpellScript::HandleBeforeCast);
                AfterCast += SpellCastFn(spell_inquisition_remnant_of_hope_button_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_inquisition_remnant_of_hope_button_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Rain of Brimstone - 238588
class spell_inquisition_calcified_quills_damage : public SpellScriptLoader
{
    public:
        spell_inquisition_calcified_quills_damage() : SpellScriptLoader("spell_inquisition_calcified_quills_damage") { }

        class spell_inquisition_calcified_quills_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_inquisition_calcified_quills_damage_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || p_Targets.empty())
                    return;

                Creature* l_Atrigan = l_Caster->ToCreature();
                if (!l_Atrigan)
                    return;

                p_Targets.remove_if([l_Atrigan](WorldObject* p_Target) -> bool
                {
                    if (Player* l_Target = p_Target->ToPlayer())
                        if (boss_atrigan::boss_atriganAI* l_LinkAI = CAST_AI(boss_atrigan::boss_atriganAI, l_Atrigan->GetAI()))
                            if (l_LinkAI->HasCalcifiedQuillsTarget(l_Target->GetGUID()))
                                return true;

                    return false;
                });
            }

            void HandleDamage(SpellEffIndex)
            {
                Unit* l_Caster = GetCaster();
                Unit* I_Target = GetHitUnit();
                if (!l_Caster || !I_Target)
                    return;

                Creature* l_Atrigan = l_Caster->ToCreature();
                if (!l_Atrigan)
                    return;

                if (boss_atrigan::boss_atriganAI* l_LinkAI = CAST_AI(boss_atrigan::boss_atriganAI, l_Atrigan->GetAI()))
                    l_LinkAI->SetCalcifiedQuillsTarget(I_Target->GetGUID());
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_inquisition_calcified_quills_damage_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_inquisition_calcified_quills_damage_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_inquisition_calcified_quills_damage_SpellScript::FilterTargets, EFFECT_2, TARGET_UNIT_DEST_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_inquisition_calcified_quills_damage_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_inquisition_calcified_quills_damage_SpellScript();
        }
};

/// Last Update 7.3.5 - 26365
/// Called by Fel Squall - 235236
class spell_inquisition_fel_squall_damage : public SpellScriptLoader
{
    public:
        spell_inquisition_fel_squall_damage() : SpellScriptLoader("spell_inquisition_fel_squall_damage") { }

        class spell_inquisition_fel_squall_damaget_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_inquisition_fel_squall_damaget_SpellScript);

            void HandleEffectHitTarget(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* I_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!I_Caster || !l_Target)
                    return;

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(GetSpellInfo()->Id, I_Caster->GetMap()->GetDifficultyID());
                if (!l_SpellInfo)
                    return;

                int32 l_Damage = l_SpellInfo->Effects[EFFECT_0].BasePoints;
                float l_ModDamagePercentTaken = l_Target->GetTotalAuraMultiplierByMiscMask(SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, l_SpellInfo->GetSchoolMask());
                l_Damage *= l_ModDamagePercentTaken;
                SetHitDamage(l_Damage);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_inquisition_fel_squall_damaget_SpellScript::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_inquisition_fel_squall_damaget_SpellScript;
        }
};

/// Last Update 7.3.5 - 26365
/// Called by Harrowing Reconstitution - 241521
class spell_inquisition_harrowing_reconstitution : public SpellScriptLoader
{
    public:
        spell_inquisition_harrowing_reconstitution() : SpellScriptLoader("spell_inquisition_harrowing_reconstitution") { }

        class spell_inquisition_harrowing_reconstitution_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_inquisition_harrowing_reconstitution_SpellScript);

            void CorrectTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr)
                        return true;

                    if (p_Object->GetEntry() != eMisc::NpcTormentedFragment)
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_inquisition_harrowing_reconstitution_SpellScript::CorrectTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_inquisition_harrowing_reconstitution_SpellScript::CorrectTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_inquisition_harrowing_reconstitution_SpellScript();
        }
};

class npc_inquisition_tormented_fragment : public CreatureScript
{
    public:
        npc_inquisition_tormented_fragment() : CreatureScript("npc_inquisition_tormented_fragment") { }

        struct npc_inquisition_tormented_fragmentAI : public ScriptedAI
        {
            npc_inquisition_tormented_fragmentAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
            }

            uint32 HarrowingReconstitution_Timer;

            Player* SelectRandomPlayer(Creature* l_Atrigan, float l_Radius)
            {
                Map* map = me->GetMap();
                if (!map->IsDungeon())
                    return nullptr;

                Map::PlayerList const &PlayerList = map->GetPlayers();
                if (PlayerList.isEmpty())
                    return nullptr;

                std::list<Player*> temp;
                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    if (l_Atrigan->getVictim() != i->getSource() && ///< check tank
                        me->IsWithinDistInMap(i->getSource(), l_Radius) && i->getSource()->isAlive() && !i->getSource()->isGameMaster() && !i->getSource()->IsActiveSpecTankSpec() && CheckPlayerInRoom(i->getSource()))
                        temp.push_back(i->getSource());

                if (!temp.empty())
                {
                    std::list<Player*>::const_iterator j = temp.begin();
                    advance(j, rand()%temp.size());
                    return (*j);
                }
                return nullptr;
            }

            void AddHarrowingReconstitution()
            {
                if (me->GetReactState() == ReactStates::REACT_PASSIVE)
                    return;

                uint8 l_Stack = 0;
                std::list<Creature*> l_Creatures;
                me->GetCreatureListWithEntryInGrid(l_Creatures, eMisc::NpcTormentedFragment, 100.f);
                if (l_Creatures.empty())
                    return;

                for (Creature* l_Creature : l_Creatures)
                {
                    if (!l_Creature->isAlive() || l_Creature->GetReactState() == ReactStates::REACT_PASSIVE)
                        continue;

                    ++l_Stack;
                }

                if (l_Stack < 2)
                    return;

                l_Stack = l_Stack - 1;
                me->SetAuraStack(eSpells::HarrowingReconstitution, me, l_Stack);
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoZoneInCombat(me, 250.0f);
                AddHarrowingReconstitution();

                if (InstanceScript* p_instance = me->GetInstanceScript())
                    if (Creature* l_Atrigan = Unit::GetCreature(*me, p_instance->GetData64(eCreatures::NpcAtrigan)))
                        if (Player* l_Player = SelectRandomPlayer(l_Atrigan, 100.f))
                            DoCast(l_Player, eSpells::SpontaneousFragmentationFixate);

                HarrowingReconstitution_Timer = 1 * IN_MILLISECONDS;
            }

            void DamageTaken(Unit* attacker, uint32 &damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (me->GetReactState() == ReactStates::REACT_PASSIVE)
                {
                    damage = 0;
                    return;
                }

                if (damage >= me->GetHealth() && me->GetReactState() != ReactStates::REACT_PASSIVE)
                {
                    me->SetReactState(ReactStates::REACT_PASSIVE);
                    me->RemoveAllAuras();
                    damage = 0;
                    me->GetMotionMaster()->Clear();
                    me->StopMoving();
                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_REMOVE_CLIENT_CONTROL);
                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);
                    me->AddUnitState(UnitState::UNIT_STATE_ROOT);
                    me->SetHealth(1);
                    me->CastSpell(me, eSpells::ExplosiveAnguish, false);
                    me->DespawnOrUnsummon(1600);
                }
            }

            void AddFixateTarget()
            {
                if (InstanceScript* p_instance = me->GetInstanceScript())
                {
                    if (Creature* l_Atrigan = Unit::GetCreature(*me, p_instance->GetData64(eCreatures::NpcAtrigan)))
                    {
                        if (Player* l_Player = SelectRandomPlayer(l_Atrigan, 100.f))
                        {
                            DoCast(l_Player, eSpells::SpontaneousFragmentationFixate);
                            AttackStart(l_Player);
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveChase(l_Player);
                        }
                    }
                }
            }

            bool UpdateVictim()
            {
                if (!me->isInCombat())
                    return false;

                if (!me->HasReactState(REACT_PASSIVE))
                {
                    if (Unit* l_Victim = me->SelectVictim())
                    {
                        if (l_Victim->GetDistance2d(6401.90f, -796.01f) > 60.f)
                        {
                            me->RemoveAurasByType(SPELL_AURA_MOD_TAUNT, l_Victim->GetGUID());
                            me->getThreatManager().resetAggro(ResetAggroSelector(l_Victim->GetGUID()));
                            l_Victim->RemoveAurasByType(SPELL_AURA_FIXATE, me->GetGUID());
                            me->RemoveAurasByType(SPELL_AURA_FIXATE, l_Victim->GetGUID());

                            AddFixateTarget();
                            return true;
                        }
                        else if (!l_Victim->HasAura(eSpells::SpontaneousFragmentationFixate))
                            AddFixateTarget();
                        else
                        {
                            AttackStart(l_Victim);
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveChase(l_Victim);
                        }
                    }
                    else
                        AddFixateTarget();

                    return me->getVictim();
                }
                else if (me->getThreatManager().isThreatListEmpty())
                {
                    me->GetMotionMaster()->Clear();
                    return false;
                }

                return true;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (HarrowingReconstitution_Timer <= p_Diff)
                {
                    HarrowingReconstitution_Timer = 1 * IN_MILLISECONDS;
                    AddHarrowingReconstitution();
                }
                else
                    HarrowingReconstitution_Timer -= p_Diff;

                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new npc_inquisition_tormented_fragmentAI(p_Creature);
        }
};

#ifndef __clang_analyzer__
void AddSC_demonic_inquisition()
{
    new boss_atrigan();
    new boss_belac();
    new npc_inquisition_tormented_soul();
    new spell_inquisition_bone_saw();
    new spell_inquisition_bone_saw_fixate();
    new spell_inquisition_calcified_quills();
    new spell_inquisition_echoing_anguis();
    new spell_inquisition_suffocating_dark();
    new spell_at_inquisition_suffocating_dark();
    new spell_inquisition_legions_prisoner();
    new npc_inquisition_remnant_of_hope();
    new spell_at_remnant_of_hope();
    new spell_inquisition_torment();
    new spell_inquisition_confess();
    new spell_inquisition_confess_button();
    new spell_inquisition_legions_belac_prisoner();
    new spell_inquisition_confess_teleport();
    new spell_inquisition_soul_corruption();
    new spell_inquisition_remnant_of_hope_button();
    new spell_inquisition_calcified_quills_damage();
    new spell_inquisition_fel_squall_damage();
    new spell_inquisition_harrowing_reconstitution();
    new npc_inquisition_tormented_fragment();
}
#endif
