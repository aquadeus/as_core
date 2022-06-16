////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "boss_tov_odyn.hpp"

/// Odyn - 114263
class boss_tov_odyn : public CreatureScript
{
    public:
        boss_tov_odyn() : CreatureScript("boss_tov_odyn") { }

        bool OnGossipSelect(Player* p_Player, Creature* p_Creature, uint32 /*p_Sender*/, uint32 p_Action) override
        {
            InstanceScript* l_Instance = p_Creature->GetInstanceScript();
            if (l_Instance == nullptr)
                return false;

            p_Player->SetDisableGravity(true);
            p_Player->NearTeleportTo(g_PositionOdynsPortalToHauvstald);

            p_Player->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_SERVER_CONTROLLED);

            uint64 l_Guid = p_Player->GetGUID();

            l_Instance->AddTimedDelayedOperation(3 * TimeConstants::IN_MILLISECONDS, [this, l_Guid, l_Instance]() -> void
            {
                if (l_Instance == nullptr || !l_Guid)
                    return;

                uint64 l_CreGuid = 0;

                if (Player* l_Player = sObjectAccessor->FindPlayer(l_Guid))
                {
                    l_Player->SetDisableGravity(false);

                    Position l_Dest = *l_Player;

                    l_Dest.m_positionZ = 120.f;

                    if (Creature* l_Trig = l_Player->SummonCreature(eTovCreatures::CreatureWorldTrigger, l_Dest, TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 15 * TimeConstants::IN_MILLISECONDS))
                    {
                        l_Trig->SetDisplayId(11686);

                        l_CreGuid = l_Trig->GetGUID();

                        l_Player->AddAura(231167, l_Player);
                    }
                }

                l_Instance->AddTimedDelayedOperation(4 * TimeConstants::IN_MILLISECONDS - 500, [this, l_Guid, l_CreGuid]() -> void
                {
                    if (!l_Guid || !l_CreGuid)
                        return;

                    if (Player* l_Player = sObjectAccessor->FindPlayer(l_Guid))
                    {
                        if (Creature* l_Creature = Creature::GetCreature(*l_Player, l_CreGuid))
                        {
                            const Position l_Pos =
                            {
                                l_Player->m_positionX, l_Player->m_positionY, 125.f, l_Player->m_orientation
                            };

                            l_Player->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_SERVER_CONTROLLED);

                            l_Player->NearTeleportTo(l_Pos);
                        }
                    }
                });

                l_Instance->AddTimedDelayedOperation(6 * TimeConstants::IN_MILLISECONDS - 300, [this, l_Guid, l_CreGuid]() -> void
                {
                    if (!l_Guid || !l_CreGuid)
                        return;

                    if (Player* l_Player = sObjectAccessor->FindPlayer(l_Guid))
                    {
                        l_Player->RemoveAura(231167);

                        if (Creature* l_Creature = Creature::GetCreature(*l_Player, l_CreGuid))
                            l_Player->CastSpell(l_Creature, 231168, false);
                    }
                });
            });

            return true;
        }

        struct boss_tov_odynAI : public BossAI
        {
            boss_tov_odynAI(Creature* p_Creature) : BossAI(p_Creature, eTovData::DataBossOdyn)
            {
                m_Instance = p_Creature->GetInstanceScript();

                m_TeleportToHaustvald = 0;
                m_EncounterIntroFinished = false;
                m_EncounterFinished = m_Instance ? m_Instance->GetBossState(eTovData::DataBossOdyn) == EncounterState::DONE : false;
            }

            enum eEquipIds
            {
                Default = 1,
                ThirdPhase
            };

            enum eEvents
            {
                /// Intro
                EventIntroOdynTalk01 = 1,
                EventIntroOdynTalk02,
                EventIntroHymdallTalk01,
                EventIntroOdynTalk03,
                EventIntroHyjraTalk01,
                EventIntroOdynTalk04,
                EventIntroFightBegins, ///< Hymdall and hyrja faction changes hostile.

                /// Might Of The Valajar - phase 1
                EventUnerringBlast = 10,
                EventBrand,
                EventStormforged,
                EventArchingStormBuff,

                /// The Prime Designate - phase 2
                EventSpearOfLight,
                EventReinforceRunes, ///< Draw power

                /// Heroic + Mythic
                EventTestOfTheAgest,

                /// The Final Test - phase 3
                EventStormforgedSpear,
                EventStormOfJustice,
                EventArcingStorm,

                /// Mythic
                EventRunicBrand,
                EventRadiantSmite,
                EventProtected
            };

            enum eTimers
            {
                /// Talks
                TalkTimer01                              = 1 * TimeConstants::IN_MILLISECONDS,
                TalkTimer02                              = 14 * TimeConstants::IN_MILLISECONDS,
                TalkTimer03                              = 14 * TimeConstants::IN_MILLISECONDS,
                TalkTimer04                              = 7 * TimeConstants::IN_MILLISECONDS,
                TalkTimer05                              = 7 * TimeConstants::IN_MILLISECONDS,
                TalkTimer06                              = 6 * TimeConstants::IN_MILLISECONDS,

                /// 1st phase
                TimerReinforcementRunes                  = 38 * TimeConstants::IN_MILLISECONDS,
                TimerUnerringBlast                       = 57 * TimeConstants::IN_MILLISECONDS,

                /// 2nd Phase
                TimerSpearOfLight                        = 8 * TimeConstants::IN_MILLISECONDS,
                TimerTestOfTheAges                       = 14 * TimeConstants::IN_MILLISECONDS,

                /// 3rd phase
                TimerRunicBrand                          = 60 * TimeConstants::IN_MILLISECONDS,
                TimerCleansingFlames                     = 35 * TimeConstants::IN_MILLISECONDS,
                TimerRadiantSmite                        = 6 * TimeConstants::IN_MILLISECONDS
            };

            enum eSpells
            {
                Revivfy                                 = 228171,
                SpellTestOfTheChosen                    = 232301,

                SpellOdynsTest01                        = 227626,
                SpellOdynsTest02                        = 228911,
                SpellUnworthyMultiEff01                 = 198189,
                SpellUnworthyPacisfySilence             = 198190,

                RunicBrandDummy                         = 197961,
                RunicBrandPeriodicDmg                   = 197963,
                RunicBrandPeriodicDmg02                 = 197964,
                RunicBrandPeriodicDmg03                 = 197965,
                RunicBrandPeriodicDmg04                 = 197966,
                RunicBrandPeriodicDmg05                 = 197967,
                RunicBrandAreatrigger                   = 197968,
                RunicBrandAreatrigger02                 = 197971,
                RunicBrandAreatrigger03                 = 197972, ///< Yellow
                RunicBrandAreatrigger04                 = 197975,
                RunicBrandAreatrigger05                 = 197977,
                RunicBrandPeriodicTriggerSpell          = 198381,
                RunicBrandDummy02                       = 198391,
                RunicBrandDummy03                       = 227602,
                RunicBrandDummy04                       = 227610, /// amazing cleansing flame
                RunicBrandAreatrigger11                 = 227611, ///< H
                RunicBrandAreatrigger12                 = 227612, ///< A
                RunicBrandAreatrigger13                 = 227614, ///< I
                RunicBrandCleanUp                       = 228243,

                RunicBrandAreatrigger14                 = 229201,
                RunicBrandAreatrigger15                 = 229601,
                RunicBrandAreatrigger16                 = 229602,
                RunicBrandAreatrigger17                 = 229603,
                RunicBrandAreatrigger18                 = 229604,
                RunicBrandAreatrigger19                 = 229605,
                RunicBrandAreatrigger20                 = 229198,
                RunicBrandAreatrigger21                 = 229613,
                RunicBrandAreatrigger22                 = 229614,
                RunicBrandAreatrigger23                 = 229615,
                RunicBrandAreatrigger24                 = 229616,
                RunicBrandAreatrigger25                 = 231297,
                RunicBrandPeriodicDummy                 = 231384,

                RunicMastery                            = 229684,

                RunicBrandAuraDummyN                    = 231342, ///< N
                RunicBrandAuraDummyK                    = 231311, ///< K
                RunicBrandAuraDummyH                    = 231344, ///< H
                RunicBrandAuraDummyA                    = 231345, ///< A
                RunicBrandAuraDummyI                    = 231346, ///< I

                /// Branded
                BrandedMultiEff01                       = 197996,
                BrandedFixateK                          = 227490,
                BrandedFixateN                          = 227491,
                BrandedFixateH                          = 227498,
                BrandedFixateA                          = 227499,
                BrandedFixateI                          = 227500,
                BrandedMythicK                          = 229579,
                BrandedMythicN                          = 229580,
                BrandedMythicH                          = 229581,
                BrandedMythicA                          = 229582,
                BrandedMythicI                          = 229583,

                /// Spear of Light
                SpearOfLightWpnDmg                      = 192017,
                SpearOfLightMultiEff01                  = 198058,
                SpearOfLightAuraAt                      = 198059,
                SpearOfLightTriggerMissDummy            = 198060,
                SpearOfLightDummy                       = 198072,
                SpearOfLightDummy02                     = 198284,
                SpearOfLightDummy03                     = 198285,
                SpearOfLightDummy04                     = 198396,
                SpearOfLightPeriodicDmg                 = 200988,
                SpearOfLightDummy05                     = 202054,
                SpearOfLightDummy06                     = 202055,
                SpearOfLightDummy07                     = 202058,
                SpearOfLightStunDmg                     = 214203,
                SpearOfLightSummonInstaKill             = 221872,
                SpearOfLightDummy08                     = 227697,
                SpearOfLightMultiEff02                  = 227712,
                SpearOfLightAuraAt02                    = 227785,
                SpearOfLightImmunityAuraDummy           = 227815,
                SpearOfLightTriggerMissile              = 228870,
                SpearOfLightTriggerMissile02            = 233270,

                /// Glowing Fragment
                GlowingFragmentAreatrigger              = 198078,
                GlowingFragmentMultiEff                 = 198088,
                GlowingFragmentAuraDummy                = 205807,
                GlowingFragmentMultiEff02               = 227781,
                GlowingFragmentAreatrigger02            = 227782,

                /// Shatter Spears
                ShatterSpearsAT                         = 198077,
                ShatterSpearsDummy                      = 231013,
                ShatterSpearsDmg                        = 231019,
                ShatterDummyDmg                         = 231016,
                ShatterSpearsDmg02                      = 213109,
                ShatterAuraDummy                        = 231582,

                /// Radiant Tempest
                RadiantTempestDmgKnockback              = 198263,
                RadiantTempestForcePlayerInteraciton    = 201006,
                RadiantTempestAuraDummyJumpDst          = 201008,

                /// Stormforged Oblietrator
                SummonStormforgedOblietrator            = 201215,
                SummonStormforged                       = 201209,
                Surge                                   = 198750,

                /// Stormforged Valajar
                StormforgedValajarSummon                = 218784,
                StormforgedValajarSummon02              = 218785,
                StormforgedValajarSummon03              = 218786,

                StormforgedSpearTriggerMissile          = 228914,
                StormforgedSpearSchoolDmg               = 228915,
                StormforgedSpearPeriodicDummy           = 228918,
                StormforgedSpearDmgAuraMechanic         = 228932,

                ValarjarGhostVisual                     = 193515,
                RageOfTheValajar                        = 200845,
                ValajarsPathStatAuraDummy               = 215956, ///< Some connection to Iron of Winter.
                ValajarsPathTransform                   = 215964,
                ValajarsPathTransform03                 = 216010,
                ValajarsPathTransform04                 = 216011,
                ValajarsPathTransform05                 = 216012,
                ValajarsPathTransform06                 = 216013,
                ValajarsPathTransform07                 = 216014,
                ValajarsPathTransform08                 = 216015,
                ValajarAspirantsSummon                  = 218781,
                ValajarAspirantsSummon02                = 218782,
                ValajarAspirantsSummon03                = 218783,

                ValajarBondTriggerSpell                 = 229469,
                ValajarBondDmgTakenDummy                = 229529,
                ValajarBondDummy                        = 229530,
                ValajarBondAuraAt                       = 228016,
                ValajarBondDmgTaken                     = 228018,

                ValajarMight                            = 228024,

                ValajarInsignia                         = 228617,

                UnerringBlastDummyEnergize              = 227629,
                UnerringBlastDamageEnergize             = 229644,
                UnerringBlastTriggerMissile             = 230989,
                UnerringBlastTriggerMisisle             = 231277,
                UnerringBlastSchoolDamage               = 231278,
                UnerringBlastSchoolDamage02             = 230990,

                DrawPowerPeriodicTriggerSpell           = 180185,
                DrawPowerTriggerSpellWithValue          = 180186,
                DrawPowerPeriodicTriggerSpell02         = 196090,
                DrawPowerTriggerMissile                 = 216976,
                DrawPowerAreaTrigger                    = 216977,
                DrawPowerTriggerMissile02               = 216978,
                DrawPowerSlowSpeed                      = 216983,
                DrawPowerAt                             = 227503,

                DrawPowerTriggerMissile03               = 227963, ///< Anim of activation?

                DrawPowerPeriodicDummy02                = 228297,
                DrawPowerDummy                          = 229576,
                DrawPowerPeriodicDummy01                = 229577,
                DrawPowerMythicCleanUp                  = 233002,

                Protected                               = 229584,
                ProtectedByKhadgar                      = 229964,
                ProtectedGround                         = 233897,

                StormOfJusticeSummon                    = 227802,
                StormOfJusticePeriodicTriggerSpell      = 227807,
                StormOfJusticeSchoolDmg                 = 227808,
                StormOfJusticePeriodicTriggerSpell02    = 227959,
                StormOfJusticeAuraDummy                 = 232250,

                RagingTempestTriggerMissile             = 24865,
                RagingTempestEffSummon                  = 24866,
                RagingTempestPeriodicDamage             = 24867,
                RagingTempestAuraPeriodicDummy          = 24868,
                RagingTempestAuraAreatrigger            = 27804,
                RagingTempestSchoolDmg                  = 27811,
                RagingTempestDummy                      = 28674,

                /// Cleansing Flames
                CleansingFlamesPeriodicTriggerSpell     = 227475,
                CleansingFlamesDummy                    = 227460,

                ArcingStormPeriodicDummyDummy           = 229254,
                ArcingStormDmg                          = 229255,
                ArcingStormModDmgDoneForMechanic        = 229256,

                RadiantSmiteDummy                       = 231350,
                RadiantSmiteDmg                         = 231355,

                EchoOfRadianceDmg                       = 231357,

                RunebearerSpawnVisu                     = 230415,

                DancingBladeDot                         = 193234,

                RunicShieldDmgPctTakenI                 = 227594,
                RunicShieldDmgPctTakenA                 = 227595,
                RunicShieldDmgPctTakenH                 = 227596,
                RunicShieldDmgPctTakenN                 = 227597,
                RunicShieldDmgPctTakenK                 = 227598,

                DrawPowerDmgIncUnerringBlastK           = 227961, ///< K ///< auras gained from the runes.
                DrawPowerDmgIncUnerringBlastN           = 227973, ///< N
                DrawPowerDmgIncUnerringBlastH           = 227974, ///< H
                DrawPowerDmgIncUnerringBlastA           = 227975, ///< A
                DrawPowerDmgIncUnerringBlastI           = 227976, ///< i

                RunicBrandAreatriggerVisK               = 227483, ///< K
                RunicBrandAreatriggerVisN               = 227484, ///< N
                RunicBrandAreatriggerVisH               = 227485, ///< H
                RunicBrandAreatriggerVisA               = 227487, ///< A
                RunicBrandAreatriggerVisI               = 227488  ///< I
            };

            const std::vector<uint32> m_SpellToReset =
            {
                eSpells::ValajarBondTriggerSpell,
                eSpells::ValajarBondDmgTakenDummy,
                eSpells::ValajarBondDummy,
                eSpells::ValajarBondAuraAt,
                eSpells::ValajarBondDmgTaken,
                eSpells::RunebearerSpawnVisu,
                eSpells::RunicBrandAuraDummyN,
                eSpells::RunicBrandAuraDummyK,
                eSpells::RunicBrandAuraDummyH,
                eSpells::RunicBrandAuraDummyA,
                eSpells::RunicBrandAuraDummyI,
                eSpells::RunicShieldDmgPctTakenI,
                eSpells::RunicShieldDmgPctTakenA,
                eSpells::RunicShieldDmgPctTakenH,
                eSpells::RunicShieldDmgPctTakenN,
                eSpells::RunicShieldDmgPctTakenK,
                eSpells::BrandedFixateK,
                eSpells::BrandedFixateN,
                eSpells::BrandedFixateH,
                eSpells::BrandedFixateA,
                eSpells::BrandedFixateI,
                eSpells::DrawPowerDmgIncUnerringBlastK,
                eSpells::DrawPowerDmgIncUnerringBlastN,
                eSpells::DrawPowerDmgIncUnerringBlastH,
                eSpells::DrawPowerDmgIncUnerringBlastA,
                eSpells::DrawPowerDmgIncUnerringBlastI,
                eSpells::RunicBrandAuraDummyN,
                eSpells::DancingBladeDot,
                228007,
                eSpells::CleansingFlamesDummy,
                eSpells::SpearOfLightStunDmg,
                eSpells::Protected,
                eSpells::RunicBrandAreatriggerVisK,
                eSpells::RunicBrandAreatriggerVisN,
                eSpells::RunicBrandAreatriggerVisA,
                eSpells::RunicBrandAreatriggerVisI,
                eSpells::RunicBrandAreatriggerVisH,
                eSpells::CleansingFlamesPeriodicTriggerSpell,
                eSpells::StormOfJusticePeriodicTriggerSpell,
                eSpells::StormforgedSpearPeriodicDummy,
                eSpells::StormforgedSpearDmgAuraMechanic
            };

            InstanceScript* m_Instance;

            EventMap m_RpEvents;

            std::list<uint32> m_RandomizedRunesList;
            std::list<uint32> m_RandomizedRunesListKillOrder;

            bool m_Achievement;
            bool m_EncounterFinished;
            bool m_EncounterIntroFinished;
            bool m_EncounterIntroStarted;
            bool m_EncounterStarted;
            bool m_TestOfTheAges[2]; /// Hymdall, Hyrja
            bool m_CleansingFlames;
            bool m_HalfTick;

            uint32 m_TeleportToHaustvald;
            int32 m_CleansingFlamesSpawnerDiff;
            uint32 m_ValarjarsBondDiff;
            uint32 m_CheckForNearbyPlayersToStartIntroDiff;
            uint32 m_CheckBrandedTargetsNearRunesGobsDiff;
            uint32 m_RevivifyDiff;
            uint32 m_CheckForCombatDiff;
            uint32 m_CheckForNoCombatDiff;
            uint32 m_PowerCount;
            uint32 m_PowerTimer;

            uint64 m_OdynsTestGuid;

            uint8 m_PhaseId;
            uint8 m_RegenerationSetting; ///< 0 - 0. 1 - draw power. 2 - radiant smite.

            int8 m_LastCleansingFlameIDx;

            uint32 m_StormOfJusticeCount = 1;
            uint32 m_StormforgedSpearCount = 1;

            void Reset() override
            {
                AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    if (!m_EncounterFinished && m_Instance)
                        m_EncounterFinished = m_Instance->GetBossState(eTovData::DataBossOdyn) == EncounterState::DONE;

                    if (m_EncounterFinished)
                    {
                        me->SetSheath(SheathState::SHEATH_STATE_UNARMED);

                        me->SetReactState(ReactStates::REACT_PASSIVE);

                        me->SetFlag(EUnitFields::UNIT_FIELD_NPC_FLAGS, NPCFlags::UNIT_NPC_FLAG_GOSSIP | NPCFlags::UNIT_NPC_FLAG_QUESTGIVER);

                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);

                        AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            me->GetMotionMaster()->Clear(false);
                            me->GetMotionMaster()->MovePoint(2, g_PositionOdynJumpTo2ndPhase, false);
                        });

                        return;
                    }

                    DefaultNpcCalls();

                    if (IsMythic())
                        me->UpdateStatsForLevel();
                });
            }

            void GetMythicHealthMod(float& p_Mod) override
            {
                p_Mod = 2.90f;
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                _EnterCombat();
            }

            void EnterEvadeMode() override
            {
                CreatureAI::EnterEvadeMode();

                me->GetMotionMaster()->Clear(true);
                me->NearTeleportTo(me->GetHomePosition());
                me->Respawn(true, true, 30 * TimeConstants::IN_MILLISECONDS);

                me->InterruptNonMeleeSpells(true);

                if (m_Instance != nullptr)
                {
                    m_Instance->SetBossState(eTovData::DataBossOdyn, EncounterState::FAIL);

                    if (Creature* l_Hyrja = Creature::GetCreature(*me, m_Instance->GetData64(eTovCreatures::CreatureHyrja)))
                    {
                        l_Hyrja->GetMotionMaster()->Clear(false);

                        l_Hyrja->SetAnimTier(1);

                        l_Hyrja->SetCanFly(false, true);

                        l_Hyrja->SetDisableGravity(false);

                        l_Hyrja->ClearUnitState(UnitState::UNIT_STATE_ROOT);

                        l_Hyrja->SetReactState(ReactStates::REACT_AGGRESSIVE);

                        l_Hyrja->ClearUnitState(UnitState::UNIT_STATE_CANNOT_AUTOATTACK);

                        l_Hyrja->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                        l_Hyrja->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_SERVER_CONTROLLED | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                        l_Hyrja->NearTeleportTo(l_Hyrja->GetHomePosition());
                        l_Hyrja->Respawn(true, true, 30 * TimeConstants::IN_MILLISECONDS);

                        if (m_EncounterIntroFinished)
                        {
                            /// Combat stance
                            l_Hyrja->setFaction(16);

                            l_Hyrja->CastSpell(l_Hyrja, eTovSpells::SpellReady1h, true);
                        }
                    }

                    if (Creature* l_Hymdall = Creature::GetCreature(*me, m_Instance->GetData64(eTovCreatures::CreatureHymdall)))
                    {
                        l_Hymdall->GetMotionMaster()->Clear(false);

                        l_Hymdall->ClearUnitState(UnitState::UNIT_STATE_ROOT);

                        l_Hymdall->SetReactState(ReactStates::REACT_AGGRESSIVE);

                        l_Hymdall->ClearUnitState(UnitState::UNIT_STATE_CANNOT_AUTOATTACK);

                        l_Hymdall->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                        l_Hymdall->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_SERVER_CONTROLLED | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                        l_Hymdall->NearTeleportTo(l_Hymdall->GetHomePosition());
                        l_Hymdall->Respawn(true, true, 30 * TimeConstants::IN_MILLISECONDS);

                        if (m_EncounterIntroFinished)
                        {
                            /// Combat stance
                            l_Hymdall->setFaction(16);

                            l_Hymdall->CastSpell(l_Hymdall, eTovSpells::SpellReady2h, true);
                        }
                    }

                    if (GameObject* l_Door = GameObject::GetGameObject(*me, m_Instance->GetData64(eTovGameObjects::GameObjectOdynGate)))
                    {
                        l_Door->ResetLootMode();
                        l_Door->ResetDoorOrButton();
                    }

                    for (uint32 l_Iter : g_Runes)
                    {
                        if (!l_Iter)
                            continue;

                        if (GameObject* l_Itr = m_Instance->instance->GetGameObject(m_Instance->GetData64(l_Iter)))
                        {
                            l_Itr->ResetLootMode();
                            l_Itr->ResetDoorOrButton();
                        }
                    }
                }

                me->setFaction(16);
            }

            uint32 GetData(uint32 p_ID /* = 0 */) override
            {
                return m_TeleportToHaustvald;
            }

            void ActivateRuneAndPlayer(uint64 l_PlayerGuid, uint8 l_I)
            {
                if (l_I < 5 && l_PlayerGuid != 0)
                {
                    if (Player* l_Player = Player::GetPlayer(*me, l_PlayerGuid))
                    {
                        if (GameObject* l_Object = GameObject::GetGameObject(*me, m_Instance->GetData64(g_Runes[l_I])))
                        {
                            /// Obj activation
                            l_Object->SetLootState(LootState::GO_READY);
                            l_Object->UseDoorOrButton(1 * TimeConstants::IN_MILLISECONDS, false, me);

                            uint32 l_ObjAura = 0;

                            switch (l_Object->GetEntry())
                            {
                                case eTovGameObjects::GameObjectOdynRuneSigilK:
                                {
                                    l_ObjAura = eTovSpells::RunicBrandAreatriggerVisK;

                                    break;
                                }
                                case eTovGameObjects::GameObjectOdynRuneSigilN:
                                {
                                    l_ObjAura = eTovSpells::RunicBrandAreatriggerVisN;

                                    break;
                                }
                                case eTovGameObjects::GameObjectOdynRuneSigilI:
                                {
                                    l_ObjAura = eTovSpells::RunicBrandAreatriggerVisI;

                                    break;
                                }
                                case eTovGameObjects::GameObjectOdynRuneSigilA:
                                {
                                    l_ObjAura = eTovSpells::RunicBrandAreatriggerVisA;

                                    break;
                                }
                                case eTovGameObjects::GameObjectOdynRuneSigilH:
                                {
                                    l_ObjAura = eTovSpells::RunicBrandAreatriggerVisH;

                                    break;
                                }
                                default:
                                    break;
                            }

                            /// Gob  vis aura
                            const SpellInfo* l_SpellInfo = sSpellMgr->GetSpellInfo(l_ObjAura, me->GetMap()->GetDifficultyID());

                            if (l_SpellInfo)
                            {
                                AreaTrigger* l_AreaTrigger = new AreaTrigger;

                                if (!l_AreaTrigger->CreateAreaTrigger(sObjectMgr->GenerateLowGuid(HIGHGUID_AREATRIGGER), 8784, me, nullptr, l_SpellInfo, *l_Object, *l_Object))
                                {
                                    delete l_AreaTrigger;
                                    return;

                                    l_AreaTrigger->SetDuration(30 * TimeConstants::IN_MILLISECONDS);
                                }
                            }

                            /// Pos for valarjar aseto spawn
                            Position l_PosTemp = g_PositionValajrRunebearersSpawns[l_I];

                            if (Creature* l_ValajarRuneBearer = l_Object->SummonCreature(eTovCreatures::CreatureValajarRuneBearer, l_PosTemp, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                            {
                                l_ValajarRuneBearer->CastSpell(l_ValajarRuneBearer, eSpells::RunebearerSpawnVisu, true);

                                uint32 l_DmgImmunity = 0;

                                if (UnitAI* l_AI = l_ValajarRuneBearer->GetAI())
                                {
                                    l_AI->SetGUID(l_Object->GetEntry(), 1); ///< SetGuidChooseObj.
                                    l_AI->SetGUID(l_Player->GetGUID(), 2);  ///< SetGuidChooseTar
                                }
                            }
                        }
                    }
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case 1: ///< 2nd phase jump
                    {
                        if (p_Type == EFFECT_MOTION_TYPE)
                        {
                            AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                me->CastSpell(me, eSpells::SpellOdynsTest01, true);

                                events.Reset();

                                events.ScheduleEvent(eEvents::EventSpearOfLight, eTimers::TimerSpearOfLight);
                                events.ScheduleEvent(eEvents::EventReinforceRunes, eTimers::TimerReinforcementRunes);

                                if (Player* l_Nearest = me->FindNearestPlayer(125.0f))
                                    me->GetMotionMaster()->MoveChase(l_Nearest);

                                SetCombatMovement(true);

                                if (IsHeroic() || IsMythic())
                                    events.ScheduleEvent(eEvents::EventTestOfTheAgest, eTimers::TimerTestOfTheAges + 10000);
                            });
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionStartPhase02:
                    {
                        if (m_PhaseId != 2)
                        {
                            if (m_Instance != nullptr)
                            {
                                if (Creature* l_Hyrja = Creature::GetCreature(*me, m_Instance->GetData64(eTovCreatures::CreatureHyrja)))
                                {
                                    if (Creature* l_Hymdall = Creature::GetCreature(*me, m_Instance->GetData64(eTovCreatures::CreatureHymdall)))
                                    {
                                        m_PhaseId = 2;

                                        if (l_Hyrja->IsAIEnabled && l_Hymdall->IsAIEnabled)
                                        {
                                            l_Hyrja->AI()->SetData(0, m_PhaseId);
                                            l_Hymdall->AI()->SetData(0, m_PhaseId);
                                        }

                                        me->SetSheath(SheathState::SHEATH_STATE_MELEE);

                                        Talk(eGeneralTalks::TalkPhase02);

                                        events.Reset();

                                        ClearDelayedOperations();

                                        for (uint32 l_Iter : g_Runes)
                                        {
                                            if (!l_Iter)
                                                continue;

                                            if (GameObject* l_Itr = m_Instance->instance->GetGameObject(m_Instance->GetData64(l_Iter)))
                                            {
                                                if (l_Itr->GetLootMode() == GO_ACTIVATED)
                                                {
                                                    l_Itr->ResetLootMode();
                                                    l_Itr->ResetDoorOrButton();
                                                }
                                            }
                                        }

                                        ClearDrawPowerAuras();

                                        m_RegenerationSetting = 0;

                                        me->SetPower(me->getPowerType(), 0, false);

                                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_REGENERATE_POWER);

                                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);

                                        l_Hymdall->StopMoving();
                                        l_Hyrja->StopMoving();

                                        l_Hymdall->AttackStop();
                                        l_Hyrja->AttackStop();

                                        l_Hyrja->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_SERVER_CONTROLLED);
                                        l_Hymdall->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_SERVER_CONTROLLED);

                                        l_Hyrja->SetAnimTier(3);

                                        l_Hyrja->SetDisableGravity(true);

                                        l_Hyrja->SetCanFly(true, true);

                                        l_Hyrja->AddUnitState(UnitState::UNIT_STATE_CANNOT_AUTOATTACK);
                                        l_Hymdall->AddUnitState(UnitState::UNIT_STATE_CANNOT_AUTOATTACK);

                                        l_Hyrja->SetReactState(ReactStates::REACT_PASSIVE);
                                        l_Hymdall->SetReactState(ReactStates::REACT_PASSIVE);

                                        l_Hyrja->SetHealth(l_Hyrja->GetMaxHealth());
                                        l_Hymdall->SetHealth(l_Hyrja->GetMaxHealth());

                                        //l_Hyrja->setFaction(35);
                                        //l_Hymdall->setFaction(35);

                                        m_Instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, l_Hymdall);
                                        m_Instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, l_Hyrja);

                                        m_TestOfTheAges[0] = true;
                                        m_TestOfTheAges[1] = false;

                                        DespawnCreaturesInArea(eTovCreatures::CreatureDancingBlade, me);

                                        l_Hyrja->GetMotionMaster()->Clear(false);
                                        l_Hyrja->GetMotionMaster()->MoveSmoothFlyPath(2, g_HyrjaReturnBack.data(), g_HyrjaReturnBack.size());

                                        l_Hymdall->GetMotionMaster()->Clear(false);
                                        l_Hymdall->GetMotionMaster()->MoveJump(2403.4414f, 480.9613f, 773.2640f, 15.0f, 8.0f, 2);

                                        me->GetMotionMaster()->MoveJump(g_PositionOdynJumpTo2ndPhase.m_positionX, g_PositionOdynJumpTo2ndPhase.m_positionY, g_PositionOdynJumpTo2ndPhase.m_positionZ, 30.0f, 10.0f, 10.0f, 1);
                                    }
                                }
                            }
                        }

                        break;
                    }
                    case eActions::ActionOdynRunes:
                    {
                        if (m_Instance != nullptr)
                        {
                            m_RegenerationSetting = 1;

                            me->SetPower(me->getPowerType(), int32(0), false);

                            me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_REGENERATE_POWER);

                            std::list<Player*> l_ListPlayers;
                            me->GetPlayerListInGrid(l_ListPlayers, 200.0f, false);

                            l_ListPlayers.remove_if([this](Player* p_Player) -> bool
                            {
                                if (p_Player == nullptr || p_Player->isDead() || p_Player->IsActiveSpecTankSpec())
                                    return true;

                                return false;
                            });

                            uint8 l_I = 0;

                            if (IsHeroic())
                            {
                                uint32 l_SpellDmgEntries[5] =
                                {
                                    DrawPowerDmgIncUnerringBlastK, ///< K ///< auras gained from the runes.
                                    DrawPowerDmgIncUnerringBlastN, ///< N
                                    DrawPowerDmgIncUnerringBlastH, ///< H
                                    DrawPowerDmgIncUnerringBlastA, ///< A
                                    DrawPowerDmgIncUnerringBlastI ///< i
                                };

                                for (Player* l_Itr : l_ListPlayers)
                                {
                                    for (uint32 l_Iter : l_SpellDmgEntries)
                                        l_Itr->AddAura(l_Iter, l_Itr);
                                }
                            }

                            for (Player* l_Player : l_ListPlayers)
                            {
                                if (l_I > 4)
                                    continue;

                                ActivateRuneAndPlayer(l_Player->GetGUID(), l_I++);
                            }

                            if (l_I < 5)
                            {
                                l_ListPlayers.clear();

                                for (uint8 l_It = l_I; l_It < 5; l_It++)
                                {
                                    me->GetPlayerListInGrid(l_ListPlayers, 150.0f, false);

                                    if (l_ListPlayers.empty())
                                        return;

                                    JadeCore::RandomResizeList(l_ListPlayers, l_ListPlayers.size());

                                    if (Player* l_Nearest = l_ListPlayers.front())
                                    {
                                        if (l_Nearest->GetGUID())
                                            ActivateRuneAndPlayer(l_Nearest->GetGUID(), l_I++);
                                    }
                                }
                            }
                        }

                       break;
                    }
                    case eActions::ActionStartPhase03:
                    {
                        ClearDelayedOperations();

                        Talk(eGeneralTalks::TalkPhase03);

                        me->RemoveAura(eSpells::SpellOdynsTest01);

                        me->RemoveAura(eSpells::ValajarBondDmgTaken);

                        me->AddAura(eSpells::ArcingStormPeriodicDummyDummy, me);

                        me->SetPower(me->getPowerType(), 0, false);

                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_REGENERATE_POWER);

                        m_CleansingFlames = true;
                        m_CleansingFlamesSpawnerDiff = 10 * TimeConstants::IN_MILLISECONDS;

                        me->SetDisableGravity(false);

                        me->SetCanFly(false, true);

                        if (m_Instance != nullptr)
                        {
                            if (Creature* l_Hyrja = Creature::GetCreature(*me, m_Instance->GetData64(eTovCreatures::CreatureHyrja)))
                            {
                                if (Creature* l_Hymdall = Creature::GetCreature(*me, m_Instance->GetData64(eTovCreatures::CreatureHymdall)))
                                {
                                    l_Hymdall->StopMoving();
                                    l_Hyrja->StopMoving();

                                    l_Hymdall->CombatStop();
                                    l_Hyrja->CombatStop();

                                    l_Hyrja->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_SERVER_CONTROLLED);
                                    l_Hymdall->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_SERVER_CONTROLLED);

                                    l_Hyrja->SetAnimTier(3);

                                    l_Hyrja->SetDisableGravity(true);

                                    l_Hyrja->SetCanFly(true, true);

                                    l_Hyrja->AddUnitState(UnitState::UNIT_STATE_CANNOT_AUTOATTACK);
                                    l_Hymdall->AddUnitState(UnitState::UNIT_STATE_CANNOT_AUTOATTACK);

                                    l_Hyrja->SetReactState(ReactStates::REACT_PASSIVE);
                                    l_Hymdall->SetReactState(ReactStates::REACT_PASSIVE);

                                    l_Hyrja->SetHealth(l_Hyrja->GetMaxHealth());
                                    l_Hymdall->SetHealth(l_Hyrja->GetMaxHealth());

                                    //l_Hyrja->setFaction(35);
                                    //l_Hymdall->setFaction(35);

                                    m_Instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, l_Hymdall);
                                    m_Instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, l_Hyrja);

                                    m_TestOfTheAges[0] = true;
                                    m_TestOfTheAges[1] = false;

                                    l_Hyrja->GetMotionMaster()->Clear(false);
                                    l_Hyrja->GetMotionMaster()->MoveSmoothFlyPath(2, g_HyrjaReturnBack.data(), g_HyrjaReturnBack.size());

                                    l_Hymdall->GetMotionMaster()->Clear(false);
                                    l_Hymdall->GetMotionMaster()->MoveJump(2403.4414f, 480.9613f, 773.2640f, 15.0f, 8.0f, 2);
                                }
                            }

                            me->SetPower(me->getPowerType(), 0, false);

                            for (uint32 l_Iter : g_Runes)
                            {
                                if (!l_Iter)
                                    continue;

                                if (GameObject* l_Itr = m_Instance->instance->GetGameObject(m_Instance->GetData64(l_Iter)))
                                {
                                    if (l_Itr->GetLootMode() == GO_ACTIVATED)
                                    {
                                        l_Itr->ResetLootMode();
                                        l_Itr->ResetDoorOrButton();
                                    }
                                }
                            }

                            ClearDrawPowerAuras();
                        }

                        me->LoadEquipment(2, true);

                        events.CancelEvent(eEvents::EventTestOfTheAgest);

                        events.Reset();

                        events.ScheduleEvent(eEvents::EventReinforceRunes, 21 * TimeConstants::IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventStormOfJustice, 4 * TimeConstants::IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventArchingStormBuff, 10 * TimeConstants::IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventStormforgedSpear, 9 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eActions::ActionWinEncounter:
                    {
                        _JustDied();
                        me->RemoveAllAuras();
                        ClearDelayedOperations();
                        me->ClearLootContainers();

                        me->SetReactState(ReactStates::REACT_PASSIVE);
                        me->SetSheath(SheathState::SHEATH_STATE_UNARMED);
                        me->CombatStop();
                        me->AttackStop();
                        me->getThreatManager().clearReferences();
                        me->setFaction(FACTION_FRIENDLY);

                        m_EncounterFinished = true;

                        m_CleansingFlames = false;

                        me->HandleEmoteCommand(53); ///< Roar emote.

                        Talk(eGeneralTalks::TalkEndFight);

                        me->SetPower(me->getPowerType(), 0, false);

                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_REGENERATE_POWER);

                        DespawnCreaturesInArea(eTovCreatures::CreatureRagingTempest, me);
                        DespawnCreaturesInArea(eTovCreatures::CreatureValajarRuneBearer, me);
                        DespawnCreaturesInArea(eTovCreatures::CreatureDancingBlade, me);
                        DespawnCreaturesInArea(eTovCreatures::CreatureValajarRuneBearer, me);

                        me->SetFlag(EUnitFields::UNIT_FIELD_NPC_FLAGS, NPCFlags::UNIT_NPC_FLAG_GOSSIP | NPCFlags::UNIT_NPC_FLAG_QUESTGIVER);
                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);
                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                        me->GetMotionMaster()->Clear(false);
                        me->GetMotionMaster()->MovePoint(2, g_PositionOdynJumpTo2ndPhase, false);

                        if (IsMythic())
                            DoCast(me, eSpells::SpellTestOfTheChosen, true);

                        AddTimedDelayedOperation(6 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            me->GetMotionMaster()->Clear(false);
                            me->GetMotionMaster()->MovePoint(3, 2419.9524f, 528.2512f, 748.9950f);

                            if (instance)
                                instance->DoCombatStopOnPlayers();
                        });

                        if (!instance)
                            return;

                        instance->UpdateEncounterState(EncounterCreditType::ENCOUNTER_CREDIT_KILL_CREATURE, me->GetEntry(), me);

                        if (Player* l_Player = me->FindNearestPlayer(50.0f))
                        {
                            if (InstanceMap* l_InsMap = instance->instance->ToInstanceMap())
                                l_InsMap->PermBindAllPlayers(l_Player);
                        }

                        instance->DoCombatStopOnPlayers();

                        if (m_Achievement)
                            instance->DoCompleteAchievement(eTovAchievements::YouRunedEverything);

                        bool l_DisabledLoots = sObjectMgr->IsDisabledMap(instance->instance->GetId(), instance->instance->GetDifficultyID());

                        if (l_DisabledLoots && sObjectMgr->GetItemContextOverride(instance->instance->GetId(), instance->instance->GetDifficultyID()) != ItemContext::None)
                            l_DisabledLoots = false;

                        if (!l_DisabledLoots)
                        {
                            if (!me->FindNearestGameObject(eTovGameObjects::GameObjectOdynChest, 100.f))
                                me->SummonGameObject(eTovGameObjects::GameObjectOdynChest, 2397.817f, 528.519f, 749.011f, 3.1535479f, 0.f, 0.f, 0.f, 0.f, 60 * TimeConstants::IN_MILLISECONDS);
                        }

                        if (GameObject* l_Door = GameObject::GetGameObject(*me, instance->GetData64(eTovGameObjects::GameObjectOdynGate)))
                        {
                            l_Door->ResetLootMode();
                            l_Door->ResetDoorOrButton();
                        }

                        if (Creature* l_Hyrja = Creature::GetCreature(*me, instance->GetData64(eTovCreatures::CreatureHyrja)))
                        {
                            instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, l_Hyrja);
                            l_Hyrja->setFaction(FACTION_FRIENDLY);
                            l_Hyrja->CombatStop();
                            l_Hyrja->DespawnOrUnsummon();
                        }

                        if (Creature* l_Hymdall = Creature::GetCreature(*me, instance->GetData64(eTovCreatures::CreatureHymdall)))
                        {
                            instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, l_Hymdall);
                            l_Hymdall->setFaction(FACTION_FRIENDLY);
                            l_Hymdall->CombatStop();
                            l_Hymdall->DespawnOrUnsummon();
                        }

                        instance->DoCombatStopOnPlayers();

                        for (uint32 l_Itr : m_SpellToReset)
                            instance->DoRemoveAurasDueToSpellOnPlayers(l_Itr);

                        for (uint32 l_Iter : g_Runes)
                        {
                            if (GameObject* l_Rune = GameObject::GetGameObject(*me, instance->GetData64(l_Iter)))
                            {
                                l_Rune->ResetLootMode();
                                l_Rune->ResetDoorOrButton();
                            }
                        }

                        for (uint32 l_AtVisualItr : g_AtsVisual)
                        {
                            std::list<AreaTrigger*> l_ListAts;

                            me->GetAreaTriggerListWithSpellIDInRange(l_ListAts, l_AtVisualItr, 100.0f);

                            for (AreaTrigger* l_Itr : l_ListAts)
                            {
                                if (l_Itr == nullptr || l_AtVisualItr != l_Itr->GetSpellId())
                                    continue;

                                l_Itr->Remove();
                            }
                        }
                        

                        break;
                    }
                    case eActions::ActionOdynIntro:
                    {
                        m_RpEvents.ScheduleEvent(eEvents::EventIntroOdynTalk01, eTimers::TalkTimer01);

                        break;
                    }
                    case 10: ///< Schedule Test of the ages timer
                    {
                        events.ScheduleEvent(eEvents::EventTestOfTheAgest, 50 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                DoAction(eActions::ActionWinEncounter);
            }

            void KilledUnit(Unit* p_Victim) override
            {
                if (p_Victim == nullptr || !p_Victim->IsPlayer())
                    return;

                Talk(eGeneralTalks::TalkKill);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo == nullptr || p_Target == nullptr)
                    return;

                switch (p_SpellInfo->Id)
                {
                    case eSpells::RadiantSmiteDmg:
                    {
                        p_Target->CastSpell(p_Target, eSpells::EchoOfRadianceDmg, true);

                       break;
                    }
                    case eSpells::UnerringBlastDummyEnergize:
                    {
                        if (!p_Target->IsPlayer())
                            break;

                        DoCast(p_Target, eSpells::UnerringBlastTriggerMissile, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHitDest(SpellDestination const* p_Dest, SpellInfo const* p_SpellInfo, SpellEffectHandleMode /*p_Mode*/) override
            {
                if (p_SpellInfo->Id == eSpells::SpearOfLightTriggerMissile)
                {
                    if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::GlowingFragmentAreatrigger02, me->GetMap()->GetDifficultyID()))
                    {
                        float l_Distance = 4.0f;
                        float l_Angle02 = 0.0f;
                        float l_Step02 = 2 * M_PI / 5;

                        for (uint8 l_I = 0; l_I < 5; l_I++)
                        {
                            Position l_Src = p_Dest->_position;

                            l_Src.SimplePosXYRelocationByAngle(l_Src, l_Distance, l_Angle02);

                            Position l_Dest2 = Position();

                            l_Src.SimplePosXYRelocationByAngle(l_Dest2, 150.0f, l_Angle02);

                            AreaTrigger* l_AreaTrigger = new AreaTrigger;

                            if (!l_AreaTrigger->CreateAreaTrigger(sObjectMgr->GenerateLowGuid(HIGHGUID_AREATRIGGER), 8810, me, nullptr, l_SpellInfo, l_Src, l_Dest2))
                            {
                                delete l_AreaTrigger;
                                return;
                            }

                            l_AreaTrigger->SetOrientation(l_Angle02);

                            l_AreaTrigger->MoveAreaTrigger(l_Dest2, 25 * TimeConstants::IN_MILLISECONDS, false, false);

                            l_Angle02 += l_Step02;
                        }
                    }
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo == nullptr)
                    return;

                switch (p_SpellInfo->Id)
                {
                    case eSpells::ArcingStormDmg:
                    {

                        break;
                    }
                    case eSpells::RadiantSmiteDummy:
                    {
                        if (const SpellInfo* l_SpellInf = sSpellMgr->GetSpellInfo(eSpells::RadiantSmiteDmg, me->GetMap()->GetDifficultyID()))
                        {
                            m_RegenerationSetting = 0;

                            me->SetPower(me->getPowerType(), 0, true);

                            me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_REGENERATE_POWER);

                            /// To each player deal 760k. crazy blizz are crazy maaaannnnn
                            std::list<Player*> l_ListPlayersBetweenBoss;
                            me->GetPlayerListInGrid(l_ListPlayersBetweenBoss, 300.0f);

                            for (Player* l_Itr : l_ListPlayersBetweenBoss)
                            {
                                if (l_Itr == nullptr || l_Itr->isDead())
                                    continue;

                                std::list<Player*> l_ListPlayersBetweenPlayers;
                                l_Itr->GetPlayerListInGrid(l_ListPlayersBetweenPlayers, 1.3f, true);

                                uint32 l_AuraGroup = 0;

                                /// Choose aura group.
                                for (uint32 l_Iter : g_RunicBrandSpells)
                                {
                                    if (!l_AuraGroup) ///< Only once, if procced more then that, should get checked because a player technically supposed to have one aura at a time.
                                    {
                                        if (l_Itr->HasAura(l_Iter))
                                        {
                                            l_AuraGroup = l_Iter;
                                            break;
                                        }
                                    }
                                }

                                l_ListPlayersBetweenPlayers.remove_if([this, l_AuraGroup](Player* p_Player) -> bool
                                {
                                    if (p_Player == nullptr || p_Player->isDead()) ///< Arcane Beacon
                                        return true;

                                    if (!p_Player->HasAura(l_AuraGroup))
                                        return true;

                                    return false;
                                });

                                uint32 l_Size = l_ListPlayersBetweenPlayers.size();

                                int32 l_Bps = l_SpellInf->Effects[EFFECT_0].BasePoints;
                                float l_Stacks = l_Size * 0.15f;
                                int32 l_DamageToRemove = l_Bps * l_Stacks;
                                int32 l_FinalDamage = l_Bps - l_DamageToRemove;

                                me->CastCustomSpell(l_Itr, l_SpellInf->Id, &l_FinalDamage, NULL, NULL, true, 0);
                            }
                        }

                        break;
                    }
                    case eSpells::UnerringBlastDummyEnergize:
                    case eSpells::UnerringBlastDamageEnergize:
                    {
                        /// Extra visuals
                        for (G3D::Vector3 l_Itr : g_UnerringBlastCoords)
                            me->PlayOrphanSpellVisual(me->GetPosition().AsVector3(), G3D::Vector3(), l_Itr, eOrphanVisual::OrphanVisualShottingUnerringBlastProjectile, 45.0f, 0, false, 0);

                        AddTimedDelayedOperation(4 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            ClearDrawPowerAuras();
                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                BossAI::JustSummoned(p_Summon);

                if (p_Summon == nullptr)
                    return;

                switch (p_Summon->GetEntry())
                {
                    case eTovCreatures::CreatureValajarRuneBearer:
                    {
                        me->RemoveAllAuras();

                        me->ClearUnitState(UnitState::UNIT_STATE_CANNOT_AUTOATTACK);

                        me->SetReactState(ReactStates::REACT_PASSIVE);

                        me->CastSpell(me, eSpells::RunebearerSpawnVisu, false);

                        break;
                    }
                    case eTovCreatures::CreatureWorldTrigger:
                    {
                        p_Summon->SetDisplayId(11686); ///< Invis
                        p_Summon->SetReactState(ReactStates::REACT_PASSIVE);
                        p_Summon->AddUnitState(UnitState::UNIT_STATE_CANNOT_AUTOATTACK);
                        p_Summon->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_SERVER_CONTROLLED | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                        break;
                    }
                    default:
                        break;
                }
            }

            void RegeneratePower(Powers p_Power, int32& p_Value) override
            {
                p_Value = 0;
            }

            void DamageTaken(Unit* p_Attacker, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (m_EncounterFinished)
                {
                    p_Damage = 0;
                    return;
                }

                if (m_PhaseId == ePhases::Phase02 && me->GetHealthPct() <= 55.0f)
                {
                    events.Reset();

                    DoAction(eActions::ActionStartPhase03);

                    m_PhaseId = ePhases::Phase03;
                }
                else if ((m_PhaseId == ePhases::Phase03 && me->GetHealthPct() <= 10.0f) || p_Damage > me->GetHealth())
                {
                    p_Damage = 0;

                    me->LoadEquipment(1, true);

                    /// Fight Ends
                    DoAction(eActions::ActionWinEncounter);
                }
            }

            void DamageDealt(Unit* p_Victim, uint32& p_Damage, DamageEffectType p_DamageType, SpellInfo const* p_SpellInfo) override
            {
                if (p_DamageType == DamageEffectType::DIRECT_DAMAGE)
                {
                    if (m_PhaseId == ePhases::Phase02)
                    {
                        if (!m_OdynsTestGuid)
                            m_OdynsTestGuid = p_Victim->GetGUID();

                        if (p_Victim->GetGUID() != m_OdynsTestGuid)
                        {
                            /// Clear all odyns test for previous target
                            if (Unit* l_Unit = Unit::GetUnit(*me, m_OdynsTestGuid))
                            {
                                me->RemoveAura(eSpells::SpellOdynsTest01);

                                m_OdynsTestGuid = p_Victim->GetGUID();
                            }
                        }
                        else
                        {
                            if (m_OdynsTestGuid)
                            {
                                if (Unit* l_Unit = Unit::GetUnit(*me, m_OdynsTestGuid))
                                    me->CastSpell(me, eSpells::SpellOdynsTest01, true);
                            }
                        }
                    }
                }
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex /*p_EffIndex*/) override
            {
                if (p_Spell->GetSpellInfo()->Id == eSpells::StormOfJusticePeriodicTriggerSpell && !p_Targets.empty())
                {
                    p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                    {
                        if (p_Object == nullptr || !p_Object->IsPlayer())
                            return true;

                        if (p_Object->ToPlayer()->GetRoleForGroup() == Roles::ROLE_TANK)
                            return true;

                        return false;
                    });
                }
            }

            void SpawnSpearsOfLight()
            {
                std::list<uint64> l_ListPlayersTemp;

                float l_Angle = 0.0f;

                uint8 l_Count = 0;

                std::list<Player*> l_ListPlayers;
                me->GetPlayerListInGrid(l_ListPlayers, 150.0f, false);

                if (l_ListPlayers.empty())
                    return;

                JadeCore::RandomResizeList(l_ListPlayers, l_ListPlayers.size());

                for (Player* l_Itr : l_ListPlayers)
                {
                    if (l_Count > 2)
                        continue;

                    l_Count++;

                    me->CastSpell(l_Itr->GetPosition(), eSpells::SpearOfLightTriggerMissile, false);
                }
            }

            void ValarjarBond(uint32 const p_Diff)
            {
                if (m_ValarjarsBondDiff <= p_Diff)
                {
                    if (m_Instance != nullptr)
                    {
                        if (Creature* l_Hyrja = Creature::GetCreature(*me, m_Instance->GetData64(eTovCreatures::CreatureHyrja)))
                        {
                            if (Creature* l_Hymdall = Creature::GetCreature(*me, m_Instance->GetData64(eTovCreatures::CreatureHymdall)))
                            {
                                if (m_PhaseId == 2 && IsHeroicOrMythic())
                                {
                                    if (l_Hyrja->getFaction() == 16 && !l_Hyrja->HasFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE))
                                    {
                                        if (l_Hyrja->GetDistance2d(me->m_positionX, me->m_positionY) >= 35.0f)
                                        {
                                            l_Hyrja->RemoveAura(eSpells::ValajarBondDmgTaken);
                                            me->RemoveAura(eSpells::ValajarBondDmgTaken);
                                        }
                                        else
                                        {
                                            if(!me->HasAura(eSpells::ValajarBondDmgTaken))
                                            me->AddAura(eSpells::ValajarBondDmgTaken, me);

                                            if (!l_Hyrja->HasAura(eSpells::ValajarBondDmgTaken))
                                            me->AddAura(eSpells::ValajarBondDmgTaken, l_Hyrja);
                                        }
                                    }

                                    if (l_Hymdall->getFaction() == 16 && !l_Hymdall->HasFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE))
                                    {
                                        if (l_Hymdall->GetDistance2d(me->m_positionX, me->m_positionY) >= 35.0f)
                                        {
                                            l_Hymdall->RemoveAura(eSpells::ValajarBondDmgTaken);
                                            me->RemoveAura(eSpells::ValajarBondDmgTaken);
                                        }
                                        else
                                        {
                                            if (!me->HasAura(eSpells::ValajarBondDmgTaken))
                                            me->AddAura(eSpells::ValajarBondDmgTaken, me);

                                            if (!l_Hymdall->HasAura(eSpells::ValajarBondDmgTaken))
                                            me->AddAura(eSpells::ValajarBondDmgTaken, l_Hymdall);
                                        }
                                    }
                                }

                                else if (m_PhaseId == 1)
                                {
                                    if (l_Hymdall->GetDistance2d(l_Hyrja->m_positionX, l_Hyrja->m_positionY) >= 35.0f)
                                    {
                                        l_Hyrja->RemoveAura(eSpells::ValajarBondDmgTaken);
                                        l_Hymdall->RemoveAura(eSpells::ValajarBondDmgTaken);
                                    }
                                    else
                                    {
                                        if (!l_Hyrja->HasAura(eSpells::ValajarBondDmgTaken))
                                        l_Hyrja->AddAura(eSpells::ValajarBondDmgTaken, l_Hyrja);

                                        if (!l_Hymdall->HasAura(eSpells::ValajarBondDmgTaken))
                                        l_Hymdall->AddAura(eSpells::ValajarBondDmgTaken, l_Hymdall);
                                    }
                                }
                            }
                        }
                    }

                    m_ValarjarsBondDiff = 500;
                }
                else
                    m_ValarjarsBondDiff -= p_Diff;
            }

            void CheckBrandedPlayersToGetProtectionBuff(uint32 const p_Diff) ///< For 2nd phase on mm and hc
            {
                if (m_Instance == nullptr)
                    return;

                if (m_RegenerationSetting != 0 && IsMythic())
                {
                    if (m_CheckBrandedTargetsNearRunesGobsDiff <= p_Diff)
                    {
                        std::list<Player*> l_ListPlayers;
                        me->GetPlayerListInGrid(l_ListPlayers, 150.f, false);

                        for (Player* l_Itr : l_ListPlayers)
                        {
                            if (!l_Itr || l_Itr->HasAura(eSpells::Protected))
                                continue;

                            uint8 l_Count = 0;

                            bool m_Ok = false;

                             do
                             {
                                 if (GameObject* l_Object = GameObject::GetGameObject(*me, m_Instance->GetData64(g_Runes[l_Count])))
                                 {
                                     if (m_PhaseId == 3)
                                     {
                                         if (l_Itr->HasAura(g_RunicBrandSpells[l_Count]) && l_Itr->GetDistance2d(l_Object) <= 4.0f)
                                         {
                                             m_Ok = true;
                                             break;
                                         }
                                     }
                                     else
                                     {
                                         if (l_Itr->HasAura(g_MythicBranded[l_Count]) && l_Itr->GetDistance2d(l_Object) <= 4.0f)
                                         {
                                             m_Ok = true;
                                             break;
                                         }
                                     }
                                 }

                                 l_Count++;

                                 if (m_Ok)
                                     l_Count = 5;

                             } while (l_Count < 5);

                             if (m_Ok)
                                 l_Itr->CastSpell(l_Itr, eSpells::Protected, true);
                        }

                        m_CheckBrandedTargetsNearRunesGobsDiff = 1 * TimeConstants::IN_MILLISECONDS;
                    }
                    else
                        m_CheckBrandedTargetsNearRunesGobsDiff -= p_Diff;
                }
            }

            void RpEventsAI(uint32 const p_Diff)
            {
                m_RpEvents.Update(p_Diff);

                if (m_Instance != nullptr)
                {
                    if (!m_EncounterIntroFinished)
                    {
                        if (!m_EncounterIntroStarted)
                        {
                            if (m_CheckForNearbyPlayersToStartIntroDiff <= p_Diff)
                            {
                                if (me->FindNearestPlayer(90.0f))
                                {
                                    m_EncounterIntroStarted = true;

                                    DoAction(eActions::ActionOdynIntro);
                                }

                                m_CheckForNearbyPlayersToStartIntroDiff = 1 * TimeConstants::IN_MILLISECONDS;
                            }
                            else
                                m_CheckForNearbyPlayersToStartIntroDiff -= p_Diff;
                        }

                        if (m_EncounterIntroStarted)
                        {
                            if (Creature* l_Hymdall = Creature::GetCreature(*me, m_Instance->GetData64(eTovCreatures::CreatureHymdall)))
                            {
                                if (Creature* l_Hyrja = Creature::GetCreature(*me, m_Instance->GetData64(eTovCreatures::CreatureHyrja)))
                                {
                                    if (CreatureAI* l_HymdallAI = l_Hymdall->AI())
                                    {
                                        if (CreatureAI* l_HyrjaAI = l_Hyrja->AI())
                                        {
                                            switch (m_RpEvents.ExecuteEvent())
                                            {
                                            case eEvents::EventIntroOdynTalk01:
                                            {
                                                Talk(eGeneralTalks::TalkOdyn01);

                                                m_RpEvents.ScheduleEvent(eEvents::EventIntroOdynTalk02, eTimers::TalkTimer02);

                                                break;
                                            }
                                            case eEvents::EventIntroOdynTalk02:
                                            {
                                                Talk(eGeneralTalks::TalkOdyn02);

                                                m_RpEvents.ScheduleEvent(eEvents::EventIntroHymdallTalk01, eTimers::TalkTimer03);

                                                break;
                                            }
                                            case eEvents::EventIntroHymdallTalk01:
                                            {
                                                l_HymdallAI->Talk(eGeneralTalks::TalkHymdallHyrja01);

                                                l_Hymdall->CastSpell(l_Hymdall, eTovSpells::SpellReady2h, true);

                                                m_RpEvents.ScheduleEvent(eEvents::EventIntroOdynTalk03, eTimers::TalkTimer04);

                                                break;
                                            }
                                            case eEvents::EventIntroOdynTalk03:
                                            {
                                                Talk(eGeneralTalks::TalkOdyn03);

                                                m_RpEvents.ScheduleEvent(eEvents::EventIntroHyjraTalk01, eTimers::TalkTimer05);

                                                break;
                                            }
                                            case eEvents::EventIntroHyjraTalk01:
                                            {
                                                l_HyrjaAI->Talk(eGeneralTalks::TalkHymdallHyrja01);

                                                /// Combat stance
                                                l_Hyrja->CastSpell(l_Hyrja, eTovSpells::SpellReady1h, true);

                                                m_RpEvents.ScheduleEvent(eEvents::EventIntroFightBegins, eTimers::TalkTimer06);

                                                break;
                                            }
                                            case eEvents::EventIntroFightBegins:
                                            {
                                                Talk(eGeneralTalks::TalkOdyn04);

                                                l_Hymdall->setFaction(16);
                                                l_Hyrja->setFaction(16);

                                                l_Hymdall->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                                                m_EncounterIntroFinished = true;

                                                break;
                                            }
                                            default:
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            void CleansingFlamesSpawner(uint32 const p_Diff)
            {
                if (m_PhaseId == ePhases::Phase03)
                {
                    if (m_CleansingFlames)
                    {
                        if (m_CleansingFlamesSpawnerDiff <= p_Diff)
                        {
                            /// Remove dot.
                            if (m_Instance != nullptr)
                                m_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::CleansingFlamesPeriodicTriggerSpell);

                            uint8 l_MaxLoop = 200;
                            uint8 l_Rand    = urand(0, 2);

                            /// Don't use the same area twice in a row
                            if (m_LastCleansingFlameIDx != -1 && l_Rand == m_LastCleansingFlameIDx)
                            {
                                do
                                {
                                    --l_MaxLoop;

                                    l_Rand = urand(0, 2);
                                }
                                while (l_Rand == m_LastCleansingFlameIDx && l_MaxLoop > 0);
                            }

                            m_LastCleansingFlameIDx = l_Rand;

                            AddTimedDelayedOperation(100, [this]() -> void
                            {
                                DoCast(g_CleansingFlame, g_ArraySpells[m_LastCleansingFlameIDx], true);
                            });

                            m_CleansingFlamesSpawnerDiff = 35 * TimeConstants::IN_MILLISECONDS;
                        }
                        else
                            m_CleansingFlamesSpawnerDiff -= p_Diff;
                    }
                }
            }

            void CheckForRevivifyStatus(uint32 const p_Diff)
            {
                if (m_RevivifyDiff <= p_Diff)
                {
                    if (m_Instance != nullptr)
                    {
                        if (Creature* l_Hyrja = Creature::GetCreature(*me, m_Instance->GetData64(eTovCreatures::CreatureHyrja)))
                        {
                            if (Creature* l_Hymdall = Creature::GetCreature(*me, m_Instance->GetData64(eTovCreatures::CreatureHymdall)))
                            {
                                Spell const* l_HyrjaSpell = l_Hyrja->GetCurrentSpell(CurrentSpellTypes::CURRENT_GENERIC_SPELL);
                                Spell const* l_HymdallSpell = l_Hymdall->GetCurrentSpell(CurrentSpellTypes::CURRENT_GENERIC_SPELL);

                                if (l_HyrjaSpell && l_HymdallSpell)
                                {
                                    if (l_HyrjaSpell->GetSpellInfo()->Id == eSpells::Revivfy && l_HymdallSpell->GetSpellInfo()->Id == eSpells::Revivfy)
                                    {
                                        l_Hymdall->InterruptNonMeleeSpells(true);
                                        l_Hyrja->InterruptNonMeleeSpells(true);

                                        DoAction(eActions::ActionStartPhase02);
                                    }
                                }
                            }
                        }
                    }

                    m_RevivifyDiff = 2 * TimeConstants::IN_MILLISECONDS;
                }
                else
                    m_RevivifyDiff -= p_Diff;
            }

            void CheckForCombat(uint32 const p_Diff)
            {
                if (m_PhaseId == 2 || m_PhaseId == 3 || m_EncounterFinished)
                    return;

                if (!m_EncounterStarted)
                {
                    if (m_CheckForCombatDiff <= p_Diff)
                    {
                        if (Creature* l_Hyrja = Creature::GetCreature(*me, m_Instance->GetData64(eTovCreatures::CreatureHyrja)))
                        {
                            if (Creature* l_Hymdall = Creature::GetCreature(*me, m_Instance->GetData64(eTovCreatures::CreatureHymdall)))
                            {
                                if (Player* l_Nearest = me->FindNearestPlayer(150.f))
                                {
                                    if ((l_Hyrja->isInCombat() || l_Hymdall->isInCombat()))
                                    {
                                        if (!l_Hyrja->isInCombat())
                                        {
                                            if (UnitAI* l_HyrjaAI = l_Hyrja->AI())
                                                l_HyrjaAI->AttackStart(l_Nearest, true);
                                        }

                                        if (!l_Hymdall->isInCombat())
                                        {
                                            if (UnitAI* l_HymdallAI = l_Hymdall->AI())
                                                l_HymdallAI->AttackStart(l_Nearest, true);
                                        }

                                        m_EncounterStarted = true;

                                        me->setFaction(16);

                                        /// Should fix evade problem
                                        std::list<Player*> l_listPlayers;
                                        me->GetPlayerListInGrid(l_listPlayers, 130.f);

                                        if (!l_listPlayers.empty())
                                        {
                                            for (Player* l_Itr : l_listPlayers)
                                            {
                                                if (!l_Itr)
                                                    continue;

                                                if (!me->GetThreatTarget(l_Itr->GetGUID()))
                                                    me->AddThreatTarget(l_Itr->GetGUID());
                                            }
                                        }

                                        if (m_Instance != nullptr)
                                        {
                                            m_Instance->SetData64(eTovData::DataSaveOdynIntroduction, 0);

                                            if (GameObject* l_Door = GameObject::GetGameObject(*me, m_Instance->GetData64(eTovGameObjects::GameObjectOdynGate)))
                                            {
                                                l_Door->SetLootState(LootState::GO_READY);
                                                l_Door->UseDoorOrButton(1 * TimeConstants::IN_MILLISECONDS, false, me);
                                            }
                                        }

                                        if (Player* l_Nearest = me->FindNearestPlayer(130.0f))
                                            AttackStartNoMove(l_Nearest);

                                        events.ScheduleEvent(eEvents::EventReinforceRunes, eTimers::TimerReinforcementRunes);
                                    }
                                }
                            }
                        }

                        m_CheckForCombatDiff = 1 * TimeConstants::IN_MILLISECONDS;
                    }
                    else
                        m_CheckForCombatDiff -= p_Diff;
                }
            }

            void PowerRegenTimer(uint32 const p_Diff)
            {
                if (m_RegenerationSetting != 0)
                {
                    if (m_PowerTimer <= p_Diff)
                    {
                        m_PowerTimer = !m_HalfTick ? 1000 : 500;
                        m_PowerCount = me->GetPower(POWER_ENERGY);

                        int32 l_PowerToAdd = !m_HalfTick ? 3 : 1;

                        if (m_RegenerationSetting == 2)
                            l_PowerToAdd = 20;

                        if (m_PowerCount < 100)
                        {
                            if(m_RegenerationSetting == 1)
                             me->SetPower(POWER_ENERGY, m_PowerCount + l_PowerToAdd);
                        }

                        if (!m_HalfTick)
                            m_HalfTick = true;
                        else
                            m_HalfTick = false;
                    }
                    else
                        m_PowerTimer -= p_Diff;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (m_EncounterFinished)
                    return;

                if (!UpdateVictim())
                    CheckForCombat(p_Diff);

                RpEventsAI(p_Diff);

                ValarjarBond(p_Diff);

                CheckForRevivifyStatus(p_Diff);

                PowerRegenTimer(p_Diff);

                CheckBrandedPlayersToGetProtectionBuff(p_Diff);

                if (!UpdateVictim())
                    return;

                if (me->GetDistance(me->GetHomePosition()) >= 183.0f)
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);

                CleansingFlamesSpawner(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventTestOfTheAgest:
                    {
                        if (m_Instance != nullptr)
                        {
                            if (m_TestOfTheAges[0])
                            {
                                m_TestOfTheAges[0] = false;

                                if (Creature* l_Hyrja = Creature::GetCreature(*me, m_Instance->GetData64(eTovCreatures::CreatureHyrja)))
                                {
                                    l_Hyrja->GetMotionMaster()->Clear(false);
                                    l_Hyrja->GetMotionMaster()->MoveSmoothFlyPath(2, g_HyrjaEnterPlatform.data(), g_HyrjaEnterPlatform.size());
                                }

                                AddTimedDelayedOperation(6000, [this]() -> void
                                {
                                    if (me && me->isInCombat())
                                    {
                                        if (m_Instance != nullptr)
                                        {
                                            if (Creature* l_Hyrja = Creature::GetCreature(*me, m_Instance->GetData64(eTovCreatures::CreatureHyrja)))
                                            {
                                                if(l_Hyrja->GetMaxHealth())
                                                  l_Hyrja->SetHealth(l_Hyrja->GetMaxHealth());

                                                if (UnitAI* l_AI = l_Hyrja->GetAI())
                                                    l_AI->DoAction(1);
                                            }
                                        }
                                    }
                                });
                            }
                            else if (m_TestOfTheAges[1])
                            {
                                m_TestOfTheAges[1] = false;

                                if (Creature* l_Hymdall = Creature::GetCreature(*me, m_Instance->GetData64(eTovCreatures::CreatureHymdall)))
                                {
                                    l_Hymdall->GetMotionMaster()->Clear(false);
                                    l_Hymdall->GetMotionMaster()->MoveJump(2400.6133f, 514.8425f, 748.9949f, 15.0f, 8.0f, 10.0f, 0);
                                }

                                AddTimedDelayedOperation(3000, [this]() -> void
                                {
                                    if (m_Instance != nullptr)
                                    {
                                        if (Creature* l_Hymdall = Creature::GetCreature(*me, m_Instance->GetData64(eTovCreatures::CreatureHymdall)))
                                        {
                                            if (l_Hymdall->GetMaxHealth())
                                               l_Hymdall->SetHealth(l_Hymdall->GetMaxHealth());

                                            if (UnitAI* l_AI = l_Hymdall->GetAI())
                                                l_AI->DoAction(1);
                                        }
                                    }
                                });
                            }
                        }
                        break;
                    }
                    case eEvents::EventReinforceRunes:
                    {
                        Talk(eGeneralTalks::TalkReinforceRunesNormal);

                        m_PowerCount = 0;
                        m_PowerTimer = 1 * TimeConstants::IN_MILLISECONDS;

                        m_HalfTick = false;

                        std::list<Creature*> l_ListValarjarRuneBearers;
                        me->GetCreatureListWithEntryInGrid(l_ListValarjarRuneBearers, eTovCreatures::CreatureValajarRuneBearer, 150.0f);

                        if (!l_ListValarjarRuneBearers.empty())
                        {
                            for (Creature* l_Itr : l_ListValarjarRuneBearers)
                            {
                                if (UnitAI* l_AI = l_Itr->GetAI())
                                    l_AI->DoAction(eActions::ActionClearAllRunesAura);
                            }
                        }

                        if (IsMythic() && (m_PhaseId == 3))
                        {
                            /// Mythic 3rd phase abillity
                            std::list<Player*> l_ListPlayersNerby;
                            me->GetPlayerListInGrid(l_ListPlayersNerby, 200.0f, false);

                            uint32 l_IndexCount = 0;

                            uint32 l_ColourCount = 0;
                            uint32 l_ColourCountMax = 1;

                            switch (l_ListPlayersNerby.size())
                            {
                                case 10:
                                    l_ColourCount = 2;
                                    break;
                                case 15:
                                    l_ColourCount = 3;
                                    break;
                                case 20:
                                    l_ColourCount = 4;
                                    break;
                                case 25:
                                    l_ColourCount = 5;
                                    break;
                                case 30:
                                    l_ColourCount = 6;
                                    break;
                                case 35:
                                    l_ColourCount = 7;
                                    break;
                                case 40:
                                    l_ColourCount = 8;
                                    break;
                                default:
                                    break;
                            }

                            if (!l_ListPlayersNerby.empty())
                            {
                                for (Player* l_Player : l_ListPlayersNerby)
                                {
                                    if (l_Player == nullptr)
                                        continue;

                                    if (l_IndexCount > 4)
                                        l_IndexCount = 0;

                                    me->AddAura(g_RunicBrandSpells[l_IndexCount], l_Player);

                                    if (l_ColourCount >= l_ColourCountMax)
                                    {
                                        l_ColourCount = 0;
                                        l_IndexCount++;
                                    }

                                    l_ColourCount++;
                                }
                            }

                            m_RegenerationSetting = 2; ///< Radiant Smite, x3 more regen then regen 1

                            me->SetPower(me->getPowerType(), int32(0), false);

                            me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_REGENERATE_POWER);

                            events.DelayEvent(eEvents::EventStormOfJustice, 10 * TimeConstants::IN_MILLISECONDS);

                            events.ScheduleEvent(eEvents::EventRadiantSmite, 9 * TimeConstants::IN_MILLISECONDS);
                            events.ScheduleEvent(eEvents::EventReinforceRunes, 30 * TimeConstants::IN_MILLISECONDS);
                        }
                        if (m_PhaseId == 2 || m_PhaseId == 1)
                        {
                            if (me->IsAIEnabled)
                                DoAction(eActions::ActionOdynRunes);

                            events.ScheduleEvent(eEvents::EventUnerringBlast, /*eTimers::TimerUnerringBlast*/ 40 * TimeConstants::IN_MILLISECONDS);
                            events.ScheduleEvent(eEvents::EventReinforceRunes, eTimers::TimerReinforcementRunes + 30 * TimeConstants::IN_MILLISECONDS);
                        }

                        break;
                    }
                    case eEvents::EventArchingStormBuff:
                    {
                        me->CastSpell(me, eSpells::ArcingStormModDmgDoneForMechanic, true);

                        events.ScheduleEvent(eEvents::EventArchingStormBuff, 10 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventRadiantSmite: ///< Pretty much runic brand + radiant smite + echo of radiance till the end of where it trace
                    {
                        m_PowerCount = 0;

                        m_PowerTimer = 1 * TimeConstants::IN_MILLISECONDS;

                        m_RegenerationSetting = 0;

                        me->SetPower(me->getPowerType(), 0, false);

                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_REGENERATE_POWER);

                        /// Radiant smite.
                        me->CastSpell(me, eSpells::RadiantSmiteDummy, false);

                        AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            if (InstanceScript* l_InstanceScript = me->GetInstanceScript())
                            {
                                for (uint64 l_Itr : g_RunicBrandSpells)
                                {
                                    m_Instance->DoRemoveAurasDueToSpellOnPlayers(l_Itr);
                                }
                            }
                        });
                        break;
                    }
                    case eEvents::EventUnerringBlast:
                    {
                        Talk(eGeneralTalks::TalkUnerringBlast);
                        Talk(eGeneralTalks::TalkReinforeceRuneWarn);

                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_REGENERATE_POWER);

                        me->SetPower(me->getPowerType(), int32(0), false);

                        m_PowerCount = 0;

                        m_PowerTimer = 1 * TimeConstants::IN_MILLISECONDS;

                        m_RegenerationSetting = 0;

                        if (IsMythic())
                            DoCast(me, eSpells::UnerringBlastDamageEnergize);
                        else
                            DoCast(me, eSpells::UnerringBlastDummyEnergize);

                        break;
                    }
                    case eEvents::EventStormforgedSpear:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO, 0, 100.0f, true))
                        {
                            Talk(eGeneralTalks::TalkStormforgedSpear);

                            uint64 l_TargetGuid = l_Target->GetGUID();

                            me->CastSpell(l_Target, eSpells::StormforgedSpearPeriodicDummy);

                            AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS + 800, [this, l_TargetGuid]() -> void
                            {
                                if (!l_TargetGuid)
                                    return;

                                if (Unit* l_Target = sObjectAccessor->GetUnit(*me, l_TargetGuid))
                                    me->CastSpell(l_Target, eSpells::StormforgedSpearTriggerMissile, true);
                            });
                        }

                        events.ScheduleEvent(eEvents::EventStormforgedSpear, (m_StormforgedSpearCount % 3 == 0) ? ((13 * TimeConstants::IN_MILLISECONDS) + 500) : 11 * TimeConstants::IN_MILLISECONDS);
                        ++m_StormforgedSpearCount;
                        break;
                    }
                    case eEvents::EventSpearOfLight:
                    {
                        SpawnSpearsOfLight();

                        events.ScheduleEvent(eEvents::EventSpearOfLight, eTimers::TimerSpearOfLight + 2000);
                        break;
                    }
                    case eEvents::EventStormOfJustice:
                    {
                        Talk(eGeneralTalks::TalkStormOfJustice);

                        DoCast(me, eSpells::StormOfJusticePeriodicTriggerSpell, true);

                        events.ScheduleEvent(eEvents::EventStormOfJustice, (m_StormOfJusticeCount % 3) == 0 ? ((13 * TimeConstants::IN_MILLISECONDS) + 500) : 11 * TimeConstants::IN_MILLISECONDS);
                        ++m_StormOfJusticeCount;
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

            void ClearDrawPowerAuras()
            {
                AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    if (m_Instance == nullptr)
                        return;

                    std::list<Creature*> l_ListValarjarRuneBearer;
                    me->GetCreatureListWithEntryInGrid(l_ListValarjarRuneBearer, eTovCreatures::CreatureValajarRuneBearer, 150.0f);

                    for (Creature* l_Itr : l_ListValarjarRuneBearer)
                    {
                        if (UnitAI* l_Ai = l_Itr->GetAI())
                            l_Ai->DoAction(eActions::ActionClearAllRunesAura);
                    }

                    for (uint32 l_Iter : g_Runes)
                    {
                        if (GameObject* l_Itr = m_Instance->instance->GetGameObject(m_Instance->GetData64(l_Iter)))
                        {
                            l_Itr->ResetLootMode();
                            l_Itr->ResetDoorOrButton();
                        }
                    }

                    for (uint32 l_AtVisualItr : g_AtsVisual)
                    {
                        std::list<AreaTrigger*> l_ListAts;
                        me->GetAreaTriggerListWithSpellIDInRange(l_ListAts, l_AtVisualItr, 150.0f);

                        for (AreaTrigger* l_Itr : l_ListAts)
                        {
                            if (l_Itr == nullptr)
                                continue;

                            l_Itr->Remove(0);
                        }
                    }

                    m_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::Protected);

                    for (uint8 l_I = 0; l_I < 5; l_I++)
                    {
                        me->RemoveAura(g_SpellEntries[l_I]);

                        m_Instance->DoRemoveAurasDueToSpellOnPlayers(g_FixationSpells[l_I]);
                        m_Instance->DoRemoveAurasDueToSpellOnPlayers(g_RunicBrandSpells[l_I]);
                        m_Instance->DoRemoveAurasDueToSpellOnPlayers(g_MythicBranded[l_I]);
                    }

                    /// Achievement
                    bool l_Achievement = true;

                    for (uint32 l_Itr : m_RandomizedRunesList)
                    {
                        for (uint32 l_Iter : m_RandomizedRunesListKillOrder)
                        {
                            if (l_Itr != l_Iter)
                            {
                                l_Achievement = false;
                                break;
                            }
                        }
                    }

                    if (l_Achievement)
                        m_Achievement = true;
                });
            }

            void DefaultNpcCalls()
            {
                _Reset();

                events.Reset();

                m_RpEvents.Reset();

                me->RemoveAllAuras();

                me->RemoveAllAreasTrigger();

                ClearDelayedOperations();

                me->SetName(me->GetName());

                m_PowerCount = 0;

                m_PowerTimer = 1 * TimeConstants::IN_MILLISECONDS;

                me->LoadEquipment(1, true);

                m_HalfTick = false;

                m_EncounterStarted = false;

                m_RegenerationSetting = 0;

                DespawnCreaturesInArea(eTovCreatures::CreatureDancingBlade, me);

                me->SetSheath(SheathState::SHEATH_STATE_UNARMED);

                m_OdynsTestGuid = 0;

                m_CheckForNoCombatDiff = 1 * TimeConstants::IN_MILLISECONDS;
                m_CheckForCombatDiff = 1 * TimeConstants::IN_MILLISECONDS;
                m_RevivifyDiff = 1 * TimeConstants::IN_MILLISECONDS;
                m_CleansingFlamesSpawnerDiff = 35 * TimeConstants::IN_MILLISECONDS;
                m_CheckBrandedTargetsNearRunesGobsDiff = 1 * TimeConstants::IN_MILLISECONDS;
                m_CheckForNearbyPlayersToStartIntroDiff = 1 * TimeConstants::IN_MILLISECONDS;
                m_ValarjarsBondDiff = 1 * TimeConstants::IN_MILLISECONDS;

                m_RandomizedRunesListKillOrder.clear();
                m_RandomizedRunesList.clear();

                me->SetFloatValue(UNIT_FIELD_BOUNDING_RADIUS, 10);
                me->SetFloatValue(UNIT_FIELD_COMBAT_REACH, 10);

                if (IsHeroicOrMythic())
                {
                    m_TestOfTheAges[0] = true;
                    m_TestOfTheAges[1] = false;
                }

                m_Achievement            = false;
                m_CleansingFlames        = false;
                m_EncounterStarted       = false;
                m_EncounterIntroStarted  = false;

                m_LastCleansingFlameIDx = -1;

                m_StormOfJusticeCount = 1;
                m_StormforgedSpearCount = 1;

                summons.DespawnAll();

                DespawnCreaturesInArea(19871, me);
                DespawnCreaturesInArea(eTovCreatures::CreatureRagingTempest, me);
                DespawnCreaturesInArea(eTovCreatures::CreatureValajarRuneBearer, me);
                DespawnCreaturesInArea(eTovCreatures::CreatureDancingBlade, me);
                DespawnCreaturesInArea(eTovCreatures::CreatureSpearOfLight, me);
                DespawnCreaturesInArea(eTovCreatures::CreatureWorldTrigger, me);
                DespawnCreaturesInArea(eTovCreatures::CreatureValajarRuneBearer, me);

                me->SetPower(me->getPowerType(), int32(0), false);

                me->setFaction(35);

                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);

                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_REGENERATE_POWER);

                me->SetFloatValue(EUnitFields::UNIT_FIELD_ATTACK_ROUND_BASE_TIME + WeaponAttackType::BaseAttack, 1500);

                if (!m_EncounterFinished)
                    me->RemoveFlag(EUnitFields::UNIT_FIELD_NPC_FLAGS, NPCFlags::UNIT_NPC_FLAG_GOSSIP | NPCFlags::UNIT_NPC_FLAG_QUESTGIVER);

                if (m_Instance != nullptr)
                {
                    m_PhaseId = 1;

                    for (uint32 l_SpellID : g_SpellEntries)
                        me->RemoveAura(l_SpellID);

                    for (uint32 l_Itr : m_SpellToReset)
                    {
                        m_Instance->DoRemoveAurasDueToSpellOnPlayers(l_Itr);
                    }

                    for (uint32 l_Itr : g_MythicBranded)
                    {
                        m_Instance->DoRemoveAurasDueToSpellOnPlayers(l_Itr);
                    }

                    for (uint32 l_Itr : g_FixationSpells)
                    {
                        m_Instance->DoRemoveAurasDueToSpellOnPlayers(l_Itr);
                    }

                    for (uint32 l_Itr : g_RunicBrandSpells)
                    {
                        m_Instance->DoRemoveAurasDueToSpellOnPlayers(l_Itr);
                    }

                    for (uint32 l_AtVisualItr : g_AtsVisual)
                    {
                        std::list<AreaTrigger*> l_ListAts;
                        me->GetAreaTriggerListWithSpellIDInRange(l_ListAts, l_AtVisualItr, 100.0f);

                        for (AreaTrigger* l_Itr : l_ListAts)
                        {
                            if (l_Itr == nullptr || l_AtVisualItr != l_Itr->GetSpellId())
                                continue;

                            l_Itr->Remove();
                        }
                    }

                    for (uint32 l_Itr : g_ArraySpells)
                    {
                        std::list<AreaTrigger*> l_ListSpellAts;
                        me->GetAreaTriggerList(l_ListSpellAts, l_Itr);

                        for (AreaTrigger* l_AT : l_ListSpellAts)
                        {
                            l_AT->Remove(0);
                        }
                    }

                    if (Creature* l_Hyrja = Creature::GetCreature(*me, m_Instance->GetData64(eTovCreatures::CreatureHyrja)))
                    {
                        if (Creature* l_Hymdall = Creature::GetCreature(*me, m_Instance->GetData64(eTovCreatures::CreatureHymdall)))
                        {
                            if (l_Hyrja->IsAIEnabled && l_Hymdall->IsAIEnabled)
                            {
                                l_Hyrja->AI()->SetData(0, m_PhaseId);
                                l_Hymdall->AI()->SetData(0, m_PhaseId);
                            }
                        }
                    }
                }

                SetCombatMovement(false);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_tov_odynAI(p_Creature);
        }
};

