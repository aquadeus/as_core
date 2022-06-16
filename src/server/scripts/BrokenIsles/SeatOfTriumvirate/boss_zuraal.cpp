////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2020 Millenium-Studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "seat_of_triumvirate.hpp"

enum eSpells
{
    /// Zuraal the Ascended
    SpellDecimate           = 244579,
    SpellNullPalmAOE        = 246133,
    SpellNullPalm           = 246134,
    SpellNullPalmMissile    = 246135,
    SpellCoalesceVoid       = 246139,
    SpellCoalesceVoidSummon = 244602,
    SpellUmbraShift         = 247576,
    SpellUmbraShiftDmg      = 244433,
    SpellMaddenedFrenzy     = 247038,
    SpellFixateAOE          = 244653,
    SpellFixateZuraalAura   = 244657,
    SpellVoidPhased         = 246913,
    SpellVoidRealm          = 244061,
    SpellVoidRealm2         = 244620,
    SpellVoid               = 244624,
    SpellPhysicRealmCosmetic= 244087,

    /// Coalesced Void
    SpellDarkExpulsion      = 244599,

    /// Dark Aberration
    SpellReleaseVoidEnergy  = 244618,
    SpellUmbralEjection     = 244731,
    SpellUmbralEjectionAt   = 244732,

    /// Urjad
    SpellGrandBarrage       = 245366,
    SpellGrimWound          = 246312,

    /// Player Spells
    SpellVoidTear           = 244621,
};

enum eEvents
{
    /// Zuraal the Ascended
    EventNullPalm   = 1,
    EventDecimate,
    EventCoalesenceVoid,
    EventUmbraShift,
    EventCheckUmbralShiftTarget
};

enum eTalks
{
    TalkAggro,
    TalkKill,
    TalkDead,
    TalkUmbraShift,
};

Position const g_CoalesceVoidPos[] = 
{
    { 5519.390f, 10836.798f, 20.151f, 4.47f },
    { 5457.870f, 10772.879f, 20.414f, 0.70f },
    { 5546.014f, 10751.687f, 19.449f, 2.01f },
    { 5551.642f, 10835.055f, 19.614f, 4.09f },
    { 5474.061f, 10841.209f, 18.527f, 5.44f },
    { 5521.718f, 10842.762f, 20.188f, 4.42f },
    { 5564.130f, 10788.686f, 19.310f, 2.90f }
};

static const uint32 PHASE_VOID_REALM = 8682;

/// Zuraal the Ascended - 122313
class boss_zuraal_the_ascended : public CreatureScript
{
    public:
        boss_zuraal_the_ascended() : CreatureScript("boss_zuraal_the_ascended")
        {}

        struct boss_zuraal_the_ascended_AI : public BossAI
        {
            explicit boss_zuraal_the_ascended_AI(Creature* p_Me) : BossAI(p_Me, eData::DataZuraal)
            {
                CheckIntro();
            }

            void GetMythicHealthMod(float& p_Mod) override
            {
                if (IsChallengeMode())
                    p_Mod = 2.65f;
            }

            void Reset() override
            {
                me->SetReactState(REACT_AGGRESSIVE);
                me->DespawnAreaTriggersInArea(eSpells::SpellUmbralEjectionAt, 250.f);
                m_Stunned = false;
                _Reset();

                if (IsChallengeMode())
                    me->UpdateStatsForLevel();

                CheckIntro();
                RespawnVoidRealmAdds();

                m_UmbralTargetGUID = 0;
            }

            void JustReachedHome() override
            {
                _JustReachedHome();
                CheckIntro();
                RespawnVoidRealmAdds();
            }

            void RespawnVoidRealmAdds()
            {
                std::list<Creature*> l_Adds;

                me->GetCreatureListWithEntryInGrid(l_Adds, eCreatures::NpcDarkAberration, 250.f);

                for (Creature* l_Itr : l_Adds)
                {
                    if (l_Itr == nullptr)
                        continue;

                    if (l_Itr->isDead())
                        l_Itr->Respawn(true, true);
                }
            }

