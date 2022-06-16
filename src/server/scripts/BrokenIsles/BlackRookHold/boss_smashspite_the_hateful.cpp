////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "black_rook_hold_dungeon.hpp"

enum eEvents
{
    EVENT_EARTHSHAKING_STOMP    = 1,
    EVENT_FEL_ATTACK            = 2,
    EVENT_HATEFUL_GAZE          = 3
};

Position const g_FelPos[10] =
{
    { 3377.12f, 7379.05f, 260.46f, 0.0f },
    { 3405.04f, 7360.28f, 250.10f, 0.0f },
    { 3410.22f, 7311.27f, 250.10f, 0.0f },
    { 3392.56f, 7317.72f, 260.46f, 0.0f },
    { 3379.99f, 7295.96f, 268.62f, 0.0f },
    { 3388.76f, 7284.25f, 237.00f, 0.0f },
    { 3257.77f, 7188.85f, 255.80f, 0.0f },
    { 3352.13f, 7259.52f, 251.29f, 0.0f },
    { 3206.34f, 7261.78f, 255.80f, 0.0f },
    { 3267.14f, 7182.61f, 244.89f, 0.0f }
};

Position const g_CenterPos = {3269.27f, 7289.28f, 231.47f};

/// Smashspite the Hateful - 98949
class boss_smashspite_the_hateful : public CreatureScript
{
    public:
        boss_smashspite_the_hateful() : CreatureScript("boss_smashspite_the_hateful") { }

        struct boss_smashspite_the_hatefulAI : public BossAI
        {
            boss_smashspite_the_hatefulAI(Creature* creature) : BossAI(creature, eData::Smashspite) { }

            uint64 m_FelbatGuids[10];
            uint32 m_FelAttackCooldown;
            std::set<uint64> m_RisenFighters;

            enum eSpells
            {
                ZeroRegen               = 72242,
                BrutalityProc           = 198114,
                EarthshakingStomp       = 198073,

                /// Heroic
                HatefulGaze             = 198079,
                HatefulCharge           = 198080,
                HatefulChargeDebuff     = 224188,

                DefeatedConversation    = 205271
            };

            enum eTalks
            {
                Aggro,
                HatefulGazeTalk,
                EarthshakingStompTalk = 3,
            };

            void Reset() override
            {
                _Reset();
                me->SetReactState(REACT_AGGRESSIVE);
                me->ClearUnitState(UNIT_STATE_ROOT);
                DoCast(me, eSpells::ZeroRegen, true);
                DoCast(me, eSpells::BrutalityProc, true);
                me->SetPower(POWER_ENERGY, 0);

                for (uint8 l_I = 0; l_I < 10; l_I++)
                {
                    m_FelbatGuids[l_I] = 0;

                    if (Creature* l_Summon = me->SummonCreature(eCreatures::FelBat, g_FelPos[l_I]))
                        m_FelbatGuids[l_I] = l_Summon->GetGUID();
                }

                AddTimedDelayedOperation(1 * IN_MILLISECONDS, [this]() -> void
                {
                    std::list<Creature*> l_RisenFighters;
                    GetCreatureListWithEntryInGrid(l_RisenFighters, me, eCreatures::RisenLancer, 140.0f);
                    GetCreatureListWithEntryInGrid(l_RisenFighters, me, eCreatures::RisenSwordsman, 140.0f);

                    for (Creature* l_Creature : l_RisenFighters)
                    {
                        if (l_Creature->m_positionZ < 224.5f)
                            m_RisenFighters.insert(l_Creature->GetGUID());

                        l_Creature->SetVisible(false);
                        l_Creature->SetReactState(REACT_PASSIVE);
                    }
                });
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                Talk(eTalks::Aggro);
                _EnterCombat();

                events.ScheduleEvent(EVENT_EARTHSHAKING_STOMP, 12000);
                events.ScheduleEvent(EVENT_FEL_ATTACK, 25000);
                if (IsHeroicOrMythic())
                    events.ScheduleEvent(EVENT_HATEFUL_GAZE, 6000);

                m_FelAttackCooldown = 30 * IN_MILLISECONDS;
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                _JustDied();
                DoCast(me, eSpells::DefeatedConversation, true);

                for (uint64 l_CreatureGUID : m_RisenFighters)
                {
                    if (Creature* l_Creature = Creature::GetCreature(*me, l_CreatureGUID))
                    {
                        l_Creature->SetVisible(true);
                        l_Creature->SetReactState(REACT_AGGRESSIVE);
                        switch (l_Creature->GetEntry())
                        {
                            case eCreatures::RisenLancer:
                            {
                                l_Creature->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, EMOTE_ONESHOT_READY1H);
                                break;
                            }
                            case eCreatures::RisenSwordsman:
                            {
                                l_Creature->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, EMOTE_STATE_READY2HL);
                                break;
                            }
                            default:
                                break;
                        }
                    }
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::HatefulCharge)
                    DoCast(p_Target, eSpells::HatefulChargeDebuff, true);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (me->GetDistance(g_CenterPos) >= 30.0f)
                {
                    EnterEvadeMode();
                    return;
                }

