////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "Common.h"
#include "DatabaseEnv.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Opcodes.h"
#include "Log.h"
#include "UpdateMask.h"
#include "World.h"
#include "ObjectMgr.h"
#include "SpellMgr.h"
#include "Player.h"
#include "Pet.h"
#include "Unit.h"
#include "Totem.h"
#include "Spell.h"
#include "DynamicObject.h"
#include "Group.h"
#include "UpdateData.h"
#include "MapManager.h"
#include "ObjectAccessor.h"
#include "CellImpl.h"
#include "SharedDefines.h"
#include "LootMgr.h"
#include "VMapFactory.h"
#include "Battleground.h"
#include "BattlegroundKT.h"
#include "Util.h"
#include "TemporarySummon.h"
#include "Vehicle.h"
#include "SpellAuraEffects.h"
#include "ScriptMgr.h"
#include "ConditionMgr.h"
#include "DisableMgr.h"
#include "SpellScript.h"
#include "InstanceScript.h"
#include "SpellInfo.h"
#include "DB2Stores.h"
#include "Battlefield.h"
#include "BattlefieldMgr.h"
#include "GuildMgr.h"
#include "ScenarioMgr.h"
#include "HelperDefines.h"
#ifndef CROSS
#include "GarrisonMgr.hpp"
#endif /* not CROSS */

std::atomic<uint64> g_CastGUID(0);
extern pEffect SpellEffects[TOTAL_SPELL_EFFECTS];

#define MaxSpellQueueGcd 400

SpellDestination::SpellDestination()
{
    _position.Relocate(0, 0, 0, 0);
    _transportGUID = 0;
    _transportOffset.Relocate(0, 0, 0, 0);
}

SpellDestination::SpellDestination(float x, float y, float z, float orientation, uint32 mapId)
{
    _position.Relocate(x, y, z, orientation);
    _transportGUID = 0;
    _position.m_mapId = mapId;
}

SpellDestination::SpellDestination(Position const& pos)
{
    _position.Relocate(pos);
    _transportGUID = 0;
}

SpellDestination::SpellDestination(WorldObject const& wObj)
{
    _transportGUID = wObj.GetTransGUID();
    _transportOffset.Relocate(wObj.GetTransOffsetX(), wObj.GetTransOffsetY(), wObj.GetTransOffsetZ(), wObj.GetTransOffsetO());
    _position.Relocate(wObj);
    _position.SetOrientation(wObj.GetOrientation());
}


SpellCastTargets::SpellCastTargets() : m_elevation(0), m_speed(0), m_strTarget()
{
    m_objectTarget = nullptr;
    m_itemTarget   = nullptr;

    m_objectTargetGUID   = 0;
    m_itemTargetGUID   = 0;
    m_itemTargetEntry  = 0;

    m_targetMask = 0;
}

SpellCastTargets::~SpellCastTargets()
{
}

void SpellCastTargets::Read(ByteBuffer& data, Unit* caster)
{
    data >> m_targetMask;

    if (m_targetMask == TARGET_FLAG_NONE)
        return;

    if (m_targetMask & (TARGET_FLAG_UNIT | TARGET_FLAG_UNIT_MINIPET | TARGET_FLAG_GAMEOBJECT | TARGET_FLAG_CORPSE_ENEMY | TARGET_FLAG_CORPSE_ALLY))
        data.readPackGUID(m_objectTargetGUID);

    if (m_targetMask & (TARGET_FLAG_ITEM | TARGET_FLAG_TRADE_ITEM))
        data.readPackGUID(m_itemTargetGUID);

    if (m_targetMask & TARGET_FLAG_SOURCE_LOCATION)
    {
        data.readPackGUID(m_src._transportGUID);
        if (m_src._transportGUID)
            data >> m_src._transportOffset.PositionXYZStream();
        else
            data >> m_src._position.PositionXYZStream();
    }
    else
    {
        m_src._transportGUID = caster->GetTransGUID();
        if (m_src._transportGUID)
            m_src._transportOffset.Relocate(caster->GetTransOffsetX(), caster->GetTransOffsetY(), caster->GetTransOffsetZ(), caster->GetTransOffsetO());
        else
            m_src._position.Relocate(caster);
    }

    if (m_targetMask & TARGET_FLAG_DEST_LOCATION)
    {
        data.readPackGUID(m_dst._transportGUID);
        if (m_dst._transportGUID)
            data >> m_dst._transportOffset.PositionXYZStream();
        else
            data >> m_dst._position.PositionXYZStream();
    }
    else
    {
        m_dst._transportGUID = caster->GetTransGUID();
        if (m_dst._transportGUID)
            m_dst._transportOffset.Relocate(caster->GetTransOffsetX(), caster->GetTransOffsetY(), caster->GetTransOffsetZ(), caster->GetTransOffsetO());
        else
            m_dst._position.Relocate(caster);
    }

    if (m_targetMask & TARGET_FLAG_STRING)
        data >> m_strTarget;

    Update(caster);
}

void SpellCastTargets::Write(ByteBuffer& data)
{
    data << uint32(m_targetMask);

    if (m_targetMask & (TARGET_FLAG_UNIT | TARGET_FLAG_CORPSE_ALLY | TARGET_FLAG_GAMEOBJECT | TARGET_FLAG_CORPSE_ENEMY | TARGET_FLAG_UNIT_MINIPET))
        data.appendPackGUID(m_objectTargetGUID);

    if (m_targetMask & (TARGET_FLAG_ITEM | TARGET_FLAG_TRADE_ITEM))
    {
        if (m_itemTarget)
            data.appendPackGUID(m_itemTarget->GetGUID());
        else
            data << uint64(0);
    }

    if (m_targetMask & TARGET_FLAG_SOURCE_LOCATION)
    {
        data.appendPackGUID(m_src._transportGUID); // relative position guid here - transport for example
        if (m_src._transportGUID)
            data << m_src._transportOffset.PositionXYZStream();
        else
            data << m_src._position.PositionXYZStream();
    }

    if (m_targetMask & TARGET_FLAG_DEST_LOCATION)
    {
        data.appendPackGUID(m_dst._transportGUID); // relative position guid here - transport for example
        if (m_dst._transportGUID)
            data << m_dst._transportOffset.PositionXYZStream();
        else
            data << m_dst._position.PositionXYZStream();
    }

    if (m_targetMask & TARGET_FLAG_STRING)
        data << m_strTarget;
}

uint64 SpellCastTargets::GetUnitTargetGUID() const
{
    switch (GUID_HIPART(m_objectTargetGUID))
    {
        case HIGHGUID_PLAYER:
        case HIGHGUID_VEHICLE:
        case HIGHGUID_UNIT:
        case HIGHGUID_PET:
            return m_objectTargetGUID;
        default:
            return 0LL;
    }
}

Unit* SpellCastTargets::GetUnitTarget() const
{
    if (m_objectTarget)
        return m_objectTarget->ToUnit();
    return NULL;
}

void SpellCastTargets::SetUnitTarget(Unit* target)
{
    if (!target)
        return;

    m_objectTarget = target;
    m_objectTargetGUID = target->GetGUID();
    m_targetMask |= TARGET_FLAG_UNIT;
}

GameObject* SpellCastTargets::GetGOTarget() const
{
    if (m_objectTarget)
        return m_objectTarget->ToGameObject();
    return NULL;
}


void SpellCastTargets::SetGOTarget(GameObject* target)
{
    if (!target)
        return;

    m_objectTarget = target;
    m_objectTargetGUID = target->GetGUID();
    m_targetMask |= TARGET_FLAG_GAMEOBJECT;
}

Corpse* SpellCastTargets::GetCorpseTarget() const
{
    if (m_objectTarget)
        return m_objectTarget->ToCorpse();
    return NULL;
}

WorldObject* SpellCastTargets::GetObjectTarget() const
{
    return m_objectTarget;
}

uint64 SpellCastTargets::GetObjectTargetGUID() const
{
    return m_objectTargetGUID;
}

void SpellCastTargets::RemoveObjectTarget()
{
    m_objectTarget = NULL;
    m_objectTargetGUID = 0LL;
    m_targetMask &= ~(TARGET_FLAG_UNIT_MASK | TARGET_FLAG_CORPSE_MASK | TARGET_FLAG_GAMEOBJECT_MASK);
}

void SpellCastTargets::SetItemTarget(Item* item)
{
    if (!item)
        return;

    m_itemTarget = item;
    m_itemTargetGUID = item->GetGUID();
    m_itemTargetEntry = item->GetEntry();
    m_targetMask |= TARGET_FLAG_ITEM;
}

void SpellCastTargets::SetTradeItemTarget(Player* caster)
{
    m_itemTargetGUID = uint64(TRADE_SLOT_NONTRADED);
    m_itemTargetEntry = 0;
    m_targetMask |= TARGET_FLAG_TRADE_ITEM;

    Update(caster);
}

void SpellCastTargets::UpdateTradeSlotItem()
{
    if (m_itemTarget && (m_targetMask & TARGET_FLAG_TRADE_ITEM))
    {
        m_itemTargetGUID = m_itemTarget->GetGUID();
        m_itemTargetEntry = m_itemTarget->GetEntry();
    }
}

SpellDestination const* SpellCastTargets::GetSrc() const
{
    return &m_src;
}

Position const* SpellCastTargets::GetSrcPos() const
{
    return &m_src._position;
}

void SpellCastTargets::SetSrc(float x, float y, float z)
{
    m_src = SpellDestination(x, y, z);
    m_targetMask |= TARGET_FLAG_SOURCE_LOCATION;
}

void SpellCastTargets::SetSrc(Position const& pos)
{
    m_src = SpellDestination(pos);
    m_targetMask |= TARGET_FLAG_SOURCE_LOCATION;
}

void SpellCastTargets::SetSrc(WorldObject const& wObj)
{
    m_src = SpellDestination(wObj);
    m_targetMask |= TARGET_FLAG_SOURCE_LOCATION;
}

void SpellCastTargets::ModSrc(Position const& pos)
{
    ASSERT(m_targetMask & TARGET_FLAG_SOURCE_LOCATION);

    if (m_src._transportGUID)
    {
        Position offset;
        m_src._position.GetPositionOffsetTo(pos, offset);
        m_src._transportOffset.RelocateOffset(offset);
    }
    m_src._position.Relocate(pos);
}

void SpellCastTargets::RemoveSrc()
{
    m_targetMask &= ~(TARGET_FLAG_SOURCE_LOCATION);
}

SpellDestination const* SpellCastTargets::GetDst() const
{
    return &m_dst;
}

WorldLocation const* SpellCastTargets::GetDstPos() const
{
    return &m_dst._position;
}

void SpellCastTargets::SetDst(float x, float y, float z, float orientation, uint32 mapId)
{
    m_dst = SpellDestination(x, y, z, orientation, mapId);
    m_targetMask |= TARGET_FLAG_DEST_LOCATION;
}

void SpellCastTargets::SetDst(Position const& pos)
{
    m_dst = SpellDestination(pos);
    m_targetMask |= TARGET_FLAG_DEST_LOCATION;
}

void SpellCastTargets::SetDst(WorldObject const& wObj)
{
    m_dst = SpellDestination(wObj);
    m_targetMask |= TARGET_FLAG_DEST_LOCATION;
}

void SpellCastTargets::SetDst(SpellCastTargets const& spellTargets)
{
    m_dst = spellTargets.m_dst;
    m_targetMask |= TARGET_FLAG_DEST_LOCATION;
}

void SpellCastTargets::ModDst(Position const& pos)
{
    ASSERT(m_targetMask & TARGET_FLAG_DEST_LOCATION);

    if (m_dst._transportGUID)
    {
        Position offset;
        m_dst._position.GetPositionOffsetTo(pos, offset);
        m_dst._transportOffset.RelocateOffset(offset);
    }
    m_dst._position.Relocate(pos);
}

void SpellCastTargets::RemoveDst()
{
    m_targetMask &= ~(TARGET_FLAG_DEST_LOCATION);
}

void SpellCastTargets::Update(Unit* p_Caster)
{
    m_objectTarget = m_objectTargetGUID ? ((m_objectTargetGUID == p_Caster->GetGUID()) ? p_Caster : ObjectAccessor::GetWorldObject(*p_Caster, m_objectTargetGUID)) : NULL;

    m_itemTarget = nullptr;
    if (p_Caster->IsPlayer())
    {
        Player* l_Player = p_Caster->ToPlayer();
        if (m_targetMask & TARGET_FLAG_ITEM)
            m_itemTarget = l_Player->GetItemByGuid(m_itemTargetGUID);
        else if (m_targetMask & TARGET_FLAG_TRADE_ITEM)
        {
            if (m_itemTargetGUID == TRADE_SLOT_NONTRADED && l_Player->GetTradeData()) // Here it is not guid but slot. Also prevents hacking slots
            {
                if (Player* l_Trader = l_Player->GetTradeData()->GetTrader())
                {
                    if (TradeData* l_HisTrade = l_Trader->GetTradeData())
                        m_itemTarget = l_HisTrade->GetItem(TRADE_SLOT_TRADED_COUNT);
                }
            }
        }

        if (m_itemTarget)
            m_itemTargetEntry = m_itemTarget->GetEntry();
    }

    // Update positions by transport move
    if (HasSrc() && m_src._transportGUID)
    {
        if (WorldObject* transport = ObjectAccessor::GetWorldObject(*p_Caster, m_src._transportGUID))
        {
            m_src._position.Relocate(transport);
            m_src._position.RelocateOffset(m_src._transportOffset);
        }
    }

    if (HasDst() && m_dst._transportGUID)
    {
        if (WorldObject* transport = ObjectAccessor::GetWorldObject(*p_Caster, m_dst._transportGUID))
        {
            m_dst._position.Relocate(transport);
            m_dst._position.RelocateOffset(m_dst._transportOffset);
        }
    }
}

void SpellCastTargets::OutDebug() const
{
    if (!m_targetMask)
        sLog->outInfo(LOG_FILTER_SPELLS_AURAS, "No targets");

    sLog->outInfo(LOG_FILTER_SPELLS_AURAS, "target mask: %u", m_targetMask);
    if (m_targetMask & (TARGET_FLAG_UNIT_MASK | TARGET_FLAG_CORPSE_MASK | TARGET_FLAG_GAMEOBJECT_MASK))
        sLog->outInfo(LOG_FILTER_SPELLS_AURAS, "Object target: " UI64FMTD, m_objectTargetGUID);
    if (m_targetMask & TARGET_FLAG_ITEM)
        sLog->outInfo(LOG_FILTER_SPELLS_AURAS, "Item target: " UI64FMTD, m_itemTargetGUID);
    if (m_targetMask & TARGET_FLAG_TRADE_ITEM)
        sLog->outInfo(LOG_FILTER_SPELLS_AURAS, "Trade item target: " UI64FMTD, m_itemTargetGUID);
    if (m_targetMask & TARGET_FLAG_SOURCE_LOCATION)
        sLog->outInfo(LOG_FILTER_SPELLS_AURAS, "Source location: transport guid:" UI64FMTD " trans offset: %s position: %s", m_src._transportGUID, m_src._transportOffset.ToString().c_str(), m_src._position.ToString().c_str());
    if (m_targetMask & TARGET_FLAG_DEST_LOCATION)
        sLog->outInfo(LOG_FILTER_SPELLS_AURAS, "Destination location: transport guid:" UI64FMTD " trans offset: %s position: %s", m_dst._transportGUID, m_dst._transportOffset.ToString().c_str(), m_dst._position.ToString().c_str());
    if (m_targetMask & TARGET_FLAG_STRING)
        sLog->outInfo(LOG_FILTER_SPELLS_AURAS, "String: %s", m_strTarget.c_str());
    sLog->outInfo(LOG_FILTER_SPELLS_AURAS, "speed: %f", m_speed);
    sLog->outInfo(LOG_FILTER_SPELLS_AURAS, "elevation: %f", m_elevation);
}

SpellValue::SpellValue(SpellInfo const* proto)
{
    for (uint32 i = 0; i < SpellEffIndex::MAX_EFFECTS; ++i)
        EffectBasePoints[i] = proto->Effects[i].BasePoints;

    MaxAffectedTargets = proto->MaxAffectedTargets;
    RadiusMod = 1.0f;
    AuraStackAmount = 1;
    IgnoreCooldowns = false;
}

Spell::Spell(Unit* caster, SpellInfo const* info, TriggerCastFlags triggerFlags, uint64 originalCasterGUID, bool skipCheck) :
m_spellInfo(sSpellMgr->GetSpellForDifficultyFromSpell(info, caster)),
m_caster((info->AttributesEx6 & SPELL_ATTR6_CAST_BY_CHARMER && caster->GetCharmerOrOwner()) ? caster->GetCharmerOrOwner() : caster),
m_spellValue(new SpellValue(m_spellInfo)), m_preGeneratedPath(PathGenerator(m_caster)), m_CastID(GUID_TYPE_CAST, 0, info->Id, 0), m_spellMissMask(0)
{

    m_customError = SPELL_CUSTOM_ERROR_NONE;
    m_skipCheck = skipCheck;
    m_selfContainer = NULL;
    m_referencedFromCurrentSpell = false;
    m_executedCurrently = false;
    m_needComboPoints = m_spellInfo->NeedsComboPoints();
    m_comboPointGain = 0;
    m_delayStart = 0;
    m_delayAtDamageCount = 0;

    m_applyMultiplierMask = 0;
    m_auraScaleMask = 0;
    destTarget = nullptr;

    memset(m_Misc, 0, sizeof(m_Misc));

    LoadAttrDummy();

    // Get data for type of attack
    switch (m_spellInfo->DmgClass)
    {
        case SPELL_DAMAGE_CLASS_MELEE:
            if (AttributesCustomEx3 & SPELL_ATTR3_REQ_OFFHAND)
                m_attackType = WeaponAttackType::OffAttack;
            else
                m_attackType = WeaponAttackType::BaseAttack;
            break;
        case SPELL_DAMAGE_CLASS_RANGED:
            m_attackType = m_spellInfo->IsRangedWeaponSpell() ? WeaponAttackType::RangedAttack : WeaponAttackType::BaseAttack;
            break;
        default:
                                                            // Wands
            if (AttributesCustomEx2 & SPELL_ATTR2_AUTOREPEAT_FLAG)
                m_attackType = WeaponAttackType::RangedAttack;
            else
                m_attackType = WeaponAttackType::BaseAttack;
            break;
    }

    m_spellSchoolMask = info->GetSchoolMask();           // Can be override for some spell (wand shoot for example)

    if (m_attackType == WeaponAttackType::RangedAttack)
        // wand case
        if ((m_caster->getClassMask() & CLASSMASK_WAND_USERS) != 0 && m_caster->IsPlayer())
            if (Item* pItem = m_caster->ToPlayer()->GetWeaponForAttack(WeaponAttackType::RangedAttack))
                m_spellSchoolMask = SpellSchoolMask(1 << pItem->GetTemplate()->DamageType);

    Unit::AuraEffectList const& l_ChangeSpellSchoolMask = m_caster->GetAuraEffectsByType(SPELL_AURA_CHANGE_SPELL_SCHOOL_MASK);
    for (Unit::AuraEffectList::const_iterator l_Iter = l_ChangeSpellSchoolMask.begin(); l_Iter != l_ChangeSpellSchoolMask.end(); ++l_Iter)
    {
        if ((*l_Iter)->IsAffectingSpell(m_spellInfo))
            m_spellSchoolMask = SpellSchoolMask((*l_Iter)->GetMiscValue());
    }

    if (originalCasterGUID)
        m_originalCasterGUID = originalCasterGUID;
    else
        m_originalCasterGUID = m_caster->GetGUID();

    if (m_originalCasterGUID == m_caster->GetGUID())
        m_originalCaster = m_caster;
    else
    {
        m_originalCaster = ObjectAccessor::GetUnit(*m_caster, m_originalCasterGUID);
        if (m_originalCaster && !m_originalCaster->IsInWorld())
            m_originalCaster = NULL;
    }

    m_spellState = SPELL_STATE_NULL;
    _triggeredCastFlags = triggerFlags;
    if (AttributesCustomEx4 & SPELL_ATTR4_CAN_CAST_WHILE_CASTING)
        _triggeredCastFlags = TriggerCastFlags(uint32(_triggeredCastFlags) | TRIGGERED_IGNORE_CAST_IN_PROGRESS | TRIGGERED_CAST_DIRECTLY);

    m_CastItem = NULL;
    m_castItemGUID = 0;
    m_castItemLevel = -1;

    m_AreaTriggerGUID = 0;

    m_SpellGuid = 0;

    /// Target used in SpellEffect
    unitTarget    = nullptr;
    itemTarget    = nullptr;
    gameObjTarget = nullptr;
    focusObject   = nullptr;
    m_AreaTrigger = nullptr;

    m_CurrentLogDamageStream = nullptr;
    m_MinDamageLog           = 0;
    m_ShoudLogDamage         = m_caster->CanApplyPvPSpellModifiers() && sSpellMgr->GetSpellDamageLog(m_spellInfo->Id, m_MinDamageLog);

    m_FromClient = false;
    m_CastItemEntry = 0;
    m_preCastSpell = 0;
    m_triggeredByAuraSpell  = NULL;
    m_triggeredByAura = nullptr;
    m_spellAura = nullptr;
    isStolen = false;

    m_CustomCritChance = -1.0f;

    //Auto Shot & Shoot (wand)
    m_autoRepeat = m_spellInfo->IsAutoRepeatRangedSpell();

    m_runesState = 0;
    m_casttime = 0;                                         // setup to correct value in Spell::prepare, must not be used before.
    m_timer = 0;                                            // will set to castime in prepare
    m_channeledDuration = 0;                                // will be setup in Spell::handle_immediate
    m_periodicDamageModifier = 0.0f;

    m_channelTargetEffectMask = 0;

    m_redirected = false;

    // Determine if spell can be reflected back to the caster
    // Patch 1.2 notes: Spell Reflection no longer reflects abilities
    m_canReflect = ((m_spellInfo->DmgClass == SPELL_DAMAGE_CLASS_MAGIC && !(AttributesCustom & SPELL_ATTR0_ABILITY)
         && !(AttributesCustomEx & SPELL_ATTR1_CANT_BE_REFLECTED) && !(AttributesCustom & SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY)
         && !m_spellInfo->IsPassive() && !m_spellInfo->IsHealingSpell() && !m_spellInfo->IsPositive()) || m_spellInfo->Id == 7268 || m_spellInfo->Id == 8122); ///< Arcane Missiles/Psychic Scream

    CleanupTargetList();
    m_effectExecuteData.clear();

    if (m_caster == NULL)
        sLog->outAshran("m_caster is null!! spellId %u", m_spellInfo->Id);

    for (uint8 i = 0; i < m_spellInfo->EffectCount; ++i)
        m_destTargets[i] = SpellDestination(*m_caster);

    m_SpellVisualID = m_spellInfo->GetSpellXSpellVisualId(m_caster);

    std::list<AuraEffect*>const& l_VisualModifiers = m_caster->GetAuraEffectsByType(SPELL_AURA_CHANGE_VISUAL_EFFECT);
    for (AuraEffect* l_Effect : l_VisualModifiers)
    {
        if (l_Effect->GetMiscValue() == m_spellInfo->Id
            && l_Effect->GetMiscValueB())
        {
            m_SpellVisualID = sSpellMgr->GetSpellXVisualForSpellID(l_Effect->GetMiscValueB(), caster->GetMap()->GetDifficultyID(), m_SpellVisualID);
            break;
        }
    }

    m_addpower = 0;
    m_addptype = -1;

    m_ArtifactKnowledgeLevelOfCastItem = 0;
}

Spell::~Spell()
{
    // unload scripts
    while (!m_loadedScripts.empty())
    {
        std::list<SpellScript*>::iterator itr = m_loadedScripts.begin();
        (*itr)->_Unload();
        delete (*itr);
        m_loadedScripts.erase(itr);
    }

    if (m_referencedFromCurrentSpell && m_selfContainer && *m_selfContainer == this)
    {
        // Clean the reference to avoid later crash.
        // If this error is repeating, we may have to add an ASSERT to better track down how we get into this case.
        *m_selfContainer = NULL;
    }

    if (m_caster && m_caster->IsPlayer())
        ASSERT(m_caster->ToPlayer()->m_spellModTakingSpell != this);
    delete m_spellValue;

    CheckEffectExecuteData();
}

void Spell::SetTriggerCastData(TriggerCastData p_TriggerCastData)
{
    m_TriggerCastData = p_TriggerCastData;

    if (m_TriggerCastData.triggeredByAura)
    {
        m_triggeredByAuraSpell = m_TriggerCastData.triggeredByAura->GetSpellInfo();
        m_triggeredByAura = m_TriggerCastData.triggeredByAura;
    }

    if (m_TriggerCastData.castItem)
        m_CastItem = m_TriggerCastData.castItem;

    if (m_TriggerCastData.originalCaster)
        m_originalCasterGUID = m_TriggerCastData.originalCaster;

    if (m_originalCasterGUID == m_caster->GetGUID())
        m_originalCaster = m_caster;
    else
    {
        m_originalCaster = ObjectAccessor::GetUnit(*m_caster, m_originalCasterGUID);
        if (m_originalCaster && !m_originalCaster->IsInWorld())
            m_originalCaster = nullptr;
    }

    if (m_skipCheck != m_TriggerCastData.skipCheck)
        m_skipCheck = m_TriggerCastData.skipCheck;

    if (m_SpellGuid != m_TriggerCastData.spellGuid)
        m_SpellGuid = m_TriggerCastData.spellGuid;

    if (m_Misc[0] != m_TriggerCastData.miscData0)
        m_Misc[0] = m_TriggerCastData.miscData0;

    if (m_Misc[1] != m_TriggerCastData.miscData1)
        m_Misc[1] = m_TriggerCastData.miscData1;
}

uint64 Spell::GenerateNewSpellGUID()
{
    if (g_CastGUID >= 0xEFFFFFFFFF)
        g_CastGUID = 0;

    return ++g_CastGUID;
}

void Spell::InitExplicitTargets(SpellCastTargets const& targets)
{
    m_targets = targets;
    // this function tries to correct spell explicit targets for spell
    // client doesn't send explicit targets correctly sometimes - we need to fix such spells serverside
    // this also makes sure that we correctly send explicit targets to client (removes redundant data)
    uint32 neededTargets = m_spellInfo->GetExplicitTargetMask();

    if (WorldObject* target = m_targets.GetObjectTarget())
    {
        // check if object target is valid with needed target flags
        // for unit case allow corpse target mask because player with not released corpse is a unit target
        if ((target->ToUnit() && !(neededTargets & (TARGET_FLAG_UNIT_MASK | TARGET_FLAG_CORPSE_MASK)))
            || (target->ToGameObject() && !(neededTargets & TARGET_FLAG_GAMEOBJECT_MASK))
            || (target->ToCorpse() && !(neededTargets & TARGET_FLAG_CORPSE_MASK)))
            if (!m_spellInfo->HasPersistenAura() || !(m_targets.GetTargetMask()&TARGET_FLAG_UNIT_MASK))
                m_targets.RemoveObjectTarget();
    }
    else
    {
        // try to select correct unit target if not provided by client or by serverside cast
        if (neededTargets & (TARGET_FLAG_UNIT_MASK))
        {
            Unit* unit = NULL;
            // try to use player selection as a target
            if (Player* playerCaster = m_caster->ToPlayer())
            {
                // selection has to be found and to be valid target for the spell
                if (Unit* selectedUnit = ObjectAccessor::GetUnit(*m_caster, playerCaster->GetSelection()))
                    if (m_spellInfo->CheckExplicitTarget(m_caster, selectedUnit) == SPELL_CAST_OK)
                        unit = selectedUnit;
            }
            // try to use attacked unit as a target
            else if ((m_caster->GetTypeId() == TYPEID_UNIT) && neededTargets & (TARGET_FLAG_UNIT_ENEMY | TARGET_FLAG_UNIT))
                unit = m_caster->getVictim();

            // didn't find anything - let's use self as target
            if (!unit && neededTargets & (TARGET_FLAG_UNIT_RAID | TARGET_FLAG_UNIT_PARTY | TARGET_FLAG_UNIT_ALLY))
                unit = m_caster;

            m_targets.SetUnitTarget(unit);
        }
    }

    // check if spell needs dst target
    if (neededTargets & TARGET_FLAG_DEST_LOCATION)
    {
        // and target isn't set
        if (!m_targets.HasDst())
        {
            // try to use unit target if provided
            if (WorldObject* target = targets.GetObjectTarget())
                m_targets.SetDst(*target);
            // or use self if not available
            else
                m_targets.SetDst(*m_caster);
        }
    }
    else
        m_targets.RemoveDst();

    if (neededTargets & TARGET_FLAG_SOURCE_LOCATION)
    {
        if (!targets.HasSrc())
            m_targets.SetSrc(*m_caster);
    }
    else
        m_targets.RemoveSrc();

    // Hack fix for some specific spells
    switch (m_spellInfo->Id)
    {
        case 107145: // Wall of Light Activation
        {
            m_targets.SetSrc(*m_caster);

            WorldLocation firstPos;
            WorldLocation secondPos;

            firstPos.m_positionX = -1045.602f;
            firstPos.m_positionY = -2822.323f;
            firstPos.m_positionZ = 38.25466f;
            secondPos.m_positionX = -989.4236f;
            secondPos.m_positionY = -2821.757f;
            secondPos.m_positionZ = 38.25455f;

            m_targets.AddExtraTarget(0, firstPos);
            m_targets.AddExtraTarget(0, secondPos);
            break;
        }
        default:
            break;
    }
}

void Spell::SelectExplicitTargets()
{
    // here go all explicit target changes made to explicit targets after spell prepare phase is finished
    if (Unit* target = m_targets.GetUnitTarget())
    {
        // check for explicit target redirection, for Grounding Totem for example
        if (m_spellInfo->GetExplicitTargetMask() & TARGET_FLAG_UNIT_ENEMY
            || (m_spellInfo->GetExplicitTargetMask() & TARGET_FLAG_UNIT && !m_spellInfo->IsPositive()))
        {
            Unit* redirect;
            switch (m_spellInfo->DmgClass)
            {
                case SPELL_DAMAGE_CLASS_MAGIC:
                    redirect = m_caster->GetMagicHitRedirectTarget(target, m_spellInfo);
                    break;
                case SPELL_DAMAGE_CLASS_MELEE:
                case SPELL_DAMAGE_CLASS_RANGED:
                    redirect = m_caster->GetMeleeHitRedirectTarget(target, m_spellInfo);
                    break;
                default:
                    redirect = NULL;
                    break;
            }
            if (redirect && (redirect != target))
            {
                m_targets.SetUnitTarget(redirect);
                m_redirected = true;
            }
        }
    }
}

void Spell::SelectSpellTargets()
{
    // select targets for cast phase
    SelectExplicitTargets();

    uint32 processedAreaEffectsMask = 0;
    for (uint32 i = 0; i < m_spellInfo->EffectCount; ++i)
    {
        // not call for empty effect.
        // Also some spells use not used effect targets for store targets for dummy effect in triggered spells
        if (!m_spellInfo->Effects[i].IsEffect())
            continue;

        if (m_spellMissMask & ((1 << SPELL_MISS_MISS) | (1 << SPELL_MISS_IMMUNE)))
        {
            if (m_spellInfo->Effects[i].TargetA.GetTarget() == TARGET_UNIT_CASTER)
                continue;
        }

        // set expected type of implicit targets to be sent to client
        uint32 implicitTargetMask = GetTargetFlagMask(m_spellInfo->Effects[i].TargetA.GetObjectType()) | GetTargetFlagMask(m_spellInfo->Effects[i].TargetB.GetObjectType());
        if (implicitTargetMask & TARGET_FLAG_UNIT)
            m_targets.SetTargetFlag(TARGET_FLAG_UNIT);
        if (implicitTargetMask & (TARGET_FLAG_GAMEOBJECT | TARGET_FLAG_GAMEOBJECT_ITEM))
            m_targets.SetTargetFlag(TARGET_FLAG_GAMEOBJECT);

        SelectEffectImplicitTargets(SpellEffIndex(i), m_spellInfo->Effects[i].TargetA, processedAreaEffectsMask);
        SelectEffectImplicitTargets(SpellEffIndex(i), m_spellInfo->Effects[i].TargetB, processedAreaEffectsMask);

        // Select targets of effect based on effect type
        // those are used when no valid target could be added for spell effect based on spell target type
        // some spell effects use explicit target as a default target added to target map (like SPELL_EFFECT_LEARN_SPELL)
        // some spell effects add target to target map only when target type specified (like SPELL_EFFECT_WEAPON)
        // some spell effects don't add anything to target map (confirmed with sniffs) (like SPELL_EFFECT_DESTROY_ALL_TOTEMS)
        SelectEffectTypeImplicitTargets(i);

        if (m_targets.HasDst() && !IsCCSpell(m_spellInfo))
            AddDestTarget(*m_targets.GetDst(), i);

        /// needed for gaze of the valkyr visual
        if (m_spellInfo->Id == 218850)
        {
            const_cast<WorldLocation*>(m_targets.GetDstPos())->m_positionZ += 5.0f;
            AddDestTarget(*m_targets.GetDst(), i);
        }

        for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
            m_spellMissMask |= (1 << ihit->missCondition);

        if (m_spellInfo->IsChanneled())
        {
            uint32 mask = (1 << i);
            for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
            {
                if (ihit->effectMask & mask)
                {
                    m_channelTargetEffectMask |= mask;
                    break;
                }
            }
        }
        else if (m_auraScaleMask)
        {
            bool checkLvl = !m_UniqueTargetInfo.empty();
            for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end();)
            {
                // remove targets which did not pass min level check
                if (m_auraScaleMask && ihit->effectMask == m_auraScaleMask)
                {
                    // Do not check for selfcast
                    if (!ihit->scaleAura && ihit->targetGUID != m_caster->GetGUID())
                    {
                        ihit = m_UniqueTargetInfo.erase(ihit);
                        continue;
                    }
                }
                ++ihit;
            }
            if (checkLvl && m_UniqueTargetInfo.empty())
            {
                SendCastResult(SPELL_FAILED_LOWLEVEL);
                finish(false);
            }
        }
    }

    /// Make sure we don't have any pets in the target list if the spell is SPELL_ATTR3_ONLY_TARGET_PLAYERS flagged
    if (AttributesCustomEx3 & SPELL_ATTR3_ONLY_TARGET_PLAYERS)
    {
        for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end();)
        {
            if (IS_PET_GUID(ihit->targetGUID))
            {
                ihit = m_UniqueTargetInfo.erase(ihit);
                continue;
            }

            ++ihit;
        }
    }

    if (m_targets.HasDst())
    {
        if (m_targets.HasTraj())
        {
            float speed = m_targets.GetSpeedXY();
            if (speed > 0.0f)
                m_delayMoment = (uint64)floor(m_targets.GetDist2d() / speed * 1000.0f);
        }
        else if (m_spellInfo->Speed > 0.0f)
        {
            float dist = m_caster->GetExactDist(&*m_targets.GetDstPos());

            /// Heroic leap missile should always last 1 second, no matter the range.
            if (m_spellInfo->Id == 94954)
                m_delayMoment = 1000.0f;
            else if (!(AttributesCustomEx9 & SPELL_ATTR9_SPECIAL_DELAY_CALCULATION))
                m_delayMoment = uint64(floor(dist / m_spellInfo->Speed * 1000.0f));
            else
                m_delayMoment = uint64(m_spellInfo->Speed * 1000.0f);

            m_delayMoment += m_spellInfo->LaunchDelay * IN_MILLISECONDS;
        }
    }
}

void Spell::SelectEffectImplicitTargets(SpellEffIndex effIndex, SpellImplicitTargetInfo const& targetType, uint32& processedEffectMask)
{
    if (!targetType.GetTarget())
        return;

    uint32 effectMask = 1 << effIndex;
    // set the same target list for all effects
    // some spells appear to need this, however this requires more research
    switch (targetType.GetSelectionCategory())
    {
        case TARGET_SELECT_CATEGORY_NEARBY:
        case TARGET_SELECT_CATEGORY_CONE:
        case TARGET_SELECT_CATEGORY_AREA:
        case TARGET_SELECT_CATEGORY_CYLINDER:
            // targets for effect already selected
            if (effectMask & processedEffectMask)
                return;
            // choose which targets we can select at once
            for (uint32 j = effIndex + 1; j < m_spellInfo->EffectCount; ++j)
            {
                if (m_spellInfo->Effects[effIndex].TargetA.GetTarget() == m_spellInfo->Effects[j].TargetA.GetTarget() &&
                    m_spellInfo->Effects[effIndex].TargetB.GetTarget() == m_spellInfo->Effects[j].TargetB.GetTarget() &&
                    m_spellInfo->Effects[effIndex].ImplicitTargetConditions == m_spellInfo->Effects[j].ImplicitTargetConditions &&
                    m_spellInfo->Effects[effIndex].CalcRadius(m_caster) == m_spellInfo->Effects[j].CalcRadius(m_caster) &&
                    m_spellInfo->Id != 119072)
                    effectMask |= 1 << j;
            }
            processedEffectMask |= effectMask;
            break;
        default:
            break;
    }

    switch (targetType.GetSelectionCategory())
    {
        case TARGET_SELECT_CATEGORY_CHANNEL:
            SelectImplicitChannelTargets(effIndex, targetType);
            break;
        case TARGET_SELECT_CATEGORY_NEARBY:
            SelectImplicitNearbyTargets(effIndex, targetType, effectMask);
            break;
        case TARGET_SELECT_CATEGORY_CONE:
            SelectImplicitConeTargets(effIndex, targetType, effectMask);
            break;
        case TARGET_SELECT_CATEGORY_AREA:
            SelectImplicitAreaTargets(effIndex, targetType, effectMask);
            break;
        case TARGET_SELECT_CATEGORY_CYLINDER:
            SelectImplicitCylinderTargets(effIndex, targetType, effectMask);
            break;
        case TARGET_SELECT_CATEGORY_DEFAULT:
            switch (targetType.GetObjectType())
            {
                case TARGET_OBJECT_TYPE_SRC:
                    switch (targetType.GetReferenceType())
                    {
                        case TARGET_REFERENCE_TYPE_CASTER:
                            m_targets.SetSrc(*m_caster);
                            break;
                        default:
                            ASSERT(false && "Spell::SelectEffectImplicitTargets: received not implemented select target reference type for TARGET_TYPE_OBJECT_SRC");
                            break;
                    }
                    break;
                case TARGET_OBJECT_TYPE_DEST:
                     switch (targetType.GetReferenceType())
                     {
                         case TARGET_REFERENCE_TYPE_CASTER:
                             SelectImplicitCasterDestTargets(effIndex, targetType);
                             break;
                         case TARGET_REFERENCE_TYPE_TARGET:
                             SelectImplicitTargetDestTargets(effIndex, targetType);
                             break;
                         case TARGET_REFERENCE_TYPE_DEST:
                             SelectImplicitDestDestTargets(effIndex, targetType);
                             break;
                         default:
                             ASSERT(false && "Spell::SelectEffectImplicitTargets: received not implemented select target reference type for TARGET_TYPE_OBJECT_DEST");
                             break;
                     }
                     break;
                default:
                    switch (targetType.GetReferenceType())
                    {
                        case TARGET_REFERENCE_TYPE_CASTER:
                            SelectImplicitCasterObjectTargets(effIndex, targetType);
                            break;
                        case TARGET_REFERENCE_TYPE_TARGET:
                            SelectImplicitTargetObjectTargets(effIndex, targetType);
                            break;
                        default:
                            ASSERT(false && "Spell::SelectEffectImplicitTargets: received not implemented select target reference type for TARGET_TYPE_OBJECT");
                            break;
                    }
                    break;
            }
            break;
        case TARGET_SELECT_CATEGORY_NYI:
            break;
        default:
            printf("Spell::SelectEffectImplicitTargets: received not implemented select target category / Spell ID = %u and Effect = %d and target type = %d \n", m_spellInfo->Id, effIndex, targetType.GetTarget());
            //ASSERT(false && "Spell::SelectEffectImplicitTargets: received not implemented select target category");
            break;
    }
}

void Spell::SelectImplicitChannelTargets(SpellEffIndex effIndex, SpellImplicitTargetInfo const& targetType)
{
    if (targetType.GetReferenceType() != TARGET_REFERENCE_TYPE_CASTER)
    {
        ASSERT(false && "Spell::SelectImplicitChannelTargets: received not implemented target reference type");
        return;
    }

    Spell* channeledSpell = m_originalCaster->GetCurrentSpell(CURRENT_CHANNELED_SPELL);
    if (!channeledSpell)
        return;

    switch (targetType.GetTarget())
    {
        case TARGET_UNIT_CHANNEL_TARGET:
        {
            for (Guid128 l_ChannelTarget : m_originalCaster->GetChannelObjects())
            {
                WorldObject* target = ObjectAccessor::GetUnit(*m_caster, Guid128To64(l_ChannelTarget));
                CallScriptObjectTargetSelectHandlers(target, effIndex);
                // unit target may be no longer avalible - teleported out of map for example
                if (target && target->ToUnit())
                    AddUnitTarget(target->ToUnit(), 1 << effIndex);
            }
            break;
        }
        case TARGET_DEST_CHANNEL_TARGET:
            if (channeledSpell->m_targets.HasDst())
                m_targets.SetDst(channeledSpell->m_targets);
            else
            {
                DynamicFieldStructuredView<Guid128> const& channelObjects = m_originalCaster->GetChannelObjects();

                if (channelObjects.size() > 0)
                {
                    Guid128 guid = *channelObjects.begin();
                    WorldObject* target = channelObjects.size() > 0 ? ObjectAccessor::GetWorldObject(*m_caster, Guid128To64(guid)) : nullptr;
                    CallScriptObjectTargetSelectHandlers(target, effIndex);

                    if (target)
                        m_targets.SetDst(*target);
                }
            }
            break;
        case TARGET_DEST_CHANNEL_CASTER:
            m_targets.SetDst(*channeledSpell->GetCaster());
            break;
        default:
            ASSERT(false && "Spell::SelectImplicitChannelTargets: received not implemented target type");
            break;
    }
}

void Spell::SelectImplicitNearbyTargets(SpellEffIndex p_EffIndex, SpellImplicitTargetInfo const& p_TargetType, uint32 p_EffMask)
{
    if (p_TargetType.GetReferenceType() != TARGET_REFERENCE_TYPE_CASTER)
    {
        ASSERT(false && "Spell::SelectImplicitNearbyTargets: received not implemented target reference type");
        return;
    }

    float l_Range = 0.0f;
    switch (p_TargetType.GetCheckType())
    {
        case TARGET_CHECK_ENEMY:
            l_Range = m_spellInfo->GetMaxRange(false, m_caster, this);
            break;
        case TARGET_CHECK_ALLY:
        case TARGET_CHECK_PARTY:
        case TARGET_CHECK_RAID:
        case TARGET_CHECK_RAID_CLASS:
        case TARGET_CHECK_ALLY_OR_RAID:
        case TARGET_CHECK_SUMMON:
            l_Range = m_spellInfo->GetMaxRange(true, m_caster, this);
            break;
        case TARGET_CHECK_ENTRY:
        case TARGET_CHECK_DEFAULT:
            l_Range = m_spellInfo->GetMaxRange(m_spellInfo->IsPositive(), m_caster, this);
            break;
        default:
            ASSERT(false && "Spell::SelectImplicitNearbyTargets: received not implemented selection check type");
            break;
    }

    ConditionContainer* l_CondList = m_spellInfo->Effects[p_EffIndex].ImplicitTargetConditions;

    // handle emergency case - try to use other provided targets if no conditions provided
    if (p_TargetType.GetCheckType() == TARGET_CHECK_ENTRY && (!l_CondList || l_CondList->empty()))
    {
        switch (p_TargetType.GetObjectType())
        {
            case TARGET_OBJECT_TYPE_GOBJ:
                if (m_spellInfo->RequiresSpellFocus)
                {
                    if (focusObject)
                        AddGOTarget(focusObject, p_EffMask);
                    return;
                }
                break;
            case TARGET_OBJECT_TYPE_DEST:
                if (m_spellInfo->RequiresSpellFocus)
                {
                    if (focusObject)
                        m_targets.SetDst(*focusObject);
                    return;
                }
                break;
            default:
                break;
        }
    }

    WorldObject* l_Target = SearchNearbyTarget(l_Range, p_TargetType.GetObjectType(), p_TargetType.GetCheckType(), l_CondList);
    if (!l_Target)
        return;

    CallScriptObjectTargetSelectHandlers(l_Target, p_EffIndex);

    switch (p_TargetType.GetObjectType())
    {
        case TARGET_OBJECT_TYPE_AREATRIGGER:
            if (AreaTrigger* l_AreaTrigger = l_Target->ToAreaTrigger())
                AddAreaTriggerTarget(l_AreaTrigger, p_EffMask);
            break;
        case TARGET_OBJECT_TYPE_UNIT:
            if (Unit* l_UnitTarget = l_Target->ToUnit())
                AddUnitTarget(l_UnitTarget, p_EffMask, true, false);
            break;
        case TARGET_OBJECT_TYPE_GOBJ:
            if (GameObject* l_GobjTarget = l_Target->ToGameObject())
                AddGOTarget(l_GobjTarget, p_EffMask);
            break;
        case TARGET_OBJECT_TYPE_DEST:
            m_targets.SetDst(*l_Target);
            break;
        default:
            ASSERT(false && "Spell::SelectImplicitNearbyTargets: received not implemented target object type");
            break;
    }

    SelectImplicitChainTargets(p_EffIndex, p_TargetType, l_Target, p_EffMask);
}

void Spell::SelectImplicitConeTargets(SpellEffIndex p_EffIndex, SpellImplicitTargetInfo const& p_TargetType, uint32 p_EffMask)
{
    if (p_TargetType.GetReferenceType() != TARGET_REFERENCE_TYPE_CASTER)
    {
        ASSERT(false && "Spell::SelectImplicitConeTargets: received not implemented target reference type");
        return;
    }
    std::list<WorldObject*> l_Targets;
    SpellTargetObjectTypes l_ObjectType = p_TargetType.GetObjectType();
    SpellTargetCheckTypes l_SelectionType = p_TargetType.GetCheckType();
    ConditionContainer* l_ConditionsList = m_spellInfo->Effects[p_EffIndex].ImplicitTargetConditions;
    Unit* caster = m_originalCaster ? m_originalCaster : m_caster;
    float l_ConeAngle = ((m_caster->IsPlayer()) ? M_PI * (2.0f / 3.0f) : M_PI / 2.0f);

    switch (m_spellInfo->Effects[p_EffIndex].TargetA.GetTarget())
    {
        case TARGET_UNIT_CONE_ENEMY_24:
            l_ConeAngle = M_PI / 7.5f;
            break;
        case TARGET_UNIT_CONE_ENEMY_54:
            l_ConeAngle = M_PI / 3.33f;
            break;
        case TARGET_UNIT_CONE_ENEMY_104:
            l_ConeAngle = M_PI / 1.73f;
            break;
        case TARGET_UNIT_CONE_ENEMY_110:
            l_ConeAngle = M_PI / 1.64f;
            break;
        case TARGET_UNIT_ENEMY_BETWEEN_DEST:
            l_ConeAngle = M_PI / 1.40f;
            break;
        case TARGET_UNIT_ENEMY_BETWEEN_DEST_2:
            l_ConeAngle = M_PI / 1.385f;
            break;
        default:
            break;
    }

    switch (m_spellInfo->Id)
    {
        case 118094:
            l_ConeAngle = M_PI/2;
            break;
        case 118105:
            l_ConeAngle = M_PI/4;
            break;
        case 198030: ///< Eye Beam
        case 118106:
            l_ConeAngle = M_PI/6;
            break;
        case 136740:// Double swipe (back)
            l_ConeAngle += M_PI;
            break;
        case 194956: ///< Reap souls
            l_ConeAngle = M_PI;
            break;
        case 237671: ///< Fel-Fire Ejection
            l_ConeAngle = M_PI / 6.400f;
            break;
        case 249248: ///< Wave of Dread
            l_ConeAngle = M_PI / 4.0f;
            break;
        default:
            break;
    }

    /// Handle ConeAngle calculation in a generic way, overriding hardcoded values if needed
    if (SpellTargetRestrictionsEntry const* l_Restrictions = m_spellInfo->GetSpellTargetRestrictions())
    {
        if (l_Restrictions->ConeAngle != 0.0f)
            l_ConeAngle = 2.0f * M_PI / 360.0f * l_Restrictions->ConeAngle;
    }

    float l_Radius = m_spellInfo->Effects[p_EffIndex].CalcRadius(caster) * m_spellValue->RadiusMod;

    if (caster->HasAura(209793))    /// T.N.T
        l_Radius *= 1.3f;

    if (m_spellInfo->Id == 101872)    /// DMF Shot
    {
        l_Radius = 100.0f;
        l_ConeAngle = M_PI / 6;
    }

    if (m_spellInfo->Id == 102292)  /// Tonk Target
    {
        l_Radius = 7.0f;
        l_ConeAngle = M_PI / 6;
    }
           
    if (uint32 l_ContainerTypeMask = GetSearcherTypeMask(l_ObjectType, l_ConditionsList))
    {
        JadeCore::WorldObjectSpellConeTargetCheck l_Check(l_ConeAngle, l_Radius, caster, m_spellInfo, l_SelectionType, l_ConditionsList);
        JadeCore::WorldObjectListSearcher<JadeCore::WorldObjectSpellConeTargetCheck> l_Searcher(caster, l_Targets, l_Check, l_ContainerTypeMask);
        SearchTargets<JadeCore::WorldObjectListSearcher<JadeCore::WorldObjectSpellConeTargetCheck> >(l_Searcher, l_ContainerTypeMask, caster, m_caster, l_Radius);

        if (m_caster->IsPlayer())
        {
            for (std::list<WorldObject*>::iterator l_Target = l_Targets.begin(); l_Target != l_Targets.end();)
            {
                if (!(AttributesCustomEx2 & SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS) && !m_caster->IsWithinLOSInMap(*l_Target))
                    l_Target = l_Targets.erase(l_Target);
                else
                    ++l_Target;
            }
        }

        CallScriptObjectAreaTargetSelectHandlers(l_Targets, p_EffIndex, p_TargetType.GetTarget());

        if (!l_Targets.empty())
        {
            // Other special target selection goes here
            if (uint32 l_MaxTargets = m_spellValue->MaxAffectedTargets)
                JadeCore::Containers::RandomResizeList(l_Targets, l_MaxTargets);

            // for compability with older code - add only unit and go targets
            // TODO: remove this
            std::list<Unit*>        l_UnitTargets;
            std::list<GameObject*>  l_GObjTargets;
            std::list<AreaTrigger*> l_AreaTriggerTargets;

            for (std::list<WorldObject*>::iterator l_Iterator = l_Targets.begin(); l_Iterator != l_Targets.end(); ++l_Iterator)
            {
                if (Unit* l_UnitTarget = (*l_Iterator)->ToUnit())
                    l_UnitTargets.push_back(l_UnitTarget);
                else if (GameObject* l_GObjTarget = (*l_Iterator)->ToGameObject())
                    l_GObjTargets.push_back(l_GObjTarget);
                else if (AreaTrigger* l_AreaTrigger = (*l_Iterator)->ToAreaTrigger())
                    l_AreaTriggerTargets.push_back(l_AreaTrigger);
            }

            // Light of Dawn
            if (m_spellInfo->Id == 85222)
            {
                uint8 l_MaxSize = m_caster->HasAura(54940) ? 4 : 6; // Glyph of Light of Dawn
                l_UnitTargets.push_back(m_caster);
                if (l_UnitTargets.size() > l_MaxSize)
                {
                    l_UnitTargets.sort(JadeCore::HealthPctOrderPred());
                    l_UnitTargets.resize(l_MaxSize);
                }
            }

            for (std::list<Unit*>::iterator l_Iterator = l_UnitTargets.begin(); l_Iterator != l_UnitTargets.end(); ++l_Iterator)
                AddUnitTarget(*l_Iterator, p_EffMask, false);

            for (std::list<GameObject*>::iterator l_Iterator = l_GObjTargets.begin(); l_Iterator != l_GObjTargets.end(); ++l_Iterator)
                AddGOTarget(*l_Iterator, p_EffMask);

            for (std::list<AreaTrigger*>::iterator l_Iterator = l_AreaTriggerTargets.begin(); l_Iterator != l_AreaTriggerTargets.end(); ++l_Iterator)
                AddAreaTriggerTarget(*l_Iterator, p_EffMask);
        }
    }
}

void Spell::SelectImplicitAreaTargets(SpellEffIndex p_EffIndex, SpellImplicitTargetInfo const& p_TargetType, uint32 p_EffMask)
{
    Unit* l_Referer = nullptr;
    switch (p_TargetType.GetReferenceType())
    {
        case TARGET_REFERENCE_TYPE_SRC:
        case TARGET_REFERENCE_TYPE_DEST:
        case TARGET_REFERENCE_TYPE_CASTER:
            l_Referer = m_caster;
            break;
        case TARGET_REFERENCE_TYPE_TARGET:
            l_Referer = m_targets.GetUnitTarget();
            break;
        case TARGET_REFERENCE_TYPE_LAST:
        {
            // find last added target for this effect
            for (std::list<TargetInfo>::reverse_iterator l_Iterator = m_UniqueTargetInfo.rbegin(); l_Iterator != m_UniqueTargetInfo.rend(); ++l_Iterator)
            {
                if (l_Iterator->effectMask & (1<<p_EffIndex))
                {
                    l_Referer = ObjectAccessor::GetUnit(*m_caster, l_Iterator->targetGUID);
                    break;
                }
            }
            break;
        }
        default:
            ASSERT(false && "Spell::SelectImplicitAreaTargets: received not implemented target reference type");
            return;
    }

    if (!l_Referer)
        return;

    Position const* l_Center = nullptr;
    switch (p_TargetType.GetReferenceType())
    {
        case TARGET_REFERENCE_TYPE_SRC:
            l_Center = m_targets.GetSrcPos();
            break;
        case TARGET_REFERENCE_TYPE_DEST:
            l_Center = m_targets.GetDstPos();
            break;
        case TARGET_REFERENCE_TYPE_CASTER:
        case TARGET_REFERENCE_TYPE_TARGET:
        case TARGET_REFERENCE_TYPE_LAST:
            l_Center = l_Referer;
            break;
         default:
             ASSERT(false && "Spell::SelectImplicitAreaTargets: received not implemented target reference type");
             return;
    }

    std::list<WorldObject*> l_Targets;
    float l_Radius = m_spellInfo->Effects[p_EffIndex].CalcRadius(m_caster, nullptr, true) * m_spellValue->RadiusMod;

    bool l_IgnoreCasterSize = p_TargetType.GetReferenceType() == SpellTargetReferenceTypes::TARGET_REFERENCE_TYPE_DEST && m_spellInfo->Effects[p_EffIndex].TargetA.GetReferenceType() == SpellTargetReferenceTypes::TARGET_REFERENCE_TYPE_DEST;

    bool l_IgnoreTargetSize = m_spellInfo->Id != 185901; ///< Marked Shot;

    /// Prayer of healing, must heals 40y targets from the selected target
    if (m_spellInfo->Id == 596 && m_caster->IsPlayer())
    {
        if (Unit* l_SelectedTarget = m_caster->ToPlayer()->GetSelectedUnit() ? m_caster->ToPlayer()->GetSelectedUnit() : m_caster)
            SearchAreaTargets(l_Targets, l_Radius, l_SelectedTarget, l_Referer, p_TargetType.GetObjectType(), p_TargetType.GetCheckType(), m_spellInfo->Effects[p_EffIndex].ImplicitTargetConditions, l_IgnoreCasterSize, l_IgnoreTargetSize);
    }
    else
        SearchAreaTargets(l_Targets, l_Radius, l_Center, l_Referer, p_TargetType.GetObjectType(), p_TargetType.GetCheckType(), m_spellInfo->Effects[p_EffIndex].ImplicitTargetConditions, l_IgnoreCasterSize, l_IgnoreTargetSize);

    /// Hackfix to remove neutral npc targeted by the infernal
    if (l_Referer->isGuardian() && m_spellInfo->Id == 20153)
    {
        Unit* l_Owner = l_Referer->GetOwner();
        std::list<WorldObject*> l_AllowedTargets;

        if (l_Owner)
        {
            for (auto l_Target : l_Targets)
            {
                Unit* l_UnitTarget = l_Target->ToUnit();

                if (!l_UnitTarget)
                    continue;

                if (l_Owner->IsValidAttackTarget(l_UnitTarget))
                    l_AllowedTargets.push_back(l_UnitTarget);
            }

            l_Targets = l_AllowedTargets;
        }
    }

    // Custom entries
    // TODO: remove those
    switch (m_spellInfo->Id)
    {
        case 117002:// Tap to the Spirit World
        {
            // remove existing targets
            CleanupTargetList();

            if (!l_Targets.empty())
                for (std::list<WorldObject*>::iterator itr = l_Targets.begin(); itr != l_Targets.end(); ++itr)
                    if ((*itr) && (*itr)->ToUnit())
                        if ((*itr)->GetEntry() == 60512)
                            AddUnitTarget((*itr)->ToUnit(), 1 << p_EffIndex, false, true, l_Center);

            return;
        }
        case 117230: // Empowered
        {
            // remove existing targets
            CleanupTargetList();

            if (!l_Targets.empty())
                for (std::list<WorldObject*>::iterator itr = l_Targets.begin(); itr != l_Targets.end(); ++itr)
                    if ((*itr) && (*itr)->ToUnit())
                        if ((*itr)->GetEntry() == 60583 || (*itr)->GetEntry() == 60585 || (*itr)->GetEntry() == 60586)
                            AddUnitTarget((*itr)->ToUnit(), 1 << p_EffIndex, false, true, l_Center);

            return;
        }
        case 120764: // Ghost Essence
        {
            // remove existing targets
            CleanupTargetList();

            if (!l_Targets.empty())
                for (std::list<WorldObject*>::iterator itr = l_Targets.begin(); itr != l_Targets.end(); ++itr)
                    if ((*itr) && (*itr)->ToUnit())
                        if ((*itr)->GetEntry() == 61334 || (*itr)->GetEntry() == 61989)
                            AddUnitTarget((*itr)->ToUnit(), 1 << p_EffIndex, false, true, l_Center);

            return;
        }
        default:
            break;
    }

    bool l_IsTotemAndNeedsLOS = false; ///< Some totems should take LoS into account, but not every totems Q.Q
    uint32 l_Entry = m_caster->GetEntry();
    switch (l_Entry)
    {
        case 2630:  ///< Earthbind totem
        case 60561: ///< Earthgrab totem (done again in  spell_sha_earthgrab)
        case 61245: ///< Lightning Surge totem
            l_IsTotemAndNeedsLOS = true;
            break;
        default:
            break;
    }

    if ((m_caster->IsPlayer() || l_IsTotemAndNeedsLOS) && m_spellInfo->Id != 596)
    {
        for (std::list<WorldObject*>::iterator l_Target = l_Targets.begin(); l_Target != l_Targets.end();)
        {
            if (!(AttributesCustomEx2 & SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS) && !(*l_Target)->IsWithinLOS(l_Center->m_positionX, l_Center->m_positionY, l_Center->m_positionZ))
                l_Target = l_Targets.erase(l_Target);
            else
                ++l_Target;
        }
    }

    CallScriptObjectAreaTargetSelectHandlers(l_Targets, p_EffIndex, p_TargetType.GetTarget());

    if (Creature* l_Creature = m_caster->ToCreature())
    {
        if (l_Creature->IsAIEnabled)
            l_Creature->AI()->FilterTargets(l_Targets, this, p_EffIndex);
    }

    std::list<Unit*> l_UnitTargets;
    std::list<GameObject*> l_GObjTargets;
    std::list<AreaTrigger*> l_AreaTriggerTargets;
    // for compability with older code - add only unit and go targets
    // TODO: remove this
    if (!l_Targets.empty())
    {
        for (std::list<WorldObject*>::iterator l_Iterator = l_Targets.begin(); l_Iterator != l_Targets.end(); ++l_Iterator)
        {
            if ((*l_Iterator))
            {
                if (Unit* l_UnitTarget = (*l_Iterator)->ToUnit())
                    l_UnitTargets.push_back(l_UnitTarget);
                else if (GameObject* l_GObjTarget = (*l_Iterator)->ToGameObject())
                    l_GObjTargets.push_back(l_GObjTarget);
                else if (AreaTrigger* l_AreaTrigger = (*l_Iterator)->ToAreaTrigger())
                    l_AreaTriggerTargets.push_back(l_AreaTrigger);
            }
        }
    }

    if (!l_UnitTargets.empty())
    {
        // Special target selection for smart heals and energizes
        uint32 l_MaxSize = 0;
        int32 l_Power = -1;
        switch (m_spellInfo->SpellFamilyName)
        {
            case SPELLFAMILY_GENERIC:
                switch (m_spellInfo->Id)
                {
                    case 71610: // Echoes of Light (Althor's Abacus normal version)
                    case 71641: // Echoes of Light (Althor's Abacus heroic version)
                    case 99152: // Cauterizing Flame, Item - Priest Healer T12 4P Bonus
                    case 96966: // Blaze of Life, Eye of Blazing Power (Normal)
                    case 97136: // Blaze of Life, Eye of Blazing Power (Heroic)
                    case 109825: // Nick of Time, Windward Heart (heroic)
                    case 108000: // Nick of Time, Windward Heart (normal)
                    case 109822: // Nick of Time, Windward Heart (lfr)
                    case 230017: // Holy Bolt, Faith's Crucible
                        l_MaxSize = 1;
                        l_Power = POWER_HEALTH;
                        break;
                    case 57669: // Replenishment
                        // In arenas Replenishment may only affect the caster
                        if (m_caster->IsPlayer() && m_caster->ToPlayer()->InArena())
                        {
                            l_UnitTargets.clear();
                            l_UnitTargets.push_back(m_caster);
                            break;
                        }
                        l_MaxSize = 10;
                        l_Power = POWER_MANA;
                        break;
                    default:
                        break;
                }
                break;
            case SPELLFAMILY_PRIEST:
            {
                bool partyOrRaidOnly = false;
                switch (m_spellInfo->Id)
                {
                    case 64844: // Divine Hymn
                        partyOrRaidOnly = true;
                        break;
                    case 204883: // Circle of Healing
                        l_MaxSize = 5;
                        l_Power = POWER_HEALTH;
                        break;
                    case 34861: ///< Holy Word: Sanctify
                        l_MaxSize = 6;
                        l_Power = POWER_HEALTH;
                        break;
                    case 596: ///< Prayer Of Healing
                        l_Power = POWER_HEALTH;
                        break;
                    default:
                        break;
                }

                // Remove targets outside caster's raid
                if (partyOrRaidOnly)
                {
                    l_UnitTargets.remove_if([this](Unit* p_Unit) -> bool
                    {
                        if (!p_Unit->IsInRaidWith(m_caster))
                            return true;

                        return false;
                    });
                }

                break;
            }
            case SPELLFAMILY_PALADIN:
            {
                switch (m_spellInfo->Id)
                {
                    case 119072:// Holy Wrath
                    {
                        static const uint8 s_TypesNoglyph[] = {CREATURE_TYPE_DEMON, CREATURE_TYPE_UNDEAD, 0};
                        static const uint8 sTypeGlyph[] = {CREATURE_TYPE_DEMON, CREATURE_TYPE_UNDEAD, CREATURE_TYPE_ELEMENTAL, CREATURE_TYPE_DRAGONKIN, 0};
                        const uint8 *l_Types = m_caster->HasAura(54923) ? sTypeGlyph: s_TypesNoglyph;

                        // Normal case
                        if (p_EffIndex == 0 && !m_caster->HasAura(115738))
                        {
                            for (std::list<Unit*>::iterator l_Iterator = l_UnitTargets.begin() ; l_Iterator != l_UnitTargets.end();)
                            {
                                bool l_Found = false;
                                uint8 l_TypesI = 0;
                                do
                                {
                                    if ((*l_Iterator)->GetCreatureType() == l_Types[l_TypesI])
                                    {
                                        l_Found = true;
                                        break;
                                    }
                                }
                                while (l_Types[++l_TypesI]);

                                if (l_Found)
                                    l_Iterator++;
                                else
                                    l_Iterator = l_UnitTargets.erase(l_Iterator);
                            }
                        }
                        // Glyph of Focused Wrath
                        else if (m_caster->HasAura(115738))
                        {
                            if (m_caster->GetTypeId() != TYPEID_PLAYER)
                                break;

                            Player* l_Player = m_caster->ToPlayer();
                            if (l_Player == nullptr)
                                break;

                            if (Unit* l_Target = l_Player->GetSelectedUnit())
                            {
                                if (p_EffIndex == 0)
                                {
                                    bool l_Found = false;
                                    uint8 l_TypesI = 0;
                                    do
                                    {
                                        if (l_Target->GetCreatureType() == l_Types[l_TypesI])
                                        {
                                            l_Found = true;
                                            break;
                                        }
                                    }
                                    while (l_Types[++l_TypesI]);

                                    l_UnitTargets.clear();
                                    if (l_Found)
                                        l_UnitTargets.push_back(l_Target);
                                }
                                else
                                {
                                    l_UnitTargets.clear();
                                    l_UnitTargets.push_back(l_Target);
                                }
                            }
                            else
                            {
                                l_UnitTargets.sort(JadeCore::WorldObjectDistanceCompareOrderPred(m_caster));
                                Unit* l_Victim = (*l_UnitTargets.begin())->ToUnit();

                                if (l_Victim)
                                {
                                    if (p_EffIndex == 0)
                                    {
                                        bool l_Found = false;
                                        uint8 l_TypesI = 0;
                                        do
                                        {
                                            if (l_Victim->GetCreatureType() == l_Types[l_TypesI])
                                            {
                                                l_Found = true;
                                                break;
                                            }
                                        }
                                        while (l_Types[++l_TypesI]);

                                        l_UnitTargets.clear();
                                        if (l_Found)
                                            l_UnitTargets.push_back(l_Victim);
                                    }
                                    else
                                    {
                                        l_UnitTargets.clear();
                                        l_UnitTargets.push_back(l_Victim);
                                    }
                                }
                            }
                        }

                        break;
                    }
                    default:
                        break;
                }

                break;
            }
            case SPELLFAMILY_DRUID:
            {
                bool l_RemoveEnemies = true;
                switch(m_spellInfo->Id)
                {
                    /// Firebloom, Item  Druid T12 Restoration 4P Bonus
                    case 99017:
                        l_MaxSize = 1;
                        l_Power = POWER_HEALTH;
                        break;
                    /// Efflorescence
                    case 81269:
                        l_MaxSize = 3;
                        l_Power = POWER_HEALTH;
                        break;
                    /// Tranquility
                    case 157982:
                        l_Power = POWER_HEALTH;
                        break;

                    default:
                        l_RemoveEnemies = false;
                        break;
                }

                if (l_RemoveEnemies)
                {
                    /// Remove targets outside caster's raid
                    for (std::list<Unit*>::iterator l_Iterator = l_UnitTargets.begin(); l_Iterator != l_UnitTargets.end();)
                    {
                        if (!(*l_Iterator)->IsInRaidWith(m_caster))
                            l_Iterator = l_UnitTargets.erase(l_Iterator);
                        else
                            ++l_Iterator;
                    }
                }

                break;
            }
            case SPELLFAMILY_MONK:
            {
                switch (m_spellInfo->Id)
                {
                    case 162530: ///< Refreshing Jade Wind
                        l_MaxSize = 6;
                        l_Power = POWER_HEALTH;
                        break;
                }
                break;
            }
            default:
                break;
        }

        if (l_MaxSize && l_Power != -1)
        {
            if (Powers(l_Power) == POWER_HEALTH)
            {
                if (l_UnitTargets.size() > l_MaxSize)
                {
                    if (m_caster->GetSpellModOwner() && m_caster->GetSpellModOwner()->IsPlayer())
                        l_UnitTargets.sort(JadeCore::HealthPctOrderPredPlayer());
                    else
                        l_UnitTargets.sort(JadeCore::HealthPctOrderPred());
                    l_UnitTargets.resize(l_MaxSize);
                }
            }
            else if (m_spellInfo->Id == 34861) ///< Prayer of Healing and Holy Word Sanctify should priorize distance
            {
                 if (l_UnitTargets.size() > l_MaxSize)
                 {
                    l_UnitTargets.sort(JadeCore::DistanceOrderPred(l_Center));

                    l_UnitTargets.resize(l_MaxSize);
                 }
            }
            else
            {
                for (std::list<Unit*>::iterator l_Iterator = l_UnitTargets.begin(); l_Iterator != l_UnitTargets.end();)
                    if ((*l_Iterator)->getPowerType() != (Powers)l_Power)
                        l_Iterator = l_UnitTargets.erase(l_Iterator);
                    else
                        ++l_Iterator;

                if (l_UnitTargets.size() > l_MaxSize)
                {
                    l_UnitTargets.sort(JadeCore::PowerPctOrderPred((Powers)l_Power));
                    l_UnitTargets.resize(l_MaxSize);
                }
            }
        }

        // todo: move to scripts, but we must call it before resize list by MaxAffectedTargets
        // Intimidating Shout
        if (m_spellInfo->Id == 5246 && p_EffIndex != EFFECT_0)
            l_UnitTargets.remove(m_targets.GetUnitTarget());

        // Custom MoP Script
        if (m_caster->IsPlayer())
        {
            switch (m_spellInfo->Id)
            {
                // Spinning Crane Kick / Rushing Jade Wind : Give 1 Chi if the spell hits at least 3 targets
                case 107270:
                case 148187:
                    if (m_caster->ToPlayer()->HasSpellCooldown(129881) || l_UnitTargets.size() < 3)
                        break;

                    m_caster->CastSpell(m_caster, 129881, true);
                    m_caster->ToPlayer()->AddSpellCooldown(129881, 0, 3 * IN_MILLISECONDS);
                    break;
                default:
                    break;
            }
        }

        // Other special target selection goes here
        if (uint32 l_MaxTargets = m_spellValue->MaxAffectedTargets)
            JadeCore::Containers::RandomResizeList(l_UnitTargets, l_MaxTargets);

        for (std::list<Unit*>::iterator l_Iterator = l_UnitTargets.begin(); l_Iterator != l_UnitTargets.end(); ++l_Iterator)
        {
            if (m_caster->ToCreature() && m_caster->ToCreature()->IsAIEnabled && (m_caster->ToCreature()->AI()->CanTargetOutOfLOS() || m_caster->ToCreature()->AI()->CanTargetOutOfLOSXYZ((*l_Iterator)->GetPositionX(), (*l_Iterator)->GetPositionY(), (*l_Iterator)->GetPositionZ())))
                AddUnitTarget(*l_Iterator, p_EffMask, false, true, nullptr);
            else
                AddUnitTarget(*l_Iterator, p_EffMask, false, true, l_Center);
        }
    }

    if (!l_GObjTargets.empty())
    {
        if (uint32 l_MaxTargets = m_spellValue->MaxAffectedTargets)
            JadeCore::Containers::RandomResizeList(l_GObjTargets, l_MaxTargets);

        for (std::list<GameObject*>::iterator l_Iterator = l_GObjTargets.begin(); l_Iterator != l_GObjTargets.end(); ++l_Iterator)
            AddGOTarget(*l_Iterator, p_EffMask);
    }

    if (!l_AreaTriggerTargets.empty())
    {
        if (uint32 l_MaxTargets = m_spellValue->MaxAffectedTargets)
            JadeCore::Containers::RandomResizeList(l_AreaTriggerTargets, l_MaxTargets);

        for (std::list<AreaTrigger*>::iterator l_Iterator = l_AreaTriggerTargets.begin(); l_Iterator != l_AreaTriggerTargets.end(); ++l_Iterator)
            AddAreaTriggerTarget(*l_Iterator, p_EffMask);
    }
}

void Spell::SelectImplicitCylinderTargets(SpellEffIndex p_EffIndex, SpellImplicitTargetInfo const& p_TargetType, uint32 p_EffMask)
{
    Position const* l_Src = m_caster;
    Position const* l_Dst = nullptr;

    switch (p_TargetType.GetReferenceType())
    {
        case SpellTargetReferenceTypes::TARGET_REFERENCE_TYPE_SRC:
        case SpellTargetReferenceTypes::TARGET_REFERENCE_TYPE_CASTER:
            l_Dst = m_caster;
            break;
        case SpellTargetReferenceTypes::TARGET_REFERENCE_TYPE_DEST:
        {
            l_Dst = m_targets.GetDstPos();
            break;
        }
        case SpellTargetReferenceTypes::TARGET_REFERENCE_TYPE_TARGET:
            l_Dst = m_targets.GetUnitTarget();
            if (!l_Dst)
                l_Dst = m_targets.GetDstPos();
            break;
        default:
            ASSERT(false && "Spell::SelectImplicitCylinderTargets: received not implemented target reference type");
            return;
    }

    if (l_Dst == nullptr)
        return;

    float l_Width   = 0.0f;
    float l_Radius  = m_spellInfo->Effects[p_EffIndex].CalcRadius(m_caster) * m_spellValue->RadiusMod;

    /// Handle ConeAngle calculation in a generic way, overriding hard coded values if needed
    if (SpellTargetRestrictionsEntry const* l_Restrictions = m_spellInfo->GetSpellTargetRestrictions())
    {
        if (l_Restrictions->Width != 0.0f)
            l_Width = l_Restrictions->Width;
    }

    std::list<WorldObject*> l_Targets;

    SpellTargetObjectTypes l_ObjectType     = p_TargetType.GetObjectType();
    SpellTargetCheckTypes l_SelectionType   = p_TargetType.GetCheckType();
    ConditionContainer* l_ConditionsList    = m_spellInfo->Effects[p_EffIndex].ImplicitTargetConditions;

    Unit* l_RefCaster = m_originalCaster ? m_originalCaster : m_caster;

    Position l_Origin = *m_caster;
    l_Origin.SetOrientation(m_caster->GetAngle(l_Dst));

    if (uint32 l_ContainerTypeMask = GetSearcherTypeMask(l_ObjectType, l_ConditionsList))
    {
        JadeCore::WorldObjectSpellWidthTargetCheck l_Check(l_Width, l_Radius, m_caster, l_RefCaster, m_spellInfo, l_SelectionType, l_ConditionsList, l_Origin);
        JadeCore::WorldObjectListSearcher<JadeCore::WorldObjectSpellWidthTargetCheck> l_Searcher(l_RefCaster, l_Targets, l_Check, l_ContainerTypeMask);
        SearchTargets<JadeCore::WorldObjectListSearcher<JadeCore::WorldObjectSpellWidthTargetCheck> >(l_Searcher, l_ContainerTypeMask, l_RefCaster, l_Src, l_Radius);

        if (m_caster->IsPlayer())
        {
            for (std::list<WorldObject*>::iterator l_Target = l_Targets.begin(); l_Target != l_Targets.end();)
            {
                if (!(AttributesCustomEx2 & SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS) && !(*l_Target)->IsWithinLOS(l_Src->m_positionX, l_Src->m_positionY, l_Src->m_positionZ))
                    l_Target = l_Targets.erase(l_Target);
                else
                    ++l_Target;
            }
        }

        CallScriptObjectAreaTargetSelectHandlers(l_Targets, p_EffIndex, p_TargetType.GetTarget());

        if (Creature* l_Creature = m_caster->ToCreature())
        {
            if (l_Creature->IsAIEnabled)
                l_Creature->AI()->FilterTargets(l_Targets, this, p_EffIndex);
        }

        if (!l_Targets.empty())
        {
            /// Other special target selection goes here
            if (uint32 l_MaxTargets = m_spellValue->MaxAffectedTargets)
                JadeCore::Containers::RandomResizeList(l_Targets, l_MaxTargets);
        }

        std::list<Unit*>        l_UnitTargets;
        std::list<GameObject*>  l_GObjTargets;
        std::list<AreaTrigger*> l_AreaTriggerTargets;

        for (WorldObject* l_Iter : l_Targets)
        {
            switch (l_Iter->GetTypeId())
            {
                case TypeID::TYPEID_UNIT:
                case TypeID::TYPEID_PLAYER:
                    l_UnitTargets.push_back(l_Iter->ToUnit());
                    break;
                case TypeID::TYPEID_GAMEOBJECT:
                    l_GObjTargets.push_back(l_Iter->ToGameObject());
                    break;
                case TypeID::TYPEID_AREATRIGGER:
                    l_AreaTriggerTargets.push_back(l_Iter->ToAreaTrigger());
                    break;
                default:
                    break;
            }
        }

        for (Unit* l_Iter : l_UnitTargets)
            AddUnitTarget(l_Iter, p_EffMask, false);

        for (GameObject* l_Iter : l_GObjTargets)
            AddGOTarget(l_Iter, p_EffMask);

        for (AreaTrigger* l_Iter : l_AreaTriggerTargets)
            AddAreaTriggerTarget(l_Iter, p_EffMask);
    }
}

void Spell::SelectImplicitCasterDestTargets(SpellEffIndex effIndex, SpellImplicitTargetInfo const& targetType)
{
    switch (targetType.GetTarget())
    {
        case TARGET_DEST_CASTER:
            m_targets.SetDst(*m_caster);
            return;
        case TARGET_DEST_HOME:
            if (Player* playerCaster = m_caster->ToPlayer())
                m_targets.SetDst(playerCaster->m_homebindX, playerCaster->m_homebindY, playerCaster->m_homebindZ, playerCaster->GetOrientation(), playerCaster->m_homebindMapId);
            return;
        case TARGET_DEST_DB:
            if (SpellTargetPosition const* st = sSpellMgr->GetSpellTargetPosition(m_spellInfo->Id, effIndex))
            {
                // @TODO: fix this check
                if (m_spellInfo->HasEffect(SPELL_EFFECT_TELEPORT_UNITS) || m_spellInfo->HasEffect(SPELL_EFFECT_TELEPORT_TO_LFG_DUNGEON))
                    m_targets.SetDst(st->target_X, st->target_Y, st->target_Z, st->target_Orientation, (int32)st->target_mapId);
                /*else if (m_spellInfo->HasEffect(SPELL_EFFECT_TELEPORT_TO_LFG_DUNGEON))
                {
                    if (LFGDungeonEntry const* l_DungeonEntry = sLFGDungeonStore.LookupEntry(m_spellInfo->Effects[effIndex].MiscValueB))
                        m_targets.SetDst(st->target_X, st->target_Y, st->target_Z, st->target_Orientation, l_DungeonEntry->map);
                }*/
                else if (st->target_mapId == m_caster->GetMapId())
                    m_targets.SetDst(st->target_X, st->target_Y, st->target_Z, st->target_Orientation);
            }
            else
            {
                WorldObject* target = m_targets.GetObjectTarget();
                m_targets.SetDst(target ? *target : *m_caster);
            }
            return;
        case TARGET_DEST_CASTER_FISHING:
        {
             float min_dis = m_spellInfo->GetMinRange(true);
             float max_dis = m_spellInfo->GetMaxRange(true);
             float dis = (float)rand_norm() * (max_dis - min_dis) + min_dis;
             float x, y, z, angle;
             angle = (float)rand_norm() * static_cast<float>(M_PI * 35.0f / 180.0f) - static_cast<float>(M_PI * 17.5f / 180.0f);
             m_caster->GetClosePoint(x, y, z, DEFAULT_WORLD_OBJECT_SIZE, dis, angle);
             m_targets.SetDst(x, y, z, m_caster->GetOrientation());
             return;
        }
        default:
            break;
    }

    float dist;
    float angle = targetType.CalcDirectionAngle();
    float objSize = m_caster->GetObjectSize();
    if (targetType.GetTarget() == TARGET_DEST_CASTER_SUMMON)
        dist = PET_FOLLOW_DIST;
    else
        dist = m_spellInfo->Effects[effIndex].CalcRadius(m_caster);

    if (dist < objSize)
        dist = objSize;
    else if (targetType.GetTarget() == TARGET_DEST_CASTER_RANDOM)
        dist = objSize + (dist - objSize) * (float)rand_norm();

    Position pos;

    if (targetType.GetTarget() == TARGET_DEST_CASTER_FRONT_LEAP || targetType.GetTarget() == TARGET_DEST_CASTER_FRONT)
        m_caster->GetFirstCollisionPosition(pos, dist, angle);
    else
        m_caster->GetNearPosition(pos, dist, angle);

    m_targets.SetDst(*m_caster);

    Map* l_Map = m_caster->GetMap();
    if (!l_Map)
        return;

    uint32 l_DestAreaID = l_Map->GetAreaId(pos.m_positionX, pos.m_positionY, pos.m_positionZ);

    if (!sWorld->getBoolConfig(CONFIG_TOURNAMENT_ENABLE) || m_caster->GetAreaId() != 2177 || l_DestAreaID == 2177) ///> Prevents leaps to go through Greymane's Gurubashi arena's gates
        m_targets.ModDst(pos);
}

void Spell::SelectImplicitTargetDestTargets(SpellEffIndex effIndex, SpellImplicitTargetInfo const& targetType)
{
    WorldObject* target = m_targets.GetObjectTarget();
    switch (targetType.GetTarget())
    {
        case TARGET_DEST_TARGET_ENEMY:
        case TARGET_DEST_TARGET_ANY:
            m_targets.SetDst(*target);
            return;
        default:
            break;
    }

    float angle = targetType.CalcDirectionAngle();

    /// Re-setup angle for calculations
    if (Creature* l_Cre = target->ToCreature())
    {
        if (l_Cre->IsAIEnabled && !l_Cre->AI()->CanBeHitInTheBack())
            angle = 0.0f;
    }

    float objSize = target->GetObjectSize();
    float dist = m_spellInfo->Effects[effIndex].CalcRadius(m_caster);
    if (dist < objSize)
        dist = objSize;
    else if (targetType.GetTarget() == TARGET_DEST_TARGET_RANDOM)
        dist = objSize + (dist - objSize) * (float)rand_norm();

    Position pos;

    if (GetSpellInfo()->Id == 204263)
    {
        if (Unit* unitTarget = target->ToUnit())
            if (!m_caster->_IsValidAssistTarget(unitTarget, GetSpellInfo()))
                target = m_caster;

        target->GetPosition(&pos);
    }
    else if (GetSpellInfo()->Id == 210128)
        target->GetNearestPosition(pos, dist, angle);
    else
        target->GetNearPosition(pos, dist, angle);

    m_targets.SetDst(*target);

    /// Re-setup caster's orientation so it still faces the target
    if (Creature* l_Cre = target->ToCreature())
    {
        if (l_Cre->IsAIEnabled && !l_Cre->AI()->CanBeHitInTheBack())
            pos.m_orientation = m_caster->m_orientation;
    }

    m_targets.ModDst(pos);
}

void Spell::SelectImplicitDestDestTargets(SpellEffIndex effIndex, SpellImplicitTargetInfo const& targetType)
{
    // set destination to caster if no dest provided
    // can only happen if previous destination target could not be set for some reason
    // (not found nearby target, or channel target for example
    // maybe we should abort the spell in such case?
    if (!m_targets.HasDst())
        m_targets.SetDst(*m_caster);

    switch (targetType.GetTarget())
    {
        case TARGET_DEST_TARGET_ENEMY:
            if (Unit* target = m_targets.GetUnitTarget())
            {
                AddUnitTarget(target, 1 << effIndex);
                m_targets.SetDst(*target);
            }
            return;
        case TARGET_DEST_TARGET_SELECT:
        {
            if (m_targets.HasDst())
                return;
            else if (Unit* target = m_targets.GetUnitTarget())
                m_targets.SetDst(*target);

            return;
        }
        case TARGET_DEST_NEARBY_ENTRY:
            if (WorldObject* target = m_targets.GetObjectTarget())
                m_targets.SetDst(*target);
            return;
        case TARGET_DEST_DYNOBJ_ENEMY:
        case TARGET_DEST_DYNOBJ_ALLY:
        case TARGET_DEST_DYNOBJ_NONE:
        case TARGET_DEST_DEST:
        case TARGET_UNK_128:
            return;
        case TARGET_DEST_TRAJ:
            SelectImplicitTrajTargets();
            return;
        default:
            break;
    }

    float angle = targetType.CalcDirectionAngle();
    float dist = m_spellInfo->Effects[effIndex].CalcRadius(m_caster);
    if (targetType.GetTarget() == TARGET_DEST_DEST_RANDOM || targetType.GetTarget() == TARGET_DEST_DEST_RADIUS)
    {
        Position l_Pos;
        uint32 l_Counter = 0;
        do
        {
            dist *= static_cast<float>(rand_norm());
            angle = static_cast<float>(rand_norm()) * static_cast<float>(2 * M_PI);
            m_caster->SimplePosXYRelocationByAngle(l_Pos, dist, angle);
            l_Counter++;
        } while (!m_caster->IsWithinLOS(l_Pos.m_positionX, l_Pos.m_positionY, l_Pos.m_positionZ) && l_Counter < 20);
    }

    Position pos = *m_targets.GetDstPos();

    float l_Z = pos.m_positionZ;
    m_caster->MovePosition(pos, dist, angle);
    float floor = m_caster->GetMap()->GetHeight(m_caster->GetPhaseMask(), pos.m_positionX, pos.m_positionY, l_Z, true);
    if (fabs(pos.m_positionZ - floor) > 1.0f)
        pos.m_positionZ = floor;

    m_targets.ModDst(pos);
}

void Spell::SelectImplicitCasterObjectTargets(SpellEffIndex effIndex, SpellImplicitTargetInfo const& targetType)
{
    WorldObject* target = NULL;
    bool checkIfValid = true;

    switch (targetType.GetTarget())
    {
        case TARGET_UNIT_CASTER:
            target = m_caster;
            checkIfValid = false;
            break;
        case TARGET_UNIT_MASTER:
            target = m_caster->GetCharmerOrOwner();
            if (target == nullptr)
            {
                target = m_caster;
                checkIfValid = false;
            }
            break;
        case TARGET_UNIT_PET:
            target = m_caster->GetGuardianPet();
            break;
        case TARGET_UNIT_TARGET_MINIPET:
            target = Unit::GetUnit(*m_caster, m_caster->GetCritterGUID());
            break;
        case TARGET_UNIT_SUMMONER:
            if (m_caster->isSummon())
                target = m_caster->ToTempSummon()->GetSummoner();
            break;
        case TARGET_UNIT_VEHICLE:
            target = m_caster->GetVehicleBase();
            break;
        case TARGET_UNIT_TARGET_PASSENGER:
        case TARGET_UNIT_PASSENGER_0:
        case TARGET_UNIT_PASSENGER_1:
        case TARGET_UNIT_PASSENGER_2:
        case TARGET_UNIT_PASSENGER_3:
        case TARGET_UNIT_PASSENGER_4:
        case TARGET_UNIT_PASSENGER_5:
        case TARGET_UNIT_PASSENGER_6:
        case TARGET_UNIT_PASSENGER_7:
            if (m_caster->GetTypeId() == TYPEID_UNIT && m_caster->ToCreature()->IsVehicle())
                target = m_caster->GetVehicleKit()->GetPassenger(targetType.GetTarget() == TARGET_UNIT_TARGET_PASSENGER ? 0 : targetType.GetTarget() - TARGET_UNIT_PASSENGER_0);
            break;
        case TARGET_UNIT_CASTER_AREA_RAID: /// Raids buffs
        {
            CleanupTargetList();

            for (uint8 l_I = 0; l_I < m_spellInfo->EffectCount; ++l_I)
            {
                if (!m_spellInfo->Effects[l_I].IsEffect())
                    continue;

                Position const* l_Center = m_caster;
                std::list<WorldObject*> l_Targets;
                float l_Radius = m_spellInfo->Effects[l_I].CalcRadius(m_caster) * m_spellValue->RadiusMod;

                SearchAreaTargets(l_Targets, l_Radius, l_Center, m_caster, TARGET_OBJECT_TYPE_UNIT, SpellTargetCheckTypes::TARGET_CHECK_RAID, m_spellInfo->Effects[l_I].ImplicitTargetConditions);

                std::list<Unit*> l_UnitTargets;

                for (WorldObject* l_Iterator : l_Targets)
                {
                    if (Unit* l_UnitTarget = l_Iterator->ToUnit())
                    {
                        /// Raid buffs work just on Players
                        if (l_UnitTarget->GetTypeId() == TypeID::TYPEID_PLAYER)
                            l_UnitTargets.push_back(l_UnitTarget);
                    }
                }

                if (!l_UnitTargets.empty())
                {
                    if (uint32 l_MaxTargets = m_spellValue->MaxAffectedTargets)
                        JadeCore::Containers::RandomResizeList(l_UnitTargets, l_MaxTargets);

                    for (Unit* l_Iterator : l_UnitTargets)
                        AddUnitTarget(l_Iterator, 1 << l_I, false);
                }
            }
            break;
        }
        default:
            break;
    }

    CallScriptObjectTargetSelectHandlers(target, effIndex);

    if (target && target->ToUnit())
        AddUnitTarget(target->ToUnit(), 1 << effIndex, checkIfValid);
}

void Spell::SelectImplicitTargetObjectTargets(SpellEffIndex p_EffIndex, SpellImplicitTargetInfo const& p_TargetType)
{
    // Persistent auras should cast trigger spells when they is activated on target
    if (m_spellInfo->HasPersistenAura())
        return;

    ASSERT((m_targets.GetObjectTarget() || m_targets.GetItemTarget() || m_targets.GetUnitTarget()) && "Spell::SelectImplicitTargetObjectTargets - no explicit object or item target available!");

    WorldObject* l_Target = m_targets.GetObjectTarget();

    CallScriptObjectTargetSelectHandlers(l_Target, p_EffIndex);

    if (l_Target)
    {
        if (Unit* l_Unit = l_Target->ToUnit())
            AddUnitTarget(l_Unit, 1 << p_EffIndex, m_redirected == true ? false : true, false);
        else if (GameObject* l_GameObject = l_Target->ToGameObject())
            AddGOTarget(l_GameObject, 1 << p_EffIndex);
        else if (AreaTrigger* l_AreaTrigger = l_Target->ToAreaTrigger())
            AddAreaTriggerTarget(l_AreaTrigger, 1 << p_EffIndex);

        SelectImplicitChainTargets(p_EffIndex, p_TargetType, l_Target, 1 << p_EffIndex);
    }
    // Script hook can remove object target and we would wrongly land here
    else if (Item* l_Item = m_targets.GetItemTarget())
        AddItemTarget(l_Item, 1 << p_EffIndex);

    switch (p_TargetType.GetTarget())
    {
        case TARGET_UNIT_ALLY_OR_RAID: // Raids buffs
        {
            if (!l_Target || !l_Target->ToUnit() || !m_caster->ToPlayer())
                break;

            bool l_SameRaid = false;
            if (Player* l_Player = l_Target->ToPlayer())
            {
                if (l_Player->IsInSameGroupWith(m_caster->ToPlayer()) || l_Player->IsInSameRaidWith(m_caster->ToPlayer()))
                    l_SameRaid = true;
            }
            else if (Unit* l_Owner = l_Target->ToUnit()->GetOwner())
            {
                if (Player* l_PlayerOwner = l_Owner->ToPlayer())
                {
                    if (l_PlayerOwner->IsInSameGroupWith(m_caster->ToPlayer()) || l_PlayerOwner->IsInSameRaidWith(m_caster->ToPlayer()))
                        l_SameRaid = true;
                }
            }

            if (l_SameRaid)
            {
                CleanupTargetList();

                for (uint8 l_I = 0; l_I < m_spellInfo->EffectCount; ++l_I)
                {
                    // not call for empty effect.
                    // Also some spells use not used effect targets for store targets for dummy effect in triggered spells
                    if (!m_spellInfo->Effects[l_I].IsEffect())
                        continue;

                    Position const* l_Center = m_caster;
                    std::list<WorldObject*> l_Targets;
                    float l_Radius = m_spellInfo->Effects[l_I].CalcRadius(m_caster) * m_spellValue->RadiusMod;

                    SearchAreaTargets(l_Targets, l_Radius, l_Center, m_caster, TARGET_OBJECT_TYPE_UNIT, TARGET_CHECK_RAID, m_spellInfo->Effects[l_I].ImplicitTargetConditions);

                    CallScriptObjectAreaTargetSelectHandlers(l_Targets, p_EffIndex, p_TargetType.GetTarget());

                    std::list<Unit*> l_UnitTargets;
                    // for compatibility with older code - add only unit and go targets
                    // TODO: remove this
                    if (!l_Targets.empty())
                    {
                        for (std::list<WorldObject*>::iterator l_Iterator = l_Targets.begin(); l_Iterator != l_Targets.end(); ++l_Iterator)
                        {
                            if ((*l_Iterator))
                            {
                                if (Unit* unitTarget = (*l_Iterator)->ToUnit())
                                {
                                    /// Raid buffs work just on Players
                                    if (unitTarget->ToPlayer())
                                        l_UnitTargets.push_back(unitTarget);
                                }
                            }
                        }
                    }

                    if (!l_UnitTargets.empty())
                    {
                        // Other special target selection goes here
                        if (uint32 l_MaxTargets = m_spellValue->MaxAffectedTargets)
                            JadeCore::Containers::RandomResizeList(l_UnitTargets, l_MaxTargets);

                        for (std::list<Unit*>::iterator l_Iterator = l_UnitTargets.begin(); l_Iterator != l_UnitTargets.end(); ++l_Iterator)
                            AddUnitTarget(*l_Iterator, 1 << l_I, false);
                    }
                }
            }
            break;
        }
        default:
            break;
    }
}

void Spell::SelectImplicitChainTargets(SpellEffIndex effIndex, SpellImplicitTargetInfo const& targetType, WorldObject* target, uint32 effMask)
{
    uint32 maxTargets = m_spellInfo->Effects[effIndex].ChainTarget;
    if (Player* modOwner = m_caster->GetSpellModOwner())
    {
        if (m_spellInfo->Id == 45297) ///< Chain Lightning Overload
        {
            if (AuraEffect* l_AurEff = modOwner->GetAuraEffect(231722, EFFECT_0)) ///< Chain lightning
                maxTargets += l_AurEff->GetAmount();
        }
        else
            modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_JUMP_TARGETS, maxTargets, this);
    }

    ///< Rampage hits with Meat Cleave
    if (m_caster->HasAura(85739) && m_spellInfo->Id == 184367)
        maxTargets = 5;

    if (maxTargets > 1)
    {
        // mark damage multipliers as used
        for (uint32 k = effIndex; k < m_spellInfo->EffectCount; ++k)
            if (effMask & (1 << k))
                m_damageMultipliers[k] = 1.0f;
        m_applyMultiplierMask |= effMask;

        std::list<WorldObject*> targets;
        SearchChainTargets(targets, maxTargets - 1, target, targetType.GetObjectType(), targetType.GetCheckType()
            , m_spellInfo->Effects[effIndex].ImplicitTargetConditions, targetType.GetTarget() == TARGET_UNIT_TARGET_CHAINHEAL_ALLY);

        // Chain primary target is added earlier
        CallScriptObjectAreaTargetSelectHandlers(targets, effIndex, targetType.GetTarget());

        // for backward compability
        std::list<Unit*> unitTargets;
        for (std::list<WorldObject*>::iterator itr = targets.begin(); itr != targets.end(); ++itr)
            if (Unit* unitTarget = (*itr)->ToUnit())
                unitTargets.push_back(unitTarget);

        for (std::list<Unit*>::iterator itr = unitTargets.begin(); itr != unitTargets.end(); ++itr)
            AddUnitTarget(*itr, effMask, false);
    }
}

float tangent(float x)
{
    x = tan(x);
    //if (x < std::numeric_limits<float>::max() && x > -std::numeric_limits<float>::max()) return x;
    //if (x >= std::numeric_limits<float>::max()) return std::numeric_limits<float>::max();
    //if (x <= -std::numeric_limits<float>::max()) return -std::numeric_limits<float>::max();
    if (x < 100000.0f && x > -100000.0f) return x;
    if (x >= 100000.0f) return 100000.0f;
    if (x <= 100000.0f) return -100000.0f;
    return 0.0f;
}

#define DEBUG_TRAJ(a) //a

void Spell::SelectImplicitTrajTargets()
{
    if (!m_targets.HasTraj())
        return;

    float dist2d = m_targets.GetDist2d();
    if (!dist2d)
        return;

    float srcToDestDelta = m_targets.GetDstPos()->m_positionZ - m_targets.GetSrcPos()->m_positionZ;

    std::list<WorldObject*> targets;
    JadeCore::WorldObjectSpellTrajTargetCheck check(dist2d, m_targets.GetSrcPos(), m_caster, m_spellInfo);
    JadeCore::WorldObjectListSearcher<JadeCore::WorldObjectSpellTrajTargetCheck> searcher(m_caster, targets, check, GRID_MAP_TYPE_MASK_ALL);
    SearchTargets<JadeCore::WorldObjectListSearcher<JadeCore::WorldObjectSpellTrajTargetCheck> > (searcher, GRID_MAP_TYPE_MASK_ALL, m_caster, m_targets.GetSrcPos(), dist2d);
    if (targets.empty())
        return;

    targets.sort(JadeCore::ObjectDistanceOrderPred(m_caster));

    float b = tangent(m_targets.GetElevation());
    float a = (srcToDestDelta - dist2d * b) / (dist2d * dist2d);
    if (a > -0.0001f)
        a = 0;

    /// Trust client :D
    float bestDist = dist2d;// m_spellInfo->GetMaxRange(false);

    Position l_Pos = *m_targets.GetSrcPos();
    float l_Angle = m_targets.GetSrcPos()->GetAngle(m_targets.GetDstPos());
    l_Pos.SetOrientation(l_Angle);
    m_targets.ModSrc(l_Pos);

    std::list<WorldObject*>::const_iterator itr = targets.begin();
    for (; itr != targets.end(); ++itr)
    {
        if (Unit* unitTarget = (*itr)->ToUnit())
            if (m_caster == *itr || m_caster->IsOnVehicle(unitTarget) || (unitTarget)->GetVehicle())//(*itr)->IsOnVehicle(m_caster))
                continue;

        const float size = std::max((*itr)->GetObjectSize(), 1.0f); // 1/sqrt(3)
        // TODO: all calculation should be based on src instead of m_caster
        const float objDist2d = m_targets.GetSrcPos()->GetExactDist2d(*itr) * std::cos(m_targets.GetSrcPos()->GetRelativeAngle(*itr));
        float dz = (*itr)->GetPositionZ() - m_targets.GetSrcPos()->m_positionZ;

        float dist = objDist2d - size;
        float height = dist * tangent(m_targets.GetElevation());

        if (dist < bestDist && height < dz + size && height > dz - size)
        {
            bestDist = dist > 0 ? dist : 0;
            break;
        }

        dz += size;

        float l_TargetAngle = atan(dz / objDist2d);
        float l_LowDistance = objDist2d + size * std::cos(l_TargetAngle + -M_PI_2);
        float l_LowZ        = dz + size * std::sin(l_TargetAngle + -M_PI_2);

        float l_HighDistance = objDist2d + size * std::cos(l_TargetAngle + M_PI_2);
        float l_HighZ        = dz + size * std::sin(l_TargetAngle + M_PI_2);

        Position l_Origin(0.0f, 0.0f, 0.0f);

        Position l_Low(l_LowDistance,   l_LowZ, 0.0f);
        Position l_High(l_HighDistance, l_HighZ, 0.0f);

        float l_A = tangent(m_targets.GetElevation());
        float l_B = tangent(l_Origin.GetAngle(&l_Low));
        float l_C = tangent(l_Origin.GetAngle(&l_High));

        if (l_A > l_B && l_A < l_C)
        {
            bestDist = dist > 0 ? dist : 0;
            break;
        }

#define CHECK_DIST {\
            DEBUG_TRAJ(sLog->outError(LOG_FILTER_SPELLS_AURAS, "Spell::SelectTrajTargets: dist %f, height %f.", dist, height);)\
            if (dist > bestDist)\
                continue;\
            if (dist < objDist2d + size && dist > objDist2d - size)\
            {\
                bestDist = dist;\
                break;\
            }\
        }

        if (!a)
        {
            height = dz - size;
            dist = height / b;
            CHECK_DIST;

            height = dz + size;
            dist = height / b;
            CHECK_DIST;

            continue;
        }

        height = dz - size;
        float sqrt1 = b * b + 4 * a * height;
        if (sqrt1 > 0)
        {
            sqrt1 = sqrt(sqrt1);
            dist = (sqrt1 - b) / (2 * a);
            CHECK_DIST;
        }

        height = dz + size;
        float sqrt2 = b * b + 4 * a * height;
        if (sqrt2 > 0)
        {
            sqrt2 = sqrt(sqrt2);
            dist = (sqrt2 - b) / (2 * a);
            CHECK_DIST;

            dist = (-sqrt2 - b) / (2 * a);
            CHECK_DIST;
        }

        if (sqrt1 > 0)
        {
            dist = (-sqrt1 - b) / (2 * a);
            CHECK_DIST;
        }
    }

    if (m_targets.GetSrcPos()->GetExactDist2d(m_targets.GetDstPos()) > bestDist)
    {
        float x = m_targets.GetSrcPos()->m_positionX + std::cos(m_caster->GetOrientation()) * bestDist;
        float y = m_targets.GetSrcPos()->m_positionY + std::sin(m_caster->GetOrientation()) * bestDist;
        float z = m_targets.GetSrcPos()->m_positionZ + bestDist * (a * bestDist + b);

        if (itr != targets.end())
        {
            float distSq = (*itr)->GetExactDistSq(x, y, z);
            float sizeSq = (*itr)->GetObjectSize();
            sizeSq *= sizeSq;
            if (distSq > sizeSq)
            {
                float factor = 1 - sqrt(sizeSq / distSq);
                x += factor * ((*itr)->GetPositionX() - x);
                y += factor * ((*itr)->GetPositionY() - y);
                z += factor * ((*itr)->GetPositionZ() - z);

                distSq = (*itr)->GetExactDistSq(x, y, z); ///< distSq is never read 01/18/16
            }
        }

        Position trajDst;
        trajDst.Relocate(x, y, z, m_caster->GetOrientation());
        m_targets.ModDst(trajDst);
    }
}

void Spell::SelectEffectTypeImplicitTargets(uint8 p_EffIndex)
{
    // special case for SPELL_EFFECT_SUMMON_RAF_FRIEND and SPELL_EFFECT_SUMMON_PLAYER
    // TODO: this is a workaround - target shouldn't be stored in target map for those spells
    switch (m_spellInfo->Effects[p_EffIndex].Effect)
    {
        case SPELL_EFFECT_RAF_TELEPORT:
            if (m_caster->IsPlayer())
            {
                if (Player* l_Target = sObjectAccessor->FindPlayer(m_caster->m_SpellHelper.GetUint64(eSpellHelpers::RAFSummonTarget)))
                    AddUnitTarget(l_Target, 1 << p_EffIndex, false);

                m_caster->m_SpellHelper.GetUint64(eSpellHelpers::RAFSummonTarget) = 0;
            }
            return;
        case SPELL_EFFECT_SUMMON_RAF_FRIEND:
        case SPELL_EFFECT_SUMMON_PLAYER:
            if (m_caster->IsPlayer() && m_caster->ToPlayer()->GetSelection())
            {
                WorldObject* l_Target = ObjectAccessor::FindPlayer(m_caster->ToPlayer()->GetSelection());

                CallScriptObjectTargetSelectHandlers(l_Target, SpellEffIndex(p_EffIndex));

                if (l_Target && l_Target->ToPlayer())
                    AddUnitTarget(l_Target->ToUnit(), 1 << p_EffIndex, false);
            }
            return;
        default:
            break;
    }

    // select spell implicit targets based on effect type
    if (!m_spellInfo->Effects[p_EffIndex].GetImplicitTargetType())
        return;

    uint32 l_TargetMask = m_spellInfo->Effects[p_EffIndex].GetMissingTargetMask();
    if (!l_TargetMask)
        return;

    WorldObject* l_Target = NULL;

    switch (m_spellInfo->Effects[p_EffIndex].GetImplicitTargetType())
    {
        // add explicit object target or self to the target map
        case EFFECT_IMPLICIT_TARGET_EXPLICIT:
            // player which not released his spirit is Unit, but target flag for it is TARGET_FLAG_CORPSE_MASK
            if (l_TargetMask & (TARGET_FLAG_UNIT_MASK | TARGET_FLAG_CORPSE_MASK))
            {
                if (Unit* l_UnitTarget = m_targets.GetUnitTarget())
                    l_Target = l_UnitTarget;
                else if (l_TargetMask & TARGET_FLAG_CORPSE_MASK)
                {
                    if (Corpse* l_CorpseTarget = m_targets.GetCorpseTarget())
                    {
                        // TODO: this is a workaround - corpses should be added to spell target map too, but we can't do that so we add owner instead
                        if (Player* l_Owner = ObjectAccessor::FindPlayer(l_CorpseTarget->GetOwnerGUID()))
                            l_Target = l_Owner;
                    }
                }
                else //if (targetMask & TARGET_FLAG_UNIT_MASK)
                    l_Target = m_caster;
            }
            if (l_TargetMask & TARGET_FLAG_ITEM_MASK)
            {
                if (Item* l_ItemTarget = m_targets.GetItemTarget())
                    AddItemTarget(l_ItemTarget, 1 << p_EffIndex);
                return;
            }
            if (l_TargetMask & TARGET_FLAG_GAMEOBJECT_MASK)
                l_Target = m_targets.GetGOTarget();
            break;
        // add self to the target map
        case EFFECT_IMPLICIT_TARGET_CASTER:
            if (l_TargetMask & TARGET_FLAG_UNIT_MASK)
                l_Target = m_caster;
            break;
        default:
            break;
    }

    CallScriptObjectTargetSelectHandlers(l_Target, SpellEffIndex(p_EffIndex));

    if (l_Target)
    {
        if (l_Target->ToUnit())
            AddUnitTarget(l_Target->ToUnit(), 1 << p_EffIndex, false);
        else if (l_Target->ToGameObject())
            AddGOTarget(l_Target->ToGameObject(), 1 << p_EffIndex);
        else if (l_Target->ToAreaTrigger())
            AddAreaTriggerTarget(l_Target->ToAreaTrigger(), 1 << p_EffIndex);
    }
}

uint32 Spell::GetSearcherTypeMask(SpellTargetObjectTypes objType, ConditionContainer* condList)
{
    // this function selects which containers need to be searched for spell target
    uint32 retMask = GRID_MAP_TYPE_MASK_ALL;

    // filter searchers based on searched object type
    switch (objType)
    {
        case TARGET_OBJECT_TYPE_UNIT:
        case TARGET_OBJECT_TYPE_UNIT_AND_DEST:
        case TARGET_OBJECT_TYPE_CORPSE:
        case TARGET_OBJECT_TYPE_CORPSE_ENEMY:
        case TARGET_OBJECT_TYPE_CORPSE_ALLY:
            retMask &= GRID_MAP_TYPE_MASK_PLAYER | GRID_MAP_TYPE_MASK_CORPSE | GRID_MAP_TYPE_MASK_CREATURE;
            break;
        case TARGET_OBJECT_TYPE_GOBJ:
        case TARGET_OBJECT_TYPE_GOBJ_ITEM:
            retMask &= GRID_MAP_TYPE_MASK_GAMEOBJECT;
            break;
        case TARGET_OBJECT_TYPE_AREATRIGGER:
            retMask &= GRID_MAP_TYPE_MASK_AREATRIGGER;
            break;
        default:
            break;
    }
    if (!(AttributesCustomEx2 & SPELL_ATTR2_CAN_TARGET_DEAD))
        retMask &= ~GRID_MAP_TYPE_MASK_CORPSE;
    if (AttributesCustomEx3 & SPELL_ATTR3_ONLY_TARGET_PLAYERS)
        retMask &= GRID_MAP_TYPE_MASK_CORPSE | GRID_MAP_TYPE_MASK_PLAYER;
    if (AttributesCustomEx3 & SPELL_ATTR3_ONLY_TARGET_GHOSTS)
        retMask &= GRID_MAP_TYPE_MASK_PLAYER;

    if (condList)
        retMask &= sConditionMgr->GetSearcherTypeMaskForConditionList(*condList);
    return retMask;
}

template<class SEARCHER>
void Spell::SearchTargets(SEARCHER& searcher, uint32 containerMask, Unit* referer, Position const* pos, float radius)
{
    if (!containerMask)
        return;

    // search world and grid for possible targets
    bool searchInGrid = containerMask & (GRID_MAP_TYPE_MASK_CREATURE | GRID_MAP_TYPE_MASK_GAMEOBJECT);
    bool searchInWorld = containerMask & (GRID_MAP_TYPE_MASK_CREATURE | GRID_MAP_TYPE_MASK_PLAYER | GRID_MAP_TYPE_MASK_CORPSE);
    if (searchInGrid || searchInWorld)
    {
        float x,y;
        x = pos->GetPositionX();
        y = pos->GetPositionY();

        CellCoord p(JadeCore::ComputeCellCoord(x, y));
        Cell cell(p);
        cell.SetNoCreate();

        Map& map = *(referer->GetMap());

        if (searchInWorld)
        {
            TypeContainerVisitor<SEARCHER, WorldTypeMapContainer> world_object_notifier(searcher);
            cell.VisitWithNearbies(p, world_object_notifier, map, radius, x, y);
        }
        if (searchInGrid)
        {
            TypeContainerVisitor<SEARCHER, GridTypeMapContainer >  grid_object_notifier(searcher);
            cell.VisitWithNearbies(p, grid_object_notifier, map, radius, x , y);
        }
    }
}

WorldObject* Spell::SearchNearbyTarget(float range, SpellTargetObjectTypes objectType, SpellTargetCheckTypes selectionType, ConditionContainer* condList)
{
    WorldObject* target = NULL;
    uint32 containerTypeMask = GetSearcherTypeMask(objectType, condList);
    if (!containerTypeMask)
        return NULL;
    JadeCore::WorldObjectSpellNearbyTargetCheck check(range, m_caster, m_spellInfo, selectionType, condList);
    JadeCore::WorldObjectLastSearcher<JadeCore::WorldObjectSpellNearbyTargetCheck> searcher(m_caster, target, check, containerTypeMask);
    SearchTargets<JadeCore::WorldObjectLastSearcher<JadeCore::WorldObjectSpellNearbyTargetCheck> > (searcher, containerTypeMask, m_caster, m_caster, range);
    return target;
}

void Spell::SearchAreaTargets(std::list<WorldObject*>& targets, float range, Position const* position, Unit* referer, SpellTargetObjectTypes objectType, SpellTargetCheckTypes selectionType, ConditionContainer* condList, bool p_IgnoreCasterSize/* = false*/, bool p_IgnoreTargetSize/* = true*/)
{
    uint32 containerTypeMask = GetSearcherTypeMask(objectType, condList);
    if (!containerTypeMask)
        return;
    JadeCore::WorldObjectSpellAreaTargetCheck check(range, position, m_caster, referer, m_spellInfo, selectionType, condList, p_IgnoreCasterSize, p_IgnoreTargetSize);
    JadeCore::WorldObjectListSearcher<JadeCore::WorldObjectSpellAreaTargetCheck> searcher(m_caster, targets, check, containerTypeMask);
    SearchTargets<JadeCore::WorldObjectListSearcher<JadeCore::WorldObjectSpellAreaTargetCheck> > (searcher, containerTypeMask, m_caster, position, range);
}

void Spell::SearchChainTargets(std::list<WorldObject*>& p_Targets, uint32 chainTargets, WorldObject* p_Target, SpellTargetObjectTypes objectType, SpellTargetCheckTypes selectType, ConditionContainer* condList, bool isChainHeal)
{
    // max dist for jump target selection
    float jumpRadius = 0.0f;
    switch (m_spellInfo->DmgClass)
    {
        case SPELL_DAMAGE_CLASS_RANGED:
            // 7.5y for multi shot
            jumpRadius = 7.5f;
            if (m_spellInfo->SpellFamilyFlags[2] & 0x1)
                jumpRadius = 2.5f; ///< Chimaera Shot has smaller radius
            break;
        case SPELL_DAMAGE_CLASS_MELEE:
            // 5y for swipe, cleave and similar
            jumpRadius = 5.0f;
            if (m_spellInfo->Id == 185123)
                jumpRadius = 10.0f; ///< The glaive can ricochet to 2 additional enemies within 10 yards.
            break;
        case SPELL_DAMAGE_CLASS_NONE:
        case SPELL_DAMAGE_CLASS_MAGIC:
            // 12.5y for chain heal spell since 3.2 patch
            if (isChainHeal)
                jumpRadius = 12.5f;
            // 10y as default for magic chain spells
            else
                jumpRadius = 10.0f;
            break;
    }

    if (Player* modOwner = m_caster->GetSpellModOwner())
        modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_JUMP_DISTANCE, jumpRadius, this);

    // chain lightning/heal spells and similar - allow to jump at larger distance and go out of los
    bool isBouncingFar = (AttributesCustomEx4 & SPELL_ATTR4_AREA_TARGET_CHAIN
        || m_spellInfo->DmgClass == SPELL_DAMAGE_CLASS_NONE
        || m_spellInfo->DmgClass == SPELL_DAMAGE_CLASS_MAGIC);

    // max dist which spell can reach
    float searchRadius = jumpRadius;
    if (isBouncingFar)
        searchRadius *= chainTargets;

    if (m_spellInfo->Id == 22482) ///< Blade Flurry
    {
        isBouncingFar = true;
        p_Target = m_caster;
    }

    std::list<WorldObject*> l_TempTargets;
    SearchAreaTargets(l_TempTargets, searchRadius, p_Target, m_caster, objectType, selectType, condList);
    l_TempTargets.remove(p_Target);

    // remove targets which are always invalid for chain spells
    // for some spells allow only chain targets in front of caster (swipe for example)
    if (!isBouncingFar)
    {
        for (std::list<WorldObject*>::iterator itr = l_TempTargets.begin(); itr != l_TempTargets.end();)
        {
            std::list<WorldObject*>::iterator checkItr = itr++;
            if (!m_caster->HasInArc(static_cast<float>(M_PI), *checkItr))
                l_TempTargets.erase(checkItr);
        }
    }

    // Remove from the target list peoople that are on CC that breaks on damage
    // Maybe cheking only if the spell is positive is enough
    switch (m_spellInfo->Id) {
        case 44425:
        case 185123:
        case 204157:
            l_TempTargets.remove_if([=](WorldObject* l_target_to_check) -> bool
            {
                return l_target_to_check->ToUnit()->HasBreakOnDamageCrowdControlAura();
            });
        default:
            break;
    }

    /// Remove targets that are not in front of the caster
    switch (m_spellInfo->Id)
    {
        case 30455:
            l_TempTargets.remove_if([=](WorldObject* l_target_to_check) -> bool
            {
                return !m_caster->isInFront(l_target_to_check);
            });
            break;
        default:
            break;
    }
    while (chainTargets)
    {
        // try to get unit for next chain jump
        std::list<WorldObject*>::iterator l_FoundItr = l_TempTargets.end();
        // get unit with highest hp deficit in dist
        if (isChainHeal)
        {
            uint32 l_MaxHPDeficit = 0;
            for (std::list<WorldObject*>::iterator l_Itr = l_TempTargets.begin(); l_Itr != l_TempTargets.end(); ++l_Itr)
            {
                if (Unit* l_UnitTarget = (*l_Itr)->ToUnit())
                {
                    uint32 l_Deficit = l_UnitTarget->GetMaxHealth(m_caster) - l_UnitTarget->GetHealth(m_caster);
                    if ((l_Deficit > l_MaxHPDeficit || l_FoundItr == l_TempTargets.end()) && p_Target->IsWithinDist(l_UnitTarget, jumpRadius) && p_Target->IsWithinLOSInMap(l_UnitTarget))
                    {
                        /// If the caster is a player, healing spells should bounce on other players before anything else
                        if (!m_caster->GetSpellModOwner() || !m_caster->GetSpellModOwner()->IsPlayer() || (*l_Itr)->IsPlayer() || l_FoundItr == l_TempTargets.end() || !(*l_FoundItr)->IsPlayer())
                        {
                            l_FoundItr = l_Itr;
                            l_MaxHPDeficit = l_Deficit;
                        }
                    }
                }
            }
        }
        // get closest object
        else
        {
            for (std::list<WorldObject*>::iterator l_Itr = l_TempTargets.begin(); l_Itr != l_TempTargets.end(); ++l_Itr)
            {
                ///< Arcane Barrage for additional targets
                bool l_SkipLOS = m_spellInfo->Id == 44425 &&
                    m_targets.GetUnitTarget() != (*l_Itr) && p_Target != (*l_Itr) && p_Target->IsInMap(*l_Itr);

                if (l_FoundItr == l_TempTargets.end())
                {
                    if (((!isBouncingFar || p_Target->IsWithinDist(*l_Itr, jumpRadius)) && (l_SkipLOS || p_Target->IsWithinLOSInMap(*l_Itr))) || m_spellInfo->Id == 185123)
                        l_FoundItr = l_Itr;
                }
                else if (p_Target->GetDistanceOrder(*l_Itr, *l_FoundItr) && (l_SkipLOS || p_Target->IsWithinLOSInMap(*l_Itr)))
                    l_FoundItr = l_Itr;
            }
        }
        // not found any valid target - chain ends
        if (l_FoundItr == l_TempTargets.end())
            break;
        p_Target = *l_FoundItr;
        l_TempTargets.erase(l_FoundItr);
        p_Targets.push_back(p_Target);
        --chainTargets;
    }
}

void Spell::prepareDataForTriggerSystem(AuraEffect const* /*triggeredByAura*/)
{
    //==========================================================================================
    // Now fill data for trigger system, need know:
    // can spell trigger another or not (m_canTrigger)
    // Create base triggers flags for Attacker and Victim (m_procAttacker, m_procVictim and m_procEx)
    //==========================================================================================

    m_procVictim = m_procAttacker = 0;
    // Get data for type of attack and fill base info for trigger
    switch (m_spellInfo->DmgClass)
    {
        case SPELL_DAMAGE_CLASS_MELEE:
            m_procAttacker = PROC_FLAG_DONE_SPELL_MELEE_DMG_CLASS;
            if (m_attackType == WeaponAttackType::OffAttack)
                m_procAttacker |= PROC_FLAG_DONE_OFFHAND_ATTACK;
            else
                m_procAttacker |= PROC_FLAG_DONE_MAINHAND_ATTACK;
            m_procVictim   = PROC_FLAG_TAKEN_SPELL_MELEE_DMG_CLASS;
            break;
        case SPELL_DAMAGE_CLASS_RANGED:
            // Auto attack
            if (AttributesCustomEx2 & SPELL_ATTR2_AUTOREPEAT_FLAG)
            {
                m_procAttacker = PROC_FLAG_DONE_RANGED_AUTO_ATTACK;
                m_procVictim   = PROC_FLAG_TAKEN_RANGED_AUTO_ATTACK;
            }
            else // Ranged spell attack
            {
                m_procAttacker = PROC_FLAG_DONE_SPELL_RANGED_DMG_CLASS;
                m_procVictim   = PROC_FLAG_TAKEN_SPELL_RANGED_DMG_CLASS;
            }
            break;
        default:
            if (m_spellInfo->EquippedItemClass == ITEM_CLASS_WEAPON &&
                m_spellInfo->EquippedItemSubClassMask & (1<<ITEM_SUBCLASS_WEAPON_WAND)
                && AttributesCustomEx2 & SPELL_ATTR2_AUTOREPEAT_FLAG) // Wands auto attack
            {
                m_procAttacker = PROC_FLAG_DONE_RANGED_AUTO_ATTACK;
                m_procVictim   = PROC_FLAG_TAKEN_RANGED_AUTO_ATTACK;
            }
            // For other spells trigger procflags are set in Spell::DoAllEffectOnTarget
            // Because spell positivity is dependant on target
    }
    m_procEx = PROC_EX_NONE;

    // HACK: maybe process all spells triggered by SPELL_AURA_OVERRIDE_AUTOATTACK_WITH_MELEE_SPELL in the same way
    if (m_spellInfo->Id == 211796 || /// < Chaos Blades damage (mainhand)
        m_spellInfo->Id == 121473)   /// < Shadow Blades damage (mainhand)
    {
        m_procAttacker = ProcFlags::PROC_FLAG_DONE_MELEE_AUTO_ATTACK | ProcFlags::PROC_FLAG_DONE_MAINHAND_ATTACK;
        m_procVictim = ProcFlags::PROC_FLAG_TAKEN_MELEE_AUTO_ATTACK;
    }
    else if (m_spellInfo->Id == 211797 || /// < Chaos Blades damage (offhand)
             m_spellInfo->Id == 121474)   /// < Shadow Blades damage (offhand)
    {
        m_procAttacker = ProcFlags::PROC_FLAG_DONE_MELEE_AUTO_ATTACK | ProcFlags::PROC_FLAG_DONE_OFFHAND_ATTACK;
        m_procVictim = ProcFlags::PROC_FLAG_TAKEN_MELEE_AUTO_ATTACK;
    }

    // Hunter trap spells - activation proc for Lock and Load, Entrapment and Misdirection
    if (m_spellInfo->SpellFamilyName == SPELLFAMILY_HUNTER &&
        (m_spellInfo->SpellFamilyFlags[0] & 0x18 ||     // Freezing and Frost Trap, Freezing Arrow
        m_spellInfo->Id == 57879 ||                     // Snake Trap - done this way to avoid double proc
        m_spellInfo->SpellFamilyFlags[2] & 0x00024000)) // Explosive and Immolation Trap
    {
        m_procAttacker |= PROC_FLAG_DONE_TRAP_ACTIVATION;
        m_procVictim |= PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_NEG;
    }

    /* Effects which are result of aura proc from triggered spell cannot proc
        to prevent chain proc of these spells */

    // Hellfire Effect - trigger as DOT
    if (m_spellInfo->SpellFamilyName == SPELLFAMILY_WARLOCK && m_spellInfo->SpellFamilyFlags[0] & 0x00000040)
    {
        m_procAttacker = PROC_FLAG_DONE_PERIODIC;
        m_procVictim   = PROC_FLAG_TAKEN_PERIODIC;
    }

    // Ranged autorepeat attack is set as triggered spell - ignore it
    if (!(m_procAttacker & PROC_FLAG_DONE_RANGED_AUTO_ATTACK))
    {
        if (_triggeredCastFlags & TRIGGERED_DISALLOW_PROC_EVENTS &&
            (AttributesCustomEx2 & SPELL_ATTR2_TRIGGERED_CAN_TRIGGER_PROC ||
            AttributesCustomEx3 & SPELL_ATTR3_TRIGGERED_CAN_TRIGGER_PROC_2))
            m_procEx |= PROC_EX_INTERNAL_CANT_PROC;
        else if (_triggeredCastFlags & TRIGGERED_DISALLOW_PROC_EVENTS)
            m_procEx |= PROC_EX_INTERNAL_TRIGGERED;
    }
    // Totem casts require spellfamilymask defined in spell_proc_event to proc
    if (m_originalCaster && m_caster != m_originalCaster && m_caster->GetTypeId() == TYPEID_UNIT && m_caster->ToCreature()->isTotem() && m_caster->IsControlledByPlayer())
        m_procEx |= PROC_EX_INTERNAL_REQ_FAMILY;
}

void Spell::CleanupTargetList()
{
    m_UniqueTargetInfo.clear();
    m_UniqueGOTargetInfo.clear();
    m_UniqueItemInfo.clear();
    m_delayMoment = 0;
}

void Spell::AddUnitTarget(Unit* p_Target, uint32 p_EffectMask, bool p_CheckIfValid /*= true*/, bool p_Implicit /*= true*/, Position const* p_LosPosition /* = nullptr */)
{
    LinkedSpell(p_Target, p_Target, SPELL_LINK_ON_ADD_TARGET, p_EffectMask);

    for (uint32 l_EffectIndex = 0; l_EffectIndex < m_spellInfo->EffectCount; ++l_EffectIndex)
        if (!m_spellInfo->Effects[l_EffectIndex].IsEffect() || !CheckEffectTarget(p_Target, l_EffectIndex, p_LosPosition) || CheckEffFromDummy(p_Target, l_EffectIndex)
            || (p_Target->isTotem() && m_spellInfo->Effects[l_EffectIndex].IsHealingOrShieldingEffect()))
            p_EffectMask &= ~(1 << l_EffectIndex);

    // no effects left
    if (!p_EffectMask)
        return;

    if (p_CheckIfValid)
        if (m_spellInfo->CheckTarget(m_caster, p_Target, IsTriggered() ? true: p_Implicit) != SPELL_CAST_OK)
            return;

    // Check for effect immune skip if immuned
    for (uint32 l_EffectIndex = 0; l_EffectIndex < m_spellInfo->EffectCount; ++l_EffectIndex)
        if (p_Target->IsImmunedToSpellEffect(m_spellInfo, l_EffectIndex))
            p_EffectMask &= ~(1 << l_EffectIndex);

    uint64 l_TargetGUID = p_Target->GetGUID();

    // Lookup target in already in list
    for (std::list<TargetInfo>::iterator l_Itr = m_UniqueTargetInfo.begin(); l_Itr != m_UniqueTargetInfo.end(); ++l_Itr)
    {
        if (l_TargetGUID == l_Itr->targetGUID)             // Found in list
        {
            l_Itr->effectMask |= p_EffectMask;             // Immune effects removed from mask
            l_Itr->scaleAura = false;
            if (m_auraScaleMask && l_Itr->effectMask == m_auraScaleMask && m_caster != p_Target)
            {
                SpellInfo const* l_AuraSpell = sSpellMgr->GetSpellInfo(sSpellMgr->GetFirstSpellInChain(m_spellInfo->Id));
                if (uint32(p_Target->getLevelForTarget(m_caster) + 10) >= l_AuraSpell->SpellLevel)
                    l_Itr->scaleAura = true;
            }
            return;
        }
    }

    // This is new target calculate data for him

    // Get spell hit result on target
    TargetInfo l_TargetInfo;
    l_TargetInfo.targetGUID = l_TargetGUID;                         // Store target GUID
    l_TargetInfo.effectMask = p_EffectMask;                         // Store all effects not immune
    l_TargetInfo.processed  = false;                              // Effects not apply on target
    l_TargetInfo.alive      = p_Target->isAlive();
    l_TargetInfo.damage     = 0;
    l_TargetInfo.crit       = false;
    l_TargetInfo.scaleAura  = false;
    l_TargetInfo.timeDelay  = 0;
    l_TargetInfo.DamageLog  = nullptr;

    if (m_auraScaleMask && l_TargetInfo.effectMask == m_auraScaleMask && m_caster != p_Target)
    {
        SpellInfo const* l_AuraSpell = sSpellMgr->GetSpellInfo(sSpellMgr->GetFirstSpellInChain(m_spellInfo->Id));
        if (uint32(p_Target->getLevelForTarget(m_caster) + 10) >= l_AuraSpell->SpellLevel)
            l_TargetInfo.scaleAura = true;
    }

    // Calculate hit result
    if (m_originalCaster)
    {
        l_TargetInfo.missCondition = m_originalCaster->SpellHitResult(p_Target, m_spellInfo, m_canReflect);
        if (m_skipCheck && l_TargetInfo.missCondition != SPELL_MISS_IMMUNE)
            l_TargetInfo.missCondition = SPELL_MISS_NONE;
    }
    else
        l_TargetInfo.missCondition = SPELL_MISS_EVADE; //SPELL_MISS_NONE;

    // Spell have speed - need calculate incoming time
    // Incoming time is zero for self casts. At least I think so.
    if (m_spellInfo->Effects[0].Effect == SPELL_EFFECT_KNOCK_BACK)
    {
        m_delayMoment = 1;
        l_TargetInfo.timeDelay = 0;
    }
    else if (m_spellInfo->Speed > 0.0f && m_caster != p_Target && m_spellInfo->Speed != 12345)
    {
        // calculate spell incoming interval
        // TODO: this is a hack
        float l_Dist = m_caster->GetDistance(p_Target->GetPositionX(), p_Target->GetPositionY(), p_Target->GetPositionZ());

        if (l_Dist < 5.0f)
            l_Dist = 5.0f;

        if (!(AttributesCustomEx9 & SPELL_ATTR9_SPECIAL_DELAY_CALCULATION))
            l_TargetInfo.timeDelay = uint64(floor(l_Dist / m_spellInfo->Speed * 1000.0f));
        else
            l_TargetInfo.timeDelay = uint64(m_spellInfo->Speed * 1000.0f);

        if ((m_spellInfo->Id == 133 || m_spellInfo->Id == 11366) && !p_Target->GetSpellModOwner()) ///< In PVE scenarios, Fireball and Pyroblast should NEVER take more than 750ms to reach their target: https://www.wowhead.com/spell=48107/heating-up#comments Rodalpho's comment
            l_TargetInfo.timeDelay = std::min(l_TargetInfo.timeDelay, uint64(750));

        // Calculate minimum incoming time
        if (m_delayMoment == 0 || m_delayMoment > l_TargetInfo.timeDelay)
            m_delayMoment = l_TargetInfo.timeDelay;
    }
    else if ((m_caster->IsPlayer() || m_caster->ToCreature()->isPet()) && m_caster != p_Target)
    {
        if (!IsTriggered() && m_spellInfo->_IsCrowdControl())
        {
            l_TargetInfo.timeDelay = 100LL;
            m_delayMoment = 100LL;
        }
        if (m_spellInfo->IsPositive() && !IsTriggered() && m_spellInfo->Targets != 0x40)
        {
            switch (m_spellInfo->Effects[0].Effect)
            {
                case SPELL_EFFECT_SCHOOL_DAMAGE:
                case SPELL_EFFECT_APPLY_AURA:
                case SPELL_EFFECT_APPLY_AURA_2:
                case SPELL_EFFECT_APPLY_AURA_ON_PET:
                case SPELL_EFFECT_POWER_BURN:
                case SPELL_EFFECT_DISPEL:
                {
                    l_TargetInfo.timeDelay = 100LL;
                    m_delayMoment = 100LL;
                }
                default:
                    break;
            }
            // Shadowstep
            if (m_spellInfo->Id == 36563)
            {
                l_TargetInfo.timeDelay = 100LL;
                m_delayMoment = 100LL;
            }
        }
    }
    // Removing Death Grip cooldown
    else if (m_spellInfo->Id == 90289)
    {
        l_TargetInfo.timeDelay = 100LL;
        m_delayMoment = 100LL;
    }
    // Misdirection
    else if (m_spellInfo->Id == 35079 || m_spellInfo->Id == 57934)
    {
        l_TargetInfo.timeDelay = 100LL;
        m_delayMoment = 100LL;
    }
    /// Apply delay for CC spells here, can be easily tweaked.
    else if (m_spellInfo->Speed == 12345)
    {
        l_TargetInfo.timeDelay = 150LL;
        m_delayMoment = 150LL;
    }
    else
        l_TargetInfo.timeDelay = 0LL;

    // If target reflect spell back to caster
    if (l_TargetInfo.missCondition == SPELL_MISS_REFLECT)
    {
        // process reflect removal (not delayed)
        if (!l_TargetInfo.timeDelay)
        {
            DamageInfo l_DamageInfo(m_caster, unitTarget, 1, this, DamageEffectType::SPELL_DIRECT_DAMAGE, WeaponAttackType::BaseAttack);
            m_caster->ProcDamageAndSpell(p_Target, PROC_FLAG_NONE, PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_NEG, PROC_EX_REFLECT, l_DamageInfo);

            if (m_spellInfo->Id == 2136) // hack to trigger impact in reflect
                m_caster->ProcDamageAndSpell(m_caster, PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_NEG, PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_NEG, PROC_EX_NORMAL_HIT, l_DamageInfo);
        }
        // Calculate reflected spell result on caster
        l_TargetInfo.reflectResult = m_caster->SpellHitResult(m_caster, m_spellInfo, m_canReflect);

        if (l_TargetInfo.reflectResult == SPELL_MISS_REFLECT)     // Impossible reflect again, so simply deflect spell
            l_TargetInfo.reflectResult = SPELL_MISS_PARRY;

        // Increase time interval for reflected spells by 1.5
        l_TargetInfo.timeDelay += l_TargetInfo.timeDelay >> 1;
    }
    else
        l_TargetInfo.reflectResult = SPELL_MISS_NONE;

    // Add target to list
    m_UniqueTargetInfo.push_back(std::move(l_TargetInfo));
}

void Spell::AddGOTarget(GameObject* go, uint32 effectMask)
{
    for (uint32 effIndex = 0; effIndex < m_spellInfo->EffectCount; ++effIndex)
    {
        if (!m_spellInfo->Effects[effIndex].IsEffect())
            effectMask &= ~(1 << effIndex);
        else
        {
            switch (m_spellInfo->Effects[effIndex].Effect)
            {
                case SPELL_EFFECT_GAMEOBJECT_DAMAGE:
                case SPELL_EFFECT_GAMEOBJECT_REPAIR:
                case SPELL_EFFECT_GAMEOBJECT_SET_DESTRUCTION_STATE:
                    if (go->GetGoType() != GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING)
                        effectMask &= ~(1 << effIndex);
                    break;
                default:
                    break;
            }
        }
    }

    if (!effectMask)
        return;

    uint64 targetGUID = go->GetGUID();

    // Lookup target in already in list
    for (std::list<GOTargetInfo>::iterator ihit = m_UniqueGOTargetInfo.begin(); ihit != m_UniqueGOTargetInfo.end(); ++ihit)
    {
        if (targetGUID == ihit->targetGUID)                 // Found in list
        {
            ihit->effectMask |= effectMask;                 // Add only effect mask
            return;
        }
    }

    // This is new target calculate data for him

    GOTargetInfo target;
    target.targetGUID = targetGUID;
    target.effectMask = effectMask;
    target.processed  = false;                              // Effects not apply on target

    // Spell have speed - need calculate incoming time
    if (m_spellInfo->Speed > 0.0f)
    {
        // calculate spell incoming interval
        float dist = m_caster->GetDistance(go->GetPositionX(), go->GetPositionY(), go->GetPositionZ());
        if (dist < 5.0f)
            dist = 5.0f;
       if (!(AttributesCustomEx9 & SPELL_ATTR9_SPECIAL_DELAY_CALCULATION))
           target.timeDelay = uint64(floor(dist / m_spellInfo->Speed * 1000.0f));
       else
           target.timeDelay = uint64(m_spellInfo->Speed * 1000.0f);

        if (m_delayMoment == 0 || m_delayMoment > target.timeDelay)
            m_delayMoment = target.timeDelay;
    }
    else
        target.timeDelay = 0LL;

    // Add target to list
    m_UniqueGOTargetInfo.push_back(std::move(target));
}

void Spell::AddItemTarget(Item* item, uint32 effectMask)
{
    for (uint32 effIndex = 0; effIndex < m_spellInfo->EffectCount; ++effIndex)
        if (!m_spellInfo->Effects[effIndex].IsEffect())
            effectMask &= ~(1 << effIndex);

    // no effects left
    if (!effectMask)
        return;

    // Lookup target in already in list
    for (std::list<ItemTargetInfo>::iterator ihit = m_UniqueItemInfo.begin(); ihit != m_UniqueItemInfo.end(); ++ihit)
    {
        if (item == ihit->item)                            // Found in list
        {
            ihit->effectMask |= effectMask;                // Add only effect mask
            return;
        }
    }

    // This is new target add data
    ItemTargetInfo target;
    target.item       = item;
    target.effectMask = effectMask;

    m_UniqueItemInfo.push_back(target);
}

void Spell::AddAreaTriggerTarget(AreaTrigger* p_AreaTrigger, uint32 p_EffectMask)
{
    for (uint8 l_EffectIndex = 0; l_EffectIndex < m_spellInfo->EffectCount; ++l_EffectIndex)
    {
        if (!m_spellInfo->Effects[l_EffectIndex].IsEffect())
            p_EffectMask &= ~(1 << l_EffectIndex);
    }

    if (!p_EffectMask)
        return;

    uint64 l_TargetGUID = p_AreaTrigger->GetGUID();

    // Lookup target in already in list
    for (std::list<AreaTriggerTargetInfo>::iterator l_Iterator = m_UniqueAreaTriggerTargetInfo.begin(); l_Iterator != m_UniqueAreaTriggerTargetInfo.end(); ++l_Iterator)
    {
        if (l_TargetGUID == l_Iterator->targetGUID)                 // Found in list
        {
            l_Iterator->effectMask |= p_EffectMask;                 // Add only effect mask
            return;
        }
    }

    AreaTriggerTargetInfo l_AreaTriggerTargetInfo;
    l_AreaTriggerTargetInfo.targetGUID = p_AreaTrigger->GetGUID();
    l_AreaTriggerTargetInfo.processed  = false;
    l_AreaTriggerTargetInfo.effectMask = p_EffectMask;
    l_AreaTriggerTargetInfo.timeDelay  = 0.0f;

    m_UniqueAreaTriggerTargetInfo.push_back(l_AreaTriggerTargetInfo);
}

void Spell::AddDestTarget(SpellDestination const& dest, uint32 effIndex)
{
    m_destTargets[effIndex] = dest;
}

void Spell::DoAllEffectOnTarget(TargetInfo* p_Target)
{
    if (!p_Target || p_Target->processed)
        return;

    m_missInfo = p_Target->missCondition;
    p_Target->processed = true;                               // Target checked in apply effects procedure

    // Get mask of effects for target
    uint32 l_Mask = p_Target->effectMask;

    Unit* l_Unit = m_caster->GetGUID() == p_Target->targetGUID ? m_caster : ObjectAccessor::GetUnit(*m_caster, p_Target->targetGUID);
    if (!l_Unit)
    {
        uint32 l_FarMask = 0;
        // create far target mask
        for (uint8 l_Itr = 0; l_Itr < m_spellInfo->EffectCount; ++l_Itr)
            if (m_spellInfo->Effects[l_Itr].IsFarUnitTargetEffect())
                if ((1 << l_Itr) & l_Mask)
                    l_FarMask |= (1 << l_Itr);

        if (!l_FarMask)
            return;
        // find unit in world
        l_Unit = ObjectAccessor::FindUnit(p_Target->targetGUID);
        if (!l_Unit)
            return;

        // do far effects on the unit
        // can't use default call because of threading, do stuff as fast as possible
        for (uint8 l_Itr = 0; l_Itr < m_spellInfo->EffectCount; ++l_Itr)
            if (l_FarMask & (1 << l_Itr))
                HandleEffects(l_Unit, nullptr, nullptr, nullptr, l_Itr, SPELL_EFFECT_HANDLE_HIT_TARGET);
        return;
    }

    if (l_Unit->isAlive() != p_Target->alive && m_spellInfo->Id == 105174)
        CallScriptAfterHitHandlers();

    if (l_Unit->isAlive() != p_Target->alive && !m_spellInfo->HasEffect(SPELL_EFFECT_SELF_RESURRECT) && !m_spellInfo->HasEffect(SPELL_EFFECT_RESURRECT) && !m_spellInfo->HasEffect(SPELL_EFFECT_RESURRECT_WITH_AURA))
        return;

    if (getState() == SPELL_STATE_DELAYED && !m_spellInfo->IsPositive() && (getMSTime() - p_Target->timeDelay) <= l_Unit->m_lastSanctuaryTime)
        return;                                             // No missinfo in that case

    // Some spells should remove Camouflage after hit (traps, some spells that have casting time)
    if (p_Target->targetGUID != m_caster->GetGUID() && m_spellInfo && m_spellInfo->IsBreakingCamouflageAfterHit())
    {
        if (TempSummon* l_Summon = m_caster->ToTempSummon())
        {
            if (Unit* l_Owner = l_Summon->GetSummoner())
                l_Owner->RemoveAurasByType(SPELL_AURA_MOD_CAMOUFLAGE);
        }
        else
            m_caster->RemoveAurasByType(SPELL_AURA_MOD_CAMOUFLAGE);
    }

    // Get original caster (if exist) and calculate damage/healing from him data
    Unit* l_Caster = m_originalCaster ? m_originalCaster : m_caster;

    // Skip if m_originalCaster not avaiable
    if (!l_Caster)
        return;

    SpellMissInfo l_MissInfo = p_Target->missCondition;

    // Need init unitTarget by default unit (can changed in code on reflect)
    // Or on missInfo != SPELL_MISS_NONE unitTarget undefined (but need in trigger subsystem)
    unitTarget = l_Unit;

    // Reset damage/healing counter
    m_damage          = p_Target->damage;
    m_BaseDamage      = m_damage;
    m_healing         = -p_Target->damage;
    m_final_damage    = 0;
    m_absorbed_damage = 0;

    // Fill base trigger info
    uint32 l_ProcAttacker = m_procAttacker;
    uint32 l_ProcVictim   = m_procVictim;
    uint32 l_ProcEx = m_procEx;

    m_spellAura = nullptr; // Set aura to null for every target-make sure that pointer is not used for unit without aura applied

                            //Spells with this flag cannot trigger if effect is casted on self
    bool l_CanEffectTrigger = (l_MissInfo == SPELL_MISS_IMMUNE || l_MissInfo == SPELL_MISS_IMMUNE2) || (CanSpellProc(unitTarget, l_Mask) && unitTarget->CanProc() && (CanExecuteTriggersOnHit(l_Mask)) && CanProcOnTarget(unitTarget));
    Unit* l_SpellHitTarget = NULL;

    if (l_MissInfo == SPELL_MISS_NONE)                        // In case spell hit target, do all effect on that target
        l_SpellHitTarget = l_Unit;
    else if (l_MissInfo == SPELL_MISS_REFLECT)                // In case spell reflect from target, do all effect on caster (if hit)
    {
        if (p_Target->reflectResult == SPELL_MISS_NONE)       // If reflected spell hit caster -> do all effect on him
            l_SpellHitTarget = m_caster;
    }

    /// Hackfix for Imonar the Soulhunter - Pulse Grenade, immunities shouldn't prevent knock back
    if (m_spellInfo->Id == 247388 && l_MissInfo == SPELL_MISS_DEFLECT)
    {
        l_SpellHitTarget = l_Unit;

        /// Remove damaging effect
        l_Mask &= ~1;
    }

    if (l_MissInfo != SpellMissInfo::SPELL_MISS_NONE)
    {
        if (m_caster->GetTypeId() == TYPEID_UNIT && m_caster->ToCreature()->IsAIEnabled)
            m_caster->ToCreature()->AI()->SpellMissTarget(l_Unit, m_spellInfo, l_MissInfo);
    }

    /// Custom WoD Script - Death from Above should give immunity to all spells while rogue is in jump effect
    if (unitTarget->GetGUID() != m_caster->GetGUID() && unitTarget->getClass() == CLASS_ROGUE && unitTarget->getLevel() == 100 && unitTarget->HasAura(152150, unitTarget->GetGUID()))
        return;

    if (l_SpellHitTarget)
    {
        SpellMissInfo l_MissInfo2 = DoSpellHitOnUnit(l_SpellHitTarget, l_Mask, p_Target->scaleAura);
        if (l_MissInfo2 != SPELL_MISS_NONE)
        {
            if (l_MissInfo2 != SPELL_MISS_MISS)
                m_caster->SendSpellMiss(l_Unit, m_spellInfo->Id, l_MissInfo2);
            m_damage = 0;
            l_SpellHitTarget = NULL;
        }
    }

    /// Keep proc ex flags
    l_ProcEx |= m_procEx;

    // Do not take combo points on dodge and miss
    if (l_MissInfo != SPELL_MISS_NONE && m_needComboPoints &&
            m_targets.GetUnitTargetGUID() == p_Target->targetGUID)
    {
        m_needComboPoints = false;
        // Restore spell mods for a miss/dodge/parry Cold Blood
        // TODO: check how broad this rule should be
        if (m_caster->IsPlayer() && (l_MissInfo == SPELL_MISS_MISS ||
                l_MissInfo == SPELL_MISS_DODGE || l_MissInfo == SPELL_MISS_PARRY))
            m_caster->ToPlayer()->RestoreSpellMods(this, 14177);
    }

    // Trigger info was not filled in spell::preparedatafortriggersystem - we do it now
    if (l_CanEffectTrigger && !l_ProcAttacker && !l_ProcVictim)
    {
        bool l_Positive = true;
        if (m_damage > 0)
            l_Positive = false;
        else if (!m_healing)
        {
            for (uint8 l_Itr = 0; l_Itr< m_spellInfo->EffectCount; ++l_Itr)
                // If at least one effect negative spell is negative hit
                if (l_Mask & (1<<l_Itr) && !m_spellInfo->IsPositiveEffect(l_Itr))
                {
                    l_Positive = false;
                    break;
                }
        }
        switch (m_spellInfo->DmgClass)
        {
            case SPELL_DAMAGE_CLASS_MAGIC:
                if (l_Positive)
                {
                    l_ProcAttacker |= PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS;
                    l_ProcVictim   |= PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_POS;
                }
                else
                {
                    l_ProcAttacker |= PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_NEG;
                    l_ProcVictim   |= PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_NEG;
                }
            break;
            case SPELL_DAMAGE_CLASS_NONE:
                if (l_Positive)
                {
                    l_ProcAttacker |= PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_POS;
                    l_ProcVictim   |= PROC_FLAG_TAKEN_SPELL_NONE_DMG_CLASS_POS;
                }
                else
                {
                    l_ProcAttacker |= PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_NEG;
                    l_ProcVictim   |= PROC_FLAG_TAKEN_SPELL_NONE_DMG_CLASS_NEG;
                }
            break;
        }
    }

    if (l_Caster->IsPlayer() && m_spellInfo->CanTriggerPoisonAdditional())
        l_Caster->ToPlayer()->CastItemCombatSpell(unitTarget, m_attackType, PROC_FLAG_TAKEN_DAMAGE, l_ProcEx);

    // All calculated do it!

    bool l_AllFalse = true;
    // Do healing and triggers
    if (m_healing > 0)
    {
        l_AllFalse = false;
        bool l_Crit = p_Target->crit;
        uint32 l_AddHealth = m_healing;
        if (l_Crit)
        {
            l_ProcEx |= PROC_EX_CRITICAL_HIT;
            l_AddHealth = l_Caster->SpellCriticalHealingBonus(m_spellInfo, l_AddHealth, unitTarget);
        }
        else
            l_ProcEx |= PROC_EX_NORMAL_HIT;

        m_healing = l_AddHealth;
        CallScriptOnHitHandlers();
        l_AddHealth = m_healing;

        uint32 l_Absorb = 0;
        int32 l_Gain = l_Caster->HealBySpell(unitTarget, m_spellInfo, l_AddHealth, l_Crit, &l_Absorb);
        unitTarget->getHostileRefManager().threatAssist(l_Caster, float(l_Gain) * 0.5f, m_spellInfo);
        m_healing = l_Gain;

        // Do triggers for unit (reflect triggers passed on hit phase for correct drop charge)
        if (l_CanEffectTrigger && l_MissInfo != SPELL_MISS_REFLECT)
        {
            DamageInfo l_DamageInfo(l_Caster, unitTarget, l_AddHealth, this, DamageEffectType::HEAL, m_attackType);
            l_DamageInfo.ChangeSchoolMask(m_spellSchoolMask);
            l_DamageInfo.SetOverHeal(int32(l_AddHealth - l_Gain - l_Absorb));

            l_Caster->ProcDamageAndSpell(unitTarget, l_ProcAttacker, l_ProcVictim, l_ProcEx, l_DamageInfo, m_triggeredByAuraSpell);
        }
    }
    else if (m_addpower != 0)
    {
        l_AllFalse = false;
        /// Do triggers for unit (reflect triggers passed on hit phase for correct drop charge)
        if (l_CanEffectTrigger && l_MissInfo != SPELL_MISS_REFLECT)
        {
            l_ProcEx |= PROC_EX_NORMAL_HIT;
            DamageInfo l_DamageInfo(l_Caster, m_targets.GetUnitTarget() ? m_targets.GetUnitTarget() : m_caster, 0, 0, this, SPELL_DIRECT_DAMAGE);
            l_DamageInfo.SetAddPower(m_addpower);
            l_DamageInfo.SetAddPType(m_addptype);
            l_Caster->ProcDamageAndSpell(unitTarget, l_ProcAttacker, l_ProcVictim, l_ProcEx, l_DamageInfo, m_triggeredByAuraSpell);
        }
    }
    // Do damage and triggers
    if (m_damage > 0)
    {
        l_AllFalse = false;
        CallScriptOnHitHandlers();
        /// Fill base damage struct (unitTarget - is real spell target)
        SpellNonMeleeDamage l_DamageInfo(l_Caster, unitTarget, m_damage, this, DamageEffectType::SPELL_DIRECT_DAMAGE, m_attackType);
        l_DamageInfo.ChangeSchoolMask(m_spellSchoolMask);

        /// Add bonuses and fill damageInfo struct
        l_Caster->CalculateSpellDamageTaken(l_DamageInfo, p_Target->crit, p_Target->DamageLog);

        l_Caster->DealDamageMods(l_DamageInfo);

        /// Send log damage message to client
        if (l_MissInfo == SPELL_MISS_REFLECT)
            l_DamageInfo.OverrideActor(l_Unit);

        l_ProcEx |= createProcExtendMask(l_DamageInfo, l_MissInfo);
        l_ProcVictim |= PROC_FLAG_TAKEN_DAMAGE;

        // Do triggers for unit (reflect triggers passed on hit phase for correct drop charge)
        if (l_CanEffectTrigger && l_MissInfo != SPELL_MISS_REFLECT)
        {
            l_Caster->ProcDamageAndSpell(unitTarget, l_ProcAttacker, l_ProcVictim, l_ProcEx, l_DamageInfo, m_triggeredByAuraSpell);
            if (l_Caster->IsPlayer() && (AttributesCustom & SPELL_ATTR0_STOP_ATTACK_TARGET) == 0 &&
                (m_spellInfo->DmgClass == SPELL_DAMAGE_CLASS_MELEE || m_spellInfo->DmgClass == SPELL_DAMAGE_CLASS_RANGED))
                l_Caster->ToPlayer()->CastItemCombatSpell(unitTarget, m_attackType, l_ProcVictim, l_ProcEx);
        }

        m_damage = l_DamageInfo.GetAmount();

        if (p_Target->DamageLog  && m_ShoudLogDamage && m_damage > m_MinDamageLog)
        {
            *p_Target->DamageLog << "Final damage " << m_damage << std::endl;
            sLog->outExtChat("#log-damage-703", "", false, p_Target->DamageLog->str().c_str());
            delete p_Target->DamageLog;
            p_Target->DamageLog = nullptr;
        }

        l_Caster->DealSpellDamage(l_DamageInfo, true);

        // Used in spell scripts
        m_final_damage = l_DamageInfo.GetAmount();
        m_absorbed_damage = l_DamageInfo.GetAbsorb();

        if (m_spellInfo->Id == 243223) ///< Helya's Scorn
            m_caster->HealBySpell(m_caster, m_spellInfo, uint32(m_final_damage), p_Target->crit);

        l_Caster->SendSpellNonMeleeDamageLog(l_DamageInfo);
    }
    // Passive spell hits/misses or active spells only misses (only triggers)
    if (l_AllFalse)
    {
        CallScriptOnHitHandlers();

        // Fill base damage struct (unitTarget - is real spell target)
        SpellNonMeleeDamage l_DamageInfo(l_Caster, unitTarget, m_damage, this, DamageEffectType::SPELL_DIRECT_DAMAGE, m_attackType);
        l_DamageInfo.ChangeSchoolMask(m_spellSchoolMask);

        l_ProcEx |= createProcExtendMask(l_DamageInfo, l_MissInfo);
        // Do triggers for unit (reflect triggers passed on hit phase for correct drop charge)
        if (l_CanEffectTrigger && l_MissInfo != SPELL_MISS_REFLECT)
            l_Caster->ProcDamageAndSpell(l_Unit, l_ProcAttacker, l_ProcVictim, l_ProcEx, l_DamageInfo, m_triggeredByAuraSpell);

        // Failed Pickpocket, reveal rogue
        if (l_MissInfo == SPELL_MISS_RESIST && AttributesCustomCu & SPELL_ATTR0_CU_PICKPOCKET && unitTarget->GetTypeId() == TYPEID_UNIT)
        {
            m_caster->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_TALK);
            if (unitTarget->ToCreature()->IsAIEnabled)
                unitTarget->ToCreature()->AI()->AttackStart(m_caster);
        }
    }

    /// Any proc spell shouldn't put the target in combat, only direct hit by auto or spell should.
    if ((!m_triggeredByAuraSpell || m_spellInfo->Id == 148187) && !m_spellInfo->IsIgnoringCombat() && l_MissInfo != SPELL_MISS_EVADE && (!m_spellInfo->IsPositive() || m_spellInfo->HasEffect(SPELL_EFFECT_DISPEL)) && m_caster->_IsValidAttackTarget(l_Unit, m_spellInfo, 0, true))
    {
        bool l_ShouldAttack = true;
        if (Creature* l_Creature = l_Unit->ToCreature())
        {
            uint32 l_CreatureType = l_Creature->GetCreatureTemplate()->type;
            if (l_CreatureType == CREATURE_TYPE_CRITTER || l_CreatureType == CREATURE_TYPE_WILD_PET)
                l_ShouldAttack = false;
        }

        if (l_ShouldAttack)
        {

            if (AttributesCustomCu & SPELL_ATTR0_CU_AURA_CC)
                if (!l_Unit->IsStandState())
                    l_Unit->SetStandState(UNIT_STAND_STATE_STAND);

            ///< Cocodev: This should fix an issue where creatures although would have 
            ///< their state set to REACT_PASSIVE would still enter combat, following players
            ///< not 100% sure if this should applied to AIEnabled creatures only
            if (Creature* l_Creature = m_caster->ToCreature())
            {
                if (l_Creature->HasReactState(ReactStates::REACT_PASSIVE) || (l_Creature->IsAIEnabled && l_Creature->AI()->IsPassived()))
                    l_ShouldAttack = false;
            }
        }

        if (l_ShouldAttack)
        {
            m_caster->CombatStart(l_Unit, !(AttributesCustomEx3 & SPELL_ATTR3_NO_INITIAL_AGGRO || AttributesCustomEx & SPELL_ATTR1_NO_THREAT));
        }
    }

    if (l_SpellHitTarget)
    {
        //AI functions
        if (l_SpellHitTarget->GetTypeId() == TYPEID_UNIT)
        {
            if (l_SpellHitTarget->ToCreature()->IsAIEnabled)
                l_SpellHitTarget->ToCreature()->AI()->SpellHit(m_caster, m_spellInfo);

            // cast at creature (or GO) quest objectives update at successful cast finished (+channel finished)
            // ignore pets or autorepeat/melee casts for speed (not exist quest for spells (hm...)
            if (m_originalCaster && m_originalCaster->IsControlledByPlayer() && !l_SpellHitTarget->ToCreature()->isPet() && !IsAutoRepeat() && !IsNextMeleeSwingSpell() && !IsChannelActive())
                if (Player* l_Player = m_originalCaster->GetCharmerOrOwnerPlayerOrPlayerItself())
                    l_Player->CastedCreatureOrGO(l_SpellHitTarget->GetEntry(), l_SpellHitTarget->GetGUID(), m_spellInfo->Id);
        }

        if (m_caster->GetTypeId() == TYPEID_UNIT && m_caster->ToCreature()->IsAIEnabled)
            m_caster->ToCreature()->AI()->SpellHitTarget(l_SpellHitTarget, m_spellInfo);

        // Needs to be called after dealing damage/healing to not remove breaking on damage auras
        DoTriggersOnSpellHit(l_SpellHitTarget, l_Mask);

        /// DoTriggersOnSpellHit may corrupt the pointers
        UpdatePointers();

        Player* l_PlayerCaster =
            m_originalCaster && m_originalCaster->IsPlayer() ? m_originalCaster->ToPlayer() :
            m_caster->IsPlayer() ? m_caster->ToPlayer() : nullptr;
        bool l_IsNonPvp = GetSpellInfo()->IsNonPvP(l_PlayerCaster, l_Unit);

        // if target is fallged for pvp also flag caster if a player
        if (l_PlayerCaster != nullptr && l_Unit->IsPvP() && !l_IsNonPvp)
            l_PlayerCaster->UpdatePvP(true);

        CallScriptAfterHitHandlers();
    }
}

SpellMissInfo Spell::DoSpellHitOnUnit(Unit* unit, uint32 effectMask, bool scaleAura)
{
    if (!unit || !effectMask)
        return SPELL_MISS_EVADE;

    MeleeHitOutcome l_HitResult = MELEE_HIT_NORMAL;
    SpellMissInfo l_SpellResult = SPELL_MISS_NONE;
    if (unit->ToCreature() && unit->IsAIEnabled)
        unit->ToCreature()->GetAI()->CheckHitResult(l_HitResult, l_SpellResult, m_caster, m_spellInfo);

    if (l_SpellResult != SPELL_MISS_NONE)
        return l_SpellResult;

    // For delayed spells immunity may be applied between missile launch and hit - check immunity for that case
    if (m_spellInfo->Speed && (unit->IsImmunedToDamage(m_spellInfo) || unit->IsImmunedToSpell(m_spellInfo)))
        return SPELL_MISS_IMMUNE;

    // Hack fix for Deterrence and charge stun effect
    if ((m_spellInfo->Id == 105771 || m_spellInfo->Id == 7922) && unit->HasAura(19263))
        return SPELL_MISS_MISS;

    /// Hack fix for Cloak of Shadows
    if (unit->HasAura(31224) && !m_spellInfo->CanPierceCloakOfShadows(m_caster, unit, IsTriggered()))
    {
        if (!(AttributesCustomEx8 & SPELL_ATTR8_CANT_MISS))
            return SPELL_MISS_MISS;
    }

    // disable effects to which unit is immune
    SpellMissInfo returnVal = SPELL_MISS_IMMUNE;
    for (uint32 effectNumber = 0; effectNumber < m_spellInfo->EffectCount; ++effectNumber)
        if (effectMask & (1 << effectNumber))
            if (unit->IsImmunedToSpellEffect(m_spellInfo, effectNumber))
                effectMask &= ~(1 << effectNumber);

    if (!effectMask)
        return returnVal;

    PrepareScriptHitHandlers();
    CallScriptBeforeHitHandlers();

    LinkedSpell(unit, unit, SPELL_LINK_BEFORE_HIT, effectMask);

    Player* l_Player = unit->IsPlayer() ? unit->ToPlayer() : nullptr;
    if (l_Player && l_Player->GetSession() && !l_Player->GetSession()->PlayerLoading())
    {
        if (m_spellInfo->CanUpdateCriteriaTimedSpellCaster)
            l_Player->StartCriteriaTimer(CRITERIA_TIMED_TYPE_SPELL_TARGET, m_spellInfo->Id);
        if (m_spellInfo->CanUpdateCriteriaBeSpellTarget)
            l_Player->UpdateCriteria(CRITERIA_TYPE_BE_SPELL_TARGET, m_spellInfo->Id, 0, 0, m_caster);
        if (m_spellInfo->CanUpdateCriteriaBeSpellTarget2)
            l_Player->UpdateCriteria(CRITERIA_TYPE_BE_SPELL_TARGET2, m_spellInfo->Id, 0, 0, m_caster);

        // Update scenario/challenge criterias
        Map* map = l_Player->GetMap();
        if (map)
        {
            if (Scenario* progress = sScenarioMgr->GetScenario(map->GetScenarioGuid()))
            {
                if (m_spellInfo->CanUpdateCriteriaBeSpellTarget)
                    progress->UpdateAchievementCriteria(CRITERIA_TYPE_BE_SPELL_TARGET, m_spellInfo->Id, 0, 0, m_caster, unit->ToPlayer());
                if (m_spellInfo->CanUpdateCriteriaBeSpellTarget2)
                    progress->UpdateAchievementCriteria(CRITERIA_TYPE_BE_SPELL_TARGET, m_spellInfo->Id, 0, 0, nullptr, unit->ToPlayer());
            }
        }
    }

    if (m_caster->IsPlayer())
    {
        if (m_spellInfo->CanUpdateCriteriaTimedSpellTarget)
            m_caster->ToPlayer()->StartCriteriaTimer(CRITERIA_TIMED_TYPE_SPELL_CASTER, m_spellInfo->Id);
        if (m_spellInfo->CanUpdateCriteriaCastSpell2)
            m_caster->ToPlayer()->UpdateCriteria(CRITERIA_TYPE_CAST_SPELL2, m_spellInfo->Id, 0, 0, unit);
    }

    if (m_caster != unit)
    {
        // Recheck  UNIT_FLAG_NON_ATTACKABLE for delayed spells
        if (m_spellInfo->Speed > 0.0f && unit->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE) && unit->GetCharmerOrOwnerGUID() != m_caster->GetGUID())
            return SPELL_MISS_EVADE;

        if (m_caster->_IsValidAttackTarget(unit, m_spellInfo, 0 , true))
        {
            if (m_spellInfo->IsBreakingStealth(m_caster))
            {
                unit->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_HITBYSPELL);
                //TODO: This is a hack. But we do not know what types of stealth should be interrupted by CC
                if (AttributesCustomCu & SPELL_ATTR0_CU_AURA_CC && unit->IsControlledByPlayer())
                    unit->RemoveAurasByType(SPELL_AURA_MOD_STEALTH, 0, NULL, 131361);
            }
            if (AttributesCustomCu & SPELL_ATTR0_CU_BINARY && !m_spellInfo->IsChanneled())
                if (m_originalCaster && m_originalCaster->IsSpellResisted(unit, m_spellSchoolMask, m_spellInfo))
                    return SPELL_MISS_RESIST;
        }
        else if (m_caster->IsFriendlyTo(unit))
        {
            // for delayed spells ignore negative spells (after duel end) for friendly targets
            // TODO: this cause soul transfer bugged
            /// Handle custom flag SPELL_ATTR0_CU_CAN_BE_CASTED_ON_ALLIES, some spells are negative but can be casted on allies
            if (m_spellInfo->Speed > 0.0f && unit->IsPlayer() && !m_spellInfo->IsPositive() && !(AttributesCustomCu & SpellCustomAttributes::SPELL_ATTR0_CU_CAN_BE_CASTED_ON_ALLIES))
                return SPELL_MISS_EVADE;

            // assisting case, healing and resurrection
            if (unit->HasUnitState(UNIT_STATE_ATTACK_PLAYER))
            {
                m_caster->SetContestedPvP();
                if (m_caster->IsPlayer())
                    m_caster->ToPlayer()->UpdatePvP(true);
            }
            if (unit->isInCombat() && !(AttributesCustomEx3 & SPELL_ATTR3_NO_INITIAL_AGGRO) && !m_spellInfo->IsIgnoringCombat())
            {
                m_caster->SetInCombatState(unit->GetCombatTimer() > 0, unit);
                unit->getHostileRefManager().threatAssist(m_caster, 0.0f);
            }
        }
    }
    else if (!m_spellInfo->IsPositive())
    {
        if (AttributesCustomCu & SPELL_ATTR0_CU_BINARY && !m_spellInfo->IsChanneled())
            if (m_originalCaster && m_originalCaster->IsSpellResisted(unit, m_spellSchoolMask, m_spellInfo))
                return SPELL_MISS_RESIST;
    }

    // Get Data Needed for Diminishing Returns, some effects may have multiple auras, so this must be done on spell hit, not aura add
    m_diminishGroup = GetDiminishingReturnsGroupForSpell(m_spellInfo, m_originalCaster);
    ///< not need check because prevent bug with Reflect
    if (m_diminishGroup /*&& m_originalCaster != unit && m_caster != unit*/)
    {
        if (GetCaster()->GetSpellModOwner())
        {
            m_diminishLevel = unit->GetDiminishing(m_diminishGroup);
            DiminishingReturnsType type = GetDiminishingReturnsGroupType(m_diminishGroup);
            // Increase Diminishing on unit, current informations for actually casts will use values above
            if ((type == DRTYPE_PLAYER &&
                (unit->GetCharmerOrOwnerPlayerOrPlayerItself() || (unit->GetTypeId() == TYPEID_UNIT && unit->ToCreature()->GetFlagsExtra() & CREATURE_FLAG_EXTRA_ALL_DIMINISH))) ||
                type == DRTYPE_ALL)
            {
                if (unit != GetCaster()) ///< Some spells CC the target, but also gives some kind of buff to the caster, so triggers DR on caster aswell, leading to UBs.
                    unit->IncrDiminishing(m_diminishGroup);

                /// Frostjaw triggers both Root and Silence DR
                if (m_spellInfo->Id == 102051)
                    unit->IncrDiminishing(DIMINISHING_SILENCE);

                /// Hack Fix Wod - Hunter WoD PvP Beast Mastery 4P Bonus
                /// Pet and owner are sharing same diminishing return
                if (unit->isPet() && unit->GetOwner() && unit->GetOwner()->HasAura(171478))
                    unit->GetOwner()->IncrDiminishing(m_diminishGroup);
                else if (unit->HasAura(171478) && unit->IsPlayer() && unit->ToPlayer()->GetPet())
                    unit->ToPlayer()->GetPet()->IncrDiminishing(m_diminishGroup);
            }
        }
    }

    uint32 aura_effmask = 0;
    for (uint8 i = 0; i < m_spellInfo->EffectCount; ++i)
        if (effectMask & (1 << i) && m_spellInfo->Effects[i].IsUnitOwnedAuraEffect())
            aura_effmask |= 1 << i;

    if (aura_effmask)
    {
        // Select rank for aura with level requirements only in specific cases
        // Unit has to be target only of aura effect, both caster and target have to be players, target has to be other than unit target
        SpellInfo const* aurSpellInfo = m_spellInfo;
        int32 basePoints[32];
        if (scaleAura)
        {
            if ((aurSpellInfo = m_spellInfo->GetAuraRankForLevel(unitTarget->getLevelForTarget(m_caster))))
            {
                for (uint8 i = 0; i < m_spellInfo->EffectCount; ++i)
                {
                    basePoints[i] = aurSpellInfo->Effects[i].BasePoints;
                    if (m_spellInfo->Effects[i].Effect != aurSpellInfo->Effects[i].Effect)
                    {
                        aurSpellInfo = m_spellInfo;
                        break;
                    }
                }
            }

            ASSERT(aurSpellInfo);
        }

        if (m_originalCaster && aurSpellInfo)
        {
            bool refresh = false;
            m_spellAura = Aura::TryRefreshStackOrCreate(aurSpellInfo, effectMask, unit,
                m_originalCaster, (aurSpellInfo == m_spellInfo)? &m_spellValue->EffectBasePoints[0] : &basePoints[0], m_CastItem, 0, &refresh, m_castItemLevel);
            if (m_spellAura)
            {
                // Set aura stack amount to desired value
                if (m_spellValue->AuraStackAmount > 1)
                {
                    if (!refresh)
                        m_spellAura->SetStackAmount(m_spellValue->AuraStackAmount);
                    else
                        m_spellAura->ModStackAmount(m_spellValue->AuraStackAmount);
                }

                /// Refresh applying time if needed, used for CC breaking with damage, should break only on DoTs applied after CC
                if (refresh)
                    m_spellAura->SetApplyTime(time(nullptr));

                // Now Reduce spell duration using data received at spell hit
                int32 duration = m_spellAura->GetMaxDuration();
                int32 limitduration = GetDiminishingReturnsLimitDuration(aurSpellInfo, m_originalCaster);
                float diminishMod = limitduration ? unit->ApplyDiminishingToDuration(m_diminishGroup, duration, m_originalCaster, m_diminishLevel, limitduration) : 1.0f;

                // unit is immune to aura if it was diminished to 0 duration
                if (diminishMod == 0.0f)
                {
                    m_spellAura->Remove();
                    bool found = false;
                    for (uint8 i = 0; i < m_spellInfo->EffectCount; ++i)
                    {
                        if (effectMask & (1 << i) && !m_spellInfo->Effects[i].IsApplyingAura())
                            found = true;
                    }

                    if (!found)
                        return SPELL_MISS_IMMUNE;
                }
                else
                {
                    UnitAura* unitAura = (UnitAura*)m_spellAura;
                    unitAura->SetDiminishGroup(m_diminishGroup);

                    bool positive = m_spellAura->GetSpellInfo()->IsPositive();
                    if (AuraApplication* aurApp = m_spellAura->GetApplicationOfTarget(m_originalCaster->GetGUID()))
                        positive = aurApp->IsPositive();

                    duration = m_originalCaster->ModSpellDuration(aurSpellInfo, unit, duration, positive, effectMask);

                    if (duration > 0)
                    {
                        if (m_spellInfo->Id != 211261) ///< Hackfix for Elisande (Permeliative Torment) Duration should remain unchanged no matter the Time Rate
                            duration = int32(duration * m_originalCaster->GetFloatValue(UNIT_FIELD_MOD_TIME_RATE));

                        if (!aurSpellInfo->IsPositive())
                        {
                            std::list<AuraEffect*> l_AuraEffectList = unit->GetAuraEffectsByType(AuraType::SPELL_AURA_MOD_DEBUFF_SPEED);
                            float l_Speed = 1.0f;
                            for (AuraEffect const* l_AuraEffect : l_AuraEffectList)
                                AddPct(l_Speed, l_AuraEffect->GetAmount());

                            duration /= l_Speed;
                        }

                        // Haste modifies duration of channeled spells
                        if (m_spellInfo->IsChanneled())
                            m_originalCaster->ModSpellCastTime(aurSpellInfo, duration, this);
                        /// If this aura not affected by new wod aura system, we should change amplitude according to amount of haste / NOW CALLED PANDEMIC 7.1.5
                        else if (!m_spellInfo->IsPandemicAffected() && !m_spellInfo->HasAttribute(SPELL_ATTR8_HASTE_AFFECT_DURATION_RECOVERY) && (m_spellInfo->HasAttribute(SpellAttr5::SPELL_ATTR5_HASTE_AFFECT_DURATION) || m_spellInfo->HasAttribute(SpellAttr8::SPELL_ATTR8_HASTE_AFFECT_DURATION)))
                        {
                            int32 l_OriginalDuration = duration;
                            float l_HastePct = std::min(1.0f, m_originalCaster->GetHastePct());
                            duration = 0;
                            for (uint8 l_I = 0; l_I < GetSpellInfo()->EffectCount; ++l_I)
                            {
                                if (AuraEffect const* l_Effect = m_spellAura->GetEffect(l_I))
                                {
                                    if (int32 l_Amplitude = l_Effect->GetAmplitude())  // amplitude is hastened by UNIT_FIELD_MOD_CASTING_SPEED
                                        duration = int32(l_OriginalDuration * l_HastePct);
                                }
                            }

                            // if there is no periodic effect
                            if (!duration)
                                duration = int32(l_OriginalDuration * l_HastePct);
                        }
                    }

                    if (duration != m_spellAura->GetMaxDuration())
                    {
                        bool periodicDamage = false;
                        for (uint8 i = 0; i < m_spellInfo->EffectCount; ++i)
                            if (m_spellAura->GetEffect(i))
                                if (m_spellAura->GetEffect(i)->GetAuraType() == SPELL_AURA_PERIODIC_DAMAGE || m_spellAura->GetEffect(i)->GetAuraType() == SPELL_AURA_PERIODIC_SCHOOL_DAMAGE)
                                    periodicDamage = true; ///< periodicDamage is never read 01/18/16
                    }

                    if (duration != m_spellAura->GetMaxDuration())
                    {
                        m_spellAura->SetMaxDuration(duration);
                        m_spellAura->SetDuration(duration);
                    }

                    if (isStolen && (!duration || duration == -1 || duration > 60000))
                    {
                        duration = 60000;
                        m_spellAura->SetMaxDuration(duration);
                        m_spellAura->SetDuration(duration);
                    }

                    m_spellAura->_RegisterForTargets();
                }
            }
        }
    }

    for (uint32 effectNumber = 0; effectNumber < m_spellInfo->EffectCount; ++effectNumber)
        if (effectMask & (1 << effectNumber))
            HandleEffects(unit, nullptr, nullptr, nullptr, effectNumber, SPELL_EFFECT_HANDLE_HIT_TARGET);

    if (m_spellAura)
    {
        if (m_spellInfo->HasEffect(SPELL_EFFECT_APPLY_AURA) || m_spellInfo->HasEffect(SPELL_EFFECT_APPLY_AURA_2))
        {
            Unit::AuraApplicationList l_AuraApplicationList;
            m_spellAura->GetApplicationList(l_AuraApplicationList);
            for (AuraApplication* l_AuraApplication : l_AuraApplicationList)
                l_AuraApplication->ClientUpdate();
        }
    }

    return SPELL_MISS_NONE;
}

void Spell::DoTriggersOnSpellHit(Unit* unit, uint32 effMask)
{
    // Apply additional spell effects to target
    // TODO: move this code to scripts
    // Blizz seems to just apply aura without bothering to cast
    if (m_preCastSpell)
        if (!m_spellInfo->IsWrongPrecastSpell(sSpellMgr->GetSpellInfo(m_preCastSpell)))
            if (sSpellMgr->GetSpellInfo(m_preCastSpell))
                m_caster->AddAura(m_preCastSpell, unit);

    // handle SPELL_AURA_ADD_TARGET_TRIGGER auras
    // this is executed after spell proc spells on target hit
    // spells are triggered for each hit spell target
    // info confirmed with retail sniffs of permafrost and shadow weaving
    if (!m_hitTriggerSpells.empty())
    {
        int _duration = 0;
        for (HitTriggerSpellList::const_iterator i = m_hitTriggerSpells.begin(); i != m_hitTriggerSpells.end(); ++i)
        {
            if (CanExecuteTriggersOnHit(effMask, i->triggeredByAura) && roll_chance_i(i->chance))
            {
                m_caster->CastSpell(unit, i->triggeredSpell, true);

                // SPELL_AURA_ADD_TARGET_TRIGGER auras shouldn't trigger auras without duration
                // set duration of current aura to the triggered spell
                if (i->triggeredSpell->GetDuration() == -1)
                {
                    Aura* triggeredAur = unit->GetAura(i->triggeredSpell->Id, m_caster->GetGUID());
                    if (triggeredAur != nullptr)
                    {
                        // get duration from aura-only once
                        if (!_duration)
                        {
                            Aura* aur = unit->GetAura(m_spellInfo->Id, m_caster->GetGUID());
                            _duration = aur ? aur->GetDuration() : -1;
                        }
                        triggeredAur->SetDuration(_duration);
                    }
                }
            }
        }
    }

    // trigger linked auras remove/apply
    // TODO: remove/cleanup this, as this table is not documented and people are doing stupid things with it
    LinkedSpell(unit, unit, SPELL_LINK_ON_HIT, effMask);
}

void Spell::LinkedSpell(Unit* _caster, Unit* _target, SpellLinkedType type, uint32 effectMask)
{
    if (const std::vector<SpellLinked> *spell_triggered = sSpellMgr->GetSpellLinked(m_spellInfo->Id + type))
    {
        std::set<uint32> spellBanList;
        std::set<uint32> groupLock;
        for (std::vector<SpellLinked>::const_iterator i = spell_triggered->begin(); i != spell_triggered->end(); ++i)
        {
            if (i->hitmask)
            {
                if (!(m_spellMissMask & i->hitmask))
                    continue;
            }

            if (i->effectMask)
            {
                if (!(effectMask & i->effectMask))
                    continue;
            }

            if (!spellBanList.empty())
            {
                std::set<uint32>::iterator itr = spellBanList.find(abs(i->effect));
                if (itr != spellBanList.end())
                    continue;
            }

            if (i->group != 0 && !groupLock.empty())
            {
                std::set<uint32>::iterator itr = groupLock.find(i->group);
                if (itr != groupLock.end())
                    continue;
            }

            if (!_target)
                _target = m_targets.GetUnitTarget();
            _caster = m_caster;

            if (i->target)
                _target = (m_originalCaster ? m_originalCaster : m_caster)->GetUnitForLinkedSpell(_caster, _target, i->target);

            if (i->caster)
                _caster = (m_originalCaster ? m_originalCaster : m_caster)->GetUnitForLinkedSpell(_caster, _target, i->caster);

            if (!_caster)
                continue;

            if (i->targetCount != -1)
            {
                switch (i->targetCountType)
                {
                    case LINK_TARGET_DEFAULT:
                        if (GetTargetCount() < i->targetCount)
                            continue;
                        break;
                    case LINK_TARGET_FROM_EFFECT:
                        if (GetEffectTargets().size() < i->targetCount)
                            continue;
                        break;
                    default:
                        break;
                }
            }

            if (i->hastalent)
                if (m_caster->HasAuraLinkedSpell(m_caster, _target, i->hastype, i->hastalent, i->hasparam))
                    continue;

            if (i->hastalent2)
                if (m_caster->HasAuraLinkedSpell(m_caster, _target, i->hastype2, i->hastalent2, i->hasparam2))
                    continue;

            if (i->effect < 0)
            {
                switch (i->actiontype)
                {
                    case LINK_ACTION_DEFAULT:
                        _caster->RemoveAurasDueToSpell(abs(i->effect));
                        break;
                    case LINK_ACTION_LEARN:
                        if (Player* _lplayer = _caster->ToPlayer())
                            _lplayer->removeSpell(abs(i->effect));
                        break;
                    case LINK_ACTION_AURATYPE:
                        if (_target)
                            _target->RemoveAurasByType(AuraType(i->hastalent2));
                        break;
                    case LINK_ACTION_CHANGE_STACK:
                        if (Aura* aura = (_target ? _target : _caster)->GetAura(abs(i->effect)))
                            aura->ModStackAmount(i->param ? -int32(i->param) : -1);
                        break;
                    case LINK_ACTION_CHANGE_CHARGES:
                        if (Aura* aura = (_target ? _target : _caster)->GetAura(abs(i->effect)))
                            aura->ModCharges(i->param ? -int32(i->param) : -1);
                        break;
                    default:
                        break;
                }
            }
            else
            {
                if (i->chance != 0)
                {
                    if (i->chance > 100)
                    {
                        if (_caster->GetTypeId() == TYPEID_PLAYER)
                        {
                            int32 _chance = _caster->ToPlayer()->GetFloatValue(PLAYER_FIELD_MASTERY) * int32(i->param);
                            if (!roll_chance_i(_chance))
                                continue;
                        }
                        else
                            continue;
                    }
                    else if (!roll_chance_i(i->chance))
                        continue;
                }

                if (i->cooldown != 0 && _caster->GetTypeId() == TYPEID_PLAYER && _caster->ToPlayer()->HasSpellCooldown(i->effect))
                    continue;

                switch (i->actiontype)
                {
                    case LINK_ACTION_DEFAULT: //0
                        _caster->CastSpell(_target ? _target : _caster, i->effect, true);
                        spellBanList.insert(i->effect); // Triggered once for a cycle
                        break;
                    case LINK_ACTION_LEARN: //1
                        if (Player* _lplayer = _caster->ToPlayer())
                            _lplayer->learnSpell(i->effect, false);
                        break;
                    case LINK_ACTION_SEND_COOLDOWN: // 3
                        _caster->SendSpellCooldown(i->effect, m_spellInfo->Id);
                        break;
                    case LINK_ACTION_CAST_NO_TRIGGER: //4
                        _caster->CastSpell(_target ? _target : _caster, i->effect, false);
                        break;
                    case LINK_ACTION_ADD_AURA: //5
                        _caster->AddAura(i->effect, _target ? _target : _caster);
                        break;
                    case LINK_ACTION_CHANGE_STACK: //7
                        if (Aura* aura = (_target ? _target : _caster)->GetAura(i->effect))
                            aura->ModStackAmount(i->param ? int32(i->param) : 1);
                        else if (i->param)
                        {
                            for (uint8 count = 0; count < uint8(i->param); ++count)
                                _caster->CastSpell(_target ? _target : _caster, i->effect, true);
                        }
                        else
                            _caster->CastSpell(_target ? _target : _caster, i->effect, true);
                        break;
                    case LINK_ACTION_REMOVE_COOLDOWN: //8
                        if (Player* _lplayer = _caster->ToPlayer())
                            _lplayer->RemoveSpellCooldown(i->effect, true);
                        break;
                    case LINK_ACTION_REMOVE_MOVEMENT: //9
                        if (i->param)
                            (_target ? _target : _caster)->RemoveAurasWithMechanic(uint64(i->param));
                        else
                            (_target ? _target : _caster)->RemoveMovementImpairingAuras();
                        break;
                    case LINK_ACTION_CHANGE_DURATION: //10
                        if (Aura* aura = (_target ? _target : _caster)->GetAura(i->effect, _caster->GetGUID()))
                        {
                            if (!int32(i->param))
                                aura->RefreshTimers();
                            else
                            {
                                int32 _duration = int32(aura->GetDuration() + int32(i->param));
                                if (i->duration)
                                {
                                    if (_duration < i->duration)
                                        aura->SetDuration(_duration);
                                    else
                                        aura->SetDuration(i->duration);
                                }
                                else if (_duration < aura->GetMaxDuration())
                                    aura->SetDuration(_duration);
                            }
                        }
                        else
                            _caster->CastSpell(_target ? _target : _caster, i->effect, true);
                        break;
                    case LINK_ACTION_CAST_DEST: //11
                        if (m_targets.HasDst())
                        {
                            CustomSpellValues values;
                            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(i->effect);
                            TriggerCastData triggerData;
                            triggerData.replaced = true;
                            triggerData.originalCaster = m_originalCasterGUID;
                            triggerData.castItem = m_CastItem;
                            triggerData.triggeredByAura = m_triggeredByAura;
                            triggerData.spellGuid = m_caster->GetGUID();
                            triggerData.SubType = SPELL_CAST_TYPE_NORMAL;
                            triggerData.delay = i->duration;

                            _caster->CastSpell(m_targets, spellInfo, &values, triggerData);
                        }
                        break;
                    case LINK_ACTION_CHANGE_CHARGES: //12
                        if (Aura* aura = (_target ? _target : _caster)->GetAura(i->effect))
                            aura->ModCharges(i->param ? int32(i->param) : 1);
                        else
                            _caster->CastSpell(_target ? _target : _caster, i->effect, true);
                        break;
                    case LINK_ACTION_MODIFY_COOLDOWN: //13
                        if (_caster->GetTypeId() == TYPEID_PLAYER)
                            _caster->ToPlayer()->ModifySpellCooldown(i->effect, int32(i->param));
                        break;
                    case LINK_ACTION_CATEGORY_COOLDOWN: //14
                        if (_caster->GetTypeId() == TYPEID_PLAYER)
                            _caster->ToPlayer()->ModSpellChargeCooldown(i->effect, int32(i->param));
                        break;
                    case LINK_ACTION_CATEGORY_CHARGES: //15
                        if (_caster->GetTypeId() == TYPEID_PLAYER)
                            _caster->ToPlayer()->ModSpellCharge(i->effect, int32(i->param));
                        break;
                    case LINK_ACTION_RECALCULATE_AMOUNT: //16
                        if (Aura* aura = _caster->GetAura(i->effect))
                            aura->RecalculateAmountOfEffects(true);
                        break;
                    case LINK_ACTION_CAST_COUNT: //17
                    {
                        if (i->duration)
                        {
                            for (uint8 count = 0; count < uint8(i->param); ++count)
                            {
                                if (uint32 _delay = i->duration * count)
                                    _caster->m_Events.AddEvent(new DelayCastEvent(*_caster, (_target ? _target : _caster)->GetGUID(), i->effect, true), _caster->m_Events.CalculateTime(_delay));
                                else
                                    _caster->CastSpell(_target ? _target : _caster, i->effect, true);
                            }
                        }
                        else
                            for (uint8 count = 0; count < uint8(i->param); ++count)
                                _caster->CastSpell(_target ? _target : _caster, i->effect, true);

                        spellBanList.insert(i->effect); // Triggered once for a cycle
                        break;
                    }
                    case LINK_ACTION_FULL_TRIGGER: //18
                    {
                        CustomSpellValues values;
                        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(i->effect);
                        TriggerCastData triggerData;
                        triggerData.triggerFlags = TRIGGERED_FULL_MASK;
                        triggerData.spellGuid = (m_TriggerCastData.SubType == SPELL_CAST_TYPE_NORMAL) ? m_SpellGuid : m_caster->GetGUID();
                        triggerData.originalCaster = m_originalCasterGUID;
                        triggerData.skipCheck = true;
                        triggerData.casttime = m_casttime;
                        triggerData.castItem = m_CastItem;
                        triggerData.triggeredByAura = m_triggeredByAura;
                        triggerData.parentTargetCount = GetTargetCount();
                        // triggerData.parentTargets = m_UniqueTargetInfo; // Need convert to guid list
                        triggerData.delay = i->duration;

                        SpellCastTargets targets;
                        targets.SetCaster(_caster);
                        targets.SetUnitTarget(_target ? _target : _caster);
                        _caster->CastSpell(targets, spellInfo, &values, triggerData);
                        break;
                    }
                    default:
                        break;
                }

                if (i->group != 0)
                    groupLock.insert(i->group);

                if (i->cooldown != 0 && _caster->GetTypeId() == TYPEID_PLAYER)
                    _caster->ToPlayer()->AddSpellCooldown(i->effect, 0, i->cooldown);
            }
        }
    }
}

void Spell::DoAllEffectOnTarget(GOTargetInfo* target)
{
    if (target->processed)                                  // Check target
        return;
    target->processed = true;                               // Target checked in apply effects procedure

    uint32 effectMask = target->effectMask;
    if (!effectMask)
        return;

    GameObject* go = m_caster->GetMap()->GetGameObject(target->targetGUID);
    if (!go)
        return;

    PrepareScriptHitHandlers();
    CallScriptBeforeHitHandlers();

    for (uint32 effectNumber = 0; effectNumber < m_spellInfo->EffectCount; ++effectNumber)
        if (effectMask & (1 << effectNumber))
            HandleEffects(nullptr, nullptr, go, nullptr, effectNumber, SPELL_EFFECT_HANDLE_HIT_TARGET);

    CallScriptOnHitHandlers();

    if (go->AI())
        go->AI()->SpellHit(m_caster, m_spellInfo);

    // cast at creature (or GO) quest objectives update at successful cast finished (+channel finished)
    // ignore autorepeat/melee casts for speed (not exist quest for spells (hm...)
    if (m_originalCaster && m_originalCaster->IsControlledByPlayer() && !IsAutoRepeat() && !IsNextMeleeSwingSpell() && !IsChannelActive())
        if (Player* p = m_originalCaster->GetCharmerOrOwnerPlayerOrPlayerItself())
            p->CastedCreatureOrGO(go->GetEntry(), go->GetGUID(), m_spellInfo->Id);
    CallScriptAfterHitHandlers();
}

void Spell::DoAllEffectOnTarget(ItemTargetInfo* target)
{
    uint32 effectMask = target->effectMask;
    if (!target->item || !effectMask)
        return;

    PrepareScriptHitHandlers();
    CallScriptBeforeHitHandlers();

    for (uint32 effectNumber = 0; effectNumber < m_spellInfo->EffectCount; ++effectNumber)
        if (effectMask & (1 << effectNumber))
            HandleEffects(nullptr, target->item, nullptr, nullptr, effectNumber, SPELL_EFFECT_HANDLE_HIT_TARGET);

    m_missInfo = SPELL_MISS_NONE;

    CallScriptOnHitHandlers();

    CallScriptAfterHitHandlers();
}

void Spell::DoAllEffectOnTarget(AreaTriggerTargetInfo* p_Target)
{
    uint32 effectMask = p_Target->effectMask;
    if (!p_Target->targetGUID || !effectMask)
        return;

    AreaTrigger* l_AreaTrigger = sObjectAccessor->GetAreaTrigger(*m_caster, p_Target->targetGUID);
    if (l_AreaTrigger == nullptr)
        return;

    PrepareScriptHitHandlers();
    CallScriptBeforeHitHandlers();

    for (uint32 effectNumber = 0; effectNumber < m_spellInfo->EffectCount; ++effectNumber)
    {
        if (effectMask & (1 << effectNumber))
            HandleEffects(nullptr, nullptr, nullptr, l_AreaTrigger, effectNumber, SPELL_EFFECT_HANDLE_HIT_TARGET);
    }

    m_missInfo = SPELL_MISS_NONE;
    CallScriptOnHitHandlers();
    CallScriptAfterHitHandlers();
}

bool Spell::UpdateChanneledTargetList()
{
    if (m_spellInfo->HasAttribute(SPELL_ATTR6_UNK19) && m_spellInfo->IsChanneled())
        return true;

    /// Automatically forces player to face target
    if ((m_spellInfo->HasAttribute(SPELL_ATTR1_CHANNEL_TRACK_TARGET)) && !m_caster->HasInArc(static_cast<float>(M_PI), m_targets.GetUnitTarget()))
    {
        if (m_targets.GetUnitTarget())
            m_caster->SetInFront(m_targets.GetUnitTarget());
    }

    // Not need check return true
    if (m_channelTargetEffectMask == 0)
        return true;

    uint32 channelTargetEffectMask = m_channelTargetEffectMask;
    uint32 channelAuraMask = 0;
    for (uint8 i = 0; i < m_spellInfo->EffectCount; ++i)
    {
        if (m_spellInfo->Effects[i].IsApplyingAura())
            channelAuraMask |= 1 << i;
    }

    channelAuraMask &= channelTargetEffectMask;

    float range = 0;
    if (channelAuraMask)
    {
        range = m_spellInfo->GetMaxRange(m_spellInfo->IsPositive());
        if (Player* modOwner = m_caster->GetSpellModOwner())
            modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_RANGE, range, this);

        /// Hackfix for Jade Serpent Statue, should have 50% more range for Soothing Mists before it breaks
        if (m_caster->isStatue() && m_spellInfo->Id == 115175)
            AddPct(range, 50.0f);
    }

    for (std::list<TargetInfo>::iterator ihit= m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
    {
        if (ihit->missCondition == SPELL_MISS_NONE && (channelTargetEffectMask & ihit->effectMask))
        {
            Unit* unit = m_caster->GetGUID() == ihit->targetGUID ? m_caster : ObjectAccessor::GetUnit(*m_caster, ihit->targetGUID);

            if (!unit)
                continue;

            if (IsValidDeadOrAliveTarget(unit))
            {
                if (channelAuraMask & ihit->effectMask)
                {
                    if (AuraApplication * aurApp = unit->GetAuraApplication(m_spellInfo->Id, m_originalCasterGUID))
                    {
                        if (m_caster != unit && !m_caster->IsWithinDistInMap(unit, range))
                        {
                            ihit->effectMask &= ~aurApp->GetEffectMask();
                            unit->RemoveAura(aurApp);
                            continue;
                        }
                    }
                    else // aura is dispelled
                        continue;
                }

                channelTargetEffectMask &= ~ihit->effectMask;   // remove from need alive mask effect that have alive target
            }
        }
    }

    // is all effects from m_needAliveTargetMask have alive targets
    return channelTargetEffectMask == 0;
}

void Spell::prepare(SpellCastTargets const* targets, AuraEffect const* triggeredByAura, uint32 p_GcdAtCast /*= 0*/)
{
    if (Unit::GetUnsafeSpellOrAuraScriptHookThreadLocal())
    {
        if (!Unit::IsAlreadyReportedSpell(m_spellInfo->Id))
        {
            std::string l_Channel = g_RealmID != 12 ? "#jarvis-spells" : "#jarvis";
            sLog->outExtChat(l_Channel, "danger", true, "RealmID : %u, worldserver_RelWithDebInfo // UnsafeSpellOrAuraScriptHook: Spell::prepare %u", g_RealmID, m_spellInfo->Id);
            if (sWorld->getBoolConfig(CONFIG_ENABLE_UNSAFE_SPELL_AURA_HOOK_STACK_TRACE))
            {
                ACE_Stack_Trace l_StackTrace;
                sLog->outExtChat(l_Channel, "danger", true, l_StackTrace.c_str());
            }

            Unit::AddReportedSpell(m_spellInfo->Id);
        }
    }

    Guid128 l_OldGuid = m_CastID;
    m_CastID = Guid128(GUID_TYPE_CAST, 0, m_spellInfo->Id, GenerateNewSpellGUID());

    if (Unit* l_Caster = GetCaster())
    {
        if ((l_Caster->IsPlayer() || l_Caster->IsVehicle()) && l_OldGuid.GetLow() && l_OldGuid != m_CastID)
        {
            WorldPacket l_Data(SMSG_SPELL_REGUID, 36);
            l_Data.appendGuid128(l_OldGuid);
            l_Data.appendGuid128(m_CastID);
            l_Caster->SendMessageToSet(&l_Data, true);
        }
    }

    if (m_CastItem)
    {
        m_castItemGUID = m_CastItem->GetGUID();

        if (m_CastItem->GetOwnerGUID() == m_caster->GetGUID())
            m_castItemLevel = m_CastItem->GetItemLevel(m_caster->ToPlayer());
        else if (Player* owner = m_CastItem->GetOwner())
            m_castItemLevel = m_CastItem->GetItemLevel(owner);
        else
        {
            SendCastResult(SPELL_FAILED_EQUIPPED_ITEM);
            finish(false);
            return;
        }
    }
    else
        m_castItemGUID = 0;

    ///< Dark Transformation
    if (m_caster->IsPlayer())
    {
        if (m_caster->ToPlayer()->GetPet() && m_spellInfo->Id == 63560)
        {
            TriggerCastFlags l_TriggerFlags = TriggerCastFlags(TriggerCastFlags::TRIGGERED_FULL_MASK & ~TriggerCastFlags::TRIGGERED_IGNORE_SPELL_AND_CATEGORY_CD);
            setTriggerCastFlags(l_TriggerFlags);
        }
    }

    InitExplicitTargets(*targets);

    // Fill aura scaling information
    if (m_caster->IsControlledByPlayer() && !m_spellInfo->IsPassive() && m_spellInfo->SpellLevel && !m_spellInfo->IsChanneled() && !(_triggeredCastFlags & TRIGGERED_IGNORE_AURA_SCALING))
    {
        for (uint8 i = 0; i < m_spellInfo->EffectCount; ++i)
        {
            if (m_spellInfo->Effects[i].IsApplyingAura())
            {
                // Change aura with ranks only if basepoints are taken from spellInfo and aura is positive
                if (m_spellInfo->IsPositiveEffect(i))
                {
                    m_auraScaleMask |= (1 << i);
                    if (m_spellValue->EffectBasePoints[i] != m_spellInfo->Effects[i].BasePoints)
                    {
                        m_auraScaleMask = 0;
                        break;
                    }
                }
            }
        }
    }

    m_spellState = SPELL_STATE_PREPARING;

    if (triggeredByAura)
    {
        m_triggeredByAuraSpell = triggeredByAura->GetSpellInfo();
        m_castItemLevel = triggeredByAura->GetBase()->GetCastItemLevel();
    }

    // create and add update event for this spell
    SpellEvent* Event = new SpellEvent(this);

    /// latency tolerance
    /// - If caster is player, check if he has GCD left on his spell cast, if he does, schedule a delayed event to cast his spell after it's GCD has ended (p_GcdAtCast)
    /// - if caster isn't player, ignore all this and schedule 1ms event
    if (Player* l_Player = m_caster->ToPlayer())
    {
        if (p_GcdAtCast && p_GcdAtCast <= MaxSpellQueueGcd)
        {
            if (Spell* const l_Spell = l_Player->GetQueuedSpell())
            {
                l_Spell->SendCastResult(SPELL_FAILED_SPELL_IN_PROGRESS);
                l_Spell->finish(false);
            }

            m_caster->m_Events.AddEvent(Event, m_caster->m_Events.CalculateTime(p_GcdAtCast));
            m_spellState = SPELL_STATE_QUEUED;
            l_Player->SetQueuedSpell(this);
            return;
        }
    }

    m_caster->m_Events.AddEvent(Event, m_caster->m_Events.CalculateTime(1));

    //Prevent casting at cast another spell (ServerSide check)
    if (!(_triggeredCastFlags & TRIGGERED_IGNORE_CAST_IN_PROGRESS) && m_caster->IsNonMeleeSpellCasted(false, true, true) &&
        m_caster->GetCurrentSpell(CURRENT_GENERIC_SPELL) &&
        (!(AttributesCustomEx9 & SPELL_ATTR9_CASTABLE_WHILE_CAST_IN_PROGRESS) || GetSpellInfo()->CalcCastTime(m_caster)))
    {
        /// Handling Hunter's Auto shot resuming when switching target during cast of another spell
        if (m_spellInfo->IsAutoRepeatRangedSpell() && m_targets.GetUnitTarget())
            m_caster->DelayedCastSpell(m_targets.GetUnitTarget(), m_spellInfo->Id, false, 100);

        /// Interrupting spells (Kick, Counterspell, Wind Shear, etc...) should now cancel current cast
        if (m_spellInfo->HasAttribute(SpellAttr13::SPELL_ATTR13_CANCEL_CURRENT_CAST))
            m_caster->InterruptNonMeleeSpells(false);
        else
        {
            SendCastResult(SPELL_FAILED_SPELL_IN_PROGRESS);
            finish(false);
            return;
        }
    }

    if (DisableMgr::IsDisabledFor(DISABLE_TYPE_SPELL, m_spellInfo->Id, m_caster))
    {
        SendCastResult(SPELL_FAILED_SPELL_UNAVAILABLE);
        finish(false);
        return;
    }

    LoadScripts();

    if (IsDarkSimulacrum() || (m_triggeredByAuraSpell && m_triggeredByAuraSpell->Id == 101056))
        isStolen = true;

    if (m_caster->IsPlayer())
        m_caster->ToPlayer()->SetSpellModTakingSpell(this, true);

    LinkedSpell(m_caster, m_targets.GetUnitTarget(), SPELL_LINK_BEFORE_CHECK);

    // Fill cost data (not use power for item casts
    memset(m_powerCost, 0, sizeof(uint32)* MAX_POWERS_COST);
    m_powerCost[MAX_POWERS_COST - 1] = 0;
    if (m_CastItem == 0)
        m_spellInfo->CalcPowerCost(m_caster, m_spellSchoolMask, m_powerCost);
    if (m_caster->IsPlayer())
        m_caster->ToPlayer()->SetSpellModTakingSpell(this, false);

    // Set combo point requirement
    if ((_triggeredCastFlags & TRIGGERED_IGNORE_COMBO_POINTS) || m_CastItem || !m_caster->m_movedPlayer)
        m_needComboPoints = false;

    if (m_CastItem && m_CastItem->GetArtifactKnowledgeOnAcquire())
        m_ArtifactKnowledgeLevelOfCastItem = m_CastItem->GetArtifactKnowledgeOnAcquire();

    SpellCastResult result = CheckCast(true);
    if (result != SPELL_CAST_OK && !IsAutoRepeat())          //always cast autorepeat dummy for triggering
    {
        // Periodic auras should be interrupted when aura triggers a spell which can't be cast
        // for example bladestorm aura should be removed on disarm as of patch 3.3.5
        // channeled periodic spells should be affected by this (arcane missiles, penance, etc)
        // a possible alternative sollution for those would be validating aura target on unit state change
        if (triggeredByAura && triggeredByAura->IsPeriodic() && !triggeredByAura->GetBase()->IsPassive())
        {
            if (result != SPELL_FAILED_BAD_TARGETS)
            {
                SendChannelUpdate(0);
                triggeredByAura->GetBase()->SetDuration(0);
                SendCastResult(result);
            }
        }
        else
            SendCastResult(result);

        /// Restore SpellMods after spell failed
        if (m_caster->GetTypeId() == TypeID::TYPEID_PLAYER)
            m_caster->ToPlayer()->RestoreSpellMods(this);

        finish(false);
        return;
    }

    // Prepare data for triggers
    prepareDataForTriggerSystem(triggeredByAura);

    if (m_caster->IsPlayer())
        m_caster->ToPlayer()->SetSpellModTakingSpell(this, true);
    // calculate cast time (calculated after first CheckCast check to prevent charge counting for first CheckCast fail)
    m_casttime = m_spellInfo->CalcCastTime(m_caster, this);

    // Unstable Afflication (30108) with Soulborn: Soul Swap (141931)
    if (m_spellInfo && m_spellInfo->Id == 30108 && _triggeredCastFlags == TRIGGERED_FULL_MASK)
        m_casttime = 0;

    // If spell not channeled and was stolen he have no cast time
    if (isStolen && !m_spellInfo->IsChanneled() && m_spellInfo->Id != 605)
        m_casttime = 0;

    if (m_caster->IsPlayer())
    {
        m_caster->ToPlayer()->SetSpellModTakingSpell(this, false);

        // Set cast time to 0 if .cheat cast time is enabled.
        if (m_caster->ToPlayer()->GetCommandStatus(CHEAT_CASTTIME))
             m_casttime = 0;
    }

    ///< Ice Floes
    if (m_caster->IsPlayer() && m_caster->ToPlayer()->getClass() == CLASS_MAGE)
    {
        if (AuraEffect* l_AuraEffect = m_caster->GetAuraEffect(108839, EFFECT_0, m_caster->GetGUID()))
        {
            if (m_spellInfo && l_AuraEffect->IsAffectingSpell(m_spellInfo) && m_casttime == 0 && m_spellInfo->CalcCastTime() > 0)
                m_caster->m_SpellHelper.GetBool(eSpellHelpers::IceFloes) = false;
            else if (m_spellInfo && l_AuraEffect->IsAffectingSpell(m_spellInfo) && m_casttime > 0)
                m_caster->m_SpellHelper.GetBool(eSpellHelpers::IceFloes) = true;
        }
    }

    // don't allow channeled spells / spells with cast time to be casted while moving
    // (even if they are interrupted on moving, spells with almost immediate effect get to have their effect processed before movement interrupter kicks in)
    // don't cancel spells which are affected by a SPELL_AURA_CAST_WHILE_WALKING or SPELL_AURA_ALLOW_ALL_CASTS_WHILE_WALKINGeffect
    if (((m_spellInfo->IsChanneled() || m_casttime) && m_caster->IsPlayer() && m_caster->IsMoving() &&
        m_spellInfo->InterruptFlags & SPELL_INTERRUPT_FLAG_MOVEMENT) && !m_caster->HasAuraTypeWithAffectMask(SPELL_AURA_CAST_WHILE_WALKING, m_spellInfo) &&
        !m_caster->HasAuraType(SPELL_AURA_ALLOW_ALL_CASTS_WHILE_WALKING) && !(m_spellInfo->AttributesEx5 & SPELL_ATTR5_USABLE_WHILE_MOVING) && !IsTriggered())
    {
        ///< Vengeful Retreat, Double Jump, Glide and Eye Beam
        if (m_spellInfo->Id == 198013 && m_caster->HasAura(196055)) {}
        if (m_spellInfo->Id == 198013 && m_caster->HasAura(131347)) {}
        if (m_spellInfo->Id == 198013 && m_caster->HasAura(198793)) {}
        else
        {
            SendCastResult(SPELL_FAILED_MOVING);
            finish(false);
            return;
        }
    }

    CallScriptOnPrepareHandlers();

    // set timer base at cast time
    ReSetTimer();

    LinkedSpell(m_caster, m_targets.GetUnitTarget(), SPELL_LINK_PREPARE_CAST);

    //Containers for channeled spells have to be set
    //TODO:Apply this to all casted spells if needed
    // Why check duration? 29350: channeled triggers channeled
    if ((_triggeredCastFlags & TRIGGERED_CAST_DIRECTLY) && (!m_spellInfo->IsChanneled() || !m_spellInfo->GetMaxDuration()))
    {
        if (m_spellInfo->HasEffect(SpellEffects::SPELL_EFFECT_LOOT_BONUS) || m_spellInfo->Id == 33395)
            SendSpellStart();

        if (!(_triggeredCastFlags & TRIGGERED_IGNORE_GCD) && result == SPELL_CAST_OK) ///< Global cooldown should be trigger before cast
            TriggerGlobalCooldown();

        cast(true);
    }
    else
    {
        GetCaster()->m_SpellHelper.GetUint32(eSpellHelpers::LastInterruptingSpell) = GetSpellInfo()->Id;

        m_caster->SetCurrentCastedSpell(this);
        // stealth must be removed at cast starting (at show channel bar)
        // skip triggered spell (item equip spell casting and other not explicit character casts/item uses)
        if (CheckInterrupt())
        {
            uint32 l_Except = GetNonInterruptableSpellBy(m_spellInfo->Id);
            m_caster->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_CAST, l_Except);
            for (uint32 i = 0; i < m_spellInfo->EffectCount; ++i)
            {
                if (m_spellInfo->Effects[i].GetUsedTargetObjectType() == TARGET_OBJECT_TYPE_UNIT)
                {
                    m_caster->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_SPELL_ATTACK, l_Except);
                    break;
                }
            }
        }

        GetCaster()->m_SpellHelper.GetUint32(eSpellHelpers::LastInterruptingSpell) = 0;

        SendSpellPendingCast(); //Send activation spell
        SendSpellStart();

        // set target for proper facing
        if (m_caster->GetTypeId() == TYPEID_UNIT && (m_casttime || m_spellInfo->IsChanneled()) && !(_triggeredCastFlags & TRIGGERED_IGNORE_SET_FACING))
        {
            if (m_targets.GetObjectTarget() && m_caster != m_targets.GetObjectTarget())
                m_caster->FocusTarget(this, m_targets.GetObjectTarget());

            if (m_spellInfo->AttributesEx5 & SPELL_ATTR5_DONT_TURN_DURING_CAST || m_spellInfo->AttributesEx11 & SPELL_ATTR11_DONT_TURN_DURING_CAST_2)
                m_caster->AddUnitState(UNIT_STATE_ROTATING);
        }

        if (!(_triggeredCastFlags & TRIGGERED_IGNORE_GCD) && result == SPELL_CAST_OK)
            TriggerGlobalCooldown();

        //item: first cast may destroy item and second cast causes crash
        if (!m_casttime && !m_spellInfo->StartRecoveryTime && !m_castItemGUID && GetCurrentContainer() == CURRENT_GENERIC_SPELL)
            cast(true);
    }
}

bool Spell::CheckInterrupt()
{
    if (CallScriptCheckInterruptHandlers())
        return true;

    if (!(_triggeredCastFlags & TRIGGERED_IGNORE_AURA_INTERRUPT_FLAGS) && m_spellInfo->IsBreakingStealth(m_caster) && (!m_caster->HasAuraType(SPELL_AURA_MOD_CAMOUFLAGE) || m_spellInfo->IsBreakingCamouflage()))
        return true;

    return false;
}

void Spell::cancel()
{
    if (m_spellState == SPELL_STATE_FINISHED)
        return;

    UpdatePointers();

    uint32 oldState = m_spellState;
    m_spellState = SPELL_STATE_FINISHED;

    m_autoRepeat = false;
    switch (oldState)
    {
        case SPELL_STATE_PREPARING:
            CancelGlobalCooldown();
            if (m_caster->IsPlayer())
                m_caster->ToPlayer()->RestoreSpellMods(this);
        case SPELL_STATE_DELAYED:
            SendInterrupted(0);
            SendCastResult(SPELL_FAILED_INTERRUPTED);
            break;

        case SPELL_STATE_CASTING:
            for (std::list<TargetInfo>::const_iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
                if ((*ihit).missCondition == SPELL_MISS_NONE)
                    if (Unit* unit = m_caster->GetGUID() == ihit->targetGUID ? m_caster : ObjectAccessor::GetUnit(*m_caster, ihit->targetGUID))
                        unit->RemoveOwnedAura(m_spellInfo->Id, m_originalCasterGUID, 0, AURA_REMOVE_BY_CANCEL);

            SendChannelUpdate(0);
            SendInterrupted(0);
            SendCastResult(SPELL_FAILED_INTERRUPTED);

            // spell is canceled-take mods and clear list
            if (m_caster->IsPlayer())
                m_caster->ToPlayer()->RemoveSpellMods(this);

            m_appliedMods.clear();
            break;

        default:
            break;
    }

    SetReferencedFromCurrent(false);
    if (m_selfContainer && *m_selfContainer == this)
        *m_selfContainer = NULL;

    m_caster->RemoveDynObject(m_spellInfo->Id);
    if (m_spellInfo->IsChanneled()) // if not channeled then the object for the current cast wasn't summoned yet
        m_caster->RemoveGameObject(m_spellInfo->Id, true);

    //set state back so finish will be processed
    m_spellState = oldState;

    finish(false);
}

void Spell::cast(bool skipCheck)
{
    // update pointers base at GUIDs to prevent access to non-existed already object
    UpdatePointers();

    // cancel at lost explicit target during cast
    if (m_targets.GetObjectTargetGUID() && !m_targets.GetObjectTarget())
    {
        cancel();
        return;
    }

    if (Player* playerCaster = m_caster->ToPlayer())
    {
        // now that we've done the basic check, now run the scripts
        // should be done before the spell is actually executed
        sScriptMgr->OnPlayerSpellCast(playerCaster, this, skipCheck);

        // As of 3.0.2 pets begin attacking their owner's target immediately
        // Let any pets know we've attacked something. Check DmgClass for harmful spells only
        // This prevents spells such as Hunter's Mark from triggering pet attack
        if (this->GetSpellInfo()->DmgClass != SPELL_DAMAGE_CLASS_NONE)
            if (Pet* playerPet = playerCaster->GetPet())
                if (playerPet->ToCreature()->IsAIEnabled && playerPet->isAlive() && playerPet->isControlled() && (m_targets.GetTargetMask() & TARGET_FLAG_UNIT))
                    if (WorldObject* target = m_targets.GetObjectTarget())
                        playerPet->AI()->OwnerAttacked(target->ToUnit());
    }
    SetExecutedCurrently(true);

    if (m_caster->GetTypeId() != TYPEID_PLAYER && m_targets.GetUnitTarget() && m_targets.GetUnitTarget() != m_caster)
        m_caster->SetInFront(m_targets.GetUnitTarget());

    // Should this be done for original caster?
    if (m_caster->IsPlayer())
    {
        // Set spell which will drop charges for triggered cast spells
        // if not successfully casted, will be remove in finish(false)
        m_caster->ToPlayer()->SetSpellModTakingSpell(this, true);
    }

    switch (m_spellInfo->Id)
    {
        case 101603: // Throw Totem
            if (m_caster->HasAura(107837))
                m_caster->RemoveAura(107837, 0, 0, AURA_REMOVE_BY_CANCEL);
                m_caster->RemoveAura(101601, 0, 0, AURA_REMOVE_BY_CANCEL);
            break;
        case 119393: // Siege Explosive
            if (m_caster->HasAura(119388))
                m_caster->RemoveAura(119388, 0, 0, AURA_REMOVE_BY_CANCEL);
                m_caster->RemoveAura(119386, 0, 0, AURA_REMOVE_BY_CANCEL);
            break;
        case 123057: // Sonic Divebomb
            if (m_caster->HasAura(123021))
                m_caster->RemoveAura(123021, 0, 0, AURA_REMOVE_BY_CANCEL);
                m_caster->RemoveAura(123057, 0, 0, AURA_REMOVE_BY_CANCEL);
            break;
        case 123039: // Player Throw Barrel
            if (m_caster->HasAura(123032))
                m_caster->RemoveAura(123032, 0, 0, AURA_REMOVE_BY_CANCEL);
                m_caster->RemoveAura(123035, 0, 0, AURA_REMOVE_BY_CANCEL);
            break;
        case 127329: // Throw Bomb
            if (m_caster->HasAura(127226))
                m_caster->RemoveAura(127226, 0, 0, AURA_REMOVE_BY_CANCEL);
                m_caster->RemoveAura(127175, 0, 0, AURA_REMOVE_BY_CANCEL);
            break;
    default:
        break;
    }

    CallScriptBeforeCastHandlers();
    LinkedSpell(m_caster, m_targets.GetUnitTarget(), SPELL_LINK_BEFORE_CAST);

    // skip check if done already (for instant cast spells for example)
    if (!skipCheck)
    {
        SpellCastResult castResult = CheckCast(false);
        if (castResult != SPELL_CAST_OK)
        {
            SendCastResult(castResult);
            SendInterrupted(0);
            //restore spell mods
            if (m_caster->IsPlayer())
            {
                m_caster->ToPlayer()->RestoreSpellMods(this);
                // cleanup after mod system
                // triggered spell pointer can be not removed in some cases
                m_caster->ToPlayer()->SetSpellModTakingSpell(this, false);
            }
            finish(false);
            SetExecutedCurrently(false);
            return;
        }

        // additional check after cast bar completes (must not be in CheckCast)
        // if trade not complete then remember it in trade data
        if (m_targets.GetTargetMask() & TARGET_FLAG_TRADE_ITEM)
        {
            if (m_caster->IsPlayer())
            {
                if (TradeData* my_trade = m_caster->ToPlayer()->GetTradeData())
                {
                    if (!my_trade->IsInAcceptProcess())
                    {
                        // Spell will be casted at completing the trade. Silently ignore at this place
                        my_trade->SetSpell(m_spellInfo->Id, m_CastItem);
                        SendCastResult(SPELL_FAILED_DONT_REPORT);
                        SendInterrupted(0);
                        m_caster->ToPlayer()->RestoreSpellMods(this);
                        // cleanup after mod system
                        // triggered spell pointer can be not removed in some cases
                        m_caster->ToPlayer()->SetSpellModTakingSpell(this, false);
                        finish(false);
                        SetExecutedCurrently(false);
                        return;
                    }
                }
            }
        }
    }

    if (m_caster->GetTypeId() == TYPEID_PLAYER && m_targets.GetUnitTarget() && m_targets.GetUnitTarget() != m_caster && IsPowerFullAura(m_spellInfo, m_caster, m_targets.GetUnitTarget()))
    {
        SendCastResult(SPELL_FAILED_AURA_BOUNCED); ///< from sniff
        SendInterrupted(0);
        if (m_caster->IsPlayer())
        {
            m_caster->ToPlayer()->RestoreSpellMods(this);
            m_caster->ToPlayer()->SetSpellModTakingSpell(this, false);
        }
        finish(false);
        SetExecutedCurrently(false);
        return;
    }

    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::time_point();
    if (sMapMgr->m_LogMapPerformance || m_caster->GetMap()->IsRaid())
        start = std::chrono::steady_clock::now();

    SelectSpellTargets();

    if (sMapMgr->m_LogMapPerformance || m_caster->GetMap()->IsRaid())
        m_caster->GetMap()->m_SpellSelectTargetsTime += std::chrono::steady_clock::now() - start;

    // Spell may be finished after target map check
    if (m_spellState == SPELL_STATE_FINISHED)
    {
        SendInterrupted(0);
        //restore spell mods
        if (m_caster->IsPlayer())
        {
            m_caster->ToPlayer()->RestoreSpellMods(this);
            // cleanup after mod system
            // triggered spell pointer can be not removed in some cases
            m_caster->ToPlayer()->SetSpellModTakingSpell(this, false);
        }
        finish(false);
        SetExecutedCurrently(false);
        return;
    }

    PrepareTriggersExecutedOnHit();

    if (m_caster->getClass() == CLASS_DEATH_KNIGHT)
        if (IsDarkSimulacrum())
            m_caster->RemoveAurasDueToSpell(77616);

    CallScriptOnCastHandlers();

    // traded items have trade slot instead of guid in m_itemTargetGUID
    // set to real guid to be sent later to the client
    m_targets.UpdateTradeSlotItem();

    if (Player* player = m_caster->ToPlayer())
    {
        if (!(_triggeredCastFlags & TRIGGERED_IGNORE_CAST_ITEM) && m_CastItem)
        {
            player->StartCriteriaTimer(CRITERIA_TIMED_TYPE_ITEM, m_CastItem->GetEntry());
            player->StartCriteriaTimer(CRITERIA_TIMED_TYPE_ITEM2, m_CastItem->GetEntry());
            player->UpdateCriteria(CRITERIA_TYPE_USE_ITEM, m_CastItem->GetEntry(), 0, 0, player);
        }

        if (m_spellInfo->CanUpdateCriteriaCastSpell)
            player->UpdateCriteria(CRITERIA_TYPE_CAST_SPELL, m_spellInfo->Id, 0, 0, player);

        /// Needed to complete toy achievements
        if (!m_CastItem && m_CastItemEntry)
            player->UpdateCriteria(CRITERIA_TYPE_USE_ITEM, m_CastItemEntry, 0, 0, player);
    }

    if (!(_triggeredCastFlags & TRIGGERED_IGNORE_POWER_AND_REAGENT_COST))
    {
        // Powers have to be taken before SendSpellGo
        TakePower();
        TakeReagents();                                         // we must remove reagents before HandleEffects to allow place crafted item in same slot
    }
    else if (Item* targetItem = m_targets.GetItemTarget())
    {
        /// Not own traded item (in trader trade slot) req. reagents including triggered spell case
        if (targetItem->GetOwnerGUID() != m_caster->GetGUID())
            TakeReagents();
    }

    LinkedSpell(m_caster, m_targets.GetUnitTarget());

    // CAST SPELL
    SendSpellCooldown();

    PrepareScriptHitHandlers();

    HandleLaunchPhase();

    bool l_SendSpellGoBeforeExecuteEffect = false;

    /// On retail, SMSG_SPELL_GO is sent before the effect is processed in most of case (if we don't do it we have sync issues with powers and some others issues)
    /// However it broke the gameobject opening so in case the spell is a SPELL_EFFECT_OPEN_LOCK we have to send it before executing the effect
    /// Otherwise the loot window instant close itself when the client receive the SMSG_SPELL_GO after SMSG_LOOT_RESPONSE
    std::vector<decltype(SPELL_EFFECT_NONE)> l_BeforeExecuteSpellGoEffects =
    {{
        SPELL_EFFECT_OPEN_LOCK,
        SPELL_EFFECT_SKINNING,
        SPELL_EFFECT_PICKPOCKET,
        SPELL_EFFECT_DISENCHANT,
        SPELL_EFFECT_PROSPECTING,
        SPELL_EFFECT_MILLING
    }};

    for (auto l_Effect : l_BeforeExecuteSpellGoEffects)
    {
        if (m_spellInfo->HasEffect(l_Effect))
        {
            l_SendSpellGoBeforeExecuteEffect = true;
            break;
        }
    }

    if (l_SendSpellGoBeforeExecuteEffect)
        SendSpellGo();

    m_caster->SendSpellCreateVisual(m_spellInfo, m_caster, m_targets.GetUnitTarget());

    if (m_targets.HasDst())
        m_caster->SendSpellPlayOrphanVisual(m_spellInfo, true, m_targets.GetDstPos(), m_targets.GetUnitTarget());
    else
        m_caster->SendSpellPlayOrphanVisual(m_spellInfo, true, &*m_caster, m_targets.GetUnitTarget());

    // Okay, everything is prepared. Now we need to distinguish between immediate and evented delayed spells
    if (((m_spellInfo->Speed > 0.0f || (m_delayMoment && (m_caster->IsPlayer() || m_caster->ToCreature()->isPet())))
        && !m_spellInfo->IsChanneled()) || m_spellInfo->_IsNeedDelay() || m_spellInfo->HasAttribute(SPELL_ATTR4_HAS_DELAY))
    {
        if (!l_SendSpellGoBeforeExecuteEffect)
        {
            // we must send smsg_spell_go packet before m_castItem delete in TakeCastItem()...
            SendSpellGo();
        }

        // Remove used for cast item if need (it can be already NULL after TakeReagents call
        // in case delayed spell remove item at cast delay start
        TakeCastItem();

        // Okay, maps created, now prepare flags
        m_immediateHandled = false;
        m_spellState = SPELL_STATE_DELAYED;
        SetDelayStart(0);

        if (IsAutoActionResetSpell())
        {
            bool found = false;
            Unit::AuraEffectList const& vIgnoreReset = m_caster->GetAuraEffectsByType(SPELL_AURA_IGNORE_MELEE_RESET);
            for (Unit::AuraEffectList::const_iterator i = vIgnoreReset.begin(); i != vIgnoreReset.end(); ++i)
            {
                if ((*i)->IsAffectingSpell(m_spellInfo))
                {
                    found = true;
                    break;
                }
            }
            if (!found && !(AttributesCustomEx2 & SPELL_ATTR2_NOT_RESET_AUTO_ACTIONS))
            {
                m_caster->resetAttackTimer(WeaponAttackType::BaseAttack);
                if (m_caster->haveOffhandWeapon())
                    m_caster->resetAttackTimer(WeaponAttackType::OffAttack);
                m_caster->resetAttackTimer(WeaponAttackType::RangedAttack);
            }
        }

        if (m_caster->HasUnitState(UNIT_STATE_CASTING) && !m_caster->IsNonMeleeSpellCasted(false, false, true))
            m_caster->ClearUnitState(UNIT_STATE_CASTING);
    }
    else
    {
        // Immediate spell, no big deal
        handle_immediate();

        if (!l_SendSpellGoBeforeExecuteEffect)
            SendSpellGo();
    }

    CallScriptAfterCastHandlers();

    if (Player* l_PlayerCaster = m_caster->ToPlayer())
    {
        sScriptMgr->AfterPlayerSpellCast(l_PlayerCaster, this, skipCheck);

        if (getTriggerCastFlags() != TRIGGERED_FULL_MASK && m_spellInfo->Id != 113656 && m_spellInfo->Id != 205320 && m_spellInfo->Id != 117952)
            l_PlayerCaster->SetLastCastedSpell(m_spellInfo->Id);

        /// Hack Fix Ice Floes - Drop charges
        if (l_PlayerCaster && l_PlayerCaster->getClass() == CLASS_MAGE && m_spellInfo->Id != 108839)
        {
            if (AuraEffect* l_AuraEffect = l_PlayerCaster->GetAuraEffect(108839, EFFECT_0, l_PlayerCaster->GetGUID()))
            {
                if (l_AuraEffect->IsAffectingSpell(m_spellInfo) && (m_casttime > 0 || (m_casttime == 0 && m_spellInfo->CalcCastTime() > 0)))
                {
                    if (l_PlayerCaster->m_SpellHelper.GetBool(eSpellHelpers::IceFloes) == true)
                    {
                        l_AuraEffect->GetBase()->DropStack();
                        l_PlayerCaster->m_SpellHelper.GetBool(eSpellHelpers::IceFloes) = false;
                        l_PlayerCaster->m_SpellHelper.GetBool(eSpellHelpers::IceFloesMove) = false;
                    }
                }
            }
        }
    }

    if (AttributesCustom & SPELL_ATTR0_ABILITY)
        m_caster->SetLastAbilityID(m_spellInfo->Id);

    /// Trigger all effects if this spell should do that after cast
    if (IsSpellTriggeredAfterCast() && m_caster->IsPlayer())
    {
        // triggered spell pointer can be not set in some cases
        // this is needed for proper apply of triggered spell mods
        m_caster->ToPlayer()->SetSpellModTakingSpell(this, true);

        // Take mods after trigger spell (needed for 14177 to affect 48664)
        // mods are taken only on succesfull cast and independantly from targets of the spell
        m_caster->ToPlayer()->RemoveSpellMods(this);
        m_caster->ToPlayer()->SetSpellModTakingSpell(this, false);
    }

    if (m_caster->GetTypeId() == TypeID::TYPEID_UNIT && m_caster->ToCreature()->IsAIEnabled)
        m_caster->ToCreature()->AI()->OnSpellCasted(m_spellInfo);

    /// Soul Swap - if we have copied DOTs and cast any other spell, soul swap will refresh duration of all DOTs
    if (m_caster->IsPlayer() && m_caster->getClass() == CLASS_WARLOCK && m_caster->ToPlayer()->GetActiveSpecializationID() == SPEC_WARLOCK_AFFLICTION)
    {
        /// Ignore generic proc spells like Soul Shards (visual), Life Steal etc, also ignore Soul Swap spell
        if (m_spellInfo && m_spellInfo->Id != 86121 && m_spellInfo->SpellFamilyName == SPELLFAMILY_WARLOCK && _triggeredCastFlags == TRIGGERED_NONE && m_caster->HasAura(86211))
            m_caster->SetSoulSwapRefreshDuration(true);
    }

    if (m_caster->IsPlayer())
    {
        m_caster->ToPlayer()->SetSpellModTakingSpell(this, false);
        //Clear spell cooldowns after every spell is cast if .cheat cooldown is enabled.
        if (m_caster->ToPlayer()->GetCommandStatus(CHEAT_COOLDOWN) || (((m_CastItem && m_CastItem->GetEntry() == 6948) || (m_CastItem && m_CastItem->GetEntry() == 140192)) && m_caster->ToPlayer()->GetSession()->IsPremium()))
        {
            m_caster->ToPlayer()->RemoveSpellCooldown(m_spellInfo->Id, true);
            m_caster->ToPlayer()->RestoreCharge(m_spellInfo->ChargeCategoryEntry);
        }
    }

    SetExecutedCurrently(false);
}

void Spell::handle_immediate()
{
    // start channeling if applicable
    if (m_spellInfo->IsChanneled())
    {
        int32 duration = m_spellInfo->GetDuration();
        if (duration)
        {
            // First mod_duration then haste - see Missile Barrage
            // Apply duration mod
            if (Player* modOwner = m_caster->GetSpellModOwner())
                modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_DURATION, duration);

            // Apply haste mods
            m_caster->ModSpellCastTime(m_spellInfo, duration, this);

            m_spellState = SPELL_STATE_CASTING;
            m_caster->AddInterruptMask(m_spellInfo->ChannelInterruptFlags);
            if (!m_channeledDuration)
                m_channeledDuration = duration;
            SendChannelStart(m_channeledDuration);
        }
        else if (duration == -1)
        {
            m_spellState = SPELL_STATE_CASTING;
            m_caster->AddInterruptMask(m_spellInfo->ChannelInterruptFlags);
            SendChannelStart(duration);
        }
    }

    PrepareTargetProcessing();

    // process immediate effects (items, ground, etc.) also initialize some variables
    _handle_immediate_phase();

    for (std::list<TargetInfo>::iterator ihit= m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
        DoAllEffectOnTarget(&(*ihit));

    for (std::list<GOTargetInfo>::iterator ihit= m_UniqueGOTargetInfo.begin(); ihit != m_UniqueGOTargetInfo.end(); ++ihit)
        DoAllEffectOnTarget(&(*ihit));

    FinishTargetProcessing();

    // spell is finished, perform some last features of the spell here
    _handle_finish_phase();

    // Remove used for cast item if need (it can be already NULL after TakeReagents call
    TakeCastItem();

    // handle ammo consumption for thrown weapons
    if (m_spellInfo->IsRangedWeaponSpell() && m_spellInfo->IsChanneled())
        TakeAmmo();

    if (m_spellState != SPELL_STATE_CASTING)
        finish(true);                                       // successfully finish spell cast (not last in case autorepeat or channel spell)
}

uint64 Spell::handle_delayed(uint64 t_offset)
{
    UpdatePointers();

    if (m_caster->IsPlayer())
        m_caster->ToPlayer()->SetSpellModTakingSpell(this, true);

    uint64 next_time = 0;

    PrepareTargetProcessing();

    if (!m_immediateHandled)
    {
        _handle_immediate_phase();
        m_immediateHandled = true;
    }

    bool single_missile = (m_targets.HasDst());

    // now recheck units targeting correctness (need before any effects apply to prevent adding immunity at first effect not allow apply second spell effect and similar cases)
    for (std::list<TargetInfo>::iterator ihit= m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
    {
        if (ihit->processed == false)
        {
            if (single_missile || ihit->timeDelay <= t_offset)
            {
                ihit->timeDelay = t_offset;
                DoAllEffectOnTarget(&(*ihit));
            }
            else if (next_time == 0 || ihit->timeDelay < next_time)
                next_time = ihit->timeDelay;
        }
    }

    // now recheck gameobject targeting correctness
    for (std::list<GOTargetInfo>::iterator ighit= m_UniqueGOTargetInfo.begin(); ighit != m_UniqueGOTargetInfo.end(); ++ighit)
    {
        if (ighit->processed == false)
        {
            if (single_missile || ighit->timeDelay <= t_offset)
                DoAllEffectOnTarget(&(*ighit));
            else if (next_time == 0 || ighit->timeDelay < next_time)
                next_time = ighit->timeDelay;
        }
    }

    FinishTargetProcessing();

    if (m_caster->IsPlayer())
        m_caster->ToPlayer()->SetSpellModTakingSpell(this, false);

    // All targets passed - need finish phase
    if (next_time == 0)
    {
        // spell is finished, perform some last features of the spell here
        _handle_finish_phase();

        finish(true);                                       // successfully finish spell cast

        // return zero, spell is finished now
        return 0;
    }
    else
    {
        // spell is unfinished, return next execution time
        return next_time;
    }
}

void Spell::_handle_immediate_phase()
{
    m_spellAura = nullptr;
    // initialize Diminishing Returns Data
    m_diminishLevel = DIMINISHING_LEVEL_1;
    m_diminishGroup = DIMINISHING_NONE;

    // handle some immediate features of the spell here
    HandleThreatSpells();

    PrepareScriptHitHandlers();

    // handle effects with SPELL_EFFECT_HANDLE_HIT mode
    for (uint8 l_Itr = 0; l_Itr < m_spellInfo->EffectCount; ++l_Itr)
    {
        // don't do anything for empty effect
        if (!m_spellInfo->Effects[l_Itr].IsEffect())
            continue;

        // call effect handlers to handle destination hit
        HandleEffects(nullptr, nullptr, nullptr, nullptr, l_Itr, SPELL_EFFECT_HANDLE_HIT);
    }

    // process items
    for (std::list<ItemTargetInfo>::iterator l_Itr= m_UniqueItemInfo.begin(); l_Itr != m_UniqueItemInfo.end(); ++l_Itr)
        DoAllEffectOnTarget(&(*l_Itr));

    if (!m_originalCaster)
        return;

    // Handle procs on cast
    // TODO: finish new proc system:P
    if (m_UniqueTargetInfo.empty() && m_targets.HasDst())
    {
        uint32 l_ProcAttacker = m_procAttacker;
        if (!l_ProcAttacker)
            l_ProcAttacker |= PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS;

        // Proc the spells that have DEST target
        Unit* l_Target = NULL;
        if (l_ProcAttacker & PROC_FLAG_DONE_TRAP_ACTIVATION)
            l_Target = m_targets.GetUnitTarget();

        DamageInfo l_DamageInfo(m_originalCaster, l_Target, 0, this, SPELL_DIRECT_DAMAGE, WeaponAttackType::BaseAttack);

        m_originalCaster->ProcDamageAndSpell(l_Target, l_ProcAttacker, 0, m_procEx | PROC_EX_NORMAL_HIT, l_DamageInfo, m_triggeredByAuraSpell);
    }
}

void Spell::_handle_finish_phase()
{
    if (m_caster->m_movedPlayer)
    {
        // Real add combo points from effects
        if (m_comboPointGain)
            m_caster->AddComboPoints(m_comboPointGain);

        /// remove used combo points
        TakeComboPoints();

        if (m_spellInfo->PowerType == POWER_HOLY_POWER && m_caster->m_movedPlayer->getClass() == CLASS_PALADIN)
            HandleHolyPower(m_caster->m_movedPlayer);
    }

    if (m_caster->m_extraAttacks && GetSpellInfo()->HasEffect(SPELL_EFFECT_ADD_EXTRA_ATTACKS))
        m_caster->HandleProcExtraAttackFor(m_caster->getVictim());

    // TODO: trigger proc phase finish here
}

void Spell::SendSpellCooldown()
{
    Player* l_Player = m_caster->ToPlayer();
    if (!l_Player)
    {
        // Handle pet cooldowns here if needed instead of in PetAI to avoid hidden cooldown restarts
        Creature* _creature = m_caster->ToCreature();
        if (_creature && (_creature->isPet() || _creature->isGuardian()))
        {
            _creature->AddCreatureSpellCooldown(m_spellInfo->Id);
            return;
        }
        else if (_creature && _creature->GetSpellModOwner() && _creature->GetSpellModOwner()->IsPlayer())
            l_Player = _creature->GetSpellModOwner()->ToPlayer();
        else
            return;
    }

    if (l_Player && l_Player->ConsumeCharge(m_spellInfo->ChargeCategoryEntry, m_spellInfo->Id))
    {
        _triggeredCastFlags = (TriggerCastFlags)(_triggeredCastFlags & ~TRIGGERED_IGNORE_SPELL_AND_CATEGORY_CD);
        return;
    }

    // mana/health/etc potions, disabled by client (until combat out as declarate)
    if (m_CastItem && (m_CastItem->IsPotion() || m_CastItem->IsHealthstone() || m_spellInfo->IsCooldownStartedOnEvent()))
    {
        l_Player->SetLastPotion(m_spellInfo, m_CastItem);
        return;
    }

    // have infinity cooldown but set at aura apply                  // do not set cooldown for triggered spells (needed by reincarnation)
    if (m_spellInfo->IsCooldownStartedOnEvent() || m_spellInfo->IsPassive() || (_triggeredCastFlags & TRIGGERED_IGNORE_SPELL_AND_CATEGORY_CD))
        return;

    if (m_caster->HasAuraTypeWithAffectMask(SPELL_AURA_ALLOW_CAST_WHILE_IN_COOLDOWN, m_spellInfo))
        return;

    ///< Grimoire Of Supremacy (Summon doomguard)
    if ((m_spellInfo->Id == 18540 || m_spellInfo->Id == 157757) && m_caster->HasAura(152107))
        return;

    l_Player->AddSpellAndCategoryCooldowns(m_spellInfo, m_CastItem ? m_CastItem->GetEntry() : m_CastItemEntry, this);
}

void Spell::update(uint32 difftime)
{
    bool l_LogDiff = m_caster && m_caster->GetMapId() == 1520;
    uint32 l_Time = l_LogDiff ? getMSTime() : 0;

    if (m_spellState != SpellState::SPELL_STATE_QUEUED)
    {
        // update pointers based at it's GUIDs
        UpdatePointers();

        if (m_targets.GetUnitTargetGUID() && !m_targets.GetUnitTarget())
        {
            cancel();
            return;
        }

        // check if the player caster has moved before the spell finished
        // with the exception of spells affected with SPELL_AURA_CAST_WHILE_WALKING or SPELL_AURA_ALLOW_ALL_CASTS_WHILE_WALKING effect
        if ((m_caster->IsPlayer() && m_timer != 0) &&
            m_caster->IsMoving() && (m_spellInfo->InterruptFlags & SPELL_INTERRUPT_FLAG_MOVEMENT) &&
            (m_spellInfo->Effects[0].Effect != SPELL_EFFECT_STUCK || !m_caster->HasUnitMovementFlag(MOVEMENTFLAG_FALLING_FAR)) &&
            !m_caster->HasAuraTypeWithAffectMask(SPELL_AURA_CAST_WHILE_WALKING, m_spellInfo) && !m_caster->HasAuraType(SPELL_AURA_ALLOW_ALL_CASTS_WHILE_WALKING) && !(m_spellInfo->AttributesEx5 & SPELL_ATTR5_USABLE_WHILE_MOVING))
        {
            ///< Vengeful Retreat, Double Jump, Glide and Eye Beam
            if (m_spellInfo->Id == 198013 && m_caster->HasAura(196055)) {}
            if (m_spellInfo->Id == 198013 && m_caster->HasAura(131347)) {}
            if (m_spellInfo->Id == 198013 && m_caster->HasAura(198793)) {}
            else
            {
                // don't cancel for melee, autorepeat, triggered and instant spells
                if (!IsNextMeleeSwingSpell() && !IsAutoRepeat() && !IsTriggered())
                    cancel();
            }
        }
    }

    switch (m_spellState)
    {
        case SpellState::SPELL_STATE_QUEUED:
        {
            Player* l_Player = m_caster->ToPlayer();
            if (!l_Player)
                break;

            /// reset queued spell
            l_Player->GetGlobalCooldownMgr().CancelGlobalCooldown(m_spellInfo);
            l_Player->SetQueuedSpell(nullptr);

            /// init new spell, copy some data and execute
            Spell* l_NewSpell = new Spell(l_Player, m_spellInfo, _triggeredCastFlags, m_originalCasterGUID, m_skipCheck);
            l_NewSpell->m_FromClient = true;
            l_NewSpell->m_Misc[0] = m_Misc[0];
            l_NewSpell->m_Misc[1] = m_Misc[1];
            l_NewSpell->SetCastGUID(m_CastID);

            /// cancel current
            cancel();

            uint64 l_Guid = m_targets.GetUnitTargetGUID();
            if (!l_Guid || ObjectAccessor::GetUnit(*l_Player, l_Guid))
                l_NewSpell->prepare(&m_targets);

            break;
        }
        case SpellState::SPELL_STATE_PREPARING:
        {
            if (m_timer > 0)
            {
                if (difftime >= (uint32)m_timer)
                    m_timer = 0;
                else
                    m_timer -= difftime;
            }

            if (m_timer == 0 && !IsNextMeleeSwingSpell() && !IsAutoRepeat())
                // don't CheckCast for instant spells - done in spell::prepare, skip duplicate checks, needed for range checks for example
                cast(!m_casttime);

            ///< Ice Floes
            if (m_caster->IsPlayer() && m_caster->ToPlayer()->getClass() == CLASS_MAGE)
            {
                if (AuraEffect* l_AuraEffect = m_caster->GetAuraEffect(108839, EFFECT_0, m_caster->GetGUID()))
                {
                    bool l_Check = m_caster->m_SpellHelper.GetBool(eSpellHelpers::IceFloes);
                    bool l_Moving = m_caster->IsMoving();
                    if (!m_caster->m_SpellHelper.GetBool(eSpellHelpers::IceFloesMove))
                        m_caster->m_SpellHelper.GetBool(eSpellHelpers::IceFloesMove) = l_Moving;
                    if (m_spellInfo && l_AuraEffect->IsAffectingSpell(m_spellInfo) && !m_caster->m_SpellHelper.GetBool(eSpellHelpers::IceFloesMove) && l_Check)
                        m_caster->m_SpellHelper.GetBool(eSpellHelpers::IceFloes) = false;
                    else if (m_spellInfo && l_AuraEffect->IsAffectingSpell(m_spellInfo) && m_caster->m_SpellHelper.GetBool(eSpellHelpers::IceFloesMove) && !l_Check && m_casttime > 0)
                        m_caster->m_SpellHelper.GetBool(eSpellHelpers::IceFloes) = true;
                }
            }

            break;
        }
        case SpellState::SPELL_STATE_CASTING:
        {
            if (m_timer)
            {
                // check if there are alive targets left
                if (!UpdateChanneledTargetList())
                {
                    SendChannelUpdate(0);
                    finish();
                }

                if (m_timer > 0)
                {
                    if (difftime >= (uint32)m_timer)
                        m_timer = 0;
                    else
                        m_timer -= difftime;
                }
            }

            if (m_timer == 0)
            {
                SendChannelUpdate(0);

                // channeled spell processed independently for quest targeting
                // cast at creature (or GO) quest objectives update at successful cast channel finished
                // ignore autorepeat/melee casts for speed (not exist quest for spells (hm...)
                if (!IsAutoRepeat() && !IsNextMeleeSwingSpell())
                {
                    if (Player* p = m_caster->GetCharmerOrOwnerPlayerOrPlayerItself())
                    {
                        for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
                        {
                            TargetInfo* target = &*ihit;
                            if (!IS_CRE_OR_VEH_GUID(target->targetGUID))
                                continue;

                            Unit* unit = m_caster->GetGUID() == target->targetGUID ? m_caster : ObjectAccessor::GetUnit(*m_caster, target->targetGUID);
                            if (unit == NULL)
                                continue;

                            p->CastedCreatureOrGO(unit->GetEntry(), unit->GetGUID(), m_spellInfo->Id);
                        }

                        for (std::list<GOTargetInfo>::iterator ihit = m_UniqueGOTargetInfo.begin(); ihit != m_UniqueGOTargetInfo.end(); ++ihit)
                        {
                            GOTargetInfo* target = &*ihit;

                            GameObject* go = m_caster->GetMap()->GetGameObject(target->targetGUID);
                            if (!go)
                                continue;

                            p->CastedCreatureOrGO(go->GetEntry(), go->GetGUID(), m_spellInfo->Id);
                        }
                    }
                }

                finish();
            }

            break;
        }
        default:
            break;
    }

    if (l_LogDiff)
    {
        uint32 l_Diff = getMSTime() - l_Time;
        if (l_Diff > 10)
            sLog->outAshran("Spell::update %u take %u ms", m_spellInfo->Id, l_Diff);
    }
}

void Spell::finish(bool ok)
{
    if (!m_caster)
        return;

    if (m_caster->IsPlayer())
        sScriptMgr->OnSpellFinished(m_caster->ToPlayer(), this, ok);

    if (m_spellState == SPELL_STATE_FINISHED)
        return;

    m_spellState = SPELL_STATE_FINISHED;

    /// Light on your feet
    if (ok && m_spellInfo->Id == 191837 && m_caster->HasAura(199401))
        m_caster->CastSpell(m_caster, 199407, true);

    if (m_spellInfo->IsChanneled())
        m_caster->UpdateInterruptMask();

    if (m_caster->HasUnitState(UNIT_STATE_CASTING) && !m_caster->IsNonMeleeSpellCasted(false, false, true))
        m_caster->ClearUnitState(UNIT_STATE_CASTING | UNIT_STATE_MOVE_IN_CASTING);

    if (m_caster->GetTypeId() == TypeID::TYPEID_UNIT && m_caster->IsAIEnabled)
    {
        m_caster->ToCreature()->AI()->OnSpellFinished(m_spellInfo);
        if (ok)
            m_caster->ToCreature()->AI()->OnSpellFinishedSuccess(m_spellInfo);
    }

    if (IsAutoActionResetSpell())
    {
        bool found = false;
        Unit::AuraEffectList const& vIgnoreReset = m_caster->GetAuraEffectsByType(SPELL_AURA_IGNORE_MELEE_RESET);
        for (Unit::AuraEffectList::const_iterator i = vIgnoreReset.begin(); i != vIgnoreReset.end(); ++i)
        {
            if ((*i)->IsAffectingSpell(m_spellInfo))
            {
                found = true;
                break;
            }
        }
        if (!found && !(AttributesCustomEx2 & SPELL_ATTR2_NOT_RESET_AUTO_ACTIONS))
        {
            m_caster->resetAttackTimer(WeaponAttackType::BaseAttack);
            if (m_caster->haveOffhandWeapon())
                m_caster->resetAttackTimer(WeaponAttackType::OffAttack);
            m_caster->resetAttackTimer(WeaponAttackType::RangedAttack);
        }
    }

    if (m_caster->HasUnitState(UNIT_STATE_CASTING) && !m_caster->IsNonMeleeSpellCasted(false, false, true))
        m_caster->ClearUnitState(UNIT_STATE_CASTING);

    // Unsummon summon as possessed creatures on spell cancel
    if (m_spellInfo->IsChanneled() && m_caster->IsPlayer())
    {
        if (Unit* charm = m_caster->GetCharm())
            if (charm->GetTypeId() == TYPEID_UNIT
                && charm->ToCreature()->HasUnitTypeMask(UNIT_MASK_PUPPET)
                && charm->GetUInt32Value(UNIT_FIELD_CREATED_BY_SPELL) == m_spellInfo->Id)
                ((Puppet*)charm)->UnSummon();
    }

    if (m_caster->GetTypeId() == TYPEID_UNIT)
    {
        m_caster->ReleaseFocus(this);

        if (m_spellInfo->AttributesEx5 & SPELL_ATTR5_DONT_TURN_DURING_CAST || m_spellInfo->AttributesEx11 & SPELL_ATTR11_DONT_TURN_DURING_CAST_2)
            m_caster->ClearUnitState(UNIT_STATE_ROTATING);
    }

    if (!ok)
        return;

    if (m_caster->GetTypeId() == TYPEID_UNIT && m_caster->ToCreature()->isSummon())
    {
        /// Unsummon stone statue
        uint32 spell = m_caster->GetUInt32Value(UNIT_FIELD_CREATED_BY_SPELL);
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spell);
        if (spellInfo && (spellInfo->Id == RoughStoneStatue || spellInfo->Id == CoarseStoneStatue || spellInfo->Id == HeavyStoneStatue || spellInfo->Id == SolidStoneStatue))
        {
            m_caster->setDeathState(JUST_DIED);
            return;
        }
    }

    // potions disabled by client, send event "not in combat" if need
    if (m_caster->IsPlayer() && !IsSpellTriggeredAfterCast())
    {
        /// There is no cooldown to update for triggered spells
        if (!m_triggeredByAuraSpell && !(_triggeredCastFlags & TRIGGERED_IGNORE_SPELL_AND_CATEGORY_CD))
            m_caster->ToPlayer()->UpdatePotionCooldown(this);

        // triggered spell pointer can be not set in some cases
        // this is needed for proper apply of triggered spell mods
        m_caster->ToPlayer()->SetSpellModTakingSpell(this, true);

        // Take mods after trigger spell (needed for 14177 to affect 48664)
        // mods are taken only on succesfull cast and independantly from targets of the spell
        m_caster->ToPlayer()->RemoveSpellMods(this);
        m_caster->ToPlayer()->SetSpellModTakingSpell(this, false);
    }

    // Stop Attack for some spells
    if (AttributesCustom & SPELL_ATTR0_STOP_ATTACK_TARGET)
        m_caster->AttackStop();

    if (m_castItemGUID && m_caster->IsPlayer())
        if (Item* item = m_caster->ToPlayer()->GetItemByGuid(m_castItemGUID))
            if (item->IsEquipable() && !item->IsEquipped())
                m_caster->ToPlayer()->ApplyItemEquipSpell(item, false);

    LinkedSpell(m_caster, m_targets.GetUnitTarget(), SPELL_LINK_FINISH_CAST);
}

void Spell::SendCastResult(SpellCastResult result)
{
    if (result == SPELL_CAST_OK)
        return;

    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    if (m_caster->ToPlayer()->GetSession()->PlayerLoading())  // don't send cast results at loading time
        return;

    SendCastResult(m_caster->ToPlayer(), m_spellInfo, m_CastID, result, m_customError);
}

void Spell::SendCastResult(Player* caster, SpellInfo const* p_SpellInfo, Guid128 p_CastGuid, SpellCastResult result, SpellCustomErrors customError /*= SPELL_CUSTOM_ERROR_NONE*/)
{
    if (result == SPELL_CAST_OK)
        return;

    WorldPacket l_Data(SMSG_CAST_FAILED, 4 + 4 + 4 + 4 + 1);
    l_Data.appendGuid128(p_CastGuid);
    l_Data << uint32(p_SpellInfo->Id);                                  ///< spellId
    l_Data << uint32(p_SpellInfo->GetSpellXSpellVisualId(caster));      ///< SpellXSpellVisualID
    l_Data << uint32(result);                                           ///< problem

    switch (result)
    {
        case SPELL_FAILED_NO_POWER:
        {
            l_Data << uint32(p_SpellInfo->GetMainPower());              ///< Power type
            l_Data << uint32(0);                                        ///< Arg2 => NULL
            break;
        }
        case SPELL_FAILED_NOT_READY:
        {
            l_Data << uint32(0);                                        ///< unknown (value 1 update cooldowns on client flag)
            l_Data << uint32(0);                                        ///< Arg2 => NULL
            break;
        }
        case SPELL_FAILED_REQUIRES_SPELL_FOCUS:
        {
            l_Data << uint32(p_SpellInfo->RequiresSpellFocus);          ///< SpellFocusObject.db2 id
            l_Data << uint32(0);                                        ///< Arg2 => NULL
            break;
        }
        case SPELL_FAILED_REQUIRES_AREA:                                ///< AreaTable.db2 id
        {
            // hardcode areas limitation case
            switch (p_SpellInfo->Id)
            {
                case 41617:                                             ///< Cenarion Mana Salve
                case 41619:                                             ///< Cenarion Healing Salve
                    l_Data << uint32(3905);
                    break;
                case 41618:                                             ///< Bottled Nethergon Energy
                case 41620:                                             ///< Bottled Nethergon Vapor
                    l_Data << uint32(3842);
                    break;
                case 45373:                                             ///< Bloodberry Elixir
                    l_Data << uint32(4075);
                    break;
                default:                                                ///< default case (don't must be)
                    l_Data << uint32(0);
                    break;
            }
            l_Data << uint32(0);                                        ///< Arg2 => NULL

            break;
        }
        case SPELL_FAILED_TOTEMS:
        {
            l_Data << uint32(p_SpellInfo->Totem[0]);
            l_Data << uint32(p_SpellInfo->Totem[1]);

            break;
        }
        case SPELL_FAILED_TOTEM_CATEGORY:
        {
            l_Data << uint32(p_SpellInfo->TotemCategory[0]);
            l_Data << uint32(p_SpellInfo->TotemCategory[1]);

            break;
        }
        case SPELL_FAILED_EQUIPPED_ITEM_CLASS:
        case SPELL_FAILED_EQUIPPED_ITEM_CLASS_MAINHAND:
        case SPELL_FAILED_EQUIPPED_ITEM_CLASS_OFFHAND:
        {
            l_Data << uint32(p_SpellInfo->EquippedItemClass);           ///< Arg1 => NULL
            l_Data << uint32(p_SpellInfo->EquippedItemSubClassMask);    ///< Arg2 => NULL

            break;
        }
        case SPELL_FAILED_TOO_MANY_OF_ITEM:
        {
            uint32 l_ItemID = 0;

            for (uint8 l_EffectIndex = 0; l_EffectIndex < p_SpellInfo->EffectCount; l_EffectIndex++)
            {
                if (p_SpellInfo->Effects[l_EffectIndex].ItemType)
                    l_ItemID = p_SpellInfo->Effects[l_EffectIndex].ItemType;
            }

            ItemTemplate const* l_ItemTemplate = sObjectMgr->GetItemTemplate(l_ItemID);

            if (l_ItemTemplate)
                l_Data << uint32(l_ItemTemplate->ItemLimitCategory);    ///< Arg1 => ItemLimitCategory
            else
                l_Data << uint32(0);                                    ///< Arg1 => NULL

            l_Data << uint32(0);                                        ///< Arg2 => NULL

            break;
        }
        case SPELL_FAILED_PREVENTED_BY_MECHANIC:
        {
            l_Data << uint32(p_SpellInfo->GetAllEffectsMechanicMask()); ///< Arg1 => required fishing skill
            l_Data << uint32(0);                                        ///< Arg2 => NULL
            break;
        }
        case SPELL_FAILED_NEED_EXOTIC_AMMO:
        {
            l_Data << uint32(p_SpellInfo->EquippedItemSubClassMask);    ///< Arg1 => required fishing skill
            l_Data << uint32(0);                                        ///< Arg2 => NULL
            break;
        }
        case SPELL_FAILED_NEED_MORE_ITEMS:
        {
            l_Data << uint32(0);                                        ///< Arg1 => Item id
            l_Data << uint32(0);                                        ///< Arg2 => Item count?
            break;
        }
        case SPELL_FAILED_MIN_SKILL:
        {
            l_Data << uint32(0);                                        ///< Arg1 => SkillLine.db2 id
            l_Data << uint32(0);                                        ///< Arg2 => required skill value
            break;
        }
        case SPELL_FAILED_FISHING_TOO_LOW:
        {
            l_Data << uint32(0);                                        ///< Arg1 => required fishing skill
            l_Data << uint32(0);                                        ///< Arg2 => NULL
            break;
        }
        case SPELL_FAILED_CUSTOM_ERROR:
        {
            l_Data << uint32(customError);                              ///< Arg1 => Custom error code
            l_Data << uint32(0);                                        ///< Arg2 => NULL
            break;
        }
        case SPELL_FAILED_SILENCED:
        {
            l_Data << uint32(0);                                        ///< Arg1 => Unknown
            l_Data << uint32(0);                                        ///< Arg2 => NULL
            break;
        }
        case SPELL_FAILED_REAGENTS:
        {
            uint32 l_MissingItem = 0;

            for (uint32 l_I = 0; l_I < MAX_SPELL_REAGENTS; l_I++)
            {
                if (p_SpellInfo->Reagent[l_I] <= 0)
                    continue;

                uint32 l_ItemID    = p_SpellInfo->Reagent[l_I];
                uint32 l_ItemCount = p_SpellInfo->ReagentCount[l_I];

                if (!caster->HasItemCount(l_ItemID, l_ItemCount))
                {
                    l_MissingItem = l_ItemID;
                    break;
                }
            }
            if (p_SpellInfo->CurrencyID != 0 && p_SpellInfo->CurrencyCount != 0)
            {
                uint32 l_CurrencyID = p_SpellInfo->CurrencyID;
                uint32 l_CurrencyCount = p_SpellInfo->CurrencyCount;

                if (!caster->HasCurrency(l_CurrencyID, l_CurrencyCount))
                    l_MissingItem = l_CurrencyID;
            }

            l_Data << uint32(l_MissingItem);                            ///< Arg1 => first missing item
            l_Data << uint32(0);                                        ///< Arg2 => NULL
            break;
        }

        // @TODO: SPELL_FAILED_NOT_STANDING
        default:
            l_Data << uint32(0);                                        ///< Arg1 => NULL
            l_Data << uint32(0);                                        ///< Arg2 => NULL
            break;
    }

    caster->GetSession()->SendPacket(&l_Data);
}

void Spell::SendSpellStart()
{
    if (!IsNeedSendToClient())
        return;

    uint32 l_CastFlags   = CAST_FLAG_HAS_TRAJECTORY;
    uint32 l_CastFlagsEx = CastFlagsEx::CAST_FLAG_EX_NONE;

    if (m_CastItemEntry)
        l_CastFlagsEx |= CastFlagsEx::CAST_FLAG_EX_TOY_COOLDOWN;

    uint32 l_SchoolImmunityMask = m_caster->GetSchoolImmunityMask();
    uint32 l_MechanicImmunityMask = m_caster->GetMechanicImmunityMask();

    if (auto creature = m_caster->ToCreature())
        if (creature->GetCreatureTemplate()->MechanicImmuneMask & (1 << (MECHANIC_INTERRUPT - 1)))
            l_MechanicImmunityMask = 3;

    if (l_SchoolImmunityMask || l_MechanicImmunityMask)
        l_CastFlags |= CAST_FLAG_IMMUNITY;

    if (m_triggeredByAuraSpell)
        l_CastFlags |= CAST_FLAG_PENDING;

    bool l_IsHealthPowerSpell = false;
    for (auto itr : m_spellInfo->SpellPowers)
    {
        if (itr->PowerType == POWER_HEALTH)
        {
            l_IsHealthPowerSpell = true;
            break;
        }
    }

    if ((m_caster->IsPlayer() ||
        (m_caster->GetTypeId() == TYPEID_UNIT && m_caster->ToCreature()->isPet()))
        && !l_IsHealthPowerSpell)
        l_CastFlags |= CAST_FLAG_POWER_LEFT_SELF;

    if (m_spellInfo->GetMainPower() == POWER_RUNES)
        l_CastFlags |= CAST_FLAG_NO_GCD;

    if (m_targets.HasTraj())
        l_CastFlags |= CAST_FLAG_ADJUST_MISSILE;

    if (m_spellInfo->HasEffect(SpellEffects::SPELL_EFFECT_LOOT_BONUS))
        l_CastFlags = SpellCastFlags::CAST_FLAG_HAS_TRAJECTORY | SpellCastFlags::CAST_FLAG_NO_GCD;

    WorldPacket data(SMSG_SPELL_START);

    uint32 l_ExtraTargetsCount = m_targets.GetExtraTargetsCount();

    uint64 l_PredicOverrideTarget = 0;
    uint32 l_PredictAmount = 0;
    uint8 l_PredicType = 0;

    uint64 l_CasterGuid1    = m_CastItem ? m_CastItem->GetGUID() : m_caster->GetGUID();
    uint64 l_CasterGuid2    = m_caster->GetGUID();
    uint64 l_TargetGUID     = m_targets.GetObjectTargetGUID();
    uint64 l_TargetItemGUID = m_targets.GetItemTargetGUID();

    // Initialize predicated heal values
    if (m_spellInfo->HasEffect(SPELL_EFFECT_HEAL))
    {
        if (Unit* target = sObjectAccessor->FindUnit(m_targets.GetObjectTargetGUID()))
        {
            l_CastFlags |= CAST_FLAG_HEAL_PREDICTION;
            l_PredicType = 2;
            l_PredicOverrideTarget = target->GetGUID();

            // Guess spell healing amount
            l_PredictAmount = m_caster->CalculateSpellDamage(target, m_spellInfo, 0);
            l_PredictAmount = m_caster->SpellHealingBonusDone(target, m_spellInfo, l_PredictAmount, m_spellInfo->GetEffectIndex(SPELL_EFFECT_HEAL), HEAL);
            l_PredictAmount = target->SpellHealingBonusTaken(m_caster, m_spellInfo, l_PredictAmount, HEAL);
        }
    }

    data.appendPackGUID(l_CasterGuid1);
    data.appendPackGUID(l_CasterGuid2);
    data.appendGuid128(m_CastID);
    data.appendPackGUID(0);
    data << uint32(m_spellInfo->Id);
    data << uint32(m_SpellVisualID);
    data << uint32(l_CastFlags);
    data << uint32(m_casttime);

    data << uint32(0);                      ///< Travel time
    data << float(m_targets.GetSpeed());

    uint32 l_AmmoDisplayID = 0;
    UpdateSpellCastDataAmmo(l_AmmoDisplayID);

    data << uint32(l_AmmoDisplayID);
    data << uint8(0);                       ///< DestLocSpellCastIndex

    data << uint32(l_SchoolImmunityMask);
    data << uint32(l_MechanicImmunityMask);

    data << uint32(l_PredictAmount);
    data << uint8(l_PredicType);
    data.appendPackGUID(l_PredicOverrideTarget);


    data.WriteBits(l_CastFlagsEx, 23);              ///< Cast flag ex
    data.WriteBits(0, 16);                          ///< HitCount
    data.WriteBits(0, 16);                          ///< MissCount
    data.WriteBits(0, 16);                          ///< Miss Count
    data.WriteBits(0, 9);                           ///< Remaining power count
    data.WriteBit(false);                           ///< HasRuneData
    data.WriteBits(l_ExtraTargetsCount, 16);

    /// SpellDest Block in the client
    {
        data.FlushBits();
        data.WriteBits(m_targets.GetTargetMask(), 25);
        data.WriteBit(m_targets.HasSrc());
        data.WriteBit(m_targets.HasDst());
        data.WriteBit(false);
        data.WriteBit(0);                       ///< Unk 7.0.3 21384
        data.WriteBits(0, 7);                   ///< Src target name
        data.FlushBits();

        data.appendPackGUID(l_TargetGUID);
        data.appendPackGUID(l_TargetItemGUID);

        if (m_targets.HasSrc())
        {
            float l_X, l_Y, l_Z;

            if (m_targets.GetSrc()->_transportGUID)
            {
                l_X = m_targets.GetSrc()->_transportOffset.m_positionX;
                l_Y = m_targets.GetSrc()->_transportOffset.m_positionY;
                l_Z = m_targets.GetSrc()->_transportOffset.m_positionZ;
            }
            else
            {
                l_X = m_targets.GetSrc()->_position.m_positionX;
                l_Y = m_targets.GetSrc()->_position.m_positionY;
                l_Z = m_targets.GetSrc()->_position.m_positionZ;
            }

            data.appendPackGUID(m_targets.GetSrc()->_transportGUID);
            data << float(l_X);
            data << float(l_Y);
            data << float(l_Z);
        }

        if (m_targets.HasDst())
        {
            float l_X, l_Y, l_Z;

            if (m_targets.GetDst()->_transportGUID)
            {
                l_X = m_targets.GetDst()->_transportOffset.m_positionX;
                l_Y = m_targets.GetDst()->_transportOffset.m_positionY;
                l_Z = m_targets.GetDst()->_transportOffset.m_positionZ;
            }
            else
            {
                l_X = m_targets.GetDst()->_position.m_positionX;
                l_Y = m_targets.GetDst()->_position.m_positionY;
                l_Z = m_targets.GetDst()->_position.m_positionZ;
            }

            data.appendPackGUID(m_targets.GetDst()->_transportGUID);
            data << float(l_X);
            data << float(l_Y);
            data << float(l_Z);
        }
    }

    for (auto l_Itr : m_targets.GetExtraTargets())
    {
        data.appendPackGUID(l_Itr._transportGUID);
        data << float(l_Itr._position.GetPositionX());
        data << float(l_Itr._position.GetPositionY());
        data << float(l_Itr._position.GetPositionZ());
    }

    m_caster->SendMessageToSet(&data, true);
}

void Spell::SendSpellPendingCast()
{
    uint32 spellID = 0;
    Player* player = m_caster->ToPlayer();
    if (!player)
        return;

    if (auto const* spellPending = sSpellMgr->GetSpellPendingCast(m_spellInfo->Id))
    {
        bool check = false;
        for (std::vector<SpellPendingCast>::const_iterator i = spellPending->begin(); i != spellPending->end(); ++i)
        {
            switch (i->Options)
            {
                case 0: // Check Spec
                {
                    if (player->GetLootSpecId() == i->Check)
                    {
                        spellID = i->PendingID;
                        check = true;
                    }
                    break;
                }
                default:
                    break;
            }

            if (check)
                break;
        }
    }

    if (spellID)
        player->SendScriptCast(spellID);
}

void Spell::SendSpellGo()
{
    // not send invisible spell casting
    if (!IsNeedSendToClient() && m_spellInfo->Id != 178236)
        return;

    uint32 l_CastFlags   = CAST_FLAG_UNKNOWN_9;
    uint32 l_CastFlagsEx = CastFlagsEx::CAST_FLAG_EX_NONE;

    // triggered spells with spell visual != 0
    if (m_triggeredByAuraSpell)
        l_CastFlags |= CAST_FLAG_PENDING;

    if ((m_caster->IsPlayer() ||
        (m_caster->GetTypeId() == TYPEID_UNIT && m_caster->ToCreature()->isPet())))
        l_CastFlags |= CAST_FLAG_POWER_LEFT_SELF; // should only be sent to self, but the current messaging doesn't make that possible

    if (m_caster->IsPlayer() && _triggeredCastFlags & TRIGGERED_IGNORE_SPELL_AND_CATEGORY_CD)
        l_CastFlags |= CAST_FLAG_NO_COOLDOWN;

    if ((m_caster->IsPlayer())
        && (m_caster->getClass() == CLASS_DEATH_KNIGHT)
        && m_spellInfo->GetMainPower() == POWER_RUNES
        && !(_triggeredCastFlags & TRIGGERED_IGNORE_POWER_AND_REAGENT_COST))
    {
        l_CastFlags |= CAST_FLAG_NO_GCD;                    ///< same as in SMSG_SPELL_START
        l_CastFlags |= CAST_FLAG_RUNE_LIST;                 ///< rune cooldowns list
    }

    if (m_spellInfo->HasEffect(SPELL_EFFECT_ACTIVATE_RUNE))
        l_CastFlags |= CAST_FLAG_RUNE_LIST;                 ///< rune cooldowns list

    if (m_targets.HasTraj())
        l_CastFlags |= CAST_FLAG_ADJUST_MISSILE;

    if (!m_spellInfo->StartRecoveryTime)
        l_CastFlags |= CAST_FLAG_NO_GCD;

    if (m_CastItemEntry)
        l_CastFlagsEx |= CastFlagsEx::CAST_FLAG_EX_TOY_COOLDOWN;

    /// Sniffed values - It triggers the bonus roll animation
    if (m_spellInfo->HasEffect(SpellEffects::SPELL_EFFECT_LOOT_BONUS))
    {
        l_CastFlags     = SpellCastFlags::CAST_FLAG_HAS_TRAJECTORY | SpellCastFlags::CAST_FLAG_NO_GCD;
        l_CastFlagsEx   = CastFlagsEx::CAST_FLAG_EX_UNK_5;
    }

    uint32 l_MissTargetsCount = 0;
    uint32 l_HitTargetsCount = 0;

    // Process targets data
    {
        // ---- Miss target ---- //
        for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
        {
            if ((*ihit).effectMask == 0)                  // No effect apply - all immuned add state
                ihit->missCondition = SPELL_MISS_IMMUNE2;
        }

        for (std::list<TargetInfo>::const_iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
            if (ihit->missCondition != SPELL_MISS_NONE)        // Add only miss
                ++l_MissTargetsCount;

        // ---- Hit target ---- //
        for (std::list<TargetInfo>::const_iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
        {
            if ((*ihit).missCondition == SPELL_MISS_NONE)       // Add only hits
            {
                m_channelTargetEffectMask |= ihit->effectMask;
                ++l_HitTargetsCount;
            }
        }

        // Reset m_needAliveTargetMask for non channeled spell
        if (!m_spellInfo->IsChanneled())
            m_channelTargetEffectMask = 0;

        for (std::list<GOTargetInfo>::const_iterator ighit = m_UniqueGOTargetInfo.begin(); ighit != m_UniqueGOTargetInfo.end(); ++ighit)
            ++l_HitTargetsCount;
    }

    bool l_HasRuneData = m_caster->IsPlayer() && m_caster->getClass() == CLASS_DEATH_KNIGHT;

    // @TODO: Find how to trigger it, never find it in sniff (6.0.2)
    bool l_HasSpellCastLogData          = false;// (l_CastFlags & CAST_FLAG_PENDING) == 0 || AttributesCustomEx7 & SPELL_ATTR7_SEND_CAST_LOG_DATA;

    Unit::PowerTypeSet l_RemainingPowerCount  = m_caster->GetUsablePowers();
    uint32 l_TargetPointsCount         = m_targets.GetExtraTargetsCount();

    // predict data are empty in SMSG_SPELL_GO
    uint64 l_PredicOverrideTarget = 0;
    uint32 l_PredictAmount        = 0;
    uint8 l_PredicType            = 0;

    // Guids
    uint64 l_CasterGUID    = m_castItemGUID ? m_castItemGUID : m_caster->GetGUID();
    uint64 l_CasterUnit    = m_caster->GetGUID();
    uint64 l_TargetGUID     = m_targets.GetUnitTarget() ? m_targets.GetUnitTarget()->GetGUID() : 0;
    uint64 l_TargetItemGUID = itemTarget ? itemTarget->GetGUID() : 0;
    uint64 l_OriginalCastID        = 0;

    bool l_HasOrientation = false;
    bool l_HasMapId       = false;
    switch (m_spellInfo->Id)
    {
        case 178236:
            l_HasOrientation = true;
            break;
        default:
            break;
    }

    // Forge the packet !
    WorldPacket l_Data(SMSG_SPELL_GO, 8 * 1024);
    l_Data.appendPackGUID(l_CasterGUID);
    l_Data.appendPackGUID(l_CasterUnit);
    l_Data.appendGuid128(m_CastID);
    l_Data.appendPackGUID(l_OriginalCastID);
    l_Data << uint32(m_spellInfo->Id);
    l_Data << uint32(m_SpellVisualID);
    l_Data << uint32(l_CastFlags);
    l_Data << uint32(getMSTime());


    // Used in CMissile::AdjustAllMissileTrajectoryDurations
    // MissileTrajectory
    {
        l_Data << uint32(0);                        ///< Travel time
        l_Data << float(m_targets.GetElevation());  ///< Pitch
    }

    uint32 l_AmmoDisplayID = 0;
    UpdateSpellCastDataAmmo(l_AmmoDisplayID);

    l_Data << uint32(l_AmmoDisplayID);
    l_Data << uint8(0);                             ///< DestLocSpellCastIndex (see Spell::AddProcessedDestLocSpellCast)

    // Immunities_Struct
    {
        l_Data << int32(0);                         ///< School
        l_Data << int32(0);                         ///< Value
    }

    {
        l_Data << uint32(l_PredictAmount);
        l_Data << uint8(l_PredicType);
        l_Data.appendPackGUID(l_PredicOverrideTarget);
    }

    l_Data.WriteBits(l_CastFlagsEx, 23);                        ///< Cast flag ex
    l_Data.WriteBits(l_HitTargetsCount, 16);
    l_Data.WriteBits(l_MissTargetsCount, 16);
    l_Data.WriteBits(l_MissTargetsCount, 16);                   ///< @todo715 should be MissStatusCount
    l_Data.WriteBits(l_RemainingPowerCount.size(), 9);          ///< Remaining power count
    l_Data.WriteBit(l_HasRuneData);                             ///< HasRuneData
    l_Data.WriteBits(l_TargetPointsCount, 16);
    l_Data.FlushBits();

    for (std::list<TargetInfo>::const_iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
    {
        l_Data.FlushBits();
        if (ihit->missCondition != SPELL_MISS_NONE)
        {
            l_Data.WriteBits(ihit->missCondition, 4);
            if (ihit->missCondition == SPELL_MISS_REFLECT)
                l_Data.WriteBits(ihit->reflectResult, 4);
        }
    }

    /// SpellDest Block in the client
    {
        l_Data.FlushBits();
        l_Data.WriteBits(m_targets.GetTargetMask(), 25);
        l_Data.WriteBit(m_targets.HasSrc()); ///< HasSrcLocation
        l_Data.WriteBit(m_targets.HasDst()); ///< HasDstLocation
        l_Data.WriteBit(l_HasOrientation);
        l_Data.WriteBit(l_HasMapId);
        l_Data.WriteBits(0, 7);             ///< SrcTargetName
        l_Data.FlushBits();

        l_Data.appendPackGUID(l_TargetGUID);
        l_Data.appendPackGUID(l_TargetItemGUID);

        if (m_targets.HasSrc())
        {
            float l_X, l_Y, l_Z;

            if (m_targets.GetSrc()->_transportGUID)
            {
                l_X = m_targets.GetSrc()->_transportOffset.m_positionX;
                l_Y = m_targets.GetSrc()->_transportOffset.m_positionY;
                l_Z = m_targets.GetSrc()->_transportOffset.m_positionZ;
            }
            else
            {
                l_X = m_targets.GetSrc()->_position.m_positionX;
                l_Y = m_targets.GetSrc()->_position.m_positionY;
                l_Z = m_targets.GetSrc()->_position.m_positionZ;
            }

            l_Data.appendPackGUID(m_targets.GetSrc()->_transportGUID);
            l_Data << float(l_X);
            l_Data << float(l_Y);
            l_Data << float(l_Z);
        }

        if (m_targets.HasDst())
        {
            float l_X, l_Y, l_Z;

            if (m_targets.GetDst()->_transportGUID)
            {
                l_X = m_targets.GetDst()->_transportOffset.m_positionX;
                l_Y = m_targets.GetDst()->_transportOffset.m_positionY;
                l_Z = m_targets.GetDst()->_transportOffset.m_positionZ;
            }
            else
            {
                l_X = m_targets.GetDst()->_position.m_positionX;
                l_Y = m_targets.GetDst()->_position.m_positionY;
                l_Z = m_targets.GetDst()->_position.m_positionZ;
            }

            if (l_TargetGUID == l_CasterGUID)
            {
                l_X = m_caster->GetPositionX();
                l_Y = m_caster->GetPositionY();
                l_Z = m_caster->GetPositionZ();
            }

            l_Data.appendPackGUID(m_targets.GetDst()->_transportGUID);
            l_Data << float(l_X);
            l_Data << float(l_Y);
            l_Data << float(l_Z);
        }

        if (l_HasOrientation)
            l_Data << float(m_caster->GetOrientation());

        if (l_HasMapId)
            l_Data << int32(m_caster->GetMapId());
    }

    l_Data.FlushBits();

    // Send hit guid
    {
        // First units ...
        for (std::list<TargetInfo>::const_iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
            if ((*ihit).missCondition == SPELL_MISS_NONE)
                l_Data.appendPackGUID(ihit->targetGUID);

        // And GameObjects
        for (std::list<GOTargetInfo>::const_iterator ighit = m_UniqueGOTargetInfo.begin(); ighit != m_UniqueGOTargetInfo.end(); ++ighit)
            l_Data.appendPackGUID(ighit->targetGUID);
    }

    // Send missed guid
    {
        for (std::list<TargetInfo>::const_iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
            if ((*ihit).missCondition != SPELL_MISS_NONE)
                l_Data.appendPackGUID(ihit->targetGUID);
    }

    // Remaining Power
    for (Unit::PowerTypeSet::const_iterator l_Itr = l_RemainingPowerCount.begin(); l_Itr != l_RemainingPowerCount.end(); l_Itr++)
    {
        Powers l_Power = Powers((*l_Itr));
        l_Data << int32(m_caster->GetPower(l_Power));
        l_Data << int8(l_Power);
    }

    // JamRuneData
    if (l_HasRuneData)
    {
        Player* l_DeathKnight = m_caster->ToPlayer();

        l_Data << uint8(m_runesState);                          ///< Start
        l_Data << uint8(l_DeathKnight->GetRunesState());        ///< Count
        l_Data << uint32(MAX_RUNES);                            ///< Cooldowns length
        l_Data.FlushBits();

        for (uint8 l_I = 0; l_I < MAX_RUNES; ++l_I)
        {
            float l_BaseCooldown    = l_DeathKnight->GetRuneBaseCooldown();
            uint32 l_CooldownDelta  = l_BaseCooldown - l_DeathKnight->GetRuneCooldown(l_I);
            uint8 l_CooldownPkt     = (float(l_CooldownDelta) / l_BaseCooldown) * 255;

            l_Data << uint8(l_CooldownPkt);                        ///< Cooldowns
        }
    }

    // Target Points
    for (auto& l_Itr : m_targets.GetExtraTargets())
    {
        l_Data.appendPackGUID(l_Itr._transportGUID);
        l_Data << float(l_Itr._position.GetPositionX());
        l_Data << float(l_Itr._position.GetPositionY());
        l_Data << float(l_Itr._position.GetPositionZ());
    }

    l_Data.WriteBit(l_HasSpellCastLogData);
    l_Data.FlushBits();

    // JamSpellCastLogData
    if (l_HasSpellCastLogData)
    {
        int64 l_Health      = m_caster->GetHealth();
        int32 l_AttackPower = m_caster->GetTotalAttackPowerValue(m_attackType);
        int32 l_SpellPower  = m_caster->SpellBaseDamageBonusDone(m_spellSchoolMask);

        l_Data << int32(l_Health);                              ///< Health
        l_Data << int32(l_AttackPower);                         ///< Attack power
        l_Data << int32(l_SpellPower);                          ///< Spell power

        uint32 l_PowerDataSize = l_RemainingPowerCount.size();

        l_Data << uint32(l_PowerDataSize);
        for (Unit::PowerTypeSet::const_iterator l_Itr = l_RemainingPowerCount.begin(); l_Itr != l_RemainingPowerCount.end(); l_Itr++)
        {
            Powers l_Power = Powers((*l_Itr));
            l_Data << int32(l_Power);                           ///< Power type
            l_Data << int32(m_caster->GetPower(l_Power));       ///< Amount
            l_Data << int32();                                  ///< Cost
        }

        ///@todo715 and this float dosent exist in WPP
        bool l_HasUnknowFloat = false;

        l_Data.WriteBit(l_HasUnknowFloat);
        if (l_HasUnknowFloat)
            l_Data << float(0.0f);
    }

    m_caster->SendMessageToSet(&l_Data, true);
}

void Spell::SendLogExecute()
{
    if (m_effectExecuteData.empty())
        return;

    WorldPacket l_Data(SMSG_SPELL_EXECUTE_LOG, 1024);

    l_Data.appendPackGUID(m_caster->GetGUID());
    l_Data << uint32(m_spellInfo->Id);
    l_Data << uint32(m_effectExecuteData.size());

    for (auto l_Iter : m_effectExecuteData)
    {
        auto l_EffIndex = l_Iter.first;
        auto l_Helper = l_Iter.second;

        l_Data << uint32(m_spellInfo->Effects[l_EffIndex].Effect);      ///< Effect
        l_Data << uint32(l_Helper.Energizes.size());                    ///< PowerDrainTargets
        l_Data << uint32(l_Helper.ExtraAttacks.size());                 ///< ExtraAttacksTargets
        l_Data << uint32(l_Helper.DurabilityDamages.size());            ///< DurabilityDamageTargets
        l_Data << uint32(l_Helper.Targets.size());                      ///< GenericVictimTargets
        l_Data << uint32(l_Helper.CreatedItems.size());                 ///< TradeSkillTargets
        l_Data << uint32(0);                                            ///< FeedPetTargets

        for (auto l_Energyze : l_Helper.Energizes)
        {
            l_Data.appendPackGUID(l_Energyze.Guid);
            l_Data << uint32(l_Energyze.Value);
            l_Data << uint32(l_Energyze.PowerType);
            l_Data << float(l_Energyze.Multiplier);
        }

        for (auto l_ExtraAttacksTarget : l_Helper.ExtraAttacks)
        {
            l_Data.appendPackGUID(l_ExtraAttacksTarget.Victim);
            l_Data << uint32(l_ExtraAttacksTarget.NumAttacks);
        }

        for (auto l_DurabilityDamage : l_Helper.DurabilityDamages)
        {
            l_Data.appendPackGUID(l_DurabilityDamage.Victim);
            l_Data << uint32(l_DurabilityDamage.ItemID);
            l_Data << uint32(l_DurabilityDamage.Amount);
        }

        for (auto l_Guid : l_Helper.Targets)
            l_Data.appendPackGUID(l_Guid);

        for (auto l_ItemID : l_Helper.CreatedItems)
            l_Data << uint32(l_ItemID);
    }

    l_Data.WriteBit(false); ///< HasLogData
    l_Data.FlushBits();
    m_caster->SendMessageToSet(&l_Data, true);

    m_effectExecuteData.clear();
}

void Spell::ExecuteLogEffectTakeTargetPower(uint8 effIndex, Unit* target, uint32 powerType, uint32 powerTaken, float gainMultiplier)
{
    InitEffectExecuteData(effIndex);
    m_effectExecuteData[effIndex].AddEnergize(target->GetGUID(), gainMultiplier, powerTaken, powerType);
}

void Spell::ExecuteLogEffectExtraAttacks(uint8 effIndex, Unit* victim, uint32 attCount)
{
    InitEffectExecuteData(effIndex);
    m_effectExecuteData[effIndex].AddExtraAttack(victim->GetGUID(), attCount);
}

void Spell::ExecuteLogEffectInterruptCast(uint8 /*effIndex*/, Unit* /*victim*/, uint32 /*spellId*/)
{
    /// Why is commented ?
    /*InitEffectExecuteData(effIndex);
    m_effectExecuteData[effIndex]->append(victim->GetPackGUID());
    *m_effectExecuteData[effIndex] << uint32(spellId);*/
}

void Spell::ExecuteLogEffectDurabilityDamage(uint8 effIndex, Unit* victim, uint32 p_ItemID, uint32 damage)
{
    InitEffectExecuteData(effIndex);
    m_effectExecuteData[effIndex].AddDurabilityDamage(victim->GetGUID(), p_ItemID, damage);
}

void Spell::ExecuteLogEffectOpenLock(uint8 effIndex, Object* obj)
{
    InitEffectExecuteData(effIndex);
    m_effectExecuteData[effIndex].AddTarget(obj->GetGUID());
}

void Spell::ExecuteLogEffectCreateItem(uint8 effIndex, uint32 entry)
{
    InitEffectExecuteData(effIndex);
    m_effectExecuteData[effIndex].AddCreatedItem(entry);
}

void Spell::ExecuteLogEffectDestroyItem(uint8 effIndex, uint32 entry)
{
    InitEffectExecuteData(effIndex);
    m_effectExecuteData[effIndex].AddCreatedItem(entry);
}

void Spell::ExecuteLogEffectSummonObject(uint8 effIndex, WorldObject* obj)
{
    InitEffectExecuteData(effIndex);
    m_effectExecuteData[effIndex].AddTarget(obj->GetGUID());
}

void Spell::ExecuteLogEffectUnsummonObject(uint8 effIndex, WorldObject* obj)
{
    InitEffectExecuteData(effIndex);
    m_effectExecuteData[effIndex].AddTarget(obj->GetGUID());
}

void Spell::ExecuteLogEffectResurrect(uint8 effIndex, Unit* target)
{
    InitEffectExecuteData(effIndex);
    m_effectExecuteData[effIndex].AddTarget(target->GetGUID());
}

void Spell::SendInterrupted(uint32 p_Result)
{
    CallScriptInterruptHandlers(getMSTime());

    WorldPacket l_Data(SMSG_SPELL_FAILURE, 16 + 2 + 1 + 4 + 2);
    l_Data.appendPackGUID(m_caster->GetGUID());
    l_Data.appendGuid128(GetCastGuid());
    l_Data << uint32(m_spellInfo->Id);
    l_Data << uint32(m_spellInfo->GetSpellXSpellVisualId(m_originalCaster));
    l_Data << uint16(p_Result);
    m_caster->SendMessageToSet(&l_Data, true);

    l_Data.Initialize(SMSG_SPELL_FAILED_OTHER, 16 + 2 + 1 + 4 + 1);
    l_Data.appendPackGUID(m_caster->GetGUID());
    l_Data.appendGuid128(GetCastGuid());
    l_Data << uint32(m_spellInfo->Id);
    l_Data << uint32(m_spellInfo->GetSpellXSpellVisualId(m_originalCaster));
    l_Data << uint8(p_Result);
    m_caster->SendMessageToSet(&l_Data, true);

    if (Player* l_Player = m_caster->ToPlayer())
    {
        for (auto const& l_Itr : l_Player->m_Controlled)
        {
            Creature* l_Creature = l_Itr->ToCreature();
            if (!l_Creature)
                continue;

            if (l_Creature->IsAIEnabled)
                l_Creature->AI()->OwnerInterrupted(m_spellInfo);
        }
    }
}

void Spell::SendChannelUpdate(uint32 p_Time)
{
    if (p_Time == 0)
    {
        m_caster->ClearDynamicValue(UNIT_DYNAMIC_FIELD_CHANNEL_OBJECTS);
        m_caster->SetChannelSpellID(nullptr);
    }

    WorldPacket l_Data(SMSG_SPELL_CHANNEL_UPDATE, 16 + 2 + 4);
    l_Data.appendPackGUID(m_caster->GetGUID());
    l_Data << uint32(p_Time);

    m_caster->SendMessageToSet(&l_Data, true);
}

void Spell::SendChannelStart(uint32 p_Duration)
{
    bool l_HaveSpellTargetedHealPrediction = false;
    bool l_HaveSpellChannelStartInterruptImmunities = false;

    WorldPacket l_Data(SMSG_SPELL_CHANNEL_START, (16 + 4 + 4 + 1) + (l_HaveSpellTargetedHealPrediction ? 4 + 1 + 16 + 16 : 0) + (l_HaveSpellChannelStartInterruptImmunities ? 4 + 4 : 0));
    l_Data.appendPackGUID(m_caster->GetGUID());
    l_Data << uint32(m_spellInfo->Id);
    l_Data << uint32(m_SpellVisualID);
    l_Data << uint32(p_Duration);
    l_Data.WriteBit(l_HaveSpellChannelStartInterruptImmunities);
    l_Data.WriteBit(l_HaveSpellTargetedHealPrediction);
    l_Data.FlushBits();

    if (l_HaveSpellChannelStartInterruptImmunities)
    {
        l_Data << uint32(0);                  ///< SchoolImmunities
        l_Data << uint32(0);                  ///< Immunities
    }

    if (l_HaveSpellTargetedHealPrediction)
    {
        l_Data.appendPackGUID(0);             ///< TargetGUID
        l_Data << uint32(0);                  ///< Points
        l_Data << uint8(0);                   ///< Type
        l_Data.appendPackGUID(0);             ///< BeaconGUID
    }

    m_caster->SendMessageToSet(&l_Data, true);

    m_timer = p_Duration;

    for (TargetInfo const& l_Target : m_UniqueTargetInfo)
    {
        //if (m_UniqueTargetInfo.size() != 1 && l_Target.targetGUID == m_caster->GetGUID())
        //    continue;

        m_caster->AddChannelObject(Guid64To128(l_Target.targetGUID));
    }

    for (GOTargetInfo const& l_Target : m_UniqueGOTargetInfo)
    {
        m_caster->AddChannelObject(Guid64To128(l_Target.targetGUID));
    }

    /// 101546 Spinning Crane Kick
    if (m_spellInfo->Id != 101546)
        m_caster->SetChannelSpellID(m_spellInfo->Id, m_SpellVisualID);
}

void Spell::SendResurrectRequest(Player* p_Target)
{
    /// Get ressurector name for creature resurrections, otherwise packet will be not accepted
    /// for player resurrections the name is looked up by guid

    uint32 l_ResurrectOffererVirtualRealmAddress = g_RealmID;
    char const* l_RessurectorName = m_caster->IsPlayer() ? "" : m_caster->GetNameForLocaleIdx(p_Target->GetSession()->GetSessionDbLocaleIndex());

    WorldPacket l_Data(SMSG_RESURRECT_REQUEST, 16 + 2 + 4 + 4 + 4 + 1 + strlen(l_RessurectorName) + 1);

    l_Data.appendPackGUID(m_caster->GetGUID());              ///< ResurrectOffererGUID
    l_Data << uint32(l_ResurrectOffererVirtualRealmAddress); ///< ResurrectOffererVirtualRealmAddress
    l_Data << uint32(0);                                     ///< PetNumber
    l_Data << uint32(m_spellInfo->Id);                       ///< SpellID

    l_Data.WriteBits(strlen(l_RessurectorName), 11);         ///< Name
    l_Data.WriteBit(false);                                  ///< UseTimer
    l_Data.WriteBit(m_caster->GetSpellModOwner() ? 0 : 1);   ///< Sickness
    l_Data.FlushBits();

    l_Data.WriteString(l_RessurectorName);
    p_Target->GetSession()->SendPacket(&l_Data);
}

void Spell::TakeCastItem()
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    m_CastItem = m_caster->ToPlayer()->GetItemByGuid(m_castItemGUID);
    if (!m_CastItem)
        return;

    // not remove cast item at triggered spell (equipping, weapon damage, etc)
    if (_triggeredCastFlags & TRIGGERED_IGNORE_CAST_ITEM)
        return;

    ItemTemplate const* proto = m_CastItem->GetTemplate();

    if (!proto)
    {
        // This code is to avoid a crash
        // I'm not sure, if this is really an error, but I guess every item needs a prototype
        return;
    }

    bool expendable = false;
    bool withoutCharges = false;

    for (int i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
    {
        if (proto->Spells[i].SpellId)
        {
            // item has limited charges
            if (proto->Spells[i].SpellCharges)
            {
                if (proto->Spells[i].SpellCharges < 0)
                    expendable = true;

                int32 charges = m_CastItem->GetSpellCharges(i);

                // item has charges left
                if (charges)
                {
                    (charges > 0) ? --charges : ++charges;  // abs(charges) less at 1 after use
                    if (proto->Stackable == 1)
                        m_CastItem->SetSpellCharges(i, charges);
                    m_CastItem->SetState(ITEM_CHANGED, (Player*)m_caster);
                }

                // all charges used
                withoutCharges = (charges == 0);
            }
        }
    }

    if (expendable && withoutCharges)
    {
        uint32 count = 1;
        m_caster->ToPlayer()->DestroyItemCount(m_CastItem, count, true);

        // prevent crash at access to deleted m_targets.GetItemTarget
        if (m_CastItem == m_targets.GetItemTarget())
            m_targets.SetItemTarget(NULL);

        m_CastItem = NULL;
    }
}

void Spell::TakeComboPoints()
{
    // Don't take power if the spell is cast while .cheat power is enabled.
    if (m_caster->IsPlayer())
    {
        if (m_caster->ToPlayer()->GetCommandStatus(CHEAT_POWER))
            return;
    }

    if (m_spellInfo->SpellPowers.empty())
        return;

    Powers powerType = Powers::POWER_COMBO_POINT;
    if (!m_powerCost[POWER_TO_INDEX(powerType)])
        return;

    /// HACK: extremely ugly hack, maybe there is another way.
    /// If the spell misses no combo points should be taken.
    if (m_caster->IsPlayer())
    {
        if (uint64 targetGUI = m_targets.GetUnitTargetGUID())
            for (const TargetInfo& info : m_UniqueTargetInfo)
                if (info.targetGUID == targetGUI)
                    if (info.missCondition != SPELL_MISS_NONE)
                        return;
    }

    m_caster->ModifyPower(powerType, -m_powerCost[POWER_TO_INDEX(powerType)]);
}

void Spell::TakePower()
{
    if (m_CastItem || m_triggeredByAuraSpell)
        return;

    // Don't take power if the spell is cast while .cheat power is enabled.
    if (m_caster->IsPlayer())
    {
        if (m_caster->ToPlayer()->GetCommandStatus(CHEAT_POWER))
            return;
    }
    else if (m_caster->GetEntry() == eMonkPets::Earth || m_caster->GetEntry() == eMonkPets::Fire)
        return;

    if (m_spellInfo->Id == 196104)
        CallScriptTakePowersHandlers(POWER_MANA, m_powerCost[POWER_MANA]);

    if (m_spellInfo->SpellPowers.empty())
        return;

    for (auto itr : m_spellInfo->SpellPowers)
    {
        Powers powerType = Powers(itr->PowerType);
        bool hit = true;
        if (m_caster->IsPlayer())
        {
            if (powerType == Powers::POWER_COMBO_POINT)
            {
                CallScriptTakePowersHandlers(powerType, m_powerCost[POWER_TO_INDEX(powerType)]);
                continue;
            }

            if (powerType == POWER_HOLY_POWER || powerType == POWER_ENERGY || powerType == POWER_RUNES || powerType == POWER_CHI ||
                powerType == POWER_INSANITY || powerType == POWER_SOUL_SHARDS)
            {
                if (uint64 targetGUID = m_targets.GetUnitTargetGUID())
                {
                    for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
                    {
                        if (ihit->targetGUID == targetGUID)
                        {
                            if (ihit->missCondition != SPELL_MISS_NONE)
                            {
                                hit = false;
                                //lower spell cost on fail (by talent aura)
                                if (Player* modOwner = m_caster->ToPlayer()->GetSpellModOwner())
                                    modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_SPELL_COST_REFUND_ON_FAIL, m_powerCost[POWER_TO_INDEX(powerType)]);
                            }
                            break;
                        }
                    }
                }
            }
        }

        if (powerType == POWER_RUNES)
            TakeRunePower(hit);

        // Check requiredAura for power which area restricted to this
        if (itr->RequiredAuraSpellId && !m_caster->HasAura(itr->RequiredAuraSpellId))
            continue;

        // health as power used
        if (powerType == POWER_HEALTH)
        {
            m_caster->ModifyHealth(-(int32)m_powerCost[POWER_TO_INDEX(powerType)]);
            continue;
        }

        if (powerType >= MAX_POWERS)
        {
            sLog->outAshran("Spel::TakePower -> Unknown power type [%u]", powerType);
            continue;
        }

        int32 pct = 20;
        if (powerType == POWER_RUNIC_POWER)
            pct = 10;

        /// WoD Custom Script - Arcane Blast mana cost with Arcane Charge and Improved Arcane Power
        if (powerType == POWER_MANA && m_spellInfo && m_spellInfo->Id == 30451 && m_caster->HasAura(157604))
        {
            /// Arcane Charge increase mana cost of Arcane Blast
            if (Aura* l_ArcaneCharge = m_caster->GetAura(36032))
            {
                /// Base Arcane Blast mana cost
                int32 l_PowerCost = 3200;
                /// For every stack of arcane charge +100% of mana cost
                uint8 l_ArcaneChargeStacks = l_ArcaneCharge->GetStackAmount() + 1;
                l_PowerCost = int32(l_PowerCost * l_ArcaneChargeStacks);
                /// Reduce mana cost for 10% by Improved Arcane Power just if we have Arcane Power buff
                if (m_caster->HasAura(12042))
                    l_PowerCost = CalculatePct(l_PowerCost, 90);

                m_powerCost[0] = l_PowerCost;
            }
        }

        CallScriptTakePowersHandlers(powerType, m_powerCost[POWER_TO_INDEX(powerType)]);

        if (!m_powerCost[POWER_TO_INDEX(powerType)])
            continue;

        if (hit)
            m_caster->ModifyPower(powerType, -m_powerCost[POWER_TO_INDEX(powerType)]);
        else
            m_caster->ModifyPower(powerType, -CalculatePct(m_powerCost[POWER_TO_INDEX(powerType)], pct)); // Refund 80% of power on fail 4.x
    }
}

void Spell::TakeAmmo()
{
    if (m_attackType == WeaponAttackType::RangedAttack && m_caster->IsPlayer())
    {
        Item* pItem = m_caster->ToPlayer()->GetWeaponForAttack(WeaponAttackType::RangedAttack);

        // wands don't have ammo
        if (!pItem  || pItem->CantBeUse() || pItem->GetTemplate()->SubClass == ITEM_SUBCLASS_WEAPON_WAND)
            return;

        if (pItem->GetTemplate()->InventoryType == INVTYPE_THROWN)
        {
            if (pItem->GetMaxStackCount() == 1)
            {
                // decrease durability for non-stackable throw weapon
                m_caster->ToPlayer()->DurabilityPointLossForEquipSlot(EQUIPMENT_SLOT_MAINHAND);
            }
            else
            {
                // decrease items amount for stackable throw weapon
                uint32 count = 1;
                m_caster->ToPlayer()->DestroyItemCount(pItem, count, true);
            }
        }
    }
}

SpellCastResult Spell::CheckRuneCost(uint32 p_RuneCostID)
{
    bool l_RunesOK = false;

    for (auto l_Itr : m_spellInfo->SpellPowers)
    {
        if (l_Itr->PowerType == POWER_RUNES)
        {
            l_RunesOK = false;
            break;
        }
        else if (l_Itr->PowerType != POWER_RUNES)
            l_RunesOK = true;
    }

    if (l_RunesOK || !p_RuneCostID)
        return SPELL_CAST_OK;

    Player* l_Player = m_caster->ToPlayer();
    if (!l_Player)
        return SPELL_CAST_OK;

    if (l_Player->getClass() != CLASS_DEATH_KNIGHT)
        return SPELL_CAST_OK;

    int32 l_ReadyRunes = 0;
    for (int32 i = 0; i < l_Player->GetMaxPower(POWER_RUNES); ++i)
    {
        if (l_Player->GetRuneCooldown(i) == 0)
            ++l_ReadyRunes;
    }

    uint8 l_RunesCost = m_powerCost[POWER_RUNES];

    if (l_ReadyRunes < l_RunesCost)
        return SPELL_FAILED_NO_POWER;   // not sure if result code is correct

    return SPELL_CAST_OK;
}

void Spell::TakeRunePower(bool didHit)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER || m_caster->getClass() != CLASS_DEATH_KNIGHT)
        return;

    ///< Howling Blast shouldn't take any runes with Rime
    if (m_spellInfo->Id == 49184 && m_caster->HasAura(59052))
        return;

    Player* player = m_caster->ToPlayer();

    m_runesState = player->GetRunesState();                 // store previous state
    uint8 _runesCost = m_powerCost[POWER_RUNES];

    sLog->outDebug(LOG_FILTER_SPELLS_AURAS, "TakeRunePower _runesCost %u", _runesCost);

    for (uint8 i = 0; i < MAX_RUNES; ++i)
    {
        if (_runesCost && !player->GetRuneCooldown(i))
        {
            uint32 cooldown = player->GetRuneBaseCooldown();

            player->SetRuneCooldown(i, cooldown, true);
            _runesCost--;

            sLog->outDebug(LOG_FILTER_SPELLS_AURAS, "TakeRunePower i %u", i);
        }
    }
}

void Spell::TakeReagents()
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    ItemTemplate const* castItemTemplate = m_CastItem ? m_CastItem->GetTemplate() : NULL;

    // do not take reagents for these item casts
    if (castItemTemplate && castItemTemplate->Flags & int32(ItemFlags::NO_REAGENT_COST))
        return;

    Player* p_caster = m_caster->ToPlayer();
    if (p_caster->CanNoReagentCast(m_spellInfo))
        return;

    for (uint32 x = 0; x < MAX_SPELL_REAGENTS; ++x)
    {
        if (m_spellInfo->Reagent[x] <= 0)
            continue;

        uint32 itemid = m_spellInfo->Reagent[x];
        uint32 itemcount = m_spellInfo->ReagentCount[x];

        // if CastItem is also spell reagent
        if (castItemTemplate && castItemTemplate->ItemId == itemid)
        {
            for (int s = 0; s < MAX_ITEM_PROTO_SPELLS; ++s)
            {
                // CastItem will be used up and does not count as reagent
                int32 charges = m_CastItem->GetSpellCharges(s);
                if (castItemTemplate->Spells[s].SpellCharges < 0 && abs(charges) < 2)
                {
                    ++itemcount;
                    break;
                }
            }

            m_CastItem = NULL;
        }

        // if GetItemTarget is also spell reagent
        if (m_targets.GetItemTargetEntry() == itemid)
            m_targets.SetItemTarget(NULL);

        p_caster->DestroyItemCount(itemid, itemcount, true);
    }

    if (m_spellInfo->CurrencyID && m_spellInfo->CurrencyCount > 0)
        p_caster->ModifyCurrency(m_spellInfo->CurrencyID, -int32(m_spellInfo->CurrencyCount));
}

void Spell::HandleThreatSpells()
{
    if (m_UniqueTargetInfo.empty())
        return;

    if ((AttributesCustomEx  & SPELL_ATTR1_NO_THREAT) ||
        (AttributesCustomEx3 & SPELL_ATTR3_NO_INITIAL_AGGRO))
        return;

    float threat = 0.0f;
    if (SpellThreatEntry const* threatEntry = sSpellMgr->GetSpellThreatEntry(m_spellInfo->Id))
    {
        if (threatEntry->apPctMod != 0.0f)
            threat += threatEntry->apPctMod * m_caster->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack);

        threat += threatEntry->flatMod;
    }
    else if ((AttributesCustomCu & SPELL_ATTR0_CU_NO_INITIAL_THREAT) == 0)
        threat += m_spellInfo->SpellLevel;

    // past this point only multiplicative effects occur
    if (threat == 0.0f)
        return;

    // since 2.0.1 threat from positive effects also is distributed among all targets, so the overall caused threat is at most the defined bonus
    threat /= m_UniqueTargetInfo.size();

    for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
    {
        if (ihit->missCondition != SPELL_MISS_NONE)
            continue;

        Unit* target = ObjectAccessor::GetUnit(*m_caster, ihit->targetGUID);
        if (!target)
            continue;

        CallScriptCalculateThreatHandlers(target, threat);

        // positive spells distribute threat among all units that are in combat with target, like healing
        if (m_spellInfo->_IsPositiveSpell())
            target->getHostileRefManager().threatAssist(m_caster, threat, m_spellInfo);
        // for negative spells threat gets distributed among affected targets
        else
        {
            if (!target->CanHaveThreatList())
                continue;

            target->AddThreat(m_caster, threat, m_spellInfo->GetSchoolMask(), m_spellInfo);
        }
    }
}

void Spell::HandleHolyPower(Player* caster)
{
    if (!caster)
        return;

    /// Don't remove power twice
    if (m_spellInfo->Id == 85256  || ///< Templar's Verdict
        m_spellInfo->Id == 114163 || ///< Eternal Flame
        m_spellInfo->Id == 85222  || ///< Light of Dawn
        m_spellInfo->Id == 53600 )   ///< Shield of the Righteous
        return;

    bool hit = true;
    Player* modOwner = caster->GetSpellModOwner();
    m_powerCost[POWER_TO_INDEX(POWER_HOLY_POWER)] = caster->GetPower(POWER_HOLY_POWER); // Always use all the holy power we have
    if (!m_powerCost[POWER_TO_INDEX(POWER_HOLY_POWER)] || !modOwner)
        return;

    if (uint64 targetGUID = m_targets.GetUnitTargetGUID())
    {
        for (std::list<TargetInfo>::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
        {
            if (ihit->targetGUID == targetGUID)
            {
                if (ihit->missCondition != SPELL_MISS_NONE && ihit->missCondition != SPELL_MISS_MISS)
                    hit = false;
                break;
            }
        }

        // The spell did hit the target, apply aura cost mods if there are any.
        if (hit)
        {
            modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_COST, m_powerCost[POWER_TO_INDEX(POWER_HOLY_POWER)]);
            m_caster->ModifyPower(POWER_HOLY_POWER, -m_powerCost[POWER_TO_INDEX(POWER_HOLY_POWER)]);
        }
    }
}

void Spell::HandleEffects(Unit* p_UnitTarget, Item* p_ItemTarget, GameObject* p_GOTarget, AreaTrigger* p_AreaTrigger, uint32 p_I, SpellEffectHandleMode p_Mode)
{
    effectHandleMode = p_Mode;
    unitTarget       = p_UnitTarget;
    itemTarget       = p_ItemTarget;
    gameObjTarget    = p_GOTarget;
    m_AreaTrigger    = p_AreaTrigger;
    destTarget       = &m_destTargets[p_I]._position;

    uint32 l_Effect = m_spellInfo->Effects[p_I].Effect;

    if (m_CurrentLogDamageStream && unitTarget && p_Mode == SPELL_EFFECT_HANDLE_LAUNCH_TARGET)
        *m_CurrentLogDamageStream << "Spell " << m_spellInfo->Id << " Eff " << p_I << " caster " << m_caster->GetGUIDLow() << " target " << unitTarget->GetGUIDLow() << std::endl;

    damage = CalculateDamage(p_I, unitTarget, false, m_CurrentLogDamageStream);

    if (m_CurrentLogDamageStream)
        *m_CurrentLogDamageStream << " basedmg : " << damage << std::endl;

    bool l_PreventDefault = CallScriptEffectHandlers((SpellEffIndex)p_I, p_Mode);
    if (!l_PreventDefault)
        l_PreventDefault = CheckEffFromDummy(p_UnitTarget, p_I);

    if (!l_PreventDefault && l_Effect < TOTAL_SPELL_EFFECTS)
        (this->*SpellEffects[l_Effect])((SpellEffIndex)p_I);

    if (p_Mode == SpellEffectHandleMode::SPELL_EFFECT_HANDLE_HIT || p_Mode == SpellEffectHandleMode::SPELL_EFFECT_HANDLE_HIT_TARGET)
    {
        if (m_targets.HasDst() && m_caster->GetTypeId() == TypeID::TYPEID_UNIT && m_caster->IsAIEnabled)
            m_caster->ToCreature()->AI()->SpellHitDest(m_targets.GetDst(), m_spellInfo, p_Mode);
    }
}

bool Spell::CheckEffFromDummy(Unit* target, uint32 eff)
{
    bool prevent = false;
    if (std::vector<SpellAuraDummy> const* spellAuraDummy = sSpellMgr->GetSpellAuraDummy(m_spellInfo->Id))
    {
        for (std::vector<SpellAuraDummy>::const_iterator itr = spellAuraDummy->begin(); itr != spellAuraDummy->end(); ++itr)
        {
            Unit* _caster = m_caster;
            Unit* _targetAura = m_caster;
            bool check = false;

            if (itr->TargetAura == 1 && _caster->ToPlayer()) //get target pet
            {
                if (Pet* pet = _caster->ToPlayer()->GetPet())
                    _targetAura = (Unit*)pet;
            }
            if (itr->TargetAura == 2) //get target owner
            {
                if (Unit* owner = _caster->GetOwner())
                    _targetAura = owner;
            }
            if (itr->TargetAura == 3) //get target
                _targetAura = target;

            if (!_targetAura)
                _targetAura = _caster;

            switch (itr->Options)
            {
                case SPELL_DUMMY_MOD_EFFECT_MASK: //4
                {
                    if (itr->Aura > 0 && !_targetAura->HasAura(itr->Aura))
                        continue;
                    if (itr->Aura < 0 && _targetAura->HasAura(abs(itr->Aura)))
                        continue;

                    if (itr->SpellDummyID > 0 && !_targetAura->HasAura(itr->SpellDummyID))
                    {
                        if (itr->EffectMask & (1 << eff))
                            prevent = true;
                        check = true;
                    }
                    if (itr->SpellDummyID < 0 && _targetAura->HasAura(abs(itr->SpellDummyID)))
                    {
                        if (itr->EffectMask & (1 << eff))
                            prevent = true;
                        check = true;
                    }

                    break;
                }
            }

            if (check && itr->RemoveAura)
                _caster->RemoveAurasDueToSpell(itr->RemoveAura);
        }
    }

    return prevent;
}

SpellCastResult Spell::CheckCast(bool strict)
{
    /// Hack fixing Psyfiend taking heal + Absorb
    if ((m_spellInfo->IsHealingSpell() || m_spellInfo->Id == 17 || m_spellInfo->Id == 152118 || m_spellInfo->Id == 236181 || m_spellInfo->Id == 211681 ||
        m_spellInfo->Id == 199954 || m_spellInfo->Id == 207400) && m_targets.GetUnitTarget() && m_targets.GetUnitTarget()->GetEntry() == 101398)
        return SPELL_FAILED_DONT_REPORT;

    // Hack fixing Skulloc LOS issue with the bridge sequence
    if (m_spellInfo->Id == 168539 || m_spellInfo->Id == 168540)
        return SPELL_CAST_OK;

    /// Hack fix for the Arcway
    if (m_spellInfo->Id == 225673)
        return SPELL_CAST_OK;

    /// Hack fix for Dead From Above
    if (m_caster->m_SpellHelper.GetBool(eSpellHelpers::BlockCPGain) && (m_spellInfo->Id == 32645 || m_spellInfo->Id == 2098 || m_spellInfo->Id == 196819))
        return SPELL_CAST_OK;

    if (m_spellInfo->IsHealingSpell() && GetUnitTarget() && GetUnitTarget()->isTotem())
        return SPELL_FAILED_IMMUNE;

    if (InstanceScript* l_Instance = m_caster->GetInstanceScript())
    {
        if (m_caster->IsPlayer() && GetUnitTarget() && GetUnitTarget()->IsPlayer())
        {
            if (!l_Instance->IsInteractingPossible(m_caster, GetUnitTarget(), m_spellInfo))
                return SPELL_FAILED_BAD_TARGETS;
        }
    }

    /// Hack fix for PvP talents, must be disabled in PvE maps
    if ((m_spellInfo->m_IsPVPTalent || m_spellInfo->Id == 134735) && (m_caster->GetMap()->IsDungeon() || m_caster->GetMap()->IsRaid() || m_caster->GetMapId() == 1779))
    {
        m_caster->RemoveAura(134735); ///< PvP rules enabled
        return SPELL_FAILED_DONT_REPORT;
    }

    /// Hacky fix for Herb Gathering exploit
    if (m_spellInfo->Id == 2368 && m_targets.GetGOTarget() && m_targets.GetGOTarget()->GetEntry() == 209059)
        return SPELL_FAILED_BAD_TARGETS;

    // Custom Spell_failed
    if (m_spellInfo->IsCustomCastCanceled(m_caster))
        return SPELL_FAILED_DONT_REPORT;

    /// Combat Resurrection spell
    if (m_spellInfo->IsBattleResurrection())
    {
        if (InstanceScript* l_InstanceScript = m_caster->GetInstanceScript())
        {
            if (!l_InstanceScript->CanUseCombatResurrection())
                return SPELL_FAILED_IN_COMBAT_RES_LIMIT_REACHED;
        }

        if (m_targets.GetUnitTarget() != nullptr)
        {
            if (Player* l_Target = m_targets.GetUnitTarget()->ToPlayer())
            {
                if (l_Target->IsRessurectRequested())
                    return SPELL_FAILED_TARGET_HAS_RESURRECT_PENDING;
            }
        }
    }

    /// Check specialization
    if (!IsTriggered() && !sWorld->getBoolConfig(CONFIG_DISABLE_SPELL_SPECIALIZATION_CHECK) && !IsDarkSimulacrum())
    {
        if (Player* l_Player = m_caster->ToPlayer())
        {
            bool l_ByPassSpec = false;

            auto l_Itr = l_Player->GetSpellMap().find(m_spellInfo->Id);
            if (l_Itr != l_Player->GetSpellMap().end() && l_Itr->second->ByPassSpec)
                l_ByPassSpec = true;

            uint32 l_CasterSpecialization = l_Player->GetActiveSpecializationID();

            if (!m_spellInfo->SpecializationIdList.empty())
            {
                bool l_Found = false;
                for (uint32 l_Specialization : m_spellInfo->SpecializationIdList)
                {
                    if (l_CasterSpecialization == l_Specialization)
                    {
                        l_Found = true;
                        break;
                    }
                }

                if (!l_Found && !l_ByPassSpec)
                    return SpellCastResult::SPELL_FAILED_SPELL_UNAVAILABLE;
            }
        }
    }

    // Check death state
    if (!m_caster->isAlive() && !(AttributesCustom & SPELL_ATTR0_PASSIVE) && !((AttributesCustom & SPELL_ATTR0_CASTABLE_WHILE_DEAD) || (IsTriggered() && !m_triggeredByAuraSpell)))
        return SPELL_FAILED_CASTER_DEAD;

    if (m_spellInfo->HasEffect(SPELL_EFFECT_RESURRECT_WITH_AURA) && m_caster->GetInstanceScript() && m_caster->GetInstanceScript()->IsEncounterInProgress())
        return SPELL_FAILED_TARGET_IN_COMBAT;

    /// Profession spells shouldn't be castable when you're in combat
    if (m_spellInfo->Attributes & SPELL_ATTR0_TRADESPELL && m_caster->isInCombat())
        return SPELL_FAILED_AFFECTING_COMBAT;

    /// Cooldown
    Player* l_PlayerForCooldown = m_caster->ToPlayer();
    // Handle pet cooldowns here if needed instead of in PetAI to avoid hidden cooldown restarts
    Creature* l_CreatureCaster = m_caster->ToCreature();
    if (l_CreatureCaster)
    {
        if (l_CreatureCaster->HasSpellCooldown(m_spellInfo->Id))
            return SPELL_FAILED_NOT_READY;
        else if (!l_CreatureCaster->isPet() && !l_CreatureCaster->isGuardian() && l_CreatureCaster->GetSpellModOwner() && l_CreatureCaster->GetSpellModOwner()->IsPlayer())
            l_PlayerForCooldown = l_CreatureCaster->GetSpellModOwner()->ToPlayer();
    }

    // Check cooldowns to prevent cheating
    if (l_PlayerForCooldown && !(AttributesCustom & SPELL_ATTR0_PASSIVE))
    {
        SpellInfo const* l_SpellInfo = nullptr;
        if (m_caster->HasAura(46924)) ///< Bladestorm
            l_SpellInfo = sSpellMgr->GetSpellInfo(46924);

        Unit::AuraEffectList const& l_AuraEffects = m_caster->GetAuraEffectsByType(SPELL_AURA_ALLOW_ONLY_ABILITY);
        for (Unit::AuraEffectList::const_iterator l_AuraEffect = l_AuraEffects.begin(); l_AuraEffect != l_AuraEffects.end(); ++l_AuraEffect)
        {
            if (l_SpellInfo && (*l_AuraEffect)->IsAffectingSpell(m_spellInfo))
                _triggeredCastFlags = TriggerCastFlags(uint32(_triggeredCastFlags) | TRIGGERED_IGNORE_CASTER_AURASTATE);
        }

        if (!(_triggeredCastFlags & TRIGGERED_IGNORE_CASTER_AURASTATE) && !(_triggeredCastFlags & TRIGGERED_IGNORE_SPELL_AND_CATEGORY_CD) &&
            (!(AttributesCustomEx9 & SPELL_ATTR9_CASTABLE_WHILE_CAST_IN_PROGRESS) && l_PlayerForCooldown->HasAura(51690)))
            return SPELL_FAILED_SPELL_IN_PROGRESS;

        //can cast triggered (by aura only?) spells while have this flag
        if (!(_triggeredCastFlags & TRIGGERED_IGNORE_CASTER_AURASTATE) && !(_triggeredCastFlags & TRIGGERED_IGNORE_SPELL_AND_CATEGORY_CD) && l_PlayerForCooldown->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_ALLOW_ONLY_ABILITY) &&
            m_caster->GetCurrentSpell(CURRENT_GENERIC_SPELL) &&
            (!(AttributesCustomEx9 & SPELL_ATTR9_CASTABLE_WHILE_CAST_IN_PROGRESS) || GetSpellInfo()->CalcCastTime(m_caster)))
            return SPELL_FAILED_SPELL_IN_PROGRESS;

        if (!GetSpellValue(SpellValueMod::SPELLVALUE_IGNORE_CD) && l_PlayerForCooldown->HasSpellCooldown(m_spellInfo->Id) && !l_PlayerForCooldown->HasAuraTypeWithAffectMask(SPELL_AURA_ALLOW_CAST_WHILE_IN_COOLDOWN, m_spellInfo))
        {
            if (m_triggeredByAuraSpell)
                return SPELL_FAILED_DONT_REPORT;
            else
                return SPELL_FAILED_NOT_READY;
        }

        if (!l_PlayerForCooldown->HasCharge(m_spellInfo->ChargeCategoryEntry))
            return m_triggeredByAuraSpell ? SPELL_FAILED_DONT_REPORT : SPELL_FAILED_NOT_READY;

        // check if we are using a potion in combat for the 2nd+ time. Cooldown is added only after caster gets out of combat
        if (!IsTriggered() && l_PlayerForCooldown->GetLastPotionItemId(m_spellInfo) && m_CastItem && (m_CastItem->IsPotion() || m_spellInfo->IsCooldownStartedOnEvent()))
            return SPELL_FAILED_NOT_READY;
    }

    if (AttributesCustomEx7 & SPELL_ATTR7_IS_CHEAT_SPELL && !m_caster->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_ALLOW_CHEAT_SPELLS))
    {
        m_customError = SPELL_CUSTOM_ERROR_GM_ONLY;
        return SPELL_FAILED_CUSTOM_ERROR;
    }

    // Check global cooldown
    if (strict && !(_triggeredCastFlags & TRIGGERED_IGNORE_GCD) && !(_triggeredCastFlags & TRIGGERED_IGNORE_SPELL_AND_CATEGORY_CD) && HasGlobalCooldown() && !m_spellInfo->DoesIgnoreGlobalCooldown(m_caster))
        return SPELL_FAILED_NOT_READY;

    // only triggered spells can be processed an ended battleground
    if (!IsTriggered() && m_caster->IsPlayer())
        if (Battleground* bg = m_caster->ToPlayer()->GetBattleground())
            if (bg->GetStatus() == STATUS_WAIT_LEAVE)
                return SPELL_FAILED_DONT_REPORT;

    if (m_caster->IsPlayer() && VMAP::VMapFactory::createOrGetVMapManager()->isLineOfSightCalcEnabled())
    {
        if (AttributesCustom & SPELL_ATTR0_OUTDOORS_ONLY &&
            !m_caster->IsOutdoors())
            return SPELL_FAILED_ONLY_OUTDOORS;

        if (AttributesCustom & SPELL_ATTR0_INDOORS_ONLY &&
                m_caster->IsOutdoors())
            return SPELL_FAILED_ONLY_INDOORS;
    }

    // only check at first call, Stealth auras are already removed at second call
    // for now, ignore triggered spells
    if (strict && !(_triggeredCastFlags & TRIGGERED_IGNORE_SHAPESHIFT))
    {
        bool checkForm = true;
        // Ignore form req aura
        Unit::AuraEffectList const& ignore = m_caster->GetAuraEffectsByType(SPELL_AURA_MOD_IGNORE_SHAPESHIFT);
        for (Unit::AuraEffectList::const_iterator i = ignore.begin(); i != ignore.end(); ++i)
        {
            if (!(*i)->IsAffectingSpell(m_spellInfo))
                continue;
            checkForm = false;
            break;
        }
        if (checkForm)
        {
            SpellCastResult shapeError = SPELL_CAST_OK;
            /// Cannot be used in this stance/form - Plunder armor - 208535 can mount
            if (!IsDarkSimulacrum() && !m_caster->HasAura(208535))
                SpellCastResult shapeError = m_spellInfo->CheckShapeshift(m_caster->GetShapeshiftForm());
            if (shapeError != SPELL_CAST_OK)
                return shapeError;

            if ((AttributesCustom & SPELL_ATTR0_ONLY_STEALTHED) && !(m_caster->HasStealthAura()))
                return SPELL_FAILED_ONLY_STEALTHED;
        }
    }

    Unit::AuraEffectList const& blockSpells = m_caster->GetAuraEffectsByType(SPELL_AURA_BLOCK_SPELL_FAMILY);
    for (Unit::AuraEffectList::const_iterator blockItr = blockSpells.begin(); blockItr != blockSpells.end(); ++blockItr)
        if (uint32((*blockItr)->GetMiscValue()) == m_spellInfo->SpellFamilyName)
            return SPELL_FAILED_SPELL_UNAVAILABLE;

    bool reqCombat = true;
    Unit::AuraEffectList const& stateAuras = m_caster->GetAuraEffectsByType(SPELL_AURA_ABILITY_IGNORE_AURASTATE);
    for (Unit::AuraEffectList::const_iterator j = stateAuras.begin(); j != stateAuras.end(); ++j)
    {
        if ((*j)->IsAffectingSpell(m_spellInfo))
        {
            m_needComboPoints = false;
            if ((*j)->GetMiscValue() == 1)
            {
                reqCombat=false;
                break;
            }
        }
    }

    // caster state requirements
    // not for triggered spells (needed by execute)
    if (!(_triggeredCastFlags & TRIGGERED_IGNORE_CASTER_AURASTATE))
    {
        if (m_spellInfo->CasterAuraState && !m_caster->HasAuraState(AuraStateType(m_spellInfo->CasterAuraState), m_spellInfo, m_caster))
            return SPELL_FAILED_CASTER_AURASTATE;
        if (m_spellInfo->ExcludeCasterAuraState && m_caster->HasAuraState(AuraStateType(m_spellInfo->ExcludeCasterAuraState), m_spellInfo, m_caster))
            return SPELL_FAILED_CASTER_AURASTATE;

        std::pair<uint32, uint32> l_VantusSpellIdsBounds = { 191464, 250167 };

        std::set<uint32> l_VantusRuneSpells =
        {
            191464, ///< Vantus Rune : Ursoc
            192761, ///< Vantus Rune : Nythendra
            192762, ///< Vantus Rune : Il'gynoth, The Heart Of Corruption
            192763, ///< Vantus Rune : Dragons of Nightmare
            192764, ///< Vantus Rune : Xavius
            192765, ///< Vantus Rune : Elerethe Renferal
            192766, ///< Vantus Rune : Cenarius
            192767, ///< Vantus Rune : Skorpyron
            192768, ///< Vantus Rune : Chronomatic Anomaly
            192769, ///< Vantus Rune : Trilliax
            192770, ///< Vantus Rune : Spellblade Aluriel
            192771, ///< Vantus Rune : Tichondrius
            192772, ///< Vantus Rune : High Botanist Tel'arn
            192773, ///< Vantus Rune : Krosus
            192774, ///< Vantus Rune : Star Augur Etraeus
            192775, ///< Vantus Rune : Grand Magistrix Elisande
            192776, ///< Vantus Rune : Gul'dan
            237820, ///< Vantus Rune : Fallen Avatar
            237821, ///< Vantus Rune : Goroth
            237822, ///< Vantus Rune : Sisters of the Moon
            237823, ///< Vantus Rune : Maiden of Vigilance
            237824, ///< Vantus Rune : Harjatan
            237825, ///< Vantus Rune : Kil'Jaeden
            237826, ///< Vantus Rune : Mistress Sassz'ine
            237827, ///< Vantus Rune : The Desolate Host
            237828, ///< Vantus Rune : Demonic Inquisition
            250144, ///< Vantus Rune : Aggramar
            250146, ///< Vantus Rune : Argus the Unmaker
            250148, ///< Vantus Rune : Kin'Garoth
            250150, ///< Vantus Rune : The defense of Eonar
            250153, ///< Vantus Rune : Garothi the Worldbreaker
            250156, ///< Vantus Rune : Felhound of Sargeras
            250158, ///< Vantus Rune : Imonar the Soulhunter
            250160, ///< Vantus Rune : Portal keeper Hasabel
            250163, ///< Vantus Rune : The Coven of Shivarra
            250165, ///< Vantus Rune : Varimathras
            250167  ///< Vantus Rune : Antoran High Command
        };

        if (l_VantusRuneSpells.find(m_spellInfo->Id) != l_VantusRuneSpells.end())
        {
            Unit::AuraApplicationMap const& l_AppliedAuras = m_caster->GetAppliedAuras();
            for (Unit::AuraApplicationMap::const_iterator l_Itr = l_AppliedAuras.begin(); l_Itr != l_AppliedAuras.end(); ++l_Itr)
            {
                if ((*l_Itr).first > l_VantusSpellIdsBounds.second)
                    break;

                if ((*l_Itr).first < l_VantusSpellIdsBounds.first)
                    continue;

                if (l_VantusRuneSpells.find((*l_Itr).first) != l_VantusRuneSpells.end())
                    return SPELL_FAILED_CASTER_AURASTATE;
            }
        }

        // Note: spell 62473 requres casterAuraSpell = triggering spell
        if (!(m_spellInfo->Id == 48020) && m_spellInfo->CasterAuraSpell && !m_caster->HasAura(m_spellInfo->CasterAuraSpell))
        {
            bool l_NeedAuraState = true;
            Unit::AuraEffectList const& stateAuras = m_caster->GetAuraEffectsByType(SPELL_AURA_ABILITY_IGNORE_AURASTATE);
            for (Unit::AuraEffectList::const_iterator j = stateAuras.begin(); j != stateAuras.end(); ++j)
            {
                if ((*j)->IsAffectingSpell(m_spellInfo))
                    l_NeedAuraState = false;
            }
            if (l_NeedAuraState)
                return SPELL_FAILED_CASTER_AURASTATE;
        }
        if (m_spellInfo->ExcludeCasterAuraSpell && m_caster->HasAura(m_spellInfo->ExcludeCasterAuraSpell))
            return SPELL_FAILED_CASTER_AURASTATE;

        if (reqCombat && m_caster->isInCombat() && !m_spellInfo->CanBeUsedInCombat())
            return SPELL_FAILED_AFFECTING_COMBAT;
    }

    // cancel autorepeat spells if cast start when moving
    // (not wand currently autorepeat cast delayed to moving stop anyway in spell update code)
    // Do not cancel spells which are affected by a SPELL_AURA_CAST_WHILE_WALKING or SPELL_AURA_ALLOW_ALL_CASTS_WHILE_WALKING effect
    if (m_caster->IsPlayer() && m_caster->ToPlayer()->IsMoving() && !m_caster->HasAuraTypeWithAffectMask(SPELL_AURA_CAST_WHILE_WALKING, m_spellInfo) &&
        !m_caster->HasAuraType(SPELL_AURA_ALLOW_ALL_CASTS_WHILE_WALKING) && !(m_spellInfo->AttributesEx5 & SPELL_ATTR5_USABLE_WHILE_MOVING) && m_spellInfo->Id != 75) ///< allow auto shot to be updated while moving
    {
        ///< Vengeful Retreat, Double Jump, Glide and Eye Beam
        if (m_spellInfo->Id == 198013 && m_caster->HasAura(196055)) {}
        if (m_spellInfo->Id == 198013 && m_caster->HasAura(131347)) {}
        if (m_spellInfo->Id == 198013 && m_caster->HasAura(198793)) {}
        else
        {
            // skip stuck spell to allow use it in falling case and apply spell limitations at movement
            if ((!m_caster->HasUnitMovementFlag(MOVEMENTFLAG_FALLING_FAR) || m_spellInfo->Effects[0].Effect != SPELL_EFFECT_STUCK) &&
                (IsAutoRepeat() || (m_spellInfo->AuraInterruptFlags & AURA_INTERRUPT_FLAG_NOT_SEATED) != 0))
                return SPELL_FAILED_MOVING;
        }
    }

    // Check vehicle flags
    if (!(_triggeredCastFlags & TRIGGERED_IGNORE_CASTER_MOUNTED_OR_ON_VEHICLE))
    {
        SpellCastResult vehicleCheck = CheckVehicle(m_caster);
        if (vehicleCheck != SPELL_CAST_OK)
            return vehicleCheck;
    }

    // check spell cast conditions from database
    {
        ConditionSourceInfo condInfo = ConditionSourceInfo(m_caster, m_targets.GetObjectTarget());
        if (!sConditionMgr->IsObjectMeetingNotGroupedConditions(CONDITION_SOURCE_TYPE_SPELL, m_spellInfo->Id, condInfo))
        {
            // send error msg to player if condition failed and text message available
            // @TODO: using WorldSession::SendNotification is not blizzlike
            if (Player* playerCaster = m_caster->ToPlayer())
            {
                // mLastFailedCondition can be NULL if there was an error processing the condition in Condition::Meets (i.e. wrong data for ConditionTarget or others)
                if (playerCaster->GetSession() && condInfo.mLastFailedCondition
                    && condInfo.mLastFailedCondition->ErrorTextId)
                {
                    playerCaster->GetSession()->SendNotification(condInfo.mLastFailedCondition->ErrorTextId);
                    return SPELL_FAILED_DONT_REPORT;
                }
            }
            if (!condInfo.mLastFailedCondition || !condInfo.mLastFailedCondition->ConditionTarget)
                return SPELL_FAILED_CASTER_AURASTATE;
            return SPELL_FAILED_BAD_TARGETS;
        }
    }

    // Don't check explicit target for passive spells (workaround) (check should be skipped only for learn case)
    // those spells may have incorrect target entries or not filled at all (for example 15332)
    // such spells when learned are not targeting anyone using targeting system, they should apply directly to caster instead
    // also, such casts shouldn't be sent to client
    if (!IsTriggered() || !m_targets.GetUnitTarget())
    {
        if (!((AttributesCustom & SPELL_ATTR0_PASSIVE) && (!m_targets.GetUnitTarget() || (m_targets.GetUnitTarget() == m_caster && !m_targets.GetUnitTarget()->HasAura(205247))))) ///< Target of Soul Effigy Aura must be attackable
        {
            // Check explicit target for m_originalCaster - todo: get rid of such workarounds
            SpellCastResult castResult = m_spellInfo->CheckExplicitTarget(m_originalCaster ? m_originalCaster : m_caster, m_targets.GetObjectTarget(), m_targets.GetItemTarget());
            if (castResult != SPELL_CAST_OK)
                return castResult;
        }
    }

    if (Unit* target = m_targets.GetUnitTarget())
    {
        SpellCastResult castResult = m_spellInfo->CheckTarget(m_caster, target, IsTriggered() ? true: false);
        if (castResult != SPELL_CAST_OK)
            return castResult;

        // If it's not a melee spell, check if vision is obscured by SPELL_AURA_INTERFERE_TARGETTING
        if (m_spellInfo->DmgClass != SPELL_DAMAGE_CLASS_MELEE && !(_triggeredCastFlags & TRIGGERED_IGNORE_LOS))
        {
            if (m_caster->IsVisionObscured(target, m_spellInfo))
                return SPELL_FAILED_VISION_OBSCURED;
        }

        if (strict && IsMorePowerfulAura(target))
            return IsTriggered() ? SPELL_FAILED_DONT_REPORT: SPELL_FAILED_AURA_BOUNCED;

        if (target != m_caster)
        {
            // Must be behind the target
            if ((AttributesCustomCu & SPELL_ATTR0_CU_REQ_CASTER_BEHIND_TARGET) && target->HasInArc(static_cast<float>(M_PI), m_caster))
                return SPELL_FAILED_NOT_BEHIND;

            // Target must be facing you
            if ((AttributesCustomCu & SPELL_ATTR0_CU_REQ_TARGET_FACING_CASTER) && !target->HasInArc(static_cast<float>(M_PI), m_caster))
                return SPELL_FAILED_NOT_INFRONT;

            // Gouge and Glyph of Gouge
            if (m_spellInfo->Id == 1776 && !m_caster->HasAura(56809) && !target->HasInArc(static_cast<float>(M_PI), m_caster))
                return SPELL_FAILED_NOT_INFRONT;

            if ((_triggeredCastFlags & TRIGGERED_IGNORE_LOS) == 0)
            {
                // Ignore LOS for gameobjects casts (wrongly casted by a trigger)
                if (m_caster->GetEntry() != WORLD_TRIGGER)
                {
                    if (!(AttributesCustomEx2 & SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS) && !m_caster->IsWithinLOSInMap(target))
                        return SPELL_FAILED_LINE_OF_SIGHT;
                }

                if (m_caster->IsVisionObscured(target, m_spellInfo))
                    return SPELL_FAILED_VISION_OBSCURED; // smoke bomb, camouflage...
            }
        }
    }

    // Check for line of sight for spells with dest
    if (m_targets.HasDst())
    {
        float x, y, z;
        m_targets.GetDstPos()->GetPosition(x, y, z);

        bool l_NeedToCheckLoS = true;
        if (Unit* l_Target = m_targets.GetUnitTarget())
        {
            if (Creature* l_Boss = l_Target->ToCreature())
            {
                if (m_caster->IsWithinLOSInMap(l_Boss))
                    l_NeedToCheckLoS = false;
            }
        }

        if (!(_triggeredCastFlags & TRIGGERED_IGNORE_LOS) && !(AttributesCustomEx2 & SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS) && l_NeedToCheckLoS && !m_caster->IsWithinLOS(x, y, z))
            return SPELL_FAILED_LINE_OF_SIGHT;
    }

    // check pet presence
    for (uint8 j = 0; j < m_spellInfo->EffectCount; ++j)
    {
        if (m_spellInfo->Effects[j].TargetA.GetTarget() == TARGET_UNIT_PET)
        {
            if (!m_caster->GetGuardianPet())
            {
                if (m_triggeredByAuraSpell)              // not report pet not existence for triggered spells
                    return SPELL_FAILED_DONT_REPORT;
                else
                    return SPELL_FAILED_NO_PET;
            }
            break;
        }

        Unit* l_Target = m_targets.GetUnitTarget();

        /// Check mini pet target
        if (m_spellInfo->Effects[j].TargetA.GetTarget() == TARGET_UNIT_TARGET_MINIPET && l_Target != nullptr && l_Target->GetGUID() != m_caster->GetCritterGUID())
            return SpellCastResult::SPELL_FAILED_BAD_TARGETS;
    }

    // Spell casted only on battleground
    if ((AttributesCustomEx3 & SPELL_ATTR3_BATTLEGROUND) &&  m_caster->IsPlayer())
        if (!m_caster->ToPlayer()->InBattleground())
            return SPELL_FAILED_ONLY_BATTLEGROUNDS;

    // do not allow spells to be cast in arenas or rated battlegrounds
    if (Player* l_Player = m_caster->GetCharmerOrOwnerPlayerOrPlayerItself())
    {
        if (l_Player->InArena() || l_Player->InRatedBattleGround())
        {
            SpellCastResult castResult = CheckArenaAndRatedBattlegroundCastRules(l_Player->GetBattleground());
            if (castResult != SPELL_CAST_OK)
                return castResult;
        }
        if (l_Player->GetBattleground() && (m_spellInfo->AttributesEx8 & SPELL_ATTR8_NOT_IN_BG_OR_ARENA))
            return SPELL_FAILED_NOT_IN_BATTLEGROUND;
    }

    // zone check
    if (m_caster->GetTypeId() == TYPEID_UNIT || !m_caster->ToPlayer()->isGameMaster())
    {
        uint32 zone, area;
        m_caster->GetZoneAndAreaId(zone, area);

        SpellCastResult locRes= m_spellInfo->CheckLocation(m_caster->GetMapId(), zone, area,
            m_caster->IsPlayer() ? m_caster->ToPlayer() : NULL);
        if (locRes != SPELL_CAST_OK)
            return locRes;
    }

    // not let players cast spells at mount (and let do it to creatures)
    if (m_caster->IsMounted() && m_caster->IsPlayer() && !m_caster->IsMountDivineSteed() && !(_triggeredCastFlags & TRIGGERED_IGNORE_CASTER_MOUNTED_OR_ON_VEHICLE) &&
        !m_spellInfo->IsPassive() && !(AttributesCustom & SPELL_ATTR0_CASTABLE_WHILE_MOUNTED))
    {
        /// Herb Gathering
        if (m_spellInfo->Id == 2369)
        {
            /// This is used for Sky Golem (Flying Mount) which allow you to gather herbs (and only herbs) without being dismounted.
            if (!m_caster->HasFlag(EPlayerFields::PLAYER_FIELD_LOCAL_FLAGS, PlayerLocalFlags::PLAYER_LOCAL_FLAG_CAN_USE_OBJECTS_MOUNTED))
            {
                if (m_caster->isInFlight())
                    return SPELL_FAILED_NOT_ON_TAXI;
                else
                    return SPELL_FAILED_NOT_MOUNTED;
            }
        }
        else
        {
            if (m_caster->isInFlight())
                return SPELL_FAILED_NOT_ON_TAXI;
            else
                return SPELL_FAILED_NOT_MOUNTED;
        }
    }

    SpellCastResult castResult = SPELL_CAST_OK;

    // always (except passive spells) check items (focus object can be required for any type casts)
    if (!m_spellInfo->IsPassive())
    {
        castResult = CheckItems();
        if (castResult != SPELL_CAST_OK)
            return castResult;
    }

    // Triggered spells also have range check
    // TODO: determine if there is some flag to enable/disable the check
    castResult = CheckRange(strict);
    if (castResult != SPELL_CAST_OK)
        return castResult;

    if (!(_triggeredCastFlags & TRIGGERED_IGNORE_POWER_AND_REAGENT_COST))
    {
        castResult = CheckPower();
        if (castResult != SPELL_CAST_OK)
            return castResult;
    }

    if (!(_triggeredCastFlags & TRIGGERED_IGNORE_CASTER_AURAS))
    {
        castResult = CheckCasterAuras();
        if (castResult != SPELL_CAST_OK)
            return castResult;
    }

    // script hook
    castResult = CallScriptCheckCastHandlers();
    if (castResult != SPELL_CAST_OK)
        return castResult;

    for (uint8 i = 0; i < m_spellInfo->EffectCount; ++i)
    {
        // for effects of spells that have only one target
        switch (m_spellInfo->Effects[i].Effect)
        {
            case SPELL_EFFECT_GIVE_ARTIFACT_POWER:
            {
                if ((!m_caster || !m_caster->ToPlayer()))
                    return SPELL_FAILED_BAD_TARGETS;

                MS::Artifact::Manager* l_Artifact = m_caster->ToPlayer()->GetCurrentlyEquippedArtifact();
                if (!l_Artifact)
                {
                    m_customError = SPELL_CUSTOM_ERROR_REQUIRES_ARTIFACT_WEAPON;
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                if (l_Artifact->GetItemTemplate() && l_Artifact->GetItemTemplate()->ItemId == 133755)
                    return SPELL_FAILED_NO_ARTIFACT_EQUIPPED;

                if (sWorld->getIntConfig(CONFIG_MAX_ARTIFACT_LEVEL) && (l_Artifact->GetLevel() >= sWorld->getIntConfig(CONFIG_MAX_ARTIFACT_LEVEL)))
                {
                    m_customError = SPELL_CUSTOM_ERROR_LEARNED_ALL_THAT_YOU_CAN_ABOUT_YOUR_ARTIFACT;
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                break;
            }
            case SPELL_EFFECT_KNOCK_BACK_DEST:
            {
                switch (m_spellInfo->Id)
                {
                    case 68645: // Rocket Pack
                        if (!m_caster->GetTransport())
                            return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
                        break;
                }
                break;
            }
            case SPELL_EFFECT_DUMMY:
            {
                // Death Coil
                if (m_spellInfo->SpellFamilyName == SPELLFAMILY_DEATHKNIGHT && m_spellInfo->SpellFamilyFlags[0] == 0x2000)
                {
                    Unit* target = m_targets.GetUnitTarget();
                    // Glyph of Death Coil
                    if (!target || (target->IsFriendlyTo(m_caster) && target->GetCreatureType() != CREATURE_TYPE_UNDEAD && !m_caster->HasAura(63333)))
                        return SPELL_FAILED_BAD_TARGETS;

                    if (!target->IsFriendlyTo(m_caster))
                    {
                        if (!m_caster->HasInArc(static_cast<float>(M_PI), target))
                            return SPELL_FAILED_UNIT_NOT_INFRONT;
                        if (!m_caster->_IsValidAttackTarget(target, GetSpellInfo()))
                            return SPELL_FAILED_BAD_TARGETS;
                    }
                    else if (m_caster->HasAura(63333) && target == m_caster)
                        return SPELL_FAILED_BAD_TARGETS;
                }
                else if (m_spellInfo->Id == 19938)          // Awaken Peon
                {
                    Unit* unit = m_targets.GetUnitTarget();
                    if (!unit || !unit->HasAura(17743))
                        return SPELL_FAILED_BAD_TARGETS;
                }
                else if (m_spellInfo->Id == 31789)          // Righteous Defense
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return SPELL_FAILED_DONT_REPORT;

                    Unit* target = m_targets.GetUnitTarget();
                    if (!target || !target->IsFriendlyTo(m_caster) || target->getAttackers().empty())
                        return SPELL_FAILED_BAD_TARGETS;

                }
                else if (m_spellInfo->Id == 217200)        // Dire Frenzy
                {
                    Unit* l_Target = m_targets.GetUnitTarget();
                    if (!m_caster->IsPlayer() || !l_Target)
                        return SPELL_FAILED_DONT_REPORT;

                    Pet* l_Pet = m_caster->ToPlayer()->GetPet();
                    if (!l_Pet)
                        return SPELL_FAILED_NO_PET;

                    if (l_Pet->GetDistance2d(l_Target) > 40.f)
                    {
                        m_customError = SPELL_CUSTOM_ERROR_PET_OUT_OF_RANGE;
                        return SPELL_FAILED_CUSTOM_ERROR;
                    }
                }
                break;
            }
            case SPELL_EFFECT_LEARN_SPELL:
            {
                if (m_caster->GetTypeId() != TYPEID_PLAYER)
                    return SPELL_FAILED_BAD_TARGETS;

                if (m_spellInfo->Effects[i].TargetA.GetTarget() != TARGET_UNIT_PET)
                    break;

                Pet* pet = m_caster->ToPlayer()->GetPet();

                if (!pet)
                    return SPELL_FAILED_NO_PET;

                SpellInfo const* learn_spellproto = sSpellMgr->GetSpellInfo(m_spellInfo->Effects[i].TriggerSpell);

                if (!learn_spellproto)
                    return SPELL_FAILED_NOT_KNOWN;

                if (m_spellInfo->SpellLevel > pet->getLevel())
                    return SPELL_FAILED_LOWLEVEL;

                break;
            }
            case SPELL_EFFECT_LEARN_PET_SPELL:
            {
                // check target only for unit target case
                if (Unit* unitTarget = m_targets.GetUnitTarget())
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return SPELL_FAILED_BAD_TARGETS;

                    Pet* pet = unitTarget->ToPet();
                    if (!pet || pet->GetOwner() != m_caster)
                        return SPELL_FAILED_BAD_TARGETS;

                    SpellInfo const* learn_spellproto = sSpellMgr->GetSpellInfo(m_spellInfo->Effects[i].TriggerSpell);

                    if (!learn_spellproto)
                        return SPELL_FAILED_NOT_KNOWN;

                    if (m_spellInfo->SpellLevel > pet->getLevel())
                        return SPELL_FAILED_LOWLEVEL;
                }
                break;
            }
            case SPELL_EFFECT_APPLY_GLYPH:
            {
                if (!m_caster->IsPlayer())
                    return SpellCastResult::SPELL_FAILED_GLYPH_NO_SPEC;

                Player* l_Caster = m_caster->ToPlayer();
                if (!l_Caster->HasSpell(m_Misc[0]))
                    return SpellCastResult::SPELL_FAILED_NOT_KNOWN;

                if (uint32 l_GlyphID = m_spellInfo->Effects[i].MiscValue)
                {
                    GlyphPropertiesEntry const* l_GlyphProperties = sGlyphPropertiesStore.LookupEntry(l_GlyphID);
                    if (!l_GlyphProperties)
                        return SpellCastResult::SPELL_FAILED_INVALID_GLYPH;

                    std::vector<uint32> const* l_GlyphBindableSpells = GetGlyphBindableSpells(l_GlyphID);
                    if (!l_GlyphBindableSpells)
                        return SpellCastResult::SPELL_FAILED_INVALID_GLYPH;

                    if (std::find(l_GlyphBindableSpells->begin(), l_GlyphBindableSpells->end(), m_Misc[0]) == l_GlyphBindableSpells->end())
                        return SpellCastResult::SPELL_FAILED_INVALID_GLYPH;

                    if (std::vector<uint32> const* l_GlyphRequiredSpecs = GetGlyphRequiredSpecs(l_GlyphID))
                    {
                        if (!l_Caster->GetActiveSpecializationID())
                            return SpellCastResult::SPELL_FAILED_GLYPH_NO_SPEC;

                        if (std::find(l_GlyphRequiredSpecs->begin(), l_GlyphRequiredSpecs->end(), l_Caster->GetActiveSpecializationID()) == l_GlyphRequiredSpecs->end())
                            return SpellCastResult::SPELL_FAILED_GLYPH_INVALID_SPEC;
                    }

                    uint32 l_ReplacedGlyph = 0;
                    for (uint32 l_ActiveGlyphID : l_Caster->GetGlyphs(l_Caster->GetActiveTalentGroup()))
                    {
                        if (std::vector<uint32> const* activeGlyphBindableSpells = GetGlyphBindableSpells(l_ActiveGlyphID))
                        {
                            if (std::find(activeGlyphBindableSpells->begin(), activeGlyphBindableSpells->end(), m_Misc[0]) != activeGlyphBindableSpells->end())
                            {
                                l_ReplacedGlyph = l_ActiveGlyphID;
                                break;
                            }
                        }
                    }

                    for (uint32 l_ActiveGlyphID : l_Caster->GetGlyphs(l_Caster->GetActiveTalentGroup()))
                    {
                        if (l_ActiveGlyphID == l_ReplacedGlyph)
                            continue;

                        if (l_ActiveGlyphID == l_GlyphID)
                            return SpellCastResult::SPELL_FAILED_UNIQUE_GLYPH;

                        GlyphPropertiesEntry const* l_PropEntry = sGlyphPropertiesStore.LookupEntry(l_ActiveGlyphID);
                        if (l_PropEntry && l_PropEntry->GlyphExclusiveCategoryID > 0 && l_PropEntry->GlyphExclusiveCategoryID == l_GlyphProperties->GlyphExclusiveCategoryID)
                            return SpellCastResult::SPELL_FAILED_GLYPH_EXCLUSIVE_CATEGORY;
                    }
                }

                /// It's impossible to change talents during challenge mode
                if (m_caster->GetMap()->IsChallengeMode())
                {
                    m_customError = SpellCustomErrors::SPELL_CUSTOM_ERROR_CANT_DO_THAT_WHILE_MYTHIC_KEYSTONE_IS_ACTIVE;
                    return SpellCastResult::SPELL_FAILED_CUSTOM_ERROR;
                }

                break;
            }
            case SPELL_EFFECT_FEED_PET:
            {
                if (m_caster->GetTypeId() != TYPEID_PLAYER)
                    return SPELL_FAILED_BAD_TARGETS;

                Item* foodItem = m_targets.GetItemTarget();
                if (!foodItem)
                    return SPELL_FAILED_BAD_TARGETS;

                Pet* pet = m_caster->ToPlayer()->GetPet();

                if (!pet)
                    return SPELL_FAILED_NO_PET;

                if (!pet->HaveInDiet(foodItem->GetTemplate()))
                    return SPELL_FAILED_WRONG_PET_FOOD;

                if (!pet->GetCurrentFoodBenefitLevel(foodItem->GetTemplate()->ItemLevel))
                    return SPELL_FAILED_FOOD_LOWLEVEL;

                if (m_caster->isInCombat() || pet->isInCombat())
                    return SPELL_FAILED_AFFECTING_COMBAT;

                break;
            }
            case SPELL_EFFECT_POWER_BURN:
            case SPELL_EFFECT_POWER_DRAIN:
            {
                if (m_spellInfo->Id == 235904)  ///< Mana rift execute even if this effect can't apply
                    break;

                if (m_spellInfo->Id == 22568)  ///< Ferocious Bite execute even if this effect can't apply
                    break;

                // Can be area effect, Check only for players and not check if target - caster (spell can have multiply drain/burn effects)
                if (m_caster->IsPlayer())
                    if (Unit* target = m_targets.GetUnitTarget())
                        if (target != m_caster && target->getPowerType() != Powers(m_spellInfo->Effects[i].MiscValue))
                            return SPELL_FAILED_BAD_TARGETS;
                break;
            }
            case SPELL_EFFECT_CHARGE:
            {
                if (m_spellInfo->SpellFamilyName == SPELLFAMILY_WARRIOR)
                {
                    // Warbringer - can't be handled in proc system - should be done before checkcast root check and charge effect process
                    if (strict && m_caster->IsScriptOverriden(m_spellInfo, 6953))
                        m_caster->RemoveMovementImpairingAuras();
                    // Intervene can be casted in root effects, so we need to remove movement impairing auras before check cast result
                    if (m_spellInfo->Id == 34784)
                        m_caster->RemoveMovementImpairingAuras();
                }
                if (m_caster->HasUnitState(UNIT_STATE_ROOT))
                    return SPELL_FAILED_ROOTED;
                if (m_caster->IsPlayer())
                    if (Unit* target = m_targets.GetUnitTarget())
                        if (!target->isAlive())
                            return SPELL_FAILED_BAD_TARGETS;

                Unit* target = m_targets.GetUnitTarget();

                if (!target)
                    return SPELL_FAILED_DONT_REPORT;

                Position pos;
                target->GetContactPoint(m_caster, pos.m_positionX, pos.m_positionY, pos.m_positionZ);
                target->GetFirstCollisionPosition(pos, CONTACT_DISTANCE, target->GetRelativeAngle(m_caster));

                m_preGeneratedPath.SetPathLengthLimit(m_spellInfo->GetMaxRange(true) * 4.0f);

                bool result = m_preGeneratedPath.CalculatePath(pos.m_positionX, pos.m_positionY, pos.m_positionZ, false, false, target, true);
                if (m_preGeneratedPath.GetPathType() & PATHFIND_SHORT)
                    return SPELL_FAILED_OUT_OF_RANGE;
                else if (!result)
                    return SPELL_FAILED_NOPATH;

                break;
            }
            case SPELL_EFFECT_SKINNING:
            {
                if (m_caster->GetTypeId() != TYPEID_PLAYER || !m_targets.GetUnitTarget() || m_targets.GetUnitTarget()->GetTypeId() != TYPEID_UNIT)
                    return SPELL_FAILED_BAD_TARGETS;

                if (!(m_targets.GetUnitTarget()->GetUInt32Value(UNIT_FIELD_FLAGS) & UNIT_FLAG_SKINNABLE))
                    return SPELL_FAILED_TARGET_UNSKINNABLE;

                Creature* creature = m_targets.GetUnitTarget()->ToCreature();
                uint64 l_LootContainerGuid = m_caster->ToPlayer()->GetLootContainerGuidForCreature(creature);
                if (!l_LootContainerGuid)
                    return SPELL_FAILED_BAD_TARGETS;

                if (creature->GetCreatureType() != CREATURE_TYPE_CRITTER && !creature->m_LootContainers[l_LootContainerGuid].isLooted())
                    return SPELL_FAILED_TARGET_NOT_LOOTED;

                uint32 skill = creature->GetCreatureTemplate()->GetRequiredLootSkill();

                int32 skillValue = m_caster->ToPlayer()->GetSkillValue(skill);
                int32 TargetLevel = creature->GetCreatureTemplate()->ScaleLevelMin;
                int32 ReqValue = (skillValue < 100 ? (TargetLevel-10) * 10 : TargetLevel * 5);

                /// Skinning in Draenor doesn't require to have a specific skill level, 1 is enough
                if (skillValue && creature->GetCreatureTemplate()->expansion >= Expansion::EXPANSION_WARLORDS_OF_DRAENOR)
                    break;

                if (ReqValue > skillValue)
                    return SPELL_FAILED_LOW_CASTLEVEL;

                // chance for fail at orange skinning attempt
                if ((m_selfContainer && (*m_selfContainer) == this) &&
                    skillValue < sWorld->GetConfigMaxSkillValue() &&
                    (ReqValue < 0 ? 0 : ReqValue) > irand(skillValue - 25, skillValue + 37))
                    return SPELL_FAILED_TRY_AGAIN;

                break;
            }
            case SPELL_EFFECT_OPEN_LOCK:
            {
                if (m_spellInfo->Effects[i].TargetA.GetTarget() != TARGET_GAMEOBJECT_TARGET &&
                    m_spellInfo->Effects[i].TargetA.GetTarget() != TARGET_GAMEOBJECT_ITEM_TARGET)
                    break;

                if (m_targets.GetGOTarget() && m_targets.GetGOTarget()->AI())
                {
                    if (!m_targets.GetGOTarget()->AI()->OnCheckOpening(m_caster, m_spellInfo))
                        return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
                }

                if (m_caster->GetTypeId() != TYPEID_PLAYER  // only players can open locks, gather etc.
                    // we need a go target in case of TARGET_GAMEOBJECT_TARGET
                    || (m_spellInfo->Effects[i].TargetA.GetTarget() == TARGET_GAMEOBJECT_TARGET && !m_targets.GetGOTarget()))
                    return SPELL_FAILED_BAD_TARGETS;

                Item* pTempItem = NULL;
                if (m_targets.GetTargetMask() & TARGET_FLAG_TRADE_ITEM)
                {
                    if (TradeData* pTrade = m_caster->ToPlayer()->GetTradeData())
                    {
                        if (m_targets.GetItemTargetGUID() < TRADE_SLOT_COUNT)
                            pTempItem = pTrade->GetTraderData()->GetItem(TradeSlots(m_targets.GetItemTargetGUID()));
                    }
                }
                else if (m_targets.GetTargetMask() & TARGET_FLAG_ITEM)
                    pTempItem = m_caster->ToPlayer()->GetItemByGuid(m_targets.GetItemTargetGUID());

                // we need a go target, or an openable item target in case of TARGET_GAMEOBJECT_ITEM_TARGET
                if (m_spellInfo->Effects[i].TargetA.GetTarget() == TARGET_GAMEOBJECT_ITEM_TARGET &&
                    !m_targets.GetGOTarget() &&
                    (!pTempItem || !pTempItem->GetTemplate()->LockID || !pTempItem->IsLocked()))
                    return SPELL_FAILED_BAD_TARGETS;

                if (m_spellInfo->Id != 1842 || (m_targets.GetGOTarget() &&
                    m_targets.GetGOTarget()->GetGOInfo()->type != GAMEOBJECT_TYPE_TRAP))
                {
                    if (m_caster->ToPlayer()->InBattleground()) // In Battleground players can use only flags and banners
                    {
                        if (m_caster->ToPlayer()->CanUseBattlegroundObject())
                        {
                            m_caster->RemoveAurasByType(SPELL_AURA_MOD_STEALTH);
                            m_caster->RemoveAurasByType(SPELL_AURA_MOD_INVISIBILITY);
                            m_caster->RemoveAurasByType(SPELL_AURA_MOD_CAMOUFLAGE);
                            m_caster->RemoveAurasByType(SPELL_AURA_MOUNTED);
                        }
                        else
                            return SPELL_FAILED_TRY_AGAIN;
                    }
                }

                // get the lock entry
                uint32 lockId = 0;
                bool l_ResultOverridedByPlayerCondition = false;
                bool l_PlayerConditionFailed = false;
                if (GameObject* go = m_targets.GetGOTarget())
                {
                    lockId = go->GetGOInfo()->GetLockId();
                    if (!lockId)
                        return SPELL_FAILED_BAD_TARGETS;

                    GameObjectTemplate const* l_Template = sObjectMgr->GetGameObjectTemplate(go->GetEntry());

                    if (l_Template && (l_Template->type == GAMEOBJECT_TYPE_CHEST || l_Template->type == GAMEOBJECT_TYPE_GATHERING_NODE) && m_caster->IsPlayer())
                    {
                        uint32 l_PlayerConditionID = l_Template->chest.conditionID1;
                        bool l_HasPlayerCondition = l_PlayerConditionID != 0 && (sPlayerConditionStore.LookupEntry(l_Template->chest.conditionID1) != nullptr || sScriptMgr->HasPlayerConditionScript(l_Template->chest.conditionID1));

                        if (l_HasPlayerCondition && m_caster->EvalPlayerCondition(l_PlayerConditionID).first)
                            l_ResultOverridedByPlayerCondition = true;
                        else if (l_HasPlayerCondition)
                            l_PlayerConditionFailed = true;
                    }
                }
                else if (Item* itm = m_targets.GetItemTarget())
                    lockId = itm->GetTemplate()->LockID;

                SkillType skillId = SKILL_NONE;
                int32 reqSkillValue = 0;
                int32 skillValue = 0;

                if (l_PlayerConditionFailed)
                    return SPELL_FAILED_ERROR;

                // check lock compatibility
                SpellCastResult res = CanOpenLock(i, lockId, skillId, reqSkillValue, skillValue);
                if (res != SPELL_CAST_OK && !l_ResultOverridedByPlayerCondition)
                    return res;

                if (l_ResultOverridedByPlayerCondition)
                    res = SPELL_CAST_OK;

                break;
            }
            case SPELL_EFFECT_SUMMON_DEAD_PET:
            {
                Creature* pet = m_caster->GetGuardianPet();

                if (pet && pet->isAlive())
                    return SPELL_FAILED_ALREADY_HAVE_SUMMON;

                break;
            }
            // This is generic summon effect
            case SPELL_EFFECT_SUMMON:
            {
                SummonPropertiesEntry const* SummonProperties = sSummonPropertiesStore.LookupEntry(m_spellInfo->Effects[i].MiscValueB);
                if (!SummonProperties)
                    break;
                switch (SummonProperties->Category)
                {
                    case SUMMON_CATEGORY_PET:
                        if (!m_spellInfo->HasAttribute(SPELL_ATTR1_DISMISS_PET) && m_caster->GetPetGUID())
                            return SPELL_FAILED_ALREADY_HAVE_SUMMON;
                    case SUMMON_CATEGORY_PUPPET:
                        if (m_caster->GetCharmGUID())
                            return SPELL_FAILED_ALREADY_HAVE_CHARM;
                        break;
                }
                break;
            }
            case SPELL_EFFECT_CREATE_TAMED_PET:
            {
                if (m_targets.GetUnitTarget())
                {
                    if (m_targets.GetUnitTarget()->GetTypeId() != TYPEID_PLAYER)
                        return SPELL_FAILED_BAD_TARGETS;
                    if (m_targets.GetUnitTarget()->GetPetGUID())
                        return SPELL_FAILED_ALREADY_HAVE_SUMMON;
                }
                break;
            }
            case SPELL_EFFECT_SUMMON_PET:
            {
                if (m_caster->GetPetGUID())                  //let warlock do a replacement summon
                {
                    if (m_caster->IsPlayer() && m_caster->getClass() == CLASS_WARLOCK)
                    {
                        if (strict)                         //starting cast, trigger pet stun (cast by pet so it doesn't attack player)
                            if (Pet* pet = m_caster->ToPlayer()->GetPet())
                                pet->CastSpell(pet, 32752, true, NULL, nullptr, pet->GetGUID());
                    }
                    else
                        return SPELL_FAILED_ALREADY_HAVE_SUMMON;
                }

                if (m_caster->GetCharmGUID())
                    return SPELL_FAILED_ALREADY_HAVE_CHARM;
                break;
            }
            case SPELL_EFFECT_SUMMON_PLAYER:
            {
                if (m_caster->GetTypeId() != TYPEID_PLAYER)
                    return SPELL_FAILED_BAD_TARGETS;
                if (!m_caster->ToPlayer()->GetSelection())
                    return SPELL_FAILED_BAD_TARGETS;

                Player* target = ObjectAccessor::FindPlayer(m_caster->ToPlayer()->GetSelection());
                if (!target || m_caster->ToPlayer() == target || (!target->IsInSameRaidWith(m_caster->ToPlayer()) && m_spellInfo->Id != 48955)) // refer-a-friend spell
                    return SPELL_FAILED_BAD_TARGETS;

                // check if our map is dungeon
                MapEntry const* map = sMapStore.LookupEntry(m_caster->GetMapId());
                if (map->IsDungeon())
                {
                    uint32 mapId = m_caster->GetMap()->GetId();
                    Difficulty difficulty = m_caster->GetMap()->GetDifficultyID();
                    if (map->IsRaid())
                        if (InstancePlayerBindPtr targetBind = target->GetBoundInstance(mapId, difficulty))
                            if (InstancePlayerBindPtr casterBind = m_caster->ToPlayer()->GetBoundInstance(mapId, difficulty))
                                if (targetBind->perm && targetBind->save != casterBind->save)
                                    return SPELL_FAILED_TARGET_LOCKED_TO_RAID_INSTANCE;

                    InstanceTemplate const* instance = sObjectMgr->GetInstanceTemplate(mapId);
                    if (!instance)
                        return SPELL_FAILED_TARGET_NOT_IN_INSTANCE;
                    if (!target->Satisfy(sObjectMgr->GetAccessRequirement(mapId, difficulty), mapId))
                        return SPELL_FAILED_BAD_TARGETS;
                }
                break;
            }
            // RETURN HERE
            case SPELL_EFFECT_SUMMON_RAF_FRIEND:
            {
                if (m_caster->GetTypeId() != TYPEID_PLAYER)
                    return SPELL_FAILED_BAD_TARGETS;

                Player* playerCaster = m_caster->ToPlayer();
                    //
                if (!(playerCaster->GetSelection()))
                    return SPELL_FAILED_BAD_TARGETS;

                Player* target = ObjectAccessor::FindPlayer(playerCaster->GetSelection());

                if (!target ||
                    !(target->GetSession()->GetRecruiterId() == playerCaster->GetSession()->GetAccountId() || target->GetSession()->GetAccountId() == playerCaster->GetSession()->GetRecruiterId()))
                    return SPELL_FAILED_BAD_TARGETS;

                break;
            }
            case SPELL_EFFECT_LEAP:
            case SPELL_EFFECT_TELEPORT_UNITS_FACE_CASTER:
            {
              //Do not allow to cast it before BG starts.
                if (m_caster->IsPlayer())
                {
                    // Don't allow jump the mythic start door
                    InstanceScript* l_Instance = m_caster->GetInstanceScript();

                    if (l_Instance && l_Instance->IsChallenge() && !l_Instance->GetChallenge()->IsRunning())
                        return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                    if (Battleground const* bg = m_caster->ToPlayer()->GetBattleground())
                        if (bg->GetStatus() != STATUS_IN_PROGRESS)
                            return SPELL_FAILED_TRY_AGAIN;
                }
                break;
            }
            case SPELL_EFFECT_STEAL_BENEFICIAL_BUFF:
            {
                if (m_targets.GetUnitTarget() == m_caster)
                    return SPELL_FAILED_BAD_TARGETS;
                break;
            }
            case SPELL_EFFECT_LEAP_BACK:
            {
                if (m_caster->HasUnitState(UNIT_STATE_ROOT))
                {
                    if (m_caster->IsPlayer())
                    {
                        switch (m_spellInfo->Id)
                        {
                            case 781:   // Disengage
                                if (m_caster->HasAura(109215)) // Posthaste
                                    break;
                                return SPELL_FAILED_ROOTED;
                            default:
                                return SPELL_FAILED_ROOTED;
                        }
                    }
                    else
                        return SPELL_FAILED_DONT_REPORT;
                }

                break;
            }
            case SPELL_EFFECT_TALENT_SPEC_SELECT:
                if (Player* l_Player = m_caster->ToPlayer())
                {
                    ArenaType l_ArenaType = ArenaType::Arena1v1;
                    MS::Battlegrounds::BattlegroundType::Type l_BGQueueTypeID = MS::Battlegrounds::GetTypeFromId(BATTLEGROUND_AA, l_ArenaType);
                    if (l_Player->GetRoleBySpecializationId(m_Misc[0]) != Roles::ROLE_DAMAGE)
                    {
                        if (l_Player->GetBattlegroundQueueIndex(l_BGQueueTypeID) < PLAYER_MAX_BATTLEGROUND_QUEUES)
                            return SPELL_FAILED_SPEC_DISABLED;
                    }

                    /// can't change during already started arena/battleground
                    Battleground const* l_BattleGround = l_Player->GetBattleground();
                    if (l_BattleGround || l_Player->IsInvitedForArenaSkirmish2v2())
                    {
                        if (l_BattleGround && l_BattleGround->GetStatus() == STATUS_IN_PROGRESS)
                            return SPELL_FAILED_NOT_IN_BATTLEGROUND;

                        /// Since Legion, healer specs are disabled in 2c2 brackets
                        if (!l_BattleGround || (l_BattleGround->isArena() && l_BattleGround->IsSkirmish() && l_BattleGround->GetArenaType() == ArenaType::Arena2v2))
                        {
                            switch (m_Misc[0])
                            {
                                case SpecIndex::SPEC_DRUID_RESTORATION:
                                case SpecIndex::SPEC_SHAMAN_RESTORATION:
                                case SpecIndex::SPEC_PALADIN_HOLY:
                                case SpecIndex::SPEC_MONK_MISTWEAVER:
                                case SpecIndex::SPEC_PRIEST_HOLY:
                                case SpecIndex::SPEC_PRIEST_DISCIPLINE:
                                    return SPELL_FAILED_SPEC_DISABLED;
                                default:
                                    break;
                            }
                        }

                        /// Healer and Tank specs are disables in 1c1 brackets in Greymane
                        if (!l_BattleGround || (l_BattleGround->GetArenaType() == ArenaType::Arena1v1))
                        {
                            switch (m_Misc[0])
                            {
                                case SpecIndex::SPEC_DRUID_RESTORATION:
                                case SpecIndex::SPEC_DRUID_GUARDIAN:
                                case SpecIndex::SPEC_SHAMAN_RESTORATION:
                                case SpecIndex::SPEC_PALADIN_HOLY:
                                case SpecIndex::SPEC_PALADIN_PROTECTION:
                                case SpecIndex::SPEC_MONK_MISTWEAVER:
                                case SpecIndex::SPEC_MONK_BREWMASTER:
                                case SpecIndex::SPEC_PRIEST_HOLY:
                                case SpecIndex::SPEC_PRIEST_DISCIPLINE:
                                case SpecIndex::SPEC_DEMON_HUNTER_VENGEANCE:
                                case SpecIndex::SPEC_DK_BLOOD:
                                case SpecIndex::SPEC_WARRIOR_PROTECTION:
                                    return SPELL_FAILED_SPEC_DISABLED;
                                default:
                                    break;
                            }
                        }
                    }

                    if (l_Player->GetRoleBySpecializationId(m_Misc[0]) != Roles::ROLE_DAMAGE)
                    {
                        if (l_BattleGround && l_BattleGround->GetArenaType() == ArenaType::Arena1v1)
                            return SPELL_FAILED_SPEC_DISABLED;

                        if (l_Player->GetBattlegroundQueueIndex(MS::Battlegrounds::BattlegroundType::Arena1v1) != PLAYER_MAX_BATTLEGROUND_QUEUES || l_Player->GetBattlegroundQueueIndex(MS::Battlegrounds::BattlegroundType::ArenaSkirmish1v1) != PLAYER_MAX_BATTLEGROUND_QUEUES)
                            return SPELL_FAILED_SPEC_DISABLED;
                    }

                    /// If the player have a artefact equip and doesn't have the artefact for the spec i want to enable, he can't active that spec
                    Item* l_CurrentItem = l_Player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);

                    if (!l_CurrentItem || !l_CurrentItem->GetTemplate()->ArtifactID)
                        l_CurrentItem = l_Player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);

                    if (l_CurrentItem && l_CurrentItem->GetTemplate()->ArtifactID)
                    {
                        uint32 l_ItemID = GetArtifactItemIDEntryBySpec(m_Misc[0]);

                        if (l_ItemID && !l_Player->GetItemCount(l_ItemID))
                        {
                            /// First try to unequip current artifact weapon
                            ItemPosCountVec l_OffDest;
                            uint8 l_OffMsg = l_Player->CanStoreItem(NULL_BAG, NULL_SLOT, l_OffDest, l_CurrentItem, false);
                            if (l_OffMsg == EQUIP_ERR_OK)
                            {
                                l_Player->RemoveItem(INVENTORY_SLOT_BAG_0, l_CurrentItem->GetSlot(), true);
                                l_Player->StoreItem(l_OffDest, l_CurrentItem, true);
                            }
                            /// If we can't enequip it, return an error, to prevent case when player has artifact weapon with other spec
                            else
                                return SPELL_FAILED_SPEC_DISABLED;
                        }
                    }
                }
                break;
            case SPELL_EFFECT_UNLOCK_GUILD_VAULT_TAB:
            {
#ifndef CROSS
                if (m_caster->GetTypeId() != TYPEID_PLAYER)
                    return SPELL_FAILED_BAD_TARGETS;
                if (Guild* guild = m_caster->ToPlayer()->GetGuild())
                    if (guild->GetLeaderGUID() != m_caster->ToPlayer()->GetGUID())
                        return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
                break;
#else /* CROSS */
                return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
#endif /* CROSS */
            }
            case SPELL_EFFECT_CREATE_HEIRLOOM:
            {
                Player* l_Player = m_caster->ToPlayer();

                if (!l_Player)
                    return SPELL_FAILED_BAD_TARGETS;

                if (!l_Player->HasHeirloom(m_Misc[0]))
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                break;
            }
            case SPELL_EFFECT_UPGRADE_HEIRLOOM:
            {
                Player* l_Player = m_caster->ToPlayer();
                HeirloomEntry const* l_Heirloom = GetHeirloomEntryByItemID(m_Misc[0]);

                if (!l_Player || !l_Heirloom || !m_CastItem)
                    return SPELL_FAILED_BAD_TARGETS;

                if (!l_Player->HasHeirloom(l_Heirloom))
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                if (!l_Player->CanUpgradeHeirloomWith(l_Heirloom, m_CastItem->GetTemplate()->ItemId))
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
                break;
            }
#ifndef CROSS
            case SPELL_EFFECT_TEACH_FOLLOWER_ABILITY:
            {
                Player* l_Player = m_caster->ToPlayer();

                if (l_Player == nullptr)
                    return SPELL_FAILED_BAD_TARGETS;

                MS::Garrison::Manager* l_Garrison = l_Player->GetGarrison();

                if (l_Garrison == nullptr)
                    return SPELL_FAILED_BAD_TARGETS;

                SpellCastResult l_Result = l_Garrison->CanLearnTrait(m_Misc[0], m_Misc[1], GetSpellInfo(), i);
                if (l_Result != SPELL_CAST_OK)
                    return l_Result;

                break;
            }
#endif
            case SPELL_EFFECT_EARN_ARTIFACT_POWER:
            {
                Player* l_Player = m_caster->ToPlayer();

                if (!l_Player)
                    return SPELL_FAILED_BAD_TARGETS;

                if (!l_Player->GetCurrentlyEquippedArtifact() || (l_Player->GetCurrentlyEquippedArtifact()->GetItemTemplate() && l_Player->GetCurrentlyEquippedArtifact()->GetItemTemplate()->ItemId != 133755))
                    return SPELL_FAILED_NO_ARTIFACT_EQUIPPED;

                break;
            }
            case SPELL_EFFECT_INCREASE_FOLLOWER_EXPERIENCE:
            {
#ifndef CROSS
                Player* l_Player = m_caster->ToPlayer();

                if (l_Player == nullptr)
                    return SPELL_FAILED_BAD_TARGETS;

                MS::Garrison::Manager* l_Garrison = l_Player->GetGarrison();

                if (l_Garrison == nullptr)
                    return SPELL_FAILED_BAD_TARGETS;

                MS::Garrison::GarrisonFollower* l_Follower = l_Garrison->GetFollower(m_Misc[0]);

                if (l_Follower == nullptr || !l_Follower->CanXP())
                    return SPELL_FAILED_BAD_TARGETS;

                break;
#else
                return SPELL_FAILED_BAD_TARGETS;
#endif
            }
            case SPELL_EFFECT_COLLECT_APPEARANCE:
            {
                TransmogSetEntry const* l_TransmogSetEntry = sTransmogSetStore.LookupEntry(m_spellInfo->Effects[i].MiscValue);
                if (!l_TransmogSetEntry)
                    return SPELL_FAILED_DONT_REPORT;

                if (l_TransmogSetEntry->ClassMask != 0 && !(m_caster->getClassMask() & l_TransmogSetEntry->ClassMask))
                    return SPELL_FAILED_DONT_REPORT;

                break;
            }
            default:
                break;
        }

        switch (m_spellInfo->Effects[i].ApplyAuraName)
        {
            case SPELL_AURA_MOD_STEALTH:
            {
                /// Smoke Bomb
                if (m_caster->HasAuraWithNegativeCaster(88611))
                    return SPELL_FAILED_CASTER_AURASTATE;

                /// No break needed, still need to check for Shadow Sight debuff
            }
            case SPELL_AURA_MOD_INVISIBILITY:
            {
                /// Battlegrounds - Shadow Sight
                if (m_caster->HasAura(34709))
                    return SPELL_FAILED_CASTER_AURASTATE;

                break;
            }
            case SPELL_AURA_MOD_POSSESS:
            case SPELL_AURA_MOD_CHARM:
            case SPELL_AURA_AOE_CHARM:
            {
                if (m_caster->GetCharmerGUID())
                    return SPELL_FAILED_CHARMED;

                if (m_spellInfo->Effects[i].ApplyAuraName == SPELL_AURA_MOD_CHARM
                    || m_spellInfo->Effects[i].ApplyAuraName == SPELL_AURA_MOD_POSSESS)
                {
                    if (m_caster->GetPetGUID())
                        return SPELL_FAILED_ALREADY_HAVE_SUMMON;

                    if (m_caster->GetCharmGUID())
                        return SPELL_FAILED_ALREADY_HAVE_CHARM;
                }

                if (Unit* target = m_targets.GetUnitTarget())
                {
                    if (target->GetTypeId() == TYPEID_UNIT && target->ToCreature()->IsVehicle())
                        return SPELL_FAILED_BAD_IMPLICIT_TARGETS;

                    if (target->IsMounted())
                        return SPELL_FAILED_CANT_BE_CHARMED;

                    if (target->GetCharmerGUID())
                        return SPELL_FAILED_CHARMED;

                    int32 damage = CalculateDamage(i, target);
                    if (damage && int32(target->getLevelForTarget(m_caster)) > damage)
                        return SPELL_FAILED_HIGHLEVEL;
                }

                break;
            }
            case SPELL_AURA_MOUNTED:
            {
                // Ignore map check if spell have AreaId. AreaId already checked and this prevent special mount spells
                bool allowMount = !m_caster->GetMap()->IsDungeon() || m_caster->GetMap()->IsBattlegroundOrArena();
                InstanceTemplate const* it = sObjectMgr->GetInstanceTemplate(m_caster->GetMapId());

                if (it)
                    allowMount = it->AllowMount;

                allowMount = allowMount || (m_spellInfo->AttributesCu & SPELL_ATTR0_CU_ALLOW_INDOOR_MOUNT);

                if (m_caster->IsPlayer() && !allowMount && !m_spellInfo->AreaGroupId)
                    return SPELL_FAILED_NO_MOUNTS_ALLOWED;

                if (m_caster->IsInDisallowedMountForm())
                    return SPELL_FAILED_NOT_SHAPESHIFT;

                // hex & mount
                if (m_caster->HasAura(51514))
                    return SPELL_FAILED_CONFUSED;

                /// Mount is not allowed if player is carrying the orb on KT battleground
                if (m_caster->IsPlayer() && m_caster->GetMapId() == 998)
                    if (BattlegroundKT* p_Battleground = dynamic_cast<BattlegroundKT*>(m_caster->ToPlayer()->GetBattleground()))
                        if (p_Battleground->IsOrbKeeper(m_caster->GetGUID()))
                            return SPELL_FAILED_NOT_IN_BATTLEGROUND;

                break;
            }
            case SPELL_AURA_RANGED_ATTACK_POWER_ATTACKER_BONUS:
            {
                if (!m_targets.GetUnitTarget())
                    return SPELL_FAILED_BAD_IMPLICIT_TARGETS;

                // can be casted at non-friendly unit or own pet/charm
                if (m_caster->IsFriendlyTo(m_targets.GetUnitTarget()))
                    return SPELL_FAILED_TARGET_FRIENDLY;

                break;
            }
            case SPELL_AURA_FLY:
            case SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED:
            {
                // not allow cast fly spells if not have req. skills  (all spells is self target)
                // allow always ghost flight spells
                if (m_originalCaster && m_originalCaster->IsPlayer() && m_originalCaster->isAlive())
                {
                    if (sWorld->getBoolConfig(CONFIG_TOURNAMENT_ENABLE) && (m_originalCaster->GetZoneId() == 1519 || m_originalCaster->GetZoneId() == 1637))
                        return SPELL_FAILED_DONT_REPORT;

                    Battlefield* Bf = sBattlefieldMgr->GetBattlefieldToZoneId(m_originalCaster->GetZoneId());
                    if (AreaTableEntry const* area = sAreaTableStore.LookupEntry(m_originalCaster->GetAreaId()))
                        if (area->Flags & AREA_FLAG_NO_FLY_ZONE  || (Bf && !Bf->CanFlyIn()))
                            return (_triggeredCastFlags & TRIGGERED_DONT_REPORT_CAST_ERROR) ? SPELL_FAILED_DONT_REPORT : SPELL_FAILED_NOT_HERE;
                }
                break;
            }
            case SPELL_AURA_PERIODIC_MANA_LEECH:
            {
                if (m_spellInfo->Effects[i].IsTargetingArea() || (m_spellInfo->Effects[i].TargetA.GetTarget() == TARGET_UNIT_NEARBY_ENTRY))
                    break;

                if (!m_targets.GetUnitTarget())
                    return SPELL_FAILED_BAD_IMPLICIT_TARGETS;

                if (m_caster->GetTypeId() != TYPEID_PLAYER || m_CastItem)
                    break;

                if (m_targets.GetUnitTarget()->getPowerType() != POWER_MANA)
                    return SPELL_FAILED_BAD_TARGETS;

                break;
            }
            default:
                break;
        }
    }

#ifndef CROSS
    /// Should be successful if at least one effect is passed
    if (m_spellInfo->HasEffect(SpellEffects::SPELL_EFFECT_INCREASE_FOLLOWER_ITEM_LEVEL))
    {
        SpellCastResult l_LastResult = SpellCastResult::SPELL_FAILED_DONT_REPORT;
        for (uint32 j = 0; j < m_spellInfo->EffectCount; ++j)
        {
            if (m_spellInfo->Effects[j].Effect != SpellEffects::SPELL_EFFECT_INCREASE_FOLLOWER_ITEM_LEVEL)
                continue;

            Player* l_Player = m_caster->ToPlayer();
            if (l_Player == nullptr)
                return SpellCastResult::SPELL_FAILED_BAD_TARGETS;

            MS::Garrison::Manager* l_Garrison = l_Player->GetGarrison();
            if (l_Garrison == nullptr)
                return SpellCastResult::SPELL_FAILED_BAD_TARGETS;

            SpellCastResult l_Result = l_Garrison->CanUpgradeFollowerItemLevelWith(m_Misc[0], GetSpellInfo(), static_cast<SpellEffIndex>(j));
            if (l_LastResult != SpellCastResult::SPELL_CAST_OK)
                l_LastResult = l_Result;
        }
        if (l_LastResult != SpellCastResult::SPELL_CAST_OK)
            return l_LastResult;
    }
#endif

    switch(m_spellInfo->Id)
    {
        case 23517: ///< Create Healthstone
        {
            if (m_caster->IsPlayer() && m_caster->ToPlayer()->HasItemCount(5512, 1))
                m_caster->ToPlayer()->DestroyItemCount(5512, 1, true);
            break;
        }
        case 50334: // Berserk
        case 61336: // Survival Instincts
        {
            if (m_caster->GetTypeId() != TYPEID_PLAYER || !m_caster->ToPlayer()->IsInFeralForm())
                return SPELL_FAILED_ONLY_SHAPESHIFT;
        }
        default: break;
    }

    /// Fix a bug when spells can be casted in fear
    if (m_caster->HasAuraType(SPELL_AURA_MOD_FEAR) || m_caster->HasAuraType(SPELL_AURA_MOD_FEAR_2))
    {
        if (!m_spellInfo->IsRemoveLossControlEffects() && !m_spellInfo->IsRemoveFear() && m_spellInfo->Id != 1022 && !(AttributesCustomEx5 & SPELL_ATTR5_USABLE_WHILE_FEARED)) ///< Specific case of Hand of Protection & Hellfire damage (Immolation aura tick)
            return SPELL_FAILED_FLEEING;
    }

    // hex
    if (m_caster->HasAuraWithMechanic(1LL << MECHANIC_POLYMORPH))
    {
        // Shapeshift
        if (m_spellInfo->HasAura(SPELL_AURA_MOD_SHAPESHIFT) && m_spellInfo->Id != 33891)    // Tree of Life is exception
            return SPELL_FAILED_CHARMED;

        // Teleport
        if (m_spellInfo->HasEffect(SPELL_EFFECT_TELEPORT_UNITS))
            return SPELL_FAILED_CHARMED;

        // Summon totem
        if (AttributesCustomEx7 & SPELL_ATTR7_SUMMON_TOTEM || m_spellInfo->HasEffect(SPELL_EFFECT_CAST_BUTTON))
            return SPELL_FAILED_CHARMED;

        // Fishing
        if (AttributesCustomEx & SPELL_ATTR1_IS_FISHING)
            return SPELL_FAILED_CHARMED;

        // Death Knight - Outbreak
        if (m_spellInfo->Id == 77575)
            return SPELL_FAILED_CHARMED;

        // Drink
        switch (m_spellInfo->GetSpellSpecific())
        {
            case SpellSpecificType::SpellSpecificDrink:
            case SpellSpecificType::SpellSpecificFood:
            case SpellSpecificType::SpellSpecificFoodAndDrink:
                return SPELL_FAILED_CHARMED;
        default:
            break;
        }
    }

    // check trade slot case (last, for allow catch any another cast problems)
    if (m_targets.GetTargetMask() & TARGET_FLAG_TRADE_ITEM)
    {
        if (m_CastItem)
            return SPELL_FAILED_ITEM_ENCHANT_TRADE_WINDOW;

        if (m_caster->GetTypeId() != TYPEID_PLAYER)
            return SPELL_FAILED_NOT_TRADING;

        TradeData* l_MyTrade = m_caster->ToPlayer()->GetTradeData();
        if (!l_MyTrade)
            return SPELL_FAILED_NOT_TRADING;

        // Slot: TRADE_SLOT_NONTRADED is only for non tradable items, enchantment case, open lock ...
        TradeSlots l_Slot = TradeSlots(m_targets.GetItemTargetGUID());
        if (l_Slot != TRADE_SLOT_NONTRADED)
            return SPELL_FAILED_BAD_TARGETS;

        if (!IsTriggered())
        {
            if (l_MyTrade->GetSpell())
                return SPELL_FAILED_ITEM_ALREADY_ENCHANTED;
        }
    }

    /// Check if caster has at least 1 combo point for spells that require combo points
    if (m_needComboPoints && !m_caster->GetPower(Powers::POWER_COMBO_POINT))
        return SPELL_FAILED_NO_COMBO_POINTS;

    // all ok
    return SPELL_CAST_OK;
}

SpellCastResult Spell::CheckPetCast(Unit* target)
{
    // Prevent spellcast interruption by another spellcast
    if (m_caster->HasUnitState(UNIT_STATE_CASTING) && !(_triggeredCastFlags & TRIGGERED_IGNORE_CAST_IN_PROGRESS) && !(AttributesCustomEx9 & SPELL_ATTR9_CASTABLE_WHILE_CAST_IN_PROGRESS))
        return SPELL_FAILED_SPELL_IN_PROGRESS;

    // Prevent using of ability if is already casting an ability that has aura type SPELL_AURA_ALLOW_ONLY_ABILITY
    if (m_caster->HasAuraType(SPELL_AURA_ALLOW_ONLY_ABILITY) && !(_triggeredCastFlags & TRIGGERED_IGNORE_CAST_IN_PROGRESS) && !(AttributesCustomEx9 & SPELL_ATTR9_CASTABLE_WHILE_CAST_IN_PROGRESS))
        return SPELL_FAILED_SPELL_IN_PROGRESS;

    // dead owner (pets still alive when owners ressed?)
    if (Unit* owner = m_caster->GetCharmerOrOwner())
        if (!owner->isAlive())
            return SPELL_FAILED_CASTER_DEAD;

    if (!target && m_targets.GetUnitTarget())
        target = m_targets.GetUnitTarget();

    if (m_spellInfo->NeedsExplicitUnitTarget())
    {
        if (!target)
            return SPELL_FAILED_BAD_IMPLICIT_TARGETS;

        m_targets.SetUnitTarget(target);
    }

    if (Unit* l_Owner = m_caster->GetCharmerOrOwner())
    {
        if (target && !m_spellInfo->IsPositive() && !l_Owner->IsValidAttackTarget(target))
            return SPELL_FAILED_BAD_TARGETS;
    }

    return CheckCast(true);
}

SpellCastResult Spell::CheckCasterAuras() const
{
    uint8 school_immune = 0;
    uint64 mechanic_immune = 0;
    uint32 dispel_immune = 0;

    // Check if the spell grants school or mechanic immunity.
    // We use bitmasks so the loop is done only once and not on every aura check below.
    if (AttributesCustomEx & SPELL_ATTR1_DISPEL_AURAS_ON_IMMUNITY)
    {
        for (uint8 i = 0; i < m_spellInfo->EffectCount; ++i)
        {
            if (m_spellInfo->Effects[i].ApplyAuraName == SPELL_AURA_SCHOOL_IMMUNITY)
                school_immune |= uint32(m_spellInfo->Effects[i].MiscValue);
            else if (m_spellInfo->Effects[i].ApplyAuraName == SPELL_AURA_MECHANIC_IMMUNITY)
                mechanic_immune |= 1LL << uint64(m_spellInfo->Effects[i].MiscValue);
            else if (m_spellInfo->Effects[i].ApplyAuraName == SPELL_AURA_DISPEL_IMMUNITY)
                dispel_immune |= SpellInfo::GetDispelMask(DispelType(m_spellInfo->Effects[i].MiscValue));
        }

        // immune movement impairment and loss of control
        if (m_spellInfo->IsRemoveLossControlEffects())
            mechanic_immune = IMMUNE_TO_MOVEMENT_IMPAIRMENT_AND_LOSS_CONTROL_MASK;
    }

    bool usableInStun = AttributesCustomEx5 & SPELL_ATTR5_USABLE_WHILE_STUNNED;

    // Life Cocoon is usable while stunned with Glyph of life cocoon
    if (m_spellInfo->Id == 116849 && m_caster->HasAura(124989))
        usableInStun = true;

    // Check whether the cast should be prevented by any state you might have.
    SpellCastResult prevented_reason = SPELL_CAST_OK;
    // Have to check if there is a stun aura. Otherwise will have problems with ghost aura apply while logging out
    uint32 unitflag = m_caster->GetUInt32Value(UNIT_FIELD_FLAGS);     // Get unit state

    if (unitflag & UNIT_FLAG_STUNNED && !usableInStun)
        prevented_reason = SPELL_FAILED_STUNNED;
    else if (unitflag & UNIT_FLAG_CONFUSED && !(AttributesCustomEx5 & SPELL_ATTR5_USABLE_WHILE_CONFUSED))
        prevented_reason = SPELL_FAILED_CONFUSED;
    else if (unitflag & UNIT_FLAG_FLEEING && !(AttributesCustomEx5 & SPELL_ATTR5_USABLE_WHILE_FEARED))
        prevented_reason = SPELL_FAILED_FLEEING;
    else if (unitflag & UNIT_FLAG_SILENCED && m_spellInfo->PreventionType & (SpellPreventionMask::Silence) && !(m_spellInfo->HasAttribute(SPELL_ATTR8_USABLE_WHILE_SILENCED)))
        prevented_reason = SPELL_FAILED_SILENCED;
    else if (unitflag & UNIT_FLAG_PACIFIED && m_spellInfo->PreventionType & (SpellPreventionMask::Pacify))
        prevented_reason = SPELL_FAILED_PACIFIED;
    else if (m_caster->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_NO_ACTIONS) && m_spellInfo->PreventionType & (SpellPreventionMask::NoActions))
        prevented_reason = SPELL_FAILED_NO_ACTIONS;

    // Barkskin & Hex hotfix 4.3 patch http://eu.battle.net/wow/ru/blog/10037151
    if (m_spellInfo->Id == 22812 && m_caster->HasAura(51514))
        prevented_reason = SPELL_FAILED_PACIFIED;

    // Attr must make flag drop spell totally immune from all effects
    if (prevented_reason != SPELL_CAST_OK)
    {
        if (school_immune || mechanic_immune || dispel_immune)
        {
            //Checking auras is needed now, because you are prevented by some state but the spell grants immunity.
            Unit::AuraApplicationMap const& auras = m_caster->GetAppliedAuras();
            for (Unit::AuraApplicationMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
            {
                Aura const* aura = itr->second->GetBase();
                SpellInfo const* auraInfo = aura->GetSpellInfo();
                if (auraInfo->GetAllEffectsMechanicMask() & mechanic_immune)
                    continue;
                if (auraInfo->GetSchoolMask() & school_immune && !(AttributesCustomEx & SPELL_ATTR1_UNAFFECTED_BY_SCHOOL_IMMUNE))
                    continue;
                if (auraInfo->GetDispelMask() & dispel_immune)
                    continue;

                //Make a second check for spell failed so the right SPELL_FAILED message is returned.
                //That is needed when your casting is prevented by multiple states and you are only immune to some of them.
                for (uint8 i = 0; i < m_spellInfo->EffectCount; ++i)
                {
                    if (AuraEffect* part = aura->GetEffect(i))
                    {
                        switch (part->GetAuraType())
                        {
                            case SPELL_AURA_MOD_STUN:
                            {
                                if (!usableInStun || !(auraInfo->GetAllEffectsMechanicMask() & (1LL << MECHANIC_STUN)))
                                    return SPELL_FAILED_STUNNED;
                                break;
                            }
                            case SPELL_AURA_MOD_CONFUSE:
                            {
                                if (!(AttributesCustomEx5 & SPELL_ATTR5_USABLE_WHILE_CONFUSED))
                                    return SPELL_FAILED_CONFUSED;
                                break;
                            }
                            case SPELL_AURA_MOD_FEAR:
                            case SPELL_AURA_MOD_FEAR_2:
                            {
                                if (!(AttributesCustomEx5 & SPELL_ATTR5_USABLE_WHILE_FEARED))
                                    return SPELL_FAILED_FLEEING;
                                break;
                            }
                            case SPELL_AURA_MOD_SILENCE:
                            case SPELL_AURA_MOD_PACIFY:
                            case SPELL_AURA_MOD_PACIFY_SILENCE:
                            {
                                if (m_spellInfo->PreventionType & (SpellPreventionMask::Pacify))
                                    return SPELL_FAILED_PACIFIED;
                                else if (m_spellInfo->PreventionType & (SpellPreventionMask::Silence) && !(m_spellInfo->HasAttribute(SPELL_ATTR8_USABLE_WHILE_SILENCED)))
                                    return SPELL_FAILED_SILENCED;
                                break;
                            }
                            default:
                                break;
                        }
                    }
                }
            }
        }
        // You are prevented from casting and the spell casted does not grant immunity. Return a failed error.
        else
            return prevented_reason;
    }
    return SPELL_CAST_OK;
}

SpellCastResult Spell::CheckArenaAndRatedBattlegroundCastRules(Battleground const* p_Battleground)
{
    bool isRatedBG = p_Battleground ? p_Battleground->IsRatedBG() : false;
    bool isArena = p_Battleground ? p_Battleground->isArena() : false;

    // check USABLE attributes
    // USABLE takes precedence over NOT_USABLE
    if (isRatedBG && AttributesCustomEx9 & SPELL_ATTR9_USABLE_IN_RATED_BATTLEGROUNDS)
        return SPELL_CAST_OK;

    if (isArena && AttributesCustomEx4 & SPELL_ATTR4_USABLE_IN_ARENA)
        return SPELL_CAST_OK;

    // check NOT_USABLE attributes
    if (AttributesCustomEx4 & SPELL_ATTR4_NOT_USABLE_IN_ARENA_OR_RATED_BG)
    {
        if (isArena)
            return SPELL_FAILED_NOT_IN_ARENA;
        else if (isRatedBG)
            return SPELL_FAILED_NOT_IN_RATED_BATTLEGROUND;
    }

    if (isArena && AttributesCustomEx9 & SPELL_ATTR9_NOT_USABLE_IN_ARENA)
        return SPELL_FAILED_NOT_IN_ARENA;

    // check cooldowns
    uint32 spellCooldown = m_spellInfo->GetRecoveryTime();
    if (isArena && spellCooldown > 10 * MINUTE * IN_MILLISECONDS) // not sure if still needed
        return SPELL_FAILED_NOT_IN_ARENA;

    if (isRatedBG && spellCooldown > 15 * MINUTE * IN_MILLISECONDS)
        return SPELL_FAILED_NOT_IN_RATED_BATTLEGROUND;

    return SPELL_CAST_OK;
}

bool Spell::CanAutoCast(Unit* target)
{
    uint64 targetguid = target->GetGUID();

    if (!BypassAuraCheckForAutoRepeatSpell())
    {
        for (uint32 j = 0; j < m_spellInfo->EffectCount; ++j)
        {
            if (m_spellInfo->Effects[j].IsApplyingAura())
            {
                if (m_spellInfo->StackAmount <= 1)
                {
                    if (target->HasAuraEffect(m_spellInfo->Id, j, m_caster->GetGUID()))
                        return false;
                }
                else
                {
                    if (AuraEffect* aureff = target->GetAuraEffect(m_spellInfo->Id, j, m_caster->GetGUID()))
                        if (aureff->GetBase()->GetStackAmount() >= m_spellInfo->StackAmount)
                            return false;
                }
            }
            else if (m_spellInfo->Effects[j].IsAreaAuraEffect())
            {
                if (target->HasAuraEffect(m_spellInfo->Id, j, m_caster->GetGUID()))
                    return false;
            }
        }
    }

    /// Pets shouldn't auto cast taunting spells on dungeon/raid bosses
    if (target && target->ToCreature() && target->ToCreature()->IsDungeonBoss() && m_spellInfo->HasEffect(SpellEffects::SPELL_EFFECT_ATTACK_ME))
        return false;

    SpellCastResult result = CheckPetCast(target);

    if (result == SPELL_CAST_OK || result == SPELL_FAILED_UNIT_NOT_INFRONT)
    {
        SelectSpellTargets();
        //check if among target units, our WANTED target is as well (->only self cast spells return false)
        for (std::list<TargetInfo>::iterator ihit= m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
            if (ihit->targetGUID == targetguid)
                return true;
    }
    return false;                                           //target invalid
}

SpellCastResult Spell::CheckRange(bool strict)
{
    Unit* target = m_targets.GetUnitTarget();
    float minRange = 0.0f;
    float maxRange = 0.0f;
    float rangeMod = 0.0f;
    if (strict && IsNextMeleeSwingSpell())
        maxRange = 100.0f;
    else if (m_spellInfo->Id == 15290) ///Range check already done in the script of the heal. Couldnt change it in SpellMgr.cpp
        return SPELL_CAST_OK;
    else if (m_spellInfo->RangeEntry)
    {
        if (m_spellInfo->RangeEntry->Flags & SPELL_RANGE_MELEE)
        {
            rangeMod = m_caster->GetCombatReach() + 4.0f / 3.0f;
            if (target)
                rangeMod += target->GetCombatReach();
            else
                rangeMod += m_caster->GetCombatReach();

            rangeMod = std::max(rangeMod, NOMINAL_MELEE_RANGE);
        }
        else
        {
            float meleeRange = 0.0f;
            if (m_spellInfo->RangeEntry->Flags & SPELL_RANGE_RANGED)
            {
                meleeRange = m_caster->GetCombatReach() + 4.0f / 3.0f;
                if (target)
                    meleeRange += target->GetCombatReach();
                else
                    meleeRange += m_caster->GetCombatReach();

                meleeRange = std::max(meleeRange, NOMINAL_MELEE_RANGE);
            }

            minRange = m_caster->GetSpellMinRangeForTarget(target, m_spellInfo) + meleeRange;
            maxRange = m_caster->GetSpellMaxRangeForTarget(target, m_spellInfo);

            if (target || m_targets.GetCorpseTarget())
            {
                rangeMod = m_caster->GetCombatReach();
                if (target)
                    rangeMod += target->GetCombatReach();

                if (minRange > 0.0f && !(m_spellInfo->RangeEntry->Flags & SPELL_RANGE_RANGED))
                    minRange += rangeMod;
            }
        }

        if (target && m_caster->IsMoving() && target->IsMoving() && !m_caster->IsWalking() && !target->IsWalking() &&
            (m_spellInfo->RangeEntry->Flags & SPELL_RANGE_MELEE || target->GetTypeId() == TYPEID_PLAYER))
            rangeMod += 5.0f / 3.0f;
    }

    if (AttributesCustom & SPELL_ATTR0_REQ_AMMO && m_caster->GetTypeId() == TYPEID_PLAYER)
        if (Item* ranged = m_caster->ToPlayer()->GetWeaponForAttack(WeaponAttackType::RangedAttack, true))
            maxRange *= ranged->GetTemplate()->RangedModRange * 0.01f;

    if (Player* modOwner = m_caster->GetSpellModOwner())
        modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_RANGE, maxRange, this);

    maxRange += rangeMod;

    minRange *= minRange;
    maxRange *= maxRange;

    ///< if a current spell has been started a casting successfully, then spell should be able to missile + 5 yds
    if (m_caster->GetCurrentSpellCastTime(GetSpellInfo()->Id) > 0)
        maxRange += 5.0f;

    /// Charge from Death Sentence should have no min range
    if (m_spellInfo->Id == 40602 && _triggeredCastFlags & TRIGGERED_FULL_MASK)
        minRange = 0;

    if (target && target != m_caster)
    {
        switch (m_spellInfo->Id)
        {
            case 116858: ///< Chaos Bolt
            case 17962: ///< Conflagrate
            case 348: ///< Immolate
            case 29722: /// < Incinerate
            case 6789: ///< Mortal Coil
                       /// Havoc && Bane of Havoc
                if (IsTriggered() && (target->HasAura(80240, m_caster->GetGUID()) || target->HasAura(200548, m_caster->GetGUID())))
                    maxRange *= 2.5;
                break;
        }

        if (m_caster->GetExactDistSq(target) > maxRange)
            return !(_triggeredCastFlags & TRIGGERED_DONT_REPORT_CAST_ERROR) ? SPELL_FAILED_OUT_OF_RANGE : SPELL_FAILED_DONT_REPORT;

        if (minRange > 0.0f && m_caster->GetExactDistSq(target) < minRange)
            return !(_triggeredCastFlags & TRIGGERED_DONT_REPORT_CAST_ERROR) ? SPELL_FAILED_OUT_OF_RANGE : SPELL_FAILED_DONT_REPORT;

        // HACK:
        bool l_SkipInFrontCheck = false;
        switch (m_spellInfo->Id)
        {
            case 116858: ///< Chaos Bolt
            case 17962: ///< Conflagrate
            case 348: ///< Immolate
            case 29722: /// < Incinerate
            case 6789: ///< Mortal Coil
                /// Havoc && Bane of Havoc
                if (IsTriggered() && (target->HasAura(80240, m_caster->GetGUID()) || target->HasAura(200548, m_caster->GetGUID())))
                    l_SkipInFrontCheck = true;
                break;
        }

        ///< Penance should skips infront check for friendly targets
        if (m_caster->IsFriendlyTo(target) && m_spellInfo->Id == 47540)
            l_SkipInFrontCheck = true;

        if (!l_SkipInFrontCheck && m_caster->GetTypeId() == TYPEID_PLAYER &&
            (((m_spellInfo->FacingCasterFlags & SPELL_FACING_FLAG_INFRONT) && !m_caster->isInFront(target))
                && !m_caster->IsWithinBoundaryRadius(target)))
            return !(_triggeredCastFlags & TRIGGERED_DONT_REPORT_CAST_ERROR) ? SPELL_FAILED_UNIT_NOT_INFRONT : SPELL_FAILED_DONT_REPORT;
    }

    if (m_targets.HasDst() && !m_targets.HasTraj())
    {
        if (m_caster->GetExactDistSq(m_targets.GetDstPos()) > maxRange)
            return !(_triggeredCastFlags & TRIGGERED_DONT_REPORT_CAST_ERROR) ? SPELL_FAILED_OUT_OF_RANGE : SPELL_FAILED_DONT_REPORT;
        if (minRange > 0.0f && m_caster->GetExactDistSq(m_targets.GetDstPos()) < minRange)
            return !(_triggeredCastFlags & TRIGGERED_DONT_REPORT_CAST_ERROR) ? SPELL_FAILED_OUT_OF_RANGE : SPELL_FAILED_DONT_REPORT;
    }

    return SPELL_CAST_OK;
}

SpellCastResult Spell::CheckPower()
{
    // item cast not used power
    if (m_CastItem)
        return SPELL_CAST_OK;

    // Dark Simulacrum case
    if (isStolen)
        return SPELL_CAST_OK;

    // health as power used - need check health amount
    for (auto itr : m_spellInfo->SpellPowers)
    {
        if (itr->PowerType == POWER_HEALTH)
        {
            if (int32(m_caster->GetHealth()) <= m_powerCost[POWER_TO_INDEX(POWER_HEALTH)])
                return SPELL_FAILED_CASTER_AURASTATE;
            return SPELL_CAST_OK;
        }
    }

    // Check valid power type
    for (auto itr : m_spellInfo->SpellPowers)
    {
        if (itr->PowerType >= MAX_POWERS)
            return SPELL_FAILED_UNKNOWN;
    }

    switch (m_spellInfo->Id)
    {
        case 53385:  ///< Divine Storm
        {
            if (m_powerCost[POWER_HOLY_POWER] != 0 && m_caster->HasAura(144595))
            {
                m_powerCost[POWER_HOLY_POWER] = 0;
                m_caster->RemoveAuraFromStack(144595);
            }

            break;
        }
        default:
            break;
    }

    // Check power amount
    for (auto itr : m_spellInfo->SpellPowers)
    {
        if (itr->RequiredAuraSpellId && !m_caster->HasAura(itr->RequiredAuraSpellId))
            continue;

        Powers powerType = Powers(itr->PowerType);

        if (itr->PowerType == POWER_RUNES)
        {
            SpellCastResult failReason = CheckRuneCost(m_powerCost[POWER_TO_INDEX(powerType)]);
            if (failReason != SPELL_CAST_OK)
                return failReason;

            continue;
        }

        if (int32(m_caster->GetPower(powerType)) < m_powerCost[POWER_TO_INDEX(powerType)])
            return SPELL_FAILED_NO_POWER;
    }

    return SPELL_CAST_OK;
}

SpellCastResult Spell::CheckItems()
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return SPELL_CAST_OK;

    Player* p_caster = (Player*)m_caster;

    if (!m_CastItem)
    {
        if (m_castItemGUID)
            return SPELL_FAILED_ITEM_NOT_READY;
    }
    else
    {
        uint32 itemid = m_CastItem->GetEntry();
        if (!p_caster->HasItemCount(itemid))
            return SPELL_FAILED_ITEM_NOT_READY;

        ItemTemplate const* proto = m_CastItem->GetTemplate();
        if (!proto)
            return SPELL_FAILED_ITEM_NOT_READY;

        for (int i = 0; i < MAX_ITEM_SPELLS; ++i)
            if (proto->Spells[i].SpellCharges)
                if (m_CastItem->GetSpellCharges(i) == 0)
                    return SPELL_FAILED_NO_CHARGES_REMAIN;

        // consumable cast item checks
        if (proto->Class == ITEM_CLASS_CONSUMABLE && m_targets.GetUnitTarget())
        {
            // such items should only fail if there is no suitable effect at all - see Rejuvenation Potions for example
            SpellCastResult failReason = SPELL_CAST_OK;
            for (int i = 0; i < m_spellInfo->EffectCount; i++)
            {
                // skip check, pet not required like checks, and for TARGET_UNIT_PET m_targets.GetUnitTarget() is not the real target but the caster
                if (m_spellInfo->Effects[i].TargetA.GetTarget() == TARGET_UNIT_PET)
                    continue;

                if (m_spellInfo->Effects[i].Effect == SPELL_EFFECT_HEAL)
                {
                    if (m_targets.GetUnitTarget()->IsFullHealth())
                    {
                        failReason = SPELL_FAILED_ALREADY_AT_FULL_HEALTH;
                        continue;
                    }
                    else
                    {
                        failReason = SPELL_CAST_OK;
                        break;
                    }
                }

                // Mana Potion, Rage Potion, Thistle Tea(Rogue), ...
                if (m_spellInfo->Effects[i].Effect == SPELL_EFFECT_ENERGIZE)
                {
                    if (m_spellInfo->Effects[i].MiscValue < 0 || m_spellInfo->Effects[i].MiscValue >= int8(MAX_POWERS))
                    {
                        failReason = SPELL_FAILED_ALREADY_AT_FULL_POWER;
                        continue;
                    }

                    Powers power = Powers(m_spellInfo->Effects[i].MiscValue);
                    if (m_targets.GetUnitTarget()->GetPower(power) == m_targets.GetUnitTarget()->GetMaxPower(power))
                    {
                        failReason = SPELL_FAILED_ALREADY_AT_FULL_POWER;
                        continue;
                    }
                    else
                    {
                        failReason = SPELL_CAST_OK;
                        break;
                    }
                }
            }
            if (failReason != SPELL_CAST_OK)
                return failReason;
        }
    }

    // check target item
    if (m_targets.GetItemTargetGUID())
    {
        if (m_caster->GetTypeId() != TYPEID_PLAYER)
            return SPELL_FAILED_BAD_TARGETS;

        if (!m_targets.GetItemTarget())
            return SPELL_FAILED_ITEM_GONE;

        if (!m_targets.GetItemTarget()->IsFitToSpellRequirements(m_spellInfo))
            return SPELL_FAILED_EQUIPPED_ITEM_CLASS;
    }
    // if not item target then required item must be equipped
    else
    {
        if (m_caster->IsPlayer() && !m_caster->ToPlayer()->HasItemFitToSpellRequirements(m_spellInfo))
            return SPELL_FAILED_EQUIPPED_ITEM_CLASS;
    }

    // check spell focus object
    if (m_spellInfo->RequiresSpellFocus)
    {
        CellCoord p(JadeCore::ComputeCellCoord(m_caster->GetPositionX(), m_caster->GetPositionY()));
        Cell cell(p);

        GameObject* ok = NULL;
        JadeCore::GameObjectFocusCheck go_check(m_caster, m_spellInfo->RequiresSpellFocus);
        JadeCore::GameObjectSearcher<JadeCore::GameObjectFocusCheck> checker(m_caster, ok, go_check);

        TypeContainerVisitor<JadeCore::GameObjectSearcher<JadeCore::GameObjectFocusCheck>, GridTypeMapContainer > object_checker(checker);
        Map& map = *m_caster->GetMap();
        cell.Visit(p, object_checker, map, *m_caster, m_caster->GetVisibilityRange());

        if (!ok)
            return SPELL_FAILED_REQUIRES_SPELL_FOCUS;

        focusObject = ok;                                   // game object found in range
    }

    // do not take reagents for these item casts
    if (!(m_CastItem && m_CastItem->GetTemplate()->Flags & int32(ItemFlags::NO_REAGENT_COST)))
    {
        bool checkReagents = !(_triggeredCastFlags & TRIGGERED_IGNORE_POWER_AND_REAGENT_COST) && !p_caster->CanNoReagentCast(m_spellInfo);
        // Not own traded item (in trader trade slot) requires reagents even if triggered spell
        if (!checkReagents)
            if (Item* targetItem = m_targets.GetItemTarget())
                if (targetItem->GetOwnerGUID() != m_caster->GetGUID())
                    checkReagents = true;

        // check reagents (ignore triggered spells with reagents processed by original spell) and special reagent ignore case.
        if (checkReagents)
        {
            // currency check
            if (m_spellInfo->CurrencyID != 0 && m_spellInfo->CurrencyCount != 0)
            {
                uint32 currencyid = m_spellInfo->CurrencyID;
                uint32 currencycount = m_spellInfo->CurrencyCount;

                if (!p_caster->HasCurrency(currencyid, currencycount))
                    return SPELL_FAILED_REAGENTS;
            }
            for (uint32 i = 0; i < MAX_SPELL_REAGENTS; i++)
            {
                if (m_spellInfo->Reagent[i] <= 0)
                    continue;

                uint32 itemid = m_spellInfo->Reagent[i];
                uint32 itemcount = m_spellInfo->ReagentCount[i];

                // if CastItem is also spell reagent
                if (m_CastItem && m_CastItem->GetEntry() == itemid)
                {
                    ItemTemplate const* proto = m_CastItem->GetTemplate();
                    if (!proto)
                        return SPELL_FAILED_ITEM_NOT_READY;
                    for (int s = 0; s < MAX_ITEM_PROTO_SPELLS; ++s)
                    {
                        // CastItem will be used up and does not count as reagent
                        int32 charges = m_CastItem->GetSpellCharges(s);
                        if (proto->Spells[s].SpellCharges < 0 && abs(charges) < 2)
                        {
                            ++itemcount;
                            break;
                        }
                    }
                }
                if (!p_caster->HasItemCount(itemid, itemcount, false, true))
                    return SPELL_FAILED_REAGENTS;
            }
        }

        // check totem-item requirements (items presence in inventory)
        uint32 totems = 2;
        for (int i = 0; i < 2; ++i)
        {
            if (m_spellInfo->Totem[i] != 0)
            {
                if (p_caster->HasItemCount(m_spellInfo->Totem[i]))
                {
                    totems -= 1;
                    continue;
                }
            }
            else
            totems -= 1;
        }
        if (totems != 0)
            return SPELL_FAILED_TOTEMS;
    }

    // special checks for spell effects
    for (int i = 0; i < m_spellInfo->EffectCount; i++)
    {
        switch (m_spellInfo->Effects[i].Effect)
        {
            case SPELL_EFFECT_CREATE_ITEM:
            case SPELL_EFFECT_CREATE_LOOT:
            {
                if (!IsTriggered() && m_spellInfo->Effects[i].ItemType)
                {
                    ItemPosCountVec dest;
                    InventoryResult msg = p_caster->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, m_spellInfo->Effects[i].ItemType, 1);
                    if (msg != EQUIP_ERR_OK)
                    {
                        ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(m_spellInfo->Effects[i].ItemType);
                        // TODO: Needs review
                        if (pProto && !(pProto->ItemLimitCategory))
                        {
                            p_caster->SendEquipError(msg, NULL, NULL, m_spellInfo->Effects[i].ItemType);
                            return SPELL_FAILED_DONT_REPORT;
                        }
                        else
                        {
                            if (!(m_spellInfo->Id == 759)) // Conjure Mana Gem
                                return SPELL_FAILED_TOO_MANY_OF_ITEM;
                            else if (!(p_caster->HasItemCount(m_spellInfo->Effects[i].ItemType)))
                                return SPELL_FAILED_TOO_MANY_OF_ITEM;
                            else if (!(m_spellInfo->Id == 759))
                                p_caster->CastSpell(m_caster, m_spellInfo->Effects[EFFECT_1].CalcValue(), false);        // move this to anywhere
                            return SPELL_FAILED_DONT_REPORT;
                        }
                    }
                }
                break;
            }
            case SPELL_EFFECT_ENCHANT_ITEM:
                if (m_spellInfo->Effects[i].ItemType && m_targets.GetItemTarget()
                    && (m_targets.GetItemTarget()->IsVellum()))
                {
                    // cannot enchant vellum for other player
                    if (m_targets.GetItemTarget()->GetOwner() != m_caster)
                        return SPELL_FAILED_NOT_TRADEABLE;
                    // do not allow to enchant vellum from scroll made by vellum-prevent exploit
                    if (m_CastItem && m_CastItem->GetTemplate()->Flags & int32(ItemFlags::NO_REAGENT_COST))
                        return SPELL_FAILED_TOTEM_CATEGORY;
                    ItemPosCountVec dest;
                    InventoryResult msg = p_caster->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, m_spellInfo->Effects[i].ItemType, 1);
                    if (msg != EQUIP_ERR_OK)
                    {
                        p_caster->SendEquipError(msg, NULL, NULL, m_spellInfo->Effects[i].ItemType);
                        return SPELL_FAILED_DONT_REPORT;
                    }
                }
            case SPELL_EFFECT_ENCHANT_ITEM_PRISMATIC:
            {
                Item* targetItem = m_targets.GetItemTarget();
                if (!targetItem)
                    return SPELL_FAILED_ITEM_NOT_FOUND;

                uint32 l_ItemLevel = 0;
                if (Player* l_Owner = targetItem->GetOwner())
                    l_ItemLevel = targetItem->GetItemLevel(l_Owner);
                else
                    l_ItemLevel = targetItem->GetTemplate()->ItemLevel;

                if (l_ItemLevel < m_spellInfo->BaseLevel)
                    return SPELL_FAILED_LOWLEVEL;

                if (m_spellInfo->MaxLevel > 0 && l_ItemLevel > m_spellInfo->MaxLevel)
                    return SPELL_FAILED_HIGHLEVEL;

                bool isItemUsable = false; m_spellInfo->MaxLevel;
                for (uint8 e = 0; e < MAX_ITEM_PROTO_SPELLS; ++e)
                {
                    ItemTemplate const* proto = targetItem->GetTemplate();
                    if (proto->Spells[e].SpellId && proto->Spells[e].SpellTrigger == ITEM_SPELLTRIGGER_ON_USE)
                    {
                        isItemUsable = true;
                        break;
                    }
                }

                SpellItemEnchantmentEntry const* pEnchant = sSpellItemEnchantmentStore.LookupEntry(m_spellInfo->Effects[i].MiscValue);
                // do not allow adding usable enchantments to items that have use effect already
                if (pEnchant && isItemUsable)
                    for (uint8 s = 0; s < MAX_ENCHANTMENT_SPELLS; ++s)
                        if (pEnchant->type[s] == ITEM_ENCHANTMENT_TYPE_USE_SPELL)
                            return SPELL_FAILED_ON_USE_ENCHANT;

                // Not allow enchant in trade slot for some enchant type
                if (targetItem->GetOwner() != m_caster)
                {
                    if (!pEnchant)
                        return SPELL_FAILED_ERROR;
                    if (pEnchant->Flags & ENCHANTMENT_CAN_SOULBOUND)
                        return SPELL_FAILED_NOT_TRADEABLE;
                }
                break;
            }
            case SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY:
            {
                Item* item = m_targets.GetItemTarget();
                if (!item)
                    return SPELL_FAILED_ITEM_NOT_FOUND;
                // Not allow enchant in trade slot for some enchant type
                if (item->GetOwner() != m_caster)
                {
                    uint32 enchant_id = m_spellInfo->Effects[i].MiscValue;
                    SpellItemEnchantmentEntry const* pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
                    if (!pEnchant)
                        return SPELL_FAILED_ERROR;
                    if (pEnchant->Flags & ENCHANTMENT_CAN_SOULBOUND)
                        return SPELL_FAILED_NOT_TRADEABLE;
                }
                break;
            }
            case SPELL_EFFECT_ENCHANT_HELD_ITEM:
                // check item existence in effect code (not output errors at offhand hold item effect to main hand for example
                break;
            case SPELL_EFFECT_DISENCHANT:
            {
                Item const* item = m_targets.GetItemTarget();
                if (!item)
                    return SPELL_FAILED_CANT_BE_DISENCHANTED;

                // prevent disenchanting in trade slot
                if (item->GetOwnerGUID() != m_caster->GetGUID())
                    return SPELL_FAILED_CANT_BE_DISENCHANTED;

                ItemTemplate const* itemProto = item->GetTemplate();
                if (!itemProto)
                    return SPELL_FAILED_CANT_BE_DISENCHANTED;

                if ((itemProto->Flags2 & (uint32)ItemFlagsEX::DISENCHANT_TO_LOOT_TABLE) == 0)
                {
                    ItemDisenchantLootEntry const* itemDisenchantLoot = item->GetDisenchantLoot(m_caster->ToPlayer());
                    if (!itemDisenchantLoot)
                        return SPELL_FAILED_CANT_BE_DISENCHANTED;

                    uint32 item_quality = itemProto->Quality;
                    if (itemDisenchantLoot->RequiredDisenchantSkill > p_caster->GetSkillValue(SKILL_ENCHANTING))
                        return SPELL_FAILED_LOW_CASTLEVEL;
                }
                break;
            }
            case SPELL_EFFECT_PROSPECTING:
            {
                if (!m_targets.GetItemTarget())
                    return SPELL_FAILED_CANT_BE_PROSPECTED;

                ///< Ensure item is a prospectable ore
                if (!(m_targets.GetItemTarget()->GetTemplate()->Flags & int32(ItemFlags::IS_PROSPECTABLE)))
                    return SPELL_FAILED_CANT_BE_PROSPECTED;

                ///< Prevent prospecting in trade slot
                if (m_targets.GetItemTarget()->GetOwnerGUID() != m_caster->GetGUID())
                    return SPELL_FAILED_CANT_BE_PROSPECTED;

                ///< Check for enough skill in jewelcrafting
                uint32 item_prospectingskilllevel = m_targets.GetItemTarget()->GetTemplate()->RequiredSkillRank;
                if (item_prospectingskilllevel >p_caster->GetSkillValue(SKILL_JEWELCRAFTING))
                    return SPELL_FAILED_LOW_CASTLEVEL;

                ///< Make sure the player has the required ores in inventory
                if (m_targets.GetItemTarget()->GetCount() < 5)
                    return SPELL_FAILED_NEED_MORE_ITEMS;

                if (!LootTemplates_Prospecting.HaveLootFor(m_targets.GetItemTargetEntry()))
                    return SPELL_FAILED_CANT_BE_PROSPECTED;

                break;
            }
            case SPELL_EFFECT_MILLING:
            {
                if (!m_targets.GetItemTarget())
                    return SPELL_FAILED_CANT_BE_MILLED;

                ///< Ensure item is a millable herb
                if (!(m_targets.GetItemTarget()->GetTemplate()->Flags & int32(ItemFlags::IS_MILLABLE)))
                    return SPELL_FAILED_CANT_BE_MILLED;

                //prevent milling in trade slot
                if (m_targets.GetItemTarget()->GetOwnerGUID() != m_caster->GetGUID())
                    return SPELL_FAILED_CANT_BE_MILLED;

                ///< Check for enough skill in inscription
                uint32 item_millingskilllevel = m_targets.GetItemTarget()->GetTemplate()->RequiredSkillRank;
                if (item_millingskilllevel >p_caster->GetSkillValue(SKILL_INSCRIPTION))
                    return SPELL_FAILED_LOW_CASTLEVEL;

                ///< Make sure the player has the required herbs in inventory
                if (m_targets.GetItemTarget()->GetCount() < 5)
                    return SPELL_FAILED_NEED_MORE_ITEMS;

                if (!LootTemplates_Milling.HaveLootFor(m_targets.GetItemTargetEntry()))
                    return SPELL_FAILED_CANT_BE_MILLED;

                break;
            }
            case SPELL_EFFECT_WEAPON_DAMAGE:
            case SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL:
            {
                if (m_caster->GetTypeId() != TYPEID_PLAYER)
                    return SPELL_FAILED_TARGET_NOT_PLAYER;

                if (m_attackType != WeaponAttackType::RangedAttack)
                    break;

                Item* pItem = m_caster->ToPlayer()->GetWeaponForAttack(m_attackType);
                if (!pItem || pItem->CantBeUse())
                    return SPELL_FAILED_EQUIPPED_ITEM;

                switch (pItem->GetTemplate()->SubClass)
                {
                    case ITEM_SUBCLASS_WEAPON_THROWN:
                    {
                        uint32 ammo = pItem->GetEntry();
                        if (!m_caster->ToPlayer()->HasItemCount(ammo))
                            return SPELL_FAILED_NO_AMMO;
                    };
                    break;
                    case ITEM_SUBCLASS_WEAPON_GUN:
                    case ITEM_SUBCLASS_WEAPON_BOW:
                    case ITEM_SUBCLASS_WEAPON_CROSSBOW:
                    case ITEM_SUBCLASS_WEAPON_WAND:
                        break;
                    default:
                        break;
                }
                break;
            }
            case SPELL_EFFECT_CREATE_OR_RECHARGE_ITEM:
            {
                 uint32 item_id = m_spellInfo->Effects[i].ItemType;
                 ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(item_id);

                 if (!pProto)
                     return SPELL_FAILED_ITEM_AT_MAX_CHARGES;

                 if (Item* pitem = p_caster->GetItemByEntry(item_id))
                 {
                     for (int x = 0; x < MAX_ITEM_PROTO_SPELLS; ++x)
                         if (pProto->Spells[x].SpellCharges != 0 && pitem->GetSpellCharges(x) == pProto->Spells[x].SpellCharges)
                             return SPELL_FAILED_ITEM_AT_MAX_CHARGES;
                 }
                 break;
            }
            case SPELL_EFFECT_CHANGE_ITEM_BONUSES:
            {
                Item* l_ItemTarget = m_targets.GetItemTarget();
                if (l_ItemTarget == nullptr)
                    return SPELL_FAILED_NO_VALID_TARGETS;

                uint32 l_OldItemBonusTreeCategory = m_spellInfo->Effects[i].MiscValue;
                uint32 l_NewItemBonusTreeCategory = m_spellInfo->Effects[i].MiscValueB;

                std::vector<uint32> const& l_CurrentItemBonus = l_ItemTarget->GetAllItemBonuses();
                if (l_OldItemBonusTreeCategory == l_NewItemBonusTreeCategory)
                {
                    uint32 l_MaxIlevel = 0;
                    bool   l_Found = false;

                    auto& l_ItemStageUpgradeRules = sSpellMgr->GetSpellUpgradeItemStage(l_OldItemBonusTreeCategory);
                    if (!l_ItemStageUpgradeRules.empty())
                    {
                        for (auto l_Itr : l_ItemStageUpgradeRules)
                        {
                            if (!sSpellMgr->HaveSameItemSourceSkill(m_CastItem, l_ItemTarget))
                                continue;

                            if (l_Itr.ItemClass != l_ItemTarget->GetTemplate()->Class)
                                continue;

                            if (l_Itr.ItemSubclassMask != 0)
                            {
                                if ((l_Itr.ItemSubclassMask & (1 << l_ItemTarget->GetTemplate()->SubClass)) == 0)
                                    continue;
                            }

                            if (l_Itr.InventoryTypeMask != 0)
                            {
                                if ((l_Itr.InventoryTypeMask & (1 << l_ItemTarget->GetTemplate()->InventoryType)) == 0)
                                    continue;
                            }

                            if (m_castItemLevel > l_Itr.MaxIlevel)
                                continue;

                            if (m_castItemLevel == l_Itr.MaxIlevel)
                            {
                                std::vector<uint32> l_UpgradeSpellIDs = { 187546, 187537, 187541, 187539, 187538, 187551, 187550, 187552, 187535 };
                                if (std::find(l_UpgradeSpellIDs.begin(), l_UpgradeSpellIDs.end(), GetSpellInfo()->Id) != l_UpgradeSpellIDs.end())
                                    continue;
                            }

                            l_Found = true;
                            l_MaxIlevel = l_Itr.MaxIlevel;
                            break;
                        }

                        if (!l_Found)
                            return SPELL_FAILED_NO_VALID_TARGETS;

                        std::vector<uint32> l_UpgradeBonusStages;

                        switch (l_ItemTarget->GetTemplate()->ItemLevel)
                        {
                            case 630:
                                l_UpgradeBonusStages = { 525, 558, 559, 594, 619, 620 };
                                break;
                            case 640:
                                l_UpgradeBonusStages = { 525, 526, 527, 593, 617, 618 };
                            default:
                                break;
                        }

                        if (l_UpgradeBonusStages.empty())
                            return SPELL_FAILED_NO_VALID_TARGETS;

                        int32 l_CurrentIdx = -1;

                        for (int l_Idx = 0; l_Idx < (int)l_UpgradeBonusStages.size(); l_Idx++)
                        {
                            for (auto l_BonusId : l_CurrentItemBonus)
                            {
                                if (l_BonusId == l_UpgradeBonusStages[l_Idx])
                                {
                                    l_CurrentIdx = l_Idx;
                                    break;
                                }
                            }
                        }

                        if (l_CurrentIdx == -1 || l_CurrentIdx == l_UpgradeBonusStages.size() - 1)
                            return SPELL_FAILED_NO_VALID_TARGETS;
                    }
                }
                break;
            }
            default:
                break;
        }
    }

    // check weapon presence in slots for main/offhand weapons
    if (m_spellInfo->EquippedItemClass >=0)
    {
        // main hand weapon required
        if (AttributesCustomEx3 & SPELL_ATTR3_MAIN_HAND)
        {
            Item* item = m_caster->ToPlayer()->GetWeaponForAttack(WeaponAttackType::BaseAttack);

            // skip spell if no weapon in slot or broken
            if (!item || item->CantBeUse())
                return (_triggeredCastFlags & TRIGGERED_DONT_REPORT_CAST_ERROR) ? SPELL_FAILED_DONT_REPORT : SPELL_FAILED_EQUIPPED_ITEM_CLASS;

            // skip spell if weapon not fit to triggered spell
            if (!item->IsFitToSpellRequirements(m_spellInfo))
                return (_triggeredCastFlags & TRIGGERED_DONT_REPORT_CAST_ERROR) ? SPELL_FAILED_DONT_REPORT : SPELL_FAILED_EQUIPPED_ITEM_CLASS;
        }

        // offhand hand weapon required
        if (AttributesCustomEx3 & SPELL_ATTR3_REQ_OFFHAND)
        {
            Item* item = m_caster->ToPlayer()->GetWeaponForAttack(WeaponAttackType::OffAttack);

            // skip spell if no weapon in slot or broken
            if (!item || item->CantBeUse())
                return (_triggeredCastFlags & TRIGGERED_DONT_REPORT_CAST_ERROR) ? SPELL_FAILED_DONT_REPORT : SPELL_FAILED_EQUIPPED_ITEM_CLASS;

            // skip spell if weapon not fit to triggered spell
            if (!item->IsFitToSpellRequirements(m_spellInfo))
                return (_triggeredCastFlags & TRIGGERED_DONT_REPORT_CAST_ERROR) ? SPELL_FAILED_DONT_REPORT : SPELL_FAILED_EQUIPPED_ITEM_CLASS;
        }
    }

    return SPELL_CAST_OK;
}

SpellCastResult Spell::CheckVehicle(Unit const* p_Caster) const
{
    /// All creatures should be able to cast as passengers freely, restriction and attribute are only for players
    if (!p_Caster->IsPlayer())
        return SPELL_CAST_OK;

    Vehicle* l_Vehicle = p_Caster->GetVehicle();
    if (l_Vehicle)
    {
        uint16 l_CheckMask = 0;
        for (uint8 l_EffIndex = EFFECT_0; l_EffIndex < m_spellInfo->EffectCount; ++l_EffIndex)
        {
            if (m_spellInfo->Effects[l_EffIndex].ApplyAuraName == SPELL_AURA_MOD_SHAPESHIFT)
            {
                SpellShapeshiftFormEntry const* l_ShapeshiftEntry = sSpellShapeshiftFormStore.LookupEntry(m_spellInfo->Effects[l_EffIndex].MiscValue);
                if (l_ShapeshiftEntry && (l_ShapeshiftEntry->Flags & SHAPESHIFT_FORM_IS_NOT_A_SHAPESHIFT) == 0)
                    l_CheckMask |= VEHICLE_SEAT_FLAG_UNCONTROLLED;
                break;
            }
        }

        if (m_spellInfo->HasAura(SPELL_AURA_MOUNTED))
            l_CheckMask |= VEHICLE_SEAT_FLAG_CAN_CAST_MOUNT_SPELL;

        if (!l_CheckMask)
            l_CheckMask = VEHICLE_SEAT_FLAG_CAN_ATTACK;

        VehicleSeatEntry const* l_VehicleSeat = l_Vehicle->GetSeatForPassenger(p_Caster);
        if (!(AttributesCustomEx6 & SPELL_ATTR6_CASTABLE_WHILE_ON_VEHICLE) && !(AttributesCustom & SPELL_ATTR0_CASTABLE_WHILE_MOUNTED)
            && (l_VehicleSeat->m_flags & l_CheckMask) != l_CheckMask)
            return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

        /// Can only summon uncontrolled minions/guardians when on controlled vehicle
        if (l_VehicleSeat->m_flags & (VEHICLE_SEAT_FLAG_CAN_CONTROL | VEHICLE_SEAT_FLAG_UNK2))
        {
            for (uint8 l_EffIndex = EFFECT_0; l_EffIndex < m_spellInfo->EffectCount; ++l_EffIndex)
            {
                if (m_spellInfo->Effects[l_EffIndex].Effect != SPELL_EFFECT_SUMMON)
                    continue;

                SummonPropertiesEntry const* l_SummonProp = sSummonPropertiesStore.LookupEntry(m_spellInfo->Effects[l_EffIndex].MiscValueB);
                if (l_SummonProp && l_SummonProp->Category != SUMMON_CATEGORY_WILD)
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
            }
        }
    }

    return SPELL_CAST_OK;
}

/// Called only in Unit::DealDamage
void Spell::Delayed()
{
    /// Spell is active and can't be time-backed
    if (!m_caster)
        return;

    /// Spells may only be delayed twice
    if (isDelayableNoMore())
        return;

    /// Check pushback reduce
    /// Spellcasting delay is normally 150ms since WOD
    int32 l_DelayTime = 150;

    /// Must be initialized to 100 for percent modifiers
    int32 l_DelayReduce = 100;

    m_caster->ToPlayer()->ApplySpellMod(m_spellInfo->Id, SPELLMOD_NOT_LOSE_CASTING_TIME, l_DelayReduce, this);

    l_DelayReduce += m_caster->GetTotalAuraModifier(SPELL_AURA_REDUCE_PUSHBACK) - 100;

    if (l_DelayReduce >= 100)
        return;

    AddPct(l_DelayTime, -l_DelayReduce);

    if (m_timer + l_DelayTime > m_casttime)
    {
        l_DelayTime = m_casttime - m_timer;
        m_timer = m_casttime;
    }
    else
        m_timer += l_DelayTime;

    WorldPacket l_Data(SMSG_SPELL_DELAYED, 8 + 4);
    l_Data.appendPackGUID(m_caster->GetGUID());
    l_Data << uint32(l_DelayTime);

    m_caster->SendMessageToSet(&l_Data, true);
}

void Spell::DelayedChannel()
{
    if (!m_caster || m_caster->GetTypeId() != TYPEID_PLAYER || getState() != SPELL_STATE_CASTING)
        return;

    if (isDelayableNoMore())                                    // Spells may only be delayed twice
        return;

    //check pushback reduce
    // should be affected by modifiers, not take the db2 duration.
    int32 duration = ((m_channeledDuration > 0) ? m_channeledDuration : m_spellInfo->GetDuration());

    int32 delaytime = CalculatePct(duration, 25); // channeling delay is normally 25% of its time per hit
    int32 delayReduce = 100;                                    // must be initialized to 100 for percent modifiers
    m_caster->ToPlayer()->ApplySpellMod(m_spellInfo->Id, SPELLMOD_NOT_LOSE_CASTING_TIME, delayReduce, this);
    delayReduce += m_caster->GetTotalAuraModifier(SPELL_AURA_REDUCE_PUSHBACK) - 100;
    if (delayReduce >= 100)
        return;

    AddPct(delaytime, -delayReduce);

    if (m_timer <= delaytime)
    {
        delaytime = m_timer;
        m_timer = 0;
    }
    else
        m_timer -= delaytime;

    for (std::list<TargetInfo>::const_iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
        if ((*ihit).missCondition == SPELL_MISS_NONE)
            if (Unit* unit = (m_caster->GetGUID() == ihit->targetGUID) ? m_caster : ObjectAccessor::GetUnit(*m_caster, ihit->targetGUID))
                unit->DelayOwnedAuras(m_spellInfo->Id, m_originalCasterGUID, delaytime);

    // partially interrupt persistent area auras
    if (DynamicObject* dynObj = m_caster->GetDynObject(m_spellInfo->Id))
        dynObj->Delay(delaytime);

    SendChannelUpdate(m_timer);
}

void Spell::UpdatePointers()
{
    if (m_originalCasterGUID == m_caster->GetGUID())
        m_originalCaster = m_caster;
    else
    {
        m_originalCaster = ObjectAccessor::GetUnit(*m_caster, m_originalCasterGUID);
        if (m_originalCaster && !m_originalCaster->IsInWorld())
            m_originalCaster = NULL;
    }

    if (m_castItemGUID && m_caster->IsPlayer())
    {
        m_CastItem = m_caster->ToPlayer()->GetItemByGuid(m_castItemGUID);
        m_castItemLevel = -1;

        // cast item not found, somehow the item is no longer where we expected
        if (m_CastItem)
        {
            if (m_CastItem->GetOwnerGUID() == m_caster->GetGUID())
                m_castItemLevel = m_CastItem->GetItemLevel(m_caster->ToPlayer());
            else if (Player* owner = m_CastItem->GetOwner())
                m_castItemLevel = m_CastItem->GetItemLevel(owner);
        }
    }

    m_targets.Update(m_caster);

    // further actions done only for dest targets
    if (!m_targets.HasDst())
        return;

    // cache last transport
    WorldObject* transport = NULL;

    // update effect destinations (in case of moved transport dest target)
    for (uint8 effIndex = 0; effIndex < m_spellInfo->EffectCount; ++effIndex)
    {
        SpellDestination& dest = m_destTargets[effIndex];
        if (!dest._transportGUID)
            continue;

        if (!transport || transport->GetGUID() != dest._transportGUID)
            transport = ObjectAccessor::GetWorldObject(*m_caster, dest._transportGUID);

        if (transport)
        {
            dest._position.Relocate(transport);
            dest._position.RelocateOffset(dest._transportOffset);
        }
    }
}

CurrentSpellTypes Spell::GetCurrentContainer() const
{
    if (IsNextMeleeSwingSpell())
        return(CURRENT_MELEE_SPELL);
    else if (IsAutoRepeat())
        return(CURRENT_AUTOREPEAT_SPELL);
    else if (m_spellInfo->IsChanneled())
        return(CURRENT_CHANNELED_SPELL);
    else
        return(CURRENT_GENERIC_SPELL);
}

bool Spell::CheckEffectTarget(Unit const* target, uint32 eff, Position const* losPosition) const
{
    if (!m_spellInfo->Effects[eff].IsEffect())
        return false;

    switch (m_spellInfo->Effects[eff].ApplyAuraName)
    {
        case SPELL_AURA_MOD_POSSESS:
        case SPELL_AURA_MOD_CHARM:
        case SPELL_AURA_AOE_CHARM:
            if (target->GetTypeId() == TYPEID_UNIT && target->IsVehicle())
                return false;
            if (target->IsMounted())
                return false;
            if (target->GetCharmerGUID())
                return false;
            if (int32 damage = CalculateDamage(eff, target))
                if ((int32)target->getLevelForTarget(m_caster) > damage)
                    return false;
            break;
        default:
            break;
    }

    if (!m_spellInfo->IsNeedAdditionalLosChecks() && (IsTriggered() || AttributesCustomEx2 & SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS))
        return true;

    // Hour of Twilight
    if (m_spellInfo->Id == 103327)
    {
        // Deterrence
        if (target->HasAura(19263))
            return false;
    }

    // Prayer of Healing
    if (m_spellInfo->Id == 596)
        return true;

    // Hack fix for Ice Tombs (Sindragosa encounter)
    if (target->GetTypeId() == TYPEID_UNIT)
        if (target->GetEntry() == 36980 || target->GetEntry() == 38320 || target->GetEntry() == 38321 || target->GetEntry() == 38322)
            return true;

    ///< Arcane Barrage for additional targets
    if (m_spellInfo->Id == 44425 && m_targets.GetUnitTarget() != target)
        return true;

    // todo: shit below shouldn't be here, but it's temporary (21/04/2015: look "temporary for life")
    if (losPosition)
        return target->IsWithinLOS(losPosition->GetPositionX(), losPosition->GetPositionY(), losPosition->GetPositionZ());
    else
    {
        // Get GO cast coordinates if original caster -> GO
        WorldObject* caster = NULL;
        if (IS_GAMEOBJECT_GUID(m_originalCasterGUID))
            caster = m_caster->GetMap()->GetGameObject(m_originalCasterGUID);
        if (!caster)
            caster = m_caster;

        // Sin and Punishment from duel bug
        if (m_spellInfo->Id == 87204)
        {
            if (caster != target && caster->IsPlayer() && caster->ToPlayer()->m_Duel && target->IsPlayer())
                return false;
        }

        /// Balance Druids - Lunar Strike should always be able to energize caster
        /// Prevent energize issues on huge bosses that are spawned out of LOS but can still be targeted
        if (m_spellInfo->Id == 194153 && eff == SpellEffIndex::EFFECT_1)
            return true;

        if (target->GetEntry() == 5925)
            return true;
        if (LOSAdditionalRules(target))
            return true;

        if (m_targets.HasDst())
        {
            float x, y, z;
            m_targets.GetDstPos()->GetPosition(x, y, z);

            if (!target->IsWithinLOS(x, y, z))
            {
                if (Creature const* l_Cre = caster->ToCreature())
                {
                    if (l_Cre->IsAIEnabled && (l_Cre->AI()->CanTargetOutOfLOS() || l_Cre->AI()->CanTargetOutOfLOSXYZ(x, y, z)))
                        return true;
                    return false;
                }
            }
        }
        else if (target != m_caster)
        {
            if (Creature const* l_Cre = target->ToCreature())
            {
                if (l_Cre->IsAIEnabled && (l_Cre->AI()->CanBeTargetedOutOfLOS() || l_Cre->AI()->CanBeTargetedOutOfLOSXYZ(caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ())))
                    return true;
            }

            if (Creature const* l_Cre = caster->ToCreature())
            {
                if (l_Cre->IsAIEnabled && (l_Cre->AI()->CanTargetOutOfLOS() || l_Cre->AI()->CanTargetOutOfLOSXYZ(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ())))
                    return true;
            }

            if (!caster->IsWithinLOSInMap(target))
                return false;
        }
    }

    return true;
}

bool Spell::IsNextMeleeSwingSpell() const
{
    return AttributesCustom & SPELL_ATTR0_ON_NEXT_SWING;
}

bool Spell::IsAutoActionResetSpell() const
{
    // TODO: changed SPELL_INTERRUPT_FLAG_AUTOATTACK -> SPELL_INTERRUPT_FLAG_INTERRUPT to fix compile - is this check correct at all?
    if (IsTriggered() || !(m_spellInfo->InterruptFlags & SPELL_INTERRUPT_FLAG_INTERRUPT))
        return false;

    if (!m_casttime && m_spellInfo->HasAttribute(SPELL_ATTR6_NOT_RESET_SWING_IF_INSTANT))
        return false;

    return true;
}

bool Spell::IsNeedSendToClient() const
{
    uint16 l_VisualID = 0;

    if (SpellXSpellVisualEntry const* l_VisualEntry = sSpellXSpellVisualStore.LookupEntry(m_spellInfo->GetSpellXSpellVisualId(m_originalCaster)))
        l_VisualID = l_VisualEntry->VisualID;

    bool l_ForceSend = false;
    switch (m_spellInfo->Id)
    {
        case 212653:
            l_ForceSend = true;
            break;
        default:
            break;
    }

    return l_VisualID || m_spellInfo->IsChanneled() || m_spellInfo->HasEffect(SpellEffects::SPELL_EFFECT_LOOT_BONUS) || l_ForceSend ||
        (AttributesCustomEx8 & SPELL_ATTR8_AURA_SEND_AMOUNT) || m_spellInfo->Speed > 0.0f || (!m_triggeredByAuraSpell && (!IsTriggered() || m_FromClient));
}

bool Spell::HaveTargetsForEffect(uint8 effect) const
{
    for (std::list<TargetInfo>::const_iterator itr = m_UniqueTargetInfo.begin(); itr != m_UniqueTargetInfo.end(); ++itr)
        if (itr->effectMask & (1 << effect))
            return true;

    for (std::list<GOTargetInfo>::const_iterator itr = m_UniqueGOTargetInfo.begin(); itr != m_UniqueGOTargetInfo.end(); ++itr)
        if (itr->effectMask & (1 << effect))
            return true;

    for (std::list<ItemTargetInfo>::const_iterator itr = m_UniqueItemInfo.begin(); itr != m_UniqueItemInfo.end(); ++itr)
        if (itr->effectMask & (1 << effect))
            return true;

    return false;
}

SpellEvent::SpellEvent(Spell* spell) : BasicEvent()
{
    m_Spell = spell;
}

SpellEvent::~SpellEvent()
{
    if (m_Spell->getState() != SPELL_STATE_FINISHED)
        m_Spell->cancel();

    if (m_Spell->IsDeletable())
    {
        delete m_Spell;
    }
    else
    {
        sLog->outError(LOG_FILTER_SPELLS_AURAS, "~SpellEvent: %s %u tried to delete non-deletable spell %u. Was not deleted, causes memory leak.",
            (m_Spell->GetCaster()->IsPlayer() ? "Player" : "Creature"), m_Spell->GetCaster()->GetGUIDLow(), m_Spell->m_spellInfo->Id);
        ASSERT(false);
    }
}

bool SpellEvent::Execute(uint64 e_time, uint32 p_time)
{
    int32 l_MapID  = -1;
    int32 l_ZoneID = -1;

    bool l_ShouldLog = false;

    if (m_Spell->GetCaster())
    {
        l_MapID  = m_Spell->GetCaster()->GetMapId();
        l_ZoneID = m_Spell->GetCaster()->GetZoneId();
        l_ShouldLog = sMapMgr->m_LogMapPerformance || m_Spell->GetCaster()->GetMap()->IsRaid();
    }

    uint32 l_StartTime = l_ShouldLog ? getMSTime() : 0;

    // update spell if it is not finished
    if (m_Spell->getState() != SPELL_STATE_FINISHED)
        m_Spell->update(p_time);

    // check spell state to process
    switch (m_Spell->getState())
    {
        case SPELL_STATE_FINISHED:
        {
            // spell was finished, check deletable state
            if (m_Spell->IsDeletable())
            {
                if (l_ShouldLog)
                {
                    uint32 l_TimeTaken = getMSTime() - l_StartTime;
                    if (l_TimeTaken >= 5)
                        sLog->outAshran("SpellEvent::Execute %u mapid %i zoneid %i taked %u ms", m_Spell->GetSpellInfo()->Id, l_MapID, l_ZoneID, l_TimeTaken);
                }
                // check, if we do have unfinished triggered spells
                return true;                                // spell is deletable, finish event
            }
            // event will be re-added automatically at the end of routine)
        } break;

        case SPELL_STATE_DELAYED:
        {
            // first, check, if we have just started
            if (m_Spell->GetDelayStart() != 0)
            {
                {
                    // run the spell handler and think about what we can do next
                    uint64 t_offset = e_time - m_Spell->GetDelayStart();
                    uint64 n_offset = m_Spell->handle_delayed(t_offset);
                    if (n_offset)
                    {
                        if (l_ShouldLog)
                        {
                            uint32 l_TimeTaken = getMSTime() - l_StartTime;
                            if (l_TimeTaken >= 5)
                                sLog->outAshran("SpellEvent::Execute %u mapid %i zoneid %i taked %u ms", m_Spell->GetSpellInfo()->Id, l_MapID, l_ZoneID, l_TimeTaken);
                        }
                        // re-add us to the queue
                        m_Spell->GetCaster()->m_Events.AddEvent(this, m_Spell->GetDelayStart() + n_offset, false);
                        return false;                       // event not complete
                    }
                    // event complete
                    // finish update event will be re-added automatically at the end of routine)
                }
            }
            else
            {
                // delaying had just started, record the moment
                m_Spell->SetDelayStart(e_time);
                // handle effects on caster if the spell has travel time but also affects the caster in some way
                if (!m_Spell->m_targets.HasDst())
                    uint64 n_offset = m_Spell->handle_delayed(0);
                // re-plan the event for the delay moment
                m_Spell->GetCaster()->m_Events.AddEvent(this, e_time + m_Spell->GetDelayMoment(), false);
                if (l_ShouldLog)
                {
                    uint32 l_TimeTaken = getMSTime() - l_StartTime;
                    if (l_TimeTaken >= 5)
                        sLog->outAshran("SpellEvent::Execute %u mapid %u zoneid %i taked %u ms", m_Spell->GetSpellInfo()->Id, l_MapID, l_ZoneID, l_TimeTaken);
                }
                return false;                               // event not complete
            }
        } break;

        default:
        {
            // all other states
            // event will be re-added automatically at the end of routine)
        } break;
    }

    // spell processing not complete, plan event on the next update interval
    m_Spell->GetCaster()->m_Events.AddEvent(this, e_time + 1, false);
    if (l_ShouldLog)
    {
        uint32 l_TimeTaken = getMSTime() - l_StartTime;
        if (l_TimeTaken >= 5)
            sLog->outAshran("SpellEvent::Execute %u mapid %u ZoneID %i taked %u ms", m_Spell->GetSpellInfo()->Id, l_MapID, l_ZoneID, l_TimeTaken);
    }
    return false;                                           // event not complete
}

void SpellEvent::Abort(uint64 /*e_time*/)
{
    // oops, the spell we try to do is aborted
    if (m_Spell->getState() != SPELL_STATE_FINISHED)
        m_Spell->cancel();
}

bool SpellEvent::IsDeletable() const
{
    return m_Spell->IsDeletable();
}

bool Spell::IsValidDeadOrAliveTarget(Unit const* target) const
{
    if (target->isAlive())
        return !m_spellInfo->IsRequiringDeadTarget();
    if (m_spellInfo->IsAllowingDeadTarget())
        return true;
    return false;
}

void Spell::HandleLaunchPhase()
{
    // handle effects with SPELL_EFFECT_HANDLE_LAUNCH mode
    for (uint32 i = 0; i < m_spellInfo->EffectCount; ++i)
    {
        // don't do anything for empty effect
        if (!m_spellInfo->Effects[i].IsEffect())
            continue;

        if (m_spellMissMask & ((1 << SPELL_MISS_MISS) | (1 << SPELL_MISS_IMMUNE)))
        {
            if (m_spellInfo->Effects[i].TargetA.GetTarget() == TARGET_UNIT_CASTER)
                continue;
        }

        HandleEffects(nullptr, nullptr, nullptr, nullptr, i, SPELL_EFFECT_HANDLE_LAUNCH);
    }

    float multiplier[SpellEffIndex::MAX_EFFECTS];
    for (uint8 i = 0; i < m_spellInfo->EffectCount; ++i)
        if (m_applyMultiplierMask & (1 << i))
            multiplier[i] = m_spellInfo->Effects[i].CalcDamageMultiplier(m_originalCaster, this);

    bool usesAmmo = AttributesCustomCu & SPELL_ATTR0_CU_DIRECT_DAMAGE;

    for (std::list<TargetInfo>::iterator ihit= m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
    {
        TargetInfo& target = *ihit;

        uint32 mask = target.effectMask;
        if (!mask)
            continue;

        // do not consume ammo anymore for Hunter's volley spell
        if (IsTriggered() && m_spellInfo->SpellFamilyName == SPELLFAMILY_HUNTER && m_spellInfo->IsTargetingArea())
            usesAmmo = false;

        if (usesAmmo)
        {
            bool ammoTaken = false;
            for (uint8 i = 0; i < m_spellInfo->EffectCount; i++)
            {
                if (!(mask & 1<<i))
                    continue;

                switch (m_spellInfo->Effects[i].Effect)
                {
                    case SPELL_EFFECT_SCHOOL_DAMAGE:
                    case SPELL_EFFECT_WEAPON_DAMAGE:
                    case SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL:
                    case SPELL_EFFECT_NORMALIZED_WEAPON_DMG:
                    case SPELL_EFFECT_WEAPON_PERCENT_DAMAGE:
                    ammoTaken=true;
                    TakeAmmo();
                }
                if (ammoTaken)
                    break;
            }
        }
        DoAllEffectOnLaunchTarget(target, multiplier);
    }
}

void Spell::DoAllEffectOnLaunchTarget(TargetInfo& targetInfo, float* multiplier)
{
    Unit* unit = NULL;
    // In case spell hit target, do all effect on that target
    if (targetInfo.missCondition == SPELL_MISS_NONE || (targetInfo.missCondition == SPELL_MISS_IMMUNE && m_spellInfo && m_spellInfo->Id == 40602))  ///< Warrior should be able to charge even if the target is immune
        unit = m_caster->GetGUID() == targetInfo.targetGUID ? m_caster : ObjectAccessor::GetUnit(*m_caster, targetInfo.targetGUID);
    // In case spell reflect from target, do all effect on caster (if hit)
    else if (targetInfo.missCondition == SPELL_MISS_REFLECT && targetInfo.reflectResult == SPELL_MISS_NONE)
    {
        unit = m_caster;
    }
    if (!unit)
        return;

    for (uint32 i = 0; i < m_spellInfo->EffectCount; ++i)
    {
        if (targetInfo.effectMask & (1<<i))
        {
            m_damage = 0;
            m_healing = 0;

            if (m_ShoudLogDamage)
            {
                if (!targetInfo.DamageLog)
                    targetInfo.DamageLog = new std::ostringstream();

                m_CurrentLogDamageStream = targetInfo.DamageLog;
                *m_CurrentLogDamageStream << "EFFECT " << i << std::endl;
            }

            HandleEffects(unit, nullptr, nullptr, nullptr, i, SPELL_EFFECT_HANDLE_LAUNCH_TARGET);

            if (m_damage > 0)
            {
                if (m_spellInfo->Effects[i].IsTargetingArea())
                {
                    // Hack Fix Frost Bomb, Beast Cleave : Doesn't add AoE damage to main target
                    if (m_spellInfo->Id == 113092 || m_spellInfo->Id == 118459)
                    {
                        if (targetInfo.targetGUID == (*m_UniqueTargetInfo.begin()).targetGUID)
                            m_damage = 0;
                    }

                    if (m_caster->IsPlayer())
                    {
                        uint32 targetAmount = m_UniqueTargetInfo.size();
                        if (targetAmount > 20)
                            m_damage = m_damage * 20 / targetAmount;
                    }
                }
            }
            else if (m_damage < 0)
            {
                switch (m_spellInfo->Id)
                {
                    case 73921:  ///< Healing Rain
                    case 215871: ///< Rainfall
                    {
                        if (m_caster->IsPlayer())
                        {
                            uint32 targetAmount = m_UniqueTargetInfo.size();
                            if (targetAmount > 6)
                                m_damage = (m_damage * 6) / int32(targetAmount);
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            if (m_applyMultiplierMask & (1 << i))
            {
                if (m_ShoudLogDamage && m_damageMultipliers[i] != 1.0f)
                    *m_CurrentLogDamageStream << " m_damageMultipliers " << m_damageMultipliers[i] << " m_damage " << m_damage << std::endl;

                m_damage = int32(m_damage * m_damageMultipliers[i]);
                m_damageMultipliers[i] *= multiplier[i];
            }
            targetInfo.damage += m_damage;
        }
    }

    if (m_CustomCritChance < 0.0f)
        targetInfo.crit = m_caster->IsSpellCrit(unit, m_spellInfo, m_spellSchoolMask, m_attackType, DamageEffectType::SPELL_DIRECT_DAMAGE);
    else
        targetInfo.crit = roll_chance_f(m_CustomCritChance);

    m_CurrentLogDamageStream = nullptr;
}

SpellCastResult Spell::CanOpenLock(uint32 effIndex, uint32 lockId, SkillType& skillId, int32& reqSkillValue, int32& skillValue)
{
    if (!lockId)    ///< possible case for GO and maybe for items.
        return SPELL_CAST_OK;

    /// Get LockInfo
    LockEntry const* lockInfo = sLockStore.LookupEntry(lockId);

    if (!lockInfo)
        return SPELL_FAILED_BAD_TARGETS;

    bool reqKey = false;    ///< some locks not have reqs

    for (int j = 0; j < MAX_LOCK_CASE; ++j)
    {
        switch (lockInfo->Type[j])
        {
            /// check key item (many fit cases can be)
            case LOCK_KEY_ITEM:
                if (lockInfo->Index[j] && m_CastItem && m_CastItem->GetEntry() == lockInfo->Index[j])
                    return SPELL_CAST_OK;
                reqKey = true;
                break;

            /// check key skill (only single first fit case can be)
            case LOCK_KEY_SKILL:
            {
                reqKey = true;

                /// wrong locktype, skip
                if (uint32(m_spellInfo->Effects[effIndex].MiscValue) != lockInfo->Index[j])
                    continue;

                skillId = SkillByLockType(LockType(lockInfo->Index[j]));

                if (skillId != SKILL_NONE || lockInfo->Index[j] == LOCKTYPE_PICKLOCK)
                {
                    reqSkillValue = lockInfo->Skill[j];

                    /// castitem check: rogue using skeleton keys. the skill values should not be added in this case.
                    skillValue = 0;
                    if (!m_CastItem && m_caster->GetTypeId() == TYPEID_PLAYER)
                        skillValue = m_caster->ToPlayer()->GetSkillValue(skillId);
                    else if (lockInfo->Index[j] == LOCKTYPE_PICKLOCK)
                        skillValue = m_caster->getLevel() * 5;

                    /// skill bonus provided by casting spell (mostly item spells)
                    /// add the effect base points modifier from the spell casted (cheat lock / skeleton key etc.)
                    if (m_spellInfo->Effects[effIndex].TargetA.GetTarget() == TARGET_GAMEOBJECT_ITEM_TARGET || m_spellInfo->Effects[effIndex].TargetB.GetTarget() == TARGET_GAMEOBJECT_ITEM_TARGET)
                        skillValue += m_spellInfo->Effects[effIndex].CalcValue(m_caster, 0);

                    if (skillValue < reqSkillValue)
                        return SPELL_FAILED_LOW_CASTLEVEL;
                }

                return SPELL_CAST_OK;
            }
        }
    }

    if (reqKey)
        return SPELL_FAILED_BAD_TARGETS;

    return SPELL_CAST_OK;
}

void Spell::SetSpellValue(SpellValueMod mod, int32 value)
{
    switch (mod)
    {
        case SPELLVALUE_BASE_POINT0:
            m_spellValue->EffectBasePoints[0] = m_spellInfo->Effects[EFFECT_0].CalcBaseValue(value);
            break;
        case SPELLVALUE_BASE_POINT1:
            m_spellValue->EffectBasePoints[1] = m_spellInfo->Effects[EFFECT_1].CalcBaseValue(value);
            break;
        case SPELLVALUE_BASE_POINT2:
            m_spellValue->EffectBasePoints[2] = m_spellInfo->Effects[EFFECT_2].CalcBaseValue(value);
            break;
        case SPELLVALUE_BASE_POINT3:
            m_spellValue->EffectBasePoints[3] = m_spellInfo->Effects[EFFECT_3].CalcBaseValue(value);
            break;
        case SPELLVALUE_BASE_POINT4:
            m_spellValue->EffectBasePoints[4] = m_spellInfo->Effects[EFFECT_4].CalcBaseValue(value);
            break;
        case SPELLVALUE_BASE_POINT5:
            m_spellValue->EffectBasePoints[5] = m_spellInfo->Effects[EFFECT_5].CalcBaseValue(value);
            break;
        case SPELLVALUE_RADIUS_MOD:
            m_spellValue->RadiusMod = (float)value / 10000;
            break;
        case SPELLVALUE_MAX_TARGETS:
            m_spellValue->MaxAffectedTargets = (uint32)value;
            break;
        case SPELLVALUE_AURA_STACK:
            m_spellValue->AuraStackAmount = uint8(value);
            break;
        case SPELLVALUE_IGNORE_CD:
            m_spellValue->IgnoreCooldowns = (bool)value;
            break;
    }
}

int32 Spell::GetSpellValue(SpellValueMod p_Mod) const
{
    switch (p_Mod)
    {
        case SPELLVALUE_BASE_POINT0:
            return m_spellValue->EffectBasePoints[0];
        case SPELLVALUE_BASE_POINT1:
            return m_spellValue->EffectBasePoints[1];
        case SPELLVALUE_BASE_POINT2:
            return m_spellValue->EffectBasePoints[2];
        case SPELLVALUE_BASE_POINT3:
            return m_spellValue->EffectBasePoints[3];
        case SPELLVALUE_BASE_POINT4:
            return m_spellValue->EffectBasePoints[4];
        case SPELLVALUE_BASE_POINT5:
            return m_spellValue->EffectBasePoints[5];
        case SPELLVALUE_RADIUS_MOD:
            return m_spellValue->RadiusMod;
        case SPELLVALUE_MAX_TARGETS:
            return m_spellValue->MaxAffectedTargets;
        case SPELLVALUE_AURA_STACK:
            return m_spellValue->AuraStackAmount;
        case SPELLVALUE_IGNORE_CD:
            return m_spellValue->IgnoreCooldowns;
        default:
            return 0;
    }
}

void Spell::PrepareTargetProcessing()
{
    CheckEffectExecuteData();
}

void Spell::FinishTargetProcessing()
{
    SendLogExecute();
}

void Spell::InitEffectExecuteData(uint8 /*effIndex*/)
{
}

void Spell::CheckEffectExecuteData()
{
}

void Spell::LoadScripts()
{
    sScriptMgr->CreateSpellScripts(m_spellInfo->Id, m_loadedScripts);
    for (std::list<SpellScript*>::iterator itr = m_loadedScripts.begin(); itr != m_loadedScripts.end();)
    {
        if (!(*itr)->_Load(this))
        {
            std::list<SpellScript*>::iterator bitr = itr;
            ++itr;
            delete (*bitr);
            m_loadedScripts.erase(bitr);
            continue;
        }
        sLog->outDebug(LOG_FILTER_SPELLS_AURAS, "Spell::LoadScripts: Script `%s` for spell `%u` is loaded now", (*itr)->_GetScriptName()->c_str(), m_spellInfo->Id);
        (*itr)->Register();
        ++itr;
    }
}

void Spell::CallScriptBeforeCastHandlers()
{
    for (std::list<SpellScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(SPELL_SCRIPT_HOOK_BEFORE_CAST);
        std::list<SpellScript::CastHandler>::iterator hookItrEnd = (*scritr)->BeforeCast.end(), hookItr = (*scritr)->BeforeCast.begin();
        for (; hookItr != hookItrEnd; ++hookItr)
            (*hookItr).Call(*scritr);

        (*scritr)->_FinishScriptCall();
    }

    CallDBScriptBeforeCast(m_spellInfo->Id);
}

void Spell::CallScriptOnCastHandlers()
{
    for (std::list<SpellScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(SPELL_SCRIPT_HOOK_ON_CAST);
        std::list<SpellScript::CastHandler>::iterator hookItrEnd = (*scritr)->OnCast.end(), hookItr = (*scritr)->OnCast.begin();
        for (; hookItr != hookItrEnd; ++hookItr)
            (*hookItr).Call(*scritr);

        (*scritr)->_FinishScriptCall();
    }

    CallDBScriptOnCast(m_spellInfo->Id);
}

void Spell::CallScriptAfterCastHandlers()
{
    for (std::list<SpellScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(SPELL_SCRIPT_HOOK_AFTER_CAST);
        std::list<SpellScript::CastHandler>::iterator hookItrEnd = (*scritr)->AfterCast.end(), hookItr = (*scritr)->AfterCast.begin();
        for (; hookItr != hookItrEnd; ++hookItr)
            (*hookItr).Call(*scritr);

        (*scritr)->_FinishScriptCall();
    }

    CallDBScriptAfterCast(m_spellInfo->Id);
}

bool Spell::CallScriptCheckInterruptHandlers()
{
    bool l_CanInterrupt = false;

    for (std::list<SpellScript*>::iterator l_Scritr = m_loadedScripts.begin(); l_Scritr != m_loadedScripts.end(); ++l_Scritr)
    {
        (*l_Scritr)->_PrepareScriptCall(SPELL_SCRIPT_HOOK_CHECK_INTERRUPT);
        std::list<SpellScript::CheckInterruptHandler>::iterator l_HookItrEnd = (*l_Scritr)->OnCheckInterrupt.end(), l_HookItr = (*l_Scritr)->OnCheckInterrupt.begin();
        for (; l_HookItr != l_HookItrEnd; ++l_HookItr)
        {
            bool l_TempResult = (*l_HookItr).Call(*l_Scritr);
            if (l_TempResult == true)
                l_CanInterrupt = l_TempResult;
        }

        (*l_Scritr)->_FinishScriptCall();
    }

    return l_CanInterrupt;
}

void Spell::CallScriptOnPrepareHandlers()
{
    for (std::list<SpellScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(SPELL_SCRIPT_HOOK_ON_PREPARE);

        std::list<SpellScript::OnPrepareHandler>::iterator hookItrEnd = (*scritr)->OnPrepare.end(), hookItr = (*scritr)->OnPrepare.begin();
        for (; hookItr != hookItrEnd; ++hookItr)
            (*hookItr).Call(*scritr);

        (*scritr)->_FinishScriptCall();
    }

    CallDBScriptOnPrepare(m_spellInfo->Id);
}


SpellCastResult Spell::CallScriptCheckCastHandlers()
{
    SpellCastResult retVal = SPELL_CAST_OK;
    for (std::list<SpellScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(SPELL_SCRIPT_HOOK_CHECK_CAST);
        std::list<SpellScript::CheckCastHandler>::iterator hookItrEnd = (*scritr)->OnCheckCast.end(), hookItr = (*scritr)->OnCheckCast.begin();
        for (; hookItr != hookItrEnd; ++hookItr)
        {
            SpellCastResult tempResult = (*hookItr).Call(*scritr);
            if (retVal == SPELL_CAST_OK)
                retVal = tempResult;
        }

        (*scritr)->_FinishScriptCall();
    }

    if (retVal == SPELL_CAST_OK)
        retVal = CustomCheckCast();

    if (retVal == SPELL_CAST_OK)
        retVal = CallDBScriptCheckCast(m_spellInfo->Id);

    return retVal;
}

SpellCastResult Spell::CustomCheckCast()
{
    SpellCastResult retVal = SPELL_CAST_OK;

    if (std::vector<SpellCheckCast> const* checkCast = sSpellMgr->GetSpellCheckCast(m_spellInfo->Id))
    {
        bool check = false;
        for (std::vector<SpellCheckCast>::const_iterator itr = checkCast->begin(); itr != checkCast->end(); ++itr)
        {
            Unit* _caster = m_originalCaster ? m_originalCaster : m_caster;
            Unit* _target = m_targets.GetUnitTarget();

            if (itr->Target)
                _target = (m_originalCaster ? m_originalCaster : m_caster)->GetUnitForLinkedSpell(_caster, _target, itr->Target);

            if (itr->Caster)
                _caster = (m_originalCaster ? m_originalCaster : m_caster)->GetUnitForLinkedSpell(_caster, _target, itr->Caster);

            if (!_caster)
                check = true;

            if (!_target)
                _target = _caster;

            if (itr->DataType)
                if (m_caster->HasAuraLinkedSpell(_caster, _target, itr->CheckType, itr->DataType, itr->Param2))
                    check = true;

            if (itr->DataType2)
                if (m_caster->HasAuraLinkedSpell(_caster, _target, itr->CheckType2, itr->DataType2, itr->Param3))
                    check = true;

            switch (itr->Type)
            {
                case SPELL_CHECK_CAST_DEFAULT: // 0
                    if (itr->Param1 < 0 && !check)
                        check = true;
                    else if (itr->Param1 < 0 && check)
                        check = false;
                    break;
                case SPELL_CHECK_CAST_HEALTH: // 1
                {
                    if (check)
                        break;
                    if (itr->Param1 > 0 && _target->GetHealthPct() < itr->Param1)
                        check = true;
                    else if (itr->Param1 < 0 && _target->GetHealthPct() >= abs(itr->Param1))
                        check = true;
                    break;
                }
                case SPELL_CHECK_CAST_AURA:
                {
                    if (itr->Param1 < 0 && m_caster->HasAura(std::abs(itr->Param1)))
                        check = true;
                    else if (itr->Param1 > 0 && !m_caster->HasAura(itr->Param1))
                        check = true;

                    break;
                }
                case SPELL_CHECK_CAST_HAS_SPELL:
                {
                    if (itr->Param1 < 0 && m_caster->HasSpell(std::abs(itr->Param1)))
                        check = true;
                    else if (itr->Param1 > 0 && !m_caster->HasSpell(itr->Param1))
                        check = true;

                    break;
                }
                default:
                    break;
            }

            if (check)
            {
                if (itr->CustomErrorID)
                {
                    retVal = SPELL_FAILED_CUSTOM_ERROR;
                    m_customError = (SpellCustomErrors)itr->CustomErrorID;
                }
                else
                    retVal = (SpellCastResult)itr->ErrorID;
                break;
            }
        }
    }

    return retVal;
}

void Spell::CallScriptTakePowersHandlers(Powers p_PowerType, int32& p_PowerCost)
{
    uint32 scriptExecuteTime = getMSTime();
    for (std::list<SpellScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(SPELL_SCRIPT_HOOK_TAKE_POWERS);
        std::list<SpellScript::TakePowersHandler>::iterator hookItrEnd = (*scritr)->OnTakePowers.end(), hookItr = (*scritr)->OnTakePowers.begin();
        for (; hookItr != hookItrEnd; ++hookItr)
            (*hookItr).Call(*scritr, p_PowerType, p_PowerCost);

        (*scritr)->_FinishScriptCall();
    }

    CallDBScriptTakePowers(m_spellInfo->Id, p_PowerType, p_PowerCost);
}

void Spell::CallScriptCalculateThreatHandlers(Unit* p_Target, float& p_Threat)
{
    uint32 scriptExecuteTime = getMSTime();
    for (std::list<SpellScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(SPELL_SCRIPT_HOOK_CALCULATE_THREAT);
        std::list<SpellScript::CalculateThreatHandler>::iterator hookItrEnd = (*scritr)->OnCalculateThreat.end(), hookItr = (*scritr)->OnCalculateThreat.begin();
        for (; hookItr != hookItrEnd; ++hookItr)
            (*hookItr).Call(*scritr, p_Target, p_Threat);

        (*scritr)->_FinishScriptCall();
    }
}

void Spell::CallScriptInterruptHandlers(uint32 p_Time)
{
    uint32 scriptExecuteTime = getMSTime();
    for (std::list<SpellScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(SPELL_SCRIPT_HOOK_INTERRUPT);
        std::list < SpellScript::InterruptHandler>::iterator hookItrEnd = (*scritr)->OnInterrupt.end(), hookItr = (*scritr)->OnInterrupt.begin();
        for (; hookItr != hookItrEnd; ++hookItr)
            (*hookItr).Call(*scritr, p_Time);

        (*scritr)->_FinishScriptCall();
    }
}

void Spell::PrepareScriptHitHandlers()
{
    for (std::list<SpellScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
        (*scritr)->_InitHit();
}

bool Spell::CallScriptEffectHandlers(SpellEffIndex effIndex, SpellEffectHandleMode mode)
{
    // execute script effect handler hooks and check if effects was prevented
    bool preventDefault = false;
    for (std::list<SpellScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        std::list<SpellScript::EffectHandler>::iterator effItr, effEndItr;
        SpellScriptHookType hookType;
        switch (mode)
        {
            case SPELL_EFFECT_HANDLE_LAUNCH:
                effItr = (*scritr)->OnEffectLaunch.begin();
                effEndItr = (*scritr)->OnEffectLaunch.end();
                hookType = SPELL_SCRIPT_HOOK_EFFECT_LAUNCH;
                break;
            case SPELL_EFFECT_HANDLE_LAUNCH_TARGET:
                effItr = (*scritr)->OnEffectLaunchTarget.begin();
                effEndItr = (*scritr)->OnEffectLaunchTarget.end();
                hookType = SPELL_SCRIPT_HOOK_EFFECT_LAUNCH_TARGET;
                break;
            case SPELL_EFFECT_HANDLE_HIT:
                effItr = (*scritr)->OnEffectHit.begin();
                effEndItr = (*scritr)->OnEffectHit.end();
                hookType = SPELL_SCRIPT_HOOK_EFFECT_HIT;
                break;
            case SPELL_EFFECT_HANDLE_HIT_TARGET:
                effItr = (*scritr)->OnEffectHitTarget.begin();
                effEndItr = (*scritr)->OnEffectHitTarget.end();
                hookType = SPELL_SCRIPT_HOOK_EFFECT_HIT_TARGET;
                break;
            default:
                ASSERT(false);
                return false;
        }
        (*scritr)->_PrepareScriptCall(hookType);
        for (; effItr != effEndItr; ++effItr)
            // effect execution can be prevented
            if (!(*scritr)->_IsEffectPrevented(effIndex) && (*effItr).IsEffectAffected(m_spellInfo, effIndex))
                (*effItr).Call(*scritr, effIndex);

        if (!preventDefault)
            preventDefault = (*scritr)->_IsDefaultEffectPrevented(effIndex);

        (*scritr)->_FinishScriptCall();
    }

    switch (mode)
    {
        case SPELL_EFFECT_HANDLE_HIT:
            CallDBScriptOnEffectHit(m_spellInfo->Id, effIndex);
            break;
        case SPELL_EFFECT_HANDLE_HIT_TARGET:
            CallDBScriptOnEffectHitTarget(m_spellInfo->Id, effIndex);
            break;
        case SPELL_EFFECT_HANDLE_LAUNCH:
        case SPELL_EFFECT_HANDLE_LAUNCH_TARGET:
        default:
            break;
    }

    return preventDefault;
}

void Spell::CallScriptBeforeHitHandlers()
{
    for (std::list<SpellScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(SPELL_SCRIPT_HOOK_BEFORE_HIT);
        std::list<SpellScript::HitHandler>::iterator hookItrEnd = (*scritr)->BeforeHit.end(), hookItr = (*scritr)->BeforeHit.begin();
        for (; hookItr != hookItrEnd; ++hookItr)
            (*hookItr).Call(*scritr);

        (*scritr)->_FinishScriptCall();
    }

    CallDBScriptBeforeHit(m_spellInfo->Id);
}

void Spell::CallScriptOnHitHandlers()
{
    if (m_missInfo != SPELL_MISS_NONE)
        return;

    for (std::list<SpellScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(SPELL_SCRIPT_HOOK_HIT);
        std::list<SpellScript::HitHandler>::iterator hookItrEnd = (*scritr)->OnHit.end(), hookItr = (*scritr)->OnHit.begin();
        for (; hookItr != hookItrEnd; ++hookItr)
            (*hookItr).Call(*scritr);

        (*scritr)->_FinishScriptCall();
    }

    CallDBScriptOnHit(m_spellInfo->Id);
}

void Spell::CallScriptAfterHitHandlers()
{
    for (std::list<SpellScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(SPELL_SCRIPT_HOOK_AFTER_HIT);
        std::list<SpellScript::HitHandler>::iterator hookItrEnd = (*scritr)->AfterHit.end(), hookItr = (*scritr)->AfterHit.begin();
        for (; hookItr != hookItrEnd; ++hookItr)
            (*hookItr).Call(*scritr);

        (*scritr)->_FinishScriptCall();
    }

    CallDBScriptAfterHit(m_spellInfo->Id);
}

void Spell::CallScriptObjectAreaTargetSelectHandlers(std::list<WorldObject*>& targets, SpellEffIndex effIndex, Targets targetId)
{
    for (std::list<SpellScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(SPELL_SCRIPT_HOOK_OBJECT_AREA_TARGET_SELECT);
        std::list<SpellScript::ObjectAreaTargetSelectHandler>::iterator hookItrEnd = (*scritr)->OnObjectAreaTargetSelect.end(), hookItr = (*scritr)->OnObjectAreaTargetSelect.begin();
        for (; hookItr != hookItrEnd; ++hookItr)
            if ((*hookItr).IsEffectAffected(m_spellInfo, effIndex))
                (*hookItr).Call(*scritr, targets);

        (*scritr)->_FinishScriptCall();
    }

    CustomTargetSelector(targets, effIndex, targetId);
}

void Spell::CustomTargetSelector(std::list<WorldObject*>& targets, SpellEffIndex effIndex, Targets targetId)
{
    if (std::vector<SpellTargetFilter> const* spellTargetFilter = sSpellMgr->GetSpellTargetFilter(m_spellInfo->Id))
    {
        uint32 targetCount = 0;
        uint32 resizeType = 0;
        Unit* _caster = m_originalCaster ? m_originalCaster : m_caster;
        Unit* _owner = _caster->GetAnyOwner();
        if (_caster->isTotem())
            if (Unit* owner = _caster->GetOwner())
                _caster = owner;

        

        Unit* _target = m_targets.GetUnitTarget();
        if (!_target && m_caster->ToPlayer())
            _target = m_caster->ToPlayer()->GetSelectedUnit();
        else if (!_target)
            _target = m_caster->getVictim();

        for (std::vector<SpellTargetFilter>::const_iterator itr = spellTargetFilter->begin(); itr != spellTargetFilter->end(); ++itr)
        {
            if (!(itr->EffectMask & (1<<effIndex)))
                continue;

            if (itr->TargetID != targetId)
                continue;

            if (targets.empty())
            {
                if (itr->AddCaster == 2)
                    targets.push_back(GetCaster());
                return;
            }

            if (itr->Count && !targetCount && !itr->MaxCount)
                targetCount = itr->Count;
            else if (itr->MaxCount && !targetCount)
            {
                if (m_caster->GetMap()->Is25ManRaid())
                    targetCount = itr->MaxCount;
                else
                    targetCount = itr->Count;
            }

            if (itr->ResizeType && !resizeType)
                resizeType = itr->ResizeType;

            /// Must be done here, because count depends on the raid size, before filter then
            if (resizeType == eFilterResizeType::FilterResizeTypeRaidSizePct)
                targetCount = std::max<uint32>(CalculatePct(uint32(targets.size()), targetCount), 1);

            if (itr->Aura > 0 && _caster->HasAura(itr->Aura))
                targetCount += itr->AddCount;
            if (itr->Aura < 0 && !_caster->HasAura(abs(itr->Aura)))
                targetCount -= itr->AddCount;

            if (itr->AddCaster < 0)
                targets.remove(GetCaster());
            if (itr->AddCaster == 2)
                targets.remove(GetCaster());

            switch (itr->Options)
            {
                case SPELL_FILTER_SORT_BY_HEALT: //0
                {
                    if (itr->Param1 < 0.0f)
                        targets.sort(JadeCore::UnitHealthState(false));
                    else
                        targets.sort(JadeCore::UnitHealthState(true));
                    break;
                }
                case SPELL_FILTER_BY_AURA: //1
                {
                    targets.remove_if(JadeCore::UnitAuraAndCheck(itr->Param1, itr->Param2, itr->Param3));
                    break;
                }
                case SPELL_FILTER_BY_DISTANCE: //2
                {
                    if (itr->Param1 < 0.0f)
                        targets.remove_if(JadeCore::UnitDistanceCheck(false, _caster, itr->Param2));
                    else
                        targets.remove_if(JadeCore::UnitDistanceCheck(true, _caster, itr->Param2));
                    break;
                }
                case SPELL_FILTER_TARGET_TYPE: //3
                {
                    if (itr->Param1 < 0.0f)
                        targets.remove_if(JadeCore::UnitTypeCheck(true, uint32(itr->Param2)));
                    else
                        targets.remove_if(JadeCore::UnitTypeCheck(false, uint32(itr->Param2)));
                    break;
                }
                case SPELL_FILTER_SORT_BY_DISTANCE: //4
                {
                    if (itr->Param1 < 0.0f)
                        targets.sort(JadeCore::UnitSortDistance(false, _caster));
                    else
                        targets.sort(JadeCore::UnitSortDistance(true, _caster));
                    break;
                }
                case SPELL_FILTER_TARGET_FRIENDLY: //5
                {
                    if (itr->Param1 < 0.0f)
                        targets.remove_if(JadeCore::UnitFriendlyCheck(true, _caster));
                    else
                        targets.remove_if(JadeCore::UnitFriendlyCheck(false, _caster));
                    break;
                }
                case SPELL_FILTER_TARGET_IN_RAID: //6
                {
                    if (itr->Param1 < 0.0f)
                        targets.remove_if(JadeCore::UnitRaidCheck(true, _caster));
                    else
                        targets.remove_if(JadeCore::UnitRaidCheck(false, _caster));
                    break;
                }
                case SPELL_FILTER_TARGET_IN_PARTY: //7
                {
                    if (itr->Param1 < 0.0f)
                        targets.remove_if(JadeCore::UnitPartyCheck(true, _caster));
                    else
                        targets.remove_if(JadeCore::UnitPartyCheck(false, _caster));
                    break;
                }
                case SPELL_FILTER_TARGET_EXPL_TARGET: //8
                {
                    targets.clear();
                    if (_target)
                        targets.push_back(_target);
                    break;
                }
                case SPELL_FILTER_TARGET_EXPL_TARGET_REMOVE: //9
                {
                    if (targets.size() > itr->Param1)
                        if (_target)
                            targets.remove(_target);
                    break;
                }
                case SPELL_FILTER_TARGET_IN_LOS: //10
                {
                    if (itr->Param1 < 0.0f)
                        targets.remove_if(JadeCore::UnitCheckInLos(true, _caster));
                    else
                        targets.remove_if(JadeCore::UnitCheckInLos(false, _caster));
                    break;
                }
                case SPELL_FILTER_TARGET_IS_IN_BETWEEN: //11
                {
                    if (_target)
                        targets.remove_if(JadeCore::UnitCheckInBetween(false, _caster, _target, itr->Param1));
                    break;
                }
                case SPELL_FILTER_TARGET_IS_IN_BETWEEN_SHIFT: //12
                {
                    if (_target)
                        targets.remove_if(JadeCore::UnitCheckInBetweenShift(false, _caster, _target, itr->Param1, itr->Param2, itr->Param3));
                    break;
                }
                case SPELL_FILTER_BY_AURA_OR: //13
                {
                    targets.remove_if(JadeCore::UnitAuraOrCheck(itr->Param1, itr->Param2, itr->Param3));
                    break;
                }
                case SPELL_FILTER_BY_ENTRY: //14
                {
                    targets.remove_if(JadeCore::UnitEntryCheck(itr->Param1, itr->Param2, itr->Param3));
                    break;
                }
                case SPELL_FILTER_TARGET_ATTACKABLE: // 15
                {
                    if (itr->Param1 < 0.0f)
                        targets.remove_if(JadeCore::UnitAttackableCheck(true, _caster));
                    else
                        targets.remove_if(JadeCore::UnitAttackableCheck(false, _caster));
                    break;
                }
                case SPELL_FILTER_BY_DISTANCE_TARGET: //16
                {
                    if (_target)
                    {
                        if (itr->Param1 < 0.0f)
                            targets.remove_if(JadeCore::UnitDistanceCheck(false, _target, itr->Param2));
                        else
                            targets.remove_if(JadeCore::UnitDistanceCheck(true, _target, itr->Param2));
                    }
                    break;
                }
                case SPELL_FILTER_BY_PLAYER_ROLE:   ///< 17
                {
                    targets.remove_if([&](WorldObject* p_Object) -> bool
                    {
                        if (p_Object == nullptr || !p_Object->IsPlayer())
                            return true;

                        /// Must not have specified role
                        if (itr->Param1 < 0.0f)
                        {
                            if (p_Object->ToPlayer()->GetRoleForGroup() == itr->Param2)
                                return true;
                        }
                        /// Must have specified role
                        else
                        {
                            if (p_Object->ToPlayer()->GetRoleForGroup() != itr->Param2)
                                return true;
                        }

                        return false;
                    });

                    break;
                }
                case SPELL_FILTER_BY_PLAYER_TYPE:   ///< 18
                {
                    targets.remove_if([&](WorldObject* p_Object) -> bool
                    {
                        if (p_Object == nullptr || !p_Object->IsPlayer())
                            return true;

                        /// Must be melee
                        if (itr->Param1 < 0.0f)
                        {
                            if (p_Object->ToPlayer()->IsMeleeDamageDealer(itr->Param2 != 0.0f))
                                return false;
                        }
                        /// Must be ranged
                        else
                        {
                            if (p_Object->ToPlayer()->IsRangedDamageDealer(itr->Param2 != 0.0f))
                                return false;
                        }

                        return true;
                    });

                    break;
                }
                case SPELL_FILTER_BY_DISTANCE_DEST: //19
                {
                    if (m_targets.HasDst())
                    {
                        if (itr->Param1 < 0.0f)
                            targets.remove_if(JadeCore::DestDistanceCheck(false, (Position*)m_targets.GetDstPos(), itr->Param2));
                        else
                            targets.remove_if(JadeCore::DestDistanceCheck(true, (Position*)m_targets.GetDstPos(), itr->Param2));
                    }
                    else if (m_targets.HasSrc())
                    {
                        if (itr->Param1 < 0.0f)
                            targets.remove_if(JadeCore::DestDistanceCheck(false, (Position*)m_targets.GetSrcPos(), itr->Param2));
                        else
                            targets.remove_if(JadeCore::DestDistanceCheck(true, (Position*)m_targets.GetSrcPos(), itr->Param2));
                    }
                    break;
                }
                case SPELL_FILTER_BY_DISTANCE_PET: //20
                {
                    _target = nullptr;
                    if (m_caster->ToPlayer())
                        _target = m_caster->ToPlayer()->GetPet();
                    if (_target)
                    {
                        if (itr->Param1 < 0.0f)
                            targets.remove_if(JadeCore::UnitDistanceCheck(false, _target, itr->Param2));
                        else
                            targets.remove_if(JadeCore::UnitDistanceCheck(true, _target, itr->Param2));
                    }
                    break;
                }
                case SPELL_FILTER_BY_OWNER: // 21
                {
                    if (itr->Param1 < 0.0f)
                        targets.remove_if(JadeCore::UnitOwnerCheck(true, _caster));
                    else
                        targets.remove_if(JadeCore::UnitOwnerCheck(false, _caster));
                    break;
                }
                case SPELL_FILTER_ONLY_RANGED_SPEC: // 22
                {
                    std::list<WorldObject*> rangeList;

                    for (auto const& target : targets)
                    {
                        if (auto player = target->ToPlayer())
                        {
                            if (player->IsRangedDamageDealer(uint32(itr->Param1))) //param1 == 1 - Also the Healer
                                rangeList.push_back(target);
                        }
                    }
                    targets = rangeList;
                    break;
                }
                case SPELL_FILTER_ONLY_MELEE_SPEC: // 23
                {
                    std::list<WorldObject*> meleeList;

                    for (auto const& target : targets)
                    {
                        if (auto player = target->ToPlayer())
                        {
                            if (player->IsMeleeDamageDealer(uint32(itr->Param1))) //param1 == 1 - Also the Tank
                            {
                                meleeList.push_back(target);
                            }
                        }
                    }
                    targets = meleeList;
                    break;
                }
                case SPELL_FILTER_ONLY_TANK_SPEC_OR_NOT: // 24
                {
                    if (targets.size() <= uint32(itr->Param2))
                        break;

                    std::list<WorldObject*> tempList;

                    for (auto const& target : targets)
                    {
                        if (auto player = target->ToPlayer())
                        {
                            if (itr->Param1 < 0.0f) //Exclude tank players
                            {
                                if (!player->IsActiveSpecTankSpec())
                                    tempList.push_back(target);
                            }
                            else
                            {
                                if (player->IsActiveSpecTankSpec()) // Exclude non-tank players
                                    tempList.push_back(target);
                            }
                        }
                    }
                    targets = tempList;
                    break;
                }
                case SPELL_FILTER_BY_AURA_CASTER: // 25
                {
                    targets.remove_if(JadeCore::UnitAuraAndCheck(itr->Param1, itr->Param2, itr->Param3, (_owner ? _owner : _caster)->GetGUID()));
                    break;
                }
                case SPELL_FILTER_PLAYER_IS_HEALER_SPEC: // 26
                {
                    targets.remove_if([](WorldObject* object) -> bool
                        {
                            if (!object || !object->IsPlayer() || object->ToPlayer()->GetSpecializationRole() != ROLE_HEALER)
                                return true;
                            return false;
                        });
                    break;
                }
                case SPELL_FILTER_RANGED_SPEC_PRIORITY: // 27
                {
                    std::list<WorldObject*> rangeList;
                    std::list<WorldObject*> meleeList;
                    std::list<WorldObject*> tankList;

                    for (auto const& target : targets)
                    {
                        if (auto player = target->ToPlayer())
                        {
                            if (player->IsRangedDamageDealer(uint32(itr->Param1))) //param1 == 1 - Also the Healer
                                rangeList.push_back(target);
                            else if (player->IsMeleeDamageDealer(uint32(itr->Param2))) //param2 == 1 - Also the Tank
                            {
                                if (!player->isInTankSpec())
                                    meleeList.push_back(target);
                                else
                                    tankList.push_back(target);
                            }
                        }
                    }
                    JadeCore::Containers::RandomShuffleList(rangeList);
                    JadeCore::Containers::RandomShuffleList(meleeList);
                    JadeCore::Containers::RandomShuffleList(tankList);

                    targets.clear();
                    targets.splice(targets.end(), rangeList);
                    targets.splice(targets.end(), meleeList);
                    targets.splice(targets.end(), tankList);

                    if (uint32 maxTargets = m_spellValue->MaxAffectedTargets)
                    {
                        resizeType = 1;
                        targetCount = maxTargets;
                    }
                    break;
                }
                case SPELL_FILTER_MELEE_SPEC_PRIORITY: // 28
                {
                    std::list<WorldObject*> meleeList;
                    std::list<WorldObject*> rangeList;
                    std::list<WorldObject*> tankList;

                    for (auto const& target : targets)
                    {
                        if (auto player = target->ToPlayer())
                        {
                            if (player->IsMeleeDamageDealer(uint32(itr->Param1))) //param1 == 1 - Also the Tank
                            {
                                if (!player->isInTankSpec())
                                    meleeList.push_back(target);
                                else if (uint32(itr->Param1) == 1) // Tank
                                    tankList.push_back(target);
                            }
                            else if (player->IsRangedDamageDealer(uint32(itr->Param2))) //param2 == 1 - Also the Healer
                                rangeList.push_back(target);
                        }
                    }
                    JadeCore::Containers::RandomShuffleList(meleeList);
                    JadeCore::Containers::RandomShuffleList(rangeList);
                    JadeCore::Containers::RandomShuffleList(tankList);

                    targets.clear();
                    targets.splice(targets.end(), meleeList);
                    targets.splice(targets.end(), rangeList);
                    targets.splice(targets.end(), tankList);

                    if (uint32 maxTargets = m_spellValue->MaxAffectedTargets)
                    {
                        resizeType = 1;
                        targetCount = maxTargets;
                    }
                    break;
                }
                case SPELL_FILTER_TANK_SPEC_PRIORITY: // 29
                {
                    std::list<WorldObject*> tankList;
                    std::list<WorldObject*> nonTankList;

                    for (auto const& target : targets)
                    {
                        if (auto player = target->ToPlayer())
                        {
                            if (player->isInTankSpec())
                                tankList.push_back(target);
                            else
                                nonTankList.push_back(target);
                        }
                    }

                    JadeCore::Containers::RandomShuffleList(tankList);
                    JadeCore::Containers::RandomShuffleList(nonTankList);

                    targets.clear();

                    if (uint32(itr->Param1) == 0) //Priority tank player
                    {
                        targets.splice(targets.end(), tankList);
                        targets.splice(targets.end(), nonTankList);
                    }
                    else //Priority non-tank player
                    {
                        targets.splice(targets.end(), nonTankList);
                        targets.splice(targets.end(), tankList);
                    }

                    if (uint32 maxTargets = m_spellValue->MaxAffectedTargets)
                    {
                        resizeType = 1;
                        targetCount = maxTargets;
                    }
                    break;
                }
                case SPELL_FILTER_ONLY_PLAYER_TARGET: // 30
                {
                    bool allowPets = itr->Param1;

                    targets.remove_if([allowPets](WorldObject* object) -> bool
                        {
                            if (auto unit = object->ToUnit())
                                if (unit->IsPlayer() || (allowPets && unit->isPet()))
                                    return false;
                            return true;
                        });
                    break;
                }
                default:
                    break;
            }

            switch (itr->AddCaster)
            {
                case 1:
                    if (!targets.empty())
                        targets.remove(GetCaster());
                    targets.push_back(GetCaster());
                    break;
                case 2:
                    if (targets.empty())
                        targets.push_back(GetCaster());
                    break;
            }
        }

        switch (resizeType)
        {
            case eFilterResizeType::FilterResizeTypeFlat:
            {
                if (targets.size() > targetCount)
                    targets.resize(targetCount);
                break;
            }
            case eFilterResizeType::FilterResizeTypeRandom:
            case eFilterResizeType::FilterResizeTypeRaidSizePct:
            {
                if (targets.size() > targetCount)
                    JadeCore::Containers::RandomResizeList(targets, targetCount);

                break;
            }
            default:
                break;
        }
    }
}

void Spell::CallScriptObjectTargetSelectHandlers(WorldObject*& target, SpellEffIndex effIndex)
{
    for (std::list<SpellScript*>::iterator scritr = m_loadedScripts.begin(); scritr != m_loadedScripts.end(); ++scritr)
    {
        (*scritr)->_PrepareScriptCall(SPELL_SCRIPT_HOOK_OBJECT_TARGET_SELECT);
        std::list<SpellScript::ObjectTargetSelectHandler>::iterator hookItrEnd = (*scritr)->OnObjectTargetSelect.end(), hookItr = (*scritr)->OnObjectTargetSelect.begin();
        for (; hookItr != hookItrEnd; ++hookItr)
            if ((*hookItr).IsEffectAffected(m_spellInfo, effIndex))
                (*hookItr).Call(*scritr, target);

        (*scritr)->_FinishScriptCall();
    }
}

bool Spell::CanExecuteTriggersOnHit(uint32 effMask, SpellInfo const* triggeredByAura) const
{
    bool only_on_caster = (triggeredByAura && (triggeredByAura->AttributesEx4 & SPELL_ATTR4_PROC_ONLY_ON_CASTER));
    // If triggeredByAura has SPELL_ATTR4_PROC_ONLY_ON_CASTER then it can only proc on a casted spell with TARGET_UNIT_CASTER
    for (uint8 i = 0; i < m_spellInfo->EffectCount; ++i)
    {
        if ((effMask & (1 << i)) && (!only_on_caster || (m_spellInfo->Effects[i].TargetA.GetTarget() == TARGET_UNIT_CASTER)))
            return true;
    }
    return false;
}

bool Spell::CanProcOnTarget(Unit *target) const
{
    // Sudden Eclipse - PVP Druid Set Bonus
    if (m_spellInfo->Id == 95746)
        return true;

    /// Whirlwind
    if (m_spellInfo->Id == 1680 || m_spellInfo->Id == 190411)
        return true;

    /// Arcane Charge
    if (m_spellInfo->Id == 36032)
        return true;

    /// Holy Paladin Holy Prism
    if (m_spellInfo->Id == 114871 || m_spellInfo->Id == 114852)
        return true;

    // swd for priest, for other it does bugs
    // TODO: maybe need check m_caster != target
    if (m_spellInfo->SpellFamilyName != SPELLFAMILY_PRIEST && !m_spellInfo->IsPositive() && m_caster->IsFriendlyTo(target))
        return false;

    // Distract can't be proced
    if (m_spellInfo->HasEffect(SPELL_EFFECT_DISTRACT))
        return false;

    return true;
}

void Spell::PrepareTriggersExecutedOnHit()
{
    // todo: move this to scripts
    if (m_spellInfo->SpellFamilyName)
    {
        SpellInfo const* excludeCasterSpellInfo = sSpellMgr->GetSpellInfo(m_spellInfo->ExcludeCasterAuraSpell);
        if (excludeCasterSpellInfo && !excludeCasterSpellInfo->IsPositive())
            m_preCastSpell = m_spellInfo->ExcludeCasterAuraSpell;
        SpellInfo const* excludeTargetSpellInfo = sSpellMgr->GetSpellInfo(m_spellInfo->ExcludeTargetAuraSpell);
        if (excludeTargetSpellInfo && !excludeTargetSpellInfo->IsPositive())
            m_preCastSpell = m_spellInfo->ExcludeTargetAuraSpell;
    }

    // todo: move this to scripts
    switch (m_spellInfo->SpellFamilyName)
    {
        case SPELLFAMILY_MAGE:
        {
             // Permafrost
             if (m_spellInfo->SpellFamilyFlags[1] & 0x00001000 ||  m_spellInfo->SpellFamilyFlags[0] & 0x00100220)
                 m_preCastSpell = 68391; ///< @todo SpellID removed
             break;
        }
        case SPELLFAMILY_ROGUE:
        {
            // Shuriken storm
            if (m_spellInfo->Id == 197835)
                m_preCastSpell = 0;
            break;
        }
    }

    if (m_spellInfo->IsPassive())
        return;

    // handle SPELL_AURA_ADD_TARGET_TRIGGER auras:
    // save auras which were present on spell caster on cast, to prevent triggered auras from affecting caster
    // and to correctly calculate proc chance when combopoints are present
    Unit::AuraEffectList const& targetTriggers = m_caster->GetAuraEffectsByType(SPELL_AURA_ADD_TARGET_TRIGGER);
    for (Unit::AuraEffectList::const_iterator i = targetTriggers.begin(); i != targetTriggers.end(); ++i)
    {
        if (!(*i)->IsAffectingSpell(m_spellInfo))
            continue;
        SpellInfo const* auraSpellInfo = (*i)->GetSpellInfo();
        uint32 auraSpellIdx = (*i)->GetEffIndex();
        if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(auraSpellInfo->Effects[auraSpellIdx].TriggerSpell))
        {
            // calculate the chance using spell base amount, because aura amount is not updated on combo-points change
            // this possibly needs fixing
            int32 auraBaseAmount = (*i)->GetBaseAmount();
            // proc chance is stored in effect amount
            int32 chance = m_caster->CalculateSpellDamage(NULL, auraSpellInfo, auraSpellIdx, &auraBaseAmount);
            // build trigger and add to the list
            HitTriggerSpell spellTriggerInfo;
            spellTriggerInfo.triggeredSpell = spellInfo;
            spellTriggerInfo.triggeredByAura = auraSpellInfo;
            spellTriggerInfo.chance = chance * (*i)->GetBase()->GetStackAmount();
            m_hitTriggerSpells.push_back(spellTriggerInfo);
        }
    }
}

/// Global cooldowns management
enum GCDLimits
{
    MIN_GCD = 750,
    MAX_GCD = 1500
};

bool Spell::HasGlobalCooldown() const
{
    // Only player or controlled units have global cooldown
    if (m_caster->GetCharmInfo())
        return m_caster->GetCharmInfo()->GetGlobalCooldownMgr().HasGlobalCooldown(m_spellInfo);
    else if (m_caster->IsPlayer())
        return m_caster->ToPlayer()->GetGlobalCooldownMgr().HasGlobalCooldown(m_spellInfo);
    else
        return false;
}

void Spell::TriggerGlobalCooldown()
{
    int32 l_Gcd = m_spellInfo->StartRecoveryTime;
    if (!l_Gcd)
        return;

    // Only players or controlled units have global cooldown
    if (m_caster->GetTypeId() != TYPEID_PLAYER && !m_caster->GetCharmInfo())
        return;

     if (m_caster->IsPlayer())
          if (m_caster->ToPlayer()->GetCommandStatus(CHEAT_COOLDOWN))
               return;

     bool l_BenefitedFromHaste = false;
    // Global cooldown can't leave range 1..1.5 secs
    // There are some spells (mostly not casted directly by player) that have < 1 sec and > 1.5 sec global cooldowns
    // but as tests show are not affected by any spell mods.
    if (m_spellInfo->StartRecoveryTime >= MIN_GCD && m_spellInfo->StartRecoveryTime <= MAX_GCD)
    {
        // gcd modifier auras are applied only to own spells and only players have such mods
        if (m_caster->IsPlayer())
        {
            m_caster->ToPlayer()->ApplySpellMod(m_spellInfo->Id, SPELLMOD_GLOBAL_COOLDOWN, l_Gcd, this);

            if (int32 l_CooldownMod = m_caster->ToPlayer()->GetTotalAuraModifier(SPELL_AURA_MOD_GLOBAL_COOLDOWN_BY_HASTE))
            {
                float l_Haste = m_caster->ToPlayer()->GetFloatValue(UNIT_FIELD_MOD_HASTE);
                l_Gcd *= ApplyPct(l_Haste, l_CooldownMod);
                l_BenefitedFromHaste = true;
            }
        }

        if (l_Gcd == 0)
            return;

        // Apply haste rating. Mod casting speed is included in unit field mod haste. NEED TO BE CHANGED WHEN CASTING SPEED IS FINALLY FIXED.
        if (!l_BenefitedFromHaste)
            l_Gcd = int32(float(l_Gcd) * m_caster->GetFloatValue(UNIT_FIELD_MOD_CASTING_SPEED));

        l_Gcd = int32(float(l_Gcd) * m_caster->GetFloatValue(UNIT_FIELD_MOD_TIME_RATE));

        if (l_Gcd < MIN_GCD)
            l_Gcd = MIN_GCD;
        else if (l_Gcd > MAX_GCD)
            l_Gcd = MAX_GCD;
    }

    // Only players or controlled units have global cooldown
    if (m_caster->GetCharmInfo())
        m_caster->GetCharmInfo()->GetGlobalCooldownMgr().AddGlobalCooldown(m_spellInfo, l_Gcd);
    else if (m_caster->IsPlayer())
        m_caster->ToPlayer()->GetGlobalCooldownMgr().AddGlobalCooldown(m_spellInfo, l_Gcd);
}

void Spell::CancelGlobalCooldown()
{
    if (!m_spellInfo->StartRecoveryTime)
        return;

    // Cancel global cooldown when interrupting current cast
    if (m_caster->GetCurrentSpell(CURRENT_GENERIC_SPELL) != this)
        return;

    // Only players or controlled units have global cooldown
    if (m_caster->GetCharmInfo())
        m_caster->GetCharmInfo()->GetGlobalCooldownMgr().CancelGlobalCooldown(m_spellInfo);
    else if (m_caster->IsPlayer())
        m_caster->ToPlayer()->GetGlobalCooldownMgr().CancelGlobalCooldown(m_spellInfo);
}

bool Spell::IsCritForTarget(Unit* target) const
{
    if (!target)
        return false;

    for (auto& itr : m_UniqueTargetInfo)
        if (itr.targetGUID == target->GetGUID() && itr.crit)
            return true;

    return false;
}

bool Spell::IsInterruptable() const
{
    switch (m_spellInfo->Id)
    {
        case 210451: ///< Bonds of Terror (Xavius)
            return false;
        default:
            break;
    }

    return !m_executedCurrently;
}

void Spell::LoadAttrDummy()
{
    AttributesCustom = m_spellInfo->Attributes;
    AttributesCustomEx = m_spellInfo->AttributesEx;
    AttributesCustomEx2 = m_spellInfo->AttributesEx2;
    AttributesCustomEx3 = m_spellInfo->AttributesEx3;
    AttributesCustomEx4 = m_spellInfo->AttributesEx4;
    AttributesCustomEx5 = m_spellInfo->AttributesEx5;
    AttributesCustomEx6 = m_spellInfo->AttributesEx6;
    AttributesCustomEx7 = m_spellInfo->AttributesEx7;
    AttributesCustomEx8 = m_spellInfo->AttributesEx8;
    AttributesCustomEx9 = m_spellInfo->AttributesEx9;
    AttributesCustomEx10 = m_spellInfo->AttributesEx10;
    AttributesCustomEx11 = m_spellInfo->AttributesEx11;
    AttributesCustomEx12 = m_spellInfo->AttributesEx12;
    AttributesCustomEx13 = m_spellInfo->AttributesEx13;
    AttributesCustomCu = m_spellInfo->AttributesCu;

    if (std::vector<SpellAuraDummy> const* spellAuraDummy = sSpellMgr->GetSpellAuraDummy(m_spellInfo->Id))
    {
        for (std::vector<SpellAuraDummy>::const_iterator itr = spellAuraDummy->begin(); itr != spellAuraDummy->end(); ++itr)
        {
            Unit* _caster = m_caster;
            bool check = false;

            switch (itr->Options)
            {
                case SPELL_DUMMY_ADD_ATTRIBUTE: //3
                {
                    if (itr->AttrValue != 0)
                    {
                        int32 saveAttrValue = itr->AttrValue;
                        if (itr->SpellDummyID > 0 && !_caster->HasAura(itr->SpellDummyID))
                            saveAttrValue *= -1;
                        if (itr->SpellDummyID < 0 && _caster->HasAura(abs(itr->SpellDummyID)))
                            saveAttrValue *= -1;

                        switch (itr->Attr)
                        {
                            case -1:
                                if (saveAttrValue > 0)
                                    AttributesCustomCu |= saveAttrValue;
                                else
                                    AttributesCustomCu &= ~saveAttrValue;
                                break;
                            case 0:
                                if (saveAttrValue > 0)
                                    AttributesCustom |= saveAttrValue;
                                else
                                    AttributesCustom &= ~saveAttrValue;
                                break;
                            case 1:
                                if (saveAttrValue > 0)
                                    AttributesCustomEx |= saveAttrValue;
                                else
                                    AttributesCustomEx &= ~saveAttrValue;
                                break;
                            case 2:
                                if (saveAttrValue > 0)
                                    AttributesCustomEx2 |= saveAttrValue;
                                else
                                    AttributesCustomEx2 &= ~saveAttrValue;
                                break;
                            case 3:
                                if (saveAttrValue > 0)
                                    AttributesCustomEx3 |= saveAttrValue;
                                else
                                    AttributesCustomEx3 &= ~saveAttrValue;
                                break;
                            case 4:
                                if (saveAttrValue > 0)
                                    AttributesCustomEx4 |= saveAttrValue;
                                else
                                    AttributesCustomEx4 &= ~saveAttrValue;
                                break;
                            case 5:
                                if (saveAttrValue > 0)
                                    AttributesCustomEx5 |= saveAttrValue;
                                else
                                    AttributesCustomEx5 &= ~saveAttrValue;
                                break;
                            case 6:
                                if (saveAttrValue > 0)
                                    AttributesCustomEx6 |= saveAttrValue;
                                else
                                    AttributesCustomEx6 &= ~saveAttrValue;
                                break;
                            case 7:
                                if (saveAttrValue > 0)
                                    AttributesCustomEx7 |= saveAttrValue;
                                else
                                    AttributesCustomEx7 &= ~saveAttrValue;
                                break;
                            case 8:
                                if (saveAttrValue > 0)
                                    AttributesCustomEx8 |= saveAttrValue;
                                else
                                    AttributesCustomEx8 &= ~saveAttrValue;
                                break;
                            case 9:
                                if (saveAttrValue > 0)
                                    AttributesCustomEx9 |= saveAttrValue;
                                else
                                    AttributesCustomEx9 &= ~saveAttrValue;
                                break;
                            case 10:
                                if (saveAttrValue > 0)
                                    AttributesCustomEx10 |= saveAttrValue;
                                else
                                    AttributesCustomEx10 &= ~saveAttrValue;
                                break;
                            case 11:
                                if (saveAttrValue > 0)
                                    AttributesCustomEx11 |= saveAttrValue;
                                else
                                    AttributesCustomEx11 &= ~saveAttrValue;
                                break;
                            case 12:
                                if (saveAttrValue > 0)
                                    AttributesCustomEx12 |= saveAttrValue;
                                else
                                    AttributesCustomEx12 &= ~saveAttrValue;
                                break;
                            case 13:
                                if (saveAttrValue > 0)
                                    AttributesCustomEx13 |= saveAttrValue;
                                else
                                    AttributesCustomEx13 &= ~saveAttrValue;
                                break;
                        }
                    }

                    break;
                }
            }

            if (check && itr->RemoveAura)
                _caster->RemoveAurasDueToSpell(itr->RemoveAura);
        }
    }
}

bool Spell::IsDarkSimulacrum() const
{
    // Dark Simulacrum
    if (AuraEffect* darkSimulacrum = m_caster->GetAuraEffect(77616, 0))
    {
        if (m_spellInfo->Id == darkSimulacrum->GetAmount())
            return true;
        else
        {
            SpellInfo const* amountSpell = sSpellMgr->GetSpellInfo(darkSimulacrum->GetAmount());
            if (!amountSpell)
                return false;

            SpellInfo const* triggerSpell = sSpellMgr->GetSpellInfo(amountSpell->Effects[0].BasePoints);
            if (!triggerSpell)
                return false;

            if (m_spellInfo->Id == triggerSpell->Id)
                return true;
        }
    }

    return false;
}

bool Spell::CanSpellProc(Unit* p_Target, uint32 p_Mask) const
{
    ///< Kil'jaeden's Burning Wish, Infernal Cinders & Ravaging Storm must be spell proc from trinket
    if (m_CastItem && m_spellInfo->Id != 235999 && m_spellInfo->Id != 242218 && m_spellInfo->Id != 257286)
        return false;
    if (AttributesCustom & SPELL_ATTR0_HIDDEN_CLIENTSIDE)
        return false;
    if (AttributesCustomEx3 & SPELL_ATTR3_CANT_TRIGGER_PROC)
        return false;
    if (AttributesCustomEx6 & SPELL_ATTR6_CANT_PROC)
        return false;
    if (AttributesCustomEx7 & SPELL_ATTR7_CONSOLIDATED_RAID_BUFF)
        return false;
    if (AttributesCustomEx2 & SPELL_ATTR2_FOOD_BUFF)
        return false;
    if (p_Target && !p_Target->CanProc())
        return false;
    if (!CanExecuteTriggersOnHit(p_Mask))
        return false;
    if (m_spellInfo->IsPassive())
        return false;

    return true;
}

bool Spell::LOSAdditionalRules(Unit const* target, int8 eff) const
{
    // Okay, custom rules for LoS
    for (uint8 x = (eff == -1 ? 0 : eff); x < (eff == -1 ? m_spellInfo->EffectCount : eff + 1); ++x)
    {
        // like paladin auras
        if (m_spellInfo->Effects[x].Effect == SPELL_EFFECT_APPLY_AREA_AURA_RAID)
            return true;

        // like bloodlust / prayers
        if (m_spellInfo->Effects[x].ApplyAuraName && (m_spellInfo->Effects[x].TargetB.GetTarget() == TARGET_UNIT_SRC_AREA_ALLY ||
            m_spellInfo->Effects[x].TargetA.GetTarget() == TARGET_UNIT_CASTER_AREA_RAID))
            return !IsMorePowerfulAura(target);

        if (m_spellInfo->IsChanneled())
            continue;

        switch (m_spellInfo->Effects[x].TargetA.GetTarget())
        {
            case TARGET_UNIT_PET:
            case TARGET_UNIT_MASTER:
                return true;
            default:
                break;
        }
    }

    return false;
}

bool Spell::IsMorePowerfulAura(Unit const* target) const
{
    if (m_spellInfo->GetDuration() >= 2 * MINUTE * IN_MILLISECONDS)
    {
        switch (m_spellInfo->Effects[0].ApplyAuraName)
        {
            case SPELL_AURA_MOD_TOTAL_STAT_PERCENTAGE:
            case SPELL_AURA_MOD_STAT:
            case SPELL_AURA_MOD_RANGED_ATTACK_POWER:
            {
                Unit::VisibleAuraMap const *visibleAuras = const_cast<Unit*>(target)->GetVisibleAuras();
                for (Unit::VisibleAuraMap::const_iterator itr = visibleAuras->begin(); itr != visibleAuras->end(); ++itr)
                    if (AuraEffect* auraeff = itr->second->GetBase()->GetEffect(0))
                    {
                        if (auraeff->GetBase()->GetDuration() <= 2*MINUTE*IN_MILLISECONDS)
                            continue;

                        if (auraeff->GetSpellInfo()->SpellFamilyName == SPELLFAMILY_POTION)
                            continue;

                        if (auraeff->GetAuraType() == m_spellInfo->Effects[0].ApplyAuraName &&
                            (m_spellInfo->Effects[0].ApplyAuraName == SPELL_AURA_MOD_RANGED_ATTACK_POWER || m_spellInfo->Effects[0].MiscValue == auraeff->GetMiscValue()))
                        {
                            uint32 dmg = abs(CalculateDamage(0, target));
                            uint32 amount = abs(auraeff->GetAmount());
                            if (amount < dmg)
                                continue;

                            else if (amount == dmg && m_spellInfo->GetDuration() > auraeff->GetBase()->GetDuration())
                                continue;

                            return true;
                        }
                    }
            }
            default:
                break;
        }
    }
    return false;
}

bool Spell::IsSpellTriggeredAfterCast() const
{
    switch (m_spellInfo->Id)
    {
        case 29722:  ///< Incinerate
            return true;
        default:
            return false;
    }
    return false;
}

void Spell::UpdateSpellCastDataAmmo(uint32& p_AmmoDisplayID)
{
    uint32 l_AmmoDisplayID = 0;

    if (m_caster->IsPlayer())
    {
        if (Item* l_Item = m_caster->ToPlayer()->GetWeaponForAttack(WeaponAttackType::RangedAttack, true))
            l_AmmoDisplayID = GetItemDisplayID(l_Item->GetVisibleEntry(m_caster->ToPlayer()), l_Item->GetVisibleAppearanceModID(m_caster->ToPlayer()));
    }
    else
    {
        for (uint8 l_I = 0; l_I < 3; ++l_I)
        {
            if (uint32 l_ItemID = m_caster->GetVirtualItemId(l_I))
            {
                if (ItemEntry const* l_ItemEntry = sItemStore.LookupEntry(l_ItemID))
                {
                    if (l_ItemEntry->ClassId == ITEM_CLASS_WEAPON)
                    {
                        switch (l_ItemEntry->SubclassId)
                        {
                            case ITEM_SUBCLASS_WEAPON_BOW:
                            case ITEM_SUBCLASS_WEAPON_CROSSBOW:
                                l_AmmoDisplayID = 5996;       // is this need fixing?
                                break;
                            case ITEM_SUBCLASS_WEAPON_GUN:
                                l_AmmoDisplayID = 5998;       // is this need fixing?
                                break;
                            default:
                                l_AmmoDisplayID = GetItemDisplayID(l_ItemID, m_caster->GetVirtualItemAppearanceMod(l_I));
                                break;
                        }

                        if (l_AmmoDisplayID)
                            break;
                    }
                }
            }
        }
    }

    p_AmmoDisplayID = l_AmmoDisplayID;
}

bool Spell::BypassAuraCheckForAutoRepeatSpell()
{
    switch (m_spellInfo->Id)
    {
        case 30213: ///< Legion Strike
        case 115625: ///< Mortal Cleave
        {
            if ((m_caster->GetEntry() == WarlockPet::FelGuard || m_caster->GetEntry() == WarlockPet::WrathGuard) && m_caster->GetPower(POWER_ENERGY) > 60)
                return true;
            break;
        }
        default:
            break;
    }

    return false;
}

void Spell::CallDBScriptBeforeCast(uint32 p_SpellId)
{
    SpellScriptMapBounds l_Bounds = sSpellMgr->mSpellScriptMap.equal_range(p_SpellId);
    for (auto l_Itr = l_Bounds.first; l_Itr != l_Bounds.second; ++l_Itr)
    {
        SpellScriptEntry l_SpellScript = (*l_Itr).second;
        if (l_SpellScript.Hook != eSpellHooks::BeforeCast)
            continue;

        DoAction(l_SpellScript);
    }
}

void Spell::CallDBScriptOnCast(uint32 p_SpellId)
{
    SpellScriptMapBounds l_Bounds = sSpellMgr->mSpellScriptMap.equal_range(p_SpellId);
    for (auto l_Itr = l_Bounds.first; l_Itr != l_Bounds.second; ++l_Itr)
    {
        SpellScriptEntry l_SpellScript = (*l_Itr).second;
        if (l_SpellScript.Hook != eSpellHooks::OnCast)
            continue;

        DoAction(l_SpellScript);
    }
}

void Spell::CallDBScriptAfterCast(uint32 p_SpellId)
{
    SpellScriptMapBounds l_Bounds = sSpellMgr->mSpellScriptMap.equal_range(p_SpellId);
    for (auto l_Itr = l_Bounds.first; l_Itr != l_Bounds.second; ++l_Itr)
    {
        SpellScriptEntry l_SpellScript = (*l_Itr).second;
        if (l_SpellScript.Hook != eSpellHooks::AfterCast)
            continue;

        DoAction(l_SpellScript);
    }
}

void Spell::CallDBScriptOnPrepare(uint32 p_SpellId)
{
    SpellScriptMapBounds l_Bounds = sSpellMgr->mSpellScriptMap.equal_range(p_SpellId);
    for (auto l_Itr = l_Bounds.first; l_Itr != l_Bounds.second; ++l_Itr)
    {
        SpellScriptEntry l_SpellScript = (*l_Itr).second;
        if (l_SpellScript.Hook != eSpellHooks::OnPrepare)
            continue;

        DoAction(l_SpellScript);
    }
}

SpellCastResult Spell::CallDBScriptCheckCast(uint32 p_SpellId)
{
    SpellCastResult l_Result = SPELL_CAST_OK;
    SpellScriptMapBounds l_Bounds = sSpellMgr->mSpellScriptMap.equal_range(p_SpellId);
    for (auto l_Itr = l_Bounds.first; l_Itr != l_Bounds.second; ++l_Itr)
    {
        SpellScriptEntry l_SpellScript = (*l_Itr).second;
        if (l_SpellScript.Hook != eSpellHooks::CheckCast)
            continue;

        DoAction(l_SpellScript);

        Unit* l_ActionTarget = GetScriptActor(l_SpellScript, l_SpellScript.ActionTarget);
        if (!l_ActionTarget)
            return l_Result;

        uint64 l_OriginalCasterGUID = 0;
        Unit* l_ActionOriginalCaster = GetScriptActor(l_SpellScript, l_SpellScript.ActionOriginalCaster);
        if (l_ActionOriginalCaster)
            l_OriginalCasterGUID = l_ActionOriginalCaster->GetGUID();

        for (uint32 l_Aura : l_SpellScript.AuraState)
        {
            if (!l_ActionTarget->HasAura(l_Aura, l_OriginalCasterGUID))
                return SPELL_FAILED_CASTER_AURASTATE;
        }

        for (uint32 l_Aura : l_SpellScript.ExcludeAuraState)
        {
            if (l_ActionTarget->HasAura(l_Aura, l_OriginalCasterGUID))
                return SPELL_FAILED_CASTER_AURASTATE;
        }
    }

    return l_Result;
}

void Spell::CallDBScriptTakePowers(uint32 p_SpellId, Powers p_PowerType, int32& p_PowerCost)
{
    SpellScriptMapBounds l_Bounds = sSpellMgr->mSpellScriptMap.equal_range(p_SpellId);
    for (auto l_Itr = l_Bounds.first; l_Itr != l_Bounds.second; ++l_Itr)
    {
        SpellScriptEntry l_SpellScript = (*l_Itr).second;
        if (l_SpellScript.Hook != eSpellHooks::TakePowers)
            continue;

        DoAction(l_SpellScript);
    }
}

void Spell::CallDBScriptOnEffectHit(uint32 p_SpellId, SpellEffIndex p_EffectIndex)
{
    SpellScriptMapBounds l_Bounds = sSpellMgr->mSpellScriptMap.equal_range(p_SpellId);
    for (auto l_Itr = l_Bounds.first; l_Itr != l_Bounds.second; ++l_Itr)
    {
        SpellScriptEntry l_SpellScript = (*l_Itr).second;
        if (l_SpellScript.Hook != eSpellHooks::OnEffectHit)
            continue;

        if (l_SpellScript.EffectId != p_EffectIndex)
            continue;

        DoAction(l_SpellScript);
    }
}

void Spell::CallDBScriptOnEffectHitTarget(uint32 p_SpellId, SpellEffIndex p_EffectIndex)
{
    SpellScriptMapBounds l_Bounds = sSpellMgr->mSpellScriptMap.equal_range(p_SpellId);
    for (auto l_Itr = l_Bounds.first; l_Itr != l_Bounds.second; ++l_Itr)
    {
        SpellScriptEntry l_SpellScript = (*l_Itr).second;
        if (l_SpellScript.Hook != eSpellHooks::OnEffectHitTarget)
            continue;

        if (l_SpellScript.EffectId != p_EffectIndex)
            continue;

        DoAction(l_SpellScript);
    }
}

void Spell::CallDBScriptBeforeHit(uint32 p_SpellId)
{
    SpellScriptMapBounds l_Bounds = sSpellMgr->mSpellScriptMap.equal_range(p_SpellId);
    for (auto l_Itr = l_Bounds.first; l_Itr != l_Bounds.second; ++l_Itr)
    {
        SpellScriptEntry l_SpellScript = (*l_Itr).second;
        if (l_SpellScript.Hook != eSpellHooks::BeforeHit)
            continue;

        DoAction(l_SpellScript);
    }
}

void Spell::CallDBScriptOnHit(uint32 p_SpellId)
{
    SpellScriptMapBounds l_Bounds = sSpellMgr->mSpellScriptMap.equal_range(p_SpellId);
    for (auto l_Itr = l_Bounds.first; l_Itr != l_Bounds.second; ++l_Itr)
    {
        SpellScriptEntry l_SpellScript = (*l_Itr).second;
        if (l_SpellScript.Hook != eSpellHooks::OnHit)
            continue;

        DoAction(l_SpellScript);
    }
}

void Spell::CallDBScriptAfterHit(uint32 p_SpellId)
{
    SpellScriptMapBounds l_Bounds = sSpellMgr->mSpellScriptMap.equal_range(p_SpellId);
    for (auto l_Itr = l_Bounds.first; l_Itr != l_Bounds.second; ++l_Itr)
    {
        SpellScriptEntry l_SpellScript = (*l_Itr).second;
        if (l_SpellScript.Hook != eSpellHooks::AfterHit)
            continue;

        DoAction(l_SpellScript);
    }
}

void Spell::DoAction(SpellScriptEntry p_SpellScript)
{
    Unit* l_ActionCaster = GetScriptActor(p_SpellScript, p_SpellScript.ActionCaster);
    Unit* l_ActionTarget = GetScriptActor(p_SpellScript, p_SpellScript.ActionTarget);

    if (l_ActionCaster)
    {
        for (uint32 l_SpellId : p_SpellScript.AuraState)
        {
            if (!l_ActionCaster->HasAura(l_SpellId))
                return;
        }

        for (uint32 l_SpellId : p_SpellScript.ExcludeAuraState)
        {
            if (l_ActionCaster->HasAura(l_SpellId))
                return;
        }
    }

    if (l_ActionTarget)
    {
        for (uint32 l_SpellId : p_SpellScript.TargetAuraState)
        {
            if (!l_ActionTarget->HasAura(l_SpellId))
                return;
        }

        for (uint32 l_SpellId : p_SpellScript.TargetExcludeAuraState)
        {
            if (l_ActionTarget->HasAura(l_SpellId))
                return;
        }
    }

    switch (p_SpellScript.Action)
    {
        case eSpellScriptActions::SpellCast:
        {
            if (!l_ActionCaster || !l_ActionTarget)
                return;

            l_ActionCaster->CastSpell(l_ActionTarget, p_SpellScript.ActionSpellId, p_SpellScript.Triggered);
            break;
        }
        case eSpellScriptActions::AuraRemove:
        {
            if (!l_ActionCaster || !l_ActionTarget)
                return;

            l_ActionTarget->RemoveAurasDueToSpell(p_SpellScript.ActionSpellId, l_ActionCaster->GetGUID());
            break;
        }
        case eSpellScriptActions::CooldownReduce:
        {
            if (!l_ActionCaster)
                return;

            Player* l_PlayerCaster = l_ActionCaster->ToPlayer();
            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(p_SpellScript.TargetSpellId);
            if (!l_PlayerCaster || !l_SpellInfo)
                return;

            uint32 l_CooldownReduction = 0;
            if (p_SpellScript.ActionSpellId && p_SpellScript.DataEffectId != -1)
            {
                switch (p_SpellScript.DataSource)
                {
                    case eDataSources::SourceSpellInfo:
                    {
                        if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(p_SpellScript.ActionSpellId))
                            l_CooldownReduction = l_SpellInfo->Effects[p_SpellScript.DataEffectId].BasePoints;
                        break;
                    }
                    case eDataSources::SourceAura:
                    {
                        if (l_ActionCaster)
                        {
                            if (AuraEffect* l_SourceEffect = l_ActionTarget->GetAuraEffect(p_SpellScript.ActionSpellId, p_SpellScript.DataEffectId))
                                l_CooldownReduction = l_SourceEffect->GetAmount();
                        }
                        break;
                    }
                    default:
                        break;
                }
            }

            if (l_SpellInfo->ChargeCategoryEntry != 0)
                l_PlayerCaster->ReduceChargeCooldown(l_SpellInfo->ChargeCategoryEntry, l_CooldownReduction * IN_MILLISECONDS);
            else
                l_PlayerCaster->ReduceSpellCooldown(p_SpellScript.TargetSpellId, l_CooldownReduction * IN_MILLISECONDS);

            break;
        }
        case eSpellScriptActions::CooldownRemove:
        {
            if (!l_ActionCaster)
                return;

            Player* l_PlayerCaster = l_ActionCaster->ToPlayer();
            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(p_SpellScript.TargetSpellId);
            if (!l_PlayerCaster || !l_SpellInfo)
                return;

            if (l_SpellInfo->ChargeCategoryEntry != 0)
                l_PlayerCaster->RestoreCharge(l_SpellInfo->ChargeCategoryEntry);
            else
                l_PlayerCaster->RemoveSpellCooldown(p_SpellScript.TargetSpellId);

            break;
        }
        case eSpellScriptActions::ModDamage:
        {
            if (!IsAnEffectHook(p_SpellScript))
                break;

            int32 l_Value = 0;
            if (p_SpellScript.ActionSpellId && p_SpellScript.DataEffectId != -1)
            {
                if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(p_SpellScript.ActionSpellId))
                    l_Value = l_SpellInfo->Effects[p_SpellScript.DataEffectId].BasePoints;
            }

            if (l_Value && p_SpellScript.ArtifactTraitId)
            {
                if (l_ActionCaster)
                {
                    if (Player* l_Player = l_ActionCaster->ToPlayer())
                        l_Value *= l_Player->GetRankOfArtifactPowerId(p_SpellScript.ArtifactTraitId);
                }
            }

            if (!l_Value)
                return;

            switch (p_SpellScript.CalculationType)
            {
                case eCalculationType::TypeCalcPct:
                    m_damage = CalculatePct(m_damage, (l_Value / 100.0f));
                    break;
                case eCalculationType::TypeAddPct:
                    m_damage = CalculatePct(m_damage, 1 + (l_Value / 100.0f));
                    break;
                case eCalculationType::TypeRemovePct:
                    m_damage = CalculatePct(m_damage, 1 - (l_Value / 100.0f));
                    break;
                case eCalculationType::TypeAddFlat:
                    m_damage += l_Value;
                    break;
                case eCalculationType::TypeRemoveFlat:
                    m_damage-= l_Value;
                    break;
                default:
                    break;
            }
            break;
        }
        case eSpellScriptActions::SetDamage:
        {
            if (!IsAnEffectHook(p_SpellScript))
                break;

            int32 l_Value = 0;
            if (p_SpellScript.ActionSpellId && p_SpellScript.DataEffectId != -1)
            {
                if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(p_SpellScript.ActionSpellId))
                    l_Value = l_SpellInfo->Effects[p_SpellScript.DataEffectId].BasePoints;
            }

            if (l_Value && p_SpellScript.ArtifactTraitId)
            {
                if (l_ActionCaster)
                {
                    if (Player* l_Player = l_ActionCaster->ToPlayer())
                        l_Value *= l_Player->GetRankOfArtifactPowerId(p_SpellScript.ArtifactTraitId);
                }
            }

            if (!l_Value)
                return;

            switch (p_SpellScript.CalculationType)
            {
                case eCalculationType::TypeCalcPct:
                {
                    if (l_ActionTarget)
                    {
                        switch (p_SpellScript.DataStat)
                        {
                            case -1:
                                m_damage = CalculatePct(l_ActionTarget->GetHealth(), (l_Value / 100.0f));
                                break;
                            default:
                                m_damage = CalculatePct(l_ActionTarget->GetPower(Powers(p_SpellScript.DataStat)), (l_Value / 100.0f));
                                break;
                        }
                    }
                    break;
                }
                case eCalculationType::TypeAddPct:
                {
                    if (l_ActionTarget)
                    {
                        switch (p_SpellScript.DataStat)
                        {
                            case -1:
                                m_damage = CalculatePct(l_ActionTarget->GetHealth(), 1 + (l_Value / 100.0f));
                                break;
                            default:
                                m_damage = CalculatePct(l_ActionTarget->GetPower(Powers(p_SpellScript.DataStat)), 1 + (l_Value / 100.0f));
                                break;
                        }
                    }
                    break;
                }
                case eCalculationType::TypeRemovePct:
                {
                    if (l_ActionTarget)
                    {
                        switch (p_SpellScript.DataStat)
                        {
                            case -1:
                                m_damage  = CalculatePct(l_ActionTarget->GetHealth(), 1 - (l_Value / 100.0f));
                                break;
                            default:
                                m_damage = CalculatePct(l_ActionTarget->GetPower(Powers(p_SpellScript.DataStat)), 1 - (l_Value / 100.0f));
                                break;
                        }
                    }
                    break;
                }
                case eCalculationType::TypeAddFlat:
                    m_damage = l_Value;
                    break;
                case eCalculationType::TypeRemoveFlat:
                    m_damage = -l_Value;
                    break;
                default:
                    break;
            }
            break;
        }
        case eSpellScriptActions::ModAuraAmount:
        {
            AuraEffect* l_AuraEffect = nullptr;

            if (p_SpellScript.TargetSpellId && p_SpellScript.TargetEffectId != -1)
            {
                if (Aura* l_Aura = GetCaster()->GetAura(p_SpellScript.TargetSpellId))
                    l_AuraEffect = l_Aura->GetEffect(p_SpellScript.TargetEffectId);
            }

            if (!l_AuraEffect)
                return;

            int32 l_Value = 0;
            if (p_SpellScript.ActionSpellId && p_SpellScript.DataEffectId != -1)
            {
                switch (p_SpellScript.DataSource)
                {
                    case eDataSources::SourceSpellInfo:
                    {
                        if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(p_SpellScript.ActionSpellId))
                            l_Value = l_SpellInfo->Effects[p_SpellScript.DataEffectId].BasePoints;
                        break;
                    }
                    case eDataSources::SourceAura:
                    {
                        if (l_ActionTarget)
                        {
                            if (AuraEffect* l_SourceEffect = l_ActionTarget->GetAuraEffect(p_SpellScript.ActionSpellId, p_SpellScript.DataEffectId))
                                l_Value = l_SourceEffect->GetAmount();
                        }
                        break;
                    }
                    default:
                        break;
                }
            }

            if (l_Value && p_SpellScript.ArtifactTraitId)
            {
                if (l_ActionCaster)
                {
                    if (Player* l_Player = l_ActionCaster->ToPlayer())
                        l_Value *= l_Player->GetRankOfArtifactPowerId(p_SpellScript.ArtifactTraitId);
                }
            }

            if (!l_Value)
                return;

            switch (p_SpellScript.CalculationType)
            {
                case eCalculationType::TypeCalcPct:
                    l_AuraEffect->SetAmount(CalculatePct(l_AuraEffect->GetAmount(), l_Value));
                    break;
                case eCalculationType::TypeAddPct:
                    l_AuraEffect->SetAmount(CalculatePct(l_AuraEffect->GetAmount(), 100 + l_Value));
                    break;
                case eCalculationType::TypeRemovePct:
                    l_AuraEffect->SetAmount(CalculatePct(l_AuraEffect->GetAmount(), 100 - l_Value));
                    break;
                case eCalculationType::TypeAddFlat:
                    l_AuraEffect->SetAmount(l_AuraEffect->GetAmount() + l_Value);
                    break;
                case eCalculationType::TypeRemoveFlat:
                    l_AuraEffect->SetAmount(l_AuraEffect->GetAmount() + l_Value);
                    break;
                default:
                    break;
            }
            break;
        }
        case eSpellScriptActions::SetAuraAmount:
        {
            AuraEffect* l_AuraEffect = nullptr;

            if (p_SpellScript.TargetSpellId && p_SpellScript.TargetEffectId != -1)
            {
                if (Aura* l_Aura = GetCaster()->GetAura(p_SpellScript.TargetSpellId))
                    l_AuraEffect = l_Aura->GetEffect(p_SpellScript.TargetEffectId);
            }

            if (!l_AuraEffect)
                return;

            int32 l_Value = 0;
            if (p_SpellScript.ActionSpellId && p_SpellScript.DataEffectId != -1)
            {
                switch (p_SpellScript.DataSource)
                {
                    case eDataSources::SourceSpellInfo:
                    {
                        if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(p_SpellScript.ActionSpellId))
                            l_Value = l_SpellInfo->Effects[p_SpellScript.DataEffectId].BasePoints;
                        break;
                    }
                    case eDataSources::SourceAura:
                    {
                        if (l_ActionTarget)
                        {
                            if (AuraEffect* l_SourceEffect = l_ActionTarget->GetAuraEffect(p_SpellScript.ActionSpellId, p_SpellScript.DataEffectId))
                                l_Value = l_SourceEffect->GetAmount();
                        }
                        break;
                    }
                    default:
                        break;
                }
            }

            if (l_Value && p_SpellScript.ArtifactTraitId)
            {
                if (l_ActionCaster)
                {
                    if (Player* l_Player = l_ActionCaster->ToPlayer())
                        l_Value *= l_Player->GetRankOfArtifactPowerId(p_SpellScript.ArtifactTraitId);
                }
            }

            if (!l_Value)
                return;

            switch (p_SpellScript.CalculationType)
            {
                case eCalculationType::TypeCalcPct:
                {
                    if (l_ActionTarget)
                    {
                        switch (p_SpellScript.DataStat)
                        {
                            case -1:
                                l_AuraEffect->SetAmount(CalculatePct(l_ActionTarget->GetMaxHealth(), l_Value ));
                                break;
                            default:
                                l_AuraEffect->SetAmount(CalculatePct(l_ActionTarget->GetMaxPower(Powers(p_SpellScript.DataStat)), l_Value ));
                                break;
                        }
                    }
                    break;
                }
                case eCalculationType::TypeAddPct:
                {
                    if (l_ActionTarget)
                    {
                        switch (p_SpellScript.DataStat)
                        {
                            case -1:
                                l_AuraEffect->SetAmount(CalculatePct(l_ActionTarget->GetMaxHealth(), 100 + l_Value));
                                break;
                            default:
                                l_AuraEffect->SetAmount(CalculatePct(l_ActionTarget->GetMaxPower(Powers(p_SpellScript.DataStat)), 100 + l_Value));
                                break;
                        }
                    }
                    break;
                }
                case eCalculationType::TypeRemovePct:
                {
                    if (l_ActionTarget)
                    {
                        switch (p_SpellScript.DataStat)
                        {
                            case -1:
                                l_AuraEffect->SetAmount(CalculatePct(l_ActionTarget->GetMaxHealth(), 100 - l_Value));
                                break;
                            default:
                                l_AuraEffect->SetAmount(CalculatePct(l_ActionTarget->GetMaxPower(Powers(p_SpellScript.DataStat)), 100 - l_Value));
                                break;
                        }
                    }
                    break;
                }
                case eCalculationType::TypeAddFlat:
                    l_AuraEffect->SetAmount(l_Value);
                    break;
                case eCalculationType::TypeRemoveFlat:
                    l_AuraEffect->SetAmount(-l_Value);
                    break;
                default:
                    break;
            }
            break;
        }
        case eSpellScriptActions::ModAuraDuration:
        {
            std::set<Aura*> l_AuraSet;

            if (p_SpellScript.TargetAuraType)
            {
                for (AuraEffect* l_AuraEffect : GetCaster()->GetAuraEffectsByType(AuraType(p_SpellScript.TargetAuraType)))
                {
                    if (Aura* l_Aura = l_AuraEffect->GetBase())
                        l_AuraSet.insert(l_Aura);
                }
            }
            else if (p_SpellScript.TargetSpellId)
            {
                if (Aura* l_Aura = GetCaster()->GetAura(p_SpellScript.TargetSpellId))
                    l_AuraSet.insert(l_Aura);
            }

            if (!l_AuraSet.size())
                return;

            for (Aura* l_Aura : l_AuraSet)
            {
                int32 l_Value = 0;
                if (p_SpellScript.ActionSpellId && p_SpellScript.DataEffectId != -1)
                {
                    switch (p_SpellScript.DataSource)
                    {
                        case eDataSources::SourceSpellInfo:
                        {
                            if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(p_SpellScript.ActionSpellId))
                                l_Value = l_SpellInfo->Effects[p_SpellScript.DataEffectId].BasePoints;
                            break;
                        }
                        case eDataSources::SourceAura:
                        {
                            if (l_ActionTarget)
                            {
                                if (AuraEffect* l_SourceEffect = l_ActionTarget->GetAuraEffect(p_SpellScript.ActionSpellId, p_SpellScript.DataEffectId))
                                    l_Value = l_SourceEffect->GetAmount();
                            }
                            break;
                        }
                        default:
                            break;
                    }
                }

                if (l_Value && p_SpellScript.ArtifactTraitId)
                {
                    if (l_ActionCaster)
                    {
                        if (Player* l_Player = l_ActionCaster->ToPlayer())
                            l_Value *= l_Player->GetRankOfArtifactPowerId(p_SpellScript.ArtifactTraitId);
                    }
                }

                if (!l_Value)
                    return;

                switch (p_SpellScript.CalculationType)
                {
                    case eCalculationType::TypeCalcPct:
                        l_Aura->SetDuration(CalculatePct(l_Aura->GetDuration(), l_Value));
                        break;
                    case eCalculationType::TypeAddPct:
                        l_Aura->SetDuration(CalculatePct(l_Aura->GetDuration(), 100 + l_Value));
                        break;
                    case eCalculationType::TypeRemovePct:
                        l_Aura->SetDuration(CalculatePct(l_Aura->GetDuration(), 100 - l_Value));
                        break;
                    case eCalculationType::TypeAddFlat:
                        l_Aura->SetDuration(l_Aura->GetDuration() + l_Value * IN_MILLISECONDS);
                        break;
                    case eCalculationType::TypeRemoveFlat:
                        l_Aura->SetDuration(l_Aura->GetDuration() + l_Value * IN_MILLISECONDS);
                        break;
                    default:
                        break;
                }
            }
            break;
        }
        case eSpellScriptActions::SetAuraDuration:
        {
            std::set<Aura*> l_AuraSet;

            if (p_SpellScript.TargetAuraType)
            {
                for (AuraEffect* l_AuraEffect : GetCaster()->GetAuraEffectsByType(AuraType(p_SpellScript.TargetAuraType)))
                {
                    if (Aura* l_Aura = l_AuraEffect->GetBase())
                        l_AuraSet.insert(l_Aura);
                }
            }
            else if (p_SpellScript.TargetSpellId)
            {
                if (Aura* l_Aura = GetCaster()->GetAura(p_SpellScript.TargetSpellId))
                    l_AuraSet.insert(l_Aura);
            }

            if (!l_AuraSet.size())
                return;

            for (Aura* l_Aura : l_AuraSet)
            {
                int32 l_Value = 0;
                if (p_SpellScript.ActionSpellId && p_SpellScript.DataEffectId != -1)
                {
                    switch (p_SpellScript.DataSource)
                    {
                        case eDataSources::SourceSpellInfo:
                        {
                            if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(p_SpellScript.ActionSpellId))
                                l_Value = l_SpellInfo->Effects[p_SpellScript.DataEffectId].BasePoints;
                            break;
                        }
                        case eDataSources::SourceAura:
                        {
                            if (l_ActionTarget)
                            {
                                if (AuraEffect* l_SourceEffect = l_ActionTarget->GetAuraEffect(p_SpellScript.ActionSpellId, p_SpellScript.DataEffectId))
                                    l_Value = l_SourceEffect->GetAmount();
                            }
                            break;
                        }
                        default:
                            break;
                    }
                }

                if (l_Value && p_SpellScript.ArtifactTraitId)
                {
                    if (l_ActionCaster)
                    {
                        if (Player* l_Player = l_ActionCaster->ToPlayer())
                            l_Value *= l_Player->GetRankOfArtifactPowerId(p_SpellScript.ArtifactTraitId);
                    }
                }

                if (!l_Value)
                    return;

                switch (p_SpellScript.CalculationType)
                {
                    case eCalculationType::TypeCalcPct:
                    {
                        if (l_ActionTarget)
                        {
                            switch (p_SpellScript.DataStat)
                            {
                                case -1:
                                    l_Aura->SetDuration((CalculatePct(l_ActionTarget->GetMaxHealth(), l_Value)) * IN_MILLISECONDS);
                                    break;
                                default:
                                    l_Aura->SetDuration((CalculatePct(l_ActionTarget->GetMaxPower(Powers(p_SpellScript.DataStat)), l_Value)) * IN_MILLISECONDS);
                                    break;
                            }
                        }
                        break;
                    }
                    case eCalculationType::TypeAddPct:
                    {
                        if (l_ActionTarget)
                        {
                            switch (p_SpellScript.DataStat)
                            {
                                case -1:
                                    l_Aura->SetDuration((CalculatePct(l_ActionTarget->GetMaxHealth(), 100 + l_Value)) * IN_MILLISECONDS);
                                    break;
                                default:
                                    l_Aura->SetDuration((CalculatePct(l_ActionTarget->GetMaxPower(Powers(p_SpellScript.DataStat)), 100 + l_Value)) * IN_MILLISECONDS);
                                    break;
                            }
                        }
                        break;
                    }
                    case eCalculationType::TypeRemovePct:
                    {
                        if (l_ActionTarget)
                        {
                            switch (p_SpellScript.DataStat)
                            {
                                case -1:
                                    l_Aura->SetDuration((CalculatePct(l_ActionTarget->GetMaxHealth(), 100 - l_Value)) * IN_MILLISECONDS);
                                    break;
                                default:
                                    l_Aura->SetDuration((CalculatePct(l_ActionTarget->GetMaxPower(Powers(p_SpellScript.DataStat)), 100 - l_Value)) * IN_MILLISECONDS);
                                    break;
                            }
                        }
                        break;
                    }
                    case eCalculationType::TypeAddFlat:
                        l_Aura->SetDuration(l_Value * IN_MILLISECONDS);
                        break;
                    case eCalculationType::TypeRemoveFlat:
                        l_Aura->SetDuration(-l_Value * IN_MILLISECONDS);
                        break;
                    default:
                        break;
                }
            }
            break;
        }
        default:
            break;
    }
}

Unit* Spell::GetScriptActor(SpellScriptEntry p_SpellScript, uint32 p_ScriptActor)
{
    switch (p_ScriptActor)
    {
        case eScriptActors::Caster:
            return GetCaster();
        case eScriptActors::Target:
            return m_targets.GetUnitTarget();
        case eScriptActors::HitUnit:
        {
            switch (p_SpellScript.Hook)
            {
                case eSpellHooks::AfterHit:
                case eSpellHooks::BeforeHit:
                case eSpellHooks::OnHit:
                case eSpellHooks::OnEffectHit:
                case eSpellHooks::OnEffectHitTarget:
                    return unitTarget;
                default:
                    return nullptr;
            }
            break;
        }
        default:
            break;
    }

    return nullptr;
}

bool Spell::IsAnEffectHook(SpellScriptEntry p_SpellScript)
{
    switch (p_SpellScript.Hook)
    {
        case eSpellHooks::OnEffectHit:
        case eSpellHooks::OnEffectHitTarget:
            return true;
        default:
            break;
    }

    return false;
}

uint32 Spell::GetNonInterruptableSpellBy(uint32 p_SpellId)
{
    switch (p_SpellId)
    {
        case 227344: ///< Effuse (Ancient Mistweaver Arts - 209520)
        case 227345: ///< Enveloping Mist (Ancient Mistweaver Arts - 209520)
            return 209525; ///< Soothing Mist (Ancient Mistweaver Arts - 209520)
        case 211412:
            return 211408;
        case 75: ///< Auto Shot
            return 120360; ///< Barrage
        case 196555: ///< Netherwalk
            return 211053; ///< Fel Barrage
        case 48792: ///< Icebound Fortitude
        case 194679: ///< Rune Tap
        case 48707: ///< Anti-Magic Shell
        case 55233: ///< Vampiric Blood
        case 49028: ///< Dancing Rune Weapon
        case 219809: ///< Tombstone
			return 206931; ///< Blooddrinker
        case 131347: ///< Glide
            return 211053; ///< Fel Barrage
        default:
            return 0;
    }

    return 0;
}

bool Spell::IsPowerFullAura(SpellInfo const* p_SpellProto, Unit* caster, Unit* target)
{
    const uint32 s_CCSpells[3] = { 6770, 118, 118699 }; ///< Sap/Polymorph/Fear
    for (uint32 s_SpellId : s_CCSpells)
    {
        if (p_SpellProto->Id == s_SpellId)
        {
            if (Aura* l_AuraSecond = target->GetAura(s_SpellId))
            {
                m_diminishGroup = GetDiminishingReturnsGroupForSpell(p_SpellProto, caster);
                m_diminishLevel = target->GetDiminishing(m_diminishGroup);
                DiminishingReturnsType type = GetDiminishingReturnsGroupType(m_diminishGroup);
                if ((type == DRTYPE_PLAYER &&
                    (target->GetCharmerOrOwnerPlayerOrPlayerItself() || (target->GetTypeId() == TYPEID_UNIT && target->ToCreature()->GetFlagsExtra() & CREATURE_FLAG_EXTRA_ALL_DIMINISH))) ||
                    type == DRTYPE_ALL)
                {
                    int32 s_SpellDuration = l_AuraSecond->GetMaxDuration();
                    int32 s_AuraDuration = l_AuraSecond->GetDuration();
                    int32 s_Limitduration = GetDiminishingReturnsLimitDuration(p_SpellProto, caster);
                    float s_DiminishMod = s_Limitduration ? target->ApplyDiminishingToDuration(m_diminishGroup, s_SpellDuration, caster, m_diminishLevel, s_Limitduration) : 1.0f;
                    if (s_DiminishMod != 0.0f && s_AuraDuration > s_SpellDuration)
                        return true;
                }
            }
        }
    }

    return false;
}

namespace JadeCore
{

WorldObjectSpellTargetCheck::WorldObjectSpellTargetCheck(Unit* caster, Unit* referer, SpellInfo const* spellInfo,
    SpellTargetCheckTypes selectionType, ConditionContainer* condList) : _caster(caster), _referer(referer), _spellInfo(spellInfo),
    _targetSelectionType(selectionType), _condList(condList)
{
    if (condList)
        _condSrcInfo = new ConditionSourceInfo(NULL, caster);
    else
        _condSrcInfo = NULL;
}

WorldObjectSpellTargetCheck::~WorldObjectSpellTargetCheck()
{
    if (_condSrcInfo)
        delete _condSrcInfo;
}

bool WorldObjectSpellTargetCheck::operator()(WorldObject* target)
{
    if (_spellInfo->CheckTarget(_caster, target, true) != SPELL_CAST_OK)
        return false;
    Unit* unitTarget = target->ToUnit();
    if (Corpse* corpseTarget = target->ToCorpse())
    {
        // use ofter for party/assistance checks
        if (Player* owner = ObjectAccessor::FindPlayer(corpseTarget->GetOwnerGUID()))
            unitTarget = owner;
        else
            return false;
    }
    if (unitTarget)
    {
        switch (_targetSelectionType)
        {
            case TARGET_CHECK_ENEMY:
                if (unitTarget->isTotem())
                    return false;
                if (unitTarget->GetAI() && unitTarget->GetAI()->IgnoreAoESpells())
                    return false;
                if (!_caster->_IsValidAttackTarget(unitTarget, _spellInfo))
                    return false;
                break;
            case TARGET_CHECK_SUMMON:
                if (unitTarget->isTotem())
                    return false;
                if (!_caster->_IsValidAssistTarget(unitTarget, _spellInfo))
                    return false;
                if (!unitTarget->IsOwnerOrSelf(_caster))
                    return false;
                break;
            case TARGET_CHECK_ALLY:
                if (unitTarget->isTotem())
                    return false;
                if (!_caster->_IsValidAssistTarget(unitTarget, _spellInfo, false))
                    return false;
                break;
            case TARGET_CHECK_PARTY:
                if (unitTarget->isTotem())
                    return false;
                if (!_caster->_IsValidAssistTarget(unitTarget, _spellInfo))
                    return false;
                if (!_referer->IsInPartyWith(unitTarget))
                    return false;
                break;
            case TARGET_CHECK_RAID_CLASS:
                if (_referer->getClass() != unitTarget->getClass())
                    return false;
                // nobreak;
            case TARGET_CHECK_RAID:
                if (unitTarget->isTotem())
                    return false;
                if (!_caster->_IsValidAssistTarget(unitTarget, _spellInfo))
                    return false;
                if (!_referer->IsInRaidWith(unitTarget) && !_referer->IsInPartyWith(unitTarget))
                    return false;
                break;
            case TARGET_CHECK_ALLY_OR_RAID:
                if (unitTarget->isTotem())
                    return false;
                if (!_caster->_IsValidAssistTarget(unitTarget, _spellInfo) &&
                    !_referer->IsInRaidWith(unitTarget) && !_referer->IsInPartyWith(unitTarget))
                    return false;
                break;
            default:
                break;
        }
    }
    if (!_condSrcInfo)
        return true;
    _condSrcInfo->mConditionTargets[0] = target;
    return sConditionMgr->IsObjectMeetToConditions(*_condSrcInfo, *_condList);
}

WorldObjectSpellNearbyTargetCheck::WorldObjectSpellNearbyTargetCheck(float range, Unit* caster, SpellInfo const* spellInfo,
    SpellTargetCheckTypes selectionType, ConditionContainer* condList)
    : WorldObjectSpellTargetCheck(caster, caster, spellInfo, selectionType, condList), _range(range), _position(caster)
{
}

bool WorldObjectSpellNearbyTargetCheck::operator()(WorldObject* target)
{
    float dist = target->GetDistance(*_position);
    if (dist < _range && WorldObjectSpellTargetCheck::operator ()(target) && _caster->GetGUID() != target->GetGUID())
    {
        _range = dist;
        return true;
    }
    return false;
}

WorldObjectSpellAreaTargetCheck::WorldObjectSpellAreaTargetCheck(float range, Position const* position, Unit* caster,
    Unit* referer, SpellInfo const* spellInfo, SpellTargetCheckTypes selectionType, ConditionContainer* condList, bool ignoreSearcherSize/* = false*/, bool p_IgnoreTargetSize/* = true*/)
    : WorldObjectSpellTargetCheck(caster, referer, spellInfo, selectionType, condList), _range(range), _position(position),
    _ignoreSearcherSize(ignoreSearcherSize), _ignoreTargetSize(p_IgnoreTargetSize)
{
}

bool WorldObjectSpellAreaTargetCheck::operator()(WorldObject* target)
{
    /// Use exact dist 3D for players
    if (target->IsPlayer())
    {
        if (!target->IsWithinDist3d(_position, _range, _ignoreTargetSize ? nullptr : _caster, _ignoreSearcherSize))
            return false;
    }
    /// Add ObjectSize to the spell radius for Creatures (mostly for huge bosses)
    else
    {
        if (!target->IsWithinDist3d(_position, _range, _ignoreTargetSize ? nullptr : _caster, false))
            return false;
    }

    return WorldObjectSpellTargetCheck::operator ()(target);
}

WorldObjectSpellConeTargetCheck::WorldObjectSpellConeTargetCheck(float coneAngle, float range, Unit* caster,
    SpellInfo const* spellInfo, SpellTargetCheckTypes selectionType, ConditionContainer* condList)
    : WorldObjectSpellAreaTargetCheck(range, caster, caster, caster, spellInfo, selectionType, condList), _coneAngle(coneAngle)
{
}

bool WorldObjectSpellConeTargetCheck::operator()(WorldObject* target)
{
    if (_spellInfo->AttributesCu & SPELL_ATTR0_CU_CONE_BACK)
    {
        if (!_caster->isInBack(target, std::fabs(_coneAngle)))
            return false;
    }
    else if (_spellInfo->AttributesCu & SPELL_ATTR0_CU_CONE_LINE)
    {
        if (!_caster->HasInLine(target, _caster->GetObjectSize()))
            return false;
    }
    else
    {
        if (!_caster->isInFront(target, _coneAngle))
        {
            if (_caster->GetTypeId() != TYPEID_PLAYER || _caster->GetDistance2d(target) > 3.0f || !_caster->isInFront(target, M_PI))
                return false;
        }
    }
    return WorldObjectSpellAreaTargetCheck::operator ()(target);
}

WorldObjectSpellWidthTargetCheck::WorldObjectSpellWidthTargetCheck(float p_Width, float p_Range, Unit* p_Caster, Unit* p_Referer,
    SpellInfo const* p_SpellInfo, SpellTargetCheckTypes p_SelectionType, ConditionContainer* p_CondList, Position p_Origin)
    : WorldObjectSpellAreaTargetCheck(p_Range, p_Caster, p_Caster, p_Referer, p_SpellInfo, p_SelectionType, p_CondList), m_Width(p_Width), m_Origin(p_Origin)
{
}

bool WorldObjectSpellWidthTargetCheck::operator()(WorldObject* p_Target)
{
    if (p_Target == nullptr)
        return false;

    if (!p_Target->IsPlayer() && _spellInfo->AttributesEx3 & SpellAttr3::SPELL_ATTR3_ONLY_TARGET_PLAYERS)
        return false;

    if (_spellInfo->Id == 192611)   ///< Fel Rush (damage)
    {
        if (!_caster->HasInLineCheckingStrafe(p_Target, m_Width))
            return false;
    }
    else if ((!!m_Origin && !m_Origin.HasInLine(p_Target, m_Width, false)) || (!m_Origin && !p_Target->IsInAxe(_caster, m_Width / 2.0f, 200.0f)))
        return false;

    return WorldObjectSpellAreaTargetCheck::operator()(p_Target);
}

WorldObjectSpellTrajTargetCheck::WorldObjectSpellTrajTargetCheck(float range, Position const* position, Unit* caster, SpellInfo const* spellInfo)
    : WorldObjectSpellAreaTargetCheck(range, position, caster, caster, spellInfo, TARGET_CHECK_DEFAULT, NULL)
{
}

bool WorldObjectSpellTrajTargetCheck::operator()(WorldObject* target)
{
    // return all targets on missile trajectory (0 - size of a missile)
    if (!_caster->HasInLine(target, 0))
        return false;
    return WorldObjectSpellAreaTargetCheck::operator ()(target);
}

} //namespace JadeCore
