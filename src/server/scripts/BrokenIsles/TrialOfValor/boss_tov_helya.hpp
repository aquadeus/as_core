////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef BOSS_TOV_HELYA_HPP
# define BOSS_TOV_HELYA_HPP

# include "trial_of_valor.hpp"

enum eHelyaData
{
    /// Counter data
    MaxGrippingTentacle         = 9,
    MaxGrippingTentacleMythic   = 8,
    MaxSides                    = 2,
    MaxStrikingTentacle         = 5,
    MaxFrontTentacles           = 2,
    MaxBackTentacles            = 3,
    MaxTentacleVisuals          = 19,
    MaxDecayingMinions          = 10,
    MaxMistwatchers             = 2,
    MaxMistwatchersIDx          = 3,

    /// Script data
    TentaclePosition            = 0
};

std::array<Position const, eHelyaData::MaxGrippingTentacle> g_GrippingTentaclePos =
{
    {
        { 500.2309f, 698.9722f, 2.877363f, 4.188790f },
        { 543.9167f, 676.7604f, 2.877363f, 4.206244f },
        { 482.5851f, 606.3368f, 2.877363f, 1.431170f },
        { 501.7535f, 601.9427f, 2.877363f, 1.431170f },
        { 468.8056f, 616.0330f, 2.877363f, 2.408554f },
        { 487.9236f, 701.2535f, 25.09828f, 3.193953f },
        { 556.7899f, 674.8438f, 18.23333f, 5.201081f },
        { 579.9722f, 667.9254f, 21.16156f, 3.211406f },
        { 473.8924f, 721.2708f, 22.44409f, 4.485496f }
    }
};

std::array<std::pair<uint32, Position const>, eHelyaData::MaxGrippingTentacle> g_GrippingTentacleRealPos =
{
    {
        { eTovCreatures::CreatureGrippingTentacle1, { 500.2309f, 698.9722f, 2.960696f, 4.188790f } },
        { eTovCreatures::CreatureGrippingTentacle1, { 543.9167f, 676.7604f, 2.960696f, 4.206244f } },
        { eTovCreatures::CreatureGrippingTentacle1, { 468.8056f, 616.0330f, 2.960696f, 2.408554f } },
        { eTovCreatures::CreatureGrippingTentacle1, { 556.7899f, 674.8438f, 18.31666f, 5.201081f } },
        { eTovCreatures::CreatureGrippingTentacle2, { 482.5851f, 606.3368f, 2.960696f, 1.431170f } },
        { eTovCreatures::CreatureGrippingTentacle2, { 501.7535f, 601.9427f, 2.960696f, 1.431170f } },
        { eTovCreatures::CreatureGrippingTentacle2, { 487.9236f, 701.2535f, 25.18161f, 3.193953f } },
        { eTovCreatures::CreatureGrippingTentacle2, { 579.9722f, 667.9254f, 21.24489f, 3.211406f } },
        { eTovCreatures::CreatureGrippingTentacle2, { 473.8924f, 721.2708f, 22.52743f, 4.485496f } }
    }
};

std::array<std::array<std::pair<uint32, Position const>, eHelyaData::MaxGrippingTentacleMythic / 2>, eHelyaData::MaxSides> g_GrippingTentacleRealMythicPos =
{
    {
        /// Left
        {
            {
                { eTovCreatures::CreatureGrippingTentacle2, { 487.9236f, 701.2535f, 25.18161f, 3.193953f } },
                { eTovCreatures::CreatureGrippingTentacle1, { 500.2309f, 698.9722f, 2.960696f, 4.188790f } },
                { eTovCreatures::CreatureGrippingTentacle2, { 473.8924f, 721.2708f, 22.52743f, 4.485496f } },
                { eTovCreatures::CreatureGrippingTentacle1, { 468.8056f, 616.0330f, 2.960696f, 2.408554f } }
            }
        },
        /// Right
        {
            {
                { eTovCreatures::CreatureGrippingTentacle1, { 556.7899f, 674.8438f, 18.31666f, 5.201081f } },
                { eTovCreatures::CreatureGrippingTentacle1, { 543.9167f, 676.7604f, 2.960696f, 4.206244f } },
                { eTovCreatures::CreatureGrippingTentacle2, { 579.9722f, 667.9254f, 21.24489f, 3.211406f } },
                { eTovCreatures::CreatureGrippingTentacle2, { 501.7535f, 601.9427f, 2.960696f, 1.431170f } }
            }
        }
    }
};

std::array<std::vector<Position>, eHelyaData::MaxSides> g_StrikingTentaclePos =
{
    {
        /// Front
        {
            { 508.380f, 688.9809f, 2.877361f, 4.596861f },
            { 534.115f, 681.2311f, 2.877361f, 3.931193f }
        },
        /// Back
        {
            { 508.165f, 598.6235f, 2.877361f, 1.649609f },
            { 491.216f, 600.4598f, 2.877361f, 1.304034f },
            { 477.292f, 605.6342f, 2.877361f, 1.099828f }
        }
    }
};

std::array<Position const, eHelyaData::MaxDecayingMinions> g_DecayingMinionsPos =
{
    {
        { 529.2761f, 702.0278f, -2.0631970f, 4.314158f },
        { 503.1823f, 583.6285f, -0.5291748f, 1.367723f },
        { 491.2552f, 586.1146f, -0.5291748f, 1.419904f },
        { 515.1875f, 580.2101f, -0.5291748f, 1.330113f },
        { 482.0417f, 590.7413f, -0.5291784f, 1.554500f },
        { 528.0590f, 583.3646f, -0.5291681f, 1.755625f },
        { 522.0399f, 709.5052f, -2.5139720f, 4.314158f },
        { 517.4809f, 719.1320f, -3.3860900f, 4.314158f },
        { 539.4479f, 699.8246f, -2.4324530f, 4.314158f },
        { 549.3594f, 698.9114f, -3.1203530f, 4.314158f }
    }
};

std::array<std::array<Position const, eHelyaData::MaxMistwatchers>, eHelyaData::MaxMistwatchersIDx> g_MistwachersPos =
{
    {
        {
            {
                { 470.06f, 703.876f, 17.965f, 3.91153f },
                { 563.76f, 655.199f, 17.955f, 4.18404f }
            }
        },
        {
            {
                { 461.93f, 652.804f, 5.0289f, 5.27559f },
                { 524.95f, 618.749f, 4.9298f, 2.23471f }
            }
        },
        {
            {
                { 486.72f, 664.001f, 2.0736f, 1.97553f },
                { 533.46f, 649.786f, 2.1960f, 5.80042f }
            }
        }
    }
};

std::vector<Position> g_LongboatSpawnPos =
{
    { 638.672f, 817.6201f, -14.82369f, 3.943812f },
    { 569.390f, 857.6201f, -14.82369f, 4.433769f }
};

Position const g_OrbOfCorruptionPos = { 524.0313f, 699.0555f, 5.259649f, 0.0f };

Position const g_SecondPhaseTeleportPos = { 552.3507f, 744.342f, 2.794028f, 4.18879f };
Position const g_FuryOfTheMawSpawnPos = { 578.031f, 792.5868f, 5.09298f, 4.153537f };

Position const g_LeftTentaclePos = { 534.936f, 674.542f, -84.9167f, 0.89158f };
Position const g_RightTentaclePos = { 523.082f, 712.904f, -70.8787f, 2.30488f };

Position const g_CenterPos = { 501.608f, 649.618f, 1.8451f, 1.161872f };

Position const g_ForcePos = { 528.1644f, 706.2152f, 5.09298f, 0.0f };

#endif ///< BOSS_TOV_HELYA_HPP
