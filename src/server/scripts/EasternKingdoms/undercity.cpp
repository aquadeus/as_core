////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/* ScriptData
SDName: Undercity
SD%Complete: 95
SDComment: Quest support: 6628, 9180(post-event).
SDCategory: Undercity
EndScriptData */

/* ContentData
npc_lady_sylvanas_windrunner
npc_highborne_lamenter
npc_parqual_fintallas
EndContentData */

#include "EasternKingdomsPCH.h"

/*######
## npc_lady_sylvanas_windrunner
######*/

enum Sylvanas
{
    QUEST_JOURNEY_TO_UNDERCITY  = 9180,
    SAY_LAMENT_END              = -1000196,
    EMOTE_LAMENT_END            = -1000197,

    SOUND_CREDIT                = 10896,
    ENTRY_HIGHBORNE_LAMENTER    = 21628,
    ENTRY_HIGHBORNE_BUNNY       = 21641,

    SPELL_HIGHBORNE_AURA        = 37090,
    SPELL_SYLVANAS_CAST         = 36568,
    SPELL_RIBBON_OF_SOULS       = 34432, // the real one to use might be 37099

    // Combat spells
    SPELL_BLACK_ARROW           = 59712,
    SPELL_FADE                  = 20672,
    SPELL_FADE_BLINK            = 29211,
    SPELL_MULTI_SHOT            = 59713,
    SPELL_SHOT                  = 59710,
    SPELL_SUMMON_SKELETON       = 59711
};

float HighborneLoc[4][3]=
{
    {1285.41f, 312.47f, 0.51f},
    {1286.96f, 310.40f, 1.00f},
    {1289.66f, 309.66f, 1.52f},
    {1292.51f, 310.50f, 1.99f}
};

#define HIGHBORNE_LOC_Y             -61.00f
#define HIGHBORNE_LOC_Y_NEW         -55.50f

class npc_lady_sylvanas_windrunner : public CreatureScript
{
public:
    npc_lady_sylvanas_windrunner() : CreatureScript("npc_lady_sylvanas_windrunner") { }

    bool OnQuestReward(Player* /*player*/, Creature* creature, const Quest *_Quest, uint32 /*slot*/)
    {
        if (_Quest->GetQuestId() == QUEST_JOURNEY_TO_UNDERCITY)
        {
            CAST_AI(npc_lady_sylvanas_windrunner::npc_lady_sylvanas_windrunnerAI, creature->AI())->LamentEvent = true;
            CAST_AI(npc_lady_sylvanas_windrunner::npc_lady_sylvanas_windrunnerAI, creature->AI())->DoPlaySoundToSet(creature, SOUND_CREDIT);
            creature->CastSpell(creature, SPELL_SYLVANAS_CAST, false);

            for (uint8 i = 0; i < 4; ++i)
                creature->SummonCreature(ENTRY_HIGHBORNE_LAMENTER, HighborneLoc[i][0], HighborneLoc[i][1], HIGHBORNE_LOC_Y, HighborneLoc[i][2], TEMPSUMMON_TIMED_DESPAWN, 160000);
        }

        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lady_sylvanas_windrunnerAI (creature);
    }

    struct npc_lady_sylvanas_windrunnerAI : public ScriptedAI
    {
        npc_lady_sylvanas_windrunnerAI(Creature* creature) : ScriptedAI(creature) {}

        uint32 LamentEventTimer;
        bool LamentEvent;
        uint64 targetGUID;

        uint32 FadeTimer;
        uint32 SummonSkeletonTimer;
        uint32 BlackArrowTimer;
        uint32 ShotTimer;
        uint32 MultiShotTimer;

        void Reset()
        {
            LamentEventTimer = 5000;
            LamentEvent = false;
            targetGUID = 0;

            FadeTimer = 30000;
            SummonSkeletonTimer = 20000;
            BlackArrowTimer = 15000;
            ShotTimer = 8000;
            MultiShotTimer = 10000;
        }

        void EnterCombat(Unit* /*who*/) {}

