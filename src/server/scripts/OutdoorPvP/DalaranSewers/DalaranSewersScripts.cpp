////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "DalaranSewersMgr.hpp"

/// 115320 - Throwing Stars
class spell_dalaran_throwing_stars : public SpellScriptLoader
{
    public:
        spell_dalaran_throwing_stars() : SpellScriptLoader("spell_dalaran_throwing_stars") {}

        class spell_dalaran_throwing_stars_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dalaran_throwing_stars_AuraScript);

            uint64 m_TargetGUID = 0;

            enum eSpells
            {
                SpellTriggerThrowingStars = 115319
            };

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                Unit* l_Target = sObjectAccessor->GetUnit(*l_Caster, m_TargetGUID);

                if (l_Target == nullptr)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::SpellTriggerThrowingStars, true);
            }

            void OnApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr || l_Caster->GetTypeId() != TYPEID_UNIT)
                    return;

                std::list<HostileReference*> l_ThreatList = l_Caster->getThreatManager().getThreatList();

                if (!l_ThreatList.empty())
                {
                    JadeCore::RandomResizeList(l_ThreatList, 1);

                    if (Unit* l_Target = l_ThreatList.front()->getTarget())
                        m_TargetGUID = l_Target->GetGUID();
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_dalaran_throwing_stars_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                OnEffectApply += AuraEffectApplyFn(spell_dalaran_throwing_stars_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_dalaran_throwing_stars_AuraScript();
        }
};

/// Crazed Mage - 97587
class npc_dalaran_crazed_mage : public CreatureScript
{
    public:
        npc_dalaran_crazed_mage() : CreatureScript("npc_dalaran_crazed_mage")
        {
        }

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_dalaran_crazed_mageAI(creature);
        }

        struct npc_dalaran_crazed_mageAI : public ScriptedAI
        {
            npc_dalaran_crazed_mageAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            enum eData
            {
                EventFireBall     = 1,
                SpellFireBall     = 9053,
                EventImmolate     = 2,
                SpellImmolate     = 11962,
                EventTasteOfBlood = 3,
                SpellTasteOfBlood = 213738,   ///< Self
                EventPlayingWithFire = 4,
                SpellPlayingWithFire = 214004 ///< Self
            };

            void Reset() override
            {
                events.Reset();

                /// Need to handle power (mana) reload ? To check
                events.ScheduleEvent(eData::EventFireBall, 10000);
                events.ScheduleEvent(eData::EventPlayingWithFire, 15000);
                events.ScheduleEvent(eData::EventImmolate, 20000);
                events.ScheduleEvent(eData::EventTasteOfBlood, 30000);
            }

            void JustDied(Unit* p_Killer) override
            {
                ZoneScript* l_ZoneScript = sOutdoorPvPMgr->GetOutdoorPvPToZoneId(eGpsData::DalaranZoneID);

                if (l_ZoneScript == nullptr)
                    return;

                if (OutdoorPvPDalaran* l_Dalaran = (OutdoorPvPDalaran*)l_ZoneScript)
                    l_Dalaran->RareCreatureKilled(me->GetEntry(), me->GetGUID());
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EventFireBall:
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            me->CastSpell(l_Target, SpellFireBall, false);
                        events.ScheduleEvent(EventFireBall, 10000);
                        break;
                    case EventImmolate:
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            me->CastSpell(l_Target, SpellImmolate, false);
                        events.ScheduleEvent(EventImmolate, 20000);
                        break;
                    case EventTasteOfBlood:
                        me->CastSpell(me, SpellTasteOfBlood, false);
                        events.ScheduleEvent(EventTasteOfBlood, 15000);
                        break;
                    case EventPlayingWithFire:
                        me->CastSpell(me, eData::SpellPlayingWithFire, false);
                        break;
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };
};

/// Thieving Scoundrel - 97390
class npc_dalaran_thieving_scoundrel : public CreatureScript
{
    public:
        npc_dalaran_thieving_scoundrel() : CreatureScript("npc_dalaran_thieving_scoundrel")
        {
        }

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_dalaran_thieving_scoundrelAI(creature);
        }

        struct npc_dalaran_thieving_scoundrelAI : public ScriptedAI
        {
            npc_dalaran_thieving_scoundrelAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            enum eData
            {
                EventBladeFlurry   = 1,
                SpellBladeFlurry   = 128096,
                EventFanOfKnices   = 2,
                SpellFanOfKnives   = 165755, ///< Requires melee weapon
                EventNightglowWisp = 3,
                SpellNightglowWisp = 225832, ///< What is this used for ?
                EventTasteOfBlood  = 4,
                SpellTaseOfBlood   = 213738  ///< Self
            };

            void Reset() override
            {
                events.Reset();

                /// Seems like Stealth (34189) is used here, but in which context ?
                events.ScheduleEvent(eData::EventBladeFlurry, 10000);
                events.ScheduleEvent(eData::EventFanOfKnices, 20000);
                events.ScheduleEvent(eData::EventNightglowWisp, 30000);
                events.ScheduleEvent(eData::EventTasteOfBlood, 40000);
            }

            void JustDied(Unit* p_Killer) override
            {
                ZoneScript* l_ZoneScript = sOutdoorPvPMgr->GetOutdoorPvPToZoneId(eGpsData::DalaranZoneID);

                if (l_ZoneScript == nullptr)
                    return;

                if (OutdoorPvPDalaran* l_Dalaran = (OutdoorPvPDalaran*)l_ZoneScript)
                    l_Dalaran->RareCreatureKilled(me->GetEntry(), me->GetGUID());
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EventBladeFlurry:
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            me->CastSpell(l_Target, SpellBladeFlurry, false);
                        events.ScheduleEvent(EventBladeFlurry, 40000);
                        break;
                    case EventFanOfKnices:
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            me->CastSpell(l_Target, SpellFanOfKnives, false);
                        events.ScheduleEvent(EventFanOfKnices, 40000);
                        break;
                    case EventNightglowWisp:
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            me->CastSpell(l_Target, SpellNightglowWisp, false);
                        events.ScheduleEvent(EventNightglowWisp, 40000);
                        break;
                    case EventTasteOfBlood:
                        me->CastSpell(me, SpellTaseOfBlood, false);
                        events.ScheduleEvent(EventTasteOfBlood, 40000);
                        break;
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };
};

