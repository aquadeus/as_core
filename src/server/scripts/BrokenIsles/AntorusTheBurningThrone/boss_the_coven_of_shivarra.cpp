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
    ShivanPactAT           = 253189, ///< Blizz casted this spell but without Area Trigger data
    ShivanPactAura         = 253203,
    SenseofDreadAura       = 253752,
    SenseofDreadAuraTrigger= 253753,
    SenseofDreadDamageAura = 258018,
    BossCovenDefeated      = 250806,

    ///<Torment of the Titans
    TormentofAmanThulCast  = 250335,
    TormentofKhazgorothCast= 250333,
    TormentofNorgannonCast = 250334,
    TormentofGolgannethCast= 249793,
    ///< Triggers after cast, for summon npc's
    TormentofAmanThul      = 259068,
    TormentofKhazgoroth    = 259066,
    TormentofNorgannon     = 259069,
    TormentofGolganneth    = 259070,
    ///< Special For Intro Visual Effect
    TormentofAmanThulIntro   = 252479, /// Thuraya
    TormentofKhazgorothIntro = 244733, /// Noura
    TormentofNorgannonIntro  = 244740, /// Asara
    TormentofGolgannethIntro = 244756, /// Diima

    ///< Asara, Mother of Night
    ShadowBlades           = 246329,
    ShadowBladesSummon     = 245281,
    ShadowBladesAura       = 245290, ///< Aura for call ShadowBladesDamage
    ShadowBladesAuraCosmetic = 258932,
    ShadowBladesDamage     = 246374,
    TouchofDarkness        = 245303,
    /// < Heroic >
    StormofDarkness        = 252861,
    StormofDarknessDamage  = 253020,
    StormofDarknessATNotSafe = 254574,
    StormofDarknessATSafe    = 255232,

    ///< Noura, Mother of Flames
    FieryStrike            = 244899,
    WhirlingSaberTarget    = 246486,
    WhirlingSaber          = 245627,
    WhirlingSaberSummon    = 245629,
    WhirlingSaberAT        = 245630,
    WhirlingSaberDamage    = 245634,
    /// < Heroic >
    FulminatingPulseTarget = 253429,
    FulminatingPulseAura   = 253520,
    FulminatingPulseDamage = 253538,
    FulminatingBurst       = 253588, ///< used when FulminatingPulseAura expired time

    ///< Diima, Mother of Gloom
    FlashFreeze            = 245518,
    ChilledBloodTarget     = 245520,
    ChilledBloodMissile    = 245532,
    ChilledBloodAura       = 245586,
    ChilledBloodStun       = 256356,
    /// < Heroic >
    OrbofFrostTarget       = 253649,
    OrbofFrostAT           = 253650,
    OrbofFrostDamage       = 253690,
    OrbofFrostAuraSlow     = 253697,

    ///< Thu'raya, Mother of the Cosmos (Mythic)
    TouchoftheCosmos       = 250648,
    CosmicGlareTarget      = 250752,
    CosmicGlareAura1       = 250757,
    CosmicGlareAura2       = 250755,
    CosmicGlareSummon      = 250763,
    CosmicGlareDamage      = 250912,

    ///< Creatures
    /// < NpcEventAmanThuls >
    MachinationsofAmanThul = 250095,
    MachinationsofAmanThulAura = 250097,
    /// < NpcEventGolganneth >
    FuryofGolganneth       = 246739,
    FuryofGolgannethAOEAT  = 246772, ///< Cast on target FuryofGolgannethAT
    FuryofGolgannethAT     = 246763,
    FuryofGolgannethDamage = 246770,
    FuryofGolgannethAOEAura = 258065, ///< Cast on target FuryofGolgannethAura
    FuryofGolgannethAura   = 260806,
    /// < NpcEventKhazgoroth >
    FlamesofKhazgoroth     = 245671,
    FlamesofKhazgorothDamage = 245674,
    /// < NpcEventNorgannon >
    VisageoftheTitan = 249863,
    SpectralArmyofNorgannonAT = 245910,
    SpectralArmyofNorgannonDamage = 245921
};

enum eEvents
{
    ///< Asara, Mother of Night
    EventShadowBlades = 1,
    EventStormofDarkness,
    EventTouchofDarkness,
    ///< Noura, Mother of Flames
    EventFieryStrike,
    EventWhirlingSaber,
    EventFulminatingPulse,
    ///< Diima, Mother of Gloom
    EventFlashFreeze,
    EventChilledBlood,
    EventOrbofFrost,
    ///< Thu'raya, Mother of the Cosmos (Mythic)
    EventTouchoftheCosmos,
    EventCosmicGlare
};

enum eMisc
{
    NpcAmanThulsSoul  = 126084,
    NpcGolgannethSoul = 122814,
    NpcKhazgorothSoul = 122781,
    NpcNorgannonSoul  = 122809,
    NpcEventAmanThuls = 125837,
    NpcEventGolganneth= 124164,
    NpcEventKhazgoroth= 124166,
    NpcEventNorgannon = 123503,
    NpcShadowBlade    = 123086,
    NpcCosmicGlare    = 126320
};

enum eAchievCovens
{
    AchievementRememberTheTitans         = 12046,
    AchievementMythicTheCovenofShivarra  = 12000
};

enum eActions
{
    ActionCancelEvents,
    ActionRepeatEvents,
    ActionNextTormentNpc,
    ActionEvade,
    ActionStartEventThuraya
};

static std::vector<uint32> g_CovensEntrys =
{
    eCreatures::NpcAsaraMotherOfNight,
    eCreatures::NpcNouraMotherOfFlames,
    eCreatures::NpcDiimaMotherofGloom,
    eCreatures::NpcThurayaMotherOfTheCosmos
};

struct DataSpawnSummon
{
    Position    SpawnPos;
    uint32      SpawnEntry;
};

static std::array<std::vector<DataSpawnSummon>, 4> g_SpawnSummons =
{
    {
        ///< Torment of the Titans - Torment of AmanThul
        {
            { { -12649.5f, -2787.94f, 2475.15f, 5.16065f }, eMisc::NpcEventAmanThuls },
            { { -12602.6f, -2804.43f, 2475.37f, 3.58119f }, eMisc::NpcEventAmanThuls },
            { { -12667.2f, -2834.27f, 2475.08f, 0.426257f}, eMisc::NpcEventAmanThuls },
            { { -12618.8f, -2851.78f, 2475.34f, 2.01902f }, eMisc::NpcEventAmanThuls }
        },
        ///< Torment of the Titans - Torment of Khazgorot
        {
            { { -12612.1f, -2842.18f, 2475.54f, 2.31361f }, eMisc::NpcEventKhazgoroth },
            { { -12656.8f, -2842.03f, 2475.55f, 0.808043f }, eMisc::NpcEventKhazgoroth},
            { { -12586.8f, -2819.73f, 2476.91f, 3.13858f}, eMisc::NpcEventKhazgoroth  },
            { { -12634.9f, -2781.54f, 2476.91f, 4.72128f }, eMisc::NpcEventKhazgoroth },
            { { -12634.3f, -2859.33f, 2476.89f, 1.57006f }, eMisc::NpcEventKhazgoroth },
            { { -12677.0f, -2820.13f, 2476.94f, 0.0032677f }, eMisc::NpcEventKhazgoroth },
            { { -12656.6f, -2797.45f, 2475.54f, 5.48286f}, eMisc::NpcEventKhazgoroth  },
            { { -12612.2f, -2797.81f, 2475.54f, 3.95917f }, eMisc::NpcEventKhazgoroth }
        },
        ///< Torment of the Titans - Torment of Norgannon
        {
            { { -12644.8f, -2780.8f, 2476.1f, 4.7037f }, eMisc::NpcEventNorgannon  },
            { { -12634.7f, -2858.38f, 2476.08f, 1.56893f }, eMisc::NpcEventNorgannon  },
            { { -12594.2f, -2819.78f, 2476.11f, 3.12464f }, eMisc::NpcEventNorgannon  },
            { { -12654.7f, -2860.16f, 2476.11f, 1.56893f }, eMisc::NpcEventNorgannon  },
            { { -12673.6f, -2829.97f, 2476.13f, 6.22161f }, eMisc::NpcEventNorgannon  },
            { { -12654.9f, -2779.8f, 2476.1f, 4.7037f }, eMisc::NpcEventNorgannon  },
            { { -12634.3f, -2781.25f, 2476.1f, 4.7037f }, eMisc::NpcEventNorgannon  },
            { { -12592.7f, -2800.43f, 2476.11f, 3.12464f }, eMisc::NpcEventNorgannon  },
            { { -12672.8f, -2819.89f, 2476.12f, 6.22161f }, eMisc::NpcEventNorgannon  },
            { { -12614.0f, -2779.59f, 2476.1f, 4.7037f }, eMisc::NpcEventNorgannon  },
            { { -12593.7f, -2830.61f, 2476.12f, 3.12464f }, eMisc::NpcEventNorgannon  },
            { { -12673.5f, -2799.74f, 2476.12f, 6.22161f }, eMisc::NpcEventNorgannon  },
            { { -12614.1f, -2859.72f, 2476.07f, 1.56893f }, eMisc::NpcEventNorgannon  },
            { { -12645.1f, -2859.03f, 2476.1f, 1.56893f }, eMisc::NpcEventNorgannon  },
            { { -12624.0f, -2858.77f, 2476.07f, 1.56893f }, eMisc::NpcEventNorgannon  },
            { { -12592.7f, -2840.41f, 2476.11f, 3.12464f }, eMisc::NpcEventNorgannon  },
            { { -12593.7f, -2809.54f, 2476.12f, 3.12464f }, eMisc::NpcEventNorgannon  },
            { { -12623.6f, -2780.66f, 2476.1f, 4.7037f }, eMisc::NpcEventNorgannon  },
            { { -12673.5f, -2839.49f, 2476.13f, 6.22161f }, eMisc::NpcEventNorgannon  },
            { { -12672.9f, -2809.39f, 2476.12f, 6.22161f }, eMisc::NpcEventNorgannon  }
        },
        ///< Torment of the Titans - Torment of Golganneth
        {
            { { -12590.5f, -2820.04f, 2475.17f, 3.14524f }, eMisc::NpcEventGolganneth  },
            { { -12634.5f, -2780.56f, 2475.23f, 4.71457f }, eMisc::NpcEventGolganneth  },
            { { -12676.6f, -2820.15f, 2475.35f, 0.0124727f }, eMisc::NpcEventGolganneth  },
            { { -12634.4f, -2860.38f, 2475.23f, 1.57003f }, eMisc::NpcEventGolganneth  }
        }
    }
};

Position const g_PosCenterRoom = { -12634.4492f, -2819.4736f, 2475.4741f, 6.248f };
Position const g_PosDiimaJump = { -12605.0469f, -2849.4302f, 2480.9294f, 5.74f };
Position const g_PosAsaraJump = { -12604.9678f, -2790.1750f, 2480.6326f, 0.90f };
Position const g_PosNouraJump = { -12664.5703f, -2789.7864f, 2480.8540f, 2.18f };
Position const g_PosThurayaJump = { -12664.3145f, -2849.8462f, 2480.9219f, 4.039f };

