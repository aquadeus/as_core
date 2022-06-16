////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/* ScriptData
Name: learn_commandscript
%Complete: 100
Comment: All learn related commands
Category: commandscripts
EndScriptData */

#include "Chat.h"
#include "ScriptMgr.h"
#include "ObjectMgr.h"
#include "SpellMgr.h"
#include "SpellInfo.h"

struct ProfessionData
{
    std::string profession;
    uint32 learnSpell;
    uint32 skillId;
};

const ProfessionData profData[] =
{
    { "archaeology", 80836, 794 },
    { "alchemy", 2275, 171 },
    { "blacksmithing", 2020, 164 },
    { "cooking", 2551, 185 },
    { "enchanting", 7414, 333 },
    { "engineering", 4039, 202 },
    { "firstaid", 3279, 129 },
    { "fishing", 7733, 356 },
    { "herbalism", 2372, 182 },
    { "inscription", 45375, 773 },
    { "jewelcrafting", 25245, 755 },
    { "leatherworking", 2155, 165 },
    { "mining", 2581, 186 },
    { "skining", 8615, 393 },
    { "tailoring", 3911, 197 }
};

class learn_commandscript: public CommandScript
{
public:
    learn_commandscript() : CommandScript("learn_commandscript") { }

    ChatCommand* GetCommands() const
    {
        static ChatCommand learnAllMyCommandTable[] =
        {
            { "class",          SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false, &HandleLearnAllMyClassCommand,       "", NULL },
            { "spells",         SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false, &HandleLearnAllMySpellsCommand,      "", NULL },
            { NULL,             0,                      0,                  false, NULL,                                "", NULL }
        };

        static ChatCommand learnAllCommandTable[] =
        {
            { "my",             SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false, NULL,                                "",  learnAllMyCommandTable },
            { "gm",             SEC_GAMEMASTER,         SEC_GAMEMASTER,     false, &HandleLearnAllGMCommand,            "", NULL },
            { "crafts",         SEC_GAMEMASTER,         SEC_GAMEMASTER,     false, &HandleLearnAllCraftsCommand,        "", NULL },
            { "default",        SEC_MODERATOR,          SEC_MODERATOR,      false, &HandleLearnAllDefaultCommand,       "", NULL },
            { "lang",           SEC_MODERATOR,          SEC_MODERATOR,      false, &HandleLearnAllLangCommand,          "", NULL },
            { "recipes",        SEC_GAMEMASTER,         SEC_GAMEMASTER,     false, &HandleLearnAllRecipesCommand,       "", NULL },
            { NULL,             0,                      0,                  false, NULL,                                "", NULL }
        };

        static ChatCommand learnProfessionsCommandTable[] =
        {
            { "archaeology",    SEC_ADMINISTRATOR,      SEC_GAMEMASTER,     false, &HandleLearnArchaeologyCommand,      "", NULL },
            { "alchemy",        SEC_ADMINISTRATOR,      SEC_GAMEMASTER,     false, &HandleLearnAlchemyCommand,          "", NULL },
            { "blacksmithing",  SEC_ADMINISTRATOR,      SEC_GAMEMASTER,     false, &HandleLearnBlacksmithingCommand,    "", NULL },
            { "cooking",        SEC_ADMINISTRATOR,      SEC_GAMEMASTER,     false, &HandleLearnCookingCommand,          "", NULL },
            { "enchanting",     SEC_ADMINISTRATOR,      SEC_GAMEMASTER,     false, &HandleLearnEnchantingCommand,       "", NULL },
            { "engineering",    SEC_ADMINISTRATOR,      SEC_GAMEMASTER,     false, &HandleLearnEngineeringCommand,      "", NULL },
            { "firstaid",       SEC_ADMINISTRATOR,      SEC_GAMEMASTER,     false, &HandleLearnFirstAidCommand,         "", NULL },
            { "fishing",        SEC_ADMINISTRATOR,      SEC_GAMEMASTER,     false, &HandleLearnFishingCommand,          "", NULL },
            { "herbalism",      SEC_ADMINISTRATOR,      SEC_GAMEMASTER,     false, &HandleLearnHerbalismCommand,        "", NULL },
            { "inscription",    SEC_ADMINISTRATOR,      SEC_GAMEMASTER,     false, &HandleLearnInscriptionCommand,      "", NULL },
            { "jewelcrafting",  SEC_ADMINISTRATOR,      SEC_GAMEMASTER,     false, &HandleLearnJewelcraftingCommand,    "", NULL },
            { "leatherworking", SEC_ADMINISTRATOR,      SEC_GAMEMASTER,     false, &HandleLearnLeatherworkingCommand,   "", NULL },
            { "mining",         SEC_ADMINISTRATOR,      SEC_GAMEMASTER,     false, &HandleLearnMiningCommand,           "", NULL },
            { "skining",        SEC_ADMINISTRATOR,      SEC_GAMEMASTER,     false, &HandleLearnSkiningCommand,          "", NULL },
            { "tailoring",      SEC_ADMINISTRATOR,      SEC_GAMEMASTER,     false, &HandleLearnTailoringCommand,        "", NULL },
            { NULL,             0,                      0,                  false, NULL,                                "", NULL }
        };

        static ChatCommand learnCommandTable[] =
        {
            { "all",            SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false, NULL,                                "",  learnAllCommandTable },
            { "profession",     SEC_GAMEMASTER,         SEC_GAMEMASTER,     false, NULL,                                "",  learnProfessionsCommandTable },
            { "",               SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false, &HandleLearnCommand,                 "", NULL },
            { NULL,             0,                      0,                  false, NULL,                                "", NULL }
        };

        static ChatCommand commandTable[] =
        {
            { "learn",          SEC_MODERATOR,          SEC_MODERATOR,      false, NULL,                                "", learnCommandTable },
            { "unlearn",        SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false, &HandleUnLearnCommand,               "", NULL },
            { NULL,             0,                      0,                  false, NULL,                                "", NULL }
        };
        return commandTable;
    }

