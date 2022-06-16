////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "ScriptPCH.h"
#include "ScriptMgr.h"

class PromotionNewPlayers : public PlayerScript
{
public:
    PromotionNewPlayers() :PlayerScript("PromotionNewPlayers") {}

    enum Values
    {
        February2015 = 1422777600,  ///< Timestamp of 1st of February 2015.
        March232015  = 1427065200,  ///< Timestamp of 23nd of March 2015.
        March152015  = 1426374000,  ///< Timestamp of 15nd of March 2015.
        SwiftNetherDrake = 37015,   ///< SpellId.
        ImperialQuilen   = 85870,   ///< ItemId
        ApprenticeRidingSkill = 33388
    };

    void OnLogin(Player* p_Player)
    {
        if (time(NULL) < Values::February2015 && p_Player->GetTotalPlayedTime() == 0)
        {
            p_Player->learnSpell(Values::ApprenticeRidingSkill, false);
            p_Player->learnSpell(Values::SwiftNetherDrake, false);
        }

        if (time(nullptr) < Values::March232015
            && p_Player->GetSession()->GetAccountJoinDate() >= Values::March152015
            && p_Player->GetTotalPlayedTime() == 0)
            p_Player->AddItem(Values::ImperialQuilen, 1);
    }
};

class MischiefEventPlayers : public PlayerScript
{
    public:
        MischiefEventPlayers() :PlayerScript("MischiefEventPlayers") {}

        enum eValues
        {
            EventBegin    = 1492207200, ///< Timestamp of 15th of April 2017
            EventEnd      = 1493589600, ///< Timestamp of 1st of May 2017
            MischiefSpell = 225761
        };

        void OnLogin(Player* p_Player)
        {
            if (time(nullptr) > eValues::EventBegin && time(nullptr) < eValues::EventEnd && !p_Player->HasSpell(eValues::MischiefSpell))
                p_Player->learnSpell(eValues::MischiefSpell, false);
        }
};

#ifndef __clang_analyzer__
void AddSC_promotion_new_players()
{
    //new PromotionNewPlayers();
    //new MischiefEventPlayers();
};
#endif