            void CheckIntro()
            {
                if (instance == nullptr)
                    return;

                if (instance->GetData(eCreatures::BossZuraalTheAscended) == EncounterState::DONE)
                    StartIntro();
            }

            void ApplyPhysicRealmCosmetic()
            {
                if (instance == nullptr)
                    return;

                instance->DoAddAuraOnPlayers(eSpells::SpellPhysicRealmCosmetic);
            }

            void EnterCombat(Unit* /**/) override
            {
                m_PhaseTwo = false;
                m_Stunned = false;
                m_UmbralTargetGUID = 0;
                Talk(eTalks::TalkAggro);
                _EnterCombat();
                ApplyPhysicRealmCosmetic();
                events.ScheduleEvent(eEvents::EventNullPalm, 11 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventCoalesenceVoid, 19 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventUmbraShift, 40 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventDecimate, 18 * IN_MILLISECONDS);
            }

            void RestartTimers()
            {
                events.RescheduleEvent(eEvents::EventNullPalm, 11 * IN_MILLISECONDS);
                events.RescheduleEvent(eEvents::EventCoalesenceVoid, 20 * IN_MILLISECONDS);
                events.RescheduleEvent(eEvents::EventDecimate, 18 * IN_MILLISECONDS);
                events.RescheduleEvent(eEvents::EventUmbraShift, 41 * IN_MILLISECONDS);
            }

            void StartIntro()
            {
                me->RemoveAura(eSharedSpells::SpellVoidContainment);
                me->RemoveAura(eSpells::SpellVoidPhased);
                
                me->SetCanFly(false, true);
                me->SetDisableGravity(false);

                me->SetHover(false);

                me->SetByteValue(UNIT_FIELD_ANIM_TIER, UNIT_BYTES_1_OFFSET_STAND_STATE, 0);
                me->SetByteValue(UNIT_FIELD_ANIM_TIER, UNIT_BYTES_1_OFFSET_VIS_FLAG, 0);
                me->SetByteValue(UNIT_FIELD_ANIM_TIER, UNIT_BYTES_1_OFFSET_ANIM_TIER, 0);
                me->SetByteValue(UNIT_FIELD_SHAPESHIFT_FORM, UNIT_BYTES_2_OFFSET_SHEATH_STATE, 0);

                AddTimedDelayedOperation(5000, [this]() -> void
                {
                    me->HandleEmoteCommand(EMOTE_ONESHOT_BATTLEROAR);
                });

                AddTimedDelayedOperation(7000, [this]() -> void
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
                });
            }

