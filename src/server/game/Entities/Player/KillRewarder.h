////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2015 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef KillRewarder_h__
#define KillRewarder_h__

#include "Define.h"

class Player;
class Unit;
class Group;
using GroupPtr = std::shared_ptr<Group>;

class KillRewarder
{
    public:
        KillRewarder(Player* killer, Unit* victim, bool isBattleGround, std::set<uint64> p_RewardedPlayers);

        void Reward();

    private:
        void _InitXP(Player* player);
        void _InitGroupData();

        void _RewardHonor(Player* player);
        void _RewardXP(Player* player, float rate);
        void _RewardReputation(Player* player, float rate);
        void _RewardKillCredit(Player* player);
        void _RewardPlayer(Player* player, bool isDungeon);
        void _RewardGroup();

        Player* _killer;
        Unit* _victim;
        GroupPtr _group;
        float _groupRate;
        Player* _maxNotGrayMember;
        uint32 _count;
        uint32 _sumLevel;
        uint32 _xp;
        bool _isFullXP;
        uint8 _maxLevel;
        bool _isBattleGround;
        bool _isPvP;
        std::set<uint64> m_RewardedMembers;
};
#endif // KillRewarder_h__
