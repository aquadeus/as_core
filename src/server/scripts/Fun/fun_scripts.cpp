// Serves as a placeholder for other branches so we dont have to recmake every time

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "GameObjectAI.h"
#include "BattlegroundMgr.hpp"
#include "BattlegroundPacketFactory.hpp"
#include "DisableMgr.h"
#include "Group.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "ScriptedGossip.h"
#include "Chat.h"
#ifndef CROSS
#include "GarrisonMgr.hpp"
#endif /* not CROSS */

#define DUEL_AREA_RANGE 200.0f

enum Gameobjects
{
    GAMEOBJECT_DUEL_AREA_OBJECT = 900000
};

/// Quick Recovery - 176253
class spell_fun_quick_recovery : public SpellScriptLoader
{
    public:
        spell_fun_quick_recovery() : SpellScriptLoader("spell_fun_quick_recovery") { }

        class spell_fun_quick_recovery_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_fun_quick_recovery_AuraScript);

            bool Load()
            {
                m_Timer = 1000;
                return true;
            }

            void OnUpdate(uint32 p_Diff, AuraEffect* /* p_AurEff */)
            {
                Unit* l_Owner = GetUnitOwner();

                if (!l_Owner)
                    return;

                if (m_Timer <= p_Diff)
                {
                    m_Timer = 1000;

                    if (!l_Owner->FindNearestGameObject(GAMEOBJECT_DUEL_AREA_OBJECT, DUEL_AREA_RANGE))
                    {
                        Remove();

                        if (l_Owner->HasByteFlag(UNIT_FIELD_SHAPESHIFT_FORM, UNIT_BYTES_2_OFFSET_PVP_FLAG, UNIT_BYTE2_FLAG_SANCTUARY))
                            l_Owner->RemoveByteFlag(UNIT_FIELD_SHAPESHIFT_FORM, UNIT_BYTES_2_OFFSET_PVP_FLAG, UNIT_BYTE2_FLAG_SANCTUARY);

                        return;
                    }
                    else
                        if (!l_Owner->HasByteFlag(UNIT_FIELD_SHAPESHIFT_FORM, UNIT_BYTES_2_OFFSET_PVP_FLAG, UNIT_BYTE2_FLAG_SANCTUARY))
                            l_Owner->SetByteFlag(UNIT_FIELD_SHAPESHIFT_FORM, UNIT_BYTES_2_OFFSET_PVP_FLAG, UNIT_BYTE2_FLAG_SANCTUARY);
                }
                else
                    m_Timer -= p_Diff;
            }

            void OnRemove(AuraEffect const* /* p_AurEff */, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Owner = GetUnitOwner();

                if (!l_Owner)
                    return;

                if (l_Owner->HasByteFlag(UNIT_FIELD_SHAPESHIFT_FORM, UNIT_BYTES_2_OFFSET_PVP_FLAG, UNIT_BYTE2_FLAG_SANCTUARY))
                    l_Owner->RemoveByteFlag(UNIT_FIELD_SHAPESHIFT_FORM, UNIT_BYTES_2_OFFSET_PVP_FLAG, UNIT_BYTE2_FLAG_SANCTUARY);
            }

            uint32 m_Timer;

            void Register()
            {
                OnEffectUpdate += AuraEffectUpdateFn(spell_fun_quick_recovery_AuraScript::OnUpdate, EFFECT_0, SPELL_AURA_RESET_COOLDOWNS_BEFORE_DUEL);
                OnEffectRemove += AuraEffectRemoveFn(spell_fun_quick_recovery_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_RESET_COOLDOWNS_BEFORE_DUEL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_fun_quick_recovery_AuraScript();
        }
};

class go_duel_area_flag : public GameObjectScript
{
    enum Spells
    {
        SPELL_FUN_QUICK_RECOVERY = 176253
    };

    public:
        go_duel_area_flag() : GameObjectScript("go_duel_area_flag") { }