std::array<std::vector<Position>, 4> g_StormofDarknessPos =
{
    {
        {
            {
                { -12658.82f, -2828.738f, 2475.47f },
                { -12658.82f, -2795.248f, 2475.47f },
                { -12642.31f, -2828.738f, 2475.47f },
                { -12625.79f, -2845.482f, 2475.47f },
                { -12625.79f, -2795.248f, 2475.47f },
                { -12609.28f, -2828.738f, 2475.47f }
            }
        },
        {
            {
                { -12658.82f, -2845.482f, 2475.47f },
                { -12658.82f, -2795.248f, 2475.47f },
                { -12642.31f, -2795.248f, 2475.47f },
                { -12625.79f, -2795.248f, 2475.47f },
                { -12609.28f, -2828.738f, 2475.47f },
                { -12609.28f, -2795.248f, 2475.47f }
            }
        },
        {
            {
                { -12658.82f, -2845.482f, 2475.47f },
                { -12658.82f, -2828.738f, 2475.47f },
                { -12658.82f, -2795.248f, 2475.47f },
                { -12625.79f, -2795.248f, 2475.47f },
                { -12609.28f, -2845.482f, 2475.47f },
                { -12609.28f, -2828.738f, 2475.47f }
            }
        },
        {
            {
                { -12658.82f, -2845.482f, 2475.47f },
                { -12658.82f, -2811.992f, 2475.47f },
                { -12658.82f, -2795.248f, 2475.47f },
                { -12625.79f, -2845.482f, 2475.47f },
                { -12625.79f, -2811.992f, 2475.47f },
                { -12609.28f, -2811.992f, 2475.47f }
            }
        }
    }
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

enum eTalks
{
    ///< General
    EMOTE_CAST_MACHINATIONS_OF_AMANTHUL = 3,
    EMOTE_CAST_FURY_OF_GOLGANNETH = 4,
    EMOTE_CAST_FLAMES_OF_KHAZGOROTH = 5,
    EMOTE_CAST_SPECTRAL_ARMY_OF_NORGANNON = 6,
    ///< Asara
    TALK_ASARA_AGGRO = 0,
    TALK_ASARA_CAST_STORM_OF_DARKNESS = 1,
    TALK_ASARA_SLAY = 2,
    ///< Noura
    TALK_NOURA_AGGRO = 0,
    TALK_NOURA_CAST_WHIRLING_SABER = 1,
    TALK_NOURA_SLAY = 2,
    ///< Diima
    TALK_DIIMA_AGGRO = 0,
    TALK_DIIMA_CAST_CHILLED_BLOOD = 1,
    TALK_DIIMA_SLAY = 2,
    ///< Thuraya
    TALK_THURAYA_CAST_COSMIC_GLARE = 0,
    TALK_THURAYA_SLAY = 1,
};

static bool CheckDistanceForBosses(Creature* p_Creature)
{
    if (!p_Creature || !p_Creature->HasAura(eSpells::ShivanPactAT))
        return false;

    InstanceScript* l_Instance = p_Creature->GetInstanceScript();
    if (!l_Instance)
        return false;

    bool pCheckDistance = false;
    for (const uint32 & l_Itr : g_CovensEntrys)
    {
        if (p_Creature->GetEntry() == l_Itr)
            continue;

        if (Creature* l_Covens = Creature::GetCreature(*p_Creature, l_Instance->GetData64(l_Itr)))
        {
            if (!l_Covens->HasAura(eSpells::ShivanPactAT))
                continue;

            if (p_Creature->GetExactDist2d(l_Covens) <= 18.0f)
            {
                pCheckDistance = true;
                break;
            }
        }
    }

    if (!pCheckDistance)
    {
        if (p_Creature->HasAura(eSpells::ShivanPactAura))
            p_Creature->RemoveAurasDueToSpell(eSpells::ShivanPactAura);
    }
    else
    {
        if (!p_Creature->HasAura(eSpells::ShivanPactAura))
            p_Creature->CastSpell(p_Creature, eSpells::ShivanPactAura, true);
    }

    return pCheckDistance;
}

static void CombatBossesWithZone(Creature* p_Creature)
{
    if (!p_Creature)
        return;

    InstanceScript* l_Instance = p_Creature->GetInstanceScript();
    if (!l_Instance)
        return;

    for (const uint32 & l_Itr : g_CovensEntrys)
    {
        if (p_Creature->GetEntry() == l_Itr)
            continue;

        if (Creature* l_Covens = Creature::GetCreature(*p_Creature, l_Instance->GetData64(l_Itr)))
        {
            if (l_Covens->isInCombat() || l_Covens->HasUnitState(UNIT_STATE_EVADE))
                continue;

            l_Covens->SetInCombatWithZone();
        }
    }
}

Player* SelectRandomPlayer(Creature* l_Coven, float l_Radius)
{
    Map* map = l_Coven->GetMap();
    if (!map->IsDungeon())
        return nullptr;

    Map::PlayerList const &PlayerList = map->GetPlayers();
    if (PlayerList.isEmpty())
        return nullptr;

    std::list<Player*> temp;
    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
        if (l_Coven->IsWithinDistInMap(i->getSource(), l_Radius) && !i->getSource()->isGameMaster())
            temp.push_back(i->getSource());

    if (!temp.empty())
    {
        std::list<Player*>::const_iterator j = temp.begin();
        advance(j, rand()%temp.size());
        return (*j);
    }
    return nullptr;
}

static void DiedAllBosses(Creature* p_Creature)
{
    if (!p_Creature)
        return;

    InstanceScript* l_Instance = p_Creature->GetInstanceScript();
    if (!l_Instance)
        return;

    Player* l_Player = SelectRandomPlayer(p_Creature, 140.0f);
    if (!l_Player)
        return;

    for (const uint32 & l_Itr : g_CovensEntrys)
    {
        if (p_Creature->GetEntry() == l_Itr)
            continue;

        if (Creature* l_Covens = Creature::GetCreature(*p_Creature, l_Instance->GetData64(l_Itr)))
        {
            if (!l_Covens->isAlive())
                continue;

            l_Player->Kill(l_Covens);
        }
    }
}

static void ApplyBerserkForBosses(Creature* p_Creature)
{
    if (!p_Creature)
        return;

    InstanceScript* l_Instance = p_Creature->GetInstanceScript();
    if (!l_Instance)
        return;

    for (const uint32 & l_Itr : g_CovensEntrys)
    {
        if (Creature* l_Covens = Creature::GetCreature(*p_Creature, l_Instance->GetData64(l_Itr)))
        {
            if (l_Covens->HasAura(eSharedSpells::SpellBerserk))
                continue;

            l_Covens->CastSpell(l_Covens, eSharedSpells::SpellBerserk, true);
        }
    }
}

static bool CheckBossInRoom(Unit const* me)
{
    if (me->GetPositionZ() < 2470.0f)
        return false;

    float min_x = -12680.9f;
    float max_x = -12582.5f;
    float min_y = -2870.0f;
    float max_y = -2770.0f;
    return !(
        me->GetPositionX() < min_x ||
        me->GetPositionX() > max_x ||
        me->GetPositionY() < min_y ||
        me->GetPositionY() > max_y
        );
}

class boss_asara_mother_night : public CreatureScript
{
    public:
        boss_asara_mother_night() : CreatureScript("boss_asara_mother_night") { }

        struct boss_asara_mother_nightAI : public BossAI
        {
            boss_asara_mother_nightAI(Creature* creature) : BossAI(creature, eData::DataTheCovenOfShivarra)
            {
                ApplyAllImmunities(true);
                DoCast(eSpells::TormentofNorgannonIntro);
                me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);
                mCanFly = false;
                m_CountTormentSouls = 0;
                m_CountCastTorment = 0;
                m_CountCovens = 0;
                mIsInEvade = false;
                mEnterCombat = false;
                m_CountCovensMythic = 0;
                m_BerserkTime = 0;
            }

            void ClearOther()
            {
                me->DespawnAreaTriggersInArea({ eSpells::ShivanPactAT, eSpells::StormofDarknessATNotSafe, eSpells::StormofDarknessATSafe, eSpells::WhirlingSaberAT, eSpells::OrbofFrostAT, eSpells::FuryofGolgannethAT }, 200.0f, false);
                me->DespawnCreaturesInArea({ eMisc::NpcEventGolganneth, eMisc::NpcEventKhazgoroth, eMisc::NpcEventNorgannon, eMisc::NpcShadowBlade, eMisc::NpcEventAmanThuls, eMisc::NpcCosmicGlare }, 200.0f);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SenseofDreadAuraTrigger);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SenseofDreadDamageAura);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::StormofDarknessDamage);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::FieryStrike);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::FulminatingPulseAura);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::FlashFreeze);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::ChilledBloodAura);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::ChilledBloodStun);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::MachinationsofAmanThulAura);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::FuryofGolgannethAT);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::FuryofGolgannethAura);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpectralArmyofNorgannonDamage);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::CosmicGlareAura1);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::CosmicGlareAura2);
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            bool CanTargetOutOfLOSXYZ(float l_X, float l_Y, float l_Z) override
            {
                return true;
            }

            bool IgnoreFlyingMovementFlagsUpdate() override
            {
                return true;
            }

            bool CanFly() override
            {
                return mCanFly;
            }

            bool IsEasyDifficulty()
            {
                return (IsNormal() || IsLFR());
            }

            void CorrectHealthWithBosses()
            {
                if (!me || !me->isInCombat() || !me->isAlive())
                    return;

                Player* l_Player = SelectRandomPlayer(me, 140.0f);
                if (!l_Player)
                    return;

                for (const uint32 & l_Itr : g_CovensEntrys)
                {
                    if (me->GetEntry() == l_Itr)
                        continue;

                    if (Creature* l_Covens = Creature::GetCreature(*me, instance->GetData64(l_Itr)))
                    {
                        if (!l_Covens->isAlive())
                            continue;

                        if (l_Covens->GetHealth() > me->GetHealth())
                        {
                            if (me->GetHealth() <= 0)
                                l_Player->Kill(l_Covens);
                            else
                                l_Covens->SetHealth(me->GetHealth());
                        }
                        else if (l_Covens->GetHealth() < me->GetHealth())
                        {
                            if (l_Covens->GetHealth() <= 0)
                                l_Player->Kill(me);
                            else
                                me->SetHealth(l_Covens->GetHealth());
                        }
                    }
                }
            }

            void FrameDisengage(bool l_Evade)
            {
                for (const uint32 & l_Itr : g_CovensEntrys)
                {
                    if (me->GetEntry() == l_Itr)
                        continue;

                    if (Creature* l_Covens = Creature::GetCreature(*me, instance->GetData64(l_Itr)))
                    {
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, l_Covens);
                        if (l_Evade)
                        {
                           if (l_Covens->AI())
                           {
                               l_Covens->AI()->DoAction(eActions::ActionEvade);
                               l_Covens->AI()->EnterEvadeMode();
                           }
                        }
                    }
                }
            }

            void AddFrameEngageForBosses()
            {
                if (Creature* l_Noura = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcNouraMotherOfFlames)))
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, l_Noura);

                if (Creature* l_Diima = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcDiimaMotherofGloom)))
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, l_Diima);

                if (IsMythic())
                   if (Creature* l_Thuraya = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcThurayaMotherOfTheCosmos)))
                       instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, l_Thuraya);
            }

            void SetTargetStormofDarkness(uint64 pGuid)
            {
                if (std::find(m_StormofDarknessTargets.begin(), m_StormofDarknessTargets.end(), pGuid) == m_StormofDarknessTargets.end())
                    m_StormofDarknessTargets.push_back(pGuid);
            }

            bool NearSafeStormofDarkness(uint64 pGuid)
            {
                return (std::find(m_StormofDarknessTargets.begin(), m_StormofDarknessTargets.end(), pGuid) != m_StormofDarknessTargets.end());
            }

            void SetClearsStormofDarkness()
            {
                m_StormofDarknessTargets.clear();
            }

            void SetClearStormofDarkness(uint64 pGuid)
            {
                if (std::find(m_StormofDarknessTargets.begin(), m_StormofDarknessTargets.end(), pGuid) != m_StormofDarknessTargets.end())
                    m_StormofDarknessTargets.remove(pGuid);
            }

            void Reset() override
            {
                _Reset();
                events.Reset();
                ClearOther();
                SetFlyMode(false);
                me->SetReactState(REACT_AGGRESSIVE);
                FrameDisengage(false);
                m_CountTormentSouls = 0;
                m_CountCastTorment = 0;
                mCanFly = false;
                m_CountCovens = 0;
                mIsInEvade = false;
                mEnterCombat = false;
                m_TormentSpells = { eSpells::TormentofKhazgorothCast, eSpells::TormentofNorgannonCast, eSpells::TormentofGolgannethCast, eSpells::TormentofAmanThulCast };
                m_StormofDarknessTargets.clear();
                m_CountCovensMythic = 0; ///< For Mythic
                ClearDelayedOperations();
                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                m_BerserkTime = 0;
            }

            uint32 GetNextTormentEvents()
            {
                if (m_TormentSpells.empty())
                    m_TormentSpells = { eSpells::TormentofKhazgorothCast, eSpells::TormentofNorgannonCast, eSpells::TormentofGolgannethCast, eSpells::TormentofAmanThulCast };

                uint32 l_SpellIds = 0;
                if (!IsMythic())
                {
                   uint8 l_Random = urand(0, m_TormentSpells.size() - 1);
                   l_SpellIds = uint32(m_TormentSpells[l_Random]);
                   m_TormentSpells.erase(m_TormentSpells.begin() + l_Random);
                }
                else ///< For Mythic non random tactics TormentofKhazgorothCast => TormentofNorgannonCast => TormentofGolgannethCast => TormentofAmanThulCast and again
                {
                   if (m_CountCovensMythic > 3)
                       m_CountCovensMythic = 0;

                   l_SpellIds = uint32(m_TormentSpells[m_CountCovensMythic]);
                   ++m_CountCovensMythic;
                }

                return l_SpellIds;
            }

            void SetTormentEvent(bool l_OnlyCast = false)
            {
                if (!me->isInCombat())
                    return;

                if (m_CountTormentSouls >= (IsMythic() ? 4 : 3))
                    m_CountTormentSouls = 0;

                uint32 l_SpellId = GetNextTormentEvents();

                switch(m_CountTormentSouls)
                {
                    case 0:
                    {
                        if (Creature* l_Diima = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcDiimaMotherofGloom)))
                        {
                            if (l_Diima->AI())
                                l_Diima->AI()->DoAction(eActions::ActionCancelEvents);

                            l_Diima->AttackStop();
                            l_Diima->GetMotionMaster()->Clear();
                            l_Diima->GetMotionMaster()->MoveJump(g_PosDiimaJump, 50.0f, 10.0f, 0, l_SpellId);
                            ++m_CountCovens;
                        }
                        break;
                    }
                    case 1:
                    {
                        if (IsMythic())
                        {
                            if (Creature* l_Thuraya = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcThurayaMotherOfTheCosmos)))
                            {
                                if (!l_OnlyCast)
                                {
                                    if (l_Thuraya->AI())
                                        l_Thuraya->AI()->DoAction(eActions::ActionCancelEvents);

                                    l_Thuraya->AttackStop();
                                    l_Thuraya->GetMotionMaster()->Clear();
                                    l_Thuraya->GetMotionMaster()->MoveJump(g_PosThurayaJump, 50.0f, 10.0f, 0, l_SpellId);
                                }
                                else
                                    l_Thuraya->CastSpell(l_Thuraya, l_SpellId, false);
                            }
                        }
                        else
                        {
                            if (Creature* l_Noura = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcNouraMotherOfFlames)))
                            {
                                if (l_Noura->AI())
                                    l_Noura->AI()->DoAction(eActions::ActionCancelEvents);

                                l_Noura->AttackStop();
                                l_Noura->GetMotionMaster()->Clear();
                                l_Noura->GetMotionMaster()->MoveJump(g_PosNouraJump, 50.0f, 10.0f, 0, l_SpellId);
                                ++m_CountCovens;
                            }
                        }
                        break;
                    }
                    case 2:
                    {
                        if (IsMythic())
                        {
                            if (Creature* l_Noura = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcNouraMotherOfFlames)))
                            {
                                if (l_Noura->AI())
                                    l_Noura->AI()->DoAction(eActions::ActionCancelEvents);

                                l_Noura->AttackStop();
                                l_Noura->GetMotionMaster()->Clear();
                                l_Noura->GetMotionMaster()->MoveJump(g_PosNouraJump, 50.0f, 10.0f, 0, l_SpellId);
                                ++m_CountCovens;
                            }
                        }
                        else
                        {
                            DoAction(eActions::ActionCancelEvents);
                            me->AttackStop();
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveJump(g_PosAsaraJump, 50.0f, 10.0f, 0, l_SpellId);
                            ++m_CountCovens;
                        }
                        break;
                    }
                    case 3:
                    {
                        if (IsMythic())
                        {
                            DoAction(eActions::ActionCancelEvents);
                            me->AttackStop();
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveJump(g_PosAsaraJump, 50.0f, 10.0f, 0, l_SpellId);
                            ++m_CountCovens;
                        }
                        break;
                    }
                }

                m_CountTormentSouls++;
            }

            void OnSpellFinishedSuccess(SpellInfo const* p_SpellInfo) override
            {
                if (!p_SpellInfo || !me->isInCombat())
                    return;

                switch (p_SpellInfo->Id)
                {
                    case eSpells::TormentofAmanThulCast:
                    case eSpells::TormentofKhazgorothCast:
                    case eSpells::TormentofNorgannonCast:
                    case eSpells::TormentofGolgannethCast:
                    {
                        ++m_CountCastTorment;
                        me->RemoveAurasDueToSpell(eSpells::SenseofDreadAura);
                        me->RemoveAurasDueToSpell(eSpells::SenseofDreadDamageAura);
                        instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SenseofDreadAura);
                        instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SenseofDreadDamageAura);
                        instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SenseofDreadAuraTrigger);
                        if (p_SpellInfo->Id == eSpells::TormentofAmanThulCast)
                        {
                            Talk(eTalks::EMOTE_CAST_MACHINATIONS_OF_AMANTHUL);
                            me->CastSpell(me, eSpells::TormentofAmanThul, true);
                        }
                        else if (p_SpellInfo->Id == eSpells::TormentofKhazgorothCast)
                        {
                            Talk(eTalks::EMOTE_CAST_FLAMES_OF_KHAZGOROTH);
                            me->CastSpell(me, eSpells::TormentofKhazgoroth, true);
                        }
                        else if (p_SpellInfo->Id == eSpells::TormentofNorgannonCast)
                        {
                            Talk(eTalks::EMOTE_CAST_SPECTRAL_ARMY_OF_NORGANNON);
                            me->CastSpell(me, eSpells::TormentofNorgannon, true);
                        }
                        else if (p_SpellInfo->Id == eSpells::TormentofGolgannethCast)
                        {
                            Talk(eTalks::EMOTE_CAST_FURY_OF_GOLGANNETH);
                            me->CastSpell(me, eSpells::TormentofGolganneth, true);
                        }

                        if (m_CountCastTorment == 2)
                        {
                            DoAction(eActions::ActionRepeatEvents);
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveJump(g_PosCenterRoom, 50.0f, 10.0f);
                            DoAction(eActions::ActionNextTormentNpc);
                            break;
                        }

                        uint32 l_SpellId = GetNextTormentEvents();
                        me->DelayedCastSpell(me, l_SpellId, false, 1500);
                        break;
                    }
                    case eSpells::TormentofAmanThul:
                    case eSpells::TormentofKhazgoroth:
                    case eSpells::TormentofNorgannon:
                    case eSpells::TormentofGolganneth:
                    {
                        uint32 l_SummonMask = 0; ///< TormentofAmanThul
                        if (p_SpellInfo->Id == eSpells::TormentofKhazgoroth)
                            l_SummonMask = 1;
                        else if (p_SpellInfo->Id == eSpells::TormentofNorgannon)
                            l_SummonMask = 2;
                        else if (p_SpellInfo->Id == eSpells::TormentofGolganneth)
                            l_SummonMask = 3;

                        for (DataSpawnSummon l_Data : g_SpawnSummons[l_SummonMask])
                        {
                            if (!l_Data.SpawnEntry)
                                continue;

                            me->SummonCreature(l_Data.SpawnEntry, l_Data.SpawnPos, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 1800000);
                        }
                        break;
                    }
                    case eSpells::ShadowBlades:
                    {
                        for (const uint32 & l_Itr : g_CovensEntrys)
                        {
                            if (me->GetEntry() == l_Itr)
                                continue;

                            if (Creature* l_Covens = Creature::GetCreature(*me, instance->GetData64(l_Itr)))
                            {
                                if (!l_Covens->HasAura(eSpells::ShivanPactAT))
                                    continue;

                                me->SetFacingTo(me->GetAngle(l_Covens));
                                break;
                            }
                        }

                        ///< Front, Front right, Front left
                        float l_Angle = 0.0f;
                        Position l_Pos;
                        for (uint8 l_Itr = 0; l_Itr < 3; ++l_Itr)
                        {
                            l_Angle = float(l_Itr == 1 ? (-M_PI/4) : l_Itr == 2 ? (M_PI/4) : 0.0f);
                            me->GetNearPosition(l_Pos, 3.0f, l_Angle);
                            l_Pos.m_positionZ += 1.0f;
                            if (Creature* l_ShadowBlade = me->SummonCreature(eMisc::NpcShadowBlade, l_Pos, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 1800000))
                                l_ShadowBlade->SetFacingTo(me->GetAngle(l_Pos.m_positionX, l_Pos.m_positionY));
                        }
                        break;
                    }
                    case eSpells::StormofDarkness:
                    {
                        Talk(eTalks::TALK_ASARA_CAST_STORM_OF_DARKNESS);
                        m_StormofDarknessTargets.clear();
                        std::vector<Position> l_Positions = g_StormofDarknessPos[urand(0, 3)];
                        for (uint8 l_Itr = 0; l_Itr < 6; ++l_Itr)
                            me->DelayedCastSpell(&l_Positions[l_Itr], eSpells::StormofDarknessATSafe, true, l_Itr == 0 ? 1 : l_Itr * 25);

                        DoCast(g_PosCenterRoom, eSpells::StormofDarknessATNotSafe, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void EnterEvadeMode() override
            {
                if (mIsInEvade)
                    return;

                me->CastStop();
                me->InterruptSpell(CURRENT_CHANNELED_SPELL, true, true);
                me->RemoveAurasDueToSpell(eSpells::TormentofAmanThulCast);
                me->RemoveAurasDueToSpell(eSpells::TormentofKhazgorothCast);
                me->RemoveAurasDueToSpell(eSpells::TormentofNorgannonCast);
                me->RemoveAurasDueToSpell(eSpells::TormentofGolgannethCast);
                m_BerserkTime = 0;
                ClearDelayedOperations();
                ClearOther();
                BossAI::EnterEvadeMode();
                FrameDisengage(true);
                instance->AddTimedDelayedOperation(2500, [this]() -> void
                {
                    ClearOther();
                });

                mIsInEvade = true;
            }

            void JustReachedHome() override
            {
                _JustReachedHome();
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionCancelEvents:
                    {
                        mCanFly = true;
                        me->SetReactState(REACT_PASSIVE);
                        me->RemoveAurasDueToSpell(eSpells::ShivanPactAT);
                        me->RemoveAurasDueToSpell(eSpells::ShivanPactAura);
                        events.CancelEvent(eEvents::EventShadowBlades);
                        events.CancelEvent(eEvents::EventTouchofDarkness);
                        events.CancelEvent(eEvents::EventStormofDarkness);
                        SetFlyMode(true);
                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                        me->RemoveAllDoTAuras();
                        break;
                    }
                    case eActions::ActionRepeatEvents:
                    {
                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                        m_CountCastTorment = 0;
                        me->SetReactState(REACT_AGGRESSIVE);
                        DoCast(eSpells::ShivanPactAT, true);
                        events.RescheduleEvent(eEvents::EventShadowBlades, 11 * IN_MILLISECONDS);
                        events.RescheduleEvent(eEvents::EventTouchofDarkness, 1 * IN_MILLISECONDS);
                        if (!IsEasyDifficulty())
                            events.RescheduleEvent(eEvents::EventStormofDarkness, 26 * IN_MILLISECONDS);

                        mCanFly = false;
                        SetFlyMode(false);
                        break;
                    }
                    case eActions::ActionNextTormentNpc:
                    {
                        AddTimedDelayedOperation(4000, [this]() -> void
                        {
                            SetTormentEvent();
                        });
                        break;
                    }
                    default:
                        break;
                }
            }

            void RandomTextForBosses()
            {
                uint8 l_TextsNumber = urand(0, 2);
                switch(l_TextsNumber)
                {
                    case 0:
                        Talk(eTalks::TALK_ASARA_AGGRO);
                        break;
                    case 1:
                    {
                        if (Creature* l_Noura = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcNouraMotherOfFlames)))
                            if (l_Noura->AI())
                                l_Noura->AI()->Talk(eTalks::TALK_NOURA_AGGRO);
                        break;
                    }
                    case 3:
                    {
                        if (Creature* l_Diima = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcDiimaMotherofGloom)))
                            if (l_Diima->AI())
                                l_Diima->AI()->Talk(eTalks::TALK_DIIMA_AGGRO);
                        break;
                    }
                    default:
                         break;
                }
            }

            void GetDamageMultiplier(float& p_DamageMulti) override
            {
                p_DamageMulti = 10.0f;
            }

            void EnterCombat(Unit* attacker) override
            {
                RandomTextForBosses();
                me->RemoveAurasDueToSpell(eSpells::TormentofNorgannonIntro);
                _EnterCombat();
                CombatBossesWithZone(me);
                AddFrameEngageForBosses();
                DoCast(eSpells::ShivanPactAT, true);
                m_BerserkTime = 720 * IN_MILLISECONDS;
                events.ScheduleEvent(eEvents::EventShadowBlades, 11 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventTouchofDarkness, 1 * IN_MILLISECONDS);
                if (!IsEasyDifficulty())
                    events.ScheduleEvent(eEvents::EventStormofDarkness, 26 * IN_MILLISECONDS);

                if (mEnterCombat)
                    return;

                AddTimedDelayedOperation(2000, [this]() -> void
                {
                    SetTormentEvent();
                    if (IsMythic())
                    {
                        if (Creature* l_Thuraya = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcThurayaMotherOfTheCosmos)))
                        {
                            if (l_Thuraya->AI())
                                l_Thuraya->AI()->DoAction(eActions::ActionStartEventThuraya);

                            l_Thuraya->AttackStop();
                            l_Thuraya->GetMotionMaster()->Clear();
                            l_Thuraya->GetMotionMaster()->MoveJump(g_PosThurayaJump, 50.0f, 10.0f);
                        }
                    }
                });

                mEnterCombat = true;
            }

            void JustDied(Unit* p_Killer) override
            {
                me->CastStop();
                me->InterruptSpell(CURRENT_CHANNELED_SPELL, true, true);
                me->RemoveAurasDueToSpell(eSpells::TormentofAmanThulCast);
                me->RemoveAurasDueToSpell(eSpells::TormentofKhazgorothCast);
                me->RemoveAurasDueToSpell(eSpells::TormentofNorgannonCast);
                me->RemoveAurasDueToSpell(eSpells::TormentofGolgannethCast);

                instance->DoCastSpellOnPlayers(eSpells::BossCovenDefeated);
                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                if (!IsLFR() && m_CountCovens <= 2)
                    instance->DoCompleteAchievement(eAchievCovens::AchievementRememberTheTitans);

                if (IsMythic())
                    instance->DoCompleteAchievement(eAchievCovens::AchievementMythicTheCovenofShivarra);

                _JustDied();
                DiedAllBosses(me);
                ClearOther();
                FrameDisengage(false);
                ClearDelayedOperations();
                instance->AddTimedDelayedOperation(2500, [this]() -> void
                {
                    ClearOther();
                });

                me->NearTeleportTo(me->GetHomePosition());

                if (Creature* l_IllidanStormrage = me->FindNearestCreature(eCreatures::NpcIllidanStormrage, 150.0f))
                {
                    l_IllidanStormrage->GetMotionMaster()->MovePoint(102, -12634.1406f, -2641.3962f, 2499.0601f);
                    instance->AddTimedDelayedOperation(15 * TimeConstants::IN_MILLISECONDS, [l_IllidanStormrage]() -> void
                    {
                        l_IllidanStormrage->SetVisible(false);
                    });
                }

                if (Creature* l_ProphetVelen = me->FindNearestCreature(eCreatures::NpcProphetVelen, 150.0f))
                {
                    l_ProphetVelen->GetMotionMaster()->MovePoint(102, -12634.1406f, -2641.3962f, 2499.0601f);
                    instance->AddTimedDelayedOperation(17 * TimeConstants::IN_MILLISECONDS, [l_ProphetVelen]() -> void
                    {
                        l_ProphetVelen->SetVisible(false);
                    });
                }
            }

            void KilledUnit(Unit* p_Who) override
            {
              if (p_Who->IsPlayer())
                  Talk(eTalks::TALK_ASARA_SLAY);
            }

            void JustSummoned(Creature* p_Summon) override
            {
                BossAI::JustSummoned(p_Summon);
            }

            bool CanAIAttack(Unit const* p_Target) const override
            {
                return CheckBossInRoom(p_Target);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
                CorrectHealthWithBosses();
                if (!UpdateVictim())
                    return;

                if (!CheckBossInRoom(me))
                {
                    EnterEvadeMode();
                    return;
                }

                if (m_BerserkTime)
                {
                    if (m_BerserkTime <= p_Diff)
                    {
                        m_BerserkTime = 0;

                        ApplyBerserkForBosses(me);
                    }
                    else
                        m_BerserkTime -= p_Diff;
                }

                events.Update(p_Diff);
                CheckDistanceForBosses(me);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eEvents::EventShadowBlades:
                        {
                            DoCast(eSpells::ShadowBlades, true);
                            events.ScheduleEvent(eEvents::EventShadowBlades, 27600);
                            break;
                        }
                        case eEvents::EventStormofDarkness:
                        {
                            DoCast(eSpells::StormofDarkness);
                            events.ScheduleEvent(eEvents::EventStormofDarkness, 56800);
                            break;
                        }
                        case eEvents::EventTouchofDarkness:
                        {
                            DoCast(eSpells::TouchofDarkness);
                            events.ScheduleEvent(eEvents::EventTouchofDarkness, urand(6, 8) * IN_MILLISECONDS);
                            break;
                        }
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

          private:
              uint8 m_CountTormentSouls;
              std::vector<uint32> m_TormentSpells;
              uint8 m_CountCastTorment;
              bool mCanFly;
              uint8 m_CountCovens; ///< For Achiev
              bool mIsInEvade;
              bool mEnterCombat;
              std::list<uint64> m_StormofDarknessTargets;
              uint8 m_CountCovensMythic;
              uint32 m_BerserkTime;
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new boss_asara_mother_nightAI(p_Creature);
        }
};

