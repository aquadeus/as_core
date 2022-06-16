////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////


#define FACTION_HOSTILE 16
#define FACTION_FRIENDLY 35

#ifndef COURT_OF_STARS_HPP
# define COURT_OF_STARS_HPP

# include "BrokenIslesPCH.h"

enum eData
{
    DATA_CAPTAIN_GERDO      = 0,
    DATA_TALIXAE            = 1,
    DATA_MELANDRUS_EVENT    = 2,
    DATA_MELANDRUS          = 3,
    MAX_ENCOUNTER,

    DATA_BEACON_ACTIVATE,
    DATA_GET_BEACON_QUANTITY,
    DATA_AREA_SECURED,
    DATA_NOBLE_COUNT,
    DATA_NOBLE_COMPLETE
};

enum eCreatures
{
    NPC_SIGNAL_LANTERN                 = 105729,

    /// Patrol Captain Gerdo
    NPC_PATROL_CAPTAIN_GERDO           = 104215,
    NPC_FLASK_OF_SOLEMN_NIGHT          = 105117,
    NPC_GERDO_TRIG_FRONT               = 104384,
    NPC_GERDO_TRIG_BACK                = 104400,
    NPC_GERDO_ARCANE_BEACON            = 104245,
    NPC_SURAMAR_CITIZEN                = 104694,
    NPC_VIGILANT_WATCH                 = 104918,

    /// Talixae adds
    NPC_TALIXAE                        = 104217,
    NPC_JAZSHARIU                      = 104273,
    NPC_BAALGAR_THE_WATCHFUL           = 104274,
    NPC_IMACUTYA                       = 104275,
    NPC_FELBOUND_ENFORCER              = 104278,
    NPC_INFERNAL_IMP                   = 112668,
    NPC_LYLETH_LUNASTRE                = 106468,

    /// Melandrus adds
    NPC_IMAGE_MELANDRUS                = 105754,
    NPC_ENVELOPING_WINDS               = 112687,
    NPC_ELLISANDE                      = 110443,
    NPC_ADVISOR_MELANDRUS              = 104218,

    /// Misc
    NPC_GERENTH_THE_VILE               = 108151,
    NPC_SUSPICIOUS_NOBLE               = 107435,
    NPC_CHATTY_RUMORMONGER             = 107486,
    NPC_GUARDIAN_CONSTRUCT             = 104270,

    NPC_CHECKPOINT_BEFORE_2ND_BOSS     = 0
};

enum eGameObjects
{
    GO_GERDO_DOOR               = 251849,
    GO_MELANDRUS_DOOR_1         = 251847,
    GO_MELANDRUS_DOOR_2         = 251848,
    GO_MALRODIS_DOOR            = 253922,
    GO_CAPTAIN_DOOR			    = 251846
};

enum eCoSSpells
{
    SPELL_DISABLED                  = 210257,
    SPELL_CALL_VIGILANT_NIGHTWATCH  = 210435,
    SPELL_SECURED_BUFF              = 210600
};

enum eSpiesData
{
    /// Kind of hints
    DATA_GLOVES = 1,
    DATA_NO_GLOVES,
    DATA_CAPE,
    DATA_NO_CAPE,
    DATA_LIGHT_VEST,
    DATA_DARK_VEST,
    DATA_FEMALE,
    DATA_MALE,
    DATA_SHORT_SLEEVES,
    DATA_LONG_SLEEVES,
    DATA_POTIONS,
    DATA_NO_POTIONS,
    DATA_BOOK,
    DATA_POUCH,

    /// Amount of hints
    DATA_FIRST_HINT = 0,
    DATA_SECOND_HINT,
    DATA_THIRD_HINT,
    DATA_FOURTH_HINT,
    DATA_FIFTH_HINT,
    DATA_SIXTH_HINT
};

enum eEventItems
{
    /// NpcIDs

    /// Functional Items
    NpcArcanePowerConduit        = 105157,
    NpcBazaarGoods               = 106018,
    NpcLifesizedNightborneStatue = 106113,
    NpcDiscardedJunk             = 105215,
    NpcWoundedNightborneCivilian = 106112,

    /// Buff Items
    NpcFelOrb                    = 105160,
    NpcInfernalTome              = 105831,
    NpcMagicalLantern            = 106024,
    NpcNightshadeRefreshments    = 105249,
    NpcStarlightRoseBrew         = 106108,
    NpcUmbralBloom               = 105340,
    NpcWaterloggedScroll         = 106110
};