/// Hymdall - 114361
class npc_tov_odyn_hymdall : public CreatureScript
{
    public:
        npc_tov_odyn_hymdall() : CreatureScript("npc_tov_odyn_hymdall") { }

        struct npc_tov_odyn_hymdallAI : public ScriptedAI
        {
            npc_tov_odyn_hymdallAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_CastedRevivify = false;

                m_Instance = p_Creature->GetInstanceScript();

                m_Phase = 1;
            }

            enum eTalks
            {
                TalkAggro,
                TalkTestOfTheAges,
                TalkHornOfValor,
                TalkKill,
                TalkHornOfValorWarning
            };

            enum eMovementInformed
            {
                MovementInformedTestOfAges = 1,
                MovementInformedBalcony
            };

            enum eSpells
            {
                SpellRevivify                   = 228171,

                DancingBladeAuraAt              = 193214, ///< Spining aura blade, probably blade is an invis trig with blade visual spell
                DancingBladeDmgKnockBack        = 193233, ///< Dmg
                DancingBladePeriodicDmg         = 193234, ///< Actually maybe this is the dmg aura? no tooltip but dmg
                DancingBladeTriggerDummy        = 193235, ///< Cast.
                DancingBladeTriggerMissile      = 228003, ///< Trigger missile
                DancingBladeSummonDmgKnockback  = 228004,
                DancingBladeAuraAt02            = 228006, ///< seems the same as 193214
                DancingBladePeriodicDmg02       = 228007, ///< Seems the same as 193233
                DancingBladeDummmy              = 229115,

