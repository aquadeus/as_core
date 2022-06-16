////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "return_to_karazhan.hpp"

enum eDatas
{
    BoardWidth = 8,
    BoardLength = 12
};

struct npc_rtk_kingAI;
using BoardCoordinates = std::pair<int8, int8>;

std::array<std::array<Position, eDatas::BoardLength>, eDatas::BoardWidth> g_Tiles =
{{
    {{
        { 3938.46f, -2266.30f, 780.85f, 0.0f },
        { 3933.32f, -2267.74f, 780.41f, 0.0f },
        { 3927.65f, -2267.67f, 780.40f, 0.0f },
        { 3921.93f, -2267.60f, 780.38f, 0.0f },
        { 3916.45f, -2267.66f, 780.40f, 0.0f },
        { 3911.01f, -2267.73f, 780.68f, 0.0f },
        { 0000.00f, -0000.00f, 000.00f, 0.0f },
        { 0000.00f, -0000.00f, 000.00f, 0.0f },
        { 0000.00f, -0000.00f, 000.00f, 0.0f },
        { 0000.00f, -0000.00f, 000.00f, 0.0f },
        { 0000.00f, -0000.00f, 000.00f, 0.0f },
        { 0000.00f, -0000.00f, 000.00f, 0.0f }
    }},
    {{
        { 3938.96f, -2260.07f, 779.99f, 0.0f },
        { 3933.24f, -2261.32f, 779.36f, 0.0f },
        { 3927.65f, -2261.46f, 779.38f, 0.0f },
        { 3922.35f, -2261.59f, 779.15f, 0.0f },
        { 3915.44f, -2260.47f, 776.91f, 0.0f },
        { 3909.35f, -2260.94f, 776.10f, 0.0f },
        { 0000.00f, -0000.00f, 000.00f, 0.0f },
        { 3895.82f, -2260.87f, 775.00f, 0.0f },
        { 0000.00f, -0000.00f, 000.00f, 0.0f },
        { 0000.00f, -0000.00f, 000.00f, 0.0f },
        { 0000.00f, -0000.00f, 000.00f, 0.0f },
        { 0000.00f, -0000.00f, 000.00f, 0.0f }
    }},
    {{
        { 3938.73f, -2254.01f, 779.46f, 0.0f },
        { 3932.95f, -2254.39f, 779.48f, 0.0f },
        { 3928.09f, -2254.41f, 778.66f, 0.0f },
        { 3921.84f, -2254.94f, 778.18f, 0.0f },
        { 3915.10f, -2254.48f, 777.62f, 0.0f },
        { 3908.76f, -2254.54f, 777.22f, 0.0f },
        { 3902.46f, -2254.87f, 776.79f, 0.0f },
        { 3895.88f, -2255.03f, 777.35f, 0.0f },
        { 3889.25f, -2255.13f, 777.26f, 0.0f },
        { 3883.09f, -2255.24f, 778.40f, 0.0f },
        { 0000.00f, -0000.00f, 000.00f, 0.0f },
        { 0000.00f, -0000.00f, 000.00f, 0.0f }
    }},
    {{
        { 3938.72f, -2248.08f, 779.22f, 0.0f },
        { 3933.26f, -2248.09f, 779.22f, 0.0f },
        { 3927.53f, -2248.18f, 778.05f, 0.0f },
        { 3922.05f, -2248.29f, 778.34f, 0.0f },
        { 3915.22f, -2248.39f, 778.12f, 0.0f },
        { 3908.56f, -2248.93f, 778.30f, 0.0f },
        { 3902.17f, -2249.05f, 778.24f, 0.0f },
        { 3895.94f, -2249.17f, 778.80f, 0.0f },
        { 3889.39f, -2249.34f, 779.51f, 0.0f },
        { 3883.09f, -2249.44f, 780.40f, 0.0f },
        { 0000.00f, -0000.00f, 000.00f, 0.0f },
        { 0000.00f, -0000.00f, 000.00f, 0.0f }
    }},
    {{
        { 3938.69f, -2241.98f, 779.29f, 0.0f },
        { 3933.20f, -2242.03f, 779.29f, 0.0f },
        { 3927.61f, -2242.02f, 779.04f, 0.0f },
        { 3921.86f, -2242.38f, 778.60f, 0.0f },
        { 3915.41f, -2242.39f, 779.01f, 0.0f },
        { 3909.09f, -2242.91f, 779.47f, 0.0f },
        { 3902.11f, -2243.12f, 779.91f, 0.0f },
        { 3895.82f, -2243.10f, 780.75f, 0.0f },
        { 3889.34f, -2243.38f, 781.32f, 0.0f },
        { 3883.27f, -2243.56f, 782.12f, 0.0f },
        { 3876.27f, -2243.75f, 783.15f, 0.0f },
        { 0000.00f, -0000.00f, 000.00f, 0.0f }
    }},
    {{
        { 3938.63f, -2236.13f, 779.65f, 0.0f },
        { 3933.19f, -2235.91f, 779.66f, 0.0f },
        { 3927.53f, -2235.97f, 779.65f, 0.0f },
        { 3921.60f, -2236.22f, 779.37f, 0.0f },
        { 3915.18f, -2236.72f, 779.55f, 0.0f },
        { 3909.13f, -2236.96f, 780.58f, 0.0f },
        { 3902.13f, -2237.08f, 781.71f, 0.0f },
        { 3895.82f, -2237.22f, 782.79f, 0.0f },
        { 3889.25f, -2237.41f, 783.95f, 0.0f },
        { 3882.98f, -2237.60f, 785.01f, 0.0f },
        { 3876.42f, -2237.78f, 786.02f, 0.0f },
        { 3869.86f, -2237.89f, 787.00f, 0.0f }
    }},
    {{
        { 3938.82f, -2229.98f, 780.33f, 0.0f },
        { 3933.25f, -2229.86f, 780.34f, 0.0f },
        { 3927.45f, -2230.06f, 780.32f, 0.0f },
        { 3921.73f, -2229.91f, 780.33f, 0.0f },
        { 3915.63f, -2230.89f, 780.67f, 0.0f },
        { 3908.87f, -2230.96f, 782.73f, 0.0f },
        { 3902.45f, -2231.02f, 784.46f, 0.0f },
        { 3895.99f, -2231.08f, 786.58f, 0.0f },
        { 3889.00f, -2231.39f, 787.34f, 0.0f },
        { 3882.78f, -2231.71f, 788.31f, 0.0f },
        { 0000.00f, -0000.00f, 000.00f, 0.0f },
        { 0000.00f, -0000.00f, 000.00f, 0.0f }
    }},
    {{
        { 3938.67f, -2223.78f, 781.43f, 0.0f },
        { 3933.15f, -2223.95f, 781.31f, 0.0f },
        { 3927.54f, -2224.24f, 781.26f, 0.0f },
        { 3922.01f, -2223.80f, 781.34f, 0.0f },
        { 3915.36f, -2224.61f, 781.93f, 0.0f },
        { 3908.66f, -2224.91f, 785.12f, 0.0f },
        { 3902.30f, -2225.08f, 787.38f, 0.0f },
        { 3895.84f, -2225.06f, 789.94f, 0.0f },
        { 3889.16f, -2225.43f, 791.40f, 0.0f },
        { 3882.80f, -2225.57f, 792.74f, 0.0f },
        { 3876.39f, -2225.74f, 794.42f, 0.0f },
        { 0000.00f, -0000.00f, 000.00f, 0.0f }
    }}
}};