/// Xullorax - 97388
class npc_dalaran_xulorax : public CreatureScript
{
    public:
        npc_dalaran_xulorax() : CreatureScript("npc_dalaran_xulorax")
        {
        }

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_dalaran_xuloraxAI(creature);
        }

        struct npc_dalaran_xuloraxAI : public ScriptedAI
        {
            npc_dalaran_xuloraxAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            enum eData
            {
                EventNightglowWisp  = 1,
                SpellNightglowWisp  = 225832,
                EventRumblingSlam   = 2,
                SpellRumblingSlam   = 223420
            };

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(eData::EventNightglowWisp, 10000);
                events.ScheduleEvent(eData::EventRumblingSlam, 15000);
            }

            void JustDied(Unit* p_Killer) override
            {
                ZoneScript* l_ZoneScript = sOutdoorPvPMgr->GetOutdoorPvPToZoneId(eGpsData::DalaranZoneID);

                if (l_ZoneScript == nullptr)
                    return;

                if (OutdoorPvPDalaran* l_Dalaran = (OutdoorPvPDalaran*)l_ZoneScript)
                    l_Dalaran->RareCreatureKilled(me->GetEntry(), me->GetGUID());
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EventNightglowWisp:
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            me->CastSpell(l_Target, SpellNightglowWisp, false);
                        events.ScheduleEvent(EventNightglowWisp, 40000);
                        break;
                    case EventRumblingSlam:
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            me->CastSpell(l_Target, SpellRumblingSlam, false);
                        events.ScheduleEvent(EventRumblingSlam, 40000);
                        break;
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };
};

/// Splint - 97380
class npc_dalaran_splint : public CreatureScript
{
    public:
        npc_dalaran_splint() : CreatureScript("npc_dalaran_splint")
        {
        }

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_dalaran_splintAI(creature);
        }

        struct npc_dalaran_splintAI : public ScriptedAI
        {
            npc_dalaran_splintAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            enum eData
            {
                EventFerociousBite = 1,
                SpellFerociousBite = 131944,
                EventHuntersRush   = 2,
                SpellHuntersRush   = 223971, ///< Self
                EventNightglowWisp = 3,
                SpellNightglowWisp = 225832,
                EventShadowstep    = 4,
                SpellShadowstep    = 128910, ///< TP behind target w/ stealh
                EventThrowingStars = 5,
                SpellThrowingStars = 115320
            };

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(eData::EventFerociousBite, 10000);
                events.ScheduleEvent(eData::EventHuntersRush, 20000);
                events.ScheduleEvent(eData::EventNightglowWisp, 30000);
                events.ScheduleEvent(eData::EventShadowstep, 40000);
                events.ScheduleEvent(eData::EventThrowingStars, 45000);
            }

            void JustDied(Unit* p_Killer) override
            {
                ZoneScript* l_ZoneScript = sOutdoorPvPMgr->GetOutdoorPvPToZoneId(eGpsData::DalaranZoneID);

                if (l_ZoneScript == nullptr)
                    return;

                if (OutdoorPvPDalaran* l_Dalaran = (OutdoorPvPDalaran*)l_ZoneScript)
                    l_Dalaran->RareCreatureKilled(me->GetEntry(), me->GetGUID());
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EventFerociousBite:
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            me->CastSpell(l_Target, SpellFerociousBite, false);
                        events.ScheduleEvent(EventFerociousBite, 45000);
                        break;
                    case EventHuntersRush:
                        me->CastSpell(me, SpellHuntersRush, false);
                        events.ScheduleEvent(EventHuntersRush, 45000);
                        break;
                    case EventNightglowWisp:
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            me->CastSpell(l_Target, SpellNightglowWisp, false);
                        events.ScheduleEvent(EventNightglowWisp, 45000);
                        break;
                    case EventShadowstep:
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            me->CastSpell(l_Target, SpellShadowstep, false);
                        events.ScheduleEvent(EventShadowstep, 45000);
                        break;
                    case EventThrowingStars:
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            me->CastSpell(l_Target, SpellThrowingStars, false);
                        events.ScheduleEvent(EventThrowingStars, 45000);
                        break;
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };
};

/// Mana Seeper - 97387
class npc_dalaran_mana_seeper : public CreatureScript
{
    public:
        npc_dalaran_mana_seeper() : CreatureScript("npc_dalaran_mana_seeper")
        {
        }

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_dalaran_mana_seeperAI(creature);
        }

        struct npc_dalaran_mana_seeperAI : public ScriptedAI
        {
            npc_dalaran_mana_seeperAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            enum eData
            {
                EventArcaneShielding = 1,
                SpellArcaneShielding = 183409,
                EventManaBurst       = 2,
                SpellManaBurst       = 217085,
                EventManaCords       = 3,
                SpellManaCords       = 223598
            };

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(eData::EventArcaneShielding, 10000);
                events.ScheduleEvent(eData::EventManaBurst, 20000);
                events.ScheduleEvent(eData::EventManaCords, 30000);
            }

            void JustDied(Unit* p_Killer) override
            {
                ZoneScript* l_ZoneScript = sOutdoorPvPMgr->GetOutdoorPvPToZoneId(eGpsData::DalaranZoneID);

                if (l_ZoneScript == nullptr)
                    return;

                if (OutdoorPvPDalaran* l_Dalaran = (OutdoorPvPDalaran*)l_ZoneScript)
                    l_Dalaran->RareCreatureKilled(me->GetEntry(), me->GetGUID());
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EventArcaneShielding:
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            me->CastSpell(l_Target, SpellArcaneShielding, false);
                        events.ScheduleEvent(EventArcaneShielding, 45000);
                        break;
                    case EventManaBurst:
                        me->CastSpell(me, SpellManaBurst, false);
                        events.ScheduleEvent(EventManaBurst, 45000);
                        break;
                    case EventManaCords:
                        me->CastSpell(me, SpellManaCords, false);
                        events.ScheduleEvent(EventManaCords, 45000);
                        break;
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };
};