                HornOfValorDummy                = 228871,
                HornOfValorDmg                  = 191284,
                HornOfValorDmg02                = 228012,
                HornOfValorDmg03                = 228874,

                OdynsTestAttckSpeed             = 227626,
                OdynsTestAurDummy               = 228911,

                ValajarBondTriggerSpell         = 229469,
                ValajarBondDmgTakenDummy        = 229529,
                ValajarBondDummy                = 229530,
                ValajarBondAuraAt               = 228016,
                ValajarBondDmgTaken             = 228018
            };

            enum eEvents
            {
                EventDancingBlade = 1,
                EventHornOfValor
            };

            std::vector<uint32> m_TimersOffsetHornOfValor =
            {
                8 * TimeConstants::IN_MILLISECONDS,
                24 * TimeConstants::IN_MILLISECONDS,
                75 * TimeConstants::IN_MILLISECONDS,
                67 * TimeConstants::IN_MILLISECONDS,
                67 * TimeConstants::IN_MILLISECONDS,
                20 * TimeConstants::IN_MILLISECONDS
            };

            std::vector<uint32> m_TimersOffsetDancingBlade =
            {
                22 * TimeConstants::IN_MILLISECONDS,
                23 * TimeConstants::IN_MILLISECONDS,
                32 * TimeConstants::IN_MILLISECONDS,
                20 * TimeConstants::IN_MILLISECONDS,
                19 * TimeConstants::IN_MILLISECONDS,
                20 * TimeConstants::IN_MILLISECONDS
            };

