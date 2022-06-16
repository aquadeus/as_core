////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2018 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Vehicle.h"

/// Demon Soul - 86963
class npc_soul_gem_86963 : public CreatureScript
{
public:
    npc_soul_gem_86963() : CreatureScript("npc_soul_gem_86963") { }

    struct npc_soul_gem_86963_gemAI : public ScriptedAI
    {
        npc_soul_gem_86963_gemAI(Creature *creature) : ScriptedAI(creature) { }

        enum eSpells
        {
            Spell_SoulGemPing = 178974,
            Spell_SoulMissile = 178753,
            Spell_SoulGemTransform = 178752
        };

        ObjectGuid m_SummonerGuid;
        bool m_Transformed = false;;

        void IsSummonedBy(Unit* p_Summoner) override
        {
            m_SummonerGuid = p_Summoner->GetGUID();
        }

        void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
        {
            if (p_Spell->Id == eSpells::Spell_SoulMissile && !m_Transformed)
            {
                DoCast(eSpells::Spell_SoulGemTransform, true);
                m_Transformed = true;
            }
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
            if (!l_Player || !l_Player->IsInWorld())
                return;

            l_Player->CastSpell(me->GetPosition(), eSpells::Spell_SoulGemPing, true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_soul_gem_86963_gemAI(creature);
    }
};

/// Stellagosa - 90982
class npc_stellagosa_90982 : public CreatureScript
{
public:
    npc_stellagosa_90982() : CreatureScript("npc_stellagosa_90982") { }

    struct npc_stellagosa_90982AI : public VehicleAI
    {
        npc_stellagosa_90982AI(Creature *creature) : VehicleAI(creature) { }

        enum eSpells
        {
            Spell_Stellagosa = 180768
        };

        enum eConversations
        {
            Conv_FlyOnStellagosa = 1388
        };

        enum ePoints
        {
            Point_FlyPathEnd = 1
        };

        enum eKillcredits
        {
            Killcredit_MeetBackAtIllidariStand = 112175
        };

        ObjectGuid m_SummonerGuid;

        void IsSummonedBy(Unit* p_Summoner) override
        {
            m_SummonerGuid = p_Summoner->GetGUID();

            me->AddPlayerInPersonnalVisibilityList(m_SummonerGuid);

            if (Vehicle* l_Vehicle = me->GetVehicleKit())
            {
                if (Unit* l_Unit = l_Vehicle->GetPassenger(0))
                {
                    l_Unit->AddPlayerInPersonnalVisibilityList(m_SummonerGuid);
                    l_Unit->SetGuidValue(UNIT_FIELD_DEMON_CREATOR, m_SummonerGuid);
                }
            }

            p_Summoner->CastSpell(me, eSpells::Spell_Stellagosa, true);

            me->AddDelayedEvent([this]() -> void
            {
                G3D::Vector3 const Path_Stellagosa[11] =
                {
                    { -507.4601f, 7512.798f, 90.474f },
                    { -268.1285f, 7515.444f, 129.5363f },
                    { -63.25f, 7452.395f, 136.4711f },
                    { -12.22917f, 7376.551f, 119.5238f },
                    { -104.7361f, 7157.25f, 42.86799f },
                    { -104.3767f, 7014.188f, 29.11961f },
                    { -136.3698f, 6953.932f, 25.89488f },
                    { -115.3611f, 6923.146f, 26.11258f },
                    { -80.07291f, 6920.13f, 22.71359f },
                    { -77.17882f, 6946.385f, 19.67522f },
                    { -95.2689f, 6960.178f, 13.17749f }
                };
                ;
                me->SetFlag(UNIT_FIELD_BYTES_1, 50331648);
                me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyPathEnd, Path_Stellagosa, 11);
            }, 2000);

            p_Summoner->AddDelayedEvent([p_Summoner]() -> void
            {
                if (Conversation* l_Conversation = new Conversation)
                {
                    if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_FlyOnStellagosa, p_Summoner, nullptr, p_Summoner->GetPosition()))
                        delete l_Conversation;
                }
            }, 7000);
        }

        void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
        {
            if (p_PointId == ePoints::Point_FlyPathEnd)
            {
                if (Vehicle* l_Vehicle = me->GetVehicleKit())
                {
                    l_Vehicle->RemoveAllPassengers();
                }

                if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                {
                    l_Player->KilledMonsterCredit(eKillcredits::Killcredit_MeetBackAtIllidariStand);
                }

                me->DespawnOrUnsummon();
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stellagosa_90982AI(creature);
    }
};

/// Archmage Khadgar - 93325
class npc_archmage_khadgar_93325 : public CreatureScript
{
public:
    npc_archmage_khadgar_93325() : CreatureScript("npc_archmage_khadgar_93325") { }

    struct npc_archmage_khadgar_93325AI : public ScriptedAI
    {
        npc_archmage_khadgar_93325AI(Creature *creature) : ScriptedAI(creature) { }

        enum eSpells
        {
            Spell_RavenForm = 187983
        };

        enum ePoints
        {
            Point_FlyPathEnd = 1
        };

        void IsSummonedBy(Unit* p_Summoner) override
        {
            uint64 l_SummonerGuid = p_Summoner->GetGUID();

            me->AddPlayerInPersonnalVisibilityList(l_SummonerGuid);

            me->AddDelayedEvent([this, l_SummonerGuid]() -> void
            {
                if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                {
                    Talk(0, l_SummonerGuid);
                }
            }, 2500);

            me->AddDelayedEvent([this]() -> void
            {
                DoCast(eSpells::Spell_RavenForm, true);
            }, 5000);

            me->AddDelayedEvent([this]() -> void
            {
                G3D::Vector3 const Path_Khadgar[4] =
                {
                    { -82.1441f, 6870.753f, 29.81108f },
                    { -76.743797f, 6888.551758f, 22.757183f },
                    { -73.263069f, 6914.273438f, 16.687225f },
                    { -91.00694f, 6936.319f, 24.222f }
                };

                me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyPathEnd, Path_Khadgar, 4);
            }, 7500);
        }

        void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
        {
            if (p_PointId == ePoints::Point_FlyPathEnd)
            {
                me->DespawnOrUnsummon();
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_archmage_khadgar_93325AI(creature);
    }
};

/// Archmage Khadgar - 93326
class npc_archmage_khadgar_93326 : public CreatureScript
{
public:
    npc_archmage_khadgar_93326() : CreatureScript("npc_archmage_khadgar_93326") { }

    struct npc_archmage_khadgar_93326AI : public ScriptedAI
    {
        npc_archmage_khadgar_93326AI(Creature *creature) : ScriptedAI(creature) { }

        enum eSpells
        {
            Spell_RavenForm = 187983
        };

        enum ePoints
        {
            Point_FlyPathEnd = 1
        };

        enum eNpcs
        {
            Npc_Stellagosa = 103580
        };

        Position const m_StellagosaSpawnPos = { -95.34722f, 6960.229f, 13.13028f, 2.51232f };

        void sQuestAccept(Player* p_Player, Quest const* /*p_Quest*/) override
        {
            if (Creature* l_Creature = p_Player->SummonCreature(eNpcs::Npc_Stellagosa, m_StellagosaSpawnPos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
            {
                uint64 l_SummonerGuid = p_Player->GetGUID();

                l_Creature->AddDelayedEvent([l_Creature, l_SummonerGuid]() -> void
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_SummonerGuid))
                    {
                        l_Creature->AI()->Talk(0, l_SummonerGuid);
                    }
                }, 9000);

                l_Creature->AddDelayedEvent([l_Creature]() -> void
                {
                    l_Creature->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                }, 11000);

                l_Creature->AddDelayedEvent([l_Creature]() -> void
                {
                    G3D::Vector3 const Path_Stellagosa[6] =
                    {
                        { -95.34722f, 6960.229f, 16.21361f },
                        { -87.58507f, 6955.745f, 16.76666f },
                        { -62.84201f, 6942.475f, 23.62559f },
                        { -2.756944f, 6926.752f, 52.61331f },
                        { 128.0885f, 6892.419f, 94.93887f },
                        { 235.3333f, 6856.44f, 114.4939f }
                    };

                    l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyPathEnd, Path_Stellagosa, 6);
                }, 20000);
            }

            if (Creature* l_Creature = p_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
            {
                uint64 l_SummonerGuid = p_Player->GetGUID();

                l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);

                l_Creature->AddDelayedEvent([l_Creature, l_SummonerGuid]() -> void
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_SummonerGuid))
                    {
                        l_Creature->AI()->Talk(0, l_SummonerGuid);
                    }
                }, 1500);

                l_Creature->AddDelayedEvent([l_Creature]() -> void
                {
                    l_Creature->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                }, 3500);

                l_Creature->AddDelayedEvent([l_Creature]() -> void
                {
                    l_Creature->SetFacingTo(6.072163f);
                }, 7500);

                l_Creature->AddDelayedEvent([l_Creature, l_SummonerGuid]() -> void
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_SummonerGuid))
                    {
                        l_Creature->AI()->Talk(2, l_SummonerGuid);
                    }
                }, 12500);

                l_Creature->AddDelayedEvent([l_Creature, l_SummonerGuid]() -> void
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_SummonerGuid))
                    {
                        l_Creature->SetFacingToObject(l_Player);
                        l_Creature->AI()->Talk(1, l_SummonerGuid);
                    }
                }, 15500);

                l_Creature->AddDelayedEvent([l_Creature]() -> void
                {
                    l_Creature->CastSpell(l_Creature, eSpells::Spell_RavenForm, true);
                }, 24000);

                l_Creature->AddDelayedEvent([l_Creature]() -> void
                {
                    G3D::Vector3 const Path_Khadgar[6] =
                    {
                        { -100.0503f, 6965.639f, 14.85147f },
                        { -93.00347f, 6968.322f, 16.78699f },
                        { -53.77257f, 6958.885f, 18.47343f },
                        { -27.39583f, 6984.241f, 19.33832f },
                        { -4.630208f, 7016.871f, 19.33832f },
                        { 6.651042f, 7029.668f, 19.33832f }
                    };

                    l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyPathEnd, Path_Khadgar, 6);
                }, 25000);
            }
        }

        void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
        {
            if (p_PointId == ePoints::Point_FlyPathEnd)
            {
                me->DespawnOrUnsummon();
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_archmage_khadgar_93326AI(creature);
    }
};

/// Archmage Khadgar - 93354
class npc_archmage_khadgar_93354 : public CreatureScript
{
public:
    npc_archmage_khadgar_93354() : CreatureScript("npc_archmage_khadgar_93354") { }

    struct npc_archmage_khadgar_93354AI : public ScriptedAI
    {
        npc_archmage_khadgar_93354AI(Creature *creature) : ScriptedAI(creature) { }

        enum eObjectives
        {
            Obj_KhadgarLosConversationHeard = 3844300
        };

        enum eQuests
        {
            Quest_JourneyToTheRepose = 38443
        };

        enum eConversations
        {
            Conv_KhadgarLos = 2234,
            Conv_AcceptQuest = 330
        };

        void MoveInLineOfSight(Unit* p_Who) override
        {
            Player* l_Player = p_Who->ToPlayer();
            if (!l_Player)
                return;

            if (me->GetExactDist2d(l_Player) < 30.0f && l_Player->HasQuest(eQuests::Quest_JourneyToTheRepose) &&
                l_Player->GetQuestObjectiveCounter(eObjectives::Obj_KhadgarLosConversationHeard) == 0)
            {
                if (Conversation* l_Conversation = new Conversation)
                {
                    if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_KhadgarLos, l_Player, nullptr, l_Player->GetPosition()))
                        delete l_Conversation;
                }

                l_Player->QuestObjectiveOptionalSatisfy(eObjectives::Obj_KhadgarLosConversationHeard);
            }
        }

        void sQuestAccept(Player* p_Player, Quest const* /*p_Quest*/) override
        {
            if (Conversation* l_Conversation = new Conversation)
            {
                if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_AcceptQuest, p_Player, nullptr, p_Player->GetPosition()))
                    delete l_Conversation;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_archmage_khadgar_93354AI(creature);
    }
};

/// Mana-Drained Whelpling - 90219
class npc_mana_drained_whelpling_90219 : public CreatureScript
{
public:
    npc_mana_drained_whelpling_90219() : CreatureScript("npc_mana_drained_whelpling_90219") { }

    struct npc_mana_drained_whelpling_90219AI : public ScriptedAI
    {
        npc_mana_drained_whelpling_90219AI(Creature *creature) : ScriptedAI(creature) { }

        enum ePoints
        {
            Point_FlyUpEnd = 1,
            Point_FlyAwayEnd = 2
        };

        Position const m_FirstFlyPos = { 626.7275f, 6632.555f, 73.59046f };
        Position const m_SecondFlyPos = { 619.2733f, 6605.832f, 73.57236f };
        Position const m_ThirdFlyPos = { 641.783f, 6640.718f, 73.6165f };
        Position const m_FourthFlyPos = { 674.6065f, 6626.3442f, 77.1465f };

