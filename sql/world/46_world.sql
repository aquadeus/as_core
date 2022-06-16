-- https://forum.wowcircle.com/showthread.php?t=1040635
update quest_template set prevquestid = 34685 where id = 33740;


-- https://forum.wowcircle.com/showthread.php?t=1040638
update quest_template set rewardspellcast = 0 where id = 34751; -- 161228
update quest_template set rewardspellcast = 0 where id = 34761; -- 161285

update quest_template set rewardspellcast = 161228 where id = 35238;
update quest_template set rewardspellcast = 161285 where id = 35239;


-- https://forum.wowcircle.com/showthread.php?t=1041083
update quest_template set prevquestid = 34579, flags = 0 where id = 34837;

DELETE FROM `creature_queststarter` WHERE id = 79356 and quest = 34837;
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(79356, 34837);

DELETE FROM `creature_questender` WHERE id = 80229 and quest = 34837;
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(80229, 34837);


-- https://forum.wowcircle.com/showthread.php?t=1041087
update creature_template set unit_flags = 768, faction = 7 where entry = 77737;


-- https://forum.wowcircle.com/showthread.php?t=1041090
update quest_template set RequiredRaces = '824181832' where id = 34234;


-- https://forum.wowcircle.com/showthread.php?t=1041097
update quest_template set flags = 3211264 where id = 34163;


-- https://forum.wowcircle.com/showthread.php?t=1041528
update quest_template set prevquestid = 0 where id = 30571;
update quest_template set nextquestid = 30571 where id in (30569,30570);


-- https://forum.wowcircle.com/showthread.php?t=1041944

DELETE FROM `creature_queststarter` WHERE id = 80470 and quest = 34838;
DELETE FROM `creature_queststarter` WHERE id = 80469 and quest = 34886;
DELETE FROM `creature_queststarter` WHERE id = 85320 and quest = 34898;
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(80470, 34838),
(80469, 34886),
(85320, 34898);

DELETE FROM `creature_questender` WHERE id = 80469 and quest = 34838;
DELETE FROM `creature_questender` WHERE id = 80469 and quest = 34886;
DELETE FROM `creature_questender` WHERE id = 85320 and quest = 34898;
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(80469, 34838),
(80469, 34886),
(85320, 34898);

delete from creature where id = 85320;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`) VALUES 
(85320, 1116, 6722, 7036, 1, 1, 0, 59518, 0, -462.592, 1881.33, 46.1369, 5.40262, 180, 0, 0, 45530, 0, 0);


-- https://forum.wowcircle.com/showthread.php?t=1041992
DELETE FROM `creature_queststarter` WHERE quest = 49293;
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(120978, 49293),
(126408, 49293),
(127051, 49293),
(127057, 49293);


DELETE FROM `creature_questender` WHERE quest = 49293;
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(120978, 49293),
(126408, 49293),
(127051, 49293),
(127057, 49293);


-- https://forum.wowcircle.com/showthread.php?t=1042400

DELETE FROM `creature_questender` WHERE quest = 48066;
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(93844, 48066);

update creature_template set npcflag = 2 where entry = 93844;

delete from creature where id in (93844);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`) VALUES
(93844, 1, 1638, 0, 1, 1, 0, 0, 0, -1210.186, -125.8802, 163.8822, 1.57, 180, 0, 0, 87000, 0, 0);


