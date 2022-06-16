-- https://forum.wowcircle.net/showthread.php?t=830683
-- update creature_template set scriptname = '', ainame = 'SmartAI' where entry = 127943; -- npc_vishaxs_portal_127943

update creature_template set scriptname = 'npc_vishaxs_portal_127943', ainame = '' where entry = 127943;

delete from npc_spellclick_spells where npc_entry in (127943);
REPLACE INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES
(127943, 17648, 1, 0);

delete from smart_scripts where entryorguid in (127943);
-- INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
-- (127943, 0, 0, 1, 8, 0, 100, 0, 253909, 0, 0, 0, 33, 127943, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Portal - on spell hit - give credit'),
-- (127943, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 85, 253910, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Portal - on link - invoker cast');

update quest_template_objective set type = 3 where questid = 49007;


-- https://forum.wowcircle.net/showthread.php?t=881028
delete from quest_template_objective where questid in (40645);
INSERT INTO `quest_template_objective` (`ID`, `QuestID`, `Type`, `Index`, `ObjectID`, `Amount`, `Flags`, `Flags2`, `Description`, `VerifiedBuild`) VALUES 
(281085, 40645, 0, 4, 101246, 1, 2, 0, NULL, 21737),
(281084, 40645, 0, 3, 101215, 1, 2, 0, NULL, 21737),
(281952, 40645, 0, 2, 103246, 1, 2, 0, NULL, 21737),
(281979, 40645, 0, 1, 103489, 1, 2, 0, NULL, 21737),
(281082, 40645, 0, 0, 73451, 1, 0, 0, NULL, 21737);

delete from smart_scripts where entryorguid in (101215);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(101215, 0, 0, 1, 10, 0, 100, 0, 1, 15, 1000, 1000, 0, 33, 101215, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'OEO - KC'),
(101215, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 85, 205654, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'L - ICS');

delete from conditions where SourceEntry in (101215) and SourceTypeOrReferenceId = 22;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 1, 101215, 0, 0, 9, 0, 40645, 0, 0, 0, 0, '', 'Smart Only If player has quest'),
(22, 1, 101215, 0, 0, 48, 0, 281084, 0, 0, 1, 0, '', 'Smart Only If player not complete criteria');

delete from creature where id = 101215;
INSERT IGNORE INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`) VALUES
(101215, 1220, 0, 0, 1, 1, 0, 0, 0, 3783.35, 7144.94, 22.5074, 3.11788, 300, 0, 0, 0, 0, 0, 0, 0, 0);

delete from smart_scripts where entryorguid in (993569,993552);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(993552, 0, 0, 0, 10, 0, 100, 0, 1, 3, 1000, 1000, 0, 62, 1220, 0, 0, 0, 0, 0, 7, 0, 0, 0, 3783.35, 7144.94, 22.5074, 0.43, 'Trigger - ooc ooc los - teleport invoker'),
(993552, 0, 1, 0, 10, 0, 100, 0, 1, 3, 1000, 1000, 0, 62, 1220, 0, 0, 0, 0, 0, 7, 0, 0, 0, 4126.99, 7310.49, 22.1021, 2.75236, 'Trigger - ooc ooc los - teleport invoker'),

(993569, 0, 1, 0, 25, 0, 100, 0, 0, 0, 0, 0, 0, 11, 199298, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Trigger - ooc spawn - cast spell'),
(993569, 0, 0, 0, 10, 0, 100, 0, 1, 3, 1000, 1000, 0, 85, 200083, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on link - invoker cast spell');

delete from conditions where SourceEntry in (993552) and SourceTypeOrReferenceId = 22;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 1, 993552, 0, 0, 9, 0, 40645, 0, 0, 0, 0, '', 'Smart Only If player has quest'),
(22, 2, 993552, 0, 0, 14, 0, 40645, 0, 0, 0, 0, '', 'Smart Only If player none quest'),
(22, 2, 993552, 0, 1, 8, 0, 40645, 0, 0, 0, 0, '', 'Smart Only If player reward quest');


delete from conditions where SourceEntry in (993569) and SourceTypeOrReferenceId = 22;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 1, 993569, 0, 0, 9, 0, 40645, 0, 0, 0, 0, '', 'Smart if player has quest');

delete from smart_scripts where entryorguid in (73451) and source_type = 0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(73451, 0, 0, 1, 10, 0, 100, 0, 1, 20, 0, 0, 0, 33, 73451, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'trigger - ooc los - give credit'),
(73451, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 85, 204836, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'trigger - on link - invoker cast spell');

delete from conditions where SourceEntry in (73451) and SourceTypeOrReferenceId = 22;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 1, 73451, 0, 0, 9, 0, 40645, 0, 0, 0, 0, '', 'Smart Only If player has quest'),
(22, 1, 73451, 0, 0, 48, 0, 281082, 0, 0, 1, 0, '', 'Smart Only If player not complete criteria');

delete from spell_target_position where id in (204836,199549,200083);
INSERT INTO `spell_target_position` (`id`, `target_map`, `target_position_x`, `target_position_y`, `target_position_z`, `target_orientation`) VALUES 
(200083, 1540, 1514.2, 1645.61, 29.2348, 5.55518),
(199549, 1220, 3767.67, 7137.56, 23.31, 0.4673992),
(204836, 1540, 1510.89, 1644.28, 29.8777, 5.55);

update creature_template set ainame = 'SmartAI' where entry in (103489,103246,103488,73451,103833,101195,101215);
update creature_template set speed_run = 1.5 where entry in (103489,103488);
update creature_template_wdb set displayid1 = 20570, displayid2 = 15880 where entry in (993569);

delete from smart_scripts where entryorguid in (103489,103246,103488,10348900,103833,101195);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(101195, 0, 0, 0, 10, 0, 100, 0, 1, 15, 1000, 1000, 0, 33, 101246, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Rensar - on ooc los - give credit'),

(103489, 0, 0, 1, 54, 0, 100, 1, 0, 0, 0, 0, 0, 1, 0, 8000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Remulos - on just summoned - talk'),
(103489, 0, 1, 2, 52, 0, 100, 1, 0, 103489, 0, 0, 0, 1, 1, 8000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Remulos - on text over - talk'),
(103489, 0, 2, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 53, 1, 103489, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Remulos - on link - start wp'),
(103489, 0, 3, 4, 40, 0, 100, 1, 10, 103489, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Remulos - on waypoint reached - talk'),
(103489, 0, 4, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 33, 103489, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Remulos - on link - give credit'),
(103489, 0, 5, 0, 40, 0, 100, 1, 10, 103489, 0, 0, 0, 80, 10348900, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Remulos - on link - run timed script'),

(10348900, 9, 0, 0, 40, 0, 100, 1, 0, 0, 0, 0, 0, 11, 204842, 0, 0, 0, 0, 0, 19, 59115, 0, 0, 0, 0, 0, 0, 'Timed Script - Cast Spell'),
(10348900, 9, 1, 0, 40, 0, 100, 1, 5000, 5000, 0, 0, 0, 11, 204842, 0, 0, 0, 0, 0, 19, 59115, 0, 0, 0, 0, 0, 0, 'Timed Script - Cast Spell'),
(10348900, 9, 2, 0, 40, 0, 100, 1, 5000, 5000, 0, 0, 0, 11, 204842, 0, 0, 0, 0, 0, 19, 59115, 0, 0, 0, 0, 0, 0, 'Timed Script - Cast Spell'),
(10348900, 9, 3, 0, 40, 0, 100, 1, 5000, 5000, 0, 0, 0, 11, 204842, 0, 0, 0, 0, 0, 19, 59115, 0, 0, 0, 0, 0, 0, 'Timed Script - Cast Spell'),
(10348900, 9, 4, 0, 61, 0, 100, 1, 5000, 5000, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed Script - Despawn'),

(103833, 0, 0, 1, 54, 0, 100, 1, 0, 0, 0, 0, 0, 1, 0, 6000, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Remulos - on just summoned - talk'),
(103833, 0, 1, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 53, 1, 103833, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Remulos - on link - start waypoint'),
(103833, 0, 2, 0, 52, 0, 100, 1, 0, 103833, 0, 0, 0, 1, 1, 9000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Remulos - on text over - talk'),
(103833, 0, 3, 0, 52, 0, 100, 1, 1, 103833, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Remulos - on text over - talk'),

(103246, 0, 0, 0, 0, 0, 100, 1, 0, 3000, 3000, 8000, 0, 11, 3391, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Nightmare Blight - ic - cast spell'),
(103246, 0, 1, 0, 0, 0, 100, 1, 12000, 12000, 15000, 15000, 0, 11, 208292, 1, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Nightmare Blight - ic - cast spell'),
(103246, 0, 2, 0, 6, 0, 100, 1, 0, 0, 0, 0, 0, 12, 103488, 1, 60000, 0, 0, 0, 8, 0, 0, 0, 1723.559, 1339.043, 2.442425, 4.830286, 'Nightmare Blight - on death - summon creature'),

(103488, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 1, 0, 8000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Remulos - on just summoned - talk'),
(103488, 0, 1, 2, 52, 0, 100, 1, 0, 103488, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Remulos - on text over - talk'),
(103488, 0, 2, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 53, 1, 103488, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Remulos - on link - start wp'),
(103488, 0, 3, 0, 40, 0, 100, 1, 10, 103488, 0, 0, 0, 41, 3000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Remulos - on wp reached - despawn');

delete from conditions where SourceEntry in (101195) and SourceTypeOrReferenceId = 22;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 1, 101195, 0, 0, 9, 0, 40645, 0, 0, 0, 0, '', 'Smart Only If player has quest');

delete from waypoints where entry in (103489,103488,103833);
INSERT INTO `waypoints` (`entry`, `pointid`, `position_x`, `position_y`, `position_z`) VALUES 
(103833, 1, 3798.71, 7153.76, 20.5147),
(103833, 2, 3811.56, 7165.95, 17.7378),
(103833, 3, 3826.46, 7175.57, 15.7549),
(103833, 4, 3846.78, 7177, 15.5364),
(103833, 5, 3868.35, 7181.1, 18.2036),
(103833, 6, 3885.79, 7187.5, 21.2834),
(103833, 7, 3900.94, 7197.34, 23.4281),
(103833, 8, 3910.41, 7211.76, 24.6967),

(103488, 1, 1732, 1352.04, 2.08415),
(103488, 2, 1735.73, 1399.62, 4.95945),
(103488, 3, 1733.48, 1415.93, 7.79513),
(103488, 4, 1738.37, 1474.43, 10.1859),
(103488, 5, 1745.79, 1492.37, 8.47462),
(103488, 6, 1766.93, 1507.87, 7.29955),
(103488, 7, 1776.35, 1504.83, 8.76202),

(103489, 1, 1523.13, 1632.87, 27.7251),
(103489, 2, 1534.8, 1624.27, 25.6444),
(103489, 3, 1553.6, 1623.24, 21.1649),
(103489, 4, 1571.07, 1622.44, 17.8588),
(103489, 5, 1592.33, 1607.29, 12.5999),
(103489, 6, 1617.17, 1583.6, 6.40238),
(103489, 7, 1639.04, 1573.09, 5.80019),
(103489, 8, 1670.35, 1562.1, 5.30901),
(103489, 9, 1696.73, 1548.58, 2.70116),
(103489, 10, 1703.92, 1544.03, 2.5893);

delete from creature_text where entry in (103489,103488,103833);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextID`) VALUES 
(103488, 0, 0, 'Excellent work! Even now the foul creep of the Nightmare has begun to fade.', 12, 0, 100, 0, 0, 0, 106096),
(103488, 1, 0, 'The way is clear. Come, let us continue to our destination.', 12, 0, 100, 0, 0, 0, 106097),

