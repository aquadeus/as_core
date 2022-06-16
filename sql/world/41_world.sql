-- https://forum.wowcircle.net/showthread.php?t=821492
update gameobject_template set displayid = 299 where entry = 205822;

delete from gameobject where id = 205822;
INSERT INTO `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `ScriptName`, `custom_flags`, `VerifiedBuild`) VALUES 
(205822, 0, 4922, 5472, 1, 1, 0, -4343.79, -4578.82, 219.456, 0.405579, 0, 0, 0.201402, 0.979509, -300, 0, 1, 0, '', 0, -1);


-- https://forum.wowcircle.net/showthread.php?t=822071
update creature_template set ainame = 'SmartAI', gossip_menu_id = 19908 where entry = 109223;
update creature_template set ainame = 'SmartAI' where entry = 108875;

delete from quest_start_scripts where id = 42833;

delete from creature where id = 108931 and position_x like '1370%';

delete from smart_scripts where entryorguid in (109223,10922300, 108875,10887500);
delete from smart_scripts where entryorguid in (108872) and id = 3;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(108872, 0, 3, 0, 19, 0, 100, 0, 42833, 0, 0, 0, 0, 85, 217309, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Margaux - quest accept - invoker cast'),

(108875, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 80, 10887500, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Guard - on just summoned - run Timed Action List'),

(10887500, 9, 0, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed Action List - talk'),
(10887500, 9, 1, 0, 0, 0, 100, 1, 4000, 4000, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 19, 108931, 0, 0, 0, 0, 0, 0, 'Timed Action List - simple talk'),
(10887500, 9, 2, 0, 0, 0, 100, 1, 5000, 5000, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed Action List - talk'),
(10887500, 9, 3, 0, 0, 0, 100, 1, 2000, 2000, 0, 0, 0, 5, 36, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed Action List - play emote'),
(10887500, 9, 4, 0, 0, 0, 100, 1, 2000, 2000, 0, 0, 0, 84, 1, 0, 0, 0, 0, 0, 19, 108931, 0, 0, 0, 0, 0, 0, 'Timed Action List - simple talk'),

(10922300, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 12, 108875, 1, 60000, 0, 0, 0, 8, 0, 0, 0, 1368.8662, 3406.8877, 132.8790, 1.11505, 'Timed Action List - summon creature'),
(10922300, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 12, 108931, 1, 60000, 0, 0, 0, 8, 0, 0, 0, 1370.2600, 3408.6499, 132.9620, 4.14343, 'Timed Action List - summon creature'),
(10922300, 9, 2, 0, 0, 0, 100, 0, 11000, 11000, 0, 0, 0, 53, 1, 1092233, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed Action List - start waypoint'),
(10922300, 9, 3, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 1, 11, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed Action List - talk'),
(10922300, 9, 4, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 0, 1, 12, 14000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed Action List - talk'),
(10922300, 9, 5, 0, 0, 0, 100, 0, 8000, 8000, 0, 0, 0, 84, 2, 0, 0, 0, 0, 0, 19, 108875, 0, 0, 0, 0, 0, 0, 'Timed Action List - talk'),

(109223, 0, 0 , 1, 54, 0, 100, 1, 0, 0, 0, 0, 0, 53, 0, 1092230, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on just summoned - start waypoint'),
(109223, 0, 1 , 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 1, 0, 6000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on link - talk'),
(109223, 0, 2 , 0, 52, 0, 100, 1, 0, 109223, 0, 0, 0, 1, 1, 8000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on text over - talk'),
(109223, 0, 3 , 0, 52, 0, 100, 1, 1, 109223, 0, 0, 0, 1, 2, 8000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on text over - talk'),
(109223, 0, 4 , 5, 40, 0, 100, 1, 3, 1092230, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on waypoint reached - set orientation'),
(109223, 0, 5 , 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 81, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on link - set npcflag'),
(109223, 0, 6 , 7, 62, 0, 100, 1, 20194, 0, 0, 0, 0, 85, 217359, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on gossip select - invoker cast'),
(109223, 0, 7 , 8, 61, 0, 100, 1, 0, 0, 0, 0, 0, 53, 0, 1092231, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on link - start waypoint'),
(109223, 0, 8 , 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on link - set npcflag'),
(109223, 0, 9 , 0, 40, 0, 100, 1, 2, 1092231, 0, 0, 0, 1, 3, 5000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on waypont reached - talk'),
(109223, 0, 10, 0, 52, 0, 100, 1, 3, 109223, 0, 0, 0, 1, 4, 10000, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on text over - talk'),
(109223, 0, 11, 5, 40, 0, 100, 1, 4, 1092231, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 2.45966, 'Margaux - on waypoint reached - set orientation'),
(109223, 0, 12, 13, 62, 0, 100, 0, 19905, 0, 0, 0, 0, 85, 217371, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on gossip select - invoker cast'),
(109223, 0, 13, 8, 61, 0, 100, 0, 0, 0, 0, 0, 0, 53, 0, 1092232, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on link - start waypoint'),
(109223, 0, 14, 0, 40, 0, 100, 0, 1, 1092232, 0, 0, 0, 1, 5, 5000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on waypoint reached - talk'),
(109223, 0, 15, 0, 52, 0, 100, 0, 5, 109223, 0, 0, 0, 1, 6, 8000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on text over - talk'),
(109223, 0, 16, 0, 52, 0, 100, 0, 6, 109223, 0, 0, 0, 1, 7, 6500, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on text over - talk'),
(109223, 0, 17, 0, 52, 0, 100, 0, 7, 109223, 0, 0, 0, 1, 8, 4000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on text over - talk'),
(109223, 0, 18, 0, 52, 0, 100, 0, 8, 109223, 0, 0, 0, 1, 9, 7000, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on text over - talk'),
(109223, 0, 19, 20, 52, 0, 100, 0, 9, 109223, 0, 0, 0, 1, 10, 4000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on text over - talk'),
(109223, 0, 20, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 80, 10922300, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on link - run timed action list'),
(109223, 0, 21, 0, 52, 0, 100, 0, 12, 109223, 0, 0, 0, 53, 0, 1092234, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on text over - start waypoint'),
(109223, 0, 22, 5, 40, 0, 100, 0, 2, 1092234, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on waypoint reached - set orientation'),
(109223, 0, 23, 24, 62, 0, 100, 1, 20200, 0, 0, 0, 0, 85, 217374, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on gossip select - invoker cast'),
(109223, 0, 24, 25, 61, 0, 100, 0, 0, 0, 0, 0, 0, 53, 0, 1092235, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on link - start waypoint'),
(109223, 0, 25, 8, 61, 0, 100, 0, 0, 0, 0, 0, 0, 1, 13, 3000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on link - talk'),
(109223, 0, 26, 0, 52, 0, 100, 0, 13, 109223, 0, 0, 0, 1, 14, 5000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on text over - talk'),
(109223, 0, 27, 0, 52, 0, 100, 0, 14, 109223, 0, 0, 0, 1, 15, 10000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on text over - talk'),
(109223, 0, 28, 0, 52, 0, 100, 0, 15, 109223, 0, 0, 0, 1, 16, 8000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on text over - talk'),
(109223, 0, 29, 0, 52, 0, 100, 0, 16, 109223, 0, 0, 0, 1, 17, 8000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on text over - talk'),
(109223, 0, 30, 31, 40, 0, 100, 0, 5, 1092235, 0, 0, 0, 33, 109244, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on waypoint reached - give credit'),
(109223, 0, 31, 32, 61, 0, 100, 0, 0, 0, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on link - set orientation'),
(109223, 0, 32, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 41, 10000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on link - despawn');

delete from gossip_menu_option where menu_id in (19905,19908,20194,20200);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `action_menu_id`, `action_poi_id`, `box_coded`, `box_money`, `box_text`, `OptionBroadcastTextID`, `BoxBroadcastTextID`, `VerifiedBuild`) VALUES 
(19905, 0, 0, 'Thank you! I am ready to move on now.', 1, 1, 0, 0, 0, 0, '', 115006, 0, 0),
(19908, 0, 0, 'She is surviving, but not easily.', 1, 1, 20194, 0, 0, 0, '', 114998, 0, 0),
(19908, 1, 0, 'Thalyssra said you had a unique way of dealing with insects. What is it?', 1, 1, 19905, 0, 0, 0, '', 114999, 0, 0),
(19908, 2, 0, 'Hope is not lost. We are making progress every day.', 1, 1, 20200, 0, 0, 0, '', 115000, 0, 0),
(20194, 0, 0, 'After you, Margaux.', 1, 1, 0, 0, 0, 0, '', 115004, 0, 0),
(20200, 0, 0, 'I am ready.', 1, 1, 0, 0, 0, 0, '', 115011, 0, 0);

delete from conditions where SourceGroup in (19908) and SourceTypeOrReferenceId in (15,14);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 19908, 0, 0, 0, 48, 0, 284879, 0, 0, 1, 0, '', 'Show Gossip If Player not complete 284879 objective'),
(15, 19908, 1, 0, 0, 48, 0, 284879, 0, 0, 0, 0, '', 'Show Gossip If Player complete 284879 objective'),
(15, 19908, 1, 0, 0, 48, 0, 284880, 0, 0, 1, 0, '', 'Show Gossip If Player not complete 284880 objective'),
(15, 19908, 2, 0, 0, 48, 0, 284880, 0, 0, 0, 0, '', 'Show Gossip If Player complete 284879 objective'),
(15, 19908, 2, 0, 0, 48, 0, 284881, 0, 0, 1, 0, '', 'Show Gossip If Player not complete 284879 objective'),

(14, 19908, 29574, 0, 0, 48, 0, 284879, 0, 0, 1, 0, '', 'Show Gossip If Player not complete 284879 objective'),
(14, 19908, 29578, 0, 0, 48, 0, 284879, 0, 0, 0, 0, '', 'Show Gossip If Player complete 284879 objective'),
(14, 19908, 29578, 0, 0, 48, 0, 284880, 0, 0, 1, 0, '', 'Show Gossip If Player not complete 284880 objective'),
(14, 19908, 29580, 0, 0, 48, 0, 284880, 0, 0, 0, 0, '', 'Show Gossip If Player complete 284879 objective'),
(14, 19908, 29580, 0, 0, 48, 0, 284881, 0, 0, 1, 0, '', 'Show Gossip If Player not complete 284879 objective');

delete from gossip_menu where entry in (19905,19908,20194,20200);
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES 
(19908, 29574),
(19908, 29578),
(19908, 29580),
(19905, 29579),
(20194, 29575),
(20200, 29581);

DELETE FROM `creature_text` WHERE `entry` in (109223,108931,108875);
INSERT INTO `creature_text` (`entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `comment`) VALUES 
(108931, 0, 0, 'Я на одну минуту. Я...', 12, 0, 100, 0, 0, 0, 115112, ''),
(108931, 1, 0, 'Ох!', 12, 0, 100, 0, 0, 0, 115114, ''),