/// Screek - 97381
class npc_dalaran_screek : public CreatureScript
{
    public:
        npc_dalaran_screek() : CreatureScript("npc_dalaran_screek")
        {
        }

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_dalaran_screekAI(creature);
        }

        struct npc_dalaran_screekAI : public ScriptedAI
        {
            npc_dalaran_screekAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            enum eData
            {
                EventDrawBlood        = 1,
                SpellDrawBlood        = 184371, ///< Self, stealth
                EventHowlingScreech   = 2,
                SpellHowlingScreech   = 32651,  ///< Target speed debuff and self damage buff
                EventHuntersRush      = 3,
                SpellHuntersRush      = 223971,
                EventNightglowWisp    = 4,
                SpellNightglowWisp    = 225832,
                EventFocusedGust      = 5,
                SpellFocusedGust      = 217340
            };

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(eData::EventDrawBlood, 10000);
                events.ScheduleEvent(eData::EventHowlingScreech, 20000);
                events.ScheduleEvent(eData::EventHuntersRush, 30000);
                events.ScheduleEvent(eData::EventNightglowWisp, 35000);
                events.ScheduleEvent(eData::EventFocusedGust, 40000);
            }

            void JustDied(Unit* p_Killer) override
            {
                ZoneScript* l_ZoneScript = sOutdoorPvPMgr->GetOutdoorPvPToZoneId(eGpsData::DalaranZoneID);

                if (l_ZoneScript == nullptr)
                    return;

                if (OutdoorPvPDalaran* l_Dalaran = (OutdoorPvPDalaran*)l_ZoneScript)
                    l_Dalaran->RareCreatureKilled(me->GetEntry(), me->GetGUID());
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EventDrawBlood:
                        me->CastSpell(me, SpellDrawBlood, false);
                        events.ScheduleEvent(EventDrawBlood, 45000);
                        break;
                    case EventHowlingScreech:
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            me->CastSpell(l_Target, SpellHowlingScreech, false);
                        events.ScheduleEvent(EventDrawBlood, 45000);
                        break;
                    case EventHuntersRush:
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            me->CastSpell(l_Target, SpellHuntersRush, false);
                        events.ScheduleEvent(EventDrawBlood, 45000);
                        break;
                    case EventNightglowWisp:
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            me->CastSpell(l_Target, SpellNightglowWisp, false);
                        events.ScheduleEvent(EventNightglowWisp, 45000);
                        break;
                    case EventFocusedGust:
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            me->CastSpell(l_Target, SpellFocusedGust, false);
                        events.ScheduleEvent(EventFocusedGust, 45000);
                        break;
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };
};

/// Segacedi - 97384
class npc_dalaran_segacedi : public CreatureScript
{
    public:
        npc_dalaran_segacedi() : CreatureScript("npc_dalaran_segacedi")
        {
        }

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_dalaran_segacediAI(creature);
        }

        struct npc_dalaran_segacediAI : public ScriptedAI
        {
            npc_dalaran_segacediAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            enum eData
            {
                EventBloodFrenzy     = 1,
                SpellBloodFrenzy     = 216282,
                EventBloodInTheWater = 2,
                SpellBloodInTheWater = 216263,
                EventHuntersRush     = 3,
                SpellHuntersRush     = 223971
            };

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(eData::EventBloodFrenzy, 10000);
                events.ScheduleEvent(eData::EventBloodInTheWater, 20000);
                events.ScheduleEvent(eData::EventHuntersRush, 30000);
            }

            void JustDied(Unit* p_Killer) override
            {
                ZoneScript* l_ZoneScript = sOutdoorPvPMgr->GetOutdoorPvPToZoneId(eGpsData::DalaranZoneID);

                if (l_ZoneScript == nullptr)
                    return;

                if (OutdoorPvPDalaran* l_Dalaran = (OutdoorPvPDalaran*)l_ZoneScript)
                    l_Dalaran->RareCreatureKilled(me->GetEntry(), me->GetGUID());
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EventBloodFrenzy:
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            me->CastSpell(l_Target, SpellBloodFrenzy, false);
                        events.ScheduleEvent(EventBloodFrenzy, 45000);
                        break;
                    case EventBloodInTheWater:
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            me->CastSpell(l_Target, SpellBloodInTheWater, false);
                        events.ScheduleEvent(EventBloodInTheWater, 45000);
                        break;
                    case EventHuntersRush:
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            me->CastSpell(l_Target, SpellHuntersRush, false);
                        events.ScheduleEvent(EventHuntersRush, 45000);
                        break;
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };
};

/// Rotten Egg - 97589
class npc_dalaran_rotten_egg : public CreatureScript
{
    public:
        npc_dalaran_rotten_egg() : CreatureScript("npc_dalaran_rotten_egg")
        {
        }

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_dalaran_rotten_eggAI(creature);
        }

        struct npc_dalaran_rotten_eggAI : public ScriptedAI
        {
            npc_dalaran_rotten_eggAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            enum eData
            {
                NpcManaEngorgedSpiderling = 107778
            };

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_REMOVE_CLIENT_CONTROL);
            }

            void JustDied(Unit* p_Killer) override
            {
                for (int l_Itr = 0; l_Itr < 35; ++l_Itr)
                {
                    Position l_Pos = *me;
                    float l_Radius = frand(0.0f, 15.0f);
                    float l_Angle  = frand(0.0f, M_PI * 2);

                    l_Pos.m_positionX += l_Radius * cos(l_Angle);
                    l_Pos.m_positionY += l_Radius * sin(l_Angle);

                    me->SummonCreature(eData::NpcManaEngorgedSpiderling, l_Pos);
                }

                ZoneScript* l_ZoneScript = sOutdoorPvPMgr->GetOutdoorPvPToZoneId(eGpsData::DalaranZoneID);

                if (l_ZoneScript == nullptr)
                    return;

                if (OutdoorPvPDalaran* l_Dalaran = (OutdoorPvPDalaran*)l_ZoneScript)
                    l_Dalaran->RareCreatureKilled(me->GetEntry(), me->GetGUID());
            }
        };
};

