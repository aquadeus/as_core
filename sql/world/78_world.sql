-- https://forum.wowcircle.com/showthread.php?t=1054165
update quest_template set method = 2, flags = 0 where id = 40863;

update creature_text set broadcasttextid = 112147 where entry = 93539 and groupid = 0;

update creature_text set broadcasttextid = 43341 where entry = 93526 and groupid = 0;

delete from gossip_menu_option where menu_id in (93526) and id = 2;
delete from gossip_menu_option where menu_id in (96198) and id = 0;
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `OptionBroadcastTextID`) VALUES 
(96198, 0, 0, 'Didi sent me to retrieve a Prismatic Felslate Diamond.', 1, 1, 104793),
(93526, 2, 0, 'Didi sent me to retrieve a Prismatic Felslate Diamond.', 1, 1, 104687);

delete from smart_scripts where entryorguid in (93526) and id in (2,3,4);
delete from smart_scripts where entryorguid in (93520) and id > 1;
delete from smart_scripts where entryorguid in (96198) and id > 1;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(96198, 0, 2, 0, 62, 0, 100, 0, 96198, 0, 0, 0, 0, 33, 102535, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Katrina - on gossip select - give credit'),

(93520, 0, 2, 3, 62, 0, 100, 0, 93520, 10, 0, 0, 0, 1, 0, 6000, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Didi - on gossip select - talk'),
(93520, 0, 3, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 33, 102483, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Didi - on link - give credit'),
(93520, 0, 4, 0, 52, 0, 100, 0, 0, 93520, 0, 0, 0, 1, 1, 6000, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Didi - on text over - talk'),
(93520, 0, 5, 0, 52, 0, 100, 0, 1, 93520, 0, 0, 0, 1, 5, 6000, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Didi - on text over - talk'),

(93520, 0, 6, 7, 62, 0, 100, 0, 93520, 11, 0, 0, 0, 1, 6, 6000, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Didi - on gossip select - talk'),
(93520, 0, 7, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 33, 102484, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Didi - on link - give credit'),
(93520, 0, 8, 0, 52, 0, 100, 0, 6, 93520, 0, 0, 0, 1, 7, 6000, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Didi - on text over - talk'),
(93520, 0, 9, 0, 52, 0, 100, 0, 7, 93520, 0, 0, 0, 1, 8, 6000, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Didi - on text over - talk'),

(93520, 0, 10, 11, 62, 0, 100, 0, 93520, 12, 0, 0, 0, 1, 9, 6000, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Didi - on gossip select - talk'),
(93520, 0, 11, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 33, 102536, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Didi - on link - give credit'),
(93520, 0, 12, 0, 52, 0, 100, 0, 9, 93520, 0, 0, 0, 1, 10, 6000, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Didi - on text over - talk'),

(93526, 0, 2, 3, 62, 0, 100, 0, 93526, 2, 0, 0, 0, 56, 133879, 1, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Tiffany - on gossip select - add item'),
(93526, 0, 3, 4, 61, 0, 100, 0, 0, 0, 0, 0, 0, 1, 9, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Tiffany - on link - talk'),
(93526, 0, 4, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Tiffany - on link - close gossip');


delete from conditions where SourceGroup in (93526) and sourceentry in (2) and SourceTypeOrReferenceId = 15;
delete from conditions where SourceGroup in (93520) and sourceentry in (10,11) and SourceTypeOrReferenceId = 15;
delete from conditions where SourceGroup in (19092) and sourceentry in (0) and SourceTypeOrReferenceId = 15;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 93520, 10, 0, 0, 9, 0, 40863, 0, 0, 0, 0, '', 'Show gossip if 40863 quest incomplete'),
(15, 93520, 10, 0, 0, 2, 0, 133879, 1, 0, 0, 0, '', 'Show gossip if item not missed'),
(15, 93520, 10, 0, 0, 48, 0, 5000008, 0, 0, 1, 0, '', 'Show gossip if criteria not completed'),

(15, 93520, 11, 0, 0, 9, 0, 40863, 0, 0, 0, 0, '', 'Show gossip if 40863 quest incomplete'),
(15, 93520, 11, 0, 0, 2, 0, 133879, 1, 0, 0, 0, '', 'Show gossip if item not missed'),
(15, 93520, 11, 0, 0, 2, 0, 133880, 1, 0, 0, 0, '', 'Show gossip if item not missed'),
(15, 93520, 11, 0, 0, 48, 0, 5000010, 0, 0, 1, 0, '', 'Show gossip if criteria not completed'),

(15, 19092, 0, 0, 0, 48, 0, 5000011, 0, 0, 0, 0, '', 'Show gossip if criteria completed'),

(15, 93526, 2, 0, 0, 9, 0, 40863, 0, 0, 0, 0, '', 'Show gossip if 40863 quest incomplete'),
(15, 93526, 2, 0, 0, 2, 0, 133879, 1, 0, 1, 0, '', 'Show gossip if item missed');


-- https://forum.wowcircle.com/showthread.php?t=1054219
DELETE FROM `spell_scene` WHERE MiscValue in (887);
INSERT INTO `spell_scene` (`SceneScriptPackageID`, `MiscValue`, `PlaybackFlags`) VALUES
(1037, 887, 0);

update creature_template set gossip_menu_id = 17298, npcflag = 3, ainame = 'SmartAI' where entry in (88530);

delete from gossip_menu_option where menu_id in (17298);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `OptionBroadcastTextID`) VALUES 
(17298, 0, 0, 'What\'s the situation?', 1, 1, 85723);

delete from gossip_menu where entry in (17298);
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES 
(17298, 24490);

delete from npc_text where id in (24490);
INSERT INTO `npc_text` (`ID`, `BroadcastTextID0`, `BroadcastTextID1`) VALUES 
(24490, 85722, 85722);

delete from smart_scripts where entryorguid in (88530);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(88530, 0, 0, 0, 62, 0, 100, 0, 17298, 0, 0, 0, 0, 85, 176779, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Choluna - on gossip select - invoker cast');

delete from conditions where SourceGroup in (17298) and sourceentry in (0) and SourceTypeOrReferenceId = 15;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 17298, 0, 0, 0, 9, 0, 36136, 0, 0, 0, 0, '', 'Show gossip if 36136 quest incomplete');

update creature_template_wdb set killcredit1 = 0 where killcredit1 = 84185;

update creature_template_wdb set killcredit2 = 0 where killcredit2 = 84185;

DELETE FROM `creature_queststarter` WHERE quest in (36342);
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(88530, 36342);


-- https://forum.wowcircle.com/showthread.php?t=1054220
update gameobject_template set flags = 0 where entry = 225596;

update gameobject_loot_template set chanceorquestchance = 100 where entry = 225596;


-- https://forum.wowcircle.com/showthread.php?t=1054319
update creature_template set ainame = 'SmartAI', flags_extra = 128, unit_flags = 768 where entry in (25664,25665,25666);

delete from smart_scripts where entryorguid in (25664,25665,25666);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(25664, 0, 0, 0, 8, 0, 100, 0, 45853, 0, 0, 0, 0, 33, 25664, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on spell hit - give credit'),
(25665, 0, 1, 0, 8, 0, 100, 0, 45853, 0, 0, 0, 0, 33, 25665, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on spell hit - give credit'),
(25666, 0, 2, 0, 8, 0, 100, 0, 45853, 0, 0, 0, 0, 33, 25666, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on spell hit - give credit');

update creature set position_z = 0 where id in (25664,25665,25666);


-- https://forum.wowcircle.com/showthread.php?t=1054397
update creature_template set scriptname = '', ainame = 'SmartAI' where entry in (25316,25474);

delete from smart_scripts where entryorguid in (25316,25474);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(25316, 0, 0, 0, 4, 0, 100, 0, 0, 0, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Set Phase 1 on Aggro'),
(25316, 0, 1, 0, 4, 1, 100, 0, 0, 0, 0, 0, 0, 21, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Stop Moving on Aggro'),
(25316, 0, 2, 0, 4, 1, 100, 0, 0, 0, 0, 0, 0, 11, 9672, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Cast bolt on Aggro'),
(25316, 0, 3, 0, 9, 1, 100, 0, 0, 40, 3400, 4700, 0, 11, 9672, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Cast bolt'),
(25316, 0, 4, 0, 9, 1, 100, 0, 40, 100, 0, 0, 0, 21, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Start Moving when not in bolt Range'),
(25316, 0, 5, 0, 9, 1, 100, 0, 10, 15, 0, 0, 0, 21, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Stop Moving at 15 Yards'),
(25316, 0, 6, 0, 9, 1, 100, 0, 0, 40, 0, 0, 0, 21, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Stop Moving when in bolt Range'),
(25316, 0, 7, 0, 3, 1, 100, 0, 0, 15, 0, 0, 0, 22, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Set Phase 2 at 15% Mana'),
(25316, 0, 8, 0, 3, 2, 100, 0, 0, 15, 0, 0, 0, 21, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Start Moving at 15% Mana'),
(25316, 0, 9, 0, 3, 2, 100, 0, 30, 100, 100, 100, 0, 22, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Set Phase 1 When Mana is above 30%'),
(25316, 0, 10, 0, 2, 1, 100, 0, 0, 50, 0, 0, 0, 11, 50648, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Cast Blink at 50% HP'),

(25316, 0, 11, 12, 8, 0, 100, 0, 45611, 0, 0, 0, 0, 85, 45626, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Beryl Mage - on spell hit - invoker cast'),
(25316, 0, 12, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Beryl Mage - on link - despawn'),

(25474, 0, 0, 1, 54, 0, 100, 0, 0, 0, 0, 0, 0, 29, 0, 0, 25262, 25474, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Imprisoned Beryl Mage - on just summoned - follow invoker'),
(25474, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 0, 85, 54324, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Imprisoned Beryl Mage - on link - invoker cast'),
(25474, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 11, 45631, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Imprisoned Beryl Mage - on link - cast');


delete from conditions where SourceEntry in (45611) and SourceTypeOrReferenceId = 17;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(17, 0, 45611, 0, 0, 31, 1, 3, 25316, 0, 0, 0, '', 'Cast spell into 25316 creature'),
(17, 0, 45611, 0, 0, 38, 1, 20, 4, 0, 0, 0, '', 'Cast spell if target <= 20%hp');


-- https://forum.wowcircle.com/showthread.php?t=1054401
delete from creature where id = 80597;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(80597, 1116, 0, 0, 1, 1, 0, 0, 0, 4178.1021, 5235.2446, 65.5300, 6.1, 120, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

DELETE FROM `creature_queststarter` WHERE id in (80597);
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(80597, 35232),
(80597, 34891),
(80597, 34965),
(80597, 35265);


-- https://forum.wowcircle.com/showthread.php?t=1054399
DELETE FROM `creature_queststarter` WHERE id in (83052);
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(83052, 35844);


-- https://forum.wowcircle.com/showthread.php?t=1039976
update creature_template set faction = 14, minlevel = 110, maxlevel = 110, scriptname = 'npc_gorgoloth_125148' where entry = 125148;

delete from spell_linked_spell where spell_trigger in (248508);
INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `hastalent`, `hastalent2`, `chance`, `cooldown`, `comment`) VALUES 
(248508, 248507, 1, 0, 0, 0, 0, '');