(108875, 0, 0, 'Эй, ты! За работу!', 12, 0, 100, 0, 0, 0, 115111, ''),
(108875, 1, 0, 'Хватит с меня ваших оправданий. За работу!', 12, 0, 100, 0, 0, 0, 115113, ''),
(108875, 2, 0, 'Надзиратель об этом узнает. Помяни мое слово, Марго.', 12, 0, 100, 0, 0, 0, 115115, ''),

(109223, 0, 0, 'Мы подходим к месту, где выдерживается вино.', 12, 0, 100, 0, 0, 0, 115021, ''),
(109223, 1, 0, 'Наверняка для тебя не будет новостью тот факт, что в процессе изготовления вина сок должен забродить.', 12, 0, 100, 0, 0, 0, 115025, ''),
(109223, 2, 0, 'Мы ускоряем этот процесс с помощью магии. Пользуемся данной нам силой!', 12, 0, 100, 0, 0, 0, 115022, ''),
(109223, 3, 0, 'Взгляни налево, и ты увидишь наш сад.', 12, 0, 100, 0, 0, 0, 115023, ''),
(109223, 4, 0, 'Здесь мы выращиваем особые плоды. Из них получаются самые сладкие настойки.', 12, 0, 100, 0, 0, 0, 115026, ''),
(109223, 5, 0, 'Прямо перед тобой – наши станции розлива.', 12, 0, 100, 0, 0, 0, 115028, ''),
(109223, 6, 0, 'Бутылки запечатываются магией, чтобы сущность Колодца не выдыхалась.', 12, 0, 100, 0, 0, 0, 115029, ''),
(109223, 7, 0, 'Наше чародейское вино может храниться столетиями!', 12, 0, 100, 0, 0, 0, 115030, ''),
(109223, 8, 0, 'А здесь у нас распределительная станция.', 12, 0, 100, 0, 0, 0, 115031, ''),
(109223, 9, 0, 'Отсюда сотни бочек и бочонков чародейского вина отправляются в город.', 12, 0, 100, 0, 0, 0, 115032, ''),
(109223, 10, 0, 'Магистр чрезвычайно внимательно следит за…', 12, 0, 100, 0, 0, 0, 115033, ''),
(109223, 11, 0, 'Не бей его!', 12, 0, 100, 0, 0, 0, 115034, ''),
(109223, 12, 0, 'Мне очень жаль, прошу простить мою ошибку. Обещаю, что этого больше никогда не повторится.', 12, 0, 100, 0, 0, 0, 115035, ''),
(109223, 13, 0, 'О чем это я?', 12, 0, 100, 0, 0, 0, 115036, ''),
(109223, 14, 0, 'По ту сторону моста происходит важнейшая часть процесса.', 12, 0, 100, 0, 0, 0, 115038, ''),
(109223, 15, 0, 'Там мы добавляем в вино сущность Ночного Колодца, наделяющую напиток восстанавливающими свойствами.', 12, 0, 100, 0, 0, 0, 115039, ''),
(109223, 16, 0, 'Здесь я тебя оставлю. Надо подготовиться к следующему пункту программы.', 12, 0, 100, 0, 0, 0, 115040, ''),
(109223, 17, 0, 'Встретимся на следующей станции. Мой помощник Сильверин с радостью покажет тебе процесс насыщения.', 12, 0, 100, 0, 0, 0, 115042, '');

