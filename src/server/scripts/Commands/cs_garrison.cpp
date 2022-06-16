#ifndef CROSS
#include "ScriptMgr.h"
#include "GameEventMgr.h"
#include "Chat.h"
#include "GarrisonMgr.hpp"
#include "ObjectMgr.h"
#include "../Draenor/Garrison/GarrisonScriptData.hpp"

/// Garrison commands
class garrison_commandscript: public CommandScript
{
    public:
        /// Constructor
        garrison_commandscript()
            : CommandScript("garrison_commandscript")
        {

        }

        /// Get command table
        ChatCommand* GetCommands() const
        {
            static ChatCommand blueprintCommandTable[] =
            {
                { "learn",          SEC_ADMINISTRATOR,          SEC_ADMINISTRATOR, true,  &HandleBlueprintLearnCommand, "", NULL },
                { NULL,             0,                          0,                 false, NULL,                         "", NULL }
            };

            static ChatCommand plotCommandTable[] =
            {
                { "info",           SEC_ADMINISTRATOR,          SEC_ADMINISTRATOR, true,  &HandlePlotInfoCommand,   "", NULL },
                { "add",            SEC_ADMINISTRATOR,          SEC_ADMINISTRATOR, true,  &HandlePlotAddCommand,    "", NULL },
                { "del",            SEC_ADMINISTRATOR,          SEC_ADMINISTRATOR, true,  &HandlePlotDelCommand,    "", NULL },
                { "import",         SEC_ADMINISTRATOR,          SEC_ADMINISTRATOR, true,  &HandlePlotImportCommand, "", NULL },
                { NULL,             0,                          0,                 false, NULL,                     "", NULL }
            };

            static ChatCommand followerCommandTable[] =
            {
                { "add",            SEC_ADMINISTRATOR,          SEC_ADMINISTRATOR, true,  &HandleFollowerAddCommand,     "", NULL },
                { "addlist",        SEC_ADMINISTRATOR,          SEC_ADMINISTRATOR, true,  &HandleFolloweraddlistCommand, "", NULL },
                { "remove",         SEC_ADMINISTRATOR,          SEC_ADMINISTRATOR, true,  &HandleFollowerRemoveCommand,  "", NULL },
                { "reroll",         SEC_ADMINISTRATOR,          SEC_ADMINISTRATOR, true,  &HandleFollowerRerollCommand,  "", NULL },
                { "list",           SEC_ADMINISTRATOR,          SEC_ADMINISTRATOR, true,  &HandleFollowerListCommand,    "", NULL },
                { NULL,             0,                          0,                  false, NULL,                          "", NULL }
            };

            static ChatCommand missionCommandTable[] =
            {
                { "add",            SEC_ADMINISTRATOR,          SEC_ADMINISTRATOR, true,  &HandleMissionAddCommand,         "", NULL },
                { "completeall",    SEC_ADMINISTRATOR,          SEC_ADMINISTRATOR, true,  &HandleMissionCompleteAllCommand, "", NULL },
                { "dumpmechanic",   SEC_ADMINISTRATOR,          SEC_ADMINISTRATOR, true,  &HandleMissionDump,               "", NULL },
                { "resetdaily",     SEC_ADMINISTRATOR,          SEC_ADMINISTRATOR, true,  &HandleMissionResetDaily,         "", NULL },
                { NULL,             0,                          0,                      false, NULL,                             "", NULL }
            };

            static ChatCommand buildingCommandTable[] =
            {
                { "complete",         SEC_ADMINISTRATOR,        SEC_ADMINISTRATOR, true,  &HandleBuildingCompleteCommand,         "", NULL },
                { NULL,               0,                        0,                 false, NULL,                                   "", NULL }
            };

            static ChatCommand shipmentCommandTable[] =
            {
                { "complete",         SEC_ADMINISTRATOR,        SEC_ADMINISTRATOR, true,  &HandleShipmentCompleteCommand,         "", NULL },
                { NULL,               0,                        0,                 false, NULL,                                   "", NULL }
            };

            static ChatCommand garrisonCommandTable[] =
            {
                { "blueprint",              SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  true,   NULL,                             "", blueprintCommandTable },
                { "plot",                   SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  true,   NULL,                             "", plotCommandTable      },
                { "follower",               SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  true,   NULL,                             "", followerCommandTable  },
                { "mission" ,               SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  true,   NULL,                             "", missionCommandTable   },
                { "building",               SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  true,   NULL,                             "", buildingCommandTable  },
                { "shipment",               SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  true,   NULL,                             "", shipmentCommandTable  },
                { "info",                   SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  true,   &HandleGarrisonInfo,              "", NULL                  },
                { "setlevel",               SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  true,   &HandleGarrisonSetLevel,          "", NULL                  },
                { "create",                 SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  true,   &HandleGarrisonCreate,            "", NULL                  },
                { "prepare",                SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  true,   &HandleGarrisonPrepare,           "", NULL                  },
                { "delete",                 SEC_CONSOLE,            SEC_CONSOLE,        true,   &HandleGarrisonDelete,            "", NULL                  },
                { "resetdailydata",         SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  true,   &HandleGarrisonResetDailyDatas,   "", NULL                  },
                { "resetweeklydata",        SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  true,   &HandleGarrisonResetWeeklyDatas,  "", NULL                  },
                { "completeTalent",         SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  true,   &HandleGarrisonCompleteTatlent,   "", NULL                  },
                { "missionsuccesschance",   SEC_PLAYER,             SEC_PLAYER,         true,   &HandleGarrisonMissionSuccessChance, "", NULL               },
                { "missionduration",        SEC_PLAYER,             SEC_PLAYER,         true,   &HandleGarrisonMissionDuration,   "", NULL                  },
                { "missionnobonusloot",     SEC_PLAYER,             SEC_PLAYER,         true,   &HandleGarrisonMissionNoBonusLoot, "", NULL                 },
                { "missionlethal",          SEC_PLAYER,             SEC_PLAYER,         true,   &HandleGarrisonMissionLethal,     "", NULL                  },
                { "missioncost",            SEC_PLAYER,             SEC_PLAYER,         true,   &HandleGarrisonMissionCost,       "", NULL                  },
                { "addonenabled",           SEC_PLAYER,             SEC_PLAYER,         true,   &HandleGarrisonAddonEnabled,      "", NULL                  },
                { NULL,                     0,                      0,                  false,  NULL,                             "", NULL                  }
            };

            static ChatCommand shipyardCommandTable[] =
            {
                { "open",       SEC_ADMINISTRATOR,          SEC_ADMINISTRATOR, false, &HandleShipyardOpen, NULL      }, ///< Missing field 'ChildCommands' initializer
                { NULL,         0,                          0,                 false, NULL, "", NULL                 }
            };

            static ChatCommand commandTable[] =
            {
                { "garrison",   SEC_ADMINISTRATOR,          SEC_ADMINISTRATOR, false, NULL, "", garrisonCommandTable },
                { "shipyard",   SEC_ADMINISTRATOR,          SEC_ADMINISTRATOR, false, NULL, "", shipyardCommandTable },
                { NULL,         0,                          0,                 false, NULL, "", NULL                 }
            };
            return commandTable;
        }


