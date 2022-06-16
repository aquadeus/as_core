-- https://forum.wowcircle.com/showthread.php?t=1061653
update creature_template set npcflag = 16777216 where entry = 41848;

delete from npc_spellclick_spells where npc_entry in (41848);
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES 
(41848, 78025, 1, 0);

delete from conditions where SourceGroup in (41848) and SourceTypeOrReferenceId = 18;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(18, 41848, 78025, 0, 0, 14, 0, 25998, 0, 0, 1, 0, '', 'spellclick if quest not none');


-- https://forum.wowcircle.com/showthread.php?t=1061555
update creature_template set npcflag = 16777216, unit_flags = 768 where entry = 75721;

update creature_template set VehicleId = 3548, spell1 = 173021, spell2 = 173000, npcflag = 16777218, minlevel = 100, maxlevel = 100, AIName = 'SmartAI', speed_walk = 2, speed_run = 2 where entry = 75942;

delete from npc_spellclick_spells where npc_entry in (75721,75942);
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES 
(75721, 153074, 1, 0),
(75942, 46598, 1, 0);

delete from conditions where SourceGroup in (75721) and SourceTypeOrReferenceId = 18;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(18, 75721, 153074, 0, 0, 9, 0, 33729, 0, 0, 0, 0, '', 'spellclick if quest incomplete');