-- https://forum.wowcircle.com/showthread.php?t=1042406
delete from creature where id = 131478;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`) VALUES 
(131478, 1220, 7637, 8345, 1, 1, 0, 0, 0, 1086.76, 3000.72, 4.51622, 0.524154, 180, 0, 0, 1305, 0, 0);

DELETE FROM `creature_questender` WHERE quest = 49973;
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(131478, 49973);

update creature_template set npcflag = 2 where entry = 131478;


-- https://forum.wowcircle.com/showthread.php?t=1042736
delete from smart_scripts where action_param1 in (28763,28764) and action_type = 33;


-- https://forum.wowcircle.com/showthread.php?t=1042743
-- нпц под контролем игрока находятся в комбате и не могут кастовать спелы например 51852
-- update quest_template set method = 2 where id = 12641;
update creature_template_wdb set displayid1 = 26320, displayid2 = 0 where entry = 28511;


-- https://forum.wowcircle.com/showthread.php?t=1043741
delete from creature_text where entry in (131345,132382);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextID`, `comment`) VALUES 
(131345, 0, 0, 'Some will tell you that the Light is the only path. The one weapon that can stop the enemies of Azeroth.', 12, 0, 100, 0, 0, 0, 143203, 'Alleria'),
(131345, 1, 0, 'But we see alternatives. Many roads--many possibilities--that are open to us.', 12, 0, 100, 0, 0, 0, 143204, 'Alleria'),
(131345, 2, 0, 'Use the lessons I have taught you to maintain control. Become a weapon for the Alliance... even as you skirt the edge of darkness.', 12, 0, 100, 0, 0, 0, 143206, 'Alleria'),
(132382, 0, 0, 'There will be those who doubt you. Who question your resolve, your ability to harness powers that have caused the downfall of weaker wills.', 12, 0, 100, 0, 0, 0, 143205, 'Umbric'),
(132382, 1, 0, 'Together, we will prove them wrong.', 12, 0, 100, 0, 0, 0, 143615, 'Umbric');


-- https://forum.wowcircle.com/showthread.php?t=1043842
DELETE FROM `creature_questender` WHERE quest = 28258;
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(914, 28258);


-- https://forum.wowcircle.com/showthread.php?t=1043846
update creature set movementtype = 0, spawndist = 0 where id = 33181;

update quest_template set flags = 0 where id = 13563; -- 134217736


