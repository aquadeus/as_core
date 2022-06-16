////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "the_nighthold.hpp"
#include "G3D/LineSegment.h"

enum eSpells
{
    /// Enchacements
    SpellIcyEnchantment         = 213864,
    SpellFieryEnchantment       = 213867,
    SpellMagicEnchantment       = 213869,

    SpellAnnihilate             = 212492,
    SpellAnnihilateDamage       = 212494,
    SpellAnnihilateDebuff       = 215458,
    SpellFelSoul                = 230901,

    /// Frost
    SpellMarkOfFrostAoe         = 212646,
    SpellMarkOfFrostFixate      = 212531,
    SpellMarkOfFrostDot         = 212587,
    SpellMarkOfFrostDotDmg      = 213624,
    SpellMarkOfFrostExplosion   = 212648,
    SpellMarkOfFrostFriendly    = 233195,
    SpellMarkOfFrostAt			= 212738,
    SpellFrostbitten            = 212647,
    SpellMarkOfFrostKnockBack   = 212649,
    SpellReplicateMarkOfFrost   = 212530,

    SpellDetonateMarkOfFrostAoe = 212735,
    SpellDetonateMarkOfFrostAt  = 212734,

    SpellAnimateMarkOfFrostAoe  = 213853,
    SpellAnimateMarkOfFrostAdd  = 213854,

    /// Fire
    SpellSearingBrandAoe        = 213145,
    SpellSearingBrandFixate     = 213148,
    SpellSearingBrandDot        = 213166,
    SpellSearingBrandAt         = 213179,
    SpellSearingBrandCharge     = 213157,
    SpellSearingBrandChargeDest = 214341,
    SpellBlazingCharge          = 233098,

    SpellReplicateSearingBrandAoe   = 213182,

    SpellDetonateSearingBrandAoe= 213275,
    SpellDetonateSearingBrandAt = 213276,

    SpellAnimateSearingBrandAoe = 213567,
    SpellAnimateSearingBrandAdd = 213566,

    /// Arcane
    SpellArcaneOrbAoe           = 215456,
    SpellArcaneOrbMissile       = 213510,
    SpellArcaneOrbDot           = 213519,
    SpellReplicateArcaneOrbAoe  = 213852,
    SpellDetonateArcaneOrbAoe   = 213390,
    SpellDetonateArcaneOrbDummy = 213377,
    SpellDetonateArcaneOrbAt    = 213326,
    SpellAnimateArcaneOrbAoe    = 213564,
    SpellAnimateArcaneOrbAdd    = 213565,
    SpellArcaneFog              = 213504,

    /// Adds

    /// Icy Enchantment
    SpellBurstOfIce             = 213796,
    SpellFrozenTempestTeleport  = 213130,
    SpellFrozenTempest          = 213083,
    SpellFrozenTempestDot       = 213085,
    SpellEntombedInIce          = 213622,

    /// Fiery Enchantment
    SpellBurstOfFlame           = 213760,
    SpellBoundInFlameAoe        = 216257,
    SpellBoundInFlame           = 216256,
    SpellPyroblast              = 213281,

    /// Arcane Orb
    SpellArmageddon             = 213569,
    SpellArmageddonDmg          = 213568,
    SpellBurstOfMagic           = 213808,

    /// Fel Soul
    SpellDecimate               = 230504,
    SpellWhirlingBarrage        = 230416,
    SpellSeveredSoul            = 230981,
    SpellFelStompAoe            = 230411,
    SpellFelStomp               = 230413,
    SpellFelStompAt             = 230412,
    SpellFelLash                = 230403,
};

enum eEvents
{
    EventAnnihilate = 1,
    EventBerserk,

    /// Frost
    EventMarkOfFrost,
    EventReplicateMarkOfFrost,
    EventDetonateMarkOfFrost,
    EventAnimateMarkOfFrost,

    /// Fire
    EventSearingBrand,
    EventReplicateSearingBrand,
    EventReplicateSearingBrand2,
    EventDetonateSearingBrand,
    EventAnimateSearingBrand,

    /// Arcane
    EventArcaneOrb,
    EventReplicateArcaneOrb,
    EventDetonateArcaneOrb,
    EventDetonateArcaneOrbFly,
    EventAnimateArcaneOrb,
    EventAnimateDespawn,

    /// Heroic
    EventSumFelSoul,

    /// Ice
    EventFrozenTempest,

    /// Fiery Enchantmend Add
    EventBoundInFlame,
    EventPyroblast,

    /// Fel Soul
    EventDecimate,
    EventWhirlingBarrage,
    EventFelStomp,
    EventFelLash,
    EventCheckDist,
    EventCheckAchievement,
};

enum eTalks
{
    TalkIntro   = 0,
    TalkAggro,
    TalkAnnihilate,
    TalkMarkOfFrost,
    TalkArcaneOrb,
    TalkReplicate,
    TalkDetonate,
    TalkAnimate,
    TalkRandom,
    TalkKill,
    TalkWipe,
    TalkDeath
};

enum ePoints
{
    PointFlyPointReached    = 10,
    PointEarthPointReached  = 11,
};

enum ePhases
{
    IcePhase    = 1,
    FirePhase,
    ArcanePhase,
};

enum eActionsAluriel
{
    ActionStopLook,
    ActionIcyEnchantment,
};

enum eDataAluriel
{
  DataArcaneOrbs    = 1,
  DataMarkedPlayers,
  DataUnMarkedPlayers,
  DataSearingBrand,
  DataDetonateTimer,
  DataFelSoulHp,
};

enum eAchievementAreaIds
{
    AreaShaldoreiTerrace    = 8348,
    AreaAstromancerRise     = 8389,
    AreaTheShatteredWalkway = 8388,
};

enum eAlurielWorldStates
{
    ShaldoreiTerraceState   = 12362,
    ShatteredWalkawy        = 12363,
    AstromancerRise         = 12364,
};

std::array<G3D::Vector3, 8> g_PatrolPath =
{
    {
        { 362.416f, 3216.757f, 142.012f },
        { 387.853f, 3200.442f, 142.012f },
        { 431.319f, 3210.574f, 142.012f },
        { 439.199f, 3247.745f, 142.012f },
        { 420.638f, 3273.112f, 142.012f },
        { 381.572f, 3301.788f, 142.012f },
        { 354.388f, 3288.092f, 142.012f },
        { 362.416f, 3216.757f, 142.012f },
    }
};

std::array<Position, 2> g_MissingPoints =
{
    {
        { 322.736f, 3318.043f, 145.479f },
        { 484.450f, 3337.457f, 140.817f },
    }
};

struct MarkOfFrostFilter
{
    MarkOfFrostFilter() : m_FrostCheck(true, eSpells::SpellMarkOfFrostDot), m_BittenCheck(true, eSpells::SpellFrostbitten)
    {}

    bool operator() (Player* p_It, Player* p_It2)
    {
        if (m_Cmp(p_It) && m_Cmp(p_It2))
            return ((!m_FrostCheck(p_It) && !m_BittenCheck(p_It)) && (m_FrostCheck(p_It2) && m_BittenCheck(p_It2)));
        else
            return ((!m_FrostCheck(p_It) && !m_BittenCheck(p_It)) && (m_FrostCheck(p_It2) && m_BittenCheck(p_It2)));
    }

    bool operator() (WorldObject*& p_It, WorldObject*& p_It2)
    {
        return MarkOfFrostFilter::operator()(p_It->ToPlayer(), p_It2->ToPlayer());
    }

    RangedFilter m_Cmp;
    JadeCore::UnitAuraCheck m_FrostCheck;
    JadeCore::UnitAuraCheck m_BittenCheck;
};

/// Spellblade Aluriel - 104881
class boss_spellblade_aluriel : public CreatureScript
{
    public:
        boss_spellblade_aluriel() : CreatureScript("boss_spellblade_aluriel")
        {}

        struct boss_spellblade_aluriel_AI : public BossAI
        {
            explicit boss_spellblade_aluriel_AI(Creature* p_Me) : BossAI(p_Me, eData::DataSpellbladeAluriel)
            {
                m_MoveId = 1;
                m_Achievement = false;
                m_SearingBrandGuid = 0;
                m_Targets.clear();
            }

            void EnterCombat(Unit* /**/) override
            {
                ClearDelayedOperations();

                m_AchievementCounter = 0;
                m_FirstIcePhase = true;

                me->GetMotionMaster()->Clear();
                me->StopMoving();

                Talk(eTalks::TalkAggro);
                _EnterCombat();
                PrepareAnnihilateTimers();
                DoCast(me, eSpells::SpellIcyEnchantment, true);

                if (!IsLFR())
                    events.ScheduleEvent(eEvents::EventCheckAchievement, TimeConstants::IN_MILLISECONDS);

                events.ScheduleEvent(eEvents::EventAnnihilate, (!IsMythic() ? 9 : GetAnnihilateTimer()) * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventCheckDist, IN_MILLISECONDS);

                if (IsHeroicOrMythic())
                    events.ScheduleEvent(eEvents::EventBerserk, 480 * IN_MILLISECONDS);
                else
                    events.ScheduleEvent(eEvents::EventBerserk, 600 * IN_MILLISECONDS);
            }