delete from creature where id = 75721;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(75721, 1116, 6662, 7132, 1, 1, 0, 0, 0, 3380.84, 2072.26, 157.885, 5.98958, 120, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(75721, 1116, 6662, 7133, 1, 1, 0, 0, 0, 2793.61, 2530.33, 121.142, 5.47058, 300, 0, 0, 2718175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

delete from smart_scripts where entryorguid in (75942);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(75942, 0, 0, 1, 54, 0, 100, 1, 0, 0, 0, 0, 0, 85, 46598, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'crosher - on just summoned - invoker cast'),
(75942, 0, 1, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 33, 75942, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'crosher - on link - give credit'),
(75942, 0, 2, 0, 28, 0, 100, 1, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'crosher - on remove passenger - despawn');

DELETE FROM `creature_queststarter` WHERE quest in (33730);
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(75942, 33730),
(75941, 33730);

DELETE FROM `creature_questender` WHERE quest in (33729);
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(75942, 33729),
(75941, 33729);


-- https://forum.wowcircle.com/showthread.php?t=1061768
update quest_template set RequiredClasses = 8 where id = 40832;


-- https://forum.wowcircle.com/showthread.php?t=1061793
delete from conditions where SourceEntry in (204887) and SourceTypeOrReferenceId = 13;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(13, 3, 204887, 0, 0, 31, 0, 4, 0, 0, 1, 0, '', 'no cast into player'),
(13, 3, 204887, 0, 0, 31, 0, 3, 100059, 0, 1, 0, '', 'no cast self');


-- https://forum.wowcircle.com/showthread.php?t=1061796
update creature_template set AIName = 'SmartAI' where entry in (104618,104661);

delete from smart_scripts where entryorguid in (104618) and id > 0;
delete from smart_scripts where entryorguid in (104661);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(104618, 0, 1, 0, 19, 0, 100, 0, 41704, 0, 0, 0, 0, 85, 207082, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'theryn - on quest accept - invoker cast'),
(104661, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 29, 0, 0, 97140, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'theryn - on just summoned - follow'),
(104661, 0, 1, 0, 60, 0, 100, 1, 0, 0, 1000, 1000, 0, 33, 104618, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'theryn - on update - give credit');

delete from conditions where SourceEntry in (104661) and SourceTypeOrReferenceId = 22 and sourcegroup = 2;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 2, 104661, 0, 0, 29, 1, 97140, 10, 0, 0, 0, '', 'smart if near creature');


-- https://forum.wowcircle.net/showthread.php?t=1061797
DELETE FROM `spell_area` WHERE `spell` in (223622) and quest_start = 41760;
INSERT INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`) VALUES 
(223622, 7840, 41760, 41760, 0, 0, 2, 1, 8, 66);

update creature_template_wdb set KillCredit2 = 0 where KillCredit2 = 112801;

update creature_template_wdb set KillCredit1 = 0 where KillCredit1 = 112801;

delete from smart_scripts where entryorguid in (98555,98979,105885,105456,100014,98555);
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(98555, '', 0, 0, 1, 8, 0, 100, 257, 0, 224552, 0, 30000, 30000, 0, 33, 112801, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'withered - on spellhit - give credit'),
(98555, '', 0, 1, 2, 61, 0, 100, 257, 0, 0, 0, 0, 0, 0, 2, 35, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'withered - on link - set faction'),
(98555, '', 0, 2, 3, 61, 0, 100, 257, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'withered - on link - evade'),
(98555, '', 0, 3, 4, 61, 0, 100, 257, 0, 0, 0, 0, 0, 0, 53, 0, 985550, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'withered - on link - waypoint movement'),
(98555, '', 0, 4, 0, 61, 0, 100, 257, 0, 0, 0, 0, 0, 0, 41, 5000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'withered - on link - despawn'),

(98979, '', 0, 0, 1, 8, 0, 100, 257, 0, 224552, 0, 30000, 30000, 0, 33, 112801, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'withered - on spellhit - give credit'),
(98979, '', 0, 1, 2, 61, 0, 100, 257, 0, 0, 0, 0, 0, 0, 2, 35, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'withered - on link - set faction'),
(98979, '', 0, 2, 3, 61, 0, 100, 257, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'withered - on link - evade'),
(98979, '', 0, 3, 4, 61, 0, 100, 257, 0, 0, 0, 0, 0, 0, 53, 0, 989790, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'withered - on link - waypoint movement'),
(98979, '', 0, 4, 0, 61, 0, 100, 257, 0, 0, 0, 0, 0, 0, 41, 5000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'withered - on link - despawn'),

(105885, '', 0, 0, 1, 8, 0, 100, 257, 0, 224552, 0, 30000, 30000, 0, 33, 112801, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'withered - on spellhit - give credit'),
(105885, '', 0, 1, 2, 61, 0, 100, 257, 0, 0, 0, 0, 0, 0, 2, 35, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'withered - on link - set faction'),
(105885, '', 0, 2, 3, 61, 0, 100, 257, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'withered - on link - evade'),
(105885, '', 0, 3, 4, 61, 0, 100, 257, 0, 0, 0, 0, 0, 0, 53, 0, 1058850, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'withered - on link - waypoint movement'),
(105885, '', 0, 4, 0, 61, 0, 100, 257, 0, 0, 0, 0, 0, 0, 41, 5000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'withered - on link - despawn'),

(105456, '', 0, 0, 1, 8, 0, 100, 257, 0, 224552, 0, 30000, 30000, 0, 33, 112801, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'withered - on spellhit - give credit'),
(105456, '', 0, 1, 2, 61, 0, 100, 257, 0, 0, 0, 0, 0, 0, 2, 35, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'withered - on link - set faction'),
(105456, '', 0, 2, 3, 61, 0, 100, 257, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'withered - on link - evade'),
(105456, '', 0, 3, 4, 61, 0, 100, 257, 0, 0, 0, 0, 0, 0, 53, 0, 1054560, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'withered - on link - waypoint movement'),
(105456, '', 0, 4, 0, 61, 0, 100, 257, 0, 0, 0, 0, 0, 0, 41, 5000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'withered - on link - despawn'),

(100014, '', 0, 0, 1, 8, 0, 100, 257, 0, 224552, 0, 30000, 30000, 0, 33, 112801, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'withered - on spellhit - give credit'),
(100014, '', 0, 1, 2, 61, 0, 100, 257, 0, 0, 0, 0, 0, 0, 2, 35, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'withered - on link - set faction'),
(100014, '', 0, 2, 3, 61, 0, 100, 257, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'withered - on link - evade'),
(100014, '', 0, 3, 4, 61, 0, 100, 257, 0, 0, 0, 0, 0, 0, 53, 0, 1000140, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'withered - on link - waypoint movement'),
(100014, '', 0, 4, 0, 61, 0, 100, 257, 0, 0, 0, 0, 0, 0, 41, 5000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'withered - on link - despawn');


-- https://forum.wowcircle.net/showthread.php?t=1061802
delete from quest_poi_points where QuestID = 42147;
INSERT INTO `quest_poi_points` (`QuestID`, `Idx1`, `Idx2`, `X`, `Y`, `VerifiedBuild`) VALUES 
(42147, 0, 0, 1709, 4628, 21414),
(42147, 1, 0, 1216, 4280, 21414),
(42147, 2, 0, 531, 4250, 21414);


-- https://forum.wowcircle.net/showthread.php?t=1061798
update quest_template set CompletedPhaseID = 0 where id = 41028;

delete from creature where id = 102600;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(102600, 1220, 7637, 8329, 1, 1, 0, 0, 0, 1997.17, 4347.34, 164.454, 0.912242, 120, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(102600, 1220, 7637, 7928, 1, 5, 0, 0, 0, 1707.19, 4629.98, 124.089, 5.13114, 120, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(102600, 1220, 7637, 8018, 1, 1, 0, 0, 0, 2014.79, 4249.68, 166.762, 4.00789, 120, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

delete from phase_definitions where zoneId = 7637 and entry in (400101,40010,41028);

delete from conditions where SourceGroup in (7637) and SourceTypeOrReferenceId = 26 and SourceEntry in (400101);
delete from conditions where SourceGroup in (7637) and SourceTypeOrReferenceId = 26 and sourceentry in (40010);
delete from conditions where SourceGroup in (7637) and SourceTypeOrReferenceId = 26 and sourceentry in (41028);

delete from spell_area where spell = 92286 and area = 8329;
delete from spell_area where spell = 92286 and area = 8018;
delete from spell_area where spell = 92286 and area = 7928;
INSERT INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`) VALUES 
(92286, 8329, 40010, 40010, 0, 0, 2, 1, 8, 66),
(92286, 8018, 40010, 41028, 0, 0, 2, 1, 66, 64),
(92286, 7928, 41028, 41138, 0, 0, 2, 1, 64, 74);

