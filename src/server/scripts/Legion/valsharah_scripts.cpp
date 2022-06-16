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

/// Commander Jarod Shadowsong - 92620
class npc_commander_jarod_shadowsong_92620 : public CreatureScript
{
public:
    npc_commander_jarod_shadowsong_92620() : CreatureScript("npc_commander_jarod_shadowsong_92620") { }

    struct npc_commander_jarod_shadowsong_92620AI : public ScriptedAI
    {
        npc_commander_jarod_shadowsong_92620AI(Creature *creature) : ScriptedAI(creature) { }

        enum eQuests
        {
            Quest_AVillageInPerilFirst = 38643,
            Quest_AVillageInPerilSecond = 39149,
            Quest_ChildrenOfTheNight = 38645,
            Quest_JarodsMission = 38691
        };

        enum eObjectives
        {
            Obj_ConversationHeardFirst = 3864300,
            Obj_ConversationHeardSecond = 3914900
        };

        enum eSpells
        {
            Spell_BradensbrookArrivalConversation = 186894,
            Spell_SummonCommanderJarodShadowsong = 220055,
            Spell_SummonBard = 219993
        };

        enum eKillcredits
        {
            Killcredit_RideWithJarodShadowsong = 92620
        };

        Position const m_JarodSummonPos = { 2798.514f, 7266.745f, 21.67302f, 1.716295f };
        Position const m_BardSummonPos = { 2800.647f, 7264.822f, 21.72191f, 1.86922f };

        void MoveInLineOfSight(Unit* p_Who) override
        {
            Player* l_Player = p_Who->ToPlayer();
            if (!l_Player)
                return;

            if (me->GetExactDist2d(l_Player) < 10.0f && (l_Player->HasQuest(eQuests::Quest_AVillageInPerilFirst) || l_Player->HasQuest(eQuests::Quest_AVillageInPerilSecond)) &&
                (l_Player->GetQuestObjectiveCounter(eObjectives::Obj_ConversationHeardFirst) == 0 && l_Player->GetQuestObjectiveCounter(eObjectives::Obj_ConversationHeardSecond) == 0))
            {
                l_Player->CastSpell(l_Player, eSpells::Spell_BradensbrookArrivalConversation, true);
                l_Player->QuestObjectiveOptionalSatisfy(eObjectives::Obj_ConversationHeardFirst);
            }
        }

        void sQuestAccept(Player* p_Player, Quest const* p_Quest) override
        {
            if (p_Quest->GetQuestId() == eQuests::Quest_ChildrenOfTheNight)
            {
                Talk(0, p_Player->GetGUID());
            }
        }

        void sGossipSelect(Player* p_Player, uint32 /*p_Sender*/, uint32 /*p_Action*/) override
        {
            p_Player->KilledMonsterCredit(eKillcredits::Killcredit_RideWithJarodShadowsong);
            p_Player->CastSpell(m_JarodSummonPos, eSpells::Spell_SummonCommanderJarodShadowsong, true);
            p_Player->CastSpell(m_BardSummonPos, eSpells::Spell_SummonBard, true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_commander_jarod_shadowsong_92620AI(creature);
    }
};

/// Bard - 110639
class npc_bard_110639 : public CreatureScript
{
public:
    npc_bard_110639() : CreatureScript("npc_bard_110639") { }

    struct npc_bard_110639AI : public VehicleAI
    {
        npc_bard_110639AI(Creature *creature) : VehicleAI(creature) { }

        G3D::Vector3 const Path_BardBeforeJump[19] =
        {
            { 2790.284180f, 7270.278809f, 21.662519f },
            { 2780.141846f, 7275.029785f, 21.762980f },
            { 2773.304443f, 7283.071777f, 21.792997f },
            { 2767.335205f, 7298.054688f, 21.815334f },
            { 2763.192871f, 7314.165039f, 21.749014f },
            { 2757.764160f, 7324.965332f, 20.303053f },
            { 2752.961182f, 7335.094238f, 18.067158f },
            { 2749.042969f, 7355.790527f, 13.528633f },
            { 2749.724121f, 7381.188965f, 8.942359f },
            { 2749.256592f, 7403.602539f, 5.451968f },
            { 2761.243652f, 7416.342773f, 5.372235f },
            { 2771.732422f, 7429.277832f, 5.321962f },
            { 2779.406982f, 7430.501465f, 5.312742f },
            { 2786.019287f, 7433.729004f, 5.444210f },
            { 2794.055420f, 7435.412598f, 5.401851f },
            { 2806.907227f, 7442.996582f, 4.362082f },
            { 2817.180664f, 7451.965332f, 3.629038f },
            { 2824.567139f, 7461.692383f, 2.952003f },
            { 2831.589111f, 7467.871582f, 0.942461f }
        };