            void EnterEvadeMode() override
            {
                m_PhaseTwo = false;
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellVoidRealm);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellVoidRealm2);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellVoid);
                BossAI::EnterEvadeMode();
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eSharedActions::ActionCreatureEvent)
                {
                    if (instance == nullptr)
                        return;

                    StartIntro();

                    instance->SetData(eCreatures::BossZuraalTheAscended, EncounterState::DONE);
                }
            }

            void OnSpellCasted(SpellInfo const* p_Spell) override
            {
                if (p_Spell == nullptr)
                    return;

                if (p_Spell->Id == eSpells::SpellUmbraShiftDmg)
                    StartMadness();
                else if (p_Spell->Id == eSpells::SpellFixateZuraalAura)
                    me->ClearUnitState(UNIT_STATE_CASTING);
                else if (p_Spell->Id == eSpells::SpellDecimate)
                {
                    uint8 l_Adds = urand(1, IsMythic() ? 7 : 4);

                    for (uint8 l_Itr = 0; l_Itr < l_Adds; ++l_Itr)
                        me->CastSpell(g_CoalesceVoidPos[l_Itr], eSpells::SpellCoalesceVoidSummon, true);
                }
            }

            void SpellHit(Unit* /**/, SpellInfo const* p_Spell) override
            {
                if (p_Spell == nullptr)
                    return;

                if (p_Spell->Id == eSpells::SpellVoidTear)
                {
                    events.CancelEvent(eEvents::EventCheckUmbralShiftTarget);

                    m_UmbralTargetGUID = 0;
                    m_PhaseTwo = false;
                    m_Stunned = true;

                    me->RemoveAura(eSpells::SpellMaddenedFrenzy);
                    me->RemoveAura(eSpells::SpellFixateZuraalAura);

                    AddTimedDelayedOperation(20 * IN_MILLISECONDS, [this] () -> void
                    {
                        m_Stunned = false;
                        RestartTimers();
                    });
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_Spell) override
            {
                if (p_Spell == nullptr)
                    return;

                if (p_Spell->Id == eSpells::SpellNullPalmAOE)
                    me->DelayedCastSpell(p_Target, eSpells::SpellNullPalm, false, 100);
                else if (p_Spell->Id == eSpells::SpellUmbraShiftDmg)
                {
                    m_UmbralTargetGUID = p_Target->GetGUID();
                    events.ScheduleEvent(eEvents::EventCheckUmbralShiftTarget, 0.5 * TimeConstants::IN_MILLISECONDS);
                }
            }

            void OnRemoveAura(uint32 p_SpellId, AuraRemoveMode /**/) override
            {
                if (p_SpellId == eSpells::SpellFixateZuraalAura && m_PhaseTwo)
                    me->DelayedCastSpell(me, eSpells::SpellFixateAOE, true, 100);
                else if (p_SpellId == eSpells::SpellMaddenedFrenzy)
                    m_PhaseTwo = false;
            }

            bool CanBeAttacked(Unit const* p_Target, SpellInfo const* p_Spell) const override
            {
                if (p_Spell != nullptr && p_Spell->Id == eSpells::SpellVoidTear)
                    return true;

                return !p_Target->HasAura(eSpells::SpellVoidRealm);
            }

            void KilledUnit(Unit* p_Victim) override
            {
                if (p_Victim == nullptr)
                    return;

                if (p_Victim->IsPlayer())
                    Talk(eTalks::TalkKill);
            }

            void CompleteAchievement()
            {
                if (instance == nullptr)
                    return;
                Creature* l_Urjad = me->FindNearestCreature(eCreatures::NpcUrjad, 250.f);

                if (l_Urjad == nullptr)
                    instance->DoCompleteAchievement(eAchievements::WelcomeTheVoid);
            }

            void JustDied(Unit* /**/) override
            {
                Talk(eTalks::TalkDead);
                ClearDelayedOperations();
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellVoidRealm);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellVoidRealm2);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellVoid);
                _JustDied();

                if (IsMythic())
                    CompleteAchievement();

                if (!IsChallengeMode())
                    instance->SendScenarioEventToAllPlayers(eScenarioEvents::ZuraalDeath);
            }

            void StartMadness()
            {
                m_PhaseTwo = true;
                events.Reset();

                AddTimedDelayedOperation(IN_MILLISECONDS, [this]() -> void
                {
                    DoCast(me, eSpells::SpellMaddenedFrenzy, true);
                    DoCastAOE(eSpells::SpellFixateAOE, true);
                });
            }

            void ExecuteEvent(uint32 const p_EventId) override
            {
                switch (p_EventId)
                {
                    case eEvents::EventNullPalm:
                    {
                        DoCastAOE(eSpells::SpellNullPalmAOE);
                        events.ScheduleEvent(eEvents::EventNullPalm, 55 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventDecimate:
                    {
                        DoCastVictim(eSpells::SpellDecimate);
                        events.ScheduleEvent(eEvents::EventDecimate, 12 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventUmbraShift:
                    {
                        RespawnVoidRealmAdds();
                        Talk(eTalks::TalkUmbraShift);
                        DoCastAOE(eSpells::SpellUmbraShift);
                        break;
                    }

                    case eEvents::EventCheckUmbralShiftTarget:
                    {
                        if (!instance)
                            break;

                        if (Player* l_UmbralShitTarget = Player::GetPlayer(*me, m_UmbralTargetGUID))
                        {
                            if (l_UmbralShitTarget->isDead())
                            {
                                m_PhaseTwo = false;
                                m_Stunned = false;

                                me->RemoveAura(eSpells::SpellMaddenedFrenzy);
                                me->RemoveAura(eSpells::SpellFixateZuraalAura);

                                me->ClearUnitState(UNIT_STATE_ROOT | UNIT_STATE_CANNOT_TURN);

                                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellVoidRealm);
                                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellVoidRealm2);
                                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellVoid);
                                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellFixateAOE);

                                RestartTimers();
                                break;
                            }
                        }

                        events.ScheduleEvent(eEvents::EventCheckUmbralShiftTarget, 0.5 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default: 
                        break;
                }
            }

            void CheckDistToEvade()
            {
                Position l_Center = me->GetHomePosition();

                if (me->GetDistance2d(l_Center.m_positionX, l_Center.m_positionY) >= 56.f)
                {
                    EnterEvadeMode();
                    return;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                CheckDistToEvade();

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING) || 
                    m_Stunned)
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                    ExecuteEvent(eventId);

                DoMeleeAttackIfReady();
            }

            uint64 m_UmbralTargetGUID;
            bool m_PhaseTwo;
            bool m_Stunned;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new boss_zuraal_the_ascended_AI(p_Me);
        }
};