/// Mana Engorged Spiderling - 107778
class npc_dalaran_mana_engorged_spiderling : public CreatureScript
{
    public:
        npc_dalaran_mana_engorged_spiderling() : CreatureScript("npc_dalaran_mana_engorged_spiderling")
        {
        }

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_dalaran_mana_engorged_spiderlingAI(creature);
        }

        struct npc_dalaran_mana_engorged_spiderlingAI : public ScriptedAI
        {
            npc_dalaran_mana_engorged_spiderlingAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            uint32 m_DespawnTimer = 5 * IN_MILLISECONDS * MINUTE;

            enum eData
            {
                EventEnchantedVenom = 1,
                SpellEnchantedVenom = 140541,
                EventTheMawMustFeed = 2,
                SpellTheMawMustFeed = 215377 ///< Self
            };

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                events.ScheduleEvent(eData::EventEnchantedVenom, 5000);
                events.ScheduleEvent(eData::EventTheMawMustFeed, 10000);
            }

            void UpdateAI(const uint32 p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EventEnchantedVenom:
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            me->CastSpell(l_Target, SpellEnchantedVenom, false);
                        events.ScheduleEvent(EventEnchantedVenom, 45000);
                        break;
                    case EventTheMawMustFeed:
                        me->CastSpell(me, SpellTheMawMustFeed, false);
                        events.ScheduleEvent(EventTheMawMustFeed, 45000);
                        break;
                    default:
                        break;
                }

                if (m_DespawnTimer)
                {
                    if (m_DespawnTimer <= p_Diff)
                    {
                        m_DespawnTimer = 0;
                        me->DespawnOrUnsummon();
                    }
                    else
                        m_DespawnTimer -= p_Diff;
                }

                DoMeleeAttackIfReady();
            }
        };
};


/// Raethan - 97359
class npc_dalaransewers_raethan : public CreatureScript
{
    public:
        npc_dalaransewers_raethan() : CreatureScript("npc_dalaransewers_raethan")
        {
        }

        enum eQuests
        {
            TheInkFlows = 39948,
            DeliverContractKillCredit = 97359
        };

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

            if (p_Player->HasQuest(eQuests::TheInkFlows)) /// Quest "The Ink Flow"
            {
                p_Player->CompleteQuest(eQuests::DeliverContractKillCredit);
            }

