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

///< Boss start combat with 75 Energy, gain 1 energy = 1 sec.
enum eSpells
{
    ///< General
    KingarothEnergy              = 248303,
    KingarothOutro               = 249794,
    ///< Stage: Deployment
    ForgingStrike                = 254919,
    ReverberatingStrike          = 254926,
    ReverberatingStrikeSummon    = 244321, ///< Trigger for ReverberatingStrike not used in LFR
    FlameReverberation           = 244328,
    FireMine                     = 244313,
    DiabolicBomb                 = 246754, ///< trigger 248214
    DiabolicBombAreaTrigger      = 246746, ///< in HM and MM 3 count
    DiabolicBombDamage           = 246779,
    Ruiner                       = 246793, ///< Select Target
    RuinerSummon                 = 246798,
    RuinerChannel                = 246833, ///< Cast on trigger NpcRuiner
    RuinerAreaTrigger            = 255618,
    RuinerDamage                 = 246840,
    ShatteringStrike             = 248375, ///< usend when target not in melee dist
    ApocalypseProtocol           = 246516, ///< used when 100 Energy
    ///< Stage: Construction
    ApocalypseBlast              = 246599,
    ApocalypseBlastMissile       = 246629,
    FlamesOfTheForge             = 246645, ///< already in ApocalypseProtocol

    ///< Mythic Mode >
    WeaponsUpgrade               = 249920,
    EmpoweredRuiner              = 254797, ///< used when cast spell Ruiner
    EmpoweredReverberatingStrike = 254795, ///< used when cast spell ReverberatingStrike
    EmpoweredDiabolicBomb        = 254796, ///< used when cast spell DiabolicBomb
    Overcharge                   = 249740, ///< used when if the Raid kills the last one and the same mechanism two or more times in a row
    ///< with Aura Empowered Ruiner
    RuinationAura                = 249430,
    RuinationSummon              = 249425,
    RuinationAreaTrigger         = 249429,
    RuinationAreaMissile         = 249453,
    RuinationBlast               = 249458, ///< used when target is empty
    ///< with Aura EmpoweredReverberatingStrike
    ReverberatingDecimation      = 249680,
    ReverberatingDecimationMissile = 249686,
    ReverberatingDecimationDamage= 249724,
    ///< with Aura EmpoweredDiabolicBomb
    Demolished                   = 249535,

    ///< Creature General
    SpellInitializing            = 246504,
    SpellFeedbackOverload        = 258643, ///< used when Garoshi npcs dying
    ///< Garoshi Demolisher
    SpellDemolish                = 246692, ///< Select Target
    SpellDemolishAura            = 246698,
    SpellDemolisDamage           = 246706,
    SpellDemolisDamageAura       = 258070,
    ///< Garothi Decimator
    SpellDecimation              = 246691,
    SpellDecimationTarget        = 246686, ///< Select Target
    SpellDecimationAura          = 246687,
    SpellDecimationMissile       = 246689,
    ///< Garothi Annihilator
    SpellAnnihilation            = 246657,
    SpellAnnihilationSummon      = 246659,
    SpellAnnihilationMissile     = 246661,
    SpellAnnihilationAreaTrigger = 246667,
    SpellAnnihilationBlast       = 246666, ///< used when target is empty

    ///< Other
    PurgingProtocolAT            = 248048,
    PurgingProtocolAT1           = 248061,
    PurgingProtocolAT2           = 258020,
    PurgingProtocolAT3           = 259493,
    PurgingProtocolDamage        = 258021,

    ///< Outro
    SpellExplosions              = 251052,
    SpellFallingDebris           = 251059
};

enum eEvents
{
    EventForgingStrike = 1,
    EventReverberatingStrike,
    EventDiabolicBomb,
    EventRuiner,
    EventShatteringStrike,
    EventApocalypseBlast
};

enum eMisc
{
    NpcRuiner                         = 124230,
    NpcDetonationCharge               = 122585,
    NpcKingarothGarothiAnnihilator    = 123906,
    NpcKingarothGarothiDecimator      = 123921,
    NpcKingarothGarothiDemolisher     = 123929,
    NpcIncineratorStalker             = 124879,
    NpcApocalypseBlastStalker         = 125462,
    NpcInfernalTower1                 = 122634,
    NpcOutro                          = 125603
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

Position const g_PosRoomUp = { -10514.5000f, 8142.4399f, 1873.1801f, 3.769910f };
Position const g_PosRoomDown = { -10635.0996f, 8142.4302f, 1873.1801f, 5.654870f };
Position const g_PosRoomRight = { -10574.77f, 8024.12f, 1873.144f, 1.570796f };
Position const g_PosOutRoom = { -10575.8291f, 8166.7280f, 1872.9000f, 2.002570f };

struct SpawnDataMechanism
{
    Position    SpawnPos;
    uint32      SpawnEntry;
};

static std::array<std::vector<SpawnDataMechanism>, 3> g_SpawnMechanismNormal =
{
    {
        ///< First Rotation (Room: Up - Demolisher, Down - Decimator)
        {
            { { g_PosRoomUp }, eMisc::NpcKingarothGarothiDemolisher },
            { { g_PosRoomDown }, eMisc::NpcKingarothGarothiDecimator }
        },
        ///< Second Rotation (Room: Up - Annihilator, Right - Demolisher)
        {
            { { g_PosRoomUp }, eMisc::NpcKingarothGarothiAnnihilator  },
            { { g_PosRoomRight }, eMisc::NpcKingarothGarothiDemolisher }
        },
        ///< Third Rotation (Room: Down - Decimator, Right - Demolisher)
        {
            { { g_PosRoomDown }, eMisc::NpcKingarothGarothiDemolisher },
            { { g_PosRoomRight }, eMisc::NpcKingarothGarothiDecimator }
        }
    }
};

static std::array<std::vector<SpawnDataMechanism>, 3> g_SpawnMechaniHeroic =
{
    {
        ///< First Rotation (Room: Up - Demolisher, Down - Decimator, Right - Annihilator)
        {
            { { g_PosRoomUp }, eMisc::NpcKingarothGarothiDemolisher },
            { { g_PosRoomDown }, eMisc::NpcKingarothGarothiDecimator },
            { { g_PosRoomRight }, eMisc::NpcKingarothGarothiAnnihilator}
        },
        ///< Second Rotation (Room: Up - Demolisher, Down - Annihilator, Right - Annihilator)
        {
            { { g_PosRoomUp }, eMisc::NpcKingarothGarothiDemolisher  },
            { { g_PosRoomDown }, eMisc::NpcKingarothGarothiAnnihilator },
            { { g_PosRoomRight }, eMisc::NpcKingarothGarothiAnnihilator }
        },
        ///< Third Rotation (Room: Up - Decimator, Down - Decimator, Right - Annihilator)
        {
            { { g_PosRoomUp }, eMisc::NpcKingarothGarothiDecimator  },
            { { g_PosRoomDown }, eMisc::NpcKingarothGarothiDecimator },
            { { g_PosRoomRight }, eMisc::NpcKingarothGarothiAnnihilator }
        }
    }
};

static std::array<std::vector<SpawnDataMechanism>, 3> g_SpawnMechanismLFR =
{
    {
        ///< First Rotation (Down - Decimator)
        {
            { { g_PosRoomDown }, eMisc::NpcKingarothGarothiDecimator }
        },
        ///< Second Rotation (Room: Up - Annihilator)
        {
            { { g_PosRoomUp }, eMisc::NpcKingarothGarothiAnnihilator  }
        },
        ///< Third Rotation (Right - Demolisher)
        {
            { { g_PosRoomRight }, eMisc::NpcKingarothGarothiDemolisher }
        }
    }
};

enum eAchievKinGaroth
{
    AchievementTheWorldRevolvesAroundMe = 12030,
    AchievementMythicKingaroth          = 11998
};

enum eTalks
{
    TALK_INTRO = 0,
    TALK_AGGRO,
    TALK_CAST_REVERBERATINGSTRIKE,
    TALK_CAST_RUINER,
    TALK_FINISH_APOCALYPSE_PROTOCOL,
    TALK_SPAWN_MECHANISM,
    TALK_SLAY,
    TALK_WIPE,
    TALK_DIE,
    EMOTE_CAST_APOCALYPSE_PROTOCOL,
    EMOTE_CAST_PURGING_PROTOCOL,
    EMOTE_CAST_REVERBERATING_STRIKE, ///< used with EmpoweredReverberatingStrike
    EMOTE_CAST_SURCHARGE,
    EMOTE_CAST_DIABOLIC_BOMB, ///< usend with EmpoweredDiabolicBomb
    EMOTE_CAST_RUINER ///< usend with EmpoweredRuiner
};

const Position g_OutroPos[35] =
{
    { -10544.13f, 8021.361f, 1888.809f, 4.894392f  },
    { -10610.03f, 8035.856f, 1900.348f, 4.494794f  },
    { -10589.29f, 8491.957f, 1881.517f, 4.894392f  },
    { -10556.54f, 8194.69f,  1882.031f, 4.894392f  },
    { -10607.24f, 8647.856f, 1888.588f, 5.157228f  },
    { -10548.07f, 8220.322f, 1890.714f, 5.157228f  },
    { -10527.38f, 8575.986f, 1886.32f, 4.894392f   },
    { -10591.7f, 8186.625f, 1888.318f, 5.239878f   },
    { -10545.13f, 7927.006f, 1895.192f, 4.894392f  },
    { -10543.83f, 8630.287f, 1884.092f, 4.894392f  },
    { -10550.58f, 8369.474f, 1879.684f, 4.894392f  },
    { -10538.79f, 8035.876f, 1903.173f, 4.494794f  },
    { -10537.97f, 8429.529f, 1885.585f, 5.157228f  },
    { -10542.93f, 7942.689f, 1893.178f, 4.494794f  },
    { -10616.18f, 8606.721f, 1897.958f, 4.894392f  },
    { -10590.48f, 8417.819f, 1872.294f, 5.157228f  },
    { -10561.47f, 8511.7f, 1874.158f, 4.894392f    },
    { -10612.76f, 7947.631f, 1900.348f, 4.494794f  },
    { -10604.73f, 8023.954f, 1888.809f, 4.894392f, },
    { -10574.81f, 8495.814f, 1894.005f, 1.581022f  },
    { -10602.63f, 8598.493f, 1872.574f, 4.894392f  },
    { -10535.02f, 8610.557f, 1897.958f, 4.894392f  },
    { -10568.06f, 8357.761f, 1915.506f, 5.157228f  },
    { -10615.42f, 8362.149f, 1888.0f, 5.157228f    },
    { -10602.96f, 7929.524f, 1888.809f, 4.894392f, },
    { -10568.87f, 8340.889f, 1871.497f, 4.270146f  },
    { -10538.15f, 8095.143f, 1872.734f, 4.270146f  },
    { -10601.56f, 8075.897f, 1872.735f, 4.270146f  },
    { -10574.63f, 8098.203f, 1909.01f, 4.494794f   },
    { -10580.76f, 8599.367f, 1871.619f, 4.270146f  },
    { -10578.27f, 8449.321f, 1871.515f, 4.270146f  },
    { -10581.09f, 8178.183f, 1871.519f, 4.270146f  },
    { -10556.33f, 8132.053f, 1872.733f, 4.270146f  },
    { -10570.18f, 8637.581f, 1871.612f, 4.270146f  },
    { -10560.94f, 8547.329f, 1871.62f, 4.270146f   }
};

class boss_kin_garoth : public CreatureScript
{
    public:
        boss_kin_garoth() : CreatureScript("boss_kin_garoth") { }

