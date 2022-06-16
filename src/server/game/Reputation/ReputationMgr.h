////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __TRINITY_REPUTATION_MGR_H
#define __TRINITY_REPUTATION_MGR_H

#include "Common.h"
#include "SharedDefines.h"
#include "Language.h"
#include "QueryResult.h"

static uint32 ReputationRankStrIndex[MAX_REPUTATION_RANK] =
{
    LANG_REP_HATED,    LANG_REP_HOSTILE, LANG_REP_UNFRIENDLY, LANG_REP_NEUTRAL,
    LANG_REP_FRIENDLY, LANG_REP_HONORED, LANG_REP_REVERED,    LANG_REP_EXALTED
};

enum FactionFlags
{
    FACTION_FLAG_NONE               = 0x00,                 // no faction flag
    FACTION_FLAG_VISIBLE            = 0x01,                 // makes visible in client (set or can be set at interaction with target of this faction)
    FACTION_FLAG_AT_WAR             = 0x02,                 // enable AtWar-button in client. player controlled (except opposition team always war state), Flag only set on initial creation
    FACTION_FLAG_HIDDEN             = 0x04,                 // hidden faction from reputation pane in client (player can gain reputation, but this update not sent to client)
    FACTION_FLAG_INVISIBLE_FORCED   = 0x08,                 // always overwrite FACTION_FLAG_VISIBLE and hide faction in rep.list, used for hide opposite team factions
    FACTION_FLAG_PEACE_FORCED       = 0x10,                 // always overwrite FACTION_FLAG_AT_WAR, used for prevent war with own team factions
    FACTION_FLAG_INACTIVE           = 0x20,                 // player controlled, state stored in characters.data (CMSG_SET_FACTION_INACTIVE)
    FACTION_FLAG_RIVAL              = 0x40,                 // flag for the two competing outland factions
    FACTION_FLAG_SPECIAL            = 0x80                  // horde and alliance home cities and their northrend allies have this flag
};

typedef uint32 RepListID;
struct FactionState
{
    uint32 ID;
    RepListID ReputationListID;
    int32  Standing;
    uint8 Flags;
    bool needSend;
    bool needSave;
};

typedef ACE_Based::LockedMap<RepListID, FactionState> FactionStateList;
typedef ACE_Based::LockedMap<uint32, ReputationRank> ForcedReactions;

class Player;

class ReputationMgr
{
    public:                                                 // constructors and global modifiers
        explicit ReputationMgr(Player* owner) : _player(owner),
            _visibleFactionCount(0), _honoredFactionCount(0), _reveredFactionCount(0), _exaltedFactionCount(0), _sendFactionIncreased(false), m_ReputationModsChanged(false) { }
        ~ReputationMgr() { }

        void SaveToDB(SQLTransaction& trans);
        void LoadFromDB(PreparedQueryResult result);
    public:                                                 // statics
        static const int32 PointsInRank[MAX_REPUTATION_RANK];
        static const int32 Reputation_Cap;
        static const int32 Reputation_Bottom;

        static ReputationRank ReputationToRank(int32 standing);
    public:                                                 // accessors
        uint8 GetVisibleFactionCount() const { return _visibleFactionCount; }
        uint8 GetHonoredFactionCount() const { return _honoredFactionCount; }
        uint8 GetReveredFactionCount() const { return _reveredFactionCount; }
        uint8 GetExaltedFactionCount() const { return _exaltedFactionCount; }

        FactionStateList const& GetStateList() const { return _factions; }

        FactionState const* GetState(FactionEntry const* factionEntry) const
        {
            return factionEntry->CanHaveReputation() ? GetState(factionEntry->ReputationIndex) : NULL;
        }

        FactionState const* GetState(RepListID id) const
        {
            FactionStateList::const_iterator repItr = _factions.find (id);
            return repItr != _factions.end() ? &repItr->second : NULL;
        }

        bool IsAtWar(uint32 faction_id) const;
        bool IsAtWar(FactionEntry const* factionEntry) const;

