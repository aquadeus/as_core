////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

enum eDalaranPhasemasks
{
    PhaseStandard  = 0x01,
    PhaseGuards    = 0x02,
};

static std::array<Position, 44> g_ChestSpawnCoords
{
    {
        { -876.3980f, 4467.520f, 702.4210f, 5.485f },
        { -863.1740f, 4473.830f, 696.6590f, 4.997f },
        { -885.5620f, 4454.060f, 697.7140f, 5.966f },
        { -878.8350f, 4455.750f, 697.5000f, 0.005f },
        { -796.3800f, 4473.420f, 701.7090f, 2.214f },
        { -867.7340f, 4472.800f, 696.6590f, 6.177f },
        { -818.3980f, 4476.610f, 696.7030f, 2.571f },
        { -790.6150f, 4450.150f, 704.2780f, 4.144f },
        { -872.3510f, 4456.280f, 696.6590f, 5.117f },
        { -829.2140f, 4512.790f, 696.7030f, 5.602f },
        { -724.1927f, 4381.330f, 697.2838f, 1.861f },
        { -718.1597f, 4400.679f, 697.2679f, 3.411f },
        { -708.8489f, 4392.652f, 699.5883f, 3.021f },
        { -725.8871f, 4409.631f, 697.2308f, 3.655f },
        { -734.5417f, 4406.978f, 700.5217f, 4.649f },
        { -725.0590f, 4393.626f, 697.2702f, 2.436f },
        { -829.2136f, 4512.795f, 696.7034f, 5.602f },
        { -731.7500f, 4394.144f, 700.5940f, 1.924f },
        { -879.1493f, 4458.298f, 696.8600f, 2.070f },
        { -840.4167f, 4498.152f, 696.7034f, 4.780f },
        { -887.3820f, 4425.456f, 704.2963f, 0.283f },
        { -882.0555f, 4434.386f, 696.7913f, 6.261f },
        { -889.0104f, 4437.207f, 696.6592f, 6.070f },
        { -884.2309f, 4420.983f, 702.0956f, 0.827f },
        { -870.7031f, 4418.454f, 696.6592f, 1.570f },
        { -772.9827f, 4483.930f, 703.2183f, 3.654f },
        { -880.3820f, 4419.383f, 697.2352f, 1.196f },
        { -884.2309f, 4420.983f, 702.0956f, 0.827f },
        { -882.0555f, 4434.386f, 696.7913f, 6.261f },
        { -870.7031f, 4418.454f, 696.6592f, 1.570f },
        { -880.3820f, 4419.383f, 697.2352f, 1.196f },
        { -887.3820f, 4425.456f, 704.2963f, 0.283f },
        { -889.0104f, 4437.207f, 696.6592f, 6.070f },
        { -796.3802f, 4473.422f, 701.7095f, 2.214f },
        { -810.1754f, 4447.908f, 706.5645f, 4.452f },
        { -819.5486f, 4440.872f, 710.3885f, 6.213f },
        { -802.6406f, 4434.328f, 708.0987f, 0.993f },
        { -795.1788f, 4419.577f, 709.7897f, 2.410f },
        { -811.7604f, 4431.129f, 710.2068f, 2.382f },
        { -785.9219f, 4427.717f, 710.9764f, 3.522f },
        { -808.2153f, 4407.556f, 712.2763f, 0.950f },
        { -872.4219f, 4398.548f, 702.0956f, 1.498f },
        { -778.5000f, 4540.888f, 696.6592f, 3.529f },
        { -872.4220f, 4398.550f, 702.0960f, 1.498f }
    }
};

enum eCreatures
{
    NpcCrazedMage        = 97587,
    NpcThievingScoundrel = 97390,
    NpcXullorax          = 97388,
    NpcSplint            = 97380,
    NpcManaSeeper        = 97387,
    NpcScreek            = 97381,
    NpcSegacedi          = 97384,
    NpcRottenEgg         = 97589,
    NpcDalaranBodyGuard  = 103112
};

static const std::array<uint32, 8> g_RareMobSEntries{ { NpcCrazedMage, NpcThievingScoundrel, NpcXullorax, NpcSplint, NpcManaSeeper, NpcScreek, NpcSegacedi, NpcRottenEgg } };

static std::map<uint32, Position> g_RareMobsSpawnLocs =
{
    { std::make_pair(eCreatures::NpcCrazedMage,        Position(-884.3727f, 4441.4644f, 696.6597f, 0.0f)) },
    { std::make_pair(eCreatures::NpcThievingScoundrel, Position(-907.5469f, 4369.5278f, 694.2419f, 0.0f)) },
    { std::make_pair(eCreatures::NpcXullorax,          Position(-784.2153f, 4534.0342f, 696.6595f, 0.0f)) },
    { std::make_pair(eCreatures::NpcSplint,            Position(-865.9040f, 4467.3599f, 696.0226f, 0.0f)) },
    { std::make_pair(eCreatures::NpcManaSeeper,        Position(-792.0043f, 4448.6182f, 704.6012f, 0.0f)) },
    { std::make_pair(eCreatures::NpcScreek,            Position(-834.9299f, 4471.5278f, 696.6779f, 0.0f)) },
    { std::make_pair(eCreatures::NpcSegacedi,          Position(-726.3710f, 4406.6455f, 694.3794f, 0.0f)) },
    { std::make_pair(eCreatures::NpcRottenEgg,         Position(-792.6104f, 4430.7964f, 706.8322f, 0.0f)) },
};

enum eGpsData
{
    DalaranSewersMapID       = 1502,
    DalaranZoneID            = 8392,
    DalaranWorldZoneID       = 7502,
    DalaranUnderbellyAreaFFA = 7594,
    DalaranSanctuaryArea     = 7825
};

static const std::vector<uint32> g_DalaranSewersAreas = { DalaranUnderbellyAreaFFA, DalaranSanctuaryArea };

enum eDalaranSewersSpells
{
    AuraHiredGuard                   = 203894,
    AuraSewerGuard                   = 192229, ///< FFA off
    AuraNoGuards                     = 192221, ///< FFA on
    AuraFairGame                     = 223176, ///< Keep player in FFA mode for 8 seconds when leaving the FFA area
    SpellSummonDalaranSewerBodyguard = 203892,
    SpellAuraExperimentalPotion      = 219725
};

enum eGameObjects
{
    GobUnderbellyTreasure  = 244597,
    GobUnderbellyCache     = 244598,
    GobUnderbellyBooty     = 244599,
    GobTrapsTreasureVisual = 244605,
};

enum eDalaranSewersActions
{
};

enum eGraveyards
{
};
