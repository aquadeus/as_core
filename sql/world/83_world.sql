-- https://forum.wowcircle.com/showthread.php?t=1055953
delete from smart_scripts where entryorguid in (73426) and id > 0;
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(73426, '', 0, 1, 2, 10, 0, 100, 0, 0, 1, 10, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Rylissa - on ooc los - talk'),
(73426, '', 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 0, 15, 38142, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Rylissa - on link - call area explored or event happens');

delete from conditions where SourceEntry in (73426) and SourceTypeOrReferenceId = 22;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 2, 73426, 0, 0, 9, 0, 38142, 0, 0, 0, 0, '', 'Smart if 38142 quest incomplete');


-- https://forum.wowcircle.com/showthread.php?t=1056189
DELETE FROM `creature_questender` WHERE quest in (34566);
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(79176, 34566);

DELETE FROM `creature_queststarter` WHERE quest in (34566);
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(79176, 34566);

delete from gameobject where id in (231231,231142,231143,231731);
INSERT INTO `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES 
(231231, 1116, 6662, 7098, 1, 1, 0, 3239.6514, 1591.0754, 163.6334, 5.63, 0, 0, 0, 0, 120, 255, 1),

(231142, 1116, 6662, 7098, 1, 0, 345662, 3219.05, 1604.84, 163.891, 5.68977, 0, 0, 0, 0, 120, 255, 1),
(231143, 1116, 6662, 7098, 1, 0, 345661, 3215.42, 1606.16, 164.963, 5.7397, 0, 0, 0, 0, 120, 255, 1),
(231731, 1116, 6662, 7098, 1, 0, 345660, 3209.02, 1605.87, 164.959, 5.77058, 0, 0, 0, 0, 120, 255, 1);

delete from phase_definitions where zoneId = 6662 and entry in (345662,345661,345660);
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `flags`, `comment`) VALUES 
(6662, 345662, 0, 345662, 0, '34566 quest'),
(6662, 345661, 0, 345661, 0, '34566 quest'),
(6662, 345660, 0, 345660, 0, '34566 quest');

delete from conditions where SourceGroup in (6662) and SourceTypeOrReferenceId = 26 and SourceEntry in (345662,345661,345660);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 6662, 345662, 0, 0, 14, 0, 34568, 0, 0, 1, 0, '', 'Phase if quest 34568 not none'),
(26, 6662, 345662, 0, 0, 28, 0, 34566, 0, 0, 0, 0, '', 'Phase if quest 34566 complete'),
(26, 6662, 345662, 0, 1, 14, 0, 34568, 0, 0, 1, 0, '', 'Phase if quest 34568 not none'),
(26, 6662, 345662, 0, 1, 8, 0, 34566, 0, 0, 0, 0, '', 'Phase if quest 34566 reward'),

(26, 6662, 345661, 0, 0, 14, 0, 34567, 0, 0, 1, 0, '', 'Phase if quest 34567 not none'),
(26, 6662, 345661, 0, 0, 28, 0, 34566, 0, 0, 0, 0, '', 'Phase if quest 34566 complete'),
(26, 6662, 345661, 0, 1, 14, 0, 34567, 0, 0, 1, 0, '', 'Phase if quest 34567 not none'),
(26, 6662, 345661, 0, 1, 8, 0, 34566, 0, 0, 0, 0, '', 'Phase if quest 34566 reward'),

(26, 6662, 345660, 0, 0, 14, 0, 34566, 0, 0, 0, 0, '', 'Phase if quest 35284 none'),
(26, 6662, 345660, 0, 1, 9, 0, 34566, 0, 0, 0, 0, '', 'Phase if quest 35284 incomplete');

delete from playerchoice_response_reward_spellcast where ChoiceId = 52 and ResponseId in (121,122);
INSERT INTO `playerchoice_response_reward_spellcast` (`ChoiceId`, `ResponseId`, `Index`, `SpellId`, `VerifiedBuild`) VALUES 
(52, 121, 0, 160027, 26365), -- armory
(52, 122, 0, 160025, 26365); -- sanctum


-- https://forum.wowcircle.com/showthread.php?t=1056289
delete from creature where id = 110419;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(110419, 1220, 7637, 8383, 1, 1, 0, 0, 0, 676.586, 4140.84, 30.8845, 0.13337, 300, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

delete from npc_spellclick_spells where npc_entry = 110419;
REPLACE INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES
(110419, 182467, 1, 0);

delete from conditions where SourceGroup in (110419) and SourceTypeOrReferenceId = 18;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(18, 110419, 182467, 0, 0, 9, 0, 43352, 0, 0, 0, 0, '', 'Spellclick if quest 43352 incomplete');

update creature_template_wdb set killcredit2 = 0 where entry = 110415;

update creature_template set AIName = 'SmartAI' where entry = 110419;

delete from smart_scripts where entryorguid in (110419);
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(110419, '', 0, 0, 0, 73, 0, 100, 0, 0, 0, 0, 0, 0, 0, 33, 110419, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Point - on gossip select - give credit');


-- https://forum.wowcircle.com/showthread.php?t=1056294
update creature_template set unit_flags = 768 where entry = 102450;


-- https://forum.wowcircle.com/showthread.php?t=1056296
delete from creature where id = 107446;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(107446, 1220, 7637, 7995, 1, 1, 0, 0, 0, 2031.4431, 5668.3691, 86.1320, 0.13337, 300, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

update creature_template set ainame = 'SmartAI', flags_extra = 128 where entry = 107446;

delete from smart_scripts where entryorguid in (107446);
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(107446, '', 0, 0, 0, 10, 0, 100, 0, 0, 1, 20, 0, 0, 0, 33, 107446, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on ooc los - give credit');

delete from conditions where SourceEntry in (107446) and SourceTypeOrReferenceId = 22;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 1, 107446, 0, 0, 9, 0, 42224, 0, 0, 0, 0, '', 'Smart if 42224 quest incomplete');

delete from quest_start_scripts where id = 42224;


-- https://forum.wowcircle.com/showthread.php?t=1056367
DELETE FROM `spell_area` WHERE `spell` in (76633) and area = 7790;
INSERT INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`) VALUES 
(76633, 7790, 38916, 39575, 0, 0, 2, 1, 74, 74);