            if (!p_Player->HasAura(eDalaranSewersSpells::AuraHiredGuard))
                p_Player->ADD_GOSSIP_ITEM_DB(999153, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

            ZoneScript* l_ZoneScript = sOutdoorPvPMgr->GetOutdoorPvPToZoneId(eGpsData::DalaranZoneID);

            if (l_ZoneScript == nullptr)
                return false;

            if (OutdoorPvPDalaran* l_Dalaran = (OutdoorPvPDalaran*)l_ZoneScript)
            {
                if (!l_Dalaran->CanSwitchFFA() && p_Player->GetSession()->GetSecurity() < SEC_ADMINISTRATOR)
                {
                    p_Player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, p_Creature->GetGUID());
                    return true;
                }

                if (l_Dalaran->GetFFAState() == false) ///< Deactivated
                    p_Player->ADD_GOSSIP_ITEM_DB(999153, 1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                else
                    p_Player->ADD_GOSSIP_ITEM_DB(999153, 2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            }

            p_Player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, p_Creature->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* p_Player, Creature* p_Creature, uint32 p_Sender, uint32 p_Action) override
        {
            ZoneScript* l_ZoneScript = sOutdoorPvPMgr->GetOutdoorPvPToZoneId(eGpsData::DalaranZoneID);

            if (l_ZoneScript == nullptr)
            {
                p_Player->PlayerTalkClass->ClearMenus();
                p_Player->PlayerTalkClass->SendCloseGossip();
                return false;
            }

            switch (p_Action)
            {
                case GOSSIP_ACTION_TRADE:
                    p_Player->GetSession()->SendListInventory(p_Creature->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF + 1: ///< Hire bodyguard
                    p_Player->CastSpell(p_Player, eDalaranSewersSpells::SpellSummonDalaranSewerBodyguard, true);
                    p_Player->AddAura(eDalaranSewersSpells::AuraHiredGuard, p_Player);
                    break;
                case GOSSIP_ACTION_INFO_DEF + 2: ///< Activate FFA
                    if (p_Player->GetCurrency(CurrencyTypes::CURRENCY_TYPE_SLIGHTLESS_EYE, false) >= 50)
                    {
                        if (OutdoorPvPDalaran* l_Dalaran = (OutdoorPvPDalaran*)l_ZoneScript)
                        {
                            if (!l_Dalaran->CanSwitchFFA() && p_Player->GetSession()->GetSecurity() < SEC_ADMINISTRATOR)
                                break;

                            if (p_Creature->AI())
                                p_Creature->AI()->Talk(0);

                            p_Player->ModifyCurrency(CurrencyTypes::CURRENCY_TYPE_SLIGHTLESS_EYE, -50);
                            l_Dalaran->SwitchFFAState(true);
                        }
                    }
                    else
                        p_Player->GetSession()->SendNotification(LANG_NOT_ENOUGH_CURRENCY);
                    break;
                case GOSSIP_ACTION_INFO_DEF + 3: ///< Deactivate FFA
                    if (p_Player->GetCurrency(CurrencyTypes::CURRENCY_TYPE_SLIGHTLESS_EYE, false) >= 50)
                    {
                        if (OutdoorPvPDalaran* l_Dalaran = (OutdoorPvPDalaran*)l_ZoneScript)
                        {
                            if (!l_Dalaran->CanSwitchFFA() && p_Player->GetSession()->GetSecurity() < SEC_ADMINISTRATOR)
                                break;

                            if (p_Creature->AI())
                                p_Creature->AI()->Talk(1);

                            p_Player->ModifyCurrency(CurrencyTypes::CURRENCY_TYPE_SLIGHTLESS_EYE, -50);
                            l_Dalaran->SwitchFFAState(false);
                        }
                    }
                    else
                        p_Player->GetSession()->SendNotification(LANG_NOT_ENOUGH_CURRENCY);
                    break;
                default:
                    break;
            }

            p_Player->PlayerTalkClass->SendCloseGossip();
            p_Player->PlayerTalkClass->ClearMenus();

            return true;
        }
};

/// Dalaran Well - 242218
class go_dalaran_sewers_well : public GameObjectScript
{
    public:
        /// Constructor
        go_dalaran_sewers_well() : GameObjectScript("go_dalaran_sewers_well") { }

        struct go_dalaran_sewers_wellAI : public GameObjectAI
        {
            /// Constructor
            go_dalaran_sewers_wellAI(GameObject* p_GameObject) : GameObjectAI(p_GameObject) { }

            std::array<uint32, 6> m_Quests = { { 42527, 42528, 42529, 42530 , 42531, 42532 } };

            /// Some static positions here might be missing
            Position m_DefaultRandomPos = { -769.95f, 4494.9282f, 702.788f, 4.0f };

            std::array<Position, 5> m_ItemPos =
            {
                {
                    { -851.4696f, 4401.5645f, 717.0493f, 0.0f }, ///< Guard Station - 42527
                    { -721.2028f, 4390.5591f, 697.2152f, 0.0f }, ///< Black Market  - 42528
                    { -842.2610f, 4466.7739f, 696.0209f, 0.0f }, ///< Inn Entrance  - 42529
                    { -905.7940f, 4367.1440f, 694.2420f, 0.0f }, ///< Alchemy       - 42530
                    { -791.8989f, 4539.1987f, 696.6593f, 0.0f }  ///< Rear Entrance - 42532
                }
            };

            void UpdateAI(uint32 p_Diff) override
            {
                std::list<Player*> l_PlayerList;
                GetPlayerListInGrid(l_PlayerList, go, 2.0f);

                for (Player* l_Player : l_PlayerList)
                {
                    std::vector<Position> l_PoolPositions;

                    l_PoolPositions.push_back(m_DefaultRandomPos);

                    for (uint8 l_Itr = 0; l_Itr < m_ItemPos.size(); ++l_Itr)
                    {
                        if (l_Player->GetQuestStatus(m_Quests[l_Itr]) == QUEST_STATUS_REWARDED)
                            l_PoolPositions.push_back(m_ItemPos[l_Itr]);
                    }

                    Position const l_Position = l_PoolPositions[urand(0, l_PoolPositions.size() - 1)];
                    l_Player->TeleportTo(eGpsData::DalaranSewersMapID, l_Position);
                }
            }
        };

        /// Called when a GameObjectAI object is needed for the GameObject.
        /// @p_GameObject : GameObject instance
        GameObjectAI* GetAI(GameObject* p_GameObject) const override
        {
            return new go_dalaran_sewers_wellAI(p_GameObject);
        }
};

/// Playing With Fire (aura : 214004)
class spell_aura_dalaran_playing_with_fire : public SpellScriptLoader
{
    public:
        spell_aura_dalaran_playing_with_fire() : SpellScriptLoader("spell_aura_dalaran_playing_with_fire") { }

        class spell_aura_dalaran_playing_with_fire_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_aura_dalaran_playing_with_fire_AuraScript);

            enum eData
            {
                SpellPlayingWithFireMissile = 214005
            };

            void OnUpdate(uint32 p_Diff)
            {
                WorldObject* l_Owner = GetOwner();

                if (l_Owner != nullptr && l_Owner->GetTypeId() == TYPEID_UNIT)
                {
                    l_Owner->ToCreature()->CastSpell(l_Owner, eData::SpellPlayingWithFireMissile, true);

                    if (urand(0, 1))
                        l_Owner->ToCreature()->CastSpell(l_Owner, eData::SpellPlayingWithFireMissile, true);
                }
            }

            void Register() override
            {
                OnAuraUpdate += AuraUpdateFn(spell_aura_dalaran_playing_with_fire_AuraScript::OnUpdate);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_aura_dalaran_playing_with_fire_AuraScript();
        }
};

class npc_dalaran_sewers_guard : public CreatureScript
{
    public:
        npc_dalaran_sewers_guard() : CreatureScript("npc_dalaran_sewers_guard") { }

        struct npc_dalaran_sewers_guardAI : public ScriptedAI
        {
            npc_dalaran_sewers_guardAI(Creature *creature) : ScriptedAI(creature) {}

            void Reset() override
            {
                me->SetReactState(REACT_DEFENSIVE);

                if (Unit* l_Owner = me->GetAnyOwner())
                {
                    if (l_Owner->getVictim() || l_Owner->getAttackerForHelper())
                        AttackStart(l_Owner->getVictim() ? l_Owner->getVictim() : l_Owner->getAttackerForHelper());

                    if (l_Owner->GetMapId() == eGpsData::DalaranSewersMapID)
                        l_Owner->RemoveByteFlag(UNIT_FIELD_SHAPESHIFT_FORM, UNIT_BYTES_2_OFFSET_PVP_FLAG, UNIT_BYTE2_FLAG_PVP | UNIT_BYTE2_FLAG_FFA_PVP);
                }
            }