        struct go_duel_area_flagAI : public GameObjectAI
        {
            go_duel_area_flagAI(GameObject* p_GameObject) : GameObjectAI(p_GameObject)
            {
                m_Timer = 1000;
            }

            void UpdateAI(uint32 p_Diff) override
            {
                if (m_Timer <= p_Diff)
                {
                    m_Timer = 1000;

                    std::list<Player*> l_Players;
                    JadeCore::AnyPlayerInObjectRangeCheck l_Checker(go, DUEL_AREA_RANGE - 10.0f);
                    JadeCore::PlayerListSearcher<JadeCore::AnyPlayerInObjectRangeCheck> l_Searcher(go, l_Players, l_Checker);
                    go->VisitNearbyWorldObject(DUEL_AREA_RANGE - 10.0f, l_Searcher); /// Prevent reapply spam with the -10.0f

                    for (auto l_Iter : l_Players)
                        if (!l_Iter->HasAura(SPELL_FUN_QUICK_RECOVERY))
                            go->CastSpell(l_Iter, SPELL_FUN_QUICK_RECOVERY);
                }
                else
                    m_Timer -= p_Diff;
            }

            uint32 m_Timer;
        };

        GameObjectAI* GetAI(GameObject* p_GameObject) const override
        {
            return new go_duel_area_flagAI(p_GameObject);
        }
};

class npc_fun_teleporter : public CreatureScript
{
    public:
        npc_fun_teleporter() : CreatureScript("npc_fun_teleporter") {}

        enum eActions
        {
            TeleportDalaran = GOSSIP_ACTION_INFO_DEF + 1,
            TeleportDuelZone,
            TeleportStranglehornArena,
            VendorHub,
            TeleportAshran,
            TeleportPandaria,
            TeleportShattrath,
            TeleportOldDalaran,
            TeleportGurubashiTower,
        };

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            p_Player->PlayerTalkClass->ClearGossipMenu();
            p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Stating Hub (vendors)",              GOSSIP_SENDER_MAIN, eActions::VendorHub);
            p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Faction Duel Zone (Main capital)",   GOSSIP_SENDER_MAIN, eActions::TeleportDuelZone);
            p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Gurubashi Arena (FFA)",              GOSSIP_SENDER_MAIN, eActions::TeleportStranglehornArena);
            p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Dalaran",                            GOSSIP_SENDER_MAIN, eActions::TeleportDalaran);
            p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Ashran",                             GOSSIP_SENDER_MAIN, eActions::TeleportAshran);
            p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Pandaria",                           GOSSIP_SENDER_MAIN, eActions::TeleportPandaria);
            p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Shattrath",                          GOSSIP_SENDER_MAIN, eActions::TeleportShattrath);
            p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Old Dalaran",                        GOSSIP_SENDER_MAIN, eActions::TeleportOldDalaran);
            p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Gurubashi Tower",                    GOSSIP_SENDER_MAIN, eActions::TeleportGurubashiTower);
            p_Player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, p_Creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* p_Player, Creature* p_Creature, uint32 p_Sender, uint32 p_Action) override
        {
            p_Player->PlayerTalkClass->ClearMenus();
            p_Player->PlayerTalkClass->SendCloseGossip();

            if (p_Sender == GOSSIP_SENDER_MAIN)
            {
                WorldLocation l_TeleportDestination;
                switch (p_Action)
                {
                    case npc_fun_teleporter::TeleportDalaran:
                        l_TeleportDestination = p_Player->GetTeamId() == TeamId::TEAM_HORDE ? WorldLocation(1220, -731.21f, 4359.23f, 737.61f, 2.38f) : WorldLocation(1220, -934.98f, 4578.56f, 729.27f, 5.61f);
                        break;
                    case npc_fun_teleporter::TeleportDuelZone:
                        l_TeleportDestination = p_Player->GetTeamId() == TeamId::TEAM_HORDE ? WorldLocation(1, 1116.13f, -4291.92f, 20.29f, 3.14f) : WorldLocation(0, -9084.8f, 389.44f, 93.01f, 2.59f);
                        break;
                    case npc_fun_teleporter::TeleportStranglehornArena:
                        l_TeleportDestination = p_Player->GetTeamId() == TeamId::TEAM_HORDE ? WorldLocation(0, -13161.73f, 245.78f, 33.24f, 2.67f) : WorldLocation(0, -13252.59f, 292.31f, 33.24f, 5.90f);
                        break;
                    case npc_fun_teleporter::VendorHub:
                        l_TeleportDestination = p_Player->GetTeamId() == TeamId::TEAM_HORDE ? WorldLocation(1, 1640.66f, -4358.37f, 26.75f, 0.498f) : WorldLocation(0, -8383.959f, 251.794f, 155.34f, 0.66f);
                        break;
                    case npc_fun_teleporter::TeleportAshran:
                        l_TeleportDestination = p_Player->GetTeamId() == TeamId::TEAM_HORDE ? WorldLocation(1116, 5355.68f, -3945.28f, 32.73f, 3.85f) : WorldLocation(1116, 3684.72f, -3841.75f, 44.98f, 4.56f);
                        break;
                    case npc_fun_teleporter::TeleportPandaria:
                        l_TeleportDestination = p_Player->GetTeamId() == TeamId::TEAM_HORDE ? WorldLocation(870, 2141.76f, 2620.94f, 543.28f, 4.44f) : WorldLocation(870, 200.61f, 2203.08f, 272.56f, 5.88f);
                        break;
                    case npc_fun_teleporter::TeleportShattrath:
                        l_TeleportDestination = WorldLocation(530, -1855.93f, 5424.28f, -10.46f, 2.52f);
                        break;
                    case npc_fun_teleporter::TeleportOldDalaran:
                        l_TeleportDestination = WorldLocation(571, 5804.61f, 624.79f, 647.76f, 1.63f);
                        break;
                    case npc_fun_teleporter::TeleportGurubashiTower:
                        l_TeleportDestination = WorldLocation(0, -13304.59f, 479.92f, -5.24f, 0.12f);
                        break;
                    default:
                        return true;
                        break;
                }

                p_Player->TeleportTo(l_TeleportDestination);
            }
            return true;
        }
};

