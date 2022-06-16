-- https://forum.wowcircle.com/showthread.php?t=1058463
delete from gossip_menu_option where menu_id in (11635);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `box_money`, `box_text`, `OptionBroadcastTextID`) VALUES 
(11635, 0, 0, 'Did you see who killed the Furlbrows?', 1, 1, 0, '', 42370),
(11635, 1, 0, 'Maybe a couple copper will loosen your tongue. Now tell me, did you see who killed the Furlbrows?', 1, 1, 2, 'Are you sure you want to give this hobo money?', 42371);

delete from locales_gossip_menu_option where menu_id in (11635);
INSERT INTO `locales_gossip_menu_option` (`menu_id`, `id`, `option_text_loc1`, `option_text_loc8`, `box_text_loc1`, `box_text_loc8`) VALUES 
(11635, 1, '', '', 'Are you sure you want to give this hobo money?', 'Вы уверены, что хотите дать этому бродяге денег?');

delete from creature_text where entry in (42384,42386,42391,42383);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `BroadcastTextID`, `comment`) VALUES 
(42384, 0, 0, 'You rich punks are all the same!', 12, 0, 100, 0, 0, 42384, ''),
(42384, 0, 1, 'Livin\' like an animal is hard enough without your insults!', 12, 0, 100, 0, 0, 42386, ''),
(42384, 0, 2, 'I ain\'t tellin\' you a damn thing!', 12, 0, 100, 0, 0, 42385, ''),
(42384, 0, 3, 'Now you\'ve gone and done it! TIME FOR THE FIST!', 12, 0, 100, 0, 0, 42389, ''),
(42384, 0, 4, 'Maybe I can sell your liver for some gold.', 12, 0, 100, 0, 0, 42387, ''),
(42384, 0, 5, 'I TOLD YOU TO PAY UP, PAL!', 12, 0, 100, 0, 0, 42388, ''),
(42384, 1, 0, 'Clue #1', 12, 0, 100, 0, 0, 42380, ''),
(42384, 2, 0, 'Clue #2', 12, 0, 100, 0, 0, 42381, ''),
(42384, 3, 0, 'Clue #3', 12, 0, 100, 0, 0, 42382, ''),
(42384, 4, 0, 'Clue #4', 12, 0, 100, 0, 0, 42383, ''),
(42386, 0, 0, 'You rich punks are all the same!', 12, 0, 100, 0, 0, 42384, ''),
(42386, 0, 1, 'Livin\' like an animal is hard enough without your insults!', 12, 0, 100, 0, 0, 42386, ''),
(42386, 0, 2, 'I ain\'t tellin\' you a damn thing!', 12, 0, 100, 0, 0, 42385, ''),
(42386, 0, 3, 'Now you\'ve gone and done it! TIME FOR THE FIST!', 12, 0, 100, 0, 0, 42389, ''),
(42386, 0, 4, 'Maybe I can sell your liver for some gold.', 12, 0, 100, 0, 0, 42387, ''),
(42386, 0, 5, 'I TOLD YOU TO PAY UP, PAL!', 12, 0, 100, 0, 0, 42388, ''),
(42386, 1, 0, 'Clue #1', 12, 0, 100, 0, 0, 42380, ''),
(42386, 2, 0, 'Clue #2', 12, 0, 100, 0, 0, 42381, ''),
(42386, 3, 0, 'Clue #3', 12, 0, 100, 0, 0, 42382, ''),
(42386, 4, 0, 'Clue #4', 12, 0, 100, 0, 0, 42383, ''),
(42391, 0, 0, 'You rich punks are all the same!', 12, 0, 100, 0, 0, 42384, ''),
(42391, 0, 1, 'Livin\' like an animal is hard enough without your insults!', 12, 0, 100, 0, 0, 42386, ''),
(42391, 0, 2, 'I ain\'t tellin\' you a damn thing!', 12, 0, 100, 0, 0, 42385, ''),
(42391, 0, 3, 'Now you\'ve gone and done it! TIME FOR THE FIST!', 12, 0, 100, 0, 0, 42389, ''),
(42391, 0, 4, 'Maybe I can sell your liver for some gold.', 12, 0, 100, 0, 0, 42387, ''),
(42391, 0, 5, 'I TOLD YOU TO PAY UP, PAL!', 12, 0, 100, 0, 0, 42388, ''),
(42391, 1, 0, 'Clue #1', 12, 0, 100, 0, 0, 42380, ''),
(42391, 2, 0, 'Clue #2', 12, 0, 100, 0, 0, 42381, ''),
(42391, 3, 0, 'Clue #3', 12, 0, 100, 0, 0, 42382, ''),
(42391, 4, 0, 'Clue #4', 12, 0, 100, 0, 0, 42383, ''),
(42383, 0, 0, 'You rich punks are all the same!', 12, 0, 100, 0, 0, 42384, ''),
(42383, 0, 1, 'Livin\' like an animal is hard enough without your insults!', 12, 0, 100, 0, 0, 42386, ''),
(42383, 0, 2, 'I ain\'t tellin\' you a damn thing!', 12, 0, 100, 0, 0, 42385, ''),
(42383, 0, 3, 'Now you\'ve gone and done it! TIME FOR THE FIST!', 12, 0, 100, 0, 0, 42389, ''),
(42383, 0, 4, 'Maybe I can sell your liver for some gold.', 12, 0, 100, 0, 0, 42387, ''),
(42383, 0, 5, 'I TOLD YOU TO PAY UP, PAL!', 12, 0, 100, 0, 0, 42388, ''),
(42383, 1, 0, 'Clue #1', 12, 0, 100, 0, 0, 42380, ''),
(42383, 2, 0, 'Clue #2', 12, 0, 100, 0, 0, 42381, ''),
(42383, 3, 0, 'Clue #3', 12, 0, 100, 0, 0, 42382, ''),
(42383, 4, 0, 'Clue #4', 12, 0, 100, 0, 0, 42383, '');