            void Reset() override
            {
                me->RemoveAllAreasTrigger();
                me->AddUnitState(UnitState::UNIT_STATE_IGNORE_PATHFINDING);

                m_Targets.clear();
                m_FirstIcePhase = true;
                m_Achievement = false;
                m_FelSoulKilled = false;
                m_AchievementCounter = 0;

                _Reset();

                CleanUp();

                if (IsMythic())
                    me->UpdateStatsForLevel();

                me->SetMaxPower(POWER_MANA, 85);
                me->SetPower(POWER_MANA, 85);

                AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    m_MoveId = 1;

                    me->SetSpeed(MOVE_WALK, 1.90f, true);
                    me->GetMotionMaster()->Clear();
                    me->StopMoving();
                    me->GetMotionMaster()->MoveSmoothPath(m_MoveId, g_PatrolPath.data(), g_PatrolPath.size(), true);
                });
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::EFFECT_MOTION_TYPE)
                    return;

                if (p_ID == m_MoveId && !me->isInCombat())
                {
                    AddTimedDelayedOperation(10, [this]() -> void
                    {
                        me->GetMotionMaster()->Clear();
                        me->StopMoving();
                        me->GetMotionMaster()->MoveSmoothPath(++m_MoveId, g_PatrolPath.data(), g_PatrolPath.size(), true);
                    });
                }
            }

            uint32 GetData(uint32 p_Data) override
            {
                if (p_Data == eDataAluriel::DataArcaneOrbs)
                    return m_Orbs;
                else if (p_Data == eDataAluriel::DataDetonateTimer)
                {
                    if (m_DetonateTimers.empty())
                        return 0;

                    uint32 l_Timer = m_DetonateTimers.front();
                    m_DetonateTimers.pop();

                    return l_Timer;
                }
                return 0;
            }

            std::vector<uint64> const & GetMarkFrostTargets() const
            {
                return m_Targets;
            }

            uint64 GetData64(uint32 p_Data) override
            {
                if (p_Data == eDataAluriel::DataFelSoulHp)
                    return m_FelSoulHp;

                return 0;
            }

            void SetGUID(uint64 p_Data, int32 p_Id) override
            {
                if (p_Id == eDataAluriel::DataMarkedPlayers)
                    m_Targets.push_back(p_Data);
                else if (p_Id == eDataAluriel::DataUnMarkedPlayers)
                    m_Targets.erase(std::remove(m_Targets.begin(), m_Targets.end(), p_Data), m_Targets.end());
                else if (p_Id == eDataAluriel::DataSearingBrand)
                    m_SearingBrandGuid = p_Data;
            }

            void SetData(uint32 p_Data, uint32 /**/) override
            {
                if (p_Data == eDataAluriel::DataArcaneOrbs)
                    m_Orbs++;
                else if (p_Data == eDataAluriel::DataDetonateTimer)
                {
                    m_DetonateTimerOrb += IN_MILLISECONDS;
                    m_DetonateTimers.push(m_DetonateTimerOrb);
                }
            }

            void EnterEvadeMode() override
            {
                Talk(eTalks::TalkWipe);
                BossAI::EnterEvadeMode();
            }

            void CleanUp()
            {
                m_FelSoulHp = 0;

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellMarkOfFrostDot);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellMarkOfFrostFixate);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellSearingBrandFixate);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellSearingBrandDot);
                }

                me->DespawnAreaTriggersInArea(eSpells::SpellFelStompAt, 250.f);
                me->DespawnCreaturesInArea(eCreatures::NpcFelSoul, 250.f);
                me->DespawnCreaturesInArea(eCreatures::NpcMarkOfFrost, 250.f);
                me->DespawnCreaturesInArea(eCreatures::NpcSearingBrand, 250.f);
                me->DespawnCreaturesInArea(eCreatures::NpcSpellbladeAlurielImage, 250.f);
                me->DespawnCreaturesInArea(eCreatures::NpcArcaneOrb);
                me->DespawnCreaturesInArea(eCreatures::NpcIcyEnchantment, 250.f);
                me->DespawnCreaturesInArea(eCreatures::NpcFieryEnchantment, 250.f);
                me->DespawnCreaturesInArea(eCreatures::NpcArcaneEnchantment, 250.f);
            }

            void SummonedCreatureDies(Creature* p_Summon, Unit* p_Killer) override
            {
                BossAI::SummonedCreatureDies(p_Summon, p_Killer);

                if (p_Summon == nullptr || p_Killer == nullptr)
                    return;

                if (p_Summon->GetEntry() == eCreatures::NpcFelSoul && (p_Killer->IsPlayer() || p_Killer->IsPetGuardianStuff()))
                    m_FelSoulKilled = true;
            }

            void GetMythicHealthMod(float& p_Modifier) override
            {
                p_Modifier = 4.46014f;
            }

            void KilledUnit(Unit* p_Victim) override
            {
                if (p_Victim != nullptr && p_Victim->IsPlayer())
                    Talk(eTalks::TalkKill);
            }

            void JustReachedHome() override
            {
                Talk(eTalks::TalkWipe);
                _JustReachedHome();
            }

            void SummonedCreatureDespawn(Creature* p_Summon) override
            {
                BossAI::SummonedCreatureDespawn(p_Summon);

                if (p_Summon == nullptr)
                    return;

                if (p_Summon->GetEntry() == eCreatures::NpcFelSoul)
                    m_FelSoulHp = p_Summon->GetHealth();
            }

            void DamageTaken(Unit* /**/, uint32 & p_Damage, SpellInfo const* /**/) override
            {
                if (me->HealthBelowPctDamaged(1, p_Damage) && m_Achievement)
                    CheckAchievement();
            }
            
            void JustDied(Unit* /**/) override
            {
                Talk(eTalks::TalkDeath);
                me->RemoveAllAreasTrigger();
                CleanUp();
                _JustDied();
            }

            void JustSummoned(Creature* p_Summon) override
            {
                BossAI::JustSummoned(p_Summon);

                if (p_Summon == nullptr)
                    return;

                switch (p_Summon->GetEntry())
                {
                    case eCreatures::NpcIcyEnchantment:
                    case eCreatures::NpcFieryEnchantment:
                    case eCreatures::NpcArcaneEnchantment:
                    {
                        DoZoneInCombat(p_Summon, 150.f);

                        if (p_Summon->GetEntry() == eCreatures::NpcIcyEnchantment && p_Summon->IsAIEnabled && !m_IcyEnchatment)
                        {
                            m_IcyEnchatment = true;
                            p_Summon->GetAI()->DoAction(eActionsAluriel::ActionIcyEnchantment);
                        }

                        break;
                    }

                    case eCreatures::NpcFelSoul:
                    {
                        if (m_FelSoulHp != 0)
                            p_Summon->SetHealth(m_FelSoulHp);

                        break;
                    }

                    default: break;
                }
            }

            void CheckMarks()
            {
                bool l_Marked = false;

                Unit* l_Target = nullptr;
                Unit* l_Target2 = nullptr;

                for (size_t i = 0; i < m_Targets.size(); ++i)
                {
                    l_Target = sObjectAccessor->GetPlayer(*me, m_Targets.at(i));

                    if (l_Target == nullptr)
                        continue;

                    for (size_t j = i + 1; j < m_Targets.size(); ++j)
                    {
                        l_Target2 = sObjectAccessor->GetPlayer(*me, m_Targets.at(j));

                        if (l_Target2 == nullptr)
                            continue;

                        float l_Dist = l_Target->GetDistance2d(l_Target2);

                        if (l_Dist <= 3.f)
                        {
                            AddMarkToPlayers(l_Target, l_Target2);
                            l_Marked = true;
                            break;
                        }
                    }

                    if (l_Marked)
                        break;
                }
            }

            void CheckAchievement()
            {
                uint32 l_AreaId = me->GetAreaId();

                switch (l_AreaId)
                {
                    case eAchievementAreaIds::AreaAstromancerRise:
                    {
                        instance->DoUpdateWorldState(eAlurielWorldStates::AstromancerRise, 1);
                        break;
                    }

                    case eAchievementAreaIds::AreaShaldoreiTerrace:
                    {
                        instance->DoUpdateWorldState(eAlurielWorldStates::ShaldoreiTerraceState, 1);
                        break;
                    }

                    case eAchievementAreaIds::AreaTheShatteredWalkway:
                    {
                        instance->DoUpdateWorldState(eAlurielWorldStates::ShatteredWalkawy, 1);
                        break;
                    }
                }
            }

            Player* GetTargetForMark(std::list<Player*>& p_Targets, Unit* p_PreviousMarkedTarget)
            {
                if (p_Targets.empty())
                    return nullptr;

                p_Targets.sort(JadeCore::UnitSortDistance(true, p_PreviousMarkedTarget));

                auto l_Itr = std::find_if(p_Targets.begin(), p_Targets.end(), [] (Player*& p_Itr) -> bool
                {
                    return !p_Itr->HasAura(eSpells::SpellMarkOfFrostDot) && !p_Itr->HasAura(eSpells::SpellFrostbitten);
                });

                if (l_Itr != p_Targets.end())
                    return *l_Itr;
                else
                    return p_Targets.front();
            }

            void AddMarkToPlayers(Unit* p_Target, Unit* p_TargetTwo)
            {
                std::list<Player*> l_TargetsOne, l_TargetsTwo;
                
                uint64 l_CasterGuid = p_Target->GetGUID();
                uint64 l_Guid = p_TargetTwo->GetGUID();

                p_Target->GetPlayerListInGrid(l_TargetsOne, 250.f);
                p_Target->GetPlayerListInGrid(l_TargetsTwo, 250.f);

                l_TargetsOne.remove_if([&l_CasterGuid, &l_Guid](Player* p_Itr)
                {
                    return !p_Itr || p_Itr->isDead() || p_Itr->GetGUID() == l_CasterGuid || p_Itr->GetGUID() == l_Guid;
                });

                Player* l_NewTargetOne = GetTargetForMark(l_TargetsOne, p_Target);

                l_TargetsTwo.remove_if([&l_CasterGuid, &l_Guid, &l_NewTargetOne](Player* p_Itr)
                {
                    return !p_Itr || p_Itr->isDead() || p_Itr->GetGUID() == l_CasterGuid || p_Itr->GetGUID() == l_Guid ||
                            p_Itr->GetGUID() == l_NewTargetOne->GetGUID();
                });

                Player* l_NewTargetTwo = GetTargetForMark(l_TargetsTwo, p_TargetTwo);

                Aura* l_MarkOne = p_Target->GetAura(eSpells::SpellMarkOfFrostDot);
                Aura* l_MarkTwo = p_TargetTwo->GetAura(eSpells::SpellMarkOfFrostDot);

                if (l_MarkOne != nullptr && l_NewTargetOne != nullptr)
                {
                    l_NewTargetOne->CastSpell(l_NewTargetOne, eSpells::SpellMarkOfFrostDot, true);

                    if (Aura* l_Aura = l_NewTargetOne->GetAura(eSpells::SpellMarkOfFrostDot))
                    {
                        l_Aura->SetDuration(l_MarkOne->GetDuration());
                        l_Aura->SetMaxDuration(l_MarkOne->GetDuration());
                    }
                }

                if (l_MarkTwo != nullptr && l_NewTargetTwo != nullptr)
                {
                    l_NewTargetTwo->CastSpell(l_NewTargetTwo, eSpells::SpellMarkOfFrostDot, true);

                    if (Aura* l_Aura = l_NewTargetTwo->GetAura(eSpells::SpellMarkOfFrostDot))
                    {
                        l_Aura->SetDuration(l_MarkTwo->GetDuration());
                        l_Aura->SetMaxDuration(l_MarkTwo->GetDuration());
                    }
                }

                p_Target->CastSpell(p_Target, eSpells::SpellMarkOfFrostExplosion, true);
                p_Target->CastSpell(p_Target, eSpells::SpellMarkOfFrostKnockBack, true);
                p_TargetTwo->CastSpell(p_TargetTwo, eSpells::SpellMarkOfFrostKnockBack, true);
                p_Target->RemoveAurasDueToSpell(eSpells::SpellMarkOfFrostDot);
                p_TargetTwo->RemoveAurasDueToSpell(eSpells::SpellMarkOfFrostDot);
            }

            void PrepareAnnihilateTimers()
            {
                if (!IsMythic())
                    return;

                while (!m_AnnihilateTimers.empty())
                    m_AnnihilateTimers.pop();
                
                for (uint8 const & l_Itr : { 78,32,77,40,80,37,80,43,77 })
                    m_AnnihilateTimers.push(85 - l_Itr);
            }

            uint8 GetAnnihilateTimer()
            {
                uint8 l_Timer = 0;

                if (IsMythic())
                {
                    l_Timer = m_AnnihilateTimers.front();
                    m_AnnihilateTimers.pop();
                    m_AnnihilateTimers.push(l_Timer);
                }
                else
                    l_Timer = 45;

                return l_Timer;
            }

            void OnSpellCasted(SpellInfo const* p_Spell) override
            {
                if (p_Spell == nullptr)
                    return;

                switch (p_Spell->Id)
                {
                case eSpells::SpellIcyEnchantment:
                {
                    DefaultEvents(eSpells::SpellIcyEnchantment);
                    break;
                }
                case eSpells::SpellFieryEnchantment:
                {
                    DefaultEvents(eSpells::SpellFieryEnchantment);
                    break;
                }
                case eSpells::SpellMagicEnchantment:
                {
                    DefaultEvents(eSpells::SpellMagicEnchantment);
                    break;
                }

                case eSpells::SpellSearingBrandChargeDest:
                {
                    if (Unit* l_Target = sObjectAccessor->GetUnit(*me, m_SearingBrandGuid))
                        l_Target->CastSpell(l_Target, eSpells::SpellSearingBrandDot, true);

                    break;
                }

                default:
                    break;
                }
            }

            void ExecuteEvent(uint32 const p_EventId) override
            {
                switch (p_EventId)
                {
                    case eEvents::EventSumFelSoul:
                    {
                        if (!m_FelSoulKilled)
                            DoCast(me, eSpells::SpellFelSoul, true);
                        
                        break;
                    }
                    case eEvents::EventAnnihilate:
                    {
                        Talk(eTalks::TalkAnnihilate);
                        DoCast(me, eSpells::SpellAnnihilate);
                        events.ScheduleEvent(eEvents::EventAnnihilate, GetAnnihilateTimer() * IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventMarkOfFrost:
                    {
                        Talk(eTalks::TalkMarkOfFrost);
                        DoCastAOE(eSpells::SpellMarkOfFrostAoe);
                        break;
                    }

                    case eEvents::EventReplicateMarkOfFrost:
                    {
                        Talk(eTalks::TalkReplicate);
                        DoCastAOE(eSpells::SpellReplicateMarkOfFrost);
                        break;
                    }

                    case eEvents::EventCheckDist:
                    {
                        CheckMarks();
                        events.ScheduleEvent(eEvents::EventCheckDist, IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventDetonateMarkOfFrost:
                    {
                        Talk(eTalks::TalkDetonate);
                        DoCastAOE(eSpells::SpellDetonateMarkOfFrostAoe);
                        break;
                    }

                    case eEvents::EventAnimateMarkOfFrost:
                    {
                        Talk(eTalks::TalkAnimate);
                        DoCastAOE(eSpells::SpellAnimateMarkOfFrostAoe);
                        break;
                    }

                    case eEvents::EventSearingBrand:
                    {
                        DoCastAOE(eSpells::SpellSearingBrandAoe);
                        break;
                    }

                    case eEvents::EventReplicateSearingBrand:
                    {
                        DoCastAOE(eSpells::SpellReplicateSearingBrandAoe);
                        break;
                    }

                    case eEvents::EventDetonateSearingBrand:
                    {
                        DoCastAOE(eSpells::SpellDetonateSearingBrandAoe);
                        break;
                    }

                    case eEvents::EventAnimateSearingBrand:
                    {
                        DoCastAOE(eSpells::SpellAnimateSearingBrandAoe);
                        break;
                    }

                    case eEvents::EventArcaneOrb:
                    {
                        m_Orbs = 0;
                        DoCastAOE(eSpells::SpellArcaneOrbAoe);
                        break;
                    }

                    case eEvents::EventReplicateArcaneOrb:
                    {
                        Talk(eTalks::TalkReplicate);
                        DoCastAOE(eSpells::SpellReplicateArcaneOrbAoe);
                        break;
                    }

                    case eEvents::EventDetonateArcaneOrb:
                    {
                        Talk(eTalks::TalkDetonate);

                        for (uint8 l_Itr = 0; l_Itr < 8; ++l_Itr)
                            SetData(eDataAluriel::DataDetonateTimer, 0);

                        DoCastAOE(eSpells::SpellDetonateArcaneOrbAoe);
                        break;
                    }

                    case eEvents::EventAnimateArcaneOrb:
                    {
                        Talk(eTalks::TalkAnimate);
                        DoCastAOE(eSpells::SpellAnimateArcaneOrbAoe);
                        break;
                    }

                    case eEvents::EventBerserk:
                    {
                        DoCast(me, eSharedSpells::SpellBerserk, true);
                        break;
                    }

                    case eEvents::EventCheckAchievement:
                    {
                        m_Achievement = CheckPointForAchievement();

                        if (m_AchievementCounter++ < 120)
                            events.ScheduleEvent(eEvents::EventCheckAchievement, IN_MILLISECONDS);
                        
                        break;
                    }
                }
            }

            void DefaultEvents(uint32 const p_Phase)
            {
                events.CancelEventGroup(ePhases::IcePhase);
                events.CancelEventGroup(ePhases::FirePhase);
                events.CancelEventGroup(ePhases::ArcanePhase);

                if (IsMythic())
                    events.ScheduleEvent(eEvents::EventSumFelSoul, 18 * TimeConstants::IN_MILLISECONDS);

                switch (p_Phase)
                {
                    case eSpells::SpellIcyEnchantment:
                    {
                        while (!m_DetonateTimers.empty())
                            m_DetonateTimers.pop();

                        m_DetonateTimerOrb = 0;
                        m_IcyEnchatment = false;

                        if (m_FirstIcePhase)
                        {
                            events.ScheduleEvent(eEvents::EventMarkOfFrost, 18 * TimeConstants::IN_MILLISECONDS, ePhases::IcePhase);
                            events.ScheduleEvent(eEvents::EventReplicateMarkOfFrost, (IsMythic() ? 28 : 38) * TimeConstants::IN_MILLISECONDS, ePhases::IcePhase);
                            events.ScheduleEvent(eEvents::EventDetonateMarkOfFrost, (IsMythic() ? 48 :69) * TimeConstants::IN_MILLISECONDS, ePhases::IcePhase);
                            events.ScheduleEvent(eEvents::EventAnimateMarkOfFrost, (IsMythic() ? 65 : 75) * TimeConstants::IN_MILLISECONDS, ePhases::IcePhase);
                            m_FirstIcePhase = false;
                        }
                        else
                        {
                            events.ScheduleEvent(eEvents::EventMarkOfFrost, (IsMythic() ? 1 : 2) * IN_MILLISECONDS, ePhases::IcePhase);
                            events.ScheduleEvent(eEvents::EventReplicateMarkOfFrost, 15 * TimeConstants::IN_MILLISECONDS, ePhases::IcePhase);
                            events.ScheduleEvent(eEvents::EventDetonateMarkOfFrost, (IsMythic() ? 35 : 45) * TimeConstants::IN_MILLISECONDS, ePhases::IcePhase);
                            events.ScheduleEvent(eEvents::EventAnimateMarkOfFrost, (IsMythic() ? 52 : 62) * TimeConstants::IN_MILLISECONDS, ePhases::IcePhase);
                        }

                        events.ScheduleEvent(eEvents::EventCheckDist, IN_MILLISECONDS);
                        break;
                    }

                    case eSpells::SpellFieryEnchantment:
                    {
                        events.ScheduleEvent(eEvents::EventSearingBrand, 18 * TimeConstants::IN_MILLISECONDS, ePhases::FirePhase);
                        events.ScheduleEvent(eEvents::EventReplicateSearingBrand, 26 * TimeConstants::IN_MILLISECONDS, ePhases::FirePhase);
                        events.ScheduleEvent(eEvents::EventDetonateSearingBrand, (IsMythic() ? 40 : 45) * TimeConstants::IN_MILLISECONDS, ePhases::FirePhase);
                        events.ScheduleEvent(eEvents::EventAnimateSearingBrand,  (IsMythic() ? 55 : 62) * TimeConstants::IN_MILLISECONDS, ePhases::FirePhase);
                        break;
                    }

                    case eSpells::SpellMagicEnchantment:
                    {
                        events.ScheduleEvent(eEvents::EventArcaneOrb, 500, ePhases::ArcanePhase);
                        events.ScheduleEvent(eEvents::EventReplicateArcaneOrb, 15 * TimeConstants::IN_MILLISECONDS, ePhases::ArcanePhase);
                        events.ScheduleEvent(eEvents::EventDetonateArcaneOrb, 35 * TimeConstants::IN_MILLISECONDS, ePhases::ArcanePhase);
                        events.ScheduleEvent(eEvents::EventAnimateArcaneOrb, (IsMythic() ? 55 : 52) * TimeConstants::IN_MILLISECONDS, ePhases::ArcanePhase);
                        break;
                    }

                    default:
                        break;
                }
            }

            bool CheckPointForAchievement()
            {
                uint32 l_Area = me->GetAreaId();

                return  (l_Area == eAchievementAreaIds::AreaAstromancerRise || l_Area == eAchievementAreaIds::AreaShaldoreiTerrace ||
                        l_Area == eAchievementAreaIds::AreaTheShatteredWalkway);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
                BossAI::UpdateAI(p_Diff);
            }

            private:
                bool m_Achievement, m_IcyEnchatment, m_FirstIcePhase, m_FelSoulKilled;
                uint64 m_FelSoulHp;
                uint32 m_DetonateTimerOrb;
                uint64 m_SearingBrandGuid;
                uint8 m_MoveId, m_AchievementCounter;
                std::queue<uint32> m_DetonateTimers;
                std::queue<uint8> m_AnnihilateTimers;
                std::vector<uint64> m_Targets;
                uint8 m_Orbs;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new boss_spellblade_aluriel_AI(p_Me);
        }
};

/// Aluriel Image - 107980
class npc_aluriel_spellblade_aluriel_image : public CreatureScript
{
    public:
        npc_aluriel_spellblade_aluriel_image() : CreatureScript("npc_aluriel_spellblade_aluriel_image")
        {}

        struct npc_aluriel_spellblade_aluriel_image_AI : public ScriptedAI
        {
            explicit npc_aluriel_spellblade_aluriel_image_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                m_IsCasting = false;
                m_TargetMarkGuid = 0;
                me->SetReactState(REACT_PASSIVE);
                me->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, Emote::EMOTE_ONESHOT_READY1H);
            }

            void SetGUID(uint64 p_Data, int32 p_Id) override
            {
                if (p_Id == eDataAluriel::DataSearingBrand)
                {
                    m_TargetMarkGuid = p_Data;
                }
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eActionsAluriel::ActionStopLook)
                    m_IsCasting = true;
            }

            void OnSpellCasted(SpellInfo const* p_Spell) override
            {
                if (!p_Spell)
                    return;

                if (p_Spell->Id != eSpells::SpellSearingBrandChargeDest)
                    return;

                if (Unit* l_MarkedPlayer = sObjectAccessor->GetPlayer(*me, m_TargetMarkGuid))
                    l_MarkedPlayer->CastSpell(l_MarkedPlayer, eSpells::SpellSearingBrandDot, true);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (m_IsCasting)
                    return;

                if (Unit* l_MarkedPlayer = sObjectAccessor->GetUnit(*me, m_TargetMarkGuid))
                    me->SetFacingToObject(l_MarkedPlayer);
            }

            bool m_IsCasting;
            uint64  m_TargetMarkGuid;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_aluriel_spellblade_aluriel_image_AI(p_Me);
        }
};