struct npc_rtk_overall_chessAI : public LegionCombatAI
{
    npc_rtk_overall_chessAI(Creature* p_Creature) : LegionCombatAI(p_Creature) { }

    std::pair<uint8, uint8> m_Position;
    std::pair<uint8, uint8> m_StartingPosition;
    bool m_Initialized = false;
    std::map<uint64, BoardCoordinates> m_ChessPieces;
    uint64 m_King;

    void Reset() override
    {
        LegionCombatAI::Reset();

        if (!m_Initialized)
            return;

        SetBoardPosition(m_StartingPosition.first, m_StartingPosition.second);
        me->NearTeleportTo(g_Tiles[m_StartingPosition.first][m_StartingPosition.second]);
    }

    void InitializeAI() override
    {
        LegionCombatAI::InitializeAI();

        m_Initialized = true;
        m_StartingPosition = { 0, 0 };
        for (uint32 l_I = 0; l_I < eDatas::BoardWidth; l_I++)
        {
            for (uint32 l_J = 0; l_J < eDatas::BoardLength; l_J++)
            {
                if (me->GetExactDist(&g_Tiles[l_I][l_J]) < 3.0f)
                {
                    SetBoardPosition(l_I, l_J);
                    m_StartingPosition = { l_I, l_J };
                    return;
                }
            }
        }
    }

    void SetBoardPosition(int8 l_I, int8 l_J)
    {
        m_Position = { l_I, l_J };

        Creature* l_King = Creature::GetCreature(*me, m_King);
        if (!l_King || !l_King->IsAIEnabled)
            return;

        npc_rtk_overall_chessAI* l_KingAI = dynamic_cast<npc_rtk_overall_chessAI*>(l_King->GetAI());
        if (!l_KingAI)
            return;

        l_KingAI->m_ChessPieces[me->GetGUID()] = m_Position;
    }

    void EnterCombat(Unit* p_Who) override
    {
        DoZoneInCombat();
        LegionCombatAI::EnterCombat(p_Who);
    }
};

struct npc_rtk_chessAI : public npc_rtk_overall_chessAI
{
    npc_rtk_chessAI(Creature* p_Creature) : npc_rtk_overall_chessAI(p_Creature) { }

    void InitializeAI() override
    {
        npc_rtk_overall_chessAI::InitializeAI();

        me->SetReactState(REACT_PASSIVE);

        AddTimedDelayedOperation(2 * IN_MILLISECONDS, [this]() -> void
        {
            Creature* l_King = me->FindNearestCreature(eCreatures::King, 100.0f);
            if (!l_King)
                return;

            m_King = l_King->GetGUID();
        });
    }

