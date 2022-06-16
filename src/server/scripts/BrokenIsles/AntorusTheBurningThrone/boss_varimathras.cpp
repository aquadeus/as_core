////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "antorus_the_burning_throne.hpp"
#include "AreaTrigger.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SpellAuras.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "Containers.h"

enum eSpells
{
    ///< General
    VarimathrasEnergyGain        = 244697, ///< used when call EnterCombat
    Berserk                      = 224264,
    ControlAuraAreaTrigger       = 243975, ///< not used but in siff
    ControlAura                  = 243957, ///< cast on Players and proc on MiseryAura
    ControlDistanceCheck         = 243959,
    AloneintheDarkness           = 243963, ///< used if they ever aren't within 8 yards of another player
    MiseryAura                   = 243961, ///< used when players a get dmg school shadow
    FlameConversation            = 250026,

    ///< Torments of the Shivarra
    TormentofFlamesAreaTrigger   = 243967,
    TormentofFlamesAura          = 243968,
    TormentofFrostAreaTrigger    = 243976,
    TormentofFrostAura           = 243977,
    TormentofFelAreaTrigger      = 243979,
    TormentofFelAura             = 243980,
    TormentofShadowsAreaTrigger  = 243974,
    TormentofShadowsAura         = 243973,

    ///< The Fallen Nathrezim
    ShadowStrikeNormal           = 257644, ///< used for NM and LFR when a have 100% energy
    DarkFissure                  = 243999,
    DarkFissureAreaTrigger       = 244003,
    DarkFissureAura              = 244005,
    DarkEruption                 = 244006, ///< used when remove DarkFissureAreaTrigger
    MarkedPreyAura               = 244042,
    MarkedPrey                   = 257156,
    MarkedPreyCylinder           = 244068,
    ShadowHunterDamage           = 244070,
    ShadowHunterKnockback        = 252105,

    ///<Heroic
    ShadowStrikeHeroic           = 243960, ///< used for HM and MM when a have 100% energy
    NecroticEmbrace              = 244093,
    NecroticEmbraceAura          = 244094,
    NecroticEmbraceTarget        = 244097,  ///< used when NecroticEmbraceAura remove

    ///< Creature spells
    AloneintheDarknessVisual     = 244326, ///< Visual effects for npc 122590
    MarkedPreyVisual             = 244522, ///< Visual effects npc 122643
    TempTormentFlameVisual       = 243969, ///< Visual effects npc 68553
    TempTormenFrostVisual        = 243970, ///< Visual effects npc 68553
    TempTormentFelVisual         = 243971, ///< Visual effects npc 68553
    TempTormentShadowVisual      = 243972, ///< Visual effects npc 68553

    ///< Mythic
    EchoesofDoomAreaTrigger      = 248725, ///<!!! Need customEntry (in sniff not)
    EchoesofDoom                 = 248732,
    EchoesofDoomAura             = 248740
};

enum eEvents
{
    EventTormentofFlames = 1,
    EventTormentofFrost,
    EventTormentofFel,
    EventTormentofShadow,
    EventDarkFissure,
    EventMarkedPrey,
    EventNecroticEmbrace,
    EventBerserk,
    EventCloseBerserk
};

enum eMisc
{
    AnimKitIDShadowofVarimathras = 13242, ///< only for npc 122590
    KitRecIDShadowofVarimathras  = 83768, ///< only for npc 122590
    NpcTrigger = 68553,
    GobjectAct1 = 272716,
    GobjectAct2 = 272717,
    GobjectAct3 = 272718,
    GobjectAct4 = 272719,
    GobjectAct5 = 272720,
    GobjectAct6 = 272721,
    GobjectAct7 = 272722,
    GobjectAct8 = 272723
};

enum eAchievVarimathras
{
    AchievementMythicVarimathras      = 11999
};

enum eTalks
{
    TALK_AGGRO = 0,
    TALK_CAST_TORMENTOFFLAMES,
    TALK_CAST_MARKEDPREY,
    TALK_CAST_NECROTICEMBRACE,
    TALK_CAST_DARKFISSURE,
    TALK_CAST_TORMENTOFFROST,
    TALK_CAST_TORMENTOFSHADOWS,
    TALK_CAST_BERSERK,
    TALK_SLAY,
    TALK_WIPE,
    TALK_DIE,
    TALK_CAST_ECHOESOFDOOM,
    TALK_INTRO_ALLIANCE1 = 12,
    TALK_INTRO_ALLIANCE2 = 13,
    TALK_INTRO_ALLIANCE3 = 14,
    TALK_INTRO_HORDE1 = 15,
    TALK_INTRO_HORDE2 = 16,
    TALK_INTRO_HORDE3 = 17,
    TALK_DIIMA_CLOSE_BERSERK = 7,
    TALK_ASARA_OUTRO = 7
};

Position const m_PosCenterRoom = { -12634.19f, -2819.49f, 2438.84f, 0.f };

const Position g_TriggerPos[8] =
{
    { -12661.71f, -2847.691f, 2441.225f, 0.6027202f },
    { -12634.38f, -2858.439f, 2441.225f, 1.537677f  },
    { -12606.86f, -2847.358f, 2441.225f, 2.296307f  },
    { -12661.82f, -2792.715f, 2441.225f, 5.432556f  },
    { -12596.19f, -2819.693f, 2441.225f, 3.019302f  },
    { -12673.06f, -2819.97f, 2441.225f, 6.096372f   },
    { -12606.96f, -2792.526f, 2441.225f, 3.862372f  },
    { -12634.62f, -2781.116f, 2441.225f, 4.513496f  }
};

class CheckDeadOrGmTarget
{
    public:
        bool operator()(WorldObject* target) const
        {
            if (Unit* unit = target->ToUnit())
                return (!unit->isAlive() || (unit->IsPlayer() && unit->ToPlayer()->isGameMaster()));
            return false;
        }
};

class boss_varimathras : public CreatureScript
{
    public:
        boss_varimathras() : CreatureScript("boss_varimathras") { }

        struct boss_varimathrasAI : public BossAI
        {
            boss_varimathrasAI(Creature* creature) : BossAI(creature, eData::DataVarimathras)
            {
                ApplyAllImmunities(true);
                m_Intro = false;
                m_countAloneInDarkness = 0;
            }

