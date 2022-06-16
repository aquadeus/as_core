-- https://forum.wowcircle.com/showthread.php?t=909448
delete from conditions where sourceentry = 42222 and sourcetypeorreferenceid = 17;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(17, 0, 42222, 0, 0, 29, 0, 23616, 30, 0, 0, 0, '', 'Tender meat within 10 yards of Kyle');

delete from creature where id = 23616;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(23616, 1, 215, 222, 1, 1, 0, 0, 0, -2407.46, -453.892, -8.89108, 0.576545, 120, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);


-- https://forum.wowcircle.com/showthread.php?t=977185
update quest_template set method = 2 where id = 38818;


-- https://forum.wowcircle.com/showthread.php?t=954196
update quest_template set method = 2 where id = 38624;

delete from conditions where SourceTypeOrReferenceId = 26 and SourceGroup = 7541 and SourceEntry = 39652;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 7541, 39652, 0, 0, 48, 0, 279773, 0, 0, 0, 0, '', 'Phase 39652 in zone 7541 if player complete obj 279773'),
(26, 7541, 39652, 0, 1, 8, 0, 39652, 0, 0, 0, 0, '', 'Phase 39652 in zone 7541 if player reward quest 39652'),
(26, 7541, 39652, 0, 1, 48, 0, 278235, 0, 0, 1, 0, '', 'Phase 39652 in zone 7541 if player not complete obj 278235'),
(26, 7541, 39652, 0, 1, 8, 0, 39803, 0, 0, 1, 0, '', 'Phase 39652 in zone 7541 if player not reward quest 39803');

delete from npc_spellclick_spells where npc_entry = 91767;
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES 
(91767, 182921, 1, 0);


-- https://forum.wowcircle.com/showthread.php?t=934298
update quest_template set method = 2 where id = 43370;

update creature set spawnmask = 1 where id = 110447;


-- https://forum.wowcircle.com/showthread.php?t=795603
update quest_template set method = 2 where id = 42835;