    void UpdateAI(uint32 const p_Diff) override
    {
        UpdateOperations(p_Diff);
    }

    void Reset() override
    {
        if (!me->isAlive())
            me->Respawn(true, true, 0);

        npc_rtk_overall_chessAI::Reset();
        me->SetReactState(REACT_PASSIVE);
    }

    void JustDied(Unit* /*p_Killer*/) override
    {
        Creature* l_King = Creature::GetCreature(*me, m_King);
        if (!l_King || !l_King->IsAIEnabled)
            return;

        l_King->GetAI()->SetGUID(me->GetGUID());
    }

    void DoAction(int32 const p_Action) override
    {
        std::vector<BoardCoordinates> l_Positions;
        std::set<BoardCoordinates> p_OccupiedPositions;
        GetOccupiedPositions(p_OccupiedPositions);
        GetValidMoves(l_Positions, p_OccupiedPositions);
        if (l_Positions.empty())
            return;

        BoardCoordinates l_Target = l_Positions[urand(0, l_Positions.size() - 1)];

        MoveToBoardPosition(l_Target.first, l_Target.second);

        SetBoardPosition(l_Target.first, l_Target.second);
    }

    virtual uint32 const GetTileVisual() const = 0;

    virtual void GetValidMoves(std::vector<BoardCoordinates>& p_ValidDestinations, std::set<BoardCoordinates> const& p_OccupiedPositions) = 0;

    virtual void MoveToBoardPosition(int8 l_I, int8 l_J) = 0;

    void IgniteTiles()
    {
        std::vector<BoardCoordinates> l_Positions;
        std::set<BoardCoordinates> p_OccupiedPositions;
        GetValidMoves(l_Positions, p_OccupiedPositions);
        for (BoardCoordinates const& l_Pos : l_Positions)
            me->CastSpell(g_Tiles[l_Pos.first][l_Pos.second], GetTileVisual(), true);
    }

    bool IsOccupied(int8 p_I, int8 p_J, std::set<BoardCoordinates> const& p_OccupiedPositions)
    {
        if (p_OccupiedPositions.empty())
            return false;

        for (BoardCoordinates const& l_Position : p_OccupiedPositions)
        {
            if (l_Position.first == p_I && l_Position.second == p_J)
                return true;
        }

        return false;
    }

    void GetOccupiedPositions(std::set<BoardCoordinates>& p_OccupiedPositions)
    {
        Creature* l_King = Creature::GetCreature(*me, m_King);
        if (!l_King || !l_King->IsAIEnabled)
            return;

        npc_rtk_overall_chessAI* l_KingAI = dynamic_cast<npc_rtk_overall_chessAI*>(l_King->GetAI());
        if (!l_KingAI)
            return;

        for (std::pair<uint64, BoardCoordinates> const& l_ChessPiece : l_KingAI->m_ChessPieces)
        {
            if (l_ChessPiece.first != me->GetGUID())
                p_OccupiedPositions.insert(l_ChessPiece.second);
        }
    }

    void GetBishopValidMoves(std::vector<BoardCoordinates>& p_ValidDestinations, std::set<BoardCoordinates> const& p_OccupiedPositions)
    {
        int8 l_I = m_Position.first + 1;
        int8 l_J = m_Position.second + 1;
        while (l_I < eDatas::BoardWidth && l_J < eDatas::BoardLength)
        {
            if (IsOccupied(l_I, l_J, p_OccupiedPositions))
                break;

            if (g_Tiles[l_I][l_J].m_positionX != 0.0f)
                p_ValidDestinations.push_back({ l_I, l_J });

            l_I++;
            l_J++;
        }

        l_I = m_Position.first + 1;
        l_J = m_Position.second - 1;
        while (l_I < eDatas::BoardWidth && l_J >= 0)
        {
            if (IsOccupied(l_I, l_J, p_OccupiedPositions))
                break;

            if (g_Tiles[l_I][l_J].m_positionX != 0.0f)
                p_ValidDestinations.push_back({ l_I, l_J });

            l_I++;
            l_J--;
        }

        l_I = m_Position.first - 1;
        l_J = m_Position.second + 1;
        while (l_I >= 0 && l_J < eDatas::BoardLength)
        {
            if (IsOccupied(l_I, l_J, p_OccupiedPositions))
                break;

            if (g_Tiles[l_I][l_J].m_positionX != 0.0f)
                p_ValidDestinations.push_back({ l_I, l_J });

            l_I--;
            l_J++;
        }

        l_I = m_Position.first - 1;
        l_J = m_Position.second - 1;
        while (l_I >= 0 && l_J >= 0)
        {
            if (IsOccupied(l_I, l_J, p_OccupiedPositions))
                break;

            if (g_Tiles[l_I][l_J].m_positionX != 0.0f)
                p_ValidDestinations.push_back({ l_I, l_J });

            l_I--;
            l_J--;
        }
    }