        int32 GetReputation(uint32 faction_id) const;
        int32 GetReputation(FactionEntry const* factionEntry) const;
        int32 GetBaseReputation(FactionEntry const* factionEntry) const;

        ReputationRank GetRank(FactionEntry const* factionEntry) const;
        ReputationRank GetBaseRank(FactionEntry const* factionEntry) const;
        uint32 GetReputationRankStrIndex(FactionEntry const* factionEntry) const
        {
            return ReputationRankStrIndex[GetRank(factionEntry)];
        };

        ReputationRank const* GetForcedRankIfAny(FactionTemplateEntry const* factionTemplateEntry) const
        {
            ForcedReactions::const_iterator forceItr = _forcedReactions.find(factionTemplateEntry->Faction);
            return forceItr != _forcedReactions.end() ? &forceItr->second : NULL;
        }

    public:                                                 // modifiers
        bool SetReputation(FactionEntry const* factionEntry, int32 standing)
        {
            return SetReputation(factionEntry, standing, false);
        }
        bool SetReputationNoSpillOver(FactionEntry const* factionEntry, int32 standing)
        {
            return SetReputation(factionEntry, standing, false, true);
        }
        bool ModifyReputation(FactionEntry const* factionEntry, int32 standing, bool p_NoSpillOver = false)
        {
            return SetReputation(factionEntry, standing, true, p_NoSpillOver);
        }

        void ResetReputation(FactionEntry const* p_Faction, bool p_SendUpdate = true);

        void SetVisible(FactionTemplateEntry const* factionTemplateEntry);
        void SetVisible(FactionEntry const* factionEntry);
        void SetAtWar(RepListID repListID, bool on);
        void SetInactive(RepListID repListID, bool on);

        void ApplyForceReaction(uint32 faction_id, ReputationRank rank, bool apply);

        //! Public for chat command needs
        bool SetOneFactionReputation(FactionEntry const* factionEntry, int32 standing, bool incremental);

        void LoadReputationMods(PreparedQueryResult result);
        void SaveReputationMods(SQLTransaction& trans);
        int32 GetReputationModifier(uint32 reputationId) const;
        void AddReputationMod(uint32 p_FactionId, uint32 p_SourceId, int32 p_Value);

    public:                                                 // senders
        void SendInitialReputations();
        void SendForceReactions();
        void SendState(FactionState const* faction);
        void SendStates();
        void SendFactionBonusInfo();
        void UpdateReputationCriterias();
        void Initialize();

    private:                                                // internal helper functions
        uint32 GetDefaultStateFlags(FactionEntry const* factionEntry) const;
        bool SetReputation(FactionEntry const* factionEntry, int32 standing, bool incremental, bool p_NoSpillOver = false);
        void SetVisible(FactionState* faction);
        void SetAtWar(FactionState* faction, bool atWar) const;
        void SetInactive(FactionState* faction, bool inactive) const;
        void SendVisible(FactionState const* faction, bool visible = true) const;
        void UpdateRankCounters(ReputationRank old_rank, ReputationRank new_rank);
    private:
        Player* _player;
        FactionStateList _factions;
        ForcedReactions _forcedReactions;
        uint8 _visibleFactionCount :8;
        uint8 _honoredFactionCount :8;
        uint8 _reveredFactionCount :8;
        uint8 _exaltedFactionCount :8;
        bool _sendFactionIncreased; //! Play visual effect on next SMSG_SET_FACTION_STANDING sent

    private:

        struct ReputationMod
        {
            ReputationMod(uint32 p_FactionId, uint32 p_SourceId, int32 p_Value)
            {
                FactionId = p_FactionId;
                SourceId = p_SourceId;
                Value = p_Value;
            }

            uint32 FactionId;
            uint32 SourceId;
            int32 Value;

            bool IsStackable() const
            {
                return false;
            }
        };

        typedef std::list<ReputationMod> ReputationMods;
        ReputationMods m_ReputationMods;
        bool m_ReputationModsChanged;
};

#endif
