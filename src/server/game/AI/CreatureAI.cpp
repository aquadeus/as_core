////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "CreatureAI.h"
#include "CreatureAIImpl.h"
#include "World.h"
#include "SpellMgr.h"
#include "Vehicle.h"
#include "Log.h"
#include "MapReference.h"
#include "Player.h"
#include "CreatureTextMgr.h"

//Disable CreatureAI when charmed
void CreatureAI::OnCharmed(bool /*apply*/)
{
    /// Hackfix for Antoran High Command (Antorus raid)
    switch (me->GetEntry())
    {
        case 122554: ///< Admiral's pod
        case 123020: ///< Engineer's pod
        case 123013: ///< General's pod
            return;
        default:
            break;
    }

    //me->IsAIEnabled = !apply;*/
    me->NeedChangeAI = true;
    me->IsAIEnabled = false;
}

void CreatureAI::SetFlyMode(bool p_Fly)
{
    me->SetCanFly(p_Fly);
    me->SetDisableGravity(p_Fly);

    if (p_Fly)
        me->SetByteFlag(UNIT_FIELD_ANIM_TIER, UNIT_BYTES_1_OFFSET_ANIM_TIER, UNIT_BYTE1_FLAG_HOVER);
    else
        me->RemoveByteFlag(UNIT_FIELD_ANIM_TIER, UNIT_BYTES_1_OFFSET_ANIM_TIER, UNIT_BYTE1_FLAG_HOVER);
}

void CreatureAI::ApplyAllImmunities(bool p_Apply)
{
    me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_INTERRUPT_CAST, p_Apply);
    me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK, p_Apply);
    me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK_DEST, p_Apply);

    me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_SILENCE, p_Apply);
    me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_FEAR, p_Apply);
    me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_STUN, p_Apply);
    me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_SLEEP, p_Apply);
    me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_CHARM, p_Apply);
    me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_SAPPED, p_Apply);
    me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_HORROR, p_Apply);
    me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_POLYMORPH, p_Apply);
    me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_DISORIENTED, p_Apply);
    me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_FREEZE, p_Apply);
    me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_KNOCKOUT, p_Apply);
    me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_GRIP, p_Apply);
}

AISpellInfoType* GetAISpellInfo(uint32 i) { return &CreatureAI::AISpellInfo[i]; }

void CreatureAI::Talk(uint8 id, uint64 WhisperGuid, uint32 range)
{
    TextRange _range = TextRange(range);
    if (range == TEXT_RANGE_NORMAL)
        _range = (me->GetMap()->IsDungeon() ? TEXT_RANGE_ZONE : TEXT_RANGE_NORMAL);
    sCreatureTextMgr->SendChat(me, id, WhisperGuid, CHAT_MSG_ADDON, LANG_ADDON, _range);
}

void CreatureAI::PersonalTalk(uint8 p_Id, uint64 p_ReceiverGuid, uint32 p_Delay, uint32 p_Range)
{
    TextRange _range = TextRange(p_Range);
    if (p_Range == TEXT_RANGE_NORMAL)
        _range = (me->GetMap()->IsDungeon() ? TEXT_RANGE_ZONE : TEXT_RANGE_NORMAL);

    if (p_Delay)
    {
        Creature* l_Me = me;
        me->AddDelayedEvent([l_Me, p_Id, p_ReceiverGuid, _range]() -> void
        {
            sCreatureTextMgr->SendChat(l_Me, p_Id, p_ReceiverGuid, CHAT_MSG_ADDON, LANG_ADDON, _range, 0, TEAM_OTHER, false, nullptr, true);
        }, p_Delay);
    }
    else
    {
        sCreatureTextMgr->SendChat(me, p_Id, p_ReceiverGuid, CHAT_MSG_ADDON, LANG_ADDON, _range, 0, TEAM_OTHER, false, nullptr, true);
    }
}

void CreatureAI::WhisperToPlayer(uint8 p_Id, uint64 p_WhisperGuid)
{
    uint32 l_AccountId = sObjectMgr->GetPlayerAccountIdByGUID(p_WhisperGuid);

    if (l_AccountId == 0)
        return;

    std::string l_Str = sCreatureTextMgr->GetLocalizedChatString(me->GetEntry(), me->getGender(), p_Id, 0, sWorld->GetAccountLocale(l_AccountId));
    if (l_Str.empty())
        return;

    auto & l_TextMap = sCreatureTextMgr->GetTextMap();

    CreatureTextMap::const_iterator sList = l_TextMap.find(me->GetEntry());

    if (sList == l_TextMap.end())
        return;

    CreatureTextHolder const& l_TextHolder = sList->second;
    CreatureTextHolder::const_iterator l_Itr = l_TextHolder.find(p_Id);

    if (l_Itr == l_TextHolder.end())
        return;

    CreatureTextGroup l_TextGroup = l_Itr->second;
    if (l_TextGroup.size() > 1)
    {
        /// To Do Add Checkfor a Text in same Group
    }
    else
    {
        auto l_TextGroupItr = l_TextGroup.begin();
        uint32 l_Sound = 0;

        if (BroadcastTextEntry const* l_BroadcastText = sBroadcastTextStore.LookupEntry(l_TextGroupItr->BroadcastTextID))
        {
            uint8 l_Gender = me->getGender();
            if (l_Gender < Gender::GENDER_NONE && l_BroadcastText->SoundID[l_Gender])
                l_Sound = l_BroadcastText->SoundID[l_Gender];
            else if (l_Gender == Gender::GENDER_NONE)
                l_Sound = l_BroadcastText->SoundID[0] ? l_BroadcastText->SoundID[0] : l_BroadcastText->SoundID[1];
        }

        if (l_Sound != 0)
        {
            Player* l_Target = sObjectAccessor->GetPlayer(*me, p_WhisperGuid);

            if (l_Target != nullptr)
                me->PlayDirectSound(l_Sound, l_Target);
        }
    }

    me->MonsterWhisper(l_Str.c_str(), p_WhisperGuid, false);
}