        static bool HandleGarrisonAddonEnabled(ChatHandler* p_Handler, char const* /*p_Args*/)
        {
            Player* l_TargetPlayer = p_Handler->GetSession()->GetPlayer();
            if (!l_TargetPlayer)
                return false;

            MS::Garrison::Manager* l_Garrison = l_TargetPlayer->GetGarrison();
            //l_Garrison->SetAddonEnabled();

            return true;
        }


        static bool HandleGarrisonMissionSuccessChance(ChatHandler* p_Handler, char const* p_Args)
        {
            Player* l_TargetPlayer = p_Handler->GetSession()->GetPlayer();
            if (!l_TargetPlayer)
                return false;

            MS::Garrison::Manager* l_Garrison = l_TargetPlayer->GetGarrison();

            if (p_Args)
            {
                //l_Garrison->SetMissionSuccessChance(atoi(p_Args));
            }

            return true;
        }

        static bool HandleGarrisonMissionDuration(ChatHandler* p_Handler, char const* p_Args)
        {
            Player* l_TargetPlayer = p_Handler->GetSession()->GetPlayer();
            if (!l_TargetPlayer)
                return false;

            MS::Garrison::Manager* l_Garrison = l_TargetPlayer->GetGarrison();

            if (p_Args)
            {
                //l_Garrison->SetMissionDuration(atoi(p_Args));
            }

            return true;
        }

        static bool HandleGarrisonMissionNoBonusLoot(ChatHandler* p_Handler, char const* p_Args)
        {
            Player* l_TargetPlayer = p_Handler->GetSession()->GetPlayer();
            if (!l_TargetPlayer)
                return false;

            MS::Garrison::Manager* l_Garrison = l_TargetPlayer->GetGarrison();

            if (p_Args)
            {
                //l_Garrison->SetMissionNoBonusLoot(atoi(p_Args));
            }

            return true;
        }

        static bool HandleGarrisonMissionLethal(ChatHandler* p_Handler, char const* p_Args)
        {
            Player* l_TargetPlayer = p_Handler->GetSession()->GetPlayer();
            if (!l_TargetPlayer)
                return false;

            MS::Garrison::Manager* l_Garrison = l_TargetPlayer->GetGarrison();

            if (p_Args)
            {
                //l_Garrison->SetMissionLethal(atoi(p_Args));
            }

            return true;
        }

        static bool HandleGarrisonMissionCost(ChatHandler* p_Handler, char const* p_Args)
        {
            Player* l_TargetPlayer = p_Handler->GetSession()->GetPlayer();
            if (!l_TargetPlayer)
                return false;

            MS::Garrison::Manager* l_Garrison = l_TargetPlayer->GetGarrison();

            if (p_Args)
            {
                //l_Garrison->SetMissionCost(atoi(p_Args));
            }

            return true;
        }


        static bool HandleGarrisonCompleteTatlent(ChatHandler* p_Handler, char const* /*p_Args*/)
        {
            Player* l_TargetPlayer = p_Handler->GetSession()->GetPlayer();
            if (!l_TargetPlayer)
                return false;

            MS::Garrison::Manager* l_Garrison = l_TargetPlayer->GetGarrison();

            std::vector<MS::Garrison::GarrisonTalent const*> m_Talents = l_Garrison->GetGarrisonTalents(MS::Garrison::GarrisonType::GarrisonBrokenIsles);
            for (MS::Garrison::GarrisonTalent const* l_ConstTalent : m_Talents)
            {
                MS::Garrison::GarrisonTalent* l_Talent = const_cast<MS::Garrison::GarrisonTalent*>(l_ConstTalent);
                if (l_Talent->IsBeingResearched())
                {
                    l_Talent->Flags |= MS::Garrison::GarrisonTalentFlags::Researched;
                    l_Garrison->SendTalentUpdate(*l_Talent);
                    l_Garrison->ApplyTalentEffect(l_Talent->GetEntry(), true, true);
                }
            }
            return true;
        }