        void IsSummonedBy(Unit* p_Summoner) override
        {
            me->AddPlayerInPersonnalVisibilityList(p_Summoner->GetGUID());

            me->AddDelayedEvent([this]() -> void
            {
                me->PlayOneShotAnimKitId(7757);
                me->SendPlaySpellVisualKit(435, 0, 0);
            }, 100);

            me->AddDelayedEvent([this]() -> void
            {
                Position l_CurPos = me->GetPosition();
                l_CurPos.m_positionZ += 3.0f;

                me->SetFlag(UNIT_FIELD_BYTES_1, 50331648);
                me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyUpEnd, l_CurPos);
            }, 5000);
        }

        void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
        {
            switch (p_PointId)
            {
            case ePoints::Point_FlyUpEnd:
            {
                me->PlayDistanceSound(me, 435);

                me->AddDelayedEvent([this]() -> void
                {
                    switch (int32(me->GetPositionX()))
                    {
                    case 640:
                    {
                        me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyAwayEnd, m_FirstFlyPos);
                        break;
                    }
                    case 645:
                    {
                        me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyAwayEnd, m_SecondFlyPos);
                        break;
                    }
                    case 641:
                    {
                        me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyAwayEnd, m_ThirdFlyPos);
                        break;
                    }
                    case 649:
                    {
                        me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyAwayEnd, m_FourthFlyPos);
                        break;
                    }
                    default:
                        break;
                    }
                }, 1500);

                break;
            }
            case ePoints::Point_FlyAwayEnd:
            {
                me->DespawnOrUnsummon();
                break;
            }
            default:
                break;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mana_drained_whelpling_90219AI(creature);
    }
};

/// Agapanthus, Azurewing Keeper - 90896, 90897, 90898
class npc_agapanthus_and_keepers : public CreatureScript
{
public:
    npc_agapanthus_and_keepers() : CreatureScript("npc_agapanthus_and_keepers") { }

    struct npc_agapanthus_and_keepersAI : public ScriptedAI
    {
        npc_agapanthus_and_keepersAI(Creature *creature) : ScriptedAI(creature) { }

        enum ePoints
        {
            Point_MoveEnd = 1
        };

        enum eNpcs
        {
            Npc_Agapanthus = 90896,
            Npc_AzurewingKeeperFirst = 90897,
            Npc_AzurewingKeeperSecond = 90898
        };

        void IsSummonedBy(Unit* p_Summoner) override
        {
            uint64 l_SummonerGuid = p_Summoner->GetGUID();

            me->AddPlayerInPersonnalVisibilityList(l_SummonerGuid);

            switch (me->GetEntry())
            {
            case eNpcs::Npc_Agapanthus:
            {
                me->AddDelayedEvent([this, l_SummonerGuid]() -> void
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                    {
                        Talk(0, l_SummonerGuid);
                    }
                }, 1000);

                me->AddDelayedEvent([this]() -> void
                {
                    G3D::Vector3 const Path_Agapanthus[13] =
                    {
                        { 640.5096f, 6590.644f, 61.11145f },
                        { 638.7596f, 6586.144f, 61.86145f },
                        { 637.5096f, 6582.394f, 62.36145f },
                        { 635.5096f, 6577.894f, 63.11145f },
                        { 634.5096f, 6574.894f, 63.86145f },
                        { 633.5096f, 6572.144f, 64.36145f },
                        { 632.9861f, 6571.441f, 64.36855f },
                        { 631.4085f, 6565.842f, 66.07139f },
                        { 630.6585f, 6563.092f, 66.82139f },
                        { 629.9085f, 6561.092f, 67.32139f },
                        { 629.4085f, 6559.342f, 68.07139f },
                        { 628.6585f, 6557.342f, 68.57139f },
                        { 627.3785f, 6553.588f, 69.48518f }
                    };

                    me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_MoveEnd, Path_Agapanthus, 13, false);
                }, 5000);

                break;
            }
            case eNpcs::Npc_AzurewingKeeperFirst:
            {
                me->AddDelayedEvent([this]() -> void
                {
                    G3D::Vector3 const Path_AzurewingKeeper[14] =
                    {
                        { 642.9063f, 6588.257f, 61.22847f },
                        { 639.1563f, 6584.757f, 61.97847f },
                        { 635.6563f, 6581.257f, 62.47847f },
                        { 633.4063f, 6579.257f, 62.97847f },
                        { 631.4063f, 6577.007f, 63.97847f },
                        { 629.1771f, 6574.986f, 64.54604f },
                        { 628.2166f, 6571.202f, 65.34633f },
                        { 627.2166f, 6567.452f, 66.09633f },
                        { 626.2166f, 6564.702f, 67.09633f },
                        { 625.3871f, 6562.196f, 67.0179f },
                        { 624.3827f, 6556.65f, 68.93456f },
                        { 623.6327f, 6553.65f, 69.68456f },
                        { 623.1327f, 6550.65f, 70.68456f },
                        { 622.1934f, 6547.533f, 71.09627f }
                    };

                    me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_MoveEnd, Path_AzurewingKeeper, 14, false);
                }, 5000);

                break;
            }
            case eNpcs::Npc_AzurewingKeeperSecond:
            {
                me->AddDelayedEvent([this]() -> void
                {
                    G3D::Vector3 const Path_AzurewingKeeper[16] =
                    {
                        { 642.5044f, 6584.27f, 61.34563f },
                        { 640.5044f, 6580.02f, 62.09563f },
                        { 638.5044f, 6576.52f, 62.59563f },
                        { 637.5044f, 6574.77f, 63.34563f },
                        { 636.7544f, 6573.02f, 63.84563f },
                        { 635.2544f, 6570.27f, 64.59563f },
                        { 634.2361f, 6568.703f, 64.818f },
                        { 633.6569f, 6566.161f, 65.7968f },
                        { 633.1569f, 6564.161f, 66.2968f },
                        { 632.4069f, 6561.161f, 67.2968f },
                        { 631.9722f, 6560.063f, 67.30598f },
                        { 630.3481f, 6555.612f, 68.82007f },
                        { 630.0981f, 6553.612f, 69.57007f },
                        { 629.0981f, 6550.862f, 70.32007f },
                        { 628.0981f, 6547.862f, 71.07007f },
                        { 626.6934f, 6543.91f, 71.59627f }
                    };

                    me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_MoveEnd, Path_AzurewingKeeper, 16, false);
                }, 5000);

                break;
            }
            default:
                break;
            }
        }

        void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
        {
            if (p_PointId == ePoints::Point_MoveEnd)
            {
                me->DespawnOrUnsummon();
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_agapanthus_and_keepersAI(creature);
    }
};

/// Senegos - 89975
class npc_senegos_89975 : public CreatureScript
{
public:
    npc_senegos_89975() : CreatureScript("npc_senegos_89975") { }

    enum eSpells
    {
        Spell_TossCrystals = 179915
    };

    bool OnGossipSelect(Player* p_Player, Creature* p_Creature, uint32 p_Sender, uint32 p_Action) override
    {
        if (p_Action == GOSSIP_ACTION_TRADE)
        {
            p_Player->CastSpell(p_Player, eSpells::Spell_TossCrystals);
            p_Player->PlayerTalkClass->SendCloseGossip();
            return true;
        }
        return false;
    }

    struct npc_senegos_89975AI : public ScriptedAI
    {
        npc_senegos_89975AI(Creature *creature) : ScriptedAI(creature) { }

        enum eObjectives
        {
            Obj_SenegosLosLineHeard = 4227100
        };

        enum eQuests
        {
            Quest_TheirDyingBreaths = 42271,
            Quest_TheLastOfTheLast = 37855
        };

        enum eSpells
        {
            Spell_SummonStellagosa = 180542
        };

        Position const m_StellagosaSpawnPos = { 613.1719f, 6670.912f, 60.28928f, 0.4719191f };

        void MoveInLineOfSight(Unit* p_Who) override
        {
            Player* l_Player = p_Who->ToPlayer();
            if (!l_Player)
                return;

            if (me->GetExactDist2d(l_Player) < 30.0f && l_Player->GetQuestStatus(eQuests::Quest_TheirDyingBreaths) == QUEST_STATUS_COMPLETE &&
                l_Player->GetQuestObjectiveCounter(eObjectives::Obj_SenegosLosLineHeard) == 0)
            {
                Talk(1, l_Player->GetGUID());
                l_Player->QuestObjectiveOptionalSatisfy(eObjectives::Obj_SenegosLosLineHeard);
            }
        }

        void sQuestAccept(Player* p_Player, Quest const* p_Quest) override
        {
            if (p_Quest->GetQuestId() == eQuests::Quest_TheLastOfTheLast)
            {
                Talk(3, p_Player->GetGUID());
            }
        }

        void sQuestReward(Player* p_Player, Quest const* /*p_Quest*/, uint32 /*p_Option*/) override
        {
            p_Player->CastSpell(m_StellagosaSpawnPos, eSpells::Spell_SummonStellagosa, true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_senegos_89975AI(creature);
    }
};

/// Stellagosa - 90900
class npc_stellagosa_90900 : public CreatureScript
{
public:
    npc_stellagosa_90900() : CreatureScript("npc_stellagosa_90900") { }

    struct npc_stellagosa_90900AI : public ScriptedAI
    {
        npc_stellagosa_90900AI(Creature *creature) : ScriptedAI(creature) { }

        G3D::Vector3 const Path_Stellagosa[5] =
        {
            { 623.9844f, 6638.148f, 66.36143f },
            { 659.559f, 6577.989f, 92.85608f },
            { 693.5469f, 6545.813f, 109.6005f },
            { 724.7031f, 6516.918f, 122.4327f },
            { 771.8195f, 6495.127f, 127.522f }
        };

        enum ePoints
        {
            Point_FirstMoveEnd = 1,
            Point_SecondMoveEnd = 2,
            Point_JumpEnd = 3,
            Point_FlyPathEnd = 4
        };

        enum eNpcs
        {
            Npc_Senegos = 89975
        };

        enum eSpells
        {
            Spell_TransformToDragon = 180541
        };

        ObjectGuid m_SummonerGuid;

        void IsSummonedBy(Unit* p_Summoner) override
        {
            m_SummonerGuid = p_Summoner->GetGUID();
            uint64 l_SummonerGuid = m_SummonerGuid;

            me->AddPlayerInPersonnalVisibilityList(l_SummonerGuid);
            me->SetWalk(false);

            me->AddDelayedEvent([this, l_SummonerGuid]() -> void
            {
                if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                {
                    if (Creature* l_Creature = l_Player->FindNearestCreature(eNpcs::Npc_Senegos, 100.0f, true))
                    {
                        l_Creature->AI()->Talk(2, l_SummonerGuid);
                    }
                }
            }, 1000);

            me->AddDelayedEvent([this, l_SummonerGuid]() -> void
            {
                if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                {
                    Talk(0, l_SummonerGuid);
                }
            }, 12000);

            me->AddDelayedEvent([this]() -> void
            {
                me->GetMotionMaster()->MovePoint(ePoints::Point_FirstMoveEnd, 614.4601f, 6663.268f, 60.37697f);
            }, 14000);
        }

        void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
        {
            uint64 l_SummonerGuid = m_SummonerGuid;

            switch (p_PointId)
            {
            case ePoints::Point_FirstMoveEnd:
            {
                me->AddDelayedEvent([this]() -> void
                {
                    me->SetFacingTo(0.5585054f);
                }, 2000);

                me->AddDelayedEvent([this, l_SummonerGuid]() -> void
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                    {
                        Talk(1, l_SummonerGuid);
                    }
                }, 4000);

                me->AddDelayedEvent([this]() -> void
                {
                    me->GetMotionMaster()->MovePoint(ePoints::Point_SecondMoveEnd, 616.4844f, 6655.902f, 60.50197f);
                }, 6000);

                break;
            }
            case ePoints::Point_SecondMoveEnd:
            {
                if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                {
                    if (Creature* l_Creature = l_Player->FindNearestCreature(eNpcs::Npc_Senegos, 100.0f, true))
                    {
                        l_Creature->AI()->Talk(4, l_SummonerGuid);
                    }
                }

                me->GetMotionMaster()->MoveJump(617.422f, 6647.58f, 66.1054f, 15.0f, 15.0f, 0.0f, ePoints::Point_JumpEnd);
                break;
            }
            case ePoints::Point_JumpEnd:
            {
                DoCast(eSpells::Spell_TransformToDragon, true);
                me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyPathEnd, Path_Stellagosa, 5);
                break;
            }
            case ePoints::Point_FlyPathEnd:
            {
                me->DespawnOrUnsummon();
                break;
            }
            default:
                break;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stellagosa_90900AI(creature);
    }
};

/// Stellagosa - 90065
class npc_stellagosa_90065 : public CreatureScript
{
public:
    npc_stellagosa_90065() : CreatureScript("npc_stellagosa_90065") { }

    struct npc_stellagosa_90065AI : public ScriptedAI
    {
        npc_stellagosa_90065AI(Creature *creature) : ScriptedAI(creature) { }

