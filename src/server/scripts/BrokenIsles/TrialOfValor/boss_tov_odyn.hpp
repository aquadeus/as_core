////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef BOSS_TOV_ODYN_HPP
# define BOSS_TOV_ODYN_HPP

# include "trial_of_valor.hpp"

enum eGeneralTalks
{
    TalkAggro = 7,
    TalkKill = 13,

    /// Intro
    TalkOdyn01 = 1,
    TalkOdyn02 = 2,
    TalkHymdallHyrja01 = 5,
    TalkOdyn03 = 4,
    TalkOdyn04 = 6,
    TalkPhase02 = 7,
    TalkPhase03 = 8,
    TalkStormOfJustice = 9,
    TalkStormforgedSpear = 10,

    TalkReinforeceRuneWarn = 16,
    TalkReinforceRunesNormal = 11,
    TalkReinforceRunesHeroic = 15,

    TalkEndFight = 14,

    TalkUnerringBlast = 12
};

enum eActions
{
    ActionStartPhase01 = 1,
    ActionStartPhase02,
    ActionTestOfTheAges,
    ActionStartPhase03,
    ActionWinEncounter,
    ActionRevivfy,
    ActionOdynIntro,
    ActionOdynRunes,
    ActionClearAllRunesAura
};

enum ePhases
{
    Phase01 = 1,
    Phase02,
    Phase03
};

enum eOrphanVisual
{
    OrphanVisualShottingUnerringBlastProjectile = 61819
};

Position const g_PositionOdynsPortalToHauvstald = { 3336.507f, 5878.790f, 265.121f, 5.567914f };

Position const g_PositionOdynJumpTo2ndPhase = { 2418.696f, 528.721f, 748.995f, 0.0f };

Position const g_PositionValajrRunebearersSpawns[8] =
{
    { 2498.250f, 500.465f, 748.995f, 2.4258f },
    { 2474.607f, 484.874f, 748.995f, 1.8123f },
    { 2440.791f, 481.193f, 748.995f, 1.6780f },
    { 2405.936f, 484.673f, 748.994f, 1.5280f },
    { 2406.954f, 573.115f, 749.021f, 4.9830f },
    { 2440.043f, 576.860f, 748.995f, 4.7795f },
    { 2474.150f, 572.660f, 749.043f, 4.5636f },
    { 2498.597f, 556.124f, 749.001f, 3.8331f }
};

std::vector<G3D::Vector3> const g_HyrjaReturnBack =
{
    { 2407.98f, 550.259f,  749.053f },
    { 2415.61f, 553.931f,  785.117f },
    { 2403.42f, 582.627f,  781.976f },
    { 2404.82f, 578.330f,  772.893f }
};

std::vector<G3D::Vector3> const g_HyrjaEnterPlatform =
{
    { 2400.99f, 582.252f,  805.918f },
    { 2439.68f, 541.544f,  748.994f }
};

std::vector<Position> const g_HymdallReturnBack =
{
    {2405.541f, 483.159f, 777.565f, 4.328872f }, ///< Jump
    {2402.459f, 470.194f, 772.892f, 1.387563f }, ///< Jump
    {2404.447f, 481.804f, 772.892f, 1.360075f }  ///< MovePoint
};

/// 1 is for 3rd phase 2nd is for 1st phase.
std::vector<G3D::Vector3> const g_UnerringBlastCoords
{
   { 2411.049f, 493.718f, 748.994f },
   { 2416.358f, 564.036f, 748.994f },
   { 2493.964f, 517.937f, 748.994f },
   { 2434.625f, 534.583f, 748.994f },
   { 2487.57f,  529.132f, 748.994f },
   { 2443.637f, 545.941f, 748.994f },
   { 2394.413f, 562.567f, 748.994f },
   { 2478.615f, 547.239f, 748.994f },
   { 2426.201f, 506.888f, 748.994f },
   { 2464.155f, 561.491f, 748.994f },
   { 2472.302f, 533.314f, 748.994f },
   { 2414.528f, 508.340f, 748.994f },
   { 2457.531f, 495.076f, 748.994f },
   { 2464.523f, 512.295f, 748.994f },
   { 2483.828f, 519.570f, 748.996f },
   { 2408.521f, 522.249f, 748.995f },
   { 2413.170f, 542.030f, 748.995f },
   { 2430.922f, 552.770f, 748.995f },
   { 2437.762f, 538.067f, 748.995f },
   { 2446.780f, 516.572f, 748.995f },
   { 2463.407f, 498.106f, 748.995f },
   { 2465.667f, 530.999f, 748.995f },
   { 2470.997f, 551.022f, 748.995f },
   { 2489.648f, 542.490f, 748.995f },
   { 2496.285f, 521.518f, 748.995f },
   { 2428.663f, 507.401f, 748.995f }
};

Position const g_CleansingFlame = { 2428.83f, 528.594f, 748.995f, 0.0f };

#endif ///< BOSS_TOV_ODYN_HPP