delete from waypoints where entry in (1092230,1092231,1092232,1092233,1092234,1092235);
INSERT INTO `waypoints` (`entry`, `pointid`, `position_x`, `position_y`, `position_z`, `point_comment`) VALUES 
(1092230, 1, 1512.7041, 3548.4907, 136.6943, ''),
(1092230, 2, 1482.1466, 3550.0710, 132.6180, ''),
(1092230, 3, 1467.6787, 3552.2388, 132.8792, ''),

(1092231, 1, 1464.6237, 3532.3711, 132.8792, ''),
(1092231, 2, 1455.7306, 3518.1343, 132.8792, ''),
(1092231, 3, 1433.4196, 3490.9312, 132.8790, ''),
(1092231, 4, 1441.3781, 3484.2859, 133.1506, ''),

(1092232, 1, 1425.7115, 3480.9963, 132.8790, ''),
(1092232, 2, 1408.5114, 3454.7058, 132.8790, ''),
(1092232, 3, 1396.6823, 3441.7034, 132.8790, ''),
(1092232, 4, 1381.4075, 3432.3733, 132.8792, ''),
(1092232, 5, 1372.6230, 3420.8699, 132.8791, ''),

(1092233, 1, 1368.6631, 3410.8401, 132.8790, ''),

(1092234, 1, 1371.3385, 3418.0618, 132.8790, ''),
(1092234, 2, 1391.0398, 3430.1433, 132.8790, ''),