class boss_noura_mother_flames : public CreatureScript
{
    public:
        boss_noura_mother_flames() : CreatureScript("boss_noura_mother_flames") { }

        struct boss_noura_mother_flamesAI : public BossAI
        {
            boss_noura_mother_flamesAI(Creature* creature) : BossAI(creature, eData::DataTheCovenOfShivarra)
            {
                ApplyAllImmunities(true);
                DoCast(eSpells::TormentofKhazgorothIntro);
                me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);
                m_CountCastTorment = 0;
                mCanFly = false;
                mIsInEvade = false;
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            bool CanTargetOutOfLOSXYZ(float l_X, float l_Y, float l_Z) override
            {
                return true;
            }

            bool IgnoreFlyingMovementFlagsUpdate() override
            {
                return true;
            }

            bool CanFly() override
            {
                return mCanFly;
            }

            bool IsEasyDifficulty()
            {
                return (IsNormal() || IsLFR());
            }

            void Reset() override
            {
                events.Reset();
                SetFlyMode(false);
                me->SetReactState(REACT_AGGRESSIVE);
                m_CountCastTorment = 0;
                mCanFly = false;
                mIsInEvade = false;
                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
            }

            void OnSpellFinishedSuccess(SpellInfo const* p_SpellInfo) override
            {
                if (!p_SpellInfo || !me->isInCombat())
                    return;

                switch (p_SpellInfo->Id)
                {
                    case eSpells::TormentofAmanThulCast:
                    case eSpells::TormentofKhazgorothCast:
                    case eSpells::TormentofNorgannonCast:
                    case eSpells::TormentofGolgannethCast:
                    {
                        ++m_CountCastTorment;
                        me->RemoveAurasDueToSpell(eSpells::SenseofDreadAura);
                        me->RemoveAurasDueToSpell(eSpells::SenseofDreadDamageAura);
                        instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SenseofDreadAura);
                        instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SenseofDreadDamageAura);
                        instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SenseofDreadAuraTrigger);
                        if (p_SpellInfo->Id == eSpells::TormentofAmanThulCast)
                        {
                            Talk(eTalks::EMOTE_CAST_MACHINATIONS_OF_AMANTHUL);
                            me->CastSpell(me, eSpells::TormentofAmanThul, true);
                        }
                        else if (p_SpellInfo->Id == eSpells::TormentofKhazgorothCast)
                        {
                            Talk(eTalks::EMOTE_CAST_FLAMES_OF_KHAZGOROTH);
                            me->CastSpell(me, eSpells::TormentofKhazgoroth, true);
                        }
                        else if (p_SpellInfo->Id == eSpells::TormentofNorgannonCast)
                        {
                            Talk(eTalks::EMOTE_CAST_SPECTRAL_ARMY_OF_NORGANNON);
                            me->CastSpell(me, eSpells::TormentofNorgannon, true);
                        }
                        else if (p_SpellInfo->Id == eSpells::TormentofGolgannethCast)
                        {
                            Talk(eTalks::EMOTE_CAST_FURY_OF_GOLGANNETH);
                            me->CastSpell(me, eSpells::TormentofGolganneth, true);
                        }

                        if (m_CountCastTorment == 2)
                        {
                            DoAction(eActions::ActionRepeatEvents);
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveJump(g_PosCenterRoom, 50.0f, 10.0f);
                            if (Creature* l_Asara = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcAsaraMotherOfNight)))
                                if (l_Asara->AI())
                                    l_Asara->AI()->DoAction(eActions::ActionNextTormentNpc);
                            break;
                        }

                        uint32 l_SpellId = 0;
                        if (Creature* l_Asara = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcAsaraMotherOfNight)))
                            if (l_Asara->AI())
                                if (boss_asara_mother_night::boss_asara_mother_nightAI* l_LinkAI = CAST_AI(boss_asara_mother_night::boss_asara_mother_nightAI, l_Asara->GetAI()))
                                    l_SpellId = l_LinkAI->GetNextTormentEvents();

                        if (!l_SpellId)
                            break;

                        me->DelayedCastSpell(me, l_SpellId, false, 1500);
                        break;
                    }
                    case eSpells::TormentofAmanThul:
                    case eSpells::TormentofKhazgoroth:
                    case eSpells::TormentofNorgannon:
                    case eSpells::TormentofGolganneth:
                    {
                        uint32 l_SummonMask = 0; ///< TormentofAmanThul
                        if (p_SpellInfo->Id == eSpells::TormentofKhazgoroth)
                            l_SummonMask = 1;
                        else if (p_SpellInfo->Id == eSpells::TormentofNorgannon)
                            l_SummonMask = 2;
                        else if (p_SpellInfo->Id == eSpells::TormentofGolganneth)
                            l_SummonMask = 3;

                        for (DataSpawnSummon l_Data : g_SpawnSummons[l_SummonMask])
                        {
                            if (!l_Data.SpawnEntry)
                                continue;

                            me->SummonCreature(l_Data.SpawnEntry, l_Data.SpawnPos, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 1800000);
                        }
                        break;
                    }
                    default:
                        break;
                }
            }

            void EnterEvadeMode() override
            {
                if (mIsInEvade)
                    return;

                me->CastStop();
                me->InterruptSpell(CURRENT_CHANNELED_SPELL, true, true);
                me->RemoveAurasDueToSpell(eSpells::TormentofAmanThulCast);
                me->RemoveAurasDueToSpell(eSpells::TormentofKhazgorothCast);
                me->RemoveAurasDueToSpell(eSpells::TormentofNorgannonCast);
                me->RemoveAurasDueToSpell(eSpells::TormentofGolgannethCast);
                BossAI::EnterEvadeMode();
                mIsInEvade = true;
            }

            void JustReachedHome() override
            {

            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionCancelEvents:
                    {
                        mCanFly = true;
                        me->SetReactState(REACT_PASSIVE);
                        me->RemoveAurasDueToSpell(eSpells::ShivanPactAT);
                        me->RemoveAurasDueToSpell(eSpells::ShivanPactAura);
                        events.CancelEvent(eEvents::EventFieryStrike);
                        events.CancelEvent(eEvents::EventWhirlingSaber);
                        events.CancelEvent(eEvents::EventFulminatingPulse);
                        SetFlyMode(true);
                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                        me->RemoveAllDoTAuras();
                        break;
                    }
                    case eActions::ActionRepeatEvents:
                    {
                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                        m_CountCastTorment = 0;
                        me->SetReactState(REACT_AGGRESSIVE);
                        DoCast(eSpells::ShivanPactAT, true);
                        events.RescheduleEvent(eEvents::EventFieryStrike, 12 * IN_MILLISECONDS);
                        events.RescheduleEvent(eEvents::EventWhirlingSaber, 9 * IN_MILLISECONDS);
                        if (!IsEasyDifficulty())
                            events.RescheduleEvent(eEvents::EventFulminatingPulse, 20600);

                        mCanFly = false;
                        SetFlyMode(false);
                        break;
                    }
                    case eActions::ActionEvade:
                    {
                        mIsInEvade = false;
                        break;
                    }
                    default:
                        break;
                }
            }

            void GetDamageMultiplier(float& p_DamageMulti) override
            {
                p_DamageMulti = 10.0f;
            }

            void EnterCombat(Unit* attacker) override
            {
                mIsInEvade = true;
                me->RemoveAurasDueToSpell(eSpells::TormentofKhazgorothIntro);
                CombatBossesWithZone(me);
                DoCast(eSpells::ShivanPactAT, true);
                events.ScheduleEvent(eEvents::EventFieryStrike, 11 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventWhirlingSaber, 8 * IN_MILLISECONDS);
                if (!IsEasyDifficulty())
                    events.ScheduleEvent(eEvents::EventFulminatingPulse, 20300);
            }

            void JustDied(Unit* p_Killer) override
            {
                me->CastStop();
                me->InterruptSpell(CURRENT_CHANNELED_SPELL, true, true);
                me->RemoveAurasDueToSpell(eSpells::TormentofAmanThulCast);
                me->RemoveAurasDueToSpell(eSpells::TormentofKhazgorothCast);
                me->RemoveAurasDueToSpell(eSpells::TormentofNorgannonCast);
                me->RemoveAurasDueToSpell(eSpells::TormentofGolgannethCast);
                DiedAllBosses(me);
                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                me->NearTeleportTo(me->GetHomePosition());
            }

            void KilledUnit(Unit* p_Who) override
            {
              if (p_Who->IsPlayer())
                  Talk(eTalks::TALK_NOURA_SLAY);
            }

            bool CanAIAttack(Unit const* p_Target) const override
            {
                return CheckBossInRoom(p_Target);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
                if (!UpdateVictim())
                    return;

                if (!CheckBossInRoom(me))
                {
                    if (Creature* l_Asara = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcAsaraMotherOfNight)))
                        if (l_Asara->AI())
                            l_Asara->AI()->EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);
                CheckDistanceForBosses(me);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eEvents::EventFieryStrike:
                        {
                            DoCastVictim(eSpells::FieryStrike);
                            events.ScheduleEvent(eEvents::EventFieryStrike, 11800);
                            break;
                        }
                        case eEvents::EventWhirlingSaber:
                        {
                            Talk(eTalks::TALK_NOURA_CAST_WHIRLING_SABER);
                            DoCast(eSpells::WhirlingSaberTarget);
                            events.ScheduleEvent(eEvents::EventWhirlingSaber, 35 * IN_MILLISECONDS);
                            break;
                        }
                        case eEvents::EventFulminatingPulse:
                        {
                            DoCast(eSpells::FulminatingPulseTarget);
                            events.ScheduleEvent(eEvents::EventFulminatingPulse, 40 * IN_MILLISECONDS);
                            break;
                        }
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

          private:
              uint8 m_CountCastTorment;
              bool mCanFly;
              bool mIsInEvade;
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new boss_noura_mother_flamesAI(p_Creature);
        }
};

