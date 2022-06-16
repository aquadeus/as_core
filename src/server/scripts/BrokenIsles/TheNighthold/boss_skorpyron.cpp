////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "the_nighthold.hpp"

enum eSpells
{
    SpellEnergize                   = 204277,
    SpellFocusedBlast               = 204468,
    SpellFocusedBlastAT             = 204471,
    SpellCallScorpid                = 204371,
    SpellCallOfTheScorpid           = 210172,
    SpellShockwave                  = 204316,
    SpellExoskeleton                = 204448,
    SpellExoskeletalVulnerability   = 204459,
    SpellInfusedExoskeleton         = 205947,
    SpellArcanoslashDummy           = 205200,
    SpellFragmentsFilter            = 209696,
    SpellCrystallFragments          = 204509,
    SpellVolatileFragments          = 214663,
    SpellVolatileResonance          = 226229,
    SpellAcidicFragments            = 214657,
    SpellAcidicFragmentsDoT         = 214718,
    SpellRemoveAcidic               = 204381,

    SpellBlueTransform              = 215055,
    SpellRedTransform               = 214800,
    SpellGreenTransform             = 215042,

    SpellArcaneTether               = 204378,
    SpellArcaneTetherAura           = 204531,
    SpellArcaneTetherLink           = 204529,
    SpellArcaneTetherVisual         = 204373,
    SpellArcaneTetherOwnerVisual    = 204529,

    /// Summons
    SpellAvailable                  = 228943,
    SpellBlueShrouded               = 205900,
    SpellRedShrouded                = 212229,
    SpellGreenShrouded              = 212230,
    SpellBurrow                     = 204732,
    SpellEnergySurge                = 204766,
    SpellBoonScorpid                = 205289,
    SpellScorpidSwarmAT             = 204697,
    SpellScorpidSwarmBuff           = 204698,
    SpellVolatileChitinVis          = 210165,
    SpellVolatileChitinDmg          = 204733,
    SpellToxicChitinAT              = 204742,
    SpellChitinousExoskeletonScale  = 204975,

    SpellCrystallFragmentsAT        = 204306,
    SpellVolatileFragmentsAT        = 214661,
    SpellAcidicFragmentsAT          = 214652,
    SpellLOSBlocker                 = 204289,
    SpellBrokenShardAura            = 204284
};

enum eEvents
{
    EventFocusedBlast = 1,
    EventCallScorpid,
    EventVulnerability,
    EventArcanoslash,
    EventShockwave,
    EventBerserker
};

enum eSpellVisuals
{
    VisualBlast1    = 53760,
    VisualBlast2    = 53762,
    VisualBlast3    = 53763,
    VisualBlast4    = 53780,

    VisualFocused1  = 55185,
    VisualFocused2  = 55188,

    VisualKitBlast  = 67693
};

enum eColors
{
    BlueMode,
    RedMode,
    GreenMode,

    MaxColors
};

enum eTalks
{
    TalkBlast,
    TalkShockwave
};

static std::map<uint32, std::array<uint32, eColors::MaxColors>> g_ColorDisplayIDs =
{
    { eCreatures::NpcCrystalSpine1,     {{ 68262, 69446, 67106 }} },
    { eCreatures::NpcCrystalSpine2,     {{ 68261, 69445, 67107 }} },
    { eCreatures::NpcCrystalSpine3,     {{ 68260, 69444, 67108 }} },
    { eCreatures::NpcCrystalTail,       {{ 67103, 69452, 69451 }} },
    { eCreatures::NpcCrystalMandible1,  {{ 67104, 69450, 69449 }} },
    { eCreatures::NpcCrystalMandible2,  {{ 67105, 69448, 69447 }} }
};

struct ColorEvent
{
    uint32 Time;
    uint32 Color;
};

static std::vector<ColorEvent> g_ColorEvents =
{
    { 22 * TimeConstants::IN_MILLISECONDS, eColors::RedMode     },
    { 45 * TimeConstants::IN_MILLISECONDS, eColors::GreenMode   },
    { 45 * TimeConstants::IN_MILLISECONDS, eColors::BlueMode    },
    { 45 * TimeConstants::IN_MILLISECONDS, eColors::RedMode     },
    { 45 * TimeConstants::IN_MILLISECONDS, eColors::GreenMode   },
    { 45 * TimeConstants::IN_MILLISECONDS, eColors::BlueMode    },
    { 45 * TimeConstants::IN_MILLISECONDS, eColors::RedMode     },
    { 45 * TimeConstants::IN_MILLISECONDS, eColors::GreenMode   },
    { 45 * TimeConstants::IN_MILLISECONDS, eColors::BlueMode    },
    { 45 * TimeConstants::IN_MILLISECONDS, eColors::RedMode     },
    { 45 * TimeConstants::IN_MILLISECONDS, eColors::GreenMode   },
    { 45 * TimeConstants::IN_MILLISECONDS, eColors::BlueMode    },
    { 45 * TimeConstants::IN_MILLISECONDS, eColors::RedMode     }
};

Position const g_CenterPos = { 91.798f, 3442.907f, -250.1464f, 2.6278f };

/// Skorpyron - 102263
class boss_skorpyron : public CreatureScript
{
    public:
        boss_skorpyron() : CreatureScript("boss_skorpyron") { }

        struct boss_skorpyronAI : BossAI
        {
            boss_skorpyronAI(Creature* p_Creature) : BossAI(p_Creature, eData::DataSkorpyron), m_Vehicle(p_Creature->GetVehicleKit())
            {
                m_IntroDone = false;
            }

            uint8 m_ColorMod;
            uint32 m_DmgCounter;
            uint64 m_HealthLimit;

            bool m_IntroDone;

            Vehicle* m_Vehicle;

            std::array<uint64, 3> m_CrystalSpineGuids;

            uint32 m_AchievCheckTimer = 0;

