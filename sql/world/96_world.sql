-- https://forum.wowcircle.com/showthread.php?t=1057139
update quest_template set RewardMailTemplateId = 500 where id = 41368;


-- https://forum.wowcircle.com/showthread.php?t=1057265
delete from spell_scripts where id in (73817);
INSERT INTO `spell_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `dataint`, `x`, `y`, `z`, `o`) VALUES 
(73817, 0, 8, 39357, 0, 0, 0, 0, 0, 0),
(73817, 2, 8, 39358, 0, 0, 0, 0, 0, 0),
(73817, 4, 8, 39359, 0, 0, 0, 0, 0, 0),
(73817, 6, 8, 39360, 0, 0, 0, 0, 0, 0);


-- https://forum.wowcircle.com/showthread.php?t=1055368
DELETE FROM `creature_questender` WHERE quest in (43341,45727);
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(90417, 43341),
(90417, 45727);

DELETE FROM `creature_queststarter` WHERE quest in (43341,45727);
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(90417, 43341),
(90417, 45727);

delete from conditions where SourceEntry in (43341,45727) and SourceTypeOrReferenceId = 19;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(19, 0, 43341, 0, 0, 17, 0, 13000, 0, 0, 1, 0, '', 'Quest if achievement not found'),
(19, 0, 45727, 0, 0, 17, 0, 13000, 0, 0, 0, 0, '', 'Quest if achievement found'),
(19, 0, 45727, 0, 0, 14, 0, 43341, 0, 0, 0, 0, '', 'Quest if quest none');


-- https://forum.wowcircle.com/showthread.php?t=1057271
DELETE FROM `spell_scene` WHERE MiscValue in (1371);
INSERT INTO `spell_scene` (`SceneScriptPackageID`, `MiscValue`, `PlaybackFlags`) VALUES
(1690, 1371, 0);

update creature_template set gossip_menu_id = 111009, AIName = 'SmartAI', npcflag = 3 where entry = 111009;
update creature_template set flags_extra = 128, InhabitType = 4, AIName = 'SmartAI' where entry = 111719;

delete from creature where id in (111719,111009);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(111719, 1512, 7834, 7834, 1, 0, 433791, 0, 0, 1368.6, 1344.27, 179.083, 6.27522, 300, 0, 0, 870, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(111009, 1512, 7834, 7834, 1, 0, 433791, 0, 0, 1366.65, 1344.28, 176.738, 6.27522, 300, 0, 0, 870, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

delete from gameobject where id = 252193;
INSERT INTO `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `ScriptName`, `custom_flags`, `VerifiedBuild`) VALUES 
(252193, 1512, 7834, 7834, 1, 0, 433790, 1368.6, 1344.27, 179.083, 0.0195227, 0, 0, 0.00976117, 0.999952, 300, 0, 1, 0, '', 0, -1);

update quest_template set InProgressPhaseID = 0 where id = 43379;

delete from spell_linked_spell where spell_trigger in (-220442) and spell_effect in (220446);
INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `hastalent`, `hastalent2`, `chance`, `cooldown`, `comment`) VALUES 
(-220442, 220446, 0, 0, 0, 0, 0, '');

delete from gossip_menu_option where menu_id in (111009);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `OptionBroadcastTextID`) VALUES 
(111009, 0, 0, 'Начинай ритуал.', 1, 1, 120207);

delete from gossip_menu where entry in (111009);
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES 
(111009, 111009);

delete from npc_text where id in (111009);
INSERT INTO `npc_text` (`ID`, `BroadcastTextID0`, `BroadcastTextID1`) VALUES 
(111009, 120206, 120206);

delete from smart_scripts where entryorguid in (111009);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(111009, 0, 0, 1, 62, 0, 100, 0, 111009, 0, 0, 0, 0, 85, 226669, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Velen - on gossip select - invoker cast'),
(111009, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 11, 222386, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Velen - on link - cast');

delete from conditions where SourceEntry in (222386) and SourceTypeOrReferenceId = 13;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(13, 1, 222386, 0, 0, 31, 0, 3, 111719, 0, 0, 0, '', 'Cast into 111719');

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 15 AND `SourceGroup` = 111009;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 111009, 0, 0, 0, 9, 0, 43379, 0, 0, 0, 0, '', '');

delete from phase_definitions where zoneId = 7834 and entry in (433791,433790);
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `flags`, `comment`) VALUES 
(7834, 433791, 0, 433791, 0, '43379 quest'),
(7834, 433790, 0, 433790, 0, '43379 quest');

