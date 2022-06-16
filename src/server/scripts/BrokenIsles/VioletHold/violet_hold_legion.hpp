////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef VIOLET_HOLD_HPP
# define VIOLET_HOLD_HPP

# include "BrokenIslesPCH.h"

static void CastSpellToPlayers(Unit* p_Caster, uint32 p_SpellID, bool p_Triggered)
{
    if (p_Caster == nullptr || !p_Caster->GetMap()->IsMythic())
        return;

    Map::PlayerList const& l_Players = p_Caster->GetMap()->GetPlayers();
    for (Map::PlayerList::const_iterator l_Iter = l_Players.begin(); l_Iter != l_Players.end(); ++l_Iter)
    {
        if (Player* l_Player = l_Iter->getSource())
        {
            if (p_Caster != nullptr)
                p_Caster->CastSpell(l_Player, p_SpellID, p_Triggered);
            else
                l_Player->CastSpell(l_Player, p_SpellID, p_Triggered);
        }
    }
}

enum eData
{
    DATA_KAAHRJ             = 0,
    DATA_MILLIFICENT        = 1,
    DATA_FESTERFACE         = 2,
    DATA_SHIVERMAW          = 3,
    DATA_ANUBESSET          = 4,
    DATA_THALENA            = 5,
    DATA_SAELORN            = 6,
    DATA_BETRUG             = 7,
    MAX_ENCOUNTER,

    DATA_MAIN_EVENT_PHASE,
    DATA_1ST_BOSS_EVENT,
    DATA_2ND_BOSS_EVENT,
    DATA_3RD_BOSS_EVENT,
    DATA_ADD_TRASH_MOB,
    DATA_DEL_TRASH_MOB,
    DATA_START_BOSS_ENCOUNTER,
    DATA_FIRST_BOSS,
    DATA_SECOND_BOSS,
    DATA_THIRD_BOSS,
    DATA_PORTAL_LOCATION,
    DATA_TELEPORTATION_PORTAL,
    DATA_SINCLARI,
    DATA_WAVE_COUNT,
    DATA_MAIN_DOOR,
    DATA_DOOR_INTEGRITY,
    DATA_REMOVE_NPC,
    DATA_KAAHRJ_CELL,
    DATA_MILLIFICENT_CELL,
    DATA_FESTERFACE_CELL,
    DATA_SHIVERMAW_CELL,
    DATA_ANUBESSET_CELL,
    DATA_THALENA_CELL,

    DATA_BLACK_BILE_KILLED,

    DATA_MAX_PORTAL = 8
};

enum eCreatures
{
    NPC_LIEUTENANT_SINCLARI             = 102278,
    NPC_VIOLET_HOLD_GUARD               = 102266,
    NPC_TELEPORTATION_PORTAL            = 102267,
    NPC_LORD_MALGATH                    = 102282,
    NPC_DEFENSE_SYSTEM                  = 30837,
    NPC_FACELESS_TENDRIL                = 101994,
    NPC_BLISTERING_BEETLE               = 102540,

    //Portal summons
    NPC_PORTAL_GUARDIAN_1               = 102335,  //+
    NPC_PORTAL_GUARDIAN_2               = 102337,  //+
    NPC_PORTAL_KEEPER_1                 = 102302,  //+
    NPC_PORTAL_KEEPER_2                 = 102336,  //+
    NPC_FELGUARD_DESTROYER_1            = 102272,  //+
    NPC_FELGUARD_DESTROYER_2            = 102368,  //+
    NPC_SHADOW_COUNCIL_WARLOCK          = 102380,  //+
    NPC_INFILTRATOR_ASSASSIN            = 102395,  //+
    NPC_EREDAR_SHADOW_MENDER            = 102400,  //+
    NPC_WRATHLORD_BULWARK               = 102397,  //+
    NPC_FELSTALKER_RAVENER_1            = 102269,  //+
    NPC_FELSTALKER_RAVENER_2            = 102369,  //+
    NPC_EREDAR_INVADER_1                = 102270,  //+
    NPC_EREDAR_INVADER_2                = 102370,  //+
    NPC_BLAZING_INFERNAL                = 102398,
    NPC_ACOLYTE_OF_SAELORN              = 112738,
    NPC_VENOMHIDE_SHADOWSPINNER         = 112733,
    NPC_BROOD_OF_SAELORN                = 112732,
    NPC_WRATHGUARD_DECIMATOR            = 112741,
    NPC_SHADOWY_OVERFIEND               = 112739,
    NPC_BLACK_BILE                      = 102169,

    //Betrug summons
    NPC_STASIS_CRYSTAL                  = 103672,