class boss_diima_mother_gloom : public CreatureScript
{
    public:
        boss_diima_mother_gloom() : CreatureScript("boss_diima_mother_gloom") { }

        struct boss_diima_mother_gloomAI : public BossAI
        {
            boss_diima_mother_gloomAI(Creature* creature) : BossAI(creature, eData::DataTheCovenOfShivarra)
            {
                ApplyAllImmunities(true);
                DoCast(eSpells::TormentofGolgannethIntro);
                me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);
                m_CountCastTorment = 0;
                mCanFly = false;
                mIsInEvade = false;
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            bool CanTargetOutOfLOSXYZ(float l_X, float l_Y, float l_Z) override
            {
                return true;
            }

            bool IgnoreFlyingMovementFlagsUpdate() override
            {
                return true;
            }

            bool CanFly() override
            {
                return mCanFly;
            }

            bool IsEasyDifficulty()
            {
                return (IsNormal() || IsLFR());
            }

            void Reset() override
            {
                events.Reset();
                SetFlyMode(false);
                m_CountCastTorment = 0;
                me->SetReactState(REACT_AGGRESSIVE);
                mCanFly = false;
                mIsInEvade = false;
                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
            }

            void OnSpellFinishedSuccess(SpellInfo const* p_SpellInfo) override
            {
                if (!p_SpellInfo || !me->isInCombat())
                    return;

                switch (p_SpellInfo->Id)
                {
                    case eSpells::TormentofAmanThulCast:
                    case eSpells::TormentofKhazgorothCast:
                    case eSpells::TormentofNorgannonCast:
                    case eSpells::TormentofGolgannethCast:
                    {
                        ++m_CountCastTorment;
                        me->RemoveAurasDueToSpell(eSpells::SenseofDreadAura);
                        me->RemoveAurasDueToSpell(eSpells::SenseofDreadDamageAura);
                        instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SenseofDreadAura);
                        instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SenseofDreadDamageAura);
                        instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SenseofDreadAuraTrigger);
                        if (p_SpellInfo->Id == eSpells::TormentofAmanThulCast)
                        {
                            Talk(eTalks::EMOTE_CAST_MACHINATIONS_OF_AMANTHUL);
                            me->CastSpell(me, eSpells::TormentofAmanThul, true);
                        }
                        else if (p_SpellInfo->Id == eSpells::TormentofKhazgorothCast)
                        {
                            Talk(eTalks::EMOTE_CAST_FLAMES_OF_KHAZGOROTH);
                            me->CastSpell(me, eSpells::TormentofKhazgoroth, true);
                        }
                        else if (p_SpellInfo->Id == eSpells::TormentofNorgannonCast)
                        {
                            Talk(eTalks::EMOTE_CAST_SPECTRAL_ARMY_OF_NORGANNON);
                            me->CastSpell(me, eSpells::TormentofNorgannon, true);
                        }
                        else if (p_SpellInfo->Id == eSpells::TormentofGolgannethCast)
                        {
                            Talk(eTalks::EMOTE_CAST_FURY_OF_GOLGANNETH);
                            me->CastSpell(me, eSpells::TormentofGolganneth, true);
                        }

                        if (m_CountCastTorment == 2)
                        {
                            DoAction(eActions::ActionRepeatEvents);
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveJump(g_PosCenterRoom, 50.0f, 10.0f);
                            if (Creature* l_Asara = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcAsaraMotherOfNight)))
                                if (l_Asara->AI())
                                    l_Asara->AI()->DoAction(eActions::ActionNextTormentNpc);
                            break;
                        }

                        uint32 l_SpellId = 0;
                        if (Creature* l_Asara = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcAsaraMotherOfNight)))
                            if (l_Asara->AI())
                                if (boss_asara_mother_night::boss_asara_mother_nightAI* l_LinkAI = CAST_AI(boss_asara_mother_night::boss_asara_mother_nightAI, l_Asara->GetAI()))
                                    l_SpellId = l_LinkAI->GetNextTormentEvents();

                        if (!l_SpellId)
                            break;

                        me->DelayedCastSpell(me, l_SpellId, false, 1500);
                        break;
                    }
                    case eSpells::TormentofAmanThul:
                    case eSpells::TormentofKhazgoroth:
                    case eSpells::TormentofNorgannon:
                    case eSpells::TormentofGolganneth:
                    {
                        uint32 l_SummonMask = 0; ///< TormentofAmanThul
                        if (p_SpellInfo->Id == eSpells::TormentofKhazgoroth)
                            l_SummonMask = 1;
                        else if (p_SpellInfo->Id == eSpells::TormentofNorgannon)
                            l_SummonMask = 2;
                        else if (p_SpellInfo->Id == eSpells::TormentofGolganneth)
                            l_SummonMask = 3;

                        for (DataSpawnSummon l_Data : g_SpawnSummons[l_SummonMask])
                        {
                            if (!l_Data.SpawnEntry)
                                continue;

                            me->SummonCreature(l_Data.SpawnEntry, l_Data.SpawnPos, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 1800000);
                        }
                        break;
                    }
                    default:
                        break;
                }
            }

            void EnterEvadeMode() override
            {
                if (mIsInEvade)
                    return;

                me->CastStop();
                me->InterruptSpell(CURRENT_CHANNELED_SPELL, true, true);
                me->RemoveAurasDueToSpell(eSpells::TormentofAmanThulCast);
                me->RemoveAurasDueToSpell(eSpells::TormentofKhazgorothCast);
                me->RemoveAurasDueToSpell(eSpells::TormentofNorgannonCast);
                me->RemoveAurasDueToSpell(eSpells::TormentofGolgannethCast);
                BossAI::EnterEvadeMode();
                mIsInEvade = true;
            }

            void JustReachedHome() override
            {

            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionCancelEvents:
                    {
                        mCanFly = true;
                        me->SetReactState(REACT_PASSIVE);
                        me->RemoveAurasDueToSpell(eSpells::ShivanPactAT);
                        me->RemoveAurasDueToSpell(eSpells::ShivanPactAura);
                        events.CancelEvent(eEvents::EventFlashFreeze);
                        events.CancelEvent(eEvents::EventChilledBlood);
                        events.CancelEvent(eEvents::EventOrbofFrost);
                        SetFlyMode(true);
                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                        me->RemoveAllDoTAuras();
                        break;
                    }
                    case eActions::ActionRepeatEvents:
                    {
                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                        m_CountCastTorment = 0;
                        me->SetReactState(REACT_AGGRESSIVE);
                        DoCast(eSpells::ShivanPactAT, true);
                        events.RescheduleEvent(eEvents::EventFlashFreeze, 10 * IN_MILLISECONDS);
                        events.RescheduleEvent(eEvents::EventChilledBlood, 6500);
                        if (!IsEasyDifficulty())
                            events.RescheduleEvent(eEvents::EventOrbofFrost, 30 * IN_MILLISECONDS);

                        mCanFly = false;
                        SetFlyMode(false);
                        break;
                    }
                    case eActions::ActionEvade:
                    {
                        mIsInEvade = false;
                        break;
                    }
                    default:
                        break;
                }
            }

            void GetDamageMultiplier(float& p_DamageMulti) override
            {
                p_DamageMulti = 10.0f;
            }

            void EnterCombat(Unit* attacker) override
            {
                mIsInEvade = true;
                me->RemoveAurasDueToSpell(eSpells::TormentofGolgannethIntro);
                CombatBossesWithZone(me);
                DoCast(eSpells::ShivanPactAT, true);
            }

            void JustDied(Unit* p_Killer) override
            {
                me->CastStop();
                me->InterruptSpell(CURRENT_CHANNELED_SPELL, true, true);
                me->RemoveAurasDueToSpell(eSpells::TormentofAmanThulCast);
                me->RemoveAurasDueToSpell(eSpells::TormentofKhazgorothCast);
                me->RemoveAurasDueToSpell(eSpells::TormentofNorgannonCast);
                me->RemoveAurasDueToSpell(eSpells::TormentofGolgannethCast);
                DiedAllBosses(me);
                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                me->NearTeleportTo(me->GetHomePosition());
            }

            void KilledUnit(Unit* p_Who) override
            {
              if (p_Who->IsPlayer())
                  Talk(eTalks::TALK_DIIMA_SLAY);
            }

            bool CanAIAttack(Unit const* p_Target) const override
            {
                return CheckBossInRoom(p_Target);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
                if (!UpdateVictim())
                    return;

                if (!CheckBossInRoom(me))
                {
                    if (Creature* l_Asara = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcAsaraMotherOfNight)))
                        if (l_Asara->AI())
                            l_Asara->AI()->EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);
                CheckDistanceForBosses(me);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eEvents::EventFlashFreeze:
                        {
                            DoCastVictim(eSpells::FlashFreeze);
                            events.ScheduleEvent(eEvents::EventFlashFreeze, 10100);
                            break;
                        }
                        case eEvents::EventChilledBlood:
                        {
                            Talk(eTalks::TALK_DIIMA_CAST_CHILLED_BLOOD);
                            DoCast(eSpells::ChilledBloodTarget);
                            events.ScheduleEvent(eEvents::EventChilledBlood, 25400);
                            break;
                        }
                        case eEvents::EventOrbofFrost:
                        {
                            DoCast(eSpells::OrbofFrostTarget);
                            events.ScheduleEvent(eEvents::EventOrbofFrost, 30 * IN_MILLISECONDS);
                            break;
                        }
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

          private:
              uint8 m_CountCastTorment;
              bool mCanFly;
              bool mIsInEvade;
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new boss_diima_mother_gloomAI(p_Creature);
        }
};

class boss_thuraya_mother_cosmos : public CreatureScript
{
    public:
        boss_thuraya_mother_cosmos() : CreatureScript("boss_thuraya_mother_cosmos") { }

        struct boss_thuraya_mother_cosmosAI : public BossAI
        {
            boss_thuraya_mother_cosmosAI(Creature* creature) : BossAI(creature, eData::DataTheCovenOfShivarra)
            {
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK_DEST, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_SILENCE, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_STUN, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_SLEEP, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_KNOCKOUT, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_GRIP, true);
                DoCast(eSpells::TormentofAmanThulIntro);
                me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);
                m_CountCastTorment = 0;
                mCanFly = false;
                mIsInEvade = false;
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            bool CanTargetOutOfLOSXYZ(float l_X, float l_Y, float l_Z) override
            {
                return true;
            }

            bool IgnoreFlyingMovementFlagsUpdate() override
            {
                return true;
            }

            bool CanFly() override
            {
                return mCanFly;
            }

            bool IsEasyDifficulty()
            {
                return (IsNormal() || IsLFR());
            }

            void SetTargetCosmicGlare(uint64 pGuid)
            {
                if (std::find(m_TargetCosmicGlare.begin(), m_TargetCosmicGlare.end(), pGuid) == m_TargetCosmicGlare.end())
                    m_TargetCosmicGlare.push_back(pGuid);
            }

            bool HasTargetCosmicGlare(uint64 pGuid)
            {
                return (std::find(m_TargetCosmicGlare.begin(), m_TargetCosmicGlare.end(), pGuid) != m_TargetCosmicGlare.end());
            }

            void Reset() override
            {
                events.Reset();
                SetFlyMode(false);
                me->SetReactState(REACT_AGGRESSIVE);
                m_CountCastTorment = 0;
                mCanFly = false;
                mIsInEvade = false;
                ClearDelayedOperations();
                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
            }

            void OnSpellFinishedSuccess(SpellInfo const* p_SpellInfo) override
            {
                if (!p_SpellInfo || !me->isInCombat())
                    return;

                switch (p_SpellInfo->Id)
                {
                    case eSpells::TormentofAmanThulCast:
                    case eSpells::TormentofKhazgorothCast:
                    case eSpells::TormentofNorgannonCast:
                    case eSpells::TormentofGolgannethCast:
                    {
                        ++m_CountCastTorment;
                        me->RemoveAurasDueToSpell(eSpells::SenseofDreadAura);
                        me->RemoveAurasDueToSpell(eSpells::SenseofDreadDamageAura);
                        instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SenseofDreadAura);
                        instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SenseofDreadDamageAura);
                        instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SenseofDreadAuraTrigger);
                        if (p_SpellInfo->Id == eSpells::TormentofAmanThulCast)
                        {
                            Talk(eTalks::EMOTE_CAST_MACHINATIONS_OF_AMANTHUL);
                            me->CastSpell(me, eSpells::TormentofAmanThul, true);
                        }
                        else if (p_SpellInfo->Id == eSpells::TormentofKhazgorothCast)
                        {
                            Talk(eTalks::EMOTE_CAST_FLAMES_OF_KHAZGOROTH);
                            me->CastSpell(me, eSpells::TormentofKhazgoroth, true);
                        }
                        else if (p_SpellInfo->Id == eSpells::TormentofNorgannonCast)
                        {
                            Talk(eTalks::EMOTE_CAST_SPECTRAL_ARMY_OF_NORGANNON);
                            me->CastSpell(me, eSpells::TormentofNorgannon, true);
                        }
                        else if (p_SpellInfo->Id == eSpells::TormentofGolgannethCast)
                        {
                            Talk(eTalks::EMOTE_CAST_FURY_OF_GOLGANNETH);
                            me->CastSpell(me, eSpells::TormentofGolganneth, true);
                        }