/// Icy Enchatment Add - 107237
/// Fiery Enchatment Add - 107285
/// Arcane Enchatment Add - 107287
class npc_aluriel_enchantment_add : public CreatureScript
{
    public:
        npc_aluriel_enchantment_add() : CreatureScript("npc_aluriel_enchantment_add")
        {}

        struct npc_aluriel_enchantment_add_AI : public ScriptedAI
        {
            explicit npc_aluriel_enchantment_add_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                m_TimerBound = 0;
            }

            void CheckRoots()
            {
                switch (me->GetEntry())
                {
                    case eCreatures::NpcIcyEnchantment:
                    {
                        DoCast(me, eSpells::SpellBurstOfIce, true);
                        break;
                    }


                    case eCreatures::NpcFieryEnchantment:
                    {
                        DoCast(me, eSpells::SpellBurstOfFlame, true);

                        if (IsHeroicOrMythic())
                        {
                            me->SetControlled(true, UNIT_STATE_ROOT);
                            events.ScheduleEvent(eEvents::EventPyroblast, 4 * IN_MILLISECONDS);
                        }
                        break;
                    }

                    case eCreatures::NpcArcaneEnchantment:
                    {
                        if (IsHeroicOrMythic())
                        {
                            m_TimerBound = 0;
                            me->SetControlled(true, UNIT_STATE_ROOT);
                            me->CastSpell(me, eSpells::SpellArmageddon, false);
                        }
                        else
                            DoCast(me, eSpells::SpellBurstOfMagic, true);
                        break;
                    }

                    default: break;
                }
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eActionsAluriel::ActionIcyEnchantment)
                {
                    if (IsHeroicOrMythic())
                        events.ScheduleEvent(eEvents::EventFrozenTempest, 10 * IN_MILLISECONDS);
                }
            }

            void Reset() override
            {
                events.Reset();

                CheckRoots();

                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void GetMythicHealthMod(float& p_Modifier) override
            {
                switch (me->GetEntry())
                {
                    case eCreatures::NpcIcyEnchantment:
                    {
                        p_Modifier = 4.10843f;
                        break;
                    }

                    case eCreatures::NpcFieryEnchantment:
                    {
                        p_Modifier = 6.1662f;
                       break;
                    }

                    case eCreatures::NpcArcaneEnchantment:
                    {
                        p_Modifier = 6.4779f;
;                       break;
                    }

                    default: break;
                }
            }

            void ExecuteEvent(uint32 const p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventPyroblast:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.f, true))
                            DoCast(l_Target, eSpells::SpellPyroblast);

                        events.ScheduleEvent(eEvents::EventPyroblast, IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventFrozenTempest:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                            DoCast(l_Target, eSpells::SpellFrozenTempestTeleport, true);

                        events.ScheduleEvent(eEvents::EventFrozenTempest, 15 * IN_MILLISECONDS);
                        break;
                    }

                    default: break;
                }
            }

            void OnSpellCasted(SpellInfo const* p_Spell) override
            {
                if (p_Spell == nullptr)
                    return;

                if (p_Spell->Id == eSpells::SpellFrozenTempestTeleport)
                    me->DelayedCastSpell(me, eSpells::SpellFrozenTempest, false, IN_MILLISECONDS);
            }

            void JustDied(Unit* /**/) override
            {
                ClearDelayedOperations();
                events.Reset();
                me->RemoveAllAuras();
                me->RemoveAllAreasTrigger();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (IsHeroicOrMythic() && me->GetEntry() == eCreatures::NpcFieryEnchantment)
                {
                    m_TimerBound += p_Diff;

                    if (m_TimerBound >= IN_MILLISECONDS)
                    {
                        m_TimerBound = 0;
                        DoCast(me, eSpells::SpellBoundInFlameAoe, true);
                    }
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }

            private:
                uint32 m_TimerBound;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_aluriel_enchantment_add_AI(p_Me);
        }
};