        struct boss_kin_garothAI : public BossAI
        {
            boss_kin_garothAI(Creature* creature) : BossAI(creature, eData::DataKinGaroth)
            {
                ApplyAllImmunities(true);
                me->AddUnitState(UNIT_STATE_ROOT);
                SetFlyMode(true);
                m_RuinerGUID = 0;
                m_RotationMech = 0;
                m_Phase = 1;
                m_countDiabolicBomb = 0;
                m_CountDiesMechanism = 0;
            }

            uint64 m_RuinerGUID;
            uint32 m_RotationMech;
            uint8 m_Phase;
            uint32 m_countDiabolicBomb;
            uint8 m_CountDiesMechanism; ///< counter for Mythic

            bool CanBeTargetedOutOfLOSXYZ(float l_X, float l_Y, float l_Z) override
            {
                return (g_PosOutRoom.GetExactDist2d(l_X, l_Y) >= 23.0f);
            }

            bool CanTargetOutOfLOSXYZ(float l_X, float l_Y, float l_Z) override
            {
                return (g_PosOutRoom.GetExactDist2d(l_X, l_Y) >= 23.0f);
            }

            bool CanFly() override
            {
                return true;
            }

            bool IgnoreFlyingMovementFlagsUpdate() override
            {
                return true;
            }

            bool BypassXPDistanceIfNeeded(WorldObject* /*p_Object*/) override
            {
                return true;
            }

            void GetDamageMultiplier(float& p_DamageMulti) override
            {
                if (IsHeroic())
                    p_DamageMulti = 17.0f;
                else if (IsLFR() || IsMythic())
                    p_DamageMulti = 15.0f;
                else
                    p_DamageMulti = 20.0f;
            }

            void SpawnMechanism()
            {
                Talk(eTalks::TALK_SPAWN_MECHANISM);
                m_CountDiesMechanism = 0;
                for (SpawnDataMechanism l_Data : (IsMythic() ? g_SpawnMechaniHeroic[0] : IsHeroic() ? g_SpawnMechaniHeroic[m_RotationMech] : IsLFR() ? g_SpawnMechanismLFR[m_RotationMech] : g_SpawnMechanismNormal[m_RotationMech]))
                {
                    if (!l_Data.SpawnEntry)
                        continue;

                    if (Creature* l_Mechanisms = me->SummonCreature(l_Data.SpawnEntry, l_Data.SpawnPos, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1800000))
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, l_Mechanisms);
                }

                ++m_RotationMech;
                if (m_RotationMech > 2)
                    m_RotationMech = 0;
            }

            void OutroEvent()
            {
                for (uint8 l_Itr = 0; l_Itr < 35; ++l_Itr)
                {
                    if (Creature* l_Creature = me->SummonCreature(eMisc::NpcOutro, g_OutroPos[l_Itr], TEMPSUMMON_TIMED_DESPAWN, 300000)) ///< despawned after 5 minutes
                    {
                        if (l_Creature->GetPositionZ() <= 1880.0f)
                        {
                            for (uint8 l_Itr2 = 0; l_Itr2 < 20; ++l_Itr2)
                            {
                                Position* l_pos = l_Creature;
                                me->DelayedCastSpell(l_pos, eSpells::SpellFallingDebris, true, l_Itr2 * (urand(5, 7) * IN_MILLISECONDS));
                            }
                        }

                        if (l_Itr <= 2) ///< because prevent low FPS
                            l_Creature->CastSpell(l_Creature, eSpells::SpellExplosions, true);
                    }
                }
            }

            void StartOrCancelProtocol(bool l_Apply)
            {
                std::list<Creature*> l_IncineratorStalkers;
                me->GetCreatureListWithEntryInGrid(l_IncineratorStalkers, eMisc::NpcIncineratorStalker, 200.0f);
                if (!l_IncineratorStalkers.empty())
                {
                    for (Creature* l_Creature : l_IncineratorStalkers)
                    {
                        if (l_Apply)
                            l_Creature->CastSpell(l_Creature, eSpells::PurgingProtocolAT, true);
                        else
                            l_Creature->RemoveAura(eSpells::PurgingProtocolAT);
                    }
                }
            }

            void ApplyFlying()
            {
                SetFlyMode(false);
                me->RemoveUnitMovementFlag(MOVEMENTFLAG_FLYING);
                me->GetMotionMaster()->Clear();
                SetFlyMode(true);
                me->AddUnitMovementFlag(MOVEMENTFLAG_FLYING);
                me->AddUnitState(UNIT_STATE_ROOT);
                me->NearTeleportTo(me->GetHomePosition());
            }

