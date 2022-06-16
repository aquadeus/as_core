////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "the_emerald_nightmare.hpp"

Position const g_SumPos[4] =
{
    { -12818.10f, 12328.30f, -245.72f, 3.79f },   ///< 105906 - Eye of Ilgynoth
    { -12843.59f, 12331.40f, -246.01f, 3.56f },
    { -12821.79f, 12302.79f, -246.01f, 3.83f },
    { -12856.12f, 12296.79f, -253.16f, 0.72f }    ///< Horror
};

std::vector<Position> g_RandomPositions =
{
    { -12847.92f, 12334.87f, -249.3293f, 0.078896f },
    { -12849.17f, 12338.94f, -249.2572f, 5.396996f },
    { -12837.65f, 12332.17f, -247.9438f, 0.078896f },
    { -12852.44f, 12341.82f, -249.8538f, 0.075850f },
    { -12838.87f, 12327.64f, -249.1769f, 0.086207f },
    { -12819.01f, 12308.67f, -247.3162f, 5.396996f },
    { -12820.23f, 12297.10f, -248.0800f, 0.078896f },
    { -12816.94f, 12295.48f, -247.6019f, 0.075850f },
    { -12817.21f, 12290.93f, -248.6207f, 0.078896f },
    { -12825.15f, 12308.94f, -248.6615f, 0.086207f }
};

Position const g_BlossPos[22] =
{
    { -12843.5f, 12321.6f, -250.77f, 0.0f },
    { -12840.4f, 12297.1f, -251.92f, 0.0f },
    { -12822.7f, 12281.8f, -250.72f, 0.0f },
    { -12832.5f, 12290.1f, -251.82f, 0.0f },
    { -12848.9f, 12327.6f, -250.40f, 0.0f },
    { -12837.4f, 12314.9f, -250.81f, 0.0f },
    { -12831.4f, 12308.4f, -250.07f, 0.0f },
    { -12824.1f, 12300.7f, -248.79f, 0.0f },
    { -12814.5f, 12290.5f, -248.35f, 0.0f },
    { -12848.0f, 12303.3f, -252.25f, 0.0f },
    { -12854.7f, 12310.2f, -252.30f, 0.0f },
    { -12861.6f, 12319.4f, -252.85f, 0.0f },
    { -12852.3f, 12319.2f, -251.93f, 0.0f },
    { -12844.8f, 12311.6f, -251.94f, 0.0f },
    { -12839.7f, 12305.9f, -251.59f, 0.0f },
    { -12831.2f, 12298.9f, -250.53f, 0.0f },
    { -12823.5f, 12291.4f, -249.49f, 0.0f },
    { -12830.6f, 12277.5f, -252.17f, 0.0f },
    { -12842.2f, 12285.1f, -253.12f, 0.0f },
    { -12852.0f, 12290.9f, -253.25f, 0.0f },
    { -12858.0f, 12298.5f, -253.22f, 0.0f },
    { -12864.3f, 12307.0f, -253.70f, 0.0f }
};

struct SpawnData
{
    uint32      SpawnTimer;
    Position    SpawnPos;
    uint32      SpawnEntry;
};

/// Only used for tentacles
static std::array<std::vector<SpawnData>, 2> g_SpawnPattern =
{
    {
        /// First P1
        {
            { 20 * TimeConstants::IN_MILLISECONDS,  { 0.0f,       0.0f,       0.0f,      0.0f     }, NPC_DEATHGLARE_TENTACLE },
            { 90 * TimeConstants::IN_MILLISECONDS,  { 0.0f,       0.0f,       0.0f,      0.0f     }, NPC_CORRUPTOR_TENTACLE  },
            { 90 * TimeConstants::IN_MILLISECONDS,  { 0.0f,       0.0f,       0.0f,      0.0f     }, NPC_CORRUPTOR_TENTACLE  },
            { 90 * TimeConstants::IN_MILLISECONDS,  { 0.0f,       0.0f,       0.0f,      0.0f     }, NPC_DEATHGLARE_TENTACLE },
            { 90 * TimeConstants::IN_MILLISECONDS,  { 0.0f,       0.0f,       0.0f,      0.0f     }, NPC_DEATHGLARE_TENTACLE },
            { 150 * TimeConstants::IN_MILLISECONDS, { 0.0f,       0.0f,       0.0f,      0.0f     }, NPC_DEATHGLARE_TENTACLE },
            { 180 * TimeConstants::IN_MILLISECONDS, { 0.0f,       0.0f,       0.0f,      0.0f     }, NPC_CORRUPTOR_TENTACLE  },
            { 180 * TimeConstants::IN_MILLISECONDS, { 0.0f,       0.0f,       0.0f,      0.0f     }, NPC_CORRUPTOR_TENTACLE  }
        },
        /// Second P1
        {
            { 05 * TimeConstants::IN_MILLISECONDS,  { -12862.73f, 12275.18f, -255.2354f, 0.90112f }, NPC_DOMINATOR_TENTACLE  },
            { 05 * TimeConstants::IN_MILLISECONDS,  { -12876.79f, 12291.41f, -256.7021f, 0.59481f }, NPC_DOMINATOR_TENTACLE  },
            { 30 * TimeConstants::IN_MILLISECONDS,  { -12847.92f, 12334.87f, -249.3293f, 0.07890f }, NPC_DEATHGLARE_TENTACLE },
            { 30 * TimeConstants::IN_MILLISECONDS,  { -12849.17f, 12338.94f, -249.2572f, 5.39700f }, NPC_DEATHGLARE_TENTACLE },
            { 55 * TimeConstants::IN_MILLISECONDS,  { -12786.07f, 12251.54f, -248.7191f, 2.42856f }, NPC_CORRUPTOR_TENTACLE  },
            { 55 * TimeConstants::IN_MILLISECONDS,  { -12781.89f, 12267.31f, -247.4853f, 2.53908f }, NPC_CORRUPTOR_TENTACLE  },
            { 55 * TimeConstants::IN_MILLISECONDS,  { -12777.38f, 12259.65f, -246.6829f, 2.52337f }, NPC_CORRUPTOR_TENTACLE  },
            { 120 * TimeConstants::IN_MILLISECONDS, { -12847.92f, 12334.87f, -249.3293f, 0.07890f }, NPC_DEATHGLARE_TENTACLE },
            { 120 * TimeConstants::IN_MILLISECONDS, { -12849.17f, 12338.94f, -249.2572f, 5.39700f }, NPC_DEATHGLARE_TENTACLE },
            { 135 * TimeConstants::IN_MILLISECONDS, { -12885.43f, 12367.24f, -250.5963f, 5.36494f }, NPC_CORRUPTOR_TENTACLE  },
            { 135 * TimeConstants::IN_MILLISECONDS, { -12862.77f, 12377.55f, -246.5188f, 5.06806f }, NPC_CORRUPTOR_TENTACLE  }
        }
    }
};