        enum ePoints
        {
            Point_FlyUpEnd = 1,
            Point_FlyPathEnd = 2
        };

        void sQuestAccept(Player* p_Player, Quest const* p_Quest) override
        {
            if (Creature* l_Creature = p_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
            {
                uint64 l_SummonerGuid = p_Player->GetGUID();

                l_Creature->AddDelayedEvent([l_Creature]() -> void
                {
                    Position const l_FlyUpPos = { 1111.717f, 6758.375f, 172.5019f };

                    l_Creature->SetAIAnimKitId(0);
                    l_Creature->SetFlag(UNIT_FIELD_BYTES_1, 50331648);
                    l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyUpEnd, l_FlyUpPos);
                }, 1000);

                l_Creature->AddDelayedEvent([l_Creature, l_SummonerGuid]() -> void
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*l_Creature, l_SummonerGuid))
                    {
                        l_Creature->SetFacingToObject(l_Player);
                        l_Creature->AI()->Talk(0, l_SummonerGuid);
                    }
                }, 3000);

                l_Creature->AddDelayedEvent([l_Creature]() -> void
                {
                    G3D::Vector3 const Path_Stellagosa[6] =
                    {
                        { 1137.976f, 6721.025f, 182.7484f },
                        { 1109.102f, 6687.161f, 204.1226f },
                        { 1015.615f, 6568.643f, 274.7945f },
                        { 899.533f, 6548.318f, 304.8834f },
                        { 719.8195f, 6537.607f, 238.4945f },
                        { 646.191f, 6658.597f, 134.8695f }
                    };

                    l_Creature->SetSpeed(MOVE_FLIGHT, 2.0f);
                    l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyPathEnd, Path_Stellagosa, 6);
                }, 14000);
            }
        }

        void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
        {
            if (p_PointId == ePoints::Point_FlyPathEnd)
            {
                me->DespawnOrUnsummon();
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stellagosa_90065AI(creature);
    }
};

/// Runas the Shamed - 90372
class npc_runas_the_shamed_90372 : public CreatureScript
{
public:
    npc_runas_the_shamed_90372() : CreatureScript("npc_runas_the_shamed_90372") { }

    struct npc_runas_the_shamed_90372AI : public ScriptedAI
    {
        npc_runas_the_shamed_90372AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        enum eSpells
        {
            Spell_RipYouToPieces = 180294,
            Spell_SummonRunas = 180066
        };

        enum eEvents
        {
            Event_SaySecondLine = 1,
            Event_SayThirdLine = 2,
            Event_SayFourthLine = 3,
            Event_SayFifthLine = 4,
            Event_SaySixthLine = 5,
            Event_SaySeventhLine = 6,
            Event_SayEighthLine = 7,
            Event_SayNinthLine = 8,
            Event_Surrender = 9
        };

        enum eKillcredits
        {
            Killcredit_RunasDefeated = 90372
        };

        EventMap m_Events;
        bool m_RunasDefeated = false;

        void Reset() override
        {
            m_Events.Reset();
            me->SetReactState(ReactStates::REACT_DEFENSIVE);
            m_RunasDefeated = false;
        }

        void EnterCombat(Unit* /*victim*/) override
        {
            me->SetReactState(ReactStates::REACT_AGGRESSIVE);
            Talk(0);
            m_Events.ScheduleEvent(eEvents::Event_SaySecondLine, 5000);
            m_Events.ScheduleEvent(eEvents::Event_SayThirdLine, 10000);
            m_Events.ScheduleEvent(eEvents::Event_SayFourthLine, 15000);
            m_Events.ScheduleEvent(eEvents::Event_SayFifthLine, 20000);
            m_Events.ScheduleEvent(eEvents::Event_SaySixthLine, 25000);
            m_Events.ScheduleEvent(eEvents::Event_SaySeventhLine, 30000);
            m_Events.ScheduleEvent(eEvents::Event_SayEighthLine, 35000);
            m_Events.ScheduleEvent(eEvents::Event_SayNinthLine, 40000);
        }

        void DamageTaken(Unit* attacker, uint32& damage, SpellInfo const* /*p_SpellInfo*/) override
        {
            if (damage >= me->GetHealth())
            {
                damage = 0;
                me->SetHealth(1);

                if (!m_RunasDefeated)
                {
                    for (auto itr : me->getThreatManager().getThreatList())
                    {
                        if (Unit* l_Unit = itr->getTarget())
                        {
                            if (Player* l_Player = l_Unit->ToPlayer())
                            {
                                l_Player->CastSpell(me->GetPosition(), eSpells::Spell_SummonRunas, true);
                                l_Player->KilledMonsterCredit(eKillcredits::Killcredit_RunasDefeated);
                            }
                        }
                    }

                    m_RunasDefeated = true;
                    m_Events.Reset();
                    me->DespawnOrUnsummon();
                }
            }
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            if (!UpdateVictim())
                return;

            m_Events.Update(p_Diff);

            switch (m_Events.ExecuteEvent())
            {
            case eEvents::Event_SaySecondLine:
            {
                Talk(1);
                break;
            }
            case eEvents::Event_SayThirdLine:
            {
                Talk(2);
                break;
            }
            case eEvents::Event_SayFourthLine:
            {
                Talk(3);
                break;
            }
            case eEvents::Event_SayFifthLine:
            {
                Talk(4);
                break;
            }
            case eEvents::Event_SaySixthLine:
            {
                Talk(5);
                break;
            }
            case eEvents::Event_SaySeventhLine:
            {
                DoCast(eSpells::Spell_RipYouToPieces);
                Talk(6);
                break;
            }
            case eEvents::Event_SayEighthLine:
            {
                Talk(7);
                break;
            }
            case eEvents::Event_SayNinthLine:
            {
                Talk(8);
                m_Events.ScheduleEvent(eEvents::Event_Surrender, 2000);
                break;
            }
            case eEvents::Event_Surrender:
            {
                for (auto itr : me->getThreatManager().GetThreatList())
                {
                    if (Unit* l_Unit = itr->getTarget())
                    {
                        if (Player* l_Player = l_Unit->ToPlayer())
                        {
                            l_Player->CastSpell(me->GetPosition(), eSpells::Spell_SummonRunas, true);
                            l_Player->KilledMonsterCredit(eKillcredits::Killcredit_RunasDefeated);
                        }
                    }
                }

                m_RunasDefeated = true;
                me->DespawnOrUnsummon();
                break;
            }
            default:
                break;
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_runas_the_shamed_90372AI(p_Creature);
    }
};

/// Runas the Shamed - 90476
class npc_runas_the_shamed_90476 : public CreatureScript
{
public:
    npc_runas_the_shamed_90476() : CreatureScript("npc_runas_the_shamed_90476") { }

    struct npc_runas_the_shamed_90476AI : public ScriptedAI
    {
        npc_runas_the_shamed_90476AI(Creature *creature) : ScriptedAI(creature) { }

        enum eConversations
        {
            Conv_RunasSummoned = 3616,
            Conv_RunasEscorted = 297
        };

        enum ePoints
        {
            Point_MovePointEnd = 1
        };

        enum eKillcredits
        {
            Killcredit_EscortRunasToSenegos = 90479
        };

        enum eSpells
        {
            Spell_StandVariant = 139717
        };

        ObjectGuid m_SummonerGuid;
        Position const m_TriggerPos = { 1084.548950f, 6593.800293f, 139.654541f };
        Position const m_MovePos = { 1099.497f, 6588.107f, 139.7379f };
        bool m_HomeDestReached = false;

        void IsSummonedBy(Unit* p_Summoner) override
        {
            uint64 l_SummonerGuid = p_Summoner->GetGUID();
            m_SummonerGuid = l_SummonerGuid;

            me->AddPlayerInPersonnalVisibilityList(m_SummonerGuid);
            me->SetFacingToObject(p_Summoner);
            DoCast(eSpells::Spell_StandVariant, true);

            me->AddDelayedEvent([this, l_SummonerGuid]() -> void
            {
                if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                {
                    me->GetMotionMaster()->MoveFollow(l_Player, 1.0f, me->GetFollowAngle());
                }
            }, 2000);

            if (Conversation* l_Conversation = new Conversation)
            {
                if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_RunasSummoned, p_Summoner, nullptr, p_Summoner->GetPosition()))
                    delete l_Conversation;
            }
        }

        void MovementInform(uint32 p_Type, uint32 p_PointId) override
        {
            if (p_PointId == ePoints::Point_MovePointEnd && p_Type == POINT_MOTION_TYPE)
            {
                me->SetFacingTo(3.007808f);
                me->DespawnOrUnsummon();

                if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                {
                    l_Player->KilledMonsterCredit(eKillcredits::Killcredit_EscortRunasToSenegos);

                    if (Conversation* l_Conversation = new Conversation)
                    {
                        if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_RunasEscorted, l_Player, nullptr, l_Player->GetPosition()))
                            delete l_Conversation;
                    }
                }
            }
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            if (me->GetAreaId() == 7359 && me->GetExactDist2d(&m_TriggerPos) <= 15.0f && !m_HomeDestReached)
            {
                me->SetWalk(false);
                me->GetMotionMaster()->MovePoint(ePoints::Point_MovePointEnd, m_MovePos);
                m_HomeDestReached = true;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_runas_the_shamed_90476AI(creature);
    }
};

/// Runas the Shamed - 90406
class npc_runas_the_shamed_90406 : public CreatureScript
{
public:
    npc_runas_the_shamed_90406() : CreatureScript("npc_runas_the_shamed_90406") { }

    struct npc_runas_the_shamed_90406AI : public ScriptedAI
    {
        npc_runas_the_shamed_90406AI(Creature *creature) : ScriptedAI(creature) { }

        enum ePoints
        {
            Point_FirstPathEnd = 1,
            Point_MoveToCrystalEnd = 2,
            Point_SecondPathEnd = 3,
            Point_MoveToHomePosEnd = 4
        };

        enum eKillcredits
        {
            Killcredit_FollowRunas = 90406
        };

        enum eSpells
        {
            Spell_SummonSenegos = 180209,
            Spell_Ping = 210090
        };

        ObjectGuid m_SummonerGuid;
        Position const m_MoveToCrystalPos = { 1058.613f, 6361.432f, 128.2108f };
        Position const m_MoveToHomePos = { 1067.91f, 6287.229f, 117.3316f };

        void IsSummonedBy(Unit* p_Summoner) override
        {
            uint64 l_SummonerGuid = p_Summoner->GetGUID();
            m_SummonerGuid = l_SummonerGuid;

            me->AddPlayerInPersonnalVisibilityList(m_SummonerGuid);

            me->AddDelayedEvent([this, l_SummonerGuid]() -> void
            {
                if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                {
                    me->Mount(61926);
                    me->PlayDistanceSound(me, 4072, l_Player);
                }
            }, 2000);

            me->AddDelayedEvent([this, l_SummonerGuid]() -> void
            {
                if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                {
                    Talk(0, l_SummonerGuid);
                }
            }, 4000);

            me->AddDelayedEvent([this, l_SummonerGuid]() -> void
            {
                G3D::Vector3 const Path_Runas[45] =
                {
                    { 1105.438f, 6583.276f, 140.1573f },
                    { 1106.438f, 6582.276f, 140.1573f },
                    { 1108.688f, 6582.276f, 140.1573f },
                    { 1111.688f, 6579.026f, 140.4073f },
                    { 1113.938f, 6577.276f, 141.4073f },
                    { 1118.938f, 6573.776f, 142.9073f },
                    { 1119.88f, 6572.946f, 142.9934f },
                    { 1122.643f, 6566.616f, 145.2258f },
                    { 1123.212f, 6564.763f, 145.6872f },
                    { 1124.708f, 6559.248f, 145.7637f },
                    { 1125.958f, 6555.498f, 145.5137f },
                    { 1125.958f, 6554.498f, 145.5137f },
                    { 1127.208f, 6550.248f, 146.0137f },
                    { 1128.026f, 6547.494f, 146.0019f },
                    { 1128.517f, 6535.667f, 145.5946f },
                    { 1128.507f, 6530.869f, 145.2467f },
                    { 1128.507f, 6521.869f, 144.7467f },
                    { 1128.425f, 6518.757f, 144.4198f },
                    { 1126.92f, 6505.863f, 144.0609f },
                    { 1126.964f, 6500.916f, 143.5136f },
                    { 1126.964f, 6496.916f, 143.0136f },
                    { 1126.964f, 6490.916f, 142.5136f },
                    { 1126.964f, 6490.416f, 142.2636f },
                    { 1126.464f, 6487.416f, 141.5136f },
                    { 1126.214f, 6485.416f, 141.0136f },
                    { 1126.214f, 6483.416f, 140.2636f },
                    { 1125.477f, 6480.374f, 139.031f },
                    { 1123.792f, 6470.939f, 137.8647f },
                    { 1121.093f, 6464.905f, 137.82f },
                    { 1119.373f, 6461.613f, 137.3396f },
                    { 1114.126f, 6456.103f, 137.0045f },
                    { 1110.126f, 6451.603f, 136.5045f },
                    { 1106.189f, 6447.654f, 135.6207f },
                    { 1101.105f, 6443.51f, 135.2006f },
                    { 1095.605f, 6439.01f, 134.7006f },
                    { 1090.726f, 6435.081f, 133.6178f },
                    { 1075.234f, 6428.179f, 133.5398f },
                    { 1067.623f, 6422.728f, 133.5398f },
                    { 1064.464f, 6411.609f, 133.5203f },
                    { 1069.455f, 6403.709f, 133.5398f },
                    { 1068.614f, 6391.922f, 133.088f },
                    { 1068.486f, 6387.74f, 132.1477f },
                    { 1066.927f, 6378.165f, 131.4661f },
                    { 1066.427f, 6375.165f, 130.7161f },
                    { 1065.545f, 6372.037f, 130.0218f }
                };

                me->SetSpeed(MOVE_RUN, 1.5f);
                me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_FirstPathEnd, Path_Runas, 45, false);
            }, 8000);