(1092235, 1, 1395.4933, 3441.1770, 132.8790, ''),
(1092235, 2, 1419.5653, 3435.6997, 132.8790, ''),
(1092235, 3, 1419.5653, 3435.6997, 132.8790, ''),
(1092235, 4, 1452.1512, 3420.6824, 139.0021, ''),
(1092235, 5, 1488.5591, 3426.5535, 132.8176, '');


-- https://forum.wowcircle.net/showthread.php?t=821501
update gossip_menu_option set option_id = 1, npc_option_npcflag = 1 where menu_id = 12343;
update creature_template set flags_extra = 128, unit_flags = 768, ainame = 'SmartAI' where entry in (47838,47839,47840);
update creature_template set ainame = 'SmartAI' where entry in (47826,47824,47823);

delete from conditions where SourceGroup in (12343) and SourceTypeOrReferenceId in (15);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 12343, 0, 0, 0, 9, 0, 28170, 0, 0, 0, 0, '', 'Show gossip if player has quest');

delete from creature where id in (47838,47839,47840);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`) VALUES 
(47838, 0, 4922, 5139, 1, 2, 0, 0, 0, -3410.22, -4241.92, 221.295, 0, 180, 0, 0, 46, 1000, 0),
(47839, 0, 4922, 5139, 1, 2, 0, 0, 0, -3461.32, -4200.27, 212.864, 0, 180, 0, 0, 46, 1000, 0),
(47840, 0, 4922, 5139, 1, 2, 0, 0, 0, -3484.68, -4244.20, 214.497, 0, 180, 0, 0, 46, 1000, 0);

update creature set phasemask = 2 where id in (47824,47823);

delete from smart_scripts where entryorguid in (47838,47839,47840, 47826,47824,47823);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(47826, 0, 0, 0, 62, 0, 100, 0, 12343, 0, 0, 0, 0, 85, 88981, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Ucheck - on gossip select - cast'),

(47838, 0, 0, 0, 10, 0, 100, 0, 1, 10, 5000, 5000, 0, 33, 47838, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Ghost - on spell hit - die'),
(47839, 0, 0, 0, 10, 0, 100, 0, 1, 10, 5000, 5000, 0, 33, 47839, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Ghost - on spell hit - die'),
(47840, 0, 0, 0, 10, 0, 100, 0, 1, 10, 5000, 5000, 0, 33, 47840, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Ghost - on spell hit - die'),

(47824, 0, 0, 0, 8, 0, 100, 1, 88983, 0, 0, 0, 0, 37, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Ghost - on spell hit - die'),
(47823, 0, 0, 0, 8, 0, 100, 1, 88983, 0, 0, 0, 0, 37, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Ghost - on spell hit - die');

DELETE FROM `spell_area` WHERE `spell` in (88981) and area = 5139;
INSERT INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`) VALUES 
(88981, 5139, 28170, 28170, 0, 0, 2, 0, 8, 66);