        void JustSummoned(Creature* summoned)
        {
            if (summoned->GetEntry() == ENTRY_HIGHBORNE_BUNNY)
            {
                if (Creature* target = Unit::GetCreature(*summoned, targetGUID))
                {
                    target->MonsterMoveWithSpeed(target->GetPositionX(), target->GetPositionY(), me->GetPositionZ()+15.0f, 0);
                    target->SetPosition(target->GetPositionX(), target->GetPositionY(), me->GetPositionZ()+15.0f, 0.0f);
                    summoned->CastSpell(target, SPELL_RIBBON_OF_SOULS, false);
                }

                summoned->SetDisableGravity(true);
                targetGUID = summoned->GetGUID();
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (LamentEvent)
            {
                if (LamentEventTimer <= diff)
                {
                    DoSummon(ENTRY_HIGHBORNE_BUNNY, me, 10.0f, 3000, TEMPSUMMON_TIMED_DESPAWN);

                    LamentEventTimer = 2000;
                    if (!me->HasAura(SPELL_SYLVANAS_CAST))
                    {
                        DoScriptText(SAY_LAMENT_END, me);
                        DoScriptText(EMOTE_LAMENT_END, me);
                        LamentEvent = false;
                    }
                }
                else
                    LamentEventTimer -= diff;
            }

            if (!UpdateVictim())
                return;

            // Combat spells

            if (FadeTimer <= diff)
            {
                DoCast(me, SPELL_FADE);
                // add a blink to simulate a stealthed movement and reappearing elsewhere
                DoCast(me, SPELL_FADE_BLINK);
                FadeTimer = 30000 + rand()%5000;
                // if the victim is out of melee range she cast multi shot
                if (Unit* victim = me->getVictim())
                    if (me->GetDistance(victim) > 10.0f)
                        DoCast(victim, SPELL_MULTI_SHOT);
            }
            else
                FadeTimer -= diff;

            if (SummonSkeletonTimer <= diff)
            {
                DoCast(me, SPELL_SUMMON_SKELETON);
                SummonSkeletonTimer = 20000 + rand()%10000;
            }
            else
                SummonSkeletonTimer -= diff;

            if (BlackArrowTimer <= diff)
            {
                if (Unit* victim = me->getVictim())
                {
                    DoCast(victim, SPELL_BLACK_ARROW);
                    BlackArrowTimer = 15000 + rand()%5000;
                }
            }
            else
                BlackArrowTimer -= diff;

            if (ShotTimer <= diff)
            {
                if (Unit* victim = me->getVictim())
                {
                    DoCast(victim, SPELL_SHOT);
                    ShotTimer = 8000 + rand()%2000;
                }
            }
            else
                ShotTimer -= diff;

            if (MultiShotTimer <= diff)
            {
                if (Unit* victim = me->getVictim())
                {
                    DoCast(victim, SPELL_MULTI_SHOT);
                    MultiShotTimer = 10000 + rand()%3000;
                }
            }
            else
                MultiShotTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};

/*######
## npc_highborne_lamenter
######*/

class npc_highborne_lamenter : public CreatureScript
{
public:
    npc_highborne_lamenter() : CreatureScript("npc_highborne_lamenter") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_highborne_lamenterAI (creature);
    }

    struct npc_highborne_lamenterAI : public ScriptedAI
    {
        npc_highborne_lamenterAI(Creature* creature) : ScriptedAI(creature) {}

        uint32 EventMoveTimer;
        uint32 EventCastTimer;
        bool EventMove;
        bool EventCast;

        void Reset()
        {
            EventMoveTimer = 10000;
            EventCastTimer = 17500;
            EventMove = true;
            EventCast = true;
        }

        void EnterCombat(Unit* /*who*/) {}

        void UpdateAI(const uint32 diff)
        {
            if (EventMove)
            {
                if (EventMoveTimer <= diff)
                {
                    me->SetDisableGravity(true);
                    me->MonsterMoveWithSpeed(me->GetPositionX(), me->GetPositionY(), HIGHBORNE_LOC_Y_NEW, me->GetDistance(me->GetPositionX(), me->GetPositionY(), HIGHBORNE_LOC_Y_NEW) / (5000 * 0.001f));
                    me->SetPosition(me->GetPositionX(), me->GetPositionY(), HIGHBORNE_LOC_Y_NEW, me->GetOrientation());
                    EventMove = false;
                }
                else
                    EventMoveTimer -= diff;
            }
            if (EventCast)
            {
                if (EventCastTimer <= diff)
                {
                    DoCast(me, SPELL_HIGHBORNE_AURA);
                    EventCast = false;
                }
                else
                    EventCastTimer -= diff;
            }
        }
    };
};

/*######
## npc_parqual_fintallas
######*/

#define SPELL_MARK_OF_SHAME 6767

#define GOSSIP_HPF1 "Gul'dan"
#define GOSSIP_HPF2 "Kel'Thuzad"
#define GOSSIP_HPF3 "Ner'zhul"

class npc_parqual_fintallas : public CreatureScript
{
public:
    npc_parqual_fintallas() : CreatureScript("npc_parqual_fintallas") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF+1)
        {
            player->PlayerTalkClass->SendCloseGossip();
            creature->CastSpell(player, SPELL_MARK_OF_SHAME, false);
        }
        if (action == GOSSIP_ACTION_INFO_DEF+2)
        {
            player->PlayerTalkClass->SendCloseGossip();
            player->AreaExploredOrEventHappens(6628);
        }
        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->isQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(6628) == QUEST_STATUS_INCOMPLETE && !player->HasAura(SPELL_MARK_OF_SHAME))
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HPF1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HPF2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HPF3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            player->SEND_GOSSIP_MENU(5822, creature->GetGUID());
        }
        else
            player->SEND_GOSSIP_MENU(5821, creature->GetGUID());

        return true;
    }
};