/// Dark Aberration - 122482
class npc_zuraal_dark_aberration : public CreatureScript
{
    public:
        npc_zuraal_dark_aberration() : CreatureScript("npc_zuraal_dark_aberration")
        {}

        struct npc_zuraal_dark_aberration_AI : public ScriptedAI
        {
            explicit npc_zuraal_dark_aberration_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            bool CanAIAttack(Unit const* p_Target) const override
            {
                return p_Target->HasAura(eSpells::SpellVoidRealm);
            }

            void JustDied(Unit* /**/) override
            {
                DoCast(me, eSpells::SpellReleaseVoidEnergy, true);
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_zuraal_dark_aberration_AI(p_Me);
        }
};

/// Coalesced Void - 122716
class npc_zuraal_coalesced_void : public CreatureScript
{
    public:
        npc_zuraal_coalesced_void() : CreatureScript("npc_zuraal_coalesced_void")
        {}

        struct npc_zuraal_coalesced_void_AI : public ScriptedAI
        {
            explicit npc_zuraal_coalesced_void_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                m_TimerFollow = 0;
                m_NotArrived = false;
                me->SetReactState(REACT_PASSIVE);
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (p_Summoner->GetEntry() == eCreatures::BossZuraalTheAscended)
                {
                    m_ZuraalGuid = p_Summoner->GetGUID();
                    me->SetWalk(true);
                    me->GetMotionMaster()->MoveFollow(p_Summoner, 0, 0.f);
                    m_TimerFollow = 0;
                    m_NotArrived = false;
                }
            }

            void Reset() override
            {
                if (IsMythic())
                    me->CastSpell(me, eSpells::SpellUmbralEjection, true);
            }

            void CheckDistance()
            {
                if (m_NotArrived)
                    return;

                Unit* l_Summoner = sObjectAccessor->FindUnit(m_ZuraalGuid);

                if (l_Summoner == nullptr)
                    return;

                if (l_Summoner->GetDistance2d(me) <= 3.0f)
                {
                    m_NotArrived = true;
                    me->GetMotionMaster()->Clear();
                    me->StopMoving();

                    DoCast(me, eSpells::SpellDarkExpulsion, true);
                }
            }

            void OnSpellCasted(SpellInfo const* p_Spell) override
            {
                if (p_Spell == nullptr)
                    return;

                if (p_Spell->Id == eSpells::SpellDarkExpulsion)
                {
                    AddTimedDelayedOperation(100, [this] () -> void
                    {
                        me->Kill(me);
                    });
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                CheckDistance();
            }

            uint32  m_TimerFollow;
            uint64  m_ZuraalGuid;
            bool    m_NotArrived;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_zuraal_coalesced_void_AI(p_Me);
        }
};

/// Urjad - 126283
class npc_zuraal_urjad : public CreatureScript
{
    public:
        npc_zuraal_urjad() : CreatureScript("npc_zuraal_urjad")
        {}

        enum eEvents
        {
            EventGrandBarrage = 1,
            EventGrimWound,
        };

        struct npc_zuraal_urjad_AI : public ScriptedAI
        {
            explicit npc_zuraal_urjad_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                me->GetMotionMaster()->MovePath(10, m_UrjadPath.data(), m_UrjadPath.size());
            }