class go_artifact_chest : public GameObjectScript
{
    public:
        go_artifact_chest() : GameObjectScript("go_artifact_chest") { }

        struct go_artifact_chestAI : public GameObjectAI
        {
            go_artifact_chestAI(GameObject* p_GameObject) : GameObjectAI(p_GameObject)
            {
            }

            bool GossipHello(Player* p_Player) override
            {
                static std::map<uint8 /*Class*/, std::pair<uint32 /*SpellId*/, uint32 /*Second Legendary Talent ID*/>> k_ArtifactChoiceSpells =
                {
                    {Classes::CLASS_WARRIOR,         std::make_pair(197999, 412)},
                    {Classes::CLASS_PALADIN,         std::make_pair(197124, 401)},
                    {Classes::CLASS_HUNTER,          std::make_pair(198430, 379)},
                    {Classes::CLASS_ROGUE,           std::make_pair(201343, 445)},
                    {Classes::CLASS_PRIEST,          std::make_pair(199700, 456)},
                    {Classes::CLASS_DEATH_KNIGHT,    std::make_pair(199985, 434)},
                    {Classes::CLASS_SHAMAN,          std::make_pair(205790, 42)},
                    {Classes::CLASS_MAGE,            std::make_pair(203654, 390)},
                    {Classes::CLASS_WARLOCK,         std::make_pair(199411, 368)},
                    {Classes::CLASS_MONK,            std::make_pair(198902, 258)},
                    {Classes::CLASS_DRUID,           std::make_pair(199653, 357)},
                    {Classes::CLASS_DEMON_HUNTER,    std::make_pair(201092, 423)}
                };

                p_Player->CastSpell(p_Player, k_ArtifactChoiceSpells[p_Player->getClass()].first, true);

#ifndef CROSS
                if (p_Player->GetGarrison())
                {
                    if (GarrTalentEntry const* l_Entry = sGarrTalentStore.LookupEntry(k_ArtifactChoiceSpells[p_Player->getClass()].second))
                        p_Player->GetGarrison()->AddTalent(l_Entry, false, false);
                }
#endif

                return true;
            }
        };