/// Fel Soul - 115905
class npc_aluriel_fel_soul : public CreatureScript
{
    public:
        npc_aluriel_fel_soul() : CreatureScript("npc_aluriel_fel_soul")
        {}

        struct npc_aluriel_fel_soul_AI : public ScriptedAI
        {
            explicit npc_aluriel_fel_soul_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
            }

            void JustDied(Unit* /**/) override
            {
                me->DespawnOrUnsummon(IN_MILLISECONDS);
            }

            void Reset() override
            {
                me->DespawnOrUnsummon(46 * IN_MILLISECONDS);
                me->UpdateStatsForLevel();

                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
            }

            void GetMythicHealthMod(float& p_Modifier) override
            {
                p_Modifier = 74.4718f;
                p_Modifier /= 1.55f;
            }

            void OnSpellCasted(SpellInfo const* p_Spell) override
            {
                if (p_Spell->Id == eSpells::SpellFelStomp && !m_CastedOnTank)
                {
                    m_CastedOnTank = true;

                    if (Unit* l_Victim = me->getVictim())
                        me->DelayedCastSpell(l_Victim, eSpells::SpellFelStomp, true, IN_MILLISECONDS);
                }
            }

            void EnterCombat(Unit* /**/) override
            {
                DoZoneInCombat();

                Creature* l_Aluriel = me->FindNearestCreature(eCreatures::BossSpellbladeAluriel, 250.f);

                if (l_Aluriel == nullptr)
                    return;

                if (l_Aluriel->HasAura(eSpells::SpellIcyEnchantment))
                    events.ScheduleEvent(eEvents::EventWhirlingBarrage, 8 * IN_MILLISECONDS);
                else if (l_Aluriel->HasAura(eSpells::SpellFieryEnchantment))
                    events.ScheduleEvent(eEvents::EventFelStomp, 8 * IN_MILLISECONDS);
                else
                    events.ScheduleEvent(eEvents::EventFelLash, 8 * IN_MILLISECONDS);

                events.ScheduleEvent(eEvents::EventDecimate, 20 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventCheckDist, IN_MILLISECONDS);
            }

