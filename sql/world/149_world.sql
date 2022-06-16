-- https://forum.wowcircle.net/showthread.php?t=1061157
update creature_template set AIName = 'SmartAI' where entry = 110564;

delete from smart_scripts where entryorguid in (110564);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(110564, 0, 0, 0, 19, 0, 100, 0, 43402, 30000, 30000, 0, 0, 1, 0, 10000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'alonsus - on quest accept - talk'),
(110564, 0, 1, 0, 52, 0, 100, 0, 0, 110564, 0, 0, 0, 1, 1, 10000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'alonsus - on text over - talk'),
(110564, 0, 2, 0, 52, 0, 100, 0, 1, 110564, 0, 0, 0, 1, 2, 3000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'alonsus - on text over - talk'),
(110564, 0, 3, 4, 52, 0, 100, 0, 2, 110564, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 19, 109776, 0, 0, 0, 0, 0, 0, 'alonsus - on text over - talk'),
(110564, 0, 4, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 33, 110564, 0, 0, 0, 0, 0, 18, 30, 0, 0, 0, 0, 0, 0, 'alonsus - on link - give credit');

delete from creature_text where entry in (110564,109776);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `BroadcastTextID`) VALUES 
(110564, 0, 0, 'Through your guidance and strength, we have emerged the victors in a great battle that was meant to destroy us.', 12, 0, 100, 0, 0, 120464),
(110564, 1, 0, 'It would be foolish to think this attack will be the last, but we should take a moment to reflect on this victory and celebrate its true hero, our High Priest.', 12, 0, 100, 0, 0, 120465),
(110564, 2, 0, 'To the High Priest!', 12, 0, 100, 0, 0, 120466),

(109776, 0, 0, 'To the High Priest!', 12, 0, 100, 0, 0, 120469);


-- https://forum.wowcircle.net/showthread.php?t=1062209
update creature_template set ScriptName = '' where entry = 85414;

update creature_template set faction = 14 where entry in (85447,85739);

delete from conditions where SourceEntry in (36449,34309,35843,35876,36136,34034) and SourceTypeOrReferenceId = 19;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(19, 0, 36449, 0, 0, 40, 0, 2, 0, 0, 0, 0, '', 'this quest needs garrison lvl 2 to be offered'),
(19, 0, 34309, 0, 0, 40, 0, 3, 0, 0, 0, 0, '', 'this quest needs garrison lvl 3 to be offered'),
(19, 0, 35843, 0, 0, 40, 0, 3, 0, 0, 0, 0, '', 'this quest needs garrison lvl 3 to be offered'),
(19, 0, 34034, 0, 0, 40, 0, 3, 0, 0, 0, 0, '', 'this quest needs garrison lvl 3 to be offered'),
(19, 0, 36136, 0, 0, 40, 0, 3, 0, 0, 0, 0, '', 'this quest needs garrison lvl 3 to be offered'),
(19, 0, 35876, 0, 0, 40, 0, 3, 0, 0, 0, 0, '', 'this quest needs garrison lvl 3 to be offered');