            void ClearOther()
            {
                me->DespawnAreaTriggersInArea({ eSpells::PurgingProtocolAT, eSpells::PurgingProtocolAT1, eSpells::PurgingProtocolAT2, eSpells::PurgingProtocolAT3, eSpells::SpellAnnihilationAreaTrigger, eSpells::RuinerAreaTrigger, eSpells::DiabolicBombAreaTrigger, eSpells::RuinationAreaTrigger }, 200.0f, false);

                std::list<Creature*> l_InfernalTowers;
                me->GetCreatureListWithEntryInGrid(l_InfernalTowers, eMisc::NpcInfernalTower1, 200.0f);
                if (!l_InfernalTowers.empty())
                    for (Creature* l_Creature : l_InfernalTowers)
                         l_Creature->SetVisible(true);

                StartOrCancelProtocol(false);

                std::list<Creature*> l_ApocalypseBlastStalkers;
                me->GetCreatureListWithEntryInGrid(l_ApocalypseBlastStalkers, eMisc::NpcApocalypseBlastStalker, 200.0f);
                if (!l_ApocalypseBlastStalkers.empty())
                    for (Creature* l_Creature : l_ApocalypseBlastStalkers)
                         if (l_Creature->AI())
                             l_Creature->AI()->DoAction(0); /// Deactivate

                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::PurgingProtocolDamage);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellDecimationAura);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellDecimationTarget);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellDemolishAura);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::RuinerDamage);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::ForgingStrike);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::Demolished);
            }

            void Reset() override
            {
                _Reset();
                events.Reset();
                me->SetDisplayId(54860);
                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                ClearOther();
                me->RemoveAura(eSpells::KingarothEnergy);
                me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_REGENERATE_POWER);
                me->SetPower(POWER_ENERGY, 75);
                m_RuinerGUID = 0;
                m_RotationMech = 0;
                m_Phase = 1;
                m_countDiabolicBomb = 0;
                m_CountDiesMechanism = 0;
                ClearDelayedOperations();
                me->SetReactState(REACT_DEFENSIVE);
                ApplyFlying();
                instance->AddTimedDelayedOperation(2500, [this]() -> void
                {
                    ClearOther();
                });

                AddTimedDelayedOperation(2 * IN_MILLISECONDS, [this]() -> void
                {
                    ApplyFlying();
                });
            }

            void EnterEvadeMode() override
            {
                ClearDelayedOperations();
                Talk(eTalks::TALK_WIPE);
                ClearOther();
                BossAI::EnterEvadeMode();
                instance->AddTimedDelayedOperation(2500, [this]() -> void
                {
                    ClearOther();
                });
                me->Respawn(true, true, 29 * TimeConstants::IN_MILLISECONDS);
                ApplyFlying();
                me->SetDisplayId(11686);
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
            }

            void JustReachedHome() override
            {
                _JustReachedHome();
            }

            void EnterCombat(Unit* attacker) override
            {
                _EnterCombat();
                Talk(eTalks::TALK_AGGRO);
                DoCast(eSpells::KingarothEnergy);
                events.ScheduleEvent(eEvents::EventForgingStrike, urand(6, 7) * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventReverberatingStrike, urand(14, 15) * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventDiabolicBomb, 11 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventRuiner, urand(21, 22) * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventShatteringStrike, 5 * IN_MILLISECONDS); ///< check melee dist

                AddTimedDelayedOperation(2000, [this]() -> void
                {
                   StartOrCancelProtocol(true);
                });

            }

            void JustDied(Unit* p_Killer) override
            {
                if (!IsLFR() && GetData(0) >= 9)
                    instance->DoCompleteAchievement(eAchievKinGaroth::AchievementTheWorldRevolvesAroundMe);

                if (IsMythic())
                    instance->DoCompleteAchievement(eAchievKinGaroth::AchievementMythicKingaroth);

                _JustDied();
                ClearOther();
                Talk(eTalks::TALK_DIE);
                OutroEvent();
                instance->AddTimedDelayedOperation(2500, [this]() -> void
                {
                    ClearOther();
                    ApplyFlying();
                });

                instance->DoCastSpellOnPlayers(eSpells::KingarothOutro);
                ApplyFlying();
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == 1 || p_Action == 2)
                    ChangePhase(uint8(p_Action));
            }

            void KilledUnit(Unit* p_Who) override
            {
                if (p_Who->IsPlayer())
                    Talk(eTalks::TALK_SLAY);
            }

            void ChangePhase(uint8 l_Phase)
            {
                if (m_Phase == l_Phase)
                    return;

                m_Phase = l_Phase;

                switch(l_Phase)
                {
                    case 1: ///< Start Stage: Deployment
                    {
                        me->SetReactState(REACT_AGGRESSIVE);
                        events.ScheduleEvent(eEvents::EventForgingStrike, urand(6, 7) * IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventReverberatingStrike, urand(14, 15) * IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventDiabolicBomb, 11 * IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventRuiner, urand(21, 22) * IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventShatteringStrike, 5 * IN_MILLISECONDS); ///< check melee dist

                        std::list<Creature*> l_InfernalTowers;
                        me->GetCreatureListWithEntryInGrid(l_InfernalTowers, eMisc::NpcInfernalTower1, 200.0f);
                        if (!l_InfernalTowers.empty())
                            for (Creature* l_Creature : l_InfernalTowers)
                                 l_Creature->SetVisible(true);

                        StartOrCancelProtocol(true);

                        std::list<Creature*> l_ApocalypseBlastStalkers;
                        me->GetCreatureListWithEntryInGrid(l_ApocalypseBlastStalkers, eMisc::NpcApocalypseBlastStalker, 200.0f);
                        if (!l_ApocalypseBlastStalkers.empty())
                            for (Creature* l_Creature : l_ApocalypseBlastStalkers)
                                 if (l_Creature->AI())
                                     l_Creature->AI()->DoAction(0); /// Deactivate

                        break;
                    }
                    case 2: ///< Start Stage: Construction
                    {
                        Talk(eTalks::EMOTE_CAST_APOCALYPSE_PROTOCOL);
                        me->AttackStop();
                        me->SetReactState(REACT_PASSIVE);
                        DoCast(eSpells::ApocalypseProtocol);
                        me->DespawnAreaTriggersInArea(eSpells::PurgingProtocolAT, 200.0f);
                        me->DespawnAreaTriggersInArea(eSpells::PurgingProtocolAT1, 200.0f);
                        me->DespawnAreaTriggersInArea(eSpells::PurgingProtocolAT2, 200.0f);
                        me->DespawnAreaTriggersInArea(eSpells::PurgingProtocolAT3, 200.0f);
                        events.CancelEvent(eEvents::EventForgingStrike);
                        events.CancelEvent(eEvents::EventReverberatingStrike);
                        events.CancelEvent(eEvents::EventDiabolicBomb);
                        events.CancelEvent(eEvents::EventRuiner);
                        events.CancelEvent(eEvents::EventShatteringStrike);

                        std::list<Creature*> l_InfernalTowers;
                        me->GetCreatureListWithEntryInGrid(l_InfernalTowers, eMisc::NpcInfernalTower1, 200.0f);
                        if (!l_InfernalTowers.empty())
                            for (Creature* l_Creature : l_InfernalTowers)
                                 l_Creature->SetVisible(false);

                        std::list<Creature*> l_ApocalypseBlastStalkers;
                        me->GetCreatureListWithEntryInGrid(l_ApocalypseBlastStalkers, eMisc::NpcApocalypseBlastStalker, 200.0f);
                        if (!l_ApocalypseBlastStalkers.empty())
                        {
                            JadeCore::RandomResizeList(l_ApocalypseBlastStalkers, 1);
                            for (Creature* l_Creature : l_ApocalypseBlastStalkers)
                                 if (l_Creature->AI())
                                     l_Creature->AI()->DoAction(1); ///< Activate
                        }

                        AddTimedDelayedOperation(2300, [this]() -> void
                        {
                            SpawnMechanism();
                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            void FillCirclePath(Creature* p_Summon, Position const& p_Center, float p_Radius, float p_Z, std::vector<G3D::Vector3>& p_Path, bool p_Clockwise)
            {
                float l_Step = p_Clockwise ? -M_PI / 8.0f : M_PI / 8.0f;
                float l_Angle = p_Center.GetAngle(p_Summon->GetPositionX(), p_Summon->GetPositionY());

                for (uint8 l_Iter = 0; l_Iter < 16; l_Angle += l_Step, ++l_Iter)
                {
                    G3D::Vector3 l_Point;
                    l_Point.x = p_Center.GetPositionX() + p_Radius * cosf(l_Angle);
                    l_Point.y = p_Center.GetPositionY() + p_Radius * sinf(l_Angle);
                    l_Point.z = p_Z;
                    p_Path.push_back(l_Point);
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                BossAI::JustSummoned(p_Summon);
                switch(p_Summon->GetEntry())
                {
                    case eMisc::NpcRuiner:
                    {
                         m_RuinerGUID = p_Summon->GetGUID();
                         uint64 l_RuinerGUID = m_RuinerGUID;
                         AddTimedDelayedOperation(1000, [this, l_RuinerGUID]() -> void
                         {
                            if (l_RuinerGUID)
                                if (Creature* l_Ruiner = ObjectAccessor::GetCreature(*me, l_RuinerGUID))
                                    DoCast(l_Ruiner, eSpells::RuinerChannel);
                         });

                         AddTimedDelayedOperation(3500, [this, l_RuinerGUID]() -> void
                         {
                            if (l_RuinerGUID)
                            {
                                if (Creature* l_Ruiner = ObjectAccessor::GetCreature(*me, l_RuinerGUID))
                                {
                                    l_Ruiner->StopMoving();
                                    l_Ruiner->GetMotionMaster()->Clear();
                                    l_Ruiner->SetWalk(true);
                                    l_Ruiner->SetSpeed(MOVE_WALK, 2.5f, true);

                                    bool m_Clockwise = bool(urand(0, 1));
                                    std::vector<G3D::Vector3> l_Path;
                                    FillCirclePath(l_Ruiner, *me, l_Ruiner->GetExactDist2d(me), l_Ruiner->m_positionZ, l_Path, m_Clockwise);
                                    l_Ruiner->GetMotionMaster()->MoveSmoothPath(0, l_Path.data(), l_Path.size(), true);
                                }
                            }
                         });
                         break;
                    }
                    default:
                         break;
                }
            }

            void _UpdateRuinerOrientation()
            {
                if (!me->HasAura(eSpells::RuinerAreaTrigger))
                    return;

                Unit* l_Ruiner = Unit::GetUnit(*me, m_RuinerGUID);

                if (l_Ruiner)
                    me->SetOrientation(me->GetAngle(l_Ruiner));
            }

            void OnSpellFinishedSuccess(SpellInfo const* p_SpellInfo) override
            {
                if (!p_SpellInfo)
                    return;

                switch (p_SpellInfo->Id)
                {
                    case eSpells::Ruiner:
                    {
                        me->AttackStop();
                        me->SetReactState(REACT_PASSIVE);
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                        {
                            me->SetFacingTo(me->GetAngle(l_Target));
                            Position l_RuinerPos = *l_Target;
                            AddTimedDelayedOperation(1500, [this, l_RuinerPos]() -> void
                            {
                                me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                                DoCast(l_RuinerPos, eSpells::RuinerSummon, true);
                            });
                        }
                        break;
                    }
                    case eSpells::RuinerChannel:
                        m_RuinerGUID = 0;
                        me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                        me->SetReactState(REACT_AGGRESSIVE);
                        break;
                    case eSpells::ApocalypseProtocol:
                        Talk(eTalks::EMOTE_CAST_PURGING_PROTOCOL);
                        Talk(eTalks::TALK_FINISH_APOCALYPSE_PROTOCOL);
                        ChangePhase(1);
                        break;
                    case eSpells::ReverberatingStrike:
                    {
                        if (!IsLFR())
                        {
                            std::list<Creature*> l_DetonationCharges;
                            me->GetCreatureListWithEntryInGrid(l_DetonationCharges, eMisc::NpcDetonationCharge, 200.0f);
                            if (!l_DetonationCharges.empty())
                                for (Creature* l_Creature : l_DetonationCharges)
                                     if (l_Creature->AI())
                                         l_Creature->AI()->DoAction(0);
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            uint64 GetGUID(int32 /*p_ID*/ = 0) override
            {
                return m_RuinerGUID;
            }

            uint32 GetData(uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case 0:
                        return m_countDiabolicBomb;
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
                        m_countDiabolicBomb = p_Value;
                        break;
                    }
                }
            }

            void SummonedCreatureDies(Creature* p_Summon, Unit* /*p_Killer*/) override
            {
                switch (p_Summon->GetEntry())
                {
                    case eMisc::NpcKingarothGarothiAnnihilator:
                    case eMisc::NpcKingarothGarothiDecimator:
                    case eMisc::NpcKingarothGarothiDemolisher:
                    {
                        me->CastSpell(me, eSpells::SpellFeedbackOverload, true);
                        if (!IsMythic())
                            break;

                        ++m_CountDiesMechanism;
                        if (m_CountDiesMechanism < 3)
                            break;

                        me->CastSpell(me, eSpells::WeaponsUpgrade, true); ///< for DBM

                        if (p_Summon->GetEntry() == eMisc::NpcKingarothGarothiAnnihilator)
                        {
                            if (me->HasAura(eSpells::EmpoweredRuiner))
                            {
                                me->CastSpell(me, eSpells::Overcharge, true);
                                Talk(eTalks::EMOTE_CAST_SURCHARGE);
                            }
                            else
                            {
                                me->CastSpell(me, eSpells::EmpoweredRuiner, true);
                                Talk(eTalks::EMOTE_CAST_RUINER);
                            }
                        }
                        else if (p_Summon->GetEntry() == eMisc::NpcKingarothGarothiDecimator)
                        {
                            if (me->HasAura(eSpells::EmpoweredReverberatingStrike))
                            {
                                me->CastSpell(me, eSpells::Overcharge, true);
                                Talk(eTalks::EMOTE_CAST_SURCHARGE);
                            }
                            else
                            {
                                me->CastSpell(me, eSpells::EmpoweredReverberatingStrike, true);
                                Talk(eTalks::EMOTE_CAST_REVERBERATING_STRIKE);
                            }
                        }
                        else if (p_Summon->GetEntry() == eMisc::NpcKingarothGarothiDemolisher)
                        {
                            if (me->HasAura(eSpells::EmpoweredDiabolicBomb))
                            {
                                me->CastSpell(me, eSpells::Overcharge, true);
                                Talk(eTalks::EMOTE_CAST_SURCHARGE);
                            }
                            else
                            {
                                me->CastSpell(me, eSpells::EmpoweredDiabolicBomb, true);
                                Talk(eTalks::EMOTE_CAST_DIABOLIC_BOMB);
                            }
                        }
                        break;
                    }
                    default:
                        break;
                }
            }

            bool CheckPlayersInRoom()
            {
                std::list<Player*> playerList;
                GetPlayerListInGrid(playerList, me, 200.0f);
                playerList.remove_if(CheckDeadOrGmTarget());
                playerList.remove_if([this](Player* p_Player) -> bool
                {
                    if (p_Player == nullptr || !p_Player->isAlive() || p_Player->GetExactDist2d(&g_PosOutRoom) < 24.0f)
                        return true;

                    return false;
                });

                if (playerList.empty())
                {
                    EnterEvadeMode();
                    return false;
                }

                return true;
            }

            Player* SelectRandomPlayer(float l_Radius)
            {
                Map* map = me->GetMap();
                if (!map->IsDungeon())
                    return nullptr;

                Map::PlayerList const &PlayerList = map->GetPlayers();
                if (PlayerList.isEmpty())
                    return nullptr;

                std::list<Player*> temp;
                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    if (me->IsWithinDistInMap(i->getSource(), l_Radius) && !i->getSource()->isGameMaster())
                        temp.push_back(i->getSource());

                if (!temp.empty())
                {
                    std::list<Player*>::const_iterator j = temp.begin();
                    advance(j, rand()%temp.size());
                    return (*j);
                }
                return nullptr;
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo && p_SpellInfo->Id == eSpells::SpellFeedbackOverload && p_Damage >= me->GetHealth())
                {
                    p_Damage = 0;
                    if (Player* l_Player = SelectRandomPlayer(100.0f))
                        l_Player->Kill(me);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
                if (me->SelectNearestPlayerNotGM(130.0f) && instance && instance->GetData(eData::DataKinGarothIntro) != DONE)
                {
                    instance->SetData(eData::DataKinGarothIntro, DONE);
                    Talk(eTalks::TALK_INTRO);
                }

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (!CheckPlayersInRoom())
                    return;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                {
                    _UpdateRuinerOrientation();
                    return;
                }

                if (me->GetPower(Powers::POWER_ENERGY) >= 100 && !me->HasAura(eSpells::RuinerAreaTrigger))
                {
                    ChangePhase(2);
                    return;
                }

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eEvents::EventForgingStrike:
                        {
                            DoCastVictim(eSpells::ForgingStrike);
                            events.ScheduleEvent(eEvents::EventForgingStrike, IsLFR() ? 8500 : 14600);
                            break;
                        }
                        case eEvents::EventReverberatingStrike:
                        {
                            Talk(eTalks::TALK_CAST_REVERBERATINGSTRIKE);
                            if (me->HasAura(eSpells::EmpoweredReverberatingStrike))
                                DoCast(ReverberatingDecimation);

                            if (Player* l_Melee = SelectPlayerTarget(eTargetTypeMask::TypeMaskMelee, {}, 8.0f, { }, true))
                            {
                                DoCast(l_Melee, eSpells::ReverberatingStrike);
                                if (!IsLFR())
                                {
                                    uint64 l_ReverberatingStrikeGUID = l_Melee->GetGUID();
                                    AddTimedDelayedOperation(1300, [this, l_ReverberatingStrikeGUID]() -> void
                                    {
                                        if (Unit* l_ReverberatingTarget = ObjectAccessor::GetUnit(*me, l_ReverberatingStrikeGUID))
                                            DoCast(l_ReverberatingTarget, eSpells::ReverberatingStrikeSummon, true);
                                    });
                                }
                            }

                            events.ScheduleEvent(eEvents::EventReverberatingStrike, (IsLFR() ? 26 : 28) * IN_MILLISECONDS);
                            break;
                        }
                        case eEvents::EventDiabolicBomb:
                        {
                            if (IsHeroicOrMythic())
                            {
                                for (uint8 i = 0; i < 3; ++i)
                                {
                                    Position l_Pos = *me;
                                    Position l_Dest;
                                    l_Pos.SimplePosXYRelocationByAngle(l_Dest, frand(25.0f, 45.0f), frand(0.0f, 2 * M_PI));
                                    DoCast(l_Dest, eSpells::DiabolicBomb);
                                }
                            }
                            else
                            {
                                Position l_Pos = *me;
                                Position l_Dest;
                                l_Pos.SimplePosXYRelocationByAngle(l_Dest, frand(25.0f, 45.0f), frand(0.0f, 2 * M_PI));
                                DoCast(l_Dest, eSpells::DiabolicBomb);
                            }

                            events.ScheduleEvent(eEvents::EventDiabolicBomb, 20 * IN_MILLISECONDS);
                            break;
                        }
                        case eEvents::EventRuiner:
                        {
                            Talk(eTalks::TALK_CAST_RUINER);
                            if (me->HasAura(eSpells::EmpoweredRuiner))
                            {
                                std::list<Player*> playerList;
                                GetPlayerListInGrid(playerList, me, 60.0f);
                                playerList.remove_if(CheckDeadOrGmTarget());
                                playerList.remove_if([this](Player* p_Player) -> bool
                                {
                                    if (p_Player == nullptr || p_Player->GetExactDist2d(&g_PosOutRoom) < 23.0f)
                                        return true;

                                    return false;
                                });

                                if (playerList.size() > 5)
                                    JadeCore::RandomResizeList(playerList, 5);

                                for (auto player : playerList)
                                {
                                    Position l_Dest;
                                    player->GetFirstCollisionPosition(l_Dest, frand(6.0f, 7.0f), frand(0.0f, 2 * M_PI));
                                    l_Dest.m_positionZ = 1873.65f;
                                    me->CastSpell(l_Dest, eSpells::RuinationSummon, true);
                                    me->CastSpell(l_Dest, eSpells::RuinationAreaMissile, true);
                                }
                            }

                            me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                            DoCast(eSpells::Ruiner);
                            events.ScheduleEvent(eEvents::EventRuiner, 29 * IN_MILLISECONDS);
                            break;
                        }
                        case eEvents::EventShatteringStrike:
                        {
                            if (Unit* l_Victim = me->getVictim())
                                if (!me->IsWithinMeleeRange(l_Victim))
                                    DoCast(eSpells::ShatteringStrike);
                            events.ScheduleEvent(eEvents::EventShatteringStrike, 1500);
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
            return new boss_kin_garothAI(p_Creature);
        }
};

/// Last Update 7.3.5 Build 26365
/// Kin'garoth Energize Periodic - 248303
class spell_kin_garoth_energy: public SpellScriptLoader
{
    public:
        spell_kin_garoth_energy() : SpellScriptLoader("spell_kin_garoth_energy") { }

        class spell_kin_garoth_energy_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_kin_garoth_energy_AuraScript);

            void HandleOnPeriodic(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || l_Caster->getPowerType() != POWER_ENERGY)
                    return;

                l_Caster->ModifyPower(POWER_ENERGY, 1);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_kin_garoth_energy_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_kin_garoth_energy_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Diabolic Bomb - 246779
class spell_kin_garoth_diabolic_bomb_damage : public SpellScriptLoader
{
    public:
        spell_kin_garoth_diabolic_bomb_damage() : SpellScriptLoader("spell_kin_garoth_diabolic_bomb_damage") { }

        class spell_kin_garoth_diabolic_bomb_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_kin_garoth_diabolic_bomb_damage_SpellScript);

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_Target || !l_SpellInfo)
                    return;

                float l_Radius = l_SpellInfo->Effects[EFFECT_0].CalcRadius(l_Caster);
                if (l_Radius == 0.0f)
                    return;

                ///< Recalculate damage from video on Retail
                Position const* l_Pos = GetExplTargetDest();
                float l_Distancce = (l_Pos->GetExactDist2d(l_Target) * (l_Caster->GetMap()->GetDifficultyID() == Difficulty::DifficultyRaidMythic ? 3.0f : 4.5f));
                float l_Mod = (1.0f - (float)((l_Distancce > 180.0f ? 180.0f : l_Distancce) / l_Radius));
                uint32 l_Damage = uint32((float)GetHitDamage() * l_Mod);

                if (Aura* l_Aura = l_Target->GetAura(eSpells::Demolished))
                {
                    if (SpellInfo const* l_DemolishSpellInfo = sSpellMgr->GetSpellInfo(eSpells::Demolished))
                    {
                        uint32 l_Basepoint = l_DemolishSpellInfo->Effects[EFFECT_0].BasePoints * l_Aura->GetStackAmount();
                        AddPct(l_Damage, l_Basepoint);
                    }
                }

                if (Aura* l_Aura = l_Caster->GetAura(eSpells::Overcharge))
                {
                    if (SpellInfo const* l_OverchargeSpellInfo = sSpellMgr->GetSpellInfo(eSpells::Overcharge))
                    {
                        uint32 l_Basepoint = l_OverchargeSpellInfo->Effects[EFFECT_0].BasePoints * l_Aura->GetStackAmount();
                        AddPct(l_Damage, l_Basepoint);
                    }
                }

                SetHitDamage(l_Damage);
            }

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target || !l_Caster->HasAura(eSpells::EmpoweredDiabolicBomb))
                    return;

                l_Caster->CastSpell(l_Target, eSpells::Demolished, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_kin_garoth_diabolic_bomb_damage_SpellScript::HandleOnHit);
                AfterHit += SpellHitFn(spell_kin_garoth_diabolic_bomb_damage_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_kin_garoth_diabolic_bomb_damage_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Diabolic Bomb - 246746
class spell_at_kin_garoth_diabolic_bomb : public AreaTriggerEntityScript
{
    public:
        spell_at_kin_garoth_diabolic_bomb() : AreaTriggerEntityScript("spell_at_kin_garoth_diabolic_bomb") { }

        uint32 m_CheckTimer = 4000;
        bool l_CheckTime = true;
        uint8 l_CountStep = 0;

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (!p_AreaTrigger || !l_CheckTime)
                 return;

            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            if (m_CheckTimer <= p_Time)
            {
               if (p_AreaTrigger->GetDistance2d(p_AreaTrigger->GetSource().x, p_AreaTrigger->GetSource().y) <= 1.0f)
               {
                    m_CheckTimer = 4000;
                    ++l_CountStep;
                    if (l_CountStep < 3)
                        return;

                    uint32 l_Speed = uint32(p_AreaTrigger->getCircleSpeed() + 10);
                    if (l_Speed >= 20)
                        l_CheckTime = false;

                    p_AreaTrigger->UpdateSpeedTriggerCircle(l_Speed, true);
               }
            }
            else
                m_CheckTimer -= p_Time;
        }

        void OnCreate(AreaTrigger* p_Areatrigger) override
        {
            if (!p_Areatrigger)
                return;

            Unit* l_Caster = p_Areatrigger->GetCaster();
            if (!l_Caster)
                return;

            Creature* l_Creature = l_Caster->ToCreature();
            if (!l_Creature || !l_Creature->AI())
                return;

            uint32 l_Count = l_Creature->AI()->GetData(0) + 1;
            l_Creature->AI()->SetData(0, l_Count);
        }

        void OnRemove(AreaTrigger* p_Areatrigger, uint32 /*p_Time*/) override
        {
            if (!p_Areatrigger)
                return;

            Unit* l_Caster = p_Areatrigger->GetCaster();
            if (!l_Caster)
                return;

            Creature* l_Creature = l_Caster->ToCreature();
            if (!l_Creature || !l_Creature->AI())
                return;

            uint32 l_Count = (l_Creature->AI()->GetData(0) ? l_Creature->AI()->GetData(0) - 1 : 0);
            l_Creature->AI()->SetData(0, l_Count);
        }

        bool OnAddTarget(AreaTrigger* p_Areatrigger, Unit* p_Target) override
        {
            if (!p_Areatrigger || !p_Target || !p_Target->IsPlayer())
                return true;

            Unit* l_Caster = p_Areatrigger->GetCaster();
            if (!l_Caster)
                return true;

            l_Caster->CastSpell(*p_Target, eSpells::DiabolicBombDamage, true);
            p_Areatrigger->Remove();
            return true;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_kin_garoth_diabolic_bomb();
        }
};

/// Last Update 7.3.5 - 26365
/// ApocalypseBlast - 246599
class spell_kin_garoth_apocalypse_blast : public SpellScriptLoader
{
    public:
        spell_kin_garoth_apocalypse_blast() : SpellScriptLoader("spell_kin_garoth_apocalypse_blast") { }

        class spell_kin_garoth_apocalypse_blast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_kin_garoth_apocalypse_blast_SpellScript);

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::ApocalypseBlastMissile, true);
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if(JadeCore::UnitAuraCheck(true, eSpells::ApocalypseBlast));
                p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr || p_Object->GetExactDist2d(&g_PosOutRoom) < 23.0f)
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_kin_garoth_apocalypse_blast_SpellScript::HandleAfterHit);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_kin_garoth_apocalypse_blast_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_kin_garoth_apocalypse_blast_SpellScript();
        }
};