            void JustReachedHome() override
            {
                DoCast(me, eSpells::SpellVoidRealm, true);

                AddTimedDelayedOperation(100, [this]() -> void
                {
                    me->GetMotionMaster()->Clear();
                    me->StopMoving();
                    me->GetMotionMaster()->MoveSmoothPath(10, m_UrjadPath.data(), m_UrjadPath.size(), true);
                });
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::EFFECT_MOTION_TYPE)
                    return;

                if (p_ID == 10 && !me->isInCombat())
                {
                    AddTimedDelayedOperation(10, [this]() -> void
                    {
                        me->GetMotionMaster()->Clear();
                        me->StopMoving();
                        me->GetMotionMaster()->MoveSmoothPath(10, m_UrjadPath.data(), m_UrjadPath.size(), true);
                    });
                }
            }

            void EnterCombat(Unit* /**/) override
            {
                DoZoneInCombat();
                events.ScheduleEvent(eEvents::EventGrandBarrage, urand(10, 12) * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventGrimWound, urand(5, 8) * IN_MILLISECONDS);
            }

            void ExecuteEvent(uint32 const p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventGrandBarrage:
                    {
                        DoCast(me, eSpells::SpellGrandBarrage);
                        events.ScheduleEvent(eEvents::EventGrandBarrage, urand(10, 18) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventGrimWound:
                    {
                        DoCastVictim(eSpells::SpellGrimWound);
                        events.ScheduleEvent(eEvents::EventGrimWound, urand(8, 15) * IN_MILLISECONDS);
                        break;
                    }

                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 const l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }

            std::array<G3D::Vector3, 13> const m_UrjadPath =
            {
                {
                    { 5585.971f, 10784.809f, 20.114f },
                    { 5591.554f, 10814.327f, 20.110f },
                    { 5565.313f, 10842.813f, 20.110f },
                    { 5532.840f, 10862.245f, 20.110f },
                    { 5490.131f, 10866.876f, 20.299f },
                    { 5445.363f, 10843.707f, 20.299f },
                    { 5430.759f, 10815.445f, 20.271f },
                    { 5435.483f, 10782.761f, 20.111f },
                    { 5466.158f, 10749.967f, 20.112f },
                    { 5508.844f, 10735.642f, 20.610f },
                    { 5542.647f, 10738.641f, 20.111f },
                    { 5561.847f, 10745.813f, 20.111f },
                    { 5577.507f, 10759.140f, 20.184f } 
                }
            };
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_zuraal_urjad_AI(p_Me);
        }
};

/// Void Realm - 244061
class spell_zuraal_void_realm : public SpellScriptLoader
{
    public:
        spell_zuraal_void_realm() : SpellScriptLoader("spell_zuraal_void_realm")
        {}

        

        class spell_zuraal_void_realm_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_zuraal_void_realm_AuraScript);

            void UpdateVisibility(Player* p_Target)
            {
                if (p_Target == nullptr)
                    return;

                std::list<Creature*> l_Mobs;

                p_Target->GetCreatureListWithEntryInGridAppend(l_Mobs, eCreatures::NpcShadowguardSubjugator, 500.f);
                p_Target->GetCreatureListWithEntryInGridAppend(l_Mobs, eCreatures::NpcFamishedBroken, 500.f);
                p_Target->GetCreatureListWithEntryInGridAppend(l_Mobs, eCreatures::NpcSappedVoidLord, 500.f);
                p_Target->GetCreatureListWithEntryInGridAppend(l_Mobs, eCreatures::NpcVoidDischarge, 500.f);
                p_Target->GetCreatureListWithEntryInGridAppend(l_Mobs, eCreatures::NpcUrjad, 500.f);
                
                if (Creature* l_Zuraal = p_Target->FindNearestCreature(eCreatures::BossZuraalTheAscended, 150.f, true))
                    l_Mobs.push_back(l_Zuraal);

                for (Creature* l_Itr : l_Mobs)
                {
                    if (l_Itr == nullptr)
                        continue;

                    l_Itr->SendUpdateToPlayer(p_Target);
                }

                Map::PlayerList const& l_Targets = p_Target->GetMap()->GetPlayers();

                for (auto l_Itr = l_Targets.begin(); l_Itr != l_Targets.end(); ++l_Itr)
                {
                    Player* l_Target = l_Itr->getSource();

                    if (l_Target == nullptr || l_Target == p_Target)
                        continue;

                    l_Target->SendUpdateToPlayer(p_Target);
                    p_Target->SendUpdateToPlayer(l_Target);
                }
            }