delete from conditions where SourceGroup in (7834) and SourceTypeOrReferenceId = 26 and SourceEntry in (433791,433790);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 7834, 433791, 0, 0, 48, 0, 285423, 0, 0, 1, 0, '', 'Phase if object incomplete'),
(26, 7834, 433791, 0, 0, 9, 0, 43379, 0, 0, 0, 0, '', 'Phase if quest incomplete'),
(26, 7834, 433791, 0, 1, 9, 0, 43379, 0, 0, 0, 0, '', 'Phase if quest incomplete'),
(26, 7834, 433791, 0, 1, 48, 0, 285418, 0, 0, 0, 0, '', 'Phase if object complete'),
(26, 7834, 433791, 0, 2, 28, 0, 43379, 0, 0, 0, 0, '', 'Phase if quest complete'),
(26, 7834, 433790, 0, 0, 9, 0, 43379, 0, 0, 0, 0, '', 'Phase if quest incomplete'),
(26, 7834, 433790, 0, 0, 48, 0, 285423, 0, 0, 1, 0, '', 'Phase if object incomplete'),
(26, 7834, 433790, 0, 0, 48, 0, 285422, 0, 0, 0, 0, '', 'Phase if object complete');


-- https://forum.wowcircle.com/showthread.php?t=1056389
delete from creature where id in (116125,116126,116127,116128,116130);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(116125, 1220, 7543, 8560, 1, 0, 45243, 0, 0, -563.791, 2929.09, 22.8268, 2.88836, 300, 0, 0, 348, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(116126, 1220, 7543, 8560, 1, 0, 45243, 0, 0, -565.987, 2924.27, 22.206, 2.69594, 300, 0, 0, 348, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(116127, 1220, 7543, 8560, 1, 0, 45243, 0, 0, -562.5790, 2932.2383, 23.1276, 2.93941, 300, 0, 0, 348, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(116128, 1220, 7543, 8560, 1, 0, 45243, 0, 0, -564.749, 2926.5, 22.5107, 2.80589, 300, 0, 0, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(116130, 1220, 7679, 7679, 1, 0, 45243, 0, 0, -1456.88, 1177.89, 318.976, 5.14356, 300, 0, 0, 261, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

update creature_template set gossip_menu_id = 116130, AIName = 'SmartAI', npcflag = 1 where entry = 116130;

delete from gossip_menu_option where menu_id in (116130);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `OptionBroadcastTextID`) VALUES 
(116130, 0, 0, 'Пора отправляться на Расколотый берег, Даумир.', 1, 1, 128153);

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 15 AND `SourceGroup` = 116130;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 116130, 0, 0, 0, 9, 0, 45243, 0, 0, 0, 0, '', '');

delete from gossip_menu where entry in (116130);
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES 
(116130, 116130);

delete from npc_text where id in (116130);
INSERT INTO `npc_text` (`ID`, `BroadcastTextID0`, `BroadcastTextID1`) VALUES 
(116130, 128152, 128152);

delete from smart_scripts where entryorguid in (116130);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(116130, 0, 0, 1, 62, 0, 100, 0, 116130, 0, 0, 0, 0, 33, 116130, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Daumir - on gossip select - give credit'),
(116130, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 0, 85, 241279, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Daumir - on link - invoker cast'),
(116130, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 62, 1220, 0, 0, 0, 0, 0, 7, 0, 0, 0, -551.3701, 3001.5120, 39.8363, 0, 'Daumir - on link - teleport');

update quest_template set InProgressPhaseID = 45243, CompletedPhaseID = 45243 where id = 45243;


-- https://forum.wowcircle.com/showthread.php?t=1056179
DELETE FROM `spell_scene` WHERE MiscValue in (1669,1670,1671);
INSERT INTO `spell_scene` (`SceneScriptPackageID`, `MiscValue`, `PlaybackFlags`) VALUES
(1850, 1669, 0),
(1851, 1670, 0),
(1852, 1671, 0);

delete from smart_scripts where entryorguid in (269829,269827,269828);

update gameobject_template set AIName = '' where entry in (269829,269827,269828);


-- https://forum.wowcircle.com/showthread.php?t=1056176
select guid from creature where id = 96410 into @id;

select linked_id from creature where id = 96410 into @linked_id;

update creature set movementtype = 2 where id = 96410;

REPLACE INTO `creature_addon` (`linked_id`, `path_id`, `bytes1`, `bytes2`, `auras`) VALUES
(@linked_id, @id, 0, 0, '');

delete from smart_scripts where entryorguid = 96410;
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(96410, '', 0, 0, 0, 2, 0, 100, 0, 0, 95, 100, 0, 0, 0, 11, 214338, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Cast Imbued'),
(96410, '', 0, 1, 0, 2, 0, 100, 0, 0, 45, 50, 0, 0, 0, 11, 214336, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Cast Enhanced'),
(96410, '', 0, 2, 0, 2, 0, 100, 0, 0, 15, 20, 0, 0, 0, 11, 214337, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Cast Amplified'),
(96410, '', 0, 3, 0, 2, 0, 100, 0, 0, 45, 50, 0, 0, 0, 28, 214338, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Remove imbued'),
(96410, '', 0, 4, 0, 2, 0, 100, 0, 0, 15, 20, 0, 0, 0, 28, 214336, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Remove Enchanced'),
(96410, '', 0, 5, 0, 0, 0, 15, 0, 0, 5000, 6000, 11000, 14000, 0, 11, 197720, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Cast Elder Charge'),
(96410, '', 0, 6, 0, 2, 0, 100, 0, 0, 5, 10, 500000, 500000, 0, 11, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Cast Hunters Rush'),
(96410, '', 0, 7, 0, 2, 0, 100, 0, 0, 50, 90, 15000, 20000, 0, 11, 225682, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Cast Furious Swipes'),
(96410, '', 0, 8, 0, 0, 0, 100, 0, 0, 5000, 6000, 25000, 30000, 0, 11, 225320, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Cast Enrage'),
(96410, '', 0, 9, 0, 0, 0, 100, 0, 0, 12000, 15000, 30000, 32000, 0, 11, 218967, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Cast Splash'),
(96410, '', 0, 10, 0, 2, 0, 100, 0, 0, 1, 30, 10000, 10000, 0, 11, 225381, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Cast Spit Blood'),
(96410, '', 0, 11, 0, 0, 0, 100, 0, 0, 20000, 30000, 35000, 40000, 0, 11, 225312, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Cast Deep Slash'),
(96410, '', 0, 12, 0, 2, 0, 100, 0, 0, 30, 60, 75000, 80000, 0, 11, 225395, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Cast Bestial Roar'),
(96410, '', 0, 13, 0, 1, 0, 100, 0, 0, 500, 500, 500, 500, 0, 28, 214338, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Remove Imbued off combat'),
(96410, '', 0, 14, 0, 1, 0, 100, 0, 0, 500, 500, 500, 500, 0, 28, 214336, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Remove Enhanced off combat'),
(96410, '', 0, 15, 0, 1, 0, 100, 0, 0, 500, 500, 500, 500, 0, 28, 214337, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Remove Amplified off combat');

delete from waypoints where entry = 96410;

delete from waypoint_data where id = @id;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `move_type`) VALUES 
(@id, 1, 5377.54, 4361.92, 684.99, 1),
(@id, 2, 5424.8, 4298.84, 684.031, 1),
(@id, 3, 5480.87, 4310.15, 677.019, 1),
(@id, 4, 5533.03, 4308.91, 663.227, 1),
(@id, 5, 5567.42, 4283.6, 654.904, 1),
(@id, 6, 5576.95, 4264.27, 651.552, 1),
(@id, 7, 5588.26, 4247.77, 650.826, 1),
(@id, 8, 5596.33, 4213.6, 654.326, 1),
(@id, 9, 5620.15, 4193.17, 655.462, 1),
(@id, 10, 5645.02, 4164.01, 648.93, 1),
(@id, 11, 5658.66, 4146.25, 641.505, 1),
(@id, 12, 5698.44, 4143.16, 629.907, 1),
(@id, 13, 5732.16, 4143.86, 634.873, 1),
(@id, 14, 5753.35, 4155.46, 644.543, 1),
(@id, 15, 5758.11, 4176.74, 650.114, 1),
(@id, 16, 5750.6, 4213.39, 653.47, 1),
(@id, 17, 5752.87, 4240.87, 656.632, 1),
(@id, 18, 5767.93, 4279.39, 657.776, 1),
(@id, 19, 5744.39, 4346.11, 658.108, 1),
(@id, 20, 5747.13, 4382.25, 655.138, 1),
(@id, 21, 5764.45, 4425.33, 660.083, 1),
(@id, 22, 5755.46, 4484.13, 656.126, 1),
(@id, 23, 5725.75, 4508.59, 652.444, 1),
(@id, 24, 5693.83, 4550.12, 652.88, 1),
(@id, 25, 5670.14, 4561.16, 654.462, 1),
(@id, 26, 5629.65, 4549.47, 654.359, 1),
(@id, 27, 5590.16, 4510.01, 655.993, 1),
(@id, 28, 5545.31, 4483.62, 661.598, 1),
(@id, 29, 5510.42, 4402.01, 668.915, 1),
(@id, 30, 5486.37, 4394.97, 674.699, 1),
(@id, 31, 5446.42, 4402.96, 683.744, 1),
(@id, 32, 5406.63, 4398.56, 685.853, 1),
(@id, 33, 5379.44, 4369.93, 684.276, 1);