                switch (events.ExecuteEvent())
                {
                    case EVENT_EARTHSHAKING_STOMP:
                    {
                        Talk(eTalks::EarthshakingStompTalk);
                        DoCast(eSpells::EarthshakingStomp);
                        events.ScheduleEvent(EVENT_EARTHSHAKING_STOMP, 17000);
                        break;
                    }
                    case EVENT_FEL_ATTACK:
                    {
                        if (Creature* l_Fel = me->GetCreature(*me, m_FelbatGuids[urand(0, 9)]))
                        {
                            if (l_Fel->IsAIEnabled)
                                l_Fel->AI()->DoAction(true);
                        }

                        events.ScheduleEvent(EVENT_FEL_ATTACK, std::max(static_cast<uint32>(5 * IN_MILLISECONDS), m_FelAttackCooldown));
                        if (m_FelAttackCooldown > 5 * IN_MILLISECONDS)
                            m_FelAttackCooldown -= 3 * IN_MILLISECONDS;
                        break;
                    }
                    case EVENT_HATEFUL_GAZE:
                    {
                        Talk(eTalks::HatefulGazeTalk);
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_FARTHEST, 0, 100.0f, true))
                        {
                            events.DelayEvents(6000);
                            me->SetReactState(REACT_PASSIVE);
                            me->GetMotionMaster()->Clear();
                            me->StopAttack();
                            me->AddUnitState(UNIT_STATE_ROOT);
                            DoCast(l_Target, eSpells::HatefulGaze);

                            AddTimedDelayedOperation(5500, [this]() -> void
                            {
                                me->SetReactState(REACT_AGGRESSIVE);
                                me->ClearUnitState(UNIT_STATE_ROOT);
                            });
                        }

                        events.ScheduleEvent(EVENT_HATEFUL_GAZE, 25000);
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
            return new boss_smashspite_the_hatefulAI(p_Creature);
        }
};

/// Fel Bat - 100759
class npc_smashspite_fel_bat : public CreatureScript
{
    public:
        npc_smashspite_fel_bat() : CreatureScript("npc_smashspite_fel_bat") { }

        struct npc_smashspite_fel_batAI : public ScriptedAI
        {
            npc_smashspite_fel_batAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            enum eSpells
            {
                FelVomit = 198446,
                FelVomitDamage = 198499
            };

            uint64 m_TargetGuid;

            void Reset() override { }

            void DoAction(int32 const /*p_Action*/) override
            {
                m_TargetGuid = 0;
                DoCast(eSpells::FelVomit);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_Target->GetTypeId() != TYPEID_PLAYER)
                    return;