            InstanceScript* m_Instance;

            bool m_TestOfTheAges;
            bool m_CastedRevivify;

            int32 m_Phase;

            uint8 m_TimersOffset[2] = { 0, 0 };

            void Reset() override
            {
                events.Reset();

                ClearDelayedOperations();

                me->RemoveAllAuras();

                me->RemoveAllAreasTrigger();

                summons.DespawnAll();

                m_TimersOffset[0] = 0;
                m_TimersOffset[1] = 0;

                m_TestOfTheAges = false;
                m_CastedRevivify = false;

                me->SetName(me->GetName());

                me->SetFloatValue(UNIT_FIELD_BOUNDING_RADIUS, 10);
                me->SetFloatValue(UNIT_FIELD_COMBAT_REACH, 10);

                me->RemoveAllAreasTrigger();

                me->RemoveAllAuras();

                me->SetSpeed(UnitMoveType::MOVE_RUN, 1.12f, true);

                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void GetMythicHealthMod(float& p_Mod) override
            {
                p_Mod = 2.80f;
            }

            void EnterEvadeMode() override
            {
                CreatureAI::EnterEvadeMode();
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                if (m_Phase == 1)
                    Talk(eTalks::TalkAggro);
                else
                    Talk(eTalks::TalkTestOfTheAges);

                if (m_Instance != nullptr)
                {
                    m_Instance->DoRemoveAurasDueToSpellOnPlayers(eTovSpells::SpellBelfrostAura);
                    m_Instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                }

                me->CallForHelp(100.0f);

                me->SetSpeed(UnitMoveType::MOVE_RUN, 1.12f, true);

                events.Reset();

                events.ScheduleEvent(eEvents::EventHornOfValor, m_TimersOffsetHornOfValor[m_TimersOffset[0]++]);
                events.ScheduleEvent(eEvents::EventDancingBlade, m_TimersOffsetDancingBlade[m_TimersOffset[1]++]);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::HornOfValorDmg03:
                    {
                        /*
                        if (const SpellInfo* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::HornOfValorDmg03, me->GetMap()->GetDifficultyID())) ///< Draw Power
                        {
                            /// Resontates across the room
                            std::list<Player*> l_ListPlayersNerby;
                            p_Target->GetPlayerListInGrid(l_ListPlayersNerby, 2.0f, false);

                            for (Player* l_Player : l_ListPlayersNerby)
                            {
                                p_Target->CastSpell(l_Player, eSpells::HornOfValorDmg03, true);
                            }
                        }
                        */
                        break;
                    }
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* p_SpellInfo) override
            {
                if (!m_Instance)
                    return;

                if (m_CastedRevivify)
                {
                    p_Damage = 0;
                    return;
                }

                if (m_Phase == 1)
                {
                    if ((p_Damage > me->GetHealth() || me->HealthBelowPctDamaged(25, p_Damage)) && !m_CastedRevivify)
                    {
                        p_Damage = 0;

                        me->InterruptNonMeleeSpells(true);

                        me->RemoveAllAuras();

                        me->RemoveAura(eSpells::ValajarBondDmgTakenDummy);

                        me->SetHealth(me->CountPctFromMaxHealth(25.0f));

                        events.Reset();

                        me->StopMoving();

                        me->CastStop();

                        me->GetMotionMaster()->Clear(false);

                        me->SetReactState(ReactStates::REACT_PASSIVE);

                        me->CastSpell(me, eSpells::SpellRevivify, false);

                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_SERVER_CONTROLLED);

                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);

                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                        m_CastedRevivify = true;
                    }
                }
                else
                {
                    if (p_Damage > me->GetHealth() || me->HealthBelowPctDamaged(85, p_Damage))
                    {
                        p_Damage = 0;

                        me->InterruptNonMeleeSpells(true);

                        me->RemoveAllAuras();

                        events.Reset();

                        me->AttackStop();

                        me->AddUnitState(UnitState::UNIT_STATE_CANNOT_AUTOATTACK);

                        me->SetReactState(ReactStates::REACT_PASSIVE);

                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_SERVER_CONTROLLED);

                        me->GetMotionMaster()->Clear(false);

                        me->GetMotionMaster()->MoveJump(g_HymdallReturnBack[1].m_positionX, g_HymdallReturnBack[1].m_positionY, g_HymdallReturnBack[1].m_positionZ, 30.0f, 10.0f, 0);

                        me->SetFullHealth();

                        if (Creature* l_Odyn = Creature::GetCreature(*me, m_Instance->GetData64(eTovCreatures::CreatureBossOdyn)))
                        {
                            l_Odyn->RemoveAura(eSpells::ValajarBondDmgTaken);

                            if (UnitAI* l_AI = l_Odyn->GetAI())
                            {
                                l_AI->DoAction(10); ///< Schedule Test of the ages

                                if (boss_tov_odyn::boss_tov_odynAI* l_LinkAI = CAST_AI(boss_tov_odyn::boss_tov_odynAI, l_AI))
                                    l_LinkAI->m_TestOfTheAges[0] = true;
                            }
                        }
                    }
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo == nullptr)
                    return;

                switch (p_SpellInfo->Id)
                {
                    case eSpells::HornOfValorDmg02:
                    {
                        if (const SpellInfo* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::HornOfValorDmg03, me->GetMap()->GetDifficultyID())) ///< Draw Power
                        {
                            me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_SERVER_CONTROLLED);

                            /// Resontates across the room
                            std::list<Player*> l_ListPlayersNerby;

                            me->GetPlayerListInGrid(l_ListPlayersNerby, 300.0f, false);

                            for (Player* l_Player : l_ListPlayersNerby)
                            {
                                if (l_Player == nullptr || l_Player->isDead())
                                    continue;

                                int32 l_Dmg = l_SpellInfo->Effects[EFFECT_0].BasePoints;

                                 if(l_Player->GetDistance2d(me) <= 15.0f)
                                    l_Dmg += l_Dmg * 0.20f;

                                 if (IsMythic())
                                 {
                                     l_Dmg = 877500;

                                     if (l_Player->GetDistance2d(me) >= 15.0f)
                                         l_Dmg = 657400;
                                 }

                                me->CastCustomSpell(l_Player, l_SpellInfo->Id, &l_Dmg, NULL, NULL, true, 0);
                            }
                        }

                        break;
                    }
                    case eSpells::SpellRevivify:
                    {
                        m_CastedRevivify = false;

                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                        me->ClearUnitState(UnitState::UNIT_STATE_CANNOT_AUTOATTACK);

                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);

                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO, 0, 100.0f, true))
                        {
                            me->GetMotionMaster()->Clear(false);

                            me->GetMotionMaster()->MoveChase(l_Target);
                        }

                        events.ScheduleEvent(eEvents::EventHornOfValor, m_TimersOffsetHornOfValor[m_TimersOffset[0]++]);
                        events.ScheduleEvent(eEvents::EventDancingBlade, m_TimersOffsetDancingBlade[m_TimersOffset[1]++]);
                        break;
                    }
                    default:
                        break;
                }
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case 1:
                    {
                        m_TestOfTheAges = true;

                        m_CastedRevivify = false;

                        Talk(eTalks::TalkTestOfTheAges);

                        me->ClearUnitState(UnitState::UNIT_STATE_CANNOT_AUTOATTACK);

                        if (me->GetReactState() != ReactStates::REACT_AGGRESSIVE)
                            me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_SERVER_CONTROLLED);

                        SetCombatMovement(true);

                        if (m_Instance != nullptr)
                            m_Instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                        if (Player* l_Plr = me->FindNearestPlayer(100.0f))
                            AttackStart(l_Plr, true);

                        events.Reset();

                        m_TimersOffset[0] = 0;
                        m_TimersOffset[1] = 0;

                        events.ScheduleEvent(eEvents::EventHornOfValor, m_TimersOffsetHornOfValor[m_TimersOffset[0]++]);
                        events.ScheduleEvent(eEvents::EventDancingBlade, m_TimersOffsetDancingBlade[m_TimersOffset[1]++]);

                        break;
                    }
                    case 3:
                    {
                        m_CastedRevivify = false;

                        events.Reset();

                        if (m_TimersOffset[0] >= 5)
                            m_TimersOffset[0] = 0;

                        if (m_TimersOffset[1] >= 5)
                            m_TimersOffset[1] = 1;

                        events.ScheduleEvent(eEvents::EventHornOfValor, m_TimersOffsetHornOfValor[m_TimersOffset[0]++]);
                        events.ScheduleEvent(eEvents::EventDancingBlade, m_TimersOffsetDancingBlade[m_TimersOffset[1]++]);
                        break;
                    }
                    default:
                        break;
                }
            }

            void KilledUnit(Unit* p_Victim) override
            {
                if (p_Victim == nullptr || !p_Victim->IsPlayer())
                    return;

                Talk(eTalks::TalkKill);
            }

            void SetData(uint32 p_ID, uint32 p_Value) override
            {
                m_Phase = p_Value;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                if (m_Instance && me->GetDistance(me->GetHomePosition()) > 150.0f)
                {
                    if (Creature* l_Odyn = Creature::GetCreature(*me, m_Instance->GetData64(eTovCreatures::CreatureBossOdyn)))
                    {
                        if (l_Odyn->IsAIEnabled)
                        {
                            l_Odyn->AI()->EnterEvadeMode();
                            return;
                        }
                    }
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventDancingBlade:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_FARTHEST, 0, 40.0f, true))
                            me->CastSpell(l_Target, eSpells::DancingBladeTriggerMissile);

                        if (m_TimersOffset[0] >= 5)
                            m_TimersOffset[0] = 0;

                        events.ScheduleEvent(eEvents::EventDancingBlade, m_TimersOffsetDancingBlade[m_TimersOffset[0]++]);

                        break;
                    }
                    case eEvents::EventHornOfValor:
                    {
                        events.DelayEvents(6 * TimeConstants::IN_MILLISECONDS);

                        Talk(eTalks::TalkHornOfValor);

                        Talk(eTalks::TalkHornOfValorWarning);

                        me->StopMoving();

                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_SERVER_CONTROLLED);

                        me->CastSpell(me, eSpells::HornOfValorDmg02, false);

                        if (m_TimersOffset[1] >= 5)
                            m_TimersOffset[1] = 1;

                        events.ScheduleEvent(eEvents::EventHornOfValor, m_TimersOffsetHornOfValor[m_TimersOffset[1]++]);

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
            return new npc_tov_odyn_hymdallAI(p_Creature);
        }
};