static std::array<uint32, 5> const g_EventNpcRedMarks =
{
    {
        eEventItems::NpcArcanePowerConduit,
        eEventItems::NpcBazaarGoods,
        eEventItems::NpcLifesizedNightborneStatue,
        eEventItems::NpcDiscardedJunk,
        eEventItems::NpcWoundedNightborneCivilian
    }
};

static std::array<uint32, 7> const g_EventNpcBlueMarks =
{
    {
        eEventItems::NpcFelOrb,
        eEventItems::NpcInfernalTome,
        eEventItems::NpcMagicalLantern,
        eEventItems::NpcNightshadeRefreshments,
        eEventItems::NpcStarlightRoseBrew,
        eEventItems::NpcUmbralBloom,
        eEventItems::NpcWaterloggedScroll
    }
};

static std::random_device g_Device;
static std::mt19937 g_RandomGenerator(g_Device());

static std::array<Position, 5> const g_EventRedMarksPos =
{
    {
        { 1189.13f,   3591.98f,     20.59f,    4.7f     },    ///< NpcArcanePowerConduit
        { 1064.72f,   3429.345f,    20.115f,   5.67f    },    ///< NpcBazaarGoods
        { 1062.1f,    3375.38f,     19.9216f,  2.44528f },    ///< NpcLifesizedNightborneStatue
        { 1133.67f,   3263.16f,     20.0f,     0.3f     },    ///< NpcDiscardedJunk
        { 1051.61f,   3410.48f,     19.9216f,  5.5f     }     ///< NpcWoundedNightborneCivilian
    }
};

static std::array<Position, 7> const g_EventBlueMarksPos =
{
    {
        { 1140.18f,   3430.33f,     19.92f,    4.2382f  },    ///< NpcFelOrb
        { 1030.739f,  3362.439f,    19.921f,   5.0f     },    ///< NpcInfernalTome
        { 1170.6469f, 3314.2365f,   23.68638f, 2.4288f  },    ///< NpcMagicalLantern
        { 1127.65f,   3320.57f,     24.513f,   4.973f   },    ///< NpcNightshadeRefreshments
        { 1191.912f,  3329.193f,    21.0157f,  4.612f   },    ///< NpcStarlightRoseBrew
        { 1076.1754f, 3380.0249f,   24.26334f, 3.4686f  },    ///< NpcUmbralBloom
        { 1211.336f,  3349.148f,    20.456f,   0.03f    }     ///< NpcWaterloggedScroll
    }
};

static std::vector<uint32> const g_SpiesDisplayIDs =
{
    69961, ///< male,   long sleeves,  light - colored vest, gloves,    cape,    potion vials on belt
    69962, ///< male,   short sleeves, dark - colored vest,  gloves,    cape,    book on belt
    69963, ///< male,   long sleeves,  light - colored vest, no gloves, cape,    pouch (gold)  on belt
    69964, ///< male,   short sleeves, light - colored vest, no gloves, no cape, potion vials on belt
    69965, ///< male,   long sleeves,  dark - colored vest,  gloves,    no cape, book on belt
    69966, ///< male,   short sleeves, dark - colored vest,  gloves,    no cape, pouch (gold)  on belt
    69967, ///< male,   long sleeves,  dark - colored vest,  no gloves, cape,    potion vials on belt
    69968, ///< male,   short sleeves, light - colored vest, no gloves, no cape, book on belt
    69970, ///< female, long sleeves,  dark - colored vest,  gloves,    cape,    pouch (gold)  on belt
    69971, ///< female, short sleeves, light - colored vest, gloves,    no cape, potion vials on belt
    69972, ///< female, long sleeves,  dark - colored vest,  no gloves, cape,    book on belt
    69973, ///< female, short sleeves, light - colored vest, no gloves, cape,    pouch (gold)  on belt
    69974, ///< female, long sleeves,  dark - colored vest,  gloves,    cape,    potion vials on belt
    69975, ///< female, short sleeves, dark - colored vest,  gloves,    no cape, book on belt
    69976, ///< female, long sleeves,  dark - colored vest,  no gloves, cape,    pouch (gold) on belt
    69977  ///< female, short sleeves, dark - colored vest,  no gloves, no cape, potion vials on belt
};