/// Last Update 7.3.5 - 26365
/// Initializing - 246504
class spell_kin_garoth_initializing : public SpellScriptLoader
{
    public:
        spell_kin_garoth_initializing() : SpellScriptLoader("spell_kin_garoth_initializing") { }

        class spell_kin_garoth_initializing_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_kin_garoth_initializing_AuraScript);

            void OnTick(const AuraEffect* /*p_AurEff*/)
            {
                Unit* l_Caster = GetCaster();
                Aura* l_Aura = GetAura();
                if (!l_Caster || !l_Aura)
                    return;

                if (l_Aura->GetStackAmount() >= 2)
                    l_Aura->SetStackAmount(l_Aura->GetStackAmount() - 1);
                else
                    l_Caster->RemoveAuraFromStack(l_Aura->GetId());
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_kin_garoth_initializing_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_kin_garoth_initializing_AuraScript();
        }
};
///< 123921 - Garoshi Decimator
class npc_kin_garoth_garothi_decimator : public CreatureScript
{
    public:
        npc_kin_garoth_garothi_decimator() : CreatureScript("npc_kin_garoth_garothi_decimator") { }

        struct npc_kin_garoth_garothi_decimatorAI : public ScriptedAI
        {
            npc_kin_garoth_garothi_decimatorAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                ApplyAllImmunities(true);
                me->AddUnitState(UNIT_STATE_ROOT);
                me->SetReactState(REACT_PASSIVE);
            }