            me->AddDelayedEvent([this, l_SummonerGuid]() -> void
            {
                if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                {
                    Talk(1, l_SummonerGuid);
                }
            }, 17000);

            me->AddDelayedEvent([this, l_SummonerGuid]() -> void
            {
                if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                {
                    Talk(2, l_SummonerGuid);
                }
            }, 22000);

            me->AddDelayedEvent([this, l_SummonerGuid]() -> void
            {
                if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                {
                    Talk(3, l_SummonerGuid);
                }
            }, 31000);
        }

        void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
        {
            uint64 l_SummonerGuid = m_SummonerGuid;

            switch (p_PointId)
            {
            case ePoints::Point_FirstPathEnd:
            {
                me->Dismount();
                me->SetWalk(false);
                me->GetMotionMaster()->MovePoint(ePoints::Point_MoveToCrystalEnd, m_MoveToCrystalPos);
                break;
            }
            case ePoints::Point_MoveToCrystalEnd:
            {
                me->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 398);

                me->AddDelayedEvent([this, l_SummonerGuid]() -> void
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                    {
                        me->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 0);
                        me->Mount(61926);
                        me->PlayDistanceSound(me, 4072, l_Player);
                    }
                }, 1000);

                me->AddDelayedEvent([this, l_SummonerGuid]() -> void
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                    {
                        Talk(4, l_SummonerGuid);
                        l_Player->CastSpell(l_Player, eSpells::Spell_SummonSenegos, true);
                    }

                    G3D::Vector3 const Path_Runas[18] =
                    {
                        { 1061.084f, 6362.088f, 128.8955f },
                        { 1062.084f, 6361.338f, 128.8955f },
                        { 1063.084f, 6362.088f, 128.1455f },
                        { 1066.584f, 6363.588f, 128.3955f },
                        { 1066.834f, 6359.588f, 127.6455f },
                        { 1066.834f, 6356.588f, 126.8955f },
                        { 1066.834f, 6353.588f, 126.1455f },
                        { 1066.834f, 6349.838f, 125.6455f },
                        { 1067.084f, 6345.838f, 124.8955f },
                        { 1067.056f, 6343.244f, 124.0802f },
                        { 1067.226f, 6337.44f, 123.253f },
                        { 1067.226f, 6334.44f, 122.753f },
                        { 1067.476f, 6332.44f, 122.003f },
                        { 1067.476f, 6328.44f, 121.253f },
                        { 1067.476f, 6325.44f, 120.753f },
                        { 1067.646f, 6322.993f, 119.8205f },
                        { 1068.444f, 6317.186f, 119.6827f },
                        { 1069.748f, 6305.009f, 118.8739f }
                    };

                me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_SecondPathEnd, Path_Runas, 18, false);
                }, 2000);

                break;
            }
            case ePoints::Point_SecondPathEnd:
            {
                me->Dismount();
                me->SetWalk(true);
                me->GetMotionMaster()->MovePoint(ePoints::Point_MoveToHomePosEnd, m_MoveToHomePos);

                me->AddDelayedEvent([this, l_SummonerGuid]() -> void
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                    {
                        Talk(5, l_SummonerGuid);
                    }
                }, 4000);

                break;
            }
            case ePoints::Point_MoveToHomePosEnd:
            {
                me->SetFacingTo(5.211836f);

                if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                {
                    l_Player->KilledMonsterCredit(eKillcredits::Killcredit_FollowRunas);
                    l_Player->CastSpell(l_Player, eSpells::Spell_Ping, true);
                }

                me->DespawnOrUnsummon();
                break;
            }
            default:
                break;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_runas_the_shamed_90406AI(creature);
    }
};

/// Projection of Senegos - 90567
class npc_projection_of_senegos_90567 : public CreatureScript
{
public:
    npc_projection_of_senegos_90567() : CreatureScript("npc_projection_of_senegos_90567") { }

    struct npc_projection_of_senegos_90567AI : public ScriptedAI
    {
        npc_projection_of_senegos_90567AI(Creature *creature) : ScriptedAI(creature) { }

        void IsSummonedBy(Unit* p_Summoner) override
        {
            uint64 l_SummonerGuid = p_Summoner->GetGUID();

            me->AddPlayerInPersonnalVisibilityList(l_SummonerGuid);

            me->AddDelayedEvent([this, l_SummonerGuid]() -> void
            {
                me->DespawnOrUnsummon(4000);

                if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                {
                    me->SendPlaySpellVisualKit(53437, 0, 0);
                    me->PlayDistanceSound(me, 39397, l_Player);
                    me->SetDisplayId(11686);
                }
            }, 2000);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_projection_of_senegos_90567AI(creature);
    }
};

/// Projection of Senegos - 90569
class npc_projection_of_senegos_90569 : public CreatureScript
{
public:
    npc_projection_of_senegos_90569() : CreatureScript("npc_projection_of_senegos_90569") { }

    struct npc_projection_of_senegos_90569AI : public ScriptedAI
    {
        npc_projection_of_senegos_90569AI(Creature *creature) : ScriptedAI(creature) { }

        enum eSpells
        {
            Spell_Ping = 210090
        };

        void IsSummonedBy(Unit* p_Summoner) override
        {
            uint64 l_SummonerGuid = p_Summoner->GetGUID();

            me->SetDisplayId(11686);
            me->DespawnOrUnsummon(60000);
            me->AddPlayerInPersonnalVisibilityList(l_SummonerGuid);

            me->AddDelayedEvent([this]() -> void
            {
                me->SendPlaySpellVisualKit(53437, 0, 0);
                me->SetDisplayId(69857);
            }, 2000);

            me->AddDelayedEvent([this, l_SummonerGuid]() -> void
            {
                if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                {
                    Talk(0, l_SummonerGuid);
                }
            }, 7000);
        }

        void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
        {
            if (p_Spell->Id == eSpells::Spell_Ping && p_Caster == me->GetAnyOwner())
            {
                me->DespawnOrUnsummon();
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_projection_of_senegos_90569AI(creature);
    }
};

/// Stellagosa - 107995
class npc_stellagosa_107995 : public CreatureScript
{
public:
    npc_stellagosa_107995() : CreatureScript("npc_stellagosa_107995") { }

    struct npc_stellagosa_107995AI : public VehicleAI
    {
        npc_stellagosa_107995AI(Creature *creature) : VehicleAI(creature) { }

        G3D::Vector3 const Path_StellagosaLand[8] =
        {
            { 627.7743f, 6511.967f, 78.75224f },
            { 629.2743f, 6515.467f, 78.25224f },
            { 631.0243f, 6519.217f, 77.50224f },
            { 632.7743f, 6523.467f, 77.00224f },
            { 634.0243f, 6526.217f, 76.25224f },
            { 636.0243f, 6530.967f, 75.75224f },
            { 638.0243f, 6535.467f, 75.00224f },
            { 640.132f, 6540.541f, 74.53824f }
            };

        enum eSpells
        {
            Spell_RideVehicle = 52391,
            Spell_StellagosaGuardianSceneAura = 214402,
            Spell_JumpToDestForcecast = 108517,
            Spell_SenegosScene = 214276
        };

        enum ePoints
        {
            Point_FlyPathEnd = 1,
            Point_FlyDownEnd = 2,
            Point_LandPathEnd = 3
        };

        enum eKillcredits
        {
            Killcredit_StellagosaGossipSelected = 3786200,
            Killcredit_StellagosaSlain = 107995
        };

        ObjectGuid m_SummonerGuid;
        Position const m_FlyDownPos = { 623.9167f, 6503.393f, 79.49447f };

        void sGossipSelect(Player* p_Player, uint32 /*p_Sender*/, uint32 /*p_Action*/) override
        {
            p_Player->PlayerTalkClass->SendCloseGossip();
            p_Player->QuestObjectiveOptionalSatisfy(eKillcredits::Killcredit_StellagosaGossipSelected);

            if (Creature* l_Creature = p_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, p_Player->GetGUID()))
            {
                l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, 3);
                p_Player->CastSpell(l_Creature, eSpells::Spell_RideVehicle, true);
                p_Player->AddAura(eSpells::Spell_StellagosaGuardianSceneAura, p_Player);

                l_Creature->AddDelayedEvent([l_Creature]() -> void
                {
                    G3D::Vector3 const Path_Stellagosa[19] =
                    {
                        { 1206.542f, 6055.114f, 125.746f },
                        { 1212.099f, 6076.133f, 127.0183f },
                        { 1198.29f, 6106.338f, 125.3077f },
                        { 1138.854f, 6176.171f, 119.6065f },
                        { 1062.436f, 6243.736f, 116.5943f },
                        { 999.2674f, 6257.922f, 139.4823f },
                        { 932.7917f, 6180.373f, 167.0043f },
                        { 849.9254f, 6162.858f, 172.4692f },
                        { 793.592f, 6186.552f, 137.6087f },
                        { 741.9288f, 6185.351f, 105.4227f },
                        { 690.9879f, 6191.478f, 83.87122f },
                        { 643.3993f, 6215.557f, 74.87909f },
                        { 626.3195f, 6243.08f, 71.8642f },
                        { 615.6371f, 6302.666f, 72.25303f },
                        { 612.7952f, 6322.383f, 74.53391f },
                        { 624.0695f, 6378.027f, 75.05906f },
                        { 630.7049f, 6401.642f, 83.63281f },
                        { 618.7604f, 6464.457f, 85.55235f },
                        { 623.9167f, 6503.393f, 84.12992f }
                    };

                    l_Creature->SetSpeed(MOVE_FLIGHT, 3.0f);
                    l_Creature->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyPathEnd, Path_Stellagosa, 19);
                }, 2000);
            }
        }

        void IsSummonedBy(Unit* p_Summoner) override
        {
            m_SummonerGuid = p_Summoner->GetGUID();
            uint64 l_SummonerGuid = m_SummonerGuid;

            me->AddDelayedEvent([this, l_SummonerGuid]() -> void
            {
                if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                {
                    Talk(0, l_SummonerGuid);
                }
            }, 4000);

            me->AddDelayedEvent([this, l_SummonerGuid]() -> void
            {
                if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                {
                    Talk(1, l_SummonerGuid);
                }
            }, 11000);

            me->AddDelayedEvent([this, l_SummonerGuid]() -> void
            {
                if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                {
                    Talk(2, l_SummonerGuid);
                }
            }, 20000);

            me->AddDelayedEvent([this, l_SummonerGuid]() -> void
            {
                if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                {
                    Talk(3, l_SummonerGuid);
                }
            }, 29000);
        }

        void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
        {
            switch (p_PointId)
            {
            case ePoints::Point_FlyPathEnd:
            {
                if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                {
                    l_Player->ExitVehicle();
                    DoCast(l_Player, eSpells::Spell_JumpToDestForcecast, true);
                    l_Player->CastSpell(l_Player, eSpells::Spell_SenegosScene, true);
                }

                me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FlyDownEnd, m_FlyDownPos);
                break;
            }
            case ePoints::Point_FlyDownEnd:
            {
                me->SetDisableGravity(false);
                me->RemoveFlag(UNIT_FIELD_BYTES_1, 50331648);
                me->SetSpeed(MOVE_RUN, 2.0f);
                me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_LandPathEnd, Path_StellagosaLand, 8, false);
                break;
            }
            case ePoints::Point_LandPathEnd:
            {
                me->SetFacingTo(1.631339f);

                if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                {
                    l_Player->QuestObjectiveOptionalSatisfy(eKillcredits::Killcredit_StellagosaSlain);
                    l_Player->RemoveAura(eSpells::Spell_StellagosaGuardianSceneAura);
                }

                me->DespawnOrUnsummon();
                break;
            }
            default:
                break;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stellagosa_107995AI(creature);
    }
};

/// Elder Aldryth - 89134
class npc_elder_aldryth_89134 : public CreatureScript
{
public:
    npc_elder_aldryth_89134() : CreatureScript("npc_elder_aldryth_89134") { }