void CreatureAI::DelayTalk(uint32 p_DelayTime, uint8 p_ID, uint64 p_WhisperGuid /*= 0*/, uint32 p_Range /*= 0*/)
{
    p_DelayTime *= IN_MILLISECONDS;

    me->m_Functions.AddFunction([this, p_ID, p_WhisperGuid]() -> void
    {
        if (me && me->isAlive())
            Talk(p_ID, p_WhisperGuid);
    }, me->m_Functions.CalculateTime(p_DelayTime));
}

void CreatureAI::ZoneTalk(uint8 p_ID, uint64 p_WhisperGuid /*= 0*/)
{
    sCreatureTextMgr->SendChat(me, p_ID, p_WhisperGuid, CHAT_MSG_ADDON, LANG_ADDON, TEXT_RANGE_ZONE);
}

void CreatureAI::DoZoneInCombat(Creature* creature /*= NULL*/, float maxRangeToNearestTarget /* = 50.0f*/)
{
    if (!creature)
        creature = me;

    if (!creature->CanHaveThreatList())
        return;

    Map* map = creature->GetMap();
    if (!map->IsDungeon())                                  //use IsDungeon instead of Instanceable, in case battlegrounds will be instantiated
        return;

    if (!creature->HasReactState(REACT_PASSIVE) && !creature->getVictim())
    {
        if (Unit* nearTarget = creature->SelectNearestTarget(maxRangeToNearestTarget))
            creature->AI()->AttackStart(nearTarget);
        else if (creature->isSummon())
        {
            if (Unit* summoner = creature->ToTempSummon()->GetSummoner())
            {
                Unit* target = summoner->getAttackerForHelper();
                if (!target && summoner->CanHaveThreatList() && !summoner->getThreatManager().isThreatListEmpty())
                    target = summoner->getThreatManager().getHostilTarget();
                if (target && (creature->IsFriendlyTo(summoner) || creature->IsHostileTo(target)))
                    creature->AI()->AttackStart(target);
            }
        }
    }

    if (!creature->HasReactState(REACT_PASSIVE) && !creature->getVictim())
        return;

    Map::PlayerList const& playerList = map->GetPlayers();

    if (playerList.isEmpty())
        return;

    for (Map::PlayerList::const_iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
    {
        if (Player* player = itr->getSource())
        {
            if (player->isGameMaster())
                continue;

            if (player->isAlive())
            {
                creature->SetInCombatWith(player);
                player->SetInCombatWith(creature);
                creature->AddThreat(player, 0.0f);
            }

            /* Causes certain things to never leave the threat list (Priest Lightwell, etc):
            for (Unit::ControlList::const_iterator itr = player->m_Controlled.begin(); itr != player->m_Controlled.end(); ++itr)
            {
                creature->SetInCombatWith(*itr);
                (*itr)->SetInCombatWith(creature);
                creature->AddThreat(*itr, 0.0f);
            }*/
        }
    }
}

void CreatureAI::DoAttackerAreaInCombat(Unit* attacker, float range, Unit* pUnit)
{
    if (!attacker)
        attacker = me;

    if (!pUnit)
        pUnit = me;

    Map* map = pUnit->GetMap();
    if (!map->IsDungeon())
        return;

    if (!pUnit->CanHaveThreatList() || pUnit->getThreatManager().isThreatListEmpty())
        return;

    Map::PlayerList const &PlayerList = map->GetPlayers();
    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
    {
        if (Player* i_pl = i->getSource())
        {
            if (i_pl->isAlive() && attacker->GetDistance(i_pl) <= range )
            {
                pUnit->SetInCombatWith(i_pl);
                i_pl->SetInCombatWith(pUnit);
                pUnit->AddThreat(i_pl, 0.0f);
            }
        }
    }
}

// scripts does not take care about MoveInLineOfSight loops
// MoveInLineOfSight can be called inside another MoveInLineOfSight and cause stack overflow
void CreatureAI::MoveInLineOfSight_Safe(Unit* who)
{
    if (m_MoveInLineOfSight_locked == true)
        return;
    m_MoveInLineOfSight_locked = true;
    MoveInLineOfSight(who);
    m_MoveInLineOfSight_locked = false;
}

void CreatureAI::MoveInLineOfSight(Unit* who)
{
    if (me->getVictim())
        return;

    if (me->GetCreatureType() == CREATURE_TYPE_NON_COMBAT_PET) // non-combat pets should just stand there and look good;)
        return;

    if (me->canStartAttack(who, false))
        AttackStart(who);
    //else if (who->getVictim() && me->IsFriendlyTo(who)
    //    && me->IsWithinDistInMap(who, sWorld->getIntConfig(CONFIG_CREATURE_FAMILY_ASSISTANCE_RADIUS))
    //    && me->canStartAttack(who->getVictim(), true)) // TODO: if we use true, it will not attack it when it arrives
    //    me->GetMotionMaster()->MoveChase(who->getVictim());
}

void CreatureAI::EnterEvadeMode()
{
    if (!_EnterEvadeMode())
        return;

    /// Otherwise me will be in evade mode forever
    if (!me->GetVehicle())
    {
        if (Unit* l_Owner = me->GetAnyOwner())
        {
            me->GetMotionMaster()->Clear(false);
            me->GetMotionMaster()->MoveFollow(l_Owner, PET_FOLLOW_DIST, me->GetFollowAngle(), MOTION_SLOT_ACTIVE);
        }
        else
        {
            /// Required to prevent attacking creatures that are evading and cause them to reenter combat
            /// Does not apply to MoveFollow
            me->AddUnitState(UNIT_STATE_EVADE);
            me->GetMotionMaster()->MoveTargetedHome();
        }
    }

    if (me->isWorldBoss())
    {
        if (InstanceScript* l_Instance = me->GetInstanceScript())
        {
            l_Instance->SendEncounterEnd(l_Instance->GetEncounterIDForBoss(me), false);
            l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_END, me);
            l_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);

            me->ClearDamageLog();
            me->ClearGroupDumps();
            me->SetEncounterStartTime(0);
        }
    }

    Reset();

    /// Use the same sequence of addtoworld, aireset may remove all summons!
    if (me->IsVehicle())
        me->GetVehicleKit()->Reset(true);

    me->SetLastDamagedTime(0);
}