-- https://forum.wowcircle.net/showthread.php?t=822761
delete from quest_template_objective where questid = 25498;
update quest_template_objective set id = 284002 where objectid = 107023;


-- https://forum.wowcircle.net/showthread.php?t=823475
delete from smart_scripts where entryorguid in (98266,98267);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(98266, 0, 0, 0, 8, 0, 100, 0, 193618, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Anomaly - on spell hit - despawn'),

(98267, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 1, 0, 2000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Thalisra - on just summoned - talk'),
(98267, 0, 1, 0, 52, 0, 100, 1, 0, 98267, 0, 0, 0, 1, 1, 6500, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Thalisra - on text over - talk'),
(98267, 0, 2, 0, 52, 0, 100, 1, 1, 98267, 0, 0, 0, 1, 2, 6500, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Thalisra - on text over - talk'),
(98267, 0, 3, 0, 52, 0, 100, 1, 2, 98267, 0, 0, 0, 1, 3, 7000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Thalisra - on text over - talk'),
(98267, 0, 4, 0, 52, 0, 100, 1, 3, 98267, 0, 0, 0, 1, 4, 4000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Thalisra - on text over - talk'),
(98267, 0, 5, 0, 52, 0, 100, 1, 4, 98267, 0, 0, 0, 1, 5, 3000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Thalisra - on text over - talk'),
(98267, 0, 6, 7, 52, 0, 100, 1, 5, 98267, 0, 0, 0, 11, 164233, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Thalisra - on text over - cast'),
(98267, 0, 7, 0, 61, 0, 100, 1, 5, 98267, 0, 0, 0, 33, 98266, 0, 0, 0, 0, 0, 18, 30, 0, 0, 0, 0, 0, 0, 'Thalisra - on link - give credit');

update creature set spawntimesecs = 30 where id = 98266;

delete from creature_template_addon where entry in (98266);
INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `bytes2`, `auras`) VALUES
(98266, 0, 0, 0, '193606');

delete from event_scripts where id in (46873);
INSERT INTO `event_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `dataint`, `x`, `y`, `z`, `o`) VALUES 
(46873, 0, 10, 98267, 30000, 0, -850.6820, 4632.3101, 749.6390, 1.47);

update creature_text set broadcasttextid = 100088 where entry = 98267 and groupid = 0;
update creature_text set broadcasttextid = 100089 where entry = 98267 and groupid = 1;
update creature_text set broadcasttextid = 100091 where entry = 98267 and groupid = 2;
update creature_text set broadcasttextid = 100094 where entry = 98267 and groupid = 3;
update creature_text set broadcasttextid = 100096 where entry = 98267 and groupid = 4;
update creature_text set broadcasttextid = 100256 where entry = 98267 and groupid = 5;

 
-- https://forum.wowcircle.com/showthread.php?t=861013
update quest_template set method = 2 where id = 43377;

-- update creature_loot_template set difficultymask = 0, lootmode = 0, chanceorquestchance = 100 where item = 139485;

DELETE FROM `spell_area` WHERE `spell` in (220880) and area = 7834;
DELETE FROM `spell_area` WHERE `spell` in (220880) and area = 8356;
INSERT INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`) VALUES 
(220880, 8356, 43377, 43377, 0, 0, 2, 1, 8, 66);

delete from conditions where SourceEntry in (220881) and SourceTypeOrReferenceId = 13;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(13, 1, 220881, 0, 0, 31, 0, 3, 110989, 0, 0, 0, '', 'Cast only 110989');

update creature_template set npcflag = 16777216, ainame = 'SmartAI' where entry in (110989);

delete from smart_scripts where entryorguid in (110989);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(110989, 0, 0, 0, 8, 0, 100, 0, 220881, 0, 0, 0, 0, 33, 102587, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on spell hit - give credit');


update quest_template_objective set amount = 0 where objectid = 49793 and questid = 43377;


-- https://forum.wowcircle.com/showthread.php?t=963930
delete from creature_queststarter where quest = 35989;
delete from creature_questender where quest = 35989;


-- https://forum.wowcircle.com/showthread.php?t=992203
delete from quest_template_objective where questid = 8488;
REPLACE INTO `quest_template_objective` (`ID`, `QuestID`, `Type`, `Order`, `Index`, `ObjectID`, `Amount`, `Flags`, `Flags2`, `ProgressBarWeight`) VALUES 
(289860, 8488, 0, 0, 1, 15958, 1, 0, 0, 0),
(289859, 8488, 0, 0, 0, 15402, 1, 0, 0, 0);

update quest_template set flags = 0, specialflags = 0 where id = 8488;