delete from creature_template_addon where entry in (113894,102600);
INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `bytes2`, `auras`) VALUES
(113894, 0, 0, 0, '92286'),
(102600, 0, 0, 0, '92286');

delete from smart_scripts where entryorguid in (102600,10260000);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 

(102600, 0, 0, 1, 10, 0, 100, 0, 1, 30, 0, 0, 0, 33, 113893, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'valtrois - on ooc los - Give Credit'),
(102600, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'valtrois - on Link - talk'),
(102600, 0, 2, 0, 38, 0, 100, 0, 0, 1, 0, 0, 0, 1, 1, 3000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'valtrois - on Update Data - talk'),
(102600, 0, 3, 0, 52, 0, 100, 0, 1, 102600, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'valtrois - on text over - talk'),
(102600, 0, 4, 5, 62, 0, 100, 0, 20453, 0, 0, 0, 0, 12, 102600, 1, 40000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'valtrois - on Gossip Select - summon creature'),
(102600, 0, 5, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 33, 102600, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'valtrois - on link - give credit'),
(102600, 0, 6, 7, 54, 0, 100, 0, 0, 0, 0, 0, 0, 1, 3, 10000, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'valtrois - on Link - talk'),
(102600, 0, 7, 8, 61, 0, 100, 0, 0, 0, 0, 0, 0, 53, 0, 1026000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'valtrois - on link - start waypoint'),
(102600, 0, 8, 9, 61, 0, 100, 0, 0, 0, 0, 0, 0, 83, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'valtrois - on link - remove npcflag'),
(102600, 0, 9, 16, 61, 0, 100, 0, 0, 0, 0, 0, 0, 28, 92286, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'valtrois - on link - remove aura'),
(102600, 0, 10, 0, 52, 0, 100, 0, 3, 102600, 0, 0, 0, 1, 4, 10000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'valtrois - on text over - talk'),
(102600, 0, 11, 0, 52, 0, 100, 0, 4, 102600, 0, 0, 0, 1, 5, 10000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'valtrois - on text over - talk'),
(102600, 0, 12, 0, 52, 0, 100, 0, 5, 102600, 0, 0, 0, 1, 6, 10000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'valtrois - on text over - talk'),
(102600, 0, 13, 0, 40, 0, 100, 0, 6, 102600, 0, 0, 0, 41, 100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'valtrois - on waypoint - despawn'),