            enum mEvents
            {
                EventDecimator = 1
            };

            bool l_Initializing = true;

            bool CanFly() override
            {
                return false;
            }

            bool IgnoreFlyingMovementFlagsUpdate() override
            {
                return true;
            }

            void GetDamageMultiplier(float& p_DamageMulti) override
            {
                if (IsHeroic() || IsMythic())
                    p_DamageMulti = 17.0f;
                else
                    p_DamageMulti = 20.0f;
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetInCombatWithZone();
                events.Reset();
                me->AddUnitState(UNIT_STATE_ROOT);
                me->SetReactState(REACT_PASSIVE);
                l_Initializing = true;
                me->SetAuraStack(eSpells::SpellInitializing, me, 30);
                me->CastSpell(me, eSpells::SpellInitializing, false);
                events.ScheduleEvent(mEvents::EventDecimator, urand(5, 10) * IN_MILLISECONDS);
            }

            void OnSpellFinishedSuccess(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellDecimation:
                    {
                        DoCast(eSpells::SpellDecimationTarget);
                        break;
                    }
                    case eSpells::SpellInitializing:
                    {
                        me->ClearUnitState(UNIT_STATE_ROOT);

                        Position l_Pos;
                        me->GetNearPosition(l_Pos, 25.0f, 0.0f);
                        me->SetWalk(false);
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MovePoint(2, l_Pos.m_positionX, l_Pos.m_positionY, l_Pos.m_positionZ);
                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case 2:
                    {
                        me->SetReactState(REACT_AGGRESSIVE);
                        l_Initializing = false;
                        break;
                    }
                    default:
                        break;
                }
            }