                        if (m_CountCastTorment == 2)
                        {
                            DoAction(eActions::ActionRepeatEvents);
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveJump(g_PosCenterRoom, 50.0f, 10.0f);
                            if (Creature* l_Asara = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcAsaraMotherOfNight)))
                                if (l_Asara->AI())
                                    l_Asara->AI()->DoAction(eActions::ActionNextTormentNpc);
                            break;
                        }

                        uint32 l_SpellId = 0;
                        if (Creature* l_Asara = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcAsaraMotherOfNight)))
                            if (l_Asara->AI())
                                if (boss_asara_mother_night::boss_asara_mother_nightAI* l_LinkAI = CAST_AI(boss_asara_mother_night::boss_asara_mother_nightAI, l_Asara->GetAI()))
                                    l_SpellId = l_LinkAI->GetNextTormentEvents();

                        if (!l_SpellId)
                            break;

                        me->DelayedCastSpell(me, l_SpellId, false, 1500);
                        break;
                    }
                    case eSpells::TormentofAmanThul:
                    case eSpells::TormentofKhazgoroth:
                    case eSpells::TormentofNorgannon:
                    case eSpells::TormentofGolganneth:
                    {
                        uint32 l_SummonMask = 0; ///< TormentofAmanThul
                        if (p_SpellInfo->Id == eSpells::TormentofKhazgoroth)
                            l_SummonMask = 1;
                        else if (p_SpellInfo->Id == eSpells::TormentofNorgannon)
                            l_SummonMask = 2;
                        else if (p_SpellInfo->Id == eSpells::TormentofGolganneth)
                            l_SummonMask = 3;

                        for (DataSpawnSummon l_Data : g_SpawnSummons[l_SummonMask])
                        {
                            if (!l_Data.SpawnEntry)
                                continue;

                            me->SummonCreature(l_Data.SpawnEntry, l_Data.SpawnPos, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 1800000);
                        }
                        break;
                    }
                    case eSpells::CosmicGlareTarget:
                    {
                        uint8 l_CountCosmicGlare = 0;
                        std::list<Player*> l_PlayerList;
                        me->GetPlayerListInGrid(l_PlayerList, 150.0f);
                        l_PlayerList.remove_if(CheckDeadOrGmTarget());
                        if (Unit* I_Victim = me->getVictim())
                            if (Player* p_Player = I_Victim->ToPlayer())
                                l_PlayerList.remove(p_Player);

                        l_PlayerList.remove_if([](Player* p_Target) -> bool
                        {
                            if (!p_Target || p_Target->ToPlayer()->IsActiveSpecTankSpec())
                                return true;

                            return false;
                        });

                        JadeCore::RandomResizeList(l_PlayerList, 2);

                        if (!l_PlayerList.empty() && l_PlayerList.size() >= 2)
                        {
                            for (Player* l_Player : l_PlayerList)
                            {
                                if (!l_Player)
                                    continue;

                                if (l_CountCosmicGlare == 0)
                                {
                                    me->CastSpell(l_Player, eSpells::CosmicGlareAura1, true);
                                    me->CastSpell(l_Player, eSpells::CosmicGlareAura2, true);
                                }
                                else
                                    me->CastSpell(l_Player, eSpells::CosmicGlareAura1, true);

                                ++l_CountCosmicGlare;
                            }
                        }
                        break;
                    }
                    default:
                        break;
                }
            }

            void EnterEvadeMode() override
            {
                if (mIsInEvade)
                    return;

                ClearDelayedOperations();
                me->CastStop();
                me->InterruptSpell(CURRENT_CHANNELED_SPELL, true, true);
                me->RemoveAurasDueToSpell(eSpells::TormentofAmanThulCast);
                me->RemoveAurasDueToSpell(eSpells::TormentofKhazgorothCast);
                me->RemoveAurasDueToSpell(eSpells::TormentofNorgannonCast);
                me->RemoveAurasDueToSpell(eSpells::TormentofGolgannethCast);
                BossAI::EnterEvadeMode();
                mIsInEvade = true;
            }

            void JustReachedHome() override
            {

            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionCancelEvents:
                    {
                        mCanFly = true;
                        me->SetReactState(REACT_PASSIVE);
                        me->RemoveAurasDueToSpell(eSpells::ShivanPactAT);
                        me->RemoveAurasDueToSpell(eSpells::ShivanPactAura);
                        events.CancelEvent(eEvents::EventTouchoftheCosmos);
                        events.CancelEvent(eEvents::EventCosmicGlare);
                        SetFlyMode(true);
                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                        me->RemoveAllDoTAuras();
                        break;
                    }
                    case eActions::ActionRepeatEvents:
                    {
                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                        m_CountCastTorment = 0;
                        me->SetReactState(REACT_AGGRESSIVE);
                        DoCast(eSpells::ShivanPactAT, true);
                        events.RescheduleEvent(eEvents::EventTouchoftheCosmos, 3 * IN_MILLISECONDS);
                        events.RescheduleEvent(eEvents::EventCosmicGlare, 5 * IN_MILLISECONDS);
                        mCanFly = false;
                        SetFlyMode(false);
                        break;
                    }
                    case eActions::ActionEvade:
                    {
                        mIsInEvade = false;
                        break;
                    }
                    case ActionStartEventThuraya:
                    {
                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                        mCanFly = true;
                        me->SetReactState(REACT_PASSIVE);
                        me->RemoveAurasDueToSpell(eSpells::ShivanPactAT);
                        me->RemoveAurasDueToSpell(eSpells::ShivanPactAura);
                        events.CancelEvent(eEvents::EventTouchoftheCosmos);
                        events.CancelEvent(eEvents::EventCosmicGlare);
                        SetFlyMode(true);
                        AddTimedDelayedOperation(45000, [this]() -> void
                        {
                            if (Creature* l_Asara = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcAsaraMotherOfNight)))
                                if (l_Asara->AI())
                                    if (boss_asara_mother_night::boss_asara_mother_nightAI* l_LinkAI = CAST_AI(boss_asara_mother_night::boss_asara_mother_nightAI, l_Asara->GetAI()))
                                        l_LinkAI->SetTormentEvent(true);
                        });
                        me->RemoveAllDoTAuras();
                        break;
                    }
                    default:
                        break;
                }
            }

            void EnterCombat(Unit* attacker) override
            {
                mIsInEvade = true;
                me->RemoveAurasDueToSpell(eSpells::TormentofAmanThulIntro);
                CombatBossesWithZone(me);
                DoCast(eSpells::ShivanPactAT, true);
                events.ScheduleEvent(eEvents::EventTouchoftheCosmos, 3 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventCosmicGlare, 5 * IN_MILLISECONDS);
            }

            void JustDied(Unit* p_Killer) override
            {
                me->CastStop();
                me->InterruptSpell(CURRENT_CHANNELED_SPELL, true, true);
                me->RemoveAurasDueToSpell(eSpells::TormentofAmanThulCast);
                me->RemoveAurasDueToSpell(eSpells::TormentofKhazgorothCast);
                me->RemoveAurasDueToSpell(eSpells::TormentofNorgannonCast);
                me->RemoveAurasDueToSpell(eSpells::TormentofGolgannethCast);
                DiedAllBosses(me);
                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                me->NearTeleportTo(me->GetHomePosition());
            }

            void KilledUnit(Unit* p_Who) override
            {
              if (p_Who->IsPlayer())
                  Talk(eTalks::TALK_THURAYA_SLAY);
            }

            void OnInterruptCast(Unit* /*p_Caster*/, SpellInfo const* /*p_SpellInfo*/, SpellInfo const* p_CurrSpellInfo, uint32 /*p_SchoolMask*/) override
            {
                if (p_CurrSpellInfo->Id == eSpells::TouchoftheCosmos)
                    events.RescheduleEvent(eEvents::EventTouchoftheCosmos, 6 * IN_MILLISECONDS);
            }

            void CheckHitResult(MeleeHitOutcome& /*p_MeleeResult*/, SpellMissInfo& /*p_SpellResult*/, Unit* p_Attacker, SpellInfo const* const p_SpellInfo ) override
            {
                if (!p_SpellInfo || !p_SpellInfo->HasAura(SPELL_AURA_MOD_SILENCE))
                    return;

                for (uint32 i = CURRENT_MELEE_SPELL; i < CURRENT_MAX_SPELL; ++i)
                {
                    if (Spell* spell = me->GetCurrentSpell(CurrentSpellTypes(i)))
                    {
                        if (spell->m_spellInfo->Id == eSpells::TouchoftheCosmos)
                        {
                            me->InterruptSpell(CurrentSpellTypes(i), false);
                            events.RescheduleEvent(eEvents::EventTouchoftheCosmos, 6 * IN_MILLISECONDS);
                            break;
                        }
                    }
                }
            }

            bool CanAIAttack(Unit const* p_Target) const override
            {
                return CheckBossInRoom(p_Target);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
                if (!UpdateVictim())
                    return;

                if (!CheckBossInRoom(me))
                {
                    if (Creature* l_Asara = Creature::GetCreature(*me, instance->GetData64(eCreatures::NpcAsaraMotherOfNight)))
                        if (l_Asara->AI())
                            l_Asara->AI()->EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);
                CheckDistanceForBosses(me);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eEvents::EventTouchoftheCosmos:
                        {
                            DoCast(eSpells::TouchoftheCosmos);
                            events.ScheduleEvent(eEvents::EventTouchoftheCosmos, 8 * IN_MILLISECONDS);
                            break;
                        }
                        case eEvents::EventCosmicGlare:
                        {
                            m_TargetCosmicGlare.clear();
                            Talk(eTalks::TALK_THURAYA_CAST_COSMIC_GLARE);
                            DoCast(eSpells::CosmicGlareTarget);
                            events.ScheduleEvent(eEvents::EventCosmicGlare, 15800);
                            break;
                        }
                        default:
                            break;
                    }
                }
            }

          private:
              uint8 m_CountCastTorment;
              bool mCanFly;
              bool mIsInEvade;
              std::list<uint64> m_TargetCosmicGlare;
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new boss_thuraya_mother_cosmosAI(p_Creature);
        }
};

/// Last Update 7.3.5 - 26365
/// Torment of AmanThul - 252479
class spell_covens_torment_of_amanthul_intro : public SpellScriptLoader
{
    public:
        spell_covens_torment_of_amanthul_intro() : SpellScriptLoader("spell_covens_torment_of_amanthul_intro") { }

        class spell_covens_torment_of_amanthul_intro_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_covens_torment_of_amanthul_intro_SpellScript);

            void CorrectTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr || p_Object->GetEntry() != eCreatures::NpcThurayaMotherOfTheCosmos)
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_covens_torment_of_amanthul_intro_SpellScript::CorrectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_covens_torment_of_amanthul_intro_SpellScript();
        }
};

/// Last Update 7.3.5 - 26365
/// Torment of Khazgoroth - 244733
class spell_covens_torment_of_khazgoroth_intro : public SpellScriptLoader
{
    public:
        spell_covens_torment_of_khazgoroth_intro() : SpellScriptLoader("spell_covens_torment_of_khazgoroth_intro") { }

        class spell_covens_torment_of_khazgoroth_intro_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_covens_torment_of_khazgoroth_intro_SpellScript);

            void CorrectTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr || p_Object->GetEntry() != eCreatures::NpcNouraMotherOfFlames)
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_covens_torment_of_khazgoroth_intro_SpellScript::CorrectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_covens_torment_of_khazgoroth_intro_SpellScript();
        }
};

/// Last Update 7.3.5 - 26365
/// Torment of Norgannon - 244740
class spell_covens_torment_of_norgannon_intro : public SpellScriptLoader
{
    public:
        spell_covens_torment_of_norgannon_intro() : SpellScriptLoader("spell_covens_torment_of_norgannon_intro") { }

        class spell_covens_torment_of_norgannon_intro_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_covens_torment_of_norgannon_intro_SpellScript);

            void CorrectTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr || p_Object->GetEntry() != eCreatures::NpcAsaraMotherOfNight)
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_covens_torment_of_norgannon_intro_SpellScript::CorrectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_covens_torment_of_norgannon_intro_SpellScript();
        }
};

/// Last Update 7.3.5 - 26365
/// Torment of Golganneth - 244756
class spell_covens_torment_of_golganneth_intro : public SpellScriptLoader
{
    public:
        spell_covens_torment_of_golganneth_intro() : SpellScriptLoader("spell_covens_torment_of_golganneth_intro") { }

        class spell_covens_torment_of_golganneth_intro_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_covens_torment_of_golganneth_intro_SpellScript);

            void CorrectTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr || p_Object->GetEntry() != eCreatures::NpcDiimaMotherofGloom)
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_covens_torment_of_golganneth_intro_SpellScript::CorrectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_covens_torment_of_golganneth_intro_SpellScript();
        }
};

/// Last Update 7.3.5 - 26365
/// Torment of AmanThul - 250335
class spell_covens_torment_of_amanthul_cast : public SpellScriptLoader
{
    public:
        spell_covens_torment_of_amanthul_cast() : SpellScriptLoader("spell_covens_torment_of_amanthul_cast") { }

        class spell_covens_torment_of_amanthul_cast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_covens_torment_of_amanthul_cast_SpellScript);

            void CorrectTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToCreature())
                    return;

                p_Targets.remove_if([this, l_Caster](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr)
                        return true;

                    if (l_Caster->GetEntry() == eCreatures::NpcAsaraMotherOfNight && p_Object->GetEntry() != eMisc::NpcNorgannonSoul)
                        return true;

                    if (l_Caster->GetEntry() == eCreatures::NpcNouraMotherOfFlames && p_Object->GetEntry() != eMisc::NpcKhazgorothSoul)
                        return true;

                    if (l_Caster->GetEntry() == eCreatures::NpcDiimaMotherofGloom && p_Object->GetEntry() != eMisc::NpcGolgannethSoul)
                        return true;

                    if (l_Caster->GetEntry() == eCreatures::NpcThurayaMotherOfTheCosmos && p_Object->GetEntry() != eMisc::NpcAmanThulsSoul)
                        return true;

                    return false;
                });
            }

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToCreature())
                    return;

                l_Caster->RemoveAurasDueToSpell(eSpells::SenseofDreadAura);
                l_Caster->RemoveAurasDueToSpell(eSpells::SenseofDreadDamageAura);
                l_Caster->CastSpell(l_Caster, eSpells::SenseofDreadAura, true);
                l_Caster->CastSpell(l_Caster, eSpells::SenseofDreadDamageAura, true);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_covens_torment_of_amanthul_cast_SpellScript::HandleOnCast);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_covens_torment_of_amanthul_cast_SpellScript::CorrectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_covens_torment_of_amanthul_cast_SpellScript();
        }
};

/// Last Update 7.3.5 - 26365
/// Torment of Khazgoroth - 250333
class spell_covens_torment_of_khazgoroth_cast : public SpellScriptLoader
{
    public:
        spell_covens_torment_of_khazgoroth_cast() : SpellScriptLoader("spell_covens_torment_of_khazgoroth_cast") { }

        class spell_covens_torment_of_khazgoroth_cast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_covens_torment_of_khazgoroth_cast_SpellScript);

            void CorrectTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToCreature())
                    return;

                p_Targets.remove_if([this, l_Caster](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr)
                        return true;

                    if (l_Caster->GetEntry() == eCreatures::NpcAsaraMotherOfNight && p_Object->GetEntry() != eMisc::NpcNorgannonSoul)
                        return true;

                    if (l_Caster->GetEntry() == eCreatures::NpcNouraMotherOfFlames && p_Object->GetEntry() != eMisc::NpcKhazgorothSoul)
                        return true;

                    if (l_Caster->GetEntry() == eCreatures::NpcDiimaMotherofGloom && p_Object->GetEntry() != eMisc::NpcGolgannethSoul)
                        return true;

                    if (l_Caster->GetEntry() == eCreatures::NpcThurayaMotherOfTheCosmos && p_Object->GetEntry() != eMisc::NpcAmanThulsSoul)
                        return true;

                    return false;
                });
            }

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToCreature())
                    return;

                l_Caster->RemoveAurasDueToSpell(eSpells::SenseofDreadAura);
                l_Caster->RemoveAurasDueToSpell(eSpells::SenseofDreadDamageAura);
                l_Caster->CastSpell(l_Caster, eSpells::SenseofDreadAura, true);
                l_Caster->CastSpell(l_Caster, eSpells::SenseofDreadDamageAura, true);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_covens_torment_of_khazgoroth_cast_SpellScript::HandleOnCast);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_covens_torment_of_khazgoroth_cast_SpellScript::CorrectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_covens_torment_of_khazgoroth_cast_SpellScript();
        }
};

/// Last Update 7.3.5 - 26365
/// Torment of Norgannon - 250334
class spell_covens_torment_of_norgannon_cast : public SpellScriptLoader
{
    public:
        spell_covens_torment_of_norgannon_cast() : SpellScriptLoader("spell_covens_torment_of_norgannon_cast") { }