        GameObjectAI* GetAI(GameObject* p_GameObject) const override
        {
            return new go_artifact_chestAI(p_GameObject);
        }
};

class npc_queue_1v1 : public CreatureScript
{
    public:
        npc_queue_1v1() : CreatureScript("npc_queue_1v1") {}

        enum eActions
        {
            Queue1v1Skirmish = GOSSIP_ACTION_INFO_DEF + 1,
            Queue1v1Rated,
        };

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            p_Player->PlayerTalkClass->ClearGossipMenu();
            std::ostringstream l_RatedText;
            l_RatedText << "Queue for 1v1 rated arena            [Personal Rating : " << p_Player->GetArenaPersonalRating((uint8)BattlegroundBracketType::Arena1v1);
            l_RatedText << " | " << p_Player->GetSeasonWins((uint8)BattlegroundBracketType::Arena1v1) << "  win / " << p_Player->GetSeasonGames((uint8)BattlegroundBracketType::Arena1v1) - p_Player->GetSeasonWins((uint8)BattlegroundBracketType::Arena1v1) << " loose]";

            p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Queue for 1v1 skirmish arena", GOSSIP_SENDER_MAIN, eActions::Queue1v1Skirmish);

            if (sWorld->getBoolConfig(CONFIG_ARENA_SEASON_IN_PROGRESS))
                p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, l_RatedText.str().c_str(), GOSSIP_SENDER_MAIN, eActions::Queue1v1Rated);

            p_Player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, p_Creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* p_Player, Creature* p_Creature, uint32 p_Sender, uint32 p_Action) override
        {
            p_Player->PlayerTalkClass->ClearMenus();
            p_Player->PlayerTalkClass->SendCloseGossip();

#ifndef CROSS
            if (p_Player->GetRoleBySpecializationId(p_Player->GetActiveSpecializationID()) != Roles::ROLE_DAMAGE)
            {
                ChatHandler(p_Player).PSendSysMessage("Healers and tanks can't queue in 1v1, you need to change your specialization.");
                return true;
            }

            ArenaType l_ArenaType = ArenaType::Arena1v1;

            bool l_Skirmish = p_Action == eActions::Queue1v1Skirmish;

            Battleground* l_BG = sBattlegroundMgr->GetBattlegroundTemplate(MS::Battlegrounds::BattlegroundType::AllArenas);
            if (!l_BG)
            {
                ChatHandler(p_Player).PSendSysMessage("Battleground: template BG for All Arenas not found.");
                return true;
            }

            if (DisableMgr::IsDisabledFor(DISABLE_TYPE_BATTLEGROUND, BATTLEGROUND_AA, NULL))
            {
                ChatHandler(p_Player).PSendSysMessage(LANG_ARENA_DISABLED);
                return true;
            }

            Player* l_Player = p_Player;

            uint32 l_ArenaRating = l_Skirmish ? 0 : l_Player->GetArenaPersonalRating((uint8)GetBracketTypeByArenaType(l_ArenaType));
            uint32 l_MatchmakerRating = l_Skirmish ? 0 : l_Player->GetArenaMatchMakerRating((uint8)GetBracketTypeByArenaType(l_ArenaType));

            // check Deserter debuff
            if (!l_Player->CanJoinToBattleground())
            {
                WorldPacket data;
                MS::Battlegrounds::PacketFactory::StatusFailed(&data, l_BG, l_Player, 0, ERR_GROUP_JOIN_BATTLEGROUND_DESERTERS);
                l_Player->GetSession()->SendPacket(&data);
                return true;
            }

            MS::Battlegrounds::BattlegroundType::Type l_BGQueueTypeIDRandom = MS::Battlegrounds::GetTypeFromId(BATTLEGROUND_RB, 0);
            MS::Battlegrounds::BattlegroundType::Type l_BGQueueTypeID = MS::Battlegrounds::GetTypeFromId(BATTLEGROUND_AA, l_ArenaType, l_Skirmish);

            MS::Battlegrounds::Bracket const* l_BracketEntry = MS::Battlegrounds::Brackets::FindForLevel(l_Player->getLevel());
            if (!l_BracketEntry)
            {
                ChatHandler(p_Player).PSendSysMessage("Invalid syntax.");
                return true;
            }

            if (l_Player->GetBattlegroundQueueIndex(l_BGQueueTypeIDRandom) < PLAYER_MAX_BATTLEGROUND_QUEUES)
            {
                /// Player is already in random queue.
                WorldPacket data;
                MS::Battlegrounds::PacketFactory::StatusFailed(&data, l_BG, l_Player, 0, ERR_IN_RANDOM_BG);
                l_Player->GetSession()->SendPacket(&data);
                return true;
            }

            // check if already in queue
            if (l_Player->GetBattlegroundQueueIndex(l_BGQueueTypeID) < PLAYER_MAX_BATTLEGROUND_QUEUES)
            {
                //player is already in this queue
                WorldPacket data;
                MS::Battlegrounds::PacketFactory::StatusFailed(&data, l_BG, l_Player, 0, ERR_BATTLEGROUND_TOO_MANY_QUEUES);
                l_Player->GetSession()->SendPacket(&data);
                return true;
            }

            // check if has free queue slots
            if (!l_Player->HasFreeBattlegroundQueueId())
            {
                WorldPacket data;
                MS::Battlegrounds::PacketFactory::StatusFailed(&data, l_BG, l_Player, 0, ERR_BATTLEGROUND_TOO_MANY_QUEUES);
                l_Player->GetSession()->SendPacket(&data);
                return true;
            }

            bool l_InterRealmEnable = sWorld->getBoolConfig(CONFIG_INTERREALM_ENABLE);
            InterRealmSession* l_Tunnel = nullptr;

            if (l_InterRealmEnable)
            {
                l_Tunnel = sWorld->GetInterRealmSession();
                if (!l_Tunnel || !l_Tunnel->IsTunnelOpened())
                {
                    ChatHandler(p_Player).PSendSysMessage(LANG_INTERREALM_DISABLED);
                    return true;
                }
            }

            /// Handle queue registration on local realm, cross realm battlegroup is disabled
            if (!l_InterRealmEnable)
            {
                MS::Battlegrounds::BattlegroundScheduler& l_Scheduler = sBattlegroundMgr->GetScheduler();
                MS::Battlegrounds::BattlegroundInvitationsMgr& l_InvitationsMgr = sBattlegroundMgr->GetInvitationsMgr();

                GroupQueueInfo* l_GInfo = l_Scheduler.AddGroup(l_Player, nullptr, l_BGQueueTypeID, nullptr, l_BracketEntry, l_ArenaType, false, l_ArenaRating, l_MatchmakerRating, l_Skirmish);
                uint32 l_AverageTime = l_InvitationsMgr.GetAverageQueueWaitTime(l_GInfo, l_BracketEntry->m_Id);

                WorldPacket l_Data;

                // add to queue
                uint32 l_QueueSlot = l_Player->AddBattlegroundQueueId(l_BGQueueTypeID);

                l_Player->AddBattlegroundQueueJoinTime(l_BGQueueTypeID, l_GInfo->m_JoinTime);

                // send status packet (in queue)
                MS::Battlegrounds::PacketFactory::Status(&l_Data, l_BG, l_Player, l_QueueSlot, STATUS_WAIT_QUEUE, l_AverageTime, l_GInfo->m_JoinTime, l_ArenaType, l_Skirmish);
                l_Player->GetSession()->SendPacket(&l_Data);

                return true;
            }

            /// Handle queue registration on cross realm
            l_Tunnel->SendRegisterArena({ l_Player }, l_ArenaType, l_Skirmish);
#endif
            return true;
        }
};

