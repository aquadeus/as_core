--  https://forum.wowcircle.com/showthread.php?t=1059058
delete from creature where id in (95395,261255);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(95395, 1220, 7558, 7617, 1, 1, 0, 0, 0, 2272.4731, 6244.2134, 123.0513, 5.53, 120, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

delete from smart_scripts where entryorguid in (95395);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(95395, 0, 1, 0, 19, 0, 100, 0, 38381, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'thaon - on quest accept - talk'),
(95395, 0, 0, 0, 10, 0, 100, 0, 1, 10, 0, 0, 0, 15, 38382, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'thaon - on ooc los - give credit');

delete from conditions where SourceEntry in (95395) and SourceTypeOrReferenceId = 22;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 1, 95395, 0, 0, 9, 0, 38382, 0, 0, 0, 0, '', 'Smart if quest incomplete');

update creature_template set AIName = 'SmartAI', gossip_menu_id = 18679 where entry in (95395);

delete from creature_text where entry in (95395);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `BroadcastTextID`, `comment`) VALUES 
(95395, 0, 0, 'Xandris has seen his last moonrise!', 12, 0, 100, 0, 0, 97138, '');


-- https://forum.wowcircle.com/showthread.php?t=1059060
delete from smart_scripts where entryorguid in (91066);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(91066, 0, 0, 0, 10, 0, 100, 0, 1, 10, 0, 0, 0, 15, 38381, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'elothir - on ooc los - give credit');

delete from conditions where SourceEntry in (91066) and SourceTypeOrReferenceId = 22;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 1, 91066, 0, 0, 9, 0, 38381, 0, 0, 0, 0, '', 'Smart if quest incomplete');

update creature_template set AIName = 'SmartAI' where entry in (91066);


-- https://forum.wowcircle.com/showthread.php?t=1059273
delete from npc_spellclick_spells where npc_entry in (36768);
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES 
(36768, 46598, 1, 0);

delete from conditions where SourceGroup in (36768) and SourceTypeOrReferenceId = 18;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(18, 36768, 46598, 0, 0, 9, 0, 14471, 0, 0, 0, 0, '', 'Spellclick if quest incomplete');

update creature set modelid = 0 where id in (6196,6193);

delete from smart_scripts where entryorguid in (6196,6193) and id = 3;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(6196, 0, 3, 0, 8, 0, 100, 0, 69281, 0, 0, 0, 0, 37, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'mob - on spellhit - die'),
(6193, 0, 3, 0, 8, 0, 100, 0, 69281, 0, 0, 0, 0, 37, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'mob - on spellhit - die');

update creature_template set AIName = 'SmartAI' where entry in (6196,6193);


-- https://forum.wowcircle.com/showthread.php?t=973390
update creature_template set faction = 35 where entry = 99544;


-- https://forum.wowcircle.com/showthread.php?t=965087
delete from creature where id = 69675;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(69675, 1064, 6507, 6507, 1, 1, 0, 0, 0, 6054.12, 5881.9, 6.54958, 0.7222, 300, 0, 0, 345060, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(69675, 1064, 6507, 6583, 1, 1, 0, 0, 0, 6086.01, 5259.48, 16.0439, 5.65023, 300, 0, 0, 345060, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);


-- https://forum.wowcircle.com/showthread.php?t=1059608
delete from creature where id = 96683 and modelid = 68482;


-- https://forum.wowcircle.com/showthread.php?t=1058280
update quest_template set PrevQuestId = 41028 where id = 41138;


-- https://forum.wowcircle.com/showthread.php?t=965086
delete from creature where id = 69677;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(69677, 1064, 6507, 6580, 1, 1, 0, 0, 0, 6092.71, 5359.69, 23.079, 0.66216, 300, 0, 0, 345060, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(69677, 1064, 6507, 6507, 1, 1, 0, 0, 0, 6043.89, 5886.82, 6.47483, 1.03966, 300, 0, 0, 345060, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);


-- https://forum.wowcircle.com/showthread.php?t=965082
delete from creature where id = 69678;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(69678, 1064, 6507, 6580, 1, 1, 0, 0, 0, 6093.75, 5348.75, 22.3757, 0.478895, 300, 0, 0, 345060, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);


-- https://forum.wowcircle.com/showthread.php?t=964048
delete from gameobject where id = 268478;
INSERT INTO `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `ScriptName`, `custom_flags`, `VerifiedBuild`) VALUES 
(268478, 1220, 7543, 8571, 1, 1, 0, -2069.32, 2863.9, -52.7696, 2.01965, 0, 0, 0.846739, 0.532008, 300, 0, 1, 0, '', 0, -1);


-- https://forum.wowcircle.com/showthread.php?t=963983
delete from npc_spellclick_spells where npc_entry in (73973);
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES 
(73973, 149007, 1, 0);

delete from conditions where SourceGroup in (73973) and SourceTypeOrReferenceId = 18;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(18, 73973, 149007, 0, 0, 9, 0, 33331, 0, 0, 0, 0, '', 'Spellclick if quest incomplete');

update creature_template set npcflag = 2 where entry in (77270);

update creature_template set npcflag = 16777216, unit_flags = 768 where entry in (73973);

delete from smart_scripts where entryorguid in (73973);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(73973, 0, 0, 1, 73, 0, 100, 1, 0, 0, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'critter - on spellclick - remove aura'),
(73973, 0, 1, 2, 61, 0, 100, 1, 0, 0, 0, 0, 0, 83, 16777216, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'critter - on link - remove npc flag'),
(73973, 0, 2, 3, 61, 0, 100, 1, 0, 0, 0, 0, 0, 89, 10, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'critter - on link - random move'),
(73973, 0, 3, 4, 61, 0, 100, 1, 0, 0, 0, 0, 0, 33, 73973, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'critter - on link - give credit'),
(73973, 0, 4, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 41, 10000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'critter - on link - despawn');