////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2018 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

enum eScenarios
{
    Scenario_Val = 1406,
    Scenario_Aurinor = 1391,
    Scenario_Cengar = 1404,
    Scenario_Bonich = 1409,
    Scenario_Sangua = 1394,
    Scenario_Naigtal = 1411
};

enum eScenarioZones
{
    // Small Invasion Points
    Zone_Val = 9127,
    Zone_Aurinor = 9100,
    Zone_Cengar = 9126,
    Zone_Bonich = 9180,
    Zone_Sangua = 9128,
    Zone_Naigtal = 9102,
    // Greater Invasion Points
    Zone_Vilemus = 9296,
    Zone_Sotanathor = 9299,
    Zone_Alluradel = 9300,
    Zone_Folnuna = 9295,
    Zone_Meto = 9297,
    Zone_Occularus = 9298
};

enum eArgusZones
{
    Zone_Krokuun = 8574,
    Zone_Antoran = 8899,
    Zone_Mac = 8701
};

enum eGlobalSpells
{
    Spell_DemonSpawn = 251298,
    Spell_DemonEscape = 252309,
    Spell_FelDissolveIn = 237765,
    // Small Invasion Points Teleports
    Spell_RiftTeleportVal = 251222,
    Spell_RiftTeleportAurinor = 255190,
    Spell_RiftTeleportCengar = 251594,
    Spell_RiftTeleportBonich = 251226,
    Spell_RiftTeleportSangua = 251213,
    Spell_RiftTeleportNaigtal = 251210,
    // Greater Invasion Points Teleports
    Spell_RiftTeleportVilemus = 251674,
    Spell_RiftTeleportSotanathor = 251675,
    Spell_RiftTeleportMistressAlluradel = 251678,
    Spell_RiftTeleportMatronFolnuna = 251595,
    Spell_RiftTeleportInquisitorMeto = 251677,
    Spell_RiftTeleportOccularus = 251676
};

enum eGlobalGobs
{
    Gob_IllidariBanner = 276221
};

enum eGlobalNpcs
{
    Npc_InvasionPoint = 124884,
    Npc_DemonHunter = 127108,
    // Small Invasion Points
    Npc_InvasionPointVal = 126499,
    Npc_InvasionPointAurinor = 125849,
    Npc_InvasionPointCengar = 126120,
    Npc_InvasionPointBonich = 126547,
    Npc_InvasionPointSangua = 125863,
    Npc_InvasionPointNaigtal = 126593,
    // Greater Invasion Points
    Npc_GreaterInvasionPointPitLordVilemus = 127531,
    Npc_GreaterInvasionPointSotanathor = 127532,
    Npc_GreaterInvasionPointMistressAlluradel = 127536,
    Npc_GreaterInvasionPointMatronFolnuna = 127528,
    Npc_GreaterInvasionPointInquisitorMeto = 127535,
    Npc_GreaterInvasionPointOccularus = 127533,
    // Exit Portal
    Npc_InvasionPointExit = 124884,
    // Greater Invasion Point Bosses
    Npc_MatronFolnuna = 124514,
    Npc_PitLordVilemus = 124719,
    Npc_InquisitorMeto = 124592,
    Npc_Occularus = 124492,
    Npc_Sotanathor = 124555,
    Npc_MistressAlluradel = 124625
};

enum eGlobalPoints
{
    Point_DemonHunterMoveEnd = 1
};

enum eGlobalEvents
{
    Event_UnsummonDemons = 10
};

enum eGlobalDatas
{
    Data_CurrentModifier = 1
};

std::vector<uint32> const g_SmallInvasionPointBosses =
{
    125272, // Fel Lord Kaz'ral
    125578, // Malphazel
    125587, // Vogrethar the Defiled
    125137, // Mazgoroth
    125483, // Harbinger Drel'nathar
    125527, // Dreadbringer Valus
    125655, // Flamebringer Az'rothel
    125280, // Flamecaller Vezrah
    125252, // Dread Knight Zak'gal
    125634, // Vel'thrak the Punisher
    125314, // Flameweaver Verathix
    125666, // Baldrazar
    125148  // Gorgoloth
};

struct InvasionPointData
{
    InvasionPointData(uint32 p_PortalEntry, Position p_PortalSpawnPosition, uint16 p_ZoneId, uint16 p_ScenarioZoneId, uint32 p_TeleportSpellId, uint32 p_QuestId) :
    PortalEntry(p_PortalEntry), PortalSpawnPosition(p_PortalSpawnPosition), ZoneId(p_ZoneId), ScenarioZoneId(p_ScenarioZoneId), TeleportSpellId(p_TeleportSpellId), QuestId(p_QuestId)  {}

    uint32 PortalEntry;
    Position PortalSpawnPosition;
    uint16 ZoneId;
    uint16 ScenarioZoneId;
    uint32 TeleportSpellId;
    uint32 QuestId;
};