            void JustDespawned() override
            {
                if (Unit* l_Owner = me->GetAnyOwner())
                {
                    l_Owner->RemoveAura(eDalaranSewersSpells::AuraHiredGuard);

                    ZoneScript* l_ZoneScript = sOutdoorPvPMgr->GetOutdoorPvPToZoneId(eGpsData::DalaranZoneID);

                    if (l_ZoneScript == nullptr)
                        return;

                    if (OutdoorPvPDalaran* l_Dalaran = (OutdoorPvPDalaran*)l_ZoneScript)
                    {
                        if (l_Dalaran->GetFFAState() && l_Owner->GetMapId() == eGpsData::DalaranSewersMapID)
                            l_Owner->SetByteFlag(UNIT_FIELD_SHAPESHIFT_FORM, UNIT_BYTES_2_OFFSET_PVP_FLAG, UNIT_BYTE2_FLAG_PVP | UNIT_BYTE2_FLAG_FFA_PVP);
                    }
                }
            }

            void UpdateAI(const uint32 p_Diff) override
            {
                if (me->GetReactState() != REACT_DEFENSIVE)
                    me->SetReactState(REACT_DEFENSIVE);

                if (!UpdateVictim())
                {
                    if (Unit* l_Owner = me->GetOwner())
                    {
                        if (Unit* l_NewVictim = l_Owner->getAttackerForHelper())
                            AttackStart(l_NewVictim);
                    }
                }

                if (me->getVictim())
                {
                    if (Unit* l_Owner = me->GetOwner())
                    {
                        if (Unit* l_OwnerVictim = l_Owner->getAttackerForHelper())
                        {
                            if (me->getVictim()->GetGUID() != l_OwnerVictim->GetGUID())
                                AttackStart(l_OwnerVictim);
                        }
                    }
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_dalaran_sewers_guardAI(creature);
        }
};

/// Hired Guard - 203894
class spell_aura_dalaran_hired_guard : public SpellScriptLoader
{
    public:
        spell_aura_dalaran_hired_guard() : SpellScriptLoader("spell_aura_dalaran_hired_guard") { }

        class spell_aura_dalaran_hired_guard_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_aura_dalaran_hired_guard_AuraScript);

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Owner = GetUnitOwner())
                {
                    if (l_Owner->IsPlayer())
                        l_Owner->ToPlayer()->RemoveAllMinionsByEntry(eCreatures::NpcDalaranBodyGuard);
                }
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_aura_dalaran_hired_guard_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_aura_dalaran_hired_guard_AuraScript();
        }
};

/// Widowsister Contract - 220266
class spell_dalaran_widowsister_contract : public SpellScriptLoader
{
    public:
        spell_dalaran_widowsister_contract() : SpellScriptLoader("spell_dalaran_widowsister_contract") { }