delete from creature where id in (85447,85739);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(85447, 1116, 6721, 7313, 1, 1, 0, 0, 0, 4954.85, 1300.65, 121.043, 1.54778, 300, 0, 0, 299132, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(85739, 1116, 6721, 7313, 1, 1, 0, 0, 0, 4952, 1304.6, 120.696, 0.943, 300, 0, 0, 59826, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(85739, 1116, 6721, 7313, 1, 1, 0, 0, 0, 4957.96, 1305.14, 120.824, 2.25069, 300, 0, 0, 59826, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(85739, 1116, 6721, 7313, 1, 1, 0, 0, 0, 4954.88, 1304.32, 120.828, 1.61452, 300, 0, 0, 59826, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

delete from creature_loot_template where entry in (85447);
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`, `requiredAura`, `requiredClass`, `difficultyMask`) VALUES 
(85447, 116153, -100, 1, 1, 1, 1, 0, 0, 0);

delete from creature_template_addon where entry in (85739);
INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `bytes2`, `emote`, `auras`) VALUES
(85739, 0, 0, 0, 234, '');


-- https://forum.wowcircle.net/showthread.php?t=1062338
update garrison_mission_rewards set ItemID = 139634, ItemQuantity = 10 where ID in (1285,1286,1287,1288,1289);


-- https://forum.wowcircle.net/showthread.php?t=1062437
update creature_template set npcflag = 131 where entry = 33977;


-- https://forum.wowcircle.net/showthread.php?t=1062698
update creature_template set gossip_menu_id = 37523, AIName = 'SmartAI' where entry in (37523);

delete from gossip_menu_option where menu_id in (37523);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `OptionBroadcastTextID`, `action_menu_id`) VALUES 
(37523, 0, 0, 'Tell me about your plan.', 1, 1, 37806, 0);

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 15 AND `SourceGroup` in (37523);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 37523, 0, 0, 0, 9, 0, 24564, 0, 0, 0, 0, '', 'Gossip if quest incomplete'),
(15, 37523, 0, 0, 0, 9, 0, 24553, 0, 0, 0, 0, '', 'Gossip if quest incomplete'),
(15, 37523, 0, 0, 0, 9, 0, 24594, 0, 0, 0, 0, '', 'Gossip if quest incomplete'),
(15, 37523, 0, 0, 0, 9, 0, 24595, 0, 0, 0, 0, '', 'Gossip if quest incomplete'),
(15, 37523, 0, 0, 0, 9, 0, 24596, 0, 0, 0, 0, '', 'Gossip if quest incomplete'),
(15, 37523, 0, 0, 0, 9, 0, 24598, 0, 0, 0, 0, '', 'Gossip if quest incomplete');

delete from gossip_menu where entry in (37523);
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES 
(37523, 15240);

delete from npc_text where id in (15240);
INSERT INTO `npc_text` (`ID`, `BroadcastTextID0`, `BroadcastTextID1`) VALUES 
(15240, 37804, 37804);

delete from smart_scripts where entryorguid in (37523);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(37523, 0, 1, 0, 62, 0, 100, 0, 37523, 0, 0, 0, 0, 85, 70746 , 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'guard - on gossip select - invoker cast');

delete from gameobject where id = 201796;
INSERT INTO `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `ScriptName`, `custom_flags`, `VerifiedBuild`) VALUES 
(201796, 580, 4075, 4075, 16, 1, 0, 1698.42, 627.916, 27.5456, 0.482918, 0, 0, 0.239119, 0.97099, -300, 0, 1, 0, '', 0, -1);

delete from event_scripts where id = 22833;
INSERT INTO `event_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `dataint`, `x`, `y`, `z`, `o`) VALUES 
(22833, 0, 17, 49871, 1, 0, 0, 0, 0, 0);

delete from gameobject where id = 201794;


-- https://forum.wowcircle.net/showthread.php?t=1062755
update creature_template set spell1 = 62684, AIName = 'SmartAI' where entry = 34207;

delete from smart_scripts where entryorguid in (34207);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(34207, 0, 0, 0, 6, 0, 100, 0, 0, 0, 0, 0, 0, 62, 1, 0, 0, 0, 0, 0, 23, 0, 0, 0, 7745.6128, -410.1957, 1.0443, 0.6, 'robot - on death - teleport');

delete from spell_linked_spell where spell_trigger in (-62684) and spell_effect in (62682);
delete from spell_linked_spell where spell_trigger in (62682) and spell_effect in (7);
INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `hastalent`, `hastalent2`, `chance`, `cooldown`, `comment`) VALUES 
(-62684, 62682, 0, 0, 0, 0, 0, ''),
(62682, 7, 0, 0, 0, 0, 0, '');

update creature set MovementType = 0, spawndist = 0 where id = 34207;


-- https://forum.wowcircle.net/showthread.php?t=1062760
update quest_template_objective set Flags = 0 where QuestID = 36615;

update quest_template set Flags = 0 where Id = 36615;