static std::map<uint16, InvasionPointData*> g_InvasionPointDatas =
{
    // Krokuun Small Invasion Points
    { 5350, new InvasionPointData(eGlobalNpcs::Npc_InvasionPointSangua, Position(1815.953f, 1045.214f, 501.5589f, 1.172006f), eArgusZones::Zone_Krokuun, eScenarioZones::Zone_Sangua, eGlobalSpells::Spell_RiftTeleportSangua, 49097) },
    { 5359, new InvasionPointData(eGlobalNpcs::Npc_InvasionPointCengar, Position(628.2518f, 1222.403f, 441.9999f, 0.09015972f), eArgusZones::Zone_Krokuun, eScenarioZones::Zone_Cengar, eGlobalSpells::Spell_RiftTeleportCengar, 49098) },
    { 5360, new InvasionPointData(eGlobalNpcs::Npc_InvasionPointVal, Position(736.7952f, 2027.215f, 390.5045f, 0.4336867f), eArgusZones::Zone_Krokuun, eScenarioZones::Zone_Val, eGlobalSpells::Spell_RiftTeleportVal, 49091) },
    // Antoran Small Invasion Points
    { 5369, new InvasionPointData(eGlobalNpcs::Npc_InvasionPointSangua, Position(-2544.95f, 8999.505f, -138.2191f, 0.5670658f), eArgusZones::Zone_Antoran, eScenarioZones::Zone_Sangua, eGlobalSpells::Spell_RiftTeleportSangua, 49097) },
    { 5370, new InvasionPointData(eGlobalNpcs::Npc_InvasionPointCengar, Position(-2763.939f, 9230.536f, -169.6172f, 1.814568f), eArgusZones::Zone_Antoran, eScenarioZones::Zone_Cengar, eGlobalSpells::Spell_RiftTeleportCengar, 49098) },
    { 5372, new InvasionPointData(eGlobalNpcs::Npc_InvasionPointVal, Position(-3267.783f, 8807.688f, -148.796f, 2.096528f), eArgusZones::Zone_Antoran, eScenarioZones::Zone_Val, eGlobalSpells::Spell_RiftTeleportVal, 49091) },
    { 5373, new InvasionPointData(eGlobalNpcs::Npc_InvasionPointAurinor, Position(-3357.363f, 9082.359f, -167.975f, 1.364537f), eArgusZones::Zone_Antoran, eScenarioZones::Zone_Aurinor, eGlobalSpells::Spell_RiftTeleportAurinor, 48982) },
    { 5374, new InvasionPointData(eGlobalNpcs::Npc_InvasionPointNaigtal, Position(-2458.948f, 9397.223f, -128.8401f, 1.616507f), eArgusZones::Zone_Antoran, eScenarioZones::Zone_Naigtal, eGlobalSpells::Spell_RiftTeleportNaigtal, 49096) },
    // Mac Small Invasion Points
    { 5366, new InvasionPointData(eGlobalNpcs::Npc_InvasionPointBonich, Position(6215.544f, 9557.909f, -79.62739f, 3.097998f), eArgusZones::Zone_Mac, eScenarioZones::Zone_Bonich, eGlobalSpells::Spell_RiftTeleportBonich, 49099) },
    { 5367, new InvasionPointData(eGlobalNpcs::Npc_InvasionPointAurinor, Position(6352.261f, 10281.95f, 38.63864f, 4.078101f), eArgusZones::Zone_Mac, eScenarioZones::Zone_Aurinor, eGlobalSpells::Spell_RiftTeleportAurinor, 48982) },
    { 5368, new InvasionPointData(eGlobalNpcs::Npc_InvasionPointNaigtal, Position(5786.783f, 9248.674f, -28.0219f, 2.338863f), eArgusZones::Zone_Mac, eScenarioZones::Zone_Naigtal, eGlobalSpells::Spell_RiftTeleportNaigtal, 49096) },
    // Greater Invasion Points
    { 5375, new InvasionPointData(eGlobalNpcs::Npc_GreaterInvasionPointMistressAlluradel, Position(-2692.599f, 8985.086f, -137.6158f, 0.6908751f), eArgusZones::Zone_Antoran, eScenarioZones::Zone_Alluradel, eGlobalSpells::Spell_RiftTeleportMistressAlluradel, 48620) },
    { 5376, new InvasionPointData(eGlobalNpcs::Npc_GreaterInvasionPointOccularus, Position(-2915.547f, 9377.62f, -161.1879f, 5.31516f), eArgusZones::Zone_Antoran, eScenarioZones::Zone_Occularus, eGlobalSpells::Spell_RiftTeleportOccularus, 49195) },
    { 5377, new InvasionPointData(eGlobalNpcs::Npc_GreaterInvasionPointPitLordVilemus, Position(-2473.896f, 9172.278f, -158.5752f, 4.654447f), eArgusZones::Zone_Antoran, eScenarioZones::Zone_Vilemus, eGlobalSpells::Spell_RiftTeleportVilemus, 49196) },
    { 5379, new InvasionPointData(eGlobalNpcs::Npc_GreaterInvasionPointInquisitorMeto, Position(-3220.726f, 9106.102f, -164.3851f, 5.513348f), eArgusZones::Zone_Antoran, eScenarioZones::Zone_Meto, eGlobalSpells::Spell_RiftTeleportInquisitorMeto, 49198) },
    { 5380, new InvasionPointData(eGlobalNpcs::Npc_GreaterInvasionPointSotanathor, Position(5519.492f, 10072.35f, -85.58864f, 1.023779f), eArgusZones::Zone_Mac, eScenarioZones::Zone_Sotanathor, eGlobalSpells::Spell_RiftTeleportSotanathor, 49197) },
    { 5381, new InvasionPointData(eGlobalNpcs::Npc_GreaterInvasionPointMatronFolnuna, Position(2016.802f, 1520.774f, 408.6734f, 5.387522f), eArgusZones::Zone_Krokuun, eScenarioZones::Zone_Folnuna, eGlobalSpells::Spell_RiftTeleportMatronFolnuna, 49199) }
};

namespace InvasionPointVal
{
    std::multimap<uint32, Position> const g_SecondStageCreaturesSpawnPositions =
    {
        { 125197, { -4125.328f, 624.7413f, 14.09526f, 5.755043f } },
        { 125197, { -4122.189f, 629.4496f, 14.0955f, 5.753866f } },
        { 125788, { -4058.901f, 530.6268f, -0.05346205f, 2.336391f } },
        { 125788, { -4079.958f, 624.8073f, 9.79238f, 4.87982f } },
        { 125199, { -4019.302f, 579.1684f, 0.5004724f, 3.8198f } },
        { 125788, { -4115.969f, 602.6458f, 11.73258f, 0.8508f } },
        { 125788, { -4047.538f, 604.4445f, 3.398548f, 3.670545f } },
        { 125199, { -4075.297f, 576.3594f, 4.78812f, 0.08615752f } },
        { 125788, { -3990.418f, 596.7049f, 1.29584f, 3.306336f } }
    };

    std::vector<Position> const g_IllidariBannersSpawnPositions =
    {
        { -4029.46f, 579.4011f, 0.4485643f, 2.398739f },
        { -4043.491f, 563.4879f, 0.4485643f, 2.398739f }
    };