    static bool HandleLearnCommand(ChatHandler* handler, char const* args)
    {
        Player* targetPlayer = handler->getSelectedPlayer();

        if (!targetPlayer)
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form
        uint32 spell = handler->extractSpellIdFromLink((char*)args);
        if (!spell || !sSpellMgr->GetSpellInfo(spell))
            return false;

        char const* all = strtok(NULL, " ");
        bool allRanks = all ? (strncmp(all, "all", strlen(all)) == 0) : false;

        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spell);
        if (!spellInfo || !SpellMgr::IsSpellValid(spellInfo, handler->GetSession()->GetPlayer()))
        {
            handler->PSendSysMessage(LANG_COMMAND_SPELL_BROKEN, spell);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!allRanks && targetPlayer->HasSpell(spell))
        {
            if (targetPlayer == handler->GetSession()->GetPlayer())
                handler->SendSysMessage(LANG_YOU_KNOWN_SPELL);
            else
                handler->PSendSysMessage(LANG_TARGET_KNOWN_SPELL, handler->GetNameLink(targetPlayer).c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (allRanks)
            targetPlayer->learnSpellHighRank(spell);
        else
            targetPlayer->learnSpell(spell, false);

        return true;
    }

    static bool HandleLearnAllGMCommand(ChatHandler* handler, char const* /*args*/)
    {
        for (uint32 i = 0; i < sSpellMgr->GetSpellInfoStoreSize(); ++i)
        {
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(i);
            if (!spellInfo || !SpellMgr::IsSpellValid(spellInfo, handler->GetSession()->GetPlayer(), false))
                continue;

            if (!spellInfo->IsAbilityOfSkillType(SKILL_INTERNAL))
                continue;

            handler->GetSession()->GetPlayer()->learnSpell(i, false);
        }

        handler->SendSysMessage(LANG_LEARNING_GM_SKILLS);
        return true;
    }

    static bool HandleLearnAllMyClassCommand(ChatHandler* handler, char const* /*args*/)
    {
        HandleLearnAllMySpellsCommand(handler, "");
        return true;
    }

    static bool HandleLearnAllMySpellsCommand(ChatHandler* handler, char const* /*args*/)
    {
        ChrClassesEntry const* classEntry = sChrClassesStore.LookupEntry(handler->GetSession()->GetPlayer()->getClass());
        if (!classEntry)
            return true;
        uint32 family = classEntry->SpellClassSet;

        for (uint32 i = 0; i < sSkillLineAbilityStore.GetNumRows(); ++i)
        {
            SkillLineAbilityEntry const* entry = sSkillLineAbilityStore.LookupEntry(i);
            if (!entry)
                continue;

            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(entry->spellId);
            if (!spellInfo)
                continue;

            // skip server-side/triggered spells
            if (spellInfo->SpellLevel == 0)
                continue;

            // skip wrong class/race skills
            if (!handler->GetSession()->GetPlayer()->IsSpellFitByClassAndRace(spellInfo->Id))
                continue;

            // skip other spell families
            if (spellInfo->SpellFamilyName != family)
                continue;

            // skip broken spells
            if (!SpellMgr::IsSpellValid(spellInfo, handler->GetSession()->GetPlayer(), false))
                continue;

            handler->GetSession()->GetPlayer()->learnSpell(spellInfo->Id, false);
        }

        handler->SendSysMessage(LANG_COMMAND_LEARN_CLASS_SPELLS);
        return true;
    }


    static bool HandleLearnAllLangCommand(ChatHandler* handler, char const* /*args*/)
    {
        // skipping UNIVERSAL language (0)
        for (uint8 i = 1; i < LANGUAGES_COUNT; ++i)
            handler->GetSession()->GetPlayer()->learnSpell(lang_description[i].spell_id, false);

        handler->SendSysMessage(LANG_COMMAND_LEARN_ALL_LANG);
        return true;
    }

    static bool HandleLearnAllDefaultCommand(ChatHandler* handler, char const* args)
    {
        Player* target;
        if (!handler->extractPlayerTarget((char*)args, &target))
            return false;

        target->LearnDefaultSkills();
        target->LearnCustomSpells();
        target->learnQuestRewardedSpells();

        handler->PSendSysMessage(LANG_COMMAND_LEARN_ALL_DEFAULT_AND_QUEST, handler->GetNameLink(target).c_str());
        return true;
    }

    static bool HandleLearnAllCraftsCommand(ChatHandler* handler, char const* /*args*/)
    {
        for (uint32 i = 0; i < sSkillLineStore.GetNumRows(); ++i)
        {
            SkillLineEntry const* skillInfo = sSkillLineStore.LookupEntry(i);
            if (!skillInfo)
                continue;

            if ((skillInfo->CategoryID == SKILL_CATEGORY_PROFESSION || skillInfo->CategoryID == SKILL_CATEGORY_SECONDARY) &&
                skillInfo->CanLink)                             // only prof. with recipes have
            {
                HandleLearnSkillRecipesHelper(handler->GetSession()->GetPlayer(), skillInfo->id);
            }
        }

        handler->SendSysMessage(LANG_COMMAND_LEARN_ALL_CRAFT);
        return true;
    }

    static bool HandleLearnAllRecipesCommand(ChatHandler* handler, char const* args)
    {
        //  Learns all recipes of specified profession and sets skill to max
        //  Example: .learn all_recipes enchanting

        Player* target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            return false;
        }

        if (!*args)
            return false;

        std::wstring namePart;

        if (!Utf8toWStr(args, namePart))
            return false;

        // converting string that we try to find to lower case
        wstrToLower(namePart);

        std::string name;

        SkillLineEntry const* targetSkillInfo = NULL;
        for (uint32 i = 1; i < sSkillLineStore.GetNumRows(); ++i)
        {
            SkillLineEntry const* skillInfo = sSkillLineStore.LookupEntry(i);
            if (!skillInfo)
                continue;

            if ((skillInfo->CategoryID != SKILL_CATEGORY_PROFESSION &&
                skillInfo->CategoryID != SKILL_CATEGORY_SECONDARY) ||
                !skillInfo->CanLink)                            // only prof with recipes have set
                continue;

            name = skillInfo->DisplayName->Get(sWorld->GetDefaultDb2Locale());
            if (name.empty())
                continue;

            if (!Utf8FitTo(name, namePart))
                continue;

            targetSkillInfo = skillInfo;
        }

        if (!targetSkillInfo)
            return false;

        HandleLearnSkillRecipesHelper(target, targetSkillInfo->id);

        uint16 maxLevel = target->GetPureMaxSkillValue(targetSkillInfo->id);
        target->SetSkill(targetSkillInfo->id, target->GetSkillStep(targetSkillInfo->id), maxLevel, maxLevel);
        handler->PSendSysMessage(LANG_COMMAND_LEARN_ALL_RECIPES, name.c_str());
        return true;
    }