            void JustDied(Unit* p_Killer) override
            {
                me->m_Events.KillAllEvents(true);
                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellDecimationAura, me->GetGUID());

                me->DespawnOrUnsummon(5000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (l_Initializing || !UpdateVictim())
                    return;

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    if (me->GetDistance2d(g_PosOutRoom.GetPositionX(), g_PosOutRoom.GetPositionY()) <= 13.0f)
                    {
                        if (Creature* l_Kingaroth = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::NpcKingaroth)))
                        {
                             if (l_Kingaroth->AI())
                             {
                                 l_Kingaroth->AI()->EnterEvadeMode();
                                 return;
                             }
                        }
                    }
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case mEvents::EventDecimator:
                            DoCast(eSpells::SpellDecimation);
                            events.ScheduleEvent(mEvents::EventDecimator, urand(20 * IN_MILLISECONDS, 30 * IN_MILLISECONDS));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_kin_garoth_garothi_decimatorAI(p_Creature);
        }
};

/// Last Update 7.3.5 Build 26365
/// Decimation - 246686
class spell_kin_garoth_decimator : public SpellScriptLoader
{
public:
    spell_kin_garoth_decimator() : SpellScriptLoader("spell_kin_garoth_decimator") { }

    class spell_kin_garoth_decimator_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_kin_garoth_decimator_SpellScript);

        void HandleHit(SpellEffIndex /*effIndex*/)
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetHitUnit();
            SpellInfo const* l_SpellInfo = GetSpellInfo();
            if (!l_Caster || !l_Target || !l_SpellInfo)
                return;

            l_Caster->CastSpell(l_Target, l_SpellInfo->Effects[EFFECT_0].BasePoints, true);
            l_Caster->DelayedCastSpell(l_Target, eSpells::SpellDecimationMissile, true, 2500);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_kin_garoth_decimator_SpellScript::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_kin_garoth_decimator_SpellScript();
    }
};
///< 123929 - Garoshi Demolisher
class npc_kin_garoth_garothi_demolisher : public CreatureScript
{
    public:
        npc_kin_garoth_garothi_demolisher() : CreatureScript("npc_kin_garoth_garothi_demolisher") { }