    struct npc_elder_aldryth_89134AI : public ScriptedAI
    {
        npc_elder_aldryth_89134AI(Creature *creature) : ScriptedAI(creature) { }

        enum ePoints
        {
            Point_FirstPathEnd = 1,
            Point_SecondPathEnd = 2
        };

        ObjectGuid m_SummonerGuid;

        void IsSummonedBy(Unit* p_Summoner) override
        {
            m_SummonerGuid = p_Summoner->GetGUID();

            me->AddPlayerInPersonnalVisibilityList(m_SummonerGuid);

            me->AddDelayedEvent([this]() -> void
            {
                G3D::Vector3 const Path_Aldryth[2] =
                {
                    { 196.625f, 6424.188f, -20.87543f },
                    { 194.6302f, 6423.189f, -1.563004f }
                };

                me->SetSpeed(MOVE_RUN, 2.0f);
                me->SetFlag(UNIT_FIELD_BYTES_1, 50331648);
                me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_FirstPathEnd, Path_Aldryth, 2);
            }, 1000);
        }

        void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
        {
            uint64 l_SummonerGuid = m_SummonerGuid;

            switch (p_PointId)
            {
            case ePoints::Point_FirstPathEnd:
            {
                if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                {
                    me->SetFacingToObject(l_Player);
                    Talk(0, m_SummonerGuid);
                }

                me->AddDelayedEvent([this, l_SummonerGuid]() -> void
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                    {
                        Talk(1, m_SummonerGuid);
                    }
                }, 3000);

                me->AddDelayedEvent([this, l_SummonerGuid]() -> void
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                    {
                        Talk(2, m_SummonerGuid);
                    }

                    G3D::Vector3 const Path_Aldryth[4] =
                    {
                        { 187.7153f, 6447.332f, -1.770546f },
                        { 178.4115f, 6461.189f, -2.234674f },
                        { 173.8472f, 6472.158f, -1.285148f },
                        { 158.5712f, 6489.99f, -1.281286f }
                    };

                    me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_SecondPathEnd, Path_Aldryth, 4);
                }, 9000);

                break;
            }
            case ePoints::Point_SecondPathEnd:
            {
                me->DespawnOrUnsummon();
                break;
            }
            default:
                break;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_elder_aldryth_89134AI(creature);
    }
};

/// Magister Garuhod - 89663
class npc_magister_garuhod_89663 : public CreatureScript
{
public:
    npc_magister_garuhod_89663() : CreatureScript("npc_magister_garuhod_89663") { }

    struct npc_magister_garuhod_89663AI : public ScriptedAI
    {
        npc_magister_garuhod_89663AI(Creature *creature) : ScriptedAI(creature) { }

        enum ePoints
        {
            Point_PathEnd = 1
        };

        enum eConversations
        {
            Conv_GaruhodScene = 194
        };

        void IsSummonedBy(Unit* p_Summoner) override
        {
            uint64 l_SummonerGuid = p_Summoner->GetGUID();

            me->AddPlayerInPersonnalVisibilityList(l_SummonerGuid);

            if (Conversation* l_Conversation = new Conversation)
            {
                if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_GaruhodScene, p_Summoner, nullptr, p_Summoner->GetPosition()))
                    delete l_Conversation;
            }

            me->AddDelayedEvent([this, l_SummonerGuid]() -> void
            {
                if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                {
                    me->SetFacingToObject(l_Player);
                    me->SendPlaySpellVisualKit(988, 0, 0);
                    me->PlayDistanceSound(me, 18190, l_Player);
                    me->SetDisplayId(10045);
                }
            }, 10000);

            me->AddDelayedEvent([this]() -> void
            {
                G3D::Vector3 const Path_Garuhod[24] =
                {
                    { 22.83854f, 6661.19f, 49.12567f },
                    { 22.76563f, 6666.209f, 49.12567f },
                    { 19.50174f, 6669.287f, 49.12567f },
                    { 15.02778f, 6668.958f, 49.12567f },
                    { 11.97222f, 6665.285f, 49.12567f },
                    { 14.09375f, 6659.189f, 49.12567f },
                    { 19.25174f, 6657.162f, 49.12567f },
                    { 21.88368f, 6661.265f, 50.15114f },
                    { 21.7066f, 6665.556f, 50.15114f },
                    { 19.08507f, 6667.664f, 50.15114f },
                    { 15.95313f, 6667.291f, 50.15114f },
                    { 13.97917f, 6664.659f, 50.15114f },
                    { 15.68229f, 6660.262f, 50.15114f },
                    { 18.53299f, 6659.265f, 50.15114f },
                    { 20.4566f, 6661.854f, 50.70323f },
                    { 20.32813f, 6664.787f, 50.70323f },
                    { 18.24653f, 6666.275f, 50.70323f },
                    { 16.32813f, 6665.557f, 50.70323f },
                    { 15.23264f, 6663.371f, 50.70323f },
                    { 16.79861f, 6660.914f, 50.70323f },
                    { 18.97743f, 6662.259f, 51.3919f },
                    { 19.16667f, 6664.412f, 51.3919f },
                    { 17.98264f, 6665.103f, 51.3919f },
                    { 17.03125f, 6663.128f, 52.86708f }
                };

                me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_PathEnd, Path_Garuhod, 24);
            }, 13000);
        }

        void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
        {
            if (p_PointId == ePoints::Point_PathEnd)
            {
                me->SendPlaySpellVisualKit(42957, 0, 0);
                me->DespawnOrUnsummon();
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_magister_garuhod_89663AI(creature);
    }
};

/// Than, Estynna - 108480, 108479
class npc_than_and_estynna : public CreatureScript
{
public:
    npc_than_and_estynna() : CreatureScript("npc_than_and_estynna") { }

    struct npc_than_and_estynnaAI : public ScriptedAI
    {
        npc_than_and_estynnaAI(Creature *creature) : ScriptedAI(creature) { }

        enum ePoints
        {
            Point_PathEnd = 1
        };

        enum eNpcs
        {
            Npc_Than = 108480
        };

        enum eSpells
        {
            Spell_Fireball = 215335
        };

        void IsSummonedBy(Unit* p_Summoner) override
        {
            uint64 l_SummonerGuid = p_Summoner->GetGUID();

            me->AddPlayerInPersonnalVisibilityList(l_SummonerGuid);

            if (me->GetEntry() == eNpcs::Npc_Than)
            {
                me->AddDelayedEvent([this, l_SummonerGuid]() -> void
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                    {
                        G3D::Vector3 const Path_Than[18] =
                        {
                            { 106.118f, 6210.251f, 3.690873f },
                            { 104.118f, 6212.251f, 3.690873f },
                            { 103.118f, 6213.001f, 4.690873f },
                            { 102.118f, 6213.501f, 4.940873f },
                            { 101.118f, 6213.751f, 4.440873f },
                            { 99.86805f, 6214.501f, 3.690873f },
                            { 98.36805f, 6215.501f, 3.440873f },
                            { 94.11805f, 6217.251f, 3.440873f },
                            { 93.11805f, 6219.251f, 3.440873f },
                            { 91.11805f, 6220.501f, 3.440873f },
                            { 89.61805f, 6221.001f, 3.440873f },
                            { 84.36805f, 6223.251f, 3.690873f },
                            { 82.61805f, 6224.001f, 3.440873f },
                            { 80.86805f, 6224.751f, 3.440873f },
                            { 79.6441f, 6225.153f, 3.322223f },
                            { 76.71114f, 6226.591f, 3.309046f },
                            { 72.96114f, 6228.091f, 3.309046f },
                            { 71.11638f, 6228.813f, 3.25159f }
                        };

                        me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_PathEnd, Path_Than, 18, false);
                    }
                }, 14000);
            }
            else
            {
                me->AddDelayedEvent([this, l_SummonerGuid]() -> void
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                    {
                        Talk(0, l_SummonerGuid);
                    }
                }, 2000);

                me->AddDelayedEvent([this, l_SummonerGuid]() -> void
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                    {
                        Talk(1, l_SummonerGuid);
                    }
                }, 7000);

                me->AddDelayedEvent([this, l_SummonerGuid]() -> void
                {
                    if (Creature* l_Creature = me->FindNearestCreature(eNpcs::Npc_Than, 10.0f, true))
                    {
                        me->SetFacingToObject(l_Creature);
                        DoCast(l_Creature, eSpells::Spell_Fireball, true);
                    }

                    if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                    {
                        Talk(2, l_SummonerGuid);
                    }
                }, 11000);

                me->AddDelayedEvent([this, l_SummonerGuid]() -> void
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                    {
                        G3D::Vector3 const Path_Estynna[17] =
                        {
                            { 101.7856f, 6209.963f, 3.690874f },
                            { 100.7856f, 6210.713f, 3.940874f },
                            { 99.0356f, 6211.963f, 3.440874f },
                            { 97.5356f, 6212.963f, 3.440874f },
                            { 96.2856f, 6213.963f, 3.440874f },
                            { 93.5356f, 6215.713f, 3.440874f },
                            { 92.5356f, 6216.213f, 4.690874f },
                            { 91.7856f, 6216.963f, 4.690874f },
                            { 90.7856f, 6217.463f, 4.440874f },
                            { 89.2856f, 6218.713f, 3.940874f },
                            { 88.2856f, 6219.213f, 3.440874f },
                            { 84.0356f, 6222.213f, 3.440874f },
                            { 82.2856f, 6223.463f, 3.440874f },
                            { 80.7856f, 6224.463f, 3.440874f },
                            { 79.6441f, 6225.153f, 3.322223f },
                            { 77.10031f, 6227.225f, 3.261139f },
                            { 73.99211f, 6229.075f, 3.184421f }
                        };

                        me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_PathEnd, Path_Estynna, 17, false);
                    }
                }, 12000);
            }
        }

        void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
        {
            if (p_PointId == ePoints::Point_PathEnd)
            {
                me->DespawnOrUnsummon();
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_than_and_estynnaAI(creature);
    }
};

/// Athissa - 88091
class npc_athissa_88091 : public CreatureScript
{
public:
    npc_athissa_88091() : CreatureScript("npc_athissa_88091") { }

    struct npc_athissa_88091AI : public ScriptedAI
    {
        npc_athissa_88091AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        enum eSpells
        {
            Spell_FrostPrison = 197644,
            Spell_CosmeticAlphaState = 80808,
            Spell_Frostbolt = 15497,
            Spell_CaptiveTide = 212091,
            Spell_WaveCrush = 212113,
            Spell_FrostPrisonPlayers = 179056,
            Spell_Teleport = 164233
        };

        enum eEvents
        {
            Event_CastFrostbolt = 1,
            Event_CastCaptiveTide = 2,
            Event_CastWaveCrush = 3
        };

        enum eKillcredits
        {
            Killcredit_AthissaDefeated = 88091
        };

        enum eObjectives
        {
            Obj_AthissaLosLineHeard = 3748600
        };

        enum eQuests
        {
            Quest_NarthalasStillSuffers = 37486
        };

        enum eNpcs
        {
            Npc_NarthalasCitizen = 89634
        };

        EventMap m_Events;
        bool m_CitizensKilled = false;
        bool m_AthissaDefeated = false;

        void Reset() override
        {
            m_Events.Reset();
            m_CitizensKilled = false;
            m_AthissaDefeated = false;

            std::list<Creature*> l_CitizensList;
            GetCreatureListWithEntryInGrid(l_CitizensList, me, eNpcs::Npc_NarthalasCitizen, 35.0f);

            if (l_CitizensList.empty())
                return;

            for (auto itr : l_CitizensList)
            {
                itr->Respawn();
            }
        }

        void MoveInLineOfSight(Unit* p_Who) override
        {
            Player* l_Player = p_Who->ToPlayer();
            if (!l_Player)
                return;

            if (me->GetExactDist2d(l_Player) < 35.0f && l_Player->HasQuest(eQuests::Quest_NarthalasStillSuffers) &&
                l_Player->GetQuestObjectiveCounter(eObjectives::Obj_AthissaLosLineHeard) == 0 && !m_CitizensKilled)
            {
                Talk(0, l_Player->GetGUID());
                l_Player->QuestObjectiveOptionalSatisfy(eObjectives::Obj_AthissaLosLineHeard);
            }
        }

        void EnterCombat(Unit* /*victim*/) override
        {
            Talk(1);

            std::list<Creature*> l_CitizensList;
            GetCreatureListWithEntryInGrid(l_CitizensList, me, eNpcs::Npc_NarthalasCitizen, 35.0f);

            if (l_CitizensList.empty())
                return;

            for (auto itr : l_CitizensList)
            {
                if (itr->HasAura(eSpells::Spell_FrostPrison))
                {
                    itr->SendPlaySpellVisualKit(61499, 0, 0);
                    itr->CastSpell(itr, eSpells::Spell_CosmeticAlphaState, true);
                    itr->DespawnOrUnsummon(1000);
                }
                else
                {
                    itr->SetWalk(false);
                    itr->GetMotionMaster()->MoveRandom(20.0f);
                    itr->DespawnOrUnsummon(3000);
                }
            }

            m_Events.ScheduleEvent(eEvents::Event_CastFrostbolt, 1000);
            m_Events.ScheduleEvent(eEvents::Event_CastCaptiveTide, 10000);
            m_Events.ScheduleEvent(eEvents::Event_CastWaveCrush, 16000);
            m_CitizensKilled = true;
        }