static std::map<uint32, std::vector<uint32>> const g_Hints = ///< All hints combination ordered by displayID...
{
    {
        69961,
        {
            DATA_MALE,
            DATA_LONG_SLEEVES,
            DATA_LIGHT_VEST,
            DATA_GLOVES,
            DATA_CAPE,
            DATA_POTIONS
        }
    },
    {
        69962,
        {
            DATA_MALE,
            DATA_SHORT_SLEEVES,
            DATA_DARK_VEST,
            DATA_GLOVES,
            DATA_CAPE,
            DATA_BOOK
        }
    },
    {
        69963,
        {
            DATA_MALE,
            DATA_LONG_SLEEVES ,
            DATA_LIGHT_VEST,
            DATA_NO_GLOVES,
            DATA_CAPE,
            DATA_POUCH
        }
    },
    {
        69964,
        {
            DATA_MALE,
            DATA_SHORT_SLEEVES,
            DATA_LIGHT_VEST,
            DATA_NO_GLOVES,
            DATA_NO_CAPE,
            DATA_POTIONS
        }
    },
    {
        69965,
        {
            DATA_MALE,
            DATA_LONG_SLEEVES,
            DATA_DARK_VEST,
            DATA_GLOVES,
            DATA_NO_CAPE,
            DATA_BOOK
        }
    },
    {
        69966,
        {
            DATA_MALE,
            DATA_SHORT_SLEEVES,
            DATA_DARK_VEST,
            DATA_GLOVES,
            DATA_NO_CAPE,
            DATA_POUCH
        }
    },
    {
        69967,
        {
            DATA_MALE,
            DATA_LONG_SLEEVES,
            DATA_DARK_VEST,
            DATA_NO_GLOVES,
            DATA_CAPE,
            DATA_POTIONS
        }
    },
    {
        69968,
        {
            DATA_MALE,
            DATA_SHORT_SLEEVES,
            DATA_LIGHT_VEST,
            DATA_NO_GLOVES,
            DATA_NO_CAPE,
            DATA_BOOK,
        }
    },
    {
        69970,
        {
            DATA_FEMALE,
            DATA_LONG_SLEEVES,
            DATA_DARK_VEST,
            DATA_GLOVES,
            DATA_CAPE,
            DATA_POUCH
        }
    },
    {
        69971,
        {
            DATA_FEMALE,
            DATA_SHORT_SLEEVES,
            DATA_LIGHT_VEST,
            DATA_GLOVES,
            DATA_NO_CAPE,
            DATA_POTIONS
        }
    },
    {
        69972,
        {
            DATA_FEMALE,
            DATA_DARK_VEST,
            DATA_LIGHT_VEST,
            DATA_NO_GLOVES,
            DATA_CAPE,
            DATA_BOOK
        }
    },
    {
        69973,
        {
            DATA_FEMALE,
            DATA_SHORT_SLEEVES,
            DATA_LIGHT_VEST,
            DATA_NO_GLOVES,
            DATA_CAPE,
            DATA_POUCH
        }
    },
    {
        69974,
        {
            DATA_FEMALE,
            DATA_LONG_SLEEVES,
            DATA_DARK_VEST,
            DATA_GLOVES,
            DATA_CAPE,
            DATA_POTIONS
        }
    },
    {
        69975,
        {
            DATA_FEMALE,
            DATA_SHORT_SLEEVES,
            DATA_DARK_VEST,
            DATA_GLOVES,
            DATA_NO_CAPE,
            DATA_BOOK
        }
    },
    {
        69976,
        {
            DATA_FEMALE,
            DATA_LONG_SLEEVES,
            DATA_DARK_VEST,
            DATA_NO_GLOVES,
            DATA_CAPE,
            DATA_POUCH
        }
    },
    {
        69977,
        {
            DATA_FEMALE,
            DATA_SHORT_SLEEVES,
            DATA_DARK_VEST,
            DATA_NO_GLOVES,
            DATA_NO_CAPE,
            DATA_POTIONS
        }
    }
};

static std::map<uint32, std::vector<uint32>> const g_Texts = ///< All possible NpcTextIDs for each hint
{
    { DATA_GLOVES,        { { 300200, 300201, 300202, 300203 }}},
    { DATA_NO_GLOVES,     { { 300204, 300205, 300206, 300207 }}},
    { DATA_CAPE,          { { 300208, 300209 }                }},
    { DATA_NO_CAPE,       { { 300210, 300211 }                }},
    { DATA_LIGHT_VEST,    { { 300212, 300213, 300214 }        }},
    { DATA_DARK_VEST,     { { 300215, 300216, 300217, 300218 }}},
    { DATA_FEMALE,        { { 300219, 300220, 300221, 300222 }}},
    { DATA_MALE,          { { 300220, 300224, 300225, 300226 }}},
    { DATA_SHORT_SLEEVES, { { 300220, 300228, 300229, 300230 }}},
    { DATA_LONG_SLEEVES,  { { 300231, 300232, 300233, 300234 }}},
    { DATA_POTIONS,       { { 300235, 300236, 300237, 300238 }}},
    { DATA_NO_POTIONS,    { { 300239, 300240 }                }},
    { DATA_BOOK,          { { 300241, 300242 }                }},
    { DATA_POUCH,         { { 300243, 300244, 300245, 300246 }}}
};

