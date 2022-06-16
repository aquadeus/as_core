////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef THE_ARCWAY_HPP
# define THE_ARCWAY_HPP

# include "BrokenIslesPCH.h"

enum eData
{
    DATA_IVANYR     = 0,
    DATA_CORSTILAX  = 1,
    DATA_XAKAL      = 2,
    DATA_NALTIRA    = 3,
    DATA_VANDROS    = 4,
    MAX_ENCOUNTER,

    DATA_RAND_TELEPORT,
    DATA_ACH_NO_TIME_TO_WASTE,
    DATA_ACH_ARCANIC_CLING
};

enum eCreatures
{
    /// Ivanyr
    NPC_NIGHT_CRYSTAL           = 98734,

    /// Corstilax
    NPC_SUPPRESSION_PROTOCOL    = 99628,
    NPC_QUARANTINE              = 99560,
    NPC_PIPE_STALKER            = 99446,
    NPC_DESTABLIZED_ORB         = 110863,

    /// Xakal
    NPC_FISSURE                 = 100342,
    NPC_DREAD_FELBAT            = 100393,

    /// Naltira
    NPC_VICIOUS_MANAFANG        = 110966,
    NPC_VICIOUS_MANAFANG_2      = 98759,
    NPC_ENCHANTED_BROODLING     = 105876,
    NPC_NALTIRA_TRIGGER         = 68553,

    /// Vandros
    NPC_UNSTABLE_OOZE           = 98426,
    NPC_UNSTABLE_OOZELING       = 98435,
    NPC_UNSTABLE_AMALGATION     = 98425,
    NPC_TIMELESS_WRATH          = 103130,

    /// Boss IDs
    NPC_CORSTILAX               = 98205,
    NPC_IVANYR                  = 98203,
    NPC_GENERAL_XAKAL           = 98206,
    NPC_NALTIRA                 = 98207,
    NPC_ADVISOR_VANDROS         = 98208,  ///< 102849

    /// Misc
    NPC_FORGOTTEN_SPIRIT        = 113699,
    NPC_PLAGUED_RATS            = 98732,
    NPC_WITHERED_FIEND          = 98733,
    NPC_ACIDIC_BILE             = 98728,
    NPC_WITHERED_MANAWRAITH     = 105952,
};

enum eGameObjects
{
    GO_CORSTILAX_DOOR_1         = 245968,
    GO_CORSTILAX_DOOR_2         = 246024,
    GO_XAKAL_FISSURE            = 245606,

    GO_DOOR_A                   = 250272,
    GO_DOOR_B                   = 250278,
    GO_DOOR_D                   = 250273,
    GO_DOOR_G                   = 250275,
    GO_DOOR_K                   = 250276,
    GO_DOOR_8                   = 250634,
    GO_DOOR_9                   = 250635,
    GO_DOOR_3                   = 250636,
    GO_DOOR_4                   = 250637
};

enum eDisplayIDs
{
    INVISIBLE_MODEL             = 11686,
    TELEPORT_MODEL              = 71960
};

enum eLocations
{
    XAKAL_CHECKPOINT            = 5875,
    NALTIRA_CHECKPOINT          = 5876,
    CORSTILAX_CHECKPOINT        = 5877,
    IVANYR_CHECKPOINT           = 5878
};

namespace TheArcwayMoves
{
    enum eMoves
    {
        FelBatPath              = 1
    };
}

#endif ///< THE_ARCWAY_HPP