            void HandleOnApply(AuraEffect const* /**/, AuraEffectHandleModes /**/)
            {
                if (GetUnitOwner() == nullptr)
                    return;
                
                GetUnitOwner()->RemoveAurasDueToSpell(eSpells::SpellPhysicRealmCosmetic);

                if (GetUnitOwner()->IsPlayer())
                {
                    GetUnitOwner()->SetPhaseMask(PHASE_VOID_REALM | PHASEMASK_NORMAL, true);
                    UpdateVisibility(GetUnitOwner()->ToPlayer());
                }
                else
                    GetUnitOwner()->SetPhaseMask(PHASE_VOID_REALM, true);
            }

            void HandleOnRemove(AuraEffect const* /**/, AuraEffectHandleModes /**/)
            {
                if (GetUnitOwner() == nullptr)
                    return;

                Unit* l_Owner = GetUnitOwner();

                if (l_Owner->IsPlayer())
                    UpdateVisibility(l_Owner->ToPlayer());

                l_Owner->SetPhaseMask(PHASEMASK_NORMAL, true);

                l_Owner->AddDelayedEvent([l_Owner]() -> void
                {
                    l_Owner->CastSpell(l_Owner, eSpells::SpellPhysicRealmCosmetic, true);
                }, 100);

            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_zuraal_void_realm_AuraScript::HandleOnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectApplyFn(spell_zuraal_void_realm_AuraScript::HandleOnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_zuraal_void_realm_AuraScript();
        }
};

/// Void Tear - 244621
class spell_zuraal_void_tear : public SpellScriptLoader
{
    public:
        spell_zuraal_void_tear() : SpellScriptLoader("spell_zuraal_void_tear")
        {}

        struct spell_zuraal_void_tear_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_zuraal_void_tear_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty() ||
                    GetCaster() == nullptr)
                    return;

                p_Targets.remove_if([] (WorldObject*& p_Itr)
                {
                    return p_Itr->GetEntry() != eCreatures::BossZuraalTheAscended;
                });

                if (p_Targets.empty())
                {
                    InstanceScript* l_Instance = GetCaster()->GetInstanceScript();

                    if (l_Instance == nullptr)
                        return;

                    if (Unit* l_Boss = sObjectAccessor->GetUnit(*GetCaster(), l_Instance->GetData64(eCreatures::BossZuraalTheAscended)))
                        p_Targets.push_back(l_Boss);
                }
            }

            void HandleAfterCast()
            {
                if (GetCaster() == nullptr)
                    return;

                Unit* l_Caster = GetCaster();

                InstanceScript* l_Instance = l_Caster->GetInstanceScript();

                if (l_Instance != nullptr)
                {
                    Creature* l_Urjad = sObjectAccessor->FindCreature(l_Instance->GetData64(eCreatures::NpcUrjad));

                    if (l_Urjad != nullptr)
                        l_Urjad->RemoveAurasDueToSpell(eSpells::SpellVoidRealm);
                }

                l_Caster->RemoveAurasDueToSpell(eSpells::SpellVoidRealm);
                l_Caster->RemoveAurasDueToSpell(eSpells::SpellVoidRealm2);
                l_Caster->RemoveAurasDueToSpell(eSpells::SpellVoid);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_zuraal_void_tear_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_zuraal_void_tear_SpellScript::HandleTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_zuraal_void_tear_SpellScript::HandleTargets, EFFECT_2, TARGET_UNIT_DEST_AREA_ENEMY);
                AfterCast += SpellCastFn(spell_zuraal_void_tear_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_zuraal_void_tear_SpellScript();
        }
};

/// Null Palm - 246134
class spell_zuraal_null_palm : public SpellScriptLoader
{
    public:
        spell_zuraal_null_palm() : SpellScriptLoader("spell_zuraal_null_palm")
        {}