-- https://forum.wowcircle.com/showthread.php?t=1058279
delete from smart_scripts where entryorguid in (102600,113894);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(113894, 0, 0, 1, 10, 0, 100, 0, 1, 1, 0, 0, 0, 33, 113894, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - On OOC Los - Give Credit'),
(113894, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 19, 102600, 0, 0, 0, 0, 0, 0, 'Trigger - On Link - Set Data'),
(113894, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 20, 259009, 0, 0, 0, 0, 0, 0, 'Trigger - On Link - Set Data'),

(102600, 0, 0, 1, 10, 0, 100, 0, 1, 30, 0, 0, 0, 33, 113893, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Valtrois - On OOC Los - Give Credit'),
(102600, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Valtrois - On Link - Talk'),
(102600, 0, 2, 0, 38, 0, 100, 0, 0, 1, 0, 0, 0, 1, 1, 3000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Valtrois - On Update Data - Talk'),
(102600, 0, 3, 0, 52, 0, 100, 0, 1, 102600, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Valtrois - On Text Over - Talk'),
(102600, 0, 4, 5, 62, 0, 100, 0, 20453, 0, 0, 0, 0, 12, 102600, 1, 40000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Valtrois - On Gossip Select - summon creature'),
(102600, 0, 5, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 33, 102600, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Valtrois - on link - give credit'),
(102600, 0, 6, 7, 54, 0, 100, 0, 0, 0, 0, 0, 0, 1, 3, 10000, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Valtrois - On Link - Talk'),
(102600, 0, 7, 8, 61, 0, 100, 0, 0, 0, 0, 0, 0, 53, 0, 102600, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Valtrois - on link - start waypoint'),
(102600, 0, 8, 9, 61, 0, 100, 0, 0, 0, 0, 0, 0, 83, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Valtrois - on link - remove npcflag'),
(102600, 0, 9, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 28, 92286, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Valtrois - on link - remove aura'),
(102600, 0, 10, 0, 52, 0, 100, 0, 3, 102600, 0, 0, 0, 1, 4, 10000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Valtrois - On Text Over - Talk'),
(102600, 0, 11, 0, 52, 0, 100, 0, 4, 102600, 0, 0, 0, 1, 5, 10000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Valtrois - On Text Over - Talk'),
(102600, 0, 12, 0, 52, 0, 100, 0, 5, 102600, 0, 0, 0, 1, 6, 10000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Valtrois - On Text Over - Talk'),
(102600, 0, 13, 0, 40, 0, 100, 0, 6, 102600, 0, 0, 0, 41, 100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Valtrois - On waypoint - despawn');

delete from conditions where SourceEntry in (113894,102600) and SourceTypeOrReferenceId = 22 or SourceGroup in (20453) and SourceTypeOrReferenceId = 15;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 20453, 0, 0, 0, 28, 0, 40010, 0, 0, 1, 0, '', 'Allow Smart - If Player Not Complete Quest'),
(15, 20453, 0, 0, 0, 9, 0, 40010, 0, 0, 0, 0, '', 'Allow Smart - If Player Has Quest'),
(15, 20453, 0, 0, 0, 48, 0, 286664, 0, 0, 0, 0, '', 'Allow Smart - If Player Complete Objective'),
(15, 20453, 0, 0, 0, 48, 0, 286665, 0, 0, 0, 0, '', 'Allow Smart - If Player Complete Objective'),

(22, 1, 102600, 0, 0, 9, 0, 40010, 0, 0, 0, 0, '', 'Allow Smart - If Player Has Quest'),
(22, 1, 102600, 0, 0, 48, 0, 286664, 0, 0, 1, 0, '', 'Allow Smart - If Player Not Complete Objective'),

(22, 1, 113894, 0, 0, 9, 0, 40010, 0, 0, 0, 0, '', 'Allow Smart - If Player Has Quest'),
(22, 1, 113894, 0, 0, 48, 0, 286665, 0, 0, 1, 0, '', 'Allow Smart - If Player Not Complete Objective');

delete from phase_definitions where zoneId = 7637 and entry in (40010);
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `flags`, `comment`) VALUES 
(7637, 40010, 1, 40010, 0, '40010 Quest Phase');

delete from conditions where SourceGroup in (7637) and SourceTypeOrReferenceId = 26 and sourceentry in (40010);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 7637, 40010, 0, 0, 28, 0, 40010, 0, 0, 1, 0, '', 'Activate Phase If Player Not Complete Quest'),
(26, 7637, 40010, 0, 0, 8, 0, 40010, 0, 0, 1, 0, '', 'Activate Phase If Player Not Reward Quest');

update gameobject set phaseid = 40010, phaseMask = 0 where id = 259009;

update quest_template set prevquestid = 41028 where id = 41138;

delete from waypoints where entry in (102600);
INSERT INTO `waypoints` (`entry`, `pointid`, `position_x`, `position_y`, `position_z`) VALUES 
(102600, 1, 2000.85, 4345.72, 165.766),
(102600, 2, 2002.52, 4338.57, 166.218),
(102600, 3, 2005.19, 4322.49, 165.749),
(102600, 4, 2010.40, 4286.37, 166.384),
(102600, 5, 2013.84, 4257.42, 166.388),
(102600, 6, 2014.79, 4249.69, 166.524);

delete from spell_area where spell = 92286 and area = 8329;
INSERT INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`) VALUES 
(92286, 8329, 40010, 40010, 0, 0, 2, 1, 8, 65);

SELECT linked_id from creature where id = 102600 and position_x like '1997%' into @linked;
SELECT linked_id from creature where id = 113894 and position_x like '2011%' into @trig;

delete from creature_addon where linked_id in (@linked,@trig);
REPLACE INTO `creature_addon` (`linked_id`, `mount`, `bytes1`, `bytes2`, `auras`) VALUES
(@linked, 0, 0, 0, '92286'),
(@trig, 0, 0, 0, '92286');


-- https://forum.wowcircle.com/showthread.php?t=1058325
delete from creature where id = 75941;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(75941, 1116, 6662, 7133, 1, 1, 0, 0, 0, 2793.85, 2513.52, 121.139, 3.45212, 300, 0, 0, 146781, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);


-- https://forum.wowcircle.com/showthread.php?t=1058328
update creature_template set gossip_menu_id = entry, AIName = 'SmartAI' where entry = 82612;

delete from gossip_menu_option where menu_id in (82612);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `OptionBroadcastTextID`) VALUES 
(82612, 0, 2, 'I need a ride.', 4, 8192, 3409),
(82612, 1, 0, 'Take me to Shadow\'s Vigil.', 1, 1, 85065);

delete from gossip_menu where entry in (82612);
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES 
(82612, 82612);

delete from npc_text where id in (82612);
INSERT INTO `npc_text` (`ID`, `BroadcastTextID0`, `BroadcastTextID1`) VALUES 
(82612, 85066, 85066);

delete from smart_scripts where entryorguid in (82612);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(82612, 0, 0, 0, 62, 0, 100, 0, 82612, 1, 0, 0, 0, 85, 167328, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Hutou - on gossip select - invoker cast');

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 15 AND `SourceGroup` = 82612;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 82612, 1, 0, 0, 9, 0, 35907, 0, 0, 0, 0, '', 'Gossip if quest incomplete');

update quest_template set flags = 0 where id = 35907;

DELETE FROM `creature_queststarter` WHERE quest in (35924,35947,36028,36047,36166);
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(83608, 35924),
(83772, 35947),
(83772, 36028),
(83899, 36047),
(83903, 36166);


-- https://forum.wowcircle.com/showthread.php?t=1058417
update creature_template set gossip_menu_id = 19687, AIName = 'SmartAI' where entry = 104618;

delete from gossip_menu_option where menu_id in (19687);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `OptionBroadcastTextID`) VALUES 
(19687, 0, 0, 'Feed: 300x |TINTERFACE\ICONS\INV_MISC_ANCIENT_MANA.BLP:20|t|Hcurrency:1155|h|cFFFFFFFF Ancient Mana|h|r', 1, 1, 111319);

delete from gossip_menu where entry in (19687);
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES 
(19687, 29172);

delete from npc_text where id in (29172);
INSERT INTO `npc_text` (`ID`, `BroadcastTextID0`, `BroadcastTextID1`) VALUES 
(29172, 111318, 111318);

delete from smart_scripts where entryorguid in (104618);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(104618, 0, 0, 0, 62, 0, 100, 0, 19687, 0, 0, 0, 0, 85, 212674, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Theryn - on gossip select - invoker cast');

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 15 AND `SourceGroup` = 19687;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 19687, 0, 0, 0, 9, 0, 40324, 0, 0, 0, 0, '', 'Gossip if quest incomplete');