class fun_enable_second_lego : public PlayerScript
{
    public:
        fun_enable_second_lego() : PlayerScript("fun_enable_second_lego") { }

        void OnLogin(Player* p_Player) override
        {
            if (!sWorld->getBoolConfig(CONFIG_TOURNAMENT_ENABLE))
                return;

            static std::map<uint8 /*Class*/, uint32 /*Second Legendary Talent ID*/> m_ClassTalents =
            {
                { Classes::CLASS_WARRIOR,         412 },
                { Classes::CLASS_PALADIN,         401 },
                { Classes::CLASS_HUNTER,          379 },
                { Classes::CLASS_ROGUE,           445 },
                { Classes::CLASS_PRIEST,          456 },
                { Classes::CLASS_DEATH_KNIGHT,    434 },
                { Classes::CLASS_SHAMAN,          42  },
                { Classes::CLASS_MAGE,            390 },
                { Classes::CLASS_WARLOCK,         368 },
                { Classes::CLASS_MONK,            258 },
                { Classes::CLASS_DRUID,           357 },
                { Classes::CLASS_DEMON_HUNTER,    423 }
            };

#ifndef CROSS
            if (p_Player->GetGarrison())
            {
                if (p_Player->GetGarrison()->IsResearchingTalent())
                    return;

                if (GarrTalentEntry const* l_Entry = sGarrTalentStore.LookupEntry(m_ClassTalents[p_Player->getClass()]))
                    p_Player->GetGarrison()->AddTalent(l_Entry, false, false);
            }
#endif
        }
};