    void GetRookValidMoves(std::vector<BoardCoordinates>& p_ValidDestinations, std::set<BoardCoordinates> const& p_OccupiedPositions)
    {
        int8 l_I = m_Position.first + 1;
        int8 l_J = m_Position.second;
        while (l_I < eDatas::BoardWidth)
        {
            if (IsOccupied(l_I, l_J, p_OccupiedPositions))
                break;

            if (g_Tiles[l_I][l_J].m_positionX != 0.0f)
                p_ValidDestinations.push_back({ l_I, l_J });

            l_I++;
        }

        l_I = m_Position.first - 1;
        l_J = m_Position.second;
        while (l_I >= 0)
        {
            if (IsOccupied(l_I, l_J, p_OccupiedPositions))
                break;

            if (g_Tiles[l_I][l_J].m_positionX != 0.0f)
                p_ValidDestinations.push_back({ l_I, l_J });

            l_I--;
        }

        l_I = m_Position.first;
        l_J = m_Position.second + 1;
        while (l_J < eDatas::BoardLength)
        {
            if (IsOccupied(l_I, l_J, p_OccupiedPositions))
                break;

            if (g_Tiles[l_I][l_J].m_positionX != 0.0f)
                p_ValidDestinations.push_back({ l_I, l_J });

            l_J++;
        }

        l_I = m_Position.first;
        l_J = m_Position.second - 1;
        while (l_J >= 0)
        {
            if (IsOccupied(l_I, l_J, p_OccupiedPositions))
                break;

            if (g_Tiles[l_I][l_J].m_positionX != 0.0f)
                p_ValidDestinations.push_back({ l_I, l_J });

            l_J--;
        }
    }
};

/// Last Update 7.1.5 Build 23420
/// King - 115388
class npc_rtk_king : public CreatureScript
{
    public:
        npc_rtk_king() : CreatureScript("npc_rtk_king") { }

        struct npc_rtk_kingAI : public npc_rtk_overall_chessAI
        {
            npc_rtk_kingAI(Creature* p_Creature) : npc_rtk_overall_chessAI(p_Creature) { }

            enum eEvents
            {
                MovePiece           = 1,
                EventRoyalSlash     = 2,
                EventCheckPlayers   = 3
            };

            enum eSpells
            {
                Royalty             = 229489,
                RoyalSlash          = 229426,
                RoyalSlashAnimation = 229429,
                GameInSession       = 39331
            };

            uint32 m_LastDeadPieceTimer;

            std::vector<G3D::Vector2> m_Corners =
            {
                { 3941.43f, -2220.68f },
                { 3941.43f, -2270.46f },
                { 3898.68f, -2271.06f },
                { 3865.92f, -2253.56f },
                { 3865.33f, -2223.38f }
            };

            void InitializeAI() override
            {
                npc_rtk_overall_chessAI::InitializeAI();

                AddTimedDelayedOperation(2 * IN_MILLISECONDS, [this]() -> void
                {
                    std::list<Creature*> l_ChessPieces;
                    me->GetCreatureListWithEntryInGrid(l_ChessPieces, eCreatures::Queen, 100.0f);
                    me->GetCreatureListWithEntryInGrid(l_ChessPieces, eCreatures::Rook, 100.0f);
                    me->GetCreatureListWithEntryInGrid(l_ChessPieces, eCreatures::Knight, 100.0f);
                    me->GetCreatureListWithEntryInGrid(l_ChessPieces, eCreatures::WhiteBishop, 100.0f);
                    me->GetCreatureListWithEntryInGrid(l_ChessPieces, eCreatures::BlackBishop, 100.0f);
                    l_ChessPieces.push_back(me);

                    for (Creature* l_Creature : l_ChessPieces)
                    {
                        if (!l_Creature->IsAIEnabled)
                            continue;

                        npc_rtk_overall_chessAI* l_AI = dynamic_cast<npc_rtk_overall_chessAI*>(l_Creature->GetAI());
                        m_ChessPieces[l_Creature->GetGUID()] = l_AI->m_Position;
                    }
                });
            }

            void Reset() override
            {
                npc_rtk_overall_chessAI::Reset();

                for (auto l_ChessPiece : m_ChessPieces)
                {
                    if (l_ChessPiece.first == me->GetGUID())
                        continue;

                    Creature* l_Creature = Creature::GetCreature(*me, l_ChessPiece.first);
                    if (!l_Creature || !l_Creature->IsAIEnabled)
                        continue;

                    l_Creature->GetAI()->Reset();
                }

                InstanceScript* l_Instance = me->GetInstanceScript();
                if (!l_Instance || !l_Instance->instance)
                    return;

                std::vector<Player*> l_LootingPlayers;
                for (Map::PlayerList::const_iterator itr = l_Instance->instance->GetPlayers().begin(); itr != l_Instance->instance->GetPlayers().end(); ++itr)
                {
                    if (Player* l_Player = itr->getSource())
                        l_Player->RemoveAura(eSpells::GameInSession);
                }
            }