-- https://forum.wowcircle.com/showthread.php?t=1044080
delete from creature where id = 82136;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`) VALUES 
(82136, 1116, 6722, 7208, 1, 1, 0, 0, 0, -678.036, 2411.28, 20.7767, 4.94992, 180, 0, 0, 0, 0, 0);


-- https://forum.wowcircle.com/showthread.php?t=1044083
delete from npc_spellclick_spells where npc_entry = 80957;
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES 
(80957, 171738, 1, 0);

delete from creature where id in (80957);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(80957, 1116, 6662, 7131, 1, 1, 0, 0, 0, 1297.93, 1815.9, 344.003, 0, 120, 0, 0, 1, 0, 0, 16777216, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(80957, 1116, 6662, 7131, 1, 1, 0, 0, 0, 1321.7, 1743.17, 315.991, 0, 120, 0, 0, 1, 0, 0, 16777216, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(80957, 1116, 6662, 7131, 1, 1, 0, 0, 0, 1439.09, 1733.41, 306.454, 0, 120, 0, 0, 1, 0, 0, 16777216, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(80957, 1116, 6662, 7131, 1, 1, 0, 0, 0, 1148.39, 1715.57, 329.807, 0, 120, 0, 0, 1, 0, 0, 16777216, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(80957, 1116, 6662, 7131, 1, 1, 0, 0, 0, 1319.89, 1798.46, 344.115, 0, 120, 0, 0, 1, 0, 0, 16777216, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(80957, 1116, 6662, 7131, 1, 1, 0, 0, 0, 1259.44, 1708.11, 314.108, 0, 120, 0, 0, 1, 0, 0, 16777216, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(80957, 1116, 6662, 7131, 1, 1, 0, 0, 0, 1361.61, 1775.8, 316.314, 0, 120, 0, 0, 1, 0, 0, 16777216, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(80957, 1116, 6662, 7131, 1, 1, 0, 0, 0, 1350.41, 1606.37, 304.893, 0, 120, 0, 0, 1, 0, 0, 16777216, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(80957, 1116, 6662, 7131, 1, 1, 0, 0, 0, 1342.12, 1713.1, 316.977, 0, 120, 0, 0, 1, 0, 0, 16777216, 0, 0, 0, 0, 0, 0, 0, '', '', -1);


update creature_template set ainame = 'SmartAI', npcflag = 16777216 where entry = 80957;

delete from smart_scripts where entryorguid in (80957);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(80957, 0, 0, 0, 73, 0, 100, 1, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Case - on spellclick - despawn');


delete from conditions where SourceGroup in (80957) and SourceTypeOrReferenceId = 18;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(18, 80957, 171738, 0, 0, 9, 0, 34977, 0, 0, 0, 0, '', 'Allow spellclick if player has Quest');


-- https://forum.wowcircle.com/showthread.php?t=1044093
update creature_template set unit_flags = 768, npcflag = 16777216 where entry = 75809;

delete from npc_spellclick_spells where npc_entry = 75809;
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES 
(75809, 152903, 1, 0);

delete from conditions where SourceGroup in (75809) and SourceTypeOrReferenceId = 18;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(18, 75809, 152903, 0, 0, 9, 0, 34091, 0, 0, 0, 0, '', 'Allow spellclick if player has Quest');

delete from creature where id = 75869;

update creature_template set ainame = 'SmartAI', npcflag = 16777216 where entry = 75869;
update creature_template set ainame = 'SmartAI', flags_extra = 128 where entry = 88765;

delete from npc_spellclick_spells where npc_entry = 75869;
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES 
(75869, 46598, 1, 0);

delete from smart_scripts where entryorguid in (75869,88765);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(75869, 0, 0, 5, 54, 0, 100, 1, 0, 0, 0, 0, 0, 85, 46598, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Shreder - on just summoned - invoker cast'),
(75869, 0, 1, 0, 28, 0, 100, 1, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Shreder - on just summoned - invoker cast'),
(75869, 0, 2, 3, 38, 0, 100, 1, 0, 1, 0, 0, 0, 33, 88765, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Shreder - on set data - give credit'),
(75869, 0, 3, 4, 61, 0, 100, 1, 0, 0, 0, 0, 0, 28, 152903, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Shreder - on link - remove aura'),
(75869, 0, 4, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 28, 46598, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Shreder - on link - remove aura'),
(75869, 0, 5, 0, 61, 0, 100, 1, 0, 1, 0, 0, 0, 33, 75809, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Shreder - on link - give credit'),

(88765, 0, 0, 0, 60, 0, 100, 0, 0, 0, 5000, 5000, 0, 45, 0, 1, 0, 0, 0, 0, 19, 75869, 20, 0, 0, 0, 0, 0, 'Shreder - on set data - give credit');

delete from creature where id = 88765;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(88765, 1116, 6662, 7132, 1, 1, 0, 0, 0, 3398.39, 2066.44, 160.399, 0.992086, 300, 0, 0, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);



-- https://forum.wowcircle.com/showthread.php?t=1044095
update quest_template set flags = 0 where id = 34090;

delete from creature where id in (75892,75894,75895);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(75892, 1116, 6662, 6947, 1, 1, 0, 0, 0, 3815.71, 2376.29, 10.9281, 4.39716, 300, 0, 0, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(75894, 1116, 6662, 6947, 1, 1, 0, 0, 0, 4042.39, 2454.81, 12.5012, 3.02272, 300, 0, 0, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(75895, 1116, 6662, 6947, 1, 1, 0, 0, 0, 3931.62, 2479.53, 2.29985, 1.12991, 300, 0, 0, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),

(75894, 1116, 6662, 6950, 1, 1, 0, 0, 0, 3365.2986, 1868.7085, 167.3140, 4.39716, 300, 0, 0, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(75892, 1116, 6662, 6950, 1, 1, 0, 0, 0, 3123.9189, 1963.0031, 116.9784, 3.02272, 300, 0, 0, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(75895, 1116, 6662, 6950, 1, 1, 0, 0, 0, 3363.5215, 2199.2278, 123.7266, 1.12991, 300, 0, 0, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

update creature_template set ainame = 'SmartAI', flags_extra = 128 where entry in (75892,75894,75895);
update creature_template set ainame = 'SmartAI' where entry in (75880,75881);

delete from smart_scripts where entryorguid in (75892,75894,75895,75880,75881);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(75892, 0, 0, 0, 10, 0, 100, 0, 1, 15, 60000, 60000, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - On OOC Los - Talk'),
(75894, 0, 0, 0, 10, 0, 100, 0, 1, 15, 60000, 60000, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - On OOC Los - Talk'),
(75895, 0, 0, 0, 10, 0, 100, 0, 1, 15, 60000, 60000, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - On OOC Los - Talk'),

(75892, 0, 1, 0, 10, 0, 100, 0, 1, 15, 60000, 60000, 0, 1, 1, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - On OOC Los - Talk'),
(75894, 0, 1, 0, 10, 0, 100, 0, 1, 15, 60000, 60000, 0, 1, 1, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - On OOC Los - Talk'),
(75895, 0, 1, 0, 10, 0, 100, 0, 1, 15, 60000, 60000, 0, 1, 1, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - On OOC Los - Talk'),

(75880, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 69, 1, 0, 0, 0, 0, 0, 19, 75892, 0, 0, 0, 0, 0, 0, 'Trigger - On Just Summoned - Move To Position'),
(75880, 0, 1, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 69, 2, 0, 0, 0, 0, 0, 19, 75894, 0, 0, 0, 0, 0, 0, 'Trigger - On Just Summoned - Move To Position'),
(75880, 0, 2, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 69, 3, 0, 0, 0, 0, 0, 19, 75895, 0, 0, 0, 0, 0, 0, 'Trigger - On Just Summoned - Move To Position'),
(75880, 0, 3, 4, 34, 0, 100, 1, 0, 1, 0, 0, 0, 11, 152986, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Bomb - On Movementinform - Cast Spell'),
(75880, 0, 4, 9, 61, 0, 100, 1, 0, 0, 0, 0, 0, 33, 75892, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Bomb - On Link - Give Credit'),
(75880, 0, 5, 6, 34, 0, 100, 1, 0, 2, 0, 0, 0, 11, 152986, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Bomb - On Movementinform - Cast Spell'),
(75880, 0, 6, 9, 61, 0, 100, 1, 0, 0, 0, 0, 0, 33, 75894, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Bomb - On Link - Give Credit'),
(75880, 0, 7, 8, 34, 0, 100, 1, 0, 3, 0, 0, 0, 11, 152986, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Bomb - On Movementinform - Cast Spell'),
(75880, 0, 8, 9, 61, 0, 100, 1, 0, 0, 0, 0, 0, 33, 75895, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Bomb - On Link - Give Credit'),
(75880, 0, 9, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Bomb - On Link - Despawn'),

(75881, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 69, 1, 0, 0, 0, 0, 0, 19, 75892, 0, 0, 0, 0, 0, 0, 'Trigger - On Just Summoned - Move To Position'),
(75881, 0, 1, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 69, 2, 0, 0, 0, 0, 0, 19, 75894, 0, 0, 0, 0, 0, 0, 'Trigger - On Just Summoned - Move To Position'),
(75881, 0, 2, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 69, 3, 0, 0, 0, 0, 0, 19, 75895, 0, 0, 0, 0, 0, 0, 'Trigger - On Just Summoned - Move To Position'),
(75881, 0, 3, 4, 34, 0, 100, 1, 0, 1, 0, 0, 0, 11, 152986, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Bomb - On Movementinform - Cast Spell'),
(75881, 0, 4, 9, 61, 0, 100, 1, 0, 0, 0, 0, 0, 33, 75892, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Bomb - On Link - Give Credit'),
(75881, 0, 5, 6, 34, 0, 100, 1, 0, 2, 0, 0, 0, 11, 152986, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Bomb - On Movementinform - Cast Spell'),
(75881, 0, 6, 9, 61, 0, 100, 1, 0, 0, 0, 0, 0, 33, 75894, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Bomb - On Link - Give Credit'),
(75881, 0, 7, 8, 34, 0, 100, 1, 0, 3, 0, 0, 0, 11, 152986, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Bomb - On Movementinform - Cast Spell'),
(75881, 0, 8, 9, 61, 0, 100, 1, 0, 0, 0, 0, 0, 33, 75895, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Bomb - On Link - Give Credit'),
(75881, 0, 9, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Bomb - On Link - Despawn');

delete from conditions where SourceEntry in (75892,75894,75895,75880,75881) and SourceTypeOrReferenceId = 22;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 1, 75892, 0, 0, 9, 0, 33721, 0, 0, 0, 0, '', 'Smart if player has quest 33721'),
(22, 1, 75894, 0, 0, 9, 0, 33721, 0, 0, 0, 0, '', 'Smart if player has quest 33721'),
(22, 1, 75895, 0, 0, 9, 0, 33721, 0, 0, 0, 0, '', 'Smart if player has quest 33721'),

(22, 1, 75892, 0, 0, 9, 0, 34090, 0, 0, 0, 0, '', 'Smart if player has quest 34090'),
(22, 1, 75894, 0, 0, 9, 0, 34090, 0, 0, 0, 0, '', 'Smart if player has quest 34090'),
(22, 1, 75895, 0, 0, 9, 0, 34090, 0, 0, 0, 0, '', 'Smart if player has quest 34090');

INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 1, 75880, 0, 0, 29, 0, 75892, 30, 0, 0, 0, '', 'allow smart if object near 75892 creature'),
(22, 2, 75880, 0, 0, 29, 0, 75894, 30, 0, 0, 0, '', 'allow smart if object near 75894 creature'),
(22, 3, 75880, 0, 0, 29, 0, 75895, 30, 0, 0, 0, '', 'allow smart if object near 75895 creature'),

(22, 1, 75881, 0, 0, 29, 0, 75892, 30, 0, 0, 0, '', 'allow smart if object near 75892 creature'),
(22, 2, 75881, 0, 0, 29, 0, 75894, 30, 0, 0, 0, '', 'allow smart if object near 75894 creature'),
(22, 3, 75881, 0, 0, 29, 0, 75895, 30, 0, 0, 0, '', 'allow smart if object near 75895 creature');

delete from creature_text where entry in (75892,75894,75895);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Emote`, `Sound`, `BroadcastTextID`, `comment`) VALUES 
(75892, 0, 0, 'Gazlowe\'s Solution is ready to deploy!', 41, 0, 0, 0, 78871, ''),
(75894, 0, 0, 'Gazlowe\'s Solution is ready to deploy!', 41, 0, 0, 0, 78871, ''),
(75895, 0, 0, 'Gazlowe\'s Solution is ready to deploy!', 41, 0, 0, 0, 78871, ''),