class fun_pvp_quest : public PlayerScript
{
    public:

        enum eArea
        {
            GurubashiBattleRing = 2177
        };

        enum eKillCredit
        {
            GurubashiPlayerSlay = 1900000,
            WorldPvPPlayerSlay  = 1900001,
        };

        enum eAura
        {
            RessurectionSikness = 15007
        };

        fun_pvp_quest() : PlayerScript("fun_pvp_quest")
        {
        }

        void OnPVPKill(Player* p_Killer, Player* p_Killed) override
        {
            if (!sWorld->getBoolConfig(CONFIG_TOURNAMENT_ENABLE))
                return;

            if (p_Killer == p_Killed)
                return;

            if (p_Killer->GetAreaId() == eArea::GurubashiBattleRing && p_Killed->GetAreaId() == eArea::GurubashiBattleRing && !p_Killed->HasAura(eAura::RessurectionSikness))
                p_Killer->KilledMonsterCredit(eKillCredit::GurubashiPlayerSlay);

            if (p_Killed->getLevel() == 110 && p_Killed->GetTeamId() != p_Killer->GetTeamId() && p_Killed->GetMap()->GetEntry()->IsContinent() && !p_Killed->HasAura(eAura::RessurectionSikness))
            {
                p_Killer->KilledMonsterCredit(eKillCredit::WorldPvPPlayerSlay);

                if (GroupPtr l_Group = p_Killer->GetGroup())
                {
                    l_Group->GetMemberSlots().foreach([&](Group::MemberSlotPtr l_Itr) -> void
                    {
                        Player* l_Member = l_Itr->player;
                        if (!l_Member || l_Member == p_Killed)
                            return;

                        if (l_Member->IsAtGroupRewardDistance(p_Killed))
                            l_Member->KilledMonsterCredit(eKillCredit::WorldPvPPlayerSlay);
                    });
                }
            }
        }
};

class capital_no_fly : public PlayerScript
{
    public:

        enum eArea
        {
            Orgrimmar     = 1637,
            Stormwind     = 1519,
            Stranglethorn = 5287
        };

        enum eSpell
        {
            SpellRestrictedFlightArea = 58600
        };

        capital_no_fly() : PlayerScript("capital_no_fly") { }