        class spell_covens_torment_of_norgannon_cast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_covens_torment_of_norgannon_cast_SpellScript);

            void CorrectTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToCreature())
                    return;

                p_Targets.remove_if([this, l_Caster](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr)
                        return true;

                    if (l_Caster->GetEntry() == eCreatures::NpcAsaraMotherOfNight && p_Object->GetEntry() != eMisc::NpcNorgannonSoul)
                        return true;

                    if (l_Caster->GetEntry() == eCreatures::NpcNouraMotherOfFlames && p_Object->GetEntry() != eMisc::NpcKhazgorothSoul)
                        return true;

                    if (l_Caster->GetEntry() == eCreatures::NpcDiimaMotherofGloom && p_Object->GetEntry() != eMisc::NpcGolgannethSoul)
                        return true;

                    if (l_Caster->GetEntry() == eCreatures::NpcThurayaMotherOfTheCosmos && p_Object->GetEntry() != eMisc::NpcAmanThulsSoul)
                        return true;

                    return false;
                });
            }

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToCreature())
                    return;

                l_Caster->RemoveAurasDueToSpell(eSpells::SenseofDreadAura);
                l_Caster->RemoveAurasDueToSpell(eSpells::SenseofDreadDamageAura);
                l_Caster->CastSpell(l_Caster, eSpells::SenseofDreadAura, true);
                l_Caster->CastSpell(l_Caster, eSpells::SenseofDreadDamageAura, true);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_covens_torment_of_norgannon_cast_SpellScript::HandleOnCast);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_covens_torment_of_norgannon_cast_SpellScript::CorrectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_covens_torment_of_norgannon_cast_SpellScript();
        }
};

/// Last Update 7.3.5 - 26365
/// Torment of Golganneth - 249793
class spell_covens_torment_of_golganneth_cast : public SpellScriptLoader
{
    public:
        spell_covens_torment_of_golganneth_cast() : SpellScriptLoader("spell_covens_torment_of_golganneth_cast") { }

        class spell_covens_torment_of_golganneth_cast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_covens_torment_of_golganneth_cast_SpellScript);

            void CorrectTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToCreature())
                    return;

                p_Targets.remove_if([this, l_Caster](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr)
                        return true;

                    if (l_Caster->GetEntry() == eCreatures::NpcAsaraMotherOfNight && p_Object->GetEntry() != eMisc::NpcNorgannonSoul)
                        return true;

                    if (l_Caster->GetEntry() == eCreatures::NpcNouraMotherOfFlames && p_Object->GetEntry() != eMisc::NpcKhazgorothSoul)
                        return true;

                    if (l_Caster->GetEntry() == eCreatures::NpcDiimaMotherofGloom && p_Object->GetEntry() != eMisc::NpcGolgannethSoul)
                        return true;

                    if (l_Caster->GetEntry() == eCreatures::NpcThurayaMotherOfTheCosmos && p_Object->GetEntry() != eMisc::NpcAmanThulsSoul)
                        return true;

                    return false;
                });
            }

            void HandleOnCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToCreature())
                    return;

                l_Caster->RemoveAurasDueToSpell(eSpells::SenseofDreadAura);
                l_Caster->RemoveAurasDueToSpell(eSpells::SenseofDreadDamageAura);
                l_Caster->CastSpell(l_Caster, eSpells::SenseofDreadAura, true);
                l_Caster->CastSpell(l_Caster, eSpells::SenseofDreadDamageAura, true);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_covens_torment_of_golganneth_cast_SpellScript::HandleOnCast);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_covens_torment_of_golganneth_cast_SpellScript::CorrectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_covens_torment_of_golganneth_cast_SpellScript();
        }
};

///< 125837 - Torment of AmanThul
class npc_covens_torment_of_amanthul : public CreatureScript
{
    public:
        npc_covens_torment_of_amanthul() : CreatureScript("npc_covens_torment_of_amanthul") { }

        struct npc_covens_torment_of_amanthulAI : public ScriptedAI
        {
            npc_covens_torment_of_amanthulAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                ApplyAllImmunities(true);
                me->AddUnitState(UNIT_STATE_ROOT);
                me->SetReactState(REACT_PASSIVE);
            }

            enum mEvents
            {
                EventReadyToCast = 1
            };

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetInCombatWithZone();
                events.Reset();
                events.ScheduleEvent(mEvents::EventReadyToCast, 600);
                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->DoCastSpellOnPlayers(eSpells::MachinationsofAmanThulAura, nullptr, false, true);
            }

            void OnSpellFinishedSuccess(SpellInfo const* p_SpellInfo) override
            {
                if (!p_SpellInfo)
                    return;

                switch (p_SpellInfo->Id)
                {
                    case eSpells::MachinationsofAmanThul:
                    {
                        if (InstanceScript* l_Instance = me->GetInstanceScript())
                            l_Instance->DoCastSpellOnPlayers(eSpells::MachinationsofAmanThulAura, nullptr, false, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            bool CanFly() override
            {
                return false;
            }

            bool IgnoreFlyingMovementFlagsUpdate() override
            {
                return true;
            }

            void JustDied(Unit* p_Killer) override
            {
                me->DespawnOrUnsummon();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case mEvents::EventReadyToCast:
                        {
                            DoCast(eSpells::MachinationsofAmanThul);
                            events.ScheduleEvent(mEvents::EventReadyToCast, 5 * IN_MILLISECONDS);
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
            return new npc_covens_torment_of_amanthulAI(p_Creature);
        }
};

///< 124166 - Torment of Khazgoroth
class npc_covens_torment_of_khazgoroth : public CreatureScript
{
    public:
        npc_covens_torment_of_khazgoroth() : CreatureScript("npc_covens_torment_of_khazgoroth") { }

        struct npc_covens_torment_of_khazgorothAI : public ScriptedAI
        {
            npc_covens_torment_of_khazgorothAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                ApplyAllImmunities(true);
                me->AddUnitState(UNIT_STATE_ROOT);
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
            }

            enum mEvents
            {
                EventReadyToCast = 1
            };

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                me->SetInCombatWithZone();
                events.Reset();
                events.ScheduleEvent(mEvents::EventReadyToCast, 600);
            }

            bool CanFly() override
            {
                return false;
            }

            bool IgnoreFlyingMovementFlagsUpdate() override
            {
                return true;
            }

            void JustDied(Unit* p_Killer) override
            {
                me->DespawnOrUnsummon();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case mEvents::EventReadyToCast:
                        {
                            DoCast(eSpells::FlamesofKhazgoroth);
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
            return new npc_covens_torment_of_khazgorothAI(p_Creature);
        }
};

///< 124164 - Torment of Golganneth
class npc_covens_torment_of_golganneth : public CreatureScript
{
    public:
        npc_covens_torment_of_golganneth() : CreatureScript("npc_covens_torment_of_golganneth") { }

        struct npc_covens_torment_of_golgannethAI : public ScriptedAI
        {
            npc_covens_torment_of_golgannethAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                ApplyAllImmunities(true);
                me->AddUnitState(UNIT_STATE_ROOT);
                me->SetReactState(REACT_PASSIVE);
            }

            enum mEvents
            {
                EventReadyToCast = 1
            };

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetInCombatWithZone();
                events.Reset();
                events.ScheduleEvent(mEvents::EventReadyToCast, 600);
                DoCast(eSpells::FuryofGolgannethAOEAT, true);
                DoCast(eSpells::FuryofGolgannethAOEAura, true);
            }

            bool CanFly() override
            {
                return false;
            }

            bool IgnoreFlyingMovementFlagsUpdate() override
            {
                return true;
            }

            void JustDied(Unit* p_Killer) override
            {
                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    Map::PlayerList const& l_Players = l_Instance->instance->GetPlayers();
                    for (Map::PlayerList::const_iterator l_Itr = l_Players.begin(); l_Itr != l_Players.end(); ++l_Itr)
                    {
                        Player* l_Player = l_Itr->getSource();
                        if (!l_Player)
                            continue;

                        l_Player->RemoveAuraFromStack(eSpells::FuryofGolgannethAura);
                    }
                }

                me->DespawnOrUnsummon();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case mEvents::EventReadyToCast:
                        {
                            DoCast(eSpells::FuryofGolganneth);
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
            return new npc_covens_torment_of_golgannethAI(p_Creature);
        }
};

///< 123503 - Torment of Norgannon
class npc_covens_torment_of_norgannon : public CreatureScript
{
    public:
        npc_covens_torment_of_norgannon() : CreatureScript("npc_covens_torment_of_norgannon") { }

        struct npc_covens_torment_of_norgannonAI : public ScriptedAI
        {
            npc_covens_torment_of_norgannonAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
                l_CheckMovingTimer = 5000;
            }

            uint32 l_CheckMovingTimer;
            Position l_DestPos;

            enum mEvents
            {
                EventReadyToCast = 1,
                EventMovement
            };

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetInCombatWithZone();
                events.Reset();
                events.ScheduleEvent(mEvents::EventReadyToCast, 2000);
                events.ScheduleEvent(mEvents::EventMovement, 3000);
                DoCast(eSpells::VisageoftheTitan, true);
                l_CheckMovingTimer = 5000;
            }

            bool CanFly() override
            {
                return false;
            }

            bool IgnoreFlyingMovementFlagsUpdate() override
            {
                return true;
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            bool CheckNorganonInRoom()
            {
                float min_x = -12679.0f;
                float max_x = -12586.0f;
                float min_y = -2863.0f;
                float max_y = -2775.0f;
                return !(
                    me->GetPositionX() < min_x ||
                    me->GetPositionX() > max_x ||
                    me->GetPositionY() < min_y ||
                    me->GetPositionY() > max_y
                    );
            }

            void CustomMovingByAngle(float l_Angle)
            {
                if (me->HasUnitState(UNIT_STATE_ROOT | UNIT_STATE_CONTROLLED))
                    return;

                Position l_Pos;
                me->GetNearPosition(l_Pos, 80.0f, l_Angle);
                me->SetWalk(true);
                me->SetSpeed(MOVE_RUN, 2.5f, true);
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MovePoint(100, l_Pos, false);
                l_DestPos = l_Pos;
            }

            void JustDied(Unit* p_Killer) override
            {
                me->DespawnOrUnsummon();
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (me->HasAura(eSpells::VisageoftheTitan))
                    p_Damage = 0;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case mEvents::EventReadyToCast:
                        {
                            DoCast(eSpells::SpectralArmyofNorgannonAT);
                            break;
                        }
                        case mEvents::EventMovement:
                        {
                            if (!me->HasUnitState(UNIT_STATE_ROOT | UNIT_STATE_CONTROLLED))
                                CustomMovingByAngle(0.0f);
                            else
                                events.ScheduleEvent(mEvents::EventMovement, 200);
                            break;
                        }
                        default:
                            break;
                    }
                }

                if (l_CheckMovingTimer <= p_Diff)
                {
                    l_CheckMovingTimer = 500;
                    if (!CheckNorganonInRoom())
                    {
                        l_CheckMovingTimer = 3500;
                        CustomMovingByAngle(M_PI);
                        return;
                    }

                    if (!me->IsMoving() && !me->HasUnitState(UNIT_STATE_ROOT | UNIT_STATE_CONTROLLED))
                    {
                        if (me->GetDistance(l_DestPos) <= 3.0f)
                            CustomMovingByAngle(M_PI);
                        else
                            CustomMovingByAngle(0.0f);
                    }
                }
                else
                    l_CheckMovingTimer -= p_Diff;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_covens_torment_of_norgannonAI(p_Creature);
        }
};

/// Last Update: 7.3.5 26365
/// Called by Touch of Darkness - 245303
class spell_covens_touch_of_darkness : public SpellScriptLoader
{
    public:
        spell_covens_touch_of_darkness() : SpellScriptLoader("spell_covens_touch_of_darkness") { }

        class spell_covens_touch_of_darkness_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_covens_touch_of_darkness_SpellScript);

            void SelectTarget(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (Unit* I_Victim = l_Caster->getVictim())
                    p_Targets.remove(I_Victim);

                p_Targets.remove_if([](WorldObject* p_Target) -> bool
                {
                    if (!p_Target->IsPlayer() || p_Target->ToPlayer()->IsActiveSpecTankSpec())
                        return true;

                    return false;
                });

                if (p_Targets.empty())
                    return;

                p_Targets.sort(JadeCore::DistanceCompareOrderPred(l_Caster, false));
                if (p_Targets.size() > 2)
                    p_Targets.resize(2);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_covens_touch_of_darkness_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_covens_touch_of_darkness_SpellScript();
        }
};

///< 123086 - Shadow Blade
class npc_covens_shadow_blades : public CreatureScript
{
    public:
        npc_covens_shadow_blades() : CreatureScript("npc_covens_shadow_blades") { }