        static bool HandleGarrisonInfo(ChatHandler* p_Handler, char const* /*p_Args*/)
        {
            Player* l_TargetPlayer = p_Handler->getSelectedPlayer();

            if (!l_TargetPlayer || !l_TargetPlayer->GetDraenorGarrison())
            {
                p_Handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            const GarrSiteLevelEntry* l_Entry = l_TargetPlayer->GetDraenorGarrison()->GetGarrisonSiteLevelEntry();

            if (!l_Entry)
            {
                p_Handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            p_Handler->PSendSysMessage("Garrison info");
            p_Handler->PSendSysMessage("Level : %u SiteLevelID : %u MapID : %u FactionID : %u", l_Entry->Level, l_Entry->SiteID, l_Entry->MapID, (uint32)l_TargetPlayer->GetGarrison()->GetGarrisonFactionIndex());
            p_Handler->PSendSysMessage("Cache Resource : %u", l_TargetPlayer->GetDraenorGarrison()->GetGarrisonCacheTokenCount());

            return true;
        }

        static bool HandleGarrisonSetLevel(ChatHandler* p_Handler, char const* p_Args)
        {
            Player* l_TargetPlayer = p_Handler->getSelectedPlayer();

            if (!l_TargetPlayer || !l_TargetPlayer->GetDraenorGarrison())
            {
                p_Handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            if (p_Args != 0)
            {
                uint32 l_Level = atoi(p_Args);

                if (!l_Level)
                    return false;

                l_TargetPlayer->GetDraenorGarrison()->SetLevel(l_Level);
            }

            return true;
        }

        /// This is a debug command, only devs should use it
        static bool HandleGarrisonPrepare(ChatHandler* p_Handler, char const* /*p_Args*/)
        {
            Player* l_Target = p_Handler->getSelectedPlayer();
            if (!l_Target)
            {
                p_Handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            /// check online security
            if (p_Handler->HasLowerSecurity(l_Target, 0))
                return false;

            std::string targetNameLink = p_Handler->GetNameLink(l_Target);

            if (l_Target->isInFlight())
            {
                p_Handler->PSendSysMessage(LANG_CHAR_IN_FLIGHT, targetNameLink.c_str());
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            /// Set higher speed
            l_Target->SetSpeed(MOVE_WALK, 5.0f, true);
            l_Target->SetSpeed(MOVE_RUN, 5.0f, true);
            l_Target->SetSpeed(MOVE_SWIM, 5.0f, true);
            l_Target->SetSpeed(MOVE_FLIGHT, 5.0f, true);

            /// Set fly
            l_Target->SetCanFly(true);

            /// Set Garrison resources + money at high values
            l_Target->ModifyCurrency(CurrencyTypes::CURRENCY_TYPE_GARRISON_RESSOURCES, 10000, true, true);
            l_Target->ModifyMoney(1000000000);

            /// Set max level to be able to test everything
            if (l_Target->getLevel() < 90)
                l_Target->SetLevel(100);

            /// Create Garrison if player doesn't have it
            HandleGarrisonCreate(p_Handler, "HandleGarrisonPrepare");

            return true;
        }

        static bool HandleGarrisonCreate(ChatHandler* p_Handler, char const* p_Args)
        {
            Player* l_TargetPlayer = p_Handler->getSelectedPlayer();

            if (!l_TargetPlayer)
            {
                p_Handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            if (p_Args != 0)
            {
                uint32 l_Type = atoi(p_Args);

                if (l_Type != 2 && l_Type != 3)
                    l_Type = 2;

                l_TargetPlayer->CastSpell(l_TargetPlayer, l_Type == 2 ? 156020 : 185506);
            }

            return true;
        }

        static bool HandleGarrisonDelete(ChatHandler* p_Handler, char const* p_Args)
        {
            Player* l_TargetPlayer = p_Handler->getSelectedPlayer();

            if (!l_TargetPlayer)
            {
                p_Handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            if (!l_TargetPlayer->GetGarrison())
            {
                p_Handler->PSendSysMessage("Player doesnt have a garrison");
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            if (p_Args != 0)
            {
                uint32 l_Type = atoi(p_Args);

                if (!l_Type)
                    return false;

                l_TargetPlayer->DeleteGarrison(static_cast<MS::Garrison::GarrisonType::Type>(l_Type));

                if (l_TargetPlayer->GetCurrency(MS::Garrison::GDraenor::Globals::CurrencyID, false))
                    l_TargetPlayer->ModifyCurrency(MS::Garrison::GDraenor::Globals::CurrencyID, -(int32)l_TargetPlayer->GetCurrency(MS::Garrison::GDraenor::Globals::CurrencyID, false));

            }

            return true;
        }

        static bool HandleGarrisonResetDailyDatas(ChatHandler* p_Handler, char const* /*p_Args*/)
        {
            Player* l_TargetPlayer = p_Handler->getSelectedPlayer();

            if (!l_TargetPlayer)
            {
                p_Handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            if (!l_TargetPlayer->GetGarrison())
            {
                p_Handler->PSendSysMessage("Player doesnt have a garrison");
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            l_TargetPlayer->ResetDailyGarrisonDatas();

            return true;
        }

        static bool HandleGarrisonResetWeeklyDatas(ChatHandler* p_Handler, char const* /*p_Args*/)
        {
            Player* l_TargetPlayer = p_Handler->getSelectedPlayer();

            if (!l_TargetPlayer)
            {
                p_Handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            if (!l_TargetPlayer->GetGarrison())
            {
                p_Handler->PSendSysMessage("Player doesnt have a garrison");
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            l_TargetPlayer->ResetWeeklyGarrisonDatas();

            return true;
        }

        static bool HandleBlueprintLearnCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            Player* l_TargetPlayer = p_Handler->getSelectedPlayer();

            if (!l_TargetPlayer || !l_TargetPlayer->GetDraenorGarrison())
            {
                p_Handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            if (p_Args != 0)
            {
                std::string l_Args = p_Args;

                if (l_Args == "all")
                {
                    for (uint32 l_I = 0; l_I < sGarrBuildingStore.GetNumRows(); ++l_I)
                    {
                        const GarrBuildingEntry* l_Entry = sGarrBuildingStore.LookupEntry(l_I);

                        if (l_Entry)
                            l_TargetPlayer->GetDraenorGarrison()->LearnBlueprint(l_Entry->ID);
                    }

                    return true;
                }
                else
                {
                    uint32 l_Blueprint = atoi(p_Args);

                    if (!l_Blueprint)
                        return false;

                    return l_TargetPlayer->GetDraenorGarrison()->LearnBlueprint(l_Blueprint);
                }
            }

            return true;
        }

        static bool HandlePlotInfoCommand(ChatHandler* p_Handler, char const* /*p_Args*/)
        {
            Player* l_TargetPlayer = p_Handler->GetSession()->GetPlayer();

            if (!l_TargetPlayer || !l_TargetPlayer->GetDraenorGarrison())
            {
                p_Handler->SendSysMessage("You don't have a garrison");
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            MS::Garrison::GarrisonPlotInstanceInfoLocation l_Info = l_TargetPlayer->GetDraenorGarrison()->GetPlot(l_TargetPlayer->GetPositionX(), l_TargetPlayer->GetPositionY(), l_TargetPlayer->GetPositionZ());
            if (!l_Info.PlotInstanceID)
            {
                p_Handler->SendSysMessage("Plot not found");
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            p_Handler->PSendSysMessage("Plot instance id %u type %u", l_Info.PlotInstanceID, l_TargetPlayer->GetDraenorGarrison()->GetPlotType(l_Info.PlotInstanceID));
            p_Handler->PSendSysMessage("Position %f %f %f %f", l_Info.X, l_Info.Y, l_Info.Z, l_Info.O);

            MS::Garrison::GDraenor::GarrisonBuilding l_Building = l_TargetPlayer->GetDraenorGarrison()->GetBuilding(l_Info.PlotInstanceID);

            if (l_Building.BuildingID)
            {
                const GarrBuildingEntry* l_Entry = sGarrBuildingStore.LookupEntry(l_Building.BuildingID);

                p_Handler->PSendSysMessage("Building : %u - %s", l_Entry->ID, l_TargetPlayer->GetGarrison()->GetGarrisonFactionIndex() == MS::Garrison::FactionIndex::Alliance ? l_Entry->NameAlliance->Get(DEFAULT_LOCALE) : l_Entry->NameHorde->Get(DEFAULT_LOCALE));
                p_Handler->PSendSysMessage("Active %u Level %u", l_Building.Active, l_Entry->Level);
            }

            float l_X = l_TargetPlayer->GetPositionX();
            float l_Y = l_TargetPlayer->GetPositionY();
            float l_Z = l_TargetPlayer->GetPositionZ();
            float l_O = l_TargetPlayer->GetOrientation();

            G3D::Matrix3 l_Mat = G3D::Matrix3::identity();
            l_Mat = l_Mat.fromAxisAngle(G3D::Vector3(0, 0, 1), -l_Info.O);

            G3D::Vector3 l_ElementPosition = G3D::Vector3(l_X, l_Y, 0);
            l_ElementPosition  -= G3D::Vector3(l_Info.X, l_Info.Y, 0);
            l_ElementPosition   = l_Mat *l_ElementPosition;
            l_ElementPosition.z = l_Z - l_Info.Z;

            float l_Orientation = Position::NormalizeOrientation((2 * M_PI) - Position::NormalizeOrientation(l_Info.O - l_O));

            p_Handler->PSendSysMessage("Relative location X:%f Y:%f Z:%f O:%f", l_ElementPosition.x, l_ElementPosition.y, l_ElementPosition.z, l_Orientation);

            return true;
        }

        static bool HandlePlotAddCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            Player* l_Player = p_Handler->GetSession()->GetPlayer();

            if (!l_Player || !l_Player->GetDraenorGarrison())
            {
                p_Handler->SendSysMessage("You don't have a garrison");
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            MS::Garrison::GarrisonPlotInstanceInfoLocation l_Info = l_Player->GetDraenorGarrison()->GetPlot(l_Player->GetPositionX(), l_Player->GetPositionY(), l_Player->GetPositionZ());

            if (!l_Info.PlotInstanceID)
            {
                p_Handler->SendSysMessage("Plot not found");
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            MS::Garrison::GDraenor::GarrisonBuilding l_Building = l_Player->GetDraenorGarrison()->GetBuilding(l_Info.PlotInstanceID);

            if (!l_Building.BuildingID)
            {
                p_Handler->SendSysMessage("Building not found");
                p_Handler->SetSentErrorMessage(true);
            }

            int32 l_ElemEntry = atoi(p_Args);
            float l_X = l_Player->GetPositionX();
            float l_Y = l_Player->GetPositionY();
            float l_Z = l_Player->GetPositionZ();
            float l_O = l_Player->GetOrientation();

            G3D::Matrix3 l_Mat = G3D::Matrix3::identity();
            l_Mat = l_Mat.fromAxisAngle(G3D::Vector3(0, 0, 1), -l_Info.O);

            G3D::Vector3 l_ElementPosition = G3D::Vector3(l_X, l_Y, 0);
            l_ElementPosition -= G3D::Vector3(l_Info.X, l_Info.Y, 0);
            l_ElementPosition = l_Mat *l_ElementPosition;
            l_ElementPosition.z = l_Z - l_Info.Z;

            if (l_ElemEntry > 0)
                l_ElementPosition.z += 0.5f;

            int32 l_TableIndex = l_Player->GetDraenorGarrison()->GetPlotType(l_Info.PlotInstanceID);
            if (l_Building.Active)
                l_TableIndex = -int32(l_Building.BuildingID);

            GarrisonPlotBuildingContent l_NewData;
            l_NewData.PlotTypeOrBuilding    = l_TableIndex;
            l_NewData.CreatureOrGob         = l_ElemEntry;
            l_NewData.FactionIndex          = l_Player->GetGarrison()->GetGarrisonFactionIndex();
            l_NewData.X                     = l_ElementPosition.x;
            l_NewData.Y                     = l_ElementPosition.y;
            l_NewData.Z                     = l_ElementPosition.z;
            l_NewData.O                     = Position::NormalizeOrientation((2 * M_PI) - Position::NormalizeOrientation(l_Info.O - l_O));

            sObjectMgr->AddGarrisonPlotBuildingContent(l_NewData);

            /// Test code
            if (true)
            {
                G3D::Vector3 l_NonRotatedPosition;

                l_Mat = G3D::Matrix3::identity();
                l_Mat = l_Mat.fromAxisAngle(G3D::Vector3(0, 0, 1), -l_Info.O);

                /// transform plot coord
                l_NonRotatedPosition = l_Mat * G3D::Vector3(l_Info.X, l_Info.Y, l_Info.Z);

                G3D::Vector3 l_Position = G3D::Vector3(l_ElementPosition.x, l_ElementPosition.y, 0);

                l_Mat = G3D::Matrix3::identity();
                l_Mat = l_Mat.fromAxisAngle(G3D::Vector3(0, 0, 1), l_Info.O);

                l_Position.x += l_NonRotatedPosition.x;
                l_Position.y += l_NonRotatedPosition.y;

                l_Position = l_Mat * l_Position;

                l_Position.z = l_ElementPosition.z + l_Info.Z;

                p_Handler->PSendSysMessage("Spawn coord %f %f %f %f", l_X, l_Y, l_Z, l_O);
                p_Handler->PSendSysMessage("Trans coord %f %f %f %f", l_Position.x, l_Position.y, l_Position.z, l_NewData.O + l_Info.O);
            }

            l_Player->GetGarrison()->OnPlayerEnter();

            return true;
        }

        static bool HandlePlotDelCommand(ChatHandler* p_Handler, char const* /*p_Args*/)
        {
            Player* l_Player     = p_Handler->GetSession()->GetPlayer();
            WorldObject* l_Target = nullptr;

            if (p_Handler->getSelectedCreature())
                l_Target = p_Handler->getSelectedCreature();

            if (!l_Player || !l_Player->GetDraenorGarrison())
            {
                p_Handler->SendSysMessage("You don't have a garrison");
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            if (!l_Target)
            {
                p_Handler->SendSysMessage("No valid target");
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            MS::Garrison::GarrisonPlotInstanceInfoLocation l_Info = l_Player->GetDraenorGarrison()->GetPlot(l_Player->GetPositionX(), l_Player->GetPositionY(), l_Player->GetPositionZ());

            if (!l_Info.PlotInstanceID)
            {
                p_Handler->SendSysMessage("Plot not found");
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            G3D::Vector3 l_NonRotatedPosition;

            G3D::Matrix3 l_Mat = G3D::Matrix3::identity();
            l_Mat = l_Mat.fromAxisAngle(G3D::Vector3(0, 0, 1), -l_Info.O);

            /// transform plot coord
            l_NonRotatedPosition = l_Mat * G3D::Vector3(l_Info.X, l_Info.Y, l_Info.Z);

            std::vector<GarrisonPlotBuildingContent> l_Contents = sObjectMgr->GetGarrisonPlotBuildingContent(l_Player->GetDraenorGarrison()->GetPlotType(l_Info.PlotInstanceID), l_Player->GetGarrison()->GetGarrisonFactionIndex());

            for (uint32 l_I = 0; l_I < l_Contents.size(); ++l_I)
            {
                if (l_Target->ToCreature() && l_Contents[l_I].CreatureOrGob > 0 && l_Contents[l_I].CreatureOrGob == l_Target->GetEntry())
                {
                    G3D::Vector3 l_Position = G3D::Vector3(l_Contents[l_I].X, l_Contents[l_I].Y, 0);

                    l_Mat = G3D::Matrix3::identity();
                    l_Mat = l_Mat.fromAxisAngle(G3D::Vector3(0, 0, 1), l_Info.O);

                    l_Position.x += l_NonRotatedPosition.x;
                    l_Position.y += l_NonRotatedPosition.y;

                    l_Position = l_Mat * l_Position;

                    l_Position.z = l_Contents[l_I].Z + l_Info.Z;


                    if (l_Target->GetDistance(l_Position.x, l_Position.y, l_Position.z) < 0.2f)
                    {
                        sObjectMgr->DeleteGarrisonPlotBuildingContent(l_Contents[l_I]);
                        l_Player->GetGarrison()->OnPlayerEnter();
                        return true;
                    }
                }
                else
                {
                    /// @TODO game object case
                }
            }

            return true;
        }

        static bool HandlePlotImportCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            Player* l_Player = p_Handler->GetSession()->GetPlayer();

            if (!l_Player || !l_Player->GetDraenorGarrison())
            {
                p_Handler->SendSysMessage("You don't have a garrison");
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            MS::Garrison::GarrisonPlotInstanceInfoLocation l_Info = l_Player->GetDraenorGarrison()->GetPlot(l_Player->GetPositionX(), l_Player->GetPositionY(), l_Player->GetPositionZ());

            if (!l_Info.PlotInstanceID)
            {
                p_Handler->SendSysMessage("Plot not found");
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            MS::Garrison::GDraenor::GarrisonBuilding l_Building = l_Player->GetDraenorGarrison()->GetBuilding(l_Info.PlotInstanceID);

            if (!l_Building.BuildingID)
            {
                p_Handler->SendSysMessage("Building not found");
                p_Handler->SetSentErrorMessage(true);
            }

            int32 l_ElemEntry = 0;
            float l_X = 0;
            float l_Y = 0;
            float l_Z = 0;
            float l_O = 0;

            Creature* l_Target = p_Handler->getSelectedCreature();

            if (l_Target)
            {
                l_ElemEntry = l_Target->GetEntry();
                l_X = l_Target->GetPositionX();
                l_Y = l_Target->GetPositionY();
                l_Z = l_Target->GetPositionZ();
                l_O = l_Target->GetOrientation();
            }
            else if (p_Args != 0)
            {
                uint32 l_GuidLow = atoi(p_Args);

                if (!l_GuidLow)
                {
                    p_Handler->PSendSysMessage(LANG_COMMAND_OBJNOTFOUND, l_GuidLow);
                    p_Handler->SetSentErrorMessage(true);
                    return false;
                }

                GameObject* l_GameObject = nullptr;

                if (GameObjectData const* l_GameObjectData = sObjectMgr->GetGOData(l_GuidLow))
                    l_GameObject = p_Handler->GetObjectGlobalyWithGuidOrNearWithDbGuid(l_GuidLow, l_GameObjectData->id);

                if (!l_GameObject)
                {
                    p_Handler->PSendSysMessage(LANG_COMMAND_OBJNOTFOUND, l_GuidLow);
                    p_Handler->SetSentErrorMessage(true);
                    return false;
                }

                l_ElemEntry = -(int32)l_GameObject->GetEntry();
                l_X = l_GameObject->GetPositionX();
                l_Y = l_GameObject->GetPositionY();
                l_Z = l_GameObject->GetPositionZ();
                l_O = l_GameObject->GetOrientation();
            }
            else
            {
                p_Handler->SendSysMessage("Not creature/gameobject selected");
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            G3D::Matrix3 l_Mat = G3D::Matrix3::identity();
            l_Mat = l_Mat.fromAxisAngle(G3D::Vector3(0, 0, 1), -l_Info.O);

            G3D::Vector3 l_ElementPosition = G3D::Vector3(l_X, l_Y, 0);
            l_ElementPosition -= G3D::Vector3(l_Info.X, l_Info.Y, 0);
            l_ElementPosition = l_Mat *l_ElementPosition;
            l_ElementPosition.z = l_Z - l_Info.Z;

            if (l_ElemEntry > 0)
                l_ElementPosition.z += 0.5f;

            int32 l_TableIndex = l_Player->GetDraenorGarrison()->GetPlotType(l_Info.PlotInstanceID);
            if (l_Building.Active)
                l_TableIndex = -int32(l_Building.BuildingID);

            GarrisonPlotBuildingContent l_NewData;
            l_NewData.PlotTypeOrBuilding    = l_TableIndex;
            l_NewData.CreatureOrGob         = l_ElemEntry;
            l_NewData.FactionIndex          = l_Player->GetGarrison()->GetGarrisonFactionIndex();
            l_NewData.X                     = l_ElementPosition.x;
            l_NewData.Y                     = l_ElementPosition.y;
            l_NewData.Z                     = l_ElementPosition.z;
            l_NewData.O                     = Position::NormalizeOrientation((2 * M_PI) - Position::NormalizeOrientation(l_Info.O - l_O));

            sObjectMgr->AddGarrisonPlotBuildingContent(l_NewData);

            /// Test code
            if (true)
            {
                G3D::Vector3 l_NonRotatedPosition;

                l_Mat = G3D::Matrix3::identity();
                l_Mat = l_Mat.fromAxisAngle(G3D::Vector3(0, 0, 1), -l_Info.O);

                /// transform plot coord
                l_NonRotatedPosition = l_Mat * G3D::Vector3(l_Info.X, l_Info.Y, l_Info.Z);

                G3D::Vector3 l_Position = G3D::Vector3(l_ElementPosition.x, l_ElementPosition.y, 0);

                l_Mat = G3D::Matrix3::identity();
                l_Mat = l_Mat.fromAxisAngle(G3D::Vector3(0, 0, 1), l_Info.O);

                l_Position.x += l_NonRotatedPosition.x;
                l_Position.y += l_NonRotatedPosition.y;

                l_Position = l_Mat * l_Position;

                l_Position.z = l_ElementPosition.z + l_Info.Z;

                p_Handler->PSendSysMessage("Spawn coord %f %f %f %f", l_X, l_Y, l_Z, l_O);
                p_Handler->PSendSysMessage("Trans coord %f %f %f %f", l_Position.x, l_Position.y, l_Position.z, l_NewData.O + l_Info.O);
            }

            l_Player->GetGarrison()->OnPlayerEnter();

            return true;
        }

        static bool HandleFollowerAddCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            Player* l_TargetPlayer = p_Handler->getSelectedPlayer();

            if (!l_TargetPlayer || !l_TargetPlayer->GetGarrison())
            {
                p_Handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            if (p_Args != 0)
            {
                std::string l_Args = p_Args;

                if (l_Args == "all")
                {
                    for (uint32 l_I = 0; l_I < sGarrFollowerStore.GetNumRows(); ++l_I)
                    {
                        const GarrFollowerEntry* l_Entry = sGarrFollowerStore.LookupEntry(l_I);

                        if (l_Entry)
                            l_TargetPlayer->GetGarrison()->AddFollower(l_Entry->ID);
                    }

                    return true;
                }
                else
                {
                    uint32 l_FollowerID = atoi(p_Args);

                    if (!l_FollowerID)
                        return false;

                    return l_TargetPlayer->GetGarrison()->AddFollower(l_FollowerID);
                }
            }

            return true;
        }

        static bool HandleFolloweraddlistCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            Player* l_TargetPlayer = p_Handler->getSelectedPlayer();

            if (!l_TargetPlayer || !l_TargetPlayer->GetGarrison())
            {
                p_Handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            std::vector<uint32> l_FollowerIDs =
            {
                34,
                87,
                89,
                90,
                92,
                97,
                98,
                101,
                103,
                104,
                106,
                107,
                108,
                110,
                112,
                116,
                119,
                120,
                153,
                159,
                179,
                180,
                181,
                182,
                183,
                184,
                185,
                186,
                204,
                207,
                208,
                216,
                463,
                467,
                580,
                581,
                582
            };

            for (uint32 l_ID : l_FollowerIDs)
            {
                const GarrFollowerEntry* l_Entry = sGarrFollowerStore.LookupEntry(l_ID);

                if (l_Entry)
                    l_TargetPlayer->GetGarrison()->AddFollower(l_Entry->ID);
            }

            return true;
        }

        static bool HandleFollowerRemoveCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            Player* l_TargetPlayer = p_Handler->getSelectedPlayer();

            if (!l_TargetPlayer || !l_TargetPlayer->GetGarrison())
            {
                p_Handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            if (!p_Args)
                return false;

            uint32 l_FollowerID = atoi(p_Args);
            return l_TargetPlayer->GetGarrison()->RemoveFollower(l_FollowerID, true);
        }

        static bool HandleFollowerRerollCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            Player* l_TargetPlayer = p_Handler->getSelectedPlayer();

            if (!l_TargetPlayer || !l_TargetPlayer->GetGarrison())
            {
                p_Handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            if (!p_Args)
                return false;

            uint32 l_FollowerID = atoi(p_Args);
            l_TargetPlayer->GetGarrison()->GenerateFollowerAbilities(l_FollowerID, true, true, true, true);

            return true;
        }

        static bool HandleFollowerListCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            Player* l_TargetPlayer = p_Handler->getSelectedPlayer();

            if (!l_TargetPlayer || !l_TargetPlayer->GetGarrison())
            {
                p_Handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            uint32 l_Type  = MS::Garrison::GarrisonType::GarrisonDraenor;

            if (p_Args)
                l_Type = atoi(p_Args);

            std::list<std::string> l_List;

            for (auto& p_Follower : l_TargetPlayer->GetGarrison()->GetFollowers(static_cast<MS::Garrison::GarrisonType::Type>(l_Type)))
            {
                auto l_Entry = p_Follower.GetEntry();

                if (!l_Entry)
                    return false;

                std::stringstream l_String;
                l_String << p_Follower.DatabaseID << " " << p_Follower.FollowerID << " " << p_Follower.GetRealName(l_TargetPlayer->GetGarrison()->GetGarrisonFactionIndex());

                l_List.push_back(l_String.str());
            }

            for (auto l_Str  : l_List)
                p_Handler->SendSysMessage(l_Str.c_str());

            return true;
        }

        static bool HandleMissionAddCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            Player* l_TargetPlayer = p_Handler->getSelectedPlayer();

            if (!l_TargetPlayer || !l_TargetPlayer->GetGarrison())
            {
                p_Handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            if (p_Args != 0)
            {
                std::string l_Args = p_Args;

                if (l_Args == "all")
                {
                    for (uint32 l_I = 0; l_I < sGarrMissionStore.GetNumRows(); ++l_I)
                    {
                        const GarrMissionEntry* l_Entry = sGarrMissionStore.LookupEntry(l_I);

                        if (l_Entry)
                            l_TargetPlayer->GetGarrison()->AddMission(l_Entry->MissionRecID);
                    }

                    return true;
                }
                else
                {
                    uint32 l_MissionID = atoi(p_Args);

                    if (!l_MissionID)
                        return false;

                    return l_TargetPlayer->GetGarrison()->AddMission(l_MissionID);
                }
            }

            return false;
        }


        static bool HandleMissionDump(ChatHandler * p_hander, char const* p_Args)
        {
            if (!p_Args)
                return false;

            uint64 GUID = atoi(p_Args);
            QueryResult l_Res = CharacterDatabase.PQuery("SELECT mission_id FROM character_garrison_mission WHERE id = %i;", atoi(p_Args));

            if (!l_Res)
                return false;

            uint32 l_ID = l_Res->Fetch()[0].GetUInt32();
            std::vector<uint32>                     l_Encounters;
            std::vector<std::pair<uint32, GarrMechanicEntry const*>>  l_EncoutersMechanics;

            for (auto l_EncounterStore : sGarrMissionXEncouterStore)
            {
                if (l_EncounterStore->MissionID == l_ID)
                {
                    if (uint32 l_EncounterSetID = l_EncounterStore->GarrEncounterSetID)
                    {
                        if (auto l_EncounterList = GetGarrEncounterListBySetID(l_EncounterSetID))
                        {
                            int32 l_Index = ((0xBC8FLL * GUID) % 0x7FFFFFFFLL) % l_EncounterList->size();
                            auto l_Encounter = (*l_EncounterList)[l_Index];
                            p_hander->PSendSysMessage("Adding encounter %i due to GarrEncounterSetXEncounter", l_Encounter);
                            l_Encounters.push_back(l_Encounter);

                        }
                    }

                    if (uint32 l_EncoutnerID = l_EncounterStore->EncounterID)
                    {
                        p_hander->PSendSysMessage("Adding encounter %i due to GarrMissionXEncouter", l_EncoutnerID);
                        l_Encounters.push_back(l_EncoutnerID);
                    }
                }
            }

            uint32 l_OldGUID = GUID;
            for (uint32 l_I = 0; l_I < l_Encounters.size(); l_I++)
            {
                for (auto l_Entry : sGarrEncouterXMechanicStore)
                {
                    if (l_Entry->EncounterID == l_Encounters[l_I])
                    {
                        if (uint32 l_MechanicSetID = l_Entry->GarrMechanicSetID)
                        {
                            if (auto l_MechanicList = GetGarrMechanicListBySetID(l_MechanicSetID))
                            {
                                int l_X = 0;

                                for (auto l_Entryy : *l_MechanicList)
                                    p_hander->PSendSysMessage("%i(%i) - %i - Ability %i Float %f", l_Entryy->ID, l_Entryy->MechanicTypeID, l_X++, l_Entryy->AbilityID, l_Entryy->Factor);

                                l_OldGUID = ((0xBC8FLL * l_OldGUID) % 0x7FFFFFFFLL);
                                uint32 l_OldIndex =  l_OldGUID % l_MechanicList->size();
                                auto l_Mechanic = (*l_MechanicList)[l_OldIndex];
                                p_hander->PSendSysMessage("Adding mechanic %i(%i)(%i, %i, %i) due to GarrMechanicSetXMechanic", l_Mechanic->ID, l_Mechanic->MechanicTypeID, static_cast<uint32>(GUID), l_OldIndex, static_cast<uint32>(l_MechanicList->size()));
                                l_EncoutersMechanics.push_back(std::make_pair(l_Encounters[l_I], l_Mechanic));

                            }
                        }

                        if (uint32 l_MechanicID = l_Entry->MechanicID)
                        {
                            l_EncoutersMechanics.push_back(std::make_pair(l_Encounters[l_I], sGarrMechanicStore[l_MechanicID]));
                            p_hander->PSendSysMessage("Adding mechanic %i(%i) due to GarrEncouterXMechanic", l_MechanicID, sGarrMechanicStore[l_MechanicID]->MechanicTypeID);
                        }
                    }
                }
            }
            return true;
        }

        static bool HandleMissionResetDaily(ChatHandler* p_Handler, char const* /*p_Args*/)
        {
            Player* l_TargetPlayer = p_Handler->getSelectedPlayer();

            if (!l_TargetPlayer || !l_TargetPlayer->GetGarrison())
            {
                p_Handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            if (auto l_Garrison = l_TargetPlayer->GetGarrison())
                l_Garrison->ResetMissionStartedTodayCount();

            return true;
        }

        static bool HandleMissionCompleteAllCommand(ChatHandler* p_Handler, char const* /*p_Args*/)
        {
            Player* l_TargetPlayer = p_Handler->getSelectedPlayer();

            if (!l_TargetPlayer || !l_TargetPlayer->GetGarrison())
            {
                p_Handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            l_TargetPlayer->GetGarrison()->SetAllInProgressMissionAsComplete();

            return true;
        }

        static bool HandleBuildingCompleteCommand(ChatHandler* p_Handler, char const* /*p_Args*/)
        {
            Player* l_TargetPlayer = p_Handler->getSelectedPlayer();

            if (!l_TargetPlayer || !l_TargetPlayer->GetDraenorGarrison())
            {
                p_Handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            if (auto  l_Garr = l_TargetPlayer->GetDraenorGarrison())
            {
                MS::Garrison::GarrisonPlotInstanceInfoLocation l_Plot = l_Garr->GetPlot(l_TargetPlayer->m_positionX, l_TargetPlayer->m_positionY, l_TargetPlayer->m_positionZ);

                if (l_Plot.PlotInstanceID)
                {
                    if (!l_Garr->PlotIsFree(l_Plot.PlotInstanceID) && !l_Garr->GetBuilding(l_Plot.PlotInstanceID).Active)
                        l_Garr->ActivateBuilding(l_Plot.PlotInstanceID);
                }
            }

            return true;
        }

        static bool HandleShipmentCompleteCommand(ChatHandler* p_Handler, char const* /*p_Args*/)
        {
            Player* l_TargetPlayer = p_Handler->getSelectedPlayer();

            if (!l_TargetPlayer || !l_TargetPlayer->GetGarrison())
            {
                p_Handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            if (MS::Garrison::Manager* l_Manager = l_TargetPlayer->GetGarrison())
            {
                uint32 l_CurrentTimeStamp = time(nullptr);
                for (MS::Garrison::GarrisonWorkOrder& l_WorkOrder : l_Manager->GetWorkOrders())
                    l_WorkOrder.CompleteTime = l_CurrentTimeStamp + 5;
            }

            if (auto l_Garr = l_TargetPlayer->GetDraenorGarrison())
            {
                MS::Garrison::GarrisonPlotInstanceInfoLocation l_Plot = l_Garr->GetPlot(l_TargetPlayer->m_positionX, l_TargetPlayer->m_positionY, l_TargetPlayer->m_positionZ);

                if (l_Plot.PlotInstanceID)
                {
                    std::vector<MS::Garrison::GarrisonWorkOrder>& l_PlotWorkOrder = l_TargetPlayer->GetGarrison()->GetWorkOrders();

                    if (l_PlotWorkOrder.size() > 0)
                    {
                        uint32 l_CurrentTimeStamp = time(0);

                        for (uint32 l_OrderI = 0; l_OrderI < l_PlotWorkOrder.size(); ++l_OrderI)
                            l_PlotWorkOrder[l_OrderI].CompleteTime = l_CurrentTimeStamp;
                    }
                }
            }
            return true;
        }

        static bool HandleShipyardOpen(ChatHandler* p_Handler, char const* /*p_Args*/)
        {
            Player* l_TargetPlayer = p_Handler->getSelectedPlayer();

            if (!l_TargetPlayer || !l_TargetPlayer->GetGarrison())
            {
                p_Handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            return true;
        }
};

#ifndef __clang_analyzer__
void AddSC_garrison_commandscript()
{
    new garrison_commandscript();
}
#endif
#endif