        G3D::Vector3 const Path_BardAfterJump[18] =
        {
            { 2857.963379f, 7482.238770f, 1.550573f },
            { 2865.813232f, 7493.573242f, 5.280665f },
            { 2875.666748f, 7502.251953f, 8.964417f },
            { 2879.800293f, 7510.713867f, 12.223770f },
            { 2895.768066f, 7525.917480f, 19.305624f },
            { 2912.058350f, 7527.921387f, 25.663715f },
            { 2926.481445f, 7525.038086f, 31.268524f },
            { 2936.796143f, 7518.777832f, 36.672832f },
            { 2947.254150f, 7507.442871f, 40.576664f },
            { 2959.010254f, 7487.521484f, 45.811348f },
            { 2972.108398f, 7476.327148f, 47.613430f },
            { 3008.059082f, 7463.397461f, 47.762547f },
            { 3015.763672f, 7452.667969f, 46.261761f },
            { 3015.978271f, 7446.416016f, 46.504906f },
            { 3016.225342f, 7434.995605f, 41.120380f },
            { 3016.528809f, 7421.648926f, 35.430428f },
            { 3016.790527f, 7402.373047f, 35.236916f },
            { 3016.993164f, 7389.738281f, 35.236916f }
        };

        enum eSpells
        {
            Spell_RideVehicleJarod = 52391,
            Spell_RiveVehiclePlayer = 82316
        };

        enum ePoints
        {
            Point_PathBeforeJumpEnd = 1,
            Point_FirstJumpEnd = 2,
            Point_SecondJumpEnd = 3,
            Point_PathAfterJumpEnd = 4
        };

        enum eNpcs
        {
            Npc_CommanderJarodShadowsong = 110658
        };

        enum eActions
        {
            Action_JarodStartPath = 1
        };

        ObjectGuid m_SummonerGuid;
        ObjectGuid m_JarodGuid;
        Position const m_FirstJumpPos = { 2841.697998f, 7473.805664f, 0.274455f };
        Position const m_SecondJumpPos = { 2851.301025f, 7479.896973f, 0.542337f };

        void IsSummonedBy(Unit* p_Summoner) override
        {
            m_SummonerGuid = p_Summoner->GetGUID();
            uint64 l_SummonerGuid = m_SummonerGuid;

            me->AddPlayerInPersonnalVisibilityList(m_SummonerGuid);
            me->PlayOneShotAnimKitId(2666);

            me->AddDelayedEvent([this, l_SummonerGuid]() -> void
            {
                if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_SummonerGuid))
                {
                    if (Creature* l_Creature = l_Player->FindNearestCreature(eNpcs::Npc_CommanderJarodShadowsong, 50.0f, true))
                    {
                        m_JarodGuid = l_Creature->GetGUID();
                        l_Creature->CastSpell(me, eSpells::Spell_RideVehicleJarod, true);
                    }

                    l_Player->CastSpell(me, eSpells::Spell_RiveVehiclePlayer, true);
                }
            }, 500);

            me->AddDelayedEvent([this]() -> void
            {
                me->SetSpeed(MOVE_RUN, 2.0f);
                me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_PathBeforeJumpEnd, Path_BardBeforeJump, 19, false);
            }, 1000);
        }

