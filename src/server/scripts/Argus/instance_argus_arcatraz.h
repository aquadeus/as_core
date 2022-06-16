////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2018 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

enum eSpells
{
    Spell_IntroducingConversation = 243308,
    Spell_FelDissolveOut = 239160,
    Spell_CreateLegionPortal = 247654,
    Spell_FelChanneling = 242061,
    Spell_FelShield = 245218
};

enum eNpcs
{
    Npc_LegionPortal = 124517,
    Npc_GenericBunnyKmart = 121501,
    Npc_FelshadowSeeker = 121319,
    Npc_EredarEnforcer = 121817,
    Npc_Lahzaruun = 121320,
    Npc_HighInquisitorRaalgar = 124511
};

enum eGobs
{
    Gob_StasisDoor = 269273,
    Gob_NaaruPrison = 269240,
    Gob_PortalToTheVindicaar = 269228
};

enum eActions
{
    Action_LahzaruunStartEvent = 1,
    Action_LahzaruunMoveToFightPos = 2,
    Action_HighInquisitorRaalgarStartEvent = 3
};

enum eQuests
{
    Quest_FoilingTheLegionsJailbreak = 47134
};

enum eWorldSafeLocs
{
    Loc_ArcatrazEnter = 6131,
    Loc_ArcatrazSecondFloor = 6132,
    Loc_ArcatrazBoss = 6134
};

enum eObjectives
{
    Obj_SearchTheArcatraz = 291047,
    Obj_CloseLegionPortals = 291048,
    Obj_HighInquisitorRaalgarFound = 290226,
    Obj_LahzaruunSlain = 290227
};

enum eKillcredits
{
    Killcredit_HighInquisitorRaalgarFound = 122018,
    Killcredit_EscapeFromTheArcatraz = 121348
};

enum eInstanceStates
{
    State_SearchTheArcatraz = 1,
    State_CloseLegionPortals = 2,
    State_HighInquisitorRaalgarFound = 3,
    State_LahzaruunSlain = 4,
    State_EscapeFromTheArcatraz = 5
};

enum eDatas
{
    Data_ActivatePortals = 1,
    Data_PortalClosed = 2,
    Data_InquisitorFound = 3,
    Data_LahzaruunSlained = 4
};

enum eConversations
{
    Conv_ClosePortals = 5342,
    Conv_EscapeFromTheArcatraz = 5052
};

const Position g_PortalToTheVindicaarSpawnPos = { 164.6597f, -45.11806f, 961.0979f };

std::vector<std::string> const g_LahzaruunMinionLinkedIds =
{
    { "311DFF7329B0B59C4168A65651B6E17EE8612110" },
    { "BC08757F864718F34202B3139EBD417E44ADC421" },
    { "1C2BE4F029DECB86AAD60C34D8CA1560332D4CE6" },
    { "F3381AA4111DA57734414DFACDE86BD72AEBA914" },
    { "0208CA86647C12176F4F3EAD79A18CB3CCF22BF9" }
};