    std::vector<Position> const g_DemonHunterSpawnPositions =
    {
        { -4033.439f, 568.9514f, 0.5318977f, 5.229022f },
        { -4033.439f, 568.9514f, 0.5318977f, 1.459022f },
        { -4033.439f, 568.9514f, 0.5318977f, 2.714706f },
        { -4033.439f, 568.9514f, 0.5318977f, 3.972599f },
        { -4033.439f, 568.9514f, 0.5318977f, 0.2023073f }
    };

    std::vector<Position> const g_IceTorrentSpawnPositions =
    {
        { -4137.26f, 633.8959f, 13.84172f, 6.084552f },
        { -4081.495f, 612.0957f, 6.693688f, 4.205145f },
        { -4112.135f, 614.7189f, 11.68436f, 0.3012444f },
        { -4062.267f, 589.5833f, 3.762804f, 4.577462f },
        { -4048.073f, 577.3782f, 2.000437f, 1.775474f },
        { -4066.965f, 541.5601f, 0.5241475f, 3.077103f },
        { -4025.255f, 603.3991f, 3.378144f, 1.400882f }
    };

    std::vector<Position> const g_BlizzardSpawnPositions =
    {
        { -4075.369f, 606.1109f, 5.560146f, 2.76443f },
        { -4071.389f, 604.8014f, 5.075615f, 0.3034014f },
        { -4062.997f, 565.1844f, 2.340672f, 4.86488f },
        { -4030.7f, 562.4739f, -0.1622313f, 1.890357f }
    };

    Position const g_BossSpawnPos = { -4035.54f, 571.6875f, 0.6500577f, 2.5145f };
    Position const g_PortalSpawnPos = { -4033.439f, 568.9514f, 0.5318977f, 1.618171f };

    enum eNpcs
    {
        Npc_DemonHunter = 126446,
        Npc_IceTorrent = 126842,
        Npc_Blizzard = 126769
    };

    enum eSteps
    {
        Step_SeizeAFoothold,
        Step_EnhancedEnemies,
        Step_ChangeInLeadership
    };

    enum eAssets
    {
        Asset_SeizeAFoothold = 59201,
        Asset_EnhancedEnemies = 59346,
        Asset_ChangeInLeadership = 59372
    };

    enum eSpells
    {
        Spell_CripplingCold = 251563,
        Spell_AfterFirstStageConversation = 251005,
        Spell_AfterSecondStageConversation = 251003,
        Spell_ScenarioCompletedConversation = 251014
    };

    enum eModifiers
    {
        Mod_BuffetingWinds = 1,
        Mod_CripplingCold = 2,
        Mod_Blizzard = 3,
        Mod_FlashFreeze = 4 // NYI
    };
}

namespace InvasionPointAurinor
{
    std::multimap<uint32, Position> const g_SecondStageCreaturesSpawnPositions =
    {
        { 125788, { -4105.03f, -4841.635f, 107.9643f, 5.769406f } },
        { 125788, { -4080.128f, -4756.575f, 87.22241f, 4.928212f } },
        { 125788, { -4097.944f, -4938.056f, 109.8031f, 1.600145f } },
        { 125788, { -4005.655f, -4938.234f, 118.9961f, 2.122577f } },
        { 125199, { -4116.519f, -4870.618f, 111.2754f, 1.595842f } },
        { 125199, { -4004.128f, -4964.078f, 123.9348f, 1.718102f } },
        { 125199, { -3997.51f, -4961.693f, 123.9348f, 1.625307f } },
        { 125788, { -4047.195f, -4900.649f, 112.6559f, 1.052702f } },
        { 125197, { -4056.783f, -4888.274f, 112.5082f, 0.6106985f } },
        { 125199, { -4058.056f, -4724.12f, 81.81429f, 3.709833f } },
        { 125788, { -4061.054f, -4718.922f, 81.81429f, 3.8198f } }
    };

    std::vector<Position> const g_IllidariBannersSpawnPositions =
    {
        { -4055.703f, -4662.953f, 80.46125f, 4.70962f   },
        { -4076.918f, -4663.325f, 80.46125f, 4.70962f   }
    };

    std::vector<Position> const g_DemonHunterSpawnPositions =
    {
        { -4066.524f, -4659.221f, 80.54459f, 1.256631f },
        { -4066.524f, -4659.221f, 80.54459f, 3.769896f },
        { -4066.524f, -4659.221f, 80.54459f, 2.51329f },
        { -4066.524f, -4659.221f, 80.54459f, 5.026554f },
        { -4066.524f, -4659.221f, 80.54459f, 0.0f }
    };

    Position const g_BossSpawnPos = { -4066.832f, -4662.799f, 80.54459f, 4.627432f };
    Position const g_PortalSpawnPos = { -4066.524f, -4659.221f, 80.54459f, 1.618171f };
    Position const g_DecimaxSpawnPos = { -3952.83f, -5076.479f, 231.9951f, 3.588549f };

    enum eNpcs
    {
        Npc_TemporalAnomaly = 125856,
        Npc_Decimax = 126007
    };

    enum eSteps
    {
        Step_SeizeAFoothold,
        Step_TemporalAnomalies,
        Step_KillTheHead
    };

    enum eAssets
    {
        Asset_SeizeAFoothold = 59201,
        Asset_TemporalAnomalies = 59346,
        Asset_KillTheHead = 59372
    };

    enum eSpells
    {
        // Modifier Spells
        Spell_TemporalPrecognition = 250252,
        Spell_PowerDilation = 250244,
        // Conversation Spells
        Spell_AfterFirstStageConversation = 248574,
        Spell_AfterSecondStageConversation = 251005,
        Spell_ScenarioCompletedConversation = 251014
    };

    enum eModifiers
    {
        Mod_TemporalPrecognition = 1,
        Mod_PowerDilation = 2,
        Mod_Decimax = 3,
        Mod_Astralfall = 4 // NYI
    };

    enum eActions
    {
        Action_DecimaxStartCirclePath = 1
    };
}