delete from creature_text where entry in (112055);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextID`, `comment`) VALUES 
(112055, 0, 0, 'Подойди к столу. Когда начнем, все надо будет делать очень быстро. Ингредиенты нежные и моментально портятся.', 12, 0, 100, 1, 0, 66296, 115231, ''),
(112055, 1, 0, 'У тебя прирожденный талант. Однако в купаже чего-то не хватает. Хм…', 12, 0, 100, 0, 0, 66297, 115237, ''),
(112055, 1, 1, 'Неплохо. Так, посмотрим…', 12, 0, 100, 1, 0, 66298, 115236, ''),
(112055, 1, 2, 'То, что надо. И все равно чего-то вроде не хватает…', 12, 0, 100, 0, 0, 66299, 115238, ''),
(112055, 2, 0, 'Мм-м... Пожалуй, нужно добавить цветочных ноток.', 12, 0, 100, 0, 0, 66300, 115239, ''),
(112055, 3, 0, 'Получилось несколько густовато. Надо бы слегка разбавить.', 12, 0, 100, 0, 0, 66301, 115240, ''),
(112055, 4, 0, 'Думаю, тут нужна кислинка. Добавь темный фрукт, пожалуйста.', 12, 0, 100, 0, 0, 66302, 115241, ''),
(112055, 5, 0, 'Сокам нужен массаж. Прыгай в чан и хорошенечко потопай ногами.', 12, 0, 100, 25, 0, 66303, 115242, ''),
(112055, 6, 0, 'Превосходно! Это правда твой самый первый опыт в виноделии? Так, надо скорее перелить в бутылки.', 12, 0, 100, 1, 0, 66297, 115230, ''),
(112055, 7, 0, 'По-моему, ты витаешь в облаках, а это сказывается на работе. Давай прервемся и начнем сначала.', 12, 0, 100, 0, 0, 0, 115229, '');

delete from conditions where SourceEntry in (217485,217482,217476) and SourceTypeOrReferenceId = 13;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(13, 1, 217485, 0, 1, 31, 0, 3, 109305, 0, 0, 0, '', 'Cast only 109305'),
(13, 1, 217482, 0, 1, 31, 0, 3, 109305, 0, 0, 0, '', 'Cast only 109305'),
(13, 1, 217476, 0, 1, 31, 0, 3, 109305, 0, 0, 0, '', 'Cast only 109305');

delete from gossip_menu where entry in (20193);
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES 
(20193, 29564);

delete from npc_text where id in (29564);
INSERT INTO `npc_text` (`ID`, `BroadcastTextID0`, `Probability0`) VALUES 
(29564, 114955, 0);

delete from gossip_menu_option where menu_id in (20193);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `action_menu_id`, `action_poi_id`, `box_coded`, `box_money`, `box_text`, `OptionBroadcastTextID`, `BoxBroadcastTextID`, `VerifiedBuild`) VALUES 
(20193, 0, 0, 'Хорошо, давай приступим.', 1, 1, 0, 0, 0, 0, '', 114922, 0, 0);

update gameobject_template set flags = 0, data1 = 0, data22 = 0, ainame = '' where entry in (251492,251493,251494);

update creature_template set npcflag = 1, ainame = 'SmartAI', gossip_menu_id = 20193 where entry = 108872;
update creature_template set ainame = 'SmartAI' where entry = 112055;
update creature_template set flags_extra = 128, ainame = 'SmartAI', inhabittype = 7 where entry in (109313,109312,109310,109305);

update creature set spawntimesecs = 90 where id = 108872;

delete from smart_scripts where entryorguid in (109305,109313,109312,109310,112055,11205500,11205501,11205502,11205503) or entryorguid in (108872) and id > 3;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(108872, 0, 4, 5, 62, 0, 100, 0, 20193, 0, 0, 0, 0, 12, 112055, 1, 90000, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on gossip select - summon creature'),
(108872, 0, 5, 6, 61, 0, 100, 0, 0, 0, 0, 0, 0, 33, 109204, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on link - give credit'),
(108872, 0, 6, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on link - despawn'),

(112055, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 80, 11205500, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on just summoned - run timed action list'),
(112055, 0, 1, 0, 8, 0, 100, 1, 217323, 0, 0, 0, 0, 80, 11205501, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on spell hit - run timed action list'),
(112055, 0, 2, 0, 8, 0, 100, 1, 217329, 0, 0, 0, 0, 80, 11205502, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on spell hit - run timed action list'),
(112055, 0, 3, 0, 8, 0, 100, 1, 217328, 0, 0, 0, 0, 80, 11205503, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on spell hit - run timed action list'),
(112055, 0, 4, 5, 8, 0, 100, 1, 217327, 0, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on spell hit - remove aura'),
(112055, 0, 5, 6, 61, 0, 100, 1, 0, 0, 0, 0, 0, 33, 109205, 0, 0, 0, 0, 0, 18, 30, 0, 0, 0, 0, 0, 0, 'Margaux - on link - give credit'),
(112055, 0, 6, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 1, 6, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on link - talk'),
(112055, 0, 7, 0, 8, 0, 100, 1, 217281, 0, 0, 0, 0, 1, 7, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Margaux - on link - talk'),

(11205503, 9, 0, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 28, 217204, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed action list - remove aura'),
(11205503, 9, 1, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed action list - talk'),
(11205503, 9, 2, 0, 0, 0, 100, 1, 6000, 6000, 0, 0, 0, 1, 4, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed action list - talk'),
(11205503, 9, 3, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 11, 217203, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed action list - cast spell'),

(11205502, 9, 0, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 28, 217205, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed action list - remove aura'),
(11205502, 9, 1, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed action list - talk'),
(11205502, 9, 2, 0, 0, 0, 100, 1, 6000, 6000, 0, 0, 0, 1, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed action list - talk'),
(11205502, 9, 3, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 11, 217204, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed action list - cast spell'),

(11205501, 9, 0, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 28, 217202, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed action list - remove aura'),
(11205501, 9, 1, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed action list - talk'),
(11205501, 9, 2, 0, 0, 0, 100, 1, 6000, 6000, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed action list - talk'),
(11205501, 9, 3, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 11, 217205, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed action list - cast spell'),

(11205500, 9, 0, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 75, 217189, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed action list - add aura'),
(11205500, 9, 1, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 12, 109305, 1, 90000, 1, 0, 0, 8, 0, 0, 0, 1358.98, 3294.02, 133.44, 0, 'Timed action list - summon creature'),
(11205500, 9, 2, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 12, 109313, 1, 90000, 1, 0, 0, 8, 0, 0, 0, 1351.73, 3297.68, 134.07, 0, 'Timed action list - summon creature'),
(11205500, 9, 3, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 12, 109312, 1, 90000, 1, 0, 0, 8, 0, 0, 0, 1352.68, 3298.98, 135.69, 0, 'Timed action list - summon creature'),
(11205500, 9, 4, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 12, 109310, 1, 90000, 1, 0, 0, 8, 0, 0, 0, 1354.24, 3300.79, 134.07, 0, 'Timed action list - summon creature'),
(11205500, 9, 5, 0, 0, 0, 100, 1, 1000, 1000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed action list - talk'),
(11205500, 9, 6, 0, 0, 0, 100, 1, 12000, 12000, 0, 0, 0, 1, 5, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed action list - talk'),
(11205500, 9, 7, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 11, 217202, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed action list - cast spell'),

(109305, 0, 0, 0, 10, 0, 100, 0, 1, 1, 5000, 5000, 0, 85, 217062, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on ooc los - invoker cast'),
(109313, 0, 0, 0, 8, 0, 100, 0, 217329, 0, 0, 0, 0, 11, 217485, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on spell hit - cast spell'),
(109312, 0, 0, 0, 8, 0, 100, 0, 217328, 0, 0, 0, 0, 11, 217482, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on spell hit - cast spell'),
(109310, 0, 0, 0, 8, 0, 100, 0, 217327, 0, 0, 0, 0, 11, 217476, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on spell hit - cast spell');

delete from conditions where SourceEntry in (112055) and SourceTypeOrReferenceId = 22;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 2, 112055, 0, 0, 1, 1, 217202, 0, 0, 0, 0, '', 'Allow Smart if creature has aura'),
(22, 3, 112055, 0, 0, 1, 1, 217205, 0, 0, 0, 0, '', 'Allow Smart if creature has aura'),
(22, 4, 112055, 0, 0, 1, 1, 217204, 0, 0, 0, 0, '', 'Allow Smart if creature has aura'),
(22, 5, 112055, 0, 0, 1, 1, 217203, 0, 0, 0, 0, '', 'Allow Smart if creature has aura'),
(22, 2, 112055, 0, 0, 1, 1, 217189, 0, 0, 0, 0, '', 'Allow Smart if creature has aura'),
(22, 3, 112055, 0, 0, 1, 1, 217189, 0, 0, 0, 0, '', 'Allow Smart if creature has aura'),
(22, 4, 112055, 0, 0, 1, 1, 217189, 0, 0, 0, 0, '', 'Allow Smart if creature has aura'),
(22, 5, 112055, 0, 0, 1, 1, 217189, 0, 0, 0, 0, '', 'Allow Smart if creature has aura');


delete from conditions where SourceGroup in (20193) and SourceTypeOrReferenceId = 15;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 20193, 0, 0, 0, 9, 0, 42835, 0, 0, 0, 0, '', 'Show gossip option if player has 42835');


-- https://forum.wowcircle.com/showthread.php?t=886525
update quest_template set method = 2 where id = 40072;