/// Hyrja - 114360
class npc_tov_odyn_hyrja : public CreatureScript
{
    public:
        npc_tov_odyn_hyrja() : CreatureScript("npc_tov_odyn_hyrja") { }

        struct npc_tov_odyn_hyrjaAI : public ScriptedAI
        {
            npc_tov_odyn_hyrjaAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_CastedRevivify = false;

                m_Instance = p_Creature->GetInstanceScript();

                m_Phase = 1;
            }

            enum eTalks
            {
                TalkShieldOfLight = 0,
                TalkTestOfTheAges,
                TalkAggro,
                TalkRevivfy,
                TalkKill,
                TalkShieldOfLightWarning = 6
            };

            enum eMovementInformed
            {
                MovementInformedTestOfAges = 1,
                MovementGoToBalcony
            };

            enum eSpells
            {
                SpellRevivify                = 228171,

                ShieldOfLightDummy           = 192018,
                ShieldOfLightDummy02         = 192022,
                ShieldOfLightSummon          = 192305,
                ShieldOfLightMultiEff        = 228162,
                ShieldOfLightAuraDummy       = 228270,
                ShieldOfLightDummy03         = 228272,

                ExpelLightEffDummy           = 192044,
                ExpelLightPeriodicDummy      = 192044,
                ExpelLightSchoolDmg          = 192048,
                ExpelLightModConfuse         = 192067,
                ExpelLightAuraDummy          = 192070,
                ExpelLightTriggerMissile     = 192095,
                ExpelLightTriggerMissile02   = 191202,
                ExpelLightTriggerSpell       = 214198,
                ExpelLightEffHeal            = 214200,
                ExpelLightEffDummy02         = 228028,
                ExpelLightPeriodicDummy02    = 228029,
                ExpelLightSchoolDmg02        = 228030,