            void ExecuteEvent(uint32 const p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventWhirlingBarrage:
                    {
                        DoCast(me, eSpells::SpellWhirlingBarrage);
                        events.ScheduleEvent(eEvents::EventWhirlingBarrage, 20 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventDecimate:
                    {
                        DoCastVictim(eSpells::SpellDecimate);
                        events.ScheduleEvent(eEvents::EventDecimate, 20 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventFelStomp:
                    {
                        m_CastedOnTank = false;
                        DoCastAOE(eSpells::SpellFelStompAoe);
                        events.ScheduleEvent(eEvents::EventFelStomp, 10 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventFelLash:
                    {
                        DoCastVictim(eSpells::SpellFelLash);
                        events.ScheduleEvent(eEvents::EventFelLash, 10 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventCheckDist:
                    {
                        events.ScheduleEvent(eEvents::EventCheckDist, IN_MILLISECONDS);

                        Creature* l_Aluriel = me->FindNearestCreature(eCreatures::BossSpellbladeAluriel, 250.f, true);

                        if (l_Aluriel)
                        {
                            if (me->GetDistance2d(l_Aluriel) > 45.f)
                            {
                                DoCast(l_Aluriel, eSpells::SpellSeveredSoul, true);
                                DoCast(me, eSpells::SpellSeveredSoul);
                                events.RescheduleEvent(eEvents::EventCheckDist, 8 * IN_MILLISECONDS);
                            }
                        }

                        break;
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 const l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }

            bool m_CastedOnTank;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_aluriel_fel_soul_AI(p_Me);
        }
};

/// Arcane Orb - 10510
class npc_aluriel_arcane_orb : public CreatureScript
{
    public:
        npc_aluriel_arcane_orb() : CreatureScript("npc_aluriel_arcane_orb")
        {}

        struct npc_aluriel_arcane_orb_AI : public ScriptedAI
        {
            explicit npc_aluriel_arcane_orb_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetSpeed(UnitMoveType::MOVE_FLIGHT, 0.2f);
                me->CastSpell(me, eSpells::SpellArcaneOrbDot, true);
                me->m_positionZ += 2.f;
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (p_Summoner == nullptr)
                    return;

                SetFlyMode(true);
                me->SetCanFly(true);
                me->SetDisableGravity(true);
                me->SetReactState(REACT_PASSIVE);
                me->SetSpeed(UnitMoveType::MOVE_FLIGHT, 0.2f);

                if (p_Summoner->GetEntry() == eCreatures::NpcArcaneOrb)
                    events.ScheduleEvent(eEvents::EventReplicateArcaneOrb, urand(1, 3) * IN_MILLISECONDS);
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* p_Spell) override
            {
                if (p_Spell == nullptr)
                    return;

                switch (p_Spell->Id)
                {
                    case eSpells::SpellReplicateArcaneOrbAoe:
                    {
                        events.ScheduleEvent(eEvents::EventReplicateArcaneOrb, 1 * IN_MILLISECONDS);
                        break;
                    }

                    case eSpells::SpellDetonateArcaneOrbAoe:
                    {
                        events.Reset();
                        me->RemoveAurasDueToSpell(eSpells::SpellArcaneOrbDot);
                        me->SetSpeed(UnitMoveType::MOVE_FLIGHT, 0.4f);

                        InstanceScript * l_Instance = me->GetInstanceScript();

                        if (l_Instance == nullptr)
                            return;

                        Unit* l_Boss = sObjectAccessor->GetUnit(*me, l_Instance->GetData64(eCreatures::BossSpellbladeAluriel));

                        if (l_Boss && l_Boss->IsAIEnabled)
                        {
                            uint32 l_Timer = l_Boss->GetAI()->GetData(eDataAluriel::DataDetonateTimer);
                            events.ScheduleEvent(eEvents::EventDetonateArcaneOrbFly, l_Timer);
                        }

                        break;
                    }

                    default: break;
                }
            }

            void BuildPath()
            {
                Position l_Pos = me->GetPosition();
                l_Pos.m_positionZ += 10.f;

                me->GetMotionMaster()->MoveTakeoff(ePoints::PointFlyPointReached, l_Pos);
            }

            void ReplicateOrb()
            {
                InstanceScript* l_Instance = me->GetInstanceScript();
                Unit* l_Boss = nullptr;

                if (l_Instance != nullptr)
                    l_Boss = sObjectAccessor->GetUnit(*me, l_Instance->GetData64(eCreatures::BossSpellbladeAluriel));

                if (l_Boss != nullptr && l_Boss->IsAIEnabled)
                {
                    if (l_Boss->GetAI()->GetData(eDataAluriel::DataArcaneOrbs) > 6)
                        return;

                    Unit* l_Target = l_Boss->GetAI()->SelectRangedTarget(true);

                    if (l_Target == nullptr)
                        l_Target = l_Boss->GetAI()->SelectMeleeTarget(true);

                    if (l_Target != nullptr)
                    {
                        l_Boss->GetAI()->SetData(eDataAluriel::DataArcaneOrbs, 0);
                        me->CastSpell(l_Target, eSpells::SpellArcaneOrbMissile, true);
                    }
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_Id) override
            {
                if (p_Type != POINT_MOTION_TYPE && p_Type != EFFECT_MOTION_TYPE)
                    return;

                if (p_Id == ePoints::PointFlyPointReached)
                {
                    me->SetSpeed(UnitMoveType::MOVE_FLIGHT, 0.8f);

                    InstanceScript* l_Instance = me->GetInstanceScript();
                    Unit* l_Boss = nullptr;

                    if (l_Instance != nullptr)
                        l_Boss = sObjectAccessor->GetUnit(*me, l_Instance->GetData64(eCreatures::BossSpellbladeAluriel));

                    if (l_Boss != nullptr && l_Boss->GetAI())
                    {
                        l_Boss->GetAI()->SetData(eDataAluriel::DataArcaneOrbs, 0);

                        Unit* l_Target = l_Boss->GetAI()->SelectTarget(SELECT_TARGET_RANDOM, 0, 0.f, true);

                        if (l_Target != nullptr)
                        {
                            m_DetonatePos = l_Target->GetPosition();
                            l_Boss->CastSpell(l_Target, eSpells::SpellDetonateArcaneOrbDummy, true);
                        }
                    }

                    events.ScheduleEvent(eEvents::EventDetonateArcaneOrb, 2 * IN_MILLISECONDS);
                }
                else if (p_Id == ePoints::PointEarthPointReached)
                {
                    Creature* l_Boss = me->FindNearestCreature(eCreatures::BossSpellbladeAluriel, 250.f);

                    if (l_Boss != nullptr)
                        l_Boss->RemoveAreaTrigger(eSpells::SpellDetonateArcaneOrbDummy);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventReplicateArcaneOrb:
                    {
                        ReplicateOrb();
                        break;
                    }

                    case eEvents::EventDetonateArcaneOrbFly:
                    {
                        BuildPath();
                        break;
                    }

                    case eEvents::EventDetonateArcaneOrb:
                    {
                        me->GetMotionMaster()->MoveJump(m_DetonatePos, 10.f, 10.f, ePoints::PointEarthPointReached, eSpells::SpellDetonateArcaneOrbAt);
                        break;
                    }

                    default: break;
                }
            }

            private:
                Position m_DetonatePos;

        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_aluriel_arcane_orb_AI(p_Me);
        }
};

/// Ice Shard - 107592
class npc_aluriel_ice_shard : public CreatureScript
{
    public:
        npc_aluriel_ice_shard() : CreatureScript("npc_aluriel_ice_shard")
        {}

        struct npc_aluriel_ice_shard_AI : public ScriptedAI
        {
            explicit npc_aluriel_ice_shard_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                m_TombedPlayerGuid = 0;
                me->SetReactState(REACT_PASSIVE);
            }

            void GetMythicHealthMod(float & p_Modifier) override
            {
                p_Modifier = 50.f;
                p_Modifier /= 1.55f;
            }

            void Reset() override
            {
                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void SetGUID(uint64 p_Data, int32 p_Id) override
            {
                if (p_Id == eDataAluriel::DataMarkedPlayers)
                    m_TombedPlayerGuid = p_Data;
            }

            void JustDied(Unit* /**/) override
            {
                if (Unit* l_Target = sObjectAccessor->GetUnit(*me, m_TombedPlayerGuid))
                    l_Target->RemoveAurasDueToSpell(eSpells::SpellEntombedInIce);
            }

            uint64 m_TombedPlayerGuid;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_aluriel_ice_shard_AI(p_Me);
        }
};

/// Icy Enchantment
/// Fiery Enchantment
/// Arcane Enchantment
class spell_aluriel_enchantment : public SpellScriptLoader
{
public:
    spell_aluriel_enchantment() : SpellScriptLoader("spell_aluriel_enchantment") { }

    class spell_enchantment_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_enchantment_AuraScript);

        void HandleOnPeriodic(AuraEffect const* /*p_AurEff*/)
        {
            if (!GetUnitOwner())
                return;

            Unit* l_Owner = GetUnitOwner();

            if (l_Owner->GetMap() == nullptr)
                return;

            bool l_IsMythic = l_Owner->GetMap()->IsMythic();
            int32 l_Power = l_Owner->GetPower(Powers::POWER_MANA);

            if (GetId() == eSpells::SpellMagicEnchantment && l_Power <= 15)
            {
                l_Owner->SetPower(Powers::POWER_MANA, 85);
                l_Owner->CastSpell(l_Owner, eSpells::SpellIcyEnchantment, true);
                Remove();
                return;
            }
            else
            {
                if (l_Power <= (l_IsMythic ? 10 : 0))
                {
                    l_Owner->SetPower(Powers::POWER_MANA, 85);

                    if (GetId() == eSpells::SpellIcyEnchantment)
                        l_Owner->CastSpell(l_Owner, eSpells::SpellFieryEnchantment, true);
                    else if (GetId() == eSpells::SpellFieryEnchantment)
                        l_Owner->CastSpell(l_Owner, eSpells::SpellMagicEnchantment, true);

                    Remove();
                }
            }
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_enchantment_AuraScript::HandleOnPeriodic, EFFECT_2, SPELL_AURA_POWER_BURN);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_enchantment_AuraScript();
    }
};

/// Arcane Orb Aoe - 215456
class spell_aluriel_arcane_orb_aoe : public SpellScriptLoader
{
    public:
        spell_aluriel_arcane_orb_aoe() : SpellScriptLoader("spell_aluriel_arcane_orb_aoe")
        {}

        class spell_arcane_orb_aoe_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_arcane_orb_aoe_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                TankFilter l_TankCmp;
                RangedFilter l_RangedCmp;
                
                Unit* l_Caster = GetCaster();
                
                p_Targets.sort([&l_TankCmp, l_RangedCmp, &l_Caster](WorldObject*& p_It, WorldObject*& p_It2)
                {
                    Player* l_Target = p_It->ToPlayer();
                    Player* l_Target2 = p_It2->ToPlayer();

                    return l_Target->IsRangedDamageDealer(true) && !l_Target2->IsRangedDamageDealer(true);
                });

                if (p_Targets.size() > 1)
                    p_Targets.resize(1);
            }

            void HandleHit(SpellEffIndex /**/)
            {
                if (!GetHitUnit())
                    return;

                GetCaster()->CastSpell(*GetHitUnit(), eSpells::SpellArcaneOrbMissile, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_arcane_orb_aoe_SpellScript::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_arcane_orb_aoe_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_arcane_orb_aoe_SpellScript();
        }
};

/// Whirling Barrage - 230416
class spell_aluriel_whirling_barrage : public SpellScriptLoader
{
    public:
        spell_aluriel_whirling_barrage() : SpellScriptLoader("spell_aluriel_whirling_barrage")
        {}

        class spell_whirling_barrage_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_whirling_barrage_AuraScript);

            void HandleOnPeriodic(AuraEffect const* /**/)
            {
                if (!GetUnitOwner())
                    return;

                Unit* l_Owner = GetUnitOwner();

                std::list<HostileReference*>& l_Targets = l_Owner->getThreatManager().getThreatList();
                RangedFilter l_Cmp;
                
                l_Targets.sort([&l_Cmp] (HostileReference*& p_Itr, HostileReference*& p_Itr2) -> bool
                {
                    return l_Cmp(p_Itr) && !l_Cmp(p_Itr2);
                });

                HostileReference* l_HostileRef = l_Targets.front();

                if (l_HostileRef == nullptr)
                    return;

                if (Unit* l_Target = l_HostileRef->getTarget())
                    l_Owner->GetMotionMaster()->MoveFollow(l_Target, 0, 0);
            }

            void HandleAfterRemove(AuraEffect const* /**/, AuraEffectHandleModes /**/)
            {
                if (GetUnitOwner() == nullptr)
                    return;

                GetUnitOwner()->GetMotionMaster()->Clear();
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_whirling_barrage_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_whirling_barrage_AuraScript();
        }
};

/// Fel Stomp - 230411
class spell_aluriel_fel_stomp_aoe : public SpellScriptLoader
{
    public:
        spell_aluriel_fel_stomp_aoe() : SpellScriptLoader("spell_aluriel_fel_stomp_aoe")
        {}

        class spell_fel_stomp_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_fel_stomp_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                RangedFilter Cmp;

                p_Targets.sort([&Cmp] (WorldObject*& p_It, WorldObject*& p_It2)
                {
                    return Cmp(p_It->ToPlayer()) && !Cmp(p_It2->ToPlayer());
                });
            }

            void HandleOnHit(SpellEffIndex /**/)
            {
                if (!GetHitUnit())
                    return;

                GetCaster()->CastSpell(GetHitUnit(), eSpells::SpellFelStomp, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_fel_stomp_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_fel_stomp_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_fel_stomp_SpellScript();
        }
};

/// Annihilate - 212492
class spell_aluriel_annihilate : public SpellScriptLoader
{
    public:
        spell_aluriel_annihilate() : SpellScriptLoader("spell_aluriel_annihilate")
        {}

        class spell_annihilate_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_annihilate_SpellScript);

            void HandleCast()
            {
                if (!GetCaster())
                    return;

                if (Unit* l_Victim = GetCaster()->getVictim())
                    GetCaster()->CastSpell(l_Victim, eSpells::SpellAnnihilateDebuff, true);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_annihilate_SpellScript::HandleCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_annihilate_SpellScript();
        }
};

/// Annihilate Damage - 212494
class spell_aluriel_annihilate_dmg : public SpellScriptLoader
{
    public:
        spell_aluriel_annihilate_dmg() : SpellScriptLoader("spell_aluriel_annihilate_dmg")
        {}

        class spell_annihilate_dmg_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_annihilate_dmg_SpellScript);

            bool Load() override
            {
                l_TargetCount = 1;
                return true;
            }


            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                if (p_Targets.size() > 2)
                    p_Targets.resize(2);

                l_TargetCount = p_Targets.size();
            }

            void HandleOnHit(SpellEffIndex /**/)
            {
                if (!GetHitUnit())
                    return;

                if (GetHitDamage() > 0)
                    SetHitDamage(GetHitDamage() / l_TargetCount);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_annihilate_dmg_SpellScript::HandleTargets, EFFECT_1, TARGET_UNIT_CONE_ENEMY_104);
                OnEffectHitTarget += SpellEffectFn(spell_annihilate_dmg_SpellScript::HandleOnHit, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
            }

            uint32 l_TargetCount;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_annihilate_dmg_SpellScript();
        }
};

/// Mark of Frost Aoe - 212646
class spell_aluriel_mark_of_frost_aoe : public SpellScriptLoader
{
    public:
        spell_aluriel_mark_of_frost_aoe() : SpellScriptLoader("spell_aluriel_mark_of_frost_aoe")
        {}