        void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
        {
            switch (p_PointId)
            {
            case ePoints::Point_PathBeforeJumpEnd:
            {
                me->GetMotionMaster()->MoveJump(m_FirstJumpPos, 15.0f, 10.0f, ePoints::Point_FirstJumpEnd);
                break;
            }
            case ePoints::Point_FirstJumpEnd:
            {
                me->GetMotionMaster()->MoveJump(m_SecondJumpPos, 15.0f, 10.0f, ePoints::Point_SecondJumpEnd);
                break;
            }
            case ePoints::Point_SecondJumpEnd:
            {
                me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_PathAfterJumpEnd, Path_BardAfterJump, 18, false);
                break;
            }
            case ePoints::Point_PathAfterJumpEnd:
            {
                if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                {
                    l_Player->ExitVehicle();
                }

                if (Creature* l_Creature = ObjectAccessor::GetCreature(*me, m_JarodGuid))
                {
                    l_Creature->ExitVehicle();
                    l_Creature->AI()->DoAction(eActions::Action_JarodStartPath);
                }

                me->DespawnOrUnsummon(2000);
                break;
            }
            default:
                break;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bard_110639AI(creature);
    }
};

/// Commander Jarod Shadowsong - 110658
class npc_commander_jarod_shadowsong_110658 : public CreatureScript
{
public:
    npc_commander_jarod_shadowsong_110658() : CreatureScript("npc_commander_jarod_shadowsong_110658") { }

    struct npc_commander_jarod_shadowsong_110658AI : public ScriptedAI
    {
        npc_commander_jarod_shadowsong_110658AI(Creature *creature) : ScriptedAI(creature) { }

        G3D::Vector3 const Path_Jarod[11] =
        {
            { 3017.272f, 7386.912f, 37.39391f },
            { 3017.485f, 7385.418f, 35.77259f },
            { 3017.485f, 7382.668f, 35.77259f },
            { 3017.485f, 7376.168f, 35.77259f },
            { 3017.485f, 7369.668f, 35.77259f },
            { 3017.485f, 7364.918f, 35.77259f },
            { 3017.485f, 7362.168f, 35.77259f },
            { 3017.735f, 7361.168f, 36.02259f },
            { 3017.735f, 7358.918f, 36.27259f },
            { 3017.735f, 7355.918f, 37.27259f },
            { 3017.697f, 7354.924f, 37.18715f }
        };

        enum ePoints
        {
            Point_PathEnd = 1
        };

        enum eActions
        {
            Action_JarodStartPath = 1
        };

        enum eKillcredits
        {
            Killcredit_PathEnded = 3869101
        };

        enum eNpcs
        {
            Npc_CommanderJarodShadowsong = 92842
        };

        ObjectGuid m_SummonerGuid;

        void IsSummonedBy(Unit* p_Summoner) override
        {
            m_SummonerGuid = p_Summoner->GetGUID();
            me->AddPlayerInPersonnalVisibilityList(m_SummonerGuid);
        }

        void DoAction(int32 const p_Action) override
        {
            if (p_Action == eActions::Action_JarodStartPath)
            {
                me->GetMotionMaster()->MoveSmoothPath(ePoints::Point_PathEnd, Path_Jarod, 11, false);
            }
        }

        void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
        {
            if (p_PointId == ePoints::Point_PathEnd)
            {
                if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid))
                {
                    Talk(1, m_SummonerGuid);
                    l_Player->QuestObjectiveOptionalSatisfy(eKillcredits::Killcredit_PathEnded);
                    me->DespawnOrUnsummon();
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_commander_jarod_shadowsong_110658AI(creature);
    }
};

/// Erwind Mistfen, Marnor Lorleaf, Del'thanar Sagewood - 93640, 93602, 93644
class npc_solid_as_a_rock_creatures : public CreatureScript
{
public:
    npc_solid_as_a_rock_creatures() : CreatureScript("npc_solid_as_a_rock_creatures") { }

    struct npc_solid_as_a_rock_creaturesAI : public ScriptedAI
    {
        npc_solid_as_a_rock_creaturesAI(Creature *creature) : ScriptedAI(creature) { }

        enum eNpcs
        {
            Npc_ErwindMistfen = 93640,
            Npc_MarnorLorleaf = 93602,
            Npc_DelthanarSagewood = 93644
        };

        enum eSpells
        {
            Spell_Rousing = 182117
        };

        enum ePoints
        {
            Point_MoveEnd = 1
        };