            void JustDied(Unit* p_Killer) override
            {
                LegionCombatAI::JustDied(p_Killer);

                std::map<uint64, BoardCoordinates> l_ChessPieces = m_ChessPieces;
                for (auto l_ChessPiece : l_ChessPieces)
                {
                    if (l_ChessPiece.first == me->GetGUID())
                        continue;

                    Creature* l_Creature = Creature::GetCreature(*me, l_ChessPiece.first);
                    if (!l_Creature || !l_Creature->IsAIEnabled)
                        continue;

                    me->Kill(l_Creature);
                }
            }

            void SetGUID(uint64 p_Guid, int32 p_ID /* = 0 */) override
            {
                m_ChessPieces.erase(p_Guid);

                me->RemoveAura(eSpells::Royalty);

                m_LastDeadPieceTimer = getMSTime();
                uint32 l_LastDeadPieceTimer = m_LastDeadPieceTimer;

                AddTimedDelayedOperation(30 * IN_MILLISECONDS, [this, l_LastDeadPieceTimer]() -> void
                {
                    if (m_LastDeadPieceTimer != l_LastDeadPieceTimer)
                        return;

                    me->CastSpell(me, eSpells::Royalty, true);
                });
            }

            bool CanChase() override
            {
                return false;
            }

            bool IsInPolygon(Position const* p_Target)
            {
                uint32 l_Count = uint32(m_Corners.size());
                uint32 l_J = 0;
                bool l_Ok = false;

                /// Algorithm taken from https://wrf.ecse.rpi.edu//Research/Short_Notes/pnpoly.html
                for (uint32 l_I = 0, l_J = l_Count - 1; l_I < l_Count; l_J = l_I++)
                {
                    if (((m_Corners[l_I].y > p_Target->m_positionY) != (m_Corners[l_J].y > p_Target->m_positionY)) &&
                        (p_Target->m_positionX < (m_Corners[l_J].x - m_Corners[l_I].x) * (p_Target->m_positionY - m_Corners[l_I].y) / (m_Corners[l_J].y - m_Corners[l_I].y) + m_Corners[l_I].x))
                    {
                        l_Ok = !l_Ok;
                    }
                }

                return l_Ok;
            }

            bool AdditionalChecks(uint32 p_EventID) override
            {
                switch (p_EventID)
                {
                    case eEvents::EventRoyalSlash:
                        return !me->HasAura(eSpells::Royalty);
                    default:
                        return LegionCombatAI::AdditionalChecks(p_EventID);
                }

                return false;
            }

            bool ExecuteEvent(uint32 p_EventID, Unit* p_Target) override
            {
                switch (p_EventID)
                {
                    case eEvents::MovePiece:
                    {
                        if (m_ChessPieces.empty())
                            return true;

                        auto l_Itr = m_ChessPieces.begin();
                        std::advance(l_Itr, urand(0, m_ChessPieces.size() - 1));

                        Creature* l_Piece = Creature::GetCreature(*me, l_Itr->first);
                        if (!l_Piece || !l_Piece->IsAIEnabled)
                            return false;

                        l_Piece->GetAI()->DoAction(0);

                        return true;
                    }
                    case eEvents::EventRoyalSlash:
                    {
                        std::vector<BoardCoordinates> l_Positions;
                        GetValidMoves(l_Positions);
                        for (BoardCoordinates const& l_Pos : l_Positions)
                            me->CastSpell(g_Tiles[l_Pos.first][l_Pos.second], eSpells::RoyalSlash, true);

                        AddTimedDelayedOperation(1 * IN_MILLISECONDS, [this]() -> void
                        {
                            DoCast(eSpells::RoyalSlashAnimation);
                        });

                        return true;
                    }
                    case eEvents::EventCheckPlayers:
                    {
                        InstanceScript* l_Instance = me->GetInstanceScript();
                        if (!l_Instance || !l_Instance->instance)
                            return false;

                        std::vector<Player*> l_LootingPlayers;
                        for (Map::PlayerList::const_iterator itr = l_Instance->instance->GetPlayers().begin(); itr != l_Instance->instance->GetPlayers().end(); ++itr)
                        {
                            if (Player* l_Player = itr->getSource())
                            {
                                if (!IsInPolygon(l_Player))
                                    l_Player->CastSpell(l_Player, eSpells::GameInSession, true);
                                else
                                    l_Player->RemoveAurasDueToSpell(eSpells::GameInSession);
                            }
                        }

                        return true;
                    }
                    default:
                        return LegionCombatAI::ExecuteEvent(p_EventID, p_Target);
                }

                return false;
            }