static void DespawnCreaturesInArea(uint32 p_Entry, WorldObject* p_WorldObject)
{
    std::list<Creature*> l_Creatures;
    GetCreatureListWithEntryInGrid(l_Creatures, p_WorldObject, p_Entry, p_WorldObject->GetMap()->GetVisibilityRange());

    for (Creature* l_Iter : l_Creatures)
        l_Iter->DespawnOrUnsummon();
}

/// L / R
const std::vector<G3D::Vector3> g_StarledPackRoom[6] =
{
    {
        { 1288.5873f, 3430.7007f, 28.8260f }, ///< Src
        { 1284.7529f, 3405.1567f, 28.8245f },
        { 1285.3641f, 3393.7632f, 31.3061f },
        { 1300.7875f, 3387.6257f, 37.0468f },
        { 1310.8071f, 3400.8701f, 43.4606f },
        { 1306.9493f, 3412.0535f, 47.4884f },
        { 1293.4698f, 3415.2705f, 51.3543f },
        { 1284.2148f, 3406.1084f, 53.4548f },
        { 1272.2993f, 3411.3035f, 53.8772f },
        { 1266.1107f, 3414.4668f, 53.8220f }  ///< Dest
    },

    {
        { 1257.9401f, 3418.0049f, 53.8635f },
        { 1238.4651f, 3426.7371f, 55.7004f },
        { 1235.4761f, 3433.9683f, 55.4358f },
        { 1212.3867f, 3442.3442f, 41.5815f },
        { 1212.3867f, 3442.3442f, 41.5815f },
        { 1230.9402f, 3426.4373f, 31.7785f },
        { 1232.2125f, 3437.0630f, 27.2271f },
        { 1217.8185f, 3442.0137f, 23.3914f },
        { 1208.6965f, 3436.9565f, 23.4430f },
        { 1195.9607f, 3416.2107f, 21.7941f },
        { 1213.9169f, 3398.7661f, 20.0006f },
        { 1187.9808f, 3376.8911f, 20.0200f },
        { 1157.1395f, 3394.7322f, 29.0918f },
        { 1119.5291f, 3416.2283f, 19.8384f },
        { 1217.8130f, 3399.8923f, 20.0011f }  ///< Dest
    },

    {
        { 1144.8250f, 3319.5383f, 22.6123f },  ///< Src R
        { 1215.5967f, 3401.4453f, 20.0007f },
        { 1207.845f, 3406.468f,   21.1508f },
        { 1195.1443f, 3412.6296f, 21.7948f }  ///< Dest
    },

    {
        { 1086.3517f, 3416.0222f, 19.8392f }, ///< Src L
        { 1109.3854f, 3423.6846f, 19.8392f },
        { 1150.2600f, 3398.8152f, 27.2708f },
        { 1196.3998f, 3379.8508f, 20.0009f },
        { 1212.983f,  3404.3322f, 20.000f  },
        { 1195.1443f, 3412.6296f, 21.7948f }  ///< Dest
    },

    {
        { 1107.907f, 3215.498f, 41.969f },
        { 1117.440f, 3230.218f, 39.305f },
        { 1114.290f, 3247.609f, 31.231f },
        { 1107.187f, 3276.354f, 24.855f },
        { 1146.182f, 3319.094f, 22.490f },
        { 1219.898f, 3400.143f, 20.000f },
        { 1210.637f, 3405.152f, 20.0f   },
        { 1193.733f, 3412.920f, 21.793f }
    },

    {
        { 1252.045f, 3328.495f, 20.120f },
        { 1224.422f, 3232.626f, 20.120f },
        { 1195.832f, 3349.073f, 20.121f },
        { 121.182f,  3402.772f, 20.000f },
        { 1195.067f, 3411.250f, 21.793f }
    },
};

const Position g_UniqueNpc = { 1220.1685f, 3433.6621f, 62.4962f, 0.0f };

enum eAchievements
{
    ACHIEVEMENT_WAITING_FOR_GERDO = 10610,
    ACHIEVEMENT_DROPPING_SOME_EAVES = 10611
};

#endif ///< COURT_OF_STARS_HPP