namespace InvasionPointCengar
{
    std::vector<Position> const g_SecondStageCreaturesSpawnPositions =
    {
        { 638.934f, 577.941f, 40.61733f, 0.7638926f },
        { 633.5226f, 580.1424f, 40.62674f, 0.7638926f },
        { 663.8924f, 616.3993f, 40.53472f, 4.740067f },
        { 644.0712f, 654.941f, 40.69618f, 1.202802f },
        { 652.1337f, 660.1111f, 40.80035f, 3.452601f },
        { 668.9375f, 611.6354f, 40.68224f, 3.522511f },
        { 641.0208f, 661.8993f, 40.45551f, 5.518114f },
        { 663.691f, 606.4236f, 40.88541f, 1.454035f }
    };

    std::vector<Position> const g_IllidariBannersSpawnPositions =
    {
        { 646.6945f, 605.1945f, 40.31978f, 1.030727f },
        { 665.1163f, 594.6597f, 40.31978f, 1.030727f }
    };

    std::vector<Position> const g_DemonHunterSpawnPositions =
    {
        { 654.0816f, 596.4531f, 40.40311f, 0.09188946f },
        { 654.0816f, 596.4531f, 40.40311f, 1.347891f },
        { 654.0816f, 596.4531f, 40.40311f, 5.117566f },
        { 654.0816f, 596.4531f, 40.40311f, 2.604293f },
        { 654.0816f, 596.4531f, 40.40311f, 3.860927f }
    };

    std::vector<Position> const g_LavaSpawnPositions =
    {
        { 735.2083f, 636.382f, 37.21225f },
        { 645.6406f, 704.4114f, 36.1204f },
        { 726.1146f, 678.4288f, 37.21225f },
        { 566.6094f, 618.7188f, 36.08305f },
        { 758.3368f, 577.9445f, 36.06422f },
        { 577.5972f, 666.1649f, 36.08305f },
        { 679.5052f, 518.2674f, 36.08316f },
        { 620.2917f, 536.0417f, 36.08305f },
        { 593.316f, 558.342f, 36.08305f  }
    };

    std::map<std::string, std::string> const g_CorruptorAndRiftLinkedIds =
    {
        { "506811F953879C4B5EA8A5FCE2D18E8FBB92C432", "529C216AD6DED39E40A5B6EC0345B34EB89FB6D7" },
        { "F5D050559D9E240D11CEE8553698B70EB239039B", "0A3CA2D4C1EEC490F1081DF83318185AE822E3C9" },
        { "5DD4551F5D26D7AB8E68AFC620C22AF15549A150", "6EEAF6FF1624A4831324D8F92BD18E974B58BEFA" },
        { "B332B4149ED22E80E8A5844A9BCB180795E11F51", "7F3F8F683721FE2F71E50AA50C51FD7B6F8F4A40" },
        { "444768B356E39A4DF473BBA72C732ACD2E3BBCD3", "8ECEEAAB03964896FC633C462515904E0F7F8F80" },
        { "7909C65224DC30CCD7808A02103051007385931B", "C2EF1A0180B1BC5DFA768BA725334693167D4E9E" },
        { "B0F6042582678584F477FA7D5D36C187B8CE6F03", "D6BDC5F1A62CB511FFE837B191949EB9A1AC63C0" },
        { "028C4A13D4CB477A71A65DB89AF0929BED0FDA57", "BCCF4365E4663F113703A0B849987A2E740DC67C" }
    };

    std::vector<std::string> const g_GiantLinkedIds =
    {
        { "EB29211CF6E3E0D54426125A0EC1E68A0C1A2E5C" },
        { "633D115C9F330AF311EF9FCF70570E6701FA7017" },
        { "5821B639986E1E16266119D6B4087FCBBE281FD1" },
        { "09B7E3ED9B8D1F1E41178BBC376091B8398A1E2D" },
        { "7947C8C76E977120EF922D9033E0BAB7659A8327" }
    };

    Position const g_BossSpawnPos = { 663.7761f, 618.4219f, 40.47853f, 1.392737f };
    Position const g_PortalSpawnPos = { 654.0816f, 596.4531f, 40.40311f, 4.222463f };
    Position const g_FieryRunesSpawnPos = { 662.7379f, 614.3316f, 40.5247f };

    enum eNpcs
    {
        Npc_BurstingElemental = 127214,
        Npc_FelflameSubjugator = 125197,
        Npc_WickedCorruptor = 125777,
        Npc_BurningHound = 125936,
        Npc_FelflameInvader = 125757,
        Npc_ConqueringRiftstalker = 125199,
        Npc_SearingInfernal = 125938,
        Npc_CrazedCorruptor = 126230,
        Npc_ClubfistObliterator = 125970,
        Npc_DiabolicalChainwarden = 125969,
        Npc_FelrageMarauder = 125781,
        Npc_FrothingRazorwing = 125933,
        Npc_BladeswornDisciple = 125788,
        Npc_BlacksoulInquisitor = 125779,
        Npc_FelfrenzyBerserker = 125785,
        Npc_ChitteringFiend = 125921,
        Npc_MagmaGiant = 124835,
        Npc_SmolderingRift = 126293,
        Npc_FieryRunes = 126895,
        Npc_Lava = 126826
    };

    enum eSteps
    {
        Step_RiftBreaker,
        Step_MagmaFlows,
        Step_SecureVictory
    };

    enum eAssets
    {
        Asset_RiftBreaker = 60406,
        Asset_MagmaFlows = 126371,
        Asset_SecureVictory = 59372
    };

    enum eSpells
    {
        // Modifier Spells
        Spell_BurningSteps = 251896,
        Spell_FireEnchanted = 251888,
        Spell_VolatileLava = 251772,
        // Misc Spells
        Spell_RiftCollapse = 252699,
        // Conversation Spells
        Spell_AfterFirstStageConversation = 251005,
        Spell_AfterSecondStageConversation = 251003,
        Spell_ScenarioCompletedConversation = 251015
    };

    enum eEvents
    {
        Event_SummonSecondStageCreaturesFirst = 1,
        Event_SummonSecondStageCreaturesSecond = 2,
        Event_CastVolatileLava = 3
    };