        void OnUpdateZone(Player* p_Player, uint32 p_NewZoneID, uint32 /*p_OldZoneID*/, uint32 /*p_NewAreaID*/) override
        {
            if (!sWorld->getBoolConfig(CONFIG_TOURNAMENT_ENABLE))
                return;

            switch (p_NewZoneID)
            {
                case eArea::Orgrimmar:
                case eArea::Stormwind:
                {
                    /// Don't refresh aura at each update
                    if (p_Player->HasAura(eSpell::SpellRestrictedFlightArea))
                        break;

                    if (p_Player->HasAuraType(SPELL_AURA_FLY) || p_Player->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED))
                        p_Player->CastSpell(p_Player, eSpell::SpellRestrictedFlightArea);

                    break;
                }
                case eArea::Stranglethorn:
                {
                    /// Don't refresh aura at each update
                    if (p_Player->HasAura(eSpell::SpellRestrictedFlightArea))
                        break;

                    if (p_Player->GetAreaId() == 2177 || p_Player->GetAreaId() == 1741)
                    {
                        if (p_Player->HasAuraType(SPELL_AURA_FLY) || p_Player->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED))
                            p_Player->CastSpell(p_Player, eSpell::SpellRestrictedFlightArea);

                        if (p_Player->GetGroup())
                            p_Player->RemoveFromGroup();
                    }

                    break;
                }
                default:
                    break;
            }
        }

        /// Don't allow player to have group in Gurubashi Arena
        void OnUpdate(Player* p_Player, uint32 p_Diff) override
        {
            if (p_Player->GetAreaId() == 2177) ///< Gurubashi
            {
                if (p_Player->GetGroup())
                    p_Player->RemoveFromGroup();
            }
        }
};

/// Arena Treasure Chest - 179697
class go_fun_arena_treasure_chest : public GameObjectScript
{
    public:
        go_fun_arena_treasure_chest() : GameObjectScript("go_fun_arena_treasure_chest") { }

        struct go_fun_arena_treasure_chestAI : public GameObjectAI
        {
            go_fun_arena_treasure_chestAI(GameObject* p_GameObject) : GameObjectAI(p_GameObject) { }

            bool OnCheckOpening(Unit* p_Who, SpellInfo const* p_SpellInfo) override
            {
                if (!p_Who || !p_Who->IsPlayer())
                    return false;

                /// Prevent chest opening while affected by any immunity aura
                if (p_Who->HasAuraType(AuraType::SPELL_AURA_SCHOOL_IMMUNITY) || p_Who->HasAuraType(AuraType::SPELL_AURA_DEFLECT_SPELLS))
                    return false;

                if (p_Who->HasAuraType(AuraType::SPELL_AURA_DAMAGE_IMMUNITY))
                    return false;

                /// Prevent chest opening without FFA flag enabled
                if (!p_Who->HasByteFlag(UNIT_FIELD_BYTES_2, UNIT_BYTES_2_OFFSET_PVP_FLAG, UNIT_BYTE2_FLAG_FFA_PVP))
                    return false;

                p_Who->RemoveAurasByType(AuraType::SPELL_AURA_MOD_STEALTH);
                p_Who->RemoveAurasByType(AuraType::SPELL_AURA_MOD_CAMOUFLAGE);
                return true;
            }