(103833, 2, 0, 'Идем. Мы должны разыскать друидов Валь\шары и попросить у них помощи.', 12, 0, 100, 0, 0, 6515, 106099),
(103833, 0, 0, 'Ну вот, мы на месте, $n. Ах! Я запомнил эти леса именно такими...', 12, 0, 100, 0, 0, 0, 103576),
(103833, 1, 0, 'Роща Снов защищена древней магией. Здесь все братья и сестры во природе найдут тихое пристанище.', 12, 0, 100, 0, 0, 0, 103577),

(103489, 1, 0, 'Не может быть... Кошмар? Даже в этой части Изумрудного Сна? Но как такое возможно?', 12, 0, 100, 0, 0, 0, 106094),
(103489, 0, 0, 'Приветствую тебя на Тропе Снов, $n. Здесь дорогу, которая заняла бы на Азероте несколько дней, можно пройти за пару шагов.', 12, 0, 100, 0, 0, 0, 106092),
(103489, 2, 0, 'Я уберу эти кошмарные лозы с нашего пути, но источник скверны, породивший их, находится где-то в другом месте. $n, отыщи этот источник и уничтожь его!', 12, 0, 100, 0, 0, 0, 106095);

delete from phase_definitions where zoneId in (7979) and entry in (1, 2);
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `comment`) VALUES 
(7979, 2, 0, 5953, '40645 quest'),
(7979, 1, 0, 6122, '40645 quest');