/// Crone's Broom - 53761
class npc_crones_broom_53761 : public CreatureScript
{
    public:
        npc_crones_broom_53761() : CreatureScript("npc_crones_broom_53761") { }

        struct npc_crones_broom_53761AI : public VehicleAI
        {
            npc_crones_broom_53761AI(Creature* p_Creature) : VehicleAI(p_Creature) { }

            G3D::Vector3 const Path_CronesBroomCircle[78] =
            {
                { -8926.179f, 513.5434f, 146.0898f },
                { -8928.653f, 527.4549f, 149.1052f },
                { -8922.42f, 543.9774f, 152.4396f },
                { -8913.674f, 553.1389f, 150.9259f },
                { -8898.094f, 565.8611f, 146.4797f },
                { -8886.982f, 578.4011f, 131.8325f },
                { -8869.654f, 590.8715f, 117.3132f },
                { -8853.676f, 606.4028f, 117.3132f },
                { -8844.554f, 624.132f, 117.3132f },
                { -8852.786f, 642.1545f, 117.3132f },
                { -8852.45f, 654.8125f, 117.3132f },
                { -8843.466f, 669.1389f, 117.3132f },
                { -8829.913f, 676.368f, 125.1623f },
                { -8811.113f, 680.8281f, 125.1623f },
                { -8794.721f, 692.4496f, 132.4432f },
                { -8785.123f, 721.7257f, 134.7661f },
                { -8799.667f, 746.6719f, 127.0066f },
                { -8814.748f, 785.5243f, 113.6912f },
                { -8827.38f, 817.5608f, 114.7905f },
                { -8826.25f, 842.1996f, 114.7905f },
                { -8818.226f, 872.2969f, 114.7905f },
                { -8788.938f, 881.9011f, 114.7905f },
                { -8745.493f, 884.1441f, 114.7905f },
                { -8703.314f, 891.467f, 114.7905f },
                { -8672.504f, 908.4236f, 114.7905f },
                { -8631.123f, 918.2483f, 114.7905f },
                { -8609.933f, 905.7743f, 114.7905f },
                { -8592.455f, 883.0208f, 114.7905f },
                { -8608.976f, 846.1024f, 114.7905f },
                { -8634.094f, 817.0295f, 114.7905f },
                { -8629.212f, 787.875f, 114.7905f },
                { -8629.723f, 757.0573f, 118.6781f },
                { -8631.821f, 728.4636f, 123.9302f },
                { -8617.967f, 711.4688f, 120.7624f },
                { -8601.147f, 717.4479f, 118.9534f },
                { -8578.191f, 738.0573f, 114.7905f },
                { -8553.924f, 765.4583f, 125.6543f },
                { -8532.451f, 771.434f, 123.3722f },
                { -8511.969f, 763.8906f, 114.7905f },
                { -8485.469f, 747.7795f, 114.7905f },
                { -8469.127f, 735.0364f, 119.7802f },
                { -8441.823f, 720.4948f, 123.9715f },
                { -8427.113f, 708.408f, 122.1203f },
                { -8414.91f, 692.5f, 122.1203f },
                { -8403.578f, 664.4479f, 122.1203f },
                { -8392.29f, 641.1111f, 124.3749f },
                { -8371.946f, 624.875f, 124.7377f },
                { -8348.292f, 617.6233f, 122.1203f },
                { -8335.897f, 635.566f, 122.1203f },
                { -8352.518f, 655.184f, 122.1203f },
                { -8377.87f, 651.5903f, 120.5722f },
                { -8403.196f, 633.1146f, 117.3857f },
                { -8434.196f, 608.2361f, 116.8653f },
                { -8458.25f, 586.2604f, 115.7205f },
                { -8474.147f, 568.7361f, 116.2499f },
                { -8492.386f, 543.6788f, 118.6645f },
                { -8487.448f, 509.0729f, 122.1203f },
                { -8503.814f, 494.2379f, 120.5533f },
                { -8526.323f, 487.724f, 122.1203f },
                { -8541.524f, 476.6371f, 122.1203f },
                { -8542.823f, 460.0712f, 122.1203f },
                { -8541.752f, 441.9774f, 122.1203f },
                { -8558.516f, 432.0885f, 122.1203f },
                { -8578.643f, 435.7708f, 122.1203f },
                { -8603.877f, 444.309f, 122.1203f },
                { -8625.169f, 447.5156f, 122.1203f },
                { -8643.45f, 449.8299f, 125.1388f },
                { -8677.507f, 448.5087f, 127.1211f },
                { -8696.103f, 431.2604f, 127.6389f },
                { -8714.389f, 422.8246f, 130.9356f },
                { -8735.772f, 427.4705f, 139.4648f },
                { -8752.978f, 444.0121f, 139.4648f },
                { -8764.692f, 457.1806f, 139.4648f },
                { -8789.797f, 466.1059f, 139.4648f },
                { -8816.232f, 472.1962f, 145.918f },
                { -8843.864f, 473.4722f, 147.29f },
                { -8885.976f, 476.8038f, 150.9953f },
                { -8915.364f, 498.5295f, 146.3356f }
            };

