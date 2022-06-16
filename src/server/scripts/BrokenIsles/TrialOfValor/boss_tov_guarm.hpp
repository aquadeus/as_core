////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2016 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef GUARM_HPP
# define GUARM_HPP

#include "trial_of_valor.hpp"

enum eTalks
{
    TalkHeadlongChargeWarning,
    TalkGuardiansBreathWarning  ///< |TInterface\Icons\SPELL_FIRE_TWILIGHTFLAMEBREATH.BLP:20|t%s begins to cast |cFFFF0000|Hspell:227573|h[Guardian's Breath]|h|r!
};

enum eEvents
{
    EventGuardianBreath = 1,
    EventHeadlongCharge,
    EventFlashingFang,
    EventFrothingRage,
    EventRoaringLeap,
    EventLick,
    EventOffTheLeash,
    EventEnrage,

    /// Mythic
    EventVolatileFoam,

    /// Meh.
    EventGuardianBreath02
};

enum eSpells
{
    GuarmChewToy                      = 231846,

    FrothingRage                      = 228174,

    FieryPhlegm                       = 227539,
    FieryPhlegmDebuff                 = 228758,
    FieryPhlegmDamage                 = 232777,

    SaltySpittle                      = 227566,
    SaltySpittleDebuff                = 228768,
    SaltySpittleDamage                = 232798,

    FrothingRageDmgPctDone            = 228174,

    DarkDischarge                     = 227570,
    DarkDischargeDebuff               = 228769,
    DarkDischargeDamage               = 232800,

    MultiHeadedAuraDummy              = 227512,
    MultiHeadedDmg                    = 227642,

    FlameLickMultiEffWithCastDuration = 228228,
    FlameLickTriggerMissile           = 228227,
    FlameLickEffDummy                 = 228226,

    FrostLickStunAura                 = 228248,
    FrostLickTriggerMissile           = 228247,
    FrostLickEffDummy                 = 228246,

    ShadowLickEffDummy                = 228250,
    ShadowLickTriggerMissile          = 228251,
    ShadowLickHealAbsorb              = 228253,

    /// Volatile Foam
    VolatileFoamDummy                 = 228684,
    FlamingVolatileFoamMultiEff01     = 228744,
    FlamingVolatileFoamMultiEff02     = 228794,

    VolatileFoamDummy02               = 228809,
    BrineyVolatileFoamMultiEff01      = 228810,
    BrineyVolatileFoamMultiEff02      = 228811,

    VolatileFoamDummy03               = 228817,
    ShadowyVolatileFoamMultiEff01     = 228818,
    ShadowyVolatileFoamMultiEff02     = 228819,

    VolatileFoamScriptEff             = 228824,

    RoaringLeapKnockBack              = 227883,
    RoaringLeapEffDummy               = 227894,
    RoaringLeapSchoolDamage           = 227902,
    RoaringLeapJumpDest               = 229350,

    HeadlongChargeEffDummy            = 227816,
    HeadlongChargeAuraAt              = 227833,
    HeadlongChargeSchoolDmg           = 228344,
    HeadlongChargeTriggerSpel         = 229480,

    GuardiansBreath                   = 227573, /// Breath, dummy, 3 breaths type.
    GuardiansBreathVisualBreath1      = 227658, ///< P, G, R
    GuardiansBreathVisualBreath2      = 227660, ///< G, P, R
    GuardiansBreathVisualBreath3      = 227666, ///< P, R, G
    GuardiansBreathVisualBreath4      = 227667, ///< R, P, G
    GuardiansBreathVisualBreath5      = 227669, ///< G, R, P
    GuardiansBreathVisualBreath6      = 227673, ///< R, G, P
    GuardiansBreathScriptEff02        = 228187,
    GuardiansBreathCreateAtRedFront   = 232773,
    GuardiansBreathCreateAtGreenRight = 232774,
    GuardiansBreathCreateAtPurpleLeft = 232776,
    GuardiansBreathCreateAtPurpleRight= 232801,
    GuardiansBreathCreateAtPurpleFront= 232802,
    GuardiansBreathCreateAtRedRight   = 232803,
    GuardiansBreathCreateAtRedLeft    = 232804,
    GuardiansBreathCreateAtGreenFront = 232805,
    GuardiansBreathCreateAtGreenLeft  = 232806,
    GuardiansBreathCombination1       = 232807, ///< P, G, R
    GuardiansBreathCombination2       = 232808, ///< G, P, R
    GuardiansBreathCombination3       = 232809, ///< P, R, G
    GuardiansBreathCombination4       = 232810, ///< R, P, G
    GuardiansBreathCombination5       = 232775, ///< G, R, P
    GuardiansBreathCombination6       = 232811, ///< R, G, P

    GuardiansImageDummyEff            = 228334,
    GuardiansImageDummyEff02          = 228338,
    GuardiansImageSummonEff           = 228339,
    GuardiansFamilliarMultiEf         = 230121,
    GuardiansFamilliarDmg             = 230166,

    Trample                           = 227843,

    OffTheLeash                       = 228201,

    FlashingFangsDmg                  = 227514,

    Enrage                            = 26662,

    BerserkCharge                     = 232173,

    MixedElements01                   = 227720,
    MixedElements02                   = 227721,
    MixedElements03                   = 227735
};

enum eMovementInformed
{
    GuremRoaringLeapHit = 1,
    HeadlongChargeStart,
    HeadlingChargeEnd,
    BerserkChargeStart,
    BerserkChargeEnd,
    MovementBreath,
    MovementBreath2
};