            void GetValidMoves(std::vector<BoardCoordinates>& p_ValidMoves)
            {
                std::array<std::pair<int8, int8>, 8> l_RelativeDests =
                {{
                    { 1, 1 },
                    { 1, 0 },
                    { 1, -1 },
                    { 0, -1 },
                    { -1, -1 },
                    { -1, 0 },
                    { -1, 1 },
                    { 0, 1 },
                }};

                int8 l_I = m_Position.first;
                int8 l_J = m_Position.second;
                for (uint8 l_Z = 0; l_Z < l_RelativeDests.size(); l_Z++)
                {
                    int8 l_NewI = l_I + l_RelativeDests[l_Z].first;
                    int8 l_NewJ = l_J + l_RelativeDests[l_Z].second;

                    if ((l_NewI < eDatas::BoardWidth) && (l_NewJ < eDatas::BoardLength) && (l_NewI >= 0) && (l_NewJ >= 0) && (g_Tiles[l_NewI][l_NewJ].m_positionX != 0.0f))
                        p_ValidMoves.push_back({ l_NewI, l_NewJ });
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rtk_kingAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Queen - 115395
class npc_rtk_queen : public CreatureScript
{
    public:
        npc_rtk_queen() : CreatureScript("npc_rtk_queen") { }

        struct npc_rtk_queenAI : public npc_rtk_chessAI
        {
            npc_rtk_queenAI(Creature* p_Creature) : npc_rtk_chessAI(p_Creature) { }

            enum eSpells
            {
                QueenAreatrigger = 229392
            };

            uint32 const GetTileVisual() const override
            {
                return eSpells::QueenAreatrigger;
            }

            void GetValidMoves(std::vector<BoardCoordinates>& p_ValidDestinations, std::set<BoardCoordinates> const& p_OccupiedPositions) override
            {
                GetBishopValidMoves(p_ValidDestinations, p_OccupiedPositions);
                GetRookValidMoves(p_ValidDestinations, p_OccupiedPositions);
            }

            void MoveToBoardPosition(int8 l_I, int8 l_J) override
            {
                Position l_Dest = g_Tiles[l_I][l_J];
                me->SetWalk(false);
                me->SetSpeed(UnitMoveType::MOVE_RUN, 7.0f);
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveTakeoff(0, l_Dest);

                AddTimedDelayedOperation(1500, [this]() -> void
                {
                    IgniteTiles();
                });
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rtk_queenAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Rook - 115407
class npc_rtk_rook : public CreatureScript
{
    public:
        npc_rtk_rook() : CreatureScript("npc_rtk_rook") { }

        struct npc_rtk_rookAI : public npc_rtk_chessAI
        {
            npc_rtk_rookAI(Creature* p_Creature) : npc_rtk_chessAI(p_Creature) { }

            enum eSpells
            {
                RookAreatrigger = 229570,
                RookMove        = 229335
            };

            uint32 const GetTileVisual() const override
            {
                return eSpells::RookAreatrigger;
            }

            void GetValidMoves(std::vector<BoardCoordinates>& p_ValidDestinations, std::set<BoardCoordinates> const& p_OccupiedPositions) override
            {
                GetRookValidMoves(p_ValidDestinations, p_OccupiedPositions);
            }

            void MoveToBoardPosition(int8 l_I, int8 l_J) override
            {
                Position l_Dest = g_Tiles[l_I][l_J];
                me->SetWalk(false);
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveCharge(&l_Dest);
                me->CastSpell(l_Dest, eSpells::RookMove, true);

                AddTimedDelayedOperation(1500, [this]() -> void
                {
                    IgniteTiles();
                });
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rtk_rookAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Knight - 115406
class npc_rtk_knight : public CreatureScript
{
    public:
        npc_rtk_knight() : CreatureScript("npc_rtk_knight") { }

        struct npc_rtk_knightAI : public npc_rtk_chessAI
        {
            npc_rtk_knightAI(Creature* p_Creature) : npc_rtk_chessAI(p_Creature) { }

            enum eSpells
            {
                KnightAreatrigger   = 229564,
                KnightMove          = 229298
            };

            uint32 const GetTileVisual() const override
            {
                return eSpells::KnightAreatrigger;
            }

            void GetValidMoves(std::vector<BoardCoordinates>& p_ValidDestinations, std::set<BoardCoordinates> const& p_OccupiedPositions) override
            {
                std::array<std::pair<int8, int8>, 8> l_RelativeDests =
                {{
                    { 2, 1 },
                    { 1, 2 },
                    { -2, 1 },
                    { -1, 2 },
                    { 2, -1 },
                    { 1, -2 },
                    { -2, -1 },
                    { -1, -2 },
                }};

                int8 l_I = m_Position.first;
                int8 l_J = m_Position.second;
                for (uint8 l_Z = 0; l_Z < l_RelativeDests.size(); l_Z++)
                {
                    int8 l_NewI = l_I + l_RelativeDests[l_Z].first;
                    int8 l_NewJ = l_J + l_RelativeDests[l_Z].second;

                    if (IsOccupied(l_NewI, l_NewJ, p_OccupiedPositions))
                        continue;

                    if ((l_NewI < eDatas::BoardWidth) && (l_NewJ < eDatas::BoardLength) && (l_NewI >= 0) && (l_NewJ >= 0) && (g_Tiles[l_NewI][l_NewJ].m_positionX != 0.0f))
                        p_ValidDestinations.push_back({l_NewI, l_NewJ});
                }
            }

            void MoveToBoardPosition(int8 l_I, int8 l_J) override
            {
                Position l_Pos = *me;
                l_Pos.m_positionZ += 7.0f;
                me->SetWalk(false);
                me->SetDisableGravity(true);
                me->SetSpeed(UnitMoveType::MOVE_RUN, 7.0f);
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveTakeoff(0, l_Pos);

                AddTimedDelayedOperation(1000, [this, l_I, l_J]() -> void
                {
                    Position l_Pos = g_Tiles[l_I][l_J];
                    l_Pos.m_positionZ += 7.0f;
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveTakeoff(0, l_Pos);
                });

                AddTimedDelayedOperation(2000, [this, l_I, l_J]() -> void
                {
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveTakeoff(0, g_Tiles[l_I][l_J]);
                });

                AddTimedDelayedOperation(3000, [this]() -> void
                {
                    me->CastSpell(me, eSpells::KnightMove, true);
                    IgniteTiles();
                });
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rtk_knightAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Bishop White - 115401
class npc_rtk_bishop_white : public CreatureScript
{
    public:
        npc_rtk_bishop_white() : CreatureScript("npc_rtk_bishop_white") { }

        struct npc_rtk_bishop_whiteAI : public npc_rtk_chessAI
        {
            npc_rtk_bishop_whiteAI(Creature* p_Creature) : npc_rtk_chessAI(p_Creature) { }

            enum eSpells
            {
                WhiteBishopAreatrigger = 229546
            };

            uint32 const GetTileVisual() const override
            {
                return eSpells::WhiteBishopAreatrigger;
            }

            void GetValidMoves(std::vector<BoardCoordinates>& p_ValidDestinations, std::set<BoardCoordinates> const& p_OccupiedPositions) override
            {
                GetBishopValidMoves(p_ValidDestinations, p_OccupiedPositions);
            }
             
            void MoveToBoardPosition(int8 l_I, int8 l_J) override
            {
                Position l_Dest = g_Tiles[l_I][l_J];
                me->NearTeleportTo(l_Dest);

                AddTimedDelayedOperation(1000, [this]() -> void
                {
                    IgniteTiles();
                });
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rtk_bishop_whiteAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Bishop Black - 115402
class npc_rtk_bishop_black : public CreatureScript
{
    public:
        npc_rtk_bishop_black() : CreatureScript("npc_rtk_bishop_black") { }

        struct npc_rtk_bishop_blackAI : public npc_rtk_chessAI
        {
            npc_rtk_bishop_blackAI(Creature* p_Creature) : npc_rtk_chessAI(p_Creature) { }

            enum eSpells
            {
                BlackBishopAreatrigger = 229560
            };

            uint32 const GetTileVisual() const override
            {
                return eSpells::BlackBishopAreatrigger;
            }

            void GetValidMoves(std::vector<BoardCoordinates>& p_ValidDestinations, std::set<BoardCoordinates> const& p_OccupiedPositions) override
            {
                GetBishopValidMoves(p_ValidDestinations, p_OccupiedPositions);
            }

            void MoveToBoardPosition(int8 l_I, int8 l_J) override
            {
                Position l_Dest = g_Tiles[l_I][l_J];
                me->NearTeleportTo(l_Dest);

                AddTimedDelayedOperation(1000, [this]() -> void
                {
                    IgniteTiles();
                });
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rtk_bishop_blackAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Rook Move - 229335
class spell_rtk_rook_move : public SpellScriptLoader
{
    public:
        spell_rtk_rook_move() : SpellScriptLoader("spell_rtk_rook_move") { }

        class spell_rtk_rook_move_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rtk_rook_move_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                WorldLocation const* l_Dest = GetExplTargetDest();
                Unit* l_Caster = GetCaster();
                if (!l_Dest || !l_Caster || p_Targets.empty())
                    return;

                p_Targets.remove_if([l_Dest, l_Caster](WorldObject* p_Target) -> bool
                {
                    if (!p_Target->IsInAxe(l_Caster, l_Dest, 3.0f))
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rtk_rook_move_SpellScript::FilterTargets, EFFECT_0, TARGET_ENNEMIES_IN_CYLINDER);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rtk_rook_move_SpellScript();
        }
};

/// Archmage Khadgar - 116259
class npc_archmage_khadgar_chess : public CreatureScript
{
    public:
        npc_archmage_khadgar_chess() : CreatureScript("npc_archmage_khadgar_chess") { }

        struct npc_archmage_khadgar_chessAI : public ScriptedAI
        {
            npc_archmage_khadgar_chessAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            enum eSpells
            {
                RavenForm       = 229645,
                EnergyPortal    = 229651
            };

            std::vector<G3D::Vector3> m_WalkToDoor =
            {
                { 3733.32f, -2244.21f, 815.32f },
                { 3713.11f, -2244.92f, 815.18f },
                { 3700.49f, -2240.14f, 815.18f }
            };

            std::vector<G3D::Vector3> m_FlyAwayPath =
            {
                { 3696.50f, -2205.80f, 815.60f },
                { 3682.50f, -2168.17f, 840.60f },
                { 3618.71f, -2066.08f, 893.79f },
                { 3508.04f, -1858.81f, 924.09f }
            };

            Position const m_MedivhSpawn = { 3743.73f, -2237.47f, 812.87f, 5.575f };
            Position const m_KhadgarWalk = { 3750.27f, -2246.45f, 808.81f };
            uint64 m_MedivhGUID = 0;

            void DoAction(int32 const p_Action) override
            {
                SetFlyMode(false);
                AddTimedDelayedOperation(2 * IN_MILLISECONDS, [this]() -> void
                {
                    if (Creature* l_Medivh = me->SummonCreature(eCreatures::MedivhChess, m_MedivhSpawn))
                    {
                        l_Medivh->CastSpell(l_Medivh, eSpells::EnergyPortal, true);
                        m_MedivhGUID = l_Medivh->GetGUID();
                    }
                });

                AddTimedDelayedOperation(6 * IN_MILLISECONDS, [this]() -> void
                {
                    Creature* l_Medivh = Creature::GetCreature(*me, m_MedivhGUID);
                    if (!l_Medivh)
                        return;

                    me->SetFacingToObject(l_Medivh);
                });

                AddTimedDelayedOperation(12 * IN_MILLISECONDS, [this]() -> void
                {
                    me->SetWalk(true);
                    me->GetMotionMaster()->MovePoint(0, m_KhadgarWalk);
                });

                AddTimedDelayedOperation(13 * IN_MILLISECONDS, [this]() -> void
                {
                    Creature* l_Medivh = Creature::GetCreature(*me, m_MedivhGUID);
                    if (!l_Medivh)
                        return;

                    me->SetFacingToObject(l_Medivh);
                });

                DelayTalk(14, 0);

                AddTimedDelayedOperation(28 * IN_MILLISECONDS, [this]() -> void
                {
                    me->SetWalk(true);
                    me->GetMotionMaster()->MoveSmoothPath(10, m_WalkToDoor.data(), m_WalkToDoor.size(), true);
                });
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_Id) override
            {
                switch (p_Id)
                {
                    case 10:
                    {
                        me->CastSpell(me, eSpells::RavenForm, true);
                        SetFlyMode(true);
                        me->GetMotionMaster()->MoveSmoothFlyPath(11, m_FlyAwayPath.data(), m_FlyAwayPath.size());

                        break;
                    }
                    case 11:
                    {
                        me->DespawnOrUnsummon();
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_archmage_khadgar_chessAI(p_Creature);
        }
};

/// Medivh - 116124
class npc_medivh_chess : public CreatureScript
{
    public:
        npc_medivh_chess() : CreatureScript("npc_medivh_chess") { }

        struct npc_medivh_chessAI : public ScriptedAI
        {
            npc_medivh_chessAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            enum eSpells
            {
                ArcaneCast = 196978,
                RavenForm = 229645
            };

            std::vector<G3D::Vector3> m_WalkToDoor =
            {
                { 3744.14f, -2240.11f, 812.05f },
                { 3732.85f, -2244.38f, 815.36f },
                { 3713.11f, -2244.92f, 815.18f },
                { 3700.49f, -2240.14f, 815.18f },
                { 3696.10f, -2226.66f, 815.18f },
                { 3696.22f, -2218.37f, 815.18f }
            };

            std::vector<G3D::Vector3> m_FlyAwayPath =
            {
                { 3691.76f, -2206.80f, 815.60f },
                { 3662.77f, -2156.61f, 833.25f },
                { 3611.85f, -2071.52f, 900.85f },
                { 3409.83f, -1847.55f, 964.92f }
            };

            void Reset() override
            {
                DelayTalk(7, 0);
                DelayTalk(14, 1);

                AddTimedDelayedOperation(26 * IN_MILLISECONDS, [this]() -> void
                {
                    me->SetWalk(false);
                    me->GetMotionMaster()->MoveSmoothPath(10, m_WalkToDoor.data(), m_WalkToDoor.size(), false);
                });
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_Id) override
            {
                switch (p_Id)
                {
                    case 10:
                    {
                        me->CastSpell(me, eSpells::ArcaneCast, true);
                        GameObject* l_Door = me->FindNearestGameObject(30.0f);
                        if (!l_Door)
                            break;

                        l_Door->SetGoState(GO_STATE_ACTIVE);

                        AddTimedDelayedOperation(3 * IN_MILLISECONDS, [this]() -> void
                        {
                            me->CastSpell(me, eSpells::RavenForm, true);
                        });

                        AddTimedDelayedOperation(4 * IN_MILLISECONDS, [this]() -> void
                        {
                            SetFlyMode(true);
                            me->GetMotionMaster()->MoveSmoothFlyPath(11, m_FlyAwayPath.data(), m_FlyAwayPath.size());
                        });

                        break;
                    }
                    case 11:
                    {
                        me->DespawnOrUnsummon();
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_medivh_chessAI(p_Creature);
        }
};

#ifndef __clang_analyzer__
void AddSC_gamesmans_room()
{
    new npc_rtk_king();
    new npc_rtk_queen();
    new npc_rtk_rook();
    new npc_rtk_knight();
    new npc_rtk_bishop_white();
    new npc_rtk_bishop_black();

    new spell_rtk_rook_move();

    new npc_archmage_khadgar_chess();
    new npc_medivh_chess();
}
#endif