            void OnLockOpened(Player* p_Player) override
            {
                if (go->GetGoState() == GOState::GO_STATE_ACTIVE)
                    return;

                go->SetLootState(LootState::GO_ACTIVATED);
                go->SetGoState(GOState::GO_STATE_ACTIVE);

                bool l_Horde = p_Player->GetBGTeam() == Team::HORDE;

                /// Construct loot for opener
                Loot* l_Loot = &go->m_LootContainers[p_Player->GetGUID()];

                l_Loot->clear();
                l_Loot->Context              = go->GetMap()->GetLootItemContext(p_Player);
                l_Loot->SpawnMode            = go->GetMap()->GetSpawnMode();
                l_Loot->m_PersonalLootFilter = false;
                l_Loot->SetSource(go->GetGUID());

                std::vector<uint32> l_Items;

                /// 20% chance to have a "big" chest
                if (roll_chance_i(20))
                {
                    l_Loot->Gold = urand(500 * MoneyConstants::GOLD, 1000 * MoneyConstants::GOLD);

                    /// Add a PvP lootbox + 100k AP (with 25 AK)
                    l_Items = { 300000, 138881 };

                    /// Add a random count of classic PvP box
                    uint8 l_BoxCount = urand(1, 5);

                    for (uint8 l_I = 0; l_I < l_BoxCount; ++l_I)
                        l_Items.push_back(l_Horde ? 138469 : 138474);
                }
                /// Otherwise, it's just a classic one
                else
                {
                    l_Loot->Gold = urand(50 * MoneyConstants::GOLD, 100 * MoneyConstants::GOLD);

                    /// Add 25k AP (with 25 AK)
                    l_Items = { 143713 };

                    /// Add a random count of classic PvP box
                    uint8 l_BoxCount = urand(1, 3);

                    for (uint8 l_I = 0; l_I < l_BoxCount; ++l_I)
                        l_Items.push_back(l_Horde ? 138469 : 138474);
                }

                for (uint32 l_Item : l_Items)
                {
                    LootStoreItem l_StoreItem = LootStoreItem(l_Item,                               ///< ItemID (or CurrencyID)
                                                              LootItemType::LOOT_ITEM_TYPE_ITEM,    ///< LootType
                                                              100.0f,                               ///< Chance or quest chance
                                                              LootModes::LOOT_MODE_DEFAULT,         ///< LootMode
                                                              LootModes::LOOT_MODE_DEFAULT,         ///< Group
                                                              1,                                    ///< MinCount (or Ref)
                                                              1,                                    ///< MaxCount
                                                              std::vector<uint32>(),                ///< ItemBonuses
                                                              0,                                    ///< Difficulty Mask
                                                              0,                                    ///< RequiredAura
                                                              0);                                   ///< RequiredClass

                    l_Loot->Items.push_back(LootItem(l_StoreItem, ItemContext::None, l_Loot, p_Player));
                }

                p_Player->SendLoot(go->GetGUID(), LootType::LOOT_CORPSE);
            }

            void OnLootReleased(Player* /*p_Player*/) override
            {
                if (go->GetGoState() == GOState::GO_STATE_READY)
                    return;

                go->SetGoState(GOState::GO_STATE_READY);
                go->SetRespawnTime(30 * TimeConstants::MINUTE);
                go->Delete();
            }
        };

        GameObjectAI* GetAI(GameObject* p_GameObject) const override
        {
            return new go_fun_arena_treasure_chestAI(p_GameObject);
        }
};

class greymane_artifact_first_connexion : public PlayerScript
{
    public:
        greymane_artifact_first_connexion() : PlayerScript("greymane_artifact_first_connexion") { }

        void OnLogin(Player* p_Player)
        {
            if (!sWorld->getBoolConfig(WorldBoolConfigs::CONFIG_TOURNAMENT_ENABLE)) ///< Only on Greymane
                return;

            if (p_Player->GetTotalPlayedTime() > 1 * TimeConstants::HOUR)
                return;

            uint32 l_Amount = 4000000000; ///< Artifact level 54

            if (MS::Artifact::Manager* l_Artifact = p_Player->GetCurrentlyEquippedArtifact())
            {
                if (l_Artifact->GetLevel() < 54 && l_Artifact->GetArtifactPower() <= l_Amount)
                    l_Artifact->SetArtifactPower(l_Amount);
            }
        }
};

#ifndef __clang_analyzer__
void AddSC_FunScripts()
{
    new spell_fun_quick_recovery();
    /// new go_duel_area_flag();
    new go_artifact_chest();
    new npc_fun_teleporter();
    new npc_queue_1v1();
    new fun_pvp_quest();
    new fun_enable_second_lego();
    new capital_no_fly();
    /// new go_fun_arena_treasure_chest();
    new greymane_artifact_first_connexion();
}
#endif