        class spell_mark_of_frost_aoe_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mark_of_frost_aoe_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                RangedFilter Cmp;

                p_Targets.sort([&Cmp] (WorldObject*& p_It, WorldObject*& p_It2)
                {
                    return Cmp(p_It->ToPlayer()) && !Cmp(p_It2->ToPlayer());
                });

                if (p_Targets.size() % 2 != 0 && !p_Targets.empty())
                    p_Targets.pop_back();

                if (p_Targets.size() > 2)
                    p_Targets.resize(2);
            }

            void HandleOnHit(SpellEffIndex /**/)
            {
                if (!GetHitUnit())
                    return;

                GetHitUnit()->CastSpell(GetHitUnit(), eSpells::SpellMarkOfFrostFixate, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mark_of_frost_aoe_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_mark_of_frost_aoe_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_mark_of_frost_aoe_SpellScript();
        }
};

/// Mark of Frost Fixate - 212531
class spell_aluriel_mark_of_frost_fixate : public SpellScriptLoader
{
    public:
        spell_aluriel_mark_of_frost_fixate() : SpellScriptLoader("spell_aluriel_mark_of_frost_fixate")
        {}

        class spell_mark_of_frost_fixate_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mark_of_frost_fixate_AuraScript);

            void HandleAfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes /**/)
            {
                if (!GetUnitOwner())
                    return;

                if (p_AurEff->GetBaseAmount() == 1)
                {
                    Aura* l_Dot = GetUnitOwner()->AddAura(eSpells::SpellMarkOfFrostDot, GetUnitOwner());

                    if (l_Dot == nullptr)
                        return;

                    l_Dot->SetMaxDuration(45 * IN_MILLISECONDS);
                    l_Dot->SetDuration(45 * IN_MILLISECONDS);

					Aura* l_Area = GetUnitOwner()->AddAura(eSpells::SpellMarkOfFrostAt, GetUnitOwner());

					if (l_Area == nullptr)
						return;

					l_Area->SetMaxDuration(45 * IN_MILLISECONDS);
					l_Area->SetDuration(45 * IN_MILLISECONDS);
                }
				else
				{
					GetUnitOwner()->CastSpell(GetUnitOwner(), eSpells::SpellMarkOfFrostAt, true);
					GetUnitOwner()->CastSpell(GetUnitOwner(), eSpells::SpellMarkOfFrostDot, true);
				}
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_mark_of_frost_fixate_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_mark_of_frost_fixate_AuraScript();
        }
};

/// Mark of Frost Dot - 212587
class spell_aluriel_mark_of_frost_dot : public SpellScriptLoader
{
	public:
		spell_aluriel_mark_of_frost_dot() : SpellScriptLoader("spell_aluriel_mark_of_frost_dot")
		{}

		class spell_mark_of_frost_dot_AuraScript : public AuraScript
		{
			PrepareAuraScript(spell_mark_of_frost_dot_AuraScript);

			void HandleAfterApply(AuraEffect const* /**/, AuraEffectHandleModes /**/)
			{
				if (GetUnitOwner() == nullptr)
					return;

				if (InstanceScript* l_Instance = GetUnitOwner()->GetInstanceScript())
				{
					Creature* l_Boss = sObjectAccessor->GetCreature(*GetUnitOwner(), l_Instance->GetData64(eCreatures::BossSpellbladeAluriel));

					if (l_Boss && l_Boss->IsAIEnabled)
						l_Boss->AI()->SetGUID(GetUnitOwner()->GetGUID(), eDataAluriel::DataMarkedPlayers);
				}
			}

            void PutMarkOnNewTarget(Creature*& p_Boss)
            {
                if (p_Boss == nullptr)
                    return;

                if (!p_Boss->HasAura(eSpells::SpellIcyEnchantment))
                    return;

                std::list<Player*> l_Targets;

                auto const & l_ThreatList = p_Boss->getThreatManager().getThreatList();

                for (auto const & l_Itr : l_ThreatList)
                {
                    Unit* l_Target = l_Itr->getTarget();

                    if (l_Target == nullptr)
                        continue;

                    if (!l_Target->IsPlayer() || l_Target->isDead())
                        continue;

                    if (!l_Target->HasAura(eSpells::SpellMarkOfFrostDot) && !l_Target->HasAura(eSpells::SpellFrostbitten))
                        l_Targets.push_back(l_Target->ToPlayer());
                }

                if (l_Targets.empty())
                    return;

                l_Targets.sort([] (Player*& p_Lhs, Player*& p_Rhs) -> bool
                {
                    return p_Lhs->GetRoleForGroup() > p_Rhs->GetRoleForGroup();
                });


                if (Aura* l_Aura = GetAura())
                {
                    Unit* l_NewTarget = l_Targets.front();

                    if (l_NewTarget == nullptr)
                        return;

                    if (Aura* l_NewMark = l_NewTarget->AddAura(eSpells::SpellMarkOfFrostDot, l_NewTarget))
                    {
                        l_NewMark->SetDuration(l_Aura->GetDuration());
                        l_NewMark->SetMaxDuration(l_Aura->GetMaxDuration());
                    }
                }
            }

			void HandleAfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes /**/)
			{
				if (GetUnitOwner() == nullptr)
					return;

				if (InstanceScript* l_Instance = GetUnitOwner()->GetInstanceScript())
				{
					Creature* l_Boss = sObjectAccessor->GetCreature(*GetUnitOwner(), l_Instance->GetData64(eCreatures::BossSpellbladeAluriel));

                    if (const AuraApplication* l_App = GetTargetApplication())
                    {
                        if (l_App->GetRemoveMode() == AuraRemoveMode::AURA_REMOVE_BY_DEATH)
                            PutMarkOnNewTarget(l_Boss);
                    }

                    if (l_Boss && l_Boss->IsAIEnabled)
                        l_Boss->AI()->SetGUID(GetUnitOwner()->GetGUID(), eDataAluriel::DataUnMarkedPlayers);
				}
			}

			void Register()
			{
				AfterEffectApply += AuraEffectApplyFn(spell_mark_of_frost_dot_AuraScript::HandleAfterApply, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
				AfterEffectRemove += AuraEffectRemoveFn(spell_mark_of_frost_dot_AuraScript::HandleAfterRemove, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
			}
		};

		AuraScript* GetAuraScript() const override
		{
			return new spell_mark_of_frost_dot_AuraScript();
		}
};

/// Replicate Mark of Frost - 212530
class spell_aluriel_replicate_mark_of_frost : public SpellScriptLoader
{
    public:
        spell_aluriel_replicate_mark_of_frost() : SpellScriptLoader("spell_aluriel_replicate_mark_of_frost")
        {}

        class spell_replicate_mark_of_frost_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_replicate_mark_of_frost_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                std::list<WorldObject*> l_NewTargets;

                std::vector<WorldObject*> l_PreviousMarked;

                p_Targets.remove_if([&l_PreviousMarked] (WorldObject*& p_Itr) -> bool
                {
                    if (!p_Itr->IsPlayer())
                        return true;

                    if (p_Itr->ToUnit()->HasAura(eSpells::SpellMarkOfFrostDot))
                    {
                        l_PreviousMarked.push_back(p_Itr);
                        return true;
                    }

                    return false;
                });

                for (WorldObject* const & l_Itr : l_PreviousMarked)
                {
                    p_Targets.sort([& l_Itr] (WorldObject* const & p_Lhs, WorldObject* const & p_Rhs) -> bool
                    {
                        Unit* l_TargetOne = p_Lhs->ToUnit();
                        Unit* l_TargetTwo = p_Rhs->ToUnit();

                        bool l_OneMarked = l_TargetOne->HasAura(eSpells::SpellFrostbitten);
                        bool l_TwoMarked = l_TargetTwo->HasAura(eSpells::SpellFrostbitten);

                        if ((!l_OneMarked && !l_TwoMarked) || (l_OneMarked && l_TwoMarked))
                            return l_Itr->GetDistance2d(l_TargetOne) < l_Itr->GetDistance2d(l_TargetTwo);
                        else
                            return !l_OneMarked;
                    });
                    
                    if (!p_Targets.empty())
                    {
                        l_NewTargets.push_back(p_Targets.front());
                        p_Targets.pop_front();
                    }
                }

                if (l_NewTargets.size() > 2)
                    l_NewTargets.resize(2);

                p_Targets = l_NewTargets;
            }

            void HandleOnHit(SpellEffIndex p_EffIndex)
            {
                if (!GetHitUnit())
                    return;

                PreventHitEffect(p_EffIndex);

                GetHitUnit()->CastCustomSpell(eSpells::SpellMarkOfFrostFixate, SPELLVALUE_BASE_POINT0, 1, GetHitUnit(), true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_replicate_mark_of_frost_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_replicate_mark_of_frost_SpellScript::HandleTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_replicate_mark_of_frost_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_FORCE_CAST);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_replicate_mark_of_frost_SpellScript();
        }
};

/// Searing Brand Aoe - 213145
class spell_aluriel_searing_brand_aoe : public SpellScriptLoader
{
    public:
        spell_aluriel_searing_brand_aoe() : SpellScriptLoader("spell_aluriel_searing_brand_aoe")
        {}

        class spell_searing_brand_aoe_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_searing_brand_aoe_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                TankFilter Cmp;