                OdynsTestAttckSpeed          = 227626,
                OdynsTestAurDummy            = 228911,

                ValajarBondTriggerSpell      = 229469,
                ValajarBondDmgTakenDummy     = 229529,
                ValajarBondDummy             = 229530,
                ValajarBondAuraAt            = 228016,
                ValajarBondDmgTaken          = 228018
            };

            enum eEvents
            {
                EventExpelLight = 1,
                EventShieldOfLight,
                EventTestOfTheAges
            };

            enum eTrigger
            {
                ShieldOfLightTrigger = 19871
            };

            std::vector<uint32> m_TimersOffsetExpelLight =
            {
                25 * TimeConstants::IN_MILLISECONDS,
                48 * TimeConstants::IN_MILLISECONDS,
                40 * TimeConstants::IN_MILLISECONDS,
                40 * TimeConstants::IN_MILLISECONDS
            };

            std::vector<uint32> m_TimersOffsetShieldOfLight =
            {
                20 * TimeConstants::IN_MILLISECONDS,
                36 * TimeConstants::IN_MILLISECONDS,
                39 * TimeConstants::IN_MILLISECONDS,
                40 * TimeConstants::IN_MILLISECONDS
            };

            InstanceScript* m_Instance;

            bool m_First;
            bool m_CastedRevivify;
            bool m_TestOfTheAges;

            int32 m_Phase;

            uint64 m_ShieldOfLightTargetGuid;

            uint8 m_TimersOffset[2] = { 0, 0 };

            void Reset() override
            {
                events.Reset();

                ClearDelayedOperations();

                summons.DespawnAll();

                me->SetCanDualWield(false);

                me->RemoveAllAuras();

                me->RemoveAllAreasTrigger();

                me->SetCanFly(false, true);

                me->SetDisableGravity(false);

                SetCombatMovement(true);

                if (m_Instance != nullptr)
                    m_Instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                m_TimersOffset[0] = 0;
                m_TimersOffset[1] = 0;

                m_ShieldOfLightTargetGuid = 0;

                m_CastedRevivify = false;

                me->SetName(me->GetName());

                me->SetFloatValue(UNIT_FIELD_BOUNDING_RADIUS, 10);

                me->SetFloatValue(UNIT_FIELD_COMBAT_REACH, 10);

                me->SetSpeed(UnitMoveType::MOVE_RUN, 1.12f, true);

                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void GetMythicHealthMod(float& p_Mod) override
            {
                p_Mod = 2.80f;
            }

            void EnterEvadeMode() override
            {
                CreatureAI::EnterEvadeMode();
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                if (m_Phase == 1)
                    Talk(eTalks::TalkAggro);
                else
                    Talk(eTalks::TalkTestOfTheAges);

                if (m_Instance != nullptr)
                {
                    m_Instance->DoRemoveAurasDueToSpellOnPlayers(eTovSpells::SpellBelfrostAura);

                    m_Instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                }

                me->CallForHelp(100.0f);

                me->SetSpeed(UnitMoveType::MOVE_RUN, 1.12f, true);

                events.ScheduleEvent(eEvents::EventExpelLight, m_TimersOffsetExpelLight[m_TimersOffset[0]++]);
                events.ScheduleEvent(eEvents::EventShieldOfLight, m_TimersOffsetShieldOfLight[m_TimersOffset[1]++]);
            }

            void OnAddThreat(Unit* p_Victim, float& p_Threat, SpellSchoolMask /*p_SchoolMask*/, SpellInfo const* /*p_ThreatSpell*/) override
            {
                if (m_ShieldOfLightTargetGuid)
                {
                    if (p_Victim->GetGUID() == m_ShieldOfLightTargetGuid)
                        p_Threat = 100.0f;
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                switch (p_Summon->GetEntry())
                {
                    case eTrigger::ShieldOfLightTrigger:
                    {
                        p_Summon->SetDisplayId(11686); ///< Invis
                        p_Summon->SetReactState(ReactStates::REACT_PASSIVE);
                        p_Summon->AddUnitState(UnitState::UNIT_STATE_CANNOT_AUTOATTACK);
                        p_Summon->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_SERVER_CONTROLLED | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                        break;
                    }
                    default:
                        break;
                }
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case 1:
                    {
                        me->SetAnimTier(1);

                        m_TestOfTheAges = false;

                        m_CastedRevivify = false;

                        me->SetCanFly(false, true);

                        me->SetDisableGravity(false);

                        Talk(eTalks::TalkTestOfTheAges);

                        me->ClearUnitState(UnitState::UNIT_STATE_CANNOT_AUTOATTACK);

                        if (me->GetReactState() != ReactStates::REACT_AGGRESSIVE)
                            me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_SERVER_CONTROLLED);

                        SetCombatMovement(true);

                        if (m_Instance != nullptr)
                            m_Instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                        if (Player* l_Plr = me->FindNearestPlayer(100.0f))
                            AttackStart(l_Plr, true);

                        events.Reset();

                        m_TimersOffset[0] = 0;
                        m_TimersOffset[1] = 0;

                        events.ScheduleEvent(eEvents::EventExpelLight, m_TimersOffsetExpelLight[m_TimersOffset[0]++]);
                        events.ScheduleEvent(eEvents::EventShieldOfLight, m_TimersOffsetShieldOfLight[m_TimersOffset[1]++]);
                        break;
                    }
                    case 2:
                    {
                        /// Clear it after 2 sec delay, to allow visual to work as intended.
                        AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                            me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                            me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_PLAYER_CONTROLLED | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                        });

                        break;
                    }
                    case 3:
                    {
                        m_CastedRevivify = false;

                        if (m_TimersOffset[0] >= 3)
                            m_TimersOffset[0] = 0;

                        if (m_TimersOffset[1] >= 3)
                            m_TimersOffset[1] = 0;

                        events.ScheduleEvent(eEvents::EventExpelLight, m_TimersOffsetExpelLight[m_TimersOffset[0]++]);
                        events.ScheduleEvent(eEvents::EventShieldOfLight, m_TimersOffsetExpelLight[m_TimersOffset[1]++]);
                        break;
                    }
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* p_SpellInfo) override
            {
                if (!m_Instance)
                    return;

                if (m_CastedRevivify)
                {
                    p_Damage = 0;
                    return;
                }

                if (m_Phase == 1)
                {
                    if ((p_Damage > me->GetHealth() || me->HealthBelowPctDamaged(25, p_Damage)) && !m_CastedRevivify)
                    {
                        p_Damage = 0;

                        me->InterruptNonMeleeSpells(true);

                        me->RemoveAllAuras();

                        me->RemoveAura(eSpells::ValajarBondDmgTakenDummy);

                        me->SetHealth(me->CountPctFromMaxHealth(25.0f));

                        events.Reset();

                        me->StopMoving();

                        me->GetMotionMaster()->Clear(false);

                        me->SetReactState(ReactStates::REACT_PASSIVE);

                        me->CastSpell(me, eSpells::SpellRevivify, false);

                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);

                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                        m_CastedRevivify = true;
                    }
                }
                else
                {
                    if (p_Damage > me->GetHealth() || me->HealthBelowPctDamaged(85, p_Damage))
                    {
                        p_Damage = 0;

                        me->InterruptNonMeleeSpells(true);

                        me->RemoveAllAuras();

                        events.Reset();

                        me->StopMoving();

                        me->AttackStop();

                        me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_SERVER_CONTROLLED);

                        me->SetAnimTier(3);

                        me->SetDisableGravity(true);

                        me->SetCanFly(true, true);

                        me->AddUnitState(UnitState::UNIT_STATE_CANNOT_AUTOATTACK);

                        me->SetReactState(ReactStates::REACT_PASSIVE);

                        me->SetFullHealth();

                        if (Creature* l_Odyn = Creature::GetCreature(*me, m_Instance->GetData64(eTovCreatures::CreatureBossOdyn)))
                        {
                            l_Odyn->RemoveAura(eSpells::ValajarBondDmgTaken);

                            if (UnitAI* l_AI = l_Odyn->GetAI())
                            {
                                l_AI->DoAction(10); ///< Schedule Test of the ages

                                if (boss_tov_odyn::boss_tov_odynAI* l_LinkAI = CAST_AI(boss_tov_odyn::boss_tov_odynAI, l_AI))
                                    l_LinkAI->m_TestOfTheAges[1] = true;
                            }
                        }

                        m_Instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                        me->GetMotionMaster()->Clear(false);
                        me->GetMotionMaster()->MoveSmoothFlyPath(0, g_HyrjaReturnBack.data(), g_HyrjaReturnBack.size());
                    }
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo == nullptr)
                    return;

                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellRevivify:
                    {
                        events.Reset();

                        m_CastedRevivify = false;

                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_SERVER_CONTROLLED | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO, 0, 100.0f, true))
                        {
                            me->GetMotionMaster()->Clear(false);

                            me->Attack(l_Target, true);
                            me->GetMotionMaster()->MoveChase(l_Target);
                        }

                        if (m_TimersOffset[0] >= 3)
                            m_TimersOffset[0] = 0;

                        if (m_TimersOffset[1] >= 3)
                            m_TimersOffset[1] = 0;

                        events.ScheduleEvent(eEvents::EventExpelLight, m_TimersOffsetExpelLight[m_TimersOffset[0]++]);
                        events.ScheduleEvent(eEvents::EventShieldOfLight, m_TimersOffsetExpelLight[m_TimersOffset[1]++]);
                        break;
                    }
                    default:
                        break;
                }
            }

            void KilledUnit(Unit* p_Victim) override
            {
                if (p_Victim == nullptr || !p_Victim->IsPlayer())
                    return;

                Talk(eTalks::TalkKill);
            }

            void SetData(uint32 p_ID, uint32 p_Value) override
            {
                m_Phase = p_Value;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                if (m_Instance && me->GetDistance(me->GetHomePosition()) > 150.0f)
                {
                    if (Creature* l_Odyn = Creature::GetCreature(*me, m_Instance->GetData64(eTovCreatures::CreatureBossOdyn)))
                    {
                        if (l_Odyn->IsAIEnabled)
                        {
                            l_Odyn->AI()->EnterEvadeMode();
                            return;
                        }
                    }
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventExpelLight:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM, 0, 60.0f, true, -eSpells::ExpelLightPeriodicDummy02))
                            me->CastSpell(l_Target, eSpells::ExpelLightPeriodicDummy02, true);

                        if (m_TimersOffset[0] >= 3)
                            m_TimersOffset[0] = 0;

                        events.ScheduleEvent(eEvents::EventExpelLight, m_TimersOffsetExpelLight[m_TimersOffset[0]++]);
                        break;
                    }
                    case eEvents::EventShieldOfLight:
                    {
                        if (Unit* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskNonTank))
                        {
                            me->AttackStop();

                            me->StopMoving();

                            me->SetReactState(ReactStates::REACT_PASSIVE);

                            m_ShieldOfLightTargetGuid = l_Target->GetGUID();

                            Talk(eTalks::TalkShieldOfLightWarning, l_Target->GetGUID());

                            Talk(eTalks::TalkShieldOfLight);

                            me->CastSpell(l_Target, eSpells::ShieldOfLightAuraDummy, true);

                            me->CastSpell(l_Target, eSpells::ShieldOfLightMultiEff, false);
                        }

                        if (m_TimersOffset[1] >= 3)
                            m_TimersOffset[1] = 0;

                        events.ScheduleEvent(eEvents::EventShieldOfLight, m_TimersOffsetShieldOfLight[m_TimersOffset[1]++]);
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
            return new npc_tov_odyn_hyrjaAI(p_Creature);
        }
};