    //Encounters
    NPC_MINDFLAYER_KAAHRJ               = 101950,  // Talks +
    NPC_MILLIFICENT_MANASTORM           = 101976,  // Talks and event+
    NPC_FESTERFACE                      = 101995,  //Warnings and talks+
    NPC_SHIVERMAW                       = 101951,  //Warnings+
    NPC_ANUBESSET                       = 102246,  //Talks empty
    NPC_SAELORN                         = 102387,  //Talks+
    NPC_PRINCESS_THALENA                = 102431,  //Talks+
    NPC_FEL_LORD_BETRUG                 = 102446,  //Talks+
    NPC_IMAGE_OF_MILLHOUSE_MANASTORM    = 102040
};

enum eGameObjects
{
    GO_MAIN_DOOR                        = 247002,
    GO_INTRO_ACTIVATION_CRYSTAL         = 193615,
    GO_ACTIVATION_CRYSTAL               = 193611,

    GO_KAAHRJ_DOOR                      = 247000,
    GO_MILLIFICENT_DOOR                 = 246990,
    GO_FESTERFACE_DOOR                  = 246998,
    GO_SHIVERMAW_DOOR                   = 246999,
    GO_ANUBESSET_DOOR                   = 247385,
    GO_THALENA_DOOR                     = 246997,

    GO_MILLIFICENT_DISCARDED_LOCKBOX    = 246430
};

enum otherSpells
{
    SPELL_ARCANE_LIGHTNING          = 57930,
    SPELL_FEL_SHIELD                = 202311,
    SPELL_PORTAL_CHANNEL            = 202268,
    SPELL_PORTAL_PERIODIC           = 201901, //Caster 102282, HitUnit 102279
    SPELL_SHIELD_DESTRUCTION        = 202312,
    SPELL_DESTROY_DOOR_SEAL         = 202708,
    SPELL_DEATH_PENALTY             = 205201
};

enum Events
{
    EVENT_ACTIVATE_CRYSTAL = 20001,

    ACTION_REMOVE_LOOT = 100,
};

enum eWorldStates
{
    WORLD_STATE_VH              = 3816,
    WORLD_STATE_VH_PRISON_STATE = 3815,
    WORLD_STATE_VH_WAVE_COUNT   = 3810
};

enum ePlayerWorldStates
{
    WORLD_STATE_BLACK_BILE = 11414 ///< Allows players to complete the achievement
};

static std::array<Position const, DATA_MAX_PORTAL> g_PortalLocation =
{
    {
        { 4682.28f, 4013.68f, 91.80f, 3.14f },  ///< Guardian or Keeper
        { 4636.86f, 3963.92f, 87.05f, 2.05f },  ///< Guardian or Keeper
        { 4623.71f, 4060.67f, 82.72f, 4.76f },  ///< Guardian or Keeper
        { 4604.43f, 3980.73f, 78.04f, 1.10f },  ///< Guardian or Keeper
        { 4653.47f, 4041.61f, 78.08f, 3.74f },  ///< Elite Squad
        { 4667.37f, 4058.25f, 86.54f, 4.01f },  ///< Elite Squad
        { 4663.58f, 4014.96f, 82.72f, 3.12f },  ///< Elite Squad
        { 4667.05f, 3981.97f, 70.02f, 2.37f }   ///< Elite Squad
    }
};

Position const bossStartMove[6] =
{
    {4638.79f, 3955.60f, 86.97f}, //Mindflayer Kaahrj
    {4650.04f, 4051.10f, 77.97f}, //Millificent Manastorm
    {4669.23f, 4060.56f, 86.48f}, //Festerface
    {4598.52f, 3973.44f, 77.96f}, //Shivermaw
    {4649.25f, 3994.59f, 77.96f}, //Anub'esset
    {4619.39f, 4071.58f, 82.64f}  //Blood-Princess Thal'ena
};

Position const saboMovePos[6] =
{
    {4638.01f, 3962.11f, 95.58f, 4.80f}, //Cage Mindflayer Kaahrj
    {4644.70f, 4043.72f, 85.37f, 1.02f}, //Cage Millificent Manastorm
    {4662.63f, 4052.92f, 97.89f, 0.88f}, //Cage Festerface
    {4603.96f, 3978.66f, 85.50f, 4.04f}, //Cage Shivermaw
    {4665.21f, 3984.07f, 75.94f, 5.49f}, //Cage Anub'esset
    {4620.95f, 4063.44f, 89.05f, 1.84f}  //Cage Blood-Princess Thal'ena
};

Position const centrPos = {4628.95f, 4013.66f, 77.97f, 3.08f};
Position const MiddleRoomSaboLoc = {4637.53f, 4016.16f, 98.52f, 6.25f};
Position const saboFightPos = {4663.58f, 4014.95f, 82.72f, 3.01f};

#endif ///< VIOLET_HOLD_HPP