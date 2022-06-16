////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "HostileRefManager.h"
#include "ThreatManager.h"
#include "Unit.h"
#include "SpellMgr.h"
#include "SpellInfo.h"

HostileRefManager::~HostileRefManager()
{
    deleteReferences();
}

//=================================================
// send threat to all my hateres for the victim
// The victim is hated than by them as well
// use for buffs and healing threat functionality

void HostileRefManager::threatAssist(Unit* victim, float baseThreat, SpellInfo const* threatSpell)
{
    if (getSize() == 0)
        return;

    std::lock_guard<std::mutex> l_Lock(m_ReferenceLock);

    HostileReference* ref = getFirst();
    float threat = ThreatCalcHelper::calcThreat(victim, iOwner, baseThreat, (threatSpell ? threatSpell->GetSchoolMask() : SPELL_SCHOOL_MASK_NORMAL), threatSpell);
    threat /= getSize();
    while (ref)
    {
        if (ThreatCalcHelper::isValidProcess(victim, ref->getSource()->getOwner(), threatSpell))
            ref->getSource()->doAddThreat(victim, threat);

        ref = ref->next();
    }
}

//=================================================

void HostileRefManager::addTempThreat(float threat, bool apply)
{
    std::lock_guard<std::mutex> l_Lock(m_ReferenceLock);

    HostileReference* ref = getFirst();

    while (ref)
    {
        if (apply)
        {
            if (ref->getTempThreatModifier() == 0.0f)
                ref->addTempThreat(threat);
        }
        else
            ref->resetTempThreat();

        ref = ref->next();
    }
}

//=================================================

void HostileRefManager::addThreatPercent(int32 percent)
{
    std::lock_guard<std::mutex> l_Lock(m_ReferenceLock);

    HostileReference* ref = getFirst();
    while (ref)
    {
        ref->addThreatPercent(percent);
        ref = ref->next();
    }
}

//=================================================
// The online / offline status is given to the method. The calculation has to be done before

void HostileRefManager::setOnlineOfflineState(bool isOnline)
{
    std::lock_guard<std::mutex> l_Lock(m_ReferenceLock);

    HostileReference* ref = getFirst();
    while (ref)
    {
        ref->setOnlineOfflineState(isOnline);
        ref = ref->next();
    }
}

//=================================================
// The online / offline status is calculated and set

void HostileRefManager::updateThreatTables()
{
    std::lock_guard<std::mutex> l_Lock(m_ReferenceLock);

    HostileReference* ref = getFirst();
    while (ref)
    {
        ref->updateOnlineStatus();
        ref = ref->next();
    }
}

//=================================================
// The references are not needed anymore
// tell the source to remove them from the list and free the mem

void HostileRefManager::deleteReferences()
{
    std::lock_guard<std::mutex> l_Lock(m_ReferenceLock);

    HostileReference* ref = getFirst();
    while (ref)
    {
        HostileReference* nextRef = ref->next();
        ref->removeReference();
        delete ref;
        ref = nextRef;
    }
}

//=================================================
// delete one reference, defined by faction

void HostileRefManager::deleteReferencesForFaction(uint32 faction)
{
    std::lock_guard<std::mutex> l_Lock(m_ReferenceLock);

    HostileReference* ref = getFirst();
    while (ref)
    {
        HostileReference* nextRef = ref->next();
        if (ref->getSource()->getOwner()->getFactionTemplateEntry()->Faction == faction)
        {
            ref->removeReference();
            delete ref;
        }
        ref = nextRef;
    }
}

//=================================================
// delete one reference, defined by Unit

void HostileRefManager::deleteReference(Unit* creature)
{
    std::lock_guard<std::mutex> l_Lock(m_ReferenceLock);

    HostileReference* ref = getFirst();
    while (ref)
    {
        HostileReference* nextRef = ref->next();
        if (ref->getSource()->getOwner() == creature)
        {
            ref->removeReference();
            delete ref;
            break;
        }
        ref = nextRef;
    }
}

//=================================================
// set state for one reference, defined by Unit

void HostileRefManager::setOnlineOfflineState(Unit* creature, bool isOnline)
{
    std::lock_guard<std::mutex> l_Lock(m_ReferenceLock);

    HostileReference* ref = getFirst();
    while (ref)
    {
        HostileReference* nextRef = ref->next();
        if (ref->getSource()->getOwner() == creature)
        {
            ref->setOnlineOfflineState(isOnline);
            break;
        }
        ref = nextRef;
    }
}

//=================================================

void HostileRefManager::UpdateVisibility()
{
    std::lock_guard<std::mutex> l_Lock(m_ReferenceLock);

    HostileReference* ref = getFirst();
    while (ref)
    {
        HostileReference* nextRef = ref->next();
        if (!ref->getSource()->getOwner()->canSeeOrDetect(getOwner()))
        {
            nextRef = ref->next();
            ref->removeReference();
            delete ref;
        }
        ref = nextRef;
    }
}

void HostileRefManager::Update()
{
    Player* l_Owner = getOwner()->ToPlayer();
    if (!l_Owner)
        return;

    std::lock_guard<std::mutex> l_Lock(m_ReferenceLock);

    HostileReference* l_Ref = getFirst();
    while (l_Ref)
    {
        HostileReference* l_NextRef = l_Ref->next();
        Unit* l_HostileUnit = l_Ref->getSource()->getOwner();

        bool l_Remove = false;

        /// The player isn't the main victim of the creature, more than 100 yards away from the creature and on a continent map
        if (l_HostileUnit->getVictim() != l_Owner && l_HostileUnit->GetMap() && l_HostileUnit->GetMap()->GetEntry()->IsContinent() && l_HostileUnit->GetDistance(l_Owner) > 70.0f && l_HostileUnit->ToCreature()  && !l_HostileUnit->ToCreature()->isWorldBoss())
        {
            auto l_Itr = l_Owner->m_LastDamageActionTime.find(l_HostileUnit->GetGUID());
            /// Player and the creature never deal damage to each others or the last time was more than 15 seconds ago
            if (l_Itr == l_Owner->m_LastDamageActionTime.end() || (l_Itr->second + 15) < time(nullptr))
                l_Remove = true;
        }

        /// Prevent bugs when a player is in non hostile creature threat list and the creature don't attack him so he stay in combat mode forever ...
        if (!l_HostileUnit->IsHostileToPlayers() && l_HostileUnit->getVictim() == nullptr)
        {
            auto l_Itr = l_Owner->m_LastDamageActionTime.find(l_HostileUnit->GetGUID());
            if (l_Itr == l_Owner->m_LastDamageActionTime.end() || (l_Itr->second + 15) < time(nullptr))
                l_Remove = true;
        }

        if (l_Remove)
        {
            l_NextRef = l_Ref->next();
            l_Ref->removeReference();
            delete l_Ref;

            /// Delete only one Hostile reference per update maximum (workaround to fix crash on "delete l_Ref;" line)
            break;
        }

        l_Ref = l_NextRef;
    }
}