(102600, 0, 14, 0, 19, 0, 100, 0, 41138, 0, 0, 0, 0, 12, 102600, 1, 50000, 0, 0, 0, 8, 0, 0, 0, 1707.8199, 4629.9502, 124.1420, 6.22, 'valtrois - on quest accept - summon creature'),

(102600, 0, 15, 16, 54, 0, 100, 1, 0, 0, 0, 0, 0, 80, 10260000, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'valtrois - on just summoned - tun timed action list'),
(102600, 0, 16, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'valtrois - on link - unaura'),

(10260000, 9, 0 , 0, 0, 0, 100, 1, 2000, 2000, 0, 0, 0, 84, 3, 0, 0, 0, 0, 0, 19, 97140, 0, 0, 0, 0, 0, 0, 'timed action list - talk'),
(10260000, 9, 1 , 0, 0, 0, 100, 1, 3000, 3000, 0, 0, 0, 84, 4, 0, 0, 0, 0, 0, 19, 97140, 0, 0, 0, 0, 0, 0, 'timed action list - talk'),
(10260000, 9, 2 , 0, 0, 0, 100, 1, 5000, 5000, 0, 0, 0, 1, 7, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'timed action list - talk'),
(10260000, 9, 3 , 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 53, 0, 1026001, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'timed action list - start waypoint'),
(10260000, 9, 4 , 0, 0, 0, 100, 1, 5000, 5000, 0, 0, 0, 1, 8 , 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'timed action list - talk'),
(10260000, 9, 5 , 0, 0, 0, 100, 1, 5000, 5000, 0, 0, 0, 1, 9 , 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'timed action list - talk'),
(10260000, 9, 6 , 0, 0, 0, 100, 1, 5000, 5000, 0, 0, 0, 1, 10, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'timed action list - talk'),
(10260000, 9, 7 , 0, 0, 0, 100, 1, 5000, 5000, 0, 0, 0, 1, 11, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'timed action list - talk'),
(10260000, 9, 8 , 0, 0, 0, 100, 1, 5000, 5000, 0, 0, 0, 1, 12, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'timed action list - talk'),
(10260000, 9, 9 , 0, 0, 0, 100, 1, 5000, 5000, 0, 0, 0, 1, 13, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'timed action list - talk'),
(10260000, 9, 10, 0, 0, 0, 100, 1, 5000, 5000, 0, 0, 0, 1, 14, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'timed action list - talk'),
(10260000, 9, 11, 0, 0, 0, 100, 1, 4500, 4500, 0, 0, 0, 33, 102600, 0, 0, 0, 0, 0, 18, 30, 0, 0, 0, 0, 0, 0, 'timed action list - give credit');

delete from conditions where SourceEntry in (113894,102600) and SourceTypeOrReferenceId = 22 or SourceGroup in (20453) and SourceTypeOrReferenceId = 15;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 20453, 0, 0, 0, 28, 0, 40010, 0, 0, 1, 0, '', 'smart if quest not complete'),
(15, 20453, 0, 0, 0, 9, 0, 40010, 0, 0, 0, 0, '', 'smart if quest incomplete'),
(15, 20453, 0, 0, 0, 48, 0, 286664, 0, 0, 0, 0, '', 'smart if objective incomplete'),
(15, 20453, 0, 0, 0, 48, 0, 286665, 0, 0, 0, 0, '', 'smart if objective incomplete'),