        struct npc_covens_shadow_bladesAI : public ScriptedAI
        {
            npc_covens_shadow_bladesAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetDisplayId(11686);
                ApplyAllImmunities(true);
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                me->SetReactState(REACT_PASSIVE);
                DoCast(eSpells::ShadowBladesAuraCosmetic, true);
                SetFlyMode(true);
                me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);
            }

            enum mEvents
            {
                EventReadyToFly = 1,
                EventCastShadowBladesAura
            };

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                events.Reset();
                events.ScheduleEvent(mEvents::EventReadyToFly, 2 * IN_MILLISECONDS);
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex /*p_EffIndex*/) override
            {
                if (p_Targets.empty())
                    return;

                switch (p_Spell->GetSpellInfo()->Id)
                {
                    case eSpells::ShadowBladesDamage:
                    {
                        p_Targets.remove_if([this](WorldObject* p_Target) -> bool
                        {
                            if (!p_Target || me->GetExactDist2d(p_Target) > 1.0f)
                                return true;

                            return false;
                        });
                        break;
                    }
                    default:
                        break;
                }
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case 0:
                    {
                        me->RemoveAurasDueToSpell(eSpells::ShadowBladesAura);
                        events.RescheduleEvent(mEvents::EventCastShadowBladesAura, 3 * IN_MILLISECONDS);
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
                    case 100:
                    {
                        me->DespawnOrUnsummon();
                        break;
                    }
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
                        case mEvents::EventReadyToFly:
                        {
                            Position l_Pos;
                            me->GetNearPosition(l_Pos, 100.0f, 0.0f);
                            me->SetSpeed(MOVE_FLIGHT, 2.5f);
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveSmoothFlyPath(100, l_Pos);
                            DoCast(eSpells::ShadowBladesAura, true);
                            break;
                        }

                        case mEvents::EventCastShadowBladesAura:
                        {
                            DoCast(eSpells::ShadowBladesAura, true);
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
            return new npc_covens_shadow_bladesAI(p_Creature);
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Storm of Darkness - 254574
class spell_at_covens_storm_darkness_not_safe : public AreaTriggerEntityScript
{
    public:
        spell_at_covens_storm_darkness_not_safe() : AreaTriggerEntityScript("spell_at_covens_storm_darkness_not_safe") {}

        uint32 m_CheckTimer = 900;

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (m_CheckTimer <= p_Time)
            {
                m_CheckTimer = 500;

                if (!p_AreaTrigger)
                     return;

                Unit* l_Caster = p_AreaTrigger->GetCaster();
                if (!l_Caster)
                    return;

                InstanceScript* l_Instance = l_Caster->GetInstanceScript();
                if (!l_Instance)
                    return;

                Creature* l_Asara = Creature::GetCreature(*l_Caster, l_Instance->GetData64(eCreatures::NpcAsaraMotherOfNight));
                if (!l_Asara || !l_Asara->AI())
                    return;

                boss_asara_mother_night::boss_asara_mother_nightAI* l_LinkAI = CAST_AI(boss_asara_mother_night::boss_asara_mother_nightAI, l_Asara->GetAI());
                if (!l_LinkAI)
                    return;

                std::list<Player*> l_PlayerList;
                l_Asara->GetPlayerListInGrid(l_PlayerList, 200.0f);
                l_PlayerList.remove_if(CheckDeadOrGmTarget());

                if (!l_PlayerList.empty())
                {
                    SpellInfo const* p_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::StormofDarknessDamage);
                    for (Player* l_Player : l_PlayerList)
                    {
                        if (!l_Player || !p_SpellInfo)
                            continue;

                        if (l_LinkAI->NearSafeStormofDarkness(l_Player->GetGUID()))
                        {
                            if (l_Player->HasAura(eSpells::StormofDarknessDamage))
                                l_Player->RemoveAurasDueToSpell(eSpells::StormofDarknessDamage);
                        }
                        else
                        {
                            if (!l_Player->HasAura(eSpells::StormofDarknessDamage) && !l_Player->IsImmunedToSpell(p_SpellInfo))
                                l_Player->CastSpell(l_Player, eSpells::StormofDarknessDamage, true);
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

            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            InstanceScript* l_Instance = l_Caster->GetInstanceScript();
            if (!l_Instance)
                return;

            l_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::StormofDarknessDamage);

            Creature* l_Asara = Creature::GetCreature(*l_Caster, l_Instance->GetData64(eCreatures::NpcAsaraMotherOfNight));
            if (!l_Asara)
                return;

            if (l_Asara->AI())
                if (boss_asara_mother_night::boss_asara_mother_nightAI* l_LinkAI = CAST_AI(boss_asara_mother_night::boss_asara_mother_nightAI, l_Asara->GetAI()))
                    l_LinkAI->SetClearsStormofDarkness();
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_covens_storm_darkness_not_safe();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Storm of Darkness - 255232
class spell_at_covens_storm_darkness_safe : public AreaTriggerEntityScript
{
    public:
        spell_at_covens_storm_darkness_safe() : AreaTriggerEntityScript("spell_at_covens_storm_darkness_safe") {}

        uint32 m_CheckTimer = 0;
        std::list<uint64> m_Targets;

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (m_CheckTimer <= p_Time)
            {
                m_CheckTimer = 100;

                if (!p_AreaTrigger)
                    return;

                Unit* l_Caster = p_AreaTrigger->GetCaster();
                if (!l_Caster)
                    return;

                InstanceScript* l_Instance = l_Caster->GetInstanceScript();
                if (!l_Instance)
                    return;

                Creature* l_Asara = Creature::GetCreature(*l_Caster, l_Instance->GetData64(eCreatures::NpcAsaraMotherOfNight));
                if (!l_Asara || !l_Asara->AI())
                    return;

                boss_asara_mother_night::boss_asara_mother_nightAI* l_LinkAI = CAST_AI(boss_asara_mother_night::boss_asara_mother_nightAI, l_Asara->GetAI());
                if (!l_LinkAI)
                    return;

                float l_Radius = p_AreaTrigger->GetRadius();
                std::list<uint64> l_Targets(m_Targets);
                for (uint64 l_TargetGuid : l_Targets)
                {
                    Player* l_Player = ObjectAccessor::GetPlayer(*l_Caster, l_TargetGuid);
                    if (!l_Player)
                        continue;

                    if (l_Player->GetDistance(p_AreaTrigger) <= l_Radius)
                        continue;

                    l_LinkAI->SetClearStormofDarkness(l_TargetGuid);
                    m_Targets.remove(l_TargetGuid);
                }

                std::list<Unit*> l_PlayerList;
                p_AreaTrigger->GetAttackableUnitListInPositionRange(l_PlayerList, *p_AreaTrigger, l_Radius, false);
                l_PlayerList.remove_if(CheckDeadOrGmTarget());
                l_PlayerList.remove_if([](WorldObject* p_Target) -> bool
                {
                    if (!p_Target->IsPlayer())
                        return true;

                    return false;
                });

                if (!l_PlayerList.empty())
                {
                    for (Unit* l_Player : l_PlayerList)
                    {
                        if (!l_Player)
                            continue;

                        if (l_Player->HasAura(eSpells::StormofDarknessDamage))
                            l_Player->RemoveAurasDueToSpell(eSpells::StormofDarknessDamage);

                        if (l_LinkAI->NearSafeStormofDarkness(l_Player->GetGUID()))
                            continue;

                        l_LinkAI->SetTargetStormofDarkness(l_Player->GetGUID());
                        m_Targets.push_back(l_Player->GetGUID());
                    }
                }
            }
            else
                m_CheckTimer -= p_Time;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_covens_storm_darkness_safe();
        }
};

/// Last Update 7.3.5 - 26365
/// Whirling Saber - 246486
class spell_covens_whirling_saber_target : public SpellScriptLoader
{
    public:
        spell_covens_whirling_saber_target() : SpellScriptLoader("spell_covens_whirling_saber_target") { }

        class spell_covens_whirling_saber_target_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_covens_whirling_saber_target_SpellScript);

            void CorrectTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (Unit* I_Victim = l_Caster->getVictim())
                    p_Targets.remove(I_Victim);

                p_Targets.remove_if([](WorldObject* p_Target) -> bool
                {
                    if (!p_Target->IsPlayer() || p_Target->ToPlayer()->IsActiveSpecTankSpec())
                        return true;

                    return false;
                });

                if (p_Targets.empty())
                    return;

                p_Targets.sort(JadeCore::DistanceCompareOrderPred(l_Caster, false));
                if (p_Targets.size() > 1)
                    p_Targets.resize(1);
            }

            void HandleAfterHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                Position l_Pos = *l_Target;
                l_Pos.m_positionZ = 2475.5f;

                l_Caster->CastSpell(l_Pos, eSpells::WhirlingSaber, false);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_covens_whirling_saber_target_SpellScript::CorrectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnEffectHitTarget += SpellEffectFn(spell_covens_whirling_saber_target_SpellScript::HandleAfterHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_covens_whirling_saber_target_SpellScript();
        }
};

///< 123348 - Whirling Saber
class npc_covens_whirling_saber : public CreatureScript
{
    public:
        npc_covens_whirling_saber() : CreatureScript("npc_covens_whirling_saber") { }

        struct npc_covens_whirling_saberAI : public ScriptedAI
        {
            npc_covens_whirling_saberAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetDisplayId(15435);
                ApplyAllImmunities(true);
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                me->SetReactState(REACT_PASSIVE);
                DoCast(eSpells::WhirlingSaberAT, true);
                me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);
                l_Despawn = false;
            }

            enum mEvents
            {
                EventSearchTarget = 1
            };

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                l_Despawn = false;
                events.Reset();
                events.ScheduleEvent(mEvents::EventSearchTarget, 1000);
                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    if (Creature* l_Noura = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::NpcNouraMotherOfFlames)))
                        me->GetMotionMaster()->MoveFollow(l_Noura, 0, 0.0f);
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            void SearchTarget()
            {
                std::list<Player*> playerList;
                GetPlayerListInGrid(playerList, me, 5.0f);
                Position l_Pos = *me;
                playerList.remove_if([l_Pos](Player* p_Target) -> bool
                {
                    if (p_Target->GetExactDist(&l_Pos) > 4.0f)
                        return true;

                    return false;
                });

                if (!playerList.empty())
                {
                    for (Player* l_Player : playerList)
                    {
                        if (!l_Player)
                            continue;

                        l_Player->CastSpell(l_Player, eSpells::WhirlingSaberDamage, true);
                    }
                }

                if (l_Despawn)
                    return;

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    if (Creature* l_Noura = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::NpcNouraMotherOfFlames)))
                    {
                        if (l_Noura->GetDistance2d(me) <= 2.0f)
                        {
                            l_Despawn = true;
                            me->DespawnOrUnsummon();
                        }
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case mEvents::EventSearchTarget:
                        {
                            SearchTarget();
                            events.ScheduleEvent(mEvents::EventSearchTarget, 1000);
                            break;
                        }
                        default:
                            break;
                    }
                }
            }

          private:
              bool l_Despawn;
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_covens_whirling_saberAI(p_Creature);
        }
};

/// Last Update: 7.3.5 26365
/// Called by Fulminating Pulse Target - 253429
class spell_covens_fulminating_pulse_target : public SpellScriptLoader
{
    public:
        spell_covens_fulminating_pulse_target() : SpellScriptLoader("spell_covens_fulminating_pulse_target") { }

        class spell_covens_fulminating_pulse_target_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_covens_fulminating_pulse_target_SpellScript);

            void SelectTarget(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (Unit* I_Victim = l_Caster->getVictim())
                    p_Targets.remove(I_Victim);

                p_Targets.remove_if([](WorldObject* p_Target) -> bool
                {
                    if (!p_Target->IsPlayer() || p_Target->ToPlayer()->IsActiveSpecTankSpec())
                        return true;

                    return false;
                });

                if (p_Targets.empty())
                    return;

                p_Targets.sort(JadeCore::DistanceCompareOrderPred(l_Caster, false));
                if (p_Targets.size() > 3)
                    p_Targets.resize(3);
            }

            void HandleAfterHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::FulminatingPulseAura, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_covens_fulminating_pulse_target_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_covens_fulminating_pulse_target_SpellScript::HandleAfterHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_covens_fulminating_pulse_target_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Fulminating Pulse 253520
class spell_covens_fulminating_pulse_aura : public SpellScriptLoader
{
    public:
        spell_covens_fulminating_pulse_aura() : SpellScriptLoader("spell_covens_fulminating_pulse_aura") { }

        class spell_covens_fulminating_pulse_aura_SpellScript : public AuraScript
        {
            PrepareAuraScript(spell_covens_fulminating_pulse_aura_SpellScript);

            void HandleAfterRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_Target->CastSpell(l_Target, eSpells::FulminatingBurst, true);
            }

            void HandleDispel(DispelInfo* dispelInfo)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_Target->CastSpell(l_Target, eSpells::FulminatingBurst, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_covens_fulminating_pulse_aura_SpellScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
                AfterDispel += AuraDispelFn(spell_covens_fulminating_pulse_aura_SpellScript::HandleDispel);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_covens_fulminating_pulse_aura_SpellScript();
        }
};

/// Last Update: 7.3.5 26365
/// Called by Chilled Blood Target - 245520
class spell_covens_chilled_blood_target : public SpellScriptLoader
{
    public:
        spell_covens_chilled_blood_target() : SpellScriptLoader("spell_covens_chilled_blood_target") { }

        class spell_covens_chilled_blood_target_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_covens_chilled_blood_target_SpellScript);

            void SelectTarget(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (Unit* I_Victim = l_Caster->getVictim())
                    p_Targets.remove(I_Victim);

                p_Targets.remove_if([](WorldObject* p_Target) -> bool
                {
                    if (!p_Target->IsPlayer() || p_Target->ToPlayer()->IsActiveSpecTankSpec())
                        return true;

                    return false;
                });

                if (p_Targets.empty())
                    return;

                p_Targets.sort(JadeCore::DistanceCompareOrderPred(l_Caster, false));
                if (p_Targets.size() > 3)
                    p_Targets.resize(3);
            }

            void HandleAfterHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::ChilledBloodMissile, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_covens_chilled_blood_target_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_covens_chilled_blood_target_SpellScript::HandleAfterHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_covens_chilled_blood_target_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Chilled Blood 245586
class spell_covens_chilled_blood_aura : public SpellScriptLoader
{
    public:
        spell_covens_chilled_blood_aura() : SpellScriptLoader("spell_covens_chilled_blood_aura") { }

        class spell_covens_chilled_blood_aura_SpellScript : public AuraScript
        {
            PrepareAuraScript(spell_covens_chilled_blood_aura_SpellScript);

            void HandleAfterRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_Target->CastSpell(l_Target, eSpells::ChilledBloodStun, true);

                Aura* l_Aura = l_Target->GetAura(eSpells::ChilledBloodStun);
                if (!l_Aura)
                    return;

                int32 l_Amount = p_AuraEffect->GetAmount();
                if (!l_Amount)
                    return;

                int32 l_MaxAmount = p_AuraEffect->GetBaseAmount();
                if (!l_MaxAmount)
                    return;

                float l_ModDuration = ((float)l_Amount / (float)l_MaxAmount);
                int32 l_Duration = int32((float)5000.0f * (float)l_ModDuration);
                l_Duration = std::max<int32>(1000, l_Duration);

                l_Aura->SetMaxDuration(l_Duration);
                l_Aura->SetDuration(l_Duration);
            }

            void HandleDispel(DispelInfo* dispelInfo)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_Target->CastSpell(l_Target, eSpells::ChilledBloodStun, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_covens_chilled_blood_aura_SpellScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_SCHOOL_HEAL_ABSORB, AURA_EFFECT_HANDLE_REAL);
                AfterDispel += AuraDispelFn(spell_covens_chilled_blood_aura_SpellScript::HandleDispel);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_covens_chilled_blood_aura_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Orb of Frost 253697
class spell_covens_orb_of_frost_aura : public SpellScriptLoader
{
    public:
        spell_covens_orb_of_frost_aura() : SpellScriptLoader("spell_covens_orb_of_frost_aura") { }

        class spell_covens_orb_of_frost_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_covens_orb_of_frost_aura_AuraScript);

            void OnTick(AuraEffect const* l_AuraEffect)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                if (AreaTrigger* l_AreaTrigger = l_Target->FindNearestAreaTrigger(eSpells::OrbofFrostAT, 200.0f))
                {
                    if (AuraEffect* l_SpeedEffect = l_AuraEffect->GetBase()->GetEffect(EFFECT_0))
                    {
                        float l_Distance = l_Target->GetDistance(l_AreaTrigger);
                        float l_MinDistance = 1.0f;
                        float l_MaxDistance = 30.0f;
                        float l_MaxAmount = 85.0f;

                        if (l_Distance > l_MaxDistance)
                            l_Distance = l_MaxDistance;

                        float l_Basepoint = float(l_MaxAmount * (1.0f - (l_Distance / l_MaxDistance)));
                        if (l_Distance <= l_MinDistance)
                            l_Basepoint = l_MaxAmount;

                        int32 l_NewAmount = int32(l_Basepoint == 0.0f ? 0 : (int32)-l_Basepoint);
                        int32 l_OldAmount = l_SpeedEffect->GetAmount();
                        if (l_OldAmount != l_NewAmount)
                        {
                            l_SpeedEffect->ChangeAmount(l_NewAmount);
                            l_Target->UpdateSpeed(MOVE_RUN, true);
                            l_Target->UpdateSpeed(MOVE_SWIM, true);
                            l_Target->UpdateSpeed(MOVE_FLIGHT, true);
                            l_Target->UpdateSpeed(MOVE_RUN_BACK, true);
                            l_Target->UpdateSpeed(MOVE_SWIM_BACK, true);
                            l_Target->UpdateSpeed(MOVE_FLIGHT_BACK, true);
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_covens_orb_of_frost_aura_AuraScript::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_covens_orb_of_frost_aura_AuraScript();
        }
};

/// Last Update: 7.3.5 26365
/// Called by Orb of Frost Target - 253649
class spell_covens_orb_of_frost_target : public SpellScriptLoader
{
    public:
        spell_covens_orb_of_frost_target() : SpellScriptLoader("spell_covens_orb_of_frost_target") { }

        class spell_covens_orb_of_frost_target_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_covens_orb_of_frost_target_SpellScript);

            void SelectTarget(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (Unit* I_Victim = l_Caster->getVictim())
                    p_Targets.remove(I_Victim);

                p_Targets.remove_if([](WorldObject* p_Target) -> bool
                {
                    if (!p_Target->IsPlayer() || p_Target->ToPlayer()->IsActiveSpecTankSpec())
                        return true;

                    return false;
                });

                if (p_Targets.empty())
                    return;

                p_Targets.sort(JadeCore::DistanceCompareOrderPred(l_Caster, false));
                if (p_Targets.size() > 1)
                    p_Targets.resize(1);
            }

            void HandleAfterHit(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::OrbofFrostAT, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_covens_orb_of_frost_target_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_covens_orb_of_frost_target_SpellScript::HandleAfterHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_covens_orb_of_frost_target_SpellScript();
        }
};
/// Last Update: 7.3.5 26365
/// Called by Flames of Khaz'goroth - 245674
class spell_covens_flames_of_khazgoroth : public SpellScriptLoader
{
    public:
        spell_covens_flames_of_khazgoroth() : SpellScriptLoader("spell_covens_flames_of_khazgoroth") { }

        class spell_covens_flames_of_khazgoroth_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_covens_flames_of_khazgoroth_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || p_Targets.empty() || !l_SpellInfo)
                    return;