REPLACE INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `bytes2`, `auras`) VALUES
(94571, 0, 0, 0, '76633'),
(96049, 0, 0, 0, '76633');

update creature set phaseMask = 4294967295, phaseID = 0 where id in (94571,96049);

delete from quest_start_scripts where id in (39575);
INSERT INTO `quest_start_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `dataint`, `x`, `y`, `z`, `o`) VALUES 
(39575, 0, 10, 945710, 120000, 0, 3865.9099, 4516.4600, 776.6620, 5.54),
(39575, 0, 10, 960490, 120000, 0, 3865.6001, 4512.0000, 776.6680, 0.70);

update quest_template set StartScript = 39575 where id = 39575;

delete from smart_scripts where entryorguid = 94571 and id = 1;
delete from smart_scripts where entryorguid = 96049 and id = 0;


-- https://forum.wowcircle.com/showthread.php?t=1056420
delete from gameobject where id in (229416,229415,229421);
INSERT INTO `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `ScriptName`, `custom_flags`, `VerifiedBuild`) VALUES 
(229416, 1116, 6720, 6968, 1, 1, 0, 7506.73, 4590.12, 168.596, 4.89013, 0, 0, 0.64156, -0.767073, 120, 0, 1, 0, '', 0, -1),
(229415, 1116, 6720, 6968, 1, 1, 0, 7369.08, 4726.2, 179.937, 6.25278, 0, 0, 0.0151997, -0.999884, 120, 0, 1, 0, '', 0, -1),
(229421, 1116, 6720, 6968, 1, 1, 0, 7656.01, 4822.92, 130.204, 0.511531, 0, 0, 0.252986, 0.96747, 120, 0, 1, 0, '', 0, -1);

update gameobject_template set flags = 0 where entry in (229416,229415,229421);

delete from phase_definitions where zoneId = 6720 and entry in (343151,343150);
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `flags`, `comment`) VALUES 
(6720, 343151, 0, 343151, 0, '34315 quest'),
(6720, 343150, 0, 343150, 0, '34315 quest');

delete from conditions where SourceGroup in (6720) and SourceTypeOrReferenceId = 26 and SourceEntry in (343151,343150);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 6720, 343150, 0, 0, 8, 0, 34315, 0, 0, 1, 0, '', 'Phase if quest 34568 not reward'),
(26, 6720, 343151, 0, 0, 8, 0, 34315, 0, 0, 0, 0, '', 'Phase if quest 34568 reward');

update creature set phaseMask = 0, phaseID = 343150 where id = 78659;
update creature set phaseMask = 0, phaseID = 343151 where id = 78746;

delete from gameobject_loot_template where entry in (52641,52642,52643);
INSERT INTO `gameobject_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(52641, 110606, -100, 0, 0, 1, 1),
(52642, 110607, -100, 0, 0, 1, 1),
(52643, 110608, -100, 0, 0, 1, 1);


-- https://forum.wowcircle.com/showthread.php?t=1039974
update smart_scripts set action_type = 11, action_param2 = 2 where action_param1 = 25281;