        struct npc_kin_garoth_garothi_demolisherAI : public ScriptedAI
        {
            npc_kin_garoth_garothi_demolisherAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                ApplyAllImmunities(true);
                me->AddUnitState(UNIT_STATE_ROOT);
                me->SetReactState(REACT_PASSIVE);
            }

            enum mEvents
            {
                EventDemolish = 1
            };

            bool l_Initializing = true;

            bool CanFly() override
            {
                return false;
            }

            bool IgnoreFlyingMovementFlagsUpdate() override
            {
                return true;
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetInCombatWithZone();
                events.Reset();
                me->AddUnitState(UNIT_STATE_ROOT);
                me->SetReactState(REACT_PASSIVE);
                l_Initializing = true;
                me->SetAuraStack(eSpells::SpellInitializing, me, 30);
                me->CastSpell(me, eSpells::SpellInitializing, false);
                events.ScheduleEvent(mEvents::EventDemolish, urand(5, 10) * IN_MILLISECONDS);
            }

            void GetDamageMultiplier(float& p_DamageMulti) override
            {
                if (IsHeroic() || IsMythic())
                    p_DamageMulti = 17.0f;
                else
                    p_DamageMulti = 20.0f;
            }

            void OnSpellFinishedSuccess(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellInitializing:
                    {
                        me->ClearUnitState(UNIT_STATE_ROOT);

                        Position l_Pos;
                        me->GetNearPosition(l_Pos, 25.0f, 0.0f);
                        me->SetWalk(false);
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MovePoint(2, l_Pos.m_positionX, l_Pos.m_positionY, l_Pos.m_positionZ);
                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case 2:
                    {
                        me->SetReactState(REACT_AGGRESSIVE);
                        l_Initializing = false;
                        break;
                    }
                    default:
                        break;
                }
            }

            void JustDied(Unit* p_Killer) override
            {
                me->DespawnOrUnsummon(5000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (l_Initializing || !UpdateVictim())
                    return;

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    if (me->GetDistance2d(g_PosOutRoom.GetPositionX(), g_PosOutRoom.GetPositionY()) <= 13.0f)
                    {
                        if (Creature* l_Kingaroth = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::NpcKingaroth)))
                        {
                             if (l_Kingaroth->AI())
                             {
                                 l_Kingaroth->AI()->EnterEvadeMode();
                                 return;
                             }
                        }
                    }
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case mEvents::EventDemolish:
                            DoCast(eSpells::SpellDemolish);
                            events.ScheduleEvent(mEvents::EventDemolish, urand(20 * IN_MILLISECONDS, 30 * IN_MILLISECONDS));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_kin_garoth_garothi_demolisherAI(p_Creature);
        }
};

/// Last Update 7.3.5 Build 26365
/// Demolish - 246692
class spell_kin_garoth_demolish : public SpellScriptLoader
{
public:
    spell_kin_garoth_demolish() : SpellScriptLoader("spell_kin_garoth_demolish") { }

    class spell_kin_garoth_demolish_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_kin_garoth_demolish_SpellScript);

        void HandleHit(SpellEffIndex /*effIndex*/)
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetHitUnit();
            SpellInfo const* l_SpellInfo = GetSpellInfo();
            if (!l_Caster || !l_Target || !l_SpellInfo)
                return;

            l_Caster->CastSpell(l_Target, eSpells::SpellDemolishAura, true);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_kin_garoth_demolish_SpellScript::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_kin_garoth_demolish_SpellScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Demolish - 246698
class spell_kin_garoth_demolish_aura : public SpellScriptLoader
{
    public:
        spell_kin_garoth_demolish_aura() : SpellScriptLoader("spell_kin_garoth_demolish_aura") { }

        class spell_kin_garoth_demolish_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_kin_garoth_demolish_aura_AuraScript);

            void HandleAfterRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_Target->CastSpell(l_Target, eSpells::SpellDemolisDamage, true);
                l_Target->CastSpell(l_Target, eSpells::SpellDemolisDamageAura, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_kin_garoth_demolish_aura_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_kin_garoth_demolish_aura_AuraScript();
        }
};

///< 123906 - Garoshi Annihilator
class npc_kin_garoth_garothi_annihilator : public CreatureScript
{
    public:
        npc_kin_garoth_garothi_annihilator() : CreatureScript("npc_kin_garoth_garothi_annihilator") { }

        struct npc_kin_garoth_garothi_annihilatorAI : public ScriptedAI
        {
            npc_kin_garoth_garothi_annihilatorAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                ApplyAllImmunities(true);
                me->AddUnitState(UNIT_STATE_ROOT);
                me->SetReactState(REACT_PASSIVE);
            }

            enum mEvents
            {
                EventAnnihilation  = 1
            };

            bool l_Initializing = true;

            bool CanFly() override
            {
                return false;
            }

            bool IgnoreFlyingMovementFlagsUpdate() override
            {
                return true;
            }

            void GetDamageMultiplier(float& p_DamageMulti) override
            {
                if (IsHeroic() || IsMythic())
                    p_DamageMulti = 17.0f;
                else
                    p_DamageMulti = 20.0f;
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetInCombatWithZone();
                events.Reset();
                me->AddUnitState(UNIT_STATE_ROOT);
                me->SetReactState(REACT_PASSIVE);
                l_Initializing = true;
                me->SetAuraStack(eSpells::SpellInitializing, me, 30);
                me->CastSpell(me, eSpells::SpellInitializing, false);
                events.ScheduleEvent(mEvents::EventAnnihilation, urand(5, 10) * IN_MILLISECONDS);
            }

            void OnSpellFinishedSuccess(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellInitializing:
                    {
                        me->ClearUnitState(UNIT_STATE_ROOT);

                        Position l_Pos;
                        me->GetNearPosition(l_Pos, 25.0f, 0.0f);
                        me->SetWalk(false);
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MovePoint(2, l_Pos.m_positionX, l_Pos.m_positionY, l_Pos.m_positionZ);
                        break;
                    }
                    default:
                        break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case 2:
                    {
                        me->SetReactState(REACT_AGGRESSIVE);
                        l_Initializing = false;
                        break;
                    }
                    default:
                        break;
                }
            }

            void SearchTarget()
            {
                DoCast(eSpells::SpellAnnihilation);
                std::list<Player*> playerList;
                GetPlayerListInGrid(playerList, me, 60.0f);
                playerList.remove_if(CheckDeadOrGmTarget());
                playerList.remove_if([this](Player* p_Player) -> bool
                {
                    if (p_Player == nullptr || p_Player->GetExactDist2d(&g_PosOutRoom) < 23.0f)
                        return true;

                    return false;
                });
                if (playerList.size() > 5)
                    JadeCore::RandomResizeList(playerList, 5);

                for (auto player : playerList)
                {
                    Position l_Dest;
                    player->GetFirstCollisionPosition(l_Dest, frand(6.0f, 7.0f), frand(0.0f, 2 * M_PI));
                    l_Dest.m_positionZ = 1873.65f;
                    me->CastSpell(l_Dest, eSpells::SpellAnnihilationSummon, true);
                    me->CastSpell(l_Dest, eSpells::SpellAnnihilationMissile, true);
                }
            }

            void JustDied(Unit* p_Killer) override
            {
                me->DespawnOrUnsummon(5000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (l_Initializing || !UpdateVictim())
                    return;

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    if (me->GetDistance2d(g_PosOutRoom.GetPositionX(), g_PosOutRoom.GetPositionY()) <= 13.0f)
                    {
                        if (Creature* l_Kingaroth = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::NpcKingaroth)))
                        {
                             if (l_Kingaroth->AI())
                             {
                                 l_Kingaroth->AI()->EnterEvadeMode();
                                 return;
                             }
                        }
                    }
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case mEvents::EventAnnihilation:
                            SearchTarget();
                            events.ScheduleEvent(mEvents::EventAnnihilation, urand(20 * IN_MILLISECONDS, 30 * IN_MILLISECONDS));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_kin_garoth_garothi_annihilatorAI(p_Creature);
        }
};

