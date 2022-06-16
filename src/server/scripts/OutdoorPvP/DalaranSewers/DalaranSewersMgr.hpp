////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef DALARAN_SEWERS_MGR_HPP_ASHRAN
# define DALARAN_SEWERS_MGR_HPP_ASHRAN

# include "OutdoorPvP.h"
# include "OutdoorPvPMgr.h"
# include "DalaranSewersDatas.hpp"
# include "ScriptMgr.h"
# include "ScriptedCreature.h"
# include "ScriptedGossip.h"
# include "ScriptedEscortAI.h"
# include "ScriptedCosmeticAI.hpp"
# include "GameObjectAI.h"
# include "Player.h"
# include "WorldPacket.h"
# include "World.h"
# include "ObjectMgr.h"
# include "Language.h"
# include "ScriptedCosmeticAI.hpp"
# include "CreatureTextMgr.h"
# include "MoveSplineInit.h"
# include "LFGMgr.h"
# include "Group.h"
# include "ScriptPCH.h"
# include "MapManager.h"
# include "Guid.h"

class OutdoorPvPDalaran;

class OutdoorPvPDalaran : public OutdoorPvP
{
    public:
        OutdoorPvPDalaran();

        bool SetupOutdoorPvP() override;
        bool GetFFAState() { return m_ActivatedFFA; }
        bool CanSwitchFFA() { return m_CanSwitchFFAState; }

        uint32 GetRemainingSwitchDelay() const { return m_PhaseSwitchingTimer; }
        void SwitchFFAState(bool p_State);

        bool HasPlayer(Player* player) const override;

        std::set<uint64>* GetPlayersInMap() { return &m_PlayersInMap; }

        uint32 GetItemTempSummonTimer() { return m_ItemTempSummonTimers; }
        void SetItemTempSummonTimer() { m_ItemTempSummonTimers = 7 * IN_MILLISECONDS * MINUTE; }

        void HandlePlayerResurrects(Player* p_Player, uint32 p_ZoneID) override;

        void SpawnChests();

        void SetControllerGUID(uint64 p_GUID) { m_ControllerGUID = p_GUID; }

        void RareCreatureKilled(uint32 p_Entry, uint64 p_GUID);

        void HandleLootReleased(Player* p_Player, uint64 p_GUID) override;

        /// Update Area
        void HandlePlayerUpdateArea(Player* p_Player, uint32 p_OldAreaID, uint32 p_NewAreaID) override;

        void HandlePlayerLeaveMap(Player* p_Player, uint32 p_MapID) override;

        void HandlePlayerEnterMap(Player* p_Player, uint32 p_MapID) override;

        void HandleKill(Player* p_KillerPlayer, Unit* p_KilledUnit) override;

        void FillCustomPvPLoots(Player* p_Looter, Loot& p_Loot, uint64 p_Container) override;

        bool Update(uint32 p_Diff) override;

    private:
        bool m_ActivatedFFA;
        bool m_CanSwitchFFAState;

        uint64 m_ControllerGUID;
        uint64 m_BigTreasureVisualGUID;

        uint32 m_PhaseSwitchingTimer;
        uint32 m_RareMobSpawningTimer;
        uint32 m_PlayersPvPStateCheckTimer;
        uint32 m_ItemTempSummonTimers;

        std::set<uint64> m_PlayersInMap;
        std::set<uint64> m_RareGUIDs;
        std::set<uint64> m_GobsGUIDs;

        std::vector<uint32> m_RareMobsSpawned;
        std::vector<uint32> m_ActiveSpells;
};

#endif ///< DALARAN_SEWERS_MGR_HPP_ASHRAN