            uint32 m_countAloneInDarkness;
            bool m_Intro;

            bool IgnoreFlyingMovementFlagsUpdate() override
            {
                return true;
            }

            bool CanFly() override
            {
                return false;
            }

            bool IsEasyDifficulty()
            {
                return (IsNormal() || IsLFR());
            }

            void GetDamageMultiplier(float& p_DamageMulti) override
            {
                if (IsHeroic() || IsMythic())
                    p_DamageMulti = 17.f;
                else if (IsLFR())
                    p_DamageMulti = 15.f;
                else
                    p_DamageMulti = 20.f;
            }

            void ClearOther()
            {
                me->DespawnAreaTriggersInArea({ eSpells::EchoesofDoomAreaTrigger, eSpells::DarkFissureAreaTrigger, eSpells::TormentofFlamesAreaTrigger, eSpells::TormentofFrostAreaTrigger, eSpells::TormentofFelAreaTrigger, eSpells::TormentofShadowsAreaTrigger }, 200.f, false);

                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::ControlAura);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::MiseryAura);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::TormentofFlamesAura);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::TormentofFrostAura);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::TormentofFelAura);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::TormentofShadowsAura);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::DarkFissureAura);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::MarkedPreyAura);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::NecroticEmbraceAura);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::EchoesofDoomAura);
            }

            void Reset() override
            {
                _Reset();
                me->AddAura(SpellAntorusNerf, me);
                ClearOther();
                SetFlyMode(false);
                me->SetReactState(REACT_DEFENSIVE);
                me->SetPower(POWER_ENERGY, 0);
                ClearDelayedOperations();
                m_countAloneInDarkness = 0;
            }

            void SetAloneInTheDarkness(Unit* l_Caster, bool l_Marked = false)
            {
                if (!l_Marked && l_Caster->ToPlayer()->HasSpellCooldown(eSpells::AloneintheDarkness))
                    return;

                if (!l_Marked && GetData(0) == 0)
                    SetData(0, 1);

                Position l_Pos;
                l_Caster->GetNearPosition(l_Pos, 3.0f, M_PI); ///< back

                if (!IsMythic())
                {
                    if (!l_Marked)
                        me->SummonCreature(eCreatures::NpcShadowofVarimathras, l_Pos, TEMPSUMMON_TIMED_DESPAWN, 400);
                }
                else
                    me->SummonCreature(eCreatures::NpcShadowofVarimathras2, l_Pos, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1800000);

                if (!l_Marked)
                {
                    me->DelayedCastSpell(l_Caster, eSpells::AloneintheDarkness, true, 300);
                    l_Caster->ToPlayer()->AddSpellCooldown(eSpells::AloneintheDarkness, 0, 5 * IN_MILLISECONDS);
                }
            }

            void SetMarkedPrey(Unit* pTarget)
            {
                me->SetFacingTo(me->GetAngle(pTarget));

                std::list<Unit*> l_Targets = me->GetThreatUnitsList(TypeID::TYPEID_PLAYER);

                l_Targets.remove_if([this, l_Cmp = CheckDeadOrGmTarget()](Unit* p_Target) -> bool
                {
                    if (!p_Target || l_Cmp(p_Target) || !me->HasInLine(p_Target, 3.0f, false))
                        return true;

                    return false;
                });

                if (l_Targets.empty())
                {
                    me->CastSpell(pTarget, eSpells::ShadowHunterDamage, true);
                    me->CastSpell(pTarget, eSpells::ShadowHunterKnockback, true);
                    SetAloneInTheDarkness(pTarget, true);
                    return;
                }
                else
                {
                    l_Targets.sort(JadeCore::DistanceCompareOrderPred(me));

                    if (l_Targets.size() > 1)
                        l_Targets.resize(1);

                    for (Unit* l_Player : l_Targets)
                    {
                        me->CastSpell(l_Player, eSpells::ShadowHunterDamage, true);
                        me->CastSpell(l_Player, eSpells::ShadowHunterKnockback, true);
                        SetAloneInTheDarkness(l_Player, true);
                    }
                }
            }

            void GobjEvent(bool l_Ended, uint32 l_SpellId = 0)
            {
                if (!l_Ended)
                {
                    for (uint8 l_Itr = 0; l_Itr < 8; ++l_Itr)
                        if (Creature* l_Trigger = me->SummonCreature(eMisc::NpcTrigger, g_TriggerPos[l_Itr], TEMPSUMMON_TIMED_DESPAWN, 10000))
                            l_Trigger->CastSpell(l_Trigger, l_SpellId, true);
                }

                std::list<GameObject*> l_ObjectList;
                for (uint32 l_I = eMisc::GobjectAct1; l_I < eMisc::GobjectAct8 + 1; l_I++)
                    me->GetGameObjectListWithEntryInGrid(l_ObjectList, l_I, 100.0f);

                for (GameObject* l_Obj : l_ObjectList)
                {
                    if (!l_Ended)
                        l_Obj->SetGoState(GO_STATE_ACTIVE);
                    else
                        l_Obj->SetGoState(GO_STATE_READY);
                }
            }

            void EnterEvadeMode() override
            {
                GobjEvent(true);
                Talk(eTalks::TALK_WIPE);
                ClearDelayedOperations();
                BossAI::EnterEvadeMode();
                ClearOther();
                instance->AddTimedDelayedOperation(2500, [this]() -> void
                {
                    ClearOther();
                });
            }

            void JustReachedHome() override
            {
                _JustReachedHome();
            }

            void EnterCombat(Unit* attacker) override
            {
                Talk(eTalks::TALK_AGGRO);
                _EnterCombat();
                me->SetReactState(REACT_AGGRESSIVE);
                events.ScheduleEvent(eEvents::EventTormentofFlames, 5 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventDarkFissure, 17400);
                events.ScheduleEvent(eEvents::EventMarkedPrey, 25200);
                events.ScheduleEvent(eEvents::EventBerserk, 310 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventCloseBerserk, 290 * IN_MILLISECONDS);

                if (!IsEasyDifficulty())
                    events.ScheduleEvent(eEvents::EventNecroticEmbrace, 35 * IN_MILLISECONDS);

                me->RemoveAurasDueToSpell(eSpells::VarimathrasEnergyGain);
                me->AddAura(eSpells::VarimathrasEnergyGain, me);
                AddTimedDelayedOperation(3000, [this]() -> void
                {
                    instance->DoCastSpellOnPlayers(eSpells::ControlAura, nullptr, false, true);
                });

                GobjEvent(false, eSpells::TempTormentFlameVisual);
                AddTimedDelayedOperation(10000, [this]() -> void ///< Deactived after 10 second when enter combat
                {
                    GobjEvent(true);
                });
            }

            void JustDied(Unit* p_Killer) override
            {
                if (Creature* l_Asara = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcAsaraMotherOfNight)))
                    if (l_Asara->AI())
                        l_Asara->AI()->Talk(eTalks::TALK_ASARA_OUTRO);

                Talk(eTalks::TALK_DIE);

                if (IsMythic())
                    instance->DoCompleteAchievement(eAchievVarimathras::AchievementMythicVarimathras);

                _JustDied();
                ClearOther();
                instance->AddTimedDelayedOperation(2500, [this]() -> void
                {
                    ClearOther();
                });
            }

            uint32 GetData(uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case 0:
                        return m_countAloneInDarkness;
                    default:
                        break;
                }

                return 0;
            }

            void SetData(uint32 p_ID, uint32 p_Value) override
            {
                switch (p_ID)
                {
                    case 0:
                    {
                        m_countAloneInDarkness = p_Value;
                        break;
                    }
                }
            }

            void KilledUnit(Unit* p_Who) override
            {
                if (p_Who->IsPlayer())
                    Talk(eTalks::TALK_SLAY);
            }

            void JustSummoned(Creature* p_Summon) override
            {
                BossAI::JustSummoned(p_Summon);
                switch (p_Summon->GetEntry())
                {
                    case eCreatures::NpcShadowofVarimathras:
                    {
                        p_Summon->CastSpell(p_Summon, eSpells::AloneintheDarknessVisual, true);
                        p_Summon->StopAttack();
                        p_Summon->SetReactState(REACT_PASSIVE);
                        p_Summon->PlayOneShotAnimKitId(eMisc::AnimKitIDShadowofVarimathras);
                        p_Summon->SendPlaySpellVisualKit(eMisc::KitRecIDShadowofVarimathras, 0);
                        break;
                    }
                    case eCreatures::NpcShadowofVarimathras2:
                    {
                        p_Summon->CastSpell(p_Summon, eSpells::MarkedPreyVisual, true);
                        p_Summon->PlayOneShotAnimKitId(eMisc::AnimKitIDShadowofVarimathras);
                        p_Summon->SendPlaySpellVisualKit(eMisc::KitRecIDShadowofVarimathras, 0);
                    }
                    default:
                        break;
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::MarkedPreyCylinder:
                    {
                        me->CastSpell(p_Target, eSpells::ShadowHunterKnockback, true);
                        me->CastSpell(p_Target, eSpells::ShadowHunterDamage, true);
                        break;
                    }
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::MarkedPreyCylinder:
                    {
                        AddTimedDelayedOperation(IN_MILLISECONDS, [this]() -> void
                        {
                            me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);
                        });

                        break;
                    }

                    default:
                        break;
                }
            }

            void OnSpellFinishedSuccess(SpellInfo const* p_SpellInfo) override
            {
                if (!p_SpellInfo)
                    return;

                switch (p_SpellInfo->Id)
                {
                    case eSpells::DarkFissure:
                    {
                        std::list<Unit*> l_PlayerCasterList = me->GetThreatUnitsList(TypeID::TYPEID_PLAYER);

                        l_PlayerCasterList.remove_if([l_Cmp = CheckDeadOrGmTarget()](Unit* p_Target) -> bool
                        {
                            if (!p_Target || p_Target->ToPlayer()->IsActiveSpecTankSpec() || l_Cmp(p_Target))
                                return true;

                            return false;
                        });

                        ///< On Retail 3 targets (2 casters and 1 melee)
                        std::list<Unit*> l_PlayerMeleeList = l_PlayerCasterList;

                        l_PlayerCasterList.sort(JadeCore::DistanceCompareOrderPred(me, false));
                        l_PlayerMeleeList.sort(JadeCore::DistanceCompareOrderPred(me));

                        if (l_PlayerCasterList.size() > 2)
                            l_PlayerCasterList.resize(2);

                        if (l_PlayerMeleeList.size() > 1)
                            l_PlayerMeleeList.resize(1);

                        std::unordered_set<Unit*> targetList;
                        targetList.insert(l_PlayerCasterList.begin(), l_PlayerCasterList.end());
                        targetList.insert(l_PlayerMeleeList.begin(), l_PlayerMeleeList.end());

                        uint32 l_Timer = 0;
                        for (auto l_Player : targetList)
                        {
                            me->DelayedCastSpell(l_Player, eSpells::DarkFissureAreaTrigger, true, l_Timer);
                            l_Timer += 350;
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void ExecuteEvent(uint32 const p_EventId) override
            {
                switch (p_EventId)
                {
                    case eEvents::EventTormentofFlames:
                    {
                        DoCast(FlameConversation);
                        me->DespawnAreaTriggersInArea({ eSpells::TormentofFlamesAreaTrigger, eSpells::TormentofFrostAreaTrigger, eSpells::TormentofFelAreaTrigger, eSpells::TormentofShadowsAreaTrigger }, 200.0f);
                        if (!IsEasyDifficulty())
                            events.ScheduleEvent(eEvents::EventTormentofFrost, 100 * IN_MILLISECONDS);
                        else
                            events.ScheduleEvent(eEvents::EventTormentofShadow, 290 * IN_MILLISECONDS);

                        DoCast(m_PosCenterRoom, eSpells::TormentofFlamesAreaTrigger, true);
                        break;
                    }
                    case eEvents::EventTormentofFrost:
                    {
                        Conversation* l_Conversation = new Conversation;
                        if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), 5656, me, nullptr, *me))
                            delete l_Conversation;

                        GobjEvent(false, eSpells::TempTormenFrostVisual);
                        AddTimedDelayedOperation(10000, [this]() -> void
                        {
                            GobjEvent(true);
                        });

                        me->DespawnAreaTriggersInArea({ eSpells::TormentofFlamesAreaTrigger, eSpells::TormentofFrostAreaTrigger, eSpells::TormentofFelAreaTrigger, eSpells::TormentofShadowsAreaTrigger }, 200.0f);
                        DoCast(m_PosCenterRoom, eSpells::TormentofFrostAreaTrigger, true);
                        events.ScheduleEvent(eEvents::EventTormentofFel, 99 * IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventTormentofFel:
                    {
                        GobjEvent(false, eSpells::TempTormentFelVisual);
                        AddTimedDelayedOperation(10000, [this]() -> void
                        {
                            GobjEvent(true);
                        });

                        me->DespawnAreaTriggersInArea({ eSpells::TormentofFlamesAreaTrigger, eSpells::TormentofFrostAreaTrigger, eSpells::TormentofFelAreaTrigger, eSpells::TormentofShadowsAreaTrigger }, 200.0f);
                        DoCast(m_PosCenterRoom, eSpells::TormentofFelAreaTrigger, true);
                        events.ScheduleEvent(eEvents::EventTormentofShadow, 90 * IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventTormentofShadow:
                    {
                        Conversation* l_Conversation = new Conversation;
                        if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), 5657, me, nullptr, *me))
                            delete l_Conversation;

                        GobjEvent(false, eSpells::TempTormentShadowVisual);
                        AddTimedDelayedOperation(10000, [this]() -> void
                        {
                            GobjEvent(true);
                        });

                        me->DespawnAreaTriggersInArea({ eSpells::TormentofFlamesAreaTrigger, eSpells::TormentofFrostAreaTrigger, eSpells::TormentofFelAreaTrigger, eSpells::TormentofShadowsAreaTrigger }, 200.0f);
                        DoCast(m_PosCenterRoom, eSpells::TormentofShadowsAreaTrigger, true);
                        break;
                    }
                    case eEvents::EventDarkFissure:
                    {
                        Talk(eTalks::TALK_CAST_DARKFISSURE);
                        DoCastVictim(eSpells::DarkFissure);
                        events.ScheduleEvent(eEvents::EventDarkFissure, 32 * IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventMarkedPrey:
                    {
                        Talk(eTalks::TALK_CAST_MARKEDPREY);
                        DoCast(eSpells::MarkedPrey, true);
                        DoCast(eSpells::MarkedPreyAura);
                        events.ScheduleEvent(eEvents::EventMarkedPrey, 32 * IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventNecroticEmbrace:
                    {
                        Talk(eTalks::TALK_CAST_NECROTICEMBRACE);
                        DoCast(eSpells::NecroticEmbrace);
                        events.ScheduleEvent(eEvents::EventNecroticEmbrace, 30 * IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventBerserk:
                    {
                        Talk(eTalks::TALK_CAST_BERSERK);
                        DoCast(eSpells::Berserk);
                        break;
                    }
                    case EventCloseBerserk:
                    {
                        if (Creature* l_Diima = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcDiimaMotherofGloom)))
                            if (l_Diima->AI())
                                l_Diima->AI()->Talk(eTalks::TALK_DIIMA_CLOSE_BERSERK);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
                if (!m_Intro)
                {
                    if (Player* p_Player = me->SelectNearestPlayerNotGM(60.0f))
                    {
                        if (p_Player->GetTeam() == ALLIANCE)
                        {
                            Talk(eTalks::TALK_INTRO_ALLIANCE1);
                            DelayTalk(18, eTalks::TALK_INTRO_ALLIANCE2);
                            DelayTalk(37, eTalks::TALK_INTRO_ALLIANCE3);
                        }
                        else
                        {
                            Talk(eTalks::TALK_INTRO_HORDE1);
                            DelayTalk(9, eTalks::TALK_INTRO_HORDE2);
                            DelayTalk(18, eTalks::TALK_INTRO_HORDE3);
                        }

                        m_Intro = true;
                    }
                }
                if (!UpdateVictim())
                    return;

                if (me->GetExactDist2d(&m_PosCenterRoom) > 50.0f)
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (me->GetPower(Powers::POWER_ENERGY) >= 100)
                {
                    if (IsEasyDifficulty())
                        DoCastVictim(eSpells::ShadowStrikeNormal);
                    else
                        DoCast(eSpells::ShadowStrikeHeroic); ///< AOE

                    me->resetAttackTimer();
                    me->SetPower(POWER_ENERGY, 0);
                    me->RemoveAurasDueToSpell(eSpells::VarimathrasEnergyGain);
                    me->AddAura(eSpells::VarimathrasEnergyGain, me);
                }

                while (uint32 l_EventId = events.ExecuteEvent())
                {
                    ExecuteEvent(l_EventId);

                    if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                        return;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new boss_varimathrasAI(p_Creature);
        }
};

/// Last Update 7.3.5 Build 26365
/// Varimathras Energy gain - 244697
class spell_varimathras_energy_gain: public SpellScriptLoader
{
    public:
        spell_varimathras_energy_gain() : SpellScriptLoader("spell_varimathras_energy_gain") { }

        class spell_varimathras_energy_gain_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_varimathras_energy_gain_AuraScript);

            float l_Energy;

            bool Load()
            {
                l_Energy = 0.0f;
                return true;
            }

            void HandleOnPeriodic(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || l_Caster->getPowerType() != POWER_ENERGY)
                    return;

                l_Energy += 5.5f;
                l_Caster->SetPower(POWER_ENERGY, int32(l_Energy));
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_varimathras_energy_gain_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_varimathras_energy_gain_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// DistanceCheck - 243959
class spell_varimathras_control_distance_check : public SpellScriptLoader
{
    public:
        spell_varimathras_control_distance_check() : SpellScriptLoader("spell_varimathras_control_distance_check") { }

        class spell_varimathras_control_distance_check_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_varimathras_control_distance_check_SpellScript);

            void CountTargets(std::list<WorldObject*>& p_Targets)
            {
                l_CountTargets = int32(p_Targets.size());
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (l_CountTargets || !l_Caster || !l_Caster->IsPlayer())
                    return;

                InstanceScript* l_Instance = l_Caster->GetInstanceScript();
                if (!l_Instance || l_Instance->GetBossState(eData::DataVarimathras) != IN_PROGRESS)
                    return;

                Creature* l_Varimathras = Creature::GetCreature(*l_Caster, l_Instance->GetData64(eCreatures::NpcVarimathras));
                if (!l_Varimathras)
                    return;

                if (boss_varimathras::boss_varimathrasAI* l_LinkAI = CAST_AI(boss_varimathras::boss_varimathrasAI, l_Varimathras->GetAI()))
                    l_LinkAI->SetAloneInTheDarkness(l_Caster, false);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_varimathras_control_distance_check_SpellScript::CountTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
                AfterCast += SpellCastFn(spell_varimathras_control_distance_check_SpellScript::HandleAfterCast);
            }

         private:
            int32 l_CountTargets;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_varimathras_control_distance_check_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Control Aura - 243957
class spell_varimathras_control_aura : public SpellScriptLoader
{
    public:
        spell_varimathras_control_aura() : SpellScriptLoader("spell_varimathras_control_aura") { }

        class spell_varimathras_control_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_varimathras_control_aura_AuraScript);

            bool HandleDoCheckProc(ProcEventInfo& p_EventInfo)
            {
                return false;
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_varimathras_control_aura_AuraScript::HandleDoCheckProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_varimathras_control_aura_AuraScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Marked Prey - 244042
class spell_varimathras_marked_prey_aura : public SpellScriptLoader
{
    public:
        spell_varimathras_marked_prey_aura() : SpellScriptLoader("spell_varimathras_marked_prey_aura") { }

        class spell_varimathras_marked_prey_aura_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_varimathras_marked_prey_aura_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (Unit* I_Victim = l_Caster->getVictim())
                    p_Targets.remove(I_Victim);

                p_Targets.remove_if(CheckDeadOrGmTarget());
                p_Targets.remove_if([](WorldObject* p_Target) -> bool
                {
                    if (!p_Target->IsPlayer() || p_Target->ToPlayer()->IsActiveSpecTankSpec())
                        return true;

                    return false;
                });

                p_Targets.sort(JadeCore::DistanceCompareOrderPred(l_Caster));
                if (p_Targets.size() > 1)
                    JadeCore::RandomResizeList(p_Targets, 1);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_varimathras_marked_prey_aura_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        class spell_varimathras_marked_prey_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_varimathras_marked_prey_aura_AuraScript);

            void HandleAfterRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                InstanceScript* l_Instance = l_Target->GetInstanceScript();
                if (!l_Instance || l_Instance->GetBossState(eData::DataVarimathras) != IN_PROGRESS)
                    return;

                Creature* l_Varimathras = Creature::GetCreature(*l_Target, l_Instance->GetData64(eCreatures::NpcVarimathras));
                if (!l_Varimathras || !l_Varimathras->AI())
                    return;

                l_Varimathras->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                l_Varimathras->SetFacingTo(l_Varimathras->GetAngle(l_Target));
                l_Instance->AddTimedDelayedOperation(100, [l_Varimathras, l_Target]() -> void
                {
                    if (boss_varimathras::boss_varimathrasAI* l_LinkAI = CAST_AI(boss_varimathras::boss_varimathrasAI, l_Varimathras->GetAI()))
                        l_LinkAI->SetMarkedPrey(l_Target);
                });

                l_Instance->AddTimedDelayedOperation(300, [l_Varimathras]() -> void
                {
                    l_Varimathras->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                });
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_varimathras_marked_prey_aura_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_varimathras_marked_prey_aura_AuraScript();
        }

        SpellScript* GetSpellScript() const override
        {
            return new spell_varimathras_marked_prey_aura_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Necrotic Embrace - 244093
class spell_varimathras_necrotic_embrace : public SpellScriptLoader
{
    public:
        spell_varimathras_necrotic_embrace() : SpellScriptLoader("spell_varimathras_necrotic_embrace") { }

        class spell_varimathras_necrotic_embrace_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_varimathras_necrotic_embrace_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                p_Targets.remove_if(CheckDeadOrGmTarget());
                p_Targets.remove_if([](WorldObject* p_Target) -> bool
                {
                    if (!p_Target->IsPlayer() || p_Target->ToPlayer()->IsActiveSpecTankSpec())
                        return true;

                    return false;
                });

                if (p_Targets.size() >= 2)
                    JadeCore::Containers::RandomResizeList(p_Targets, l_Caster->GetMap()->IsMythic() ? 2 : 1);
            }

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::NecroticEmbraceAura, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_varimathras_necrotic_embrace_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                AfterHit += SpellHitFn(spell_varimathras_necrotic_embrace_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_varimathras_necrotic_embrace_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Necrotic Embrace 244094
class spell_varimathras_necrotic_embrace_aura : public SpellScriptLoader
{
    public:
        spell_varimathras_necrotic_embrace_aura() : SpellScriptLoader("spell_varimathras_necrotic_embrace_aura") { }

        class spell_varimathras_necrotic_embrace_aura_SpellScript : public AuraScript
        {
            PrepareAuraScript(spell_varimathras_necrotic_embrace_aura_SpellScript);

            void HandleAfterRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                InstanceScript* l_Instance = l_Target->GetInstanceScript();
                if (!l_Instance || l_Instance->GetBossState(eData::DataVarimathras) != IN_PROGRESS)
                    return;

                l_Target->CastSpell(l_Target, eSpells::NecroticEmbraceTarget, true);
            }

            void HandleDispel(DispelInfo* dispelInfo)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                InstanceScript* l_Instance = l_Target->GetInstanceScript();
                if (!l_Instance || l_Instance->GetBossState(eData::DataVarimathras) != IN_PROGRESS)
                    return;

                l_Target->CastSpell(l_Target, eSpells::NecroticEmbraceTarget, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_varimathras_necrotic_embrace_aura_SpellScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
                AfterDispel += AuraDispelFn(spell_varimathras_necrotic_embrace_aura_SpellScript::HandleDispel);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_varimathras_necrotic_embrace_aura_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Necrotic Embrace - 244097
class spell_varimathras_necrotic_embrace_target : public SpellScriptLoader
{
    public:
        spell_varimathras_necrotic_embrace_target() : SpellScriptLoader("spell_varimathras_necrotic_embrace_target") { }

        class spell_varimathras_necrotic_embrace_target_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_varimathras_necrotic_embrace_target_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                p_Targets.remove_if([l_Caster](WorldObject* p_Target) -> bool
                {
                    if (!p_Target || l_Caster->GetExactDist2d(p_Target) > 8.0f)
                        return true;

                    return false;
                });
            }

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Target = GetHitUnit();
                SpellInfo const* p_SpellInfo = GetSpellInfo();
                if (!l_Target || !p_SpellInfo || l_Target->HasAura(eSpells::NecroticEmbraceAura))
                    return;

                InstanceScript* l_Instance = l_Target->GetInstanceScript();
                if (!l_Instance || l_Instance->GetBossState(eData::DataVarimathras) != IN_PROGRESS)
                    return;

                l_Target->CastSpell(l_Target, eSpells::NecroticEmbraceAura, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_varimathras_necrotic_embrace_target_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_varimathras_necrotic_embrace_target_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_varimathras_necrotic_embrace_target_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Dark Fissure - 244003
class spell_at_varimathras_dark_fissure : public AreaTriggerEntityScript
{
    public:
        spell_at_varimathras_dark_fissure() : AreaTriggerEntityScript("spell_at_varimathras_dark_fissure") {}

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
        {
            if (!p_AreaTrigger)
                return;

            auto l_AreaTriggerCaster = p_AreaTrigger->GetCaster();
            if (!l_AreaTriggerCaster)
                return;

            InstanceScript* p_instance = l_AreaTriggerCaster->GetInstanceScript();
            if (!p_instance)
                return;

            l_AreaTriggerCaster->CastSpell(*p_AreaTrigger, eSpells::DarkEruption, true);
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_varimathras_dark_fissure();
        }
};

/// Last Update: 7.3.5 build 26365
/// MiseryAura 243961
/// Called by 243973, 244094, 244005, 244006, 244070, 243960, 257644, 243963
class spell_varimathras_misery_proc : public SpellScriptLoader
{
public:
    spell_varimathras_misery_proc() : SpellScriptLoader("spell_varimathras_misery_proc") { }

    class spell_varimathras_misery_proc_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_varimathras_misery_proc_AuraScript);

        void OnPeriodic(AuraEffect const* /*p_AuraEffect*/)
        {
            Unit* l_Target = GetTarget();
            if (!l_Target || !l_Target->GetMap() || l_Target->GetMap()->IsLFR())
                return;

            l_Target->CastSpell(l_Target, eSpells::MiseryAura, true);
        }

        void Register() override
        {
            if (m_scriptSpellId == eSpells::TormentofShadowsAura || m_scriptSpellId == eSpells::NecroticEmbraceAura || m_scriptSpellId == eSpells::DarkFissureAura)
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_varimathras_misery_proc_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
        }
    };

    class spell_varimathras_misery_proc_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_varimathras_misery_proc_SpellScript);

        void HandleDamage(SpellEffIndex /*effIndex*/)
        {
            Unit* l_Target = GetHitUnit();
            if (!l_Target || !l_Target->GetMap() || l_Target->GetMap()->IsLFR())
                return;

            l_Target->CastSpell(l_Target, eSpells::MiseryAura, true);
        }

        void Register() override
        {
            if (m_scriptSpellId == eSpells::DarkEruption || m_scriptSpellId == eSpells::ShadowHunterDamage || m_scriptSpellId == eSpells::ShadowStrikeHeroic || m_scriptSpellId == eSpells::ShadowStrikeNormal || m_scriptSpellId == eSpells::AloneintheDarkness)
                OnEffectHitTarget += SpellEffectFn(spell_varimathras_misery_proc_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_varimathras_misery_proc_SpellScript();
    }

    AuraScript* GetAuraScript() const override
    {
        return new spell_varimathras_misery_proc_AuraScript();
    }
};

/// Last Update: 7.3.5 build 26365
/// Torment of Flames  - 243967
class spell_at_varimathras_torment_of_flames : public AreaTriggerEntityScript
{
    public:
        spell_at_varimathras_torment_of_flames() : AreaTriggerEntityScript("spell_at_varimathras_torment_of_flames") {}

        std::list<uint64> m_Targets;
        uint32 m_CheckTimer = 500;
        float l_Radius = 100.0f;

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

                std::list<Player*> l_PlayerList;
                std::list<uint64> l_Targets(m_Targets);
                for (uint64 l_TargetGuid : l_Targets)
                {
                    Player* l_Player = ObjectAccessor::GetPlayer(*l_AreaTriggerCaster, l_TargetGuid);
                    if (!l_Player)
                        continue;

                    if (l_Player->HasAura(eSpells::TormentofFlamesAura) && l_Player->GetDistance(p_AreaTrigger) <= l_Radius)
                        continue;

                    if (l_Player->HasAura(eSpells::TormentofFlamesAura))
                        l_Player->RemoveAurasDueToSpell(eSpells::TormentofFlamesAura);

                    m_Targets.remove(l_TargetGuid);
                }

                JadeCore::AnyPlayerInObjectRangeCheck l_Check(p_AreaTrigger, l_Radius);
                JadeCore::PlayerListSearcher<JadeCore::AnyPlayerInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_PlayerList, l_Check);
                p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);
                l_PlayerList.remove_if(CheckDeadOrGmTarget());

                if (!l_PlayerList.empty())
                {
                    SpellInfo const* p_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::TormentofFlamesAura);
                    for (Player* l_Player : l_PlayerList)
                    {
                        for (Player* l_Player : l_PlayerList)
                        {
                            if (!l_Player || !p_SpellInfo || l_Player->IsImmunedToSpell(p_SpellInfo) || l_Player->HasAura(eSpells::TormentofFlamesAura))
                                continue;

                            l_Player->CastSpell(l_Player, eSpells::TormentofFlamesAura, true);
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

                if (l_Player->HasAura(eSpells::TormentofFlamesAura))
                    l_Player->RemoveAurasDueToSpell(eSpells::TormentofFlamesAura);

                m_Targets.remove(l_TargetGuid);
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_varimathras_torment_of_flames();
        }
};

/// Last Update: 7.3.5 build 26365
/// Torment of Frost  - 243976
class spell_at_varimathras_torment_of_frost : public AreaTriggerEntityScript
{
    public:
        spell_at_varimathras_torment_of_frost() : AreaTriggerEntityScript("spell_at_varimathras_torment_of_frost") {}

        std::list<uint64> m_Targets;
        uint32 m_CheckTimer = 500;
        float l_Radius = 100.0f;

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

                std::list<Player*> l_PlayerList;
                std::list<uint64> l_Targets(m_Targets);
                for (uint64 l_TargetGuid : l_Targets)
                {
                    Player* l_Player = ObjectAccessor::GetPlayer(*l_AreaTriggerCaster, l_TargetGuid);
                    if (!l_Player)
                        continue;

                    if (l_Player->HasAura(eSpells::TormentofFrostAura) && l_Player->GetDistance(p_AreaTrigger) <= l_Radius)
                        continue;

                    if (l_Player->HasAura(eSpells::TormentofFrostAura))
                        l_Player->RemoveAurasDueToSpell(eSpells::TormentofFrostAura);

                    m_Targets.remove(l_TargetGuid);
                }

                JadeCore::AnyPlayerInObjectRangeCheck l_Check(p_AreaTrigger, l_Radius);
                JadeCore::PlayerListSearcher<JadeCore::AnyPlayerInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_PlayerList, l_Check);
                p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);
                l_PlayerList.remove_if(CheckDeadOrGmTarget());

                if (!l_PlayerList.empty())
                {
                    SpellInfo const* p_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::TormentofFrostAura);
                    for (Player* l_Player : l_PlayerList)
                    {
                        for (Player* l_Player : l_PlayerList)
                        {
                            if (!l_Player || !p_SpellInfo || l_Player->IsImmunedToSpell(p_SpellInfo) || l_Player->HasAura(eSpells::TormentofFrostAura))
                                continue;

                            l_Player->CastSpell(l_Player, eSpells::TormentofFrostAura, true);
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

                if (l_Player->HasAura(eSpells::TormentofFrostAura))
                    l_Player->RemoveAurasDueToSpell(eSpells::TormentofFrostAura);

                m_Targets.remove(l_TargetGuid);
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_varimathras_torment_of_frost();
        }
};

/// Last Update: 7.3.5 build 26365
/// Torment of Fel  - 243979
class spell_at_varimathras_torment_of_fel : public AreaTriggerEntityScript
{
    public:
        spell_at_varimathras_torment_of_fel() : AreaTriggerEntityScript("spell_at_varimathras_torment_of_fel") {}

        std::list<uint64> m_Targets;
        uint32 m_CheckTimer = 500;
        float l_Radius = 100.0f;

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

                std::list<Player*> l_PlayerList;
                std::list<uint64> l_Targets(m_Targets);
                for (uint64 l_TargetGuid : l_Targets)
                {
                    Player* l_Player = ObjectAccessor::GetPlayer(*l_AreaTriggerCaster, l_TargetGuid);
                    if (!l_Player)
                        continue;

                    if (l_Player->HasAura(eSpells::TormentofFelAura) && l_Player->GetDistance(p_AreaTrigger) <= l_Radius)
                        continue;

                    if (l_Player->HasAura(eSpells::TormentofFelAura))
                        l_Player->RemoveAurasDueToSpell(eSpells::TormentofFelAura);

                    m_Targets.remove(l_TargetGuid);
                }

                JadeCore::AnyPlayerInObjectRangeCheck l_Check(p_AreaTrigger, l_Radius);
                JadeCore::PlayerListSearcher<JadeCore::AnyPlayerInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_PlayerList, l_Check);
                p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);
                l_PlayerList.remove_if(CheckDeadOrGmTarget());

                if (!l_PlayerList.empty())
                {
                    SpellInfo const* p_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::TormentofFelAura);
                    for (Player* l_Player : l_PlayerList)
                    {
                        for (Player* l_Player : l_PlayerList)
                        {
                            if (!l_Player || !p_SpellInfo || l_Player->IsImmunedToSpell(p_SpellInfo) || l_Player->HasAura(eSpells::TormentofFelAura))
                                continue;

                            l_Player->CastSpell(l_Player, eSpells::TormentofFelAura, true);
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

                if (l_Player->HasAura(eSpells::TormentofFelAura))
                    l_Player->RemoveAurasDueToSpell(eSpells::TormentofFelAura);

                m_Targets.remove(l_TargetGuid);
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_varimathras_torment_of_fel();
        }
};

/// Last Update: 7.3.5 build 26365
/// Torment of Shadows  - 243974
class spell_at_varimathras_torment_of_shadows : public AreaTriggerEntityScript
{
    public:
        spell_at_varimathras_torment_of_shadows() : AreaTriggerEntityScript("spell_at_varimathras_torment_of_shadows") {}

        std::list<uint64> m_Targets;
        uint32 m_CheckTimer = 500;
        float l_Radius = 100.0f;

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

                std::list<Player*> l_PlayerList;
                std::list<uint64> l_Targets(m_Targets);
                for (uint64 l_TargetGuid : l_Targets)
                {
                    Player* l_Player = ObjectAccessor::GetPlayer(*l_AreaTriggerCaster, l_TargetGuid);
                    if (!l_Player)
                        continue;

                    if (l_Player->HasAura(eSpells::TormentofShadowsAura) && l_Player->GetDistance(p_AreaTrigger) <= l_Radius)
                        continue;

                    if (l_Player->HasAura(eSpells::TormentofShadowsAura))
                        l_Player->RemoveAurasDueToSpell(eSpells::TormentofShadowsAura);

                    m_Targets.remove(l_TargetGuid);
                }

                JadeCore::AnyPlayerInObjectRangeCheck l_Check(p_AreaTrigger, l_Radius);
                JadeCore::PlayerListSearcher<JadeCore::AnyPlayerInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_PlayerList, l_Check);
                p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);
                l_PlayerList.remove_if(CheckDeadOrGmTarget());

                if (!l_PlayerList.empty())
                {
                    SpellInfo const* p_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::TormentofShadowsAura);
                    for (Player* l_Player : l_PlayerList)
                    {
                        for (Player* l_Player : l_PlayerList)
                        {
                            if (!l_Player || !p_SpellInfo || l_Player->IsImmunedToSpell(p_SpellInfo) || l_Player->HasAura(eSpells::TormentofShadowsAura))
                                continue;

                            l_Player->CastSpell(l_Player, eSpells::TormentofShadowsAura, true);
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

                if (l_Player->HasAura(eSpells::TormentofShadowsAura))
                    l_Player->RemoveAurasDueToSpell(eSpells::TormentofShadowsAura);

                m_Targets.remove(l_TargetGuid);
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_varimathras_torment_of_shadows();
        }
};

/// NPC 122643 - Shadow of Varimathras
class npc_varimathras_shadow : public CreatureScript
{
    public:
        npc_varimathras_shadow() : CreatureScript("npc_varimathras_shadow") { }

        struct npc_varimathras_shadowAI : public ScriptedAI
        {
            npc_varimathras_shadowAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum mEvents
            {
                EventEchoesofDoom = 1
            };

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetInCombatWithZone();
                me->SetReactState(REACT_PASSIVE);
                me->AddUnitState(UNIT_STATE_ROOT);
                events.Reset();
                events.ScheduleEvent(mEvents::EventEchoesofDoom, 500);
            }

            void SearchTarget()
            {
                std::list<Player*> l_PlayerList;
                me->GetPlayerListInGrid(l_PlayerList, 100.0f);
                l_PlayerList.remove_if(CheckDeadOrGmTarget());
                l_PlayerList.remove_if([](Player* p_Target) -> bool
                {
                    if (!p_Target || p_Target->ToPlayer()->IsActiveSpecTankSpec())
                        return true;

                    return false;
                });

                if (l_PlayerList.size() > 1)
                    l_PlayerList.resize(1);

                for (Player* l_Player : l_PlayerList)
                {
                    DoCast(l_Player, eSpells::EchoesofDoom);
                }
            }

            void JustDied(Unit* p_Killer) override
            {
                me->DespawnOrUnsummon();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case mEvents::EventEchoesofDoom:
                        {
                            SearchTarget();
                            events.ScheduleEvent(mEvents::EventEchoesofDoom, 2000);
                            break;
                        }
                        default:
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_varimathras_shadowAI(p_Creature);
        }
};

/// Last Update: 7.3.5 build 26365
/// Echoes of Doom - 248732
class spell_varimathras_echoes_of_doom : public SpellScriptLoader
{
    public:
        spell_varimathras_echoes_of_doom() : SpellScriptLoader("spell_varimathras_echoes_of_doom") { }

        class spell_varimathras_echoes_of_doom_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_varimathras_echoes_of_doom_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                if (Unit* l_Target = GetTarget())
                    if (InstanceScript* l_Instance = l_Target->GetInstanceScript())
                        if (l_Instance->GetBossState(eData::DataVarimathras) == IN_PROGRESS)
                           if (Creature* l_Varimathras = Creature::GetCreature(*l_Target, l_Instance->GetData64(eCreatures::NpcVarimathras)))
                               l_Varimathras->CastSpell(*l_Target, eSpells::EchoesofDoomAreaTrigger, true);
            }

            void HandleDispel(DispelInfo* dispelInfo)
            {
                if (Unit* l_Target = GetTarget())
                    if (InstanceScript* l_Instance = l_Target->GetInstanceScript())
                        if (l_Instance->GetBossState(eData::DataVarimathras) == IN_PROGRESS)
                           if (Creature* l_Varimathras = Creature::GetCreature(*l_Target, l_Instance->GetData64(eCreatures::NpcVarimathras)))
                               l_Varimathras->CastSpell(*l_Target, eSpells::EchoesofDoomAreaTrigger, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_varimathras_echoes_of_doom_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterDispel += AuraDispelFn(spell_varimathras_echoes_of_doom_AuraScript::HandleDispel);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_varimathras_echoes_of_doom_AuraScript();
        }
};

class achievement_varimathras_together_we_stand : public AchievementCriteriaScript
{
    public:
        achievement_varimathras_together_we_stand() : AchievementCriteriaScript("achievement_varimathras_together_we_stand") { }

        bool OnCheck(Player* p_Source, Unit* /*p_Target*/) override
        {
            if (!p_Source || !p_Source->GetInstanceScript() || !p_Source->GetMap() || p_Source->GetMap()->IsLFR())
                return false;

            if (Creature* l_Varimathras = Creature::GetCreature(*p_Source, p_Source->GetInstanceScript()->GetData64(eCreatures::NpcVarimathras)))
                if (l_Varimathras->AI())
                    if (l_Varimathras->AI()->GetData(0) == 0)
                        return true;

            return false;
        }
};

/// Last Update 7.3.5 Build 26365
/// Torment of Fel - 243980
class spell_varimathras_torment_of_fel: public SpellScriptLoader
{
    public:
        spell_varimathras_torment_of_fel() : SpellScriptLoader("spell_varimathras_torment_of_fel") { }

        class spell_varimathras_torment_of_fel_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_varimathras_torment_of_fel_AuraScript);

            bool l_Apply;
            uint32 l_Stack;

            bool Load()
            {
                l_Apply = true;
                l_Stack = 0;
                return true;
            }

            void OnTick(AuraEffect const* aurEff)
            {
                if (l_Apply) /// doesn't proc first tick
                {
                    l_Apply = false;
                    l_Stack = 1;
                    return;
                }

                Unit* l_Target = GetTarget();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Target || !l_SpellInfo)
                    return;

                int32 l_Amount = aurEff->GetBaseAmount();
                uint32 l_Basepoint = l_SpellInfo->Effects[EFFECT_1].BasePoints * l_Stack;
                AddPct(l_Amount, l_Basepoint);
                const_cast<AuraEffect*>(aurEff)->ChangeAmount(l_Amount);
                ++l_Stack;
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_varimathras_torment_of_fel_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_varimathras_torment_of_fel_AuraScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_varimathras()
{
    new boss_varimathras();
    new spell_varimathras_energy_gain();
    new spell_varimathras_control_distance_check();
    new spell_varimathras_control_aura();
    new spell_varimathras_marked_prey_aura();
    new spell_varimathras_necrotic_embrace();
    new spell_varimathras_necrotic_embrace_aura();
    new spell_varimathras_necrotic_embrace_target();
    new spell_at_varimathras_dark_fissure();
    new spell_varimathras_misery_proc();
    new spell_at_varimathras_torment_of_flames();
    new spell_at_varimathras_torment_of_frost();
    new spell_at_varimathras_torment_of_fel();
    new spell_at_varimathras_torment_of_shadows();
    new npc_varimathras_shadow();
    new spell_varimathras_echoes_of_doom();
    new achievement_varimathras_together_we_stand();
    new spell_varimathras_torment_of_fel();
}
#endif