    enum eCreaturesPaths
    {
        Path_FelfrenzyBerserker = 12578500,
        Path_BlacksoulInquisitor = 12577900,
        Path_DiabolicalChainwarden = 12596900
    };

    enum eActions
    {
        Action_ActivateMagmaGiant = 1
    };

    enum eModifiers
    {
        Mod_FireEnchanted = 1,
        Mod_BurningSteps = 2,
        Mod_VolatileLava = 3,
        Mod_SearingPower = 4
    };
}

namespace InvasionPointBonich
{
    std::multimap<uint32, Position> const g_SecondStageCreaturesSpawnPositions =
    {
        { 125921, { -3673.79f, -7953.304f, 1.09892f, 4.004723f } },
        { 125921, { -3800.7f, -7999.832f, 1.988578f, 5.983314f } },
        { 125931, { -3834.42f, -8017.042f, 1.723799f, 3.309091f } },
        { 127982, { -3706.806f, -8093.556f, 0.9848051f, 1.47862f } },
        { 127982, { -3642.42f, -8058.29f, 6.208961f, 0.9580829f } },
        { 125931, { -3631.153f, -7990.677f, 2.144269f, 6.252821f } },
        { 125931, { -3696.861f, -8092.066f, 0.4746959f, 2.954302f } },
        { 125921, { -3648.017f, -8066.028f, 4.449783f, 0.9580829f } },
        { 125931, { -3714.639f, -8091.057f, 0.4992039f, 0.1001349f } },
        { 125931, { -3850.451f, -8018.753f, 2.615426f, 0.04264307f } },
        { 127982, { -3843.542f, -8024.728f, 1.965558f, 1.414993f } },
        { 125931, { -3706.372f, -8085.003f, 0.7746559f, 4.688044f } },
        { 125921, { -3704.97f, -8019.351f, 2.008111f, 3.023272f } },
        { 125931, { -3843.266f, -8013.647f, 1.913124f, 4.861271f } },
        { 127982, { -3703.45f, -8026.823f, 3.062016f, 3.023272f } },
        { 125931, { -3714.903f, -8093.453f, 0.4899736f, 0.3652483f } },
        { 125921, { -3705.715f, -8034.424f, 3.542023f, 3.023272f } },
        { 125921, { -3689.517f, -7948.116f, 1.615951f, 4.004723f } },
        { 125921, { -3807.306f, -8005.717f, 1.497469f, 5.983314f } },
        { 125931, { -3835.247f, -8024.024f, 1.235016f, 2.486789f } },
        { 127982, { -3681.068f, -7944.439f, 1.083478f, 4.004724f } },
        { 125921, { -3647.076f, -8052.741f, 5.8689f, 0.9580829f } },
        { 127982, { -3808.818f, -8002.552f, 1.394304f, 5.983314f } },
        { 125931, { -3700.563f, -8088.887f, 0.5257548f, 3.365568f } },
        { 125931, { -3848.675f, -8011.7f, 2.155144f, 5.451744f } },
        { 125921, { -3773.719f, -7894.467f, 0.7404246f, 0.9580829f } },
        { 127982, { -3856.342f, -7941.243f, 3.360084f, 0.9580829f } },
        { 125921, { -3858.899f, -7953.783f, 2.562581f, 0.9580829f } },
        { 127982, { -3620.738f, -7994.134f, 3.436466f, 0.7285626f } },
        { 125931, { -3617.837f, -7985.099f, 3.30404f, 4.795938f } },
        { 127982, { -3771.686f, -7890.969f, 0.5411215f, 0.9580829f } },
        { 125931, { -3626.078f, -7990.838f, 2.819673f, 6.253658f } },
        { 125931, { -3625.113f, -7985.191f, 2.77691f, 5.63378f } },
        { 125921, { -3848.323f, -7940.771f, 3.48631f, 0.9580829f } },
        { 125931, { -3626.918f, -7998.757f, 3.324789f, 0.6742985f } },
        { 125921, { -3758.311f, -7900.564f, 0.7036973f, 0.9580829f } },
        { 127982, { -3770.634f, -8024.535f, 1.281083f, 1.031366f } },
        { 125931, { -3766.78f, -8016.99f, 2.152405f, 4.355099f } },
        { 125921, { -3753.922f, -7927.519f, 1.15309f, 4.772182f } },
        { 125931, { -3779.266f, -8016.564f, 0.9696639f, 5.995753f } },
        { 125921, { -3745.016f, -7924.243f, 1.158563f, 4.772182f } },
        { 125931, { -3763.753f, -8024.193f, 1.33513f, 2.339751f } },
        { 125931, { -3770.656f, -8014.771f, 2.241783f, 5.206379f } },
        { 127982, { -3753.165f, -7920.413f, 0.9734855f, 4.772182f } },
        { 125931, { -3765.349f, -8030.957f, 0.9966176f, 1.796227f } },
        { 125921, { -3791.399f, -8090.531f, 3.840925f, 0.9580829f } },
        { 125921, { -3802.13f, -8087.066f, 3.678443f, 0.9580829f } },
        { 127982, { -3797.474f, -8093.285f, 4.038947f, 0.9580829f } }
    };

    std::vector<Position> const g_IllidariBannersSpawnPositions =
    {
        { -3713.248f, -8003.493f, 3.149687f, 4.70962f },
        { -3734.464f, -8003.865f, 3.163017f, 4.70962f }
    };

    std::vector<Position> const g_DemonHunterSpawnPositions =
    {
        { -3724.07f, -7999.76f, 3.268338f, 0.00008139014f },
        { -3724.07f, -7999.76f, 3.268338f, 2.51329f },
        { -3724.07f, -7999.76f, 3.268338f, 1.257196f },
        { -3724.07f, -7999.76f, 3.268338f, 3.770108f },
        { -3724.07f, -7999.76f, 3.268338f, 5.025928f }
    };

