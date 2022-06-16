////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "ClassHallUpgrades.h"

namespace MS { namespace Garrison { namespace GBrokenIsles {


    npc_class_hall_upgrades::npc_class_hall_upgrades() : CreatureScript("npc_class_hall_upgrades")
    {
    }

    enum eObjectives
    {
        LoramusStory = 285293
    };

    enum eQuests
    {
        DemonicImprovements = 42683
    };

    bool npc_class_hall_upgrades::OnGossipHello(Player* p_Player, Creature* p_Creature)
    {
        for (auto const& l_QuestID : m_TroopsInTheFieldQuestIDs)
        {
            if (p_Player->IsQuestBitFlaged(GetQuestUniqueBitFlag(l_QuestID)))
            {
                if (p_Player->GetQuestStatus(eQuests::DemonicImprovements) == QUEST_STATUS_INCOMPLETE && p_Player->GetQuestObjectiveCounter(eObjectives::LoramusStory) == 0)
                {
                    p_Player->PrepareGossipMenu(p_Creature, p_Creature->GetEntry());
                    p_Player->SendPreparedGossip(p_Creature);
                    return true;
                }

                WorldPacket l_Data(SMSG_GARRISON_OPEN_TALENT_NPC);
                l_Data.appendPackGUID(p_Creature->GetGUID());
                l_Data << uint32(0);    ///< Unk
                l_Data << uint32(0);    ///< Unk
                p_Player->SendDirectMessage(&l_Data);
                p_Player->TalkedToCreature(p_Creature->GetEntry(), p_Player->GetGUID());

                return true;
            }
        }
        return false;
    }

    bool npc_class_hall_upgrades::OnGossipSelect(Player* p_Player, Creature* p_Creature, uint32 /*sender*/, uint32 p_Action)
    {
        p_Player->PlayerTalkClass->ClearMenus();
        p_Player->PlayerTalkClass->SendCloseGossip();
        p_Player->KilledMonsterCredit(p_Creature->GetEntry());
        p_Creature->AI()->Talk(0, p_Player->GetGUID());
        p_Creature->AI()->DelayTalk(10, 1, p_Player->GetGUID());
        p_Creature->AI()->DelayTalk(20, 2, p_Player->GetGUID());
        p_Creature->AI()->DelayTalk(30, 3, p_Player->GetGUID());
        p_Creature->AI()->DelayTalk(40, 4, p_Player->GetGUID());
        p_Creature->AI()->DelayTalk(50, 5, p_Player->GetGUID());
        p_Creature->AI()->DelayTalk(60, 6, p_Player->GetGUID());
        p_Creature->AI()->DelayTalk(70, 7, p_Player->GetGUID());
        return true;
    }

    npc_class_hall_upgrades::npc_class_hall_upgradesAI::npc_class_hall_upgradesAI(Creature* p_Creature) : GarrisonNPCAI(p_Creature)
    {
    }
}   ///< namespace GBrokenIsles
}   ///< namespace Garrison
}   ///< namespace MS


#ifndef __clang_analyzer__
void AddSC_ClassHallUpgrades()
{
    /// Npc
    new MS::Garrison::GBrokenIsles::npc_class_hall_upgrades();
}
#endif