enum eTimers
{
    TimerEnragedNM   = 360 * TimeConstants::IN_MILLISECONDS,
    TimerEnragedHC   = 300 * TimeConstants::IN_MILLISECONDS,
    TimerEnragedMM   = 240 * TimeConstants::IN_MILLISECONDS,
    TimerOffTheLeash = 50 * TimeConstants::IN_MILLISECONDS
};

const std::vector<uint32> eTimersOffsetHeadlingCharge
{
    60 * TimeConstants::IN_MILLISECONDS,
    70 * TimeConstants::IN_MILLISECONDS,
    80 * TimeConstants::IN_MILLISECONDS,
    90 * TimeConstants::IN_MILLISECONDS
};

const std::vector<uint32> eTimersOffsetRoaringLeap
{
    40 * TimeConstants::IN_MILLISECONDS,
    21 * TimeConstants::IN_MILLISECONDS,
    35 * TimeConstants::IN_MILLISECONDS,
};

const std::vector<uint32> eTimersOffsetGuardiansBreath
{
    15 * TimeConstants::IN_MILLISECONDS,
    18 * TimeConstants::IN_MILLISECONDS,
    25 * TimeConstants::IN_MILLISECONDS
};

const std::vector<uint32>  eTimersOffsetFlashingFangs
{
    3 * TimeConstants::IN_MILLISECONDS,
    15 * TimeConstants::IN_MILLISECONDS,
    10 * TimeConstants::IN_MILLISECONDS,
    18 * TimeConstants::IN_MILLISECONDS
};

const std::vector<uint32> eTimersOffsetLick
{
    10 * TimeConstants::IN_MILLISECONDS,
    8 * TimeConstants::IN_MILLISECONDS,
    4 * TimeConstants::IN_MILLISECONDS,
    7 * TimeConstants::IN_MILLISECONDS,
    3 * TimeConstants::IN_MILLISECONDS
};

const std::vector<uint32> eTimersOffsetVolatileFoam
{
    9 * TimeConstants::IN_MILLISECONDS,
    7 * TimeConstants::IN_MILLISECONDS,
    14 * TimeConstants::IN_MILLISECONDS,
    12 * TimeConstants::IN_MILLISECONDS
};

/// 9 SEC duration.
const std::vector<uint32> g_VolatileFoamSpellsFullDurationVector =
{
    eSpells::ShadowyVolatileFoamMultiEff01,
    eSpells::FlamingVolatileFoamMultiEff01,
    eSpells::BrineyVolatileFoamMultiEff01
};

/// 4 SEC duration.
const std::vector<uint32> g_VolatileFoamSpellsShortDurationVector =
{
    eSpells::ShadowyVolatileFoamMultiEff02,
    eSpells::FlamingVolatileFoamMultiEff02,
    eSpells::BrineyVolatileFoamMultiEff02
};

const std::vector<uint32> g_LickSpellsEntriesVector =
{
    eSpells::ShadowLickTriggerMissile,
    eSpells::FlameLickTriggerMissile,
    eSpells::FrostLickTriggerMissile
};

const std::vector<uint32> g_LickAuras =
{
    eSpells::ShadowLickHealAbsorb,
    eSpells::FlameLickMultiEffWithCastDuration,
    eSpells::FrostLickStunAura
};

const std::vector<uint32> g_SpellsToRemove =
{
    eSpells::FrostLickStunAura, eSpells::FieryPhlegmDebuff, eSpells::FlameLickMultiEffWithCastDuration, eSpells::FieryPhlegmDebuff, eSpells::FrothingRage,
    eSpells::BerserkCharge, eSpells::Enrage, eSpells::FlameLickMultiEffWithCastDuration, eSpells::FrostLickStunAura, eSpells::ShadowLickHealAbsorb,
    eSpells::ShadowyVolatileFoamMultiEff01, eSpells::FlamingVolatileFoamMultiEff01, eSpells::BrineyVolatileFoamMultiEff01, eSpells::SaltySpittleDebuff, eSpells::FieryPhlegmDebuff, eSpells::ShadowLickHealAbsorb
};

const std::vector<G3D::Vector3> g_HeadlongCharge[2] =
{
    {
       { 481.186f, 443.651f, 5.057f }, ///, 1.161265f
       { 479.424f, 501.854f, 5.809f }, ///, 1.584876f
       { 478.441f, 550.094f, 2.651f }, ///, 1.698758f
       { 449.032f, 556.260f, 1.837f }, ///, 4.819140f
       { 452.525f, 499.583f, 5.914f }, ///, 4.752382f
       { 452.830f, 450.865f, 5.148f }  ///, 4.830918f
    },

    {
       { 452.9309f, 559.1723f, 0.9460f },
       { 450.5112f, 505.3584f, 5.4769f },
       { 453.5724f, 445.9250f, 5.1833f },
       { 485.1578f, 451.4817f, 5.1758f },
       { 481.1292f, 498.4098f, 5.9713f },
       { 478.4789f, 553.0405f, 2.6338f }
    }
};

const std::vector<float> g_HeadAngleSpawnDiff =
{
   +0.85252f,
    0.0f,
   -0.85752f
};

Position const g_PositionBeforeHeadlongCharge = { 481.044f, 445.430f, 5.08720f, 1.353416f };

Position const g_BereserkChargeSrc = { 464.143f, 555.734f, 1.934465f, 4.7749654f };

Position const g_BereserkChargeDest = { 469.232f, 442.803f, 5.211156f, 1.602715f };
#endif ///< GUARM_HPP