(22, 1, 102600, 0, 0, 9, 0, 40010, 0, 0, 0, 0, '', 'smart if quest incomplete'),
(22, 1, 102600, 0, 0, 48, 0, 286664, 0, 0, 1, 0, '', 'smart if objective incomplete'),

(22, 7, 102600, 0, 0, 23, 0, 8329, 0, 0, 0, 0, '', 'smart in 8329 area'),
(22, 16, 102600, 0, 0, 23, 0, 7928, 0, 0, 0, 0, '', 'smart in 7928 area'),

(22, 1, 113894, 0, 0, 9, 0, 40010, 0, 0, 0, 0, '', 'smart if quest incomplete'),
(22, 1, 113894, 0, 0, 48, 0, 286665, 0, 0, 1, 0, '', 'smart if objective incomplete');

update gameobject set phaseid = 40010, phaseMask = 0 where id = 259009;

update quest_template set prevquestid = 41028 where id = 41138;

delete from waypoints where entry in (1026000,1026001,102600);
INSERT INTO `waypoints` (`entry`, `pointid`, `position_x`, `position_y`, `position_z`) VALUES 
(1026000, 1, 2000.85, 4345.72, 165.766),
(1026000, 2, 2002.52, 4338.57, 166.218),
(1026000, 3, 2005.19, 4322.49, 165.749),
(1026000, 4, 2010.40, 4286.37, 166.384),
(1026000, 5, 2013.84, 4257.42, 166.388),
(1026000, 6, 2014.79, 4249.69, 166.524),

(1026001, 1, 1711.62, 4631.19, 124.006),
(1026001, 2, 1714.54, 4628.31, 123.946),
(1026001, 3, 1716.52, 4625.52, 123.946),
(1026001, 4, 1722.81, 4628.82, 122.264),
(1026001, 5, 1730.92, 4629.1, 118.937),
(1026001, 6, 1736.76, 4626.07, 113.868),
(1026001, 7, 1741.53, 4621.9, 110.413),
(1026001, 8, 1745.52, 4616.37, 105.948),
(1026001, 9, 1746.2, 4610, 101.01),
(1026001, 10, 1746.79, 4604.22, 97.5538),
(1026001, 11, 1742.15, 4599.83, 97.5464),
(1026001, 12, 1737.79, 4601.56, 96.2824),
(1026001, 13, 1734.1899, 4603.1001, 96.365);

DELETE FROM `spell_area` WHERE `spell` in (52597) and area = 7928;
INSERT INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`) VALUES 
(52597, 7928, 41138, 0, 0, 0, 2, 1, 66, 0);

delete from creature_text where entry in (102600) and groupid > 6;
delete from creature_text where entry in (97140) and groupid > 2;
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `BroadcastTextID`) VALUES 
(97140, 3, 0, 'And Valtrois?', 12, 0, 100, 0, 0, 105901),
(97140, 4, 0, 'You would be wise not to turn your nose up at the outsiders. None of us would be here without their help.', 12, 0, 100, 0, 0, 105902),

(102600, 7 , 0, 'Of course, First Arcanist.', 12, 0, 100, 0, 0, 105903),
(102600, 8 , 0, 'Come, we can examine the feed from down here.', 12, 0, 100, 0, 0, 105904),
(102600, 9 , 0, 'Do you have much experience with leyline energy? I should think not.', 12, 0, 100, 0, 0, 105906),
(102600, 10, 0, 'Allow me to educate you on the fundamentals.', 12, 0, 100, 0, 0, 105907),
(102600, 11, 0, 'Far below us, rivers of raw arcane power course throughout the land. The leylines are eternal, primal, and impossible to control.', 12, 0, 100, 0, 0, 105909),
(102600, 12, 0, 'However, just as one pulls thread from cotton, so too can we draw power from the leylines.', 12, 0, 100, 0, 0, 105911),
(102600, 13, 0, 'See here. This is the feed from Anora. And there are many others still lying dormant.', 12, 0, 100, 0, 0, 105930),
(102600, 14, 0, 'Restore them, and help us create a new locus of power here in Shal\'Aran!', 12, 0, 100, 0, 0, 105931);