/// Valarjar Runebearer - 114996
class npc_tov_odyn_valarjar_runebearer : public CreatureScript
{
    public:
       npc_tov_odyn_valarjar_runebearer() : CreatureScript("npc_tov_odyn_valarjar_runebearer") { }

        struct npc_tov_odyn_valarjar_runebearerAI : public ScriptedAI
        {
            npc_tov_odyn_valarjar_runebearerAI(Creature* p_Creature) : ScriptedAI(p_Creature), m_RuneGuid(0)
            {
                m_Instance = p_Creature->GetInstanceScript();

                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISTRACT, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
            }

            enum eSpells
            {
                RunebearerSpawnVisu                = 230415,
                Protected                          = 229584
            };

            enum eEvents
            {
                EventRunicShields = 1,
                EventCheckIfNearRune,
                EventCheckForTarget
            };

            enum eMovementInformeds
            {
                MovementInformedJump,
                MovementInformedMoveToPointNearRuneA,
                MovementInformedMoveToPointNearRune
            };

            enum eSetGuid
            {
                SetGuidChooseObj = 1,
                SetGuidChooseTar
            };

            enum eTimers
            {
                TimerCheckIfNearRune = 1 * TimeConstants::IN_MILLISECONDS
            };

            InstanceScript* m_Instance;

            bool m_DrawPowerActivated;

            uint32 m_Index;
            uint32 m_CheckFixatedTargetStatusDiff;
            uint32 m_MythicBrandSpell;
            uint32 m_FixatingSpell;
            uint32 m_DmgImmunitySpell;
            uint32 m_MarkSpell;
            uint32 m_ObjAura;

            uint64 m_FixatedTargetGuid;
            uint64 m_RuneGuid;

            void Reset() override
            {
                events.Reset();

                ClearDelayedOperations();

                m_DrawPowerActivated = true;

                m_Index = 0;
                m_RuneGuid = 0;
                m_FixatedTargetGuid = 0;

                m_MarkSpell = 0;
                m_MythicBrandSpell = 0;
                m_FixatingSpell = 0;
                m_DmgImmunitySpell = 0;

                m_ObjAura = 0;

                me->SetReactState(ReactStates::REACT_PASSIVE);

                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void GetMythicHealthMod(float& p_Mod) override
            {
                p_Mod = 1.533335f;
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (m_Instance != nullptr)
                {
                    if (!m_DrawPowerActivated)
                        return;

                    if (m_RuneGuid)
                    {
                        if (GameObject* l_Rune = GameObject::GetGameObject(*me, m_Instance->GetData64(m_RuneGuid)))
                        {
                            if (l_Rune->GetDistance2d(me->m_positionX, me->m_positionY) <= 5.0f)
                            {
                                if (Creature* l_Odyn = Creature::GetCreature(*me, m_Instance->GetData64(eTovCreatures::CreatureBossOdyn)))
                                {
                                    if (!IsMythic())
                                        l_Odyn->RemoveAura(m_MarkSpell);

                                    if (boss_tov_odyn::boss_tov_odynAI* l_LinkAI = CAST_AI(boss_tov_odyn::boss_tov_odynAI, l_Odyn->GetAI()))
                                    {
                                        /// Deactivates visuals only on LFR/Normal/Heroic because of Branded mechanic for Mythic mode
                                        if (!IsMythic())
                                        {
                                            l_Rune->ResetLootMode();
                                            l_Rune->ResetDoorOrButton();

                                            std::list<AreaTrigger*> l_ListAts;
                                            me->GetAreaTriggerListWithSpellIDInRange(l_ListAts, m_ObjAura, 200.0f);

                                            for (AreaTrigger* l_Itr : l_ListAts)
                                                l_Itr->Remove();
                                        }

                                        m_Instance->DoRemoveAurasDueToSpellOnPlayers(m_DmgImmunitySpell);

                                        m_Instance->DoRemoveAurasDueToSpellOnPlayers(m_MarkSpell);

                                        if (IsMythic()) ///< 1st phase changes on mythic, the 3rd phase runic brand is from eventreinforce.
                                        {
                                            l_LinkAI->m_RandomizedRunesListKillOrder.push_back(l_Rune->GetEntry());

                                            if (l_LinkAI->m_PhaseId == 2 || l_LinkAI->m_PhaseId == 1)
                                            {
                                                std::list<Player*> l_ListPlayers;
                                                me->GetPlayerListInGrid(l_ListPlayers, 300.0f, true);

                                                l_ListPlayers.remove_if([this](Player* p_Player) -> bool
                                                {
                                                    if (p_Player == nullptr || p_Player->isDead())
                                                        return true;

                                                    if (p_Player->HasAura(eTovSpells::BrandedMythicN) || p_Player->HasAura(eTovSpells::BrandedMythicK)
                                                        || p_Player->HasAura(eTovSpells::BrandedMythicI) || p_Player->HasAura(eTovSpells::BrandedMythicA)
                                                        || p_Player->HasAura(eTovSpells::BrandedMythicH) || p_Player->HasAura(eSpells::Protected))
                                                        return true;

                                                    return false;
                                                });

                                                int32 l_Count = 0;

                                                for (Player* l_Itr : l_ListPlayers)
                                                {
                                                    if (l_Count > 3)
                                                        break;

                                                    l_Count++;

                                                    l_Itr->CastSpell(l_Itr, g_MythicBranded[urand(0, 4)], true);
                                                }
                                            }
                                            else
                                            {
                                                std::list<Player*> l_ListPlayers;
                                                me->GetPlayerListInGrid(l_ListPlayers, 300.0f, true);

                                                l_ListPlayers.remove_if([this](Player* p_Player) -> bool
                                                {
                                                    if (p_Player == nullptr || p_Player->isDead())
                                                        return true;

                                                    if (p_Player->HasAura(eTovSpells::RunicBrandAuraDummyK) || p_Player->HasAura(eTovSpells::RunicBrandAuraDummyK)
                                                        || p_Player->HasAura(eTovSpells::RunicBrandAuraDummyI) || p_Player->HasAura(eTovSpells::RunicBrandAuraDummyA)
                                                        || p_Player->HasAura(eTovSpells::RunicBrandAuraDummyH) || p_Player->HasAura(eSpells::Protected))
                                                        return true;

                                                    return false;
                                                });

                                                int32 l_Count = 0;

                                                for (Player* l_Itr : l_ListPlayers)
                                                {
                                                    if (l_Count > 3)
                                                        break;

                                                    l_Count++;

                                                    l_Itr->CastSpell(l_Itr, g_RunicBrandSpells[urand(0, 4)], true);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            void SetGUID(uint64 p_Guid, int32 p_ID) override
            {
                if (p_ID == eSetGuid::SetGuidChooseObj)
                {
                    m_RuneGuid = p_Guid;

                    if (m_Instance != nullptr)
                    {
                        if (GameObject* l_Object = GameObject::GetGameObject(*me, m_Instance->GetData64(uint32(m_RuneGuid))))
                        {
                            if (Creature* l_Odyn = Creature::GetCreature(*me, m_Instance->GetData64(eTovCreatures::CreatureBossOdyn)))
                            {
                                if (boss_tov_odyn::boss_tov_odynAI* l_LinkAI = CAST_AI(boss_tov_odyn::boss_tov_odynAI, l_Odyn->GetAI()))
                                {
                                    switch (l_Object->GetEntry())
                                    {
                                        case eTovGameObjects::GameObjectOdynRuneSigilK:
                                        {
                                            m_MarkSpell = eTovSpells::DrawPowerDmgIncUnerringBlastK;
                                            m_MythicBrandSpell = eTovSpells::RunicBrandAuraDummyK;
                                            m_FixatingSpell = eTovSpells::BrandedFixateK;
                                            m_DmgImmunitySpell = eTovSpells::RunicShieldDmgPctTakenK;

                                            m_ObjAura = eTovSpells::RunicBrandAreatriggerVisK;
                                            break;
                                        }
                                        case eTovGameObjects::GameObjectOdynRuneSigilN:
                                        {
                                            m_MarkSpell = eTovSpells::DrawPowerDmgIncUnerringBlastN;
                                            m_MythicBrandSpell = eTovSpells::RunicBrandAuraDummyN;
                                            m_FixatingSpell = eTovSpells::BrandedFixateN;
                                            m_DmgImmunitySpell = eTovSpells::RunicShieldDmgPctTakenN;

                                            m_ObjAura = eTovSpells::RunicBrandAreatriggerVisN;
                                            break;
                                        }
                                        case eTovGameObjects::GameObjectOdynRuneSigilI:
                                        {
                                            m_MarkSpell = eTovSpells::DrawPowerDmgIncUnerringBlastI;
                                            m_MythicBrandSpell = eTovSpells::RunicBrandAuraDummyI;
                                            m_FixatingSpell = eTovSpells::BrandedFixateI;
                                            m_DmgImmunitySpell = eTovSpells::RunicShieldDmgPctTakenI;

                                            m_ObjAura = eTovSpells::RunicBrandAreatriggerVisI;
                                            break;
                                        }
                                        case eTovGameObjects::GameObjectOdynRuneSigilA:
                                        {
                                            m_MarkSpell = eTovSpells::DrawPowerDmgIncUnerringBlastA;
                                            m_MythicBrandSpell = eTovSpells::RunicBrandAuraDummyA;
                                            m_FixatingSpell = eTovSpells::BrandedFixateA;
                                            m_DmgImmunitySpell = eTovSpells::RunicShieldDmgPctTakenA;

                                            m_ObjAura = eTovSpells::RunicBrandAreatriggerVisA;
                                            break;
                                        }
                                        case eTovGameObjects::GameObjectOdynRuneSigilH:
                                        {
                                            m_MarkSpell = eTovSpells::DrawPowerDmgIncUnerringBlastH;
                                            m_MythicBrandSpell = eTovSpells::RunicBrandAuraDummyH;
                                            m_FixatingSpell = eTovSpells::BrandedFixateH;
                                            m_DmgImmunitySpell = eTovSpells::RunicShieldDmgPctTakenH;

                                            m_ObjAura = eTovSpells::RunicBrandAreatriggerVisH;
                                            break;
                                        }
                                        default:
                                            break;
                                    }

                                    if (!IsMythic())
                                        l_Odyn->CastSpell(l_Odyn, m_MarkSpell, true);

                                    if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(m_ObjAura, me->GetMap()->GetDifficultyID()))
                                    {
                                        AreaTrigger* l_AreaTrigger = new AreaTrigger;

                                        if (!l_AreaTrigger->CreateAreaTrigger(sObjectMgr->GenerateLowGuid(HIGHGUID_AREATRIGGER), 8784, me, nullptr, l_SpellInfo, *l_Object, *l_Object))
                                        {
                                            delete l_AreaTrigger;
                                            return;

                                            l_AreaTrigger->SetDuration(15 * TimeConstants::IN_MILLISECONDS);
                                            l_AreaTrigger->SetTimeToTarget(l_AreaTrigger->GetDuration());
                                        }

                                        events.ScheduleEvent(eEvents::EventCheckIfNearRune, eTimers::TimerCheckIfNearRune);
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    m_FixatedTargetGuid = p_Guid;

                    me->RemoveAllAuras();

                    me->CastSpell(me, eSpells::RunebearerSpawnVisu, false);

                    if (Player* l_Target = Player::GetPlayer(*me, m_FixatedTargetGuid))
                    {
                        AttackStart(l_Target, true);

                        me->CastSpell(l_Target, m_FixatingSpell, true);

                        me->AddThreat(l_Target, 5000.0f);
                    }

                    events.ScheduleEvent(eEvents::EventCheckForTarget, 1 * TimeConstants::IN_MILLISECONDS);
                }
            }

            void OnAddThreat(Unit* p_Victim, float& p_Threat, SpellSchoolMask /*p_SchoolMask*/, SpellInfo const* /*p_ThreatSpell*/) override
            {
                if (!m_FixatedTargetGuid || (p_Victim->GetGUID() != m_FixatedTargetGuid))
                    p_Threat = 0.0f;
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::EFFECT_MOTION_TYPE)
                    return;

                if (m_RuneGuid)
                {
                    if (GameObject* l_Rune = GameObject::GetGameObject(*me, m_RuneGuid))
                    {
                        switch (p_ID)
                        {
                            case eMovementInformeds::MovementInformedJump:
                            {
                                if (m_FixatedTargetGuid)
                                {
                                    if (Unit* l_Target = Unit::GetUnit(*me, m_FixatedTargetGuid))
                                    {
                                        AttackStart(l_Target, true);
                                        me->AddThreat(l_Target, 200.0f);
                                    }
                                }
                                break;
                            }
                            default:
                                break;
                        }
                    }
                }
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionClearAllRunesAura:
                    {
                        m_DrawPowerActivated = false;

                        events.Reset();

                        uint32 l_SpellToRemove[2] = { m_FixatingSpell, m_DmgImmunitySpell };

                        for (uint32 l_Itr : l_SpellToRemove)
                        {
                            me->RemoveAura(l_Itr);
                        }
                        break;
                    }
                    default:
                        break;
                }
            }

            void TargetDied()
            {
                if (!m_FixatingSpell)
                    return;

                /// Clear auras and
                if (m_FixatedTargetGuid)
                {
                    if (Player* l_Player = Player::GetPlayer(*me, m_FixatedTargetGuid))
                    {
                        l_Player->RemoveAura(m_FixatingSpell);
                    }

                    m_FixatedTargetGuid = 0;
                }

                /// Look for a plr without branded fixations.
                std::list<Player*> l_listPlayers;

                me->GetPlayerListInGrid(l_listPlayers, 200.0f, false);

                JadeCore::RandomResizeList(l_listPlayers, l_listPlayers.size());

                uint64 l_ItrGuid = 0;

                bool l_Caught = false;

                if (!l_listPlayers.empty())
                {
                    for (std::list<Player*>::iterator l_Plr = l_listPlayers.begin(); l_Plr != l_listPlayers.end(); l_Plr++)
                    {
                        if ((*l_Plr)->isDead())
                            continue;

                        if ((*l_Plr)->HasAura(eTovSpells::BrandedFixateN) || (*l_Plr)->HasAura(eTovSpells::BrandedFixateK) || (*l_Plr)->HasAura(eTovSpells::BrandedFixateI) ||
                            (*l_Plr)->HasAura(eTovSpells::BrandedFixateA) || (*l_Plr)->HasAura(eTovSpells::BrandedFixateH))
                            continue;

                        if (l_ItrGuid)
                            break;

                        l_Caught = true;

                        l_ItrGuid = (*l_Plr)->GetGUID();
                    }

                    if (!l_Caught)
                    {
                        std::list<Player*>::iterator l_Plr = l_listPlayers.begin();

                        if ((*l_Plr) != nullptr)
                            l_ItrGuid = (*l_Plr)->GetGUID();
                    }

                    if (l_ItrGuid)
                    {
                        if (Player* l_Player = Player::GetPlayer(*me, l_ItrGuid))
                        {
                            if (!l_Player->HasAura(m_FixatingSpell))
                            {
                                m_FixatedTargetGuid = l_ItrGuid;

                                AttackStart(l_Player, true);

                                me->CastSpell(l_Player, m_FixatingSpell, true);

                                me->AddThreat(l_Player, 5000.0f);
                            }
                        }
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (events.ExecuteEvent() == eEvents::EventCheckIfNearRune)
                {
                    if (m_RuneGuid)
                    {
                        if (m_Instance != nullptr)
                        {
                            std::list<HostileReference*> l_ThreatList = me->getThreatManager().getThreatList();

                            for (HostileReference* l_Ref : l_ThreatList)
                            {
                                if (Player* l_Player = me->GetPlayer(*me, l_Ref->getUnitGuid()))
                                {
                                    if (!l_Player->HasAura(m_FixatingSpell))
                                        me->RemoveThreatTarget(l_Player->GetGUID());
                                }
                            }

                            if (GameObject* l_Rune = GameObject::GetGameObject(*me, m_Instance->GetData64(m_RuneGuid)))
                            {
                                if (me->GetDistance2d(l_Rune->m_positionX, l_Rune->m_positionY) < 3.0f)
                                    me->RemoveAura(m_DmgImmunitySpell);

                                else if (!me->HasAura(m_DmgImmunitySpell))
                                    me->AddAura(m_DmgImmunitySpell, me);
                            }
                        }
                    }

                    events.ScheduleEvent(eEvents::EventCheckIfNearRune, 1000);
                }

                /// Check if target is dead
                if (m_FixatedTargetGuid)
                {
                    if (Player* l_Player = Player::GetPlayer(*me, m_FixatedTargetGuid))
                    {
                        if (l_Player->isDead())
                        {
                            TargetDied();
                            return;
                        }
                    }
                }

                me->ClearUnitState(UnitState::UNIT_STATE_CASTING);
                me->ClearUnitState(UnitState::UNIT_STATE_CANNOT_AUTOATTACK);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tov_odyn_valarjar_runebearerAI(p_Creature);
        }
};

/// Dancing Blade - 114560
class npc_tov_odyn_dancing_sword : public CreatureScript
{
    public:
        npc_tov_odyn_dancing_sword() : CreatureScript("npc_tov_odyn_dancing_sword") { }

        struct npc_tov_odyn_dancing_swordAI : public ScriptedAI
        {
            npc_tov_odyn_dancing_swordAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpell
            {
                DancingBladeAuraAt = 228006
            };

            void IsSummonedBy(Unit* p_Summoner) override
            {
                DoCast(me, eSpell::DancingBladeAuraAt, true);

                me->SetWalk(true);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->SetSpeed(UnitMoveType::MOVE_WALK, 0.4f, true);

                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);

                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveRandom(10.0f);
            }

            void UpdateAI(uint32 const p_Diff) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tov_odyn_dancing_swordAI(p_Creature);
        }
};

/// Raging Tempest - 112996
class npc_tov_odyn_raging_tempest : public CreatureScript
{
    public:
        npc_tov_odyn_raging_tempest() : CreatureScript("npc_tov_odyn_raging_tempest") { }

        struct npc_tov_odyn_raging_tempestAI : public ScriptedAI
        {
            npc_tov_odyn_raging_tempestAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            enum eSpells
            {
                RagingTempestSchoolDmg = 227811,
                RagingTempestAura      = 227804
            };

            enum eEvents
            {
                EventDamage = 1,
                EventMove
            };

            enum eTimers
            {
                TimerDamage = 4000,
                TimerMove   = 2500
            };

            InstanceScript* m_Instance;

            void Reset() override
            {
                me->SetWalk(true);

                me->CastSpell(me, eSpells::RagingTempestAura, true);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);

                events.ScheduleEvent(eEvents::EventDamage, eTimers::TimerDamage);
                events.ScheduleEvent(eEvents::EventMove, eTimers::TimerMove);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventDamage:
                    {
                        std::list<Player*> l_ListPlayersNerby;
                        me->GetPlayerListInGrid(l_ListPlayersNerby, 2.0f, false);

                        for (Player* l_Player : l_ListPlayersNerby)
                        {
                            Position l_Pos = me->GetPosition();
                            if (l_Pos.GetExactDist(l_Player) <= 2.0f)
                                l_Player->CastSpell(l_Player, eSpells::RagingTempestSchoolDmg, true);
                        }

                        events.ScheduleEvent(eEvents::EventDamage, eTimers::TimerDamage / 4);
                        break;
                    }
                    case eEvents::EventMove:
                    {
                        me->GetMotionMaster()->MoveRandom(2.0f);

                        events.ScheduleEvent(eEvents::EventMove, eTimers::TimerMove);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tov_odyn_raging_tempestAI(p_Creature);
        }
};

/// Shield of Light - 228162
class spell_tov_odyn_shield_of_light : public SpellScriptLoader
{
    public:
        spell_tov_odyn_shield_of_light() : SpellScriptLoader("spell_tov_odyn_shield_of_light") { }

        class spell_tov_odyn_shield_of_light_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tov_odyn_shield_of_light_SpellScript);

            enum eTrigger
            {
                WorldTrigger = 19871
            };

            enum eSpell
            {
                TargetRestrict = 33789
            };

            int32 m_Size;

            bool Load() override
            {
                m_Size = 0;
                return true;
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Target = GetExplTargetUnit();

                Unit* l_Caster = GetCaster();

                if (!l_Target || !l_Caster || p_Targets.empty())
                    return;

                SpellTargetRestrictionsEntry const* l_Restriction = sSpellTargetRestrictionsStore.LookupEntry(eSpell::TargetRestrict);
                if (l_Restriction == nullptr)
                    return;

                p_Targets.remove_if([l_Caster, l_Restriction](WorldObject* p_Target) -> bool
                {
                    if (p_Target == nullptr)
                        return true;

                    if (!p_Target->IsInAxe(l_Caster, l_Restriction->Width, 200.0f))
                        return true;

                    return false;
                });

                if (!p_Targets.empty())
                    m_Size = p_Targets.size();
            }

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                if (!m_Size)
                    return;

                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_HitUnit = GetHitUnit())
                    {
                        if (int32 l_HitDamage = GetHitDamage())
                        {
                            l_HitDamage /= m_Size;

                            SetHitDamage(l_HitDamage);
                        }
                    }
                }
            }

            void HandleKnockBack(SpellEffIndex /*p_EffIndex*/)
            {
                if (!m_Size)
                    return;

                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_HitUnit = GetHitUnit())
                    {
                        if (int32 l_HitDamage = GetSpell()->GetDamage())
                        {
                            l_HitDamage /= m_Size;

                            /// Split knock back force between affected players
                            GetSpell()->SetDamage(l_HitDamage);
                        }
                    }
                }
            }

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (GetSpell() && GetExplTargetUnit() && l_Caster && l_Caster->ToCreature())
                {
                    GetSpell()->destTarget = &*GetExplTargetUnit();

                    l_Caster->ToCreature()->AttackStop();

                    l_Caster->ToCreature()->SetReactState(ReactStates::REACT_PASSIVE);

                    l_Caster->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                   /// l_Caster->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);
                }
            }

            void HandleAfterCast()
            {
                if (GetExplTargetUnit() && GetCaster())
                {
                    GetCaster()->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                    GetCaster()->UpdateOrientation(GetCaster()->GetAngle(GetExplTargetUnit()->m_positionX, GetExplTargetUnit()->m_positionY));

                    if (Player* l_Plr = GetExplTargetUnit()->ToPlayer())
                        l_Plr->RemoveAura(228270);

                    if (UnitAI* l_AI = GetCaster()->GetAI())
                        l_AI->DoAction(2); ///< Stop SOL
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_tov_odyn_shield_of_light_SpellScript::HandleAfterCast);
                BeforeCast += SpellCastFn(spell_tov_odyn_shield_of_light_SpellScript::HandleBeforeCast);
                OnEffectHitTarget += SpellEffectFn(spell_tov_odyn_shield_of_light_SpellScript::HandleDamage, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
                OnEffectHitTarget += SpellEffectFn(spell_tov_odyn_shield_of_light_SpellScript::HandleKnockBack, EFFECT_2, SPELL_EFFECT_KNOCK_BACK);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tov_odyn_shield_of_light_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_ENEMY_BETWEEN_DEST);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tov_odyn_shield_of_light_SpellScript::FilterTargets, EFFECT_2, TARGET_UNIT_ENEMY_BETWEEN_DEST);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tov_odyn_shield_of_light_SpellScript();
        }
};

/// Horn of Valor - 228012
class spell_tov_odyn_horn_of_valor : public SpellScriptLoader
{
    public:
        spell_tov_odyn_horn_of_valor() : SpellScriptLoader("spell_tov_odyn_horn_of_valor") { }

        class spell_tov_odyn_horn_of_valor_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tov_odyn_horn_of_valor_SpellScript);

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                if (GetSpellInfo() != nullptr)
                {
                    if (Unit* l_Caster = GetCaster())
                    {
                        if (InstanceScript* l_Instance = l_Caster->GetInstanceScript())
                        {
                            if (Creature* l_Hymdall = Creature::GetCreature(*l_Caster, l_Instance->GetData64(eTovCreatures::CreatureHymdall)))
                            {
                                if (Unit* l_HitUnit = GetHitUnit())
                                {
                                    if (int32 l_HitDamage = GetHitDamage())
                                    {
                                        int32 l_Dmg = GetHitDamage();

                                        if (!l_Hymdall->IsWithinDist(l_HitUnit, 15.0f))
                                            l_Dmg -= l_Dmg * 0.25f;
                                        else
                                            l_Dmg += l_Dmg * 0.19f;

                                        SetHitDamage(l_Dmg);
                                    }
                                }
                            }
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_tov_odyn_horn_of_valor_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tov_odyn_horn_of_valor_SpellScript();
        }
};

/// Echo of Radiance - 231357
class spell_tov_odyn_echo_of_radiance : public SpellScriptLoader
{
    public:
        spell_tov_odyn_echo_of_radiance() : SpellScriptLoader("spell_tov_odyn_echo_of_radiance") { }

        class spell_tov_odyn_echo_of_radiance_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tov_odyn_echo_of_radiance_SpellScript);

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_HitUnit = GetHitUnit())
                    {
                        for (uint32 l_Itr : g_RunicBrandSpells)
                        {
                            if (l_Caster->HasAura(l_Itr) && l_HitUnit->HasAura(l_Itr))
                                SetHitDamage(0);
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_tov_odyn_echo_of_radiance_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tov_odyn_echo_of_radiance_SpellScript();
        }
};

/// Expel Light - 228029
class spell_tov_odyn_expel_light : public SpellScriptLoader
{
    public:
        spell_tov_odyn_expel_light() : SpellScriptLoader("spell_tov_odyn_expel_light") { }

        class spell_tov_odyn_expel_light_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tov_odyn_expel_light_AuraScript);

            enum eSpells
            {
                ExpelLightDmg = 228030
            };

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    PreventDefaultAction();
                }
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetTarget())
                        l_Caster->CastSpell(*l_Target, eSpells::ExpelLightDmg, true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_tov_odyn_expel_light_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_tov_odyn_expel_light_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tov_odyn_expel_light_AuraScript();
        }
};

/// Stormforged Spear - 228932
class spell_tov_odyn_stormforged_spear : public SpellScriptLoader
{
    public:
        spell_tov_odyn_stormforged_spear() : SpellScriptLoader("spell_tov_odyn_stormforged_spear") { }

        class spell_tov_odyn_stormforged_spear_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tov_odyn_stormforged_spear_SpellScript);

            enum eSpell
            {
                StormforgedSpell = 228932
            };

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_HitUnit = GetHitUnit())
                    {
                        if (const SpellInfo* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpell::StormforgedSpell, l_Caster->GetMap()->GetDifficultyID())) ///< Draw Power
                        {
                            float const l_Distance = l_Caster->GetDistance2d(l_HitUnit->m_positionX, l_HitUnit->m_positionY); ///< With raging tempest, 3d distance maybe will be better?

                            int32 l_HitDamage = GetHitDamage();

                            float l_DiffMultip = 3.0f;

                            if (InstanceScript* l_Instance = l_Caster->GetInstanceScript())
                            {
                                if (l_Instance->instance->IsMythic())
                                    l_DiffMultip = 2.3f;
                                else if (l_Instance->instance->IsHeroic())
                                    l_DiffMultip = 2.5f;
                                else if(l_Instance->instance->IsLFR())
                                    l_DiffMultip = 3.4f;

                                l_HitDamage -= int32(l_Distance * (140000.0f * l_DiffMultip));

                                SetHitDamage(l_HitDamage);
                            }
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_tov_odyn_stormforged_spear_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tov_odyn_stormforged_spear_SpellScript();
        }
};

/// Arc Storm - 229254
class spell_tov_odyn_arc_storm : public SpellScriptLoader
{
    public:
        spell_tov_odyn_arc_storm() : SpellScriptLoader("spell_tov_odyn_arc_storm") { }

        class spell_tov_odyn_arc_storm_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tov_odyn_arc_storm_AuraScript);

            enum eSpells
            {
                ArcStormDmg = 229255
            };

            uint8 m_PowerCount = 0;

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                Creature* l_Caster = GetCaster() ? GetCaster()->ToCreature() : nullptr;
                if (!l_Caster || !l_Caster->isInCombat())
                    return;

                if (Player* l_Plr = l_Caster->FindNearestPlayer(150.0f))
                    l_Caster->CastSpell(l_Plr, eSpells::ArcStormDmg, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_tov_odyn_arc_storm_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tov_odyn_arc_storm_AuraScript();
        }
};

/// Odyn's Test - 227626
class spell_tov_odyn_odyns_test : public SpellScriptLoader
{
    public:
        spell_tov_odyn_odyns_test() : SpellScriptLoader("spell_tov_odyn_odyns_test") { }