    std::vector<Position> const g_SwarmingInsectsSpawnPositions =
    {
        { -3800.805f, -8152.692f, 9.200006f },
        { -3759.071f, -8170.286f, 9.946498f },
        { -3812.748f, -8100.209f, 5.406636f },
        { -3718.117f, -8204.398f, 11.28199f },
        { -3862.725f, -8110.444f, 5.496825f },
        { -3758.578f, -8056.557f, 0.9038987f },
        { -3675.257f, -8192.642f, 12.16885f },
        { -3763.152f, -8042.713f, 0.8985779f },
        { -3684.031f, -8098.844f, 0.4282732f },
        { -3747.267f, -8026.469f, 1.118128f },
        { -3827.054f, -8002.855f, 1.059012f },
        { -3684.027f, -8028.915f, 2.954937f },
        { -3927.479f, -8048.549f, 0.6480591f },
        { -3765.703f, -7986.908f, 0.8687893f },
        { -3604.06f, -8113.739f, 7.756563f },
        { -3602.856f, -8104.322f, 7.671309f },
        { -3665.938f, -8007.789f, 1.937773f },
        { -3627.855f, -8024.037f, 5.226893f },
        { -3659.818f, -7984.726f, 1.779944f },
        { -3789.626f, -7893.268f, 0.8314284f },
        { -3939.698f, -7982.305f, -3.453226f },
        { -3707.535f, -7890.656f, 1.341323f },
        { -3688.417f, -7915.094f, 4.874319f },
        { -3863.146f, -7886.328f, 1.851239f },
        { -3803.553f, -7877.08f, 0.2501124f }
    };

    Position const g_BossSpawnPos = { -3722.05f, -7994.531f, 3.177111f, 4.192229f };
    Position const g_PortalSpawnPos = { -3724.07f, -7999.76f, 3.268338f, 1.618171f };

    enum eNpcs
    {
        Npc_InvasiveGazer = 127982,
        Npc_SwarmingInsects = 126813
    };

    enum eSteps
    {
        Step_ClearingTheClearing,
        Step_PushBackTheLegion,
        Step_TheLegionsCommander
    };

    enum eAssets
    {
        Asset_ClearingTheClearing = 59201,
        Asset_PushBackTheLegion = 127982,
        Asset_TheLegionsCommander = 59372
    };

    enum eSpells
    {
        // Modifier Spells
        Spell_SacredVines = 252083,
        Spell_InvigoratingMushroom = 252048,
        // Conversation Spells
        Spell_AfterFirstStageConversation = 250976,
        Spell_AfterSecondStageConversation = 251006,
        Spell_ScenarioCompletedConversation = 251014
    };

    enum eModifiers
    {
        Mod_SwarmingInsects = 1,
        Mod_InvigoratingPollen = 2,
        Mod_SacredVines = 3
    };
}

namespace InvasionPointSangua
{
    std::multimap<uint32, Position> const g_SecondStageCreaturesSpawnPositions =
    {
        { 125790, { -1358.535f, 754.9445f, 62.76679f, 2.481683f } },
        { 125970, { -1391.134f, 761.9861f, 62.60615f, 2.765286f } },
        { 125755, { -1365.264f, 744.1996f, 62.70736f, 3.284997f } },
        { 125939, { -1366.443f, 752.6632f, 62.60501f, 2.481683f } },
        { 125781, { -1369.179f, 700.1042f, 66.21336f, 2.497942f } },
        { 125197, { -1353.625f, 704.0018f, 66.62115f, 1.171003f } },
        { 125939, { -1321.165f, 799.3837f, 63.05594f, 3.394447f } },
        { 125755, { -1435.576f, 767.8021f, 62.76375f, 0.531257f } },
        { 125790, { -1332.42f, 779.0972f, 62.50553f, 1.38611f } },
        { 125197, { -1392.792f, 792.8715f, 61.99653f, 1.511606f } },
        { 125781, { -1397.399f, 785.7535f, 61.68925f, 3.597098f } },
        { 125790, { -1335.795f, 769.8542f, 62.65799f, 4.029824f } },
        { 125790, { -1446.514f, 768.8472f, 62.85226f, 2.808058f } },
        { 125939, { -1441.339f, 757.934f, 62.48225f, 4.575993f } },
        { 125757, { -1329.757f, 739.0243f, 60.60129f, 1.700307f } },
        { 125790, { -1339.274f, 738.7361f, 60.98785f, 1.700307f } },
        { 125939, { -1508.035f, 720.8108f, 61.09257f, 0.4646854f } },
        { 125779, { -1389.325f, 729.3125f, 60.99444f, 4.847795f } },
        { 125781, { -1394.991f, 736.0486f, 61.09133f, 2.769453f } },
        { 125781, { -1387.116f, 739.3264f, 61.08328f, 1.245084f } },
        { 125939, { -1333.868f, 736.0243f, 60.82804f, 1.700307f } },
        { 125757, { -1503.84f, 717.9184f, 61.03465f, 0.4646854f } },
        { 125970, { -1363.524f, 712.2101f, 64.18613f, 1.81572f } },
        { 125790, { -1507.247f, 726.816f, 61.24366f, 0.4646854f } },
        { 125197, { -1345.484f, 813.1024f, 61.16493f, 3.312896f } },
        { 125755, { -1377.771f, 806.2031f, 61.97743f, 1.295139f } },
        { 125790, { -1384.41f, 801.9011f, 62.02257f, 2.845596f } },
        { 125781, { -1341.092f, 809.9011f, 61.09292f, 5.270924f } }
    };

    std::vector<Position> const g_IllidariBannersSpawnPositions =
    {
        { -1401.347f, 756.7188f, 62.50092f, 2.286167f },
        { -1385.615f, 770.9549f, 62.50092f, 2.286167f }
    };

    std::vector<Position> const g_DemonHunterSpawnPositions =
    {
        { -1390.74f, 761.0278f, 62.58426f, 0.08982736f },
        { -1390.74f, 761.0278f, 62.58426f, 3.859308f },
        { -1390.74f, 761.0278f, 62.58426f, 5.115378f },
        { -1390.74f, 761.0278f, 62.58426f, 1.346444f },
        { -1390.74f, 761.0278f, 62.58426f, 2.60391f }
    };