                if (p_SpellInfo->Id == eSpells::FelVomit)
                {
                    m_TargetGuid = p_Target->GetGUID();
                    events.ScheduleEvent(EVENT_1, 6000);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        if (Unit* l_Target = Unit::GetUnit(*me, m_TargetGuid))
                        {
                            /// Relocate center of circle to 0 to avoid useless calculation
                            float l_BatRelativeX = me->m_positionX - g_CenterPos.m_positionX;
                            float l_BatRelativeY = me->m_positionY - g_CenterPos.m_positionY;;
                            float l_TargetRelativeX = l_Target->m_positionX - g_CenterPos.m_positionX;
                            float l_TargetRelativeY = l_Target->m_positionY - g_CenterPos.m_positionY;

                            float l_Radius = 40.0f;

                            /// straight line equation (ax +b) between Bat and player
                            float l_A = (l_BatRelativeY - l_TargetRelativeY) / (l_BatRelativeX - l_TargetRelativeX);
                            float l_B = - (l_A * l_BatRelativeX) + l_BatRelativeY;

                            /// Circle equation ((x-c)^2 + (y-d)^2 = radius^2) (center 0, y replaced by straight line equation)
                            float l_C = 1.0f + (l_A * l_A);
                            float l_D = (2.0f * l_A * l_B);
                            float l_R = (l_B * l_B) - (l_Radius * l_Radius);

                            /// Roots of the circle equation
                            float l_RootA = (-l_D + std::sqrt((l_D * l_D) - 4.0f * l_C * l_R)) / (2.0f * l_C);
                            float l_RootB = (-l_D - std::sqrt((l_D * l_D) - 4.0f * l_C * l_R)) / (2.0f * l_C);

                            /// Abscissa of Roots
                            float l_RootAY = l_A * l_RootA + l_B;
                            float l_RootBY = l_A * l_RootB + l_B;

                            /// Relocate at correct coordinates
                            Position l_IntersectA = { g_CenterPos.m_positionX + l_RootA, g_CenterPos.m_positionY + l_RootAY, g_CenterPos.m_positionZ };
                            Position l_IntersectB = { g_CenterPos.m_positionX + l_RootB, g_CenterPos.m_positionY + l_RootBY, g_CenterPos.m_positionZ };

                            /// Set closest intersect to the bat first
                            if (me->GetExactDist2d(&l_IntersectA) > me->GetExactDist2d(&l_IntersectB))
                            {
                                Position l_TempPos = l_IntersectA;
                                l_IntersectA = l_IntersectB;
                                l_IntersectB = l_TempPos;
                            }

                            float l_Angle = l_IntersectA.GetAngle(&l_IntersectB);
                            for (uint8 l_I = 0; l_I < 25; l_I++)
                            {
                                Position l_Pos;
                                l_IntersectA.SimplePosXYRelocationByAngle(l_Pos, l_I * 3.0f, l_Angle);
                                me->CastSpell(l_Pos, eSpells::FelVomitDamage, true);

                                if (l_IntersectB.GetExactDist2d(&l_Pos) < 3.0f)
                                    break;
                            }
                        }
                        break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_smashspite_fel_batAI(p_Creature);
        }
};

/// Brutality - 198114
class spell_smashpite_brutality_proc : public SpellScriptLoader
{
    public:
        spell_smashpite_brutality_proc() : SpellScriptLoader("spell_smashpite_brutality_proc") { }

        class spell_smashpite_brutality_proc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_smashpite_brutality_proc_AuraScript);

            enum eSpells
            {
                BrutalHaymaker = 198245,
            };

            enum eTalks
            {
                Brutality           = 4,
                BrutalHaymakerTalk
            };

            uint8 m_PowerCount = 0;
            uint8 m_Counter = 0;

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                m_Counter++;
                if (m_Counter > 8)
                {
                    Creature* l_Creature = GetCaster()->ToCreature();
                    if (l_Creature && l_Creature->IsAIEnabled)
                        l_Creature->AI()->Talk(eTalks::Brutality);
                    m_Counter = 0;
                }
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                Unit* l_Caster = GetCaster();
                if (!l_Caster || l_Caster->HasUnitState(UNIT_STATE_CASTING) || !l_DamageInfo)
                    return;

                uint32 l_MissingDamage = l_DamageInfo->GetAbsorb() + l_DamageInfo->GetBlock();
                uint32 l_TotalDamage = l_MissingDamage + l_DamageInfo->GetAmount();
                uint32 l_Pct = l_DamageInfo->GetAmount() * 100 / l_TotalDamage;
                uint32 l_Energy = l_Pct / 10;

                m_PowerCount = l_Caster->GetPower(POWER_ENERGY);