                float l_Radius = l_SpellInfo->Effects[EFFECT_0].CalcRadius(l_Caster);
                p_Targets.remove_if([l_Caster, l_Radius](WorldObject* p_Target) -> bool
                {
                    if (!p_Target->IsInAxe(l_Caster, 1.5f, l_Radius))
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_covens_flames_of_khazgoroth_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_ENEMY_BETWEEN_DEST_2);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_covens_flames_of_khazgoroth_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Fury of Golganneth 260806
class spell_covens_fury_of_golganneth_aura : public SpellScriptLoader
{
    public:
        spell_covens_fury_of_golganneth_aura() : SpellScriptLoader("spell_covens_fury_of_golganneth_aura") { }

        class spell_covens_fury_of_golganneth_aura_SpellScript : public AuraScript
        {
            PrepareAuraScript(spell_covens_fury_of_golganneth_aura_SpellScript);

            void HandleAfterRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_Target->RemoveAurasDueToSpell(eSpells::FuryofGolgannethAT);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_covens_fury_of_golganneth_aura_SpellScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_covens_fury_of_golganneth_aura_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Spectral Army of Norgannon 245910
class spell_at_covens_spectral_army_norgannon : public AreaTriggerEntityScript
{
    public:
        spell_at_covens_spectral_army_norgannon() : AreaTriggerEntityScript("spell_at_covens_spectral_army_norgannon") {}

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

                float l_Radius = p_AreaTrigger->GetRadius();
                std::list<uint64> l_Targets(m_Targets);
                for (uint64 l_TargetGuid : l_Targets)
                {
                    Player* l_Player = ObjectAccessor::GetPlayer(*l_AreaTriggerCaster, l_TargetGuid);
                    if (!l_Player)
                        continue;

                    if (l_Player->HasAura(eSpells::SpectralArmyofNorgannonDamage) && l_Player->GetDistance(p_AreaTrigger) <= l_Radius)
                        continue;

                    if (l_Player->HasAura(eSpells::SpectralArmyofNorgannonDamage))
                        l_Player->RemoveAurasDueToSpell(eSpells::SpectralArmyofNorgannonDamage);

                    m_Targets.remove(l_TargetGuid);
                }

                std::list<Unit*> l_PlayerList;
                p_AreaTrigger->GetAttackableUnitListInPositionRange(l_PlayerList, *p_AreaTrigger, l_Radius, false);
                l_PlayerList.remove_if(CheckDeadOrGmTarget());
                l_PlayerList.remove_if([](WorldObject* p_Target) -> bool
                {
                    if (!p_Target->IsPlayer())
                        return true;

                    return false;
                });

                if (!l_PlayerList.empty())
                {
                    SpellInfo const* p_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::SpectralArmyofNorgannonDamage);
                    for (Unit* l_Player : l_PlayerList)
                    {
                        if (!l_Player || !p_SpellInfo || l_Player->IsImmunedToSpell(p_SpellInfo) || l_Player->HasAura(eSpells::SpectralArmyofNorgannonDamage))
                            continue;

                        l_Player->CastSpell(l_Player, eSpells::SpectralArmyofNorgannonDamage, true);
                        m_Targets.push_back(l_Player->GetGUID());
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

                if (l_Player->HasAura(eSpells::SpectralArmyofNorgannonDamage))
                    l_Player->RemoveAurasDueToSpell(eSpells::SpectralArmyofNorgannonDamage);

                m_Targets.remove(l_TargetGuid);
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_covens_spectral_army_norgannon();
        }
};

/// Last Update 7.3.5 Build 26365
/// Fury of Golganneth 246763
class spell_at_covens_fury_of_golganneth : public AreaTriggerEntityScript
{
    public:
        spell_at_covens_fury_of_golganneth() : AreaTriggerEntityScript("spell_at_covens_fury_of_golganneth") {}

        uint32 m_CheckTimer = 1000;

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (m_CheckTimer <= p_Time)
            {
                m_CheckTimer = 1000;
                if (!p_AreaTrigger)
                    return;

                auto l_AreaTriggerCaster = p_AreaTrigger->GetCaster();
                if (!l_AreaTriggerCaster)
                    return;

                Player* p_Player = l_AreaTriggerCaster->ToPlayer();
                if (!p_Player)
                    return;

                float l_Radius = p_AreaTrigger->GetRadius();
                std::list<Unit*> l_PlayerList;
                p_AreaTrigger->GetAttackableUnitListInPositionRange(l_PlayerList, *p_AreaTrigger, l_Radius, false);
                l_PlayerList.remove_if(CheckDeadOrGmTarget());
                l_PlayerList.remove(p_Player);
                l_PlayerList.remove_if([](WorldObject* p_Target) -> bool
                {
                    if (!p_Target->IsPlayer())
                        return true;

                    return false;
                });

                if (!l_PlayerList.empty())
                {
                    SpellInfo const* p_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::FuryofGolgannethDamage);
                    for (Unit* l_Player : l_PlayerList)
                    {
                        if (!l_Player || !p_SpellInfo || l_Player->IsImmunedToSpell(p_SpellInfo))
                            continue;

                        if (Creature* l_Golganneth = p_Player->FindNearestCreature(eMisc::NpcEventGolganneth, 100.0f))
                            l_Golganneth->CastSpell(l_Player, eSpells::FuryofGolgannethDamage, true);
                    }
                }
            }
            else
                m_CheckTimer -= p_Time;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_covens_fury_of_golganneth();
        }
};

/// Last Update 7.3.5 Build 26365
/// Fury of Golganneth 246770
class spell_covens_fury_of_golganneth_damage : public SpellScriptLoader
{
    public:
        spell_covens_fury_of_golganneth_damage() : SpellScriptLoader("spell_covens_fury_of_golganneth_damage") { }

        class spell_covens_fury_of_golganneth_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_covens_fury_of_golganneth_damage_SpellScript);

            void HandleDamage(SpellEffIndex /*p_EffectIndex*/)
            {
                Unit* l_Target = GetHitUnit();
                if (!l_Target || !l_Target->HasAura(eSpells::FuryofGolgannethAura))
                    return;

                int32 l_Damage = GetHitDamage();
                uint8 l_Stacks = 1;
                if (Aura* l_Aura = l_Target->GetAura(eSpells::FuryofGolgannethAura))
                    l_Stacks = l_Aura->GetStackAmount();

                SetHitDamage(l_Damage * l_Stacks);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_covens_fury_of_golganneth_damage_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_covens_fury_of_golganneth_damage_SpellScript();
        }
};

/// Last Update: 7.3.5 26365
/// Called by Touch of the Cosmos  - 250648
class spell_covens_touch_of_the_cosmos : public SpellScriptLoader
{
    public:
        spell_covens_touch_of_the_cosmos() : SpellScriptLoader("spell_covens_touch_of_the_cosmos") { }

        class spell_covens_touch_of_the_cosmos_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_covens_touch_of_the_cosmos_SpellScript);

            void SelectTarget(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (Unit* I_Victim = l_Caster->getVictim())
                    p_Targets.remove(I_Victim);

                p_Targets.remove_if([](WorldObject* p_Target) -> bool
                {
                    if (!p_Target->IsPlayer() || p_Target->ToPlayer()->IsActiveSpecTankSpec())
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_covens_touch_of_the_cosmos_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_covens_touch_of_the_cosmos_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Cosmic Glare Aura 250757
class spell_covens_cosmic_glare_aura : public SpellScriptLoader
{
    public:
        spell_covens_cosmic_glare_aura() : SpellScriptLoader("spell_covens_cosmic_glare_aura") { }

        class spell_covens_cosmic_glare_aura_SpellScript : public AuraScript
        {
            PrepareAuraScript(spell_covens_cosmic_glare_aura_SpellScript);

            void HandleAfterRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                Unit* l_Target = GetTarget();
                if (!l_Target || l_Target->HasAura(eSpells::CosmicGlareAura2))
                    return;

                l_Target->CastSpell(l_Target, eSpells::CosmicGlareSummon, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_covens_cosmic_glare_aura_SpellScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_MOD_SCALE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_covens_cosmic_glare_aura_SpellScript();
        }
};

///< 126320 - Cosmic Glare
class npc_covens_cosmic_glare : public CreatureScript
{
    public:
        npc_covens_cosmic_glare() : CreatureScript("npc_covens_cosmic_glare") { }

        struct npc_covens_cosmic_glareAI : public ScriptedAI
        {
            npc_covens_cosmic_glareAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetDisplayId(23767);
                me->AddUnitState(UNIT_STATE_ROOT);
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                me->SetReactState(REACT_PASSIVE);
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                SearchTarget();
            }

            bool CanTargetOutOfLOS() override
            {
                return true;
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex /*p_EffIndex*/) override
            {
                if (p_Targets.empty() || p_Spell->GetSpellInfo()->Id != eSpells::CosmicGlareDamage)
                    return;

                InstanceScript* l_Instance = me->GetInstanceScript();
                if (!l_Instance)
                    return;

                Creature* l_Thuraya = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::NpcThurayaMotherOfTheCosmos));
                if (!l_Thuraya|| !l_Thuraya->GetAI())
                    return;

                p_Targets.remove_if([this, l_Thuraya](WorldObject* p_Object) -> bool
                {
                    if (!p_Object)
                        return true;

                    if (boss_thuraya_mother_cosmos::boss_thuraya_mother_cosmosAI* l_LinkAI = CAST_AI(boss_thuraya_mother_cosmos::boss_thuraya_mother_cosmosAI, l_Thuraya->GetAI()))
                        if (l_LinkAI->HasTargetCosmicGlare(p_Object->GetGUID()))
                            return true;

                    return false;
                });
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::CosmicGlareDamage)
                {
                    InstanceScript* l_Instance = me->GetInstanceScript();
                    if (!l_Instance)
                        return;

                    Creature* l_Thuraya = Creature::GetCreature(*me, l_Instance->GetData64(eCreatures::NpcThurayaMotherOfTheCosmos));
                    if (!l_Thuraya || !l_Thuraya->GetAI())
                        return;

                    if (boss_thuraya_mother_cosmos::boss_thuraya_mother_cosmosAI* l_LinkAI = CAST_AI(boss_thuraya_mother_cosmos::boss_thuraya_mother_cosmosAI, l_Thuraya->GetAI()))
                        l_LinkAI->SetTargetCosmicGlare(p_Target->GetGUID());
                }
            }

            void CastSpellOnFrontAndBack()
            {
               for (uint8 l_Itr = 0; l_Itr < 20; ++l_Itr)
               {
                  Position l_BackPos;
                  me->GetNearPosition(l_BackPos, l_Itr * 5.0f, M_PI); ///< back
                  me->DelayedCastSpell(&l_BackPos, eSpells::CosmicGlareDamage, true, l_Itr == 0 ? 1 : l_Itr * 100);
                  Position l_FrontPos;
                  me->GetNearPosition(l_FrontPos, l_Itr * 5.0f, 0.0f); ///< front
                  me->DelayedCastSpell(&l_FrontPos, eSpells::CosmicGlareDamage, true, l_Itr == 0 ? 1 : l_Itr * 100);
               }
            }

            void SearchTarget()
            {
                std::list<Player*> playerList;
                GetPlayerListInGrid(playerList, me, 150.0f);
                if (!playerList.empty())
                {
                    for (Player* l_Player : playerList)
                    {
                        if (l_Player && l_Player->HasAura(eSpells::CosmicGlareAura2))
                        {
                            me->SetFacingTo(me->GetAngle(l_Player));
                            CastSpellOnFrontAndBack();
                            me->DespawnOrUnsummon(2500);
                            break;
                        }
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_covens_cosmic_glareAI(p_Creature);
        }
};

/// Last Update 7.3.5 Build 26365
/// Orb of Frost 253650
class spell_at_covens_orb_of_frost : public AreaTriggerEntityScript
{
    public:
        spell_at_covens_orb_of_frost() : AreaTriggerEntityScript("spell_at_covens_orb_of_frost") {}

        std::list<uint64> m_Targets;
        uint32 m_CheckTimer = 0;

        void ReduceAmount(Unit* l_Target, AreaTrigger* p_AreaTrigger)
        {
            if (p_AreaTrigger)
            {
                if (AuraEffect* l_SpeedEffect = l_Target->GetAuraEffect(eSpells::OrbofFrostAuraSlow, EFFECT_0))
                {
                    float l_Distance = l_Target->GetDistance(p_AreaTrigger);
                    float l_MinDistance = 1.0f;
                    float l_MaxDistance = 30.0f;
                    float l_MaxAmount = 85.0f;

                    if (l_Distance > l_MaxDistance)
                        l_Distance = l_MaxDistance;

                    float l_Basepoint = float(l_MaxAmount * (1.0f - (l_Distance / l_MaxDistance)));
                    if (l_Distance <= l_MinDistance)
                        l_Basepoint = l_MaxAmount;

                    int32 l_NewAmount = int32(l_Basepoint == 0.0f ? 0 : (int32)-l_Basepoint);
                    int32 l_OldAmount = l_SpeedEffect->GetAmount();
                    if (l_OldAmount != l_NewAmount)
                    {
                        l_SpeedEffect->ChangeAmount(l_NewAmount);
                        l_Target->UpdateSpeed(MOVE_RUN, true);
                        l_Target->UpdateSpeed(MOVE_SWIM, true);
                        l_Target->UpdateSpeed(MOVE_FLIGHT, true);
                        l_Target->UpdateSpeed(MOVE_RUN_BACK, true);
                        l_Target->UpdateSpeed(MOVE_SWIM_BACK, true);
                        l_Target->UpdateSpeed(MOVE_FLIGHT_BACK, true);
                    }
                }
            }
        }

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

                float l_Radius = 30.0f;
                std::list<uint64> l_Targets(m_Targets);
                for (uint64 l_TargetGuid : l_Targets)
                {
                    Player* l_Player = ObjectAccessor::GetPlayer(*l_AreaTriggerCaster, l_TargetGuid);
                    if (!l_Player)
                        continue;

                    if (l_Player->HasAura(eSpells::OrbofFrostAuraSlow) && l_Player->GetDistance(p_AreaTrigger) < l_Radius)
                        continue;

                    if (l_Player->HasAura(eSpells::OrbofFrostAuraSlow))
                        l_Player->RemoveAurasDueToSpell(eSpells::OrbofFrostAuraSlow);

                    m_Targets.remove(l_TargetGuid);
                }

                std::list<Unit*> l_PlayerList;
                p_AreaTrigger->GetAttackableUnitListInPositionRange(l_PlayerList, *p_AreaTrigger, l_Radius, true);
                l_PlayerList.remove_if(CheckDeadOrGmTarget());
                l_PlayerList.remove_if([](WorldObject* p_Target) -> bool
                {
                    if (!p_Target->IsPlayer())
                        return true;

                    return false;
                });

                if (!l_PlayerList.empty())
                {
                    SpellInfo const* p_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::OrbofFrostAuraSlow);
                    for (Unit* l_Player : l_PlayerList)
                    {
                        if (!l_Player || !p_SpellInfo || l_Player->IsImmunedToSpell(p_SpellInfo) || l_Player->HasAura(eSpells::OrbofFrostAuraSlow))
                            continue;

                        l_Player->CastSpell(l_Player, eSpells::OrbofFrostAuraSlow, true);
                        ReduceAmount(l_Player, p_AreaTrigger);
                        m_Targets.push_back(l_Player->GetGUID());
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

                if (l_Player->HasAura(eSpells::OrbofFrostAuraSlow))
                    l_Player->RemoveAurasDueToSpell(eSpells::OrbofFrostAuraSlow);

                m_Targets.remove(l_TargetGuid);
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_covens_orb_of_frost();
        }
};

/// Last Update 7.3.5 Build 26365
/// Shadow Blades - 246374
class spell_covens_shadow_blades_damage : public SpellScriptLoader
{
    public:
        spell_covens_shadow_blades_damage() : SpellScriptLoader("spell_covens_shadow_blades_damage") { }

        class spell_covens_shadow_blades_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_covens_shadow_blades_damage_SpellScript);

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (Creature* l_ShadowBlade = l_Caster->ToCreature())
                   if (l_ShadowBlade->AI())
                       l_ShadowBlade->AI()->DoAction(0);
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_covens_shadow_blades_damage_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_covens_shadow_blades_damage_SpellScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_the_coven_of_shivarra()
{
    ///< Bosses
    new boss_asara_mother_night();
    new boss_noura_mother_flames();
    new boss_diima_mother_gloom();
    /// < Mythic >
    new boss_thuraya_mother_cosmos();

    ///< Spells
    new spell_covens_torment_of_amanthul_intro();
    new spell_covens_torment_of_khazgoroth_intro();
    new spell_covens_torment_of_norgannon_intro();
    new spell_covens_torment_of_golganneth_intro();
    new spell_covens_torment_of_amanthul_cast();
    new spell_covens_torment_of_khazgoroth_cast();
    new spell_covens_torment_of_norgannon_cast();
    new spell_covens_torment_of_golganneth_cast();
    new spell_covens_touch_of_darkness();
    new spell_covens_whirling_saber_target();
    new spell_covens_fulminating_pulse_target();
    new spell_covens_fulminating_pulse_aura();
    new spell_covens_chilled_blood_target();
    new spell_covens_chilled_blood_aura();
    new spell_covens_orb_of_frost_aura();
    new spell_covens_orb_of_frost_target();
    new spell_covens_flames_of_khazgoroth();
    new spell_covens_fury_of_golganneth_aura();
    new spell_covens_fury_of_golganneth_damage();
    new spell_covens_shadow_blades_damage();
    /// < Mythic >
    new spell_covens_touch_of_the_cosmos();
    new spell_covens_cosmic_glare_aura();

    ///< Creatures
    new npc_covens_torment_of_amanthul();
    new npc_covens_torment_of_khazgoroth();
    new npc_covens_torment_of_golganneth();
    new npc_covens_torment_of_norgannon();
    new npc_covens_shadow_blades();
    new npc_covens_whirling_saber();
    /// < Mythic >
    new npc_covens_cosmic_glare();

    ///< AreaTriggers
    new spell_at_covens_storm_darkness_not_safe();
    new spell_at_covens_storm_darkness_safe();
    new spell_at_covens_spectral_army_norgannon();
    new spell_at_covens_fury_of_golganneth();
    new spell_at_covens_orb_of_frost();
}
#endif