(75892, 1, 0, 'Gazlowe\'s Solution is ready to deploy!', 41, 0, 0, 0, 80164, ''),
(75894, 1, 0, 'Gazlowe\'s Solution is ready to deploy!', 41, 0, 0, 0, 80164, ''),
(75895, 1, 0, 'Gazlowe\'s Solution is ready to deploy!', 41, 0, 0, 0, 80164, '');


-- https://forum.wowcircle.com/showthread.php?t=1044155
update creature_template set gossip_menu_id = 13225, npcflag = 1, faction = 7, unit_flags = 768 where entry = 56206;
update creature_template set gossip_menu_id = 13226, npcflag = 1, faction = 7, unit_flags = 768 where entry = 56209;
update creature_template set gossip_menu_id = 13227, npcflag = 1, faction = 7, unit_flags = 768 where entry = 56210;

delete from gossip_menu_option where menu_id in (13225,13226,13227);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `OptionBroadcastTextID`) VALUES 
(13225, 0, 0, 'I challenge you!', 1, 1, 67113),
(13226, 0, 0, 'I challenge you!', 1, 1, 67113),
(13227, 0, 0, 'I challenge you!', 1, 1, 67113);

delete from gossip_menu where entry in (13225,13226,13227);
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES 
(13225, 18646),
(13226, 18643),
(13227, 18644);

delete from npc_text where id in (18646,18643,18644);
INSERT INTO `npc_text` (`ID`, `BroadcastTextID0`) VALUES 
(18643, 54787),
(18644, 54792),
(18646, 54794);