        class spell_zuraal_null_palm_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_zuraal_null_palm_SpellScript);

            void HandleOnCast()
            {
                if (GetCaster() == nullptr)
                    return;

                Unit* l_Caster = GetCaster();

                Position l_RightEdge, l_LeftEdge, l_Mid;

                l_Caster->GetFirstCollisionPosition(l_LeftEdge, 30, float(M_PI) / 9.f);
                l_Caster->GetFirstCollisionPosition(l_RightEdge, 30, -float(M_PI) / 9.f);
                l_Caster->GetFirstCollisionPosition(l_Mid, 30, 0);

                G3D::Vector3 l_Start = l_Caster->GetPosition().AsVector3();
                G3D::Vector3 l_FinalMid = l_Mid.AsVector3();
                G3D::Vector3 l_RightLimit = l_RightEdge.AsVector3();
                G3D::Vector3 l_LeftLimit = l_LeftEdge.AsVector3();

                for (uint8 l_Itr = 2; l_Itr < 10; ++l_Itr)
                {
                    G3D::Vector3 l_Dir = l_FinalMid - l_Start;
                    G3D::Vector3 l_Tgt = (l_Start + (l_Dir * (l_Itr / 10.f)));

                    l_Caster->CastSpell(l_Tgt, eSpells::SpellNullPalmMissile, true);
                }

                for (uint8 l_Itr = 2; l_Itr < 10; ++l_Itr)
                {
                    G3D::Vector3 l_Dir = l_RightLimit - l_Start;
                    G3D::Vector3 l_Tgt = (l_Start + (l_Dir * (l_Itr / 10.f)));

                    l_Caster->CastSpell(l_Tgt, eSpells::SpellNullPalmMissile, true);
                }

                for (uint8 l_Itr = 2; l_Itr < 10; ++l_Itr)
                {
                    G3D::Vector3 l_Dir = l_LeftLimit - l_Start;
                    G3D::Vector3 l_Tgt = (l_Start + (l_Dir * (l_Itr / 10.f)));

                    l_Caster->CastSpell(l_Tgt, eSpells::SpellNullPalmMissile, true);
                }
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_zuraal_null_palm_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript()  const override
        {
            return new spell_zuraal_null_palm_SpellScript();
        }
};

/// Umbra Shift AOE - called by 247576
class spell_zuraal_umbra_shift : public SpellScriptLoader
{
    public:
        spell_zuraal_umbra_shift() : SpellScriptLoader("spell_zuraal_umbra_shift")
        {}

        class spell_zuraal_umbra_shift_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_zuraal_umbra_shift_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if([] (WorldObject*& p_Itr) -> bool
                {
                    Player* l_Target = p_Itr->ToPlayer();
                    if (l_Target->IsHealer() || l_Target->isDead())
                        return true;
                    
                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_zuraal_umbra_shift_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_zuraal_umbra_shift_SpellScript();
        }
};

/// Fixate AOE - 244653
class spell_zuraal_fixate : public SpellScriptLoader
{
    public:
        spell_zuraal_fixate() : SpellScriptLoader("spell_zuraal_fixate")
        {}

        class spell_zuraal_fixate_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_zuraal_fixate_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                JadeCore::UnitAuraCheck l_Cmp(true, eSpells::SpellVoidRealm);

                p_Targets.remove_if(l_Cmp);

                p_Targets.remove_if([](WorldObject*& p_Itr) -> bool
                {
                    if (Player* l_Target = p_Itr->ToPlayer())
                        return l_Target->IsHealer();

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_zuraal_fixate_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_zuraal_fixate_SpellScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_zuraal_the_ascended()
{
    /// Creatures
    new boss_zuraal_the_ascended();
    new npc_zuraal_dark_aberration();
    new npc_zuraal_coalesced_void();
    new npc_zuraal_urjad();

    /// Spells
    new spell_zuraal_umbra_shift();
    new spell_zuraal_null_palm();
    new spell_zuraal_void_realm();
    new spell_zuraal_void_tear();
    new spell_zuraal_fixate();
}
#endif