    static void HandleLearnSkillRecipesHelper(Player* player, uint32 skillId)
    {
        uint32 classmask = player->getClassMask();

        for (uint32 j = 0; j < sSkillLineAbilityStore.GetNumRows(); ++j)
        {
            SkillLineAbilityEntry const* skillLine = sSkillLineAbilityStore.LookupEntry(j);
            if (!skillLine)
                continue;

            // wrong skill
            if (skillLine->SkillLine != skillId)
                continue;

            // not high rank
            if (skillLine->SupercedesSpell)
                continue;

            // skip racial skills
            if (skillLine->RaceMask != 0)
                continue;

            // skip wrong class skills
            if (skillLine->ClassMask && (skillLine->ClassMask & classmask) == 0)
                continue;

            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(skillLine->spellId);
            if (!spellInfo || !SpellMgr::IsSpellValid(spellInfo, player, false))
                continue;

            player->learnSpell(skillLine->spellId, false);
        }
    }

    static bool HandleUnLearnCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r
        uint32 spellId = handler->extractSpellIdFromLink((char*)args);
        if (!spellId)
            return false;

        char const* allStr = strtok(NULL, " ");
        bool allRanks = allStr ? (strncmp(allStr, "all", strlen(allStr)) == 0) : false;

        Player* target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (allRanks)
            spellId = sSpellMgr->GetFirstSpellInChain (spellId);