        class spell_dalaran_widowsister_contract_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dalaran_widowsister_contract_SpellScript);

            std::array<Position, 4> const m_SpawnPos =
            {
                {
                    { -804.3669f, 4415.0986f, 711.2101f, 0.0f },
                    { -726.1672f, 4394.4077f, 697.2934f, 0.0f },
                    { -796.5510f, 4458.8447f, 703.4780f, 0.0f },
                    { -821.9622f, 4475.1167f, 696.7036f, 0.0f }
                }
            };

            enum eData
            {
                SummonEntry = 110737 ///< Widowsister
            };

            SpellCastResult HandleCheckCast()
            {
                ZoneScript* l_ZoneScript = sOutdoorPvPMgr->GetOutdoorPvPToZoneId(eGpsData::DalaranZoneID);

                if (l_ZoneScript == nullptr)
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                if (OutdoorPvPDalaran* l_Dalaran = (OutdoorPvPDalaran*)l_ZoneScript)
                {
                    if (l_Dalaran->GetItemTempSummonTimer())
                        return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
                }

                return SPELL_CAST_OK;
            }

            void HandleDummy(SpellEffIndex /*index*/)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr || !l_Caster->IsPlayer())
                    return;

                for (Position l_Pos : m_SpawnPos)
                    l_Caster->SummonCreature(eData::SummonEntry, l_Pos);

                ZoneScript* l_ZoneScript = sOutdoorPvPMgr->GetOutdoorPvPToZoneId(eGpsData::DalaranZoneID);

                if (l_ZoneScript == nullptr)
                    return;

                if (OutdoorPvPDalaran* l_Dalaran = (OutdoorPvPDalaran*)l_ZoneScript)
                    l_Dalaran->SetItemTempSummonTimer();
            }

            void Register()
            {
                OnEffectLaunch += SpellEffectFn(spell_dalaran_widowsister_contract_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnCheckCast += SpellCheckCastFn(spell_dalaran_widowsister_contract_SpellScript::HandleCheckCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dalaran_widowsister_contract_SpellScript();
        }
};

/// Screecher Whistle - 220260
class spell_dalaran_screecher_whistle : public SpellScriptLoader
{
    public:
        spell_dalaran_screecher_whistle() : SpellScriptLoader("spell_dalaran_screecher_whistle") { }

        class spell_dalaran_screecher_whistle_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dalaran_screecher_whistle_SpellScript);

            std::array<Position, 4> const m_SpawnPos =
            {
                {
                    { -825.7399f, 4503.5537f, 699.7780f, 0.0f },
                    { -822.8644f, 4506.4692f, 699.7780f, 0.0f },
                    { -816.2800f, 4503.4404f, 699.7780f, 0.0f },
                    { -820.4978f, 4500.5811f, 697.5479f, 0.0f }
                }
            };

            enum eData
            {
                SummonEntry = 110729 ///< Sewer Screecher
            };

            SpellCastResult HandleCheckCast()
            {
                ZoneScript* l_ZoneScript = sOutdoorPvPMgr->GetOutdoorPvPToZoneId(eGpsData::DalaranZoneID);

                if (l_ZoneScript == nullptr)
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                if (OutdoorPvPDalaran* l_Dalaran = (OutdoorPvPDalaran*)l_ZoneScript)
                {
                    if (l_Dalaran->GetItemTempSummonTimer())
                        return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
                }

                return SPELL_CAST_OK;
            }

            void HandleDummy(SpellEffIndex /*index*/)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr || !l_Caster->IsPlayer())
                    return;

                for (Position l_Pos : m_SpawnPos)
                    l_Caster->SummonCreature(eData::SummonEntry, l_Pos);

                ZoneScript* l_ZoneScript = sOutdoorPvPMgr->GetOutdoorPvPToZoneId(eGpsData::DalaranZoneID);

                if (l_ZoneScript == nullptr)
                    return;

                if (OutdoorPvPDalaran* l_Dalaran = (OutdoorPvPDalaran*)l_ZoneScript)
                    l_Dalaran->SetItemTempSummonTimer();
            }

            void Register()
            {
                OnEffectLaunch += SpellEffectFn(spell_dalaran_screecher_whistle_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnCheckCast += SpellCheckCastFn(spell_dalaran_screecher_whistle_SpellScript::HandleCheckCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dalaran_screecher_whistle_SpellScript();
        }
};

/// Imp-Binding Contract - 220265
class spell_dalaran_imp_binding_contract : public SpellScriptLoader
{
    public:
        spell_dalaran_imp_binding_contract() : SpellScriptLoader("spell_dalaran_imp_binding_contract") { }

        class spell_dalaran_imp_binding_contract_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dalaran_imp_binding_contract_SpellScript);

            std::array<Position, 6> const m_SpawnPos =
            {
                {
                    { -790.7942f, 4449.8867f, 704.3280f, 0.0f },
                    { -776.6133f, 4452.3784f, 702.8268f, 0.0f },
                    { -779.0182f, 4483.6665f, 701.9518f, 0.0f },
                    { -804.1818f, 4432.1816f, 708.8354f, 0.0f },
                    { -804.2693f, 4414.6733f, 711.2598f, 0.0f },
                    { -800.3973f, 4479.2261f, 699.4231f, 0.0f }
                }
            };

            enum eData
            {
                SummonEntry = 110721 ///< Unamused Imp
            };

            SpellCastResult HandleCheckCast()
            {
                ZoneScript* l_ZoneScript = sOutdoorPvPMgr->GetOutdoorPvPToZoneId(eGpsData::DalaranZoneID);

                if (l_ZoneScript == nullptr)
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                if (OutdoorPvPDalaran* l_Dalaran = (OutdoorPvPDalaran*)l_ZoneScript)
                {
                    if (l_Dalaran->GetItemTempSummonTimer())
                        return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
                }

                return SPELL_CAST_OK;
            }

            void HandleDummy(SpellEffIndex /*index*/)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr || !l_Caster->IsPlayer())
                    return;

                for (Position l_Pos : m_SpawnPos)
                    l_Caster->SummonCreature(eData::SummonEntry, l_Pos);

                ZoneScript* l_ZoneScript = sOutdoorPvPMgr->GetOutdoorPvPToZoneId(eGpsData::DalaranZoneID);

                if (l_ZoneScript == nullptr)
                    return;

                if (OutdoorPvPDalaran* l_Dalaran = (OutdoorPvPDalaran*)l_ZoneScript)
                    l_Dalaran->SetItemTempSummonTimer();
            }

            void Register()
            {
                OnEffectLaunch += SpellEffectFn(spell_dalaran_imp_binding_contract_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnCheckCast += SpellCheckCastFn(spell_dalaran_imp_binding_contract_SpellScript::HandleCheckCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dalaran_imp_binding_contract_SpellScript();
        }
};

/// Croc Flusher - 220253
class spell_dalaran_croc_flusher : public SpellScriptLoader
{
    public:
        spell_dalaran_croc_flusher() : SpellScriptLoader("spell_dalaran_croc_flusher") { }

        class spell_dalaran_croc_flusher_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dalaran_croc_flusher_SpellScript);

            std::array<Position, 3> const m_SpawnPos =
            {
                {
                    { -885.6231f, 4454.3833f, 705.6115f, 5.26f },
                    { -876.6485f, 4468.4370f, 706.3830f, 5.26f },
                    { -868.2581f, 4475.5718f, 705.6415f, 5.26f }
                }
            };

            enum eData
            {
                SummonEntry = 110719 ///< Underbelly Croc
            };

            SpellCastResult HandleCheckCast()
            {
                ZoneScript* l_ZoneScript = sOutdoorPvPMgr->GetOutdoorPvPToZoneId(eGpsData::DalaranZoneID);

                if (l_ZoneScript == nullptr)
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                if (OutdoorPvPDalaran* l_Dalaran = (OutdoorPvPDalaran*)l_ZoneScript)
                {
                    if (l_Dalaran->GetItemTempSummonTimer())
                        return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
                }

                return SPELL_CAST_OK;
            }

            void HandleDummy(SpellEffIndex /*index*/)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr || !l_Caster->IsPlayer())
                    return;

                for (Position l_Pos : m_SpawnPos)
                    l_Caster->SummonCreature(eData::SummonEntry, l_Pos);

                ZoneScript* l_ZoneScript = sOutdoorPvPMgr->GetOutdoorPvPToZoneId(eGpsData::DalaranZoneID);

                if (l_ZoneScript == nullptr)
                    return;

                if (OutdoorPvPDalaran* l_Dalaran = (OutdoorPvPDalaran*)l_ZoneScript)
                    l_Dalaran->SetItemTempSummonTimer();
            }

            void Register()
            {
                OnEffectLaunch += SpellEffectFn(spell_dalaran_croc_flusher_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnCheckCast += SpellCheckCastFn(spell_dalaran_croc_flusher_SpellScript::HandleCheckCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dalaran_croc_flusher_SpellScript();
        }
};

/// Widowsister - 110737
class npc_dalaran_widowsister : public CreatureScript
{
    public:
        npc_dalaran_widowsister() : CreatureScript("npc_dalaran_widowsister")
        {
        }

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_dalaran_widowsisterAI(creature);
        }

        struct npc_dalaran_widowsisterAI : public ScriptedAI
        {
            npc_dalaran_widowsisterAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            enum eData
            {
                SpellAuraStealth      = 34189,
                SpellAuraTasteOfBlood = 213738,
                SpellTheMaxMustFeed   = 215377
            };

            void EnterCombat(Unit* p_Attacker) override
            {
                me->AddAura(eData::SpellAuraStealth, me);
                me->AddAura(eData::SpellAuraTasteOfBlood, me);
                me->AddAura(eData::SpellTheMaxMustFeed, me);
            }
        };
};