delete from conditions where SourceGroup in (7979) and SourceTypeOrReferenceId = 26;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 7979, 1, 0, 0, 9, 0, 40645, 0, 0, 0, 0, '', 'Activate phase - if player has quest'),

(26, 7979, 2, 0, 0, 9, 0, 40645, 0, 0, 0, 0, '', 'Activate phase - if player has quest'),
(26, 7979, 2, 0, 0, 48, 0, 281952, 0, 0, 1, 0, '', 'Activate phase - if player not complete criteria');

delete from gameobject where map = 1540 and phaseid = 6122;
delete from gameobject where map = 1540 and phaseid = 5953;
delete from creature where map = 1540 and phaseid = 6122;

INSERT IGNORE INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`) VALUES
(73451, 1540, 0, 0, 3, 0, 6122, 0, 0, 1520.65, 1634.2, 28.1979, 0, 300, 0, 0, 0, 0, 0, 0, 0, 0),
(101091, 1540, 0, 0, 3, 0, 6122, 0, 0, 1674.074, 1488.626, 4.240042, 1.533646, 300, 12, 0, 0, 0, 2, 0, 0, 0),
(101091, 1540, 0, 0, 3, 0, 6122, 0, 0, 1673.704, 1484.461, 4.086721, 1.517911, 300, 21, 0, 0, 0, 2, 0, 0, 0),
(103240, 1540, 0, 0, 3, 0, 6122, 0, 0, 1671.918, 1519.681, 4.657891, 4.964817, 300, 3, 0, 0, 0, 1, 0, 0, 0),
(101091, 1540, 0, 0, 3, 0, 6122, 0, 0, 1677.168, 1506.005, 5.185967, 1.412847, 300, 10, 0, 0, 0, 1, 0, 0, 0),
(103240, 1540, 0, 0, 3, 0, 6122, 0, 0, 1682.446, 1454.8, 3.375653, 0.9345092, 300, 4, 0, 0, 0, 1, 0, 0, 0),
(101091, 1540, 0, 0, 3, 0, 6122, 0, 0, 1690.405, 1419.083, 2.530956, 2.714179, 300, 2, 0, 0, 0, 1, 0, 0, 0),
(101091, 1540, 0, 0, 3, 0, 6122, 0, 0, 1691.438, 1395.901, 3.102511, 0.1039269, 300, 29, 0, 0, 0, 2, 0, 0, 0),
(101091, 1540, 0, 0, 3, 0, 6122, 0, 0, 1693.953, 1417.631, 2.297129, 0.3575085, 300, 13, 0, 0, 0, 2, 0, 0, 0),
(103240, 1540, 0, 0, 3, 0, 6122, 0, 0, 1657.202, 1385.443, 3.236901, 3.155108, 300, 0, 0, 0, 0, 0, 0, 0, 0),
(103240, 1540, 0, 0, 3, 0, 6122, 0, 0, 1672.029, 1521.667, 4.481744, 0.5831751, 300, 10, 0, 0, 0, 1, 0, 0, 0),
(101091, 1540, 0, 0, 3, 0, 6122, 0, 0, 1673.073, 1472.061, 4.283987, 4.583772, 300, 22, 0, 0, 0, 2, 0, 0, 0),
(101091, 1540, 0, 0, 3, 0, 6122, 0, 0, 1672.481, 1500.548, 5.21856, 3.648976, 300, 4, 0, 0, 0, 1, 0, 0, 0),
(101091, 1540, 0, 0, 3, 0, 6122, 0, 0, 1674.721, 1497.256, 4.793997, 3.648976, 300, 4, 0, 0, 0, 1, 0, 0, 0),
(103240, 1540, 0, 0, 3, 0, 6122, 0, 0, 1681.079, 1447.577, 3.375653, 4.824541, 300, 4, 0, 0, 0, 1, 0, 0, 0),
(101091, 1540, 0, 0, 3, 0, 6122, 0, 0, 1687.519, 1430.712, 3.047129, 1.876335, 300, 10, 0, 0, 0, 1, 0, 0, 0),
(101091, 1540, 0, 0, 3, 0, 6122, 0, 0, 1691.158, 1415.87, 2.4192, 1.635225, 300, 13, 0, 0, 0, 2, 0, 0, 0),
(101091, 1540, 0, 0, 3, 0, 6122, 0, 0, 1694.71, 1412.706, 2.297129, 1.917608, 300, 19, 0, 0, 0, 2, 0, 0, 0),
(103240, 1540, 0, 0, 3, 0, 6122, 0, 0, 1658.161, 1387.638, 3.552178, 2.337979, 300, 4, 0, 0, 0, 1, 0, 0, 0),
(103742, 1540, 0, 0, 3, 0, 6122, 0, 0, 1699.705, 1372.866, 2.481599, 3.958387, 300, 6, 0, 0, 0, 1, 0, 0, 0),
(101091, 1540, 0, 0, 3, 0, 6122, 0, 0, 1659.257, 1355.911, 2.222589, 3.37425, 300, 6, 0, 0, 0, 1, 0, 0, 0),
(101091, 1540, 0, 0, 3, 0, 6122, 0, 0, 1661.797, 1356.313, 2.512531, 2.498287, 300, 10, 0, 0, 0, 1, 0, 0, 0),
(101091, 1540, 0, 0, 3, 0, 6122, 0, 0, 1662.193, 1356.671, 2.496149, 1.105838, 300, 3, 0, 0, 0, 1, 0, 0, 0),
(101091, 1540, 0, 0, 3, 0, 6122, 0, 0, 1761.475, 1362.436, 2.424127, 0.9274117, 300, 2, 0, 0, 0, 1, 0, 0, 2),
(101091, 1540, 0, 0, 3, 0, 6122, 0, 0, 1760.817, 1359.822, 2.424127, 2.591755, 300, 1, 0, 0, 0, 1, 0, 0, 2),
(103742, 1540, 0, 0, 3, 0, 6122, 0, 0, 1735.954, 1354.907, 3.049127, 2.315493, 300, 10, 0, 0, 0, 1, 0, 0, 0),
(101091, 1540, 0, 0, 3, 0, 6122, 0, 0, 1761.982, 1359.332, 2.491754, 0.3063908, 300, 5, 0, 0, 0, 1, 0, 0, 2),
(103742, 1540, 0, 0, 3, 0, 6122, 0, 0, 1691.25, 1344.048, 3.509533, 5.658478, 300, 3, 0, 0, 0, 1, 0, 0, 0),
(101091, 1540, 0, 0, 3, 0, 6122, 0, 0, 1714.195, 1326.497, 2.508938, 3.057703, 300, 1, 0, 0, 0, 1, 0, 0, 0),
(101091, 1540, 0, 0, 3, 0, 6122, 0, 0, 1710.443, 1325.348, 2.199612, 0.5018046, 300, 7, 0, 0, 0, 1, 0, 0, 0),
(103246, 1540, 0, 0, 3, 0, 6122, 0, 0, 1750.29, 1327.941, 2.531792, 2.460431, 300, 0, 0, 0, 0, 0, 0, 0, 0),
(103240, 1540, 0, 0, 3, 0, 6122, 0, 0, 1660.307, 1327.541, 2.170362, 3.876683, 300, 1, 0, 0, 0, 1, 0, 0, 0),
(101091, 1540, 0, 0, 3, 0, 6122, 0, 0, 1712.599, 1304.146, 2.434475, 4.50431, 300, 8, 0, 0, 0, 1, 0, 0, 0),
(103240, 1540, 0, 0, 3, 0, 6122, 0, 0, 1659.092, 1319.052, 2.153863, 2.839822, 300, 1, 0, 0, 0, 1, 0, 0, 0),
(103742, 1540, 0, 0, 3, 0, 6122, 0, 0, 1676.985, 1295.946, 2.730002, 3.8331, 300, 17, 0, 0, 0, 2, 0, 0, 0),
(101091, 1540, 0, 0, 3, 0, 6122, 0, 0, 1708.839, 1271.936, 2.646857, 1.062559, 300, 2, 0, 0, 0, 1, 0, 0, 0),
(101091, 1540, 0, 0, 3, 0, 6122, 0, 0, 1712.235, 1272.051, 2.563549, 1.029319, 300, 11, 0, 0, 0, 2, 0, 0, 0),
(101091, 1540, 0, 0, 3, 0, 6122, 0, 0, 1706.437, 1267.265, 3.272167, 3.590697, 300, 2, 0, 0, 0, 1, 0, 0, 0),
(103742, 1540, 0, 0, 3, 0, 6122, 0, 0, 1810.842, 1322.894, 31.88512, 5.290317, 300, 3, 0, 0, 0, 1, 0, 0, 0),
(59115, 1540, 0, 0, 3, 0, 6122, 0, 0, 1711.91, 1541.17, 2.886, 5.290317, 300, 3, 0, 0, 0, 1, 0, 0, 0); 

delete from creature_template_addon where entry in (101091,103240,103246);
INSERT INTO `creature_template_addon` (`entry`, `mount`, `emote`, `bytes2`, `auras`) VALUES
(101091, 0, 0, 0, '208288'),
(103240, 0, 0, 0, '208289 208290'),
(103246, 0, 0, 0, '195110');

update creature set spawndist = 3, movementtype = 1 where id in (103240,103742);
update creature set spawndist = 2, movementtype = 1 where id in (101091);

INSERT IGNORE INTO `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES
(250363, 1540, 0, 0, 3, 0, 5953, 1718.359, 1542.521, 3.373571, 2.043474, 0, 0, 0, 1, 300, 255, 1),
(247069, 1540, 0, 0, 3, 0, 5953, 1552.76, 1550.307, 18.10002, 0.1281971, 0, 0, 0, 1, 300, 255, 1),
(247069, 1540, 0, 0, 3, 0, 5953, 1703.991, 1669.549, 10.03862, 4.366319, 0, 0, 0, 1, 300, 255, 1),
(250363, 1540, 0, 0, 3, 0, 5953, 1711.42, 1534.743, 0.813005, 2.498142, 0, 0, 0, 1, 300, 255, 1),
(247069, 1540, 0, 0, 3, 0, 5953, 1635.955, 1446.663, 9.280378, 1.229847, 0, 0, 0, 1, 300, 255, 1),
(250363, 1540, 0, 0, 3, 0, 5953, 1721.79, 1538.172, -1.60725, 4.154918, 0, 0, 0, 1, 300, 255, 1),
(247069, 1540, 0, 0, 3, 0, 5953, 1759.052, 1605.016, 3.939034, 3.618761, 0, 0, 0, 1, 300, 255, 1),
(247069, 1540, 0, 0, 3, 0, 5953, 1570.585, 1466.823, 9.585012, 3.779797, 0, 0, 0, 1, 300, 255, 1),
(250363, 1540, 0, 0, 3, 0, 5953, 1715.052, 1529.384, -2.115633, 3.928291, 0, 0, 0, 1, 300, 255, 1),
(247070, 1540, 0, 0, 3, 0, 5953, 1544.977, 1547.158, 21.78308, 0.305662, 0, 0, 0, 1, 300, 255, 1),
(247070, 1540, 0, 0, 3, 0, 5953, 1758.524, 1602.842, 4.898145, 3.704865, 0, 0, 0, 1, 300, 255, 1),
(247070, 1540, 0, 0, 3, 0, 5953, 1634.806, 1448.594, 9.139194, 1.335991, 0, 0, 0, 1, 300, 255, 1),
(247070, 1540, 0, 0, 3, 0, 5953, 1564.726, 1464.748, 12.05964, 1.907379, 0, 0, 0, 1, 300, 255, 1),
(247070, 1540, 0, 0, 3, 0, 5953, 1705.082, 1667.59, 9.798828, 4.452422, 0, 0, 0, 1, 300, 255, 1),
(247069, 1540, 0, 0, 3, 0, 5953, 1635.955, 1446.663, 9.280378, 1.229847, 0, 0, 0, 1, 300, 255, 1),
(247069, 1540, 0, 0, 3, 0, 5953, 1703.991, 1669.549, 10.03862, 4.366319, 0, 0, 0, 1, 300, 255, 1),
(247069, 1540, 0, 0, 3, 0, 5953, 1570.585, 1466.823, 9.585012, 3.779797, 0, 0, 0, 1, 300, 255, 1),
(247069, 1540, 0, 0, 3, 0, 5953, 1552.76, 1550.307, 18.10002, 0.1281971, 0, 0, 0, 1, 300, 255, 1),
(247070, 1540, 0, 0, 3, 0, 5953, 1758.524, 1602.842, 4.898145, 3.704865, 0, 0, 0, 1, 300, 255, 1),
(247069, 1540, 0, 0, 3, 0, 5953, 1759.052, 1605.016, 3.939034, 3.618761, 0, 0, 0, 1, 300, 255, 1),
(247070, 1540, 0, 0, 3, 0, 5953, 1544.977, 1547.158, 21.78308, 0.305662, 0, 0, 0, 1, 300, 255, 1),
(247070, 1540, 0, 0, 3, 0, 5953, 1564.726, 1464.748, 12.05964, 1.907379, 0, 0, 0, 1, 300, 255, 1),
(247070, 1540, 0, 0, 3, 0, 5953, 1634.806, 1448.594, 9.139194, 1.335991, 0, 0, 0, 1, 300, 255, 1);