    std::vector<Position> const g_WyrmtongueBloodhaulerSpawnPositions =
    {
        { -1357.443f, 700.2327f, 67.29667f, 1.12845f },
        { -1360.615f, 699.4566f, 67.22638f, 1.67396f },
        { -1363.674f, 698.8733f, 66.98914f, 2.57108f }
    };

    std::vector<Position> const g_CongealedPlasmaSpawnPositions =
    {
        { -1348.473f, 823.0748f, 61.08904f, 2.281415f },
        { -1416.435f, 784.7388f, 61.25552f, 0.8766478f },
        { -1316.897f, 777.0588f, 61.36307f, 0.6523758f },
        { -1462.077f, 754.6079f, 61.13459f, 2.148868f },
        { -1351.182f, 765.7812f, 61.10913f, 5.626202f },
        { -1485.663f, 779.3561f, 61.30341f, 4.780539f },
        { -1325.961f, 744.7141f, 60.75779f, 4.777221f },
        { -1399.562f, 725.5892f, 60.62375f, 5.400249f },
        { -1463.687f, 720.3539f, 61.28921f, 4.029409f },
        { -1484.75f, 746.4374f, 61.12532f, 1.480186f  },
        { -1395.526f, 802.6302f, 61.0921f, 1.060665f  },
        { -1361.095f, 792.9653f, 61.09292f, 5.19996f  },
        { -1438.054f, 741.4566f, 61.09294f, 2.660208f }
    };

    Position const g_BossSpawnPos = { -1361.656f, 704.0347f, 65.68648f, 1.881438f };
    Position const g_PortalSpawnPos = { -1390.74f, 761.0278f, 62.58426f, 5.477909f };

    enum eNpcs
    {
        Npc_LegionBloodDrainer = 125874,
        Npc_WyrmtongueBloodhauler = 125963,
        Npc_NoxiousShadowstalker = 125939,
        Npc_ClubfistObliterator = 125970,
        Npc_FelflameInvader = 125755,
        Npc_FelflameSubjugator = 125197,
        Npc_LegionPortal = 126083,
        Npc_CongealedPlasma = 126025
    };

    enum eSteps
    {
        Step_StemTheBleeding,
        Step_StopTheLegion,
        Step_RestoreBalance
    };

    enum eAssets
    {
        Asset_StemTheBleeding = 59752,
        Asset_StopTheLegion = 59201,
        Asset_RestoreBalance = 59372
    };

    enum eSpells
    {
        // Modifier Spells
        Spell_CoalescedPowerEndless = 250740,
        Spell_CoalescedPowerMissile = 250756,
        // Misc Spells
        Spell_FelDissolveIn = 211762,
        // Conversation Spells
        Spell_StartScenarioConversation = 250974,
        Spell_AfterFirstStageConversation = 251004,
        Spell_AfterSecondStageConversation = 251005
    };

    enum eEvents
    {
        Event_SummonWyrmtongueBloodhaulers = 1
    };

    enum eWyrmtongueBloodhaulerPaths
    {
        Path_First = 12596300,
        Path_Second = 12596301,
        Path_Third = 12596302
    };

    enum eModifiers
    {
        Mod_CoalescedPowerOnCreate = 1,
        Mod_CoalescedPowerOnDeath = 2
    };

    enum eSubModifiers
    {
        SubMod_CongealedPlasma = 1
    };
}

namespace InvasionPointNaigtal
{
    std::multimap<uint32, Position> const g_SecondStageCreaturesSpawnPositionsFirst =
    {
        { 125779, { -1805.875f, -1463.842f, 8.092013f, 3.493137f } },
        { 125965, { -1816.559f, -1461.606f, 6.756945f, 3.670651f } },
        { 125936, { -1803.826f, -1455.88f, 8.427083f, 3.638829f } },
        { 125958, { -1815.451f, -1454.004f, 7.224291f, 3.829341f } },
        { 125865, { -1811.323f, -1458.738f, 7.656489f, 3.666339f } }
    };

    std::multimap<uint32, Position> const g_SecondStageCreaturesSpawnPositionsSecond =
    {
        { 125969, { -1841.439f, -1390.094f, 10.19965f, 1.826847f } },
        { 125936, { -1829.814f, -1391.97f, 10.39236f, 1.892905f } },
        { 125958, { -1837.311f, -1394.828f, 9.716729f, 1.843921f } },
        { 125931, { -1831.863f, -1399.932f, 10.34201f, 1.866655f } }
    };

    std::multimap<uint32, Position> const g_SecondStageCreaturesSpawnPositionsThird =
    {
        { 125958, { -1892.064f, -1492.879f, 7.427083f, 3.666339f } },
        { 125936, { -1884.568f, -1490.021f, 7.120403f, 3.638829f } },
        { 125931, { -1886.616f, -1497.983f, 7.324653f, 3.493137f } },
        { 125969, { -1896.193f, -1488.146f, 8.090278f, 3.829341f } },
        { 126231, { -1897.3f, -1495.747f, 7.384141f, 3.670651f } }
    };

    std::multimap<uint32, Position> const g_SecondStageCreaturesSpawnPositionsFourth =
    {
        { 125958, { -1757.616f, -1560.802f, 6.21724f, 2.813807f } },
        { 125931, { -1759.665f, -1568.764f, 5.833333f, 2.567086f } },
        { 125967, { -1765.113f, -1563.66f, 5.489172f, 2.617552f } },
        { 125965, { -1769.241f, -1558.925f, 5.611615f, 2.717339f } },
        { 126231, { -1770.349f, -1566.528f, 4.652778f, 2.400608f } }
    };

    std::multimap<uint32, Position> const g_SecondStageCreaturesSpawnPositionsFifth =
    {
        { 125958, { -1821.285f, -1666.95f, 2.449559f, 1.892905f } },
        { 125931, { -1823.335f, -1674.911f, 3.072398f, 1.866655f } },
        { 125967, { -1828.781f, -1669.807f, 3.803922f, 1.843921f } },
        { 125965, { -1832.91f, -1665.073f, 3.892586f, 1.826847f } },
        { 126231, { -1834.017f, -1672.675f, 3.818501f, 1.808701f } }
    };

