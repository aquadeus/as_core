////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "SpellMgr.h"
#include "SpellInfo.h"
#include "ObjectMgr.h"
#include "SpellAuras.h"
#include "SpellAuraDefines.h"
#include "SharedDefines.h"
#include "World.h"
#include "Chat.h"
#include "Spell.h"
#include "BattlegroundMgr.hpp"
#include "CreatureAI.h"
#include "MapManager.h"
#include "BattlegroundIC.h"
#include "BattlefieldWG.h"
#include "BattlefieldMgr.h"
#include "InstanceScript.h"

bool IsPrimaryProfessionSkill(uint32 skill)
{
    SkillLineEntry const* pSkill = sSkillLineStore.LookupEntry(skill);
    return pSkill && pSkill->CategoryID == SKILL_CATEGORY_PROFESSION;
}

bool IsWeaponSkill(uint32 skill)
{
    SkillLineEntry const* pSkill = sSkillLineStore.LookupEntry(skill);
    return pSkill && pSkill->CategoryID == SKILL_CATEGORY_WEAPON;
}

bool IsPartOfSkillLine(uint32 skillId, uint32 spellId)
{
    SkillLineAbilityMapBounds skillBounds = sSpellMgr->GetSkillLineAbilityMapBounds(spellId);
    for (SkillLineAbilityMap::const_iterator itr = skillBounds.first; itr != skillBounds.second; ++itr)
        if (itr->second->SkillLine == skillId)
            return true;

    return false;
}

DiminishingGroup GetDiminishingReturnsGroupForSpell(SpellInfo const* spellproto, Unit* p_Caster)
{
    if (spellproto->IsPositive())
        return DIMINISHING_NONE;

    for (SpellEffectInfo const& effect : spellproto->Effects._effects)
        if (effect.ApplyAuraName == SPELL_AURA_MOD_TAUNT)
            return DIMINISHING_TAUNT;

    // Explicit Diminishing Groups
    switch (spellproto->SpellFamilyName)
    {
        case SPELLFAMILY_GENERIC:
        {
            switch (spellproto->Id)
            {
                case 77505:  ///< Earthquake knockback + stun
                case 214573: ///< Stuffed (Trilliax)
                case 213688: ///< Fel Cleave
                case 255029: ///< Sleep Canister - Imonar the Soulhunter
                case 257196: ///< Sleep Canister - Imonar the Soulhunter
                case 247565: ///< Slumber Gas - Imonar the Soulhunter
                case 247641: ///< Stasis Trap - Imonar the Soulhunter
                case 243977: ///< Torment of Frost - Varimathras
                case 245518: ///< Flash Freeze - The Covens of Shivarra
                case 253697: ///< Orb of Frost - The Covens of Shivarra
                case 256356: ///< Chilled Blood - The Covens of Shivarra
                    return DIMINISHING_NONE;
                case 118345: ///< Pulverize (Primal Earth Elemental Ability)
                case 118905: ///< static charge
                case 24394:  ///< Intimidation
                case 115001: ///< Remorseless Winter
                case 199085: ///< Warpath
                    return DIMINISHING_STUN;
                case 25046:  ///< Arcane Torrent
                case 28730:  ///< Arcane Torrent
                case 50613:  ///< Arcane Torrent
                case 69179:  ///< Arcane Torrent
                case 80483:  ///< Arcane Torrent
                case 129597: ///< Arcane Torrent
                case 155145: ///< Arcane Torrent
                case 202719: ///< Arcane Torrent
                case 222783: ///< Arcane Torrent
                case 232633: ///< Arcane Torrent (Priest)
                    return DIMINISHING_SILENCE;
                case 20549:  ///< War Stomp
                case 46026:  ///< War Stomp
                case 74606:  ///< War Stomp
                case 81500:  ///< War Stomp
                    return DIMINISHING_STUN;
                case 107079: ///< Quacking Palm
                case 191244: ///< Sticky Bomb
                    return DIMINISHING_INCAPACITATE;
                case 135373: ///< Entrapment
                    return DIMINISHING_ROOT;
                case 108199: ///< Gorefiend's Grasp
                    return DIMINISHING_AOE_KNOCKBACK;
                default:
                    break;
            }
            break;
        }
        case SPELLFAMILY_MAGE:
        {
            switch (spellproto->Id)
            {
                case 122: ///< Frost Nova
                case 33395: ///< Freeze
                case 157997: ///< Ice Nova
                case 198121: ///< FrostBite
                case 233582: ///< Entrenched in Flame (Warlock Honor Talent)
                    return DIMINISHING_ROOT;
                case 31661: ///< Dragon's Breath
                    return DIMINISHING_DISORIENT;
                case 82691: ///< Ring of Frost
                case 118: ///< Polymorph
                    return DIMINISHING_INCAPACITATE;
                default:
                    break;
            }
            ///< All variations of Polymorph.
            if (spellproto->Mechanic == MECHANIC_POLYMORPH)
                return DIMINISHING_INCAPACITATE;
            break;
        }
        case SPELLFAMILY_WARRIOR:
        {
            switch (spellproto->Id)
            {
                case 237744: ///< Warbringer
                    return DIMINISHING_NONE;
                case 5246: ///< Intimidating Shout
                    return DIMINISHING_DISORIENT;
                case 132168: ///< Shockwave
                case 132169: ///< Storm Bolt
                    return DIMINISHING_STUN;
                case 7922:
                case 1715:
                    return DIMINISHING_LIMITONLY;
                case 236077: ///< Disarm
                case 236236: ///< Disarm (Prot)
                    return DIMINISHING_DISARM;
                default:
                    break;
            }
            break;
        }
        case SPELLFAMILY_WARLOCK:
        {
            switch (spellproto->Id)
            {
                case 118699: ///< Fear
                case 130616: ///< Fear
                case 5484: ///< Howl of Terror
                    return DIMINISHING_DISORIENT;
                case 710: ///< Banish
                case 6789: ///< Mortal Coil
                    return DIMINISHING_INCAPACITATE;
                case 30283: ///< Shadowfury
                case 171017: ///< Meteor Strike (Infernal)
                case 171018: ///< Meteor Strike (Abyssal)
                    return DIMINISHING_STUN;
                case 170996: ///< Debilitate
                    return DIMINISHING_ROOT;
                case 43523: ///< Unstable Affliction Silence Effect
                case 196364: ///< Unstable Affliction Silence Effect 2
                case 22703: ///< Summon Infernal Stun Effect
                    return DIMINISHING_NONE;
                default:
                    break;
            }
            break;
        }
        case SPELLFAMILY_WARLOCK_PET:
        {
            switch (spellproto->Id)
            {
                case 6358: ///< Seduction
                case 115268: ///< Mesmerize
                    return DIMINISHING_DISORIENT;
                case 89766: ///< Axe Toss
                    return DIMINISHING_STUN;
                case 6360: ///< Fellash
                case 115770: ///< Whiplash
                    return DIMINISHING_AOE_KNOCKBACK;
                default:
                    break;
            }
            break;
        }
        case SPELLFAMILY_DRUID:
        {
            switch (spellproto->Id)
            {
                case 339: ///< Entangling Root
                case 170855: ///< Entangling Roots
                case 247564: ///< Entangling Roots
                case 201589: ///< Entangling Roots
                case 235963: ///< Entangling Roots
                case 102359: ///< Mass Entanglement
                case 45334: ///< Immobilized (Wild Charge)
                    return DIMINISHING_ROOT;
                case 163505: ///< Rake
                case 168877: ///< Maim
                case 168878: ///< Maim
                case 168879: ///< Maim
                case 168880: ///< Maim
                case 168881: ///< Maim
                case 203123: ///< Maim (Stun effect)
                case 5211: ///< Mighty Bash
                    return DIMINISHING_STUN;
                case 99: ///< Incapacitation Roar
                case 33786: ///< Cyclone
                case 209753: ///< Cyclone Honor Talent
                    return DIMINISHING_DISORIENT;
                case 81261: ///< Solar Beam
                    return DIMINISHING_SILENCE;
                case 61391: ///< Typhoon
                case 102793: ///< Ursol's Vortex
                case 118283: ///< Ursol's Vortex
                    return DIMINISHING_AOE_KNOCKBACK;
                default:
                    break;
            }
            break;
        }
        case SPELLFAMILY_ROGUE:
        {
            switch (spellproto->Id)
            {
                case 2094: ///< Blind
                    return DIMINISHING_DISORIENT;
                case 1776: ///< Gouge
                case 6770: ///< Sap
                    return DIMINISHING_INCAPACITATE;
                case 1330: ///< Garrote
                    return DIMINISHING_SILENCE;
                case 408: ///< Kidney Shot
                case 1833: ///< Cheap Shot
                case 199804: ///< Between The Eyes
                    return DIMINISHING_STUN;
                case 207777: ///< Dismantle
                    return DIMINISHING_DISARM;
                default:
                    break;
            }
            break;
        }
        case SPELLFAMILY_HUNTER:
        {
            switch (spellproto->Id)
            {
                case 191241: ///< Sticky Bomb
                    return DIMINISHING_AOE_KNOCKBACK;
                case 202933: ///< Spider Sting
                    return DIMINISHING_SILENCE;
                case 117526: ///< Binding Shot
                case 24394: ///< Intimidation
                case 53148: ///< Charge
                case 19577: ///< Intimidation
                    return DIMINISHING_STUN;
                case 209790: ///< Freezing Arrow
                case 19386: ///< Wyvern Sting
                case 3355: ///< Freezing trap
                case 203337: ///< Freezing Trap (Honor Talent)
                case 213691: ///< Scatter Shot
                    return DIMINISHING_INCAPACITATE;
                case 224729: ///< Bursting Shot
                    return DIMINISHING_DISORIENT;
                case 212638: ///< Trackers Net
                case 64803: ///< Entrapment
                case 136634: ///< Narrow Escape
                case 200108: ///< Ranger's Net
                case 162480: ///< Steel Trap Root Effect
                case 201158: ///< Super Sticky Tar Trap Effect
                case 190927: ///< Harpoon
                    return DIMINISHING_ROOT;
                default:
                    break;
            }
            break;
        }
        case SPELLFAMILY_PALADIN:
        {
            switch (spellproto->Id)
            {
                case 20066: ///< Repentance
                    return DIMINISHING_INCAPACITATE;
                case 115750: ///< Blinding Light
                case 105421: ///< Blinding Light Triggered
                    return DIMINISHING_DISORIENT;
                case 853: ///< Hammer Of Justice
                    return DIMINISHING_STUN;
                case 31935: ///< Avengers Shield
                case 217824: ///< Shield of Virtue
                    return DIMINISHING_SILENCE;
                default:
                    break;
            }
            break;
        }
        case SPELLFAMILY_SHAMAN:
        {
            switch (spellproto->Id)
            {
                case 51514: ///< Hex
                case 196942: ///< Voodoo Totem
                    return DIMINISHING_INCAPACITATE;
                case 204437: ///< Lightning Lasso
                case 118905: ///< Lightning Surge Totem
                    return DIMINISHING_STUN;
                case 64695: ///< Earthgrab Totem
                    return DIMINISHING_ROOT;
                case 51490: ///< Thunderstorm
                    return DIMINISHING_AOE_KNOCKBACK;
                case 204399: ///< Earth Shock (with Earthfury trait)
                    return DIMINISHING_NONE;
                default:
                    break;
            }
            /// All variations of Polymorph.
            if (spellproto->Mechanic == MECHANIC_POLYMORPH)
                return DIMINISHING_INCAPACITATE;
            break;
        }
        case SPELLFAMILY_DEATHKNIGHT:
        {
            switch (spellproto->Id)
            {
                case 47476: ///< Strangulate
                    return DIMINISHING_SILENCE;
                case 108194: ///< Asphyxiate
                case 221562: ///< Asphyxiate
                case 207171: ///< Winter is Coming
                case 91800: ///< Gnaw
                case 91797: ///< Monstrous Blow
                case 210141: ///< Zombie Explosion
                case 212332: ///< Smash
                    return DIMINISHING_STUN;
                case 108199: ///< GoreFriends Grasp
                    return DIMINISHING_AOE_KNOCKBACK;
                case 207167: ///< Blinding Sleet
                    return DIMINISHING_DISORIENT;
                case 204085: ///< Deathchill
                    return DIMINISHING_ROOT;
                default:
                    break;
            }
            break;
        }
        case SPELLFAMILY_PRIEST:
        {
            switch (spellproto->Id)
            {
                case 605: ///< Mind Control
                case 8122: ///< Psychic Scream
                    return DIMINISHING_DISORIENT;
                case 200200: ///< Censure
                case 226943: ///< Mind Bomb
                    return DIMINISHING_STUN;
                case 15487: ///< Silence
                case 199683: ///< Last Word
                    return DIMINISHING_SILENCE;
                case 200196: ///< HW: Chastise
                case 64044: ///< Psychic Horror
                case 9484: ///< Shackle Undead
                    return DIMINISHING_INCAPACITATE;
                case 204263: ///< Shining Force
                    return DIMINISHING_AOE_KNOCKBACK;
                case 114404: ///< Void Tendril
                    return DIMINISHING_ROOT;
                case 87204: ///< Sin and Punishment
                    return DIMINISHING_NONE;
                default:
                    break;
            }
            break;
        }
        case SPELLFAMILY_MONK:
        {
            switch (spellproto->Id)
            {
                case 116706: ///< Disable
                    return DIMINISHING_ROOT;
                case 115078: ///< Paralysis
                    return DIMINISHING_INCAPACITATE;
                case 120086: ///< Fists of Fury
                case 232055: ///< Fists of Fury
                case 119381: ///< Leg Sweep
                    return DIMINISHING_STUN;
                case 198909: ///< Song of Chi-Ji
                case 202274: ///< Incendiary Brew
                    return DIMINISHING_DISORIENT;
                case 233759: ///< Grapple Weapon
                    return DIMINISHING_DISARM;
                default:
                    break;
            }
            break;
        }
        case SPELLFAMILY_DEMONHUNTER:
        {
            switch (spellproto->Id)
            {
                case 179057: ///< Chaos Nova
                case 211881: ///< Fel Eruption
                case 205630: ///< Illidan's Grasp, primary effect
                case 208618: ///< Illidan's Grasp, secondary effect
                    return DIMINISHING_STUN;
                case 217832: ///< Imprison
                case 221527: ///< Imprison (Honor Talent)
                case 207685: ///< Sigil of Misery
                    return DIMINISHING_INCAPACITATE;
                case 204490: ///< Sigil of Silence
                    return DIMINISHING_SILENCE;
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }

    /// Lastly - Set diminishing depending on mechanic
    uint64 mechanic = spellproto->GetAllEffectsMechanicMask();

    if (mechanic & ((1LL << MECHANIC_SAPPED) | (1LL << MECHANIC_HORROR) | (1LL << MECHANIC_BANISH) | (1LL << MECHANIC_DISORIENTED) | (1LL << MECHANIC_POLYMORPH) | (1LL << MECHANIC_SHACKLE) | (1LL << MECHANIC_SLEEP) | (1LL << MECHANIC_CHARM) | (1LL << MECHANIC_FEAR)))
        return DIMINISHING_DISORIENT;

    /// Mechanic Knockout, except Blast Wave
    if (mechanic & (1LL << MECHANIC_KNOCKOUT) && spellproto->IconFileDataId != 135903)
        return DIMINISHING_DISORIENT;

    if (mechanic & (1LL << MECHANIC_SNARE))
        return DIMINISHING_LIMITONLY;

    if ((mechanic & (1LL << MECHANIC_STUN)) || (mechanic & (1LL << MECHANIC_FREEZE)))
        return DIMINISHING_STUN;

    if (mechanic & (1LL << MECHANIC_ROOT))
        return DIMINISHING_ROOT;

    if (mechanic & (1LL << MECHANIC_DISARM))
        return DIMINISHING_DISARM;

    return DIMINISHING_NONE;
}

DiminishingReturnsType GetDiminishingReturnsGroupType(DiminishingGroup group)
{
    switch (group)
    {
        case DIMINISHING_TAUNT:
        case DIMINISHING_STUN:
        case DIMINISHING_AOE_KNOCKBACK:
            return DRTYPE_ALL;
        case DIMINISHING_LIMITONLY:
        case DIMINISHING_NONE:
            return DRTYPE_NONE;
        default:
            return DRTYPE_PLAYER;
    }
}

DiminishingLevels GetDiminishingReturnsMaxLevel(DiminishingGroup group)
{
    switch (group)
    {
        case DIMINISHING_TAUNT:
            return DIMINISHING_LEVEL_TAUNT_IMMUNE;
        case DIMINISHING_AOE_KNOCKBACK:
            return DIMINISHING_LEVEL_2;
        default:
            return DIMINISHING_LEVEL_IMMUNE;
    }
}

int32 GetDiminishingReturnsLimitDuration(SpellInfo const* spellproto, Unit* p_OriginalCaster)
{
    // Explicit diminishing duration
    switch (spellproto->SpellFamilyName)
    {
        case SPELLFAMILY_GENERIC:
            return 8 * IN_MILLISECONDS;
        case SPELLFAMILY_WARRIOR:
        {
            /// Hamstring - 12 seconds in PvP
            if (spellproto->Id == 1715)
                return 12 * IN_MILLISECONDS;
            break;
        }
        case SPELLFAMILY_DRUID:
        {
            /// Faerie Swarm - 8 seconds in PvP
            if (spellproto->SpellFamilyFlags[0] & 0x100)
                return 8 * IN_MILLISECONDS;
            /// Faerie Fire - 20 seconds in PvP (6.0)
            if (spellproto->SpellFamilyFlags[0] & 0x400)
                return 20 * IN_MILLISECONDS;
            break;
        }
        case SPELLFAMILY_HUNTER:
        {
            // Hunter's Mark - 30 seconds in PvP (6.0)
            if (spellproto->Id == 1130)
                return 30 * IN_MILLISECONDS;
            /// Wyvern Sting - 6 seconds in PvP
            if (spellproto->SpellFamilyFlags[1] & 0x1000 ||
                spellproto->Id == 162480) ///< Steel Trap (Root Effect) - 6 seconds in PvP (7.0.3)
                return 6 * IN_MILLISECONDS;
            /// Binding Shot 3 seconds in PVP
            if (spellproto->Id == 117526)
                return 3 * IN_MILLISECONDS;
            break;
        }
        case SPELLFAMILY_MONK:
        {
            // Paralysis - 4 seconds in PvP regardless of if they are facing you (6.0)
            if (spellproto->SpellFamilyFlags[2] & 0x800000)
                return 4 * IN_MILLISECONDS;
            /// Song of Chi-Ji - 6 seconds in PvP
            if (spellproto->Id == 198909)
                return 6 * IN_MILLISECONDS;
            break;
        }
        case SPELLFAMILY_PRIEST:
        {
            /// Psychic Scream - 6 seconds in PvP
            if (spellproto->Id == 8122)
                return 6 * IN_MILLISECONDS;
            break;
        }
        case SPELLFAMILY_WARLOCK:
        {
            /// Howl of Terror - 6 seconds in PvP (6.0)
            if (spellproto->Id == 5484)
                return 6 * IN_MILLISECONDS;
            /// Fear - 6 seconds in PvP (6.0)
            if (spellproto->Id == 118699 || spellproto->Id == 130616)
                return 6 * IN_MILLISECONDS;
            /// Debilitate - 4 seconds in PvP
            if (spellproto->Id == 170996)
                return 4 * IN_MILLISECONDS;
            break;
        }
        case SPELLFAMILY_ROGUE:
        {
            /// Parley
            if (spellproto->Id == 199743)
                return 6 * IN_MILLISECONDS;
            /// Kidney Shot
            if (spellproto->Id == 408)
                return   p_OriginalCaster->HasAura(193531) ? 7 * IN_MILLISECONDS : 6 * IN_MILLISECONDS; /// < Deeperstratagem
            ///Between The Eyes
            if (spellproto->Id == 199804)
                return 6 * IN_MILLISECONDS;
            break;
        }
        case SPELLFAMILY_DEMONHUNTER:
        {
            /// Imprison
            if (spellproto->Id == 221527)
                return 4 * IN_MILLISECONDS;
            /// Imprison
            if (spellproto->Id == 217832)
                return 3 * IN_MILLISECONDS;
            break;
        }
        case SPELLFAMILY_DEATHKNIGHT:
        {
            /// Tremble Before Me
            if (spellproto->Id == 206961)
                return 3 * IN_MILLISECONDS;
            break;
        }
        case SPELLFAMILY_MAGE:
        {
            /// Dragon's Breath - 3 seconds in PvP
            if (spellproto->Id == 31661)
                return 3 * IN_MILLISECONDS;
            break;
        }
        default:
            break;
    }

    return 8 * IN_MILLISECONDS;
}

SpellMgr::SpellMgr()
{
}

SpellMgr::~SpellMgr()
{
    UnloadSpellInfoStore();
}

/// Some checks for spells, to prevent adding deprecated/broken spells for trainers, spell book, etc
bool SpellMgr::IsSpellValid(SpellInfo const* spellInfo, Player* player, bool msg)
{
    // not exist
    if (!spellInfo)
        return false;

    bool need_check_reagents = false;

    // check effects
    for (uint8 i = 0; i < spellInfo->EffectCount; ++i)
    {
        switch (spellInfo->Effects[i].Effect)
        {
        case 0:
            continue;

            // craft spell for crafting non-existed item (break client recipes list show)
        case SPELL_EFFECT_CREATE_ITEM:
        case SPELL_EFFECT_CREATE_LOOT:
            {
                if (spellInfo->Effects[i].ItemType == 0)
                {
                    // skip auto-loot crafting spells, its not need explicit item info (but have special fake items sometime)
                    if (!spellInfo->IsLootCrafting())
                    {
                        if (msg)
                        {
                            if (player)
                                ChatHandler(player).PSendSysMessage("Craft spell %u not have create item entry.", spellInfo->Id);
                            else
                                sLog->outError(LOG_FILTER_SQL, "Craft spell %u not have create item entry.", spellInfo->Id);
                        }
                        return false;
                    }

                }
                // also possible IsLootCrafting case but fake item must exist anyway
                else if (!sObjectMgr->GetItemTemplate(spellInfo->Effects[i].ItemType))
                {
                    if (msg)
                    {
                        if (player)
                            ChatHandler(player).PSendSysMessage("Craft spell %u create not-exist in DB item (Entry: %u) and then...", spellInfo->Id, spellInfo->Effects[i].ItemType);
                        else
                            sLog->outError(LOG_FILTER_SQL, "Craft spell %u create not-exist in DB item (Entry: %u) and then...", spellInfo->Id, spellInfo->Effects[i].ItemType);
                    }
                    return false;
                }

                need_check_reagents = true;
                break;
            }
        case SPELL_EFFECT_LEARN_SPELL:
            {
                SpellInfo const* spellInfo2 = sSpellMgr->GetSpellInfo(spellInfo->Effects[i].TriggerSpell);
                if (!IsSpellValid(spellInfo2, player, msg))
                {
                    if (msg)
                    {
                        if (player)
                            ChatHandler(player).PSendSysMessage("Spell %u learn to broken spell %u, and then...", spellInfo->Id, spellInfo->Effects[i].TriggerSpell);
                        else
                            sLog->outError(LOG_FILTER_SQL, "Spell %u learn to invalid spell %u, and then...", spellInfo->Id, spellInfo->Effects[i].TriggerSpell);
                    }
                    return false;
                }
                break;
            }
        }
    }

    if (need_check_reagents)
    {
        for (uint8 j = 0; j < MAX_SPELL_REAGENTS; ++j)
        {
            if (spellInfo->Reagent[j] > 0 && !sObjectMgr->GetItemTemplate(spellInfo->Reagent[j]))
            {
                if (msg)
                {
                    if (player)
                        ChatHandler(player).PSendSysMessage("Craft spell %u have not-exist reagent in DB item (Entry: %u) and then...", spellInfo->Id, spellInfo->Reagent[j]);
                    else
                        sLog->outError(LOG_FILTER_SQL, "Craft spell %u have not-exist reagent in DB item (Entry: %u) and then...", spellInfo->Id, spellInfo->Reagent[j]);
                }
                return false;
            }
        }
    }

    return true;
}

bool SpellMgr::IsSpellForbidden(uint32 spellid)
{
    std::list<uint32>::iterator Itr;

    for (Itr = mForbiddenSpells.begin(); Itr != mForbiddenSpells.end(); Itr++)
        if ((*Itr) == spellid)
            return true;

    return false;
}

uint32 SpellMgr::GetSpellDifficultyId(uint32 spellId) const
{
    SpellDifficultySearcherMap::const_iterator i = mSpellDifficultySearcherMap.find(spellId);
    return i == mSpellDifficultySearcherMap.end() ? 0 : (*i).second;
}

void SpellMgr::SetSpellDifficultyId(uint32 spellId, uint32 id)
{
    mSpellDifficultySearcherMap[spellId] = id;
}

SpellInfo const* SpellMgr::GetSpellForDifficulty(uint32 p_SpellId, Difficulty p_Difficulty) const
{
    return GetSpellInfo(p_SpellId, p_Difficulty);
}

SpellInfo const* SpellMgr::GetSpellForDifficultyFromSpell(SpellInfo const* spell, Unit const* caster) const
{
    if (!spell)
        return nullptr;

    if (!caster || !caster->GetMap() || !caster->GetMap()->IsDungeon())
        return spell;

    uint32 mode = uint32(caster->GetMap()->GetSpawnMode());
    return GetSpellInfo(spell->Id, (Difficulty)mode);
}

SpellChainNode const* SpellMgr::GetSpellChainNode(uint32 spell_id) const
{
    SpellChainMap::const_iterator itr = mSpellChains.find(spell_id);
    if (itr == mSpellChains.end())
        return NULL;

    return &itr->second;
}

uint32 SpellMgr::GetFirstSpellInChain(uint32 spell_id) const
{
    if (SpellChainNode const* node = GetSpellChainNode(spell_id))
        return node->first->Id;

    return spell_id;
}

uint32 SpellMgr::GetLastSpellInChain(uint32 spell_id) const
{
    if (SpellChainNode const* node = GetSpellChainNode(spell_id))
        return node->last->Id;

    return spell_id;
}

uint32 SpellMgr::GetNextSpellInChain(uint32 spell_id) const
{
    if (SpellChainNode const* node = GetSpellChainNode(spell_id))
        if (node->next)
            return node->next->Id;

    return 0;
}

uint32 SpellMgr::GetPrevSpellInChain(uint32 spell_id) const
{
    if (SpellChainNode const* node = GetSpellChainNode(spell_id))
        if (node->prev)
            return node->prev->Id;

    return 0;
}

uint8 SpellMgr::GetSpellRank(uint32 spell_id) const
{
    if (SpellChainNode const* node = GetSpellChainNode(spell_id))
        return node->rank;

    return 0;
}

uint32 SpellMgr::GetSpellWithRank(uint32 spell_id, uint32 rank, bool strict) const
{
    if (SpellChainNode const* node = GetSpellChainNode(spell_id))
    {
        if (rank != node->rank)
            return GetSpellWithRank(node->rank < rank ? node->next->Id : node->prev->Id, rank, strict);
    }
    else if (strict && rank > 1)
        return 0;
    return spell_id;
}

SpellRequiredMapBounds SpellMgr::GetSpellsRequiredForSpellBounds(uint32 spell_id) const
{
    return SpellRequiredMapBounds(mSpellReq.lower_bound(spell_id), mSpellReq.upper_bound(spell_id));
}

SpellsRequiringSpellMapBounds SpellMgr::GetSpellsRequiringSpellBounds(uint32 spell_id) const
{
    return SpellsRequiringSpellMapBounds(mSpellsReqSpell.lower_bound(spell_id), mSpellsReqSpell.upper_bound(spell_id));
}

bool SpellMgr::IsSpellRequiringSpell(uint32 spellid, uint32 req_spellid) const
{
    SpellsRequiringSpellMapBounds spellsRequiringSpell = GetSpellsRequiringSpellBounds(req_spellid);
    for (SpellsRequiringSpellMap::const_iterator itr = spellsRequiringSpell.first; itr != spellsRequiringSpell.second; ++itr)
    {
        if (itr->second == spellid)
            return true;
    }
    return false;
}

const SpellsRequiringSpellMap SpellMgr::GetSpellsRequiringSpell()
{
    return this->mSpellsReqSpell;
}

uint32 SpellMgr::GetSpellRequired(uint32 spell_id) const
{
    SpellRequiredMap::const_iterator itr = mSpellReq.find(spell_id);

    if (itr == mSpellReq.end())
        return 0;

    return itr->second;
}

SpellLearnSkillNode const* SpellMgr::GetSpellLearnSkill(uint32 spell_id) const
{
    SpellLearnSkillMap::const_iterator itr = mSpellLearnSkills.find(spell_id);
    if (itr != mSpellLearnSkills.end())
        return &itr->second;
    else
        return NULL;
}

SpellLearnSpellMapBounds SpellMgr::GetSpellLearnSpellMapBounds(uint32 spell_id) const
{
    return SpellLearnSpellMapBounds(mSpellLearnSpells.lower_bound(spell_id), mSpellLearnSpells.upper_bound(spell_id));
}

bool SpellMgr::IsSpellLearnSpell(uint32 spell_id) const
{
    return mSpellLearnSpells.find(spell_id) != mSpellLearnSpells.end();
}

bool SpellMgr::IsSpellLearnToSpell(uint32 spell_id1, uint32 spell_id2) const
{
    SpellLearnSpellMapBounds bounds = GetSpellLearnSpellMapBounds(spell_id1);
    for (SpellLearnSpellMap::const_iterator i = bounds.first; i != bounds.second; ++i)
        if (i->second.spell == spell_id2)
            return true;
    return false;
}

SpellTargetPosition const* SpellMgr::GetSpellTargetPosition(uint32 spell_id, SpellEffIndex effIndex) const
{
    SpellTargetPositionMap::const_iterator itr = mSpellTargetPositions.find(std::make_pair(spell_id, effIndex));
    if (itr != mSpellTargetPositions.end())
        return &itr->second;
    return NULL;
}

SpellSpellGroupMapBounds SpellMgr::GetSpellSpellGroupMapBounds(uint32 spell_id) const
{
    spell_id = GetFirstSpellInChain(spell_id);
    return SpellSpellGroupMapBounds(mSpellSpellGroup.lower_bound(spell_id), mSpellSpellGroup.upper_bound(spell_id));
}

uint32 SpellMgr::IsSpellMemberOfSpellGroup(uint32 spellid, SpellGroup groupid) const
{
    SpellSpellGroupMapBounds spellGroup = GetSpellSpellGroupMapBounds(spellid);
    for (SpellSpellGroupMap::const_iterator itr = spellGroup.first; itr != spellGroup.second; ++itr)
    {
        if (itr->second == groupid)
            return true;
    }
    return false;
}

SpellGroupSpellMapBounds SpellMgr::GetSpellGroupSpellMapBounds(SpellGroup group_id) const
{
    return SpellGroupSpellMapBounds(mSpellGroupSpell.lower_bound(group_id), mSpellGroupSpell.upper_bound(group_id));
}

void SpellMgr::GetSetOfSpellsInSpellGroup(SpellGroup group_id, std::set<uint32>& foundSpells) const
{
    std::set<SpellGroup> usedGroups;
    GetSetOfSpellsInSpellGroup(group_id, foundSpells, usedGroups);
}

void SpellMgr::GetSetOfSpellsInSpellGroup(SpellGroup group_id, std::set<uint32>& foundSpells, std::set<SpellGroup>& usedGroups) const
{
    if (usedGroups.find(group_id) != usedGroups.end())
        return;
    usedGroups.insert(group_id);

    SpellGroupSpellMapBounds groupSpell = GetSpellGroupSpellMapBounds(group_id);
    for (SpellGroupSpellMap::const_iterator itr = groupSpell.first; itr != groupSpell.second; ++itr)
    {
        if (itr->second < 0)
        {
            SpellGroup currGroup = (SpellGroup)abs(itr->second);
            GetSetOfSpellsInSpellGroup(currGroup, foundSpells, usedGroups);
        }
        else
        {
            foundSpells.insert(itr->second);
        }
    }
}

bool SpellMgr::AddSameEffectStackRuleSpellGroups(SpellInfo const* spellInfo, int32 amount, std::map<SpellGroup, int32>& groups) const
{
    uint32 spellId = spellInfo->GetFirstRankSpell()->Id;
    SpellSpellGroupMapBounds spellGroup = GetSpellSpellGroupMapBounds(spellId);
    // Find group with SPELL_GROUP_STACK_RULE_EXCLUSIVE_SAME_EFFECT if it belongs to one
    for (SpellSpellGroupMap::const_iterator itr = spellGroup.first; itr != spellGroup.second; ++itr)
    {
        SpellGroup group = itr->second;
        SpellGroupStackMap::const_iterator found = mSpellGroupStack.find(group);
        if (found != mSpellGroupStack.end())
        {
            if (found->second == SPELL_GROUP_STACK_RULE_EXCLUSIVE_SAME_EFFECT)
            {
                // Put the highest amount in the map
                if (groups.find(group) == groups.end())
                    groups[group] = amount;
                else
                {
                    int32 curr_amount = groups[group];
                    // Take absolute value because this also counts for the highest negative aura
                    if (abs(curr_amount) < abs(amount))
                        groups[group] = amount;
                }
                // return because a spell should be in only one SPELL_GROUP_STACK_RULE_EXCLUSIVE_SAME_EFFECT group
                return true;
            }
        }
    }
    // Not in a SPELL_GROUP_STACK_RULE_EXCLUSIVE_SAME_EFFECT group, so return false
    return false;
}

SpellGroupStackRule SpellMgr::CheckSpellGroupStackRules(SpellInfo const* spellInfo1, SpellInfo const* spellInfo2) const
{
    uint32 spellid_1 = spellInfo1->GetFirstRankSpell()->Id;
    uint32 spellid_2 = spellInfo2->GetFirstRankSpell()->Id;
    if (spellid_1 == spellid_2)
        return SPELL_GROUP_STACK_RULE_DEFAULT;
    // find SpellGroups which are common for both spells
    SpellSpellGroupMapBounds spellGroup1 = GetSpellSpellGroupMapBounds(spellid_1);
    std::set<SpellGroup> groups;
    for (SpellSpellGroupMap::const_iterator itr = spellGroup1.first; itr != spellGroup1.second; ++itr)
    {
        if (IsSpellMemberOfSpellGroup(spellid_2, itr->second))
        {
            bool add = true;
            SpellGroupSpellMapBounds groupSpell = GetSpellGroupSpellMapBounds(itr->second);
            for (SpellGroupSpellMap::const_iterator itr2 = groupSpell.first; itr2 != groupSpell.second; ++itr2)
            {
                if (itr2->second < 0)
                {
                    SpellGroup currGroup = (SpellGroup)abs(itr2->second);
                    if (IsSpellMemberOfSpellGroup(spellid_1, currGroup) && IsSpellMemberOfSpellGroup(spellid_2, currGroup))
                    {
                        add = false;
                        break;
                    }
                }
            }
            if (add)
                groups.insert(itr->second);
        }
    }

    SpellGroupStackRule rule = SPELL_GROUP_STACK_RULE_DEFAULT;

    for (std::set<SpellGroup>::iterator itr = groups.begin(); itr!= groups.end(); ++itr)
    {
        SpellGroupStackMap::const_iterator found = mSpellGroupStack.find(*itr);
        if (found != mSpellGroupStack.end())
            rule = found->second;
        if (rule)
            break;
    }
    return rule;
}

SpellProcEventEntry const* SpellMgr::GetSpellProcEvent(uint32 spellId) const
{
    return mSpellProcEventMap[spellId];
}

bool SpellMgr::IsSpellProcEventCanTriggeredBy(SpellProcEventEntry const* spellProcEvent, uint32 EventProcFlag, SpellInfo const* procSpell, uint32 procFlags, uint32 procExtra, bool active, SpellInfo const* AuraInfo)
{
    // No extra req need
    uint32 procEvent_procEx = PROC_EX_NONE;

    // check prockFlags for condition
    if ((procFlags & EventProcFlag) == 0)
        return false;

    bool hasFamilyMask = false;

    /* Check Periodic Auras

    *Dots can trigger if spell has no PROC_FLAG_SUCCESSFUL_NEGATIVE_MAGIC_SPELL
    nor PROC_FLAG_TAKEN_POSITIVE_MAGIC_SPELL

    *Only Hots can trigger if spell has PROC_FLAG_TAKEN_POSITIVE_MAGIC_SPELL

    *Only dots can trigger if spell has both positivity flags or PROC_FLAG_SUCCESSFUL_NEGATIVE_MAGIC_SPELL

    *Aura has to have PROC_FLAG_TAKEN_POSITIVE_MAGIC_SPELL or spellfamily specified to trigger from Hot

    */

    if (procFlags & PROC_FLAG_DONE_PERIODIC)
    {
        if (EventProcFlag & PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_NEG)
        {
            if (!(procExtra & PROC_EX_INTERNAL_DOT))
                return false;
        }
        else if (procExtra & PROC_EX_INTERNAL_HOT)
            procExtra |= PROC_EX_INTERNAL_REQ_FAMILY;
        else if (EventProcFlag & PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS)
            return false;
    }

    if (procFlags & PROC_FLAG_TAKEN_PERIODIC)
    {
        if (EventProcFlag & PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_POS)
        {
            if (!(procExtra & PROC_EX_INTERNAL_DOT))
                return false;
        }
        else if (procExtra & PROC_EX_INTERNAL_HOT)
            procExtra |= PROC_EX_INTERNAL_REQ_FAMILY;
        else if (EventProcFlag & PROC_FLAG_TAKEN_SPELL_NONE_DMG_CLASS_POS)
            return false;
    }

    // Trap casts are active by default
    if (procFlags & PROC_FLAG_DONE_TRAP_ACTIVATION)
        active = true;

    // Always trigger for this
    if (procFlags & (PROC_FLAG_KILLED | PROC_FLAG_KILL | PROC_FLAG_DEATH))
        return true;

    if (spellProcEvent)     // Exist event data
    {
        // Store extra req
        procEvent_procEx = spellProcEvent->procEx;
        if (!active)
            active = spellProcEvent->Active;

        // For melee triggers
        if (procSpell == NULL)
        {
            // Check (if set) for school (melee attack have Normal school)
            if (spellProcEvent->schoolMask && (spellProcEvent->schoolMask & SPELL_SCHOOL_MASK_NORMAL) == 0)
                return false;
        }
        else // For spells need check school/spell family/family mask
        {
            // Check (if set) for school
            if (spellProcEvent->schoolMask && (spellProcEvent->schoolMask & procSpell->SchoolMask) == 0)
                return false;

            // Check (if set) for spellFamilyName
            if (spellProcEvent->spellFamilyName && (spellProcEvent->spellFamilyName != procSpell->SpellFamilyName))
                return false;

            // spellFamilyName is Ok need check for spellFamilyMask if present
            if (spellProcEvent->spellFamilyMask)
            {
                if (!(spellProcEvent->spellFamilyMask & procSpell->SpellFamilyFlags))
                    return false;
                hasFamilyMask = true;
                // Some spells are not considered as active even with have spellfamilyflags
                if (!(procEvent_procEx & PROC_EX_ONLY_ACTIVE_SPELL))
                    active = true;
            }
        }
    }

    if (procExtra & (PROC_EX_INTERNAL_REQ_FAMILY))
    {
        if (!hasFamilyMask && (!AuraInfo || AuraInfo->Id != 195330)) ///< Hack fix for Honor talent "Defender of the week" shared by all healers
            return false;
    }

    // Check for extra req (if none) and hit/crit
    if (procEvent_procEx == PROC_EX_NONE)
    {
        // No extra req, so can trigger only for hit/crit/absorb - spell has to be active
        if ((procExtra & (PROC_EX_NORMAL_HIT | PROC_EX_CRITICAL_HIT | PROC_EX_ABSORB)) && active)
            return true;
    }
    else // Passive spells hits here only if resist/reflect/immune/evade
    {
        if (procExtra & AURA_SPELL_PROC_EX_MASK)
        {
            // if spell marked as procing only from not active spells
            if (active && procEvent_procEx & PROC_EX_NOT_ACTIVE_SPELL)
                return false;
            // if spell marked as procing only from active spells
            if (!active && procEvent_procEx & PROC_EX_ONLY_ACTIVE_SPELL)
                return false;
            // Exist req for PROC_EX_EX_TRIGGER_ALWAYS
            if (procEvent_procEx & PROC_EX_EX_TRIGGER_ALWAYS)
                return true;
            // PROC_EX_NOT_ACTIVE_SPELL and PROC_EX_ONLY_ACTIVE_SPELL flags handle: if passed checks before
            if ((procExtra & (PROC_EX_NORMAL_HIT|PROC_EX_CRITICAL_HIT|PROC_EX_ABSORB)) && ((procEvent_procEx & (AURA_SPELL_PROC_EX_MASK)) == 0))
                return true;
        }
        // Check Extra Requirement like (hit/crit/miss/resist/parry/dodge/block/immune/reflect/absorb and other)
        if (procEvent_procEx & procExtra)
            return true;
    }
    return false;
}

SpellProcEntry const* SpellMgr::GetSpellProcEntry(uint32 spellId) const
{
    SpellProcMap::const_iterator itr = mSpellProcMap.find(spellId);
    if (itr != mSpellProcMap.end())
        return &itr->second;
    return NULL;
}

bool SpellMgr::CanSpellTriggerProcOnEvent(SpellProcEntry const& procEntry, ProcEventInfo& eventInfo)
{
    // proc type doesn't match
    if (!(eventInfo.GetTypeMask() & procEntry.typeMask))
        return false;

    // check XP or honor target requirement
    if (procEntry.attributesMask & PROC_ATTR_REQ_EXP_OR_HONOR)
        if (Player* actor = eventInfo.GetActor()->ToPlayer())
            if (eventInfo.GetActionTarget() && !actor->isHonorOrXPTarget(eventInfo.GetActionTarget()))
                return false;

    // always trigger for these types
    if (eventInfo.GetTypeMask() & (PROC_FLAG_KILLED | PROC_FLAG_KILL | PROC_FLAG_DEATH))
        return true;

    // check school mask (if set) for other trigger types
    if (procEntry.schoolMask && !(eventInfo.GetSchoolMask() & procEntry.schoolMask))
        return false;

    // check spell family name/flags (if set) for spells
    if (eventInfo.GetTypeMask() & (PERIODIC_PROC_FLAG_MASK | SPELL_PROC_FLAG_MASK | PROC_FLAG_DONE_TRAP_ACTIVATION))
    {
        if (procEntry.spellFamilyName && (procEntry.spellFamilyName != eventInfo.GetSpellInfo()->SpellFamilyName))
            return false;

        if (procEntry.spellFamilyMask && !(procEntry.spellFamilyMask & eventInfo.GetSpellInfo()->SpellFamilyFlags))
            return false;
    }

    // check spell type mask (if set)
    if (eventInfo.GetTypeMask() & (SPELL_PROC_FLAG_MASK | PERIODIC_PROC_FLAG_MASK))
    {
        if (procEntry.spellTypeMask && !(eventInfo.GetSpellTypeMask() & procEntry.spellTypeMask))
            return false;
    }

    // check spell phase mask
    if (eventInfo.GetTypeMask() & REQ_SPELL_PHASE_PROC_FLAG_MASK)
    {
        if (!(eventInfo.GetSpellPhaseMask() & procEntry.spellPhaseMask))
            return false;
    }

    // check hit mask (on taken hit or on done hit, but not on spell cast phase)
    if ((eventInfo.GetTypeMask() & TAKEN_HIT_PROC_FLAG_MASK) || ((eventInfo.GetTypeMask() & DONE_HIT_PROC_FLAG_MASK) && !(eventInfo.GetSpellPhaseMask() & PROC_SPELL_PHASE_CAST)))
    {
        uint32 hitMask = procEntry.hitMask;
        // get default values if hit mask not set
        if (!hitMask)
        {
            // for taken procs allow normal + critical hits by default
            if (eventInfo.GetTypeMask() & TAKEN_HIT_PROC_FLAG_MASK)
                hitMask |= PROC_HIT_NORMAL | PROC_HIT_CRITICAL;
            // for done procs allow normal + critical + absorbs by default
            else
                hitMask |= PROC_HIT_NORMAL | PROC_HIT_CRITICAL | PROC_HIT_ABSORB;
        }
        if (!(eventInfo.GetHitMask() & hitMask))
            return false;
    }

    return true;
}

SpellBonusEntry const* SpellMgr::GetSpellBonusData(uint32 spellId) const
{
    // Lookup data
    SpellBonusMap::const_iterator itr = mSpellBonusMap.find(spellId);
    if (itr != mSpellBonusMap.end())
        return &itr->second;
    // Not found, try lookup for 1 spell rank if exist
    if (uint32 rank_1 = GetFirstSpellInChain(spellId))
    {
        SpellBonusMap::const_iterator itr2 = mSpellBonusMap.find(rank_1);
        if (itr2 != mSpellBonusMap.end())
            return &itr2->second;
    }
    return NULL;
}

SpellThreatEntry const* SpellMgr::GetSpellThreatEntry(uint32 spellID) const
{
    SpellThreatMap::const_iterator itr = mSpellThreatMap.find(spellID);
    if (itr != mSpellThreatMap.end())
        return &itr->second;
    else
    {
        uint32 firstSpell = GetFirstSpellInChain(spellID);
        itr = mSpellThreatMap.find(firstSpell);
        if (itr != mSpellThreatMap.end())
            return &itr->second;
    }
    return NULL;
}

SkillLineAbilityMapBounds SpellMgr::GetSkillLineAbilityMapBounds(uint32 spell_id) const
{
    return SkillLineAbilityMapBounds(mSkillLineAbilityMap.lower_bound(spell_id), mSkillLineAbilityMap.upper_bound(spell_id));
}

PetAura const* SpellMgr::GetPetAura(uint32 spell_id, uint8 eff)
{
    SpellPetAuraMap::const_iterator itr = mSpellPetAuraMap.find((spell_id<<8) + eff);
    if (itr != mSpellPetAuraMap.end())
        return &itr->second;
    else
        return NULL;
}

SpellEnchantProcEntry const* SpellMgr::GetSpellEnchantProcEvent(uint32 enchId) const
{
    SpellEnchantProcEventMap::const_iterator itr = mSpellEnchantProcEventMap.find(enchId);
    if (itr != mSpellEnchantProcEventMap.end())
        return &itr->second;
    return NULL;
}

bool SpellMgr::IsArenaAllowedEnchancment(uint32 ench_id) const
{
    return mEnchantCustomAttr[ench_id];
}

std::vector<SpellLinked> const* SpellMgr::GetSpellLinked(int32 spell_id) const
{
    SpellLinkedMap::const_iterator itr = mSpellLinkedMap.find(spell_id);
    return itr != mSpellLinkedMap.end() ? &(itr->second) : NULL;
}

const SpellScene* SpellMgr::GetSpellScene(int32 miscValue) const
{
    SpellSceneMap::const_iterator itr = mSpellSceneMap.find(miscValue);
    return itr != mSpellSceneMap.end() ? &(itr->second) : NULL;
}

uint32 const* SpellMgr::GetSpellSceneTrigger(uint32 p_MiscID) const
{
    SpellSceneTriggerMap::const_iterator l_Iter = m_SpellSceneTriggerMap.find(p_MiscID);
    return l_Iter != m_SpellSceneTriggerMap.end() ? &(l_Iter->second) : nullptr;
}

uint32 const* SpellMgr::GetTriggeredSpellScene(uint32 p_SpellID) const
{
    for (auto& l_Iter : m_SpellSceneTriggerMap)
    {
        if (l_Iter.second == p_SpellID)
            return &(l_Iter.first);
    }

    return nullptr;
}

std::vector<SpellTriggered> const* SpellMgr::GetSpellTriggered(int32 p_SpellID) const
{
    SpellTriggeredMap::const_iterator l_Iter = m_SpellTriggeredMap.find(p_SpellID);
    return l_Iter != m_SpellTriggeredMap.end() ? &(l_Iter->second) : nullptr;
}

std::vector<SpellDummyTrigger> const* SpellMgr::GetSpellDummyTrigger(int32 p_SpellID) const
{
    SpellDummyTriggerMap::const_iterator l_Iter = m_SpellDummyTriggerMap.find(p_SpellID);
    return l_Iter != m_SpellDummyTriggerMap.end() ? &(l_Iter->second) : nullptr;
}

std::vector<SpellDummyTrigger> const* SpellMgr::GetSpellAuraTrigger(int32 p_SpellID) const
{
    SpellAuraTriggerMap::const_iterator l_Iter = m_SpellAuraTriggerMap.find(p_SpellID);
    return l_Iter != m_SpellAuraTriggerMap.end() ? &(l_Iter->second) : nullptr;
}

std::vector<SpellAuraDummy> const* SpellMgr::GetSpellAuraDummy(int32 p_SpellID) const
{
    SpellAuraDummyMap::const_iterator l_Iter = m_SpellAuraDummyMap.find(p_SpellID);
    return l_Iter != m_SpellAuraDummyMap.end() ? &(l_Iter->second) : nullptr;
}

std::vector<SpellTargetFilter> const* SpellMgr::GetSpellTargetFilter(int32 p_SpellID) const
{
    SpellTargetFilterMap::const_iterator l_Iter = m_SpellTargetFilterMap.find(p_SpellID);
    return l_Iter != m_SpellTargetFilterMap.end() ? &(l_Iter->second) : nullptr;
}

std::vector<SpellCheckCast> const* SpellMgr::GetSpellCheckCast(int32 p_SpellID) const
{
    SpellCheckCastMap::const_iterator l_Iter = m_SpellCheckCastMap.find(p_SpellID);
    return l_Iter != m_SpellCheckCastMap.end() ? &(l_Iter->second) : nullptr;
}

std::vector<SpellVisual> const* SpellMgr::GetPlaySpellVisualData(int32 p_SpellID) const
{
    PlaySpellVisualMap::const_iterator l_Iter = m_SpellVisualMap.find(p_SpellID);
    return l_Iter != m_SpellVisualMap.end() ? &(l_Iter->second) : nullptr;
}

std::vector<SpellVisualPlayOrphan> const* SpellMgr::GetSpellVisualPlayOrphan(int32 p_SpellID) const
{
    SpellVisualPlayOrphanMap::const_iterator l_Iter = m_SpellVisualPlayOrphanMap.find(p_SpellID);
    return l_Iter != m_SpellVisualPlayOrphanMap.end() ? &(l_Iter->second) : nullptr;
}

std::vector<SpellPendingCast> const* SpellMgr::GetSpellPendingCast(int32 p_SpellID) const
{
    auto l_Iter = m_SpellPendingCastMap.find(p_SpellID);
    if (l_Iter != m_SpellPendingCastMap.end())
        return &l_Iter->second;

    return nullptr;
}

uint32 const* SpellMgr::GetRespecializationIDBySpell(uint32 p_SpellID) const
{
    auto const l_Iter = m_RespecializationSpellsMap.find(p_SpellID);
    if (l_Iter != m_RespecializationSpellsMap.end())
        return &l_Iter->second;

    return nullptr;
}

PetLevelupSpellSet const* SpellMgr::GetPetLevelupSpellList(uint32 petFamily) const
{
    PetLevelupSpellMap::const_iterator itr = mPetLevelupSpellMap.find(petFamily);
    if (itr != mPetLevelupSpellMap.end())
        return &itr->second;
    else
        return NULL;
}

PetDefaultSpellsEntry const* SpellMgr::GetPetDefaultSpellsEntry(int32 id) const
{
    PetDefaultSpellsMap::const_iterator itr = mPetDefaultSpellsMap.find(id);
    if (itr != mPetDefaultSpellsMap.end())
        return &itr->second;
    return NULL;
}

SpellAreaMapBounds SpellMgr::GetSpellAreaMapBounds(uint32 spell_id) const
{
    return mSpellAreaMap.equal_range(spell_id);
}

SpellAreaForQuestMapBounds SpellMgr::GetSpellAreaForQuestMapBounds(uint32 quest_id) const
{
    return mSpellAreaForQuestMap.equal_range(quest_id);
}

SpellAreaForQuestMapBounds SpellMgr::GetSpellAreaForQuestEndMapBounds(uint32 quest_id) const
{
    return mSpellAreaForQuestEndMap.equal_range(quest_id);
}

SpellAreaForAuraMapBounds SpellMgr::GetSpellAreaForAuraMapBounds(uint32 spell_id) const
{
    return mSpellAreaForAuraMap.equal_range(spell_id);
}

SpellAreaForAreaMapBounds SpellMgr::GetSpellAreaForAreaMapBounds(uint32 area_id) const
{
    return mSpellAreaForAreaMap.equal_range(area_id);
}

const std::set<MinorTalentEntry const*>* SpellMgr::GetSpecializationPerks(uint32 specializationId) const
{
    SpecializatioPerkMap::const_iterator iter = mSpecializationPerks.find(specializationId);
    return iter != mSpecializationPerks.end() ? &(iter->second) : nullptr;
}

bool SpellArea::IsFitToRequirements(Player const* player, uint32 newZone, uint32 newArea) const
{
    if (gender != GENDER_NONE)                   // not in expected gender
        if (!player || gender != player->getGender())
            return false;

    if (raceMask)                                // not in expected race
        if (!player || !(raceMask & player->getRaceMask()))
            return false;

    if (areaId)                                  // not in expected zone
        if (newZone != areaId && newArea != areaId)
            return false;

    if (questStart)                              // not in expected required quest state
        if (!player || ((questStartStatus & (1 << player->GetQuestStatus(questStart))) == 0))
            return false;

    if (questEnd)                                // not in expected forbidden quest state
        if (!player || (questEndStatus & (1 << player->GetQuestStatus(questEnd))))
            return false;

    if (auraSpell)                               // not have expected aura
        if (!player || (auraSpell > 0 && !player->HasAura(auraSpell)) || (auraSpell < 0 && player->HasAura(-auraSpell)))
            return false;

    // Extra conditions -- leaving the possibility add extra conditions...
    switch (spellId)
    {
        case 58600: // No fly Zone - Dalaran
        {
            if (!player)
                return false;

            AreaTableEntry const* pArea = sAreaTableStore.LookupEntry(player->GetAreaId());
            if (!(pArea && pArea->Flags & AREA_FLAG_NO_FLY_ZONE))
                return false;
            if (!player->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED) && !player->HasAuraType(SPELL_AURA_FLY))
                return false;
            break;
        }
        case 58730: // No fly Zone - Wintergrasp
        case 91604: // No fly Zone - Wintergrasp
        {
            if (!player)
                return false;

            Battlefield* Bf = sBattlefieldMgr->GetBattlefieldToZoneId(player->GetZoneId());
            if (!Bf || Bf->CanFlyIn() || (!player->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED) && !player->HasAuraType(SPELL_AURA_FLY)))
                return false;
            break;
        }
        case 68719: // Oil Refinery - Isle of Conquest.
        case 68720: // Quarry - Isle of Conquest.
        {
            if (!player || player->GetBattlegroundTypeId() != BATTLEGROUND_IC || !player->GetBattleground())
                return false;

            uint8 nodeType = spellId == 68719 ? NODE_TYPE_REFINERY : NODE_TYPE_QUARRY;
            uint8 nodeState = player->GetTeamId() == TEAM_ALLIANCE ? NODE_STATE_CONTROLLED_A : NODE_STATE_CONTROLLED_H;

            BattlegroundIC* pIC = static_cast<BattlegroundIC*>(player->GetBattleground());
            if (pIC->GetNodeState(nodeType) == nodeState)
                return true;

            return false;
        }
        case 56618: // Horde Controls Factory Phase Shift
        case 56617: // Alliance Controls Factory Phase Shift
        {
            if (!player)
                return false;

            Battlefield* bf = sBattlefieldMgr->GetBattlefieldToZoneId(player->GetZoneId());

            if (!bf || bf->GetTypeId() != BATTLEFIELD_WG)
                return false;

            // team that controls the workshop in the specified area
            uint32 team = bf->GetData(newArea);

            if (team == TEAM_HORDE)
                return spellId == 56618;
            else if (team == TEAM_ALLIANCE)
                return spellId == 56617;
            break;
        }
        case 57940: // Essence of Wintergrasp - Northrend
        case 58045: // Essence of Wintergrasp - Wintergrasp
        {
            if (!player)
                return false;

            if (Battlefield* battlefieldWG = sBattlefieldMgr->GetBattlefieldByBattleId(BATTLEFIELD_BATTLEID_WG))
                return battlefieldWG->IsEnabled() && (player->GetTeamId() == battlefieldWG->GetDefenderTeam()) && !battlefieldWG->IsWarTime();
            break;
        }
        case 74411: // Battleground - Dampening
        {
            if (!player)
                return false;

            if (Battlefield* bf = sBattlefieldMgr->GetBattlefieldToZoneId(player->GetZoneId()))
                return bf->IsWarTime();
            break;
        }
        case 73822: // Hellscream's Warsong
        case 73828: // Strength of Wrynn
        {
            if (!player)
                return false;

            InstanceScript* instanceScript = ((Player*)player)->GetInstanceScript();
            if (!instanceScript)
                return false;

            if (instanceScript->GetData(41) == 3) // 41 - DATA_BUFF_REMOVED
                return false;
            break;
        }
    }

    return true;
}

void SpellMgr::LoadSpellScene()
{
    uint32 l_OldMSTime = getMSTime();

    mSpellSceneMap.clear();    ///< need for reload case
    m_SpellSceneTriggerMap.clear();

    ///                                                       0                1           2            3                4           5
    QueryResult l_Result = WorldDatabase.Query("SELECT SceneScriptPackageID, MiscValue, TrigerSpell, MonsterCredit, PlaybackFlags, ScriptName FROM spell_scene");
    if (!l_Result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 visual spells. DB table `spell_scene` is empty.");
        return;
    }

    uint32 l_Count = 0;
    do
    {
        Field* l_Fields = l_Result->Fetch();
        int8 l_ColIt = 0;

        SpellScene l_SpellScene;
        l_SpellScene.SceneScriptPackageID   = l_Fields[l_ColIt++].GetInt32();
        l_SpellScene.MiscValue              = l_Fields[l_ColIt++].GetInt32();
        l_SpellScene.TrigerSpell            = l_Fields[l_ColIt++].GetInt32();
        l_SpellScene.MonsterCredit          = l_Fields[l_ColIt++].GetInt32();
        l_SpellScene.PlaybackFlags          = l_Fields[l_ColIt++].GetInt32();
        l_SpellScene.scriptID               = sObjectMgr->GetScriptId(l_Fields[l_ColIt++].GetString().c_str());

        mSpellSceneMap[l_SpellScene.MiscValue] = l_SpellScene;

        ++l_Count;
    }
    while (l_Result->NextRow());

    l_Result = WorldDatabase.Query("SELECT MiscID, SpellID FROM spell_scene_trigger");
    if (l_Result)
    {
        do
        {
            Field* l_Fields = l_Result->Fetch();

            uint32 l_MiscID = l_Fields[0].GetUInt32();
            uint32 l_SpellID = l_Fields[1].GetUInt32();

            m_SpellSceneTriggerMap[l_MiscID] = l_SpellID;
        }
        while (l_Result->NextRow());
    }

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u scene spells in %u ms", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

void SpellMgr::LoadSpellRanks()
{
    uint32 oldMSTime = getMSTime();

    // cleanup core data before reload - remove reference to ChainNode from SpellInfo
    for (SpellChainMap::iterator itr = mSpellChains.begin(); itr != mSpellChains.end(); ++itr)
    {
        for (int difficulty = 0; difficulty < Difficulty::MaxDifficulties; difficulty++)
        {
            if (mSpellInfoMap[difficulty][itr->first])
                mSpellInfoMap[difficulty][itr->first]->ChainEntry = NULL;
        }
    }
    mSpellChains.clear();
    //                                                     0             1      2
    std::map<uint32 /*spell*/, uint32 /*next*/> chains;
    std::set<uint32> hasPrev;
    for (uint32 i = 0; i < sSkillLineAbilityStore.GetNumRows(); ++i)
    {
        SkillLineAbilityEntry const* skillAbility = sSkillLineAbilityStore.LookupEntry(i);
        if (!skillAbility)
            continue;

        if (!skillAbility->SupercedesSpell)
            continue;

        if (!GetSpellInfo(skillAbility->SupercedesSpell) || !GetSpellInfo(skillAbility->spellId))
            continue;

        chains[skillAbility->SupercedesSpell] = skillAbility->spellId;
        hasPrev.insert(skillAbility->spellId);
    }

    // each key in chains that isn't present in hasPrev is a first rank
    for (auto itr = chains.begin(); itr != chains.end(); ++itr)
    {
        if (hasPrev.count(itr->first))
            continue;

        SpellInfo const* first = sSpellMgr->GetSpellInfo(itr->first);
        SpellInfo const* next = sSpellMgr->GetSpellInfo(itr->second);

        mSpellChains[itr->first].first = first;
        mSpellChains[itr->first].prev = nullptr;
        mSpellChains[itr->first].next = next;
        mSpellChains[itr->first].last = next;
        mSpellChains[itr->first].rank = 1;
            for (int difficulty = 0; difficulty < Difficulty::MaxDifficulties; difficulty++)
                if (mSpellInfoMap[difficulty][itr->first])
                    mSpellInfoMap[difficulty][itr->first]->ChainEntry = &mSpellChains[itr->first];

        mSpellChains[itr->second].first = first;
        mSpellChains[itr->second].prev = first;
        mSpellChains[itr->second].next = nullptr;
        mSpellChains[itr->second].last = next;
        mSpellChains[itr->second].rank = 2;
            for (int difficulty = 0; difficulty < Difficulty::MaxDifficulties; difficulty++)
                if (mSpellInfoMap[difficulty][itr->second])
                    mSpellInfoMap[difficulty][itr->second]->ChainEntry = &mSpellChains[itr->second];

        uint8 rank = 3;
        auto nextItr = chains.find(itr->second);
        while (nextItr != chains.end())
        {
            SpellInfo const* prev = sSpellMgr->GetSpellInfo(nextItr->first); // already checked in previous iteration (or above, in case this is the first one)
            SpellInfo const* last = sSpellMgr->GetSpellInfo(nextItr->second);

            mSpellChains[nextItr->first].next = last;

            mSpellChains[nextItr->second].first = first;
            mSpellChains[nextItr->second].prev = prev;
            mSpellChains[nextItr->second].next = nullptr;
            mSpellChains[nextItr->second].last = last;
            mSpellChains[nextItr->second].rank = rank++;
            for (int difficulty = 0; difficulty < Difficulty::MaxDifficulties; difficulty++)
                if (mSpellInfoMap[difficulty][nextItr->second])
                    mSpellInfoMap[difficulty][nextItr->second]->ChainEntry = &mSpellChains[nextItr->second];
            // fill 'last'
            do
            {
                mSpellChains[prev->Id].last = last;
                prev = mSpellChains[prev->Id].prev;
            } while (prev);

            nextItr = chains.find(nextItr->second);
        }
    }

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u spell rank records in %u ms", uint32(mSpellChains.size()), GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::LoadSpellRequired()
{
    uint32 oldMSTime = getMSTime();

    mSpellsReqSpell.clear();                                   // need for reload case
    mSpellReq.clear();                                         // need for reload case

    //                                                   0        1
    QueryResult result = WorldDatabase.Query("SELECT spell_id, req_spell from spell_required");

    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 spell required records. DB table `spell_required` is empty.");

        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 spell_id = fields[0].GetUInt32();
        uint32 spell_req = fields[1].GetUInt32();

        // check if chain is made with valid first spell
        SpellInfo const* spell = GetSpellInfo(spell_id);
        if (!spell)
        {
            sLog->outError(LOG_FILTER_SQL, "spell_id %u in `spell_required` table is not found in dbcs, skipped", spell_id);
            continue;
        }

        SpellInfo const* req_spell = GetSpellInfo(spell_req);
        if (!req_spell)
        {
            sLog->outError(LOG_FILTER_SQL, "req_spell %u in `spell_required` table is not found in dbcs, skipped", spell_req);
            continue;
        }

        if (GetFirstSpellInChain(spell_id) == GetFirstSpellInChain(spell_req))
        {
            sLog->outError(LOG_FILTER_SQL, "req_spell %u and spell_id %u in `spell_required` table are ranks of the same spell, entry not needed, skipped", spell_req, spell_id);
            continue;
        }

        if (IsSpellRequiringSpell(spell_id, spell_req))
        {
            sLog->outError(LOG_FILTER_SQL, "duplicated entry of req_spell %u and spell_id %u in `spell_required`, skipped", spell_req, spell_id);
            continue;
        }

        mSpellReq.insert (std::pair<uint32, uint32>(spell_id, spell_req));
        mSpellsReqSpell.insert (std::pair<uint32, uint32>(spell_req, spell_id));
        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u spell required records in %u ms", count, GetMSTimeDiffToNow(oldMSTime));

}

void SpellMgr::LoadSpellLearnSkills()
{
    uint32 oldMSTime = getMSTime();

    mSpellLearnSkills.clear();                              // need for reload case

    // search auto-learned skills and add its to map also for use in unlearn spells/talents

    uint32 l_ProfessionSkillForStep[] { 0, 75, 150, 225, 300, 375, 450, 525, 600, 700, 800};

    uint32 dbc_count = 0;
    for (uint32 spell = 0; spell < GetSpellInfoStoreSize(); ++spell)
    {
        SpellInfo const* entry = GetSpellInfo(spell);

        if (!entry)
            continue;

        for (uint8 i = 0; i < entry->EffectCount; ++i)
        {
            if (entry->Effects[i].Effect == SPELL_EFFECT_SKILL)
            {
                SpellLearnSkillNode dbc_node;
                dbc_node.skill = entry->Effects[i].MiscValue;
                dbc_node.step = entry->Effects[i].CalcValue();

                if (IsProfessionSkill(dbc_node.skill))
                {
                    uint16 l_Step = std::min(dbc_node.step, (uint16)((sizeof(l_ProfessionSkillForStep) / sizeof(uint32)) - 1));
                    dbc_node.value = 1;
                    dbc_node.maxvalue = l_ProfessionSkillForStep[l_Step];
                }
                else
                {
                    if (dbc_node.skill != SKILL_RIDING)
                        dbc_node.value = 1;
                    else
                        dbc_node.value = dbc_node.step * 75;
                    dbc_node.maxvalue = dbc_node.step * 75;
                }

                mSpellLearnSkills[spell] = dbc_node;
                ++dbc_count;
                break;
            }
        }
    }

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u Spell Learn Skills from DB2 in %u ms", dbc_count, GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::LoadSpellLearnSpells()
{
    uint32 oldMSTime = getMSTime();

    mSpellLearnSpells.clear();                              // need for reload case

    //                                                  0      1        2
    QueryResult result = WorldDatabase.Query("SELECT entry, SpellID, Active FROM spell_learn_spell");
    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 spell learn spells. DB table `spell_learn_spell` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 spell_id = fields[0].GetUInt32();

        SpellLearnSpellNode node;
        node.spell       = fields[1].GetUInt32();
        node.active      = fields[2].GetBool();
        node.autoLearned = false;

        if (!GetSpellInfo(spell_id))
        {
            sLog->outError(LOG_FILTER_SQL, "Spell %u listed in `spell_learn_spell` does not exist", spell_id);
            continue;
        }

        if (!GetSpellInfo(node.spell))
        {
            sLog->outError(LOG_FILTER_SQL, "Spell %u listed in `spell_learn_spell` learning not existed spell %u", spell_id, node.spell);
            continue;
        }

        mSpellLearnSpells.insert(SpellLearnSpellMap::value_type(spell_id, node));

        ++count;
    }
    while (result->NextRow());

    // search auto-learned spells and add its to map also for use in unlearn spells/talents
    uint32 dbc_count = 0;
    for (uint32 spell = 0; spell < GetSpellInfoStoreSize(); ++spell)
    {
        SpellInfo const* entry = GetSpellInfo(spell);

        if (!entry)
            continue;

        for (uint8 i = 0; i < entry->EffectCount; ++i)
        {
            if (entry->Effects[i].Effect == SPELL_EFFECT_LEARN_SPELL)
            {
                SpellLearnSpellNode dbc_node;
                dbc_node.spell = entry->Effects[i].TriggerSpell;
                dbc_node.active = true;                     // all db2 based learned spells is active (show in spell book or hide by client itself)

                // ignore learning not existed spells (broken/outdated/or generic learning spell 483
                if (!GetSpellInfo(dbc_node.spell))
                    continue;

                // talent or passive spells or skill-step spells auto-casted and not need dependent learning,
                // pet teaching spells must not be dependent learning (casted)
                // other required explicit dependent learning
                dbc_node.autoLearned = entry->Effects[i].TargetA.GetTarget() == TARGET_UNIT_PET || entry->IsPassive() || entry->HasEffect(SPELL_EFFECT_SKILL_STEP);

                SpellLearnSpellMapBounds db_node_bounds = GetSpellLearnSpellMapBounds(spell);

                bool found = false;
                for (SpellLearnSpellMap::const_iterator itr = db_node_bounds.first; itr != db_node_bounds.second; ++itr)
                {
                    if (itr->second.spell == dbc_node.spell)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Spell %u auto-learn spell %u in spell.db2 then the record in `spell_learn_spell` is redundant, please fix DB.",
                            spell, dbc_node.spell);
                        found = true;
                        break;
                    }
                }

                if (!found)                                  // add new spell-spell pair if not found
                {
                    mSpellLearnSpells.insert(SpellLearnSpellMap::value_type(spell, dbc_node));
                    ++dbc_count;
                }
            }
        }
    }

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u spell learn spells + %u found in DB2 in %u ms", count, dbc_count, GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::LoadSpellTargetPositions()
{
    uint32 oldMSTime = getMSTime();

    mSpellTargetPositions.clear(); // need for reload case

    //                                                0    1         2           3                  4                     5                  6
    QueryResult result = WorldDatabase.Query("SELECT id, effIndex, target_map, target_position_x, target_position_y, target_position_z, target_orientation FROM spell_target_position");
    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 spell target coordinates. DB table `spell_target_position` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 Spell_ID = fields[0].GetUInt32();
        SpellEffIndex effIndex = SpellEffIndex(fields[1].GetUInt8());

        SpellTargetPosition st;

        st.target_mapId = fields[2].GetUInt16();
        st.target_X = fields[3].GetFloat();
        st.target_Y = fields[4].GetFloat();
        st.target_Z = fields[5].GetFloat();
        st.target_Orientation = fields[6].GetFloat();

        MapEntry const* mapEntry = sMapStore.LookupEntry(st.target_mapId);
        if (!mapEntry)
        {
            sLog->outError(LOG_FILTER_SQL, "Spell (Id: %u, effIndex: %u) target map (ID: %u) does not exist in `Map.db2`.", Spell_ID, effIndex, st.target_mapId);
            continue;
        }

        if (st.target_X == 0 && st.target_Y == 0 && st.target_Z == 0)
        {
            sLog->outError(LOG_FILTER_SQL, "Spell (Id: %u, effIndex: %u) target coordinates not provided.", Spell_ID, effIndex);
            continue;
        }

        SpellInfo const* spellInfo = GetSpellInfo(Spell_ID);
        if (!spellInfo)
        {
            sLog->outError(LOG_FILTER_SQL, "Spell (ID:%u) listed in `spell_target_position` does not exist.", Spell_ID);
            continue;
        }

        std::pair<uint32, SpellEffIndex> key = std::make_pair(Spell_ID, effIndex);
        mSpellTargetPositions[key] = st;
        ++count;

    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u spell teleport coordinates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::LoadSpellGroups()
{
    uint32 oldMSTime = getMSTime();

    mSpellSpellGroup.clear();                                  // need for reload case
    mSpellGroupSpell.clear();

    //                                                0     1
    QueryResult result = WorldDatabase.Query("SELECT id, spell_id FROM spell_group");
    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 spell group definitions. DB table `spell_group` is empty.");
        return;
    }

    std::set<uint32> groups;
    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 group_id = fields[0].GetUInt32();
        if (group_id <= SPELL_GROUP_DB_RANGE_MIN && group_id >= SPELL_GROUP_CORE_RANGE_MAX)
        {
            sLog->outError(LOG_FILTER_SQL, "SpellGroup id %u listed in `spell_group` is in core range, but is not defined in core!", group_id);
            continue;
        }
        int32 spell_id = fields[1].GetInt32();

        groups.insert(std::set<uint32>::value_type(group_id));
        mSpellGroupSpell.insert(SpellGroupSpellMap::value_type((SpellGroup)group_id, spell_id));

    }
    while (result->NextRow());

    for (SpellGroupSpellMap::iterator itr = mSpellGroupSpell.begin(); itr!= mSpellGroupSpell.end();)
    {
        if (itr->second < 0)
        {
            if (groups.find(abs(itr->second)) == groups.end())
            {
                sLog->outError(LOG_FILTER_SQL, "SpellGroup id %u listed in `spell_group` does not exist", abs(itr->second));
                mSpellGroupSpell.erase(itr++);
            }
            else
                ++itr;
        }
        else
        {
            SpellInfo const* spellInfo = GetSpellInfo(itr->second);

            if (!spellInfo)
            {
                sLog->outError(LOG_FILTER_SQL, "Spell %u listed in `spell_group` does not exist", itr->second);
                mSpellGroupSpell.erase(itr++);
            }
            else if (spellInfo->GetRank() > 1)
            {
                sLog->outError(LOG_FILTER_SQL, "Spell %u listed in `spell_group` is not first rank of spell", itr->second);
                mSpellGroupSpell.erase(itr++);
            }
            else
                ++itr;
        }
    }

    for (std::set<uint32>::iterator groupItr = groups.begin(); groupItr != groups.end(); ++groupItr)
    {
        std::set<uint32> spells;
        GetSetOfSpellsInSpellGroup(SpellGroup(*groupItr), spells);

        for (std::set<uint32>::iterator spellItr = spells.begin(); spellItr != spells.end(); ++spellItr)
        {
            ++count;
            mSpellSpellGroup.insert(SpellSpellGroupMap::value_type(*spellItr, SpellGroup(*groupItr)));
        }
    }

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u spell group definitions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::LoadSpellGroupStackRules()
{
    uint32 oldMSTime = getMSTime();

    mSpellGroupStack.clear();                                  // need for reload case

    //                                                       0         1
    QueryResult result = WorldDatabase.Query("SELECT group_id, stack_rule FROM spell_group_stack_rules");
    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 spell group stack rules. DB table `spell_group_stack_rules` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 group_id = fields[0].GetUInt32();
        uint8 stack_rule = fields[1].GetInt8();
        if (stack_rule >= SPELL_GROUP_STACK_RULE_MAX)
        {
            sLog->outError(LOG_FILTER_SQL, "SpellGroupStackRule %u listed in `spell_group_stack_rules` does not exist", stack_rule);
            continue;
        }

        SpellGroupSpellMapBounds spellGroup = GetSpellGroupSpellMapBounds((SpellGroup)group_id);

        if (spellGroup.first == spellGroup.second)
        {
            sLog->outError(LOG_FILTER_SQL, "SpellGroup id %u listed in `spell_group_stack_rules` does not exist", group_id);
            continue;
        }

        mSpellGroupStack[(SpellGroup)group_id] = (SpellGroupStackRule)stack_rule;

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u spell group stack rules in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::LoadForbiddenSpells()
{
    uint32 oldMSTime = getMSTime();

    mForbiddenSpells.clear();

    uint32 count = 0;

    QueryResult result = WorldDatabase.Query("SELECT spell_id FROM spell_forbidden");
    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u spell group definitions", count);
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        mForbiddenSpells.push_back(fields[0].GetUInt32());

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u forbidden spells in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::InitializeItemUpgradeDatas()
{
    uint32 oldMSTime = getMSTime();

    uint16 spTable[71][2] =
    {
        {458, 4914}, {463, 5152}, {466, 5293}, {470, 5497}, {471, 5552}, {474, 5704}, {476, 5812}, {478, 5920}, {480, 6037}, {483, 6206},
        {484, 6262}, {487, 6445}, {489, 6564}, {490, 6628}, {491, 6684}, {493, 6810}, {494, 6874}, {496, 7007}, {497, 7070}, {498, 7140},
        {500, 7272}, {501, 7337}, {502, 7410}, {503, 7478}, {504, 7548}, {505, 7619}, {506, 7690}, {507, 7759}, {508, 7836}, {509, 7907},
        {510, 7982}, {511, 8054}, {512, 8132}, {513, 8209}, {514, 8286}, {515, 8364}, {516, 8441}, {517, 8521}, {518, 8603}, {519, 8680},
        {520, 8764}, {521, 8841}, {522, 8925}, {524, 9093}, {525, 9179}, {526, 9265}, {528, 9440}, {530, 9618}, {532, 9797}, {535, 10078},
        {536, 10169}, {539, 10458}, {540, 10557}, {541, 10655}, {543, 10859}, {544, 10957}, {545, 11060}, {548, 11372}, {549, 11479}, {553, 11916},
        {557, 12370}, {559, 12602}, {561, 12841}, {563, 13079}, {566, 13452}, {567, 13578}, {570, 13961}, {572, 14225}, {574, 14492}, {576, 14766}, {580, 15321}
    };

    for (uint8 i = 0; i < 71; ++i)
        mItemUpgradeDatas.insert(std::make_pair(spTable[i][0], spTable[i][1]));

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 71 item upgrade datas in %u ms", GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::LoadSpellInfoOverride()
{
    uint32 l_OldMSTime = getMSTime();

    QueryResult l_Result = WorldDatabase.Query("SELECT SpellID, EffectIndex, Field, Value FROM spellinfo_override");
    if (!l_Result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 spellinfo overrides. DB table `spellinfo_override` is empty!");
        return;
    }

    uint32 l_Count = 0;

    do
    {
        uint8 l_Index       = 0;
        Field* l_Fields     = l_Result->Fetch();

        uint32 l_SpellID    = l_Fields[l_Index++].GetUInt32();

        SpellInfoOverrideEntry l_Entry;

        l_Entry.spellId         = l_SpellID;
        l_Entry.effectID        = l_Fields[l_Index++].GetInt8();
        l_Entry.overrideValue   = l_Fields[l_Index++].GetUInt32();
        l_Entry.value           = l_Fields[l_Index++].GetInt64();

        if (l_Entry.effectID >= MAX_EFFECTS)
            sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : effect id > MAX_EFFECT", l_SpellID);

        for (int l_Difficulty = 0; l_Difficulty < Difficulty::MaxDifficulties; l_Difficulty++)
        {
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(l_SpellID, Difficulty(l_Difficulty));
            if (!spellInfo)
                continue;

            SpellInfo* l_SpellInfo = const_cast<SpellInfo*>(spellInfo);
            switch (l_Entry.overrideValue)
            {
                case eOverrideValues::SpellDurationId:
                {
                    if (!sSpellDurationStore.LookupEntry(l_Entry.value))
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id;%d) : Duration Index doesn't exist in store", l_SpellID);
                        continue;
                    }

                    l_SpellInfo->SetDurationIndex(l_Entry.value);
                    break;
                }
                case eOverrideValues::SpellOverrideSpellList:
                {
                    if (l_Entry.value > 0xFFFFFFFF || l_Entry.value < 0)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : OverrideSpellList isn't UINT32", l_SpellID);
                        continue;
                    }

                    l_SpellInfo->OverrideSpellList.push_back(l_Entry.value);
                    break;
                }
                case eOverrideValues::SpellRangeId:
                {
                    if (!sSpellRangeStore.LookupEntry(l_Entry.value))
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id;%d) : Range Index doesn't exist in store", l_SpellID);
                        continue;
                    }

                    l_SpellInfo->SetRangeIndex(l_Entry.value);
                    break;
                }
                case eOverrideValues::SpellStackAmount:
                {
                    if (l_Entry.value > 0xFFFFFFFF || l_Entry.value < 0)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : StackAmount isn't UINT32", l_SpellID);
                        continue;
                    }

                    l_SpellInfo->StackAmount = l_Entry.value;
                    break;
                }
                case eOverrideValues::SpellAttribute0:
                {
                    if (std::abs(l_Entry.value) > 0xFFFFFFFF)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : SpellAttribute0 isn't UINT32", l_SpellID);
                        continue;
                    }

                    if (l_Entry.value > 0)
                        l_SpellInfo->Attributes |= l_Entry.value;
                    else
                        l_SpellInfo->Attributes &= ~(-l_Entry.value);

                    break;
                }
                case eOverrideValues::SpellAttribute1:
                {
                    if (std::abs(l_Entry.value) > 0xFFFFFFFF)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : SpellAttribute1 isn't UINT32", l_SpellID);
                        continue;
                    }

                    if (l_Entry.value > 0)
                        l_SpellInfo->AttributesEx |= l_Entry.value;
                    else
                        l_SpellInfo->AttributesEx &= ~(-l_Entry.value);

                    break;
                }
                case eOverrideValues::SpellAttribute2:
                {
                    if (std::abs(l_Entry.value) > 0xFFFFFFFF)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : SpellAttribute2 isn't UINT32", l_SpellID);
                        continue;
                    }

                    if (l_Entry.value > 0)
                        l_SpellInfo->AttributesEx2 |= l_Entry.value;
                    else
                        l_SpellInfo->AttributesEx2 &= ~(-l_Entry.value);

                    break;
                }
                case eOverrideValues::SpellAttribute3:
                {
                    if (std::abs(l_Entry.value) > 0xFFFFFFFF)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : SpellAttribute3 isn't UINT32", l_SpellID);
                        continue;
                    }

                    if (l_Entry.value > 0)
                        l_SpellInfo->AttributesEx3 |= l_Entry.value;
                    else
                        l_SpellInfo->AttributesEx3 &= ~(-l_Entry.value);

                    break;
                }
                case eOverrideValues::SpellAttribute4:
                {
                    if (std::abs(l_Entry.value) > 0xFFFFFFFF)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : SpellAttribute4 isn't UINT32", l_SpellID);
                        continue;
                    }

                    if (l_Entry.value > 0)
                        l_SpellInfo->AttributesEx4 |= l_Entry.value;
                    else
                        l_SpellInfo->AttributesEx4 &= ~(-l_Entry.value);

                    break;
                }
                case eOverrideValues::SpellAttribute5:
                {
                    if (std::abs(l_Entry.value) > 0xFFFFFFFF)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : SpellAttribute5 isn't UINT32", l_SpellID);
                        continue;
                    }

                    if (l_Entry.value > 0)
                        l_SpellInfo->AttributesEx5 |= l_Entry.value;
                    else
                        l_SpellInfo->AttributesEx5 &= ~(-l_Entry.value);

                    break;
                }
                case eOverrideValues::SpellAttribute6:
                {
                    if (std::abs(l_Entry.value) > 0xFFFFFFFF)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : SpellAttribute6 isn't UINT32", l_SpellID);
                        continue;
                    }

                    if (l_Entry.value > 0)
                        l_SpellInfo->AttributesEx6 |= l_Entry.value;
                    else
                        l_SpellInfo->AttributesEx6 &= ~(-l_Entry.value);

                    break;
                }
                case eOverrideValues::SpellAttribute7:
                {
                    if (std::abs(l_Entry.value) > 0xFFFFFFFF)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : SpellAttribute7 isn't UINT32", l_SpellID);
                        continue;
                    }

                    if (l_Entry.value > 0)
                        l_SpellInfo->AttributesEx7 |= l_Entry.value;
                    else
                        l_SpellInfo->AttributesEx7 &= ~(-l_Entry.value);

                    break;
                }
                case eOverrideValues::SpellAttribute8:
                {
                    if (std::abs(l_Entry.value) > 0xFFFFFFFF)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : SpellAttribute8 isn't UINT32", l_SpellID);
                        continue;
                    }

                    if (l_Entry.value > 0)
                        l_SpellInfo->AttributesEx8 |= l_Entry.value;
                    else
                        l_SpellInfo->AttributesEx8 &= ~(-l_Entry.value);

                    break;
                }
                case eOverrideValues::SpellAttribute9:
                {
                    if (std::abs(l_Entry.value) > 0xFFFFFFFF)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : SpellAttribute9 isn't UINT32", l_SpellID);
                        continue;
                    }

                    if (l_Entry.value > 0)
                        l_SpellInfo->AttributesEx9 |= l_Entry.value;
                    else
                        l_SpellInfo->AttributesEx9 &= ~(-l_Entry.value);

                    break;
                }
                case eOverrideValues::SpellAttribute10:
                {
                    if (std::abs(l_Entry.value) > 0xFFFFFFFF)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : SpellAttribute10 isn't UINT32", l_SpellID);
                        continue;
                    }

                    if (l_Entry.value > 0)
                        l_SpellInfo->AttributesEx10 |= l_Entry.value;
                    else
                        l_SpellInfo->AttributesEx10 &= ~(-l_Entry.value);

                    break;
                }
                case eOverrideValues::SpellAttribute11:
                {
                    if (std::abs(l_Entry.value) > 0xFFFFFFFF)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : SpellAttribute11 isn't UINT32", l_SpellID);
                        continue;
                    }

                    if (l_Entry.value > 0)
                        l_SpellInfo->AttributesEx11 |= l_Entry.value;
                    else
                        l_SpellInfo->AttributesEx11 &= ~(-l_Entry.value);

                    break;
                }
                case eOverrideValues::SpellAttribute12:
                {
                    if (std::abs(l_Entry.value) > 0xFFFFFFFF)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : SpellAttribute12 isn't UINT32", l_SpellID);
                        continue;
                    }

                    if (l_Entry.value > 0)
                        l_SpellInfo->AttributesEx12 |= l_Entry.value;
                    else
                        l_SpellInfo->AttributesEx12 &= ~(-l_Entry.value);

                    break;
                }
                case eOverrideValues::SpellAttribute13:
                {
                    if (std::abs(l_Entry.value) > 0xFFFFFFFF)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : SpellAttribute13 isn't UINT32", l_SpellID);
                        continue;
                    }

                    if (l_Entry.value > 0)
                        l_SpellInfo->AttributesEx13 |= l_Entry.value;
                    else
                        l_SpellInfo->AttributesEx13 &= ~(-l_Entry.value);

                    break;
                }
                case eOverrideValues::EffectName:
                {
                    if (l_Entry.value >= TOTAL_SPELL_EFFECTS)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : EffectName > TOTAL_SPELL_EFFECT", l_SpellID);
                        continue;
                    }

                    l_SpellInfo->Effects[l_Entry.effectID].Effect = l_Entry.value;
                    break;
                }
                case eOverrideValues::EffectApplyAuraName:
                {
                    if (l_Entry.value >= TOTAL_AURAS)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : ApplyAuraName > TOTAL_AURAS", l_SpellID);
                        continue;
                    }

                    l_SpellInfo->Effects[l_Entry.effectID].ApplyAuraName = l_Entry.value;
                    break;
                }
                case eOverrideValues::EffectBasePoints:
                {
                    if (l_Entry.value > 0x7FFFFFFFF || l_Entry.value < -0x7FFFFFFFF)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : BasePoint isn't INT32", l_SpellID);
                        continue;
                    }

                    l_SpellInfo->Effects[l_Entry.effectID].BasePoints = l_Entry.value;
                    break;
                }
                case eOverrideValues::EffectTargetA:
                {
                    if (l_Entry.value >= TOTAL_SPELL_TARGETS)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : TargetA > TOTAL_SPELL_TARGETS", l_SpellID);
                        continue;
                    }

                    l_SpellInfo->Effects[l_Entry.effectID].TargetA = l_Entry.value;
                    break;
                }
                case eOverrideValues::EffectTargetB:
                {
                    if (l_Entry.value >= TOTAL_SPELL_TARGETS)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : TargetB > TOTAL_SPELL_TARGETS", l_SpellID);
                        continue;
                    }

                    l_SpellInfo->Effects[l_Entry.effectID].TargetB = l_Entry.value;
                    break;
                }
                case eOverrideValues::EffectMiscA:
                {
                    if (l_Entry.value > 0x7FFFFFFFF || l_Entry.value < -0x7FFFFFFFF)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : MiscA isn't INT32", l_SpellID);
                        continue;
                    }

                    l_SpellInfo->Effects[l_Entry.effectID].MiscValue = l_Entry.value;
                    break;
                }
                case eOverrideValues::EffectMiscB:
                {
                    if (l_Entry.value > 0x7FFFFFFFF || l_Entry.value < -0x7FFFFFFFF)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : MiscB isn't INT32", l_SpellID);
                        continue;
                    }

                    l_SpellInfo->Effects[l_Entry.effectID].MiscValueB = l_Entry.value;
                    break;
                }
                case eOverrideValues::EffectRadiusId:
                {
                    if (!sSpellRadiusStore.LookupEntry(l_Entry.value))
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id;%d) : Radius Index doesn't exist in store", l_SpellID);
                        continue;
                    }

                    l_SpellInfo->Effects[l_Entry.effectID].SetRadiusIndex(l_Entry.value);
                    break;
                }
                case eOverrideValues::EffectTriggerSpell:
                {
                    SpellInfo const* l_TempspellInfo = GetSpellInfo(l_Entry.value);
                    if ((!l_TempspellInfo) && (l_Entry.value != 0))
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : Trigger Spell doesn't exist", l_SpellID);
                        continue;
                    }

                    l_SpellInfo->Effects[l_Entry.effectID].TriggerSpell = l_Entry.value;
                    break;
                }
                case eOverrideValues::EffectAmplitude:
                {
                    if (l_Entry.value > 0xFFFFFFFF || l_Entry.value < 0)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : Amplitude isn't UINT32", l_SpellID);
                        continue;
                    }

                    l_SpellInfo->Effects[l_Entry.effectID].Amplitude = l_Entry.value;
                    break;
                }
                case eOverrideValues::EffectSpellClassMask0:
                {
                    if (l_Entry.value > 0xFFFFFFFF || l_Entry.value < 0)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : SpellClassMask0 isn't UINT32", l_SpellID);
                        continue;
                    }

                    l_SpellInfo->Effects[l_Entry.effectID].SpellClassMask[0] = l_Entry.value;
                    break;
                }
                case eOverrideValues::EffectSpellClassMask1:
                {
                    if (l_Entry.value > 0xFFFFFFFF || l_Entry.value < 0)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : SpellClassMask1 isn't UINT32", l_SpellID);
                        continue;
                    }

                    l_SpellInfo->Effects[l_Entry.effectID].SpellClassMask[1] = l_Entry.value;
                    break;
                }
                case eOverrideValues::EffectSpellClassMask2:
                {
                    if (l_Entry.value > 0xFFFFFFFF || l_Entry.value < 0)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : SpellClassMask2 isn't UINT32", l_SpellID);
                        continue;
                    }

                    l_SpellInfo->Effects[l_Entry.effectID].SpellClassMask[2] = l_Entry.value;
                    break;
                }
                case eOverrideValues::EffectSpellClassMask3:
                {
                    if (l_Entry.value > 0xFFFFFFFF || l_Entry.value < 0)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : SpellClassMask3 isn't UINT32", l_SpellID);
                        continue;
                    }

                    l_SpellInfo->Effects[l_Entry.effectID].SpellClassMask[3] = l_Entry.value;
                    break;
                }
                case eOverrideValues::StartRecoveryTime:
                {
                    if (l_Entry.value > 0xFFFFFFFF || l_Entry.value < 0)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : StartRecoveryTime isn't UINT32", l_SpellID);
                        continue;
                    }

                    l_SpellInfo->StartRecoveryTime = l_Entry.value;
                    break;
                }
                case eOverrideValues::ProcCharges:
                {
                    if (l_Entry.value > 0xFFFFFFFF || l_Entry.value < 0)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : ProcCharges isn't UINT32", l_SpellID);
                        continue;
                    }

                    l_SpellInfo->ProcCharges = l_Entry.value;
                    break;
                }
                case eOverrideValues::ProcFlags:
                {
                    if (std::abs(l_Entry.value) > 0xFFFFFFFF)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : ProcFlags isn't UINT32", l_SpellID);
                        continue;
                    }

                    if (l_Entry.value > 0)
                        l_SpellInfo->ProcFlags |= l_Entry.value;
                    else
                        l_SpellInfo->ProcFlags &= ~(-l_Entry.value);

                    break;
                }
                case eOverrideValues::InterruptFlags:
                {
                    if (std::abs(l_Entry.value) > 0xFFFFFFFF)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : InterruptFlag isn't UINT32", l_SpellID);
                        continue;
                    }

                    if (l_Entry.value > 0)
                        l_SpellInfo->InterruptFlags |= l_Entry.value;
                    else
                        l_SpellInfo->InterruptFlags &= ~(-l_Entry.value);

                    break;
                }
                case eOverrideValues::AuraInterruptFlags:
                {
                    if (std::abs(l_Entry.value) > 0xFFFFFFFF)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : AuraInterruptFlag isn't UINT32", l_SpellID);
                        continue;
                    }

                    if (l_Entry.value > 0)
                        l_SpellInfo->AuraInterruptFlags |= l_Entry.value;
                    else
                        l_SpellInfo->AuraInterruptFlags &= ~(-l_Entry.value);

                    break;
                }
                case eOverrideValues::ProcChance:
                {
                    l_SpellInfo->ProcChance = static_cast<float>(l_Entry.value);
                    break;
                }
                case eOverrideValues::SpellCustomAttributes:
                {
                    if (std::abs(l_Entry.value) > 0xFFFFFFFF)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : SpellCustomAttributes isn't UINT32", l_SpellID);
                        continue;
                    }

                    if (l_Entry.value > 0)
                        l_SpellInfo->AttributesCu |= l_Entry.value;
                    else
                        l_SpellInfo->AttributesCu &= ~(-l_Entry.value);

                    break;
                }
                case eOverrideValues::ChannelInterruptFlags:
                {
                    if (std::abs(l_Entry.value) > 0xFFFFFFFF)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Failed to override spellinfo (spell_id:%d) : ChannelInterruptFlag isn't UINT32", l_SpellID);
                        continue;
                    }

                    if (l_Entry.value > 0)
                        l_SpellInfo->ChannelInterruptFlags |= l_Entry.value;
                    else
                        l_SpellInfo->ChannelInterruptFlags &= ~(-l_Entry.value);

                    break;
                }
                case eOverrideValues::OverrideRecoveryTime:
                {
                    l_SpellInfo->RecoveryTime = l_Entry.value;
                    break;
                }
                case eOverrideValues::OverrideDispelType:
                {
                    if (l_Entry.value > 0)
                        l_SpellInfo->Dispel |= l_Entry.value;
                    else
                        l_SpellInfo->Dispel &= ~(-l_Entry.value);

                    break;
                }
                case eOverrideValues::OverrideTargetAuraState:
                {
                    l_SpellInfo->TargetAuraState = l_Entry.value;
                    break;
                }
                case eOverrideValues::OverrideSpellSchoolMask:
                {
                    l_SpellInfo->SchoolMask = l_Entry.value;
                    break;
                }
                case eOverrideValues::OverrideSpellFamily:
                {
                    l_SpellInfo->SpellFamilyName = l_Entry.value;
                    break;
                }
                default:
                    break;
            }

            /// This must be re-done if targets changed since the spellinfo load
            l_SpellInfo->ExplicitTargetMask = l_SpellInfo->IsTargetingFollower() ? TARGET_FLAG_UNIT : l_SpellInfo->_GetExplicitTargetMask();
        }

        ++l_Count;
    }
    while (l_Result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u spellinfo overrides in %u ms", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

void SpellMgr::LoadAuraScripts(bool p_Reload)
{
    uint32 oldMSTime = getMSTime();

    mAuraScriptMap.clear();                            // need for reload case

    if (!p_Reload)
    {
        PreparedStatement* l_Statement = WorldDatabase.GetPreparedStatement(WORLD_DEL_AURA_SCRIPTS);
        WorldDatabase.Query(l_Statement);
    }

    PreparedStatement* l_Statement = WorldDatabase.GetPreparedStatement(WORLD_SEL_AURA_SCRIPTS);
    PreparedQueryResult l_Result = WorldDatabase.Query(l_Statement);

    if (!l_Result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 aura scripts. DB table `aura_scripts` is empty!");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* l_Fields = l_Result->Fetch();

        uint8 l_Itr = 0;
        uint32 spellId = l_Fields[l_Itr++].GetUInt32();

        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
        if (!spellInfo)
        {
            sLog->outError(LOG_FILTER_SQL, "Failed to load script for (spell_id:%d) : spell does not exist in `Spell.db2`.", spellId);
            continue;
        }

        AuraScriptEntry l_Entry;

        l_Entry.SpellId                 = spellId;
        l_Entry.Hook                    = l_Fields[l_Itr++].GetUInt8();
        l_Entry.EffectId                = l_Fields[l_Itr++].GetInt8();
        l_Entry.Action                  = l_Fields[l_Itr++].GetUInt8();
        l_Entry.ActionSpellId           = l_Fields[l_Itr++].GetUInt32();
        l_Entry.TargetSpellId           = l_Fields[l_Itr++].GetUInt32();
        l_Entry.TargetEffectId          = l_Fields[l_Itr++].GetInt8();
        l_Entry.TargetAuraType          = l_Fields[l_Itr++].GetUInt32();
        l_Entry.ActionCaster            = l_Fields[l_Itr++].GetUInt32();
        l_Entry.ActionOriginalCaster    = l_Fields[l_Itr++].GetUInt32();
        l_Entry.ActionTarget            = l_Fields[l_Itr++].GetUInt32();
        l_Entry.Triggered               = l_Fields[l_Itr++].GetBool();

        if (char const* l_Str = l_Fields[l_Itr++].GetCString())
        {
            Tokenizer l_Tokens(l_Str, ' ');

            for (uint8 l_I = 0; l_I < l_Tokens.size(); ++l_I)
            {
                if (uint32 l_Value = atoi(l_Tokens[l_I]))
                    l_Entry.AuraState.insert(l_Value);
            }
        }

        if (char const* l_Str = l_Fields[l_Itr++].GetCString())
        {
            Tokenizer l_Tokens(l_Str, ' ');

            for (uint8 l_I = 0; l_I < l_Tokens.size(); ++l_I)
            {
                if (uint32 l_Value = atoi(l_Tokens[l_I]))
                    l_Entry.ExcludeAuraState.insert(l_Value);
            }
        }

        if (char const* l_Str = l_Fields[l_Itr++].GetCString())
        {
            Tokenizer l_Tokens(l_Str, ' ');

            for (uint8 l_I = 0; l_I < l_Tokens.size(); ++l_I)
            {
                if (uint32 l_Value = atoi(l_Tokens[l_I]))
                    l_Entry.TargetAuraState.insert(l_Value);
            }
        }

        if (char const* l_Str = l_Fields[l_Itr++].GetCString())
        {
            Tokenizer l_Tokens(l_Str, ' ');

            for (uint8 l_I = 0; l_I < l_Tokens.size(); ++l_I)
            {
                if (uint32 l_Value = atoi(l_Tokens[l_I]))
                    l_Entry.TargetExcludeAuraState.insert(l_Value);
            }
        }

        l_Entry.CalculationType         = l_Fields[l_Itr++].GetUInt8();
        l_Entry.DataSource              = l_Fields[l_Itr++].GetUInt8();
        l_Entry.DataEffectId            = l_Fields[l_Itr++].GetInt8();
        l_Entry.ArtifactTraitId         = l_Fields[l_Itr++].GetUInt32();
        l_Entry.DataStat                = l_Fields[l_Itr++].GetInt8();

        if (char const* l_Str = l_Fields[l_Itr++].GetCString())
        {
            Tokenizer l_Tokens(l_Str, ' ');

            for (uint8 l_I = 0; l_I < l_Tokens.size(); ++l_I)
            {
                if (uint32 l_Value = atoi(l_Tokens[l_I]))
                    l_Entry.ActionSpellList.insert(l_Value);
            }
        }

        mAuraScriptMap.insert(AuraScriptMap::value_type(l_Entry.SpellId, l_Entry));

        ++count;
    }
    while (l_Result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u aura scripts in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::LoadSpellScripts(bool p_Reload)
{
    uint32 oldMSTime = getMSTime();

    mSpellScriptMap.clear();                            // need for reload case

    if (!p_Reload)
    {
        PreparedStatement* l_Statement = WorldDatabase.GetPreparedStatement(WORLD_DEL_SPELL_SCRIPTING);
        WorldDatabase.Query(l_Statement);
    }

    PreparedStatement* l_Statement = WorldDatabase.GetPreparedStatement(WORLD_SEL_SPELL_SCRIPTING);
    PreparedQueryResult l_Result = WorldDatabase.Query(l_Statement);

    if (!l_Result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 spell scripts. DB table `aura_scripts` is empty!");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* l_Fields = l_Result->Fetch();

        uint8 l_Itr = 0;
        uint32 spellId = l_Fields[l_Itr++].GetUInt32();

        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
        if (!spellInfo)
        {
            sLog->outError(LOG_FILTER_SQL, "Failed to load script for (spell_id:%d) : spell does not exist in `Spell.db2`.", spellId);
            continue;
        }

        SpellScriptEntry l_Entry;

        l_Entry.SpellId                 = spellId;
        l_Entry.Hook                    = l_Fields[l_Itr++].GetUInt8();
        l_Entry.EffectId                = l_Fields[l_Itr++].GetInt8();
        l_Entry.Action                  = l_Fields[l_Itr++].GetUInt8();
        l_Entry.ActionSpellId           = l_Fields[l_Itr++].GetUInt32();
        l_Entry.TargetSpellId           = l_Fields[l_Itr++].GetUInt32();
        l_Entry.TargetEffectId          = l_Fields[l_Itr++].GetInt8();
        l_Entry.TargetAuraType          = l_Fields[l_Itr++].GetUInt32();
        l_Entry.ActionCaster            = l_Fields[l_Itr++].GetUInt32();
        l_Entry.ActionOriginalCaster    = l_Fields[l_Itr++].GetUInt32();
        l_Entry.ActionTarget            = l_Fields[l_Itr++].GetUInt32();
        l_Entry.Triggered               = l_Fields[l_Itr++].GetBool();

        if (l_Fields[l_Itr].GetCString())
        {
            Tokenizer l_Tokens(l_Fields[l_Itr++].GetCString(), ' ');

            for (uint8 l_I = 0; l_I < l_Tokens.size(); ++l_I)
            {
                if (uint32 l_Value = atoi(l_Tokens[l_I]))
                    l_Entry.AuraState.insert(l_Value);
            }
        }

        if (l_Fields[l_Itr].GetCString())
        {
            Tokenizer l_Tokens(l_Fields[l_Itr++].GetCString(), ' ');

            for (uint8 l_I = 0; l_I < l_Tokens.size(); ++l_I)
            {
                if (uint32 l_Value = atoi(l_Tokens[l_I]))
                    l_Entry.ExcludeAuraState.insert(l_Value);
            }
        }

        if (l_Fields[l_Itr].GetCString())
        {
            Tokenizer l_Tokens(l_Fields[l_Itr++].GetCString(), ' ');

            for (uint8 l_I = 0; l_I < l_Tokens.size(); ++l_I)
            {
                if (uint32 l_Value = atoi(l_Tokens[l_I]))
                    l_Entry.TargetAuraState.insert(l_Value);
            }
        }

        if (l_Fields[l_Itr].GetCString())
        {
            Tokenizer l_Tokens(l_Fields[l_Itr++].GetCString(), ' ');

            for (uint8 l_I = 0; l_I < l_Tokens.size(); ++l_I)
            {
                if (uint32 l_Value = atoi(l_Tokens[l_I]))
                    l_Entry.TargetExcludeAuraState.insert(l_Value);
            }
        }

        l_Entry.CalculationType         = l_Fields[l_Itr++].GetUInt8();
        l_Entry.DataSource              = l_Fields[l_Itr++].GetUInt8();
        l_Entry.DataEffectId            = l_Fields[l_Itr++].GetInt8();
        l_Entry.ArtifactTraitId         = l_Fields[l_Itr++].GetUInt32();
        l_Entry.DataStat                = l_Fields[l_Itr++].GetInt8();

        if (l_Fields[l_Itr].GetCString())
        {
            Tokenizer l_Tokens(l_Fields[l_Itr++].GetCString(), ' ');

            for (uint8 l_I = 0; l_I < l_Tokens.size(); ++l_I)
            {
                if (uint32 l_Value = atoi(l_Tokens[l_I]))
                    l_Entry.ActionSpellList.insert(l_Value);
            }
        }

        mSpellScriptMap.insert(SpellScriptMap::value_type(l_Entry.SpellId, l_Entry));

        ++count;
    }
    while (l_Result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u aura scripts in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::LoadSpellProcEvents()
{
    uint32 oldMSTime = getMSTime();

    mSpellProcEventMap.clear();                             // need for reload case
    mSpellProcEventMap.resize(300000, nullptr);

    //                                                0      1           2                3                 4                 5                 6                   7           8        9         10         11        12
    QueryResult result = WorldDatabase.Query("SELECT entry, SchoolMask, SpellFamilyName, SpellFamilyMask0, SpellFamilyMask1, SpellFamilyMask2, spellFamilyMask3, procFlags, procEx, ppmRate, CustomChance, Cooldown, Active FROM spell_proc_event");
    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 spell proc event conditions. DB table `spell_proc_event` is empty.");
        return;
    }

    uint32 count = 0;
    uint32 customProc = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();

        SpellInfo const* spell = GetSpellInfo(entry);
        if (!spell)
        {
            WorldDatabase.PExecute("DELETE FROM spell_proc_event WHERE entry = %u;", entry);
            sLog->outError(LOG_FILTER_SQL, "Spell %u listed in `spell_proc_event` does not exist", entry);
            continue;
        }

        SpellProcEventEntry* spe = new SpellProcEventEntry();

        spe->schoolMask          = fields[1].GetInt8();
        spe->spellFamilyName     = fields[2].GetUInt16();
        spe->spellFamilyMask[0]  = fields[3].GetUInt32();
        spe->spellFamilyMask[1]  = fields[4].GetUInt32();
        spe->spellFamilyMask[2]  = fields[5].GetUInt32();
        spe->spellFamilyMask[3]  = fields[6].GetUInt32();
        spe->procFlags           = fields[7].GetUInt32();
        spe->procEx              = fields[8].GetUInt32();
        spe->ppmRate             = fields[9].GetFloat();
        spe->customChance        = fields[10].GetFloat();
        spe->cooldown            = fields[11].GetUInt32();
        spe->Active              = fields[12].GetBool();

        mSpellProcEventMap[entry] = spe;

        if (spell->ProcFlags == 0)
        {
            if (!spell->IsPeriodic() || spe->procFlags == 0)
            {
                sLog->outError(LOG_FILTER_SQL, "Spell %u listed in `spell_proc_event` probally not triggered spell", entry);
                continue;
            }
            customProc++;
        }
        ++count;
    }
    while (result->NextRow());

    if (customProc)
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u extra and %u custom spell proc event conditions in %u ms",  count, customProc, GetMSTimeDiffToNow(oldMSTime));
    else
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u extra spell proc event conditions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));

}

void SpellMgr::LoadSpellProcs()
{
    uint32 oldMSTime = getMSTime();

    mSpellProcMap.clear();                             // need for reload case

    //                                                 0        1           2                3                 4                 5                 6         7              8               9        10              11             12      13        14
    QueryResult result = WorldDatabase.Query("SELECT spellId, schoolMask, spellFamilyName, spellFamilyMask0, spellFamilyMask1, spellFamilyMask2, typeMask, spellTypeMask, spellPhaseMask, hitMask, attributesMask, ratePerMinute, chance, cooldown, charges FROM spell_proc");
    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 spell proc conditions and data. DB table `spell_proc` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        int32 spellId = fields[0].GetInt32();

        bool allRanks = false;
        if (spellId <=0)
        {
            allRanks = true;
            spellId = -spellId;
        }

        SpellInfo const* spellEntry = GetSpellInfo(spellId);
        if (!spellEntry)
        {
            sLog->outError(LOG_FILTER_SQL, "Spell %u listed in `spell_proc` does not exist", spellId);
            continue;
        }

        if (allRanks)
        {
            if (GetFirstSpellInChain(spellId) != uint32(spellId))
            {
                sLog->outError(LOG_FILTER_SQL, "Spell %u listed in `spell_proc` is not first rank of spell.", fields[0].GetInt32());
                continue;
            }
        }

        SpellProcEntry baseProcEntry;

        baseProcEntry.schoolMask      = fields[1].GetInt8();
        baseProcEntry.spellFamilyName = fields[2].GetUInt16();
        baseProcEntry.spellFamilyMask[0] = fields[3].GetUInt32();
        baseProcEntry.spellFamilyMask[1] = fields[4].GetUInt32();
        baseProcEntry.spellFamilyMask[2] = fields[5].GetUInt32();
        baseProcEntry.typeMask        = fields[6].GetUInt32();
        baseProcEntry.spellTypeMask   = fields[7].GetUInt32();
        baseProcEntry.spellPhaseMask  = fields[8].GetUInt32();
        baseProcEntry.hitMask         = fields[9].GetUInt32();
        baseProcEntry.attributesMask  = fields[10].GetUInt32();
        baseProcEntry.ratePerMinute   = fields[11].GetFloat();
        baseProcEntry.chance          = fields[12].GetFloat();
        float cooldown                = fields[13].GetFloat();
        baseProcEntry.cooldown        = uint32(cooldown);
        baseProcEntry.charges         = fields[14].GetUInt32();

        while (true)
        {
            if (mSpellProcMap.find(spellId) != mSpellProcMap.end())
            {
                sLog->outError(LOG_FILTER_SQL, "Spell %u listed in `spell_proc` has duplicate entry in the table", spellId);
                break;
            }
            SpellProcEntry procEntry = SpellProcEntry(baseProcEntry);

            // take defaults from dbcs
            if (!procEntry.typeMask)
                procEntry.typeMask = spellEntry->ProcFlags;
            if (!procEntry.charges)
                procEntry.charges = spellEntry->ProcCharges;
            if (!procEntry.chance && !procEntry.ratePerMinute)
                procEntry.chance = float(spellEntry->ProcChance);

            // validate data
            if (procEntry.schoolMask & ~SPELL_SCHOOL_MASK_ALL)
                sLog->outError(LOG_FILTER_SQL, "`spell_proc` table entry for spellId %u has wrong `schoolMask` set: %u", spellId, procEntry.schoolMask);
            if (procEntry.spellFamilyName && (procEntry.spellFamilyName < 3 || procEntry.spellFamilyName > 17 || procEntry.spellFamilyName == 14 || procEntry.spellFamilyName == 16))
                sLog->outError(LOG_FILTER_SQL, "`spell_proc` table entry for spellId %u has wrong `spellFamilyName` set: %u", spellId, procEntry.spellFamilyName);
            if (procEntry.chance < 0)
            {
                sLog->outError(LOG_FILTER_SQL, "`spell_proc` table entry for spellId %u has negative value in `chance` field", spellId);
                procEntry.chance = 0;
            }
            if (procEntry.ratePerMinute < 0)
            {
                sLog->outError(LOG_FILTER_SQL, "`spell_proc` table entry for spellId %u has negative value in `ratePerMinute` field", spellId);
                procEntry.ratePerMinute = 0;
            }
            if (cooldown < 0)
            {
                sLog->outError(LOG_FILTER_SQL, "`spell_proc` table entry for spellId %u has negative value in `cooldown` field", spellId);
                procEntry.cooldown = 0;
            }
            if (procEntry.chance == 0 && procEntry.ratePerMinute == 0)
                sLog->outError(LOG_FILTER_SQL, "`spell_proc` table entry for spellId %u doesn't have `chance` and `ratePerMinute` values defined, proc will not be triggered", spellId);
            if (procEntry.charges > 99)
            {
                sLog->outError(LOG_FILTER_SQL, "`spell_proc` table entry for spellId %u has too big value in `charges` field", spellId);
                procEntry.charges = 99;
            }
            if (!procEntry.typeMask)
                sLog->outError(LOG_FILTER_SQL, "`spell_proc` table entry for spellId %u doesn't have `typeMask` value defined, proc will not be triggered", spellId);
            if (procEntry.spellTypeMask & ~PROC_SPELL_PHASE_MASK_ALL)
                sLog->outError(LOG_FILTER_SQL, "`spell_proc` table entry for spellId %u has wrong `spellTypeMask` set: %u", spellId, procEntry.spellTypeMask);
            if (procEntry.spellTypeMask && !(procEntry.typeMask & (SPELL_PROC_FLAG_MASK | PERIODIC_PROC_FLAG_MASK)))
                sLog->outError(LOG_FILTER_SQL, "`spell_proc` table entry for spellId %u has `spellTypeMask` value defined, but it won't be used for defined `typeMask` value", spellId);
            if (!procEntry.spellPhaseMask && procEntry.typeMask & REQ_SPELL_PHASE_PROC_FLAG_MASK)
                sLog->outError(LOG_FILTER_SQL, "`spell_proc` table entry for spellId %u doesn't have `spellPhaseMask` value defined, but it's required for defined `typeMask` value, proc will not be triggered", spellId);
            if (procEntry.spellPhaseMask & ~PROC_SPELL_PHASE_MASK_ALL)
                sLog->outError(LOG_FILTER_SQL, "`spell_proc` table entry for spellId %u has wrong `spellPhaseMask` set: %u", spellId, procEntry.spellPhaseMask);
            if (procEntry.spellPhaseMask && !(procEntry.typeMask & REQ_SPELL_PHASE_PROC_FLAG_MASK))
                sLog->outError(LOG_FILTER_SQL, "`spell_proc` table entry for spellId %u has `spellPhaseMask` value defined, but it won't be used for defined `typeMask` value", spellId);
            if (procEntry.hitMask & ~PROC_HIT_MASK_ALL)
                sLog->outError(LOG_FILTER_SQL, "`spell_proc` table entry for spellId %u has wrong `hitMask` set: %u", spellId, procEntry.hitMask);
            if (procEntry.hitMask && !(procEntry.typeMask & TAKEN_HIT_PROC_FLAG_MASK || (procEntry.typeMask & DONE_HIT_PROC_FLAG_MASK && (!procEntry.spellPhaseMask || procEntry.spellPhaseMask & (PROC_SPELL_PHASE_HIT | PROC_SPELL_PHASE_FINISH)))))
                sLog->outError(LOG_FILTER_SQL, "`spell_proc` table entry for spellId %u has `hitMask` value defined, but it won't be used for defined `typeMask` and `spellPhaseMask` values", spellId);

            mSpellProcMap[spellId] = procEntry;

            if (allRanks)
            {
                spellId = GetNextSpellInChain(spellId);
                spellEntry = GetSpellInfo(spellId);
            }
            else
                break;
        }
        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u spell proc conditions and data in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::LoadSpellBonusess()
{
    uint32 oldMSTime = getMSTime();

    mSpellBonusMap.clear();                             // need for reload case

    //                                                0      1             2          3         4
    QueryResult result = WorldDatabase.Query("SELECT entry, direct_bonus, dot_bonus, ap_bonus, ap_dot_bonus FROM spell_bonus_data");
    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 spell bonus data. DB table `spell_bonus_data` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();
        uint32 entry = fields[0].GetUInt32();

        SpellInfo const* spell = GetSpellInfo(entry);
        if (!spell)
        {
            WorldDatabase.PExecute("DELETE FROM spell_bonus_data WHERE entry = %u;", entry);
            sLog->outError(LOG_FILTER_SQL, "Spell %u listed in `spell_bonus_data` does not exist", entry);
            continue;
        }

        SpellBonusEntry& sbe = mSpellBonusMap[entry];
        sbe.direct_damage = fields[1].GetFloat();
        sbe.dot_damage    = fields[2].GetFloat();
        sbe.ap_bonus      = fields[3].GetFloat();
        sbe.ap_dot_bonus   = fields[4].GetFloat();

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u extra spell bonus data in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::LoadSpellThreats()
{
    uint32 oldMSTime = getMSTime();

    mSpellThreatMap.clear();                                // need for reload case

    //                                                0      1        2       3
    QueryResult result = WorldDatabase.Query("SELECT entry, flatMod, pctMod, apPctMod FROM spell_threat");
    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 aggro generating spells. DB table `spell_threat` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();

        if (!GetSpellInfo(entry))
        {
            sLog->outError(LOG_FILTER_SQL, "Spell %u listed in `spell_threat` does not exist", entry);
            continue;
        }

        SpellThreatEntry ste;
        ste.flatMod  = fields[1].GetInt32();
        ste.pctMod   = fields[2].GetFloat();
        ste.apPctMod = fields[3].GetFloat();

        mSpellThreatMap[entry] = ste;
        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u SpellThreatEntries in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::LoadSkillLineAbilityMap()
{
    uint32 oldMSTime = getMSTime();

    mSkillLineAbilityMap.clear();

    uint32 count = 0;

    for (uint32 i = 0; i < sSkillLineAbilityStore.GetNumRows(); ++i)
    {
        SkillLineAbilityEntry const* SkillInfo = sSkillLineAbilityStore.LookupEntry(i);
        if (!SkillInfo)
            continue;

        mSkillLineAbilityMap.insert(SkillLineAbilityMap::value_type(SkillInfo->spellId, SkillInfo));
        ++count;
    }

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u SkillLineAbility MultiMap Data in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::LoadSpellPetAuras()
{
    uint32 oldMSTime = getMSTime();

    mSpellPetAuraMap.clear();                                  // need for reload case

    //                                                  0       1       2    3
    QueryResult result = WorldDatabase.Query("SELECT spell, effectId, pet, aura FROM spell_pet_auras");
    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 spell pet auras. DB table `spell_pet_auras` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 spell = fields[0].GetUInt32();
        uint8 eff = fields[1].GetUInt8();
        uint32 pet = fields[2].GetUInt32();
        uint32 aura = fields[3].GetUInt32();

        SpellPetAuraMap::iterator itr = mSpellPetAuraMap.find((spell<<8) + eff);
        if (itr != mSpellPetAuraMap.end())
            itr->second.AddAura(pet, aura);
        else
        {
            SpellInfo const* spellInfo = GetSpellInfo(spell);
            if (!spellInfo)
            {
                sLog->outError(LOG_FILTER_SQL, "Spell %u listed in `spell_pet_auras` does not exist", spell);
                continue;
            }
            if (spellInfo->Effects[eff].Effect != SPELL_EFFECT_DUMMY &&
                (spellInfo->Effects[eff].IsApplyingAura()) &&
                spellInfo->Effects[eff].ApplyAuraName != SPELL_AURA_DUMMY)
            {
                sLog->outError(LOG_FILTER_SPELLS_AURAS, "Spell %u listed in `spell_pet_auras` does not have dummy aura or dummy effect", spell);
                continue;
            }

            SpellInfo const* spellInfo2 = GetSpellInfo(aura);
            if (!spellInfo2)
            {
                sLog->outError(LOG_FILTER_SQL, "Aura %u listed in `spell_pet_auras` does not exist", aura);
                continue;
            }

            PetAura pa(pet, aura, spellInfo->Effects[eff].TargetA.GetTarget() == TARGET_UNIT_PET, spellInfo->Effects[eff].CalcValue());
            mSpellPetAuraMap[(spell<<8) + eff] = pa;
        }

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u spell pet auras in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

bool IsCCSpell(SpellInfo const* p_SpellProto)
{
    if (p_SpellProto->SpellFamilyName == SPELLFAMILY_HUNTER ||
        p_SpellProto->SpellFamilyName == SPELLFAMILY_GENERIC)
        return false;

    for (uint8 l_EffectIndex = 0; l_EffectIndex < p_SpellProto->EffectCount; l_EffectIndex++)
    {
        switch (p_SpellProto->Effects[l_EffectIndex].ApplyAuraName)
        {
            case SPELL_AURA_MOD_CONFUSE:
            case SPELL_AURA_MOD_FEAR:
            case SPELL_AURA_MOD_STUN:
            case SPELL_AURA_MOD_ROOT:
            case SPELL_AURA_TRANSFORM:
                if (!p_SpellProto->IsPositiveEffect(l_EffectIndex))
                    return true;
                break;

            default:
                break;
        }
    }

    return false;
}

// Fill custom data about enchantments
void SpellMgr::LoadEnchantCustomAttr()
{
    uint32 oldMSTime = getMSTime();

    uint32 size = sSpellItemEnchantmentStore.GetNumRows();
    mEnchantCustomAttr.resize(size);

    for (uint32 i = 0; i < size; ++i)
        mEnchantCustomAttr[i] = 0;

    uint32 count = 0;
    for (uint32 i = 0; i < GetSpellInfoStoreSize(); ++i)
    {
        SpellInfo const* spellInfo = GetSpellInfo(i);
        if (!spellInfo)
            continue;

        // TODO: find a better check
        if (!(spellInfo->AttributesEx2 & SPELL_ATTR2_PRESERVE_ENCHANT_IN_ARENA) || !(spellInfo->Attributes & SPELL_ATTR0_NOT_SHAPESHIFT))
            continue;

        for (uint8 j = 0; j < spellInfo->EffectCount; ++j)
        {
            if (spellInfo->Effects[j].Effect == SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY)
            {
                uint32 enchId = spellInfo->Effects[j].MiscValue;
                SpellItemEnchantmentEntry const* ench = sSpellItemEnchantmentStore.LookupEntry(enchId);
                if (!ench)
                    continue;
                mEnchantCustomAttr[enchId] = true;
                ++count;
                break;
            }
        }
    }

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u custom enchant attributes in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::LoadSpellPlaceHolder()
{
    uint32 oldMSTime = getMSTime();
    uint32 count = 0;

    for (uint32 i = 0; i < GetSpellInfoStoreSize(); ++i)
    {
        SpellInfo const* spellInfo = GetSpellInfo(i);
        if (!spellInfo)
            continue;

        if (spellInfo->AttributesEx8 & SPELL_ATTR8_UNK13 && spellInfo->AttributesEx10 & SPELL_ATTR10_UNK15)
        {
            mPlaceHolderSpells.insert(spellInfo->Id);
            ++count;
        }
    }

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u talent place holder in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::LoadSpellEnchantProcData()
{
    uint32 oldMSTime = getMSTime();

    mSpellEnchantProcEventMap.clear();                             // need for reload case

    //                                                  0         1           2         3
    QueryResult result = WorldDatabase.Query("SELECT entry, customChance, PPMChance, procEx FROM spell_enchant_proc_data");
    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 spell enchant proc event conditions. DB table `spell_enchant_proc_data` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 enchantId = fields[0].GetUInt32();

        SpellItemEnchantmentEntry const* ench = sSpellItemEnchantmentStore.LookupEntry(enchantId);
        if (!ench)
        {
            sLog->outError(LOG_FILTER_SQL, "Enchantment %u listed in `spell_enchant_proc_data` does not exist", enchantId);
            continue;
        }

        SpellEnchantProcEntry spe;

        spe.customChance = fields[1].GetUInt32();
        spe.PPMChance = fields[2].GetFloat();
        spe.procEx = fields[3].GetUInt32();

        mSpellEnchantProcEventMap[enchantId] = spe;

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u enchant proc data definitions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::LoadSpellLinked()
{
    uint32 oldMSTime = getMSTime();

    mSpellLinkedMap.clear();    // need for reload case

                                //                                                0              1             2      3       4         5          6         7        8       9         10        11          12         13           14             15          16         17        18         19          20         21
    QueryResult result = WorldDatabase.Query("SELECT spell_trigger, spell_effect, type, caster, target, hastalent, hastalent2, chance, cooldown, hastype, hitmask, removeMask, hastype2, actiontype, targetCount, targetCountType, `group`, `duration`, `param`, effectMask , hasparam , hasparam2 FROM spell_linked_spell");
    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 linked spells. DB table `spell_linked_spell` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();
        uint8 l_Index = 0;

        int32 trigger           = fields[l_Index++].GetInt32();
        int32 effect            = fields[l_Index++].GetInt32();
        int32 type              = fields[l_Index++].GetUInt8();
        int32 caster            = fields[l_Index++].GetUInt8();
        int32 target            = fields[l_Index++].GetUInt8();
        int32 hastalent         = fields[l_Index++].GetInt32();
        int32 hastalent2        = fields[l_Index++].GetInt32();
        int32 chance            = fields[l_Index++].GetInt32();
        int32 cooldown          = fields[l_Index++].GetInt32();
        int32 hastype           = fields[l_Index++].GetUInt8();
        uint32 hitmask          = fields[l_Index++].GetUInt32();
        int32 removeMask        = fields[l_Index++].GetInt32();
        int32 hastype2          = fields[l_Index++].GetInt8();
        int32 actiontype        = fields[l_Index++].GetInt8();
        int8 targetCount        = fields[l_Index++].GetInt8();
        int8 targetCountType    = fields[l_Index++].GetInt8();
        int8 group              = fields[l_Index++].GetInt8();
        int32 duration          = fields[l_Index++].GetInt32();
        float param             = fields[l_Index++].GetFloat();
        uint32 effectMask       = fields[l_Index++].GetUInt32();
        uint32 hasparam         = fields[l_Index++].GetInt32();
        uint32 hasparam2        = fields[l_Index++].GetInt32();

        SpellInfo const* spellInfo = GetSpellInfo(abs(trigger));
        if (!spellInfo)
        {
            sLog->outError(LOG_FILTER_SQL, "Spell %u listed in `spell_linked_spell` does not exist", abs(trigger));
            continue;
        }
        spellInfo = GetSpellInfo(abs(effect));
        if (!spellInfo)
        {
            sLog->outError(LOG_FILTER_SQL, "Spell %u listed in `spell_linked_spell` does not exist", abs(effect));
            continue;
        }

        if (type) //we will find a better way when more types are needed
        {
            if (trigger > 0)
                trigger += SPELL_LINKED_MAX_SPELLS * type;
            else
                trigger -= SPELL_LINKED_MAX_SPELLS * type;
        }

        SpellLinked templink;
        templink.effect = effect;
        templink.hastalent = hastalent;
        templink.hastalent2 = hastalent2;
        templink.chance = chance;
        templink.cooldown = cooldown;
        templink.hastype = hastype;
        templink.caster = caster;
        templink.target = target;
        templink.hitmask = hitmask;
        templink.removeMask = removeMask;
        templink.hastype2 = hastype2;
        templink.actiontype = actiontype;
        templink.targetCount = targetCount;
        templink.targetCountType = targetCountType;
        templink.group = group;
        templink.duration = duration;
        templink.param = param;
        templink.effectMask = effectMask;
        templink.hasparam = hasparam;
        templink.hasparam2 = hasparam2;

        mSpellLinkedMap[trigger].push_back(templink);

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u linked spells in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::LoadPetLevelupSpellMap()
{
    uint32 oldMSTime = getMSTime();

    mPetLevelupSpellMap.clear();                                   // need for reload case

    uint32 count = 0;
    uint32 family_count = 0;

    for (uint32 i = 0; i < sCreatureFamilyStore.GetNumRows(); ++i)
    {
        CreatureFamilyEntry const* creatureFamily = sCreatureFamilyStore.LookupEntry(i);
        if (!creatureFamily)                                     // not exist
            continue;

        for (uint8 j = 0; j < 2; ++j)
        {
            if (!creatureFamily->skillLine[j])
                continue;

            for (uint32 k = 0; k < sSkillLineAbilityStore.GetNumRows(); ++k)
            {
                SkillLineAbilityEntry const* skillLine = sSkillLineAbilityStore.LookupEntry(k);
                if (!skillLine)
                    continue;

                if (skillLine->SkillLine != creatureFamily->skillLine[j])
                    continue;

                if ((skillLine->Flags & SKILL_LINE_ABILITY_LEARNED_ON_SKILL_LEARN) == 0)
                    continue;

                SpellInfo const* spell = GetSpellInfo(skillLine->spellId);
                if (!spell) // not exist or triggered or talent
                    continue;

                if (!spell->SpellLevel)
                    continue;

                PetLevelupSpellSet& spellSet = mPetLevelupSpellMap[creatureFamily->ID];
                if (spellSet.empty())
                    ++family_count;

                spellSet.insert(PetLevelupSpellSet::value_type(spell->SpellLevel, spell->Id));
                ++count;
            }
        }
    }

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u pet levelup and default spells for %u families in %u ms", count, family_count, GetMSTimeDiffToNow(oldMSTime));
}

bool LoadPetDefaultSpells_helper(CreatureTemplate const* cInfo, PetDefaultSpellsEntry& petDefSpells)
{
    // skip empty list;
    bool have_spell = false;
    for (uint8 j = 0; j < MAX_CREATURE_SPELL_DATA_SLOT; ++j)
    {
        if (petDefSpells.spellid[j])
        {
            have_spell = true;
            break;
        }
    }
    if (!have_spell)
        return false;

    // remove duplicates with levelupSpells if any
    if (PetLevelupSpellSet const* levelupSpells = cInfo->family ? sSpellMgr->GetPetLevelupSpellList(cInfo->family) : NULL)
    {
        for (uint8 j = 0; j < MAX_CREATURE_SPELL_DATA_SLOT; ++j)
        {
            if (!petDefSpells.spellid[j])
                continue;

            for (PetLevelupSpellSet::const_iterator itr = levelupSpells->begin(); itr != levelupSpells->end(); ++itr)
            {
                if (itr->second == petDefSpells.spellid[j])
                {
                    petDefSpells.spellid[j] = 0;
                    break;
                }
            }
        }
    }

    // skip empty list;
    have_spell = false;
    for (uint8 j = 0; j < MAX_CREATURE_SPELL_DATA_SLOT; ++j)
    {
        if (petDefSpells.spellid[j])
        {
            have_spell = true;
            break;
        }
    }

    return have_spell;
}

void SpellMgr::LoadPetDefaultSpells()
{
    uint32 oldMSTime = getMSTime();

    mPetDefaultSpellsMap.clear();

    uint32 countCreature = 0;
    uint32 countData = 0;

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded addition spells for %u pet spell data entries in %u ms", countData, GetMSTimeDiffToNow(oldMSTime));

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, "Loading summonable creature templates...");
    oldMSTime = getMSTime();

    // different summon spells
    for (uint32 i = 0; i < GetSpellInfoStoreSize(); ++i)
    {
        SpellInfo const* spellEntry = GetSpellInfo(i);
        if (!spellEntry)
            continue;

        for (uint8 k = 0; k < spellEntry->EffectCount; ++k)
        {
            if (spellEntry->Effects[k].Effect == SPELL_EFFECT_SUMMON || spellEntry->Effects[k].Effect == SPELL_EFFECT_SUMMON_PET)
            {
                uint32 creature_id = spellEntry->Effects[k].MiscValue;
                CreatureTemplate const* cInfo = sObjectMgr->GetCreatureTemplate(creature_id);
                if (!cInfo)
                    continue;

                /// Get default pet spells from creature_template
                int32 petSpellsId = cInfo->Entry;
                if (mPetDefaultSpellsMap.find(cInfo->Entry) != mPetDefaultSpellsMap.end())
                    continue;

                PetDefaultSpellsEntry petDefSpells;
                for (uint8 j = 0; j < MAX_CREATURE_SPELL_DATA_SLOT; ++j)
                    petDefSpells.spellid[j] = cInfo->spells[j];

                if (LoadPetDefaultSpells_helper(cInfo, petDefSpells))
                {
                    mPetDefaultSpellsMap[petSpellsId] = petDefSpells;
                    ++countCreature;
                }
            }
        }
    }

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u summonable creature templates in %u ms", countCreature, GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::LoadSpellAreas()
{
    uint32 oldMSTime = getMSTime();

    mSpellAreaMap.clear();                                  // need for reload case
    mSpellAreaForQuestMap.clear();
    mSpellAreaForActiveQuestMap.clear();
    mSpellAreaForQuestEndMap.clear();
    mSpellAreaForAuraMap.clear();

    //                                                  0     1         2              3               4                 5          6          7       8         9
    QueryResult result = WorldDatabase.Query("SELECT spell, area, quest_start, quest_start_status, quest_end_status, quest_end, aura_spell, racemask, gender, autocast FROM spell_area");
    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 spell area requirements. DB table `spell_area` is empty.");

        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 spell = fields[0].GetUInt32();
        SpellArea spellArea;
        spellArea.spellId             = spell;
        spellArea.areaId              = fields[1].GetUInt32();
        spellArea.questStart          = fields[2].GetUInt32();
        spellArea.questStartStatus    = fields[3].GetUInt32();
        spellArea.questEndStatus      = fields[4].GetUInt32();
        spellArea.questEnd            = fields[5].GetUInt32();
        spellArea.auraSpell           = fields[6].GetInt32();
        spellArea.raceMask            = fields[7].GetUInt64();
        spellArea.gender              = Gender(fields[8].GetUInt8());
        spellArea.autocast            = fields[9].GetBool();

        if (SpellInfo const* spellInfo = GetSpellInfo(spell))
        {
            if (spellArea.autocast)
                const_cast<SpellInfo*>(spellInfo)->Attributes |= SPELL_ATTR0_CANT_CANCEL;
        }
        else
        {
            sLog->outError(LOG_FILTER_SQL, "Spell %u listed in `spell_area` does not exist", spell);
            continue;
        }

        {
            bool ok = true;
            SpellAreaMapBounds sa_bounds = GetSpellAreaMapBounds(spellArea.spellId);
            for (SpellAreaMap::const_iterator itr = sa_bounds.first; itr != sa_bounds.second; ++itr)
            {
                if (spellArea.spellId != itr->second.spellId)
                    continue;
                if (spellArea.areaId != itr->second.areaId)
                    continue;
                if (spellArea.questStart != itr->second.questStart)
                    continue;
                if (spellArea.auraSpell != itr->second.auraSpell)
                    continue;
                if ((spellArea.raceMask & itr->second.raceMask) == 0)
                    continue;
                if (spellArea.gender != itr->second.gender)
                    continue;

                // duplicate by requirements
                ok =false;
                break;
            }

            if (!ok)
            {
                sLog->outError(LOG_FILTER_SQL, "Spell %u listed in `spell_area` already listed with similar requirements.", spell);
                continue;
            }
        }

        if (spellArea.areaId && !sAreaTableStore.LookupEntry(spellArea.areaId))
        {
            sLog->outError(LOG_FILTER_SQL, "Spell %u listed in `spell_area` have wrong area (%u) requirement", spell, spellArea.areaId);
            continue;
        }

        if (spellArea.questStart && !sObjectMgr->GetQuestTemplate(spellArea.questStart))
        {
            sLog->outError(LOG_FILTER_SQL, "Spell %u listed in `spell_area` have wrong start quest (%u) requirement", spell, spellArea.questStart);
            continue;
        }

        if (spellArea.questEnd)
        {
            if (!sObjectMgr->GetQuestTemplate(spellArea.questEnd))
            {
                sLog->outError(LOG_FILTER_SQL, "Spell %u listed in `spell_area` have wrong end quest (%u) requirement", spell, spellArea.questEnd);
                continue;
            }
        }

        if (spellArea.auraSpell)
        {
            SpellInfo const* spellInfo = GetSpellInfo(abs(spellArea.auraSpell));
            if (!spellInfo)
            {
                sLog->outError(LOG_FILTER_SQL, "Spell %u listed in `spell_area` have wrong aura spell (%u) requirement", spell, abs(spellArea.auraSpell));
                continue;
            }

            if (uint32(abs(spellArea.auraSpell)) == spellArea.spellId)
            {
                sLog->outError(LOG_FILTER_SQL, "Spell %u listed in `spell_area` have aura spell (%u) requirement for itself", spell, abs(spellArea.auraSpell));
                continue;
            }

            // not allow autocast chains by auraSpell field (but allow use as alternative if not present)
            if (spellArea.autocast && spellArea.auraSpell > 0)
            {
                bool chain = false;
                SpellAreaForAuraMapBounds saBound = GetSpellAreaForAuraMapBounds(spellArea.spellId);
                for (SpellAreaForAuraMap::const_iterator itr = saBound.first; itr != saBound.second; ++itr)
                {
                    if (itr->second->autocast && itr->second->auraSpell > 0)
                    {
                        chain = true;
                        break;
                    }
                }

                if (chain)
                {
                    sLog->outError(LOG_FILTER_SQL, "Spell %u listed in `spell_area` have aura spell (%u) requirement that itself autocast from aura", spell, spellArea.auraSpell);
                    continue;
                }

                SpellAreaMapBounds saBound2 = GetSpellAreaMapBounds(spellArea.auraSpell);
                for (SpellAreaMap::const_iterator itr2 = saBound2.first; itr2 != saBound2.second; ++itr2)
                {
                    if (itr2->second.autocast && itr2->second.auraSpell > 0)
                    {
                        chain = true;
                        break;
                    }
                }

                if (chain)
                {
                    sLog->outError(LOG_FILTER_SQL, "Spell %u listed in `spell_area` have aura spell (%u) requirement that itself autocast from aura", spell, spellArea.auraSpell);
                    continue;
                }
            }
        }

        if (spellArea.raceMask && (spellArea.raceMask & RACEMASK_ALL_PLAYABLE) == 0)
        {
            sLog->outError(LOG_FILTER_SQL, "Spell %u listed in `spell_area` have wrong RaceMask (%lu) requirement", spell, spellArea.raceMask);
            continue;
        }

        if (spellArea.gender != GENDER_NONE && spellArea.gender != GENDER_FEMALE && spellArea.gender != GENDER_MALE)
        {
            sLog->outError(LOG_FILTER_SQL, "Spell %u listed in `spell_area` have wrong gender (%u) requirement", spell, spellArea.gender);
            continue;
        }

        SpellArea const* sa = &mSpellAreaMap.insert(SpellAreaMap::value_type(spell, spellArea))->second;

        // for search by current zone/subzone at zone/subzone change
        if (spellArea.areaId)
            mSpellAreaForAreaMap.insert(SpellAreaForAreaMap::value_type(spellArea.areaId, sa));

        // for search at quest start/reward
        if (spellArea.questStart)
            mSpellAreaForQuestMap.insert(SpellAreaForQuestMap::value_type(spellArea.questStart, sa));

        // for search at quest start/reward
        if (spellArea.questEnd)
            mSpellAreaForQuestEndMap.insert(SpellAreaForQuestMap::value_type(spellArea.questEnd, sa));

        // for search at aura apply
        if (spellArea.auraSpell)
            mSpellAreaForAuraMap.insert(SpellAreaForAuraMap::value_type(abs(spellArea.auraSpell), sa));

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u spell area requirements in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

static const uint32 SkillClass[MAX_CLASSES] = {0, 840, 800, 795, 921, 804, 796, 924, 904, 849, 829, 798, 1848};

void SpellMgr::LoadSpellClassInfo()
{
    mSpellClassInfo.resize(MAX_CLASSES);
    for (int l_ClassID = 0; l_ClassID < MAX_CLASSES; l_ClassID++)
    {
        ChrClassesEntry const* classEntry = sChrClassesStore.LookupEntry(l_ClassID);
        if (!classEntry)
            continue;

        // Player mastery activation
        mSpellClassInfo[l_ClassID].insert(114585);
        /// Learn talent
        mSpellClassInfo[l_ClassID].insert(127650);
        /// Activate Spec
        mSpellClassInfo[l_ClassID].insert(200749);

        // Opening gameobject
        if (l_ClassID == CLASS_MONK)
        {
            mSpellClassInfo[l_ClassID].insert(3365);
            mSpellClassInfo[l_ClassID].insert(6247);
            mSpellClassInfo[l_ClassID].insert(6477);
            mSpellClassInfo[l_ClassID].insert(6478);
            mSpellClassInfo[l_ClassID].insert(21651);
            mSpellClassInfo[l_ClassID].insert(22810);
            mSpellClassInfo[l_ClassID].insert(61437);
            mSpellClassInfo[l_ClassID].insert(68398);
            mSpellClassInfo[l_ClassID].insert(96220);
        }

        for (uint32 l_I = 0; l_I < sSkillLineAbilityStore.GetNumRows(); ++l_I)
        {
            SkillLineAbilityEntry const* l_SkillLine = sSkillLineAbilityStore.LookupEntry(l_I);
            if (!l_SkillLine)
                continue;

            SpellInfo const* l_SpellEntry = sSpellMgr->GetSpellInfo(l_SkillLine->spellId);
            if (!l_SpellEntry)
                continue;

            SkillLineEntry const* l_Skill = sSkillLineStore.LookupEntry(l_SkillLine->SkillLine);
            if (!l_Skill)
                continue;

            if (l_Skill->CategoryID != 6 && l_Skill->CategoryID != 7 && l_Skill->CategoryID != 8)
                continue;

            if ((l_SkillLine->Flags & SKILL_LINE_ABILITY_LEARNED_ON_SKILL_VALUE) == 0)
                continue;

            if ((l_SkillLine->ClassMask & 1 << (l_ClassID - 1)) == 0 && SkillClass[l_ClassID] != l_SkillLine->SkillLine)
                continue;

            if (sSpellMgr->IsTalent(l_SpellEntry->Id))
                continue;

            mSpellClassInfo[l_ClassID].insert(l_SpellEntry->Id);
        }

        for (uint32 l_I = 0; l_I < sSpecializationSpellStore.GetNumRows(); ++l_I)
        {
            SpecializationSpellEntry const* l_SpecializationInfo = sSpecializationSpellStore.LookupEntry(l_I);
            if (!l_SpecializationInfo)
                continue;

            ChrSpecializationsEntry const* l_ChrSpec = sChrSpecializationsStore.LookupEntry(l_SpecializationInfo->SpecializationEntry);
            if (!l_ChrSpec)
                continue;

            mSpellClassInfo[l_ChrSpec->ClassID].insert(l_SpecializationInfo->SpellID);
        }

        for (uint32 l_I = 0; l_I < sMinorTalentStore.GetNumRows(); l_I++)
        {
            MinorTalentEntry const* l_MinorTalent = sMinorTalentStore.LookupEntry(l_I);

            if (!l_MinorTalent)
                continue;

            mSpecializationPerks[l_MinorTalent->specializationID].insert(l_MinorTalent);
        }
    }
}

void SpellMgr::InitializeSpellDifficulty()
{
    mAvaiableDifficultyBySpell.clear();
    mSpellBaseDifficultyDBCs.clear();

    /// SpellAuraOptions
    for (uint32 l_I = 0; l_I < sSpellAuraOptionsStore.GetNumRows(); ++l_I)
    {
        if (SpellAuraOptionsEntry const* l_SpellAuraOption = sSpellAuraOptionsStore.LookupEntry(l_I))
        {
            mAvaiableDifficultyBySpell[l_SpellAuraOption->SpellID].insert(l_SpellAuraOption->DifficultyID);

            if (l_SpellAuraOption->DifficultyID != Difficulty::DifficultyNone)
                mDatastoreSpellDifficultyKey[sSpellAuraOptionsStore.GetDbcFileName()].insert(std::make_pair(std::make_pair(l_SpellAuraOption->SpellID, l_SpellAuraOption->DifficultyID), l_SpellAuraOption->Id));
            else
                mSpellBaseDifficultyDBCs[sSpellAuraOptionsStore.GetDbcFileName()].insert(std::make_pair(std::make_pair(l_SpellAuraOption->SpellID, l_SpellAuraOption->DifficultyID), l_SpellAuraOption->Id));
        }
    }

    /// SpellCategories
    for (uint32 l_I = 0; l_I < sSpellCategoriesStore.GetNumRows(); ++l_I)
    {
        if (SpellCategoriesEntry const* l_SpellCategories = sSpellCategoriesStore.LookupEntry(l_I))
        {
            mAvaiableDifficultyBySpell[l_SpellCategories->SpellID].insert(l_SpellCategories->DifficultyID);

            if (l_SpellCategories->DifficultyID != Difficulty::DifficultyNone)
                mDatastoreSpellDifficultyKey[sSpellCategoriesStore.GetDbcFileName()].insert(std::make_pair(std::make_pair(l_SpellCategories->SpellID, l_SpellCategories->DifficultyID), l_SpellCategories->Id));
            else
                mSpellBaseDifficultyDBCs[sSpellCategoriesStore.GetDbcFileName()].insert(std::make_pair(std::make_pair(l_SpellCategories->SpellID, l_SpellCategories->DifficultyID), l_SpellCategories->Id));
        }
    }

    /// SpellCooldowns
    for (uint32 l_I = 0; l_I < sSpellCooldownsStore.GetNumRows(); ++l_I)
    {
        if (SpellCooldownsEntry const* l_SpellCooldown = sSpellCooldownsStore.LookupEntry(l_I))
        {
            mAvaiableDifficultyBySpell[l_SpellCooldown->SpellID].insert(l_SpellCooldown->DifficultyID);

            if (l_SpellCooldown->DifficultyID != Difficulty::DifficultyNone)
                mDatastoreSpellDifficultyKey[sSpellCooldownsStore.GetDbcFileName()].insert(std::make_pair(std::make_pair(l_SpellCooldown->SpellID, l_SpellCooldown->DifficultyID), l_SpellCooldown->Id));
            else
                mSpellBaseDifficultyDBCs[sSpellCooldownsStore.GetDbcFileName()].insert(std::make_pair(std::make_pair(l_SpellCooldown->SpellID, l_SpellCooldown->DifficultyID), l_SpellCooldown->Id));
        }
    }

    /// SpellEffect
    for (uint32 l_I = 0; l_I < sSpellEffectStore.GetNumRows(); ++l_I)
    {
        if (SpellEffectEntry const* l_SpellEffect = sSpellEffectStore.LookupEntry(l_I))
        {
            mAvaiableDifficultyBySpell[l_SpellEffect->EffectSpellId].insert(l_SpellEffect->EffectDifficulty);

            if (l_SpellEffect->EffectDifficulty != Difficulty::DifficultyNone)
                mDatastoreSpellDifficultyKey[sSpellEffectStore.GetDbcFileName()].insert(std::make_pair(std::make_pair(l_SpellEffect->EffectSpellId, l_SpellEffect->EffectDifficulty), l_SpellEffect->Id));
        }
    }

    /// SpellEquippedItems
    for (uint32 l_I = 0; l_I < sSpellEquippedItemsStore.GetNumRows(); ++l_I)
    {
        if (SpellEquippedItemsEntry const* l_SpellEquippedItem = sSpellEquippedItemsStore.LookupEntry(l_I))
        {
            mSpellBaseDifficultyDBCs[sSpellEquippedItemsStore.GetDbcFileName()].insert(std::make_pair(std::make_pair(l_SpellEquippedItem->SpellID, 0), l_SpellEquippedItem->Id));
        }
    }

    /// SpellInterrupts
    for (uint32 l_I = 0; l_I < sSpellInterruptsStore.GetNumRows(); ++l_I)
    {
        if (SpellInterruptsEntry const* l_SpellInterrupt = sSpellInterruptsStore.LookupEntry(l_I))
        {
            mAvaiableDifficultyBySpell[l_SpellInterrupt->SpellID].insert(l_SpellInterrupt->DifficultyID);

            if (l_SpellInterrupt->DifficultyID != Difficulty::DifficultyNone)
                mDatastoreSpellDifficultyKey[sSpellInterruptsStore.GetDbcFileName()].insert(std::make_pair(std::make_pair(l_SpellInterrupt->SpellID, l_SpellInterrupt->DifficultyID), l_SpellInterrupt->Id));
            else
                mSpellBaseDifficultyDBCs[sSpellInterruptsStore.GetDbcFileName()].insert(std::make_pair(std::make_pair(l_SpellInterrupt->SpellID, l_SpellInterrupt->DifficultyID), l_SpellInterrupt->Id));

        }
    }

    /// SpellLevels
    for (uint32 l_I = 0; l_I < sSpellLevelsStore.GetNumRows(); ++l_I)
    {
        if (SpellLevelsEntry const* l_SpellLevel = sSpellLevelsStore.LookupEntry(l_I))
        {
            mAvaiableDifficultyBySpell[l_SpellLevel->SpellID].insert(l_SpellLevel->DifficultyID);

            if (l_SpellLevel->DifficultyID != Difficulty::DifficultyNone)
                mDatastoreSpellDifficultyKey[sSpellLevelsStore.GetDbcFileName()].insert(std::make_pair(std::make_pair(l_SpellLevel->SpellID, l_SpellLevel->DifficultyID), l_SpellLevel->Id));
            else
                mSpellBaseDifficultyDBCs[sSpellLevelsStore.GetDbcFileName()].insert(std::make_pair(std::make_pair(l_SpellLevel->SpellID, l_SpellLevel->DifficultyID), l_SpellLevel->Id));
        }
    }

    /// SpellMisc
    for (uint32 l_I = 0; l_I < sSpellMiscStore.GetNumRows(); ++l_I)
    {
        if (SpellMiscEntry const* l_SpellMisc = sSpellMiscStore.LookupEntry(l_I))
        {
            mAvaiableDifficultyBySpell[l_SpellMisc->SpellID].insert(l_SpellMisc->DifficultyId);

            if (l_SpellMisc->DifficultyId != Difficulty::DifficultyNone)
                mDatastoreSpellDifficultyKey[sSpellMiscStore.GetDbcFileName()].insert(std::make_pair(std::make_pair(l_SpellMisc->SpellID, l_SpellMisc->DifficultyId), l_SpellMisc->ID));
            else
                mSpellBaseDifficultyDBCs[sSpellMiscStore.GetDbcFileName()].insert(std::make_pair(std::make_pair(l_SpellMisc->SpellID, l_SpellMisc->DifficultyId), l_SpellMisc->ID));
        }
    }

    /// SpellTargetStriction
    for (uint32 l_I = 0; l_I < sSpellTargetRestrictionsStore.GetNumRows(); ++l_I)
    {
        if (SpellTargetRestrictionsEntry const* l_SpellTargetRestriction = sSpellTargetRestrictionsStore.LookupEntry(l_I))
        {
            mAvaiableDifficultyBySpell[l_SpellTargetRestriction->SpellId].insert(l_SpellTargetRestriction->DifficultyID);

            if (l_SpellTargetRestriction->DifficultyID != Difficulty::DifficultyNone)
                mDatastoreSpellDifficultyKey[sSpellTargetRestrictionsStore.GetDbcFileName()].insert(std::make_pair(std::make_pair(l_SpellTargetRestriction->SpellId, l_SpellTargetRestriction->DifficultyID), l_SpellTargetRestriction->Id));
            else
                mSpellBaseDifficultyDBCs[sSpellTargetRestrictionsStore.GetDbcFileName()].insert(std::make_pair(std::make_pair(l_SpellTargetRestriction->SpellId, l_SpellTargetRestriction->DifficultyID), l_SpellTargetRestriction->Id));
        }
    }

    /// SpellXSpellVisual
    for (uint32 l_I = 0; l_I < sSpellXSpellVisualStore.GetNumRows(); ++l_I)
    {
        if (SpellXSpellVisualEntry const* l_Visual = sSpellXSpellVisualStore.LookupEntry(l_I))
        {
            mAvaiableDifficultyBySpell[l_Visual->SpellId].insert(l_Visual->DifficultyID);

            if (l_Visual->DifficultyID != Difficulty::DifficultyNone)
                mDatastoreSpellDifficultyKey[sSpellXSpellVisualStore.GetDB2FileName()].insert(std::make_pair(std::make_pair(l_Visual->VisualID, l_Visual->DifficultyID), l_Visual->Id));
        }
    }
}

void SpellMgr::LoadSpellInfoStore()
{
    uint32 oldMSTime = getMSTime();

    std::unordered_map<uint32, std::unordered_set<uint32>> l_SpellCriterias;

    for (auto criteria : sCriteriaStore)
    {
        if (criteria->Type == CRITERIA_TYPE_BE_SPELL_TARGET || criteria->Type == CRITERIA_TYPE_BE_SPELL_TARGET2 || criteria->Type == CRITERIA_TYPE_CAST_SPELL || criteria->Type == CRITERIA_TYPE_CAST_SPELL2)
            l_SpellCriterias[criteria->Asset.SpellID].insert(criteria->Type);

        if (criteria->StartTimer && (criteria->StartEvent == CRITERIA_TIMED_TYPE_SPELL_TARGET || criteria->StartEvent == CRITERIA_TIMED_TYPE_SPELL_CASTER))
            l_SpellCriterias[criteria->Asset.SpellID].insert(criteria->StartEvent);
    }

    UnloadSpellInfoStore();
    for (int difficulty = 0; difficulty < Difficulty::MaxDifficulties; difficulty++)
        mSpellInfoMap[difficulty].resize(sSpellStore.GetNumRows(), nullptr);

    for (uint32 l_ID = 0; l_ID < sSpellXSpellVisualStore.GetNumRows(); ++l_ID)
    {
        SpellXSpellVisualEntry const* l_Entry = sSpellXSpellVisualStore.LookupEntry(l_ID);
        if (!l_Entry)
            continue;

        VisualsBySpellMap[l_Entry->SpellId][l_Entry->DifficultyID].push_back(l_Entry);
    }

    for (auto& l_MapEntry : VisualsBySpellMap)
    {
        for (auto& l_DifficultyEntry : l_MapEntry.second)
            std::sort(l_DifficultyEntry.second.begin(), l_DifficultyEntry.second.end(), [](SpellXSpellVisualEntry const* first, SpellXSpellVisualEntry const* second) { return first->PlayerConditionID > second->PlayerConditionID; });
    }

    SpellVisualMap emptyMap;

    ParallelFor(0, sSpellStore.GetNumRows(), [this, l_SpellCriterias, &emptyMap](uint32 l_I) -> void
    {
        if (SpellEntry const* spellEntry = sSpellStore.LookupEntry(l_I))
        {
            auto l_Itr = VisualsBySpellMap.find(l_I);

            SpellVisualMap& visualMap = (l_Itr == VisualsBySpellMap.end()) ? emptyMap : l_Itr->second;
            std::set<uint32> difficultyInfo = mAvaiableDifficultyBySpell[l_I];

            for (std::set<uint32>::iterator itr = difficultyInfo.begin(); itr != difficultyInfo.end(); ++itr)
            {
                SpellInfo* l_SpellInfo = new SpellInfo(spellEntry, (*itr), visualMap);

                auto l_CriteriaItr = l_SpellCriterias.find(l_I);
                if (l_CriteriaItr != l_SpellCriterias.end())
                {
                    for (uint32 l_CriteriaType : l_CriteriaItr->second)
                    {
                        if (l_CriteriaType == CRITERIA_TYPE_BE_SPELL_TARGET)
                            l_SpellInfo->CanUpdateCriteriaBeSpellTarget = true;

                        if (l_CriteriaType == CRITERIA_TYPE_BE_SPELL_TARGET2)
                            l_SpellInfo->CanUpdateCriteriaBeSpellTarget2 = true;

                        if (l_CriteriaType == CRITERIA_TYPE_CAST_SPELL)
                            l_SpellInfo->CanUpdateCriteriaCastSpell = true;

                        if (l_CriteriaType == CRITERIA_TYPE_CAST_SPELL2)
                            l_SpellInfo->CanUpdateCriteriaCastSpell2 = true;

                        if (l_CriteriaType == CRITERIA_TIMED_TYPE_SPELL_TARGET)
                            l_SpellInfo->CanUpdateCriteriaTimedSpellTarget = true;

                        if (l_CriteriaType == CRITERIA_TIMED_TYPE_SPELL_CASTER)
                            l_SpellInfo->CanUpdateCriteriaTimedSpellCaster = true;
                    }
                }

                mSpellInfoMap[(*itr)][l_I] = l_SpellInfo;
                sLog->outInfo(LOG_FILTER_POINTERS, "Pointer 0x%" PRIxPTR " -> Spell Id: %u \t\t Difficulty: %u", std::uintptr_t(l_SpellInfo), l_I, *itr);
            }
        }
    });

    for (uint32 l_I = 0; l_I < sSpellPowerStore.GetNumRows(); l_I++)
    {
        SpellPowerEntry const* spellPower = sSpellPowerStore.LookupEntry(l_I);
        if (!spellPower)
            continue;

        for (int difficulty = 0; difficulty < Difficulty::MaxDifficulties; difficulty++)
        {
            SpellInfo* spell = mSpellInfoMap[difficulty][spellPower->SpellId];
            if (!spell)
                continue;

            spell->SpellPowers.push_back(spellPower);
        }
    }

    for (uint32 l_I = 0; l_I < sTalentStore.GetNumRows(); l_I++)
    {
        TalentEntry const* l_TalentEntry = sTalentStore.LookupEntry(l_I);
        if (!l_TalentEntry)
            continue;

        SpellInfo* l_SpellInfo = mSpellInfoMap[DifficultyNone][l_TalentEntry->SpellID];
        if (l_SpellInfo)
            l_SpellInfo->m_TalentIDs.push_back(l_TalentEntry->Id);

        /// Load talents override spell
        if (l_TalentEntry->OverridesSpellID)
        {
            l_SpellInfo = (SpellInfo*)sSpellMgr->GetSpellInfo(l_TalentEntry->OverridesSpellID);
            if (l_SpellInfo)
                l_SpellInfo->OverrideSpellList.push_back(l_TalentEntry->SpellID);
        }
    }

    for (uint32 l_I = 0; l_I < sPvpTalentStore.GetNumRows(); l_I++)
    {
        PvpTalentEntry const* l_Entry = sPvpTalentStore.LookupEntry(l_I);
        if (!l_Entry)
            continue;

        for (int l_Difficulty = 0; l_Difficulty < Difficulty::MaxDifficulties; ++l_Difficulty)
        {
            SpellInfo* l_SpellProto = mSpellInfoMap[l_Difficulty][l_Entry->m_SpellID];
            if (!l_SpellProto)
                continue;

            l_SpellProto->m_IsPVPTalent = true;
        }
    }

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded spell info store in %u ms", GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::UnloadSpellInfoStore()
{
    for (int difficulty = 0; difficulty < Difficulty::MaxDifficulties; difficulty++)
    {
        for (uint32 i = 0; i < mSpellInfoMap[difficulty].size(); ++i)
        {
            if (mSpellInfoMap[difficulty][i])
                delete mSpellInfoMap[difficulty][i];
        }
        mSpellInfoMap[difficulty].clear();
    }
}

void SpellMgr::UnloadSpellInfoImplicitTargetConditionLists()
{
    for (int difficulty = 0; difficulty < Difficulty::MaxDifficulties; difficulty++)
    {
        for (uint32 i = 0; i < mSpellInfoMap[difficulty].size(); ++i)
        {
            if (mSpellInfoMap[difficulty][i])
                mSpellInfoMap[difficulty][i]->_UnloadImplicitTargetConditionLists();
        }
    }
}

void SpellMgr::LoadSpellCustomAttr()
{
    uint32 oldMSTime = getMSTime();

    SpellInfo* spellInfo = NULL;
    for (uint32 i = 0; i < GetSpellInfoStoreSize(); ++i)
    {
        for (int difficulty = 0; difficulty < Difficulty::MaxDifficulties; difficulty++)
        {
            spellInfo = mSpellInfoMap[difficulty][i];
            if (!spellInfo)
                continue;

            /// Bonus loot auras should hit no matter what
            if (spellInfo->HasAura(AuraType::SPELL_AURA_TRIGGER_BONUS_LOOT) || spellInfo->HasAura(AuraType::SPELL_AURA_TRIGGER_BONUS_LOOT_2))
                spellInfo->Attributes |= SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY;

            for (uint8 j = 0; j < spellInfo->EffectCount; ++j)
            {
                switch (spellInfo->Effects[j].ApplyAuraName)
                {
                    case SPELL_AURA_MOD_POSSESS:
                    case SPELL_AURA_MOD_CONFUSE:
                    case SPELL_AURA_MOD_CHARM:
                    case SPELL_AURA_AOE_CHARM:
                    case SPELL_AURA_MOD_FEAR:
                    case SPELL_AURA_MOD_FEAR_2:
                    case SPELL_AURA_MOD_STUN:
                        spellInfo->AttributesCu |= SPELL_ATTR0_CU_AURA_CC;
                        break;
                    case SPELL_AURA_PERIODIC_HEAL:
                    case SPELL_AURA_PERIODIC_DAMAGE:
                    case SPELL_AURA_PERIODIC_SCHOOL_DAMAGE:
                    case SPELL_AURA_PERIODIC_DAMAGE_PERCENT:
                    case SPELL_AURA_PERIODIC_LEECH:
                    case SPELL_AURA_PERIODIC_MANA_LEECH:
                    case SPELL_AURA_PERIODIC_HEALTH_FUNNEL:
                    case SPELL_AURA_PERIODIC_ENERGIZE:
                    case SPELL_AURA_OBS_MOD_HEALTH:
                    case SPELL_AURA_OBS_MOD_POWER:
                    case SPELL_AURA_POWER_BURN:
                        spellInfo->AttributesCu |= SPELL_ATTR0_CU_NO_INITIAL_THREAT;
                        break;
                    case SPELL_AURA_PROC_MELEE_TRIGGER_SPELL:
                        spellInfo->ProcChance = 100;
                        spellInfo->ProcFlags = PROC_FLAG_DONE_SPELL_MELEE_DMG_CLASS | PROC_FLAG_DONE_MELEE_AUTO_ATTACK;
                        break;
                    default:
                        break;
                }

                switch (spellInfo->Effects[j].Effect)
                {
                    case SPELL_EFFECT_SELF_RESURRECT:
                    {
                        /// Needed for spell_proc
                        /// Fixes visuals of self-ress spells
                        if (!spellInfo->Effects[j].TargetA.GetTarget() && !spellInfo->Effects[j].TargetB.GetTarget())
                            spellInfo->Effects[j].TargetA = TARGET_UNIT_CASTER;

                        break;
                    }
                    case SPELL_EFFECT_INCREASE_FOLLOWER_ITEM_LEVEL:
                    case SPELL_EFFECT_TEACH_FOLLOWER_ABILITY:
                        spellInfo->Effects[j].TargetA = TARGET_UNIT_CASTER;
                        spellInfo->Effects[j].TargetB = TARGET_UNIT_CASTER;

                        if (j == EFFECT_0 && spellInfo->Effects[EFFECT_1].Effect == SPELL_EFFECT_DUMMY)
                        {
                            spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_CASTER;
                            spellInfo->Effects[EFFECT_1].TargetB = TARGET_UNIT_CASTER;
                        }
                        break;
                    case SPELL_EFFECT_SCHOOL_DAMAGE:
                    case SPELL_EFFECT_WEAPON_DAMAGE:
                    case SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL:
                    case SPELL_EFFECT_NORMALIZED_WEAPON_DMG:
                    case SPELL_EFFECT_WEAPON_PERCENT_DAMAGE:
                    case SPELL_EFFECT_HEAL:
                        spellInfo->AttributesCu |= SPELL_ATTR0_CU_DIRECT_DAMAGE;
                        break;
                    case SPELL_EFFECT_POWER_DRAIN:
                    case SPELL_EFFECT_POWER_BURN:
                    case SPELL_EFFECT_HEAL_MAX_HEALTH:
                    case SPELL_EFFECT_HEALTH_LEECH:
                    case SPELL_EFFECT_HEAL_PCT:
                    case SPELL_EFFECT_ENERGIZE_PCT:
                    case SPELL_EFFECT_ENERGIZE:
                    case SPELL_EFFECT_HEAL_MECHANICAL:
                        spellInfo->AttributesCu |= SPELL_ATTR0_CU_NO_INITIAL_THREAT;
                        break;
                    case SPELL_EFFECT_CHARGE:
                    case SPELL_EFFECT_CHARGE_DEST:
                    case SPELL_EFFECT_JUMP:
                    case SPELL_EFFECT_JUMP_DEST:
                    case SPELL_EFFECT_LEAP_BACK:
                        spellInfo->AttributesCu |= SPELL_ATTR0_CU_CHARGE;
                        break;
                    case SPELL_EFFECT_PICKPOCKET:
                        spellInfo->AttributesCu |= SPELL_ATTR0_CU_PICKPOCKET;
                        break;
                    case SPELL_EFFECT_CREATE_ITEM:
                    case SPELL_EFFECT_CREATE_LOOT:
                    {
                        mSpellCreateItemList.push_back(i);

                        SkillLineAbilityMapBounds l_SpellBounds = sSpellMgr->GetSkillLineAbilityMapBounds(spellInfo->Id);
                        for (SkillLineAbilityMap::const_iterator l_SpellIdx = l_SpellBounds.first; l_SpellIdx != l_SpellBounds.second; ++l_SpellIdx)
                            m_ItemSourceSkills[spellInfo->Effects[j].ItemType].push_back(l_SpellIdx->second->SkillLine);

                        break;
                    }
                    case SPELL_EFFECT_SUMMON_PET:
                    case SPELL_EFFECT_SUMMON:
                    {
                        m_SummoningSpellForEntry[spellInfo->Effects[j].MiscValue] = spellInfo->Id;
                        break;
                    }
                    case SPELL_EFFECT_ENCHANT_ITEM:
                    case SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY:
                    case SPELL_EFFECT_ENCHANT_ITEM_PRISMATIC:
                    case SPELL_EFFECT_ENCHANT_HELD_ITEM:
                    {
                        // only enchanting profession enchantments procs can stack
                        if (IsPartOfSkillLine(SKILL_ENCHANTING, i))
                        {
                            uint32 enchantId = spellInfo->Effects[j].MiscValue;
                            SpellItemEnchantmentEntry const* enchant = sSpellItemEnchantmentStore.LookupEntry(enchantId);
                            for (uint8 s = 0; s < MAX_ENCHANTMENT_SPELLS; ++s)
                            {
                                if (!enchant)
                                    continue;

                                if (enchant->type[s] != ITEM_ENCHANTMENT_TYPE_COMBAT_SPELL)
                                    continue;

                                SpellInfo* procInfo = (SpellInfo*)GetSpellInfo(enchant->spellid[s]);
                                if (!procInfo)
                                    continue;

                                // if proced directly from enchantment, not via proc aura
                                // NOTE: Enchant Weapon - Blade Ward also has proc aura spell and is proced directly
                                // however its not expected to stack so this check is good
                                if (procInfo->HasAura(SPELL_AURA_PROC_TRIGGER_SPELL))
                                    continue;

                            procInfo->AttributesCu |= SPELL_ATTR0_CU_ENCHANT_STACK;
                        }
                    }
                    else if (IsPartOfSkillLine(SKILL_RUNEFORGING, i))
                    {
                        uint32 enchantId = spellInfo->Effects[j].MiscValue;
                        SpellItemEnchantmentEntry const* enchant = sSpellItemEnchantmentStore.LookupEntry(enchantId);
                        for (uint8 s = 0; s < MAX_ENCHANTMENT_SPELLS; ++s)
                        {
                            SpellInfo* procInfo = (SpellInfo*)GetSpellInfo(enchant->spellid[s]);
                            if (!procInfo)
                                continue;

                            switch (procInfo->Id)
                            {
                                case 53365: ///< Unholy Strength
                                case 51714: ///< Razorice
                                    continue;
                            }

                            procInfo->AttributesCu |= SPELL_ATTR0_CU_ENCHANT_STACK;
                        }
                    }

                    break;
                }
            }
        }

        if (!spellInfo->_IsPositiveEffect(EFFECT_0, false))
            spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE_EFF0;

        if (!spellInfo->_IsPositiveEffect(EFFECT_1, false))
            spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE_EFF1;

        if (!spellInfo->_IsPositiveEffect(EFFECT_2, false))
            spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE_EFF2;

        uint16 l_VisualID = 0;
        if (SpellXSpellVisualEntry const* l_VisualEntry = sSpellXSpellVisualStore.LookupEntry(spellInfo->GetSpellXSpellVisualId()))
            l_VisualID = l_VisualEntry->VisualID;

        if (l_VisualID == 3879 || spellInfo->Id == 74117)
            spellInfo->AttributesCu |= SPELL_ATTR0_CU_CONE_BACK;

        /// Negative ConeAngle means it should hit in the back
        if (SpellTargetRestrictionsEntry const* l_Restrictions = spellInfo->GetSpellTargetRestrictions())
        {
            if (l_Restrictions->ConeAngle < 0.0f)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_CONE_BACK;
        }

        ////////////////////////////////////
        ///      DEFINE BINARY SPELLS   ////
        ////////////////////////////////////
        for (uint8 j = 0; j < spellInfo->EffectCount; ++j)
        {
            switch (spellInfo->Effects[j].Effect)
            {
                case SPELL_EFFECT_DISPEL:
                case SPELL_EFFECT_STEAL_BENEFICIAL_BUFF:
                case SPELL_EFFECT_POWER_DRAIN:
                case SPELL_EFFECT_POWER_BURN:
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_BINARY;
                    break;
            }

            switch (spellInfo->Effects[j].Mechanic)
            {
                case MECHANIC_FEAR:
                case MECHANIC_CHARM:
                case MECHANIC_SNARE:
                    // Frost Bolt is not binary
                    if (spellInfo->Id == 116)
                        break;
                case MECHANIC_FREEZE:
                case MECHANIC_BANISH:
                case MECHANIC_POLYMORPH:
                case MECHANIC_ROOT:
                case MECHANIC_INTERRUPT:
                case MECHANIC_SILENCE:
                case MECHANIC_HORROR:
                    spellInfo->AttributesCu |= SPELL_ATTR0_CU_BINARY;
                    break;
                default:
                    break;
             }

            switch (spellInfo->Effects[j].Effect)
            {
                case SPELL_EFFECT_TEACH_FOLLOWER_ABILITY:
                {
                    m_FollowerAbilitiesFromSpell.insert(spellInfo->Effects[j].MiscValue);
                    break;
                }
                default:
                    break;
            }
        }

        switch (spellInfo->Mechanic)
        {
            case MECHANIC_FEAR:
            case MECHANIC_CHARM:
            case MECHANIC_SNARE:
            case MECHANIC_FREEZE:
            case MECHANIC_BANISH:
            case MECHANIC_POLYMORPH:
            case MECHANIC_ROOT:
            case MECHANIC_INTERRUPT:
            case MECHANIC_SILENCE:
            case MECHANIC_HORROR:
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_BINARY;
                break;
        }

        if (spellInfo->Attributes & SPELL_ATTR0_TRADESPELL)
        {
            SkillLineAbilityMapBounds l_SpellBounds = sSpellMgr->GetSkillLineAbilityMapBounds(spellInfo->Id);
            for (SkillLineAbilityMap::const_iterator l_SpellIdx = l_SpellBounds.first; l_SpellIdx != l_SpellBounds.second; ++l_SpellIdx)
                m_SkillTradeSpells[l_SpellIdx->second->SkillLine].push_back(l_SpellIdx->second);
        }

        ///////////////////////////
        //////   END BINARY  //////
        ///////////////////////////

        std::vector<int32> l_AllowIndoorMountSpells = { 190784, 221883, 221885, 221886, 221887 };

        if (std::find(l_AllowIndoorMountSpells.begin(), l_AllowIndoorMountSpells.end(), spellInfo->Id) != std::end(l_AllowIndoorMountSpells))
            spellInfo->AttributesCu |= SPELL_ATTR0_CU_ALLOW_INDOOR_MOUNT;

        /// Some CasterAuraSpell are internal spell IDs, don't use them
        if (spellInfo->CasterAuraSpell && sSpellMgr->GetSpellInfo(spellInfo->CasterAuraSpell) == nullptr)
            spellInfo->CasterAuraSpell = 0;

        /// Some TargetAuraSpell are internal spell IDs, don't use them
        if (spellInfo->TargetAuraSpell && sSpellMgr->GetSpellInfo(spellInfo->TargetAuraSpell) == nullptr)
            spellInfo->TargetAuraSpell = 0;

        switch (spellInfo->Id)
        {
            case 181502: ///< Energy Drain
                spellInfo->Attributes &= ~SpellAttr0::SPELL_ATTR0_LEVEL_DAMAGE_CALCULATION;
                break;
            /// Court of Stars Buffs
            case 211081:
            case 211093:
            case 211080:
            case 211110:
            case 211084:
            case 211071:
            case 211102:
                spellInfo->AttributesEx3 |= SPELL_ATTR3_DEATH_PERSISTENT;
                break;
            case 206150: ///< Challengers Might
            {
                spellInfo->Effects[EFFECT_2].TriggerSpell = 0;
                break;
            }
            case 228318: ///< Enrage (Raging Affix)
                spellInfo->Effects[EFFECT_2].Effect = SPELL_EFFECT_DUMMY;
                break;
            case 195838: ///< Horde (Mercenary)
            case 195843: ///< Alliance (Mercenary)
                spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_APPLY_AURA;
                spellInfo->Effects[EFFECT_1].Effect = SPELL_EFFECT_APPLY_AURA;
                spellInfo->Effects[EFFECT_2].Effect = SPELL_EFFECT_APPLY_AURA;
                break;
            case 252751: ///< Seething Shore - Thrust
                spellInfo->Attributes = 0;
                spellInfo->Effects[EFFECT_0].BasePoints = 20;
                break;
            case 82224:
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_10_YARDS);
                break;
            case 95958:  ///< Throw Torch
            case 96022:  ///< Dousing Agent
            case 253545: ///< Empowered Doom
                spellInfo->MaxAffectedTargets = 1;
                break;
            case 224351: ///< General Trigger (Foxflower)
                spellInfo->MaxAffectedTargets = 1;
                break;
            case 248697: ///< Fel Spike
                spellInfo->MaxAffectedTargets = 2;
                break;
            case 178974: ///< Demon Souls: Soul Gem Ping
                spellInfo->ExplicitTargetMask = TARGET_FLAG_DEST_LOCATION;
                spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(187); ///< 300y
                break;
            case 242960: ///< Void Rift
                spellInfo->AuraInterruptFlags = 0;
                break;
            case 242716: ///< Arkhaan Summon
            case 242718: ///< Alleria Summon
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DB;
                break;
            case 251080: ///< Light's Vengeance
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_NEARBY_ENEMY;
                break;
            case 248624: ///< Fel Blast
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_100_YARDS);
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_DEST_AREA_ENEMY;
                break;
            case 247282: ///< Summon Turalyon
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                break;
            case 252315: ///< Energy Breach
                spellInfo->MaxAffectedTargets = 0;
                break;
            case 246789: ///< Laser Field - Visual
                spellInfo->MaxAffectedTargets = 1;
                break;
            case 246721: ///< Summon Mobile Area Trigger (F to B)
            case 246728: ///< Summon Mobile Area Trigger (B to F)
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(1); ///< 10s
                break;
            case 246695: ///< Summon Mobile Area Trigger (R to L)
            case 246718: ///< Summon Mobile Area Trigger (L to R)
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(18); ///< 20s
                break;
            case 252138: ///< Arcane Discharge
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_1_YARD);
                spellInfo->Effects[EFFECT_1].SetRadiusIndex(EFFECT_RADIUS_1_YARD);
                break;
            case 252205: ///< Mystic Barrage
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_NEARBY_ENEMY;
                break;
            case 117472: ///< Pandaren Healing Draught
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_NEARBY_ENTRY;
                break;
            case 245703: ///< Arcane Barrier
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_SRC_CASTER;
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_PARTY;
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_2_YARDS);
                break;
            case 247709: ///< Crystal Channel
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_SRC_CASTER;
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_NEARBY_ENTRY;
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_10_YARDS);
                break;
            case 247734: ///< Arcane Beam
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_SRC_CASTER;
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENTRY;
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_5_YARDS);
                break;
            case 240768: ///< Holy Pummel
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_DEST_AREA_ENEMY;
                break;
            case 250288: ///< Rebuke
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_DEST_AREA_ENEMY;
                break;
            case 235763: ///< Summon Command Center
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(837); ///< Too much
                break;
            case 232897: ///< Felfire Shot
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            case 239173: ///< Rain of Fire
                spellInfo->AttributesEx = SPELL_ATTR1_DONT_DISPLAY_IN_AURA_BAR;
                break;
            case 239431: ///< Destroying
                spellInfo->StartRecoveryTime = 3000;
                spellInfo->StartRecoveryCategory = 133;
                spellInfo->PreventionType = SpellPreventionMask::Silence;
                break;
            case 59643: ///< Plant Horde Battle Standard
            case 4338: ///< Plant Alliance Battle Standard
                spellInfo->RecoveryTime = 0;
                break;
            case 234629: ///< Fel Discharge
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST_RANDOM;
                break;
            case 235047: ///< Dark Wave
            case 235049: ///< Dark Wave
            case 235050: ///< Dark Wave
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(28); ///< 5s
                break;
            case 251749: ///< Fel Eruption
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            case 162645: ///< Summon Gar'rok
            case 162650: ///< Summon Agrea
            case 199598: ///< Void Rip
            case 248272: ///< Summon Lightforged Warframe
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DB;
                break;
            case 241015: ///< Shadow Flurry Damage
                spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_DUMMY;
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_SRC_AREA_ENTRY;
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_100_YARDS);
                break;
            case 241014: ///< Shadow Flurry Periodic
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                break;
            case 241438: ///< Free Prisoner
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_SRC_AREA_ALLY;
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_SRC_AREA_ALLY;
                break;
            case 241185: ///< Grip of the Pit King
                spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_KNOCK_BACK;
                spellInfo->Effects[EFFECT_0].BasePoints = 140;
                spellInfo->Effects[EFFECT_0].MiscValue = 140;
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_100_YARDS);
                spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                spellInfo->Effects[EFFECT_1].TriggerSpell = 0;
                break;
            case 252856: ///< Avenging Wrath
                spellInfo->Effects[EFFECT_0].BasePoints = 3;
                spellInfo->Effects[EFFECT_1].BasePoints = 0;
                break;
            case 241000: ///< Crash of Light
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_DEST_DEST;
                break;
            case 250895: ///< Light's Protection
                spellInfo->AuraInterruptFlags = 0;
                break;
            case 215970: ///< Holy Might
                spellInfo->Effects[EFFECT_0].Amplitude = 1500;
                break;
            case 245035: ///< Devastator Bombardment
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_SRC_AREA_ENTRY;
                break;
            case 245036: ///< Devastator Bombardment Missle
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            case 215314: ///< Raging Storm heal
                spellInfo->MaxAffectedTargets = 5;
                break;
            case 200040: ///< Nether Venom Mastery
                spellInfo->Effects[EFFECT_1].BasePoints = 0; ///< Prevent to add 500% haste and make the boss instant killable
                break;
            case 234229: ///< Return Home
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_VEHICLE;
                break;
            case 234224: ///< Overload Energy
                spellInfo->Effects[0].Effect = SPELL_EFFECT_DUMMY;
                spellInfo->Effects[0].BasePoints = 0;
                break;
            case 235105: ///< Into the fray
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                break;
            case 235579: ///< Summon Hippogryph Taxi
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                break;
            case 225158: ///< Upgrade Class Hall Armor
            case 225160: ///< Upgrade Class Hall Armor
            case 225161: ///< Upgrade Class Hall Armor
                spellInfo->Effects[EFFECT_2].TargetA = 0;
                break;
            case 215884: ///< Withering Consumption (Trinket)
                spellInfo->Effects[0].BasePoints = 16002;
                break;
            case 211408:
                spellInfo->Effects[0].Effect = SPELL_EFFECT_TRIGGER_SPELL;
                spellInfo->Effects[0].TriggerSpell = 211412;
                break;
            case 209490: ///< Drain Magic (aura) (CoS)
                spellInfo->Effects[0].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[0].TargetB = 0;
                break;
            case 210253:
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                break;
            case 207883: ///< Infernal Eruption (CoS)
               /// spellInfo->AttributesEx3 &= ~SPELL_ATTR3_IGNORE_HIT_RESULT;
               /// spellInfo->AttributesEx9 |= ~SPELL_ATTR9_SPECIAL_DELAY_CALCULATION;
                spellInfo->Effects[0].TriggerSpell = 0;
                spellInfo->Effects[0].TargetA = 0;
                spellInfo->Effects[0].TargetB = 2;
                spellInfo->AttributesEx4 &= ~SPELL_ATTR4_CAN_CAST_WHILE_CASTING;
                spellInfo->CastTimeEntry = sSpellCastTimesStore.LookupEntry(5); ///< 2s
                break;
            case 219498: ///< Streetsweeper (CoS)
                spellInfo->TargetAuraSpell = 0;
                break;
            case 210295: ///< Interact (COS)
                spellInfo->AuraInterruptFlags = 0;
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(1); ///< 10s
                break;
            case 207981: ///< Disintegration Beam (COS)
                spellInfo->Effects[EFFECT_2].m_CanBeStacked = false;
                break;
            case 211464: ///< Fel Detonation (COS)
            {
                if (difficulty == Difficulty::DifficultyMythic)
                {
                    spellInfo->PreventionType = 0;
                    spellInfo->InterruptFlags &= ~SPELL_INTERRUPT_FLAG_INTERRUPT;
                }
                break;
            }
            case 207980: ///< Disintegration Beam (COS)
                spellInfo->PreventionType = 0;
                spellInfo->InterruptFlags &= ~SPELL_INTERRUPT_FLAG_INTERRUPT;
                break;
            case 209667: ///< Blade Surge (COS)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_IGNORE_ARMOR;
                spellInfo->AttributesEx3 |= SPELL_ATTR3_IGNORE_HIT_RESULT;
                break;
            case 225100: ///< Charging Station (COS)
                spellInfo->InterruptFlags |= SPELL_INTERRUPT_FLAG_INTERRUPT;
                break;
            case 199055:
                spellInfo->AttributesEx3 |= SPELL_ATTR3_CAN_PROC_WITH_TRIGGERED;
                break;
            case 207694:
                spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_DONE_BONUS;
                break;
            case 209388:
                spellInfo->StackAmount = 1;
                break;
            case 236299:
                spellInfo->Effects[0].BasePoints = -30;
                break;
            case 197915:
                spellInfo->ProcCharges = 0;
                break;
            case 216413:
                spellInfo->ProcChance = 100;
                spellInfo->ProcCharges = 1;
                break;
            case 199121:
                spellInfo->Effects[0].TargetA = TARGET_DEST_TARGET_SELECT;
                break;
            case 190319:
                spellInfo->Effects[1].BasePoints = 50;
                break;
            case 207498:
                spellInfo->Effects[1].Effect = SPELL_EFFECT_DUMMY;
                break;
            case 213050: ///< Smashed
                spellInfo->Effects[EFFECT_0].BasePoints = 10;
                break;
            case 193566: ///< Arise, Fallen (Ymiron, the Fallen King)
                spellInfo->CasterAuraSpell = 0;
                break;
            case 243029: ///< Soul Siphon (Ymiron, the Fallen King)
                spellInfo->Effects[0].Effect = SPELL_EFFECT_INSTAKILL;
                break;
            case 198752: ///< Fracture (Seacursed Slaver)
                spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_TRIGGER_SPELL;
                break;
            case 224473: ///< Defiant Strike (Searcursed Soulkeeper)
                spellInfo->Effects[EFFECT_0].Effect = 0;
                break;
            case 195036: ///< Defiant Strike - Final (Seacursed Soulkeeper)
                spellInfo->Effects[EFFECT_1].Effect = 0;
                break;
            case 220229: ///< Ratstallion Harnesses (Dalaran Sewers)
                spellInfo->Effects[0].Effect     = SPELL_EFFECT_SUMMON;
                spellInfo->Effects[0].TargetA    = TARGET_DEST_CASTER_FRONT;
                spellInfo->Effects[0].MiscValue  = 110743; ///< NPC : Crate of Ratstallion Harnesses
                spellInfo->Effects[0].MiscValueB = 719; ///< NPC : Crate of Ratstallion Harnesses
                break;
            case 220266: ///< Widowsister Contract (Dalaran Underbelly)
            case 220260: ///< Screecher Whistle    (Dalaran Underbelly)
            case 220265: ///< Imp-Binding Contract (Dalaran Underbelly)
            case 220253: ///< Croc Flusher         (Dalaran Underbelly)
                spellInfo->Effects[0].Effect = SPELL_EFFECT_DUMMY;
                break;
            case 220742: ///< Hidden Appearance Unlock - Death Knight, Unholy
                spellInfo->Effects[EFFECT_1].TriggerSpell = 220655;
                break;
            case 203892: ///< Summon Underbelly Guard
                spellInfo->SetDurationIndex(5); ///< 300s
                break;
            case 214005:
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(124); ///< 5yd
                break;
            case 217338: ///< Focused Gust (Dalaran Sewers)
                spellInfo->Effects[0].Effect = SPELL_EFFECT_TRIGGER_SPELL;
                spellInfo->Effects[0].TriggerSpell = 217340;
                break;
            case 115320: ///< Throwing Stars (Dalaran Sewers)
                spellInfo->Effects[0].ApplyAuraName = SPELL_AURA_PERIODIC_DUMMY;
                break;
            case 121471:
            case 247938:
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                break;
            case 219735: ///< Nature's Charge (Dalaran Sewers)
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(4); ///< 120s
                break;
            case 199063: ///< Strangling Roots (Oakheart)
                spellInfo->Effects[EFFECT_5].Effect = 0;
                break;
            case 200768: ///< Propelling Charge
                spellInfo->Effects[EFFECT_1].Effect = 0;
                break;
            case 201226: ///< Blood Assault
                spellInfo->Effects[EFFECT_2].Effect = 0;
                break;
            case 198379: ///< Primal Rampage (Archdruid Glaidalis)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                break;
            case 205233: ///< Lord Betrug: Execution veh
                spellInfo->Effects[EFFECT_0].Effect = 0;
                spellInfo->Effects[EFFECT_0].ApplyAuraName = 0;
                break;
            case 197147: ///< Festering Wound same duration as 194310
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(106);
                break;
            case 206967: ///< Will of the Necropolis
                spellInfo->Effects[EFFECT_0].BasePoints = 0;
                break;
            case 146347: ///< Life Steal
                spellInfo->AttributesEx3 |= SpellAttr3::SPELL_ATTR3_CANT_TRIGGER_PROC;
                break;
            ///////////////////////////////////////////////////////////////////////////////////
            /// Rush Orders
            ///////////////////////////////////////////////////////////////////////////////////
            case 180704: ///< Rush order visual
                spellInfo->Effects[0].TargetA = TARGET_UNIT_CASTER;
                break;
            case 181507: ///< Alchemy Lab
                spellInfo->Reagent[0]              = 122576;
                spellInfo->ReagentCount[0]         = 1;
                spellInfo->Effects[0].Effect       = SPELL_EFFECT_TRIGGER_SPELL;
                spellInfo->Effects[0].TriggerSpell = 181228;
                spellInfo->Effects[0].BasePoints   = 0; ///< TEAM_ALLIANCE
                spellInfo->Effects[1].Effect       = SPELL_EFFECT_TRIGGER_SPELL;
                spellInfo->Effects[1].TriggerSpell = 181229;
                spellInfo->Effects[1].BasePoints   = 1; ///< TEAM_HORDE
                break;
            case 181518: ///< The Tannery
                spellInfo->Reagent[0]              = 122596;
                spellInfo->ReagentCount[0]         = 1;
                spellInfo->Effects[0].Effect       = SPELL_EFFECT_TRIGGER_SPELL;
                spellInfo->Effects[0].TriggerSpell = 181240;
                spellInfo->Effects[0].BasePoints   = 0; ///< TEAM_ALLIANCE
                spellInfo->Effects[1].Effect       = SPELL_EFFECT_TRIGGER_SPELL;
                spellInfo->Effects[1].TriggerSpell = 181241;
                spellInfo->Effects[1].BasePoints   = 1; ///< TEAM_HORDE
                break;
            case 181519: ///< Scribes Quarters
                spellInfo->Reagent[0]              = 122593;
                spellInfo->ReagentCount[0]         = 1;
                spellInfo->Effects[0].Effect       = SPELL_EFFECT_TRIGGER_SPELL;
                spellInfo->Effects[0].TriggerSpell = 181236;
                spellInfo->Effects[0].BasePoints   = 0; ///< TEAM_ALLIANCE
                spellInfo->Effects[1].Effect       = SPELL_EFFECT_TRIGGER_SPELL;
                spellInfo->Effects[1].TriggerSpell = 181237;
                spellInfo->Effects[1].BasePoints   = 1; ///< TEAM_HORDE
                break;
            case 181520: ///< The Forge
                spellInfo->Reagent[0]              = 122595;
                spellInfo->ReagentCount[0]         = 1;
                spellInfo->Effects[0].Effect       = SPELL_EFFECT_TRIGGER_SPELL;
                spellInfo->Effects[0].TriggerSpell = 181230;
                spellInfo->Effects[0].BasePoints   = 0; ///< TEAM_ALLIANCE
                spellInfo->Effects[1].Effect       = SPELL_EFFECT_TRIGGER_SPELL;
                spellInfo->Effects[1].TriggerSpell = 181231;
                spellInfo->Effects[1].BasePoints   = 1; ///< TEAM_HORDE
                break;
            case 181521: ///< Tailoring Emporium
                spellInfo->Reagent[0]              = 122594;
                spellInfo->ReagentCount[0]         = 1;
                spellInfo->Effects[0].Effect       = SPELL_EFFECT_TRIGGER_SPELL;
                spellInfo->Effects[0].TriggerSpell = 181242;
                spellInfo->Effects[0].BasePoints   = 0; ///< TEAM_ALLIANCE
                spellInfo->Effects[1].Effect       = SPELL_EFFECT_TRIGGER_SPELL;
                spellInfo->Effects[1].TriggerSpell = 181243;
                spellInfo->Effects[1].BasePoints   = 1; ///< TEAM_HORDE
                break;
            case 181522: ///< Gem Boutique
                spellInfo->Reagent[0]              = 122592;
                spellInfo->ReagentCount[0]         = 1;
                spellInfo->Effects[0].Effect       = SPELL_EFFECT_TRIGGER_SPELL;
                spellInfo->Effects[0].TriggerSpell = 181238;
                spellInfo->Effects[0].BasePoints   = 0; ///< TEAM_ALLIANCE
                spellInfo->Effects[1].Effect       = SPELL_EFFECT_TRIGGER_SPELL;
                spellInfo->Effects[1].TriggerSpell = 181239;
                spellInfo->Effects[1].BasePoints   = 1; ///< TEAM_HORDE
                break;
            case 181524: ///< Enchanters Study
                spellInfo->Reagent[0]              = 122590;
                spellInfo->ReagentCount[0]         = 1;
                spellInfo->Effects[0].Effect       = SPELL_EFFECT_TRIGGER_SPELL;
                spellInfo->Effects[0].TriggerSpell = 181232;
                spellInfo->Effects[0].BasePoints   = 0; ///< TEAM_ALLIANCE
                spellInfo->Effects[1].Effect       = SPELL_EFFECT_TRIGGER_SPELL;
                spellInfo->Effects[1].TriggerSpell = 181233;
                spellInfo->Effects[1].BasePoints   = 1; ///< TEAM_HORDE
                break;
            case 181525: ///< Engineering Works
                spellInfo->Reagent[0]              = 122591;
                spellInfo->ReagentCount[0]         = 1;
                spellInfo->Effects[0].Effect       = SPELL_EFFECT_TRIGGER_SPELL;
                spellInfo->Effects[0].TriggerSpell = 181234;
                spellInfo->Effects[0].BasePoints   = 0; ///< TEAM_ALLIANCE
                spellInfo->Effects[1].Effect       = SPELL_EFFECT_TRIGGER_SPELL;
                spellInfo->Effects[1].TriggerSpell = 181235;
                spellInfo->Effects[1].BasePoints   = 1; ///< TEAM_HORDE
                break;
            ///////////////////////////////////////////////////////////////////////////////////
            /// Engineering Works
            ///////////////////////////////////////////////////////////////////////////////////
            case 156756:
                spellInfo->Effects[0].MiscValue = 191605; ///< Missing data, take back entry from spell 54710
                break;
            ///////////////////////////////////////////////////////////////////////////////////
            /// Scribe Quarters
            ///////////////////////////////////////////////////////////////////////////////////
            case 176513:    ///< Draenor Merchant Order
                spellInfo->Effects[0].Effect = SPELL_EFFECT_CREATE_RANDOM_ITEM;
                break;
            case 176791:    ///< Combine
                spellInfo->Effects[1].Effect = 0;
                /// No break needed here
            case 176482:    ///< Combine
            case 176483:    ///< Combine
            case 176484:    ///< Combine
            case 176485:    ///< Combine
            case 176486:    ///< Combine
            case 176487:    ///< Combine
            case 176488:    ///< Combine
            case 176489:    ///< Combine
            case 176490:    ///< Combine
            case 176491:    ///< Combine
            case 176934:    ///< Combine
                spellInfo->Effects[0].Effect = SPELL_EFFECT_DUMMY;
                break;
            ///////////////////////////////////////////////////////////////////////////////////
#ifndef CROSS
            case 61551:     ///< Toy Train Set (Pulse)
                spellInfo->TargetAuraSpell = 0;
                break;
#endif /* not CROSS */
            case 179244:    ///< Summon Chauffeur (Horde)
            case 179245:    ///< Summon Chauffeur (Alliance)
                spellInfo->Effects[EFFECT_0].MiscValueB = 284;  ///< This will allow level 1 mounting at 160% normal speed
                break;
            case 1843:      ///< Hack for disarm. Client sends the spell instead of gameobjectuse.
            case 161710:    ///< Garrison enchanter study
            case 160201:    ///< Garrison enchanter study
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_ALWAYS_ACTIVE;
                break;
            case 178444: ///< Create Armor Enhancement (garrison loot spell)
            case 178445: ///< Create Weapon Boost (garrison loot spell)
                spellInfo->Effects[0].Effect = SPELL_EFFECT_CREATE_RANDOM_ITEM;
                break;
            case 167946: ///< Journeyman Logging (Lumber Mill)
                spellInfo->Effects[0].BasePoints = 1;
                break;
            case 168361: ///< Forward Thrust
                spellInfo->Effects[0].MiscValue = 500;
                break;
            case 105157: ///< See Quest Invis 14, Wandering Island spell
                spellInfo->AreaGroupId = 0;
                break;
            ///////////////////////////////////////////////////////////////////////////////////
            /// Stables
            ///////////////////////////////////////////////////////////////////////////////////
            case 173702: ///< Lasso Break
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                break;
            case 173686: ///< Stables Lasso
            case 174070: ///< Stables Lasso
                spellInfo->Effects[EFFECT_1].TriggerSpell = 0;
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(5); ///< 300s
                break;
            ///////////////////////////////////////////////////////////////////////////////////
            case 182464: ///< Portal to Garrison
                spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_DUMMY;
                break;
            case 179478: ///< Voidtalon of the Dark Star
                spellInfo->Effects[EFFECT_0].MiscValue = 89959;
                spellInfo->Effects[EFFECT_0].MiscValueB = 230;
                break;
            ///////////////////////////////////////////////////////////////////////////////////
            /// Blackrock Foundry
            ///////////////////////////////////////////////////////////////////////////////////
            case 175609: ///< Unbind Flame
                spellInfo->Effects[EFFECT_0].MiscValueB = 64;
                break;
            case 175638: ///< Spinning Blade
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[EFFECT_1].TargetB = 0;
                break;
            case 158724: ///< End Ship Phase (Iron Maidens)
                spellInfo->Effects[EFFECT_2].Effect = 0;
                break;
            case 158730: ///< Dreadnaught Destruction (Iron Maidens)
                spellInfo->Effects[EFFECT_1].ValueMultiplier = 40.0f;
                spellInfo->Effects[EFFECT_1].MiscValue = 250;
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[EFFECT_1].TargetB = 0;
                spellInfo->Effects[EFFECT_2].Effect = 0;
                break;
            case 175643: ///< Spinning Blade (DoT)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_DONT_RESET_PERIODIC_TIMER;
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                spellInfo->AttributesEx5 |= SPELL_ATTR5_HIDE_DURATION;
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(39); ///< 2s
                spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                break;
            case 173192: ///< Cave In (Dot)
            case 159686: ///< Acidback Puddle (DoT)
            case 156203: ///< Retched Blackrock (Oregorger)
            case 155265: ///< Containment (Primal Elementalist)
                spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                break;
            case 175091: ///< Animate Slag
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                break;
            case 170687: ///< Killing Spree
                spellInfo->AttributesEx &= ~SPELL_ATTR1_CHANNELED_2;
                break;
            case 155077: ///< Overwhelming Blows (Gruul)
                spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                break;
            case 159632: ///< Insatiable Hunger
            case 156631: ///< Rapid Fire (Admiral Gar'an - Iron Maidens)
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                break;
            case 160665: ///< Rolling Box (Oregorger)
            case 160833: ///< Bust Loose (Heart of the Mountain)
            case 155738: ///< Slag Pool (Heart of the Mountain)
            case 155224: ///< Melt (Heart of the Mountain)
            case 156220: ///< Tactical Retreat
            case 156883: ///< Tactical Retreat (Other)
            case 163636: ///< Firestorm V2 Missile (Firestorm Stalker)
            case 162757: ///< Ice Trap (Iron Marksman)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                break;
            case 82058: ///< Harris's Ampule
            case 114956: ///< Nether Tempest (launcher visual)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                break;
            case 155819: ///< Hunger Drive (Oregorger)
                spellInfo->Attributes &= ~SPELL_ATTR0_DEBUFF;
                break;
            case 155897: ///< Earthshaking Collision (Oregorger)
                spellInfo->Mechanic = MECHANIC_DISCOVERY;
                break;
            case 159958: ///< Earthshaking Stomp (Oregorger)
                spellInfo->Effects[EFFECT_1].Effect = 0;
                break;
            case 173461: ///< Blackrock Barrage (Oregorger)
                spellInfo->CasterAuraSpell = 0;
                spellInfo->InterruptFlags |= SPELL_INTERRUPT_FLAG_INTERRUPT;
                break;
            case 160382: ///< Defense (Security Guard)
            case 158246: ///< Hot Blooded (Foreman Feldspar)
            case 156932: ///< Rupture DoT (Foreman Feldspar)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_DONT_RESET_PERIODIC_TIMER;
                spellInfo->AttributesEx5 |= SPELL_ATTR5_HIDE_DURATION;
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(39); ///< 2s
                break;
            case 159558: ///< Bomb (Furnace Engineer)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_CAN_BE_CASTED_ON_ALLIES;
                break;
            case 155201: ///< Electrocution (Furnace Engineer)
                spellInfo->Effects[EFFECT_0].ChainTarget = 2;
                break;
            case 155196: ///< Fixate (Slag Elemental)
                spellInfo->MaxAffectedTargets = 1;
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                break;
            case 158702: ///< Fixate (Iron Eviscerator - Iron Maidens)
                spellInfo->MaxAffectedTargets = 1;
                spellInfo->Effects[EFFECT_1].ApplyAuraName = SPELL_AURA_DUMMY;
                break;
            case 159115: ///< Erupt (Firecaller)
                spellInfo->Attributes |= SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY;
                break;
            case 108977: ///< Way of the Monk
                spellInfo->Effects[EFFECT_1].Effect = SPELL_EFFECT_DUMMY;
                break;
            case 208631: ///< Summon Boat
                spellInfo->Effects[EFFECT_0].MiscValue = 0;
                break;
            case 178209: ///< Chest of Iron (T17)
            case 178210: ///< Legs of Iron (T17)
            case 178211: ///< Gauntlets of the Iron Conqueror (T17)
            case 178212: ///< Helm of Iron (T17)
            case 178213: ///< Shoulders of Iron (T17)
            case 178216: ///< Helm of Iron (Normal - T17 - Quest)
            case 178217: ///< Chest of Iron (Normal - T17 - Quest)
            case 178218: ///< Legs of Iron (Normal - T17 - Quest)
            case 178219: ///< Gauntlets of Iron (Normal - T17 - Quest)
            case 178220: ///< Shoulders of Iron (Normal - T17 - Quest)
            case 178221: ///< Legs of Iron (Heroic - T17 - Quest)
            case 178222: ///< Gauntlets of Iron (Heroic - T17 - Quest)
            case 178223: ///< Shoulders of Iron (Heroic - T17 - Quest)
            case 178224: ///< Helm of Iron (Heroic - T17 - Quest)
            case 178225: ///< Chest of Iron (Heroic - T17 - Quest)
            case 178226: ///< Helm of Iron (Mythic - T17 - Quest)
            case 178227: ///< Chest of Iron (Mythic - T17 - Quest)
            case 178228: ///< Legs of Iron (Mythic - T17 - Quest)
            case 178229: ///< Gauntlets of Iron (Mythic - T17 - Quest)
            case 178230: ///< Shoulders of Iron (Mythic - T17 - Quest)
                spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_DUMMY;
                break;
            case 155200: ///< Burn (Slag Elemental)
            case 155890: ///< Molten Torrent (Dummy visual - Molten Torrent Stalker)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            case 155747: ///< Body Slam
            case 157923: ///< Jump Slam
                spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_DUMMY;
                spellInfo->Effects[EFFECT_0].ValueMultiplier = 30;
                break;
            case 156324: ///< Acid Torrent (AoE)
            case 155225: ///< Melt (Heart of the Mountain)
            case 164279: ///< Penetrating Shot - Cylinder damage (Admiral Gar'an - Iron Maidens)
                spellInfo->AttributesEx3 |= SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                break;
            case 177756: ///< Deafening Roar (Bellows Operator)
                spellInfo->EffectCount = 1;
                spellInfo->Effects[EFFECT_1].Effect = 0;
                spellInfo->Effects[EFFECT_1].TriggerSpell = 0;
                break;
            case 177858: ///< Ember in the Wind (Mol'dana Two Blade)
                spellInfo->TargetAuraSpell = 177855;    ///< Ember in the Wind (aura)
                break;
            case 177891: ///< Rising Flame Kick (Mol'dana Two Blade)
            case 177855: ///< Ember in the Wind (aura - Mol'dana Two Blade)
            case 154932: ///< Molten Torrent (aura - Flamebender Ka'graz)
            case 161570: ///< Searing Plates (DoT - Franzok)
            case 159481: ///< Delayed Siege Bomb (Channel - Iron Gunnery Sergeant)
            case 164271: ///< Penetrating Shot - Aura (Admiral Gar'an - Iron Maidens)
            case 158010: ///< Bloodsoaked Heartseeker - Marker (Marak the Blooded - Iron Maidens)
            case 159724: ///< Blood Ritual - Aura (Marak the Blooded - Iron Maidens)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 156039: ///< Drop the Hammer (Aknor Steelbringer)
            case 155571: ///< Jump Out of Lava (Cinder Wolf)
            case 162285: ///< Rend and Tear (Beastlord Darmac)
            case 162279: ///< Rend and Tear (Beastlord Darmac)
            case 155567: ///< Rend and Tear (Cruelfang)
            case 155060: ///< Rend and Tear (Cruelfang)
                spellInfo->Effects[EFFECT_0].ValueMultiplier = 50;
                break;
            case 174215: ///< Summon Armament (Flamebender Ka'graz)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_TARGET_BACK;
                break;
            case 163644: ///< Summon Enchanted Armament (Flamebender Ka'graz)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_NEARBY_ENTRY;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(187);  ///< 300y
                break;
            case 174217: ///< Summon Enchanted Armament (Enchanted Armament)
                spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(187);  ///< 300y
                break;
            case 163153: ///< Enchant Armament (Jump - Enchanted Armament)
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                spellInfo->Effects[EFFECT_0].ValueMultiplier = 50;
                spellInfo->Effects[EFFECT_0].MiscValueB = 300;
                break;
            case 155074: ///< Charring Breath (Cinder Wolf)
            case 173790: ///< Spirit Bond (Stubborn Ironhoof)
                spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                break;
            case 151271: ///< Collect Mask
                spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_NONE;
                break;
            case 155049: ///< Singe (Cinder Wolf)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_DONT_RESET_PERIODIC_TIMER;
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 154938: ///< Molten Torrent (AoE Damage - 154938)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_SHARE_DAMAGE;
                break;
            case 162349: ///< Fists of Stone (Kromog)
                spellInfo->AttributesEx3 |= SPELL_ATTR3_BLOCKABLE_SPELL;
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_SHARE_DAMAGE;
                break;
            case 155745: ///< Charring Breath (Jump - Overheated Cinderwolf)
                spellInfo->Attributes |= SPELL_ATTR0_HIDDEN_CLIENTSIDE;
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[EFFECT_1].Effect = 0;
                break;
            case 154952: ///< Fixate (Cinder Wolf)
                spellInfo->MaxAffectedTargets = 1;
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                spellInfo->InterruptFlags &= ~SPELL_INTERRUPT_FLAG_MOVEMENT;
                break;
            case 163633: ///< Magma Monsoon
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                break;
            case 161049: ///< Rippling Smash (Kromog)
                spellInfo->MaxAffectedTargets = 1;
                break;
            case 157060: ///< Rune of Grasping Earth - Cast (Kromog)
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_PERIODIC_TRIGGER_SPELL;
                break;
            case 157054: ///< Thundering Blows (Kromog)
                spellInfo->Effects[EFFECT_0].TriggerSpell = 162355;
                spellInfo->Effects[EFFECT_1].TriggerSpell = 157055;
                break;
            case 161923: ///< Rune of Crushing Earth - Damage (Stone Wall)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_ENEMY_BETWEEN_DEST;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_ENEMY_BETWEEN_DEST;
                spellInfo->Effects[EFFECT_1].TargetB = 0;
                spellInfo->Effects[EFFECT_2].TargetA = TARGET_UNIT_ENEMY_BETWEEN_DEST;
                spellInfo->Effects[EFFECT_2].TargetB = 0;
                break;
            case 154951: ///< Pin Down (Beastlord Darmac)
            case 163045: ///< Flame Vent Cosmetics (Flame Vent)
            case 158599: ///< Deploy Turret (Admiral Gar'an - Iron Maidens)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            case 186260: ///< Harpon
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_TARGET_ENEMY;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            case 155653: ///< Flame Infusion (Pack Beast)
                spellInfo->Attributes |= SPELL_ATTR0_CASTABLE_WHILE_DEAD;
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                break;
            case 155198: ///< Savage Howl (Cruelfang)
                spellInfo->PreventionType = 0;
                break;
            case 155221: ///< Tantrum (Ironcrusher)
            case 155520: ///< Tantrum (Beastlord Darmac)
                spellInfo->AuraInterruptFlags = 0;
                spellInfo->ChannelInterruptFlags = 0;
                spellInfo->InterruptFlags = 0;
                break;
            case 156294: ///< Throw Grenade (Iron Raider)
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            case 160177: ///< Cautorizing Bolt (Grom'kar Firemender)
                spellInfo->MaxAffectedTargets = 1;
                break;
            case 159480: ///< Delayed Siege Bomb - Searcher (Iron Gunnery Sergeant)
                spellInfo->Effects[EFFECT_1].Effect = 0;
                break;
            case 159482: ///< Delayed Siege Bomb - Missile (Iron Gunnery Sergeant)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_TARGET_ENEMY;
                break;
            case 158084: ///< Delayed Siege Bomb - Damage (Iron Gunnery Sergeant)
            case 160050: ///< Delayed Siege Bomb - Damage (Operator Thogar)
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                break;
            case 171209: ///< Load Crate (Iron Dockworker - Iron Maidens intro)
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                break;
            case 158008: ///< Bloodsoaked Heartseeker - Cast (Marak the Blooded - Iron Maidens)
            case 159585: ///< Deploy Turret - Jump (Admiral Gar'an - Iron Maidens)
                spellInfo->SpellPowers.clear();
                break;
            case 156626: ///< Rapid Fire (Admiral Gar'an)
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_PERIODIC_DUMMY;
                break;
            case 158681: ///< Corrupted Blood - Missile (Uk'urogg - Iron Maidens)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_TARGET_ENEMY;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            case 157056: ///< Rune of Grasping Earth
                spellInfo->AttributesEx6 |= SPELL_ATTR6_CAN_TARGET_UNTARGETABLE;
                break;
            ///////////////////////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////////////////////
            /// Skills
            ///////////////////////////////////////////////////////////////////////////////////
            case 203139: ///< Arcway's Advisor Vandros Force Nova DOT
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(6); ///< 0 to 100 yards
                spellInfo->AttributesEx6 &= ~SPELL_ATTR6_NO_DONE_PCT_DAMAGE_MODS;
                break;
            case 217725: ///< Arc Weld - Arcway
                spellInfo->Targets |= TARGET_FLAG_UNIT;
                break;
            case 211917: ///< Felstorm
                spellInfo->Effects[0].TriggerSpell = 211921;
                break;
            case 196562: ///< Volatile Magic
                spellInfo->StackAmount = 1;
                break;
            case 169092: ///< Temporal Crystal
                spellInfo->Effects[EFFECT_0].BasePoints = 1;
                spellInfo->Effects[EFFECT_0].DieSides = 0;
                break;
            case 196824: ///< Nether Link DOT
                spellInfo->AttributesEx5 &= ~SPELL_ATTR5_START_PERIODIC_AT_APPLY;
                break;
            ///////////////////////////////////////////////////////////////////////////////////
            case 113095: ///< Demon Hunter's Aspect
                spellInfo->Effects[EFFECT_0].MiscValue = 21178;
                break;
            case 163661: ///< Cho'gall Night
                spellInfo->EffectCount = 2;
                spellInfo->Effects._effects.push_back(SpellEffectInfo(sSpellStore.LookupEntry(spellInfo->Id), spellInfo, EFFECT_1, Difficulty::DifficultyNone));
                spellInfo->Effects[EFFECT_1].Effect = SPELL_EFFECT_APPLY_AURA;
                spellInfo->Effects[EFFECT_1].ApplyAuraName = SPELL_AURA_PHASE;
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[EFFECT_1].MiscValue = 2;
                break;
            case 168178: ///< Salvage (garrison loot spell)
            case 168179: ///< Salvage (garrison loot spell)
            case 168180: ///< Salvage (garrison loot spell)
                spellInfo->Effects[0].Effect = SPELL_EFFECT_CREATE_RANDOM_ITEM;
                break;
            case 167650: ///< Loose Quills (Rukhmar)
            case 167630: ///< Blaze of Glory (Rukhmar)
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_5_YARDS); ///< 5yd
                break;
            case 178851: ///< Rukhmar Bonus
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                break;
            case 128254: ///< Brew Finale Wheat Effect (Yan-Zhu - Stormstout Brewery)
            case 128256: ///< Brew Finale Medium Effect (Yan-Zhu - Stormstout Brewery)
            case 128258: ///< Brew Finale Dark Effect (Yan-Zhu - Stormstout Brewery)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_NEARBY_ENEMY;
                break;
            case 133601: ///< Durumu Debuff 2A (Durumu - Throne of Thunder)
                spellInfo->Effects[EFFECT_0].BasePoints *= 35000;
                break;
            case 134169:
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Attributes &= ~SPELL_ATTR0_HIDDEN_CLIENTSIDE;
                break;
            case 140016: ///< Drop Feathers (Ji Kun - Throne of Thunder) (ToT - #6 Ji Kun)
                spellInfo->Effects[EFFECT_0].MiscValue = 218543;
                break;
            case 134755: ///< Eye Sore (Durumu - Throne of Thunder)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 133740: ///< Bright Light (Durumu - Throne of Thunder)
                spellInfo->Effects[EFFECT_1].MiscValue = 0;
                spellInfo->AttributesEx8 &= ~SPELL_ATTR8_UNK27;
                spellInfo->AttributesEx &= ~SPELL_ATTR1_CHANNEL_TRACK_TARGET;
                spellInfo->Effects[EFFECT_3].TriggerSpell = 0;
                break;
            case 133795: ///< Drain Life (Durumu - Throne of Thunder)
                spellInfo->Effects[EFFECT_2].TargetA = TARGET_UNIT_TARGET_ANY;
                break;
            case 133798: ///< Drain Life (Durumu - Throne of Thunder)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[EFFECT_1].TargetB = TARGET_UNIT_TARGET_ANY;
                break;
            case 133796: ///< Drain Life (Durumu - Throne of Thunder)
            case 138908: ///< Transfusion (Dark Animus - Throne of Thunder)
            case 147234: ///< Dummy Nuke (Iron Qon - Throne of Thunder)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                break;
            case 140495: ///< Lingering Gaze (Durumu - Throne of Thunder)
                spellInfo->Effects[EFFECT_0].BasePoints = int32(spellInfo->Effects[EFFECT_0].BasePoints * 2.9f);
                break;
            case 136413: ///< Force of Will (Durumu - Throne of Thunder)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CONE_ENEMY_104;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            case 138378: ///< Transfusion (Dark Animus - Throne of Thunder)
                spellInfo->ExplicitTargetMask = 0;
                break;
            case 101257: ///< Wracking Pain dmg
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_SRC_CASTER;
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_100_YARDS);
                break;
            case 136954: ///< Anima Ring (Dark Animu - Throne of Thunder)
                for (uint8 l_Itr = 0; l_Itr < 12; ++l_Itr)
                    spellInfo->Effects[l_Itr].TriggerSpell = 0;
                break;
            case 136955: ///< Anima Ring (Triggered) (Dark Animus - Throne of Thunder)
            case 136956: ///< Anima Ring (Triggered) (Dark Animus - Throne of Thunder)
            case 136957: ///< Anima Ring (Triggered) (Dark Animus - Throne of Thunder)
            case 136958: ///< Anima Ring (Triggered) (Dark Animus - Throne of Thunder)
            case 136959: ///< Anima Ring (Triggered) (Dark Animus - Throne of Thunder)
            case 136960: ///< Anima Ring (Triggered) (Dark Animus - Throne of Thunder)
            case 138671: ///< Anima Ring (Triggered) (Dark Animus - Throne of Thunder)
            case 138672: ///< Anima Ring (Triggered) (Dark Animus - Throne of Thunder)
            case 138673: ///< Anima Ring (Triggered) (Dark Animus - Throne of Thunder)
            case 138674: ///< Anima Ring (Triggered) (Dark Animus - Throne of Thunder)
            case 138675: ///< Anima Ring (Triggered) (Dark Animus - Throne of Thunder)
            case 138676: ///< Anima Ring (Triggered) (Dark Animus - Throne of Thunder)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                spellInfo->ExplicitTargetMask = TARGET_FLAG_DEST_LOCATION;
                break;
            case 85739: /// Meat Cleaver
                spellInfo->ProcFlags = 0;
                spellInfo->Effects[EFFECT_0].SpellClassMask[3] = 0x09001000;
                break;
            case 12950: /// Whirlwind Aura
                spellInfo->ProcFlags = 0;
                break;
            case 136962: ///< Anima Ring (Debuff) (Dark Animus - Throne of Thunder)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 209651: ///< Shattered Souls
                spellInfo->Effects[EFFECT_1].TriggerSpell = 226263;
                break;
            case 204254: ///< Shattered Souls (Vengeance)
            case 178940: ///< Shattered Souls (Havoc)
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_PROC_TRIGGER_SPELL;
                spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                break;
            case 228533: ///< Shattered Souls (Vengeance GREATER souls when u kill someone)
                spellInfo->Effects[1].TriggerSpell = 226263;
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_DEST_CASTER_RANDOM;
                spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(72); ///< 10-20 yards
                break;
            case 138613: ///< Matter Swap (Dark Animus - Throne of Thunder)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                break;
            case 139803: /// Triumphant Rush (ToT - #10 Iron Qon)
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_DEST_AREA_ENEMY;
                spellInfo->Effects[EFFECT_1].TargetB = TARGET_UNIT_DEST_AREA_ENEMY;
                spellInfo->Effects[EFFECT_2].TargetB = TARGET_UNIT_DEST_AREA_ENEMY;
                break;
            case 167615: ///< Pierced Armor
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 97709:  ///< Altered Form (Racial)
                spellInfo->AttributesEx4 |= SPELL_ATTR4_UNK21;
                break;
            case 159407: ///< Combo Breaker: Chi Explosion
                spellInfo->ProcCharges = 0;
                spellInfo->ProcFlags = 0;
                spellInfo->ProcChance = 0;
                break;
                /// Shadowmoon Burial Grounds
            case 153068: ///< Void Devestation
                spellInfo->Effects[0].TargetA = TARGET_DEST_CASTER;
                spellInfo->Effects[0].TargetB = TARGET_DEST_DEST_RADIUS;
                spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(10); // 30y
                break;
            case 152962: ///< Soul Steal
                spellInfo->Effects[0].TriggerSpell = 0;
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[0].TargetB = 0;
                break;
            case 152979: ///< Soul Shreads
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[1].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 164693: ///< Lunar Runes
            case 164695:
            case 164696:
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(4); ///< 120s
                break;
            case 154327: ///< Domination
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                break;
            case 153164: ///< Dark Communion
                spellInfo->Effects[0].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[1].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[2].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[0].TargetB = 0;
                spellInfo->Effects[1].TargetB = 0;
                spellInfo->Effects[2].TargetB = 0;
                break;
            case 153153: ///< Dark Communion
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[0].TargetB = 0;
                break;
            case 153501: ///< Void Blast
            case 153070: ///< Void Devestation
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(39); // 2s
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 173073: ///< Ground Marker
                sSpellDurationStore.LookupEntry(1); ///< 10s
                break;
            case 153686: ///< Body Slam
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                break;
            case 154469: ///< Ritaul Of Bones - Darkness
            case 153692: ///< Necrotic Pitch Debuff
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(39); // 2s
                break;
            case 153236: ///< DaggerFall
                spellInfo->Effects[0].TargetA = 0;
                spellInfo->Effects[0].TargetB = 0;
                break;
            case 107079: ///< Quaking Palm
                spellInfo->SpellFamilyName = SPELLFAMILY_GENERIC;
                break;
            case 164685: ///< Dark Eclipse
                spellInfo->Effects[0].Amplitude = 600;
                break;
            case 219655: ///< Artificial Damage
                spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_APPLY_AURA;
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[EFFECT_0].MiscValue = 127;
                break;
            case 197912:
                spellInfo->Effects[EFFECT_2].Effect = SPELL_EFFECT_DUMMY;
                spellInfo->Effects[EFFECT_2].ApplyAuraName = 0;
                break;
            case 196757: ///< Craft Skyhorn Kite
            case 228214: ///< Solving Puzzle
                spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_CREATE_LOOT;
                break;
                /// Everbloom
            case 164643: ///< Rending Charge
            case 164886: ///< Dreadpetal Toxin
            case 169658: ///< Infected Wounds
            case 164965: ///< Choking Vines
            case 164834: ///< Barrage of Leaves
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 169326: ///< Xeritac Beam
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                break;
            case 166491: ///< FireBloom - experimental, trying to produce triggers
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[0].TargetB = 0;
                break;
            case 169223: ///< Toxic Gas
            case 166492: ///< Firebloom
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(36); // 1s
                break;
            case 167977: ///< Bramble Patch
            case 169495: ///< Living Leaves
            case 164294: ///< Unchecked Growth
            case 166726: ///< Frozen Rain
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(36); // 1s
                break;
            case 173080: ///< Fixate
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(36); // 1s
                spellInfo->Effects[1].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[1].TargetB = 0;
                break;
            case 169322: ///< Descend Beam
            case 143569: ///< Sand Beam
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                break;
            case 169376: ///< Venomous Sting
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                spellInfo->Effects[0].TargetB = 0;
                break;
            case 164885: ///< Dreadpetal Toxin
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
                /// Iron Docks
            case 163665:  ///< Flaming Slash
                spellInfo->Effects[0].Effect = 0;
                break;
            case 163705:  ///< Abrupt Restoration
                spellInfo->Effects[0].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[0].TargetB = 0;
                break;
            case 178154:  ///< Acid Spit
            case 178155:  ///< Acid Spit Trigger Missile
            case 163689:  ///< Sanguine Sphere
            case 176287:  ///< Chain Drag
            case 172885:  ///< Charging Slash
            case 168348:  ///< Rapid Fire
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[0].TargetB = 0;
                break;
            case 168537:  ///< Cannon Barrage:
                spellInfo->Effects[0].Amplitude = 1000; // was 400
                break;
            case 164632:  ///< Burning Arrow
            case 164648:  ///< Barbed Arrow Dot
            case 173324:  ///< Jagged Caltrops Dot
            case 178156:  ///< Acid Splash
            case 172963:  ///  Gatecrasher
            case 173517:  /// Lava Blast
            case 164734:  /// Shredding Swipes
            case 173349:  /// Trampled
            case 173489:  /// Lava Barrage
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(36); // 1s
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 164504:  /// Initimidated
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 162424:  ///< Feeding Frenzy
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(563); // 20500ms
                break;
            case 163379:  ///< Big Boom
                spellInfo->InterruptFlags = 0;
                break;
            case 165152:  ///< Lava Sweep
                spellInfo->Effects[0].TargetA = TARGET_SRC_CASTER;
                spellInfo->Effects[0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                spellInfo->Effects[0].Amplitude = 1000;
                break;
            case 172636:  ///< Grease Vial
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(39); // 2s
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 166923:  ///< Barbed Arrow Barrage
                spellInfo->Effects[0].BasePoints = 164370;
                spellInfo->Effects[0].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[0].TargetB = 0;
                break;
            case 163740: ///< Tainted Blood
                // case 163668: ///< Flaming Slash
                spellInfo->Effects[0].TargetB = TARGET_UNIT_NEARBY_ENEMY;
            case 211006: ///< Mask of Mirror Image
                spellInfo->Effects[0].TargetA = TARGET_UNIT_NEARBY_ENTRY;
                spellInfo->Effects[0].TargetB = 0;
                spellInfo->Effects[1].TargetA = TARGET_UNIT_NEARBY_ENTRY;
                spellInfo->Effects[1].TargetB = 0;
                break;
            case 169132: ///< Back Draft
                spellInfo->Effects[0].TargetB = TARGET_DEST_DEST;
                break;
                /// Auchindoun
            case 157505: ///< Arcane Bolt
                spellInfo->Effects[0].Amplitude = 1500;
                break;
            case 154340: ///< Sanctified Ground
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(36); // 1s
                break;
            case 156862: ///< Drain Soul Cosmetic
                spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                break;
            case 153726: ///< Fel Spark
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(27); ///< 3s
            case 169682: ///< Azakkel visual pillar
                spellInfo->Effects[0].TargetA = TARGET_UNIT_CASTER;
                break;
            case 153775: ///< Summon Imp
            case 164127: ///< Summon Pyromaniac
            case 164080: ///< Summon FelGuard
                spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(10); ///< from 15.0f (RangeEntry.ID 11) to 40.0f
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                break;
            case 153430: ///< Areatrigger Damage
                spellInfo->Effects[0].Amplitude = 2000;
            case 119975: ///< Conversion
                spellInfo->AttributesEx8 |= SPELL_ATTR8_AURA_SEND_AMOUNT;
                break;
            case 178533: ///< Horde Reward
            case 178531: ///< Alliance Reward
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_SRC_CASTER;
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ALLY;
                spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(27);  ///< 50y
                break;
            case 143314: /// Glyph of Nightmares
                spellInfo->InterruptFlags &= ~AURA_INTERRUPT_FLAG_TAKE_DAMAGE;
                break;
            case 167625: ///< Blood feather
                spellInfo->Effects[0].Effect = SPELL_EFFECT_TRIGGER_SPELL;
                spellInfo->Effects[0].TriggerSpell = 167626; ///< Blood Feather
                break;
            case 167687: ///< Solar Breath
                spellInfo->Effects[0].TargetA = TARGET_UNIT_CONE_ENEMY_54;
                break;
            case 167629: ///< Blaze Of Glory
                spellInfo->Effects[1].TargetB = TARGET_UNIT_DEST_AREA_ENTRY;
                break;
            case 165907: ///< Earthrending Slam
                spellInfo->Effects[0].TargetA = TARGET_UNIT_CONE_ENEMY_54;
                spellInfo->Effects[1].TargetA = TARGET_UNIT_CONE_ENEMY_54;
                break;
            case 124694: ///< Way of the Grill
            case 125584: ///< Way of the Wok
            case 125586: ///< Way of the Pot
            case 125587: ///< Way of the Steamer
            case 125588: ///< Way of the Oven
            case 125589: ///< Way of the Brew
                spellInfo->Effects[EFFECT_0].BasePoints = 8;
                break;
            case 161299: ///< Impact Spit
                spellInfo->Effects[0].TriggerSpell = 161304;
                break;
            case 166452: ///< Tormented Soul
                spellInfo->Effects[1].Effect = 0;
                break;
            case 164042: ///< Taste of Iron Game Aura
                spellInfo->Effects[2].Effect = 0;
                spellInfo->Effects[3].Effect = 0;
                break;
            case 109150: ///< Demonic Leap (Backward)
                spellInfo->Effects[0].Effect = SPELL_EFFECT_KNOCK_BACK_DEST;
                spellInfo->Effects[0].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[0].TargetB = TARGET_DEST_CASTER_FRONT;
                spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(8); ///< 5 yards
                break;
            case 109166: ///< Demonic Leap (Forward Left)
                spellInfo->Effects[0].Effect = SPELL_EFFECT_KNOCK_BACK_DEST;
                spellInfo->Effects[0].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[0].TargetB = TARGET_DEST_CASTER_BACK_RIGHT;
                spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(8); ///< 5 yards
                break;
            case 133123: ///< Arcane Barrage
                spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(10); ///< from 15.0f (RangeEntry.ID 11) to 40.0f
                break;
            case 144757: /// Increased All Resist 05
                spellInfo->AttributesEx11 &= ~SPELL_ATTR11_SCALES_WITH_ITEM_LEVEL;
                break;
            case 170893:///< Kronus: Fracture
            case 177607:///< Fangraal: Entangling Roots
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_TARGET_ENEMY;
                break;
            case 165712:///< Stormshield Gladiator: Devotion Aura
                spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_APPLY_AREA_AURA_FRIEND;
                break;
            case 176172:///< Ancient Inferno: Molten Firestorm
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST_RANDOM;
                break;
            case 222944: ///< Inspiring Presence
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ALLY;
                spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(153); ///< 60 yards
                break;
            case 117032:
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_TARGET_ANY;
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_DEST_DEST_RANDOM;
                spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(29); ///< 6 yards
                break;
            case 175093:///< Alliance Reward (Ashran events)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[EFFECT_1].BasePoints = 50;
                break;
            case 174094:///< Horde Reward (Ashran events)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[EFFECT_1].MiscValue = 1681;
                break;
            case 162472:///< Earth Breaker (Vul'gor)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                break;
            case 162304:///< Earth Breaker (Summon - Vul'gor)
                spellInfo->Effects[EFFECT_0].Effect = 0;
                break;
            case 159995:///< Chain Hurl (Kargath)
            case 209313:///< Cosmetic Rope Jump (Understone Drudge)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                spellInfo->Effects[EFFECT_0].ValueMultiplier = 30.0f;
                spellInfo->Effects[EFFECT_0].MiscValueB = 250;
                break;
            case 160061:///< Chain Hurl (Kargath)
                spellInfo->Effects[EFFECT_0].ValueMultiplier = 30.0f;
                spellInfo->Effects[EFFECT_0].MiscValue = 250;
                spellInfo->Effects[EFFECT_0].MiscValueB = 250;
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                break;
            case 151991:///< Chain Grip (Gorian Guardsman)
                spellInfo->Effects[EFFECT_0].ValueMultiplier = 30.0f;
                spellInfo->Effects[EFFECT_0].MiscValue = 250;
                spellInfo->Effects[EFFECT_0].MiscValueB = 250;
                break;
            case 159113:///< Impale (Kargath)
                spellInfo->ChannelInterruptFlags = 0;
                break;
            case 159265:///< Blade Dance (charge visual - Kargath)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                break;
            case 158986:///< Berserker Rush (Kargath)
            case 162497:///< On the Hunt (Ravenous Bloodmaw - Kargath)
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                break;
            case 160953:///< Fire Bomb (Iron Bomber - Kargath)
                spellInfo->AttributesEx6 |= SPELL_ATTR6_CASTABLE_WHILE_ON_VEHICLE;
                break;
            case 159414:///< Mauling Brew (Drunken Bileslinger - Kargath)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_TARGET_ENEMY;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            case 159412:///< Mauling Brew (Drunken Bileslinger - Kargath)
                spellInfo->AttributesEx3 &= ~SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                break;
            case 161218:///< Maul (Ravenous Bloodmaw - Kargath)
                spellInfo->Effects[EFFECT_0].Effect = 0;
                if(spellInfo->DifficultyID == Difficulty::DifficultyRaidNormal)
                    spellInfo->Effects[EFFECT_2].Effect = 0;
                break;
            case 163408:///< Heckle (Kargath)
                spellInfo->CasterAuraSpell = 0;
                break;
            case 156127:///< Meat Hook (The Butcher)
                spellInfo->Effects[EFFECT_0].ValueMultiplier = 100.0f;
                break;
            case 156160:///< Bounding Cleave (The Butcher)
                spellInfo->AttributesEx3 |= SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_SRC_CASTER;
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_SRC_CASTER;
                spellInfo->Effects[EFFECT_1].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                spellInfo->Effects[EFFECT_2].TargetA = TARGET_SRC_CASTER;
                spellInfo->Effects[EFFECT_2].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                break;
            case 156171:///< Bounding Cleave (The Butcher)
            case 162398:///< Expel Magic: Arcane - Missile (Ko'ragh)
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            case 156172:///< Bounding Cleave (The Butcher)
            case 233484: ///< Meteor Slash (Brutallus)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_SHARE_DAMAGE;
                break;
            case 156157:///< Cleave (The Butcher)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_DEST_AREA_ENEMY;
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_SHARE_DAMAGE;
                break;
            case 163042:///< Pale Vitriol
                spellInfo->Attributes |= SPELL_ATTR0_CASTABLE_WHILE_DEAD;
                break;
            case 166225:///< Boar's Rush (Krush)
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(23); ///< 40 yards
                break;
            case 174465:///< Unstoppable Charge (Iron Flame Technician)
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                spellInfo->Effects[EFFECT_1].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                break;
            case 174473:///< Corrupted Blood (Iron Blood Mage)
                spellInfo->MaxAffectedTargets = 5;
                break;
            case 163113:///< Withering (Living Mushroom - Brackenspore)
            case 163124:///< Withering (Rejuvenating Mushroom - Brackenspore)
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_PERIODIC_DAMAGE_PERCENT;
                break;
            case 159996:///< Infested Spores (Brackenspore)
            {
                for (auto l_Iter : spellInfo->SpellPowers)
                {
                    ((SpellPowerEntry*)l_Iter)->Cost = 500;
                    ((SpellPowerEntry*)l_Iter)->PowerType = POWER_RAGE;
                }

                break;
            }
            case 163242:///< Infested Spores - triggered (Brackenspore)
                spellInfo->AttributesEx &= ~SPELL_ATTR1_CHANNELED_2;
                break;
            case 160446:///< Spore Shooter - summon (Brackenspore)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_SRC_CASTER;
                break;
            case 162346:///< Crystalline Barrage (Tectus)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 162371:///< Crystalling Barrage - Summon (Tectus)
            case 163208:///< Fracture - Missile (Tectus)
            case 174856:///< Knockback (Ko'ragh)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                break;
            case 117624:///< Suicide No Blood No Logging (Tectus)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                break;
            case 162475:///< Tectonic Upheaval (Tectus)
                spellInfo->InterruptFlags = 0;
                spellInfo->ChannelInterruptFlags = 0;
                break;
            case 166185:///< Rending Slash (Highmaul Conscript)
            case 158026:///< Enfeebling Roar - Debuff (Phemos - Twin Ogron)
            case 163134:///< Nullification Barrier - Abosrb (Ko'ragh)
            case 161345:///< Suppression Field - DoT (Ko'ragh)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 166199:///< Arcane Volatility (Gorian Arcanist)
            case 158521:///< Double Slash (Phemos - Twin Ogron)
            case 175598:///< Devouring Leap (Night-Twisted Devout)
            case 172747:///< Expel Magic: Frost - AreaTrigger (Ko'ragh)
                spellInfo->Effects[EFFECT_1].Effect = 0;
                break;
            case 158419:///< Pulverize - Third AoE (Pol - Twin Ogron)
                spellInfo->CastTimeEntry = sSpellCastTimesStore.LookupEntry(4); ///< 1s
                break;
            case 158241:///< Blaze - DoT (Phemos - Twin Ogron)
            case 174405:///< Frozen Core - DoT (Breaker Ritualist - Frost)
            case 173827:///< Wild Flames - DoT (Breaker Ritualist - Fire)
            case 161242:///< Caustic Energy - DoT (Ko'ragh)
            case 172813:///< Expel Magic: Frost - Decrease Speed (Ko'ragh)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_DONT_RESET_PERIODIC_TIMER;
                spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                break;
            case 172685:///< Expel Magic: Fire (Ko'ragh)
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ALLY;
                break;
            case 162397:///< Expel Magic: Arcane (Ko'ragh)
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_DEST_AREA_ALLY;
                break;
            case 161376:///< Volatile Anomalies - Missile 1 (Ko'ragh)
            case 161380:///< Volatile Anomalies - Missile 2 (Ko'ragh)
            case 161381:///< Volatile Anomalies - Missile 3 (Ko'ragh)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_SRC_CASTER;
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_DEST_CASTER_RANDOM;
                break;
            case 162595:///< Suppression Field - Silence (Ko'ragh)
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(39); ///< 2s
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 122121:
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(66);
                spellInfo->Effects[0].TargetA = SELECT_TARGET_SELF;
                spellInfo->ExplicitTargetMask = spellInfo->_GetExplicitTargetMask();
                break;
            case 165096:///< Ogreic Landing
                spellInfo->Effects[1].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                break;
            case 164868:///< Ogreic Landing
                spellInfo->Effects[0].TargetA = TARGET_DEST_DEST;
                break;
            case 164850:///< Nature Channeling (Cosmetic)
            case 175581:///< Void Touch
            case 175915:///< Acid Breath (Drov the Ruiner)
            case 139550:///< Torment
            case 138742:///< Chocking Sands
            case 99212: ///< Stormfire, Item - Shaman T12 Enhancement 4P Bonus
            case 116000:///< Voodoo Dolls
            case 38112: ///< Magic Barrier, Lady Vashj
            case 70602: ///< Corruption
            case 48278: ///< Paralyze
            case 65584: ///< Growth of Nature (Freya)
            case 64381: ///< Strength of the Pack (Auriaya)
            case 166289:///< Arcane Force (Gorian Arcanist)
                spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                break;
            case 203528: ///< Greater Blessing of Might
                spellInfo->Effects[0].Effect = SPELL_EFFECT_APPLY_AURA;
                spellInfo->Effects[1].Effect = SPELL_EFFECT_APPLY_AURA;
                break;
            case 211947: ///< Shadow Empowerment
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ALLY;
                spellInfo->Effects[0].TargetB = 0;
                break;
            case 150055:///< Volcanic Tantrum
            case 149963:///< Shatter Earth
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(27); // 3 sec
                break;
            case 150004:///< Magma Barrage
                spellInfo->InterruptFlags = SPELL_INTERRUPT_FLAG_MOVEMENT;
                spellInfo->Effects[EFFECT_1].Effect = 0;
                spellInfo->Effects[EFFECT_0].TriggerSpell = 150306;
                break;
            case 102401:///< Wild Charge (Unform spell)
                spellInfo->Effects[EFFECT_1].ValueMultiplier = 25.0f;
                spellInfo->Effects[EFFECT_1].MiscValue = 50;
                break;
            case 167718:///< Item - Monk T17 Mistweaver 4P Bonus
            case 17007:///< Leader of the Pack
            case 16864:///< Omen of Clarity
            case 16961:///< Primal Fury
            case 76672:///< Mastery: Hand of Light
            case 204023: ///< Crusader's Judgment
            case 186788: ///< Echo of the Highlord
                spellInfo->AttributesEx3 |= SPELL_ATTR3_CAN_PROC_WITH_TRIGGERED;
                break;
            case 30814: ///< Mental Quickness
                spellInfo->Effects[EFFECT_4].Effect = 0;
                break;
            case 155783: ///< Mastery: Nature's Guardian
                spellInfo->ProcFlags |= ProcFlags::PROC_FLAG_TAKEN_PERIODIC;
                spellInfo->Effects[1].ApplyAuraName = SPELL_AURA_DUMMY;
                break;
            case 149959: // Earth Shatter
            case 149968:
            case 149969:
            case 149970:
                spellInfo->Effects[0].TargetA = TARGET_DEST_DEST;
                spellInfo->ExplicitTargetMask = spellInfo->_GetExplicitTargetMask();
                break;
            case 175975:///< Genesis
                spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(10);  ///< 30y
                break;
            case 77442: ///< Focus
                spellInfo->Effects[0].Effect = 0;
                spellInfo->Effects[0].ApplyAuraName = 0;
                break;
            case 162537: ///< Poisoned Ammo
                spellInfo->Effects[0].ApplyAuraName = SPELL_AURA_PROC_TRIGGER_SPELL;
                break;
            case 162543: ///< Poisoned Ammo (triggered)
                spellInfo->Speed = 0.0f;
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_DONT_RESET_PERIODIC_TIMER;
                break;
            case 162546: ///< Frozen Ammo (triggered)
                spellInfo->Speed = 0.0f;
                break;
            case 121818: ///< Stampede
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ENEMY;
                spellInfo->Effects[EFFECT_1].TargetB = 0;
                break;
            case 45470: ///< Death Strike (no heal bonus in SPELL_DAMAGE_CLASS_NONE)
                spellInfo->DmgClass = SPELL_DAMAGE_CLASS_MELEE;
                break;
            case 117962: ///< Crackling Jade Shock
                spellInfo->Effects[0].Mechanic = MECHANIC_KNOCKOUT;
                break;
            case 154302: ///< Seal Conduit (first)
                spellInfo->MaxAffectedTargets = 1;
                break;
            case 154900: ///< Seal Conduit (second)
            case 160425: ///< Call of the Tides (Brackenspore)
                spellInfo->MaxAffectedTargets = 2;
                break;
            case 174627: ///< Fixate (Phantasmal Weapon)
                spellInfo->AttributesEx &= ~SPELL_ATTR1_CHANNELED_1;
                break;
            case 157278: ///< Awaken Runestone (Imperator Mar'gok)
                spellInfo->InterruptFlags = 0;
                spellInfo->ChannelInterruptFlags = 0;
                spellInfo->AuraInterruptFlags = 0;
                break;
            case 157763: ///< Fixate (Imperator Mar'gok)
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE_EFF0;
                break;
            case 137143:
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE_EFF0;
                break;
            case 156734: ///< Destructive Resonance - Summon (Imperator Mar'gok)
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            case 157265: ///< Volatile Anomalies (Imperator Mar'gok)
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_PERIODIC_DUMMY;
                spellInfo->Effects[EFFECT_1].ApplyAuraName = SPELL_AURA_PERIODIC_DUMMY;
                spellInfo->Effects[EFFECT_2].ApplyAuraName = SPELL_AURA_PERIODIC_DUMMY;
                break;
            case 158512: ///< Volatile Anomalies (Imperator Mar'gok)
            case 159158: ///< Volatile Anomalies (Imperator Mar'gok)
            case 159159: ///< Volatile Anomalies (Imperator Mar'gok)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(18);    ///< 15 yards
                break;
            case 156799: ///< Destructive Resonance (Other - Imperator Mar'gok)
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            case 158639: ///< Orbs of Chaos (1 - Imperator Mar'gok)
                for (uint8 l_I = SpellEffIndex::EFFECT_8; l_I < spellInfo->EffectCount; ++l_I)
                    spellInfo->Effects[l_I].Effect = 0;
                break;
            case 154901: ///< Seal Conduit (third)
                spellInfo->MaxAffectedTargets = 3;
                break;
            case 157347: ///< Fiery Charge
                spellInfo->Effects[0].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[1].Effect = 0;   ///< Need to be scripted
                break;
            case 77762: ///< Lava Surge
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(1); ///< 10s
                break;
            case 8188:   ///< Magma Totem Passive
            case 5672:   ///< Healing Streams
            case 114941: ///< Healing Tide
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_PERIODIC_TRIGGER_SPELL;
                spellInfo->Effects[EFFECT_0].Amplitude = 2000;
                break;
            case 154150: ///< Energize
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[1].TargetA = TARGET_UNIT_TARGET_ANY;
                break;
            case 45477: ///< Icy touch
                spellInfo->Effects[EFFECT_0].AttackPowerMultiplier = 0.319f;
                break;
            case 65075:  ///< Tower of Flames
            case 65077:  ///< Tower of Frost
            case 64482:  ///< Tower of Life
            case 55076:  ///< Sun Scope
            case 60023:  ///< Scourge Banner Aura
            case 66118:  ///< Leeching Swarm
            case 137502: ///< Growing Fury
            case 58105:  ///< Power of Shadron
            case 61248:  ///< Power of Tenebron
            case 61251:  ///< Power of Vesperon
            case 98255:  ///< Molten Armor
                spellInfo->AttributesEx4 |= SPELL_ATTR4_NOT_STEALABLE;
                break;
            case 132205: ///< Sha of Anger Bonus
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                break;
            case 137200: ///< Blessed Loa Spirit
                spellInfo->Effects[0].TargetA = TARGET_DEST_CASTER;
                spellInfo->Effects[0].TargetB = TARGET_DEST_DEST_RIGHT;
                break;
            case 8936: ///< Regrowth
                spellInfo->Effects[0].BasePoints = 1;
                break;
            case 136339: ///< Lightning Tether
                spellInfo->Effects[0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 136350: ///< Lightning Tether (periodic)
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                spellInfo->Effects[1].TargetA = TARGET_UNIT_TARGET_ENEMY;
                break;
            case 136340: ///< Storm Cloud
                spellInfo->Effects[0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 136345: ///< Storm Cloud (periodic)
                spellInfo->Effects[0].TargetB = TARGET_UNIT_SRC_AREA_ALLY;
                break;
            case 121673: ///< Fire Shot
                spellInfo->MaxAffectedTargets = 1;
                break;
            case 136050: ///< Malformed Blood
                spellInfo->Effects[1].BasePoints = int32(spellInfo->Effects[1].BasePoints * 2.85f);
                break;
            case 136521: ///< QuickSand
            case 136878: ///< QuickSand
                spellInfo->Effects[0].TriggerSpell = 0;
                break;
            case 137967: ///< Twisted Fate
                spellInfo->Effects[0].TargetA =  TARGET_UNIT_TARGET_ANY;
                break;
            case 134447: ///< Submerge
            case 122841: ///< Sha Active Sky
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(21); ///< Infinite
                break;
            case 134398: ///< Slime Trail (aura)
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(27); ///< 3s
                break;
            case 134531: ///< Web Thread
                spellInfo->AttributesEx &= ~SPELL_ATTR1_CHANNELED_1;
                break;
            case 132413: ///< Shadow Bulwark
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT;
                break;
            case 152150:///< Death from Above (periodic dummy)
                spellInfo->Attributes |= SPELL_ATTR0_IMPOSSIBLE_DODGE_PARRY_BLOCK;
                spellInfo->Effects[5].TargetA = TARGET_UNIT_TARGET_ENEMY;
                spellInfo->PreventionType = SpellPreventionMask::Pacify;
                break;
            case 178236:///< Death from Above (jump dest)
                spellInfo->Effects[0].TargetB = TARGET_DEST_TARGET_BACK;
                spellInfo->Effects[0].ValueMultiplier = 40.0f;
                break;
            case 139498: ///< Web Spray
                spellInfo->Effects[1].TriggerSpell = 0;
                break;
            case 138359: ///< Jump to Boss Platform
                spellInfo->Effects[0].TargetA = TARGET_DEST_DEST;
                break;
            case 51128: ///< Killing Machine
                spellInfo->ProcChance = 0;
                break;
            case 137641: ///< Soul Fragment
                spellInfo->Effects[1].BasePoints = int32(spellInfo->Effects[1].BasePoints * 2.7f);
                break;
            case 174597:///< Ruthlessness (passive aura)
                spellInfo->Effects[0].Effect = 0;
                break;
            case 137650: ///< Shadowed Soul
                spellInfo->Effects[0].BasePoints = 3;
                spellInfo->Effects[1].BasePoints = 3;
                break;
            case 159234: ///< Thunderlord
            case 159675: ///< Warsong
            case 159676: ///< Frostwolf
            case 173322: ///< BleedingHollow
            case 159238: ///< Shattered Bleed
            case 118334: ///< Dancing Steel (agility)
            case 118335: ///< Dancing Steel (strength)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_ENCHANT_STACK;
                break;
            case 1329: ///< Mutilate
                spellInfo->AttributesEx3 &= ~SPELL_ATTR3_CANT_TRIGGER_PROC;
                break;
            case 113828: ///< Healing Touch (treant)
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ALLY;
                spellInfo->Effects[0].TargetB = 0;
                break;
            case 142421: ///< Swiftmend (treant)
                spellInfo->Effects[1].TargetA = TARGET_DEST_TARGET_ANY;
                break;
            case 129869: ///< Strike from the Heavens
                spellInfo->Effects[0].TriggerSpell = 129639;
                break;
            case 165376: ///< Enlightenment
                spellInfo->Effects[1].ApplyAuraName = SPELL_AURA_MOD_CRIT_PCT;
                break;
            case 139834: ///< Cinders (summon)
                spellInfo->Effects[0].TargetA = TARGET_DEST_TARGET_ENEMY;
                break;
            case 140620: ///< Fungi Spores
                spellInfo->Effects[0].TargetB = TARGET_UNIT_DEST_AREA_ENEMY;
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 140621: ///< Fungi Spores (damage)
            case 140619: ///< Crystal Barbs
            case 140598: ///< Fungal Explosion (damage)
            case 139850: ///< Acid Rain (damage)
                spellInfo->Effects[0].TargetB = TARGET_UNIT_DEST_AREA_ENEMY;
                break;
            case 137575: ///< Frostbite (aura)
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[0].TargetB = 0;
                break;
            case 136990: ///< Frostbite
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[0].TargetB = 0;
                break;
            case 136917: ///< Biting Cold
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[0].TargetB = 0;
                break;
            case 136467: ///< Lingering Presence
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[1].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[1].TriggerSpell = 0;
                break;
            case 137131: ///< Reckless Charge
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                break;
            case 137133: ///< Reckless Charge
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                spellInfo->Effects[0].TargetB = 0;
                break;
            case 137117: ///< Reckless Charge (Rolling)
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                break;
            case 140138: ///< Nether Tear
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                break;
            case 214871: ///< Odyn's Fury
            case 163294: ///< Darkmoon Card of Draenor
                spellInfo->Effects[0].Effect = SPELL_EFFECT_DUMMY;
                break;
            case 138652: ///< Eruption
                spellInfo->Effects[0].Effect = SPELL_EFFECT_DUMMY;
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                break;
            case 140946: ///< Dire Fixation
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[0].ApplyAuraName = SPELL_AURA_DUMMY;
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 136644: ///< Venomous Effusion
                spellInfo->Effects[1].Effect = 0;
                spellInfo->Effects[2].Effect = 0;
                spellInfo->Effects[3].Effect = 0;
                break;
            case 136654: ///< Rending Charge
                spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(153); ///< 60 yards
                break;
            case 204150: ///< Aura of Sacrifice
            case 136740: ///< Double Swipe (back)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_CONE_BACK;
                break;
            case 136797: ///< Dino Mending
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[1].TargetA = TARGET_UNIT_TARGET_ANY;
                break;
            case 139900: ///< Stormcloud
                spellInfo->Effects[0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 137633: ///< Crystal Shell (damage absorb)
            case 137648: ///< Crystal Shell (heal absorb)
            case 134916: ///< Decapitate (debuff) (Lei Shen - Throne of Thunder)
                spellInfo->Attributes |= SPELL_ATTR0_CANT_CANCEL;
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 134030: ///< Kick Shell
                spellInfo->Effects[0].Effect = SPELL_EFFECT_APPLY_AURA;
                break;
            case 134476: ///< Rockfall (large damage)
                spellInfo->Effects[0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                break;
            case 255667: ///< Ethereal Connection
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_REDUCE_ITEM_MODIFY_COST;
                spellInfo->Effects[EFFECT_0].BasePoints = -spellInfo->Effects[EFFECT_0].BasePoints;
                break;
            case 135103: ///< Drain the Weak
                spellInfo->Effects[0].TriggerSpell = 0;
                break;
            case 137313: ///< Lightning Storm
                spellInfo->Effects[1].TriggerSpell = 0;
                break;
            case 157096: ///< Empowered Demons
                spellInfo->Effects[2].BasePoints = 0;
                spellInfo->Effects[3].BasePoints = 0;
                break;
            case 138732: ///< Ionization
                spellInfo->Effects[0].TargetA = TARGET_SRC_CASTER;
                break;
            case 137422: ///< Focused Lightning (eyes)
                spellInfo->Effects[0].ApplyAuraName = SPELL_AURA_DUMMY;
                break;
            case 140555: ///< Lightning Storm (cosmetic - not sure)
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(37); ///< 1ms
                break;
            case 244910:
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(1); ///< 10 secs - prevent exploits
                break;
            case 137145: ///< Conductive Water (summon)
                spellInfo->Effects[0].TargetA = TARGET_DEST_DEST;
                break;
            case 139364: ///< Spirit Lantern
                spellInfo->Effects[1].Effect = 0;
                break;
            case 139461: ///< Spirit Light
                spellInfo->Effects[0].TargetA = TARGET_DEST_TARGET_ENEMY;
                spellInfo->Effects[0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                spellInfo->Effects[1].TargetA = TARGET_UNIT_TARGET_ENEMY;
                spellInfo->Effects[2].TargetA = TARGET_DEST_TARGET_ENEMY;
                spellInfo->Effects[2].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                break;
            case 79423: ///< Reaping Blows
            case 137905: ///< Lightning Diffusion
                spellInfo->Effects[0].TargetA = TARGET_SRC_CASTER;
                spellInfo->Effects[0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(7); ///< 2 yards
                break;
            case 138470: ///< Conductive Water (Damage taken)
            case 138006: ///< Electrified Waters (periodic damage)
                spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(10); ///< 30 yards
                spellInfo->Effects[0].TargetA = TARGET_SRC_CASTER;
                spellInfo->Effects[0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                break;
            case 138002: ///< Conductive Water (Damage - Healing done)
                spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(10); ///< 30 yards
                spellInfo->Effects[0].TargetA = TARGET_SRC_CASTER;
                spellInfo->Effects[0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(10); ///< 30 yards
                spellInfo->Effects[1].TargetA = TARGET_SRC_CASTER;
                spellInfo->Effects[1].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                spellInfo->Effects[2].RadiusEntry = sSpellRadiusStore.LookupEntry(10); ///< 30 yards
                spellInfo->Effects[2].TargetA = TARGET_SRC_CASTER;
                spellInfo->Effects[2].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                spellInfo->Effects[3].RadiusEntry = sSpellRadiusStore.LookupEntry(10); ///< 30 yards
                spellInfo->Effects[3].TargetA = TARGET_SRC_CASTER;
                spellInfo->Effects[3].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                break;
            case 137261: ///< Lightning Storm (damage)
                spellInfo->Effects[0].TargetA = TARGET_SRC_CASTER;
                break;
            case 137530: ///< Focused Lightning Conduction
            case 138133: ///< Lightning Fissure Conduction
                spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(10); ///< 30 yards
                spellInfo->Effects[0].TargetA = TARGET_SRC_CASTER;
                spellInfo->Effects[0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                break;
            case 137484: ///< Lightning Fissure (periodic)
                spellInfo->Effects[0].TargetA = TARGET_UNIT_CASTER;
                break;
            case 137485: ///< Lightning Fissure (damage)
                spellInfo->Effects[0].TargetA = TARGET_SRC_CASTER;
                spellInfo->Effects[0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(8); ///< 5 yards
                break;
            case 137194: ///< Focused Lightning
                spellInfo->CastTimeEntry = sSpellCastTimesStore.LookupEntry(5); ///< 2.000
                break;
            case 139218: ///< Storm Weapon
                spellInfo->Effects[0].TriggerSpell = 0;
                break;
            case 113504: ///< Wind Sword
                spellInfo->Effects[0].BasePoints *= 10;
                break;
            case 119691: ///< Heavy Dynamite
                spellInfo->Effects[0].BasePoints *= 31;
                break;
            case 89909: ///< Water Spout
                spellInfo->Effects[0].BasePoints *= 22;
                break;
            case 118600: ///< Chi Torpedo
                spellInfo->Effects[0].BasePoints *= 10;
                break;
            case 118592: ///< Spinning Crane Kick
                spellInfo->Effects[0].BasePoints = urand(180, 230);
                break;
            case 48505: ///< Starfall
                spellInfo->Effects[1].Effect = SPELL_EFFECT_APPLY_AURA;
                spellInfo->Effects[1].ApplyAuraName = SPELL_AURA_DUMMY;
                spellInfo->Effects[1].BasePoints = 20;
                break;
            case 119539: ///< Chi Torpedo
                spellInfo->Effects[0].TriggerSpell = 0;
                break;
            case 152261: ///< Holy Shield
                spellInfo->Effects[2].Effect = 0;
                break;
            /// Curiously, these spells grants Intellect ...
            case 146046: ///< Expanded Mind
            case 148897: ///< Extravagant Visions
            case 148906: ///< Toxic Power
            case 148908: ///< Mark of Salvation
                spellInfo->Effects[0].BasePoints = 963;
                break;
            case 53390: ///< Tidal Waves
                spellInfo->Effects[0].BasePoints = -20;
                spellInfo->Effects[1].BasePoints = 30;
                spellInfo->Effects[0].m_CanBeStacked = false;
                spellInfo->Effects[1].m_CanBeStacked = false;
                break;
            case 164991: ///< Entangling Energy
            case 15286: ///< Vampiric Embrace
                spellInfo->Effects[0].ApplyAuraName = SPELL_AURA_DUMMY;
                break;
            case 15290:///< Vampiric Embrace (Heal)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ALLY;
                break;
            case 126408: ///< Forward Thrust
                spellInfo->Effects[0].Effect = SPELL_EFFECT_KNOCK_BACK_DEST;
                spellInfo->Effects[0].TargetB = TARGET_DEST_CASTER_BACK;
                spellInfo->Effects[0].BasePoints = 75;
                spellInfo->Effects[0].MiscValue = 250;
                spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(8);
                break;
            case 119403: ///< Glyph of Explosive Trap
                spellInfo->Effects[0].ApplyAuraName = SPELL_AURA_DUMMY;
                break;
            case 149575: ///< Explosive Trap (knock back)
                spellInfo->Effects[0].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[1].TargetA = TARGET_DEST_DEST;
                break;
            case 13813:  ///< Explosive Trap
                spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(8); ///< 5 yards
                break;
            case 115176: ///< Zen Meditation
                spellInfo->ProcFlags = 0;
                spellInfo->ProcCharges = 0;
                spellInfo->Effects[0].BasePoints = 5;
                break;
            case 137639: ///< Storm, Earth and Fire
                spellInfo->AttributesCu &= ~(SPELL_ATTR0_CU_NEGATIVE_EFF1 | SPELL_ATTR0_CU_NEGATIVE_EFF0);
                break;
            case 138130: ///< Storm, Earth and Fire (for spirits)
                spellInfo->Effects[0].Effect = 0;
                break;
            case 5740:   ///< Rain of Fire
                spellInfo->Effects[0].TargetB = 0;
                spellInfo->AttributesEx6 |= SPELL_ATTR6_CAN_TARGET_INVISIBLE;
                break;
            /*case 130393: ///< Blink Strike
                spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(10); ///< 30y
                break;*/
            case 138234: ///< Lightning Storm (damage) (Lei Shen trash - Throne of Thunder)
            case 81782:  ///< Power Word: Barrier (buff)
            case 139485: ///< Dark Winds
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(285); ///< 1s
                spellInfo->AttributesEx5 |= SPELL_ATTR5_HIDE_DURATION;
                break;
            case 134821: ///< Discharged Energy (Lei Shen - Throne of Thunder)
            case 135153: ///< Crashing Thunder (DoT) (Lei Shen - Throne of Thunder)
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(285); ///< 1s
                spellInfo->AttributesEx5 |= SPELL_ATTR5_HIDE_DURATION;
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_DONT_RESET_PERIODIC_TIMER | SPELL_ATTR0_CU_TRIGGERED_IGNORE_RESILENCE;
                break;
            case 137252: ///< Overcharge (Lei Shen - Throne of Thunder)
                spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(35); ///< Min: 0, Max: 35
                spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(21); ///< 35
                break;
            case 103965: ///< Metamorphosis (override auras)
                ///< All this effects are Override with old spell id
                spellInfo->Effects[4].Effect = SPELL_EFFECT_NONE; ///< No more use (Void Ray : 115422)
                spellInfo->Effects[5].Effect = SPELL_EFFECT_NONE; ///< No more use (Aura of Enfeeblement : 116198)
                spellInfo->Effects[6].Effect = SPELL_EFFECT_NONE; ///< No more use (Aura of the Elements : 116202)
                spellInfo->Effects[7].Effect = SPELL_EFFECT_NONE; ///< No more use (Sleep : 104045)
                spellInfo->Effects[8].Effect = SPELL_EFFECT_NONE; ///< No more use (Provocation : 97827)
                spellInfo->Effects[9].Effect = SPELL_EFFECT_NONE;
                spellInfo->Effects[11].Effect = SPELL_EFFECT_NONE;
                spellInfo->Effects[14].Effect = SPELL_EFFECT_NONE; ///< No more use (Drain Life : 103990)
                spellInfo->Effects[15].Effect = SPELL_EFFECT_NONE;
                break;
            case 119890: ///< Death Blossom
                spellInfo->Effects[0].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[0].TargetB = 0;
                spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(139); ///< 40y
                break;
            case 120629: ///< Huddle in Terror
                spellInfo->MaxAffectedTargets = 3;
                break;
            case 119956: ///< Dread Spray (visual)
                spellInfo->Effects[0].TargetA = TARGET_DEST_DEST;
                break;
            case 119958: ///< Dread Spray
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(285); ///< 1s
                spellInfo->AttributesEx &= ~SPELL_ATTR1_CHANNELED_1;
                spellInfo->Effects[0].Effect = 0;
                break;
            case 119414: ///< Breath of Fear
                spellInfo->Effects[2].Effect = 0;
                break;
            case 213084: ///< Flamestrike (visual)
                spellInfo->Effects[0].Effect = SPELL_EFFECT_DUMMY;
                spellInfo->Effects[1].Effect = 0;
                break;
            case 107145: ///< Wall of Light activation
                spellInfo->Effects[0].TargetA = TARGET_SRC_CASTER;
                break;
            case 119312: ///< Conjure Terror Spawn (01)
            case 119370: ///< Conjure Terror Spawn (02)
            case 119371: ///< Conjure Terror Spawn (03)
            case 119372: ///< Conjure Terror Spawn (04)
                spellInfo->Effects[0].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[0].TargetB = 0;
                break;
            case 117865: ///< Wall of Light
                spellInfo->Effects[0].Effect = 0;
                break;
            case 126848: ///< Ritual of Purification
                spellInfo->Effects[0].BasePoints = -10;
                spellInfo->Effects[1].BasePoints = -10;
                break;
            case 125706: ///< Channeling Corruption
                spellInfo->Effects[0].TriggerSpell = 0;
                break;
            case 125713: ///< Channeling Corruption (triggered)
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                break;
            case 125736: ///< Night Terrors (missile)
                spellInfo->Effects[0].Effect = SPELL_EFFECT_DUMMY;
                break;
            case 117230: ///< Overpowered
                spellInfo->Effects[0].Effect = SPELL_EFFECT_APPLY_AURA;
                break;
            case 117988: ///< Defiled Ground
                spellInfo->Effects[0].Effect = 0;
                spellInfo->Effects[1].Mechanic = MECHANIC_NONE;
                break;
            case 117052: ///< Corruption Sha
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[0].TargetB = 0;
                spellInfo->Effects[1].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[1].TargetB = 0;
                spellInfo->Effects[2].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[2].TargetB = 0;
                break;
            case 161209: ///< Vileblood Serum (missile)
                spellInfo->Effects[0].TargetA = TARGET_DEST_TARGET_ANY;
                spellInfo->Effects[0].TargetB = 0;
                break;
            case 161199: ///< Debilitating Fixation (Kyrak)
                spellInfo->Effects[1].Effect = 0;
                spellInfo->Effects[1].ApplyAuraName = 0;
                spellInfo->ChannelInterruptFlags |= 0x1008;
                break;
            case 155498: ///< Rejuvenating Serum
            case 161203: ///< Rejuvenating Serum (Kyrak)
                spellInfo->DmgClass = SPELL_DAMAGE_CLASS_MAGIC;
                break;
            case 152742:///< Fiery Boulder - AoE impact damage
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_SRC_CASTER;
                spellInfo->Effects[EFFECT_1].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                break;
            case 152867:///< Heat Wave (Create AT)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                break;
            case 150759:///< Ferocious Yell
                spellInfo->InterruptFlags |= SPELL_INTERRUPT_FLAG_INTERRUPT;
                break;
            case 164616:///< Channel Flames (DoT)
            case 167739:///< Scorching Aura (Debuff)
            case 153227:///< Burning Slag (DoT)
            case 150784:///< Magma Eruption (DoT)
            case 161288:///< Vileblood Serum (DoT)
            case 161833:///< Noxious Spit (DoT)
            case 157420:///< Fiery Trail (DoT)
            case 155057:///< Magma Pool (DoT)
            case 166730:///< Burning Bridge (DoT)
            case 176037:///< Noxious Spit (DoT)
            case 161635:///< Molten Bomb
            case 159311:///< Flame Jet
            case 161517:///< Splitting Breath (DoT)
            case 176146:///< Volcanic Fallout
            case 159413:///< Mauling Brew
            case 175654:///< Rune of Disintegration
            case 163046:///< Pale Vitriol
            case 163140:///< Mind Fungus
            case 163590:///< Creeping Moss (damage)
            case 165494:///< Creeping Moss (healing)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_DONT_RESET_PERIODIC_TIMER;
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                spellInfo->AttributesEx5 |= SPELL_ATTR5_HIDE_DURATION;
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(39); // 2s
                break;
            case 162370:///< Crystalline Barrage (DoT)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_DONT_RESET_PERIODIC_TIMER;
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                spellInfo->AttributesEx5 |= SPELL_ATTR5_HIDE_DURATION;
                break;
            case 163312: ///< Raving Assault
                spellInfo->Effects[EFFECT_1].Effect = SPELL_EFFECT_DUMMY;
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->ExplicitTargetMask = TARGET_FLAG_UNIT;
                break;
            case 154996: ///< Engulfing Fire (searcher)
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[0].TargetB = 0;
                break;
            case 155721: ///< Black Iron Cyclone
                spellInfo->Effects[0].Effect = 0;
                spellInfo->Effects[0].ApplyAuraName = 0;
                spellInfo->Effects[1].TargetA = TARGET_UNIT_CASTER;
                spellInfo->InterruptFlags &= ~SPELL_INTERRUPT_FLAG_MOVEMENT;
                break;
            case 127731: ///< Corruption Sha (triggered)
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[0].TargetB = 0;
                break;
            case 123244: ///< Hide
                spellInfo->Effects[0].Effect = 0;
                spellInfo->Effects[2].Effect = 0;
                break;
            case 124009: ///< Tiger Lust
            case 130793: ///< Provoke
            case 123996: ///< Crackling Tiger Lightning
                spellInfo->StartRecoveryTime = 1500;
                spellInfo->StartRecoveryCategory = 133;
                //spellInfo->Effects[3].Effect = 0;
                break;
            case 127424: ///< Jade Fire
                spellInfo->Effects[0].TargetA = TARGET_UNIT_CONE_ENEMY_54;
                spellInfo->Effects[0].TargetB = 0;
                break;
            case 45257: ///< Using Steam Tonk Controller
            case 45440: ///< Steam Tonk Controller
            case 49352: ///< Crashin' Thrashin' Racer Controller
            case 75111: ///< Blue Crashin' Thrashin' Racer Controller
            case 60256: ///< Collect Sample
                /// Crashes client on pressing ESC (Maybe because of ReqSpellFocus and GameObject)
                spellInfo->AttributesEx4 &= ~SPELL_ATTR4_CAN_CAST_WHILE_CASTING;
                break;
            case 12540:
            case 13579:
            case 24698:
            case 28456:
            case 29425:
            case 34940:
            case 36862:
            case 38863:
            case 52743: ///< Head Smack
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_REQ_TARGET_FACING_CASTER;
                break;
            case 21987: ///< Lash of Pain
            case 58563: ///< Assassinate Restless Lookout
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_REQ_CASTER_BEHIND_TARGET;
                break;
            case 120751: ///< Pandashan's Dragon Gun
            case 120876:
            case 120964:
            case 124347:
                spellInfo->Effects[0].TargetA = TARGET_UNIT_CONE_ENEMY_54;
                break;
            case 26029: ///< Dark Glare
            case 37433: ///< Spout
            case 43140: ///< Flame Breath
            case 43215: ///< Flame Breath
            case 70461: ///< Coldflame Trap
            case 72133: ///< Pain and Suffering
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_CONE_LINE;
                break;
            case 24340:  ///< Meteor
            case 26558:  ///< Meteor
            case 28884:  ///< Meteor
            case 36837:  ///< Meteor
            case 38903:  ///< Meteor
            case 41276:  ///< Meteor
            case 57467:  ///< Meteor
            case 26789:  ///< Shard of the Fallen Star
            case 31436:  ///< Malevolent Cleave
            case 35181:  ///< Dive Bomb
            case 40810:  ///< Saber Lash
            case 43267:  ///< Saber Lash
            case 43268:  ///< Saber Lash
            case 42384:  ///< Brutal Swipe
            case 45150:  ///< Meteor Slash
            case 64688:  ///< Sonic Screech
            case 72373:  ///< Shared Suffering
            case 71904:  ///< Chaos Bane
            case 70492:  ///< Ooze Eruption
            case 88942:  ///< Meteor Slash
            case 82935:  ///< Caustic Slime
            case 86825:  ///< Blackout
            case 77679:  ///< Scorching Blast
            case 98474:  ///< Flame Scythe
            case 105069: ///< Seething Hate
            case 103414: ///< Stomp
            case 103527: ///< Void Diffusion dmg
            case 106375: ///< Unstable Twilight
            case 107439: ///< Twilight Barrage
            case 106401: ///< Twilight Onslaught
            case 155152: ///< Prismatic Crystal damage
            case 172073: ///< Meteoric Earthspire (Rokka & Lokk)
            case 135703: ///< Static Shock (Lei Shen - Throne of Thunder)
            case 136366: ///< Bounding Bolt (Lei Shen - Throne of Thunder)
                /// ONLY SPELLS WITH SPELLFAMILY_GENERIC and EFFECT_SCHOOL_DAMAGE
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_SHARE_DAMAGE;
                break;
            case 114093: ///< Windlash Off-Hand
            case 114089: ///< Windlash
            case 18500:  ///< Wing Buffet
            case 33086:  ///< Wild Bite
            case 49749:  ///< Piercing Blow
            case 52890:  ///< Penetrating Strike
            case 53454:  ///< Impale
            case 59446:  ///< Impale
            case 64777:  ///< Machine Gun
            case 65239:  ///< Machine Gun
            case 65919:  ///< Impale
            case 74439:  ///< Machine Gun
            case 63278:  ///< Mark of the Faceless (General Vezax)
            case 62544:  ///< Thrust (Argent Tournament)
            case 64588:  ///< Thrust (Argent Tournament)
            case 66479:  ///< Thrust (Argent Tournament)
            case 68505:  ///< Thrust (Argent Tournament)
            case 62709:  ///< Counterattack! (Argent Tournament)
            case 62626:  ///< Break-Shield (Argent Tournament, Player)
            case 64590:  ///< Break-Shield (Argent Tournament, Player)
            case 64342:  ///< Break-Shield (Argent Tournament, NPC)
            case 64686:  ///< Break-Shield (Argent Tournament, NPC)
            case 65147:  ///< Break-Shield (Argent Tournament, NPC)
            case 68504:  ///< Break-Shield (Argent Tournament, NPC)
            case 62874:  ///< Charge (Argent Tournament, Player)
            case 68498:  ///< Charge (Argent Tournament, Player)
            case 64591:  ///< Charge (Argent Tournament, Player)
            case 63003:  ///< Charge (Argent Tournament, NPC)
            case 63010:  ///< Charge (Argent Tournament, NPC)
            case 68321:  ///< Charge (Argent Tournament, NPC)
            case 72255:  ///< Mark of the Fallen Champion (Deathbringer Saurfang)
            case 62775:  ///< Tympanic Tantrum (XT-002 encounter)
            case 102598: ///< Void Strike, Infinite Warden
            case 154448: ///< Shrapnel Nova (Orebender Gor'Ashan)
            case 163047: ///< Paleobomb (Night-Twisted Cadaver - The Butcher)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_IGNORE_ARMOR;
                break;
            case 64422: ///< Sonic Screech (Auriaya)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_SHARE_DAMAGE;
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_IGNORE_ARMOR;
                break;
            case 156791: ///< Call Adds
                spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(6);    ///< 100yards
                break;
            case 176544: ///< Fixate (Skyreach)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[EFFECT_1].TargetB = TARGET_UNIT_TARGET_ANY;
                break;
            case 152973: ///< Protective Barrier (Skyreach)
                spellInfo->Effects[1].TargetA = TARGET_UNIT_CASTER;
                break;
            case 156634: ///< Four winds (Skyreach)
            case 156636: ///< Four winds (Skyreach)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_TARGET_ANY;
                break;
            case 166623: ///< Four winds (Skyreach)
            case 166664: ///< Four winds (Skyreach)
            {
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_TARGET_ANY;
                SpellDurationEntry const* durationIndex = sSpellDurationStore.LookupEntry(8);
                if (!durationIndex)
                    break;
                spellInfo->DurationEntry = durationIndex;
                break;
            }
            case 81744: ///< Horde    (rated battleground faction override)
            case 81748: ///< Alliance (rated battleground faction override)
                spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_APPLY_AURA;
                spellInfo->Effects[EFFECT_1].Effect = SPELL_EFFECT_APPLY_AURA;
                spellInfo->Effects[EFFECT_2].Effect = SPELL_EFFECT_APPLY_AURA;
                break;
            case 159226: ///< Solar storm (Skyreach)
            case 153759: ///< WindWalls (Skyreach)
            case 153139: ///< Four winds (Skyreach)
            case 158441: ///< Solar Zone (Skyreach)
            case 153907: ///< Dervish (Skyreach)
            case 156841: ///< Storm (Skyreach)
            case 72293:  ///< Mark of the Fallen Champion (Deathbringer Saurfang)
            case 159178: ///< Open Wounds (Kargath Bladefist)
            case 156152: ///< Gushing Wounds (The Butcher)
            case 158553: ///< Crush Armor (Imperator Mar'gok)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE_EFF0;
                break;
            case 151272: ///< Wheel of Pain Knockback
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            case 101184: ///< Leyara's Locket
                spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_DUMMY;
                spellInfo->Effects[EFFECT_1].Effect = 0;
                spellInfo->Effects[EFFECT_2].Effect = 0;
                break;
            case 116711: ///< Draw Flame
                spellInfo->ChannelInterruptFlags = 0x0;
                spellInfo->AuraInterruptFlags = 0x0;
                break;
            case 118443: ///< Instantly Death
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[0].TargetB = 0;
                break;
            case 116989: ///< Overloaded Missile
            case 117220: ///< Overloaded Triggered
            case 118430: ///< Core Beam
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                break;
            case 129724: ///< Grasping Energy Tendrils
                spellInfo->Effects[0].Effect = SPELL_EFFECT_DUMMY;
                break;
            case 117960: ///< Celestial Breathe
                spellInfo->AttributesEx5 &= ~SPELL_ATTR5_START_PERIODIC_AT_APPLY;
                break;
            case 118018: ///< Draw Power (Summons)
                spellInfo->Effects[0].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[1].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[2].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[3].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[4].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[5].TargetA = TARGET_UNIT_CASTER;
                break;
            case 116661: ///< Draw Power (lightning damage for activated focus)
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                break;
            case 117558: ///< Coalescing Shadows
                spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(13); ///< 10 yards
                break;
            case 117829: ///< Cowardice (DoT)
            case 117006: ///< Devastating Arc
            case 119775: ///< Reaching Attack
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 117708: ///< Maddening Shout
                spellInfo->AttributesEx5 |= SPELL_ATTR5_USABLE_WHILE_FEARED;
                break;
            case 204054: ///< Consecrated Ground
                spellInfo->ProcChance = 100;
                break;
            case 17076: ///< Glyph of Rejuvenation
                spellInfo->Effects[0].BasePoints = 0;
                break;
            case 118047: ///< Pillage
                spellInfo->Effects[0].Effect = SPELL_EFFECT_CHARGE;
                spellInfo->Effects[0].TriggerSpell = 0;
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                break;
            case 53595: ///< Hammer of the Righteous
                spellInfo->OverrideSpellList.push_back(204019); ///< Blessed Hammer
                break;
            case 114108: ///< Soul of the Forst (Resto)
                spellInfo->ProcFlags = 0;
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                break;
            case 688: ///< Summon Imp
                spellInfo->OverrideSpellList.push_back(112866); ///< Summon Fel Imp
                break;
            case 697: ///< Summon Voidwalker
                spellInfo->OverrideSpellList.push_back(112867); ///< Summon Voidlord
                break;
            case 712: ///< Summon Succubus
                spellInfo->OverrideSpellList.push_back(112868); ///< Summon Shivarra
                spellInfo->OverrideSpellList.push_back(240263); ///< Fel Succubus
                spellInfo->OverrideSpellList.push_back(240266); ///< Shadow Succubus
                break;
            case 691: ///< Summon Felhunter
                spellInfo->OverrideSpellList.push_back(112869); ///< Summon Observer
                break;
            case 30146: ///< Summon Felguard
                spellInfo->OverrideSpellList.push_back(112870); ///< Summon Wrathguard
                break;
            case 1122: ///< Summon Infernal
                spellInfo->OverrideSpellList.push_back(112921); ///< Summon Abyssal
                break;
            case 112921: ///< Summon Abyssal
                spellInfo->OverrideSpellList.push_back(157899); ///< Summon Abyssal
                break;
            case 18540: ///< Summon Doomguard
                spellInfo->OverrideSpellList.push_back(112927); ///< Summon Terrorguard
                break;
            case 112927: ///< Summon Terrorguard
                spellInfo->OverrideSpellList.push_back(157897); ///< Summon Terrorguard (permanent)
                break;
            case 105174: ///< Hand of Gul'dan
                spellInfo->OverrideSpellList.push_back(123194);
                break;
            case 120517: ///< Halo (Holy)
                spellInfo->AttributesCu &= ~SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 121129: ///< Daybreak (heal)
                spellInfo->Effects[1].TargetA = TARGET_SRC_CASTER;
                break;
            case 114925: ///< Demonic Calling
                spellInfo->ProcFlags = 0;
                break;
            case 51699:  ///< Honor Among Thieves (triggered)
            case 57934:  ///< Tricks of the Trade
            case 138275: ///< Cosmetic Visual (Lei Shen - Throne of Thunder)
            case 138274: ///< Cosmetic Visual (triggered - Lei Shen - Throne of Thunder)
            case 2094: ///< Blind
            case 6770: ///< Sap
            case 217832: ///< Imprison
                spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_INITIAL_AGGRO;
                break;
            case 154294: ///< Power Conduit
                spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_INITIAL_AGGRO | SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                spellInfo->AttributesEx5 |= SPELL_ATTR5_USABLE_WHILE_STUNNED;
                break;
            case 170493: ///< Honorbound
                spellInfo->Attributes |= SPELL_ATTR0_CANT_CANCEL;
                spellInfo->AttributesEx3 |= SPELL_ATTR3_DEATH_PERSISTENT;
                spellInfo->AttributesEx4 |= SPELL_ATTR4_NOT_STEALABLE;
                break;
            case 57723:  ///< Exhaustion
            case 57724:  ///< Sated
            case 80354:  ///< Temporal Displacement
            case 95809:  ///< Insanity
            case 160455: ///< Fatigued
                spellInfo->AttributesEx3 |= SPELL_ATTR3_DEATH_PERSISTENT;
                break;
            case 186842: ///< Podling Disguise
                spellInfo->Effects[0].MiscValue = 85411;
                break;
            case 105770: ///< Item - Druid T13 Restoration 4P Bonus (Rejuvenation)
                spellInfo->Effects[0].SpellClassMask = flag128(0x00000050, 0, 0);
                break;
            case 99009: ///< Item - Druid T12 Feral 4P Bonus
                spellInfo->ProcChance = 100;
                spellInfo->ProcFlags = 16;
                break;
            case 99209: ///< Item - Shaman T12 Enhancement 2P Bonus
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                break;
            case 165440:///< Item - Paladin T17 Retribution 2P Bonus
            case 165439:///< Item - Paladin T17 Retribution 4P Bonus
            case 165446:///< Item - Paladin T17 Protection 2P Bonus
            case 167694:///< Item - Priest T17 Discipline 4P Bonus
            case 167684:///< Item - Priest T17 Holy 4P Bonus
            case 165629:///< Item - Priest T17 Shadow 4P Bonus
            case 167702:///< Item - Shaman T17 Restoration 4P Bonus
            case 165610:///< Item - Shaman T17 Enhancement 4P Bonus
            case 165525:///< Item - Hunter T17 Marksmanship 4P Bonus
            case 165544:///< Item - Hunter T17 Survival 2P Bonus
            case 165450:///< Item - Warlock T17 Demonology 2P Bonus
            case 165337:///< Item - Warrior T17 Fury 2P Bonus
            case 165349:///< Item - Warrior T17 Fury 4P Bonus
            case 165469:///< Item - Mage T17 Frost 4P Bonus
            case 165432:///< Item - Druid T17 Feral 4P Bonus
            case 165410:///< Item - Druid T17 Guardian 2P Bonus
            case 165478:///< Item - Rogue T17 Combat 4P Bonus
            case 165547:///< Item - Death Knight T17 Frost 2P Bonus
            case 165568:///< Item - Death Knight T17 Frost 4P Bonus
            case 165345:///< Item - Warrior T17 Arms 4P Bonus
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                break;
            case 167697:///< Item - Paladin T17 Holy 4P Bonus
                spellInfo->Effects[EFFECT_0].BasePoints = 20;
                break;
            case 181608:///< Inner Demon (for Warlock T17 Demonology 2P Bonus)
                spellInfo->Effects[EFFECT_1].Effect = 0;
                spellInfo->Effects[EFFECT_1].ApplyAuraName = 0;
                break;
            case 166881: ///< Shadow Strikes (for Rogue T17 Subtlety 4P Bonus)
                spellInfo->ProcFlags = 0;
                break;
            case 165437:///< Item - Druid T17 Restoration 2P Bonus
                spellInfo->Effects[EFFECT_0].BasePoints = 2;
                break;
            case 170205:///< Item - Death Knight T17 Frost 4P Driver
                spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                spellInfo->Effects[EFFECT_1].TriggerSpell = 0;
                break;
            case 530:   ///< Charm (Possess)
                spellInfo->Effects[EFFECT_0].BasePoints = 102;
                break;
            case 36032: ///< Arcane Charge
                spellInfo->AttributesEx3 &= ~SPELL_ATTR3_CANT_TRIGGER_PROC;
                break;
            case 99213: ///< Item - Shaman T12 Enhancement 4P Bonus
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                spellInfo->ProcChance = 100;
                spellInfo->ProcFlags = 16;
                break;
            case 99206: ///< Item - Shaman T12 Elemental 4P Bonus
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                break;
            case 99175: ///< Item - Rogue T12 4P Bonus
                spellInfo->ProcChance = 0;
                spellInfo->ProcFlags = 0;
                break;
            case 99234: ///< Item - Warrior T12 DPS 2P Bonus
                spellInfo->ProcChance = 0;
                spellInfo->ProcFlags = 0;
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                break;
            case 77489: ///< Echo of Light
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_DONT_RESET_PERIODIC_TIMER;
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CANT_CRIT;
                break;
            case 99002: ///< Fiery Claws, Item - Druid T12 Feral 2P Bonus
            case 99132: ///< Divine Fire, Item - Priest T12 Healer 2P Bonus
            case 99173: ///< Burning Wounds, Item - Rogue T12 2P Bonus
            case 55078: ///< Blood Plague
            case 55095: ///< Frost Fever
            case 186576:///< Petrifying Cloud (Blightshard Shaper - Neltharion's Lair)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_DONT_RESET_PERIODIC_TIMER;
                break;
            case 91107: ///< Unholy Might
                spellInfo->OverrideSpellList.push_back(109260); ///< Add Aspect of the Iron Hack to override spell list of Aspect of the Hawk
                break;
            case 53490: ///< Bullheaded
                spellInfo->Effects[1].TargetA = TARGET_UNIT_CASTER;
                break;
            case 143024:
            case 143022:
                spellInfo->Effects[0].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[0].TargetB = 0;
                break;
            case 109260: ///< Aspect of the Iron Hawk
                spellInfo->Effects[0].BasePoints = -10;
                break;
            case 4074: ///< Explosive Sheep
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(25); ///< 3min
                break;
            case 185776: ///< Mining trap
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(25);
                break;
            case 185498: ///< Mining trap
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(25);
                break;
            case 703: ///< Garrote
                spellInfo->ProcChance = 100;
                break;
            case 24275: ///< Hammer of Wrath
                spellInfo->Attributes |= SPELL_ATTR0_IMPOSSIBLE_DODGE_PARRY_BLOCK;
                spellInfo->AttributesEx |= SPELL_ATTR1_CANT_BE_REFLECTED;
                spellInfo->Effects[0].Effect = SPELL_EFFECT_SCHOOL_DAMAGE;
                break;
            case 53592: ///< Guarded by the Light
                spellInfo->Attributes |= SPELL_ATTR0_ABILITY;
                spellInfo->Attributes |= SPELL_ATTR0_PASSIVE;
                spellInfo->Attributes |= SPELL_ATTR0_HIDE_IN_COMBAT_LOG;
                spellInfo->AttributesEx3 |= SPELL_ATTR3_DEATH_PERSISTENT;
                break;
            case 29144: ///< Unwavering Sentinel
                spellInfo->Attributes |= SPELL_ATTR0_ABILITY;
                spellInfo->Attributes |= SPELL_ATTR0_PASSIVE;
                spellInfo->AttributesEx3 |= SPELL_ATTR3_DEATH_PERSISTENT;
                break;
            case 5246: ///< Intimidating Shout
                spellInfo->MaxAffectedTargets |= 5;
                break;
            case 116481: ///< Tiger's lust ///< 7.0.3 name Pounce check it
                spellInfo->Attributes |= SPELL_ATTR0_ABILITY;
                spellInfo->AttributesEx5 &= ~SPELL_ATTR5_HASTE_AFFECT_DURATION;
                break;
            case 86704: ///< Ancient Fury
                spellInfo->Effects[0].Effect = SPELL_EFFECT_SCHOOL_DAMAGE;
                break;
            case 155627: ///< Lunar Inspiration
                spellInfo->Attributes |= SPELL_ATTR0_PASSIVE;
                spellInfo->AttributesEx3 |= SPELL_ATTR3_DEATH_PERSISTENT;
                break;
            case 2818: ///< Deadly Poison
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_DONT_RESET_PERIODIC_TIMER;
                spellInfo->Attributes |= SPELL_ATTR0_IMPOSSIBLE_DODGE_PARRY_BLOCK;
                spellInfo->AttributesEx |= SPELL_ATTR1_CANT_BE_REDIRECTED;
                spellInfo->AttributesEx |= SPELL_ATTR1_CANT_BE_REFLECTED;
                break;
            case 185311: ///< Crimson Vial
            case 118922: ///< Posthaste
            case 108212: ///< Burst of Speed
                spellInfo->AttributesEx |= SPELL_ATTR1_NOT_BREAK_STEALTH;
                break;
            case 156297: ///< Acid Torrent
                spellInfo->Attributes |= SPELL_ATTR0_IMPOSSIBLE_DODGE_PARRY_BLOCK;
                break;
            case 8680: ///< Wound Poison
            case 3409: ///< Crippling Poison
                spellInfo->DmgClass = SpellDmgClass::SPELL_DAMAGE_CLASS_MELEE; /// To use MeleeSpellHitResult instead of MagicSpellHitResult
                spellInfo->Attributes |= SPELL_ATTR0_IMPOSSIBLE_DODGE_PARRY_BLOCK;
                spellInfo->AttributesEx |= SPELL_ATTR1_CANT_BE_REDIRECTED;
                spellInfo->AttributesEx |= SPELL_ATTR1_CANT_BE_REFLECTED;
                break;
            case 115098: ///< Chi Wave - Add Aura for bounce counting
                spellInfo->Effects._effects.push_back(SpellEffectInfo(sSpellStore.LookupEntry(spellInfo->Id), spellInfo, EFFECT_1, Difficulty::DifficultyNone));
                spellInfo->Effects[1].Effect = SPELL_EFFECT_APPLY_AURA;
                spellInfo->Effects[1].ApplyAuraName = SPELL_AURA_DUMMY;
                spellInfo->Effects[1].BasePoints = 1;
                spellInfo->Effects[1].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[1].TargetB = 0;
                spellInfo->EffectCount++;
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(1); ///< 10s
                spellInfo->Speed = 100.0f;
                break;
            case 120755: ///< Glaive Toss (Glaive right)
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                break;
            case 66: ///< Invisibility
                spellInfo->OverrideSpellList.push_back(110959); ///< Greater Invisibility
                spellInfo->Dispel = DISPEL_INVISIBILITY;
                break;
            case 110959: ///< Greater Invisibility
                spellInfo->Dispel = DISPEL_INVISIBILITY;
                break;
            case 47476: ///< Strangulate
                spellInfo->OverrideSpellList.push_back(108194); ///< Asphyxiate
                break;
            case 34428: ///< Victory Rush
                spellInfo->OverrideSpellList.push_back(202168); ///< Impending Victory
                break;
            case 118779: ///< Victory Rush
                spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_DONE_BONUS;
                break;
            case 1856:  ///< Vanish
                spellInfo->Effects[1].TriggerSpell = 131368;
                spellInfo->Effects[0].Effect = SPELL_EFFECT_SANCTUARY;
                break;
            case 131368:  ///< Vanish (triggered)
                spellInfo->Effects[2].TriggerSpell = 131361;
                break;
            case 131361:///< Vanish - Improved Stealth
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(27); ///< 3s
                break;
            case 116784:///< Wildfire Spark - Boss Feng
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                spellInfo->Effects[0].TargetB = 0;
                spellInfo->Effects[1].TargetA = TARGET_UNIT_TARGET_ENEMY;
                spellInfo->Effects[1].TargetB = 0;
                break;
            case 120954: ///< Fortifying Brew
                spellInfo->Effects[4].ApplyAuraName = SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT;
                break;
            case 114871: ///< Holy Prism (Allies)
            case 114852: ///< Holy Prism (ennemies)
                spellInfo->Effects[2].Effect = 0;
                break;
            case 123154: ///< Fists of Fury Visual Target
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(36); ///< 1s
                break;
            case 126311: ///< Surface Trot
                spellInfo->Effects[0].Effect = SPELL_EFFECT_APPLY_AREA_AURA_PET;
                spellInfo->Effects[0].TargetA = TARGET_UNIT_CASTER;
                break;
            case 117837: ///< Delirious
                spellInfo->Dispel = DISPEL_MAGIC;
                break;
            case 125972: ///< Felin Grace
                spellInfo->Effects[0].ApplyAuraName = SPELL_AURA_SAFE_FALL;
                break;
            case 116833: ///< Cosmetic Spirit Totem (MSV - #3 Gara'Jal) ///< named chenneling in DB2
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                break;
            case 130774: ///< Amethyst Pool - Periodic damage (MSV - #1 Stone Guard)
                spellInfo->Attributes |= SPELL_ATTR0_CANT_CANCEL;
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CANT_CRIT;
                break;
            case 116782: ///< Titan Gas (MSV - #6 Will of Emperor)
                spellInfo->Effects[0].Effect = SPELL_EFFECT_SCHOOL_DAMAGE;
                spellInfo->Effects[0].BasePoints = 17000;
                spellInfo->Effects[0].TargetA = TARGET_SRC_CASTER;
                spellInfo->Effects[0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(113); ///< radius 1000
                break;
            case 122336: ///< Sonic Ring (HoF - #1 Zor'lok)
                spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(16); ///< radius 1.0 instead of 6.0
            case 124673: ///< Sonic Pulse (HoF - #1 Zor'lok)
                spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(16); ///< radius 1.0 instead of 6.0
            case 123811: ///< Pheromones of Zeal (HoF - #1 Zor'lok)
                spellInfo->Effects[0].TargetA = TARGET_UNIT_DEST_AREA_ENTRY;
                break;
            case 178153: ///< Death from Above
                spellInfo->Effects[1].TargetA = TARGET_DEST_TARGET_FRONT;
                break;
            case 97817: ///< Leap of Faith
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_TARGET_FRONT;
                spellInfo->ExplicitTargetMask = TARGET_FLAG_DEST_LOCATION;
                spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(7);
                spellInfo->Effects[EFFECT_0].MiscValue = 70;
                spellInfo->Effects[EFFECT_0].ValueMultiplier = 0;
                break;
            case 248101: ///< The Wind Blows (7.3.2 Monk Legendary)
                spellInfo->Effects[EFFECT_1].ApplyAuraName = SPELL_AURA_DUMMY;
                spellInfo->Effects[EFFECT_1].TriggerSpell = 0;
                break;
            case 241334: ///< Cloth Legendary - Celumbra The Night's Dichotomy
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                break;
            case 242553:
                spellInfo->Speed = 0.0f;
                break;
            case 122706: ///< Noise Cancelling (HoF - #1 Zor'lok)
                spellInfo->Effects[0].Effect = SPELL_EFFECT_APPLY_AURA;
                spellInfo->Effects[0].BasePoints = 60;
                spellInfo->Effects[0].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[1].Effect = SPELL_EFFECT_APPLY_AURA;
                spellInfo->Effects[1].BasePoints = 75;
                spellInfo->Effects[1].TargetA = TARGET_UNIT_CASTER;
                break;
            case 122707: ///< Noise Cancelling (HoF - #1 Zor'lok)
                spellInfo->Effects[0].BasePoints = 10;
                spellInfo->Effects[0].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[0].TargetB = 0;
                break;
            case 122731: ///< Create Noise Cancelling AreaTrigger (HoF - #1 Zor'lok)
                spellInfo->Effects[0].BasePoints = 351;
                spellInfo->Effects[0].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[0].TargetB = 0;
                break;
            case 122842: ///< Tempest Slash (summoning tornado) (HoF - #2 Ta'yak)
                spellInfo->Effects[0].TargetB = TARGET_DEST_DEST_RIGHT;
                break;
            case 125312: ///< Blade Tempest (HoF - #2 Ta'yak)
                spellInfo->AttributesEx3 = SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                spellInfo->Effects[0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                break;
            case 45819: // Midsummer - Throw Torch
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_DEST_AREA_ENTRY;
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(15);
                spellInfo->MaxAffectedTargets = 1;
                break;
            case 125327: ///< Blade Tempest (jump on Ta'yak) (HoF - #2 Ta'yak)
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                break;
            case 123633: ///< Gale Winds (HoF - #2 Ta'yak)
                spellInfo->Effects[0].Effect = SPELL_EFFECT_APPLY_AURA;
                spellInfo->Effects[0].BasePoints = 0;
                spellInfo->Effects[0].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[0].TargetB = 0;
                spellInfo->Effects[0].Amplitude = 500;
                break;
            case 122786: ///< Broken leg (HoF - #3 Garalon)
                spellInfo->Effects[0].MiscValue = -15;
                spellInfo->Effects[1].BasePoints = 0; ///< Cancel damages, done by Garalon' script
                spellInfo->Effects[2].Effect = 3;
                break;
            case 121896: ///< Whirling Blade (HoF - #4 Mel'jarak)
                spellInfo->Effects[2].Effect = 0;
                spellInfo->Effects[3].Effect = 0;
                break;
            case 122370: ///< Reshape Life (HoF - #5 Un'sok)
                spellInfo->Effects[0].Effect = SPELL_EFFECT_APPLY_AURA;
                spellInfo->Effects[0].TargetA = TARGET_UNIT_CHANNEL_TARGET;
                spellInfo->Effects[0].ApplyAuraName = SPELL_AURA_MOD_FACTION;
                spellInfo->Effects[0].MiscValue = 2577;
                spellInfo->Effects[0].MiscValueB = 1;
                spellInfo->Effects[1].Effect = SPELL_EFFECT_APPLY_AURA;
                spellInfo->Effects[1].TargetA = TARGET_UNIT_CHANNEL_TARGET;
                spellInfo->Effects[1].ApplyAuraName = SPELL_AURA_TRANSFORM;
                spellInfo->Effects[1].MiscValue = 62701;
                spellInfo->Effects[5].Effect = SPELL_EFFECT_APPLY_AURA;
                spellInfo->Effects[5].TargetA = TARGET_UNIT_CHANNEL_TARGET;
                spellInfo->Effects[5].ApplyAuraName = SPELL_AURA_PROC_MELEE_TRIGGER_SPELL;
                spellInfo->Effects[5].MiscValue = 58044;
                break;
            case 122457: ///< Rough Landing (HoF - #5 Un'sok)
                spellInfo->Effects[0].TargetB = TARGET_UNIT_SRC_AREA_ALLY;
                spellInfo->Effects[1].TargetB = TARGET_UNIT_SRC_AREA_ALLY;
                break;
            case 123713: ///< Servant of the Empress (HoF - #6 - Shek'zeer)
                spellInfo->Effects[0].Effect = SPELL_EFFECT_DUMMY;
                break;
            case 123255: ///< Dissonance Field (HoF - #6 - Shek'zeer)
                spellInfo->Effects[1].Effect = SPELL_EFFECT_DUMMY; ///< Cancel triggering 123596 (knock back)
                break;
            case 126121: ///< Corrupted Dissonance Field (HoF - #6 - Shek'zeer)
                spellInfo->Effects[3].Effect = SPELL_EFFECT_DUMMY; ///< Same as previous spell (123255);
            case 124845: ///< Calamity (HoF - #6 - Shek'zeer)
                spellInfo->Effects[0].BasePoints = 50;
                spellInfo->Effects[1].BasePoints = 50;
                break;
            case 126125: ///< Corrupt Dissonance Field (HoF - #6 - Shek'zeer)
                spellInfo->Effects[0].TargetB = TARGET_UNIT_TARGET_ALLY;
                break;
            case 124506: ///< Gift of the Ox
            case 124503: ///< Gift of the OX
            case 214420: ///< Gift of the Ox
            case 214418: ///< Gift of the Ox
                spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(29); ///< radius 6
                break;
            case 203560: ///< Talon Strike
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                spellInfo->Effects[1].TargetA = TARGET_UNIT_TARGET_ENEMY;
                break;
            case 125451: ///< Ultimate Corruption (HoF - #6 - Shek'zeer)
            case 139866: ///< Torrent of Ice (Megaera - Throne of Thunder) (ToT - #5 - Megaera)
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                break;
            case 108503: ///< Grimoire of Sacrifice
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                break;
            case 136192: ///< Lightning Storm (Iron Qon - Throne of Thunder) (ToT - #10 Iron Qon)
            {
                for (uint8 l_Idx = 0; l_Idx < 5; ++l_Idx)
                    spellInfo->Effects[l_Idx].TargetB = TARGET_UNIT_TARGET_ENEMY;
                break;
            }
            case 136330: ///< Burning Cinders (ToT - #10 Iron Qon)
            case 136419: ///< Storm Cloud (ToT - #10 Iron Qon)
            case 136449: ///< Frozen Blood (ToT - #10 Iron Qon)
            case 137118: ///< Bloom Moon Lotus (ToT - #11 Twin Consorts)
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_TARGET_ANY;
                break;
            case 137749: ///< Cosmic Barrage (ToT - #11 Twin Consorts)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_SRC_CASTER;
                break;
            case 138318: ///< Crane Rush (ToT - #11 Twin Consorts)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_SRC_CASTER;
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_SRC_CASTER;
                spellInfo->Effects[EFFECT_1].TargetB = TARGET_UNIT_TARGET_ANY;
                break;
            case 143412: ///< Immerseus Swirl (Immerseus - Siege of Orgrimmar)
                spellInfo->Effects[EFFECT_0].Amplitude = 1000;
                break;
            case 143413:
                spellInfo->Effects[EFFECT_0].Amplitude = 3000;
                break;
            case 119905: ///< Cauterize (Command Demon)
            case 119907: ///< Suffering (Command Demon)
            case 119909: ///< Whiplash (Command Demon)
            case 119910: ///< Spell Lock (Command Demon)
            case 119911: ///< Optical Blast (Command Demon)
            case 119913: ///< Fellash (Command Demon)
            case 119914: ///< Felstorm (Command Demon)
            case 119915: ///< Wrathstorm (Command Demon)
            case 171140: ///< Shadow Lock (Command Demon)
            case 171152: ///< Meteor Strike (Infernal) (Command Demon)
            case 171154: ///< Meteor Strike (Abyssal) (Command Demon)
                spellInfo->AttributesEx6 &= ~SPELL_ATTR6_DONT_DISPLAY_COOLDOWN;
                break;
            case 119904: ///< Supplant Command Demon
                spellInfo->Effects[0].Effect = SPELL_EFFECT_APPLY_AURA;
                spellInfo->Effects[1].Effect = 0;
                break;
            case 108199: ///< Gorefiend's Grasp
                spellInfo->Mechanic = Mechanics::MECHANIC_GRIP;
                spellInfo->Effects[1].Effect = 0;
                spellInfo->Effects[2].Effect = 0;
                break;
            case 49206: ///< Summon Gargoyle
                spellInfo->AttributesEx5 |= SPELL_ATTR5_SINGLE_TARGET_SPELL;
                break;
            case 82691: ///< Ring of Frost
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                spellInfo->Effects[0].TargetB = 0;
                spellInfo->AttributesEx |= SPELL_ATTR1_CANT_BE_REFLECTED;
                spellInfo->AttributesEx5 &= ~SPELL_ATTR5_SINGLE_TARGET_SPELL;
                spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(13);
                break;
            case 91021: ///< Find Weakness
                spellInfo->Effects[0].BasePoints = 100;
                spellInfo->AttributesEx |= SPELL_ATTR1_NOT_BREAK_STEALTH;
                break;
            case 118699: ///< Fear Effect
                spellInfo->Dispel = DISPEL_MAGIC;
                spellInfo->AttributesEx3 &= ~SPELL_ATTR3_IGNORE_HIT_RESULT;
                spellInfo->Effects[0].ApplyAuraName = SPELL_AURA_MOD_FEAR;
                break;
            case 6203: ///< Soulstone
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_DEAD;
                break;
            case 118291: ///< Greater Fire Elemental
            case 118323: ///< Greater Earth Elemental
            case 157319: ///< Greater Storm Elemental
                spellInfo->Effects[0].Effect = SPELL_EFFECT_SUMMON_PET;
                break;
            case 116943: ///< Earthgrab
                spellInfo->AttributesEx5 |= SPELL_ATTR5_START_PERIODIC_AT_APPLY;
                spellInfo->Effects[0].TargetB = 0;
                break;
            case 6474:   ///< Earthbind Totem
            case 177096: ///< Forgemaster's Vigor
            case 177067: ///< Detonation
            case 177102: ///< Battering
            case 177086: ///< Sanitizing
            case 177081: ///< Molten Metal
                spellInfo->AttributesEx5 |= SPELL_ATTR5_START_PERIODIC_AT_APPLY;
                break;
            case 33110: ///< Prayer of Mending
                spellInfo->Effects[0].BonusMultiplier = 0.0f;
                break;
            case 109186: ///< Surge of light
                spellInfo->ProcFlags = PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS;
                break;
            case 853: ///< Hammer of Justice
                spellInfo->OverrideSpellList.push_back(105593); ///< Replace Hammer of Justice by Fist of Justice
                break;
            case 20066:  ///< Repentance
                spellInfo->InterruptFlags |= SPELL_INTERRUPT_FLAG_INTERRUPT;
                spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_INITIAL_AGGRO;
                break;
            case 114163: ///< Eternal Flame
                spellInfo->Effects[2].Effect = SPELL_EFFECT_APPLY_AURA;
                spellInfo->Effects[2].ApplyAuraName = SPELL_AURA_DUMMY;
                spellInfo->InterruptFlags |= SPELL_INTERRUPT_FLAG_INTERRUPT;
                break;
            case 202767: ///< New Moon
            case 202768: ///< Half Moon
            case 202771: ///< Full Moon
                spellInfo->InterruptFlags |= SPELL_INTERRUPT_FLAG_INTERRUPT;
                break;
            case 61999: ///< Raise Ally
                spellInfo->Effects[1].TargetA = TARGET_UNIT_TARGET_ALLY;
                break;
            case 31935: ///< Avenger's Shield
                spellInfo->Effects[EFFECT_1].ChainTarget = 3;
                spellInfo->Effects[EFFECT_2].ChainTarget = 3;
                spellInfo->DmgClass = SPELL_DAMAGE_CLASS_MAGIC;
                spellInfo->AttributesEx5 &= ~SPELL_ATTR5_CANT_IMMUNITY_SPELL;
                break;
            case 498:    ///< Divine Protection
            case 51490:  ///< Thunderstorm
            case 108416: ///< Dark Pact
            case 134758: ///< Burning Cinders
                spellInfo->AttributesEx5 |= SPELL_ATTR5_USABLE_WHILE_STUNNED;
                break;
            case 173545: /// Chi Wave (healing bolt)
                spellInfo->Effects[0].Effect = SPELL_EFFECT_DUMMY;
                break;
            case 115611: ///< Temporal Ripples
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CANT_CRIT;
                break;
            case 115610: ///< Temporal Shield
                spellInfo->AttributesEx5 |= SPELL_ATTR5_USABLE_WHILE_STUNNED;
                spellInfo->AttributesEx5 |= SPELL_ATTR5_USABLE_WHILE_FEARED;
                break;
            case 158624: ///< Angelic Feather
                spellInfo->Effects[0].TargetB = TARGET_UNIT_DEST_AREA_ENTRY;
                break;
            case 121536: ///< Angelic Feather
                spellInfo->Effects[0].TargetB = TARGET_DEST_DEST;
                break;
            case 177345: ///< Meteor
                spellInfo->Effects[0].TargetB = TARGET_UNIT_DEST_AREA_ENEMY;
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(39); ///< 2 seconds
                break;
            case 87935: ///< Serpent Spread
                spellInfo->Effects[0].Effect = SPELL_EFFECT_APPLY_AURA;
                spellInfo->Effects[0].ApplyAuraName = SPELL_AURA_DUMMY;
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(21); ///< -1s
                break;
            case 118253: /// < Serpent Sting
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_DONT_RESET_PERIODIC_TIMER;
                spellInfo->Dispel = DISPEL_NONE;
                spellInfo->Effects[EFFECT_0].Amplitude = 2600;
                break;
            case 53257: ///< Cobra Strikes
                spellInfo->Effects[0].BasePoints = 0;
                spellInfo->Effects[1].BasePoints = 0;
                break;
            case 11958: ///< Cold Snap
                spellInfo->AttributesEx5 |= SPELL_ATTR5_USABLE_WHILE_FEARED;
                spellInfo->AttributesEx5 |= SPELL_ATTR5_USABLE_WHILE_STUNNED;
                spellInfo->AttributesEx5 |= SPELL_ATTR5_USABLE_WHILE_CONFUSED;
                break;
            case 86674: ///< Ancient Healer
                spellInfo->ProcCharges = 5;
                break;
            case 3602:
                spellInfo->ProcCharges = 4;
                break;
            case 86657: ///< Ancient Guardian
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[1].ApplyAuraName = SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN;
                break;
            case 5782:  ///< Fear
                spellInfo->AttributesEx |= SPELL_ATTR1_CANT_BE_REFLECTED;
                spellInfo->Dispel = DISPEL_MAGIC;
                spellInfo->Mechanic = 0;
                spellInfo->Effects[0].Mechanic = MECHANIC_NONE;
                spellInfo->OverrideSpellList.push_back(104045); ///< Add Sleep (Metamorphosis)
                break;
            case 45204: ///< Mirror Image - Clone Me!
                spellInfo->AttributesEx6 |= SPELL_ATTR6_CAN_TARGET_INVISIBLE;
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                break;
            case 11371: ///< Arthas's Gift
                spellInfo->Effects[0].TriggerSpell = 0;
                break;
            case 33891:  ///< Incarnation: Tree of Life
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(9);
                break;
            case 115072: ///< Expel Harm
                spellInfo->Effects[0].TargetB = TARGET_UNIT_TARGET_ALLY;
                spellInfo->ExplicitTargetMask &= ~TARGET_FLAG_UNIT;
                break;
            case 117833: ///< Crazy Thought
                spellInfo->AttributesEx5 |= SPELL_ATTR5_USABLE_WHILE_FEARED;
                break;
            case 172:   ///< Corruption
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_DONT_RESET_PERIODIC_TIMER;
                break;
            case 34433: ///< Shadowfiend
                spellInfo->Effects[EFFECT_0].MiscValueB = 1561;
                spellInfo->OverrideSpellList.push_back(123040); ///< Add Mindbender to override spell list of Shadowfiend
                break;
            case 126135: ///< Lightwell
                spellInfo->OverrideSpellList.push_back(724); ///< Add old Lightwell to override list
                break;
            case 108201: ///< Desecrated Ground
                spellInfo->AttributesEx5 |= SPELL_ATTR5_USABLE_WHILE_FEARED;
                spellInfo->AttributesEx5 |= SPELL_ATTR5_USABLE_WHILE_STUNNED;
                spellInfo->AttributesEx5 |= SPELL_ATTR5_USABLE_WHILE_CONFUSED;
                spellInfo->AttributesEx |= SPELL_ATTR1_DISPEL_AURAS_ON_IMMUNITY;
                break;
            case 102359: ///< Mass Entanglement
                spellInfo->AttributesEx5 &= ~SPELL_ATTR5_SINGLE_TARGET_SPELL;
                break;
            case 64380: ///< Shattering Throw
                spellInfo->Effects[0].Effect = SPELL_EFFECT_DISPEL_MECHANIC;
                spellInfo->Effects[0].MiscValue = 29;
                break;
            case 3411:  ///< Intervene
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_RAID;
                spellInfo->AttributesEx6 |= SPELL_ATTR6_ASSIST_IGNORE_IMMUNE_FLAG;
                spellInfo->AttributesEx7 |= SPELL_ATTR7_HAS_CHARGE_EFFECT;
                ///spellInfo->OverrideSpellList.push_back(114029); ///< Add Safeguard to override spell list of Intervene ///< intervene dosent exist anymore
                break;
            case 1784:  ///< Stealth
                spellInfo->OverrideSpellList.push_back(115191); ///< Add Stealth (talent) to override spell list of Stealth
                break;
            case 115191: ///< Stealth
                spellInfo->AttributesEx |= SPELL_ATTR0_DISABLED_WHILE_ACTIVE;
                spellInfo->AttributesEx8 |= SPELL_ATTR8_AURA_SEND_AMOUNT;
                break;
            case 115192: ///< Subterfuge
                spellInfo->Attributes |= SPELL_ATTR0_DONT_AFFECT_SHEATH_STATE;
                spellInfo->Attributes |= SPELL_ATTR0_NOT_SHAPESHIFT;
                spellInfo->AttributesEx |= SPELL_ATTR1_NOT_BREAK_STEALTH;
                break;
            case 20578: ///< Cannibalize
            case 20577: ///< Cannibalize
                spellInfo->AttributesEx |= SPELL_ATTR1_NOT_BREAK_STEALTH;
                break;
            case 57879: ///< Snake Trap
            case 162480: ///< Steel Trap Root Effect
            case 162487: ///< Steel Trap Dmg Effect
                spellInfo->AttributesEx &= ~SPELL_ATTR1_NOT_BREAK_STEALTH;
                spellInfo->AttributesEx6 |= SPELL_ATTR6_CAN_TARGET_INVISIBLE;
                break;
            case 121733: ///< Throw
                spellInfo->OverrideSpellList.push_back(114014); ///< Add Shuriken Toss to override spell list of Throw
                break;
            case 44457: ///< Living Bomb
                spellInfo->AttributesEx5 &= ~SPELL_ATTR5_SINGLE_TARGET_SPELL;
                spellInfo->MaxAffectedTargets = 3;
                break;
            case 23691: ///< Berzerker Rage Effect
                spellInfo->Effects[0].BasePoints = 100;
                break;
            case 122128: ///< Divine Star (shadow)
            case 110745: ///< Divine Star (other)
                spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(29);
                //spellInfo->Effects[1].RadiusEntry = sSpellRadiusStore.LookupEntry(29);
                break;
            case 12975: ///< Last Stand
                spellInfo->Effects[0].ApplyAuraName = SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT;
                break;
            case 114695: ///< Pursuit of Justice
                spellInfo->Effects[0].BasePoints = 0;
                break;
            case 115315: ///< Summon Black Ox Statue
                spellInfo->Effects[1].Effect = SPELL_EFFECT_APPLY_AURA;
                spellInfo->Effects[1].ApplyAuraName = SPELL_AURA_DUMMY;
                spellInfo->Effects[1].TargetA = TARGET_UNIT_CASTER;
                break;
            case 114908: ///< Spirit Shell
            case 50273: ///< Arcane Barrage
                spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_DONE_BONUS;
                break;
            case 153596:///< Comet Storm
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                spellInfo->Speed = 0.20f;
                break;
            case 12654: ///< Ignite
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_DONT_RESET_PERIODIC_TIMER;
                break;
            case 153564:///< Meteor
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_SHARE_PLAYER_DAMAGE;
                spellInfo->AttributesEx5 |= SPELL_ATTR5_USABLE_WHILE_FEARED;
                spellInfo->AttributesEx5 |= SPELL_ATTR5_USABLE_WHILE_STUNNED;
                spellInfo->AttributesEx5 |= SPELL_ATTR5_USABLE_WHILE_CONFUSED;
                spellInfo->AttributesEx |= SPELL_ATTR1_CANT_BE_REFLECTED;
                break;
            case 153561:///< Meteor (launch spell)
                spellInfo->AttributesEx |= SPELL_ATTR1_CANT_BE_REFLECTED;
                break;
            case 155158:///< Meteor (periodic damage)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_DONT_RESET_PERIODIC_TIMER;
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                spellInfo->AttributesEx |= SPELL_ATTR1_CANT_BE_REFLECTED;
                spellInfo->AttributesEx5 |= SPELL_ATTR5_HIDE_DURATION;
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(39); // 2s
                break;
            case 52610: ///< Savage Roar
                spellInfo->Effects[2].BasePoints = 40;
                break;
            case 8676:  ///< Ambush
                spellInfo->Effects[0].BonusMultiplier = 0;
                spellInfo->Effects[1].BonusMultiplier = 0;
                break;
            case 49376: ///< Wild Charge
                spellInfo->Effects[1].MiscValue = 25;
                spellInfo->Effects[1].ValueMultiplier = 0;
                break;
            case 169157:///< Demonic Leap (jump)
                spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(43);
                spellInfo->Effects[0].MiscValue = 35;
                spellInfo->Effects[0].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[0].TargetB = TARGET_DEST_CASTER_FRONT;
                break;
            case 77535: ///< Blood Shield
                spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_DONE_BONUS;
                spellInfo->AttributesEx6 |= SPELL_ATTR6_NO_DONE_PCT_DAMAGE_MODS;
                break;
            case 53:    ///< Backstap
                spellInfo->Effects[0].BonusMultiplier = 0;
                break;
            case 86121: ///< Soul Swap
                spellInfo->AttributesEx |= SPELL_ATTR1_CANT_BE_REFLECTED;
                break;
            case 86213: ///< Soul Swap Exhale
                spellInfo->Speed = 0;
                break;
            case 157695:///< Demonbolt
                spellInfo->AttributesEx2 |= SPELL_ATTR2_NOT_NEED_SHAPESHIFT;
                spellInfo->AttributesEx4 |= SPELL_ATTR4_IGNORE_RESISTANCES;
                break;
            case 169686:///< Unyielding Strikes
                spellInfo->ProcCharges = 0;
                break;
            case 781:   ///< Disengage
                spellInfo->SpecializationIdList.push_back(SPEC_HUNTER_SURVIVAL);
                spellInfo->Effects[0].TriggerSpell = 0; ///< Handled in Player::HandleFall()
                spellInfo->AttributesEx |= SPELL_ATTR1_NOT_BREAK_STEALTH;
                break;
            case 111400:///< Burning Rush
                spellInfo->AuraInterruptFlags = AURA_INTERRUPT_FLAG_NOT_ABOVEWATER + AURA_INTERRUPT_FLAG_NOT_UNDERWATER;
                break;
            case 96840: ///< Flame Patch for Glyph of the Blazing Trail
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(285); ///< 1s
                break;
            case 7922: ///< Warbringer stun effect
            case 103828: ///< Warbringer
            case 100:   ///< Charge
                spellInfo->AttributesEx3 |= SPELL_ATTR3_IGNORE_HIT_RESULT;
                break;
            case 45438: ///< Ice Block
                spellInfo->AttributesEx5 |= SPELL_ATTR5_USABLE_WHILE_STUNNED;
                spellInfo->Effects[EFFECT_2].MiscValue = SPELL_SCHOOL_MASK_ALL; ///< Chaos Damage
                break;
            case 170995:///< Cripple
                spellInfo->CastTimeEntry = sSpellCastTimesStore.LookupEntry(2); ///< 250ms - hack fix to imagine Seduction mechanic
                spellInfo->AttributesEx |= SPELL_ATTR1_NO_THREAT;
                spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_INITIAL_AGGRO;
                break;
            case 118283:///< Ursol's Vortex
                spellInfo->Effects[0].ValueMultiplier = 60;
                break;
            case 201467:
                spellInfo->AttributesEx9 &= ~SPELL_ATTR9_SPECIAL_DELAY_CALCULATION;
                break;
            case 642:   ///< Divine Shield
                spellInfo->AttributesEx5 |= SPELL_ATTR5_USABLE_WHILE_STUNNED;
                break;
            case 189999:///< Grove Warden
            case 171828:///< Solar Spikehawk
                spellInfo->Effects[0].MiscValue = 74410;
                spellInfo->Effects[0].MiscValueB = 248;
                break;
            case 187356: ///< Mysic Image (Magic Pet Mirror)
                spellInfo->AttributesEx3 &= ~SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                break;
            case 7268: ///< Arcane Missiles
                spellInfo->AttributesEx &= ~SPELL_ATTR1_CHANNELED_1;
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 5143: ///< Arcane Missiles source
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ENEMY;
                break;
            case 174004:///< Spirit of Shinri
                spellInfo->Effects[1].MiscValue = 82415;
                break;
            case 202940:///< Moon and Stars
                spellInfo->ProcFlags = PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_NEG;
                break;
            case 157154:///< High Tide
                spellInfo->AttributesEx12 &= ~SPELL_ATTR12_UNK21;
                break;
            case 168941:///< Aimed Shot copy for Trick Shot
                spellInfo->Effects[1].Effect = SPELL_EFFECT_SCHOOL_DAMAGE;
                /// no break
            case 63468: ///< Careful Aim DOT
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CANT_CRIT;
                spellInfo->AttributesEx3 |= SPELL_ATTR3_IGNORE_HIT_RESULT;
                spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_DONE_BONUS;
                spellInfo->AttributesEx6 |= SPELL_ATTR6_NO_DONE_PCT_DAMAGE_MODS;
                break;
            /// All spells - BonusMultiplier = 0
            case 77758: ///< Thrash (bear)
            case 106830:///< Thrash (cat)
            case 5221:  ///< Shred
            case 22599: ///< Chromatic Mantle of the Dawn
                spellInfo->Effects[0].BonusMultiplier = 0;
                break;
            case 22568: ///< Ferocious Bite
                spellInfo->Effects[0].BonusMultiplier = 0;
                spellInfo->Effects[1].Effect = SPELL_EFFECT_DUMMY; /// 7.3.2 Build 25549 Added this because the 'burn' is dealt with in Ferocious Bite script. It needs to be done BEFORE Soul of the Forest PlayerScript (OnModifyPower) to prevent Ferocious bite to use extra Energy that was given by Soul of the Forest's proc.
                break;
            /// All spells - ProcFlags = 0
            case 212035: ///< Cheap Tricks
            case 207654: ///< Servant of the Queen
            case 207982: ///< Focused Rage
            case 166061: ///< Item - Death Knight WoD PvP Unholy 4P Bonus
            case 170848: ///< Item - Druid WoD PvP Feral 2P Bonus
            case 170853: ///< Item - Druid WoD PvP Restoration 2P Bonus
            case 165691: ///< Item - Monk WoD PvP Windwalker/Brewmaster 2P Bonus
            case 165639: ///< Item - Warrior WoD PvP Fury 2P Bonus
            case 165636: ///< Item - Warrior WoD PvP Arms 2P Bonus
            case 165641: ///< Item - Warrior WoD PvP Protection 2P Bonus
            case 165995: ///< Item - Rogue WoD PvP 2P Bonus
            case 170877: ///< Item - Rogue WoD PvP Subtlety 4P Bonus
            case 182303: ///< Item - Rogue WoD PvP Combat 4P Bonus
            case 170883: ///< Item - Rogue WoD PvP Assassination 4P Bonus
            case 165886: ///< Item - Paladin WoD PvP Retribution 2P Bonus
            case 165905: ///< Item - Paladin WoD PvP Protection 2P Bonus
            case 166005: ///< Item - Hunter WoD PvP 2P Bonus
            case 166009: ///< Item - Hunter WoD PvP 2P Bonus
            case 165519: ///< Item - Hunter WoD PvP 2P Bonus
            case 171383: ///< Item - Warlock WoD PvP Destruction 2P Bonus
            case 171379: ///< Item - Warlock WoD PvP Affliction 4P Bonus
            case 165482: ///< Item - Rogue T17 Subtlety 2P Bonus
            case 87160:  ///< Surge of Darkness
            case 73685:  ///< Unleash Life (restoration)
            case 116768: ///< Combo Breaker (blackout kick)
            case 64803:  ///< Entrapment
            case 135373: ///< Entrapment
            case 193539: ///< Alacrity
            case 251882:
                spellInfo->ProcFlags = 0;
                break;
            /// All spells - ProcCharges = 1
            case 48108:  ///< Hot Streak
            case 114250: ///< Selfless Healer
            case 131567: ///< Holy Spark
            case 114028: ///< Mass Spell Reflection
            case 122510: ///< Ultimatum
            case 52437:  ///< Sudden Death
            case 246261: ///< Ignition
                spellInfo->ProcCharges = 1;
                break;
            case 111771: ///< Demonic Gateway (launch spell)
                spellInfo->Effects[1].Effect = 0;
                break;
            case 198670: ///< Piercing Shot
                spellInfo->Effects[1].Effect = SPELL_EFFECT_DUMMY;
                spellInfo->Effects[2].Effect = 0;
                spellInfo->Effects[3].Effect = 0;
                spellInfo->Effects[4].Effect = 0;
                break;
            case 113890: ///< Demonic Gateway
                spellInfo->Effects[0].TargetA = TARGET_DEST_DEST;
                break;
            case 113886: ///< Demonic Gateway
                spellInfo->Effects[0].TargetA = TARGET_UNIT_CASTER;
                break;
            case 131116: ///< Allow to use Raging Blow
                spellInfo->StackAmount = 2;
                break;
            case 137017: ///< Survival Hunter
                spellInfo->Effects[EFFECT_1].SpellClassMask[1] &= ~0x80000000;
                break;
            case 85222: ///< Light of Dawn
                spellInfo->MaxAffectedTargets = 6;
                spellInfo->InterruptFlags |= SPELL_INTERRUPT_FLAG_INTERRUPT;
                spellInfo->DmgClass = SPELL_DAMAGE_CLASS_MAGIC;
                break;
            case 225311: ///< Light of Dawn
            case 185984: ///< Light of Dawn
            case 210376: ///< Light of Dawn
                spellInfo->DmgClass = SPELL_DAMAGE_CLASS_MAGIC;
                break;
            case 2641: ///< Dismiss Pet
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_DEAD;
                break;
            case 52042: ///< Healing Stream - Totem
                spellInfo->Effects[0].Effect = SPELL_EFFECT_HEAL;
                spellInfo->Effects[0].BasePoints = 31;
                break;
            case 115008: ///< Chi Torpedo
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(596); // 900 ms
                spellInfo->OverrideSpellList.push_back(121828); ///< Override List with Chi Torpedo - Talent
                spellInfo->Effects[2].ApplyAuraName = SPELL_AURA_MOD_SPEED_NOT_STACK;
                break;
            case 109132: ///< Roll
                spellInfo->OverrideSpellList.push_back(121827); ///< Override List with Roll - Talent
                break;
            case 124273: ///< Heavy Stagger
            case 124274: ///< Moderate Stagger
            case 124275: ///< Light Stagger
                spellInfo->Effects[0].Effect = SPELL_EFFECT_APPLY_AURA;
                spellInfo->Effects[0].ApplyAuraName = SPELL_AURA_PERIODIC_DAMAGE;
                spellInfo->Effects[0].Amplitude = 500;
                spellInfo->Effects[0].BasePoints = 0;
                spellInfo->AttributesCu |= SpellCustomAttributes::SPELL_ATTR0_CU_DONT_RESET_PERIODIC_TIMER;
                break;
            case 124255: ///< Stagger
                spellInfo->Effects[0].Effect = SPELL_EFFECT_DUMMY;
                break;
            case 108937: ///< Baby Elephant Takes a Bath
                spellInfo->Effects[1].BasePoints = 40;
                spellInfo->Effects[1].MiscValue = 100;
                break;
            case 115129: ///< Expel Harm - Damage to a nearby ennemy within 10 yards
                spellInfo->Effects[0].TargetA = TARGET_SRC_CASTER;
                spellInfo->Effects[0].TargetB = TARGET_UNIT_NEARBY_ENEMY;
                spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(13);
                break;
            case 145110: ///< Ysera's Gift
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_SRC_CASTER;
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ALLY;
                break;
            case 126892: ///< Zen Pilgrimage
            case 126895: ///< Zen Pilgrimage : Return
                spellInfo->Effects[0].Effect = SPELL_EFFECT_DUMMY;
                break;
            case 126: ///< Eye of Kilrogg
                spellInfo->Effects._effects.push_back(SpellEffectInfo(sSpellStore.LookupEntry(spellInfo->Id), spellInfo, EFFECT_2, Difficulty::DifficultyNone));
                spellInfo->Effects[EFFECT_2].Effect = SPELL_EFFECT_APPLY_AURA;
                spellInfo->Effects[EFFECT_2].ApplyAuraName = SPELL_AURA_MOD_STEALTH_DETECT;
                spellInfo->Effects[EFFECT_2].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[EFFECT_2].BasePoints = 500;
                spellInfo->Effects[EFFECT_2].MiscValue = 1;
                break;
            case 107223: ///< Sunfire Rays
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                break;
            case 106909: ///< Jade Soul
            {
                SpellInfo const* spell = sSpellMgr->GetSpellInfo(113379);
                if (!spell)
                    break;
                spellInfo->DurationEntry = spell->DurationEntry;
                break;
            }
            case 113315: ///< Intensity
            {
                SpellInfo const* spell = sSpellMgr->GetSpellInfo(113379);
                if (!spell)
                    break;
                spellInfo->DurationEntry = spell->DurationEntry;
                break;
            }
            case 106736: ///< Wither Will
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                spellInfo->Effects[0].TargetB = TARGET_UNIT_TARGET_ENEMY;
                break;
            case 106113: ///< Touch of Nothingness
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                spellInfo->Effects[0].TargetB = TARGET_UNIT_TARGET_ENEMY;
                break;
            case 119922: ///< Shockwave
            case 119929: ///< Shockwave
            case 119930: ///< Shockwave
            case 119931: ///< Shockwave
            case 119932: ///< Shockwave
            case 119933: ///< Shockwave
                spellInfo->Speed = 5.0f;
                break;
            case 106112: ///< Release Doubt
            {
                const SpellRadiusEntry* radius = sSpellRadiusStore.LookupEntry(22);
                if (!radius)
                    break;
                spellInfo->Effects[0].RadiusEntry = radius; ///< 200yards.
                break;
            }
            case 106847: ///< Barrel Toss
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                break;
            case 106334: ///< Wash Away
                spellInfo->AttributesEx3 &= ~ SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CONE_ENEMY_24;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            case 124974: ///< Nature's Vigil
                spellInfo->AttributesEx7 &= ~SPELL_ATTR7_ALLIANCE_ONLY;
                spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(16);
                break;
            case 120552: ///< Mantid Munition Explosion
                spellInfo->Effects[0].RadiusEntry = sSpellRadiusStore.LookupEntry(16);
                break;
            case 119684: ///< Ground Slam
                spellInfo->Effects[0].TargetA = TARGET_UNIT_CONE_ENEMY_24;
                spellInfo->Effects[0].TargetB = 0;
                spellInfo->Effects[1].TargetA = TARGET_UNIT_CONE_ENEMY_24;
                spellInfo->Effects[1].TargetB = 0;
                break;
            case 106853: ///< Fists of Fury
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                spellInfo->Effects[0].TargetB = 0;
                break;
            case 112060: ///< Apparitions
                spellInfo->Effects[0].TargetB = 0;
                break;
            case 118685: ///< Battering Headbutt
                spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(5);
                break;
            case 60670: ///< Malygos Enrage
                spellInfo->Effects[1].TriggerSpell = 0;
                spellInfo->Effects[2].TriggerSpell = 0;
                break;
            case 114746: ///< Snatch Master's Flame
                spellInfo->Effects[2].TargetA = TARGET_UNIT_TARGET_ALLY;
                spellInfo->Effects[2].TargetB = 0;
                break;
            case 104855: ///< Overpacked Firework
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ANY;
                break;
            case 225137: ///< Star Gate
                spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                break;
            case 225776: ///< Devil's Due - Whispers in the Dark
                spellInfo->AttributesCu |= SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY;
                break;
            case 225766: ///< Temporal Shift Buff
            case 225767: ///< Temporal Shift Buff
            case 225768: ///< Temporal Shift Buff
            case 225769: ///< Temporal Shift Buff
            case 225770: ///< Temporal Shift Buff
            case 225771: ///< Temporal Shift Buff
                spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                break;
            //////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////////
            /// Fishing
            /// Add Server-Side dummy spell for Fishing
            /// TODO : Add more generic system to load server-side spell
            case 7733:   ///< Apprentice Fishing
            case 7734:   ///< Journeyman Fishing
            case 18249:  ///< Artisan Fishing
            case 54083:  ///< Expert Fishing
            case 54084:  ///< Master Fishing
            case 51293:  ///< Grand Master Fishing
            case 88869:  ///< Illustrious Grand Master Fishing
            case 110412: ///< Zen Master Fishing
            {
                std::unordered_map<uint32, SpellVisualMap> l_VisualsBySpell;
                SpellInfo* fishingDummy = new SpellInfo(sSpellStore.LookupEntry(131474), difficulty, std::move(l_VisualsBySpell[spellInfo->Effects[0].TriggerSpell]));
                fishingDummy->Id = spellInfo->Effects[0].TriggerSpell;
                mSpellInfoMap[difficulty][spellInfo->Effects[0].TriggerSpell] = fishingDummy;
                break;
            }
            case 131474: ///< Fishing (dummy), it has no aura effects, remove channel flags to prevent 'Interrupted' message
                spellInfo->AttributesEx &= ~(SPELL_ATTR1_CHANNELED_1 | SPELL_ATTR1_CHANNEL_TRACK_TARGET | SPELL_ATTR1_CHANNEL_DISPLAY_SPELL_NAME);
                break;
            //////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////////
            /// Mogu'shan Vault
            case 116161: ///< Crossed Over
                spellInfo->Effects[EFFECT_1].MiscValue = 2; ///< Set Phase to 2
                spellInfo->Effects[EFFECT_3].Effect    = 0; ///< No need to summon
                break;
            case 116272: ///< Banishment
                spellInfo->Effects[EFFECT_0].MiscValue = 2; ///< Set Phase to 2
                break;
            case 116606: ///< Troll Rush
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                break;
            case 118303: ///< Fixate
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                break;
            case 15850: ///< Chilled
            case 16927: ///< Chilled
            case 20005: ///< Chilled
                spellInfo->Mechanic = MECHANIC_SNARE;
                break;
            case 89640: ///< Water Spirit
                spellInfo->Effects[EFFECT_0].BasePoints = 5000;
                spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_DONE_BONUS;
                break;
            case 130649: ///< Life Spirit
                spellInfo->Effects[EFFECT_0].BasePoints = 20000;
                spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_DONE_BONUS;
                break;
            case 127362: ///< Grasping Energy Tendrils
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                spellInfo->Effects[EFFECT_1].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                break;
            case 66630: ///< Isle of Conquest Gunship Portal Alliance
            case 66637: ///< Isle of Conquest Gunship Portal Horde
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                break;
            case 66550: ///< teleports outside (Isle of Conquest)
            case 66551: ///< teleports inside (Isle of Conquest)
                spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_DUMMY;
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            case 60206: ///< Ram
                spellInfo->Effects[2].RadiusEntry = sSpellRadiusStore.LookupEntry(13);
                break;
            case 101085: ///< Wrath of Tarecgosa
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_TRIGGERED_IGNORE_RESILENCE;
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CANT_CRIT;
                spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_DONE_BONUS;
                spellInfo->AttributesEx3 |= SPELL_ATTR3_IGNORE_HIT_RESULT;
                spellInfo->AttributesEx6 |= SPELL_ATTR6_IGNORE_CASTER_AURAS;
                spellInfo->AttributesEx6 |= SPELL_ATTR6_NO_DONE_PCT_DAMAGE_MODS;
                break;
            case 147362: ///< Counter Shot
                spellInfo->Speed = 0;
                break;
            case 36819: ///< Pyroblast (Kael'thas)
                spellInfo->AttributesEx |= SPELL_ATTR1_CANT_BE_REFLECTED;
                break;
            case 25677: ///< Snowball
                spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_DUMMY;
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ANY;
                break;
            case 54402: ///< Land Mine Knockback
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_DEST_AREA_ALLY;
                spellInfo->Effects[EFFECT_1].TargetB = TARGET_UNIT_DEST_AREA_ALLY;
                break;
            case 81262: ///< Efflorensence
                spellInfo->AttributesEx5 &= ~SPELL_ATTR5_START_PERIODIC_AT_APPLY;
                break;
            case 92601: ///< Detonate Mana, Tyrande's Favorite Doll
                spellInfo->CasterAuraSpell = 92596;
                break;
            case 21079: ///< Shard of the Defiler - Echo of Archimonde
                spellInfo->Effects[EFFECT_1].Effect = 0;
                spellInfo->Effects[EFFECT_1].ApplyAuraName = 0;
                spellInfo->Effects[EFFECT_2].Effect = 0;
                spellInfo->Effects[EFFECT_2].ApplyAuraName = 0;
                break;
            case 88646: ///< Burn Hay, Braizer Torch
                spellInfo->Effects[EFFECT_1].Effect = 0;
                spellInfo->Effects[EFFECT_1].TargetA = 0;
                spellInfo->Effects[EFFECT_1].TargetB = 0;
                break;
            case 99152: ///< Cauterizing Flame
                spellInfo->AttributesEx |= SPELL_ATTR1_CANT_TARGET_SELF;
                break;
            case 100403: ///< Moonwell Chalice
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ANY;
                break;
            case 78628: ///< Arest, Sturdy Manacles, Seize the Ambassador
                spellInfo->Effects[EFFECT_0].Effect = 0;
                break;
            case 75206: ///< Flamebreaker, Flameseer's Staff, Flamebreaker quest
                spellInfo->Effects[EFFECT_0].Effect = 0;
                spellInfo->Effects[EFFECT_0].ApplyAuraName = 0;
                spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                break;
            case 74723: ///< Summon Unbound Flamesparks, Flameseer's Staff, Flamebreaker quest
                spellInfo->Effects[EFFECT_0].MiscValue = 40065;
                break;
            case 75346: ///< Inflate Air Balloon, Undersea Inflation quest
                spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_KILL_CREDIT;
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[EFFECT_0].MiscValue = 40399;
                spellInfo->Effects[EFFECT_1].Effect = 0;
                spellInfo->Effects[EFFECT_1].ApplyAuraName = 0;
                break;
            case 89821: ///< Hercular's Rod
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                break;
            case 87604: ///< Fortune Cookie
                spellInfo->Effects[EFFECT_2].Effect = SPELL_EFFECT_SCRIPT_EFFECT;
                break;
            case 91041: ///< Heart's Judgment, Heart of Ignacious trinket
                spellInfo->CasterAuraSpell = 91027;
                break;
            case 92328: ///< Heart's Judgment, Heart of Ignacious trinket (heroic)
                spellInfo->CasterAuraSpell = 92325;
                break;
            case 45182: ///< Cheat Death
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN;
                break;
            case 68219: ///< Rescue Krenann, Save Krenann Aranas quest
            case 72971: ///< Rockin' Powder Infused Rocket Boots
                spellInfo->RequiresSpellFocus = 0;
                break;
            case 69971: ///< Summon Steamwheedle Shark 000
            case 69976: ///< Summon Steamwheedle Shark 001
            case 69977: ///< Summon Steamwheedle Shark 002
            case 69978: ///< Summon Steamwheedle Shark 003
            case 69979: ///< Summon Steamwheedle Shark 004
            case 69980: ///< Summon Steamwheedle Shark 005
            case 69981: ///< Summon Steamwheedle Shark 006
            case 69982: ///< Summon Steamwheedle Shark 007
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_DEST_DEST;
                break;
            case 253304: ///< Corruption of Shatug - Shadow-Singed Fang
            case 253323: ///< Shadow Strike - Seeping Scourgewing
            case 242325: ///< Guilty Conscience - Barbaric Mindslaver
            case 16589: ///< Noggenfogger Elixir
                spellInfo->AttributesEx8 |= SpellAttr8::SPELL_ATTR8_NOT_IN_BG_OR_ARENA;
                break;
            //////////////////////////////////////////////////////////////////////////////////////
            /// RUBY SANCTUM SPELLS
            //////////////////////////////////////////////////////////////////////////////////////
            case 74769: ///< Twilight Cutter
            {
                const SpellRadiusEntry* radius = sSpellRadiusStore.LookupEntry(12);
                if (!radius)
                    break;
                spellInfo->Effects[0].RadiusEntry = radius; ///< 200yards.
                break;
            }
            case 75509: ///< Twilight Mending
                spellInfo->AttributesEx6 |= SPELL_ATTR6_CAN_TARGET_INVISIBLE;
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                break;
            case 75888: ///< Awaken Flames
                spellInfo->AttributesEx |= SPELL_ATTR1_CANT_TARGET_SELF;
                break;
            case 74799: ///< Soul Consumption
            {
                const SpellRadiusEntry* radius = sSpellRadiusStore.LookupEntry(32);
                if (!radius)
                    break;
                spellInfo->Effects[1].RadiusEntry = radius; ///< 200yards.
                break;
            }
            case 74802: ///< Consumption (10man normal)
            case 74630: ///< Combustion (10man normal)
            {
                const SpellRadiusEntry* radius = sSpellRadiusStore.LookupEntry(22);
                if (!radius)
                    break;
                spellInfo->Effects[0].RadiusEntry = radius; ///< 200yards.
                break;
            }
            case 74562: ///< Fiery Combustion
            case 74792: ///< Soul Consumption
                spellInfo->AttributesEx |= (SPELL_ATTR1_CANT_BE_REFLECTED|SPELL_ATTR1_CANT_BE_REDIRECTED);
                break;
            /// ENDOF RUBY SANCTUM SPELLS
            //////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////////
            /// EYE OF ETERNITY SPELLS
            /// All spells below work even without these changes. The LOS attribute is due to problem
            /// from collision between maps & gos with active destroyed state.
            case 57473: ///< Arcane Storm bonus explicit visual spell
            case 57431: ///< Summon Static Field
            case 56091: ///< Flame Spike (Wyrmrest Skytalon)
            case 56092: ///< Engulf in Flames (Wyrmrest Skytalon)
            case 57090: ///< Revivify (Wyrmrest Skytalon)
            case 57143: ///< Life Burst (Wyrmrest Skytalon)
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                break;
            /// This would never crit on retail and it has attribute for SPELL_ATTR3_NO_DONE_BONUS because is handled from player,
            /// until someone figures how to make scions not critting without hack and without making them main casters this should stay here.
            case 63934: ///< Arcane Barrage (casted by players and NONMELEEDAMAGELOG with caster Scion of Eternity (original caster)).
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CANT_CRIT;
                break;
            /// ENDOF EYE OF ETERNITY SPELLS
            //////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////////
            ///
            /// ICECROWN CITADEL SPELLS
            ///
            /// THESE SPELLS ARE WORKING CORRECTLY EVEN WITHOUT THIS HACK
            /// THE ONLY REASON ITS HERE IS THAT CURRENT GRID SYSTEM
            /// DOES NOT ALLOW FAR OBJECT SELECTION (dist > 333)
            case 70781: ///< Light's Hammer Teleport
            case 70856: ///< Oratory of the Damned Teleport
            case 70857: ///< Rampart of Skulls Teleport
            case 70858: ///< Deathbringer's Rise Teleport
            case 70859: ///< Upper Spire Teleport
            case 70860: ///< Frozen Throne Teleport
            case 70861: ///< Sindragosa's Lair Teleport
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DB;
                break;
            case 69055: ///< Saber Lash (Lord Marrowgar)
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_5_YARDS); ///< 5yd
                break;
            case 69075: ///< Bone Storm (Lord Marrowgar)
            case 72864: ///< Death Plague (Rotting Frost Giant)
            case 71160: ///< Plague Stench (Stinky)
            case 71123: ///< Decimate (Stinky & Precious)
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_100_YARDS); ///< 100yd
                break;
            case 72378: ///< Blood Nova (Deathbringer Saurfang)
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_200_YARDS);
                spellInfo->Effects[EFFECT_1].SetRadiusIndex(EFFECT_RADIUS_200_YARDS);
                break;
            case 72385: ///< Boiling Blood (Deathbringer Saurfang)
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_200_YARDS);
                break;
            case 72769: ///< Scent of Blood (Deathbringer Saurfang)
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_200_YARDS);
                /// no break
            case 72771: ///< Scent of Blood (Deathbringer Saurfang)
                spellInfo->Effects[EFFECT_1].SetRadiusIndex(EFFECT_RADIUS_200_YARDS);
                break;
            case 72723: ///< Resistant Skin (Deathbringer Saurfang adds)
                /// this spell initially granted Shadow damage immunity, however it was removed but the data was left in client
                spellInfo->Effects[EFFECT_2].Effect = 0;
                break;
            case 70460: ///< Coldflame Jets (Traps after Saurfang)
                spellInfo->SetDurationIndex(1); ///< 10 seconds
                break;
            case 71412: ///< Green Ooze Summon (Professor Putricide)
            case 71415: ///< Orange Ooze Summon (Professor Putricide)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                break;
            case 71159: ///< Awaken Plagued Zombies
                spellInfo->SetDurationIndex(21);
                break;
            case 71127: ///< Mortal Wound
                spellInfo->AttributesEx3 |= SPELL_ATTR3_IGNORE_HIT_RESULT;
                break;
            case 70530: ///< Volatile Ooze Beam Protection (Professor Putricide)
                spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_APPLY_AURA; ///< for an unknown reason this was SPELL_EFFECT_APPLY_AREA_AURA_RAID
                break;
            case 69508: ///< Slime Spray
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                break;
            /// THIS IS HERE BECAUSE COOLDOWN ON CREATURE PROCS IS NOT IMPLEMENTED
            case 71604: ///< Mutated Strength (Professor Putricide)
                spellInfo->Effects[EFFECT_1].Effect = 0;
                break;
            case 72454: ///< Mutated Plague (Professor Putricide)
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_50000_YARDS); ///< 50000yd
                break;
            case 70911: ///< Unbound Plague (Professor Putricide) (needs target selection script)
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_TARGET_ENEMY;
                break;
            case 71518: ///< Unholy Infusion Quest Credit (Professor Putricide)
            case 72934: ///< Blood Infusion Quest Credit (Blood-Queen Lana'thel)
            case 72289: ///< Frost Infusion Quest Credit (Sindragosa)
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_50000_YARDS); ///< another missing radius
                break;
            case 70232: ///< Empowered Blood
            case 70320: ///< Empowered Blood
                spellInfo->Effects[EFFECT_0].MiscValue = 127;
                spellInfo->Effects[EFFECT_0].MiscValueB = 127;
                break;
            case 71708: ///< Empowered Flare (Blood Prince Council)
                spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_DONE_BONUS;
                break;
            case 71266: ///< Swarming Shadows
                spellInfo->AreaGroupId = 0; ///< originally, these require area 4522, which is... outside of Icecrown Citadel
                break;
            case 26023: ///< Pursuit of Justice (Rank 1)
                spellInfo->Effects[1].Effect = SPELL_EFFECT_APPLY_AURA;
                spellInfo->Effects[1].BasePoints = 14;
                spellInfo->Effects[1].ApplyAuraName = SPELL_AURA_MOD_MOUNTED_SPEED_NOT_STACK;
                spellInfo->Effects[1].TargetA = TARGET_UNIT_CASTER;
                break;
            case 70715: ///< Column of Frost (visual marker)
                spellInfo->SetDurationIndex(32); ///< 6 seconds (missing)
                break;
            case 71085: ///< Mana Void (periodic aura)
                spellInfo->SetDurationIndex(9); ///< 30 seconds (missing)
                break;
            case 70936: ///< Summon Suppressor (needs target selection script)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            case 72706: ///< Achievement Check (Valithria Dreamwalker)
            case 71357: ///< Order Whelp
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_200_YARDS);   ///< 200yd
                break;
            case 70598: ///< Sindragosa's Fury
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                break;
            case 69846: ///< Frost Bomb
                spellInfo->Speed = 0.0f;    ///< This spell's summon happens instantly
                break;
            case 71614: ///< Ice Lock
                spellInfo->Mechanic = MECHANIC_STUN;
                break;
            case 72762: ///< Defile
                spellInfo->SetDurationIndex(559); ///< 53 seconds
                break;
            case 72743: ///< Defile
                spellInfo->SetDurationIndex(22); ///< 45 seconds
                break;
            case 72754: ///< Defile
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_200_YARDS); ///< 200yd
                spellInfo->Effects[EFFECT_1].SetRadiusIndex(EFFECT_RADIUS_200_YARDS); ///< 200yd
                break;
            case 69030: ///< Val'kyr Target Search
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_200_YARDS); ///< 200yd
                spellInfo->Effects[EFFECT_1].SetRadiusIndex(EFFECT_RADIUS_200_YARDS); ///< 200yd
                break;
            case 69198: ///< Raging Spirit Visual
                spellInfo->SetRangeIndex(13); ///< 50000yd
                break;
            case 73654: ///< Harvest Souls
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_50000_YARDS); ///< 50000yd
                spellInfo->Effects[EFFECT_1].SetRadiusIndex(EFFECT_RADIUS_50000_YARDS); ///< 50000yd
                spellInfo->Effects[EFFECT_2].SetRadiusIndex(EFFECT_RADIUS_50000_YARDS); ///< 50000yd
                break;
            case 72546: ///< Harvest Soul
            case 72597: ///< Harvest Soul
            case 72608: ///< Harvest Soul
                spellInfo->AttributesEx7 |= SPELL_ATTR7_ZONE_TELEPORT;
                break;
            case 73655: ///< Harvest Soul
                spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_DONE_BONUS;
                spellInfo->AttributesEx7 |= SPELL_ATTR7_ZONE_TELEPORT;
                break;
            case 73540: ///< Summon Shadow Trap
                spellInfo->SetDurationIndex(23); ///< 90 seconds
                break;
            case 73530: ///< Shadow Trap (visual)
                spellInfo->SetDurationIndex(28); ///< 5 seconds
                break;
            case 73529: ///< Shadow Trap
                spellInfo->Effects[EFFECT_1].SetRadiusIndex(EFFECT_RADIUS_10_YARDS); ///< 10yd
                break;
            case 74282: ///< Shadow Trap (searcher)
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_3_YARDS); ///< 3yd
                break;
            case 72595: ///< Restore Soul
            case 73650: ///< Restore Soul
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_200_YARDS); ///< 200yd
                break;
            case 74086: ///< Destroy Soul
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_200_YARDS); ///< 200yd
                break;
            case 74302: ///< Summon Spirit Bomb
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_200_YARDS); ///< 200yd
                spellInfo->MaxAffectedTargets = 1;
                break;
            case 73579: ///< Summon Spirit Bomb
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_25_YARDS); ///< 25yd
                break;
            case 72350: ///< Fury of Frostmourne
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_50000_YARDS); ///< 50000yd
                spellInfo->Effects[EFFECT_1].SetRadiusIndex(EFFECT_RADIUS_50000_YARDS); ///< 50000yd
                spellInfo->Effects[EFFECT_1].Effect = SPELL_EFFECT_INSTAKILL;
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_SRC_CASTER;
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                spellInfo->Effects[EFFECT_0].Amplitude = 50000;
                break;
            case 75127: ///< Kill Frostmourne Players
            case 72351: ///< Fury of Frostmourne
            case 72431: ///< Jump (removes Fury of Frostmourne debuff)
            case 72429: ///< Mass Resurrection
            case 73159: ///< Play Movie
            case 73582: ///< Trigger Vile Spirit (Inside, Heroic)
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_50000_YARDS); ///< 50000yd
                break;
            case 72376: ///< Raise Dead
                spellInfo->MaxAffectedTargets = 3;
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_50000_YARDS); ///< 50000yd
                break;
            case 71809: ///< Jump
                spellInfo->SetRangeIndex(3); ///< 20yd
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_25_YARDS); ///< 25yd
                break;
            case 72405: ///< Broken Frostmourne
                spellInfo->Effects[EFFECT_1].SetRadiusIndex(EFFECT_RADIUS_200_YARDS); ///< 200yd
                break;
            case 68872: ///< Soulstorm (Bronjahm Encounter)
                spellInfo->InterruptFlags = 0;
                break;
            case 202818: ///< Blur (used by Vengeful Retreat when you have the honor talent Glimpse)
                spellInfo->SetDurationIndex(27);
            /// ENDOF ICECROWN CITADEL SPELLS
            //////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////////
            /// SHADOWFANG KEEP SPELLS
            /// Baron Ashbury
            case 93720: ///< Wracking Pain
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(28);
                spellInfo->Effects[EFFECT_1].SetRadiusIndex(28);
                break;
            case 93810: ///< Calamity dmg
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(28);
                spellInfo->Effects[EFFECT_1].SetRadiusIndex(28);
                break;
            case 93423: ///< Asphyxiate
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(28);
                spellInfo->Effects[EFFECT_1].SetRadiusIndex(28);
                spellInfo->Effects[EFFECT_2].SetRadiusIndex(28);
                break;
            case 93468: ///< Stay of Execution
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(28);
                break;
            case 93706: ///< Stay of Execution t
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(28);
                spellInfo->Effects[EFFECT_1].SetRadiusIndex(28);
                break;
            /// Baron Silverlaine
            case 93956: ///< Cursed Veil
                spellInfo->Effects[EFFECT_1].SetRadiusIndex(18);
                break;
            /// Commander Springvale
            case 93722: ///< Shield of Perfidious dmg
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CONE_ENEMY_24;
                break;
            /// Lord Valden
            case 93697: ///< Conjure Poisonous Mixture
                spellInfo->Effects[EFFECT_1].SetRadiusIndex(18);
                spellInfo->Effects[EFFECT_2].SetRadiusIndex(18);
                break;
            case 93505: ///< Conjure Frost Mixture
                spellInfo->Effects[EFFECT_1].SetRadiusIndex(18);
                break;
            case 93617: ///< Toxic Coagulant dmg
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(12);
                spellInfo->Effects[EFFECT_1].SetRadiusIndex(12);
                spellInfo->Effects[EFFECT_2].SetRadiusIndex(12);
                break;
            case 93689: ///< Toxic Catalyst dmg
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(12);
                spellInfo->Effects[EFFECT_1].SetRadiusIndex(12);
                spellInfo->Effects[EFFECT_2].SetRadiusIndex(12);
                break;
            /// Lord Godfrey
            case 96344: ///< Pistol Barrage sum
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(12);
                break;
            /// ENDOF SHADOWFANG SPELLS
            //////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////////
            /// DEADMINES SPELLS
            /// Admiral Ripsnarl
            case 88736: ///< Taste for Blood
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                spellInfo->ProcChance = 0;
                spellInfo->ProcFlags = 0;
                break;
            case 95647: ///< Ripsnarl Achievement Aura
                spellInfo->AttributesEx3 = SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                break;
            /// Captain Cookie
            case 89250: ///< Summon Cauldron
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                break;
            case 175216: ///< Savage Feast
                spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_TRIGGER_SPELL;
                break;
            case 175217: ///< Savage Feast
                spellInfo->Effects[EFFECT_1].Effect = SPELL_EFFECT_TRIGGER_SPELL;
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[EFFECT_1].TriggerSpell = 160598;
                spellInfo->Effects[EFFECT_2].Effect = SPELL_EFFECT_TRIGGER_SPELL;
                spellInfo->Effects[EFFECT_2].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[EFFECT_2].TriggerSpell = 160599;
                break;
            case 89268: ///< Throw Food Targeting
            case 89740:
            case 90561:
            case 90562:
            case 90582:
            case 90583:
            case 90563:
            case 90584:
            case 90564:
            case 90585:
            case 90565:
            case 90586:
                spellInfo->MaxAffectedTargets = 1;
                spellInfo->AttributesEx3 = SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                break;
            /// Vanessa Vancleef
            case 92620: ///< Backslash targeting
                spellInfo->MaxAffectedTargets =1;
                break;
            /// ENDOF DEADMINES
            //////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////////
            /// ULDUAR SPELLS
            ///
            case 64014: ///< Expedition Base Camp Teleport
            case 64032: ///< Formation Grounds Teleport
            case 64028: ///< Colossal Forge Teleport
            case 64031: ///< Scrapyard Teleport
            case 64030: ///< Antechamber Teleport
            case 64029: ///< Shattered Walkway Teleport
            case 64024: ///< Conservatory Teleport
            case 64025: ///< Halls of Invention Teleport
            case 64027: ///< Descent into Madness Teleport
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DB;
                break;
            case 62374: ///< Pursued (Flame Leviathan)
                spellInfo->Effects[0].SetRadiusIndex(EFFECT_RADIUS_50000_YARDS);   ///< 50000yd
                break;
            case 62383: ///< Shatter (Ignis)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_IGNORE_ARMOR;
                break;
            case 63342: ///< Focused Eyebeam Summon Trigger (Kologarn)
                spellInfo->MaxAffectedTargets = 1;
                break;
            case 63018: ///< Searing Light (XT-002)
            case 65121: ///< Searing Light (25m) (XT-002)
            case 63024: ///< Gravity Bomb (XT-002)
            case 64234: ///< Gravity Bomb (25m) (XT-002)
                spellInfo->MaxAffectedTargets = 1;
                break;
            case 62834: ///< Boom (XT-002)
            /// This hack is here because we suspect our implementation of spell effect execution on targets
            /// is done in the wrong order. We suspect that EFFECT_0 needs to be applied on all targets,
            /// then EFFECT_1, etc - instead of applying each effect on target1, then target2, etc.
            /// The above situation causes the visual for this spell to be bugged, so we remove the instakill
            /// effect and implement a script hack for that.
                spellInfo->Effects[EFFECT_1].Effect = 0;
                break;
            case 64386: ///< Terrifying Screech (Auriaya)
            case 64389: ///< Sentinel Blast (Auriaya)
            case 64678: ///< Sentinel Blast (Auriaya)
                spellInfo->SetDurationIndex(28); ///< 5 seconds, wrong DB2 data?
                break;
            case 64321: ///< Potent Pheromones (Freya)
                /// spell should dispel area aura, but doesn't have the attribute
                /// may be db data bug, or blizz may keep reapplying area auras every update with checking immunity
                /// that will be clear if we get more spells with problem like this
                spellInfo->AttributesEx |= SPELL_ATTR1_DISPEL_AURAS_ON_IMMUNITY;
                break;
            case 62301: ///< Cosmic Smash (Algalon the Observer)
                spellInfo->MaxAffectedTargets = 1;
                break;
            case 64598: ///< Cosmic Smash (Algalon the Observer)
                spellInfo->MaxAffectedTargets = 3;
                break;
            case 62293: ///< Cosmic Smash (Algalon the Observer)
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_DEST_CASTER;
                break;
            case 62311: ///< Cosmic Smash (Algalon the Observer)
            case 64596: ///< Cosmic Smash (Algalon the Observer)
                spellInfo->SetRangeIndex(6);  ///< 100yd
                break;
            /// ENDOF ULDUAR SPELLS
            //////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////////
            ///
            case 108194: ///< Asphyxiate
            case 221562: ///< Asphyxiate
                spellInfo->SchoolMask = (SPELL_SCHOOL_MASK_SHADOW | SPELL_SCHOOL_MASK_NORMAL);
                break;
            case 114255: ///< Surge of Light (proc)
                spellInfo->StackAmount = 2;
                break;
            case 1543: ///< Flare
                spellInfo->Effects[0].TriggerSpell = 132950;
                spellInfo->ProcChance = 100;
                break;
            case 109772: ///< Flare
                spellInfo->SetDurationIndex(18); ///< 20s Duration
                spellInfo->AttributesEx3 |= SPELL_ATTR3_IGNORE_HIT_RESULT;
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                break;
            case 123011: ///< Terrorize Player (tsulong spell)
                spellInfo->MaxAffectedTargets = 1;
                break;
            case 117866: ///< Champion of Light (Sha of Fear)
            case 90047:  ///< Renegade Strength
                spellInfo->AuraInterruptFlags |= AURA_INTERRUPT_FLAG_CHANGE_MAP;
                break;
            case 152118: ///< Clarity of Will
                spellInfo->InterruptFlags = 0x0000000F;
                break;
            case 44425: ///< Arcane barrage
                spellInfo->AttributesEx6 &= ~SPELL_ATTR6_CANT_TARGET_CROWD_CONTROLLED;
                break;
            case 125050: ///< Fetch
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            case 77472: ///< Healing Wave
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ALLY;
                break;
            case 145153: ///< Dream of Cenarius (Heal from Wrath)
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ALLY;
                spellInfo->Effects[0].TargetB = 0;
                break;
            case 171253: ///< Garrison heartstone
                spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_DUMMY;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                spellInfo->Effects[EFFECT_0].ItemType = 0;
                break;
            case 124280:///< Touch of Karma (DoT)
                spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_DONE_BONUS;
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_TRIGGERED_IGNORE_RESILENCE;
                spellInfo->AttributesEx |= SPELL_ATTR1_CANT_BE_REDIRECTED;
                spellInfo->AttributesEx |= SPELL_ATTR1_CANT_BE_REFLECTED;
                break;
            case 113344:///< Bloodbath (DoT)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_TRIGGERED_IGNORE_RESILENCE;
                break;
            case 148022:///< Icicle hit
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_TRIGGERED_IGNORE_RESILENCE;
                spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_DONE_BONUS;
                break;
            case 51657: ///< Taunt Flag
                spellInfo->Effects[SpellEffIndex::EFFECT_0].TargetA = Targets::TARGET_DEST_DEST;
                spellInfo->Effects[SpellEffIndex::EFFECT_1].Effect = 0;
                break;
            case 81297: ///< Consecration Damages
            case 81298: ///< Consecration Visual
                spellInfo->Effects[SpellEffIndex::EFFECT_0].TargetA = Targets::TARGET_DEST_DEST;
                break;
            case 84721: ///< Frozen Orb damage
                spellInfo->AttributesEx2 |= SpellAttr2::SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                break;
            case 174556: ///< Meteor
                spellInfo->Effects[0].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[0].TargetB = 0;
                break;
            case 91809: ///< Leap
                spellInfo->Effects[EFFECT_1].ValueMultiplier = 0;
                break;
            case 217616: ///< Nightwell Font - Missile
                spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(143);
                break;
            case 91802: ///< Shambling Rush
                spellInfo->Effects[EFFECT_0].ValueMultiplier = 0;
                spellInfo->Effects[EFFECT_1].Effect = SPELL_EFFECT_DUMMY;
                spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(4); ///< Min: 0, Max: 30
                break;
            case 143333: ///< Water walking aura
                spellInfo->AuraInterruptFlags |= AURA_INTERRUPT_FLAG_NOT_MOUNTED;
                break;
            case 89792: ///< Flee
                spellInfo->Effects[SpellEffIndex::EFFECT_0].TargetB = Targets::TARGET_UNIT_SUMMONER;
                spellInfo->Effects[SpellEffIndex::EFFECT_1].TargetB = Targets::TARGET_UNIT_SUMMONER;
                break;
            case 110310: ///< Dampening
                spellInfo->Effects[SpellEffIndex::EFFECT_1].Amplitude = 10000;  ///< 10 secs
                break;
            case 108415: ///< Soul Link
            case 108446: ///< Soul Link
                spellInfo->Attributes &= ~SPELL_ATTR0_NOT_SHAPESHIFT;
                spellInfo->AttributesEx3 &= ~SPELL_ATTR3_CANT_TRIGGER_PROC;
                spellInfo->AttributesEx3 |= SPELL_ATTR3_CAN_PROC_WITH_TRIGGERED;
                break;
            case 34709: ///< Shadow Sight
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;

            //////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////////////////
            /// This file is a mess please order all new or modified attr in category
            //////////////////////////////////////////////////////////////////////////////////////////////
            case 134735: ///< PvP Rules Enabled
                spellInfo->SetDurationIndex(21);
                break;
            case 195710: ///< Honorable Medallion
                spellInfo->OverrideSpellList.push_back(208683); ///< Gladiator's Medallion
                spellInfo->OverrideSpellList.push_back(196029); ///< Relentless
                break;
            case 60352:
                spellInfo->SetRangeIndex(13); ///< Anywhere (0y-50000y)
                break;
            case 236374: ///< Norgannon's Foresight (moving)
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_PERIODIC_DUMMY;
                spellInfo->Effects[EFFECT_0].Amplitude = 250;
                break;
            case 222715: ///< Poisoned Dreams (spread)
                spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(14); ///< Nearby (8y)
                break;
            //////////////////////////////////////////////////////////////////////////////////////////////
            /// Death Knight
            //////////////////////////////////////////////////////////////////////////////////////////////
            case 190778: /// Sindragosa's Fury
                spellInfo->SetDurationIndex(65);
                break;
            case 47482: /// DK Ghoul pet 'Leap'
            case 91807: /// DK Ghoul pet 'Shambling Rush'
                spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(4); ///< Min: 0, Max: 30
                break;
            case 196770: ///< Remorseless Winter
                spellInfo->AttributesCu &= ~SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 55233:
                spellInfo->Effects[EFFECT_1].ApplyAuraName = SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT;
                break;
            case 194912: ///< Gathering Storm
                spellInfo->ProcFlags = 0;
                break;
            case 195181: ///< Bone Shield
                spellInfo->Effects[EFFECT_3].m_CanBeStacked = false;
                break;
            case 51462: ///< Runic Corruption
                spellInfo->ProcChance = 100;
                spellInfo->ProcFlags = 0x00010010;
                break;
            case 206962: ///< Tremble Before Me
                spellInfo->Effects[EFFECT_0].Mechanic = MECHANIC_NONE;
                break;
            case 51963: ///< Gargoyle Strike
                spellInfo->InterruptFlags |= SpellInterruptFlags::SPELL_INTERRUPT_FLAG_INTERRUPT;
                spellInfo->PreventionType |= SpellPreventionMask::Silence;
                break;
            case 198715: ///< Val'kyr Strike
                spellInfo->InterruptFlags |= SpellInterruptFlags::SPELL_INTERRUPT_FLAG_INTERRUPT;
                spellInfo->PreventionType |= SpellPreventionMask::Silence;
                break;
            case 91778: ///< Sweeping Claw
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_IGNORE_ARMOR;
                break;
            case 207260: ///< Arrow Spray
                spellInfo->Effects[SpellEffIndex::EFFECT_1].TargetA = TARGET_UNIT_DEST_AREA_ENEMY;
                break;
            case 207319: ///< Corpse Shield
                spellInfo->AttributesEx6 |= SPELL_ATTR6_NOT_LIMIT_ABSORB;
                break;
            case 191592:    ///< Runic Tattoos
            case 191593:    ///< Runic Tattoos
            case 191594:    ///< Runic Tattoos
            case 224466:    ///< Runic Tattoos
            case 224467:    ///< Runic Tattoos
                spellInfo->AttributesEx |= (SPELL_ATTR0_HIDDEN_CLIENTSIDE);
                break;
            case 51399: ///< Death Grip Taunt
                spellInfo->Effects[EFFECT_0].BasePoints *= 2;
                break;
            case 238115: ///< Thronebreaker
                spellInfo->Effects[EFFECT_0].TriggerSpell = 0;;
                break;
            case 145629: ///< Anti-Magic Zone
                spellInfo->SetDurationIndex(1);
                break;
            case 253594: ///< Inexorable Assault
                spellInfo->AttributesEx3 |= SpellAttr3::SPELL_ATTR3_DEATH_PERSISTENT;
                break;
            case 211042: ///< Item - Death Knight T19 Frost 2P Bonus
                spellInfo->Effects[EFFECT_0].MiscValue = SPELLMOD_EFFECT2;
                break;
            case 191494: ///< Scourge the Unbeliever
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_PROC_TRIGGER_SPELL;
                break;
            case 218321: ///< Dragged to Helheim
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_CASTER;
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_DEST_AREA_ENEMY;
                spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(14);
                spellInfo->Effects[EFFECT_1].Effect = SPELL_EFFECT_DUMMY;
                break;
            case 242010: ///< Gravewarden
                spellInfo->Effects[EFFECT_1].BasePoints = 0;
                break;
            case 212654: ///< Wraith Walk
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(27); ///< 3s
                break;
            case 228581: ///< Decomposing Aura
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_10_YARDS);
                break;
            //////////////////////////////////////////////////////////////////////////////////////////////
            /// Druid
            //////////////////////////////////////////////////////////////////////////////////////////////
            case 22842:  ///< Frenzied Regeneration
                spellInfo->StartRecoveryTime = 500;
                break;
            case 236696: ///< Thorns
                spellInfo->ProcCooldown = 1000;
                break;
            case 170856: ///< Nature's Grasp
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(31); ///< 8s
                break;
            case 210723: ///< Ashamane's Frenzy // Damage & DoT
                spellInfo->Effects[EFFECT_1].ApplyAuraName = SPELL_AURA_DUMMY; ///< doesn't have AP coeff in DBC and not used in script
                break;
            case 222270:
                spellInfo->Attributes |= SPELL_ATTR0_OUTDOORS_ONLY;
                break;
            case 1822:
                spellInfo->Effects[EFFECT_2].Effect = SPELL_EFFECT_DUMMY;
                break;
            case 203553: ///< Focused Growth
                spellInfo->Attributes |= SPELL_ATTR0_PASSIVE;
                break;
            case 202425: ///< Warrior of Elune
                spellInfo->ProcCharges = 3;
                break;
            case 202890: ///< Rapid Innervation
                spellInfo->ProcFlags = 0;
                break;
            case 235040: ///< X'oni's Caress
                spellInfo->AttributesEx3 &= ~SPELL_ATTR3_NO_DONE_BONUS;
                break;
            case 212040: ///< Revitalize
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_DEAD;
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                break;
            case 168877: ///< Maim
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(36); ///< 1s
                break;
            case 168878: ///< Maim
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(39); ///< 2s
                break;
            case 168879: ///< Maim
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(27); ///< 3s
                break;
            case 168880: ///< Maim
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(35); ///< 4s
                break;
            case 168881: ///< Maim
            case 241503: ///< Felfire Missiles (Apocron)
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(28); ///< 5s
                break;
            case 58180: ///< Infected Wounds
                spellInfo->SetRangeIndex(5); ///< Long range : 0y-40y
                break;
            case 236716: ///< Enraged Maul
                spellInfo->SpecializationIdList.push_back(SPEC_DRUID_FERAL);
                break;
            case 145152: ///< Bloodtalons
                spellInfo->Effects[SpellEffIndex::EFFECT_0].m_CanBeStacked = false;
                break;
            case 208190: ///< The Emerald Dreamcatcher
                spellInfo->ProcChance = 0;
                spellInfo->ProcFlags = 0;
                break;
            case 159286: ///< Primal Fury
                spellInfo->ProcCooldown = 0;
                spellInfo->InternalCooldown = 0;
                break;
            case 16953: ///< Primal Fury
                spellInfo->InternalCooldown = 0;
                break;
            case 26008: ///< Toast
                spellInfo->Effects[EFFECT_0].m_CanBeStacked = false;
                break;
            case 48438: ///< Wild growth
                spellInfo->Effects[EFFECT_0].BonusMultiplier = 0.3277f; /// From https://www.askmrrobot.com/wow/theory/mechanic/spell/wildgrowth?spec=DruidRestoration&version=7_1_5_23420
                break;
            case 203018: ///< Touch of the Moon
                spellInfo->Effects[EFFECT_1].ApplyAuraName = SPELL_AURA_DUMMY;
                break;
            case 236068: ///< Moment Of Clarity
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                break;
            case 202739: ///< Blessing of An'she
                spellInfo->Effects[EFFECT_0].Amplitude = 2600;
                break;
            case 240670: ///< Fury Of Ashamane
                spellInfo->Effects[EFFECT_0].m_CanBeStacked = false;
                break;
            case 189870: ///< Power of the Archdruid
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                break;
            case 210686: /// Shadow Trash
                spellInfo->Effects[EFFECT_0].Amplitude = 1200;
                break;
            case 238086: ///< Joy of Spring
                spellInfo->Effects[SpellEffIndex::EFFECT_0].ApplyAuraName = AuraType::SPELL_AURA_DUMMY;
                break;
            case 203406: ///< Revitalize
                spellInfo->DmgClass = SPELL_DAMAGE_CLASS_MAGIC;
                break;
            case 135700: ///< Clearcasting
                spellInfo->StackAmount = 2;
                spellInfo->ProcCharges = 0;
                break;
            case 210676: ///< Shadow Thrash
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                spellInfo->ProcFlags = 0;
                break;
            case 189800: ///< Nature's Essence
                spellInfo->DmgClass = SPELL_DAMAGE_CLASS_MAGIC;
                break;
            //////////////////////////////////////////////////////////////////////////////////////////////
            /// Hunter
            //////////////////////////////////////////////////////////////////////////////////////////////
            case 3355: ///< Freezing Trap
                spellInfo->AttributesEx &= ~SPELL_ATTR1_NOT_BREAK_STEALTH;
                spellInfo->AttributesEx3 |= SPELL_ATTR3_CANT_TRIGGER_PROC;
                spellInfo->AttributesEx6 |= SPELL_ATTR6_CAN_TARGET_INVISIBLE;
                break;
            case 212329: ///< Apex Predator Legendary ring
                spellInfo->AttributesEx8 |= SpellAttr8::SPELL_ATTR8_NOT_IN_BG_OR_ARENA;
                break;
            case 118455: ///< Beast Cleave
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ALLY;
                break;
            case 211181: ///< Item - Hunter T19 Beast Mastery 4P Bonus
                spellInfo->ProcFlags = 0;
                spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                break;
            case 211183: ///< Item - Hunter T19 Beast Mastery 4P Bonus - Proc
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ALLY;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                spellInfo->Effects[EFFECT_1].Effect = 0;
                break;
            case 201142: ///< Frozen Wake (Expert Trapper Freezing Trap)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_TARGET_ENEMY;
                break;
            case 13812: ///< Explosive Trap
                spellInfo->AttributesEx &= ~SPELL_ATTR1_NOT_BREAK_STEALTH;
                spellInfo->AttributesEx6 |= SPELL_ATTR6_CAN_TARGET_INVISIBLE;
                break;
            case 195638: ///< Focussed Fire Honor Talent buff (7.1.5 - Build 23420: buff wasnt lost on leaving arena or BG)
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
                spellInfo->m_IsPVPTalent = true;
                break;
            case 213543:
                spellInfo->AttributesEx3 |= SPELL_ATTR3_CAN_PROC_WITH_TRIGGERED;
                break;
            case 156843:
                spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_DUMMY;
                spellInfo->Effects[EFFECT_1].Effect = SPELL_EFFECT_DUMMY;
                break;
            case 193468: ///< Mastery: Sniper Training
                /// Add Sniper Shot  to damage modifier
                spellInfo->Effects[EFFECT_1].SpellClassMask[0] |= 0x20000000;
                spellInfo->Effects[EFFECT_1].SpellClassMask[2] |= 0x00000400;
                break;
            case 213691: ///< Scatter Shot - Should break on any damage taken
                spellInfo->AuraInterruptFlags = 0x00480002;
                break;
            case 34026: ///< Kill Command
                spellInfo->CasterAuraSpell = 0;
                break;
            case 207280: ///< Roar of the Seven Lions
                spellInfo->ProcFlags &= ~PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS;
                spellInfo->ProcFlags &= ~PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_NEG;
                break;
            case 199921: ///< Trailblazer
                spellInfo->Effects[EFFECT_0].Amplitude = spellInfo->Effects[EFFECT_0].BasePoints;
                break;
            case 206817: /// < Sentinel
                spellInfo->Speed = 0.0f;
                break;
            case 135299: ///< Tar Trap (snare)
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(285); ///< 1s
                spellInfo->AttributesEx5 |= SPELL_ATTR5_HIDE_DURATION;
                break;
            case 214579: ///< Sidewinders
                spellInfo->Effects[2].TargetB = TARGET_UNIT_CONE_ENEMY_54;
                break;
            case 203924: ///< Healing Sell
            case 197161: ///< Mimiron's Shell
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CANT_CRIT;
                break;
            case 217200: ///< Dire Frenzy
                spellInfo->Effects[EFFECT_2].Effect = SpellEffects::SPELL_EFFECT_APPLY_AURA;
                spellInfo->Effects[EFFECT_2].ApplyAuraName = AuraType::SPELL_AURA_MOD_SPEED_ALWAYS;
                spellInfo->Effects[EFFECT_2].BasePoints = 30;
                break;
            case 126393: ///< Eternal Guardian
            case 159931: ///< Gift Of Chi
            case 159956: ///< Dust Of Life
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ALLY;
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_DEAD;
                break;
            case 201075: ///< Mortal Wounds
                spellInfo->ProcChance = 2;
                break;
            case 199523: ///< Farstrider
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                spellInfo->AttributesEx3 &= ~SPELL_ATTR3_CAN_PROC_WITH_TRIGGERED;
                break;
            case 240711: ///< Sidewinders
                spellInfo->Effects._effects.push_back(SpellEffectInfo(sSpellStore.LookupEntry(spellInfo->Id), spellInfo, EFFECT_1, Difficulty::DifficultyNone));
                spellInfo->Effects[EFFECT_1].Effect = SpellEffects::SPELL_EFFECT_ENERGIZE;
                spellInfo->Effects[EFFECT_1].BasePoints = 35;
                spellInfo->Effects[EFFECT_1].MiscValue = 2;
                spellInfo->Effects._effects.push_back(SpellEffectInfo(sSpellStore.LookupEntry(spellInfo->Id), spellInfo, EFFECT_2, Difficulty::DifficultyNone));
                spellInfo->Effects[EFFECT_2].Effect = SpellEffects::SPELL_EFFECT_DUMMY;
                spellInfo->Effects[EFFECT_2].TargetA = TARGET_DEST_CASTER;
                spellInfo->Effects[EFFECT_2].TargetB = TARGET_UNIT_CONE_ENEMY_54;
                spellInfo->Effects[EFFECT_2].RadiusEntry = sSpellRadiusStore.LookupEntry(48);
                break;
            case 240152: ///< Flanking Strike - Enables spell client side
                spellInfo->Attributes |= SPELL_ATTR0_PASSIVE;
                break;
            case 223138: ///< Marking Targets
                spellInfo->ProcFlags = 0;
                break;
            case 186387: ///< Bursting Shot
                spellInfo->OverrideSpellList.push_back(213691); ///< Scatter Shot
                break;
            case 218955: ///< Fluffy, Go (pet aura)
                spellInfo->Attributes |= SPELL_ATTR0_PASSIVE;
            //////////////////////////////////////////////////////////////////////////////////////////////
            /// Mage
            //////////////////////////////////////////////////////////////////////////////////////////////
            case 86949: ///< Cauterize
                spellInfo->Effects[EFFECT_2].Effect = SPELL_EFFECT_NONE;
                break;
            case 87023: ///< Cauterize (Burn)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_TRIGGERED_IGNORE_RESILENCE;
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CANT_CRIT;
                break;
            case 87024: ///< Cauterize (Marker)
                spellInfo->AttributesEx3 &= ~SpellAttr3::SPELL_ATTR3_DEATH_PERSISTENT;
                break;
            case 12472: ///< Icy Veins
                spellInfo->OverrideSpellList.push_back(198144); ///< Ice Form
                break;
            case 205021: ///< Ray of Frost
                spellInfo->AttributesEx5 &= ~SpellAttr5::SPELL_ATTR5_HASTE_AFFECT_DURATION;
                break;
            case 198120: ///< Frostbite
                spellInfo->AttributesEx3 |= SPELL_ATTR3_CAN_PROC_WITH_TRIGGERED;
                spellInfo->ProcFlags |= PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_NEG;
                break;
            case 190357: ///< Blizzard damage
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            case 205472: ///< Flame Patch
                spellInfo->Effects[0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                spellInfo->Effects[0].TargetB = 0;
                break;
            case 205029: ///< Flame On
                // Hide an incorrect effect (but it's shown by the client)
                spellInfo->Effects[SpellEffIndex::EFFECT_1].ApplyAuraName = AuraType::SPELL_AURA_DUMMY;
                spellInfo->Effects[SpellEffIndex::EFFECT_1].BasePoints = -17; ///< Be careful, this magic number is used in SpellInfo::GetFloatAmount
                // Create a correct effect
                spellInfo->Effects[SpellEffIndex::EFFECT_2].Effect = SpellEffects::SPELL_EFFECT_APPLY_AURA;
                spellInfo->Effects[SpellEffIndex::EFFECT_2].ApplyAuraName = AuraType::SPELL_AURA_CHARGE_RECOVERY_MOD;
                spellInfo->Effects[SpellEffIndex::EFFECT_2].MiscValue = spellInfo->Effects[SpellEffIndex::EFFECT_1].MiscValue;
                spellInfo->Effects[SpellEffIndex::EFFECT_2].BasePoints = -2000;
                break;
            case 187131: ///< Vulnerable
                spellInfo->Effects[3].Amplitude = 800;
                break;
            case 56377: ///< Splitting Ice
                spellInfo->Effects[SpellEffIndex::EFFECT_0].SpellClassMask[3] &= ~0x00080000; /// Remove Icicle mask, it's handled in a spellscript
                break;
            case 11426: ///< Ice Barrier
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CANT_CRIT;
                break;
            case 212799: ///< Displacement Beacon
                spellInfo->AuraInterruptFlags &= ~(AURA_INTERRUPT_FLAG_CHANGE_MAP | AURA_INTERRUPT_FLAG_ENTER_PVP_COMBAT);
                break;
            case 198144: ///< Ice Form
                spellInfo->Effects._effects.push_back(SpellEffectInfo(sSpellStore.LookupEntry(spellInfo->Id), spellInfo, EFFECT_3, Difficulty::DifficultyNone));
                spellInfo->Effects[EFFECT_3].Effect = SpellEffects::SPELL_EFFECT_APPLY_AURA;
                spellInfo->Effects[EFFECT_3].ApplyAuraName = AuraType::SPELL_AURA_MECHANIC_IMMUNITY_MASK;
                break;
            case 116011: ///< Rune of Power
                /// Increase range up to 50 yards
                spellInfo->SetRangeIndex(37);
                break;
            case 242251: ///< Critical Massive
                spellInfo->ProcCharges = 1;
                break;
            case 30451: ///< Arcane Blast
                spellInfo->Effects[EFFECT_1].Effect = SPELL_EFFECT_DUMMY;
                break;
            case 238091: ///< Warmth of the Phoenix
                spellInfo->ProcFlags = 0;
                break;
            case 198151: ///< Torment of Weak
                spellInfo->ProcFlags |= (PROC_FLAG_DONE_PERIODIC | PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_NEG | PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_NEG);
                spellInfo->ProcChance = 100.f;
                spellInfo->ProcCharges = 0;
                spellInfo->ProcCooldown = 0;
                break;
            //////////////////////////////////////////////////////////////////////////////////////////////
            /// Monk
            //////////////////////////////////////////////////////////////////////////////////////////////
            case 199668: ///< Blessings of Yu'lon
            case 115310: ///< Revival
                spellInfo->Attributes |= SPELL_ATTR1_NO_THREAT;
                spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_INITIAL_AGGRO;
                break;
            case 228287: ///< Mark of the crane
                spellInfo->AttributesEx3 |= SPELL_ATTR3_DEATH_PERSISTENT;
                break;
            case 213664: ///< Nimble Brew
                spellInfo->AttributesEx5 |= SPELL_ATTR5_USABLE_WHILE_STUNNED;
                break;
            case 115078: ///< Paralysis
                spellInfo->AttributesEx3 &= ~SPELL_ATTR3_CANT_TRIGGER_PROC;
                break;
            case 137025: ///< Windwalker Monk Aura
                spellInfo->Effects[EFFECT_0].BasePoints = 9; ///< https://www.mmo-champion.com/content/7000-Patch-7-3-2-PTR-Build-25079
                spellInfo->Effects[EFFECT_1].BasePoints = 9;
                    break;
            case 196730: ///< Special Delivery
                spellInfo->AttributesEx3 |= SPELL_ATTR3_CAN_PROC_WITH_TRIGGERED;
                break;
            case 216161: ///< Way of the crane healing is based on damage dealt, so shouldn't be modified by buffs / debuffs
                spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_DONE_BONUS;
                break;
            case 196734: ///< Special Delivery
                spellInfo->AttributesEx9 |= SPELL_ATTR9_SPECIAL_DELAY_CALCULATION;
                break;
            case 209525: ///< Soothing Mist (from Honor Talent 209520: Ancient Mistweaver Arts)
                spellInfo->m_IsPVPTalent = true;
                spellInfo->Effects._effects.push_back(SpellEffectInfo(sSpellStore.LookupEntry(spellInfo->Id), spellInfo, EFFECT_2, Difficulty::DifficultyNone));
                spellInfo->Effects[EFFECT_2].Effect = SpellEffects::SPELL_EFFECT_APPLY_AURA;
                spellInfo->Effects[EFFECT_2].ApplyAuraName = AuraType::SPELL_AURA_ADD_PCT_MODIFIER;
                spellInfo->Effects[EFFECT_2].TargetA = Targets::TARGET_UNIT_CASTER;
                spellInfo->Effects[EFFECT_2].MiscValue = SpellModOp::SPELLMOD_CASTING_TIME;
                spellInfo->Effects[EFFECT_2].BasePoints = -1000;
                spellInfo->Effects[EFFECT_2].SpellClassMask = spellInfo->Effects[EFFECT_1].SpellClassMask;
                break;
            case 113656: ///< Fists of Fury
                spellInfo->Effects[0].Effect = SPELL_EFFECT_APPLY_AURA;
                spellInfo->Effects[1].Amplitude = 1000;
                spellInfo->Effects[2].Amplitude = 1000;
                spellInfo->Effects[2].ApplyAuraName = SPELL_AURA_PERIODIC_TRIGGER_SPELL;
                spellInfo->Effects[4].Effect = 0;
                break;
            case 107428: ///< Rising sun Kick
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 122470: ///< Touch of karma
                spellInfo->AttributesEx6 &= ~SPELL_ATTR6_NOT_LIMIT_ABSORB;
                break;
            case 229980: /// Touch of Death (damages)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_IGNORE_ARMOR;
                break;
            case 212051: ///< Reawaken
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_DEAD;
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                break;
            case 233954: ///< Control the Mists
                // Remove procs, it is handled manually
                spellInfo->ProcCharges = 0;
                spellInfo->ProcFlags = 0;
                break;
            case 233766: ///< Control the Mists
                spellInfo->ProcCooldown = 0;
                break;
            case 216519: /// < Celestial Fortune
                spellInfo->ProcFlags |= PROC_FLAG_TAKEN_PERIODIC;
                break;
            case 213063: ///< Dark Side of the Moon
                spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(187);  ///< 300y
                break;
                /// 6 ranks of Dark Side of the Moon (Aura Proc): they shouldnt give a shield
            case 213062:
            case 227688:
            case 227689:
            case 227690:
            case 227691:
            case 227692:
                spellInfo->Effects[EFFECT_1].BasePoints = 0;
                spellInfo->Effects[EFFECT_1].ApplyAuraName = AuraType::SPELL_AURA_DUMMY;
                break;
            case 116847: ///< Rushing Jade Wind
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                break;
            case 163317: ///< Rushing Jade Wind
                spellInfo->Attributes = 0x00050010;
                spellInfo->AttributesEx5 = 0x00000200;
                spellInfo->AttributesEx8 = 0x00420000;
                spellInfo->AttributesEx11 = 0x00000010;
                spellInfo->AttributesEx13 = 0x00100001;
                spellInfo->SchoolMask = SPELL_SCHOOL_MASK_NATURE;
                spellInfo->DmgClass = SPELL_DAMAGE_CLASS_MELEE;
                spellInfo->PreventionType = 5;
                break;
            case 238095: ///< Master of Combinations
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_PROC_TRIGGER_SPELL;
                spellInfo->Effects[EFFECT_0].TriggerSpell = 240672;
                break;
            case 124081: ///< Zen Pulse
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ALLY;
                break;
            case 198898: ///< Song of Chi-Ji
                spellInfo->InterruptFlags |= SPELL_INTERRUPT_FLAG_INTERRUPT;
                break;
            case 191837: ///< Essence Font
                spellInfo->ChannelInterruptFlags |= CHANNEL_INTERRUPT_FLAG_INTERRUPT;
                break;
            case 132578: ///< Invoke Niuzao, the Black Ox
                spellInfo->Effects[EFFECT_0].MiscValueB = 3262;
                break;
            case 148135: ///< Chi Burst (damage)
                spellInfo->Effects[EFFECT_0].AttackPowerMultiplier = 4.125f;
                break;
            case 123986: ///< Chi Burst
                spellInfo->InterruptFlags |= SPELL_INTERRUPT_FLAG_INTERRUPT;
                break;
            //////////////////////////////////////////////////////////////////////////////////////////////
            /// Paladin
            //////////////////////////////////////////////////////////////////////////////////////////////
            case 199443: ///< Holy Pala Honor talent 'Avenging Light'
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                break;
            case 53563:  ///< Beacon of Light
                spellInfo->Effects[EFFECT_0].Effect = 0;
                spellInfo->Effects[EFFECT_1].Effect = 0;
                break;
            case 217020: ///< Zeal
                spellInfo->StackAmount = 3;
                break;
            case 223817: ///< Divine Purpose
                spellInfo->ProcFlags = 0;
                break;
            case 212056: ///< Absolution
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_DEAD;
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                break;
            case 115675: ///< Boundless Conviction
                spellInfo->SpecializationIdList.push_back(SPEC_PALADIN_RETRIBUTION);
                break;
            case 200373:
                spellInfo->SpellFamilyFlags[2] = 0x00000000;
                break;
            case 209202: ///< Eye of Tyr
                spellInfo->Effects[SpellEffIndex::EFFECT_2].SetRadiusIndex(spellInfo->Effects[SpellEffIndex::EFFECT_1].RadiusEntry->ID);
                break;
            case 234299: ///< Fist of Justice
                spellInfo->ProcCooldown = 0;
                break;
            case 210380: ///< Aura of Sacrifice
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CANT_CRIT;
                break;
            case 216371: ///< Avenging Crusader
            case 216372: ///< Avenging Crusader
                spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_DONE_BONUS;
                break;
            case 174333: ///< Divine Storm
            {
                SpellInfo const* l_SpellInfo = GetSpellInfo(53385);
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(l_SpellInfo->Effects[EFFECT_0].RadiusEntry->ID);
                break;
            }
            case 132403: ///< Shield of the Righteous
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(555); ///< 4500, Max 4500
                break;
            case 238996: ///< Righteous Verdict
                spellInfo->ProcCharges = 0;
                break;
            case 31850: ///< Ardent Defender
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                spellInfo->Effects._effects.push_back(SpellEffectInfo(sSpellStore.LookupEntry(spellInfo->Id), spellInfo, EFFECT_2, Difficulty::DifficultyNone));
                spellInfo->Effects[EFFECT_2].Effect = SPELL_EFFECT_APPLY_AURA;
                spellInfo->Effects[EFFECT_2].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[EFFECT_2].BasePoints = 20;
                spellInfo->Effects[EFFECT_2].ApplyAuraName = SPELL_AURA_SCHOOL_ABSORB;
                spellInfo->Effects[EFFECT_2].MiscValue = 127;
                break;
            case 248289: ///< Scarlet Inquisitor's Expurgation
                spellInfo->AttributesEx8 |= SpellAttr8::SPELL_ATTR8_NOT_IN_BG_OR_ARENA;
                break;
            //////////////////////////////////////////////////////////////////////////////////////////////
            /// Priest
            //////////////////////////////////////////////////////////////////////////////////////////////
            case 194249: ///< VoidForm
            {
                for (uint8 l_EffectIndex = EFFECT_0; l_EffectIndex < MAX_EFFECTS; ++l_EffectIndex)
                    spellInfo->Effects[l_EffectIndex].m_CanBeStacked = l_EffectIndex == EFFECT_2 ? true : false;
                spellInfo->Effects[EFFECT_9].m_AmountNotAlterByStacks = true;
                break;
            }
            case 252909: ///< Priest T21 Shadow 4P is hackfixed in Unit::GetUnitSpellCriticalChance for Mind Flay, Void Bolt and Mind Blast
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                break;
            case 243241: ///< Cosmic Ripple
                spellInfo->MaxAffectedTargets = 5;
                break;
            case 47788:  ///< Guardian Spirit
                spellInfo->AttributesEx5 |= SPELL_ATTR5_USABLE_WHILE_STUNNED;
                break;
            case 32592:  ///< Mass Dispel
                spellInfo->Attributes |= SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY;
                spellInfo->AttributesEx6 |= SPELL_ATTR6_CAN_TARGET_INVISIBLE;
                break;
            case 194384: ///< Atonement
                spellInfo->Effects[EFFECT_1].SpellClassMask[0] |= 0x1;
                break;
            case 199845: ///< Psyfiend
            {
                for (uint8 l_EffectIndex = EFFECT_0; l_EffectIndex < EFFECT_3; ++l_EffectIndex)
                {
                    spellInfo->Effects[l_EffectIndex].TargetA = TARGET_UNIT_TARGET_ENEMY;
                    spellInfo->Effects[l_EffectIndex].TargetB = 0;
                }
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_PERIODIC_DAMAGE_PERCENT;
                break;
            }
            case 586: ///< Fade
                spellInfo->OverrideSpellList.push_back(213602); ///< Greater Fade
                break;
            case 211681: ///< Power Word: Fortitude
                spellInfo->AttributesEx5 &= ~SPELL_ATTR5_SINGLE_TARGET_SPELL;
                break;
            case 196812: ///< Light Eruption (T'uure)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_CASTER;
                break;
            case 212036: ///< Mass Resurrection
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_DEAD;
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                break;
            case 47666: /// < Penance Damage
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                spellInfo->FacingCasterFlags |= SPELL_FACING_FLAG_INFRONT;
                break;
            case 47750: /// < Penance Heal
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ALLY;
                spellInfo->ChannelInterruptFlags |= CHANNEL_INTERRUPT_FLAG_INTERRUPT;
                spellInfo->AttributesEx5 |= SPELL_ATTR5_USABLE_WHILE_MOVING;
                break;
            case 47757: /// < Penance Heal source
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ALLY;
                spellInfo->ChannelInterruptFlags |= CHANNEL_INTERRUPT_FLAG_INTERRUPT;
                spellInfo->AttributesEx5 |= SPELL_ATTR5_USABLE_WHILE_MOVING;
                break;
            case 47758: /// < Penance Damage source
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ENEMY;
                spellInfo->ChannelInterruptFlags |= CHANNEL_INTERRUPT_FLAG_INTERRUPT;
                spellInfo->AttributesEx5 |= SPELL_ATTR5_USABLE_WHILE_MOVING;
                spellInfo->FacingCasterFlags |= SPELL_FACING_FLAG_INFRONT;
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 47540: ///< Penance source
                spellInfo->FacingCasterFlags |= SPELL_FACING_FLAG_INFRONT;
                break;
            case 207946: /// < Light's Wrath
                spellInfo->ChannelInterruptFlags |= CHANNEL_INTERRUPT_FLAG_INTERRUPT;
                spellInfo->InterruptFlags |= SPELL_INTERRUPT_FLAG_INTERRUPT;
                break;
            case 187464: ///< Shadow Mend
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CANT_CRIT;
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_PERIODIC_DAMAGE;
                spellInfo->AttributesEx4 |= SPELL_ATTR4_IGNORE_RESISTANCES;
                break;
            case 107903: /// < Shadowform visual
            case 107904: /// < Shadowform visual
                spellInfo->AuraInterruptFlags &= ~AURA_INTERRUPT_FLAG_CHANGE_MAP;
                break;
            case 223166:    ///< Overloaded With Light
                spellInfo->AuraInterruptFlags |= (AURA_INTERRUPT_FLAG_CHANGE_MAP);
                break;
            case 48153: /// < Guardian Spirit
                spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_DONE_BONUS;
                break;
            case 248296: /// < Heart Of the Void - Legendary
                spellInfo->Effects[EFFECT_0].BasePoints = 75; /// < 7.3.2 hotfix
                spellInfo->Effects[EFFECT_1].BasePoints = 25; /// < 7.3.2 hotfix
            case 193223: /// < Surrender to Madness
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_MOD_POWER_GAIN_PCT;
                break;
            case 238065:
                spellInfo->Effects[EFFECT_1].BasePoints = 1500;
                break;
            case 204263: ///< Shining Force
                spellInfo->Effects[1].TargetA = TARGET_DEST_TARGET_FRIEND;
                break;
            case 221594: /// < Xal'atath Whispers
                spellInfo->ProcChance = 15;
                break;
            case 145722: ///< Glyph of Angels
                spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                break;
            case 204065: ///< Shadow Covenant
                spellInfo->Effects[EFFECT_0].TargetA = 0;
                break;
            case 196602: ///< Divine Attendant
                spellInfo->Effects[EFFECT_0].BasePoints = -120000;
                break;
            case 240673: ///< Mind Quickening
                spellInfo->Effects[EFFECT_0].BasePoints = 800;
                break;
            case 197470: ///< Strength of Soul
                spellInfo->DmgClass = SPELL_DAMAGE_CLASS_MAGIC;
                spellInfo->SpellFamilyName = SPELLFAMILY_PRIEST;
                break;
            case 246519: ///< Penitent
            case 242269: ///< T20 Discipline 4P Bonus
                spellInfo->ProcFlags = SPELL_CAST_PROC_FLAG_MASK;
                break;
            case 196781: ///< Holy Mending
                spellInfo->DmgClass = SPELL_DAMAGE_CLASS_MAGIC;
                break;
            case 197341: ///< Ray of Hope (damage)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 211522: ///< Psyfiend
                spellInfo->PreventionType = SpellPreventionMask::Pacify;
                break;
            case 193470: ///< Call to the Void
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(38); ///< 11s
                break;
            case 262861: ///< Smite
                spellInfo->SpellFamilyFlags[3] = 0x00000000;
                break;
            case 251862: ///< Item - Mage T21 Arcane 4P Bonus
                spellInfo->ProcFlags = 0;
                spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                break;
            case 237388: ///< Mind Sear
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_TARGET_ENEMY;
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_DEST_AREA_ENEMY;
                spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(14); ///< 8y
                break;
            case 137031: ///< Holy Priest
                spellInfo->Effects[EFFECT_0].BasePoints = 0;
                break;
            //////////////////////////////////////////////////////////////////////////////////////////////
            /// Rogue
            //////////////////////////////////////////////////////////////////////////////////////////////
            case 137036: ///< Outlaw Rogue Aura https://www.mmo-champion.com/content/7000-Patch-7-3-2-PTR-Build-25079
                spellInfo->Effects[EFFECT_0].BasePoints = 6;
                spellInfo->Effects[EFFECT_1].BasePoints = 6;
                break;
            case 199804:
                spellInfo->Effects[EFFECT_2].Effect = 0;
                break;
            case 193356: ///< Broadsides
            case 199600: ///< BuriedTreasure
            case 199603: ///< JollyRoger
            case 193358: ///< GrandMelee
            case 193357: ///< SharkInfestedWaters
            case 193359: ///< TrueBearing
            case 206237: ///< Envelopping Shadows
                spellInfo->AttributesEx4 &= ~SPELL_ATTR4_HAS_DELAY;
                break;
            case 185438: ///< ShadowStrike
                spellInfo->Effects._effects.push_back(SpellEffectInfo(sSpellStore.LookupEntry(spellInfo->Id), spellInfo, EFFECT_3, Difficulty::DifficultyNone));
                spellInfo->Effects[EFFECT_3].Effect = SPELL_EFFECT_TELEPORT_UNITS;
                spellInfo->Effects[EFFECT_3].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[EFFECT_3].TargetB = TARGET_DEST_TARGET_BACK;
                spellInfo->Effects[EFFECT_3].SetRadiusIndex(7); ///< 2y
                break;
            case 197603:
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CANT_CRIT;
                break;
            case 212155: ///< THICC ASS THIEVES damage buff (7.1.5 - Build 23420 duration is 8sec on spellInfo when it should be 6sec)
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(32); ///< 6s
                break;
            case 197020: ///< Cut To The Chase
                spellInfo->Attributes |= SPELL_ATTR0_PASSIVE;
                break;
            case 197023: ///< Cut to the Chase Speed buff
                spellInfo->Effects[0].ApplyAuraName = SPELL_AURA_MOD_INCREASE_SPEED;
                break;
            case 32645: ///< Envenom
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_IGNORE_ARMOR;
                spellInfo->AttributesCu &= ~SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 79134: ///< Venomous Wounds
                spellInfo->ProcFlags &= ~PROC_FLAG_DONE_TRAP_ACTIVATION;
                break;
            case 31224: ///< Cloak of Shadows
                spellInfo->Effects[0].BasePoints = -200;
                spellInfo->Effects[0].ApplyAuraName = SPELL_AURA_MOD_ATTACKER_SPELL_HIT_CHANCE;
                spellInfo->Effects[0].ValueMultiplier = -200;
                break;
            case 137619: ///< Marked for Death
                spellInfo->AttributesEx |= SPELL_ATTR1_NO_THREAT;
                break;
            case 192660: ///< bqg of tricks DoT
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            case 130493: ///< NightStalker
                spellInfo->Effects[EFFECT_1].ApplyAuraName = SPELL_AURA_ADD_PCT_MODIFIER;
                break;
            case 211667: ///< Item - Rogue T19 Outlaw 2P Bonus
                spellInfo->SpellFamilyName = SPELLFAMILY_ROGUE;
                break;
            case 195452: ///< Nightblade
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(553); ///< 6000, Max 18000
                break;
            case 211672: ///< Mutilated Flesh
                spellInfo->AttributesEx10 &= ~SPELL_ATTR10_STACK_DAMAGE_OR_HEAL;
                spellInfo->Effects[EFFECT_0].Mechanic = MECHANIC_BLEED;
                break;
            case 98440: ///< Relentless Strikes
                spellInfo->Effects[EFFECT_0].BasePoints = 0;
                spellInfo->Effects[EFFECT_0].BonusMultiplier = 0;
                break;
            case 238139: ///< Loaded Dice
            case 240837: ///< Loaded Dice (buff)
                spellInfo->ProcFlags = 0;
                break;
            case 202628: ///< Blademaster
            case 202631: ///< Blademaster
                spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                break;
            case 208403: ///< Will of Valeera
                spellInfo->AttributesEx |= SPELL_ATTR1_NOT_BREAK_STEALTH;
                break;
            case 192431: ///< From the Shadows
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ALLY;
                break;
            case 245623: ///< Shadowstrike
                spellInfo->Effects[EFFECT_1].ApplyAuraName = SPELL_AURA_DUMMY;
                break;
            case 22482: ///< Blade Flurry
                spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(2);
                break;
            case 45181: ///< Cheated Death
                spellInfo->AttributesEx8 |= SPELL_ATTR8_CANT_MISS;
                break;
            case 185422: /// Shadow Dance
                spellInfo->Effects[EFFECT_4].BasePoints = 12;
                break;
            case 196937: ///< Ghostly Strike
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            //< For rogue's Cloak of Shadows
            case 210279: ///< Creeping Nightmares
            case 212997: ///< Carrion Plague
            case 223021: ///< Carrion Plague
            case 206480: ///< Carrion Plague
            case 206223: ///< Bonds of Fel
            case 200227: ///< Tangled Web
            case 204316: ///< Shockwave
                spellInfo->AttributesEx3 |= SPELL_ATTR3_IGNORE_HIT_RESULT;
                break;
            case 115834: ///< Shroud of Concealment
                spellInfo->AttributesEx |= SPELL_ATTR1_CANT_TARGET_IN_COMBAT;
                break;
            case 193538: ///< Alacrity
                spellInfo->AttributesEx |= SPELL_ATTR1_NOT_BREAK_STEALTH;
                break;
            //////////////////////////////////////////////////////////////////////////////////////////////
            /// Shaman
            //////////////////////////////////////////////////////////////////////////////////////////////
            case 137041: ///< Enhancement Shaman Aura https://www.wowhead.com/news=270118/7-3-ptr-build-24853-spell-updates
                spellInfo->Effects[EFFECT_0].BasePoints = 10;
                spellInfo->Effects[EFFECT_1].BasePoints = 10;
                break;
            case 187874: ///< Crash Lightning
                spellInfo->EquippedItemInventoryTypeMask |= (1 << INVTYPE_WEAPONMAINHAND);
                break;
            case 210801: ///< Crashing Storm AT Damage
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            case 77451:  ///< Lava Burst Overload
            case 45297:  ///< Chain Lightning Overload
            case 45284:  ///< Lightning Bolt Overload
            case 120588: ///< Elemental Blast Overload
            case 219271: ///< Icefury Overload
                spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_DONE_BONUS;
                break;
            case 207994: ///< Eye of the Twisting Nether
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(21); ///< -1
                break;
            case 195222: ///< Stormlash
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ALLY;
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ALLY;
                break;
            case 207356: ///< Refreshing Currents
                spellInfo->ProcFlags = 0;
                spellInfo->ProcChance = 0;
                spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                break;
            case 207354: ///<Caress of the Tidemother
                spellInfo->ProcFlags = 0;
                break;
            case 2825: ///< BloodLust
                spellInfo->OverrideSpellList.push_back(204361); ///< BloodLust
                break;
            case 193876: ///< Shamanism
                spellInfo->Effects[EFFECT_1].BasePoints = 204361; ///< BloodLust
                break;
            case 235967: ///< Velen's Future Shight - Proc heal
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_SHARE_PLAYER_DAMAGE;
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CANT_CRIT;
                spellInfo->MaxAffectedTargets = 3;
                break;
            case 235991: ///< Kil'jaeden's Burning Wish
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            case 212048: ///< Ancestral Vision
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_DEAD;
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                break;
            case 206647: ///< Electrocute
                spellInfo->AttributesEx6 |= SPELL_ATTR6_NO_DONE_PCT_DAMAGE_MODS;
                break;
            case 205533: ///< Volcanic Inferno
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            case 199116: ///< Doom Vortex
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            case 214815: ///< Lightning Bolt energize
            case 214816: ///< Lightning Bolt overloaded energize
                /// Remove casttime to prevent using casttime spellmods
                spellInfo->CastTimeEntry = nullptr;
                break;
            case 117588: ///< Fire Nova (Last Update 7.1.5 Build 23420)
                spellInfo->Effects[EFFECT_0].BonusMultiplier = 1.8f;
                break;
            case 118297: ///< Immolate (Last Update 7.1.5 Build 23420)
                spellInfo->Effects[EFFECT_0].BonusMultiplier = 0.54f;
                spellInfo->Effects[EFFECT_1].BonusMultiplier = 1.44f;
                break;
            case 157375: ///< Gale force (Last Update 7.1.5 Build 23420)
                spellInfo->Effects[EFFECT_0].MiscValue = 75;
                break;
            ///  210643 Totem Mastery summons
            case 210643: ///< Totem Mastery prevent summon not needed creature
                spellInfo->Effects[EFFECT_4].Effect = SPELL_EFFECT_NONE;
                break;
            case 202188: ///< Resonance Totem apply water display
                spellInfo->Effects[EFFECT_0].MiscValueB = 3400;
                break;
            case 210651: ///< Storm Totem apply wind display
                spellInfo->Effects[EFFECT_0].MiscValueB = 82;
                break;
            case 210657: ///< Ember Totem apply fire display
                spellInfo->Effects[EFFECT_0].MiscValueB = 3211;
                break;
            case 210660: ///< Tailwind Totem apply wind display
                spellInfo->Effects[EFFECT_0].MiscValueB = 3399;
                break;
            case 204281: ///< Rippling Watres heal
                spellInfo->DmgClass = SPELL_DAMAGE_CLASS_MAGIC;
                break;
            case 208723: ///< Echoes of the Great Sundering
                spellInfo->Effects[EFFECT_0].BasePoints = -100;
                break;
            case 242281: ///< Item - Shaman T20 Elemental 2P Bonus
                spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                break;
            case 191634: ///< Static Overload
                spellInfo->ProcFlags = 0;
                break;
            case 207553: ///< Totemic Revival
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_DEAD;
                spellInfo->ExplicitTargetMask = TARGET_FLAG_CORPSE_ALLY;
                spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(6);
                spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_RESURRECT;
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                break;
            case 252159: ///< Downpour
                spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_DONE_BONUS;
                break;
            case 191861: ///< Power of Maelstrom
                spellInfo->ProcFlags = 0;
                break;
            //////////////////////////////////////////////////////////////////////////////////////////////
            /// Warlock
            //////////////////////////////////////////////////////////////////////////////////////////////
            case 248814: ///< Nightfall
                spellInfo->ProcFlags = 0;
                spellInfo->ProcCharges = 0;
                break;
            case 211715: ///< Thal'kiel's consumption
                spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_DONE_BONUS;
                break;
            case 117828: ///< Backdraft
                spellInfo->Effects[EFFECT_0].m_CanBeStacked = false;
                spellInfo->Effects[EFFECT_1].m_CanBeStacked = false;
                break;
            case 205231: ///< DarkGlare's Eye Laser
                spellInfo->Effects[EFFECT_0].ChainTarget = 0;
                break;
            case 108370: ///< Soul leech
                spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_APPLY_AURA;
                spellInfo->Effects[EFFECT_1].Effect = SPELL_EFFECT_APPLY_AURA;
                break;
            case 205196: ///< Dreadbite
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            case 196282: ///< Hand of Gul'Dan (Summon)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST_RANDOM;
                spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(29); ///< 6 yards
                spellInfo->Effects[EFFECT_0].MiscValue = 55659;
                spellInfo->CasterAuraState = 0;
                break;
            case 86040: ///< Hand Of Gul'dan (Damage)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_DEST_AREA_ENEMY;
                break;
            case 212295: ///< Nether Ward
                spellInfo->AttributesEx3 |= SPELL_ATTR3_CAN_PROC_WITH_TRIGGERED;
                spellInfo->ProcFlags |= 0x222A0; ///< Proc flags of Spell Reflection - 216890
                break;
            case 212291: ///< Burning Legion
                spellInfo->Effects[EFFECT_1].SpellClassMask |= flag128(0, 0x100, 0, 0); ///< For Summon Infernal Guardian - 111685
                break;
            case 196414: ///< Eradication
                spellInfo->SetRangeIndex(5); ///< Long range : 0y-40y
                break;
            case 80240: ///< Havoc
                spellInfo->OverrideSpellList.push_back(200546); ///< Bane of Havoc
                break;
            case 199646: ///< Wrath of Consumption
                spellInfo->Effects[EFFECT_0].SpellClassMask |= flag128(0x100, 0, 0, 0); ///< For Siphon Life - 63106
                break;
            case 242834: /// Rend Soul
                spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(187);
                break;
            case 242922: ///< Jaws of Shadow
                spellInfo->Effects[EFFECT_0].BasePoints = 10;
                break;
            case 219455: ///< Glyph of the Terrorguard
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS;
                spellInfo->Effects[EFFECT_0].MiscValue = 18540;
                break;
            case 205690: ///< Roaring Blaze
                spellInfo->Effects[EFFECT_0].m_CanBeStacked = true;
                spellInfo->StackAmount = 10;
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
            case 205702: ///< Legendary 'Feretory of Souls' item ID = 132456
                spellInfo->ProcFlags = 0x11000;
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                break;
            case 113896: ///< Demonic Gateway
                spellInfo->Effects[EFFECT_3].Effect = SPELL_EFFECT_DUMMY;
                spellInfo->Effects[EFFECT_3].TriggerSpell = 0;
                break;
            case 193440: ///< Demonwrath
                spellInfo->ChannelInterruptFlags |= CHANNEL_INTERRUPT_FLAG_INTERRUPT;
                break;
            case 211714: ///< Thal'kiel's Consumption
                spellInfo->Effects[EFFECT_1].Effect = SPELL_EFFECT_SCHOOL_DAMAGE;
                break;
            case 205145: ///< Demonic Calling
                spellInfo->ProcChance = 100;
                spellInfo->Effects[EFFECT_0].BasePoints = 20;
                break;
            case 242291: ///< Item - Warlock T20 Affliction 4P Bonus
                spellInfo->ProcFlags = 0;
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                break;
            case 242294: ///< Item - Warlock T20 Demonology 4P Bonus
                spellInfo->ProcFlags = 0;
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                break;
            case 152108: ///< Cataclysm 1000% SpellPower 7.3.5
                spellInfo->Effects[EFFECT_0].BonusMultiplier = 10;
                break;
            case 196675: ///< Planeswalker
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                spellInfo->ProcFlags = 0;
                break;
            //////////////////////////////////////////////////////////////////////////////////////////////
            /// Warrior
            //////////////////////////////////////////////////////////////////////////////////////////////
            case 147833: ///< Intercept
                spellInfo->Effects._effects.push_back(SpellEffectInfo(sSpellStore.LookupEntry(spellInfo->Id), spellInfo, EFFECT_2, Difficulty::DifficultyNone));
                spellInfo->Effects[EFFECT_2].Effect = SPELL_EFFECT_APPLY_AURA;
                spellInfo->Effects[EFFECT_2].ApplyAuraName = SPELL_AURA_DUMMY;
                spellInfo->Effects[EFFECT_2].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[EFFECT_2].TargetB = 0;
                spellInfo->AttributesEx3 |= SPELL_ATTR3_CAN_PROC_WITH_TRIGGERED;
                break;
            case 76856: ///< Mastery: Unshackled Fury
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                break;
            case 198827:
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_PERIODIC_DUMMY;
                break;
            case 40602: /// Charge
                spellInfo->SetRangeIndex(95); /// RangeId 95 = "Charge" (8y-25y);
                spellInfo->m_SpellVisuals.clear();
                break;
            case 215556:    ///< War Machine
            case 215557:    ///< War Machine
                spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(187);  ///< 300y
                break;
            case 206572: ///< Dragon Charge
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(39); ///< 2s
                break;
            case 206579: ///< Dragon Charge
                spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(13); ///< 10 yards
                break;
            case 202574: ///< Vengeance: Ignore Pain
                spellInfo->Effects[EFFECT_1].ApplyAuraName = SPELL_AURA_ADD_PCT_MODIFIER;
                spellInfo->Effects[EFFECT_1].BasePoints = 5;
                spellInfo->Effects[EFFECT_1].MiscValue = SPELLMOD_SPELL_COST2;
                break;
            case 23920: ///< Spell Reflection (Prot)
                spellInfo->ProcCharges = 1;
                spellInfo->OverrideSpellList.push_back(213915); ///< Mass Spell Reflection (Prot)
                break;
            case 237261: ///< Duel
                spellInfo->Attributes &= ~SPELL_ATTR0_DEBUFF;
                spellInfo->AttributesEx |= SPELL_ATTR1_NO_THREAT;
                spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_INITIAL_AGGRO;
                break;
            case 215537: ///< Trauma
            case 253384: ///< T21 Fury 2P debuff 'Slaughter' based on damage dealt, shouldn't crit
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CANT_CRIT;
                break;
            case 200977: ///< Unrivaled Strength
                spellInfo->Effects[EFFECT_0].BasePoints = 5;
                break;
            //////////////////////////////////////////////////////////////////////////////////////////////
            /// Demon Hunter
            //////////////////////////////////////////////////////////////////////////////////////////////
            case 189111:
                spellInfo->ChargeCategoryEntry = nullptr;
                break;
            case 226377: ///< Vengeance Talent Spirit Bomb
                spellInfo->SchoolMask = SPELL_SCHOOL_MASK_FIRE;
                spellInfo->Effects[EFFECT_0].AttackPowerMultiplier = 1.8;
                break;
            case 203650: ///< Prepared
                spellInfo->Effects[EFFECT_0].BasePoints = 1;
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_PERIODIC_DUMMY;
                spellInfo->Effects[EFFECT_0].Amplitude = 100;
                break;
            case 217070: ///< Rage of The Illidari
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_SHARE_PLAYER_DAMAGE;
                break;
            case 201473: ///< Anguish of the Deceiver
                spellInfo->Effects[0].ApplyAuraName = SPELL_AURA_DUMMY;
                spellInfo->Effects[0].TriggerSpell = 0;
                break;
            case 198013: ///< Eye Beam
                spellInfo->Effects[0].TriggerSpell = 198030;
                spellInfo->AttributesEx5 &= ~SPELL_ATTR5_START_PERIODIC_AT_APPLY;
                spellInfo->ChannelInterruptFlags |= CHANNEL_INTERRUPT_FLAG_INTERRUPT;
                spellInfo->AttributesEx5 |= SPELL_ATTR5_HASTE_AFFECT_DURATION;
                break;
            case 198030: ///< Eye Beam
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_CONE_ENEMY_24;
                break;
            case 201427: ///< Annihilation
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ENEMY;
                spellInfo->Effects[EFFECT_2].TargetA = TARGET_UNIT_TARGET_ENEMY;
                break;
            case 211053: ///< Fel Barrage
                spellInfo->ChannelInterruptFlags |= CHANNEL_INTERRUPT_FLAG_INTERRUPT;
                break;
            case 235893: ///< Demonic Origin
                spellInfo->AuraInterruptFlags |= AURA_INTERRUPT_FLAG_CHANGE_MAP;
                break;
            case 192611: ///< Fel Rush
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            case 238118: ///< Flaming Soul
                spellInfo->ProcFlags |= (DONE_HIT_PROC_FLAG_MASK);
                spellInfo->ProcChance = 100;
                spellInfo->AttributesEx3 |= SPELL_ATTR3_CAN_PROC_WITH_TRIGGERED;
                break;
            case 235894: ///< Demonic Origin Buff, that used to stay if you left an arena / BG (7.1.5 - Build 23420)
                spellInfo->m_IsPVPTalent = true;
                break;
            case 200166: ///< Metamorphosis
                spellInfo->Effects[SpellEffIndex::EFFECT_2].Effect = SPELL_EFFECT_DUMMY;
                break;
            case 202387: ///< Inner Demons
                spellInfo->Effects[SpellEffIndex::EFFECT_0].TargetA = Targets::TARGET_DEST_CASTER;
                break;
            case 198793: ///< Vengeful Retreat
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(39); ///< 2s
                break;
            case 202138: ///< Sigil of Chains
                spellInfo->AttributesEx4 |= SPELL_ATTR4_CAN_CAST_WHILE_CASTING;
                break;
            case 204598: ///< Sigil of Flame Dot
                spellInfo->Effects[EFFECT_1].AttackPowerMultiplier = 2.04f;
                break;
            case 209426: ///< Darkness
                spellInfo->AttributesEx6 |= SPELL_ATTR6_NOT_LIMIT_ABSORB;
                break;
            case 215393: ///< Lesser Soul Fragment
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_CASTER;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            case 208985: ///< Eternal Hunger - Achor, the Eternal Hunger Item 137014
                spellInfo->AttributesEx8 |= SpellAttr8::SPELL_ATTR8_NOT_IN_BG_OR_ARENA;
                break;
            //////////////////////////////////////////////////////////////////////////////////////////////
            /// Item
            //////////////////////////////////////////////////////////////////////////////////////////////
            case 246560:
                spellInfo->Effects[SpellEffIndex::EFFECT_0].TargetA = Targets::TARGET_DEST_DEST;
                break;
            case 172944: ///< Detonate Iron Grenade
                spellInfo->Effects._effects.push_back(SpellEffectInfo(sSpellStore.LookupEntry(spellInfo->Id), spellInfo, EFFECT_3, Difficulty::DifficultyNone));
                spellInfo->Effects[SpellEffIndex::EFFECT_3].Effect = SPELL_EFFECT_DUMMY;
                spellInfo->Effects[SpellEffIndex::EFFECT_3].TargetA = Targets::TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[SpellEffIndex::EFFECT_3].TargetB = 0;
                break;
            case 225729: /// Spell Frost Enchant from Entwined Elemental Foci (Nighthold Trinket - 140796) Hotfix has been made on DB2, but until 7.3 it can't take effect.
                spellInfo->Effects[EFFECT_0].ScalingMultiplier = 0;
                break;
            case 174732: ///< Punt Podling
                spellInfo->Effects[SpellEffIndex::EFFECT_0].TargetA = Targets::TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[SpellEffIndex::EFFECT_0].TargetB = 0;
                spellInfo->Effects[SpellEffIndex::EFFECT_1].Effect = SPELL_EFFECT_DUMMY;
                spellInfo->Effects[SpellEffIndex::EFFECT_1].TargetA = Targets::TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[SpellEffIndex::EFFECT_1].TargetB = 0;
                break;
            case 170494: ///< Flask of Conquest
                spellInfo->Effects[SpellEffIndex::EFFECT_0].MiscValue = CURRENCY_TYPE_CONQUEST_META_ARENA_BG;
                spellInfo->Effects[SpellEffIndex::EFFECT_1].MiscValue = CURRENCY_TYPE_CONQUEST_META_ARENA_BG;
                break;
            case 127250: ///< Ancient Knowledge
                spellInfo->AttributesEx3 |= SPELL_ATTR3_DEATH_PERSISTENT;
                break;
            case 225131: ///< Carrion Swarm - Icon of Rot
                spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                break;
            case 225731: ///< Carrion Swarm Dot - Icon of Rot
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 225125: ///< Accelerando - Erratic Metronome
                spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                break;
            case 225725: ///< Nightwell Tranquility - Aluriel's Mirror
                spellInfo->MaxAffectedTargets = 6;
                break;
            case 242209: ///< Infernal Skin
                spellInfo->Effects[EFFECT_1].ApplyAuraName = SPELL_AURA_SCHOOL_ABSORB;
                spellInfo->Effects[EFFECT_1].BasePoints = 5000000;
                spellInfo->Effects[EFFECT_1].TriggerSpell = 0;
                spellInfo->Effects[EFFECT_1].MiscValue = SPELL_SCHOOL_MASK_ALL;
                spellInfo->ProcFlags = 0;
                spellInfo->AttributesEx6 |= SPELL_ATTR6_NOT_LIMIT_ABSORB;
                break;
            case 242524: ///< Terror From Below
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                break;
            case 101641: ///< Tarecgosa's Visage
                spellInfo->Effects[SpellEffIndex::EFFECT_0].MiscValue = 0;
                break;
            case 246464: ///< Dread Torrent
            {
                auto l_Itr = sSpellScalingMap.find(spellInfo->Id);
                if (l_Itr != sSpellScalingMap.end()) sSpellScalingMap.erase(l_Itr);
                spellInfo->AttributesEx11 &= ~SpellAttr11::SPELL_ATTR11_SCALES_WITH_ITEM_LEVEL;
                break;
            }
            case 256719: ///< Eye of F'harg
            case 256718: ///< Eye of Shatug
                spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_APPLY_AURA;
                break;
            case 255724: ///< Legion Bombardment
                spellInfo->Effects[EFFECT_0].TriggerSpell = 255713;
                spellInfo->Effects[EFFECT_1].TriggerSpell = 253248;
                break;
            case 224001: ///< Defiled Augmentation, Defiled Augment Rune
            case 284307: ///< Defiled Augmentation, Lightforged Augment Rune
                spellInfo->AttributesEx8 |= SpellAttr8::SPELL_ATTR8_NOT_IN_BG_OR_ARENA;
                break;
            case 254483: ///< Set Item Level, Relinquished Armor Set
                spellInfo->Effects[SpellEffIndex::EFFECT_1].MiscValue = 35; /// Custom GarrItemLevelUpgradeDataEntry with MaxItemLevel=880
                break;
            //////////////////////////////////////////////////////////////////////////////////////////////
            /// Follower Spells
            //////////////////////////////////////////////////////////////////////////////////////////////
            case 219211:
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_CASTER_FRONT;
                break;
            case 222129:
            {
                spellInfo->Effects._effects.push_back(SpellEffectInfo(sSpellStore.LookupEntry(spellInfo->Id), spellInfo, EFFECT_1, Difficulty::DifficultyNone));
                spellInfo->Effects[EFFECT_1].Effect = SPELL_EFFECT_APPLY_AURA;
                spellInfo->Effects[EFFECT_1].ApplyAuraName = SPELL_AURA_PERIODIC_DUMMY;
                spellInfo->Effects[EFFECT_1].BasePoints = 0;
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_SUMMONER;
                spellInfo->Effects[EFFECT_1].TargetB = 0;
                spellInfo->Effects[EFFECT_1].Amplitude = 250;
                spellInfo->EffectCount++;
                break;
            }
            case 222624: ///< Summon Blood Knight (Left 1)
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(8);
                break;
            case 222626: ///< Summon Blood Knight (Left 2)
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(15);
                break;
            case 222627: ///< Summon Blood Knight (Right 1)
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(8);
                break;
            case 222628: ///< Summon Blood Knight (Right 2)
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(15);
                break;
            case 222622: ///< Summon Lady Liadrin
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(8);
                break;
            case 223161: /// Wirlwind from Mother's Caress
                spellInfo->Effects[EFFECT_0].BasePoints *= 100;
            case 221756: ///< Summon Whitemane
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(8);
                break;
            case 221762: ///< Summon Trollbane
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(15);
                break;
            case 221764: ///< Summon Darion Mograine
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(8);
                break;
            case 221766: ///< Summon Nazgrim
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(15);
                break;
            case 219578: ///< That One's Mine!
                spellInfo->InterruptFlags = 0;
                break;
            case 233970: ///< Tidal Wave (Si'vash)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_AREATRIGGER_BIG_OBJ;
                break;
            case 229900: ///< Demon Barrier (Agatha)
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_PERIODIC_DUMMY;
                break;
            case 242987: ///< Translocate (Agatha)
                spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(181); ///< 200y
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_DEST_DEST;
                break;
            case 235263: ///< Fel Lava (Agatha)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            case 230943: ///< Summon Imp Servant (Agatha)
            case 230944: ///< Summon Imp Servant (Agatha)
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(152); ///< 150y
                break;
            case 242950: ///< Translocate (Agatha)
                spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(9); ///< 0-20y
                break;
            case 242468: ///< Spit Bile (Tugar Bloodtotem)
            case 243238: ///< Fel-Infused Saliva (Jormog the Behemoth)
            case 235619: ///< Hand from Beyond (Karam Magespear)
            case 235826: ///< Beaming Gaze (End of the Risen Threat)
                spellInfo->AttributesEx3 |= SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                break;
            case 242730: ///< Fel Shock (Tugar Bloodtotem)
                spellInfo->Effects[EFFECT_1].Mechanic = Mechanics::MECHANIC_NONE;
                break;
            case 232661: ///< Razor Ice (Archmage Xylem)
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(39); ///< 2s
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 232007: ///< Darkness (Archmage Xylem)
                spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_DAMAGE_FROM_MAX_HEALTH_PCT;
                spellInfo->Effects[EFFECT_0].BasePoints = 30;
                spellInfo->Attributes &= ~SPELL_ATTR0_LEVEL_DAMAGE_CALCULATION;
                break;
            case 242208: ///< Comet Storm (Archmage Xylem)
                spellInfo->AttributesEx5 |= SPELL_ATTR5_USABLE_WHILE_STUNNED;
                spellInfo->AttributesEx5 |= SPELL_ATTR5_USABLE_WHILE_FEARED;
                spellInfo->AttributesEx5 |= SPELL_ATTR5_USABLE_WHILE_CONFUSED;
                spellInfo->AttributesEx5 |= SPELL_ATTR8_USABLE_WHILE_SILENCED;
                break;
            case 202081: ///< Fixate (Karam Magespear)
                spellInfo->Effects[EFFECT_1].ApplyAuraName = SPELL_AURA_FIXATE;
                break;
            case 237914: ///< Runic Detonation (Sigryn)
            case 238020: ///< Orb of Valor (Sigryn)
            case 243820: ///< Break Focus (Xylem)
            case 243821: ///< Break Focus (Xylem)
            case 243822: ///< Break Focus (Xylem)
            case 234764: ///< Challenger's Triumph
            case 243095: ///< Plague Zone (Agatha)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                break;
            case 237849: ///< Advance (Sigryn)
                spellInfo->CasterAuraSpell = 0;
                break;
            case 231563: ///< Break Focus (Xylem)
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[EFFECT_2].TriggerSpell = 0;
                spellInfo->Effects[EFFECT_3].TriggerSpell = 0;
                spellInfo->Effects[EFFECT_4].TriggerSpell = 0;
                break;
            case 235984: ///< Mana Sting (Corrupted Risen Arbalest)
                spellInfo->AttributesEx3 |= SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(152); ///< 150y
                break;
            case 242733: ///< Fel Burst (Tugar Bloodtotem)
                spellInfo->InterruptFlags |= SPELL_INTERRUPT_FLAG_INTERRUPT;
                break;
            case 237173: ///< Unstable Fel Explosion (End of the Risen Threat)
                spellInfo->ProcFlags = 0;
                break;
            case 237652: ///< Fel Fire (End of the Risen Threat)
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(21); ///< -1s
                break;
            case 233447: ///< Soul Missiles (Highlord Kruul)
                spellInfo->Effects[EFFECT_1].Effect = 0;
                spellInfo->Effects[EFFECT_1].TriggerSpell = 0;
                break;
            case 234676: ///< Twisted Reflection (Highlord Kruul)
                spellInfo->InterruptFlags |= SPELL_INTERRUPT_FLAG_INTERRUPT | SPELL_INTERRUPT_FLAG_PUSH_BACK;
                break;
            case 235823: ///< Knife Dance (Corrupted Risen Soldier)
                spellInfo->Effects[EFFECT_0].TriggerSpell = 200325;
                break;
            case 236720: ///< Frenzied Assault (Corrupted Risen Soldier)
                spellInfo->AttributesEx3 |= SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                break;
            case 237937: ///< Empower (Sigryn)
                spellInfo->ExcludeTargetAuraSpell = 0;
                break;
            //////////////////////////////////////////////////////////////////////////////////////////////
            /// Broken Shore Building Buffs
            //////////////////////////////////////////////////////////////////////////////////////////////
            case 239645: ///< Fel Treasures (Mage Tower)
            case 239647: ///< Epic Hunter (Nether Disruptor)
                spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_APPLY_AURA;
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(21); ///< Infinite
                break;
            case 239648: ///< Forces of the Order (Command Center)
                spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_APPLY_AURA;
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_PERIODIC_DUMMY;
                spellInfo->Effects[EFFECT_0].Amplitude = 5 * IN_MILLISECONDS;
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(21); ///< Infinite
                break;
            //////////////////////////////////////////////////////////////////////////////////////////////
            /// Allied Races Spells
            //////////////////////////////////////////////////////////////////////////////////////////////
            case 255653: ///< Demonbane
                spellInfo->Effects[EFFECT_0].MiscValue = CreatureType::CREATURE_TYPE_DEMON;
                break;
            case 263154: ///< Opening (Kirin Tor Chest)
            case 263155: ///< Opening (Gilded Trunk)
            case 263156: ///< Opening (Legionfall Chest)
            case 263157: ///< Opening (Scuffed Krokul Cache)
            case 249085: ///< Opening (Dreamweaver Cache)
            case 243128: ///< Opening (Valarjar Strongbox)
            case 249084: ///< Opening (Farondis Chest)
            case 249086: ///< Opening (Highmountain Supplies)
            case 249087: ///< Opening (Nightfallen Cache)
            case 249090: ///< Opening (Warden's Supply Kit)
            case 249091: ///< Opening (Legionfall Chest)
            case 253747: ///< Opening (Brittle Krokul Chest)
            case 253748: ///< Opening (Gleaming Footlocker)
                spellInfo->Speed = 0.0f;
                break;
            case 43731:  ///< Lightning Zap
                spellInfo->Effects[EFFECT_0].BasePoints = 660;
                break;
            case 43730:  ///< Electrified
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[EFFECT_1].TargetB = 0;
                break;
            //////////////////////////////////////////////////////////////////////////////////////////////
            /// Recruit A Friend Spell
            //////////////////////////////////////////////////////////////////////////////////////////////
            case 13: ///< Recruit A Friend !
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(21); ///< Infinite
                break;
            //////////////////////////////////////////////////////////////////////////////////////////////
            /// SPELL_AURA_MOD_COOLDOWN_RECOVERY_RATE auras
            //////////////////////////////////////////////////////////////////////////////////////////////
            case 198926: ///< Castaway
                spellInfo->Effects[EFFECT_2].BasePoints = 0;
                break;
            case 204366: ///< Thundercharge
                spellInfo->Effects[EFFECT_1].BasePoints = 0;
                spellInfo->Effects[EFFECT_2].BasePoints = 0;
                spellInfo->Effects[EFFECT_3].BasePoints = 0;
                spellInfo->Effects[EFFECT_4].BasePoints = 0;
                break;
            case 209427: ///< Shadow's Caress
                spellInfo->Effects[EFFECT_0].BasePoints = 0;
                spellInfo->Effects[EFFECT_2].BasePoints = 0;
                break;
            case 209708: ///< Shadow's Caress
            case 233397: ///< Delirium
                spellInfo->Effects[EFFECT_1].BasePoints = 0;
                break;
            case 214975: ///< Heartstop Aura
                spellInfo->Effects[EFFECT_1].BasePoints = 0;
                spellInfo->Effects[EFFECT_2].Effect = SPELL_EFFECT_NONE;
                spellInfo->Effects[EFFECT_2].BasePoints = 0;
                break;
            case 216331: ///< Avenging Crusader
                spellInfo->Effects[EFFECT_4].ApplyAuraName = AuraType::SPELL_AURA_ADD_PCT_MODIFIER;
                spellInfo->Effects[EFFECT_4].BasePoints = spellInfo->Effects[EFFECT_5].BasePoints;
                spellInfo->Effects[EFFECT_4].SpellClassMask = spellInfo->Effects[EFFECT_2].SpellClassMask;
                spellInfo->Effects[EFFECT_5].SpellClassMask = spellInfo->Effects[EFFECT_2].SpellClassMask;
                spellInfo->Effects[EFFECT_7].BasePoints = 0;
                break;
            default:
                break;
        }

        switch (spellInfo->Id)
        {
            case 19386: ///< Wyvern Sting
            case 339:   ///< Entangling Roots
            case 118:   ///< Polymorph
            case 20066: ///< Repentance
            case 9484:  ///< Shackle Undead
            case 2094:  ///< Blind
            case 51514: ///< Hex
            case 710:   ///< Banish
            case 6358:  ///< Seduction
                spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_INITIAL_AGGRO;
                break;
            default:
                break;
        }

        /// Handle some specific spells for AoE avoidance
        /// Many of AreaTrigger DoTs are single target spells but must be considered as AoE spells
        switch (spellInfo->Id)
        {
            case 155223: ///< Melt (Blast Furnace)
            case 155743: ///< Slag Pool (Blast Furnace)
            case 156932: ///< Rupture (Blast Furnace)
            case 176133: ///< Slag Bomb (Blast Furnace)
            case 155080: ///< Inferno Slice (Gruul)
            case 155301: ///< Overhead Smash (Gruul)
            case 155078: ///< Overwhelming Blows (Gruul)
            case 155530: ///< Shatter (Gruul)
            case 173192: ///< Cave In (Gruul)
            case 156203: ///< Retched Blackrock (Oregorger)
            case 155897: ///< Earthshaking Collision (Oregorger)
            case 156388: ///< Explosive Shard - Missile (Oregorger)
            case 156374: ///< Explosive Shard - Explosion (Oregorger)
            case 155900: ///< Rolling Fury (Oregorger)
            case 155318: ///< Lava Slash - AoE missile (Flamebender Ka'graz)
            case 155314: ///< Lava Slash - DoT (Flamebender Ka'graz)
            case 162370: ///< Crystalline Barrage (Tectus)
            case 162510: ///< Tectonic Upheaval (Tectus)
            case 159311: ///< Flame Jet (Kargath Bladefist)
            case 159002: ///< Berserker Rush (Kargath Bladefist)
            case 159413: ///< Mauling Brew (Kargath Bladefist)
            case 158519: ///< Quake (Twin Ogron)
            case 158241: ///< Blaze (Twin Ogron)
            case 157944: ///< Whirlwind (Twin Ogron)
            case 158336: ///< Pulverize - First AoE damage (Twin Ogron)
            case 158417: ///< Pulverize - Second AoE damage (Twin Ogron)
            case 158420: ///< Pulverize - Third AoE damage (Twin Ogron)
            case 156844: ///< Stone Breath (Kromog)
            case 156704: ///< Slam (Kromog)
            case 162349: ///< Fists of Stone (Kromog)
            case 157055: ///< Thundering Blows (Kromog)
            case 161893: ///< Shattered Earth (Kromog)
            case 157659: ///< Rippling Smash (Kromog)
            case 161923: ///< Rune of Crushing Earth (Kromog)
            case 157247: ///< Reverberations (Kromog)
            case 162516: ///< Whirling Steel (Iron Assembly Warden)
            case 162672: ///< Goring Swipe (Stubborn Ironhoof & Ornery Ironhoof)
            case 162675: ///< Ground Slam (Stubborn Ironhoof & Ornery Ironhoof)
            case 162663: ///< Electrical Storm - DoT (Thunderlord Beast-Tender)
            case 162772: ///< Colossal Roar (Markog Aba'dir)
            case 154956: ///< Pin Down - Damage (Beastlord Darmac)
            case 154960: ///< Pinned Down - DoT (Beastlord Darmac)
            case 155222: ///< Tantrum - AoE (Ironcrusher)
            case 155249: ///< Stampede - 1st (Ironcrusher)
            case 155531: ///< Stampede - 2nd (Ironcrusher)
            case 155520: ///< Tantrum - AoE (Beastlord Darmac)
            case 155061: ///< Rend and Tear - AoE (Cruelfang)
            case 155198: ///< Savage Howl (Cruelfang)
            case 162283: ///< Rend and Tear - AoE (Beastlord Darmac)
            case 154989: ///< Inferno Breath (Dreadwing)
            case 156824: ///< Inferno Pyre - DoT (Dreadwing)
            case 156823: ///< Superheated Scrap - DoT (Beastlord Darmac)
            case 155657: ///< Flame Infusion - DoT (Pack Beast)
            case 163754: ///< Iron Bellow (Grom'kar Man-at-Arms)
            case 163752: ///< Reckless Slash (Grom'kar Man-at-Arms)
            case 156655: ///< Throw Grenade (Iron Raider)
            case 158084: ///< Delayed Siege Bomb - Damage (Iron Gunnery Sergeant)
            case 160050: ///< Delayed Siege Bomb - Damage (Operator Thogar)
            case 165195: ///< Prototype Pulse Grenade - DoT (Operator Thogar)
            case 156553: ///< Moving Train (Operator Thogar)
            case 171614: ///< Inferno (Inferno Totem - Iron Earthbinder)
            case 156637: ///< Rapid Fire - AoE (Admiral Gar'an - Iron Maidens)
            case 158078: ///< Blood Ritual - AoE (Marak the Blooded - Iron Maidens)
            case 164279: ///< Penetrating Shot (Admiral Gar'an - Iron Maidens)
            case 156610: ///< Sanguine Strikes - proc (Marak the Blooded - Iron Maidens)
            case 158710: ///< Chain Lightning (Battle Medic Rogg - Iron Maidens)
            case 158695: ///< Grapeshot Blast (Uktar - Iron Maidens)
            case 158684: ///< Corrupted Blood - DoT (Uk'urogg - Iron Maidens)
            case 160733: ///< Bomb Impact - AoE (Iron Cannon - Iron Maidens)
            case 157884: ///< Detonation Sequence - AoE (Cluster Bomb Alpha - Iron Maidens)
            case 230143: ///< Hydra Shot Damage (Sasszine)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_IS_CUSTOM_AOE_SPELL;
                break;
            default:
                break;
        }

        /// Some bosses spells shoudn't target pets and don't have the attriute
        switch (spellInfo->Id)
        {
            case 226512: ///< Sanguine Ichor
            case 209862: ///< Volcanic Plume
            case 193152: ///< Quake
            case 192794: ///< Lightning Strike
            case 193235: ///< Dancing Blade
            case 192307: ///< Sanctify
            case 221872: ///< Spear of Light
            case 240443: ///< Bursting Affix
                spellInfo->AttributesEx3 |= SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                break;
        }

#pragma region CHALLENGES_AFFIX
        switch (spellInfo->Id)
        {
            case 209858: ///< Necrotic Rot
            {
                spellInfo->AuraInterruptFlags |= AURA_INTERRUPT_FLAG_LEAVE_COMBAT;
                spellInfo->Effects[EFFECT_2].ApplyAuraName = SPELL_AURA_MOD_ABSORPTION_PCT;
                break;
            }
            default:
                break;
        }
#pragma endregion

#pragma region ZUL_GURUB

        switch (spellInfo->Id)
        {
            case 96325: ///< Zanzil's Cauldron of Burning Blood
            case 96326: ///< Zanzil's Cauldron of Frostburn Formula
            case 96328: ///< Zanzil's Cauldron of Toxic Torment
                spellInfo->AuraInterruptFlags |= AURA_INTERRUPT_FLAG_CHANGE_MAP;
                break;
        }

#pragma endregion

#pragma region VIOLET_HOLD
        switch (spellInfo->Id)
        {
            case 202217: ///< Mandibul Strike (Anub'esset)
            {
                spellInfo->Effects[EFFECT_1].BasePoints = -spellInfo->Effects[EFFECT_1].BasePoints;
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            }
            case 204763: ///< Violent Fel Energy (Portal Keeper - Felguard)
            {
                spellInfo->Effects[EFFECT_1].Amplitude = 1000;
                break;
            }
            case 201153: ///< Eternal Darkness (Mindflayer Kaahrj)
            {
                spellInfo->Effects[EFFECT_0].Amplitude = 100;
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_PERIODIC_TRIGGER_SPELL;
                break;
            }
            case 201172: ///< Eternal Darkess - DoT (Faceless Tendril)
            {
                spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                break;
            }
            case 201300: ///< Tentacle (Mindflayer Kaahrj)
            {
                spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(51);  ///< 60y
                spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_DUMMY;
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST_RANDOM;
                break;
            }
            case 202480: ///< Fixated (Anub'esset)
            {
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                break;
            }
            case 201369: ///< Rocket Chicken Rocket (Millificent Manastorm)
            {
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_ENNEMIES_IN_CYLINDER;
                spellInfo->Effects[EFFECT_2].TargetA = TARGET_ENNEMIES_IN_CYLINDER;
                break;
            }
            case 202339: ///< Mighty Smash - Jump (Fel Lord Betrug)
            {
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            }
            default:
                break;
        }
#pragma endregion

#pragma region EYE_OF_AZSHARA
        switch (spellInfo->Id)
        {
            case 192794: ///< Lightning Strike
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CANT_CRIT;
                spellInfo->AttributesEx3 |= SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                break;
            case 192796: ///< Lightning Strikes
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[EFFECT_1].Effect = 0;
                break;
            case 196630: ///< Monsoon (Lady Hatecoil)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                break;
            case 193066: ///< Sand Dunes 'protection' aura (Lady Hatecoil)
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(21); ///< -1s
                break;
            case 198245: ///< Brutal Haymaker
                spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(3); ///< 20 yards
                break;
            case 192072: ///< Call Reinforcements 1 (Warlord Parjesh)
            case 192073: ///< Call Reinforcements 2 (Warlord Parjesh)
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(21);    ///< 35 yards
                spellInfo->Effects[EFFECT_1].SetRadiusIndex(21);    ///< 35 yards
                break;
            case 193497: ///< Rising Tide (Wrath of Azshara)
                spellInfo->Effects[EFFECT_0].BasePoints = -10;
                break;
            case 192620: ///< Massive Deluge (Wrath of Azshara)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                break;
            case 193196: ///< Storm conduit (Wrath of Azshara)
                spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                break;
            case 191975: ///< Impaling Spear (Warlord Parjesh)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_IGNORE_ARMOR;
                break;
            case 193018: ///< Gaseous Bubbles (King Deepbeard)
                spellInfo->InterruptFlags = 0;
                break;
            case 191797: ///< Violent Winds
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(1); ///< 10s
                break;
            case 196298: ///< Roiling Storm
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            case 197105: ///< Polymorph: Fish (Hatecoil Arcanist)
                spellInfo->Effects[EFFECT_1].ApplyAuraName = SPELL_AURA_DUMMY;
                break;
            case 191856: ///< Toxic Puddle (Serpentrix)
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(21); ///< -1s
                break;
            case 195827: ///< Massive Quake (Skrog Tidestomper)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_SRC_CASTER;
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_SRC_CASTER;
                spellInfo->Effects[EFFECT_1].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                break;
            default:
                break;
        }
#pragma endregion

#pragma region BLACK_ROOK_HOLD
        switch (spellInfo->Id)
        {
            case 225578: /// Dark Mending - Trahsmob
            {
                spellInfo->AttributesEx |= SPELL_ATTR1_CANT_TARGET_SELF;
                break;
            }
            case 198820: ///< Dark Blast
            case 199567: ///< Dark Obliteration
            {
                SpellTargetRestrictionsEntry const* l_SpellTargetRestriction = spellInfo->GetSpellTargetRestrictions();
                const_cast<SpellTargetRestrictionsEntry*>(l_SpellTargetRestriction)->Width = 8.5f;
                break;
            }
            case 225732: ///< Strike Down
            case 198635: ///< Unerrgin Shear
            case 194956: ///< Reap of Souls - Amalgalm of Souls
            {
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            }
            default:
                break;
        }
#pragma endregion

#pragma region RETURN_TO_KARAZHAN
        switch (spellInfo->Id)
        {
            case 228164: ///< Hammer Down (Phantom Crew)
            case 151384: ///< Kiss Of Death (Reformed Maiden)
            case 227742: ///< Garrote (Moroes)
            case 230050: ///< Force Blade (Abstract Nullifier)
            case 229693: ///< Poison Fang (Rat)
            case 228252: ///< Shadow Rend
            {
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            }
            case 227925: ///< Final Curtain (Ghostly Understudy)
            {
                spellInfo->Effects._effects.push_back(SpellEffectInfo(sSpellStore.LookupEntry(spellInfo->Id), spellInfo, EFFECT_1, Difficulty::DifficultyNone));
                spellInfo->Effects[EFFECT_1].Effect = SPELL_EFFECT_INSTAKILL;
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[EFFECT_1].TargetB = 0;
                spellInfo->EffectCount++;
                break;
            }
            case 228852: ///< Shared Suffering (Attumen the Huntsman)
            {
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_SHARE_DAMAGE;
                break;
            }
            default:
                break;
        }
#pragma endregion

#pragma region  NELTHARIONS_LAIR
        switch (spellInfo->Id)
        {
            case 217233: ///< Intro Jump (Ularogg Cragshaper)
                spellInfo->AttributesEx4 &= ~SPELL_ATTR4_CAN_CAST_WHILE_CASTING;
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                spellInfo->Effects[EFFECT_0].ValueMultiplier = 150.0f;
                spellInfo->Effects[EFFECT_0].MiscValueB = 100;
                break;
            case 198509: ///< Stance of the Mountain
                spellInfo->AttributesEx4 &= ~SPELL_ATTR4_CAN_CAST_WHILE_CASTING;
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                spellInfo->Effects[EFFECT_0].ValueMultiplier = 150.0f;
                spellInfo->Effects[EFFECT_0].MiscValueB = 100;
                spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                break;
            case 205288: ///< Worm Safe Fall
                spellInfo->Effects[EFFECT_0].BasePoints = 1000000;
                break;
            case 204136: ///< Spiked Tongue - Jump (Naraxas)
                spellInfo->Effects[EFFECT_0].ValueMultiplier = 200.0f;
                break;
            case 184483: ///< Cosmetic Jump (Navarrogg)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                spellInfo->Effects[EFFECT_0].ValueMultiplier = 500.0f;
                spellInfo->Effects[EFFECT_0].MiscValueB = 250;
                break;
            case 198564: ///< Stance of the Mountain - Summon (Ularogg Cragshaper)
            case 198565: ///< Stance of the Mountain - Summon (Ularogg Cragshaper)
            case 216249: ///< Stance of the Mountain - Summon (Ularogg Cragshaper)
            case 216250: ///< Stance of the Mountain - Summon (Ularogg Cragshaper)
            case 213589: ///< Summon Pillar of Creation (Dargrul)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                break;
            case 188494: ///< Rancid Maw (Naraxas)
                spellInfo->Effects[EFFECT_1].BasePoints = -spellInfo->Effects[EFFECT_1].BasePoints;
                break;
            case 205609: ///< Rancid Maw - DoT (Naraxas)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ANY;
                break;
            case 217851: ///< Toxic Retch - 5s debuff (Naraxas)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 215898: ///< Crystalline Ground (Rokmora)
            case 199176: ///< Spiked Tongue (Naraxas)
            case 210166: ///< Toxic Retch (Naraxas)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                break;
            case 192800: ///< Choking Dust (Rokmora)
            case 216407: ///< Lava Geyser (Dargrul)
                spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(36); ///< 45 yards
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Attributes |= SPELL_ATTR0_CASTABLE_WHILE_DEAD;
                break;
            case 198616: ///< Stance of the Mountain - Move (Bellowing Idol)
                spellInfo->Effects[EFFECT_0].ValueMultiplier = 50.0f;
                break;
            case 210159: ///< Toxic Retch - Missile AT (Naraxas)
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                break;
            case 200154: ///< Burning Hatred - Molten Charskin (Dargrul)
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                break;
            case 217090: ///< Magma Wave (Dargrul)
                spellInfo->AttributesEx3 |= SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                break;
            default:
                break;
        }
#pragma endregion

#pragma region LEGION_WORLD_BOSSES
        switch (spellInfo->Id)
        {
            case 212943: ///< Lightning Rod (Nithogg)
            {
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_FIXATE;
                break;
            }
            case 217907: ///< Wrathful Flames (Calamir)
            {
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                break;
            }
            case 223599: ///< Withered Presence (Withered J'im)
            {
                spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(36); ///< 45 yards

                for (uint8 l_I = 0; l_I < spellInfo->EffectCount; ++l_I)
                    spellInfo->Effects[l_I].TargetA = TARGET_UNIT_TARGET_ANY;

                spellInfo->Effects[EFFECT_0].BasePoints = -50;
                spellInfo->ExcludeTargetAuraSpell = spellInfo->Id;
                spellInfo->AuraInterruptFlags &= ~AURA_INTERRUPT_FLAG_LEAVE_COMBAT;
                break;
            }
            case 217925: ///< Icy Comet (Calamir)
            {
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ANY;
                break;
            }
            case 217967: ///< Howling Gale (Calamir)
            case 223708: ///< Nightshifted Bolts (Withered J'im)
            {
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                break;
            }
            case 218003: ///< Arcanopulse (Calamir)
            {
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_CASTER_RANDOM;
                break;
            }
            case 217206: ///< Gust of Wind (Levantus)
            {
                spellInfo->AuraInterruptFlags |= AURA_INTERRUPT_FLAG_DIRECT_DAMAGE;
                break;
            }
            case 217235: ///< Rending Whirl (Levantus)
            {
                spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(24); ///< 65y
                spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(24); ///< 65y
                break;
            }
            case 223614: ///< Resonance (Withered J'im)
            {
                spellInfo->ProcCharges = 10;
                break;
            }
            case 223688: ///< Nightstable Energy (Withered J'im)
            {
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_TARGET_RADIUS;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            }
            case 223634: ///< Nightshifted Bolts (Withered J'im)
            case 220277: ///< Summon Jetsam Stalker (Flotsam)
            {
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            }
            case 227128: ///< 7.0 Raid World Boss - Bonus Roll Prompt (Common)
            {
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                break;
            }
            case 212884: ///< Electrical Storm - DoT (Nithogg)
            {
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            }
            case 219861: ///< Web Wrap (Na'zak the Fiend)
            {
                spellInfo->AttributesEx3 |= SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                break;
            }
            case 219600: ///< Foundational Collapse (Na'zak the Fiend)
            {
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_DEST_TARGET_RANDOM;
                break;
            }
            case 197376: ///< Fel Geyser (Ana-Mouz)
            {
                spellInfo->Effects[EFFECT_0].TriggerSpell = 226018;
                break;
            }
            default:
                break;
        }

        spellInfo->EffectCount = spellInfo->Effects._effects.size();
#pragma endregion

#pragma region EMERALD_NIGHTMARE
        switch (spellInfo->Id)
        {
            case 215449: ///< Necrotic Venom (Nythendra)
            case 221028: ///< Unstable Decay (Gelatinized Decay)
            case 223912: ///< Crush Armor (Bo'lan the Marked)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 204894: ///< Transform Bugs (Nythendra)
                spellInfo->AttributesEx6 |= SPELL_ATTR6_CAN_TARGET_INVISIBLE;
                break;
            case 204470: ///< Volatile Rot (Nythendra)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_CAN_BE_CASTED_ON_ALLIES;
                break;
            case 203646: ///< Burst of Corruption (Nythendra)
                if (difficulty == Difficulty::DifficultyRaidMythic || difficulty == Difficulty::DifficultyRaidHeroic)
                    spellInfo->Effects[EFFECT_1].BasePoints = 75;
                spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                break;
            case 204504: ///< Infested (Nythendra)
                spellInfo->Effects[EFFECT_0].Amplitude = 2000;
                break;
            case 202977: ///< Infested Breath - Periodic (Nythendra)
            case 218536: ///< Dark Storm Cosmetic (Elerethe Renferal)
                spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                break;
            case 203537: ///< Infested Ground - Triggered (Nythendra)
            case 218537: ///< Violent Winds - Jump (Elerethe Renferal)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            case 197969: ///< Roaring Cacophony (Ursoc)
            case 197980: ///< Nightmarish Cacophony (Ursoc's Images)
            case 206369: ///< Corruption Meteor (Xavius)
            case 207849: ///< Corruption Meteor - Energize (Xavius)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_SHARE_DAMAGE;
                break;
            case 211073: ///< Desiccating Stomp (Cenarius)
            {
                spellInfo->CasterAuraSpell = 0;
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_SHARE_DAMAGE;

                SpellPowerEntry const* l_SpellPower = nullptr;
                for (auto l_Iter : spellInfo->SpellPowers)
                {
                    if (!l_SpellPower || l_Iter->Cost > l_SpellPower->Cost)
                        l_SpellPower = l_Iter;
                }

                spellInfo->SpellPowers.clear();
                spellInfo->SpellPowers.push_back(l_SpellPower);
                break;
            }
            case 208697: ///< Mind Flay (Il'gynoth)
                spellInfo->ChannelInterruptFlags |= CHANNEL_INTERRUPT_FLAG_INTERRUPT;
                break;
            case 215128: ///< Cursed Blood (Il'gynoth)
                spellInfo->AttributesEx3 |= SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                break;
            case 215234: ///< Nightmarish Fury (Il'gynoth)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                break;
            case 212639: ///< Cleansed Ground (Cenarius)
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(3); ///< 60.000
                break;
            case 206656: ///< Nightmare Blades (Xavius)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_IGNORE_ARMOR;
                break;
            case 208860: ///< Crushing Shadows (Xavius)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_IGNORE_ARMOR;
                spellInfo->AttributesEx3 |= SPELL_ATTR3_IGNORE_HIT_RESULT;
                break;
            case 204044: ///< Shadow Burst (Dragons of Nightmare)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_CAN_BE_CASTED_ON_ALLIES;
                break;
            case 203897: ///< Dark Offering (Dragons of Nightmare)
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CANT_CRIT;
                break;
            case 206758: ///< Gloom (Dragons of Nightmare)
                spellInfo->Effects[EFFECT_0].Amplitude = 3500;
                break;
            case 204109: ///< Shades of Taerar (Dragons of Nightmare)
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_DEST_CASTER_LEFT;
                break;
            case 205300: ///< Corruption (Xavius)
            case 221059: ///< Wave of Decay (Nythendra)
                spellInfo->InterruptFlags |= SPELL_INTERRUPT_FLAG_INTERRUPT;
                break;
            case 197942: ///< Rend Flesh (Ursoc)
                spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(10); ///< 30y
                break;
            case 206005: ///< Dream Simulacrum (Xavius)
                spellInfo->AuraInterruptFlags &= ~AURA_INTERRUPT_FLAG_LEAVE_COMBAT;
                spellInfo->AuraInterruptFlags |= AURA_INTERRUPT_FLAG_CHANGE_MAP;
                break;
            case 214275: ///< Feeding Time - Stalker summon (Elerethe Renferal)
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            case 208931: ///< Nightmare Corruption (Il'gynoth)
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(21); ///< -1s
                break;
            case 210048: ///< Nightmare Explosion (Il'gynoth)
            {
                /// The Nightmare Ichors' Nightmare Explosion will now damage the Eye of Il'gynoth for 6% of its health instead of 5%, to shorten Phase 1
                if (difficulty == Difficulty::DifficultyRaidMythic)
                    spellInfo->Effects[EFFECT_0].BasePoints = 6;

                spellInfo->Effects[EFFECT_0].RadiusEntry = sSpellRadiusStore.LookupEntry(18); ///< 15y
                break;
            }
            case 209471: ///< Nightmare Explosion (Il'gynoth)
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(35);
                break;
            case 203102: ///< Mark of Ysondre (Dragons of Nightmare)
            case 203121: ///< Mark of Taerar (Dragons of Nightmare)
            case 203124: ///< Mark of Lethon (Dragons of Nightmare)
            case 203125: ///< Mark of Emeriss (Dragons of Nightmare)
                if (difficulty == Difficulty::DifficultyRaidMythic)
                    spellInfo->Effects[EFFECT_1].RadiusEntry = sSpellRadiusStore.LookupEntry(48); ///< 60y
                break;
            case 218124: ///< Violent Winds (Elerethe Renferal)
                spellInfo->AttributesEx5 &= ~SPELL_ATTR5_START_PERIODIC_AT_APPLY;
                break;
            case 225149: ///< Violent Winds (Elrethe Renferal)
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ALLY;
                break;
            case 189448: ///< Nightmare Reverberations (Xavius)
                spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_DAMAGE_FROM_MAX_HEALTH_PCT;
                spellInfo->Effects[EFFECT_0].BasePoints = 10;
                break;
            case 214456: ///< Swarm - Dmg + haste (Elerethe Renferal)
            case 225202: ///< Swarm - Scale (Elerethe Renferal)
                spellInfo->StackAmount = 0;
                spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                break;
            case 210781: ///< Dark Reconstitution (Il'gynoth)
                spellInfo->CastTimeEntry = sSpellCastTimesStore.LookupEntry(333); ///< 55s
                break;
            case 205294: ///< Essence of Corruption (Dragons of Nightmare)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_SRC_CASTER;
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_DEST_CASTER_RANDOM;
                break;
            case 211137: ///< Foul Winds (Elerethe Renferal)
                spellInfo->AttributesEx3 &= ~SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                break;
            case 213936: ///< Dark Storm (Elerethe Renferal)
                spellInfo->AttributesEx6 &= ~SPELL_ATTR6_NO_DONE_PCT_DAMAGE_MODS;
                break;
            case 215233: ///< Nightmarish Fury (Il'gynoth)
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(21); ///< -1
                break;
            case 211634: ///< The Infinite Dark (Xavius)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_DONT_RESET_PERIODIC_TIMER;
                break;
            case 214513: ///< Crimson Moon (Dragons of Nightmare)
                spellInfo->AttributesEx2 |= SPELL_ATTR2_NOT_RESET_AUTO_ACTIONS;
                break;
            case 214540: ///< Collapsing Nightmare (Dragons of Nightmare)
                spellInfo->InterruptFlags |= SPELL_INTERRUPT_FLAG_INTERRUPT;
                spellInfo->AttributesEx2 |= SPELL_ATTR2_NOT_RESET_AUTO_ACTIONS;
                break;
            case 210342: ///< Dread Thorns (Cenarius)
                spellInfo->AttributesEx6 |= SPELL_ATTR6_NO_DONE_PCT_DAMAGE_MODS;
                break;
            case 206308: ///< Corruption Meteor (Xavius)
                spellInfo->Effects[EFFECT_1].Effect = 0;
                break;
            case 226821: ///< Desiccating Stomp (Cenarius)
            case 213162: ///< Nightmare Blast (Cenarius)
                spellInfo->PreventionType = 0;
                spellInfo->InterruptFlags &= ~SPELL_INTERRUPT_FLAG_INTERRUPT;
                break;
            case 214529: ///< Spear of Nightmares (Cenarius)
                spellInfo->Attributes |= SPELL_ATTR0_IMPOSSIBLE_DODGE_PARRY_BLOCK;
                break;
            default:
                break;
        }
#pragma endregion

#pragma region HALLS_OF_VALOR
        switch (spellInfo->Id)
        {
            case 196828: ///< Fenryr Scent of Blood Damage buff
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(32); ///< 6s
                break;
            case 200901: ///< Eye of the Storm (Hyrja from HOV) Shouldn't tick on apply (retail checked)
                spellInfo->AttributesEx5 &= ~SPELL_ATTR5_START_PERIODIC_AT_APPLY;
                break;
            case 191478: ///< Ascendance (Hyrja)
            case 193991: ///< Aegis of Aggramar - Player drop (God King Skovald)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                break;
            case 193940: ///< Aegis of Aggramar - Summon (God King Skovald)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                break;
            case 192067: ///< Expel Light (Hyrja)
            case 192206: ///< Sanctify (Hyrja)
            case 193827: ///< Ragnarok - Damage (God King Skovald)
                spellInfo->Mechanic = 33;
                break;
            case 191976: ///< Arcing Bolt (Hyrja)
            case 200682: ///< Eye of the Storm (Hyrja)
                spellInfo->Mechanic = 34;
                break;
            case 198263: ///< Radiant Tempest (Odyn)
            case 198077: ///< Shatter Spears (Odyn)
            case 197961: ///< Runic Brand (Odyn)
                spellInfo->CasterAuraSpell = 0;
                break;
            case 196512: ///< Claw Frenzy (Fenryr)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_IGNORE_ARMOR;
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_SHARE_DAMAGE;
                break;
            case 192635: ///< Bifrost
                spellInfo->AuraInterruptFlags |= AURA_INTERRUPT_FLAG_CHANGE_MAP;
                break;
            case 214538: ///< Summon Bolas Launcher
            case 197968: ///< Runic Brand - AreaTrigger (Odyn)
            case 197971: ///< Runic Brand - AreaTrigger (Odyn)
            case 197972: ///< Runic Brand - AreaTrigger (Odyn)
            case 197975: ///< Runic Brand - AreaTrigger (Odyn)
            case 197977: ///< Runic Brand - AreaTrigger (Odyn)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DB;
                break;
            case 197556: ///< Ravenous Leap - Aura (Fenryr)
                spellInfo->Attributes |= SPELL_ATTR0_CANT_CANCEL;
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 193765: ///< Aegis of Aggramar - Root + AT (God-King Skovald)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                break;
            case 197996: ///< Branded (Odyn)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                spellInfo->Attributes |= SPELL_ATTR0_CANT_CANCEL;
                spellInfo->AttributesEx |= SPELL_ATTR1_NOT_BREAK_STEALTH;
                break;
            default:
                break;
        }
#pragma endregion

#pragma region VAULT_OF_THE_WARDENS
        switch (spellInfo->Id)
        {
            case 199132: ///< Jump Down (Tirathon Saltheril)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DB;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            case 193607: ///< Double Strike (Glayvianna Soulrender)
                spellInfo->Attributes |= SPELL_ATTR0_CANT_CANCEL;
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 191766: ///< Swoop - Jump (Glayvianna Soulrender)
                spellInfo->Effects[EFFECT_0].ValueMultiplier = 150.0f;
                spellInfo->Effects[EFFECT_0].MiscValueB = 100;
                break;
            case 193559: ///< Throw Glaive (Glayvianna Soulrender)
            case 193578: ///< Throw Glaive (Glayvianna Soulrender)
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                spellInfo->Effects[EFFECT_1].ApplyAuraName = SPELL_AURA_DUMMY;
                break;
            case 214957: ///< Inquisitive Stare (Inquisitor Tormentorum)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            case 212565: ///< Inquisitive Stare (Inquisitor Tormentorum)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ENEMY;
                spellInfo->Effects[EFFECT_1].TargetB = 0;
                spellInfo->Effects[EFFECT_2].TargetA = TARGET_UNIT_TARGET_ENEMY;
                spellInfo->Effects[EFFECT_2].TargetB = 0;
                break;
            case 199205: ///< Meteor - Damage (Dreadlord Mendacius)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_SHARE_DAMAGE;
                break;
            case 199172: ///< Charge (Grimguard)
                spellInfo->Effects[EFFECT_0].TriggerSpell = 199173;
                break;
            case 225814: ///< Rekindle (Blade Dancer Illianna)
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENTRY;
                spellInfo->Effects[EFFECT_1].TargetB = TARGET_UNIT_SRC_AREA_ENTRY;
                spellInfo->Effects[EFFECT_1].Effect = SPELL_EFFECT_FORCE_CAST;
                break;
            case 200202: ///< Chilled to the Bone (Ash'Golm)
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                break;
            case 203805: ///< Safety Net (Glazer)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_DEST_DEST;
                break;
            case 198165: ///< Close Tile (Glazer)
            case 197910: ///< Elune's Light - Orb AT
                spellInfo->Attributes |= SPELL_ATTR0_CASTABLE_WHILE_DEAD;
                break;
            case 194463: ///< Beam (Glazer)
            case 194468: ///< Beam (Glazer)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_ENNEMIES_IN_CYLINDER;
                break;
            case 202635: ///< Torment (Grimhorn the Enslaver)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_SHARE_DAMAGE;
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_SRC_CASTER;
                break;
            case 197250: ///< Turn Kick (Cordana Felsong)
            case 189469: ///< Turn Kick (Spirit of Vengeance)
                spellInfo->Effects[EFFECT_1].BasePoints = 75;
                spellInfo->Effects[EFFECT_1].MiscValue = 200;
                break;
            case 203416: ///< Shadowstep (Cordana Felsong)
                spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_TRIGGER_SPELL;
                spellInfo->Effects[EFFECT_0].TriggerSpell = spellInfo->Effects[EFFECT_0].BasePoints;
                break;
            case 197333: ///< Fel Glaive (Cordana Felsong)
                spellInfo->Effects[EFFECT_0].Amplitude = 3000;
                break;
            case 196249: ///< Meteor (Dreadlord Mendacius)
                spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_TRIGGER_SPELL;
                break;
            case 191767: ///< Swoop - Damage (Glayvianna Soulrender)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_ENNEMIES_IN_CYLINDER;
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_ENNEMIES_IN_CYLINDER;
                break;
            case 202826: ///< Fel Chain (Tirathon Saltheril)
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_ENNEMIES_IN_CYLINDER;
                break;
            case 202621: ///< Jailer's Cage (Grimhorn the Enslaver)
                spellInfo->Effects[EFFECT_2].Effect = 0;
                break;
            case 206019: ///< Corrupted Touch (Lingering Corruption)
                spellInfo->Speed = 0.0f;
                break;
            case 194035: ///< Mortal (Foul Mother)
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(21); ///< 35y
                break;
            case 193956: ///< Barbed Web
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_DONT_RESET_PERIODIC_TIMER;
                break;
            case 197258: ///< Fel Glaive (Cordana Felsong)
            case 197328: ///< Fel Glaive (Cordana Felsong)
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(1); ///< 10s
                break;
            default:
                break;
        }
#pragma endregion

#pragma region THE_NIGHTHOLD
        switch (spellInfo->Id)
        {
            case 213281: ///< Pyroblast - Spellblade Aluriel - Fiery Add
            {
                if (difficulty == Difficulty::DifficultyRaidMythic)
                    spellInfo->CastTimeEntry = sSpellCastTimesStore.LookupEntry(15); /// 4 sec
                break;
            }

            case 223243: ///< Plasma Sphere Dot - High Botanist Tel'arn
            case 218520: ///< Plasma Sphere Dot - High Botanist Tel'arn
            {
                spellInfo->Effects[EFFECT_0].BasePoints = 2;
                break;
            }
            case 213130: ///< Frozen Tempest - Spellblade Aluriel
                spellInfo->Effects[EFFECT_1].TriggerSpell = 0;
                break;
            case 206219: ///< Liquid Hellfire (Gul'dan)
            case 206220: ///< Empowered Liquid Hellfire (Gul'dan)
                spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_DUMMY;
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_SRC_CASTER;
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                break;
            case 206554: ///< Liquid Hellfire - Missile (Gul'dan)
                spellInfo->Effects[EFFECT_0].TriggerSpell = 206555;
                break;
            case 206586: ///< Empowered Liquid Hellfire - Missile (Gul'dan)
                spellInfo->Effects[EFFECT_0].TriggerSpell = 206581;
                break;
            case 225509: ///< Hungering Frenzy (Shambling Hungerer)
            case 227489: ///< The Eye of Aman'thul (Gul'dan)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                break;
            case 204975: ///< Chitinous Exoskeleton (Skorpyron)
                spellInfo->StackAmount = 6;
                break;
            case 205200: ///< Arcanoslash Dummy (Skorpyron)
            case 204468: ///< Focused Blast (Skorpyron)
            case 204371: ///< Call of the Scorpid (Skorpyron)
            case 221160: ///< Compress the Void (Chaotid)
            case 221172: ///< Scatter (Fulminant)
            case 221173: ///< Scatter (Fulminant)
                spellInfo->Effects[EFFECT_1].Effect = 0;
                break;
            case 206609: ///< Time Release (Chronomatic Anomaly)
            case 206617: ///< Time Bomb (Chronomatic Anomaly)
            case 223958: ///< Sludge Eruption (Sludgerax)
            case 209454: ///< Eye of Gul'dan (Gul'dan)
                spellInfo->Attributes |= SPELL_ATTR0_CANT_CANCEL;
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 219823: ///< Power Overwhelming (Chronomatic Anomaly)
                spellInfo->AttributesEx3 &= ~SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                break;
            case 221486: ///< Purified Essence (Gul'dan)
                spellInfo->AttributesEx3 &= ~SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                spellInfo->Effects[EFFECT_2].Effect = SPELL_EFFECT_DUMMY;
                break;
            case 223539: ///< Vile Sludge (Putrid Sludge)
            case 226205: ///< Slime Pool (Slime Pool)
            case 223718: ///< Bursting Slime (Sludgerax)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ANY;
                break;
            case 206488: ///< Arcane Seepage (Trilliax)
                spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(6); ///< 100y
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ANY;
                break;
            case 218342: ///< Parasitic Fixate - High Botanis Tel'arn
                spellInfo->Effects[EFFECT_0].ApplyAuraName = 0;
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                spellInfo->AuraInterruptFlags |= (AURA_INTERRUPT_FLAG_CHANGE_MAP | AURA_INTERRUPT_FLAG_LEAVE_COMBAT | AURA_INTERRUPT_FLAG_TELEPORTED);
                break;
            case 211927: ///< Power Overwhelming (Chronomatic Anomaly)
                spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                break;
            case 208952: ///< Ice Phase SkyBox - Star Augur Etraeus
            case 208955: ///< Fel Phase Skybox - Star Augur Etraeus
            case 208957: ///< Void Phase Skybox - Star Augur Etraeus
                spellInfo->Attributes |= (SPELL_ATTR0_HIDDEN_CLIENTSIDE | SPELL_ATTR0_HIDE_IN_COMBAT_LOG | SPELL_ATTR0_PASSIVE);
                spellInfo->AttributesEx |= SPELL_ATTR1_DONT_DISPLAY_IN_AURA_BAR;
                break;
            case 209615: ///< Ablation
            case 209973: ///< Ablating Explosion
            case 211887: ///< Ablated
            case 211261: ///< Permeliative Torment (Elisande)
            case 213148: ///< Searing Brand Fixate - Spellblade Aluriel
            case 221606: ///< Flames of Sargeras (Gul'dan)
            case 221603: ///< Flames of Sargeras (Gul'dan)
            case 222101: ///< Seed of Flame - Felweawer Pharamere's
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 215458: ///< Annihilate Debuff - Spellblade Aluriel
                spellInfo->AttributesEx3 |= SPELL_ATTR3_IGNORE_HIT_RESULT;
                break;
            case 213166: ///< Searing Brand Dot - Spellblade Aluriel
            {
                spellInfo->Attributes |= SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY;
                spellInfo->AttributesEx |= SPELL_ATTR1_UNAFFECTED_BY_SCHOOL_IMMUNE;
                break;
            }
            case 206954: ///< Frost Nova - Star Augur Etraeus
            case 187960: ///< Fel Beam Channel - Krosus Intro
            case 223668: ///< Oozing Rush (Sludgerax)
                spellInfo->Effects[EFFECT_0].TargetA = SpellImplicitTargetInfo(TARGET_UNIT_TARGET_ANY);
                break;
            case 205370: ///< Krosus Fel Beam (Left)
                spellInfo->Effects._effects.push_back(SpellEffectInfo(sSpellStore.LookupEntry(spellInfo->Id), spellInfo, EFFECT_1, Difficulty::DifficultyNone));
                spellInfo->Effects[EFFECT_1].Effect = SPELL_EFFECT_TRIGGER_SPELL;
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_NEARBY_ENTRY;
                spellInfo->Effects[EFFECT_1].TriggerSpell = 205391;
                spellInfo->EffectCount++;
                break;
            case 217054: ///< Devouring Remmant - Remmant of the Void
            case 224632: ///< Heavenly Crash - Astrologer Jarin
            case 206340: ///< Bonds of Fel (Gul'dan)
            case 206370: ///< Empowered Bonds of Fel (Gul'dan)
            case 230403: ///< Fel Lash (Spellblade Aluriel)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_SHARE_DAMAGE;
                break;
            case 212109: ///< Temporal Smash (Chronomatic Anomaly)
                spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_JUMP_DEST;
                spellInfo->Effects[EFFECT_0].MiscValue = 0;
                spellInfo->Effects[EFFECT_0].MiscValueB = 150;
                spellInfo->Effects[EFFECT_0].ValueMultiplier = 63;
                break;
            case 206978: ///< The Eye of Aman'thul - Summon LOS Blocker (Gul'dan)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DB;
                break;
            case 205649: /// Fel Ejection - Star Augur Etraeus
                spellInfo->MaxAffectedTargets = 3;
                spellInfo->AttributesEx3 |= SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                break;
            case 213326: ///< Detonate Arcane Orb Areatrigger - Spellblade Aluriel
            case 207620: ///< Annihilation (Trilliax)
            case 221436: ///< Demonic Essence (Gul'dan)
                spellInfo->Effects[EFFECT_0].TargetA = SpellImplicitTargetInfo(TARGET_DEST_DEST);
                spellInfo->Effects[EFFECT_1].TargetB = 0;
                break;
            case 219340: ///< Parasitic Fetter Root - High Botanist Tel'arn
                spellInfo->Mechanic = 0;
                break;
            case 206953: ///< Frigid Nova - Star Augur Etraeus
            case 206517: ///< Fel Nova - Star Augur Etraeus
            case 207439: ///< Void Nova - Star Augur Etraeus
            case 206464: ///< Coronal Ejection
            case 206936: ///< Icy Ejection
            case 207143: ///< Void Ejection
            case 221189: ///< Scatter (Fulminant)
            case 219984: ///< Burst of Time (Chronomatic Anomaly)
            case 206610: ///< Time Release (Chronomatic Anomaly)
                spellInfo->AttributesEx3 |= SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                break;
            case 209518: ///< Eye of Gul'dan (Gul'dan)
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_CASTER;
                break;
            case 206366: ///< Empowered Bonds of Fel (Gul'dan)
                spellInfo->Effects[EFFECT_0].Effect = 0;
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[EFFECT_2].TargetA = TARGET_UNIT_TARGET_ANY;
                break;
            case 206939: ///< Well of Souls (Gul'dan)
                spellInfo->CastTimeEntry = sSpellCastTimesStore.LookupEntry(32); ///< 7s
                break;
            case 217766: ///< Soul Siphon (Gul'dan)
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(568); ///< 1.25s
                break;
            case 206446: ///< Soul Siphon (Gul'dan)
                spellInfo->ExcludeTargetAuraSpell = 221891;
                break;
            case 161121: ///< Storm of the Destroyer (Gul'dan)
                spellInfo->Effects[EFFECT_0].Effect = 0;
                spellInfo->Effects[EFFECT_3].Effect = 0;
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(38); ///< 11s
                break;
            case 167819: ///< Storm of the Destroyer (Gul'dan)
            case 167935: ///< Storm of the Destroyer (Gul'dan)
            case 177380: ///< Storm of the Destroyer (Gul'dan)
                spellInfo->Effects[EFFECT_1].Effect = 0;
                spellInfo->Effects[EFFECT_2].Effect = 0;
                break;
            case 32939:  ///< Phase Burst (Gul'dan)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            case 227300: ///< Arcanetic Eruption (Gul'dan)
                spellInfo->Effects[EFFECT_1].BasePoints = 100;
                break;
            case 206985: ///< Scattering Field (Gul'dan)
                spellInfo->Effects[EFFECT_1].Effect = SPELL_EFFECT_DUMMY;
                break;
            case 171458: ///< The Eye of Aman'thul
                if (difficulty == Difficulty::DifficultyRaidMythic)
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(21); ///< -1s
                break;
            case 221731: ///< Empowered Eye of Gul'dan (Gul'dan)
                spellInfo->MaxAffectedTargets = 6;
                break;
            case 221864: ///< Blast (Recursive Elemental (Elisande))
                spellInfo->InterruptFlags |= SPELL_INTERRUPT_FLAG_INTERRUPT;
                break;
            case 227661: ///< Arcane Blast (Archmage Khadgar)
            case 227662: ///< Arcane Barrage (Archmage Khadgar)
            case 227663: ///< Arcane Missiles (Archmage Khadgar)
                spellInfo->ExcludeTargetAuraSpell = 206500; ///< Gul'dan fade out
                break;
            case 227008: ///< Visions of the Dark Titan (Gul'dan)
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(8); ///< 15s
                break;
            case 226980: ///< Visions of the Dark Titan (Gul'dan)
                spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_DUMMY;
                spellInfo->Effects[EFFECT_1].Effect = SPELL_EFFECT_DUMMY;
                break;
            case 198294: ///< Valid Target (Gul'dan)
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(31); ///< 8s
                break;
            case 214882: ///< Dual Personalities - Cleaner (Trilliax)
            case 215066: ///< Dual Personalities - Maniac (Trilliax)
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ANY;
                break;
            case 213569: ///< Armaggedon - Spellblade Aluriel
            {
                if (difficulty == Difficulty::DifficultyRaidMythic)
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(8); ///< 15s

                break;
            }
            case 224638: ///< Heavenly Crash - Astrologer Jarin
            {
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_SHARE_DAMAGE | SPELL_ATTR0_CU_SHARE_PLAYER_DAMAGE;
                break;
            }
            case 221487: ///< Call to the Stars
            {
                spellInfo->InterruptFlags |= SPELL_INTERRUPT_FLAG_INTERRUPT;
                spellInfo->ChannelInterruptFlags |= CHANNEL_INTERRUPT_FLAG_INTERRUPT;
                spellInfo->AuraInterruptFlags |= AURA_INTERRUPT_FLAG_TRANSFORM | AURA_INTERRUPT_FLAG_MOVE;
                break;
            }
            case 212647: /// Frostbitten - Spellblade Aluriel
            {
                if (difficulty != Difficulty::DifficultyRaidMythic)
                    spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(35); //4s
                break;
            }
            case 227551: ///< Fel Scythe - Gul'dan
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_TARGET_ENEMY;
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_DEST_AREA_ENEMY;
                break;
            default:
                break;
        }
#pragma endregion

#pragma region TRIAL_OF_VALOR
        switch (spellInfo->Id)
        {
            case 228890: ///< Salt Spray (Deepbrine Monstruosity)
            case 228891: ///< Frigid Spray (Deepbrine Monstruosity)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_SHARE_DAMAGE | SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 232497: ///< Orb of Corruption (Helya)
            case 232496: ///< Orb of Corrosion (Helya)
            case 231167: ///< Sky Jump
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            case 229119: ///< Orb of Corruption (Helya)
            case 230267: ///< Orb of Corrosion (Helya)
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_FIXATE;
                break;
            case 228061: ///< Orb of Corrosion (Helya)
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(31); ///< 8s
                break;
            case 157932: ///< Corrupted Slicer (Helya)
            case 231420: ///< Corrosive Slicer (Helya)
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(21); ///< -1s
                break;
            case 228032: ///< Fury of the Maw (Helya)
                spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_TRIGGER_SPELL;
                break;
            case 228730: ///< Tentacle Strike (Helya)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_SHARE_DAMAGE;
                break;
            case 228854: ///< Mist Infusion (Helya)
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                spellInfo->Effects[EFFECT_1].Effect = 0;
                spellInfo->Effects[EFFECT_2].Effect = 0;
                break;
            case 230990: ///< Unerring Blast Odyn  - 114263
            case 231278: ///< Unerring Blast Odyn  - 114263
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CANT_CRIT;
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                break;
            case 227490: ///< Branded Odyn  - 114263
            case 227491: ///< Branded Odyn  - 114263
            case 227499: ///< Branded Odyn  - 114263
            case 227500: ///< Branded Odyn  - 114263
            case 227498: ///< Branded Odyn  - 114263
            case 228270: ///< Shield of Light Hyrja  - 114360
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 231342: ///< Protection Brand Odyn  - 114263
            case 231311: ///< Protection Brand Odyn  - 114263
            case 231344: ///< Protection Brand Odyn  - 114263
            case 231345: ///< Protection Brand Odyn  - 114263
            case 231346: ///< Protection Brand Odyn  - 114263
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(4); ///< 120s
                break;
            case 227961: ///< HC brand Odyn  - 114263
            case 227973: ///< HC brand Odyn  - 114263
            case 227974: ///< HC brand Odyn  - 114263
            case 227975: ///< HC brand Odyn  - 114263
            case 227976: ///< HC brand Odyn  - 114263
                spellInfo->Attributes |= SPELL_ATTR0_CANT_CANCEL;
                break;
            case 192048: ///< Expel Light Hyrja  - 114360
            case 230356: ///< Fury of the Maw (Helya)
            case 228125: ///< Mistcaster (Helya)
            {
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            }
            case 227720: ///< Mixed Elements, Guarm
            case 227721: ///< Mixed Elements, Guarm
            case 227735: ///< Mixed Elements, Guarm
            {
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            }
            case 198060: ///< Spear of light Odyn  - 114263
            {
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[EFFECT_1].TargetB = 0;
                break;
            }
            case 192018: ///< Shield of Light  Hyrja  - 114360
            {
                spellInfo->Effects[EFFECT_0].MiscValue = 80;
                break;
            }
            case 227570: ///< Dark Discharge  Guarm - 114323
            case 227539: ///< Fiery Phelgem Guarm - 114323
            case 227566: ///< Salty Spittle Guarm - 114323
            {
    ///         spellInfo->m_SpellVisuals.clear();
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_SHARE_DAMAGE;
                break;
            }
            case 228803: ///< Building Storm (Helya)
                spellInfo->Effects[EFFECT_0].MiscValue = Mechanics::MECHANIC_UNK_33;
                spellInfo->Effects[EFFECT_0].Mechanic = Mechanics::MECHANIC_UNK_33;
                break;
            case 234189: ///< Fixate (Helya)
                spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                break;
            case 232580: ///< Odyn Teleport (Odyn outro)
                spellInfo->Effects[EFFECT_0].Effect = 0;
                break;
            case 228007: ///< Dancing Blade (Odyn)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(6); ///< 100y
                break;
            case 231862: ///< Bleak Eruption (Helya)
                spellInfo->Effects[EFFECT_0].ApplyAuraName = 0;
                break;
            case 231854: ///< Unchecked Rage (Harjatan)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_SHARE_DAMAGE;
                break;
            case 240580: ///< Serpent Rush
                spellInfo->Effects[0].TriggerSpell = 0;
                break;
            case 241179: ///< Grand Cleave
                spellInfo->SchoolMask = SPELL_SCHOOL_MASK_NATURE;
                break;
            case 231395: ///< Burning Eruption (Goroth)
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_CASTER;
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[1].TriggerSpell = 0;
                break;
            case 233019: ///< Infernal Spike (Goroth)
                spellInfo->Attributes &= ~SPELL_ATTR0_HIDDEN_CLIENTSIDE;
                break;
            case 231768: ///< Drenching Waters (Harjatan)
                spellInfo->Effects[1].TriggerSpell = 0;
                break;
            case 233429: ///< Frigid Blows (Harjatan)
                spellInfo->ProcCharges = 0;
                break;
            case 234346: ///< Fel Eruption Missile(Goroth)
                spellInfo->Effects[EFFECT_0].Amplitude = 500;
                break;
            case 234348: ///< Fel Eruption Missile(Goroth)
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(162); ///< min: 10, max: 25 yards
                break;
            case 234381: ///< Fel Eruption Missile(Goroth)
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(172); ///< min: 30, max: 40 yards
                spellInfo->Effects[EFFECT_0].Amplitude = 500;
                break;
            case 234380: ///< Fel Periodic Trigger(Goroth)
                spellInfo->Effects[EFFECT_0].Amplitude = 500;
                break;
            case 232061: ///< Draw In (Harjatan)
                spellInfo->AuraInterruptFlags &= ~AURA_INTERRUPT_FLAG_MELEE_ATTACK;
                break;
            case 230920: ///< Consuming Hunger (Sasszine)
                spellInfo->AttributesCu |= (SPELL_ATTR0_CU_NEGATIVE | SPELL_ATTR0_CU_IGNORE_ARMOR);
                break;
            case 232732: ///< Slicing Tornado (Sasszine)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_IGNORE_ARMOR;
                break;
            case 230143: ///< Hydra Shot Damage (Sasszine)
                spellInfo->AttributesCu |= (SPELL_ATTR0_CU_IGNORE_ARMOR | SPELL_ATTR0_CU_IS_CUSTOM_AOE_SPELL);
                spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_DONE_BONUS;
                break;
            case 234264: ///< Melted Armor
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_CASTER;
                break;
            case 248713: ///< Soul Corruption (Demonic Inquisition)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 231998: ///< Jagged Abrasion (Harjatan)
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                break;
            case 233520: ///< Frigid Blows (Harjatan)
            case 239358: ///< Anguished Outburst (Demonic Inquisition)
                spellInfo->AttributesEx3 |= SPELL_ATTR3_IGNORE_HIT_RESULT;
                break;
            case 233432: ///< Calcified Quills (Demonic Inquisition)
                spellInfo->Speed = 0.0f;
                break;
            case 233435: ///< Calcified Quills (Demonic Inquisition)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_IGNORE_ARMOR;
                spellInfo->AttributesEx3 |= SPELL_ATTR3_IGNORE_HIT_RESULT;
                break;
            case 236283: ///< Belac Prisoner (Demonic Inquisition)
                spellInfo->AuraInterruptFlags |= (AURA_INTERRUPT_FLAG_CHANGE_MAP | AURA_INTERRUPT_FLAG_LEAVE_COMBAT | AURA_INTERRUPT_FLAG_TELEPORTED);
                break;
            case 233431: ///< Calcified Quills (Demonic Inquisition)
                spellInfo->ChannelInterruptFlags = 0;
                break;
            case 233430: ///< Unbearable Torment (Demonic Inquisition)
                spellInfo->Effects[4].BasePoints = 90;
                break;
            case 241590: ///< Tantrum (Harjatan)
                spellInfo->AttributesEx &= ~SPELL_ATTR1_CHANNELED_2;
                spellInfo->ChannelInterruptFlags = 0;
                break;
            case 241455: ///< Fel Fire (Goroth)
                spellInfo->PreventionType = 0;
                break;
            case 238587: ///< Rain of Brimstone (Goroth)
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_8_YARDS);
                break;
            case 235230: ///< Fel Squall (Demonic Inquisition)
                spellInfo->ChannelInterruptFlags = 0;
                break;
            case 234016: ///< Driven Assault (Harjatan)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 241600: ///< Sickly Fixate (Harjatan)
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_FIXATE;
                break;
            case 247782: ///< Enraged (Harjatan)
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_15_YARDS);
                break;
            case 241713: ///< Spontaneous Fragmentation Fixate (Demonic Inquisition)
                spellInfo->AttributesEx &= ~SPELL_ATTR1_CHANNELED_1;
                break;
            default:
                break;
        }
#pragma endregion

#pragma region TOMB_OF_SARGERAS
        switch (spellInfo->Id)
        {
            case 234565: ///< Light Moon AreaTrigger - Sisters of The Moon
            case 234657: ///< Dark Moon Areatrigger - Sisters of The Moon
            case 234587:
            case 234659:
            {
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_CASTER;
                break;
            }
            case 236529: ///< Twilight Glaive - Sisters of the Moon
            {
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ANY;
                spellInfo->Effects[EFFECT_1].Effect = SPELL_EFFECT_APPLY_AURA;
                spellInfo->Effects[EFFECT_1].ApplyAuraName = SPELL_AURA_AREATRIGGER;
                break;
            }
            case 224720: ///< Teleport - Sisters of the Moon
            case 239267: ///< Flaming Detonation - Flaming Orb
            {
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_DEST_DEST;
                break;
            }
            case 236306: /// Incorporeal Shot - Sisters of the Moon
            {
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_SHARE_DAMAGE;
                break;
            }
            case 236235: ///< Soul Rot - Desolate Host
            {
                spellInfo->ExcludeCasterAuraSpell = 0;
                spellInfo->ExcludeTargetAuraSpell = 0;
                spellInfo->CasterAuraSpell = 0;
                spellInfo->TargetAuraSpell = 0;
                break;
            }
            case 236075: ///< Wailing Souls - Desolate Host
            {
                spellInfo->AttributesEx &= ~SPELL_ATTR1_CHANNELED_2;
                spellInfo->ChannelInterruptFlags = 0;
                spellInfo->AttributesEx5 &= SPELL_ATTR5_DONT_SHOW_AURA_IF_SELF_CAST;
                break;
            }
            case 235988: ///< Tormented Cries - Desolate Host
            {
                spellInfo->Attributes &= ~SPELL_ATTR0_HIDDEN_CLIENTSIDE;
                break;
            }
            case 236515: ///< Shattering Scream - Desolate Host
            {
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_TARGET_ENEMY;
                spellInfo->Effects[EFFECT_2].TargetA = TARGET_UNIT_TARGET_ENEMY;
                break;
            }
            case 236454: /// Soulbind - Soul Queen Dehjanna
            case 236455: /// Soulbind - Soul Queen Dehjanna
            {
                spellInfo->TargetAuraSpell = 0;
                break;
            }
            case 240317: ///< Diminishing Soul - Desolate Host
            {
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                break;
            }
            case 240358: ///< Diminishing Soul AOE - Desolate Host
            case 240356: ///< Diminishing Soul AOE - Desolate Host
            {
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_5_YARDS);
                spellInfo->MaxAffectedTargets = 1;
                break;
            }
            case 236459: ///< Soulbind Periodic - Soul Queen Dejahna
            {
                spellInfo->Effects[EFFECT_0].Amplitude = 2 * IN_MILLISECONDS;
                spellInfo->AuraInterruptFlags |= AURA_INTERRUPT_FLAG_LEAVE_COMBAT;
                break;
            }
            case 236541: ///< Soul Rot - Desolate Host
            {
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(21);
                break;
            }

            case 240359: ///< Diminishing Soul - Desolate Host
            {
                spellInfo->AuraInterruptFlags |= (AURA_INTERRUPT_FLAG_CHANGE_MAP | AURA_INTERRUPT_FLAG_LEAVE_COMBAT);
                break;
            }

            case 235734: /// Spiritual Barrier - Desolate Host
            case 235732: /// Spiritual Barrier - Desolate Host
            case 235621: /// Spirit Realm - Desolate Host
            {
                spellInfo->AttributesEx3 &= ~SPELL_ATTR3_DEATH_PERSISTENT;
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                break;
            }
            case 242796:///< Spear of Anguish Mythic - Desolate Host
            {
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_SHARE_DAMAGE;
                spellInfo->Effects[EFFECT_2].TriggerSpell = 0;
                break;
            }
            case 236563: ///< Sundering Doom - Desolate Host
            case 236567: ///< Doomed Sundering - Desolate Host
            {
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_SHARE_DAMAGE;
                break;
            }
            case 235933: ///< Spear of Anguish - Desolate Host
            {
                spellInfo->Effects[EFFECT_2].TriggerSpell = 0;
                break;
            }
            case 235989: /// Tormented Cries Areatrigger - Desolate Host
            {
                spellInfo->Effects[EFFECT_2].TargetA = TARGET_UNIT_CASTER;
                break;
            }
            case 243299: ///< Agony - Shadow Council Warlock
            {
                spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                break;
            }
            case 238502: ///< Focused Dreadflame - Kil'jaeden
            case 239742: ///< Dark Mark (Damage) - Fallen Avatar
            case 239058: ///< Touch Of Sargeras (Soaked Damage) - Fallen Avatar
            case 239439: ///< Obliterating Smash - Eonic Defender
            case 235569: ///< Hammer of Creation - Maiden of Vigilance
            case 235573: ///< Hammer of Obliteration - Maiden of Vigilance
            {
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_SHARE_DAMAGE;
                break;
            }
            case 236494: ///< Desolate - Fallen Avatar
            {
                spellInfo->Attributes |= SPELL_ATTR0_IMPOSSIBLE_DODGE_PARRY_BLOCK;
                break;
            }
            case 238999: ///< Darkness of a Thousand Souls - Kil'jaeden
            case 237590: ///< Shadow Reflection: Hopeless - Kil'jaeden
            case 238429: ///< Bursting Dreadflame - Kil'jaeden
            case 236710: ///< Shadow Reflection: Erupting - Kil'jaeden
            {
                spellInfo->AttributesEx3 |= SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                break;
            }
            case 239932: ///< Felclaws - Kil'jaeden
            {
                spellInfo->ProcFlags &= ~PROC_FLAG_DONE_SPELL_MELEE_DMG_CLASS;
                break;
            }
            case 239216: ///< Darkness of a Thousand Souls - Kil'jaeden
            {
                spellInfo->AttributesEx |= SPELL_ATTR1_UNAFFECTED_BY_SCHOOL_IMMUNE;
                break;
            }
            case 235133: ///< Unstable Soul (AoE) - Maiden of Vigilance
            case 254062: ///< Unstable Soul - Maiden of Vigilance
            {
                spellInfo->Attributes |= SPELL_ATTR0_UNAFFECTED_BY_INVULNERABILITY;
                break;
            }
            case 239931: ///< Felclaws - Kil'jaeden
            case 245509: ///< Felclaws (Cone + buff) - Kil'jaeden
            case 240916: ///< Armageddon Hail (AoE) - Kil'jaeden
            {
                spellInfo->Attributes |= SPELL_ATTR0_IMPOSSIBLE_DODGE_PARRY_BLOCK;
                break;
            }
            case 239280: ///< Fixate - Flaming Orb
            {
                spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(199); ///< 300y
                spellInfo->AttributesEx3 |= SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                break;
            }
            case 245017: ///< Chaotic Eruption - Kil'jaeden
            {
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_DEST_CASTER;
                spellInfo->Effects[EFFECT_1].TargetB = TARGET_UNIT_DEST_AREA_ALLY;
                break;
            }
            case 243625: ///< Wailing Grasp (Main) - Kil'jaeden
            {
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_CASTER;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_DEST_CASTER;
                spellInfo->Effects[EFFECT_1].TargetB = TARGET_UNIT_DEST_AREA_ALLY;
                break;
            }
            case 243626: ///< Wailing Grasp (Other) - Kil'jaeden
            {
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            }
            case 235240: ///< Fel Infusion - Maiden of Vigilance
            case 235213: ///< Light Infusion - Maiden of Vigilance
            case 241315: ///< Light Infusion (AreaTrigger) - Maiden of Vigilance
            case 241316: ///< Fel Infusion (AreaTrigger) - Maiden of Vigilance
            {
                spellInfo->AttributesEx3 &= ~SPELL_ATTR3_DEATH_PERSISTENT;
                break;
            }
            case 258803: ///< Argus Filter
            {
                spellInfo->AuraInterruptFlags = 0;
                break;
            }
            default:
                break;
        }
#pragma endregion

#pragma region CATHEDRAL_OF_ETERNAL_NIGHT
        switch (spellInfo->Id)
        {
            case 239156: /// Book Of Eternal winter
            {
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_TARGET_ENEMY;
                break;
            }
            case 236524: /// Poisonous Spores - Agronox
            {
                spellInfo->AttributesEx3 |= SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                break;
            }

            case 237726: /// Scornful Charge - Trashbite the Scornful
            {
                spellInfo->ExcludeCasterAuraSpell = 0;
                break;
            }

            case 238656: /// Shadow Wave Dot - Dul'zak
            case 241937: /// Shadow Wall
            {
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(21);
                break;
            }

            case 236592: /// Fulminating Lashers - Agronox
            {
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                break;
            }

            case 240948: /// Destructive Rampage - Trashbite the Scornful
            {
                spellInfo->InterruptFlags &= ~SPELL_INTERRUPT_FLAG_MOVEMENT;
                break;
            }

            case 233963: /// Demonic Upheaval - Mephistroth
            {
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            }

            case 240710: /// Leap - Illidan Stormrage
            {
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                break;
            }

            case 233206: /// Shadowfade - Mephistroth
            {
                spellInfo->InterruptFlags = 0;
                spellInfo->AuraInterruptFlags = 0;

                for (auto l_Itr : spellInfo->SpellPowers)
                {
                    if (auto l_Ptr = const_cast<SpellPowerEntry*>(l_Itr))
                        l_Ptr->Cost = 0;
                }
                break;
            }

            case 234107: /// Chaotic Energies - Domatrax
            {
                for (auto l_Itr : spellInfo->SpellPowers)
                {
                    if (auto l_Ptr = const_cast<SpellPowerEntry*>(l_Itr))
                        l_Ptr->Cost = 0;
                }

                break;
            }

            case 236233: /// Shadow Blast Areatrigger - Mephistroth
            {
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_DEST_CASTER_FRONT;
                spellInfo->Effects[EFFECT_1].TargetB = 0;
                break;
            }

            case 243124: /// Heave Cudgel - Trashbite the Scornful
            {
                spellInfo->ExcludeCasterAuraSpell = 0;
                spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(187);
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            }

            case 234083: /// Aegis of Aggramar - Mephistroth
            {
                spellInfo->AuraInterruptFlags = 0;
                break;
            }

            case 238653: /// Shadow Wave Cast - Dul'zak
            case 236543: /// Felsoul Cleave - Domatrax
            {
                spellInfo->AttributesEx5 |= SPELL_ATTR5_DONT_TURN_DURING_CAST;
                break;
            }

            case 236740: /// Shadow Fangs - Trashmob
            {
                spellInfo->Effects[EFFECT_1].TriggerSpell = 0;
                break;
            }

            case 243256: /// Mildly Amused
            case 243258: /// Moderately Stirred
            case 243259: /// Fairly Excited
            case 243261: /// Totally Tantalized
            {
                spellInfo->AuraInterruptFlags = 0;
                break;
            }
            default: break;
        }

#pragma endregion

#pragma region SEAT_OF_THRIUMVIRATE
        switch (spellInfo->Id)
        {
            case 247145: ///< Hunter's Rush - Saprish
            {
                spellInfo->ProcCharges = 0;
                break;
            }
            case 248411: ///< Unstable Trap - Trashmob
            case 246860: ///< Wave Rift Missile - Trashmob
            {
                spellInfo->AttributesEx3 |= SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                break;
            }
            case 248184: ///< Darkflay - Trashmob
            case 244916: ///< Void Lashing
            {
                spellInfo->InterruptFlags |= SPELL_INTERRUPT_FLAG_INTERRUPT;
                spellInfo->ChannelInterruptFlags |= CHANNEL_INTERRUPT_FLAG_INTERRUPT;
                break;
            }
            case 244588: ///< Void Sludge - Zuraal
            case 244300: ///< Void Infusion - Zuraal
            {
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(21); ///< Infinite
                break;
            }
            case 244653: ///< Fixate Player Aura - Zuraal
            {
                spellInfo->AttributesEx &= SPELL_ATTR1_CHANNELED_1;
                break;
            }
            case 245802: ///< Ravaging Darkness - Darkfang
            {
                spellInfo->ChannelInterruptFlags = 0;
                break;
            }
            case 249058: ///< Remnant of Anguish AOE - L'ura
            {
                spellInfo->AttributesEx3 &= ~SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                spellInfo->Attributes |= SPELL_ATTR0_CASTABLE_WHILE_DEAD;
                break;
            }
            case 244061: ///< Void Realm - Zuraal the Ascended
            {
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                break;
            }
            case 247246: ///< Umbral Flanking - Saprish
            {
                spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                spellInfo->Effects[EFFECT_1].Effect = SPELL_EFFECT_CHARGE;
                spellInfo->Effects[EFFECT_1].TargetA = SpellImplicitTargetInfo(TARGET_UNIT_TARGET_ENEMY);
                spellInfo->Effects[EFFECT_1].TargetB = 0;
                spellInfo->Effects[EFFECT_1].TriggerSpell = 0;
                spellInfo->MaxAffectedTargets = 0;
                break;
            }
            case 249057: ///< Remnant of Anguish Trigger - L'ura
            {
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_TARGET_DEST;
                spellInfo->Effects[EFFECT_0].RadiusEntry = nullptr;
                break;
            }
            case 248831: ///< Dread Screech - Saprish
            {
                spellInfo->InterruptFlags |= SPELL_INTERRUPT_FLAG_INTERRUPT;
                break;
            }
            case 244621: ///< Void Tear (Stun AoE) - Zuraal the Ascended
            {
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_CASTER;
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_DEST_CASTER;
                spellInfo->Effects[EFFECT_2].TargetA = TARGET_DEST_CASTER;
                break;
            }
            default:
                break;
        }
#pragma endregion

#pragma region ANTORUS_THE_BURNING_THRONE
        switch (spellInfo->Id)
        {
            case 244399:
                spellInfo->MaxAffectedTargets = 0;
                break;
            case 245810: ///< Annihilation - Garothi Annihilator
            case 244761: ///< Annihilation - Garothi Worldbreaker
            case 246971: ///< Annihilation (Haywire) - Garothi Worldbreaker
            case 247044: ///< Shrapnel - Garothi Worldbreaker
            case 244583: ///< Siphoned - Shatug
            case 252762: ///< Demolish - Garothi Demolisher (Pre Kin'garoth)
            case 244033: ///< Aggramar - Flame Rend
            case 247079: ///< Aggramar - Empowered Flame Rend
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_SHARE_DAMAGE;
                break;
            case 257286: ///< Ravaging Storm - Golganneth's Vitality
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_SHARE_DAMAGE;
                spellInfo->AttributesEx3 &= ~SPELL_ATTR3_CANT_TRIGGER_PROC;
                break;
            case 246706: ///< Decimation - Kin'garoth
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_SHARE_DAMAGE;
                spellInfo->Effects[EFFECT_1].TriggerSpell = 0;
                break;
            case 253543: ///< Infernal Bombardment - Legion Devastator
            case 248334: ///< Meteor Storm - The Paraxis
                spellInfo->AttributesEx3 |= SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                break;
            case 243983: ///< Collapsing World (summon) - Portak Keeper Hasabel
            case 244946: ///< Felsilk Wrap (summon) - Lady Dacidion
            case 256674: ///< Golganneth's Wrath - Argus the Unmaker
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                break;
            case 244692: ///< Transport Portal (Summons Imp) - Transport Portal
            case 245485: ///< Transport Portal (Summons Stalker) - Transport Portal
            case 246192: ///< Transport Portal (Summons Skitterer) - Transport Portal
            case 244689: ///< Collapsing World (summon) - Portal Keeper Hasabel
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_DEST_CASTER_RANDOM;
                break;
            case 244661: ///< Everburning Flames (summon) - Players
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_DEST_DEST_RANDOM;
                break;
            case 244001: ///< Felstorm Barrage - Felcrush Portal
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_SRC_CASTER;
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_SRC_CASTER;
                spellInfo->Effects[EFFECT_1].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                break;
            case 254508: ///< Portal: Xoroth - Everburning Flames
            case 254510: ///< Portal: Rancora - Eternal Darkness
            case 254514: ///< Portal: Nathreza - ??
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(21); ///< -1s
                break;
            case 244607: ///< Flames of Xoroth - Vulcanar
            case 255805: ///< Unstable Portal - Vulcanar
            case 245504: ///< Howling Shadows - Hungering Stalker
            case 249196: ///< Mind Blast - Paraxis Inquisitor
            case 214003: ///< Coup De Grace - Risen Swordsman
            case 214001: ///< Risen's Dive - Risen Lancer
                spellInfo->InterruptFlags |= SPELL_INTERRUPT_FLAG_INTERRUPT;
                break;
            case 249155: ///< Torment - Paraxis Inquisitor
            case 249126: ///< Wrack - Paraxis Inquisitor
                spellInfo->InterruptFlags |= SPELL_INTERRUPT_FLAG_INTERRUPT;
                spellInfo->ChannelInterruptFlags |= CHANNEL_INTERRUPT_FLAG_INTERRUPT;
                break;
            case 245075: ///< Hungering Gloom - Portal Keeper Hasabel
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_CASTER;
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_DEST_CASTER;
                break;
            case 253438: ///< Cosmetic Life Beam - Daughter of Eonar
                spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(6); ///< 100y
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS;
                break;
            case 259304: ///< Dummy - Eonar
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_FIXATE;
                break;
            case 246951: ///< Spawn - Eonar
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[EFFECT_2].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[EFFECT_3].TargetA = TARGET_UNIT_CASTER;
                break;
            case 250467: ///< Life Force (AoE) - Eonar
                spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(174);  ///< 1000y
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(113);           ///< 1000y
                spellInfo->Effects[EFFECT_1].SetRadiusIndex(113);           ///< 1000y
                spellInfo->Effects[EFFECT_2].SetRadiusIndex(113);           ///< 1000y
                break;
            case 250030: ///< Life Force (Paraxis dmg) - Eonar
            case 246301: ///< Artillery Mode - Fel-Infused Destructor
                spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(174);  ///< 1000y
                break;
            case 246305: ///< Artillery Strike (Missile) - Fel-Infused Destructor
                spellInfo->InterruptFlags |= SPELL_INTERRUPT_FLAG_INTERRUPT;
                spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(174);  ///< 1000y
                break;
            case 250475: ///< Blue Beam - Eonar
            case 250523: ///< Red Beam - Eonar
            case 250524: ///< Green Beam - Eonar
            case 250525: ///< Yellow Beam - Eonar
                spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(245);  ///< 500y
                break;
            case 246310: ///< Artillery Strike (damage) - Eonar
                if (difficulty == Difficulty::DifficultyNone)
                    spellInfo->Effects[EFFECT_0].BasePoints = 50;

                spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(174);  ///< 1000y
                break;
            case 254269: ///< Life Force (Waves damages) - Eonar
                spellInfo->Speed = 0.0f;
                break;
            case 244578: ///< Siphon Corruption - Shatug
                spellInfo->ExcludeTargetAuraSpell = 248819; ///< Siphoned
                break;
            case 244471: ///< Enflame Corruption - F'harg
                spellInfo->ExcludeTargetAuraSpell = 248815; ///< Enflamed
                break;
            case 245098: ///< Decay - Shatug
                spellInfo->Effects[EFFECT_1].ApplyAuraName = SPELL_AURA_PERIODIC_DAMAGE;
                break;
            case 249156: ///< Torment - Paraxis Inquisitor
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_SRC_CASTER;
                spellInfo->Effects[EFFECT_1].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                spellInfo->Effects[EFFECT_1].SetRadiusIndex(12); ///< 100y
                break;
            case 245786: ///< Surge of Life (up) - Eonar the Lifebinder
                spellInfo->AuraInterruptFlags = AURA_INTERRUPT_FLAG_CHANGE_MAP;
                spellInfo->Effects[EFFECT_1].Effect = 0;
                break;
            case 254506: ///< Surge of Life (down) - Eonar the Lifebinder
            case 253875: ///< Essence of the Lifebinder (before fight) - Eonar the Lifebinder
            case 245764: ///< Essence of the Lifebinder (during fight) - Eonar the Lifebinder
                spellInfo->AuraInterruptFlags = AURA_INTERRUPT_FLAG_CHANGE_MAP;
                break;
            case 244069: ///< Weight Of Darkness - Shatug
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EffectRadiusIndex::EFFECT_RADIUS_200_YARDS);           ///< 1000y
                break;
            case 250183: ///< Blasting Beam - Imonar the Soulhunter
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(30);            ///< 0-500y
                spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(245);  ///< 0-500y
                break;
            case 248233: ///< Conflagration - Imonar the Soulhunter
                spellInfo->ChannelInterruptFlags = 0;
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_PERIODIC_TRIGGER_SPELL;
                spellInfo->Effects[EFFECT_0].TriggerSpell = 248424;
                spellInfo->ChannelInterruptFlags |= CHANNEL_INTERRUPT_FLAG_INTERRUPT;
                break;
            case 247711: ///< Charged Blasts (searcher)  - Imonar the Soulhunter
                spellInfo->Effects[EFFECT_1].Effect = 0;
                spellInfo->ExcludeTargetAuraSpell = 257197;
                break;
            case 247707: ///< Charged Blasts (AT) - Imonar the Soulhunter
                spellInfo->AttributesEx3 &= ~SPELL_ATTR3_ONLY_TARGET_PLAYERS;
                break;
            case 247716: ///< Charged Blasts (damages) - Imonar the Soulhunter
                spellInfo->RangeEntry = sSpellRangeStore.LookupEntry(245);  ///< 0-500y
                break;
            case 252797: ///< Decimation - Garothi Decimator (Pre Kin'garoth)
            case 248332: ///< Rain of Fel - Eonar the Life-binder
            case 254429: ///< Weight Of Darkness - Shatug
            case 244410: ///< Decimation - Garothi Worldbreaker
            case 244912: ///< Blazing Eruption - Aggramar
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE;
                break;
            case 252795: ///< Decimation - Garothi Decimator (Pre Kin'garoth)
                spellInfo->Speed = 1.0f;
                break;
            case 246690: ///< Decimation - Kin'garoth
                spellInfo->Speed = 2.0f;
                break;
            case 244383: ///< Aegis of Flames - Portal Keeper Hasabel
                spellInfo->StackAmount = 3;
                break;
            case 248396: ///< Soulblight - Argus the Unmaker
                spellInfo->ExcludeTargetAuraSpell = 248396;
                break;
            case 246798: ///< Ruiner - Kin'garoth
                spellInfo->DurationEntry = sSpellDurationStore.LookupEntry(8); ///< 15s
                break;
            case 246754: ///< Diabolic Bomb - Kin'garoth
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                spellInfo->ExplicitTargetMask |= TARGET_FLAG_DEST_LOCATION;
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_0_YARDS);
                break;
            case 240796: ///< Citadel -> Nexus Teleport - Portal Keeper Hasabel
                spellInfo->Attributes &= ~SPELL_ATTR0_CANT_USED_IN_COMBAT;
                break;
            case 254926: ///< Reverberating Strike - Kin'garoth
                spellInfo->Effects[1].TriggerSpell = 0;
                break;
            case 244321: ///< Reverberating Strike - Kin'garoth
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_0_YARDS);
                break;
            case 258643: ///< Feedback Overload - Kin'garoth
               spellInfo->AttributesEx2 |= SPELL_ATTR2_CANT_CRIT;
               break;
            case 254497: ///< Dive Down - Eonar the Lifebinder
                spellInfo->Effects[EFFECT_0].TriggerSpell = 245764;
                break;
            case 254797: ///< Empowered Ruiner - Kin'garoth
                spellInfo->Attributes &= ~SPELL_ATTR0_DEBUFF;
                spellInfo->AttributesCu &= ~SPELL_ATTR0_CU_NEGATIVE_EFF0;
                break;
            case 249453: ///< Ruination - Kin'garoth
            case 255646: ///< Golganneth's Wrath (damages) - Argus the Unmaker
            case 258375: ///< Grasp of the Unmaker - Argus the Unmaker
            case 243975: ///< Control Aura - Varimathras, unused but in sniff
            case 243967: ///< Torment of Flames - Varimathras
            case 243976: ///< Torment of Frost - Varimathras
            case 243979: ///< Torment of Fel - Varimathras
            case 243974: ///< Torment of Shadows - Varimathras
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                break;
            case 255683: ///< Golganneth's Wrath (Periodic Aura) - Argus the Unmaker
                spellInfo->AttributesEx &= ~SPELL_ATTR1_CHANNELED_2;
                break;
            case 251815: ///< Edge of Obliteration - Argus the Unmaker
            case 258834: ///< Edge of Annihilation - Argus the Unmaker
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_SRC_CASTER;
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_SRC_CASTER;
                spellInfo->Effects[EFFECT_1].TargetB = TARGET_UNIT_SRC_AREA_ENEMY;
                spellInfo->Attributes |= SPELL_ATTR0_IMPOSSIBLE_DODGE_PARRY_BLOCK;
                break;
            case 255976: ///< Arcane Dissolve Out - Argus the Unmaker
            case 256543: ///< Arcane Dissolve Out - Argus the Unmaker
                spellInfo->Effects[EFFECT_1].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[EFFECT_1].TargetB = TARGET_DEST_DB;
                break;
            case 46841: ///< Escape to the Isle of Quel'Danas
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_DEST_DB;
                break;
            case 251059: ///< Falling Debris - Kin'garoth
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                spellInfo->ExplicitTargetMask |= TARGET_FLAG_DEST_LOCATION;
                spellInfo->SetRangeIndex(13); ///< 50000yd
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_50000_YARDS); ///< 50000yd
                break;
            case 243959: ///< Distance Check - Varimathras
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_8_YARDS);
                break;
            case 255235: ///< Argus Outro - Argus the Unmaker
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_DEST_DB;
                break;
            case 255826: ///< Edge of Obliteration - Argus the Unmaker
            case 250763: ///< Cosmic Glare - Covens of Shivarra
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                break;
            case 253026: ///< Impending Inevitability - Argus the Unmaker
                spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                break;
            case 248499: ///< Sweeping Scythe - Argus the Unmaker
            case 258039: ///< Deadly Scythe - Argus the Unmaker
            case 247367: ///< Shock Lance - Imonar the Soulhunter
            case 250255: ///< Empowered Shock Lance - Imonar the Soulhunter
            case 258838: ///< Soulrending Scythe - Argus the Unmaker
            case 244675: ///< Taeshalach's Reach - Aggramar
                spellInfo->Attributes |= SPELL_ATTR0_IMPOSSIBLE_DODGE_PARRY_BLOCK;
                break;
            case 247687: ///< Sever - Imonar the Soulhunter
                spellInfo->Effects[EFFECT_1].Mechanic = Mechanics::MECHANIC_BLEED;
                spellInfo->Attributes |= SPELL_ATTR0_IMPOSSIBLE_DODGE_PARRY_BLOCK;
                break;
            case 257442: ///< Emerald Blossom - Eonar's Compassion
                spellInfo->MaxAffectedTargets = 1;
                break;
            case 257470: ///< Eonar's Verdant Embrace - Pantheon's Proc (Druid)
            case 257472: ///< Eonar's Verdant Embrace - Pantheon's Proc (Paladin)
            case 257473: ///< Eonar's Verdant Embrace - Pantheon's Proc (Discipline Priest)
            case 257474: ///< Eonar's Verdant Embrace - Pantheon's Proc (Holy Priest)
            case 257475: ///< Eonar's Verdant Embrace - Pantheon's Proc (Shaman)
            case 256836: ///< Norgannon's Command
                spellInfo->ProcCharges = 0;
                break;
            case 257471: ///< Eonar's Verdant Embrace - Pantheon's Proc (Monk)
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_PROC_TRIGGER_SPELL;
                spellInfo->ProcCharges = 0;
                break;
            case 245281: ///< Shadow Blades - Covens of Shivarra
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_DEST_DEST;
                spellInfo->SetRangeIndex(13); ///< 50000yd
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_50000_YARDS); ///< 50000yd
                break;
            case 245518: ///< Flashfreeze - Covens of Shivarra
                spellInfo->StackAmount = 10;
                break;
            case 244016: ///< Reality Tear - Portal Keeper Hasabel
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_IGNORE_ARMOR;
                break;
            case 245099: ///< Mind Fog - Portal Keeper Hasabel
                spellInfo->AttributesEx |= SPELL_ATTR1_UNAFFECTED_BY_SCHOOL_IMMUNE;
                break;
            case 249863: ///< Visage of the Titan - Covens of Shivarra
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                break;
            case 250095: ///< Machinations of Aman'Thul
                spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_DUMMY;
                spellInfo->Effects[EFFECT_0].TriggerSpell = 0;
                break;
            case 250097: ///< Machinations of Aman'Thul
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CASTER;
                spellInfo->Effects[EFFECT_0].TargetB = 0;
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(EFFECT_RADIUS_0_YARDS);
                spellInfo->AttributesEx2 |= SPELL_ATTR2_CANT_CRIT;
                spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_DONE_BONUS;
                spellInfo->AttributesEx6 |= SPELL_ATTR6_NO_DONE_PCT_DAMAGE_MODS;
                break;
            case 244093: ///< Necrotic Embrace
                spellInfo->InterruptFlags = 0;
                break;
            case 248303: ///< Kin'garoth Energize Periodic
                spellInfo->Effects[EFFECT_0].Amplitude = 1200;
                break;
            case 246779: ///< Diaboling Bomb
            case 250139: ///< Foul Steps - Eonar the Lifebinder
                spellInfo->AttributesEx3 |= SPELL_ATTR3_NO_DONE_BONUS;
                break;
            case 244208: ///< Flame Rend - Aggramar
                spellInfo->Effects[EFFECT_0].Effect = SPELL_EFFECT_KNOCK_BACK;
                spellInfo->Effects[EFFECT_0].BasePoints = 350;
                spellInfo->Effects[EFFECT_0].MiscValue = 400;
                spellInfo->Effects[EFFECT_0].TargetA = TARGET_UNIT_CONE_ENEMY_104;
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(20);
                break;
            case 244903: ///< Cataliyzing Presence - Aggramar
                spellInfo->Effects[EFFECT_7].BasePoints = 50;
                break;
            case 244678:
                spellInfo->Effects[EFFECT_0].Amplitude = 2500;
                break;
            case 245458: ///< Foe Breaker - Aggramar
            case 255059: ///< Empowered Foe Breaker - Aggramar
                spellInfo->Effects[EFFECT_0].SetRadiusIndex(10); ///< 30y
                spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_CONE_ENEMY_104;
                /// no break intended
            case 244291: ///< Foe Breaker - Aggramar
                spellInfo->Attributes |= SPELL_ATTR0_IMPOSSIBLE_DODGE_PARRY_BLOCK;
                break;
            case 244892: ///< Exploit Weakness - Antoran High Command
                spellInfo->Attributes |= SPELL_ATTR0_IMPOSSIBLE_DODGE_PARRY_BLOCK;
                break;
            case 248789: ///< Spear of Doom - The Paraxis
                spellInfo->AttributesEx9 |= SPELL_ATTR9_CASTABLE_WHILE_CAST_IN_PROGRESS;
                break;
            case 248795: ///< Fel Wake - The Paraxis
                spellInfo->AttributesCu |= SPELL_ATTR0_CU_DONT_RESET_PERIODIC_TIMER;
                break;
            case 249535: ///< Demolished
                spellInfo->Effects[EFFECT_0].ApplyAuraName = SPELL_AURA_DUMMY;
                break;
            case 244097: ///< Necrotic Embrace
                spellInfo->ExcludeTargetAuraSpell = 0;
                break;
            case 245303: ///< Touch of Darkness - Covens of Shivarra
                spellInfo->AttributesEx5 |= SPELL_ATTR5_USABLE_WHILE_MOVING;
                break;
            case 247932: ///< Shrapnel Blast - Imonar the Soulhunter
                spellInfo->AttributesEx3 |= SPELL_ATTR3_STACK_FOR_DIFF_CASTERS;
                break;
            case 244915: ///< Leech Essence - Portal Keeper Hasabel
            case 246316: ///< Poison Essence - Portal Keeper Hasabel
            case 244729: ///< Shock Grenade Missiles - Antoran High Command
            case 244722: ///< Shock Grenade - Antoran High Command
            case 244737: ///< Shock Grenade - Antoran High Command
                spellInfo->AttributesEx |= SPELL_ATTR1_CANT_BE_REFLECTED;
                break;
            default:
                break;
        }
#pragma endregion

        /// Speed needs to be set for some reason, else delay won't apply
        if (IsCCSpell(spellInfo) && !spellInfo->Speed)
            spellInfo->Speed = 12345.0f;

            switch (spellInfo->SpellFamilyName)
            {
                case SPELLFAMILY_GENERIC:
                    /// Wind lash (auto-attack of Ascendance)
                    if (spellInfo->Id == 147051)
                        spellInfo->AttributesCu |= SPELL_ATTR0_CU_IGNORE_ARMOR;
                break;
                case SPELLFAMILY_SHAMAN:
                    /// Lava Lash / Windstrike Main Hand and Off Hand - Override Stormstrike (17364) with Ascendance (114051)
                    if (spellInfo->Id == 60103 || spellInfo->Id == 115357 || spellInfo->Id == 115360)
                        spellInfo->AttributesCu |= SPELL_ATTR0_CU_IGNORE_ARMOR;
                    break;
                case SPELLFAMILY_WARRIOR:
                    /// Shout
                    if (spellInfo->SpellFamilyFlags[0] & 0x20000 || spellInfo->SpellFamilyFlags[1] & 0x20)
                        spellInfo->AttributesCu |= SPELL_ATTR0_CU_AURA_CC;
                    /// Dragon Roar
                    if (spellInfo->Id == 118000)
                        spellInfo->AttributesCu |= SPELL_ATTR0_CU_IGNORE_ARMOR;
                    break;
                case SPELLFAMILY_DRUID:
                    /// Roar
                    if (spellInfo->SpellFamilyFlags[0] & 0x8)
                        spellInfo->AttributesCu |= SPELL_ATTR0_CU_AURA_CC;
                    /// Rake
                    if (spellInfo->Id == 1822)
                        spellInfo->AttributesCu |= SPELL_ATTR0_CU_IGNORE_ARMOR;
                    if (spellInfo->Id == 168877)
                        spellInfo->Speed = 0.0f;
                    break;
                default:
                    break;
            }

            /// This must be re-done if targets changed since the spellinfo load
            spellInfo->ExplicitTargetMask = spellInfo->IsTargetingFollower() ? TARGET_FLAG_UNIT : spellInfo->_GetExplicitTargetMask();

            switch (spellInfo->Id)
            {
                case 147490: ///< Healing Rain Visual
                case 120517: ///< Halo (heal)
                case 61882:  ///< Earthquake Totem
                case 152280: ///< Defile
                case 109248: ///< Binding Shot
                case 173229: ///< Creeping Moss (Brackenspore)
                case 102793: ///< Ursol's Vortex
                case 123986: ///< Chi Burst
                case 155738: ///< Slag Pool (Heart of the Mountain)
                case 174556: ///< Meteor
                    spellInfo->ExplicitTargetMask &= ~TARGET_FLAG_UNIT;
                    break;
                case 116011:///< Rune of Power
                    spellInfo->ExplicitTargetMask &= ~TARGET_FLAG_UNIT;
                    break;
                case 107223:///< Sunfire Rays
                case 106736:///< Wither Will
                case 106113:///< Touch of Nothingness
                    spellInfo->ExplicitTargetMask = TARGET_FLAG_UNIT_MASK;
                    break;
                case 78675:///< Solar beam
                case 106112:///< Release Doubt
                    spellInfo->ExplicitTargetMask |= TARGET_FLAG_DEST_LOCATION;
                    break;
                case 189111: ///< Infernal Strike (Jump)
                    spellInfo->Effects[EFFECT_0].TriggerSpell = 189112; ///< Infernal Strike (damage)
                    break;
                case 227296: ///< Loose Mana (Return to Karazhan: Mana Devourer)
                case 230141: ///< Loose Mana (Return to Karazhan: Mana Devourer)
                    spellInfo->Effects[EFFECT_0].TargetB = TARGET_UNIT_CASTER;
                    spellInfo->ExplicitTargetMask |= TARGET_FLAG_UNIT;
                    break;
                case 197467:    ///< Bottomless Depths
                    spellInfo->ProcFlags = PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS;
                    break;
                default:
                    break;
            }

            /// Our targetting system is weird as fuck - would need a full rewrite for this to work properly, do not touch - hours of debugging
            if (spellInfo->HasEffect(SPELL_EFFECT_INCREASE_FOLLOWER_ITEM_LEVEL) || spellInfo->HasEffect(SPELL_EFFECT_TEACH_FOLLOWER_ABILITY) || spellInfo->HasEffect(SPELL_EFFECT_RANDOMIZE_FOLLOWER_ABILITIES))
                spellInfo->ExplicitTargetMask = TARGET_FLAG_NONE;

            spellInfo->UpdateSpellEffectCount(); ///< Re-cache the maximum number of effects
        }
    }

    CreatureAI::FillAISpellInfo();

    LoadChangeRecoveryRateSpells();

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded spell custom attributes in %u ms", GetMSTimeDiffToNow(oldMSTime));
}

void SpellMgr::LoadTalentSpellInfo()
{
    for (uint32 i = 0; i < sTalentStore.GetNumRows(); i++)
    {
        TalentEntry const* talent = sTalentStore.LookupEntry(i);
        if (!talent)
            continue;

        mTalentSpellInfo.insert(talent->SpellID);
    }
}

const SpellInfo* SpellMgr::GetSpellInfo(uint32 p_SpellID, Difficulty p_Difficulty) const
{
    if (p_SpellID < GetSpellInfoStoreSize())
    {
        if (p_Difficulty != DifficultyNone)
        {
            /// If spell isn't available in difficulty we want, check fallback difficulty ...
            DifficultyEntry const* l_Difficulty = sDifficultyStore.LookupEntry(p_Difficulty);
            while (l_Difficulty != nullptr)
            {
                SpellInfo const* l_SpellInfo = mSpellInfoMap[l_Difficulty->ID][p_SpellID];
                if (l_SpellInfo != nullptr)
                    return l_SpellInfo;

                l_Difficulty = sDifficultyStore.LookupEntry(l_Difficulty->FallbackDifficultyID);
            }
        }

        return mSpellInfoMap[DifficultyNone][p_SpellID];
    }

    return nullptr;
}

void SpellMgr::LoadSpellPowerInfo()
{
    mSpellPowerInfo.resize(sSpellStore.GetNumRows());
    for (uint32 i = 0; i < sSpellPowerStore.GetNumRows(); i++)
    {
        SpellPowerEntry const* spellPower = sSpellPowerStore.LookupEntry(i);
        if (!spellPower)
            continue;

        mSpellPowerInfo[spellPower->SpellId].push_back(spellPower->Id);
    }
}

SpellPowerEntry const* SpellMgr::GetSpellPowerEntryByIdAndPower(uint32 id, Powers power) const
{
    for (auto itr : GetSpellPowerList(id))
    {
        SpellPowerEntry const* spellPower = sSpellPowerStore.LookupEntry(itr);
        if (!spellPower)
            continue;

        if (spellPower->PowerType == power)
            return spellPower;
    }

    return NULL;
}

void SpellMgr::LoadSpellUpgradeItemStage()
{
    QueryResult l_Result = WorldDatabase.PQuery("SELECT ItemBonusTreeCategory, ItemClass, ItemSubClassMask, InventoryTypeMask, MaxIlevel FROM spell_upgrade_item_stage");

    if (!l_Result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 spell upgrade item stage. DB table `spell_upgrade_item_stage` is empty.");
        return;
    }

    uint32 l_Count = 0;

    do
    {
        Field* l_Fields = l_Result->Fetch();

        SpellUpgradeItemStage l_SpellUpgradeItemStage;

        uint32 l_ItemBonusTreeCategory            = l_Fields[0].GetUInt32();
        l_SpellUpgradeItemStage.ItemClass         = l_Fields[1].GetInt32();
        l_SpellUpgradeItemStage.ItemSubclassMask  = l_Fields[2].GetInt32();
        l_SpellUpgradeItemStage.InventoryTypeMask = l_Fields[3].GetInt32();
        l_SpellUpgradeItemStage.MaxIlevel         = l_Fields[4].GetInt32();

        m_SpellUpgradeItemStages[l_ItemBonusTreeCategory].push_back(l_SpellUpgradeItemStage);
        l_Count++;
    }
    while (l_Result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u spell upgrade item stage.", l_Count);
}

void SpellMgr::LoadSpellTriggered()
{
    uint32 l_OldMSTime = getMSTime();

    m_SpellTriggeredMap.clear();    ///< Needed for reload case
    m_SpellDummyTriggerMap.clear(); ///< Needed for reload case
    m_SpellAuraTriggerMap.clear();  ///< Needed for reload case
    m_SpellAuraDummyMap.clear();    ///< Needed for reload case
    m_SpellTargetFilterMap.clear(); ///< Needed for reload case
    m_SpellCheckCastMap.clear();    ///< Needed for reload case

    uint32 l_Count = 0;
    ///                                                   0           1                    2           3         4          5          6          7      8      9         10         11       12       13         14          15            16            17           18          19           20              21          22          23
    QueryResult l_Result = WorldDatabase.Query("SELECT `spell_id`, `spell_trigger`, `spell_cooldown`, `option`, `target`, `caster`, `targetaura`, `bp0`, `bp1`, `bp2`, `effectmask`, `aura`, `chance`, `group`, `procFlags`, `procEx`, `check_spell_id`, `addptype`, `schoolMask`, `dummyId`, `dummyEffect`, `targetaura2`, `aura2`, `CreatureType` FROM `spell_trigger`");
    if (l_Result)
    {
        do
        {
            Field* l_Fields         = l_Result->Fetch();
            uint8 l_Index           = 0;

            int32 l_SpellID         = l_Fields[l_Index++].GetInt32();
            int32 l_SpellTrigger    = l_Fields[l_Index++].GetInt32();
            int32 l_SpellCooldown   = l_Fields[l_Index++].GetInt32();
            uint8 l_Options         = l_Fields[l_Index++].GetUInt8();
            uint8 l_Target          = l_Fields[l_Index++].GetUInt8();
            uint8 l_Caster          = l_Fields[l_Index++].GetUInt8();
            uint8 l_TargetAura      = l_Fields[l_Index++].GetUInt8();
            float l_Bp0             = l_Fields[l_Index++].GetFloat();
            float l_Bp1             = l_Fields[l_Index++].GetFloat();
            float l_Bp2             = l_Fields[l_Index++].GetFloat();
            uint8 l_EffectMask      = l_Fields[l_Index++].GetUInt8();
            int32 l_Aura            = l_Fields[l_Index++].GetInt32();
            int32 l_Chance          = l_Fields[l_Index++].GetInt32();
            uint8 l_Group           = l_Fields[l_Index++].GetUInt8();
            int64 l_ProcFlags       = l_Fields[l_Index++].GetInt64();
            int32 l_ProcEx          = l_Fields[l_Index++].GetInt32();
            int32 l_CheckSpellID    = l_Fields[l_Index++].GetInt32();
            int32 l_AddPType        = l_Fields[l_Index++].GetInt32();
            int32 l_SchoolMask      = l_Fields[l_Index++].GetInt32();
            int32 l_DummyID         = l_Fields[l_Index++].GetInt32();
            int32 l_DummyEffect     = l_Fields[l_Index++].GetInt32();
            uint8 l_TargetAura2     = l_Fields[l_Index++].GetUInt8();
            int32 l_Aura2           = l_Fields[l_Index++].GetInt32();
            int32 l_CreatureType    = l_Fields[l_Index++].GetInt32();

            SpellInfo const* l_SpellInfo = GetSpellInfo(abs(l_SpellID));
            if (!l_SpellInfo)
            {
                sLog->outError(LogFilterType::LOG_FILTER_SQL, "Spell_id %i listed in `spell_trigger` does not exist", l_SpellID);
                continue;
            }

            l_SpellInfo = GetSpellInfo(abs(l_SpellTrigger));
            if (!l_SpellInfo)
            {
                sLog->outError(LogFilterType::LOG_FILTER_SQL, "Spell_trigger %i listed in `spell_trigger` does not exist", l_SpellTrigger);
                continue;
            }

            l_SpellInfo = GetSpellInfo(abs(l_DummyID));
            if (l_DummyID && !l_SpellInfo)
            {
                sLog->outError(LogFilterType::LOG_FILTER_SQL, "DummyId %i listed in `spell_trigger` does not exist", l_DummyID);
                continue;
            }

            SpellTriggered l_TempTrigger;
            l_TempTrigger.SpellID   = l_SpellID;
            l_TempTrigger.SpellTrigger  = l_SpellTrigger;
            l_TempTrigger.SpellCooldown = l_SpellCooldown;
            l_TempTrigger.Options       = l_Options;
            l_TempTrigger.Target        = l_Target;
            l_TempTrigger.Caster        = l_Caster;
            l_TempTrigger.TargetAura    = l_TargetAura;
            l_TempTrigger.Bp0           = l_Bp0;
            l_TempTrigger.Bp1           = l_Bp1;
            l_TempTrigger.Bp2           = l_Bp2;
            l_TempTrigger.EffectMask    = l_EffectMask;
            l_TempTrigger.Aura          = l_Aura;
            l_TempTrigger.Chance        = l_Chance;
            l_TempTrigger.Group         = l_Group;
            l_TempTrigger.ProcFlags     = l_ProcFlags;
            l_TempTrigger.ProcEx        = l_ProcEx;
            l_TempTrigger.CheckSpellID  = l_CheckSpellID;
            l_TempTrigger.AddPType      = l_AddPType;
            l_TempTrigger.SchoolMask    = l_SchoolMask;
            l_TempTrigger.DummyID       = l_DummyID;
            l_TempTrigger.DummyEffect   = l_DummyEffect;
            l_TempTrigger.TargetAura2   = l_TargetAura2;
            l_TempTrigger.Aura2         = l_Aura2;
            l_TempTrigger.CreatureType  = l_CreatureType;

            m_SpellTriggeredMap[l_SpellID].push_back(l_TempTrigger);

            ++l_Count;
        }
        while (l_Result->NextRow());
    }

    ///                                       0             1             2         3         4          5          6      7      8         9          10       11
    l_Result = WorldDatabase.Query("SELECT `spell_id`, `spell_trigger`, `option`, `target`, `caster`, `targetaura`, `bp0`, `bp1`, `bp2`, `effectmask`, `aura`, `chance` FROM `spell_dummy_trigger`");
    if (l_Result)
    {
        do
        {
            Field* l_Fields         = l_Result->Fetch();
            uint8 l_Index           = 0;

            int32 l_SpellID         = l_Fields[l_Index++].GetInt32();
            int32 l_SpellTrigger    = l_Fields[l_Index++].GetInt32();
            int32 l_Options         = l_Fields[l_Index++].GetUInt8();
            int32 l_Target          = l_Fields[l_Index++].GetUInt8();
            int32 l_Caster          = l_Fields[l_Index++].GetUInt8();
            int32 l_TargetAura      = l_Fields[l_Index++].GetUInt8();
            int32 l_Bp0             = l_Fields[l_Index++].GetFloat();
            int32 l_Bp1             = l_Fields[l_Index++].GetFloat();
            int32 l_Bp2             = l_Fields[l_Index++].GetFloat();
            int32 l_EffectMask      = l_Fields[l_Index++].GetInt8();
            int32 l_Aura            = l_Fields[l_Index++].GetInt32();
            int32 l_Chance          = l_Fields[l_Index++].GetInt32();

            SpellInfo const* l_SpellInfo = GetSpellInfo(abs(l_SpellID));
            if (!l_SpellInfo)
            {
                sLog->outError(LogFilterType::LOG_FILTER_SQL, "Spell %u listed in `spell_dummy_trigger` does not exist", abs(l_SpellID));
                continue;
            }

            SpellDummyTrigger l_TempDummy;
            l_TempDummy.SpellID         = l_SpellID;
            l_TempDummy.SpellTrigger    = l_SpellTrigger;
            l_TempDummy.Options         = l_Options;
            l_TempDummy.Target          = l_Target;
            l_TempDummy.Caster          = l_Caster;
            l_TempDummy.TargetAura      = l_TargetAura;
            l_TempDummy.Bp0             = l_Bp0;
            l_TempDummy.Bp1             = l_Bp1;
            l_TempDummy.Bp2             = l_Bp2;
            l_TempDummy.EffectMask      = l_EffectMask;
            l_TempDummy.Aura            = l_Aura;
            l_TempDummy.Chance          = l_Chance;

            m_SpellDummyTriggerMap[l_SpellID].push_back(l_TempDummy);

            ++l_Count;
        }
        while (l_Result->NextRow());
    }

    ///                                       0             1             2         3         4          5          6      7      8         9          10       11       12
    l_Result = WorldDatabase.Query("SELECT `spell_id`, `spell_trigger`, `option`, `target`, `caster`, `targetaura`, `bp0`, `bp1`, `bp2`, `effectmask`, `aura`, `chance`, `slot` FROM `spell_aura_trigger`");
    if (l_Result)
    {
        do
        {
            Field* l_Fields         = l_Result->Fetch();
            uint8 l_Index           = 0;

            int32 l_SpellID         = l_Fields[l_Index++].GetInt32();
            int32 l_SpellTrigger    = l_Fields[l_Index++].GetInt32();
            int32 l_Options         = l_Fields[l_Index++].GetUInt8();
            int32 l_Target          = l_Fields[l_Index++].GetUInt8();
            int32 l_Caster          = l_Fields[l_Index++].GetUInt8();
            int32 l_TargetAura      = l_Fields[l_Index++].GetUInt8();
            int32 l_Bp0             = l_Fields[l_Index++].GetFloat();
            int32 l_Bp1             = l_Fields[l_Index++].GetFloat();
            int32 l_Bp2             = l_Fields[l_Index++].GetFloat();
            int32 l_EffectMask      = l_Fields[l_Index++].GetInt8();
            int32 l_Aura            = l_Fields[l_Index++].GetInt32();
            int32 l_Chance          = l_Fields[l_Index++].GetInt32();
            int32 l_Slot            = l_Fields[l_Index++].GetInt32();

            SpellInfo const* l_SpellInfo = GetSpellInfo(abs(l_SpellID));
            if (!l_SpellInfo)
            {
                sLog->outError(LogFilterType::LOG_FILTER_SQL, "Spell %u listed in `spell_aura_trigger` does not exist", abs(l_SpellID));
                continue;
            }

            SpellDummyTrigger l_TempAura;
            l_TempAura.SpellID      = l_SpellID;
            l_TempAura.SpellTrigger	= l_SpellTrigger;
            l_TempAura.Options      = l_Options;
            l_TempAura.Target       = l_Target;
            l_TempAura.Caster       = l_Caster;
            l_TempAura.TargetAura   = l_TargetAura;
            l_TempAura.Bp0          = l_Bp0;
            l_TempAura.Bp1          = l_Bp1;
            l_TempAura.Bp2          = l_Bp2;
            l_TempAura.EffectMask   = l_EffectMask;
            l_TempAura.Aura         = l_Aura;
            l_TempAura.Chance       = l_Chance;
            l_TempAura.Slot         = l_Slot;

            m_SpellAuraTriggerMap[l_SpellID].push_back(l_TempAura);

            ++l_Count;
        }
        while (l_Result->NextRow());
    }

    ///                                       0             1          2         3         4           5              6             7           8        9          10         11        12           13        14       15
    l_Result = WorldDatabase.Query("SELECT `spellId`, `spellDummyId`, `option`, `target`, `caster`, `targetaura`, `effectmask`, `effectDummy`, `aura`, `chance`, `removeAura`, `attr`, `attrValue`, `custombp`, `type`, `charge` FROM `spell_aura_dummy`");
    if (l_Result)
    {
        do
        {
            Field* l_Fields         = l_Result->Fetch();
            uint8 l_Index           = 0;

            int32 l_SpellID         = l_Fields[l_Index++].GetInt32();
            int32 l_SpellDummyID    = l_Fields[l_Index++].GetInt32();
            int32 l_Options         = l_Fields[l_Index++].GetInt32();
            int32 l_Target          = l_Fields[l_Index++].GetInt32();
            int32 l_Caster          = l_Fields[l_Index++].GetInt32();
            int32 l_TargetAura      = l_Fields[l_Index++].GetInt32();
            int32 l_EffectMask      = l_Fields[l_Index++].GetInt32();
            int32 l_EffectDummy     = l_Fields[l_Index++].GetInt32();
            int32 l_Aura            = l_Fields[l_Index++].GetInt32();
            int32 l_Chance          = l_Fields[l_Index++].GetInt32();
            int32 l_RemoveAura      = l_Fields[l_Index++].GetInt32();
            int32 l_Attr            = l_Fields[l_Index++].GetInt32();
            int32 l_AttrValue       = l_Fields[l_Index++].GetInt32();
            int32 l_CustomBP        = l_Fields[l_Index++].GetInt32();
            int32 l_Type            = l_Fields[l_Index++].GetInt32();
            int32 l_Charge          = l_Fields[l_Index++].GetInt32();

            if (l_RemoveAura)
            {
                sLog->outError(LogFilterType::LOG_FILTER_SQL, "Spell %u : Using RemoveAura leads to crash in calculateAmount", l_SpellID);
                continue;
            }

            SpellInfo const* l_SpellInfo = GetSpellInfo(abs(l_SpellID));
            if (!l_SpellInfo)
            {
                sLog->outError(LogFilterType::LOG_FILTER_SQL, "Spell %i listed in `spell_aura_dummy` does not exist", l_SpellID);
                continue;
            }

            SpellAuraDummy l_TempDummy;
            l_TempDummy.SpellID         = l_SpellID;
            l_TempDummy.SpellDummyID    = l_SpellDummyID;
            l_TempDummy.Options         = l_Options;
            l_TempDummy.Target          = l_Target;
            l_TempDummy.Caster          = l_Caster;
            l_TempDummy.TargetAura      = l_TargetAura;
            l_TempDummy.EffectMask      = l_EffectMask;
            l_TempDummy.EffectDummy     = l_EffectDummy;
            l_TempDummy.Aura            = l_Aura;
            l_TempDummy.RemoveAura      = l_RemoveAura;
            l_TempDummy.Chance          = l_Chance;
            l_TempDummy.Attr            = l_Attr;
            l_TempDummy.AttrValue       = l_AttrValue;
            l_TempDummy.CustomBP        = l_CustomBP;
            l_TempDummy.Type            = l_Type;
            l_TempDummy.Charge          = l_Charge;

            m_SpellAuraDummyMap[l_SpellID].push_back(l_TempDummy);

            ++l_Count;
        }
        while (l_Result->NextRow());
    }

    ///                                      0          1          2        3        4           5              6          7         8           9           10          11        12        13
    l_Result = WorldDatabase.Query("SELECT `spellId`, `targetId`, `option`, `aura`, `chance`, `effectMask`, `resizeType`, `count`, `maxcount`, `addcount`, `addcaster`, `param1`, `param2`, `param3` FROM `spell_target_filter`");
    if (l_Result)
    {
        do
        {
            Field* l_Fields     = l_Result->Fetch();
            uint8 l_Index       = 0;

            int32 l_SpellID     = l_Fields[l_Index++].GetInt32();
            int32 l_TargetID	= l_Fields[l_Index++].GetInt32();
            int32 l_Options     = l_Fields[l_Index++].GetInt32();
            int32 l_Aura        = l_Fields[l_Index++].GetInt32();
            int32 l_Chance      = l_Fields[l_Index++].GetInt32();
            int32 l_EffectMask  = l_Fields[l_Index++].GetInt32();
            int32 l_ResizeType  = l_Fields[l_Index++].GetInt32();
            int32 l_Count       = l_Fields[l_Index++].GetInt32();
            int32 l_MaxCount    = l_Fields[l_Index++].GetInt32();
            int32 l_AddCount    = l_Fields[l_Index++].GetInt32();
            int32 l_AddCaster   = l_Fields[l_Index++].GetInt32();
            float l_Param1      = l_Fields[l_Index++].GetFloat();
            float l_Param2      = l_Fields[l_Index++].GetFloat();
            float l_Param3      = l_Fields[l_Index++].GetFloat();

            SpellInfo const* l_SpellInfo = GetSpellInfo(abs(l_SpellID));
            if (!l_SpellInfo)
            {
                sLog->outError(LogFilterType::LOG_FILTER_SQL, "Spell %i listed in `spell_target_filter` does not exist", l_SpellID);
                continue;
            }

            SpellTargetFilter l_TempFilter;
            l_TempFilter.SpellID    = l_SpellID;
            l_TempFilter.TargetID   = l_TargetID;
            l_TempFilter.Options    = l_Options;
            l_TempFilter.Aura       = l_Aura;
            l_TempFilter.Chance     = l_Chance;
            l_TempFilter.EffectMask = l_EffectMask;
            l_TempFilter.ResizeType = l_ResizeType;
            l_TempFilter.Count      = l_Count;
            l_TempFilter.MaxCount   = l_MaxCount;
            l_TempFilter.AddCount   = l_AddCount;
            l_TempFilter.AddCaster  = l_AddCaster;
            l_TempFilter.Param1     = l_Param1;
            l_TempFilter.Param2     = l_Param2;
            l_TempFilter.Param3     = l_Param3;

            m_SpellTargetFilterMap[l_SpellID].push_back(l_TempFilter);

            ++l_Count;
        }
        while (l_Result->NextRow());
    }

    ///                                      0        1         2             3            4         5           6           7            8            9           10        11        12
    l_Result = WorldDatabase.Query("SELECT `spellId`, `type`, `errorId`, `customErrorId`, `caster`, `target`, `checkType`, `dataType`, `checkType2`, `dataType2`, `param1`, `param2`, `param3` FROM `spell_check_cast`");
    if (l_Result)
    {
        do
        {
            Field* l_Fields = l_Result->Fetch();
            uint8 l_Index	= 0;
            int32 l_SpellID = l_Fields[l_Index++].GetInt32();

            SpellInfo const* l_SpellInfo = GetSpellInfo(abs(l_SpellID));
            if (!l_SpellInfo)
            {
                sLog->outError(LogFilterType::LOG_FILTER_SQL, "Spell %i listed in `spell_check_cast` does not exist", l_SpellID);
                continue;
            }

            SpellCheckCast l_CheckCast;
            l_CheckCast.SpellID         = l_SpellID;
            l_CheckCast.Type            = l_Fields[l_Index++].GetInt32();
            l_CheckCast.ErrorID         = l_Fields[l_Index++].GetInt32();
            l_CheckCast.CustomErrorID   = l_Fields[l_Index++].GetInt32();
            l_CheckCast.Caster          = l_Fields[l_Index++].GetInt32();
            l_CheckCast.Target          = l_Fields[l_Index++].GetInt32();
            l_CheckCast.CheckType       = l_Fields[l_Index++].GetInt32();
            l_CheckCast.DataType        = l_Fields[l_Index++].GetInt32();
            l_CheckCast.CheckType2      = l_Fields[l_Index++].GetInt32();
            l_CheckCast.DataType2       = l_Fields[l_Index++].GetInt32();
            l_CheckCast.Param1          = l_Fields[l_Index++].GetInt32();
            l_CheckCast.Param2          = l_Fields[l_Index++].GetInt32();
            l_CheckCast.Param3          = l_Fields[l_Index++].GetInt32();

            m_SpellCheckCastMap[l_SpellID].push_back(l_CheckCast);

            ++l_Count;
        }
        while (l_Result->NextRow());
    }

    sLog->outInfo(LogFilterType::LOG_FILTER_SERVER_LOADING, ">> Loaded %u triggered spell in %u ms", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

void SpellMgr::LoadSpellVisual()
{
    uint32 l_OldMSTime = getMSTime();

    m_SpellVisualMap.clear();           ///< Needed for reload case
    m_SpellVisualPlayOrphanMap.clear(); ///< Needed for reload case

    uint32 l_Count = 0;

    ///                                                 0            1            2            3             4            5        6        7
    QueryResult l_Result = WorldDatabase.Query("SELECT spellId, SpellVisualID, MissReason, ReflectStatus, TravelSpeed, SpeedAsTime, type, HasPosition FROM spell_visual");
    if (!l_Result)
        sLog->outInfo(LogFilterType::LOG_FILTER_SERVER_LOADING, ">> Loaded 0 visual spells. DB table `spell_visual` is empty.");
    else
    {
        do
        {
            Field* l_Fields = l_Result->Fetch();
            uint8 l_Index = 0;

            int32 l_SpellID = l_Fields[l_Index++].GetInt32();
            int32 l_SpellVisualID = l_Fields[l_Index++].GetInt32();
            int32 l_MissReason = l_Fields[l_Index++].GetInt32();
            int32 l_ReflectStatus = l_Fields[l_Index++].GetInt32();
            float l_TravelSpeed = l_Fields[l_Index++].GetFloat();
            bool  l_SpeedAsTime = bool(l_Fields[l_Index++].GetInt32());
            int32 l_Type = l_Fields[l_Index++].GetInt32();
            bool  l_HasPosition = bool(l_Fields[l_Index++].GetUInt8());

            SpellInfo const* l_SpellInfo = GetSpellInfo(abs(l_SpellID));
            if (!l_SpellInfo)
            {
                sLog->outError(LogFilterType::LOG_FILTER_SQL, "Spell %u listed in `spell_visual` does not exist", abs(l_SpellID));
                continue;
            }

            SpellVisual l_SpellVisual;
            l_SpellVisual.SpellID = l_SpellID;
            l_SpellVisual.SpellVisualID = l_SpellVisualID;
            l_SpellVisual.MissReason = l_MissReason;
            l_SpellVisual.ReflectStatus = l_ReflectStatus;
            l_SpellVisual.TravelSpeed = l_TravelSpeed;
            l_SpellVisual.SpeedAsTime = l_SpeedAsTime;
            l_SpellVisual.Type = l_Type;
            l_SpellVisual.HasPosition = l_HasPosition;

            m_SpellVisualMap[l_SpellID].push_back(l_SpellVisual);

            ++l_Count;
        }
        while (l_Result->NextRow());
    }

    ///                                     0            1            2            3          4        5  6  7  8
    l_Result = WorldDatabase.Query("SELECT spellId, SpellVisualID, TravelSpeed, SpeedAsTime, UnkFloat, X, Y, Z, type FROM spell_visual_play_orphan");
    if (!l_Result)
    {
        sLog->outInfo(LogFilterType::LOG_FILTER_SERVER_LOADING, ">> Loaded 0 visual spells. DB table `spell_visual_play_orphan` is empty.");
        return;
    }

    do
    {
        Field* l_Fields         = l_Result->Fetch();
        uint8 l_Index           = 0;

        int32 l_SpellID         = l_Fields[l_Index++].GetInt32();
        int32 l_SpellVisualID   = l_Fields[l_Index++].GetInt32();
        float l_TravelSpeed     = l_Fields[l_Index++].GetFloat();
        bool  l_SpeedAsTime     = bool(l_Fields[l_Index++].GetInt32());
        float l_UnkFloat        = l_Fields[l_Index++].GetFloat();

        SpellInfo const* l_SpellInfo = GetSpellInfo(abs(l_SpellID));
        if (!l_SpellInfo)
        {
            sLog->outError(LogFilterType::LOG_FILTER_SQL, "Spell %u listed in `spell_visual_play_orphan` does not exist", abs(l_SpellID));
            continue;
        }

        SpellVisualPlayOrphan l_SpellVisualPlayOrphan;
        l_SpellVisualPlayOrphan.SpellID         = l_SpellID;
        l_SpellVisualPlayOrphan.SpellVisualID   = l_SpellVisualID;
        l_SpellVisualPlayOrphan.TravelSpeed     = l_TravelSpeed;
        l_SpellVisualPlayOrphan.SpeedAsTime     = l_SpeedAsTime;
        l_SpellVisualPlayOrphan.UnkFloat        = l_UnkFloat;
        float l_X                               = l_Fields[l_Index++].GetFloat();
        float l_Y                               = l_Fields[l_Index++].GetFloat();
        float l_Z                               = l_Fields[l_Index++].GetFloat();
        l_SpellVisualPlayOrphan.SourceOrientation.Relocate(l_X, l_Y, l_Z);
        l_SpellVisualPlayOrphan.Type            = l_Fields[l_Index++].GetInt32();

        m_SpellVisualPlayOrphanMap[l_SpellID].push_back(l_SpellVisualPlayOrphan);

        ++l_Count;
    }
    while (l_Result->NextRow());

    sLog->outInfo(LogFilterType::LOG_FILTER_SERVER_LOADING, ">> Loaded %u visual spells in %u ms", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

void SpellMgr::LoadSpellPendingCast()
{
    uint32 l_OldMSTime = getMSTime();

    m_SpellPendingCastMap.clear();  ///< Needed for reload case

    ///                                                 0          1          2      3
    QueryResult l_Result = WorldDatabase.Query("SELECT `spell_id`, `pending_id`, `option`, `check` FROM `spell_pending_cast`");
    if (!l_Result)
    {
        sLog->outInfo(LogFilterType::LOG_FILTER_SERVER_LOADING, ">> Loaded 0 pending spells. DB table `spell_pending_cast` is empty.");
        return;
    }

    uint32 l_Count = 0;
    do
    {
        Field* l_Fields     = l_Result->Fetch();
        uint8 l_Index       = 0;

        int32 l_SpellID     = l_Fields[l_Index++].GetInt32();
        int32 l_PendingID   = l_Fields[l_Index++].GetInt32();
        int8  l_Options     = l_Fields[l_Index++].GetUInt8();
        int32 l_Check       = l_Fields[l_Index++].GetInt32();

        SpellInfo const* l_SpellInfo = GetSpellInfo(abs(l_SpellID));
        if (!l_SpellInfo)
        {
            sLog->outError(LogFilterType::LOG_FILTER_SQL, "spell_id %u listed in `spell_pending_cast` does not exist", abs(l_SpellID));
            continue;
        }

        l_SpellInfo = GetSpellInfo(abs(l_PendingID));
        if (!l_SpellInfo)
        {
            sLog->outError(LogFilterType::LOG_FILTER_SQL, "pending_id %u listed in `spell_pending_cast` does not exist", abs(l_PendingID));
            continue;
        }

        SpellPendingCast l_PendingCast;
        l_PendingCast.SpellID   = l_SpellID;
        l_PendingCast.PendingID = l_PendingID;
        l_PendingCast.Options   = l_Options;
        l_PendingCast.Check     = l_Check;

        m_SpellPendingCastMap[l_SpellID].push_back(l_PendingCast);

        ++l_Count;
    }
    while (l_Result->NextRow());

    sLog->outInfo(LogFilterType::LOG_FILTER_SERVER_LOADING, ">> Loaded %u pending spells in %u ms", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

void SpellMgr::LoadRespecializationSpells()
{
    uint32 l_OldMSTime = getMSTime();

    m_RespecializationSpellsMap.clear();  ///< Needed for reload case

    ///                                                 0          1
    QueryResult l_Result = WorldDatabase.Query("SELECT `SpellID`, `SpecID` FROM `spell_respecialization`");
    if (!l_Result)
    {
        sLog->outInfo(LogFilterType::LOG_FILTER_SERVER_LOADING, ">> Loaded 0 respecialization spells. DB table `spell_respecialization` is empty.");
        return;
    }

    uint32 l_Count = 0;
    do
    {
        Field* l_Fields     = l_Result->Fetch();
        uint8 l_Index       = 0;

        uint32 l_SpellID    = l_Fields[l_Index++].GetUInt32();
        uint32 l_SpecID     = l_Fields[l_Index++].GetUInt32();

        SpellInfo const* l_SpellInfo = GetSpellInfo(l_SpellID);
        if (!l_SpellInfo)
        {
            sLog->outError(LogFilterType::LOG_FILTER_SQL, "spell_id %u listed in `spell_respecialization` does not exist", l_SpellID);
            continue;
        }

        ChrSpecializationsEntry const* l_SpecEntry = sChrSpecializationsStore.LookupEntry(l_SpecID);
        if (!l_SpecEntry)
        {
            sLog->outError(LogFilterType::LOG_FILTER_SQL, "SpecID %u listed in `spell_respecialization` does not exist", l_SpecID);
            continue;
        }

        m_RespecializationSpellsMap[l_SpellID] = l_SpecID;

        ++l_Count;
    }
    while (l_Result->NextRow());

    sLog->outInfo(LogFilterType::LOG_FILTER_SERVER_LOADING, ">> Loaded %u respecialization spells in %u ms", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

bool SpellMgr::HaveSameItemSourceSkill(Item* p_Item1, Item* p_Item2) const
{
    if (p_Item1 == nullptr
        || p_Item2 == nullptr)
        return false;

    auto l_SourceSkill1 = sSpellMgr->GetItemSourceSkills(p_Item1->GetEntry());
    auto l_SourceSkill2 = sSpellMgr->GetItemSourceSkills(p_Item2->GetEntry());

    if (l_SourceSkill1 == nullptr
        || l_SourceSkill2 == nullptr)
        return false;

    for (auto l_Skill1 : *l_SourceSkill1)
    {
        for (auto l_Skill2 : *l_SourceSkill2)
        {
            if (l_Skill1 == l_Skill2)
                return true;
        }
    }

    return false;
}

void SpellMgr::LoadSpellDamageLog()
{
    m_SpellDamageLogMap.clear();

    QueryResult l_Result = WorldDatabase.PQuery("SELECT spell, damage FROM spell_damage_log");
    if (!l_Result)
        return;

    do
    {
        Field* l_Fields = l_Result->Fetch();
        m_SpellDamageLogMap[l_Fields[0].GetUInt32()] = l_Fields[1].GetUInt32();
    }
    while (l_Result->NextRow());
}

void SpellMgr::LoadChangeRecoveryRateSpells()
{
    /// Avenging Crusader (Honor Talent)
    m_ChangeRecoveryRateSpells[216331] =
    {
        35395,  ///< Crusader Strike
        20271   ///< Judgment
    };

    /// Derilium
    m_ChangeRecoveryRateSpells[233397] =
    {
        1850,   ///< Dash
        186247, ///< Aspect of the Cheetah
        2983,   ///< Sprint
        106898  ///< Stampending Roar
    };

    /// Thundercharge (Honor Talent) (All CoolDowns)
    m_ChangeRecoveryRateSpells[204366] = { };

    /// Blurred Time (All Cooldowns)
    m_ChangeRecoveryRateSpells[202776] = { };

    /// Shadow's Caress (Honor Talent) (All Cooldowns)
    m_ChangeRecoveryRateSpells[209427] = { };

    /// Serenity - All Chi users are reduced
    m_ChangeRecoveryRateSpells[152173] =
    {
        100784, ///< Blackout Kick
        101546, ///< Spinning Crane Kick
        113656, ///< Fists of Fury
        116847, ///< Rushing Jade Winds
        205320, ///< Strike of the Wind Lord
        205523, ///< Blackout Strike
        107428  ///< Rising Sun Kick
    };

    /// Castaway
    m_ChangeRecoveryRateSpells[198926] =
    {
        185311  ///< Crimson Vial
    };

    /// Forbearant Faithful
    m_ChangeRecoveryRateSpells[209376] =
    {
        633,    ///< Lay on Hands
        642,    ///< Divine Shield
        1022,   ///< Blessing of Protection
        150630, ///< Hand of Protection
        204018  ///< Blessing of Spellwarding
    };

    /// For all other spells
    m_ChangeRecoveryRateSpells[0] = { 0 };

    /// Change every spell that uses charge system to put the ChargeCategoryID as a negative value
    for (std::pair<uint32 const, std::set<int32>>& l_Iter : m_ChangeRecoveryRateSpells)
    {
        for (int32 const& l_Spell : l_Iter.second)
        {
            if (SpellInfo const* l_SpellInfo = GetSpellInfo(l_Spell))
            {
                if (l_SpellInfo->ChargeCategoryEntry != nullptr)
                    const_cast<int32&>(l_Spell) = -int32(l_SpellInfo->ChargeCategoryEntry->Id);
            }
        }
    }

    /// Spiritual Journey
    m_ChangeRecoveryRateSpells[214170] =
    {
        51533   ///< Feral Spirit
    };
}

std::set<int32> const& SpellMgr::GetChangeRecoveryRateSpells(uint32 spellId) const
{
    std::map<uint32, std::set<int32> >::const_iterator l_Itr = m_ChangeRecoveryRateSpells.find(spellId);
    if (l_Itr != m_ChangeRecoveryRateSpells.end())
        return l_Itr->second;

    return m_ChangeRecoveryRateSpells.at(0);
}
