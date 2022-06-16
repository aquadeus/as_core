-- https://forum.wowcircle.net/showthread.php?t=675572
update gameobject_template set data10 = 0, data2 = 0, data5 = 1, data9 = 9822, faction = 125, ainame = 'SmartGameObjectAI', data0 = 43, castbarcaption = 'Установка' where entry = 203413;

delete from smart_scripts where entryorguid in (203413); 
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(203413, 1, 0, 1, 70, 0, 100, 1, 2, 0, 0, 0, 33, 34341, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Bomb - on set state - give credit');


-- https://forum.wowcircle.net/showthread.php?t=695637
delete from vehicle_template_accessory where entryoraura in (42839);
INSERT INTO `vehicle_template_accessory` (`EntryOrAura`, `accessory_entry`, `seat_id`, `minion`, `summontype`, `summontimer`, `description`) VALUES 
(42839, 42494, 0, 0, 8, 0, '');

DELETE FROM `npc_spellclick_spells` WHERE `npc_entry` in (42839);
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES 
(42839, 46598, 1, 0);

update creature_template set flags_extra = 128, ainame = 'SmartAI' where entry = 42929;
update creature_template set dmg_multiplier = 10, faction = 35 where entry in (42852,	42849);
update creature_template set unit_flags = 768, faction = 14 where entry = 42839;
delete from creature where id = 42494;

delete from smart_scripts where entryorguid in (42849,42839, 42929,42852);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(42929, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 19, 42849, 0, 0, 0, 0, 0, 0, 'Trigger - on just summoned - set data'),

(42839, 0, 0, 0, 38, 0, 100, 1, 0, 1, 0, 0, 0, 1, 0, 8000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Razlo - on data set - talk'),
(42839, 0, 1, 0, 52, 0, 100, 1, 0, 42839, 0, 0, 0, 1, 1, 8000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Razlo - on text over - talk'),
(42839, 0, 2, 0, 52, 0, 100, 1, 1, 42839, 0, 0, 0, 1, 2, 8000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Razlo - on text over - talk'),
(42839, 0, 3, 0, 52, 0, 100, 1, 2, 42839, 0, 0, 0, 19, 768, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Razlo - on text over - remove unit_flag'),
(42839, 0, 4, 5, 6, 0, 100, 1, 0, 0, 0, 0, 0, 45, 0, 2, 0, 0, 0, 0, 19, 42849, 0, 0, 0, 0, 0, 0, 'Razlo - on death - set data'),
(42839, 0, 5, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 33, 42839, 0, 0, 0, 0, 0, 18, 50, 0, 0, 0, 0, 0, 0, 'Razlo - on link - give credit'),

(42852, 0, 0, 0, 38, 0, 100, 0, 0, 1, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Set phase 1'),
(42852, 0, 1, 0, 1, 1, 100, 1, 1000, 1000, 10000, 10000, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Talk after 1 sec'),
(42852, 0, 2, 0, 1, 1, 100, 1, 1000, 1000, 10000, 10000, 0, 53, 1, 428520, 0, 0, 0, 2, 1, 0, 0, 0, 0, 0, 0, 0, 'Start waypoint after 1 sec'),
(42852, 0, 3, 5, 40, 0, 100, 0, 5, 0, 0, 0, 0, 22, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Set phase 2 after waypoint reached'),
(42852, 0, 5, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 101, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Set home position'),
(42852, 0, 6, 0, 1, 2, 100, 0, 26000, 26000, 26000, 26000, 0, 49, 0, 0, 0, 0, 0, 0, 11, 42839, 30, 0, 0, 0, 0, 0, 'Attack Razlo'),
(42852, 0, 7, 0, 1, 3, 100, 0, 120000, 120000, 120000, 120000, 0, 41, 1000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Despawn if idle for too long'),

(42849, 0, 0, 19, 62, 0, 100, 0, 11662, 0, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Set phase 1'),
(42849, 0, 1, 0, 1, 1, 100, 1, 1000, 1000, 10000, 10000, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Talk after 1 sec'),
(42849, 0, 2, 0, 1, 1, 100, 1, 15000, 15000, 10000, 10000, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Talk after 15 sec'),
(42849, 0, 3, 0, 1, 1, 100, 1, 25000, 25000, 10000, 10000, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Talk after 25 sec'),
(42849, 0, 4, 18, 1, 1, 100, 1, 32000, 32000, 10000, 10000, 0, 53, 1, 428490, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 'Start waypoint after 32 sec'),
(42849, 0, 5, 0, 40, 0, 100, 1, 6, 0, 0, 0, 0, 22, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Set phase 2 after waypoint reached'),
(42849, 0, 6, 0, 40, 0, 100, 1, 6, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 11, 42839, 30, 0, 0, 0, 0, 0, 'Send data 1 to Crushcog after 1 sec'),
(42849, 0, 7, 0, 0, 2, 100, 0, 8000, 8000, 56000, 66000, 0, 11, 22739, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Cast Goblin Dragon Gun on target'),
(42849, 0, 8, 9, 1, 2, 100, 1, 20000, 20000, 16000, 16000, 0, 2, 57, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Set faction 57 on self'),
(42849, 0, 9, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 101, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Set home position'),
(42849, 0, 10, 0, 1, 2, 100, 0, 26000, 26000, 26000, 26000, 0, 49, 0, 0, 0, 0, 0, 0, 11, 42839, 30, 0, 0, 0, 0, 0, 'Attack Razlo'),
(42849, 0, 11, 0, 62, 0, 100, 0, 11662, 0, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Close Gossip'),
(42849, 0, 12, 0, 38, 0, 100, 0, 0, 1, 0, 0, 0, 11, 80110, 0, 0, 0, 0, 0, 11, 42929, 30, 0, 0, 0, 0, 0, 'Mekkatorque - on data set - cast spell'),
(42849, 0, 13, 16, 38, 0, 100, 0, 0, 2, 0, 0, 0, 22, 4, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data set 2 set phase 4'),
(42849, 0, 14, 0, 60, 4, 100, 1, 1000, 1000, 10000, 10000, 0, 1, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Talk'),
(42849, 0, 15, 0, 60, 4, 100, 1, 9000, 9000, 10000, 10000, 0, 1, 4, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Talk'),
(42849, 0, 16, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 41, 15000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Despawn'),
(42849, 0, 17, 0, 1, 7, 100, 0, 120000, 120000, 120000, 120000, 0, 41, 1000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Despawn if idle for too long'),
(42849, 0, 18, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 11, 42852, 30, 0, 0, 0, 0, 0, 'Send data 1 to Stronegrind after 32 sec'),
(42849, 0, 19, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Mekkatorque - on link - set npcflag');

delete from creature_text where entry in (42839,42849,42852);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextID`, `comment`) VALUES 
(42839, 0, 0, 'You! How did you escape detection by my sentry-bots?', 12, 0, 100, 0, 0, 21243, 42761, 'Razlo talk'),
(42839, 1, 0, 'No matter! My guardians and I will make short work of you. To arms, men!', 12, 0, 100, 0, 0, 21244, 42762, 'Razlo talk'),
(42839, 2, 0, 'You will never defeat the true sons of Gnomeregan!', 12, 0, 100, 0, 0, 21245, 42781, 'Razlo talk'),

(42849, 0, 0, 'Mekgineer Thermaplugg refuses to acknowledge that his defeat is imminent! He has sent Razlo Crushcog to prevent us from rebuilding our beloved Gnomeregan!', 12, 0, 100, 0, 0, 20890, 42758, 'Mekkatorque talk'),
(42849, 1, 0, 'But our brave friend here has thwarted his plans at every turn, and the dwarves of Ironforge stand with us!.', 12, 0, 100, 0, 0, 20891, 42759, 'Mekkatorque talk'),
(42849, 2, 0, 'Lets send him crawling back to his master in defeat!.', 12, 0, 100, 0, 0, 20892, 42760, 'Mekkatorque talk'),
(42849, 3, 0, 'Weve done it! Were victorious!', 12, 0, 100, 0, 0, 20893, 42763, 'Mekkatorque talk'),
(42849, 4, 0, 'Thermaplugg will be quaking in his mechano-tank when he hears of our victory! Well deal with him later. Head back to town and Ill meet you there for the celebration!', 12, 0, 100, 0, 0, 20894, 42765, 'Mekkatorque talk'),

(42852, 0, 0, 'Aye, lets teach this addle-brained gnome a lesson!', 12, 0, 100, 0, 0, 0, 42767, 'Mountainer talk'),
(42852, 1, 0, 'Thatll teach you to mess with the might of Ironforge and Gnomeregan!', 12, 0, 100, 0, 0, 0, 42766, 'Mountainer talk');

update smart_scripts set action_param6 = 1 where entryorguid = 42852 and id = 2;


-- https://forum.wowcircle.net/showthread.php?t=710364
delete from gameobject where id = 194026 and position_z like '341%';
INSERT INTO `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `ScriptName`, `custom_flags`, `VerifiedBuild`) VALUES 
(194026, 571, 3537, 4128, 1, 1, 0, 3739.5, 3567.19, 341.56, 5.72379, 0, 0, 0, 0, 300, 100, 0, 0, '', 0, -1);

delete from areatrigger_scripts where entry = 5339;
insert into areatrigger_scripts (`entry`, `ScriptName`) values
(5339, 'at_last_rites');


-- https://forum.wowcircle.net/showthread.php?t=760840
update creature_loot_template set chanceorquestchance = -100, lootmode = 0, groupid = 0 where item = 140762;
update creature_template set lootid = entry where entry in (select entry from creature_loot_template where item = 140762) and lootid = 0;


-- https://forum.wowcircle.net/showthread.php?t=780022
update creature_template set npcflag = 16777216 where entry = 109114;
update creature_template set npcflag = 0, ainame = 'SmartAI', vehicleid = 4851, unit_flags = 768, hoverheight = 1.5 where entry = 109130;
update creature_template set npcflag = 1, ainame = 'SmartAI', gossip_menu_id = 19885 where entry in (109144);

delete from gossip_menu_option where menu_id in (19885);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `OptionBroadcastTextID`) VALUES 
(19885, 0, 0, 'Thalyssra said you would be able to help me infiltrate the Twilight Vineyards.', 1, 1, 121276);

delete from gossip_menu where entry in (19885);
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES 
(19885, 29551);

delete from npc_text where id in (29551);
INSERT INTO `npc_text` (`ID`, `BroadcastTextID0`, `BroadcastTextID1`) VALUES 
(29551, 114861, 114861);

delete from creature_text where entry in (109144,108872);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextID`) VALUES 
(108872, 0, 0, 'I\'ve procured a palanquin for you. Try to look imperious- it helps with the illusion.', 12, 0, 100, 0, 0, 0, 114973),

(109144, 0, 0, 'I\'ve procured a palanquin for you. Try to look imperious- it helps with the illusion.', 12, 0, 100, 0, 0, 0, 114863),
(109144, 1, 0, 'The woman you are looking for is named Margaux. She has shown interest in our cause.', 12, 0, 100, 0, 0, 0, 114864);

DELETE FROM `npc_spellclick_spells` WHERE `npc_entry` in (109114,109130);
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES 
(109114, 216980, 1, 0);

delete from conditions where SourceGroup in (109114) and SourceTypeOrReferenceId = 18;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(18, 109114, 216980, 0, 0, 9, 0, 42829, 0, 0, 0, 0, '', 'Allow spellclick if player has quest');

delete from creature_template_addon where entry in (109114,109130);
INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `bytes2`, `auras`) VALUES
(109130, 0, 0, 0, '216951'),
(109114, 0, 0, 0, '216951'); 

delete from spell_target_position where id in (216980);
INSERT INTO `spell_target_position` (`id`, `target_map`, `target_position_x`, `target_position_y`, `target_position_z`, `target_orientation`) VALUES 
(216980, 1220, 1509.1899, 3817.7100, 116.1480, 4.76);

update quest_template_objective set `index` = 0 where id = 284837;
update quest_template_objective set `index` = 1 where id = 286209;
update quest_template_objective set `index` = 2 where id = 284873;

delete from smart_scripts where entryorguid in (109130,109144,10913000);
delete from smart_scripts where entryorguid in (108872) and id in (1,2);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(108872, 0, 1, 2, 10, 0, 100, 0, 1, 10, 0, 0, 0, 33, 108872, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Margoux - on ooc los - give credit'),
(108872, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Margoux - on link - talk'),

(109144, 0, 0, 1, 62, 0, 100, 0, 19885, 0, 0, 0, 0, 33, 109144, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Lilet - on gossip select - give credit'),
(109144, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Lilet - on link - talk'),
(109144, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Lilet - on link - close gossip'),
(109144, 0, 3, 0, 38, 0, 100, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Lilet - on set data - talk'),

(109130, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 80, 10913000, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Palayquin - on just summoned - call timed script'),
(109130, 0, 1, 2, 40, 0, 100, 1, 13, 109130, 0, 0, 0, 33, 109114, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Palayquin - on waypoint reached - give credit'),
(109130, 0, 2, 3, 61, 0, 100, 1, 0, 0, 0, 0, 0, 28, 46598, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Palayquin - on link - remove aura'),
(109130, 0, 3, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 41, 5000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Palayquin - on link - despawn'),

(10913000, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 85, 46598, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed script - invoker cast'),
(10913000, 9, 1, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 19, 109144, 0, 0, 0, 0, 0, 0, 'Timed script - set data'),
(10913000, 9, 2, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 0, 53, 1, 109130, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed script - start waypoint movement');

delete from conditions where SourceGroup in (19885) and SourceTypeOrReferenceId = 15;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 19885, 0, 0, 0, 9, 0, 42829, 0, 0, 0, 0, '', 'Show Gossip If Player Has 41094 quest'),
(15, 19885, 0, 0, 0, 48, 0, 284837, 0, 0, 1, 0, '', 'Show Gossip If player not complete criteria');

delete from conditions where SourceEntry in (108872) and SourceTypeOrReferenceId = 22;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 2, 108872, 0, 0, 9, 0, 42829, 0, 0, 0, 0, '', 'Smart Only If player has quest'),
(22, 2, 108872, 0, 0, 48, 0, 284837, 0, 0, 0, 0, '', 'Smart Only If player complete criteria'),
(22, 2, 108872, 0, 0, 48, 0, 286209, 0, 0, 0, 0, '', 'Smart Only If player complete criteria'),
(22, 2, 108872, 0, 0, 48, 0, 284873, 0, 0, 1, 0, '', 'Smart Only If player not complete criteria');

delete from waypoints where entry in (109130); 
INSERT INTO `waypoints` (`entry`, `pointid`, `position_x`, `position_y`, `position_z`) VALUES 
(109130, 7, 1516.33, 3719.04, 114.687),
(109130, 6, 1516.67, 3728.58, 114.687),
(109130, 5, 1516.46, 3748.6, 114.887),
(109130, 4, 1516.04, 3758.22, 113.092),
(109130, 3, 1515.13, 3779.46, 113.39),
(109130, 2, 1514.7, 3789.3, 114.904),
(109130, 1, 1511.59, 3812.93, 114.903),
(109130, 8, 1511.83, 3711.95, 114.687),
(109130, 9, 1504.01, 3704.22, 114.689),
(109130, 10, 1503.08, 3697.55, 114.689),
(109130, 11, 1505.67, 3690.58, 114.688),
(109130, 12, 1510.01, 3688.54, 114.688),
(109130, 13, 1517.02, 3687.91, 114.688);

update quest_template set method = 2 where id = 42829;