        void DamageTaken(Unit* attacker, uint32& damage, SpellInfo const* /*p_SpellInfo*/) override
        {
            if (damage >= me->GetHealth())
            {
                damage = 0;
                me->SetHealth(1);

                if (!m_AthissaDefeated)
                {
                    Talk(2);
                    me->CastStop();
                    me->StopAttack();
                    m_Events.Reset();
                    DoCastAOE(eSpells::Spell_FrostPrisonPlayers, true);

                    me->AddDelayedEvent([this]() -> void
                    {
                        DoCast(eSpells::Spell_Teleport, true);
                        DoCast(eSpells::Spell_CosmeticAlphaState, true);

                        for (auto itr : me->getThreatManager().getThreatList())
                        {
                            if (Unit* l_Unit = itr->getTarget())
                            {
                                if (Player* l_Player = l_Unit->ToPlayer())
                                {
                                    l_Player->KilledMonsterCredit(eKillcredits::Killcredit_AthissaDefeated);
                                }
                            }
                        }

                        me->DespawnOrUnsummon();
                    }, 4000);

                    m_AthissaDefeated = true;
                }
            }
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            if (!UpdateVictim())
                return;

            m_Events.Update(p_Diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            switch (m_Events.ExecuteEvent())
            {
            case eEvents::Event_CastFrostbolt:
            {
                DoCastVictim(eSpells::Spell_Frostbolt);
                m_Events.ScheduleEvent(eEvents::Event_CastFrostbolt, urand(1000, 6000));
                break;
            }
            case eEvents::Event_CastCaptiveTide:
            {
                DoCast(eSpells::Spell_CaptiveTide);
                m_Events.ScheduleEvent(eEvents::Event_CastCaptiveTide, urand(10000, 20000));
                break;
            }
            case eEvents::Event_CastWaveCrush:
            {
                DoCast(eSpells::Spell_WaveCrush);
                m_Events.ScheduleEvent(eEvents::Event_CastWaveCrush, urand(10000, 20000));
                break;
            }
            default:
                break;
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new npc_athissa_88091AI(p_Creature);
    }
};

/// Instructor Nidriel - 89661
class npc_instructor_nidriel_89661 : public CreatureScript
{
public:
    npc_instructor_nidriel_89661() : CreatureScript("npc_instructor_nidriel_89661") { }

    struct npc_instructor_nidriel_89661AI : public ScriptedAI
    {
        npc_instructor_nidriel_89661AI(Creature *creature) : ScriptedAI(creature) { }

        enum eQuests
        {
            Quest_Wanding = 42370,
            Quest_PopQuizAdvancedRuneDrawing = 37729
        };

        enum eSpells
        {
            Spell_WandPractice = 212782,
            Spell_RuneDrawingRuneA = 179151
        };

        enum eNpcs
        {
            Npc_WandTarget = 107279,
            Npc_Sythorne = 107334
        };

        enum eMisc
        {
            Misc_WandPracticeOverride = 711
        };

        struct sWandTargetGuidAndPos
        {
            uint64 m_Guid;
            Position m_Pos;
        };

        std::vector<sWandTargetGuidAndPos> m_WandTargets =
        {
            { 0, Position(203.9028f, 6440.327f, -53.69449f, 1.758775f)  },
            { 0, Position(188.9931f, 6431.245f, -52.8118f, 1.112558f)   },
            { 0, Position(169.5972f, 6457.395f, -53.1122f, 4.720047f)   },
            { 0, Position(179.9826f, 6440.074f, -52.80374f, 0.4902049f) },
            { 0, Position(189.5052f, 6448.728f, -53.69449f, 0.2087934f) },
            { 0, Position(223.8281f, 6430.567f, -53.1134f, 3.899321f)   },
            { 0, Position(201.2969f, 6463.351f, -53.69449f, 4.365468f)  },
            { 0, Position(195.4792f, 6464.325f, -53.69449f, 5.199088f)  },
            { 0, Position(178.2969f, 6429.004f, -53.11722f, 5.730799f)  },
            { 0, Position(196.0069f, 6420.08f, -53.11263f, 3.026369f)   },
            { 0, Position(201.3438f, 6427.701f, -52.80354f, 1.627967f)  },
            { 0, Position(210.6129f, 6458.342f, -53.69449f, 4.138951f)  }
        };

        void sQuestAccept(Player* p_Player, Quest const* p_Quest) override
        {
            if (p_Quest->GetQuestId() == eQuests::Quest_Wanding)
            {
                p_Player->SetUInt16Value(PLAYER_FIELD_OVERRIDE_SPELLS_ID, PLAYER_FIELD_OVERRIDE_SPELLS_ID_OFFSET_OVERRIDE_SPELLS_ID, eMisc::Misc_WandPracticeOverride);
                p_Player->AddTemporarySpell(eSpells::Spell_WandPractice);

            }
            else if (p_Quest->GetQuestId() == eQuests::Quest_PopQuizAdvancedRuneDrawing)
            {
                Talk(1, p_Player->GetGUID());
                p_Player->CastSpell(p_Player, eSpells::Spell_RuneDrawingRuneA, true);
            }
        }

        void sQuestReward(Player* p_Player, Quest const* p_Quest, uint32 /*p_Option*/) override
        {
            if (p_Quest->GetQuestId() == eQuests::Quest_Wanding)
            {
                if (Creature* l_Creature = p_Player->FindNearestCreature(eNpcs::Npc_Sythorne, 20.0f, true))
                {
                    l_Creature->AI()->Talk(0, p_Player->GetGUID());
                }
            }
        }

        void InitializeAI() override
        {
            std::random_shuffle(m_WandTargets.begin(), m_WandTargets.end());

            for (uint8 i = 0; i < 5; i++)
            {
                if (Creature* l_Creature = me->SummonCreature(eNpcs::Npc_WandTarget, m_WandTargets[i].m_Pos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                {
                    l_Creature->SetPhaseMask(0, true);
                    l_Creature->SetInPhase(42370, true, true);
                    m_WandTargets[i].m_Guid = l_Creature->GetGUID();
                }
            }
        }

        void SetGUID(uint64 p_Guid, int32 /*p_ID = 0*/) override
        {
            std::random_shuffle(m_WandTargets.begin(), m_WandTargets.end());

            for (int8 i = 0; i < m_WandTargets.size(); i++)
            {
                if (m_WandTargets[i].m_Guid == 0)
                {
                    if (Creature* l_Creature = me->SummonCreature(eNpcs::Npc_WandTarget, m_WandTargets[i].m_Pos, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        l_Creature->SetPhaseMask(0, true);
                        l_Creature->SetInPhase(42370, true, true);
                        m_WandTargets[i].m_Guid = l_Creature->GetGUID();
                        break;
                    }
                }
            }

            for (int8 i = 0; i < m_WandTargets.size(); i++)
            {
                if (m_WandTargets[i].m_Guid == p_Guid)
                {
                    m_WandTargets[i].m_Guid = 0;
                    break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_instructor_nidriel_89661AI(creature);
    }
};

/// Demon Ward - 239452, 239984, 239985
class gob_azsuna_demon_ward : public GameObjectScript
{
public:
    gob_azsuna_demon_ward() : GameObjectScript("gob_azsuna_demon_ward") { }

    struct gob_azsuna_demon_wardAI : public GameObjectAI
    {
        gob_azsuna_demon_wardAI(GameObject* go) : GameObjectAI(go) { }

        enum eNpcs
        {
            Npc_DemonHunterFirst = 86969,
            Npc_DemonHunterSecond = 89278
        };

        enum eKillcredits
        {
            Killcredit_SouthWardVisible = 3765800,
            Killcredit_WestWardVisible = 3765801,
            Killcredit_NorthWardVisible = 3765802
        };

        enum eGobs
        {
            Gob_NorthWard = 239985,
            Gob_WestWard = 239984
        };

        void InitializeAI() override
        {
            if (!go->GetPhaseMask())
            {
                go->SetUInt32Value(GAMEOBJECT_FIELD_STATE_ANIM_KIT_ID, 3761);
                go->SetFlag(GAMEOBJECT_FIELD_FLAGS, 20);
            }
        }

        void OnLootStateChanged(uint32 p_State, Unit* p_Unit) override
        {
            if (p_State == 2)
            {
                if (Player* l_Player = p_Unit->ToPlayer())
                {
                    if (go->GetEntry() == eGobs::Gob_NorthWard)
                    {
                        if (Creature* l_Creature = go->FindNearestCreature(eNpcs::Npc_DemonHunterFirst, 10.0f, true))
                        {
                            l_Creature->AI()->Talk(0, l_Player->GetGUID());
                        }

                        l_Player->AddDelayedEvent([l_Player]() -> void
                        {
                            l_Player->QuestObjectiveOptionalSatisfy(eKillcredits::Killcredit_NorthWardVisible);
                        }, 600);
                    }
                    else if (go->GetEntry() == eGobs::Gob_WestWard)
                    {
                        if (Creature* l_Creature = go->FindNearestCreature(eNpcs::Npc_DemonHunterSecond, 10.0f, true))
                        {
                            l_Creature->AI()->Talk(1, l_Player->GetGUID());
                        }

                        l_Player->AddDelayedEvent([l_Player]() -> void
                        {
                            l_Player->QuestObjectiveOptionalSatisfy(eKillcredits::Killcredit_WestWardVisible);
                        }, 600);
                    }
                    else
                    {
                        l_Player->AddDelayedEvent([l_Player]() -> void
                        {
                            l_Player->QuestObjectiveOptionalSatisfy(eKillcredits::Killcredit_SouthWardVisible);
                        }, 600);
                    }
                }
            }
        }
    };

    GameObjectAI* GetAI(GameObject* go) const override
    {
        return new gob_azsuna_demon_wardAI(go);
    }
};

/// Sythorne's Podium - 250362
class gob_sythornes_podium_250362 : public GameObjectScript
{
public:
    gob_sythornes_podium_250362() : GameObjectScript("gob_sythornes_podium_250362") { }

    struct gob_sythornes_podium_250362AI : public GameObjectAI
    {
        gob_sythornes_podium_250362AI(GameObject* go) : GameObjectAI(go) { }

        enum eObjectives
        {
            Obj_KoboldDrawingSlain = 2787013,
            Obj_SeaSkrogDrawingSlain = 2787016,
            Obj_SenegosDrawingSlain = 2787019
        };

        enum eItems
        {
            Item_EncyclopediaAzsunica = 137423,
            Item_CourtshipRitualsOfTheSkrog = 137422,
            Item_DraconicCompendium = 137426
        };

        enum eSpells
        {
            Spell_BookCreditFirst = 212912,
            Spell_BookCreditSecond = 212913,
            Spell_BookCreditThird = 212914,
            Spell_BetweenThePagesKobold = 212949,
            Spell_BetweenThePagesSkrog = 212956,
            Spell_BetweenThePagesSenegos = 212957
        };

        void OnLootStateChanged(uint32 p_State, Unit* p_Unit) override
        {
            if (p_State == 2)
            {
                if (Player* l_Player = p_Unit->ToPlayer())
                {
                    if (l_Player->HasItemCount(eItems::Item_EncyclopediaAzsunica) && !l_Player->GetQuestObjectiveCounter(eObjectives::Obj_KoboldDrawingSlain))
                    {
                        l_Player->CastSpell(l_Player, eSpells::Spell_BookCreditFirst, true);
                        l_Player->DelayedCastSpell(l_Player, eSpells::Spell_BetweenThePagesKobold, true, 3000);
                        return;
                    }
                    else if (l_Player->HasItemCount(eItems::Item_CourtshipRitualsOfTheSkrog) && !l_Player->GetQuestObjectiveCounter(eObjectives::Obj_SeaSkrogDrawingSlain) && l_Player->GetQuestObjectiveCounter(eObjectives::Obj_KoboldDrawingSlain))
                    {
                        l_Player->CastSpell(l_Player, eSpells::Spell_BookCreditSecond, true);
                        l_Player->DelayedCastSpell(l_Player, eSpells::Spell_BetweenThePagesSkrog, true, 3000);
                        return;
                    }
                    else if (l_Player->HasItemCount(eItems::Item_DraconicCompendium) && !l_Player->GetQuestObjectiveCounter(eObjectives::Obj_SenegosDrawingSlain) && l_Player->GetQuestObjectiveCounter(eObjectives::Obj_SeaSkrogDrawingSlain))
                    {
                        l_Player->CastSpell(l_Player, eSpells::Spell_BookCreditThird, true);
                        l_Player->DelayedCastSpell(l_Player, eSpells::Spell_BetweenThePagesSenegos, true, 3000);
                        return;
                    }
                }
            }
        }
    };

    GameObjectAI* GetAI(GameObject* go) const override
    {
        return new gob_sythornes_podium_250362AI(go);
    }
};

// Wand Practice - 212782 - 7042
class at_azsuna_wand_practice : public AreaTriggerEntityScript
{
public:
    at_azsuna_wand_practice() : AreaTriggerEntityScript("at_azsuna_wand_practice") { }

    enum eNpcs
    {
        Npc_WandTarget = 107279,
        Npc_InstructorNidriel = 89661
    };

    enum eKillcredits
    {
        Killcredit_WandTarget = 107279
    };

    Position m_SpawnPos;

    void OnCreate(AreaTrigger* p_AreaTrigger) override
    {
        if (Unit* l_Owner = p_AreaTrigger->GetCaster())
        {
            p_AreaTrigger->SetPhaseMask(l_Owner->GetPhaseMask(), true);
            p_AreaTrigger->SetInPhase(42370, true, true);
        }

        m_SpawnPos = p_AreaTrigger->GetPosition();
    }

    bool OnAddTarget(AreaTrigger* p_Areatrigger, Unit* p_Target) override
    {
        if (p_Target->GetEntry() == eNpcs::Npc_WandTarget)
        {
            if (p_Target->GetExactDist(&m_SpawnPos) <= 5.0f)
            {
                if (Unit* l_Owner = p_Areatrigger->GetCaster())
                {
                    if (Creature* l_Creature = l_Owner->FindNearestCreature(eNpcs::Npc_InstructorNidriel, 50.0f, true))
                    {
                        l_Creature->AI()->Talk(0, l_Owner->GetGUID());
                    }
                }
            }

            if (Unit* l_Owner = p_Target->GetAnyOwner())
            {
                if (p_Target->ToCreature() && l_Owner->ToCreature())
                {
                    l_Owner->ToCreature()->AI()->SetGUID(p_Target->GetGUID());
                    p_Target->HandleEmoteCommand(EMOTE_ONESHOT_COMBATCRITICAL);
                    p_Target->ToCreature()->DespawnOrUnsummon();
                }
            }

            if (Unit* l_Owner = p_Areatrigger->GetCaster())
            {
                if (l_Owner->ToPlayer())
                {
                    l_Owner->ToPlayer()->KilledMonsterCredit(eKillcredits::Killcredit_WandTarget);
                }
            }
        }

        return false;
    }

    AreaTriggerEntityScript* GetAI() const override
    {
        return new at_azsuna_wand_practice();
    }
};

/// Demon Souls: Soul Gem Ping - 178974
class spell_azsuna_soul_gem_ping : public SpellScriptLoader
{
public:
    spell_azsuna_soul_gem_ping() : SpellScriptLoader("spell_azsuna_soul_gem_ping") { }

    class spell_azsuna_soul_gem_ping_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_azsuna_soul_gem_ping_SpellScript);

        enum eQuests
        {
            Quest_DemonSouls = 37653
        };

        enum eNpcs
        {
            Npc_SoulGem = 86963
        };

        enum eSpells
        {
            Spell_SoulMissile = 178753,
            Spell_SoulGemTransform = 178752,
            Spell_SoulGemPing = 178974,
            Spell_SoulTrapped = 178763
        };

        enum eKillcredits
        {
            Killcredit_DemonSoulsTrapped = 90298
        };

        void HandleAreaTargetSelect(std::list<WorldObject*>& p_Targets)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            p_Targets.remove_if([l_Caster](WorldObject* p_Target) -> bool
            {
                if (Creature* l_Creature = p_Target->ToCreature())
                {
                    if (l_Creature->isAlive())
                        return true;

                    if (!l_Creature->HasAura(eSpells::Spell_SoulGemPing, l_Caster->GetGUID()))
                        return false;
                }

                return true;
            });
        }

        void HandleAfterHit()
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetHitUnit();
            if (!l_Caster || !l_Caster->ToPlayer() || !l_Target)
                return;

            std::list<Creature*> l_SoulGemsList;
            GetCreatureListWithEntryInGrid(l_SoulGemsList, l_Target, eNpcs::Npc_SoulGem, 15.0f);

            if (l_SoulGemsList.empty())
                return;

            for (auto itr : l_SoulGemsList)
            {
                if (itr->GetAnyOwner()->GetGUID() == l_Caster->GetGUID())
                {
                    l_Caster->ToPlayer()->KilledMonsterCredit(eKillcredits::Killcredit_DemonSoulsTrapped);
                    l_Target->SendPlaySpellVisualKit(21930, 0, 0);
                    l_Target->CastSpell(l_Target, eSpells::Spell_SoulTrapped, true);
                    l_Target->CastSpell(itr, eSpells::Spell_SoulMissile, true);
                }
            }
        }

        void Register() override
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_azsuna_soul_gem_ping_SpellScript::HandleAreaTargetSelect, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
            AfterHit += SpellHitFn(spell_azsuna_soul_gem_ping_SpellScript::HandleAfterHit);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_azsuna_soul_gem_ping_SpellScript();
    }
};

/// Toss Crystals - 179915, 179913
class spell_azsuna_toss_crystals : public SpellScriptLoader
{
public:
    spell_azsuna_toss_crystals() : SpellScriptLoader("spell_azsuna_toss_crystals") { }

    class spell_azsuna_toss_crystals_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_azsuna_toss_crystals_SpellScript);

        std::array<Position const, 6> m_CrystalsDestPos =
        { {
            { 657.6094f, 6682.875f, 56.94364f, 2.406769f },
            { 661.7782f, 6680.714f, 56.94364f, 2.406769f },
            { 655.0085f, 6681.035f, 56.94364f, 2.406769f },
            { 660.1054f, 6676.035f, 56.94364f, 2.406769f },
            { 658.1044f, 6675.773f, 56.94364f, 2.406769f },
            { 654.7281f, 6679.082f, 56.94364f, 2.406769f }
        } };

        enum eSpells
        {
            Spell_TossCrystals = 179913
        };

        void HandleAfterCast()
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            for (auto itr : m_CrystalsDestPos)
            {
                l_Caster->CastSpell(itr, eSpells::Spell_TossCrystals, true);
            }
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_azsuna_toss_crystals_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_azsuna_toss_crystals_SpellScript();
    }

    class spell_azsuna_toss_crystals_SpellScript_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_azsuna_toss_crystals_SpellScript_AuraScript);

        enum eKillcredits
        {
            Killcredit_ThrowCrystalsIntoSenegosPool = 90315
        };

        enum eNpcs
        {
            Npc_Senegos = 89975
        };

        void HandlePeriodic(AuraEffect const* /*aurEff*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster || !l_Caster->ToPlayer())
                return;

            if (Creature* l_Creature = l_Caster->FindNearestCreature(eNpcs::Npc_Senegos, 100.0f, true))
            {
                l_Creature->AI()->Talk(0, l_Caster->GetGUID(), 1);
                l_Caster->ToPlayer()->KilledMonsterCredit(eKillcredits::Killcredit_ThrowCrystalsIntoSenegosPool);
            }
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_azsuna_toss_crystals_SpellScript_AuraScript::HandlePeriodic, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_azsuna_toss_crystals_SpellScript_AuraScript();
    }
};

/// Resuscitating - 180713
class spell_azsuna_resuscitating : public SpellScriptLoader
{
public:
    spell_azsuna_resuscitating() : SpellScriptLoader("spell_azsuna_resuscitating") { }

    class spell_azsuna_resuscitating_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_azsuna_resuscitating_SpellScript);

        enum eSpells
        {
            Spell_SummonWhelpling = 179810,
            Spell_SummonDeadWhelpling = 180722
        };

        enum eObjectives
        {
            Obj_ManaDrainedWhelplingRevived = 284015
        };

        enum eNpcs
        {
            Npc_Agapanthus = 90543
        };

        void HandleHitTarget(SpellEffIndex /*p_EffectIndex*/)
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetHitUnit();
            if (!l_Caster || !l_Caster->ToPlayer() || !l_Target)
                return;

            l_Target->DestroyForPlayer(l_Caster->ToPlayer());

            if (l_Caster->ToPlayer()->GetQuestObjectiveCounter(eObjectives::Obj_ManaDrainedWhelplingRevived) == 3)
            {
                if (Creature* l_Creature = l_Caster->FindNearestCreature(eNpcs::Npc_Agapanthus, 50.0f, true))
                {
                    l_Creature->AI()->Talk(0, l_Caster->GetGUID());
                }

                l_Caster->CastSpell(l_Target->GetPosition(), eSpells::Spell_SummonDeadWhelpling, true);
            }
            else
            {
                l_Caster->CastSpell(l_Target->GetPosition(), eSpells::Spell_SummonWhelpling, true);
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_azsuna_resuscitating_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_azsuna_resuscitating_SpellScript();
    }
};

/// Mana Surge - 197482
class spell_azsuna_mana_surge : public SpellScriptLoader
{
public:
    spell_azsuna_mana_surge() : SpellScriptLoader("spell_azsuna_mana_surge") { }