/// Last Update 7.3.5 Build 26365
/// Annihilation - 246664
class spell_kin_garoth_annihilation : public SpellScriptLoader
{
    public:
        spell_kin_garoth_annihilation() : SpellScriptLoader("spell_kin_garoth_annihilation") { }

        class spell_kin_garoth_annihilation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_kin_garoth_annihilation_SpellScript);

            void CountTargets(std::list<WorldObject*>& p_Targets)
            {
                l_CountTargets = int32(p_Targets.size());
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                WorldLocation const* l_Dest = GetExplTargetDest();
                if (l_CountTargets || !l_Caster || !l_Dest)
                    return;

                l_Caster->CastSpell(l_Dest, eSpells::SpellAnnihilationBlast, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_kin_garoth_annihilation_SpellScript::CountTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                AfterCast += SpellCastFn(spell_kin_garoth_annihilation_SpellScript::HandleAfterCast);
            }

        private:
            int32 l_CountTargets;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_kin_garoth_annihilation_SpellScript();
        }
};

/// 125462 - Apocalypse Blast Stalker
class npc_kin_garoth_apocalypse_blast : public CreatureScript
{
    public:
        npc_kin_garoth_apocalypse_blast() : CreatureScript("npc_kin_garoth_apocalypse_blast") { }

        struct npc_kin_garoth_apocalypse_blastAI : public ScriptedAI
        {
            npc_kin_garoth_apocalypse_blastAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->AddUnitState(UNIT_STATE_ROOT);
                events.Reset();
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case 0:
                        events.CancelEvent(eEvents::EventApocalypseBlast);
                        break;
                    case 1:
                        events.ScheduleEvent(eEvents::EventApocalypseBlast, 1 * IN_MILLISECONDS);
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eEvents::EventApocalypseBlast:
                        {
                            me->CastSpell(me, eSpells::ApocalypseBlast, true);
                            events.ScheduleEvent(eEvents::EventApocalypseBlast, urand(2, 3) * IN_MILLISECONDS);
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
            return new npc_kin_garoth_apocalypse_blastAI(p_Creature);
        }
};
/// 122585 - Detonation Charge
class npc_kin_garoth_detonation_charge : public CreatureScript
{
    public:
        npc_kin_garoth_detonation_charge() : CreatureScript("npc_kin_garoth_detonation_charge") { }

        struct npc_kin_garoth_detonation_chargeAI : public ScriptedAI
        {
            npc_kin_garoth_detonation_chargeAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                me->SetReactState(REACT_PASSIVE);
                me->AddUnitState(UNIT_STATE_ROOT);
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->CastSpell(me, eSpells::FireMine, true);
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == 0)
                    me->CastSpell(me, eSpells::FlameReverberation, true);
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex /*p_EffIndex*/) override
            {
                if (p_Targets.empty())
                    return;

                switch (p_Spell->GetSpellInfo()->Id)
                {
                    case eSpells::FlameReverberation:
                    {
                        p_Targets.remove_if([this](WorldObject* p_Target) -> bool
                        {
                            if (!p_Target || me->GetExactDist2d(p_Target) > 4.0f)
                                return true;

                            return false;
                        });
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_kin_garoth_detonation_chargeAI(p_Creature);
        }
};

/// Last Update 7.3.5 - 26365
/// Reverberating Decimation - 249680
class spell_kin_garoth_reverberating_decimation : public SpellScriptLoader
{
    public:
        spell_kin_garoth_reverberating_decimation() : SpellScriptLoader("spell_kin_garoth_reverberating_decimation") { }

        class spell_kin_garoth_reverberating_decimation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_kin_garoth_reverberating_decimation_SpellScript);

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::ReverberatingDecimationMissile, true);
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_kin_garoth_reverberating_decimation_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_kin_garoth_reverberating_decimation_SpellScript();
        }
};

/// NPC 125646 - Empowered Ruiner Stalker
class npc_kin_garoth_empowered_ruiner_stalker : public CreatureScript
{
    public:
        npc_kin_garoth_empowered_ruiner_stalker() : CreatureScript("npc_kin_garoth_empowered_ruiner_stalker") { }

        struct npc_kin_garoth_empowered_ruiner_stalkerAI : public ScriptedAI
        {
            npc_kin_garoth_empowered_ruiner_stalkerAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum mEvents
            {
                EventRuinationAT  = 1,
                EventRuinationCheck
            };

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->AddUnitState(UNIT_STATE_ROOT);
                events.Reset();
                events.ScheduleEvent(mEvents::EventRuinationAT, 100);
                events.ScheduleEvent(mEvents::EventRuinationCheck, 200);
            }

            void SearchTarget()
            {
                std::list<Player*> playerList;
                GetPlayerListInGrid(playerList, me, 3.0f);
                playerList.remove_if(CheckDeadOrGmTarget());
                if (playerList.empty())
                {
                    if (!me->HasAura(eSpells::RuinationAura))
                        DoCast(eSpells::RuinationAura);
                }
                else
                {
                    if (me->HasAura(eSpells::RuinationAura))
                        me->RemoveAura(eSpells::RuinationAura);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case mEvents::EventRuinationAT:
                            DoCast(eSpells::RuinationAreaTrigger, true);
                            DoCast(eSpells::RuinationAura, true);
                            break;
                        case mEvents::EventRuinationCheck:
                            SearchTarget();
                            events.ScheduleEvent(mEvents::EventRuinationCheck, 200);
                            break;
                        default:
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_kin_garoth_empowered_ruiner_stalkerAI(p_Creature);
        }
};

/// Last Update 7.3.5 Build 26365
/// Ruination - 249455
class spell_kin_garoth_ruination : public SpellScriptLoader
{
    public:
        spell_kin_garoth_ruination() : SpellScriptLoader("spell_kin_garoth_ruination") { }

        class spell_kin_garoth_ruination_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_kin_garoth_ruination_SpellScript);

            void CountTargets(std::list<WorldObject*>& p_Targets)
            {
                l_CountTargets = int32(p_Targets.size());
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                WorldLocation const* l_Dest = GetExplTargetDest();
                if (l_CountTargets || !l_Caster || !l_Dest)
                    return;

                l_Caster->CastSpell(l_Dest, eSpells::RuinationBlast, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_kin_garoth_ruination_SpellScript::CountTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                AfterCast += SpellCastFn(spell_kin_garoth_ruination_SpellScript::HandleAfterCast);
            }

        private:
            int32 l_CountTargets;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_kin_garoth_ruination_SpellScript();
        }
};

/// Last Update 7.3.5 - 26365
/// Explosions - 251060
class spell_kin_garoth_explosions : public SpellScriptLoader
{
    public:
        spell_kin_garoth_explosions() : SpellScriptLoader("spell_kin_garoth_explosions") { }

        class spell_kin_garoth_explosions_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_kin_garoth_explosions_SpellScript);

            void CorrectTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr)
                        return true;

                    if (p_Object->GetEntry() != eMisc::NpcOutro)
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_kin_garoth_explosions_SpellScript::CorrectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_kin_garoth_explosions_SpellScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_kin_garoth()
{
    /// < Boss >
    new boss_kin_garoth();
    new spell_kin_garoth_energy();
    new spell_kin_garoth_diabolic_bomb_damage();
    new spell_at_kin_garoth_diabolic_bomb();
    new spell_kin_garoth_apocalypse_blast();

    ///< Creatures
    new spell_kin_garoth_initializing();
    new npc_kin_garoth_garothi_decimator();
    new spell_kin_garoth_decimator();
    new npc_kin_garoth_garothi_demolisher();
    new spell_kin_garoth_demolish();
    new spell_kin_garoth_demolish_aura();
    new npc_kin_garoth_garothi_annihilator();
    new spell_kin_garoth_annihilation();
    new npc_kin_garoth_apocalypse_blast();
    new npc_kin_garoth_detonation_charge();

    ///< Mythic
    new spell_kin_garoth_reverberating_decimation();
    new npc_kin_garoth_empowered_ruiner_stalker();
    new spell_kin_garoth_ruination();

    ///< Outro
    new spell_kin_garoth_explosions();
}
#endif
