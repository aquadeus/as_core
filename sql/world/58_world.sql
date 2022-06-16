-- https://forum.wowcircle.com/showthread.php?t=1042746
update quest_template set method = 2, flags = 130, specialflags = 2 where id = 12720;


-- https://forum.wowcircle.com/showthread.php?t=1043192
update creature_template set vehicleid = 4782, npcflag = 16777216, unit_flags = 768, ainame = 'SmartAI' where entry = 107474;

update creature_template set npcflag = 16777216 where entry = 107473;

update creature_template set ainame = 'SmartAI' where entry = 107318;

delete from npc_spellclick_spells where npc_entry in (107474);
REPLACE INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES
(107474, 46598, 1, 0);

delete from npc_spellclick_spells where npc_entry in (107473);
REPLACE INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES
(107473, 213203, 1, 0);

delete from conditions where SourceEntry in (213203) and SourceTypeOrReferenceId = 18;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(18, 107473, 213203, 0, 0, 9, 0, 40745, 0, 0, 0, 0, '', 'Spell Click - If Player Has Quest');

delete from creature_template_addon where entry in (107318);
INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `bytes2`, `auras`) VALUES
(107318, 0, 0, 0, '212826');

delete from smart_scripts where entryorguid in (107318, 107474, 10731800, 10731801);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(107474, 0, 0, 1, 54, 0, 100, 1, 0, 0, 0, 0, 0, 85, 46598, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Vehicle - On just summoned - invoker cast'),
(107474, 0, 1, 2, 61, 0, 100, 1, 0, 0, 0, 0, 0, 12, 107318, 1, 600000, 0, 0, 0, 8, 0, 0, 0, 529.4758, 4073.3506, 8.1637, 0.7, 'Vehicle - On link - summon creature'),
(107474, 0, 2, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 33, 107474, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Vehicle - On link - give credit'),
(107474, 0, 3, 0, 28, 0, 100, 1, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Vehicle - On passenger remove - despawn'),

(107318, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 53, 0, 1073180, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Guard - on just summoned - start waypoint'),
(107318, 0, 1, 0, 40, 0, 100, 1, 7, 1073180, 0, 0, 0, 80, 10731800, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Guard - on waypoint reached - run timed action list'),
(107318, 0, 2, 0, 40, 0, 100, 1, 13, 1073181, 0, 0, 0, 80, 10731801, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Guard - on waypoint reached - run timed action list'),
(107318, 0, 3, 4, 40, 0, 100, 1, 10, 1073182, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Guard - on waypoint reached - talk'),
(107318, 0, 4, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 33, 107318, 0, 0, 0, 0, 0, 18, 50, 0, 0, 0, 0, 0, 0, 'Guard - on link - give credit'),
(107318, 0, 5, 0, 40, 0, 100, 1, 12, 1073182, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Guard - on waypoint reached - despawn'),
(107318, 0, 6, 0, 10, 0, 100, 0, 1, 7, 60000, 60000, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Guard - on ooc los - talk'),

(10731800, 9, 0, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 5, 376, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed action list - play emote'),
(10731800, 9, 1, 0, 0, 0, 100, 1, 10000, 10000, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed action list - play emote'),
(10731800, 9, 2, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 53, 0, 1073181, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed action list - start waypoint'),

(10731801, 9, 0, 0, 0, 0, 100, 1, 10000, 10000, 0, 0, 0, 53, 0, 1073182, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed action list - start waypoint');

delete from creature_text where entry in (107318);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `BroadcastTextID`) VALUES 
(107318, 0, 0, 'Keep it down, brats. You will join the others soon.', 12, 0, 100, 0, 0, 115406),
(107318, 1, 0, 'Stop following me!', 12, 0, 100, 0, 0, 111601);

update quest_template set flags = 0 where id = 40745;

update gameobject set phaseid = 40745, phasemask = 0 where id = 250991 and map = 1220;

delete from phase_definitions where zoneId = 7637 and entry in (40745);
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `flags`, `comment`) VALUES 
(7637, 40745, 0, 40745, 0, '40745 quest');

delete from conditions where SourceGroup in (7637) and SourceTypeOrReferenceId = 26 and sourceentry in (40745);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 7637, 40745, 0, 1, 14, 0, 40745, 0, 0, 0, 0, '', 'Phase If quest 40745 none'),
(26, 7637, 40745, 0, 0, 9, 0, 40745, 0, 0, 0, 0, '', 'Phase If Player has quest 40745'),
(26, 7637, 40745, 0, 0, 48, 0, 284379, 0, 0, 1, 0, '', 'Phase If Player incomplete objective 284379');

delete from waypoints where entry in (1073180,1073181,1073182);
INSERT INTO `waypoints` (`entry`, `pointid`, `position_x`, `position_y`, `position_z`) VALUES 
(1073180, 7, 598.252, 4114.96, 6.444),
(1073180, 6, 594.266, 4111.34, 6.444),
(1073180, 5, 581.362, 4106.68, 6.39393),
(1073180, 4, 568.686, 4103.15, 6.39393),
(1073180, 3, 548.796, 4096.67, 6.39393),
(1073180, 2, 537.124, 4087.3, 6.39393),
(1073180, 1, 533.608, 4076.58, 6.39393),

(1073181, 7, 595.921, 4129.71, 1.64039),
(1073181, 6, 579.771, 4121.65, 6.11903),
(1073181, 5, 573.224, 4116.52, 6.43783),
(1073181, 4, 573.013, 4110.1, 6.39356),
(1073181, 3, 576.623, 4106.94, 6.39356),
(1073181, 2, 582.26, 4105.97, 6.39356),
(1073181, 1, 589.715, 4109.67, 6.444),
(1073181, 8, 609.392, 4132.33, 1.49344),
(1073181, 9, 627.107, 4134.22, 1.49344),
(1073181, 10, 637.109, 4135.94, 1.49344),
(1073181, 11, 645.199, 4136.94, 1.49344),
(1073181, 12, 652.456, 4133.25, 1.49344),
(1073181, 13, 655.175, 4127.63, 1.49305),

(1073182, 12, 743.993, 4200.8, 3.57811),
(1073182, 11, 747.077, 4196.6, 3.30746),
(1073182, 10, 748.83, 4189.86, 3.66112),
(1073182, 9, 748.556, 4179.33, 1.49369),
(1073182, 8, 743.817, 4169.3, 1.49415),
(1073182, 7, 734.818, 4166.99, 2.11138),
(1073182, 6, 720.202, 4165.12, 7.70087),
(1073182, 5, 714.406, 4164.37, 5.44497),
(1073182, 4, 687.092, 4160.48, 1.49306),
(1073182, 3, 671.728, 4155.01, 1.49306),
(1073182, 2, 660.302, 4142.53, 1.49306),
(1073182, 1, 653.469, 4133.77, 1.49306);

delete from creature where id = 108616;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(108616, 1220, 7637, 8386, 1, 0, 40745, 0, 1, 744.7676, 4196.8262, 3.2525, 5.08, 30, 0, 0, 924436, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

delete from creature_loot_template where item in (138147);
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(108616, 138147, -100, 0, 0, 1, 1);


-- https://forum.wowcircle.com/showthread.php?t=1040877
UPDATE spell_area SET quest_end_status= 64 WHERE spell in (58551,58530);


-- https://forum.wowcircle.com/showthread.php?t=1049690
update quest_template_objective set amount = 20 where objectid = 40334 and questid = 25464;

update creature_template_wdb set killcredit1 = 0 where killcredit1 = 40334;

update creature_template_wdb set killcredit2 = 0 where killcredit2 = 40334;

delete from smart_scripts where entryorguid in (40147) and id = 1;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(40147, 0, 1, 0, 60, 0, 100, 0, 0, 0, 1000, 1000, 0, 33, 40334, 0, 0, 0, 0, 0, 18, 10, 0, 0, 0, 0, 0, 0, 'Geddon - on update - give credit');

delete from conditions where SourceEntry in (40147) and SourceTypeOrReferenceId = 22;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 2, 40147, 0, 0, 1, 1, 75192, 0, 0, 0, 0, '', 'allow smart - if object has aura');