    class spell_azsuna_mana_surge_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_azsuna_mana_surge_SpellScript);

        void HandleAfterCast()
        {
            Unit* l_Caster = GetCaster();
            WorldLocation const* l_WorldLoc = GetExplTargetDest();
            if (!l_Caster || !l_WorldLoc)
                return;

            l_Caster->PlayOrphanSpellVisual(G3D::Vector3(1225.04f, 6053.44f, 130.433f), G3D::Vector3(0, 0, 0), G3D::Vector3(l_WorldLoc->GetPositionX(), l_WorldLoc->GetPositionY(), l_WorldLoc->GetPositionZ()), 51291, 4.0f, 0, true, 0.0f);
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_azsuna_mana_surge_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_azsuna_mana_surge_SpellScript();
    }
};

/// Freeing - 178389
class spell_azsuna_freeing : public SpellScriptLoader
{
public:
    spell_azsuna_freeing() : SpellScriptLoader("spell_azsuna_freeing") { }

    class spell_azsuna_freeing_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_azsuna_freeing_SpellScript);

        enum eSpells
        {
            Spell_SummonAldryth = 178392
        };

        enum eKillcredits
        {
            Killcredit_ElderAldrythFound = 88743
        };

        Position const m_AldrythSummonPos = { 201.0521f, 6424.347f, -22.13207f, 3.902813f };

        void HandleAfterCast()
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster || !l_Caster->ToPlayer())
                return;

            l_Caster->ToPlayer()->KilledMonsterCredit(eKillcredits::Killcredit_ElderAldrythFound);
            l_Caster->CastSpell(m_AldrythSummonPos, eSpells::Spell_SummonAldryth, true);
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_azsuna_freeing_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_azsuna_freeing_SpellScript();
    }
};

/// Between the Pages - 212949, 212956, 212957
class spell_azsuna_between_the_pages : public SpellScriptLoader
{
public:
    spell_azsuna_between_the_pages() : SpellScriptLoader("spell_azsuna_between_the_pages") { }

    class spell_azsuna_between_the_pages_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_azsuna_between_the_pages_SpellScript);

        enum eSpells
        {
            Spell_BetweenThePagesKobold = 212949,
            Spell_BetweenThePagesSkrog = 212956,
            Spell_BetweenThePagesSenegos = 212957
        };

        enum eNpcs
        {
            Npc_KoboldDrawing = 107301,
            Npc_SeaSkrogDrawing = 107300,
            Npc_SenegosDrawing = 107299
        };

        Position const m_KoboldSpawnPos = { 201.151f, 6451.206f, -53.69449f, 2.129485f };
        Position const m_SkrogSpawnPos = { 202.2431f, 6454.12f, -53.69449f, 2.289265f };
        Position const m_SenegosSpawnPos = { 201.0417f, 6451.967f, -53.69449f, 2.148296f };

        void HandleAfterCast()
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            switch (m_scriptSpellId)
            {
            case eSpells::Spell_BetweenThePagesKobold:
            {
                l_Caster->SummonCreature(eNpcs::Npc_KoboldDrawing, m_KoboldSpawnPos, TempSummonType::TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 300000, l_Caster->GetGUID());
                break;
            }
            case eSpells::Spell_BetweenThePagesSkrog:
            {
                l_Caster->SummonCreature(eNpcs::Npc_SeaSkrogDrawing, m_SkrogSpawnPos, TempSummonType::TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 300000, l_Caster->GetGUID());
                break;
            }
            case eSpells::Spell_BetweenThePagesSenegos:
            {
                l_Caster->SummonCreature(eNpcs::Npc_SenegosDrawing, m_SenegosSpawnPos, TempSummonType::TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 300000, l_Caster->GetGUID());
                break;
            }
            default:
                break;
            }
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_azsuna_between_the_pages_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_azsuna_between_the_pages_SpellScript();
    }
};

class playerscript_azsuna_scripts : public PlayerScript
{
public:
    playerscript_azsuna_scripts() : PlayerScript("playerscript_azsuna_scripts") {}