                if (m_PowerCount < 100)
                    l_Caster->SetPower(POWER_ENERGY, m_PowerCount + l_Energy);
                else
                {
                    if (l_Caster->getVictim())
                    {
                        Creature* l_Creature = l_Caster->ToCreature();
                        if (l_Creature && l_Creature->IsAIEnabled)
                            l_Creature->AI()->Talk(eTalks::BrutalHaymakerTalk);
                        l_Caster->CastSpell(l_Caster->getVictim(), eSpells::BrutalHaymaker);
                    }
                }
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_smashpite_brutality_proc_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_smashpite_brutality_proc_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Hateful Gaze - 198079
class spell_smashpite_hateful_gaze : public SpellScriptLoader
{
    public:
        spell_smashpite_hateful_gaze() : SpellScriptLoader("spell_smashpite_hateful_gaze") { }

        class spell_smashpite_hateful_gaze_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_smashpite_hateful_gaze_AuraScript);

            enum eSpells
            {
                HatefulChargeDmg  = 198080,
                HatefulChargeMove = 198250,
            };

            enum eTalks
            {
                HatefulChargeTalk = 2
            };

            void AfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Marked = GetTarget();
                if (!l_Caster || !l_Marked)
                    return;

                Creature* l_Creature = l_Caster->ToCreature();

                if (l_Creature && l_Creature->IsAIEnabled)
                    l_Creature->AI()->Talk(eTalks::HatefulChargeTalk);

                std::list<HostileReference*> const & l_Targets = l_Caster->getThreatManager().getThreatList();
                std::list<Unit*> l_ColinealTargets;

                for (HostileReference* l_Itr : l_Targets)
                {
                    Unit* l_Target = l_Itr->getTarget();

                    if (l_Target == nullptr)
                        continue;

                    if (l_Target->GetGUID() == l_Marked->GetGUID())
                        continue;

                    if (l_Target->IsInAxe(l_Caster, l_Marked, 5.f))
                        l_ColinealTargets.push_back(l_Target);
                }