            void Reset() override
            {
                _Reset();

                me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                me->SetPower(Powers::POWER_ENERGY, 0);

                me->RemoveAurasDueToSpell(eSpells::SpellExoskeleton);
                me->RemoveAurasDueToSpell(eSpells::SpellArcanoslashDummy);
                me->RemoveAurasDueToSpell(eSpells::SpellInfusedExoskeleton);
                me->RemoveAurasDueToSpell(eSpells::SpellBlueTransform);
                me->RemoveAurasDueToSpell(eSpells::SpellRedTransform);
                me->RemoveAurasDueToSpell(eSpells::SpellGreenTransform);
                me->RemoveAurasDueToSpell(eSharedSpells::SpellBerserk);

                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                m_DmgCounter = 0;
                m_HealthLimit = me->CountPctFromMaxHealth(2.5f);
                SetColor(eColors::BlueMode);

                /// Reset EligibilityWorldState value
                if (!IsLFR())
                {
                    if (InstanceScript* l_Instance = me->GetInstanceScript())
                        l_Instance->SetData(eData::DataSkorpyronAchiev, true);
                }

                AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    std::vector<uint32> l_Entries =
                    {
                        eCreatures::NpcCrystalTail,
                        eCreatures::NpcCrystalSpine1,
                        eCreatures::NpcCrystalMandible1,
                        eCreatures::NpcCrystalMandible2,
                        eCreatures::NpcCrystalSpine3,
                        eCreatures::NpcCrystalSpine2
                    };

                    int8 l_SeatID = 0;

                    for (uint32& l_Entry : l_Entries)
                    {
                        if (Creature* l_Creature = me->SummonCreature(l_Entry, me->GetPosition()))
                            l_Creature->EnterVehicle(me, l_SeatID++);
                    }
                });
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                _EnterCombat();

                if (!IsLFR())
                    m_AchievCheckTimer = 500;

                DoCast(me, eSpells::SpellEnergize, true);
                DoCast(me, eSpells::SpellExoskeleton, true);

                CallScorpions(true, 0);

                events.ScheduleEvent(eEvents::EventFocusedBlast, 13 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventCallScorpid, 20 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventArcanoslash, 5 * TimeConstants::IN_MILLISECONDS);

                if (IsHeroicOrMythic())
                    events.ScheduleEvent(eEvents::EventBerserker, 542 * TimeConstants::IN_MILLISECONDS);