        class spell_tov_odyn_odyns_test_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tov_odyn_odyns_test_AuraScript);

            void AfterApply(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                float l_Mod = 1.0f + p_AurEff->GetAmount() / 100.0f;

                if (Unit* l_Target = GetTarget())
                    l_Target->SetFloatValue(EUnitFields::UNIT_FIELD_ATTACK_ROUND_BASE_TIME + WeaponAttackType::BaseAttack, 1500 / l_Mod);
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                    l_Target->SetFloatValue(EUnitFields::UNIT_FIELD_ATTACK_ROUND_BASE_TIME + WeaponAttackType::BaseAttack, 1500);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_tov_odyn_odyns_test_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_MOD_ATTACKSPEED, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                AfterEffectRemove += AuraEffectRemoveFn(spell_tov_odyn_odyns_test_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_MOD_ATTACKSPEED, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tov_odyn_odyns_test_AuraScript();
        }
};

/// Cleansing Flame - 227448
/// Cleansing Flame - 227455
/// Cleansing Flame - 227456
class at_tov_odyn_cleansing_flame : public AreaTriggerEntityScript
{
    public:
        at_tov_odyn_cleansing_flame() : AreaTriggerEntityScript("at_tov_odyn_cleansing_flame") { }

        enum eSpell
        {
            CleansingFlameDebuff = 227475
        };

        std::set<uint64> m_AffectedPlayers;

        uint32 m_ActivationTimer = 0;

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Diff) override
        {
            m_ActivationTimer += p_Diff;

            /// Doesn't deal damage before 10 seconds
            if (m_ActivationTimer < 10 * TimeConstants::IN_MILLISECONDS)
                return;

            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            std::list<Player*> l_PlayerList;
            p_AreaTrigger->GetPlayerListInGrid(l_PlayerList, p_AreaTrigger->GetRadius());

            std::set<uint64> l_Targets;

            for (Player* l_Player : l_PlayerList)
            {
                if (CanAddTarget(p_AreaTrigger, l_Player))
                {
                    l_Targets.insert(l_Player->GetGUID());

                    if (!l_Player->HasAura(eSpell::CleansingFlameDebuff))
                    {
                        m_AffectedPlayers.insert(l_Player->GetGUID());
                        l_Caster->CastSpell(l_Player, eSpell::CleansingFlameDebuff, true);
                    }
                }
            }

            for (std::set<uint64>::iterator l_Iter = m_AffectedPlayers.begin(); l_Iter != m_AffectedPlayers.end();)
            {
                if (l_Targets.find((*l_Iter)) != l_Targets.end())
                {
                    ++l_Iter;
                    continue;
                }

                if (Player* l_Player = Player::GetPlayer(*l_Caster, (*l_Iter)))
                {
                    l_Iter = m_AffectedPlayers.erase(l_Iter);
                    l_Player->RemoveAura(eSpell::CleansingFlameDebuff);

                    continue;
                }

                ++l_Iter;
            }
        }

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            for (uint64 l_Guid : m_AffectedPlayers)
            {
                if (Player* l_Player = Player::GetPlayer(*p_AreaTrigger, l_Guid))
                    l_Player->RemoveAura(eSpell::CleansingFlameDebuff);
            }
        }

        bool CanAddTarget(AreaTrigger* p_AreaTrigger, Player* p_Target)
        {
            float l_InnerRadius = 29.105f;
            switch (p_AreaTrigger->GetSpellId())
            {
                case eCleansingFlames::CleansingFlamesOnLeft:
                {
                    /// If inside the center, can affect
                    if (p_Target->GetExactDist2d(&g_CleansingFlame) < l_InnerRadius)
                        return true;

                    float l_LeftAngle   = 2.49262643f;
                    float l_Angle       = g_CleansingFlame.GetAngle(p_Target);

                    /// If inside safe quarter, don't affect
                    if (l_Angle > 0.0f && l_Angle < l_LeftAngle)
                        return false;

                    break;
                }
                case eCleansingFlames::CleansingFlamesOnRight:
                {
                    /// If inside the center, can affect
                    if (p_Target->GetExactDist2d(&g_CleansingFlame) < l_InnerRadius)
                        return true;

                    float l_RightAngle  = (2.0f * M_PI) - 2.49262643f;
                    float l_Angle       = g_CleansingFlame.GetAngle(p_Target);

                    /// If inside safe quarter, don't affect
                    if (l_Angle < (2.0f * M_PI) && l_Angle > l_RightAngle)
                        return false;

                    break;
                }
                case eCleansingFlames::CleansingFlamesOnCenter:
                {
                    /// If inside the center, don't affect
                    if (p_Target->GetExactDist2d(&g_CleansingFlame) < l_InnerRadius)
                        return false;

                    float l_BackAngle = 1.29895067f;

                    /// If below the throne, in the safe angle, don't affect either
                    if (p_AreaTrigger->isInBack(p_Target, l_BackAngle))
                        return false;

                    break;
                }
                default:
                    break;
            }

            return true;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_tov_odyn_cleansing_flame();
        }
};

#ifndef __clang_analyzer__
void AddSC_tov_boss_odyn()
{
    /// Boss
    new boss_tov_odyn();

    /// Creatures
    new npc_tov_odyn_hymdall();
    new npc_tov_odyn_hyrja();
    new npc_tov_odyn_valarjar_runebearer();
    new npc_tov_odyn_dancing_sword();
    new npc_tov_odyn_raging_tempest();

    /// Spells
    new spell_tov_odyn_horn_of_valor();
    new spell_tov_odyn_shield_of_light();
    new spell_tov_odyn_stormforged_spear();
    new spell_tov_odyn_arc_storm();
    new spell_tov_odyn_echo_of_radiance();
    new spell_tov_odyn_expel_light();
    new spell_tov_odyn_odyns_test();

    /// AreaTrigger
    new at_tov_odyn_cleansing_flame();
}
#endif