                p_Targets.sort([&Cmp] (WorldObject*& p_It, WorldObject*& p_It2)
                {
                    return !Cmp(p_It->ToPlayer()) && Cmp(p_It->ToPlayer());
                });
            }

            void HandleOnHit(SpellEffIndex /**/)
            {
                if (!GetHitUnit() || !GetCaster())
                    return;

                GetCaster()->CastSpell(GetHitUnit(), eSpells::SpellSearingBrandFixate, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_searing_brand_aoe_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_searing_brand_aoe_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_searing_brand_aoe_SpellScript();
        }
};

/// Searing Brand Fixate - 213148
class spell_aluriel_searing_brand_fixate : public SpellScriptLoader
{
    public:
        spell_aluriel_searing_brand_fixate() : SpellScriptLoader("spell_aluriel_searing_brand_fixate")
        {}

        class spell_searing_brand_fixate_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_searing_brand_fixate_AuraScript);

            void HandleAfterApply(AuraEffect const* /**/, AuraEffectHandleModes /**/)
            {
                if (!GetCaster() || !GetUnitOwner())
                    return;

                if (GetCaster()->IsAIEnabled)
                    GetCaster()->GetAI()->SetGUID(GetUnitOwner()->GetGUID(), eDataAluriel::DataSearingBrand);
            }

            void HandleAfterRemove(AuraEffect const* /**/, AuraEffectHandleModes /**/)
            {
                if (!GetCaster() || !GetUnitOwner())
                    return;

                if (GetCaster()->GetEntry() == eCreatures::NpcSpellbladeAlurielImage && GetCaster()->IsAIEnabled)
                    GetCaster()->GetAI()->DoAction(eActionsAluriel::ActionStopLook);

                GetCaster()->CastSpell(GetCaster(), eSpells::SpellSearingBrandAt, true);
                GetCaster()->CastSpell(GetUnitOwner(), eSpells::SpellSearingBrandCharge, true);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_searing_brand_fixate_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_searing_brand_fixate_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_searing_brand_fixate_AuraScript();
        }
};

/// Replicate Searing Brand - 213182
class spell_aluriel_replicate_searing_brand_aoe : public SpellScriptLoader
{
    public:
        spell_aluriel_replicate_searing_brand_aoe() : SpellScriptLoader("spell_aluriel_replicate_searing_brand_aoe")
        {}

        class spell_searing_brand_aoe_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_searing_brand_aoe_SpellScript);

            bool Load() override
            {
                l_Count = 0;
                return true;
            }

            void HandleOnHit(SpellEffIndex /**/)
            {
                if (!GetHitUnit())
                    return;

                if (l_Count >= 4)
                {
                    GetCaster()->CastSpell(GetHitUnit(), eSpells::SpellSearingBrandFixate, true);
                    l_Count++;
                    return;
                }

                Creature* l_Image = nullptr;

                l_Image = GetCaster()->SummonCreature(eCreatures::NpcSpellbladeAlurielImage, *GetHitUnit(), TEMPSUMMON_TIMED_DESPAWN, 10 * IN_MILLISECONDS);

                if (l_Image)
                {
                    l_Count++;
                    l_Image->SetReactState(REACT_PASSIVE);
                    l_Image->CastSpell(GetHitUnit(), eSpells::SpellSearingBrandFixate, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_searing_brand_aoe_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }

            uint8 l_Count;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_searing_brand_aoe_SpellScript();
        }
};

/// Replicate Arcane Orb - 213852
class spell_aluriel_replicate_arcane_orb_aoe : public SpellScriptLoader
{
    public:
        spell_aluriel_replicate_arcane_orb_aoe() : SpellScriptLoader("spell_aluriel_replicate_arcane_orb_aoe")
        {}

        class spell_replicate_arcane_orb_aoe_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_replicate_arcane_orb_aoe_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if([] (WorldObject*& p_It)
                {
                    return p_It->GetEntry() != eCreatures::NpcArcaneOrb;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_replicate_arcane_orb_aoe_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_replicate_arcane_orb_aoe_SpellScript();
        }
};

/// Detonate Mark of Frost  - 212735
/// Detonate Searing Brand  - 213275
class spell_aluriel_detonate : public SpellScriptLoader
{
    public:
        spell_aluriel_detonate() : SpellScriptLoader("spell_aluriel_detonate")
        {}

        class spell_aluriel_detonate_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_aluriel_detonate_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty() || !GetSpellInfo())
                    return;

                SpellInfo const* p_Spell = GetSpellInfo();

                if (p_Spell->Id == eSpells::SpellDetonateMarkOfFrostAoe)
                    p_Targets.remove_if(JadeCore::UnitAuraCheck(false, eSpells::SpellMarkOfFrostDot));
                else if (p_Spell->Id == eSpells::SpellDetonateSearingBrandAoe)
                    p_Targets.remove_if(JadeCore::UnitAuraCheck(false, eSpells::SpellSearingBrandDot));
            }

            void HandleOnHit(SpellEffIndex /**/)
            {
                if (!GetHitUnit() || !GetSpellInfo())
                    return;

                if (GetSpellInfo()->Id == eSpells::SpellDetonateMarkOfFrostAoe)
                    GetCaster()->CastSpell(GetHitUnit(), eSpells::SpellDetonateMarkOfFrostAt, true);
                else if (GetSpellInfo()->Id == eSpells::SpellDetonateSearingBrandAoe)
                    GetCaster()->CastSpell(GetHitUnit(), eSpells::SpellDetonateSearingBrandAt, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_aluriel_detonate_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_aluriel_detonate_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }

        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_aluriel_detonate_SpellScript();
        }
};

/// Detonate Arcane Orb - 213390
class spell_aluriel_detonate_arcane_orb : public SpellScriptLoader
{
    public:
        spell_aluriel_detonate_arcane_orb() : SpellScriptLoader("spell_aluriel_detonate_arcane_orb")
        {}

        class spell_detonate_arcane_orb_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_detonate_arcane_orb_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if([](WorldObject* p_It)
                {
                    return p_It->GetEntry() != eCreatures::NpcArcaneOrb;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_detonate_arcane_orb_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_detonate_arcane_orb_SpellScript::HandleTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_detonate_arcane_orb_SpellScript();
        }
};

/// Animate: Mark of Frost  - 213853
/// Animate: Searing Brand  - 213567
/// Animate: Arcane Orb     - 213564
class spell_aluriel_animate : public SpellScriptLoader
{
    public:
        spell_aluriel_animate() : SpellScriptLoader("spell_aluriel_animate")
        {}

        class spell_aluriel_animate_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_aluriel_animate_SpellScript);

            void SummonIcyAdds(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.size() == 1)
                {
                    for (uint8 l_I = 0; l_I < 4; ++l_I)
                        GetCaster()->SummonCreature(eCreatures::NpcIcyEnchantment, *p_Targets.front());
                }
                else if (p_Targets.size() == 2)
                {
                    for (uint8 l_I = 0; l_I < 2; ++l_I)
                        GetCaster()->SummonCreature(eCreatures::NpcIcyEnchantment, *p_Targets.front());

                    for (uint8 l_I = 0; l_I < 2; ++l_I)
                        GetCaster()->SummonCreature(eCreatures::NpcIcyEnchantment, *p_Targets.back());
                }
                else if (p_Targets.size() == 3)
                {
                    for (WorldObject*& p_It : p_Targets)
                        GetCaster()->SummonCreature(eCreatures::NpcIcyEnchantment, *p_It);

                    GetCaster()->SummonCreature(eCreatures::NpcIcyEnchantment, *p_Targets.back());
                }
                else if (p_Targets.size() >= 4)
                {
                    JadeCore::Containers::RandomResizeList(p_Targets, 4);

                    for (WorldObject*& p_It : p_Targets)
                        GetCaster()->SummonCreature(eCreatures::NpcIcyEnchantment, *p_It);
                }
            }

            void SummonFieryAdds(std::list<WorldObject*>& p_Targets)
            {
                for (auto & p_Itr : p_Targets)
                    GetCaster()->SummonCreature(eCreatures::NpcFieryEnchantment, *p_Itr);
            }

            void SummonArcaneAdds(std::list<WorldObject*>& p_Targets)
            {
                for (auto & p_Itr : p_Targets)
                {
                    GetCaster()->SummonCreature(eCreatures::NpcArcaneEnchantment, *p_Itr, TEMPSUMMON_TIMED_DESPAWN, 90 * IN_MILLISECONDS);

                    if (Unit* l_Add = p_Itr->ToUnit())
                        l_Add->RemoveAllAreasTrigger();
                }
            }

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty() || !GetSpellInfo())
                    return;

                uint32 l_Id = GetSpellInfo()->Id;

                switch (l_Id)
                {
                    case eSpells::SpellAnimateMarkOfFrostAoe:
                    {
                        p_Targets.remove_if([] (WorldObject*& p_It)
                        {
                            return p_It->GetEntry() != eCreatures::NpcMarkOfFrost;
                        });

                        SummonIcyAdds(p_Targets);
                        p_Targets.clear();
                        break;
                    }

                    case eSpells::SpellAnimateSearingBrandAoe:
                    {
                        p_Targets.remove_if([] (WorldObject*& p_It)
                        {
                            return p_It->GetEntry() != eCreatures::NpcSearingBrand;
                        });

                        SummonFieryAdds(p_Targets);
                        p_Targets.clear();
                        break;
                    }

                    case eSpells::SpellAnimateArcaneOrbAoe:
                    {
                        p_Targets.remove_if([] (WorldObject*& p_It)
                        {
                            return p_It->GetEntry() != eCreatures::NpcArcaneOrb;
                        });
                        SummonArcaneAdds(p_Targets);
                        break;
                    }

                    default:
                        break;
                }
            }

            void HandleAfterCast()
            {
                if (!GetCaster() || !GetSpellInfo())
                    return;

                Unit* l_Caster = GetCaster();
                uint32 l_Id = GetSpellInfo()->Id;

                switch (l_Id)
                {
                    case eSpells::SpellAnimateMarkOfFrostAoe:
                    {
                        l_Caster->RemoveAreaTrigger(eSpells::SpellDetonateMarkOfFrostAt);
                        break;
                    }

                    case eSpells::SpellAnimateSearingBrandAoe:
                    {
                        l_Caster->RemoveAreaTrigger(eSpells::SpellDetonateSearingBrandAt);
                        break;
                    }

                    default: break;
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_aluriel_animate_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                AfterCast += SpellCastFn(spell_aluriel_animate_SpellScript::HandleAfterCast);
            }

            uint8 l_Count;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_aluriel_animate_SpellScript();
        }
};