/// Sewer Screecher - 110729
class npc_dalaran_sewer_screecher : public CreatureScript
{
    public:
        npc_dalaran_sewer_screecher() : CreatureScript("npc_dalaran_sewer_screecher")
        {
        }

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_dalaran_sewer_screecherAI(creature);
        }

        struct npc_dalaran_sewer_screecherAI : public ScriptedAI
        {
            npc_dalaran_sewer_screecherAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            enum eData
            {
                SpellHuntersRush = 223971
            };

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->GetMotionMaster()->MoveCharge(-845.2622f, 4474.4619f, 696.6650f);
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                me->AddAura(eData::SpellHuntersRush, me);
            }
        };
};

/// Unamused Imp - 110721
class npc_dalaran_unamused_imp : public CreatureScript
{
    public:
        npc_dalaran_unamused_imp() : CreatureScript("npc_dalaran_unamused_imp")
        {
        }

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_dalaran_unamused_impAI(creature);
        }

        struct npc_dalaran_unamused_impAI : public ScriptedAI
        {
            npc_dalaran_unamused_impAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            enum eData
            {
                SpellAuraTheMawMustFeed = 215377,
                EventFirebolt           = 1,
                SpellFirebolt           = 220151
            };

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                me->AddAura(eData::SpellAuraTheMawMustFeed, me);
                events.ScheduleEvent(eData::EventFirebolt, 7000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (events.ExecuteEvent() == eData::EventFirebolt)
                {
                    if (Unit* l_Target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                        me->CastSpell(l_Target, eData::SpellFirebolt, false);

                    events.ScheduleEvent(eData::EventFirebolt, 6000);
                }

                DoMeleeAttackIfReady();
            }
        };
};

/// Underbelly Croc - 110719
class npc_dalaran_underbelly_croc : public CreatureScript
{
    public:
        npc_dalaran_underbelly_croc() : CreatureScript("npc_dalaran_underbelly_croc")
        {
        }

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_dalaran_underbelly_crocAI(creature);
        }

        struct npc_dalaran_underbelly_crocAI : public ScriptedAI
        {
            npc_dalaran_underbelly_crocAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            enum eData
            {
                SpellHuntersRush = 223971
            };

            void EnterCombat(Unit* p_Attacker) override
            {
                me->AddAura(eData::SpellHuntersRush, me);
            }
        };
};

/// Ratstallion Crate - 110743
class npc_dalaran_ratstallion_crate : public CreatureScript
{
    public:
        npc_dalaran_ratstallion_crate() : CreatureScript("npc_dalaran_ratstallion_crate")
        {
        }

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_dalaran_ratstallion_crateAI(creature);
        }

        struct npc_dalaran_ratstallion_crateAI : public ScriptedAI
        {
            npc_dalaran_ratstallion_crateAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            uint32 const m_Aura = 220123;
            uint32 m_BuffCounter = 25;

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->setFaction(FACTION_FRIENDLY);
            }

            void sGossipHello(Player* p_Clicker) override
            {
                if (!p_Clicker->HasAura(m_Aura) && !p_Clicker->IsOnVehicle())
                {
                    p_Clicker->CastSpell(p_Clicker, m_Aura, true);

                    if (!--m_BuffCounter)
                        me->DespawnOrUnsummon();
                }
            }
        };
};
/// Dalaran Sewers spawn manager - 300007
class npc_dalaran_sewers_spawn_manager : public CreatureScript
{
    public:
        npc_dalaran_sewers_spawn_manager() : CreatureScript("npc_dalaran_sewers_spawn_manager")
        {
        }

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_dalaran_sewers_spawn_managerAI(creature);
        }

        struct npc_dalaran_sewers_spawn_managerAI : public ScriptedAI
        {
            npc_dalaran_sewers_spawn_managerAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            void Reset() override
            {
                ZoneScript* l_ZoneScript = sOutdoorPvPMgr->GetOutdoorPvPToZoneId(eGpsData::DalaranZoneID);

                if (l_ZoneScript == nullptr)
                    return;

                if (OutdoorPvPDalaran* l_Dalaran = (OutdoorPvPDalaran*)l_ZoneScript)
                    l_Dalaran->SetControllerGUID(me->GetGUID());
            }
        };
};

#ifndef __clang_analyzer__
void AddSC_DalaranSewersScripts()
{
    /// Spells
    new spell_dalaran_throwing_stars();
    new spell_aura_dalaran_playing_with_fire();
    new spell_aura_dalaran_hired_guard();
    new spell_dalaran_widowsister_contract();
    new spell_dalaran_screecher_whistle();
    new spell_dalaran_imp_binding_contract();
    new spell_dalaran_croc_flusher();

    /// Rare Mobs
    new npc_dalaran_crazed_mage();
    new npc_dalaran_thieving_scoundrel();
    new npc_dalaran_xulorax();
    new npc_dalaran_splint();
    new npc_dalaran_mana_seeper();
    new npc_dalaran_screek();
    new npc_dalaran_segacedi();
    new npc_dalaran_rotten_egg();

    /// Farm Mobs
    new npc_dalaran_widowsister();
    new npc_dalaran_sewer_screecher();
    new npc_dalaran_unamused_imp();
    new npc_dalaran_underbelly_croc();

    /// Adds
    new npc_dalaran_mana_engorged_spiderling();

    /// Misc
    new npc_dalaransewers_raethan();
    new npc_dalaran_sewers_guard();
    new npc_dalaran_ratstallion_crate();
    new npc_dalaran_sewers_spawn_manager();

    /// Game Objects
    new go_dalaran_sewers_well();
}
#endif