delete from creature where id = 103246 and phaseid = 0;


-- https://forum.wowcircle.net/showthread.php?t=1031285
select guid from gameobject where id = 184833 into @go1;
select guid from gameobject where id = 184834 into @go2;
select guid from gameobject where id = 184835 into @go3;

delete from smart_scripts where entryorguid in (4548, 184833,184834,184835,21633,2163300,21502);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(21633, 0, 0, 1, 11, 0, 100, 0, 0, 0, 0, 0, 0, 0, 11, 34427, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Deathbringer Jovaan - Just summoned - Spellcast Etheral Teleport'),
(21633, 0, 1, 4, 61, 0, 100, 0, 0, 0, 0, 0, 0, 0, 53, 0, 21633, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Deathbringer Jovaan - Just summoned - Start WP movement'),
(21633, 0, 2, 3, 40, 0, 100, 0, 0, 4, 21633, 0, 0, 0, 54, 45000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Deathbringer Jovaan - On WP 4 reached - Pause 45 seconds'),
(21633, 0, 3, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 0, 80, 2163300, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Deathbringer Jovaan - On WP 4 reached - Run Script'),
(21633, 0, 4, 5, 46, 0, 100, 0, 0, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 14, @go2, 184834, 0, 0, 0, 0, 0, 'Deathbringer Jovaan - On Link - Set Data GO'),
(21633, 0, 5, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 14, @go3, 184835, 0, 0, 0, 0, 0, 'Deathbringer Jovaan - On Link - Set Data GO'),

(2163300, 9, 0, 0, 0, 0, 100, 0, 0, 500, 500, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 14, 25736, 184833, 0, 0, 0, 0, 0, 'Deathbringer Jovaan script - Turn to Legion Communication Device'),
(2163300, 9, 1, 0, 0, 0, 100, 0, 0, 2500, 2500, 0, 0, 0, 5, 25, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Deathbringer Jovaan script - Play emote'),
(2163300, 9, 2, 0, 0, 0, 100, 0, 0, 2000, 2000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Deathbringer Jovaan script - Say line'),
(2163300, 9, 3, 0, 0, 0, 100, 0, 0, 100, 100, 0, 0, 0, 90, 8, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Deathbringer Jovaan script - Set unit_field_bytes1 (kneel)'),
(2163300, 9, 4, 0, 0, 0, 100, 0, 0, 100, 100, 0, 0, 0, 12, 21502, 1, 37500, 0, 0, 0, 8, 0, 0, 0, -3300.09, 2927.05, 173.335, 2.19912, 'Deathbringer Jovaan script - Summon Image of Warbringer Razuun'),
(2163300, 9, 5, 0, 0, 0, 100, 0, 0, 800, 800, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 19, 21502, 10, 0, 0, 0, 0, 0, 'Deathbringer Jovaan script - Set data 0 1 Image of Warbringer Razuun'),
(2163300, 9, 6, 0, 0, 0, 100, 0, 0, 5000, 5000, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Deathbringer Jovaan script - Say line'),
(2163300, 9, 7, 0, 0, 0, 100, 0, 0, 5000, 5000, 0, 0, 0, 45, 0, 2, 0, 0, 0, 0, 19, 21502, 10, 0, 0, 0, 0, 0, 'Deathbringer Jovaan script - Set data 0 2 Image of Warbringer Razuun'),
(2163300, 9, 8, 0, 0, 0, 100, 0, 0, 7000, 7000, 0, 0, 0, 91, 8, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Deathbringer Jovaan script - Remove unit_field_bytes1 (kneel)'),
(2163300, 9, 9, 0, 0, 0, 100, 0, 0, 1000, 1000, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Deathbringer Jovaan script - Say line'),
(2163300, 9, 10, 0, 0, 0, 100, 0, 0, 6000, 6000, 0, 0, 0, 45, 0, 3, 0, 0, 0, 0, 19, 21502, 10, 0, 0, 0, 0, 0, 'Deathbringer Jovaan script - Set data 0 3 Image of Warbringer Razuun'),
(2163300, 9, 11, 0, 0, 0, 100, 0, 0, 4000, 4000, 0, 0, 0, 1, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Deathbringer Jovaan script - Say line'),
(2163300, 9, 12, 0, 0, 0, 100, 0, 0, 4000, 4000, 0, 0, 0, 45, 0, 4, 0, 0, 0, 0, 19, 21502, 10, 0, 0, 0, 0, 0, 'Deathbringer Jovaan script - Set data 0 4 Image of Warbringer Razuun'),
(2163300, 9, 13, 0, 0, 0, 100, 0, 0, 3500, 3500, 0, 0, 0, 33, 21502, 0, 0, 0, 0, 0, 18, 30, 0, 0, 0, 0, 0, 0, 'Deathbringer Jovaan script - Give Quest Credit'),
(2163300, 9, 14, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 5, 66, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Deathbringer Jovaan script - Play emote'),

(21502, 0, 0, 0, 38, 0, 100, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Image of Warbringer Razuun - On data 0 1 set - Say line'),
(21502, 0, 1, 0, 38, 0, 100, 0, 0, 0, 2, 0, 0, 0, 1, 1, 2000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Image of Warbringer Razuun - On data 0 2 set - Say line'),
(21502, 0, 2, 0, 52, 0, 100, 0, 0, 1, 21502, 0, 0, 0, 5, 25, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Image of Warbringer Razuun - On text over - Play emote'),
(21502, 0, 3, 0, 38, 0, 100, 0, 0, 0, 3, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Image of Warbringer Razuun - On data 0 3 set - Say line'),
(21502, 0, 4, 0, 38, 0, 100, 0, 0, 0, 4, 0, 0, 0, 1, 3, 1500, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Image of Warbringer Razuun - On data 0 4 set - Say line'),
(21502, 0, 5, 0, 52, 0, 100, 0, 0, 3, 21502, 0, 0, 0, 5, 25, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Image of Warbringer Razuun - On text over - Play emote'),

(184834, 1, 0, 0, 38, 0, 100, 0, 0, 0, 1, 0, 0, 0, 99, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Infernal  - On Data Set- Change State'),
(184834, 1, 1, 0, 70, 0, 100, 0, 0, 2, 0, 0, 0, 0, 70, 50, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Infernal - On Data Set- Despawn GO'),

(184835, 1, 0, 0, 38, 0, 100, 0, 0, 0, 1, 0, 0, 0, 99, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Infernal 2 - On Data Set- Change State'),
(184835, 1, 1, 0, 70, 0, 100, 0, 0, 2, 0, 0, 0, 0, 70, 50, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Infernal 2 - On Data Set - Despawn GO');

update creature_template set unit_flags = 768 where entry in (21502,21633);

delete from conditions where sourceentry = 184833 and sourcetypeorreferenceid = 22;
delete from conditions where sourceentry = 4548 and sourcetypeorreferenceid = 22;

delete from spell_script_names where spell_id = 37097;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES 
(37097, 'spell_crate_dusguise');

update gameobject set spawntimesecs = -300 where id = 300112;

update creature_template_wdb set displayid1 = 20366, displayid2 = 0 where entry in (21629);