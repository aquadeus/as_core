-- https://forum.wowcircle.com/showthread.php?t=963983
update creature set npcflag = 0 where id = 77270;


-- https://forum.wowcircle.com/showthread.php?t=1059760
delete from creature where id = 91158 and position_x like '4473%' and phaseID = 0;

delete from conditions where SourceGroup in (7541) and SourceTypeOrReferenceId = 26 and sourceentry in (383600);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 7541, 383600, 0, 0, 48, 0, 3836000, 0, 0, 0, 0, '', 'Phase 383600 in zone 7541 if player complete obj 3836000'), 
(26, 7541, 383600, 0, 1, 8, 0, 38360, 0, 0, 0, 0, '', 'Phase 383600 in zone 7541 if player reward quest 38360'), 
(26, 7541, 383600, 0, 1, 14, 0, 38362, 0, 0, 0, 0, '', 'Phase 383600 in zone 7541 if player none quest 38362');


-- https://forum.wowcircle.com/showthread.php?t=1059762
update quest_template set ExclusiveGroup = 38459, NextQuestId = 38331 where id in (38210,38459);


-- https://forum.wowcircle.com/showthread.php?t=1059772
delete from quest_start_scripts where id = 38613;

update quest_template set StartScript = 0 where id = 38613;


-- https://forum.wowcircle.com/showthread.php?t=1059773
update gameobject_template set data3 = 120000 where entry = 251257;


-- https://forum.wowcircle.com/showthread.php?t=1059775
update creature_template_wdb set KillCredit2 = 0 where entry = 91240;


-- https://forum.wowcircle.com/showthread.php?t=1060099
delete from quest_poi_points where QuestID = 40668;
INSERT INTO `quest_poi_points` (`QuestID`, `Idx1`, `Idx2`, `X`, `Y`, `VerifiedBuild`) VALUES 
(40668, 0, 0, 654, 6680, 21414), 
(40668, 1, 0, 3, 5807, 21414), 
(40668, 1, 1, 13, 5811, 21414), 
(40668, 2, 0, -5348, -648, 21414);

delete from quest_poi where QuestID = 40668;
INSERT INTO `quest_poi` (`QuestID`, `BlobIndex`, `Idx1`, `ObjectiveIndex`, `QuestObjectiveID`, `QuestObjectID`, `MapID`, `WorldMapAreaId`, `Floor`, `Priority`, `Flags`, `WorldEffectID`, `PlayerConditionID`, `WoDUnk1`, `AlwaysAllowMergingBlobs`, `VerifiedBuild`) VALUES 
(40668, 0, 0, -1, 0, 0, 1220, 1015, 0, 0, 0, 0, 0, 1194288, 0, 21414), 
(40668, 0, 1, 0, 281034, 132738, 1220, 1015, 0, 0, 2, 0, 0, 0, 0, 21414), 
(40668, 0, 2, 1, 281042, 46381, 1220, 1015, 0, 0, 2, 0, 0, 0, 0, 21414), 
(40668, 0, 3, 32, 0, 0, 0, 27, 0, 0, 0, 0, 0, 1106619, 0, 21414);


-- https://forum.wowcircle.com/showthread.php?t=1060113
update creature_template set spell4 = 117671, spell2 = 117677, spell1 = 117623, spell6 = 117771, npcflag = 16777216 where entry = 60754;

update creature_template set AIName = 'SmartAI' where entry in (60754,60587);

delete from npc_spellclick_spells where npc_entry in (60754);
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES 
(60754, 46598, 1, 0);

update creature_template set gossip_menu_id = 60587 where entry in (60587);

delete from gossip_menu_option where menu_id in (60587);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `OptionBroadcastTextID`) VALUES 
(60587, 0, 0, '<Unscrew the flask and offer it to her.>', 1, 1, 60198);

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 15 AND `SourceGroup` = 60587;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 60587, 0, 0, 0, 9, 0, 30747, 0, 0, 0, 0, '', 'Gossip if quest incomplete');

delete from gossip_menu where entry in (60587);
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES 
(60587, 60587);

delete from npc_text where id in (60587);
INSERT INTO `npc_text` (`ID`, `BroadcastTextID0`, `BroadcastTextID1`) VALUES 
(60587, 60197, 60197);

delete from smart_scripts where entryorguid in (60587,60754);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(60587, 0, 0, 0, 62, 0, 100, 0, 60587, 0, 0, 0, 0, 85, 117621, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'yeti - on gossip select - invoker cast'),
(60754, 0, 0, 0, 54, 0, 100, 0, 0, 0, 0, 0, 0, 85, 46598, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'yeti - on gossip select - invoker cast'),
(60754, 0, 1, 0, 28, 0, 100, 0, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'yeti - on remove passanger - despawn');

delete from conditions where SourceEntry in (117632) and SourceTypeOrReferenceId = 13;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(13, 1, 117632, 0, 0, 31, 0, 3, 60754, 0, 1, 0, '', 'no cast into creature'),
(13, 1, 117632, 0, 0, 31, 0, 4, 0, 0, 1, 0, '', 'no cast into player');

update creature set phaseMask = 0, phaseID = 30747 where id in (60742,60744,60745,60737,60747,60749,60753,60743,60746,60752);

update quest_template set InProgressPhaseId = 30747 where id = 30747;


-- https://forum.wowcircle.com/showthread.php?t=1060144
delete from garrison_mission_rewards where id in (1392,1393,1394,1395);
INSERT INTO `garrison_mission_rewards` (`ID`, `GarrMissionID`, `FollowerXP`, `ItemID`, `ItemQuantity`, `CurrencyType`, `CurrencyQuantity`, `TreasureChance`, `TreasureXP`, `TreasureQuality`, `GarrMssnBonusAbilityID`) VALUES 
(1392, 173, 0, 140561, 1, 824, 250, 0, 0, 0, 0),
(1393, 335, 0, 140496, 1, 824, 250, 0, 0, 0, 0),
(1394, 335, 0, 140562, 1, 824, 250, 0, 0, 0, 0),
(1395, 921, 0, 140563, 1, 824, 250, 0, 0, 0, 0);