            enum eSpells
            {
                Spell_RideVehicle = 46598,
                Spell_TeleportToStormwind = 100028,
                Spell_ReturnHome = 100030,
                Spell_TeleportToUndercity = 100027
            };

            enum ePoints
            {
                Point_PathBeforeTeleportEnd = 1,
                Point_CirclePathEnd = 2,
                Point_ReturnToLandPathEnd = 3
            };

            enum eNpcs
            {
                Npc_Crone = 53762
            };

            ObjectGuid m_SummonerGuid;
            bool m_CanStartCirclePath = true;
            bool m_FlyghtEnd = false;
            bool m_CanReturnToLand = true;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_SummonerGuid = p_Summoner->GetGUID();
                me->SetReactState(ReactStates::REACT_PASSIVE);
                p_Summoner->EnterVehicle(me, 1, true);
                me->SetDisableGravity(true);

                if (Creature* l_Creature = p_Summoner->SummonCreature(eNpcs::Npc_Crone, me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                {
                    l_Creature->EnterVehicle(me, 0, true);
                }

                me->AddDelayedEvent([this]() -> void
                {
                    G3D::Vector3 const Path_CronesBroomBeforeTeleport[4] =
                    {
                        { 1816.457f, 225.092f, 66.14496f },
                        { 1814.953f, 215.5642f, 73.33482f },
                        { 1811.198f, 200.3229f, 84.63605f },
                        { 1804.604f, 182.7813f, 97.03205f }
                    };

                    me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_PathBeforeTeleportEnd, Path_CronesBroomBeforeTeleport, 4);
                }, 1000);
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                if (!l_Player)
                    return;

                switch (p_PointId)
                {
                case ePoints::Point_PathBeforeTeleportEnd:
                {
                    DoCast(eSpells::Spell_TeleportToStormwind, true);
                    break;
                }
                case ePoints::Point_CirclePathEnd:
                {
                    if (!m_FlyghtEnd)
                    {
                        me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_CirclePathEnd, Path_CronesBroomCircle, 78);
                    }

                    break;
                }
                case ePoints::Point_ReturnToLandPathEnd:
                {
                    if (Vehicle* l_Vehicle = me->GetVehicleKit())
                    {
                        if (Unit* l_Unit = l_Vehicle->GetPassenger(0))
                        {
                            if (l_Unit->ToCreature())
                            {
                                l_Unit->ExitVehicle();
                                l_Unit->ToCreature()->DespawnOrUnsummon();
                            }
                        }
                    }

                    l_Player->ExitVehicle();
                    me->DespawnOrUnsummon();
                    break;
                }
                default:
                    break;
                }
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                if (p_Spell->Id == eSpells::Spell_ReturnHome)
                {
                    m_FlyghtEnd = true;
                    me->GetMotionMaster()->Clear();
                    DoCast(eSpells::Spell_TeleportToUndercity, true);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                if (!l_Player || !l_Player->IsInWorld())
                {
                    if (Vehicle* l_Vehicle = me->GetVehicleKit())
                    {
                        if (Unit* l_Unit = l_Vehicle->GetPassenger(0))
                        {
                            if (l_Unit->ToCreature())
                            {
                                l_Unit->ExitVehicle();
                                l_Unit->ToCreature()->DespawnOrUnsummon();
                            }
                        }
                    }

                    me->DespawnOrUnsummon();
                    return;
                }

                if (me->GetZoneId() == 1519 && m_CanStartCirclePath)
                {
                    m_CanStartCirclePath = false;
                    me->SetRooted(true);

                    me->AddDelayedEvent([this]() -> void
                    {
                        G3D::Vector3 const Path_CronesBroomCircle[78] =
                        {
                            { -8926.179f, 513.5434f, 146.0898f },
                            { -8928.653f, 527.4549f, 149.1052f },
                            { -8922.42f, 543.9774f, 152.4396f },
                            { -8913.674f, 553.1389f, 150.9259f },
                            { -8898.094f, 565.8611f, 146.4797f },
                            { -8886.982f, 578.4011f, 131.8325f },
                            { -8869.654f, 590.8715f, 117.3132f },
                            { -8853.676f, 606.4028f, 117.3132f },
                            { -8844.554f, 624.132f, 117.3132f },
                            { -8852.786f, 642.1545f, 117.3132f },
                            { -8852.45f, 654.8125f, 117.3132f },
                            { -8843.466f, 669.1389f, 117.3132f },
                            { -8829.913f, 676.368f, 125.1623f },
                            { -8811.113f, 680.8281f, 125.1623f },
                            { -8794.721f, 692.4496f, 132.4432f },
                            { -8785.123f, 721.7257f, 134.7661f },
                            { -8799.667f, 746.6719f, 127.0066f },
                            { -8814.748f, 785.5243f, 113.6912f },
                            { -8827.38f, 817.5608f, 114.7905f },
                            { -8826.25f, 842.1996f, 114.7905f },
                            { -8818.226f, 872.2969f, 114.7905f },
                            { -8788.938f, 881.9011f, 114.7905f },
                            { -8745.493f, 884.1441f, 114.7905f },
                            { -8703.314f, 891.467f, 114.7905f },
                            { -8672.504f, 908.4236f, 114.7905f },
                            { -8631.123f, 918.2483f, 114.7905f },
                            { -8609.933f, 905.7743f, 114.7905f },
                            { -8592.455f, 883.0208f, 114.7905f },
                            { -8608.976f, 846.1024f, 114.7905f },
                            { -8634.094f, 817.0295f, 114.7905f },
                            { -8629.212f, 787.875f, 114.7905f },
                            { -8629.723f, 757.0573f, 118.6781f },
                            { -8631.821f, 728.4636f, 123.9302f },
                            { -8617.967f, 711.4688f, 120.7624f },
                            { -8601.147f, 717.4479f, 118.9534f },
                            { -8578.191f, 738.0573f, 114.7905f },
                            { -8553.924f, 765.4583f, 125.6543f },
                            { -8532.451f, 771.434f, 123.3722f },
                            { -8511.969f, 763.8906f, 114.7905f },
                            { -8485.469f, 747.7795f, 114.7905f },
                            { -8469.127f, 735.0364f, 119.7802f },
                            { -8441.823f, 720.4948f, 123.9715f },
                            { -8427.113f, 708.408f, 122.1203f },
                            { -8414.91f, 692.5f, 122.1203f },
                            { -8403.578f, 664.4479f, 122.1203f },
                            { -8392.29f, 641.1111f, 124.3749f },
                            { -8371.946f, 624.875f, 124.7377f },
                            { -8348.292f, 617.6233f, 122.1203f },
                            { -8335.897f, 635.566f, 122.1203f },
                            { -8352.518f, 655.184f, 122.1203f },
                            { -8377.87f, 651.5903f, 120.5722f },
                            { -8403.196f, 633.1146f, 117.3857f },
                            { -8434.196f, 608.2361f, 116.8653f },
                            { -8458.25f, 586.2604f, 115.7205f },
                            { -8474.147f, 568.7361f, 116.2499f },
                            { -8492.386f, 543.6788f, 118.6645f },
                            { -8487.448f, 509.0729f, 122.1203f },
                            { -8503.814f, 494.2379f, 120.5533f },
                            { -8526.323f, 487.724f, 122.1203f },
                            { -8541.524f, 476.6371f, 122.1203f },
                            { -8542.823f, 460.0712f, 122.1203f },
                            { -8541.752f, 441.9774f, 122.1203f },
                            { -8558.516f, 432.0885f, 122.1203f },
                            { -8578.643f, 435.7708f, 122.1203f },
                            { -8603.877f, 444.309f, 122.1203f },
                            { -8625.169f, 447.5156f, 122.1203f },
                            { -8643.45f, 449.8299f, 125.1388f },
                            { -8677.507f, 448.5087f, 127.1211f },
                            { -8696.103f, 431.2604f, 127.6389f },
                            { -8714.389f, 422.8246f, 130.9356f },
                            { -8735.772f, 427.4705f, 139.4648f },
                            { -8752.978f, 444.0121f, 139.4648f },
                            { -8764.692f, 457.1806f, 139.4648f },
                            { -8789.797f, 466.1059f, 139.4648f },
                            { -8816.232f, 472.1962f, 145.918f },
                            { -8843.864f, 473.4722f, 147.29f },
                            { -8885.976f, 476.8038f, 150.9953f },
                            { -8915.364f, 498.5295f, 146.3356f }
                        };

                        me->SetRooted(false);
                        me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_CirclePathEnd, Path_CronesBroomCircle, 78);
                    }, 5000);
                }

                if (me->GetZoneId() == 85 && m_FlyghtEnd && m_CanReturnToLand)
                {
                    m_CanReturnToLand = false;
                    me->SetRooted(true);
                    me->RemoveAura(eSpells::Spell_TeleportToStormwind);

                    me->AddDelayedEvent([this]() -> void
                    {
                        G3D::Vector3 const Path_CronesBroomReturnToLand[4] =
                        {
                            { 1851.396f, 273.809f, 99.8739f },
                            { 1833.333f, 258.9566f, 89.68661f },
                            { 1820.847f, 242.1337f, 73.4329f },
                            { 1814.005f, 230.6424f, 63.48659f }
                        };

                        me->SetRooted(false);
                        me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_ReturnToLandPathEnd, Path_CronesBroomReturnToLand, 4);
                    }, 5000);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_crones_broom_53761AI(p_Creature);
        }
};

#ifndef __clang_analyzer__
void AddSC_undercity()
{
    new npc_lady_sylvanas_windrunner();
    new npc_highborne_lamenter();
    new npc_parqual_fintallas();
    new npc_crones_broom_53761();
}
#endif