        if (target->HasSpell(spellId))
            target->removeSpell(spellId, false, !allRanks);
        else
            handler->SendSysMessage(LANG_FORGET_SPELL);

        return true;
    }

    static bool HandleLearnArchaeologyCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        ProfessionData _data = profData[0];
        LearnProfession(handler, args, _data.learnSpell, _data.skillId);

        return true;
    }

    static bool HandleLearnAlchemyCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        ProfessionData _data = profData[1];
        LearnProfession(handler, args, _data.learnSpell, _data.skillId);

        return true;
    }

    static bool HandleLearnBlacksmithingCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        ProfessionData _data = profData[2];
        LearnProfession(handler, args, _data.learnSpell, _data.skillId);

        return true;
    }

    static bool HandleLearnCookingCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        ProfessionData _data = profData[3];
        LearnProfession(handler, args, _data.learnSpell, _data.skillId);

        return true;
    }

    static bool HandleLearnEnchantingCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        ProfessionData _data = profData[4];
        LearnProfession(handler, args, _data.learnSpell, _data.skillId);

        return true;
    }

    static bool HandleLearnEngineeringCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        ProfessionData _data = profData[5];
        LearnProfession(handler, args, _data.learnSpell, _data.skillId);

        return true;
    }

    static bool HandleLearnFirstAidCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        ProfessionData _data = profData[6];
        LearnProfession(handler, args, _data.learnSpell, _data.skillId);

        return true;
    }

    static bool HandleLearnFishingCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        ProfessionData _data = profData[7];
        LearnProfession(handler, args, _data.learnSpell, _data.skillId);

        return true;
    }

    static bool HandleLearnHerbalismCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        ProfessionData _data = profData[8];
        LearnProfession(handler, args, _data.learnSpell, _data.skillId);

        return true;
    }

    static bool HandleLearnInscriptionCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        ProfessionData _data = profData[9];
        LearnProfession(handler, args, _data.learnSpell, _data.skillId);

        return true;
    }

    static bool HandleLearnJewelcraftingCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        ProfessionData _data = profData[10];
        LearnProfession(handler, args, _data.learnSpell, _data.skillId);

        return true;
    }

    static bool HandleLearnLeatherworkingCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        ProfessionData _data = profData[11];
        LearnProfession(handler, args, _data.learnSpell, _data.skillId);

        return true;
    }

    static bool HandleLearnMiningCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        ProfessionData _data = profData[12];
        LearnProfession(handler, args, _data.learnSpell, _data.skillId);

        return true;
    }

    static bool HandleLearnSkiningCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        ProfessionData _data = profData[13];
        LearnProfession(handler, args, _data.learnSpell, _data.skillId);

        return true;
    }

    static bool HandleLearnTailoringCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        ProfessionData _data = profData[14];
        LearnProfession(handler, args, _data.learnSpell, _data.skillId);

        return true;
    }

    static void LearnProfession(ChatHandler* handler, char const* args, uint32 profession, uint32 skillId)
    {
        uint32 profLevel = uint32(atoi(args));
        if (profLevel <= 0)
        {
            handler->PSendSysMessage("Error! Enter profession skill level.");
            return;
        }

        auto target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return;
        }

        target->CastSpell(target, profession, true);
        target->AddDelayedEvent([target, skillId, profLevel]
            {
                if (target && skillId && profLevel)
                    target->SetSkill(skillId, target->GetSkillStep(skillId), profLevel, profLevel);
            }, 1000);
    }
};

#ifndef __clang_analyzer__
void AddSC_learn_commandscript()
{
    new learn_commandscript();
}
#endif