                if (!l_ColinealTargets.empty())
                {
                    l_ColinealTargets.sort(JadeCore::ObjectDistanceOrderPred(l_Caster));

                    l_Caster->DelayedCastSpell(l_ColinealTargets.front(), eSpells::HatefulChargeDmg, true, 10);
                    l_Caster->CastSpell(l_ColinealTargets.front(), eSpells::HatefulChargeMove, true);
                }
                else
                {
                    l_Caster->DelayedCastSpell(l_Marked, eSpells::HatefulChargeDmg, true, 10);
                    l_Caster->CastSpell(l_Marked, eSpells::HatefulChargeMove, true);
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_smashpite_hateful_gaze_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_smashpite_hateful_gaze_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Wyrmtongue Scavenger - 98792
class npc_wyrmtongue_scavenger : public CreatureScript
{
    public:
        npc_wyrmtongue_scavenger() : CreatureScript("npc_wyrmtongue_scavenger") { }

        struct npc_wyrmtongue_scavengerAI : public ScriptedAI
        {
            npc_wyrmtongue_scavengerAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                DrinkAncientPotion      = 200784,
                ThrowPricelessArtifact  = 201176,
                HyperactiveAreatrigger  = 201063,
                FrenzyPotion            = 201061,
                Indigestion             = 200913,
                Hyperactive             = 201064,
                Dizzy                   = 201070
            };

            enum eTalks
            {
                HyperactiveTrigger
            };

            int32 m_AncientPotionTimer;
            int32 m_ThrowArtifactTimer;
            bool m_PotionUsed;
            bool m_IsRandomMoving;
            bool m_IsNotMoving;

            void Reset() override { }

            void EnterCombat(Unit* /*p_Ennemy*/) override
            {
                m_AncientPotionTimer = 5 * IN_MILLISECONDS;
                m_ThrowArtifactTimer = 12 * IN_MILLISECONDS;
                m_PotionUsed = false;
                m_IsRandomMoving = false;
                m_IsNotMoving = false;
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::Indigestion:
                    {
                        AddTimedDelayedOperation(100, [this]() -> void
                        {
                            me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                        });
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (me->HasAura(eSpells::HyperactiveAreatrigger))
                {
                    if (!m_IsRandomMoving)
                    {
                        m_IsRandomMoving = true;
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveRandom(10.0f);
                        me->SetReactState(REACT_PASSIVE);
                        DoStopAttack();
                    }
                    return;
                }

                if (me->HasAura(eSpells::Dizzy))
                {
                    if (m_IsRandomMoving)
                    {
                        me->GetMotionMaster()->Clear();
                        m_IsRandomMoving = false;
                        m_IsNotMoving = true;
                    }
                    return;
                }

                if (m_IsNotMoving)
                {
                    m_IsNotMoving = false;
                    me->GetMotionMaster()->Clear();
                    me->SetReactState(REACT_AGGRESSIVE);
                    DoStopAttack();
                }

                if (!me->isInCombat())
                    return;

                m_ThrowArtifactTimer -= p_Diff;
                m_AncientPotionTimer -= p_Diff;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (!UpdateVictim())
                    return;

                if (m_AncientPotionTimer < 0 && !m_PotionUsed)
                {
                    me->CastSpell(me, eSpells::DrinkAncientPotion, false);
                    m_PotionUsed = true;
                    return;
                }

                if (m_PotionUsed && m_ThrowArtifactTimer < 0)
                {
                    Unit* l_Victim = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true);

                    if (me->HasAura(eSpells::Hyperactive))
                    {
                        if (!l_Victim)
                            return;

                        me->CastSpell(l_Victim, eSpells::HyperactiveAreatrigger, false);
                        Talk(eTalks::HyperactiveTrigger);
                        m_ThrowArtifactTimer = 20 * IN_MILLISECONDS;
                        return;
                    }

                    if (!me->HasAura(eSpells::FrenzyPotion))
                    {
                        if (!l_Victim)
                            return;

                        me->CastSpell(l_Victim, eSpells::ThrowPricelessArtifact, false);
                    }
                    m_ThrowArtifactTimer = 8 * IN_MILLISECONDS;
                    return;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_wyrmtongue_scavengerAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by "Drink" Ancient Potion - 200784
class spell_brh_drink_ancient_potion: public SpellScriptLoader
{
    public:
        spell_brh_drink_ancient_potion() : SpellScriptLoader("spell_brh_drink_ancient_potion") { }

        class spell_brh_drink_ancient_potion_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_brh_drink_ancient_potion_SpellScript);

            enum eSpells
            {
                FrenzyPotion = 201061,
                Indigestion = 200913,
                Hyperactive = 201064,
                HyperactiveAreatrigger = 201063
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                switch (urand(0,2))
                {
                    case 0:
                    {
                        l_Caster->CastSpell(l_Caster, eSpells::FrenzyPotion, true);
                        break;
                    }
                    case 1:
                    {
                        l_Caster->CastSpell(l_Caster, eSpells::Indigestion, true);
                        l_Caster->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                        break;
                    }
                    case 2:
                    {
                        l_Caster->CastSpell(l_Caster, eSpells::Hyperactive, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_brh_drink_ancient_potion_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_brh_drink_ancient_potion_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Wrathguard Bladelord - 98810
class npc_wrathguard_bladelord : public CreatureScript
{
    public:
        npc_wrathguard_bladelord() : CreatureScript("npc_wrathguard_bladelord") { }

        struct npc_wrathguard_bladelordAI : public ScriptedAI
        {
            npc_wrathguard_bladelordAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                BrutalAssault   = 201139,
                Enrage          = 8599
            };

            int32 m_BrutalAssaultTimer;
            bool m_Enraged;

            void Reset() override 
            {
                me->RemoveAurasDueToSpell(eSpells::Enrage);
            }

            void EnterCombat(Unit* /*p_Ennemy*/) override
            {
                m_BrutalAssaultTimer = 15 * IN_MILLISECONDS;
                m_Enraged = false;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!me->isInCombat())
                    return;

                m_BrutalAssaultTimer -= p_Diff;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (me->GetHealthPct() < 25.0f && !me->HasAura(eSpells::Enrage))
                {
                    me->CastSpell(me, eSpells::Enrage, false);
                    return;
                }

                if (!UpdateVictim())
                    return;

                if (m_BrutalAssaultTimer < 0)
                {
                    me->CastSpell(me, eSpells::BrutalAssault, false);
                    m_BrutalAssaultTimer = 20 * IN_MILLISECONDS;
                    return;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_wrathguard_bladelordAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Brutal Assault - 201139
class spell_brh_brutal_assault: public SpellScriptLoader
{
    public:
        spell_brh_brutal_assault() : SpellScriptLoader("spell_brh_brutal_assault") { }

        class spell_brh_brutal_assault_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_brh_brutal_assault_AuraScript);

            enum eSpells
            {
                BrutalAssaultDamage = 201141
            };

            void OnTick(AuraEffect const* l_AuraEffect)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_Target = l_Caster->getVictim();
                if (!l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::BrutalAssaultDamage, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_brh_brutal_assault_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_brh_brutal_assault_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Felspite Dominator - 102788
class npc_felspite_dominator : public CreatureScript
{
    public:
        npc_felspite_dominator() : CreatureScript("npc_felspite_dominator") { }

        struct npc_felspite_dominatorAI : public ScriptedAI
        {
            npc_felspite_dominatorAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_InstanceScript = p_Creature->GetInstanceScript();
            }

            enum eSpells
            {
                SicBats = 203163,
                FelFrenzy = 227913
            };

            InstanceScript* m_InstanceScript;
            int32 m_SicBatsTimer;
            int32 m_FelFrenzyTimer;

            void Reset() override { }

            void EnterCombat(Unit* /*p_Ennemy*/) override
            {
                m_SicBatsTimer = 15 * IN_MILLISECONDS;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!me->isInCombat())
                    return;

                m_SicBatsTimer -= p_Diff;
                m_FelFrenzyTimer -= p_Diff;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (!UpdateVictim())
                    return;

                if (m_SicBatsTimer < 0)
                {
                    Unit* l_Victim = SelectPlayerTarget(eTargetTypeMask::TypeMaskNonTank);
                    if (!l_Victim)
                        return;

                    me->CastSpell(l_Victim, eSpells::SicBats, false);
                    m_SicBatsTimer = 20 * IN_MILLISECONDS;
                    return;
                }

                if (IsMythic() && m_FelFrenzyTimer < 0)
                {
                    me->CastSpell(me, eSpells::FelFrenzy, false);
                    m_FelFrenzyTimer = 20 * IN_MILLISECONDS;
                    return;
                }

                DoMeleeAttackIfReady();
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                m_InstanceScript->SetData64(me->GetEntry(), me->GetGUID());
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->SetHomePosition(*me);
                Unit* l_Victim = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true);
                if (!l_Victim)
                    me->DespawnOrUnsummon();

                AttackStart(l_Victim, true);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_felspite_dominatorAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Sic Bats! - 203163
class spell_brh_sic_bats: public SpellScriptLoader
{
    public:
        spell_brh_sic_bats() : SpellScriptLoader("spell_brh_sic_bats") { }

        class spell_brh_sic_bats_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_brh_sic_bats_SpellScript);

            enum eNPCs
            {
                FelBatPup = 102781
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                std::list<Creature*> l_CreatureList;
                GetCreatureListWithEntryInGrid(l_CreatureList, l_Caster, eNPCs::FelBatPup,  15.0f);
                for (Creature* l_Creature : l_CreatureList)
                    l_Creature->AddThreat(l_Target, 1000000000.0f);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_brh_sic_bats_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_brh_sic_bats_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Boulder - 111706
class npc_boulder : public CreatureScript
{
    public:
        npc_boulder() : CreatureScript("npc_boulder") { }

        struct npc_boulderAI : public ScriptedAI
        {
            npc_boulderAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            enum eSpells
            {
                BoulderCrush = 222378,
                BoulderDespawn = 222388
            };

            std::array<std::vector<G3D::Vector3>, 4> m_Pathes =
            { {
                {
                    { 3179.115f, 7315.341f, 129.8934f },
                    { 3178.955f, 7312.581f, 129.8101f },
                    { 3178.795f, 7309.82f,  129.7268f },
                    { 3176.5f,   7305.61f,  128.4733f },
                    { 3174.281f, 7303.528f, 126.5653f },
                    { 3172.203f, 7302.246f, 125.5041f },
                    { 3167.22f,  7300.728f, 122.9033f },
                    { 3160.021f, 7300.182f, 119.6351f },
                    { 3156.063f, 7302.952f, 117.2217f },
                    { 3152.26f,  7306.499f, 113.8119f },
                    { 3149.172f, 7315.389f, 108.1658f },
                    { 3153.592f, 7324.115f, 103.6225f },
                    { 3158.42f,  7327.024f, 102.6821f },
                    { 3172.057f, 7325.502f, 99.08191f },
                    { 3174.845f, 7324.731f, 98.74758f }
                },

                {
                    { 3174.92f,  7317.136f, 129.8934f },
                    { 3174.606f, 7313.834f, 129.8101f },
                    { 3174.292f, 7310.533f, 129.7268f },
                    { 3172.497f, 7307.643f, 127.957f },
                    { 3170.175f, 7305.052f, 125.5795f },
                    { 3165.748f, 7302.913f, 122.0027f },
                    { 3162.448f, 7303.2f,   120.4336f },
                    { 3158.609f, 7304.304f, 118.1291f },
                    { 3154.634f, 7309.561f, 112.832f },
                    { 3153.817f, 7317.096f, 106.1596f },
                    { 3158.566f, 7321.717f, 102.6821f },
                    { 3167.995f, 7322.456f, 99.80836f },
                    { 3170.046f, 7321.687f, 98.99516f }
                },

                {
                    { 3171.11f, 7396.598f, 196.3463f },
                    { 3174.29f, 7396.42f,  195.7945f },
                    { 3177.47f, 7396.243f, 195.2428f },
                    { 3182.8f,  7394.917f, 191.3927f },
                    { 3186.63f, 7391.321f, 187.0906f },
                    { 3187.91f, 7385.942f, 183.9955f },
                    { 3185.50f, 7380.564f, 179.2047f },
                    { 3180.24f, 7378.146f, 173.6349f },
                    { 3175.20f, 7377.793f, 169.9319f },
                    { 3170.71f, 7379.915f, 167.3087f },
                    { 3167.34f, 7383.639f, 164.9337f },
                    { 3167.44f, 7386.967f, 163.2067f },
                    { 3168.33f, 7391.927f, 161.1615f },
                    { 3170.99f, 7396.063f, 161.1159f },
                    { 3174.59f, 7398.284f, 161.0478f },
                    { 3179.19f, 7396.338f, 159.629f  },
                    { 3181.35f, 7395.578f, 157.4754f },
                    { 3183.43f, 7393.913f, 155.0812f },
                    { 3186.46f, 7391.561f, 152.4962f },
                    { 3187.53f, 7385.343f, 148.8089f },
                    { 3185.17f, 7379.852f, 143.7104f },
                    { 3180.35f, 7377.463f, 139.8137f },
                    { 3174.58f, 7377.682f, 137.0626f },
                    { 3171.22f, 7380.024f, 135.6559f },
                    { 3167.63f, 7385.112f, 132.5935f },
                    { 3162.3f,  7392.113f, 131.0014f }
                },

                {
                    { 3173.466f, 7400.026f, 195.7867f },
                    { 3175.615f, 7400.617f, 195.7038f },
                    { 3179.115f, 7401.067f, 194.2708f },
                    { 3185.943f, 7399.217f, 191.1641f },
                    { 3190.708f, 7394.601f, 187.9762f },
                    { 3192.778f, 7390.299f, 186.1577f },
                    { 3192.559f, 7383.187f, 182.6607f },
                    { 3188.384f, 7375.801f, 177.8578f },
                    { 3181.992f, 7372.616f, 173.7892f },
                    { 3176.072f, 7372.088f, 171.0771f },
                    { 3168.604f, 7375.294f, 168.0041f },
                    { 3164.877f, 7379.786f, 165.9374f },
                    { 3162.98f,  7385.084f, 163.7544f },
                    { 3163.255f, 7389.731f, 162.2214f },
                    { 3165.497f, 7395.184f, 161.0755f },
                    { 3172.352f, 7400.72f,  161.0597f },
                    { 3178.786f, 7400.527f, 159.8175f },
                    { 3188.029f, 7396.971f, 154.2875f },
                    { 3190.132f, 7392.836f, 152.263f  },
                    { 3190.882f, 7389.71f,  151.0753f },
                    { 3191.232f, 7385.506f, 148.5145f },
                    { 3189.833f, 7378.788f, 145.1518f },
                    { 3182.2f,   7371.038f, 140.039f  },
                    { 3175.101f, 7371.841f, 138.02f   },
                    { 3167.184f, 7372.649f, 136.8291f }
                },
            }};

            bool m_PathLaunched = false;
            int32 m_PathToUse = -1;

            void Reset() override
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                if (me->m_positionZ > 150.0f)
                {
                    if (me->m_positionY > 7398.0f)
                        m_PathToUse = 3;
                    else
                    {
                        m_PathToUse = 2;
                        me->SetSpeed(MOVE_WALK, 0.4f);
                    }
                }
                else
                {
                    if (me->m_positionX > 3176.0f)
                        m_PathToUse = 0;
                    else
                        m_PathToUse = 1;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (m_PathLaunched)
                    return;

                me->CastSpell(me, eSpells::BoulderCrush, true);

                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveSmoothPath(0, m_Pathes[m_PathToUse].data(), m_Pathes[m_PathToUse].size(), false);

                m_PathLaunched = true;
            }

            void MovementInform(uint32 /*p_Type*/, uint32 /*p_ID*/) override
            {
                me->CastSpell(me, eSpells::BoulderDespawn, true);
                me->DespawnOrUnsummon(1);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_boulderAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Boulder Crush - 222397
class spell_brh_boulder_crush: public SpellScriptLoader
{
    public:
        spell_brh_boulder_crush() : SpellScriptLoader("spell_brh_boulder_crush") { }

        class spell_brh_boulder_crush_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_brh_boulder_crush_SpellScript);

            enum eSpells
            {
                BoulderStun = 222417
            };

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->DelayedCastSpell(l_Target, eSpells::BoulderStun, true, 250);
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_brh_boulder_crush_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_brh_boulder_crush_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Fel Bat Pup - 102781
class npc_fel_bat_pup : public CreatureScript
{
    public:
        npc_fel_bat_pup() : CreatureScript("npc_fel_bat_pup") { }

        struct npc_fel_bat_pupAI : public ScriptedAI
        {
            npc_fel_bat_pupAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = me->GetInstanceScript();
            }

            InstanceScript* m_Instance;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (!m_Instance || !m_Instance->instance || m_Instance->instance->GetPlayers().isEmpty())
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                Map::PlayerList::const_iterator l_Itr = m_Instance->instance->GetPlayers().begin();

                if (Player* l_Player = l_Itr->getSource())
                    AttackStart(l_Player, true);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (!m_Instance)
                    return;

                m_Instance->SetData64(me->GetEntry(), me->GetGUID());
            }

            void JustDespawned() override
            {
                if (!m_Instance)
                    return;

                m_Instance->SetData64(me->GetEntry(), me->GetGUID());
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                ScriptedAI::UpdateAI(p_Diff);

                if (!UpdateVictim())
                    me->DespawnOrUnsummon();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_fel_bat_pupAI(p_Creature);
        }
};


#ifndef __clang_analyzer__
void AddSC_boss_smashspite_the_hateful()
{
    /// Boss
    new boss_smashspite_the_hateful();

    /// Creature
    new npc_smashspite_fel_bat();

    /// Spells
    new spell_smashpite_brutality_proc();
    new spell_smashpite_hateful_gaze();

    /// Trashes
    new npc_wyrmtongue_scavenger();
    new npc_wrathguard_bladelord();
    new npc_felspite_dominator();
    new npc_fel_bat_pup();
    new npc_boulder();

    new spell_brh_drink_ancient_potion();
    new spell_brh_brutal_assault();
    new spell_brh_sic_bats();
    new spell_brh_boulder_crush();
}
#endif