    enum eQuests
    {
        Quest_DarkRevelations = 37449,
        Quest_TheDeathOfTheEldest = 37853,
        Quest_Agapanthus = 37991,
        Quest_TheirDyingBreaths = 42271,
        Quest_LeylineAbuse = 37960,
        Quest_TheNightbornePrince = 37861,
        Quest_ARatherLongWalk = 37492,
        Quest_TheMagisterOfMixology = 37727,
        Quest_PresentationIsEverything = 37728,
        Quest_NarthalasStillSuffers = 37486,
        Quest_YouScratchMyBack = 37860,
        Quest_CursedToWither = 42567,
        Quest_StudyHallCombatResearch = 42371,
        Quest_Wanding = 42370,
        Quest_PopQuizAdvancedRuneDrawing = 37729
    };

    enum eConversations
    {
        Conv_AcceptQuestDarkRevelations = 1385,
        Conv_RewardQuestTheDeathOfTheEldest = 254,
        Conv_RewardQuestARatherLongWalk = 184,
        Conv_AcceptQuestNarthalasStillSuffers = 161,
        Conv_AcceptQuestStudyHallCombatResearch = 2263,
        Conv_KoboldDrawingSlain = 2264,
        Conv_SeaSkrogDrawingSlain = 2265
    };

    enum eItems
    {
        Item_CrackedLeyCrystal = 122306
    };

    enum eNpcs
    {
        Npc_Flashwyrm = 107098,
        Npc_InstructorNidriel = 89661,
        Npc_Stellagosa = 90982
    };

    enum eSpells
    {
        Spell_SummonAgapanthus = 180535,
        Spell_AgapanthusConversation = 180292,
        Spell_AelyithIntro = 197472,
        Spell_AelyithIshkaneth = 197475,
        Spell_AelyithTyndrissen = 197474,
        Spell_AelyithGettingMadder = 197476,
        Spell_SummonMagisterGaruhod = 179162,
        Spell_SummanThan = 215389,
        Spell_YouScratchMyBackConversation = 180835,
        Spell_SenegosScene = 214276,
        Spell_BetweenThePagesKobold = 212949,
        Spell_BetweenThePagesSkrog = 212956,
        Spell_BetweenThePagesSenegos = 212957,
        Spell_WandPractice = 212782,
        Spell_RuneDrawingRuneB = 179152,
        Spell_RuneDrawingRuneC = 179153
    };

    enum eObjectives
    {
        Obj_IshkanethSlain = 2787009,
        Obj_TyldrissenSlain = 2787010,
        Obj_SearchNearThePlazaFountain = 284615,
        Obj_KoboldDrawingSlain = 2787013,
        Obj_SeaSkrogDrawingSlain = 2787016,
        Obj_SenegosDrawingSlain = 2787019,
        Obj_NightglaiveTheTraitorSlain = 277301
    };

    enum eScenes
    {
        Scene_RuneA = 935,
        Scene_RuneB = 936,
        Scene_RuneC = 937
    };

    enum eKillcredits
    {
        Killcredit_ArcaneRuneDrawn = 89655,
        Killcredit_FireRuneDrawn = 89656,
        Killcredit_FrostRuneDrawn = 89657
    };

    Position const m_MagisterGaruhodSummonPos = { -13.44271f, 6726.504f, 55.6716f, 1.081242f };

    void OnSceneCancel(Player* p_Player, uint32 p_SceneInstanceId) override
    {
        if (p_Player->HasQuest(eQuests::Quest_PopQuizAdvancedRuneDrawing))
        {
            if (p_Player->GetMiscIDBySceneInstanceID(p_SceneInstanceId) == eScenes::Scene_RuneA)
            {
                if (Creature* l_Creature = p_Player->FindNearestCreature(eNpcs::Npc_InstructorNidriel, 30.0f, true))
                {
                    l_Creature->AI()->Talk(2);
                }

                p_Player->KilledMonsterCredit(eKillcredits::Killcredit_ArcaneRuneDrawn);
                p_Player->DelayedCastSpell(p_Player, eSpells::Spell_RuneDrawingRuneB, true, 3000);
            }
            else if (p_Player->GetMiscIDBySceneInstanceID(p_SceneInstanceId) == eScenes::Scene_RuneB)
            {
                if (Creature* l_Creature = p_Player->FindNearestCreature(eNpcs::Npc_InstructorNidriel, 30.0f, true))
                {
                    l_Creature->AI()->Talk(3, 0, 1);
                }

                p_Player->KilledMonsterCredit(eKillcredits::Killcredit_FireRuneDrawn);
                p_Player->DelayedCastSpell(p_Player, eSpells::Spell_RuneDrawingRuneC, true, 3000);
            }
            else if (p_Player->GetMiscIDBySceneInstanceID(p_SceneInstanceId) == eScenes::Scene_RuneC)
            {
                if (Creature* l_Creature = p_Player->FindNearestCreature(eNpcs::Npc_InstructorNidriel, 30.0f, true))
                {
                    l_Creature->AI()->Talk(4, p_Player->GetGUID());
                }

                p_Player->KilledMonsterCredit(eKillcredits::Killcredit_FrostRuneDrawn);
            }
        }
    }

    void OnQuestAccept(Player * p_Player, const Quest * p_Quest) override
    {
        switch (p_Quest->GetQuestId())
        {
            case eQuests::Quest_DarkRevelations:
            {
                p_Player->AddDelayedEvent([p_Player]() -> void
                {
                    if (Conversation* l_Conversation = new Conversation)
                    {
                        if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_AcceptQuestDarkRevelations, p_Player, nullptr, p_Player->GetPosition()))
                            delete l_Conversation;
                    }
                }, 3000);

                break;
            }
            case eQuests::Quest_LeylineAbuse:
            {
                p_Player->DelayedCastSpell(p_Player, eSpells::Spell_AelyithIntro, true, 6000);
                break;
            }
            case eQuests::Quest_TheNightbornePrince:
            {
                p_Player->DelayedCastSpell(p_Player, eSpells::Spell_AelyithGettingMadder, true, 6000);
                break;
            }
            case eQuests::Quest_NarthalasStillSuffers:
            {
                p_Player->AddDelayedEvent([p_Player]() -> void
                {
                    if (Conversation* l_Conversation = new Conversation)
                    {
                        if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_AcceptQuestNarthalasStillSuffers, p_Player, nullptr, p_Player->GetPosition()))
                            delete l_Conversation;
                    }
                }, 10000);

                break;
            }
            case eQuests::Quest_YouScratchMyBack:
            {
                p_Player->CastSpell(p_Player, eSpells::Spell_YouScratchMyBackConversation, true);
                break;
            }
            case eQuests::Quest_StudyHallCombatResearch:
            {
                if (Conversation* l_Conversation = new Conversation)
                {
                    if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_AcceptQuestStudyHallCombatResearch, p_Player, nullptr, p_Player->GetPosition()))
                        delete l_Conversation;
                }

                break;
            }
            default:
                break;
        }
    }

    void OnQuestComplete(Player* p_Player, const Quest* p_Quest) override
    {
        switch (p_Quest->GetQuestId())
        {
            case eQuests::Quest_Agapanthus:
            {
                p_Player->CastSpell(p_Player, eSpells::Spell_AgapanthusConversation, true);
                break;
            }
            case eQuests::Quest_TheirDyingBreaths:
            {
                p_Player->CastSpell(p_Player, eSpells::Spell_SummonAgapanthus, true);
                break;
            }
            case eQuests::Quest_Wanding:
            {
                p_Player->SetUInt16Value(PLAYER_FIELD_OVERRIDE_SPELLS_ID, PLAYER_FIELD_OVERRIDE_SPELLS_ID_OFFSET_OVERRIDE_SPELLS_ID, 0);
                p_Player->RemoveTemporarySpell(eSpells::Spell_WandPractice);
                break;
            }
            default:
                break;
        }
    }

    void OnQuestReward(Player* p_Player, Quest const* p_Quest) override
    {
        switch (p_Quest->GetQuestId())
        {
            case eQuests::Quest_TheDeathOfTheEldest:
            {
                if (Conversation* l_Conversation = new Conversation)
                {
                    if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_RewardQuestTheDeathOfTheEldest, p_Player, nullptr, p_Player->GetPosition()))
                        delete l_Conversation;
                }

                break;
            }
            case eQuests::Quest_ARatherLongWalk:
            {
                if (Conversation* l_Conversation = new Conversation)
                {
                    if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_RewardQuestARatherLongWalk, p_Player, nullptr, p_Player->GetPosition()))
                        delete l_Conversation;
                }

                break;
            }
            case eQuests::Quest_TheMagisterOfMixology:
            case eQuests::Quest_PresentationIsEverything:
            {
                if (p_Player->IsQuestRewarded(eQuests::Quest_TheMagisterOfMixology) && p_Player->IsQuestRewarded(eQuests::Quest_PresentationIsEverything))
                {
                    p_Player->CastSpell(m_MagisterGaruhodSummonPos, eSpells::Spell_SummonMagisterGaruhod, true);
                }

                break;
            }
            case eQuests::Quest_CursedToWither:
            {
                p_Player->RemoveAura(eSpells::Spell_SenegosScene);
                break;
            }
            default:
                break;
        }
    }

    void OnObjectiveValidate(Player* p_Player, uint32 /*p_QuestID*/, uint32 p_ObjectiveID) override
    {
        switch (p_ObjectiveID)
        {
            case eObjectives::Obj_NightglaiveTheTraitorSlain:
            {
                p_Player->SummonCreature(eNpcs::Npc_Stellagosa, -522.636f, 7522.38f, 74.07090f, 5.1876f, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN);
            }
            case eObjectives::Obj_IshkanethSlain:
            {
                p_Player->CastSpell(p_Player, eSpells::Spell_AelyithIshkaneth, true);
                break;
            }
            case eObjectives::Obj_TyldrissenSlain:
            {
                p_Player->CastSpell(p_Player, eSpells::Spell_AelyithTyndrissen, true);
                break;
            }
            case eObjectives::Obj_SearchNearThePlazaFountain:
            {
                p_Player->CastSpell(p_Player, eSpells::Spell_SummanThan, true);
                break;
            }
            case eObjectives::Obj_KoboldDrawingSlain:
            {
                p_Player->RemoveAura(eSpells::Spell_BetweenThePagesKobold);

                if (Conversation* l_Conversation = new Conversation)
                {
                    if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_KoboldDrawingSlain, p_Player, nullptr, p_Player->GetPosition()))
                        delete l_Conversation;
                }

                break;
            }
            case eObjectives::Obj_SeaSkrogDrawingSlain:
            {
                p_Player->RemoveAura(eSpells::Spell_BetweenThePagesSkrog);

                if (Conversation* l_Conversation = new Conversation)
                {
                    if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), eConversations::Conv_SeaSkrogDrawingSlain, p_Player, nullptr, p_Player->GetPosition()))
                        delete l_Conversation;
                }

                break;
            }
            case eObjectives::Obj_SenegosDrawingSlain:
            {
                p_Player->RemoveAura(eSpells::Spell_BetweenThePagesSenegos);
                break;
            }
            default:
                break;
            }
    }

    void OnItemLooted(Player* p_Player, Item* p_Item) override
    {
        switch (p_Item->GetEntry())
        {
            case  eItems::Item_CrackedLeyCrystal:
            {
                if (Creature* l_Creature = p_Player->FindNearestCreature(eNpcs::Npc_Flashwyrm, 30.0f, true))
                {
                    l_Creature->setFaction(14);
                    l_Creature->Attack(p_Player, false);
                }

                break;
            }
            default:
                break;
        }
    }
};

#ifndef __clang_analyzer__
void AddSC_azsuna_scripts()
{
    new npc_soul_gem_86963();
    new npc_stellagosa_90982();
    new npc_archmage_khadgar_93325();
    new npc_archmage_khadgar_93326();
    new npc_archmage_khadgar_93354();
    new npc_mana_drained_whelpling_90219();
    new npc_agapanthus_and_keepers();
    new npc_senegos_89975();
    new npc_stellagosa_90900();
    new npc_stellagosa_90065();
    new npc_runas_the_shamed_90372();
    new npc_runas_the_shamed_90476();
    new npc_runas_the_shamed_90406();
    new npc_projection_of_senegos_90567();
    new npc_projection_of_senegos_90569();
    new npc_stellagosa_107995();
    new npc_elder_aldryth_89134();
    new npc_magister_garuhod_89663();
    new npc_than_and_estynna();
    new npc_athissa_88091();
    new npc_instructor_nidriel_89661();
    new gob_azsuna_demon_ward();
    new gob_sythornes_podium_250362();
    new at_azsuna_wand_practice();
    new spell_azsuna_soul_gem_ping();
    new spell_azsuna_toss_crystals();
    new spell_azsuna_resuscitating();
    new spell_azsuna_mana_surge();
    new spell_azsuna_freeing();
    new spell_azsuna_between_the_pages();
    new playerscript_azsuna_scripts();
}
#endif