    std::vector<Position> const g_IllidariBannersSpawnPositions =
    {
        { -1823.043f, -1599.976f, 0.8188952f, 1.540312f },
        { -1844.26f, -1599.764f, 0.8188952f, 1.540312f }
    };

    std::vector<Position> const g_DemonHunterSpawnPositions =
    {
        { -1833.563f, -1603.79f, 0.9022285f, 4.370516f },
        { -1833.563f, -1603.79f, 0.9022285f, 1.857869f },
        { -1833.563f, -1603.79f, 0.9022285f, 3.113879f },
        { -1833.563f, -1603.79f, 0.9022285f, 0.600616f },
        { -1833.563f, -1603.79f, 0.9022285f, 5.628192f }
    };

    std::vector<std::string> const g_MarshShamblerLinkedIds =
    {
        { "5FE6648A5B73F334ED3F0DDCFA04ED8C9F6FD563" },
        { "742F850C870F132B7FD0DE52E14E6B612D61BA25" }
    };

    std::multimap<uint32, Position> const g_MaddeningSporeSpawnPositions =
    {
        { 127035001, { -1824.063f, -1449.749f, 7.58167f } },
        { 127035002, { -1797.376f, -1440.893f, 20.15012f } },
        { 127035003, { -1814.582f, -1561.663f, 2.128365f } },
        { 127035004, { -1805.443f, -1569.226f, 2.209373f } },
        { 127035005, { -1878.377f, -1498.711f, 7.016893f } },
        { 127035006, { -1783.007f, -1419.807f, 26.68835f } },
        { 127035007, { -1857.015f, -1581.407f, 0.4895823f } },
        { 127035008, { -1828.44f, -1604.397f, 1.682035f } }
    };

    Position const g_BossSpawnPos = { -1834.573f, -1604.095f, 0.8263019f, 1.677646f };
    Position const g_PortalSpawnPos = { -1833.563f, -1603.79f, 0.9022285f, 4.732049f };

    enum eNpcs
    {
        Npc_ChitteringFiend = 125921,
        Npc_CrazedCorruptor = 126231,
        Npc_WickedCorruptor = 125776,
        Npc_MarshShambler = 127098,
        Npc_SinuousDevourer = 126565,
        Npc_EnergizedVine = 126556,
        Npc_MaddeningSpores = 127035
    };

    enum eSteps
    {
        Step_ScatteredTotheWinds,
        Step_RepelTheLegion,
        Step_SomethingWicked
    };

    enum eAssets
    {
        Asset_ScatteredTotheWinds = 59201,
        Asset_RepelTheLegion = 125958,
        Asset_SomethingWicked = 59372
    };

    enum eSpells
    {
        // Modifier Spells
        Spell_EmbeddedSpores = 251123,
        Spell_SporeFilled = 252113,
        Spell_SporePool = 252289,
        Spell_SporeLaden = 252135,
        Spell_EnergizedVine = 252176,
        Spell_EnergizedVineAreatrigger = 252183,
        Spell_MaddeningSporesAreatrigger = 252121,
        // Conversation Spells
        Spell_StartScenarioConversation = 250988,
        Spell_AfterFirstStageConversation = 251005,
        Spell_AfterSecondStageConversation = 251004,
        Spell_ScenarioCompletedConversation = 251015
    };

    enum eEvents
    {
        Event_CastEmbeddedSporesOnPlayers = 1
    };

    enum eActions
    {
        Action_FirstMarshShamblerFreed = 1,
        Action_SecondMarshShamblerFreed = 2
    };

    enum ePaths
    {
        Path_BossCircle = 14119102
    };

    enum eModifiers
    {
        Mod_ClickableVines = 1,
        Mod_UnclickableVines = 2,
        Mod_UnpresentedVines = 3
    };

    enum eSubModifiers
    {
        SubMod_MaddeningSpores = 1,
        SubMod_SporeFilled = 2,
        SubMod_DarkSpores = 3
    };
}

namespace GreaterInvasionPointMatronFolnuna
{
    std::map<std::string, std::string> const g_CorruptorAndRiftLinkedIds =
    {
        { "697A1EE1DA5D07A86C4F4C37DE6F05B129341654", "0229FA1AE06B75A51453FB99370D066D4DB40D3C" },
        { "1FDC34E932489D4970D629FAB057D269CEE05F1E", "07AA1D717C7D5558F4F83BF1BB3F04CEC7BE0D80" },
        { "9429DB147FDDFA5BD9D3BD8E0CEF66C94157EF11", "0FB08795DAD08480CE4EAF1974A64C3AE5A330F1" },
        { "D15A358DDB4B0E94E48A143FBA717799A8246D30", "4FAF877199F0820DAC67CD2D1D3206DC6EC0EF7D" },
        { "3E5D9682DCB0BDD3672123D351103AE71600339F", "1E870341CE5E9984AA16DCE06FA9410A333476F1" },
        { "FDD4789C80FEC0ED135B7C7E613E81BECC70DBF3", "F024AA860D8BB95C015D397FEFA45664EFE63A27" },
        { "37529B0A139E72F99843D20F042CDD903D1D08B2", "B33ED63F3CED91CB09B648A7D05AD192059528DA" },
        { "E355BE8569EFFC5A35B595953038D2C73603318F", "69BD2732288F1A245AD0743D38A87CC269A1F5CB" }
    };

    enum eNpcs
    {
        Npc_CrazedCorruptor = 126230,
        Npc_SmolderingRift = 126293
    };

    enum eSpells
    {
        Spell_RiftCollapse = 252699
    };
}

namespace GreaterInvasionPointMistressAlluradel
{
    enum eSpells
    {
        Spell_InvigoratingMushroom = 252048
    };
}


Player* GetAnyPlayer(Unit* p_Unit);
void IncreaseScenarioProgress(uint8 p_Count, Scenario* p_Scenario, Player* p_Player, uint32 p_Asset);
bool IsInvasionPointPOI(uint16 p_AreaPoiID);