                if (IsMythic())
                {
                    uint32 l_Time = 0;

                    for (ColorEvent& l_Event : g_ColorEvents)
                    {
                        l_Time += l_Event.Time;

                        uint32 l_Color = l_Event.Color;
                        AddTimedDelayedOperation(l_Time, [this, l_Color]() -> void
                        {
                            SetColor(eColors(l_Color));
                        });
                    }
                }
            }

            void EnterEvadeMode() override
            {
                BossAI::EnterEvadeMode();

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellArcaneTetherAura);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellAcidicFragmentsDoT);
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                _JustDied();

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellArcaneTetherAura);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellAcidicFragmentsDoT);

                    /// For next event. We need use it here, because event can be play not every time
                    if (GameObject* l_Gob = GameObject::GetGameObject(*me, instance->GetData64(eGameObjects::GoAnomalyPre)))
                        l_Gob->SetByteValue(GAMEOBJECT_FIELD_BYTES_1, 0, 1);
                }
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (p_Who->IsPlayer() && !m_IntroDone && me->IsWithinDistInMap(p_Who, 60.0f))
                {
                    m_IntroDone = true;

                    if (instance)
                        instance->SetData(eData::DataSkorpyronIntro, 1);
                }

                if (me->getVictim())
                    return;

                if (me->GetCreatureType() == CreatureType::CREATURE_TYPE_NON_COMBAT_PET)
                    return;

                if (me->IsWithinDistInMap(p_Who, 6.0f))
                {
                    if (me->canStartAttack(p_Who, false))
                        AttackStart(p_Who);
                }
            }

            void SetData(uint32 p_Type, uint32 p_Data) override
            {
                if (p_Type == eData::DataSkorpyronPath)
                    CallScorpions(false, p_Data);
            }

            uint64 GetGUID(int32 p_Type) override
            {
                switch (p_Type)
                {
                    case eCreatures::NpcCrystalSpine1:
                        return m_CrystalSpineGuids[0];
                    case eCreatures::NpcCrystalSpine2:
                        return m_CrystalSpineGuids[1];
                    case eCreatures::NpcCrystalSpine3:
                        return m_CrystalSpineGuids[2];
                    default:
                        break;
                }

                return 0;
            }

            void CallScorpions(bool p_All, uint32 p_PathID)
            {
                uint32 l_Entry;

                if (p_All)
                {
                    for (uint8 l_I = 0; l_I < 32; l_I++)
                    {
                        if (IsHeroicOrMythic() && (l_I == 0 || l_I == 8 || l_I == 16 || l_I == 24))
                            l_Entry = eCreatures::NpcVolatileScorpid;
                        else if (IsMythic() && (l_I == 4 || l_I == 12 || l_I == 20 || l_I == 28))
                            l_Entry = eCreatures::NpcAcidmawScorpid;
                        else
                            l_Entry = eCreatures::NpcCrystallineScorpid;

                        if (Creature* l_Scorpion = me->SummonCreature(l_Entry, 121.33f + frand(-0.3f, 0.3f), 3418.02f + frand(-0.3f, 0.3f), -250.12f, 2.36f))
                        {
                            l_Scorpion->GetMotionMaster()->MovePath(10226300 + l_I, false);

                            if (l_Scorpion->IsAIEnabled)
                                l_Scorpion->AI()->SetData(eData::DataSkorpyronPath, 10226300 + l_I);
                        }
                    }
                }
                else
                {
                    if (IsHeroicOrMythic() && (p_PathID == 10226300 || p_PathID == 10226308 || p_PathID == 10226316 || p_PathID == 10226324))
                        l_Entry = eCreatures::NpcVolatileScorpid;
                    else if (IsMythic() && (p_PathID == 10226304 || p_PathID == 102263012 || p_PathID == 10226320 || p_PathID == 10226328))
                        l_Entry = eCreatures::NpcAcidmawScorpid;
                    else
                        l_Entry = eCreatures::NpcCrystallineScorpid;

                    AddTimedDelayedOperation(20 * TimeConstants::IN_MILLISECONDS, [this, l_Entry, p_PathID]() -> void
                    {
                        if (me->isAlive() && me->isInCombat())
                        {
                            if (Creature* l_Scorpion = me->SummonCreature(l_Entry, 121.33f + frand(-0.3f, 0.3f), 3418.02f + frand(-0.3f, 0.3f), -250.12f, 2.36f))
                            {
                                l_Scorpion->GetMotionMaster()->MovePath(p_PathID, false);

                                if (l_Scorpion->IsAIEnabled)
                                    l_Scorpion->AI()->SetData(eData::DataSkorpyronPath, p_PathID);
                            }
                        }
                    });
                }
            }

            void SummonedCreatureDies(Creature* p_Summon, Unit* /*p_Killer*/) override
            {
                p_Summon->DespawnOrUnsummon(3 * TimeConstants::IN_MILLISECONDS);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellFocusedBlast:
                    {
                        me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                        me->CastSpell(me, eSpells::SpellFocusedBlastAT, false);
                        Talk(eTalks::TalkBlast);
                        break;
                    }
                    case eSpells::SpellFragmentsFilter:
                    {
                        switch (m_ColorMod)
                        {
                            case eColors::RedMode:
                            {
                                DoCast(p_Target, eSpells::SpellVolatileFragments, true);
                                break;
                            }
                            case eColors::GreenMode:
                            {
                                DoCast(p_Target, eSpells::SpellAcidicFragments, true);
                                break;
                            }
                            default:
                            {
                                DoCast(p_Target, eSpells::SpellCrystallFragments, true);
                                break;
                            }
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SpellFocusedBlastAT:
                    {
                        me->SendPlaySpellVisualKit(0, eSpellVisuals::VisualKitBlast);

                        float l_Z = me->GetPositionZ();
                        float l_LineDist = 2.5f;
                        uint8 l_CountLine = 24;
                        uint8 l_CountPointInLine = 2;
                        Position l_FirstPos;

                        for (uint8 l_I = 1; l_I <= l_CountLine; l_I++)
                        {
                            me->GetNearPosition(l_FirstPos, l_I * 2.5f, 0.0f);

                            SendVisual(l_FirstPos.GetPositionX(), l_FirstPos.GetPositionY(), l_FirstPos.GetPositionZ(), l_I);

                            for (uint8 l_J = 1; l_J <= l_CountPointInLine; l_J++)
                            {
                                float l_XLeft = l_FirstPos.GetPositionX() + l_J * l_LineDist * std::cos(me->GetOrientation() + 1.5f);
                                float l_YLeft = l_FirstPos.GetPositionY() + l_J * l_LineDist * std::sin(me->GetOrientation() + 1.5f);

                                JadeCore::NormalizeMapCoord(l_XLeft);
                                JadeCore::NormalizeMapCoord(l_YLeft);

                                SendVisual(l_XLeft, l_YLeft, l_Z, l_I);

                                float l_XRight = l_FirstPos.GetPositionX() + l_J * l_LineDist * std::cos(me->GetOrientation() - 1.5f);
                                float l_YRight = l_FirstPos.GetPositionY() + l_J * l_LineDist * std::sin(me->GetOrientation() - 1.5f);

                                JadeCore::NormalizeMapCoord(l_XRight);
                                JadeCore::NormalizeMapCoord(l_YRight);

                                SendVisual(l_XRight, l_YRight, l_Z, l_I);
                            }
                        }

                        AddTimedDelayedOperation(10, [this]() -> void
                        {
                            me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                        });

                        break;
                    }
                    case eSpells::SpellShockwave:
                    {
                        AddTimedDelayedOperation(200, [this]() -> void
                        {
                            summons.DespawnEntry(eCreatures::NpcCrystallineShard);
                            summons.DespawnEntry(eCreatures::NpcAcidicShard);

                            /// Volatile Shards doesn't despawn instantly, they explode after 3 seconds
                            EntryCheckPredicate l_Pred(eCreatures::NpcVolatileShard);
                            summons.DoAction(c_actions::ACTION_1, l_Pred);

                            DoCast(me, eSpells::SpellRemoveAcidic, true);
                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            void SendVisual(float p_X, float p_Y, float p_Z, uint8 p_IDx)
            {
                Position l_TargetPos = { p_X, p_Y, p_Z, 0.0f };

                AddTimedDelayedOperation(40 * p_IDx, [this, l_TargetPos]() -> void
                {
                    uint32 l_Visual = RAND(eSpellVisuals::VisualBlast1, eSpellVisuals::VisualBlast2, eSpellVisuals::VisualBlast3, eSpellVisuals::VisualBlast4);

                    me->SendPlaySpellVisual(l_Visual, nullptr, 1.0f, l_TargetPos, true);
                });
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (me->HasAura(eSpells::SpellExoskeleton))
                {
                    /// Fragment events
                    m_DmgCounter += p_Damage;

                    if (m_DmgCounter >= m_HealthLimit)
                    {
                        m_DmgCounter = 0;
                        DoCast(me, eSpells::SpellFragmentsFilter, true);
                    }
                }
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case c_actions::ACTION_1:
                    {
                        me->RemoveAurasDueToSpell(eSpells::SpellArcanoslashDummy);
                        events.ScheduleEvent(eEvents::EventVulnerability, 1);
                        break;
                    }
                    case c_actions::ACTION_2:
                    {
                        events.ScheduleEvent(eEvents::EventShockwave, 1);
                        break;
                    }
                    default:
                        break;
                }
            }

            void PassengerBoarded(Unit* p_Passenger, int8 /*p_SeatID*/, bool p_Apply) override
            {
                if (p_Apply)
                {
                    switch (p_Passenger->GetEntry())
                    {
                        case eCreatures::NpcCrystalSpine1:
                        {
                            m_CrystalSpineGuids[0] = p_Passenger->GetGUID();
                            break;
                        }
                        case eCreatures::NpcCrystalSpine2:
                        {
                            m_CrystalSpineGuids[1] = p_Passenger->GetGUID();
                            break;
                        }
                        case eCreatures::NpcCrystalSpine3:
                        {
                            m_CrystalSpineGuids[2] = p_Passenger->GetGUID();
                            break;
                        }
                        default:
                            return;
                    }

                    for (uint8 l_I = 0; l_I < 6; ++l_I)
                        p_Passenger->CastSpell(p_Passenger, eSpells::SpellChitinousExoskeletonScale, true);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                if (m_AchievCheckTimer)
                {
                    if (m_AchievCheckTimer <= p_Diff)
                    {
                        m_AchievCheckTimer = 500;

                        std::list<HostileReference*> l_ThreatList = me->getThreatManager().getThreatList();
                        for (HostileReference* l_Ref : l_ThreatList)
                        {
                            if (Unit* l_Target = l_Ref->getTarget())
                            {
                                if (!l_Target->IsPlayer())
                                    continue;

                                /// If achievement is fail, no need to continue checking
                                if (g_CenterPos.GetExactDist(l_Target) > 25.0f)
                                {
                                    /// Set EligibilityWorldState to 0 for client
                                    if (InstanceScript* l_Instance = me->GetInstanceScript())
                                        l_Instance->SetData(eData::DataSkorpyronAchiev, false);

                                    m_AchievCheckTimer = 0;
                                    break;
                                }
                            }
                        }
                    }
                    else
                        m_AchievCheckTimer -= p_Diff;
                }

                if (me->GetDistance(g_CenterPos) > 40.0f)
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING | UnitState::UNIT_STATE_STUNNED))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventFocusedBlast:
                    {
                        me->RemoveAurasDueToSpell(eSpells::SpellArcanoslashDummy);

                        if (Player* l_Target = SelectPlayerTarget(eTargetTypeMask::TypeMaskNonTank))
                            me->SetFacingTo(me->GetAngle(l_Target));

                        DoCast(me, eSpells::SpellFocusedBlast, true);

                        if (m_Vehicle != nullptr)
                        {
                            uint32 l_Time = 0;
                            for (uint8 l_I = 0; l_I < 12; ++l_I)
                            {
                                if (l_Time)
                                {
                                    AddTimedDelayedOperation(l_Time, [this]() -> void
                                    {
                                        if (Unit* l_Tail = m_Vehicle->GetPassenger(0))
                                        {
                                            me->SendPlaySpellVisual(eSpellVisuals::VisualFocused1, l_Tail, 0.25f, Position(), true);
                                            me->SendPlaySpellVisual(eSpellVisuals::VisualFocused2, l_Tail, 0.25f, Position(), true);
                                        }
                                    });
                                }
                                else
                                {
                                    if (Unit* l_Tail = m_Vehicle->GetPassenger(0))
                                    {
                                        me->SendPlaySpellVisual(eSpellVisuals::VisualFocused1, l_Tail, 0.25f, Position(), true);
                                        me->SendPlaySpellVisual(eSpellVisuals::VisualFocused2, l_Tail, 0.25f, Position(), true);
                                    }
                                }

                                l_Time += 200;
                            }
                        }

                        events.ScheduleEvent(eEvents::EventFocusedBlast, 30 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventCallScorpid:
                    {
                        DoCast(me, eSpells::SpellCallScorpid, true);
                        events.ScheduleEvent(eEvents::EventCallScorpid, 20 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventVulnerability:
                    {
                        DoCast(eSpells::SpellExoskeletalVulnerability);

                        /// This will prevent any other cast from interrupting the next one (Spell Exoskeleton)
                        me->AddUnitState(UnitState::UNIT_STATE_CASTING);

                        AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            DoCast(eSpells::SpellExoskeleton);

                            for (uint64& l_Guid : m_CrystalSpineGuids)
                            {
                                if (Creature* l_Spine = Creature::GetCreature(*me, l_Guid))
                                {
                                    l_Spine->SetUInt32Value(EUnitFields::UNIT_FIELD_SCALE_DURATION, 2000);
                                    l_Spine->RemoveAura(eSpells::SpellChitinousExoskeletonScale);
                                }
                            }

                            if (Creature* l_Spine = Creature::GetCreature(*me, m_CrystalSpineGuids[2]))
                            {
                                for (uint8 l_I = 0; l_I < 6; ++l_I)
                                    l_Spine->CastSpell(l_Spine, eSpells::SpellChitinousExoskeletonScale, true);
                            }

                            AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                if (Creature* l_Spine = Creature::GetCreature(*me, m_CrystalSpineGuids[1]))
                                {
                                    for (uint8 l_I = 0; l_I < 6; ++l_I)
                                        l_Spine->CastSpell(l_Spine, eSpells::SpellChitinousExoskeletonScale, true);
                                }

                                AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                                {
                                    if (Creature* l_Spine = Creature::GetCreature(*me, m_CrystalSpineGuids[0]))
                                    {
                                        for (uint8 l_I = 0; l_I < 6; ++l_I)
                                            l_Spine->CastSpell(l_Spine, eSpells::SpellChitinousExoskeletonScale, true);
                                    }
                                });
                            });
                        });

                        break;
                    }
                    case eEvents::EventArcanoslash:
                    {
                        DoCast(me, eSpells::SpellArcanoslashDummy, true);
                        events.ScheduleEvent(eEvents::EventArcanoslash, 10 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventShockwave:
                    {
                        Talk(eTalks::TalkShockwave);
                        DoCast(eSpells::SpellShockwave);
                        break;
                    }
                    case eEvents::EventBerserker:
                    {
                        DoCast(me, eSharedSpells::SpellBerserk, true);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

            void SetColor(eColors p_Color)
            {
                m_ColorMod = p_Color;

                if (m_Vehicle == nullptr || m_ColorMod >= eColors::MaxColors)
                    return;

                for (int8 l_I = 0; l_I < MAX_VEHICLE_SEATS; ++l_I)
                {
                    if (Unit* l_Passenger = m_Vehicle->GetPassenger(l_I))
                    {
                        auto const& l_Iter = g_ColorDisplayIDs.find(l_Passenger->GetEntry());

                        if (l_Iter == g_ColorDisplayIDs.end())
                            continue;

                        l_Passenger->SetDisplayId(l_Iter->second[m_ColorMod]);
                    }
                }

                if (!IsMythic())
                    return;

                switch (m_ColorMod)
                {
                    case eColors::RedMode:
                    {
                        me->RemoveAurasDueToSpell(eSpells::SpellBlueTransform);
                        DoCast(me, eSpells::SpellRedTransform, true);
                        break;
                    }
                    case eColors::GreenMode:
                    {
                        summons.DespawnEntry(eCreatures::NpcVolatileShard);

                        me->RemoveAurasDueToSpell(eSpells::SpellRedTransform);
                        DoCast(me, eSpells::SpellGreenTransform, true);
                        break;
                    }
                    default:
                    {
                        summons.DespawnEntry(eCreatures::NpcVolatileShard);

                        me->RemoveAurasDueToSpell(eSpells::SpellGreenTransform);
                        DoCast(me, eSpells::SpellBlueTransform, true);
                        break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_skorpyronAI(p_Creature);
        }
};

/// Crystalline Scorpid - 103217
/// Volatile Scorpid - 103224
/// Acidmaw Scorpid - 103225
class npc_skorpyron_generic_scorp : public CreatureScript
{
    public:
        npc_skorpyron_generic_scorp() : CreatureScript("npc_skorpyron_generic_scorp") { }

        struct npc_skorpyron_generic_scorpAI : public ScriptedAI
        {
            npc_skorpyron_generic_scorpAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_ATTACKABLE_1);
            }

            uint32 m_DataPath;
            bool m_Active;

            uint32 m_CheckScorpidTimer = 0;

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                m_Active = false;

                switch (me->GetEntry())
                {
                    case eCreatures::NpcCrystallineScorpid:
                    {
                        DoCast(me, eSpells::SpellBlueShrouded, true);
                        break;
                    }
                    case eCreatures::NpcVolatileScorpid:
                    {
                        DoCast(me, eSpells::SpellRedShrouded, true);
                        break;
                    }
                    case eCreatures::NpcAcidmawScorpid:
                    {
                        DoCast(me, eSpells::SpellGreenShrouded, true);
                        break;
                    }
                    default:
                        break;
                }

                events.ScheduleEvent(c_events::EVENT_1, 1 * TimeConstants::IN_MILLISECONDS);
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                switch (me->GetEntry())
                {
                    case eCreatures::NpcCrystallineScorpid:
                    {
                        events.ScheduleEvent(c_events::EVENT_2, 5 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eCreatures::NpcAcidmawScorpid:
                    {
                        events.ScheduleEvent(c_events::EVENT_4, 6 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                events.ScheduleEvent(c_events::EVENT_3, 12 * TimeConstants::IN_MILLISECONDS);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                switch (me->GetEntry())
                {
                    case eCreatures::NpcVolatileScorpid:
                    {
                        DoCast(me, eSpells::SpellVolatileChitinDmg, true);
                        break;
                    }
                    case eCreatures::NpcAcidmawScorpid:
                    {
                        if (Unit* l_Owner = me->GetAnyOwner())
                            me->CastSpell(me, eSpells::SpellToxicChitinAT, true, nullptr, nullptr, l_Owner->GetGUID());

                        break;
                    }
                    default:
                        break;
                }
            }

            void LastWPReached() override
            {
                switch (m_DataPath)
                {
                    case 10226300:
                    case 10226304:
                    case 10226308:
                    case 10226312:
                    case 10226316:
                    case 10226320:
                    case 10226324:
                        DoCast(me, eSpells::SpellAvailable, true);
                        break;
                    default:
                        break;
                }
            }

            void SetData(uint32 p_Type, uint32 p_Data) override
            {
                if (p_Type == eData::DataSkorpyronPath)
                    m_DataPath = p_Data;
            }

            void SpellHit(Unit* /*p_Caster*/, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SpellCallOfTheScorpid)
                    ScorpActive();
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == c_actions::ACTION_1 && !m_Active)
                {
                    m_Active = true;

                    if (me->GetAnyOwner())
                    {
                        if (Creature* l_Owner = me->GetAnyOwner()->ToCreature())
                        {
                            if (l_Owner->IsAIEnabled)
                                l_Owner->AI()->SetData(eData::DataSkorpyronPath, m_DataPath);
                        }
                    }

                    DoCast(me, eSpells::SpellBurrow, true);

                    /// Hack
                    {
                        me->AddUnitState(UnitState::UNIT_STATE_STUNNED);
                        me->m_Functions.AddFunction([this]() -> void
                        {
                            if (me && me->isAlive())
                                me->ClearUnitState(UnitState::UNIT_STATE_STUNNED);
                        }, me->m_Functions.CalculateTime(1500));
                    }

                    me->RemoveAurasDueToSpell(eSpells::SpellBlueShrouded);
                    me->RemoveAurasDueToSpell(eSpells::SpellRedShrouded);
                    me->RemoveAurasDueToSpell(eSpells::SpellGreenShrouded);

                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_ATTACKABLE_1);
                    me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                    DoZoneInCombat(me, 100.0f);

                    if (me->GetEntry() == eCreatures::NpcVolatileScorpid)
                    {
                        m_CheckScorpidTimer = 200;

                        DoCast(me, eSpells::SpellScorpidSwarmAT, true);
                    }
                }
            }

            void ScorpActive()
            {
                me->RemoveAurasDueToSpell(eSpells::SpellAvailable);

                std::list<Creature*> l_CreatureList;

                me->GetCreatureListWithEntryInGrid(l_CreatureList, eCreatures::NpcVolatileScorpid, 15.0f);
                me->GetCreatureListWithEntryInGrid(l_CreatureList, eCreatures::NpcAcidmawScorpid, 15.0f);
                me->GetCreatureListWithEntryInGrid(l_CreatureList, eCreatures::NpcCrystallineScorpid, 15.0f);

                if (l_CreatureList.size() > 5)
                {
                    l_CreatureList.sort(JadeCore::UnitSortDistance(true, me));
                    l_CreatureList.resize(5);
                }

                for (Creature* l_Scorpion : l_CreatureList)
                {
                    if (l_Scorpion->IsAIEnabled)
                        l_Scorpion->AI()->DoAction(c_actions::ACTION_1);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                if (m_CheckScorpidTimer)
                {
                    if (m_CheckScorpidTimer <= p_Diff)
                    {
                        std::list<Creature*> l_CreatureList;

                        me->GetCreatureListWithEntryInGrid(l_CreatureList, eCreatures::NpcVolatileScorpid, 15.0f);
                        me->GetCreatureListWithEntryInGrid(l_CreatureList, eCreatures::NpcAcidmawScorpid, 15.0f);
                        me->GetCreatureListWithEntryInGrid(l_CreatureList, eCreatures::NpcCrystallineScorpid, 15.0f);

                        if (!l_CreatureList.empty())
                            l_CreatureList.remove(me);

                        for (Creature*& l_Creature : l_CreatureList)
                        {
                            /// Already affected
                            if (l_Creature->HasAura(eSpells::SpellScorpidSwarmBuff, me->GetGUID()))
                                continue;

                            /// Affect scorpid
                            DoCast(l_Creature, eSpells::SpellScorpidSwarmBuff, true);
                        }

                        m_CheckScorpidTimer = 200;
                    }
                    else
                        m_CheckScorpidTimer -= p_Diff;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case c_events::EVENT_1:
                    {
                        if (!m_Active)
                        {
                            if (me->SelectNearestPlayerNotGM(5.0f))
                            {
                                ScorpActive();
                                break;
                            }

                            events.ScheduleEvent(c_events::EVENT_1, 1 * TimeConstants::IN_MILLISECONDS);
                        }

                        break;
                    }
                    case c_events::EVENT_2:
                    {
                        if (m_Active)
                            DoCastVictim(eSpells::SpellEnergySurge);

                        events.ScheduleEvent(c_events::EVENT_2, 5 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case c_events::EVENT_3:
                    {
                        if (m_Active)
                            DoCast(me, eSpells::SpellBoonScorpid, true);

                        events.ScheduleEvent(c_events::EVENT_3, 10 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case c_events::EVENT_4:
                    {
                        if (m_Active)
                        {
                            if (Unit* l_Owner = me->GetAnyOwner())
                                me->CastSpell(me, eSpells::SpellToxicChitinAT, true, nullptr, nullptr, l_Owner->GetGUID());
                        }

                        events.ScheduleEvent(c_events::EVENT_4, 10 * TimeConstants::IN_MILLISECONDS);
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
            return new npc_skorpyron_generic_scorpAI(p_Creature);
        }
};

/// Crystalline Shard - 103209
/// Volatile Shard - 108132
/// Acidic Shard - 108131
class npc_skorpyron_crystalline_fragments : public CreatureScript
{
    public:
        npc_skorpyron_crystalline_fragments() : CreatureScript("npc_skorpyron_crystalline_fragments") { }

        struct npc_skorpyron_crystalline_fragmentsAI : public Scripted_NoMovementAI
        {
            npc_skorpyron_crystalline_fragmentsAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature)
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            uint64 m_LOSBlocker = 0;

            void Reset() override { }

            void AreaTriggerCreated(AreaTrigger* p_AreaTrigger) override
            {
                if (p_AreaTrigger->GetSpellId() == eSpells::SpellCrystallFragmentsAT)
                    p_AreaTrigger->SetRadius(3.0f);
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == c_actions::ACTION_1 && me->GetEntry() == eCreatures::NpcVolatileShard)
                    DoCast(me, eSpells::SpellVolatileResonance);
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                switch (me->GetEntry())
                {
                    case eCreatures::NpcCrystallineShard:
                    {
                        DoCast(me, eSpells::SpellCrystallFragmentsAT, true);
                        break;
                    }
                    case eCreatures::NpcVolatileShard:
                    {
                        DoCast(me, eSpells::SpellVolatileFragmentsAT, true);
                        break;
                    }
                    case eCreatures::NpcAcidicShard:
                    {
                        DoCast(me, eSpells::SpellAcidicFragmentsAT, true);
                        DoCast(me, eSpells::SpellAcidicFragmentsDoT, true);
                        break;
                    }
                    default:
                        break;
                }

                DoCast(me, eSpells::SpellLOSBlocker, true);
            }

            void JustDespawned() override
            {
                if (GameObject* l_Gob = GameObject::GetGameObject(*me, m_LOSBlocker))
                    l_Gob->Delete();
            }

            void JustSummonedGO(GameObject* p_GameObject) override
            {
                p_GameObject->EnableCollision(false);

                m_LOSBlocker = p_GameObject->GetGUID();
            }

            void UpdateAI(uint32 const /*p_Diff*/) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_skorpyron_crystalline_fragmentsAI(p_Creature);
        }
};

/// Arcane Tether - 103682
class npc_skorpyron_arcane_tether : public CreatureScript
{
    public:
        npc_skorpyron_arcane_tether() : CreatureScript("npc_skorpyron_arcane_tether") { }

        struct npc_skorpyron_arcane_tetherAI : public Scripted_NoMovementAI
        {
            npc_skorpyron_arcane_tetherAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature), m_CheckOwnerTimer(0)
            {
                me->SetReactState(REACT_PASSIVE);
                m_Instance = me->GetInstanceScript();
            }

            InstanceScript* m_Instance;

            int32 m_CheckOwnerTimer;

            void Reset() override { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                DoCast(me, eSpells::SpellArcaneTetherVisual, true);
                DoCast(p_Summoner, eSpells::SpellArcaneTetherOwnerVisual, true);

                m_CheckOwnerTimer = 500;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (m_Instance && m_Instance->GetBossState(eData::DataSkorpyron) != EncounterState::IN_PROGRESS)
                {
                    me->DespawnOrUnsummon(1);
                    return;
                }

                if (m_CheckOwnerTimer <= p_Diff)
                {
                    /// If the tank enters the 10-yard radius around the Arcane Tether, the Arcane Tether will attach itself again
                    if (Unit* l_Owner = me->GetAnyOwner())
                    {
                        if (l_Owner->GetDistance(me) <= 10.0f && !l_Owner->HasAura(eSpells::SpellArcaneTetherOwnerVisual, me->GetGUID()))
                            DoCast(l_Owner, eSpells::SpellArcaneTetherOwnerVisual, true);
                    }

                    m_CheckOwnerTimer = 500;
                }
                else
                    m_CheckOwnerTimer -= p_Diff;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new npc_skorpyron_arcane_tetherAI(p_Creature);
        }
};

/// Power - 204277
class spell_skorpyron_periodic_energize : public SpellScriptLoader
{
    public:
        spell_skorpyron_periodic_energize() : SpellScriptLoader("spell_skorpyron_periodic_energize") { }

        class spell_skorpyron_periodic_energize_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_skorpyron_periodic_energize_AuraScript);

            uint8 m_PowerCount = 0;
            uint8 m_Counter = 0;

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                if (!GetCaster())
                    return;

                Creature* l_Caster = GetCaster()->ToCreature();
                if (!l_Caster || !l_Caster->isInCombat())
                    return;

                m_PowerCount = l_Caster->GetPower(Powers::POWER_ENERGY);

                if (m_PowerCount < 100)
                {
                    l_Caster->SetPower(Powers::POWER_ENERGY, m_PowerCount + (m_Counter ? 2 : 1));
                    m_Counter < 4 ? m_Counter++ : m_Counter = 0;
                }
                else if (!l_Caster->HasUnitState(UnitState::UNIT_STATE_CASTING | UnitState::UNIT_STATE_STUNNED))
                {
                    if (l_Caster->IsAIEnabled)
                        l_Caster->AI()->DoAction(c_actions::ACTION_2);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_skorpyron_periodic_energize_AuraScript::OnTick, SpellEffIndex::EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_skorpyron_periodic_energize_AuraScript();
        }
};

/// Chitinous Exoskeleton - 204448
class spell_skorpyron_chitinous_exoskeleton : public SpellScriptLoader
{
    public:
        spell_skorpyron_chitinous_exoskeleton() : SpellScriptLoader("spell_skorpyron_chitinous_exoskeleton") { }

        class spell_skorpyron_chitinous_exoskeleton_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_skorpyron_chitinous_exoskeleton_AuraScript);

            int8 m_Stack = 0;
            uint8 m_HealthPct = 100;
            uint8 m_HealthLimit;
            uint16 m_CheckTimer = 1000;

            void AfterApply(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->ToCreature())
                    return;

                if (Aura* l_Aura = p_AurEff->GetBase())
                    l_Aura->SetStackAmount(GetSpellInfo()->Effects[SpellEffIndex::EFFECT_1].BasePoints);

                m_HealthLimit = l_Caster->GetHealthPct();

                if (m_HealthLimit != 100 && !l_Caster->GetMap()->IsLFR())
                    l_Caster->CastSpell(l_Caster, eSpells::SpellInfusedExoskeleton, true);

                if (Creature* l_Boss = l_Caster->ToCreature())
                {
                    if (!l_Boss->IsAIEnabled)
                        return;

                    if (Creature* l_Spine = Creature::GetCreature(*l_Caster, l_Boss->AI()->GetGUID(eCreatures::NpcCrystalSpine3)))
                        l_Spine->SetUInt32Value(EUnitFields::UNIT_FIELD_SCALE_DURATION, 100);

                    if (Creature* l_Spine = Creature::GetCreature(*l_Caster, l_Boss->AI()->GetGUID(eCreatures::NpcCrystalSpine2)))
                        l_Spine->SetUInt32Value(EUnitFields::UNIT_FIELD_SCALE_DURATION, 100);

                    if (Creature* l_Spine = Creature::GetCreature(*l_Caster, l_Boss->AI()->GetGUID(eCreatures::NpcCrystalSpine1)))
                        l_Spine->SetUInt32Value(EUnitFields::UNIT_FIELD_SCALE_DURATION, 100);
                }
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (!GetCaster())
                    return;

                if (Creature* l_Caster = GetCaster()->ToCreature())
                {
                    if (l_Caster->IsAIEnabled)
                        l_Caster->AI()->DoAction(c_actions::ACTION_1);
                }
            }

            void OnUpdate(uint32 p_Diff, AuraEffect* p_AurEff)
            {
                if (m_CheckTimer <= p_Diff)
                {
                    if (!GetCaster())
                        return;

                    m_CheckTimer = 1 * TimeConstants::IN_MILLISECONDS;

                    Creature* l_Caster = GetCaster()->ToCreature();
                    if (!l_Caster || !l_Caster->isInCombat() || !l_Caster->IsAIEnabled)
                        return;

                    if (uint8(ceilf(l_Caster->GetHealthPct())) < m_HealthPct)
                    {
                        if (Aura* l_Aura = p_AurEff->GetBase())
                        {
                            m_HealthPct = uint8(ceilf(l_Caster->GetHealthPct()));
                            m_Stack = 25 - (m_HealthLimit - m_HealthPct);

                            /// Makes sure it doesn't applies more than 25 stacks
                            if (m_Stack > GetSpellInfo()->Effects[SpellEffIndex::EFFECT_1].BasePoints)
                                m_Stack = GetSpellInfo()->Effects[SpellEffIndex::EFFECT_1].BasePoints;

                            /// Break Crystal Spines while loosing stacks
                            if (m_Stack < 10)
                            {
                                int8 l_SpineStacks = m_Stack - 4;

                                if (Creature* l_Spine = Creature::GetCreature(*l_Caster, l_Caster->AI()->GetGUID(eCreatures::NpcCrystalSpine3)))
                                {
                                    if (l_SpineStacks > 0)
                                    {
                                        if (Aura* l_SpineAura = l_Spine->GetAura(eSpells::SpellChitinousExoskeletonScale))
                                        {
                                            if (l_SpineAura->GetStackAmount() > l_SpineStacks)
                                                l_SpineAura->SetStackAmount(l_SpineStacks);
                                        }
                                    }
                                    else
                                        l_Spine->RemoveAura(eSpells::SpellChitinousExoskeletonScale);
                                }

                                if (Creature* l_Spine = Creature::GetCreature(*l_Caster, l_Caster->AI()->GetGUID(eCreatures::NpcCrystalSpine2)))
                                    l_Spine->RemoveAura(eSpells::SpellChitinousExoskeletonScale);

                                if (Creature* l_Spine = Creature::GetCreature(*l_Caster, l_Caster->AI()->GetGUID(eCreatures::NpcCrystalSpine1)))
                                    l_Spine->RemoveAura(eSpells::SpellChitinousExoskeletonScale);
                            }
                            else if (m_Stack < 17)
                            {
                                uint8 l_SpineStacks = m_Stack - 11;

                                if (Creature* l_Spine = Creature::GetCreature(*l_Caster, l_Caster->AI()->GetGUID(eCreatures::NpcCrystalSpine2)))
                                {
                                    if (Aura* l_SpineAura = l_Spine->GetAura(eSpells::SpellChitinousExoskeletonScale))
                                    {
                                        if (l_SpineAura->GetStackAmount() > l_SpineStacks)
                                            l_SpineAura->SetStackAmount(l_SpineStacks);
                                    }
                                }

                                if (Creature* l_Spine = Creature::GetCreature(*l_Caster, l_Caster->AI()->GetGUID(eCreatures::NpcCrystalSpine1)))
                                    l_Spine->RemoveAura(eSpells::SpellChitinousExoskeletonScale);
                            }
                            else if (m_Stack < 24)
                            {
                                uint8 l_SpineStacks = m_Stack - 18;

                                if (Creature* l_Spine = Creature::GetCreature(*l_Caster, l_Caster->AI()->GetGUID(eCreatures::NpcCrystalSpine1)))
                                {
                                    if (Aura* l_SpineAura = l_Spine->GetAura(eSpells::SpellChitinousExoskeletonScale))
                                    {
                                        if (l_SpineAura->GetStackAmount() > l_SpineStacks)
                                            l_SpineAura->SetStackAmount(l_SpineStacks);
                                    }
                                }
                            }

                            if (m_Stack > 0)
                                l_Aura->SetStackAmount(m_Stack);
                            else
                                p_AurEff->GetBase()->Remove();
                        }
                    }
                }
                else
                    m_CheckTimer -= p_Diff;
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_skorpyron_chitinous_exoskeleton_AuraScript::AfterApply, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectApplyFn(spell_skorpyron_chitinous_exoskeleton_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
                OnEffectUpdate += AuraEffectUpdateFn(spell_skorpyron_chitinous_exoskeleton_AuraScript::OnUpdate, SpellEffIndex::EFFECT_1, AuraType::SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_skorpyron_chitinous_exoskeleton_AuraScript();
        }
};

/// Arcanoslash - 205200
class spell_skorpyron_arcanoslash_periodic : public SpellScriptLoader
{
    public:
        spell_skorpyron_arcanoslash_periodic() : SpellScriptLoader("spell_skorpyron_arcanoslash_periodic") { }

        class spell_skorpyron_arcanoslash_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_skorpyron_arcanoslash_periodic_AuraScript);

            uint8 m_PowerCount = 0;
            uint8 m_Counter = 0;

            void OnTick(AuraEffect const* p_AurEff)
            {
                GetTarget()->CastSpell(GetTarget(), p_AurEff->GetAmount(), false);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_skorpyron_arcanoslash_periodic_AuraScript::OnTick, SpellEffIndex::EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_skorpyron_arcanoslash_periodic_AuraScript();
        }
};

/// Arcanoslash - 204275
class spell_skorpyron_arcanoslash : public SpellScriptLoader
{
    public:
        spell_skorpyron_arcanoslash() : SpellScriptLoader("spell_skorpyron_arcanoslash") { }

        class spell_skorpyron_arcanoslash_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_skorpyron_arcanoslash_SpellScript);

            void HandleOnHit()
            {
                if (!GetHitUnit())
                    return;

                if (Player* l_Player = GetHitUnit()->ToPlayer())
                {
                    if (l_Player->GetRoleForGroup() == Roles::ROLE_TANK)
                        l_Player->CastSpell(l_Player, eSpells::SpellArcaneTether, true);
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_skorpyron_arcanoslash_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_skorpyron_arcanoslash_SpellScript();
        }
};

/// Arcane Tether - 211659
class spell_skorpyron_arcane_tether : public SpellScriptLoader
{
    public:
        spell_skorpyron_arcane_tether() : SpellScriptLoader("spell_skorpyron_arcane_tether") { }

        class spell_skorpyron_arcane_tether_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_skorpyron_arcane_tether_SpellScript);

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                if (!GetHitUnit())
                    return;

                if (Aura* l_Aura = GetHitUnit()->GetAura(eSpells::SpellArcaneTetherAura))
                {
                    if (l_Aura->GetStackAmount() > 0)
                        SetHitDamage(GetHitDamage() * l_Aura->GetStackAmount());
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_skorpyron_arcane_tether_SpellScript::HandleDamage, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_skorpyron_arcane_tether_SpellScript();
        }
};

/// Arcane Tether (Visual) - 204529
class spell_skorpyron_arcane_tether_visual : public SpellScriptLoader
{
    public:
        spell_skorpyron_arcane_tether_visual() : SpellScriptLoader("spell_skorpyron_arcane_tether_visual") { }

        class spell_skorpyron_arcane_tether_visual_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_skorpyron_arcane_tether_visual_AuraScript);

            void AfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_Target->CastSpell(l_Target, eSpells::SpellArcaneTetherAura, true);
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();

                if (!l_Target)
                    return;

                if (Aura* l_Aura = l_Target->GetAura(eSpells::SpellArcaneTetherAura))
                    l_Aura->DropStack();

                if (l_Caster && l_Caster->ToCreature())
                    l_Caster->ToCreature()->DespawnOrUnsummon(l_Caster->GetMap()->IsMythic() ? 15 * TimeConstants::IN_MILLISECONDS : 0);
            }

            void OnUpdate(uint32 /*p_Diff*/, AuraEffect* p_AurEff)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetUnitOwner();

                if (!l_Caster || !l_Target)
                {
                    p_AurEff->GetBase()->Remove();
                    return;
                }

                if (l_Target->GetDistance(l_Caster) > 10.0f)
                    p_AurEff->GetBase()->Remove();
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_skorpyron_arcane_tether_visual_AuraScript::AfterApply, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_DUMMY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectApplyFn(spell_skorpyron_arcane_tether_visual_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_DUMMY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
                OnEffectUpdate += AuraEffectUpdateFn(spell_skorpyron_arcane_tether_visual_AuraScript::OnUpdate, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_skorpyron_arcane_tether_visual_AuraScript();
        }
};

/// Shockwave (Knock Back) - 204316
/// Shockwave (Damage) - 210074
class spell_skorpyron_shockwave_filter : public SpellScriptLoader
{
    public:
        spell_skorpyron_shockwave_filter() : SpellScriptLoader("spell_skorpyron_shockwave_filter") { }

        class spell_skorpyron_shockwave_filter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_skorpyron_shockwave_filter_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                if (Unit* l_Caster = GetCaster())
                {
                    p_Targets.remove_if([l_Caster](WorldObject* p_Object) -> bool
                    {
                        /// If a Crystalline Shard is between the target and Skorpyron, prevent hit effect
                        if (!p_Object->IsWithinLOS(l_Caster->m_positionX, l_Caster->m_positionY, l_Caster->m_positionZ))
                            return true;

                        float l_Distance = l_Caster->GetDistance(p_Object) + 5.0f;

                        std::list<GameObject*> l_Shards;
                        l_Caster->GetGameObjectListWithEntryInGrid(l_Shards, eGameObjects::GoSkorpyronLOSBlocker, l_Distance);

                        float l_Radius = 1.75f;

                        for (GameObject* l_Gob : l_Shards)
                        {
                            l_Distance = l_Caster->GetExactDist(l_Gob);
                            if (G3D::fuzzyEq(l_Distance, 0.0f))
                                return true;

                            Position l_Pos = l_Caster->GetPosition();

                            l_Pos.SetOrientation(l_Pos.GetAngle(l_Gob));

                            float l_Angle = 2.0f * std::atan(l_Radius / l_Distance);

                            /// If a LOS Blocker is between the target and Skorpyron, prevent hit effect
                            if (l_Pos.HasInArc(l_Angle, p_Object))
                                return true;
                        }

                        return false;
                    });
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_skorpyron_shockwave_filter_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_skorpyron_shockwave_filter_SpellScript();
        }
};

/// Scorpid Swarm - 204698
class spell_skorpyron_scorpid_swarm : public SpellScriptLoader
{
    public:
        spell_skorpyron_scorpid_swarm() : SpellScriptLoader("spell_skorpyron_scorpid_swarm") { }

        class spell_skorpyron_scorpid_swarm_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_skorpyron_scorpid_swarm_AuraScript);

            void OnUpdate(uint32 /*p_Diff*/, AuraEffect* p_AurEff)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetUnitOwner();

                if (!l_Target || !l_Caster || l_Target->GetDistance(l_Caster) > 15.0f || !l_Caster->isAlive())
                    p_AurEff->GetBase()->Remove();
            }

            void Register() override
            {
                OnEffectUpdate += AuraEffectUpdateFn(spell_skorpyron_scorpid_swarm_AuraScript::OnUpdate, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_MELEE_SLOW);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_skorpyron_scorpid_swarm_AuraScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_skorpyron()
{
    /// Boss
    new boss_skorpyron();

    /// Creatures
    new npc_skorpyron_generic_scorp();
    new npc_skorpyron_crystalline_fragments();
    new npc_skorpyron_arcane_tether();

    /// Spells
    new spell_skorpyron_periodic_energize();
    new spell_skorpyron_chitinous_exoskeleton();
    new spell_skorpyron_arcanoslash_periodic();
    new spell_skorpyron_arcanoslash();
    new spell_skorpyron_arcane_tether();
    new spell_skorpyron_arcane_tether_visual();
    new spell_skorpyron_scorpid_swarm();
}
#endif
