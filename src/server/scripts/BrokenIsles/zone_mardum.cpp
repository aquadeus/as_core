////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "zone_mardum.h"

class sceneTrigger_dh_init : public SceneTriggerScript
{
    enum Constants
    {
        ConversationID = 705
    };

    public:
        sceneTrigger_dh_init()
            : SceneTriggerScript("sceneTrigger_dh_init")
        {

        }

        /// Called when the scene event trigger by call CMSG_SCENE_TRIGGER_EVENT
        /// @p_Player : Scene player
        /// @p_Trigger      : Scene trigger
        /// @p_Triggername  : Trigger name
        bool OnTrigger(Player* p_Player, SpellScene const* p_Trigger, const std::string& p_Triggername) override
        {
            if (p_Triggername == "CUEILLIDANTH")
            {
                Conversation* l_Conversation = new Conversation();

                if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), Constants::ConversationID, p_Player, nullptr, *p_Player))
                    delete l_Conversation;
            }
            return true;
        }
};
/// Passive Scene Object
class playerScript_demon_hunter_homebind : public PlayerScript
{
    public:
        playerScript_demon_hunter_homebind() : PlayerScript("playerScript_demon_hunter_homebind") { }

        void OnQuestReward(Player* p_Player, const Quest* p_Quest) override
        {
            switch (p_Player->GetTeamId())
            {
                case TEAM_ALLIANCE:
                {
                    if (p_Quest->GetQuestId() == 39689)
                    {
                        /// Stormwind
                        WorldLocation l_Location(0, -8542.659180f, 461.016296f, 104.728722f, 5.389493f);
                        p_Player->SetHomebind(l_Location, 5390);
                    }
                    break;
                }
                case TEAM_HORDE:
                {
                    if (p_Quest->GetQuestId() == 39690)
                    {
                        /// Orgrimmar
                        WorldLocation l_Location(1, 1464.276367f, -4420.542969f, 25.454105f, 0.139330f);
                        p_Player->SetHomebind(l_Location, 1637);
                    }
                    break;
                }
                default:
                    break;
            }
        }
};

class playerscript_demon_hunter_fel_secrets_objective : public PlayerScript
{
    public:
        playerscript_demon_hunter_fel_secrets_objective() : PlayerScript("playerscript_demon_hunter_fel_secrets_objective") { }

        enum eQuests
        {
            FelSecrets          = 40051,
            FelSecretsCredit    = 99071
        };

        enum eDisplayChoices
        {
            FelSecretsChoice    = 231
        };

        enum eResponses
        {
            HavocResponse       = 478,
            VengeanceResponse   = 479
        };

        void OnPlayerChoiceResponse(Player* p_Player, uint32 p_ChoiceID, uint32 p_ResponseID) override
        {
            if (!p_Player || !p_ChoiceID || !p_ResponseID || p_ChoiceID != eDisplayChoices::FelSecretsChoice)
                return;

            if (!p_Player->HasQuest(eQuests::FelSecrets) || p_Player->GetQuestStatus(eQuests::FelSecrets) == QUEST_STATUS_COMPLETE)
                return;

            p_Player->KilledMonsterCredit(eQuests::FelSecretsCredit);
        }
};

#ifndef __clang_analyzer__
void AddSC_mardum()
{
    /// Scene script
    new sceneTrigger_dh_init();

    /// Playerscripts
    new playerScript_demon_hunter_homebind();
    new playerscript_demon_hunter_fel_secrets_objective();
}
#endif