bool CreatureAI::CanBeSeenByPlayers() const
{
    CreatureTemplate const* l_Template = me->GetCreatureTemplate();
    if (l_Template == nullptr)
        return false;

    if (!l_Template->VignetteID || l_Template->expansion < Expansion::EXPANSION_LEGION)
        return true;

    if (MS::WorldQuest::Template::GetTemplate(l_Template->VignetteID) == nullptr)
        return true;

    if (!sWorldQuestGenerator->HasQuestActive(l_Template->VignetteID))
        return false;

    return true;
}

void CreatureAI::UpdateOperations(uint32 const p_Diff)
{
    if (me->HasUnitState(UnitState::UNIT_STATE_EVADE))
        return;

    std::vector<std::function<void()>> l_OperationsReady;

    for (auto l_Iter = m_TimedDelayedOperations.begin(); l_Iter != m_TimedDelayedOperations.end();)
    {
        auto& l_Pair = *l_Iter;

        l_Pair.first -= p_Diff;

        if (l_Pair.first < 0)
        {
            l_OperationsReady.push_back(l_Pair.second);

            l_Iter = m_TimedDelayedOperations.erase(l_Iter);
        }
        else
            ++l_Iter;
    }

    for (auto l_Operation : l_OperationsReady)
        l_Operation();

    if (m_TimedDelayedOperations.empty() && !m_EmptyWarned)
    {
        m_EmptyWarned = true;
        LastOperationCalled();
    }
}

bool CreatureAI::IsInDisable()
{
    if (me->HasAuraType(SPELL_AURA_MOD_STUN) || me->HasAuraType(SPELL_AURA_MOD_FEAR) ||
        me->HasAuraType(SPELL_AURA_MOD_CHARM) || me->HasAuraType(SPELL_AURA_MOD_CONFUSE) ||
        me->HasAuraType(SPELL_AURA_MOD_ROOT) || me->HasAuraType(SPELL_AURA_MOD_FEAR_2) ||
        me->HasAuraType(SPELL_AURA_MOD_ROOT_2))
        return true;

    return false;
}

bool CreatureAI::IsInControl()
{
    if (me->HasAuraType(SPELL_AURA_MOD_STUN) || me->HasAuraType(SPELL_AURA_MOD_FEAR) ||
        me->HasAuraType(SPELL_AURA_MOD_CHARM) || me->HasAuraType(SPELL_AURA_MOD_CONFUSE) ||
        me->HasAuraType(SPELL_AURA_MOD_FEAR_2) || me->HasAuraType(SPELL_AURA_MOD_PACIFY_SILENCE))
        return true;

    return false;
}

/*void CreatureAI::AttackedBy(Unit* attacker)
{
    if (!me->getVictim())
        AttackStart(attacker);
}*/