        void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
        {
            Player* l_Player = p_Caster->ToPlayer();
            if (!l_Player || p_Spell->Id != eSpells::Spell_Rousing)
                return;

            uint64 l_SummonerGuid = l_Player->GetGUID();

            l_Player->KilledMonsterCredit(me->GetEntry());

            switch (me->GetEntry())
            {
            case eNpcs::Npc_ErwindMistfen:
            {
                if (Creature* l_Creature = l_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                {
                    l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    l_Creature->RemoveAllAuras();
                    l_Creature->SetAIAnimKitId(0);

                    l_Creature->AddDelayedEvent([l_Creature, l_SummonerGuid]() -> void
                    {
                        if (Player* l_Summoner = ObjectAccessor::GetPlayer(*l_Creature, l_SummonerGuid))
                        {
                            l_Creature->AI()->Talk(0, l_SummonerGuid);
                        }
                    }, 2000);

                    l_Creature->AddDelayedEvent([l_Creature]() -> void
                    {
                        G3D::Vector3 const Path_ErwindMistfen[23] =
                        {
                            { 2159.097f, 7035.49f, 70.85098f },
                            { 2160.097f, 7037.24f, 70.35098f },
                            { 2161.097f, 7038.99f, 70.10098f },
                            { 2162.097f, 7040.74f, 69.85098f },
                            { 2163.347f, 7042.49f, 69.35098f },
                            { 2164.347f, 7044.24f, 68.85098f },
                            { 2165.347f, 7045.74f, 68.60098f },
                            { 2165.847f, 7046.74f, 68.35098f },
                            { 2166.847f, 7048.49f, 67.85098f },
                            { 2167.347f, 7049.24f, 67.60098f },
                            { 2168.347f, 7050.99f, 67.35098f },
                            { 2168.597f, 7051.49f, 66.85098f },
                            { 2169.847f, 7053.24f, 66.10098f },
                            { 2170.847f, 7054.99f, 65.85098f },
                            { 2171.347f, 7055.74f, 65.60098f },
                            { 2172.347f, 7057.49f, 65.35098f },
                            { 2173.347f, 7059.24f, 65.10098f },
                            { 2174.347f, 7060.99f, 64.85098f },
                            { 2175.347f, 7062.74f, 64.60098f },
                            { 2176.597f, 7064.49f, 64.35098f },
                            { 2177.597f, 7065.99f, 64.10098f },
                            { 2178.597f, 7067.74f, 63.85098f },
                            { 2178.925f, 7068.361f, 63.43956f }
                        };

                        l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_MoveEnd, Path_ErwindMistfen, 23, true);
                    }, 5000);
                }

                break;
            }
            case eNpcs::Npc_MarnorLorleaf:
            {
                if (Creature* l_Creature = l_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                {
                    l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    l_Creature->RemoveAllAuras();
                    l_Creature->SetAIAnimKitId(0);

                    l_Creature->AddDelayedEvent([l_Creature, l_SummonerGuid]() -> void
                    {
                        if (Player* l_Summoner = ObjectAccessor::GetPlayer(*l_Creature, l_SummonerGuid))
                        {
                            l_Creature->AI()->Talk(0, l_SummonerGuid);
                        }
                    }, 1000);

                    l_Creature->AddDelayedEvent([l_Creature]() -> void
                    {
                        G3D::Vector3 const Path_MarnorLorleaf[10] =
                        {
                            { 2385.601f, 7193.236f, 71.43311f },
                            { 2388.851f, 7183.986f, 71.18311f },
                            { 2389.851f, 7181.236f, 70.93311f },
                            { 2390.851f, 7178.486f, 70.93311f },
                            { 2391.65f, 7177.034f, 70.64827f },
                            { 2392.65f, 7176.784f, 70.39827f },
                            { 2394.65f, 7176.534f, 69.89827f },
                            { 2396.4f, 7176.034f, 69.64827f },
                            { 2398.15f, 7175.534f, 69.39827f },
                            { 2399.37f, 7175.185f, 68.69254f }
                        };

                        l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_MoveEnd, Path_MarnorLorleaf, 10, true);
                    }, 5000);
                }

                break;
            }
            case eNpcs::Npc_DelthanarSagewood:
            {
                if (Creature* l_Creature = l_Player->SummonCreature(me->GetEntry(), me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0, 0, l_Player->GetGUID()))
                {
                    l_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    l_Creature->RemoveAllAuras();
                    l_Creature->SetAIAnimKitId(0);

                    l_Creature->AddDelayedEvent([l_Creature, l_SummonerGuid]() -> void
                    {
                        if (Player* l_Summoner = ObjectAccessor::GetPlayer(*l_Creature, l_SummonerGuid))
                        {
                            l_Creature->AI()->Talk(0, l_SummonerGuid);
                        }
                    }, 2000);

                    l_Creature->AddDelayedEvent([l_Creature]() -> void
                    {
                        G3D::Vector3 const Path_DelthanarSagewood[8] =
                        {
                            { 2196.979f, 7192.304f, 58.78127f },
                            { 2195.229f, 7191.304f, 59.03127f },
                            { 2193.729f, 7190.304f, 59.28127f },
                            { 2191.979f, 7189.054f, 59.78127f },
                            { 2190.479f, 7188.304f, 60.28127f },
                            { 2188.729f, 7187.304f, 60.53127f },
                            { 2186.979f, 7186.304f, 60.78127f },
                            { 2182.729f, 7183.804f, 60.53127f }
                        };

                        l_Creature->GetMotionMaster()->MoveSmoothPath(ePoints::Point_MoveEnd, Path_DelthanarSagewood, 8, true);
                    }, 5000);
                }

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
        return new npc_solid_as_a_rock_creaturesAI(creature);
    }
};

/// Ysera - 92705
class npc_ysera_92705 : public CreatureScript
{
public:
    npc_ysera_92705() : CreatureScript("npc_ysera_92705") { }

    struct npc_ysera_92705AI : public ScriptedAI
    {
        npc_ysera_92705AI(Creature *creature) : ScriptedAI(creature) { }

        enum ePoints
        {
            Point_PathEnd = 1
        };

        void IsSummonedBy(Unit* p_Summoner) override
        {
            me->AddPlayerInPersonnalVisibilityList(p_Summoner->GetGUID());
            Talk(0, p_Summoner->GetGUID());

            me->AddDelayedEvent([this]() -> void
            {
                G3D::Vector3 const Path_Ysera[5] =
                {
                    { 2704.47f, 6263.097f, 247.9193f },
                    { 2705.72f, 6246.097f, 275.1693f },
                    { 2687.72f, 6213.097f, 300.4193f },
                    { 2687.97f, 6176.847f, 317.9193f },
                    { 2698.391f, 6139.634f, 332.2152f }
                };

                me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_PathEnd, Path_Ysera, 5);
            }, 4000);
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
        return new npc_ysera_92705AI(creature);
    }
};

/// Unleashed Madness - 188605, 188619
class spell_valsharah_unleashed_madness : public SpellScriptLoader
{
public:
    spell_valsharah_unleashed_madness() : SpellScriptLoader("spell_valsharah_unleashed_madness") { }

    class spell_valsharah_unleashed_madness_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_valsharah_unleashed_madness_SpellScript);

        enum eSpells
        {
            Spell_UnleashedMadnessArea = 188605,
            Spell_UnleashedMadnessMissile = 188612,
            Spell_UnleashedMadnessSummon = 188571
        };

        void HandleAfterHit()
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetHitUnit();
            if (!l_Caster || !l_Target)
                return;

            if (m_scriptSpellId == eSpells::Spell_UnleashedMadnessArea)
            {
                l_Caster->CastSpell(l_Target, eSpells::Spell_UnleashedMadnessMissile, true);
            }
            else
            {
                l_Target->CastSpell(l_Target, eSpells::Spell_UnleashedMadnessSummon, true);
            }
        }

        void Register() override
        {
            AfterHit += SpellHitFn(spell_valsharah_unleashed_madness_SpellScript::HandleAfterHit);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_valsharah_unleashed_madness_SpellScript();
    }
};

#ifndef __clang_analyzer__
void AddSC_valsharah_scripts()
{
    new npc_commander_jarod_shadowsong_92620();
    new npc_bard_110639();
    new npc_commander_jarod_shadowsong_110658();
    new npc_solid_as_a_rock_creatures();
    new npc_ysera_92705();
    new spell_valsharah_unleashed_madness();
}
#endif