/// Mark of Frost Detonate Searcher - 233195
class spell_aluriel_mark_of_frost_searcher : public SpellScriptLoader
{
    public:
        spell_aluriel_mark_of_frost_searcher() : SpellScriptLoader("spell_aluriel_mark_of_frost_searcher")
        {}

        class spell_mark_of_frost_searcher_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mark_of_frost_searcher_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if([] (WorldObject*& p_Target)
                {
                    return p_Target == nullptr || p_Target->ToUnit()->isDead();
                });
            }

            void HandleOnHit(SpellEffIndex /**/)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                std::list<Player*> l_Targets;

                l_Target->GetPlayerListInGrid(l_Targets, 100.f);

                if (l_Targets.empty())
                    return;

                l_Targets.remove_if([] (Player* p_It)
                {
                    return p_It == nullptr || p_It->isDead();
                });

                l_Targets.sort([] (Player* p_It, Player* p_It2)
                {
                    return !p_It->HasAura(eSpells::SpellFrostbitten) && p_It2->HasAura(eSpells::SpellFrostbitten);
                });

                if (l_Targets.size() > 2)
                    l_Targets.resize(2);

                Aura* l_MarkOne = l_Caster->GetAura(eSpells::SpellMarkOfFrostDot);
                Aura* l_MarkTwo = l_Target->GetAura(eSpells::SpellMarkOfFrostDot);

                if (l_MarkOne != nullptr)
                {
                    Unit* l_NewTargetOne = l_Targets.back();

                    if (l_NewTargetOne != nullptr)
                        return;

                    l_NewTargetOne->CastSpell(l_NewTargetOne, eSpells::SpellMarkOfFrostDot, true);

                    if (Aura* l_Aura = l_NewTargetOne->GetAura(eSpells::SpellMarkOfFrostDot))
                    {
                        l_Aura->SetDuration(l_MarkOne->GetDuration());
                        l_Aura->SetMaxDuration(l_MarkOne->GetDuration());
                    }

                    l_Caster->CastSpell(l_Caster, eSpells::SpellMarkOfFrostExplosion, true);
                }

                if (l_MarkTwo != nullptr && l_Targets.size() > 1)
                {
                    Unit* l_NewTargetTwo = l_Targets.front();

                    if (l_NewTargetTwo != nullptr)
                        return;

                    l_NewTargetTwo->CastSpell(l_NewTargetTwo, eSpells::SpellMarkOfFrostDot, true);

                    if (Aura* l_Aura = l_NewTargetTwo->GetAura(eSpells::SpellMarkOfFrostDot))
                    {
                        l_Aura->SetDuration(l_MarkTwo->GetDuration());
                        l_Aura->SetMaxDuration(l_MarkTwo->GetDuration());
                    }
                }

                l_Caster->RemoveAurasDueToSpell(eSpells::SpellMarkOfFrostDot);
                l_Target->RemoveAurasDueToSpell(eSpells::SpellMarkOfFrostDot);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_mark_of_frost_searcher_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mark_of_frost_searcher_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };


        SpellScript* GetSpellScript() const override
        {
            return new spell_mark_of_frost_searcher_SpellScript();
        }
};

/// Detonate: Arcane Orb - 213328
class spell_aluriel_detonate_arcane_orb_dmg : public SpellScriptLoader
{
    public:
        spell_aluriel_detonate_arcane_orb_dmg() : SpellScriptLoader("spell_aluriel_detonate_arcane_orb_dmg") { }

        class spell_aluriel_detonate_arcane_orb_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_aluriel_detonate_arcane_orb_SpellScript);

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                float l_Distance = GetCaster()->GetExactDist2d(GetHitUnit()) * 1.2f;
                uint32 l_Pct = 1;

                if (l_Distance < 100.0f)
                    l_Pct = 100 - l_Distance;

                uint32 damage = CalculatePct(GetHitDamage(), l_Pct);
                SetHitDamage(damage);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_aluriel_detonate_arcane_orb_SpellScript::HandleDamage, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_aluriel_detonate_arcane_orb_SpellScript();
        }
};

/// Searing Brand - 213166
class spell_aluriel_searing_brand : public SpellScriptLoader
{
    public:
        spell_aluriel_searing_brand() : SpellScriptLoader("spell_aluriel_searing_brand") { }

        class spell_aluriel_searing_brand_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_aluriel_searing_brand_AuraScript);

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    if (GetTargetApplication()->GetRemoveMode() == AuraRemoveMode::AURA_REMOVE_BY_DEATH)
                    {
                        if (InstanceScript* l_Instance = l_Target->GetInstanceScript())
                        {
                            if (l_Instance->GetBossState(eData::DataSpellbladeAluriel) == EncounterState::IN_PROGRESS)
                            {
                                if (Creature* l_Aluriel = Creature::GetCreature(*l_Target, l_Instance->GetData64(eCreatures::BossSpellbladeAluriel)))
                                    l_Target->CastSpell(l_Target, eSpells::SpellDetonateSearingBrandAt, true, nullptr, nullptr, l_Aluriel->GetGUID());
                            }
                        }
                    }
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_aluriel_searing_brand_AuraScript::AfterRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PERIODIC_DAMAGE, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_aluriel_searing_brand_AuraScript();
        }
};

/// Bound in Flame - 216257
class spell_aluriel_bound_in_flame : public SpellScriptLoader
{
    public:
        spell_aluriel_bound_in_flame() : SpellScriptLoader("spell_aluriel_bound_in_flame") { }

        class spell_aluriel_bound_in_flame_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_aluriel_bound_in_flame_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                p_Targets.remove_if([](WorldObject*& p_Itr)
                {
                    return p_Itr->GetEntry() != eCreatures::NpcFieryEnchantment;
                });
            }

            void HandleOnHit(SpellEffIndex /**/)
            {
                if (GetCaster() == nullptr || GetHitUnit() == nullptr)
                    return;

                GetCaster()->CastSpell(GetCaster(), eSpells::SpellBoundInFlame, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_aluriel_bound_in_flame_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_aluriel_bound_in_flame_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_aluriel_bound_in_flame_SpellScript();
        }
};

/// Armageddon - 213569
class spell_aluriel_armageddon : public SpellScriptLoader
{
  public:
    spell_aluriel_armageddon() : SpellScriptLoader("spell_aluriel_armageddon")
    {}

    class spell_armageddon_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_armageddon_AuraScript);

        void HandleOnPeriodic(AuraEffect const* /**/)
        {
            if (GetUnitOwner() == nullptr)
                return;

            GetUnitOwner()->CastSpell(GetUnitOwner(), eSpells::SpellArmageddonDmg, true);
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_armageddon_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_armageddon_AuraScript();
    }
};

/// Frozen Tempest - 213083
class at_aluriel_frozen_tempest : public AreaTriggerEntityScript
{
    public:
        at_aluriel_frozen_tempest() : AreaTriggerEntityScript("at_aluriel_frozen_tempest")
        {
            m_IsRemoved = false;
        }

        void OnRemove(AreaTrigger* p_At, uint32 /**/) override
        {
            m_IsRemoved = true;

            for (auto& p_Itr : m_Targets)
            {
                Unit* l_Target = sObjectAccessor->GetUnit(*p_At, p_Itr);
                if (l_Target != nullptr)
                    OnRemoveTarget(p_At, l_Target);
            }

            m_Targets.clear();
        }

        bool OnAddTarget(AreaTrigger* p_At, Unit* p_Target) override
        {
            if (p_Target == nullptr)
                return false;

            if (!p_Target->IsPlayer())
                return false;

            if (!p_Target->ToPlayer()->HasMovementForce())
                p_Target->ToPlayer()->SendApplyMovementForce(p_At->GetGUID(), true, *p_At, -3, 1);

            if (!p_Target->HasAura(eSpells::SpellFrozenTempestDot))
                p_At->GetCaster()->CastSpell(p_Target, eSpells::SpellFrozenTempestDot, true);

            m_Targets.insert(p_Target->GetGUID());
            return true;
        }

        bool OnRemoveTarget(AreaTrigger* p_At, Unit* p_Target) override
        {
            if (p_Target == nullptr)
                return true;

            if (p_Target->IsPlayer())
            {
                p_Target->RemoveAurasDueToSpell(eSpells::SpellFrozenTempestDot);

                if (p_Target->ToPlayer()->HasMovementForce(p_At->GetGUID()))
                {
                    p_Target->ToPlayer()->SendApplyMovementForce(p_At->GetGUID(), false, *p_At, -3);

                    if (p_At->GetCaster()->isDead())
                        return true;

                    if (m_IsRemoved)
                        return true;

                    Creature* l_Tomb = p_Target->SummonCreature(eCreatures::NpcIceShards, *p_Target, TEMPSUMMON_CORPSE_DESPAWN);

                    if (l_Tomb != nullptr && l_Tomb->IsAIEnabled)
                    {
                        l_Tomb->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                        l_Tomb->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
                        l_Tomb->GetAI()->SetGUID(p_Target->GetGUID(), eDataAluriel::DataMarkedPlayers);
                        p_Target->CastSpell(p_Target, eSpells::SpellEntombedInIce, true);
                    }

                    m_Targets.erase(p_Target->GetGUID());
                }
            }

            return true;
        }

        std::unordered_set<uint64> m_Targets;
        bool m_IsRemoved;

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_aluriel_frozen_tempest();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_spellblade_aluriel()
{
    /// Boss
    new boss_spellblade_aluriel();

    /// Creatures
    new npc_aluriel_enchantment_add();
    new npc_aluriel_arcane_orb();
    new npc_aluriel_fel_soul();
    new npc_aluriel_spellblade_aluriel_image();
    new npc_aluriel_ice_shard();

    /// Spells
    new spell_aluriel_enchantment();
    new spell_aluriel_annihilate();
    new spell_aluriel_annihilate_dmg();

    new spell_aluriel_mark_of_frost_aoe();
    new spell_aluriel_mark_of_frost_fixate();
    new spell_aluriel_replicate_mark_of_frost();
    new spell_aluriel_mark_of_frost_dot();

    new spell_aluriel_searing_brand_aoe();
    new spell_aluriel_searing_brand();
    new spell_aluriel_searing_brand_fixate();
    new spell_aluriel_replicate_searing_brand_aoe();

    new spell_aluriel_detonate();
    new spell_aluriel_animate();

    new spell_aluriel_arcane_orb_aoe();
    new spell_aluriel_replicate_arcane_orb_aoe();
    new spell_aluriel_detonate_arcane_orb();
    new spell_aluriel_detonate_arcane_orb_dmg();
    new spell_aluriel_armageddon();

    new spell_aluriel_bound_in_flame();

    new spell_aluriel_whirling_barrage();
    new spell_aluriel_fel_stomp_aoe();

    new at_aluriel_frozen_tempest();
}
#endif
