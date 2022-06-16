-- https://forum.wowcircle.net/showthread.php?t=1064043
update quest_template set Method = 2 where Id = 14218;


-- https://forum.wowcircle.net/showthread.php?t=1064049
update quest_template set Flags = 2097160, Method = 0 where Id = 14375;

update quest_template_locale set OfferRewardText = 'Я тебя вытащу, Ёорл. Я тебе приготовил такую дозу, что хватит свалить лошадь, но я знаю, что делаю. Я знаю, из какого теста ты сделан. Ты выдержишь, и все будет хорошо.

Верь мне, Ёорл. Я понимаю, через что тебе придется пройти...

А теперь выпей это и закрой глаза.' where ID = 14375 and locale = 'ruRU';


-- https://forum.wowcircle.net/showthread.php?t=1064051
update quest_template set Method = 2 where Id = 14382;


-- https://forum.wowcircle.net/showthread.php?t=1064129
update creature_template set faction = 35, ScriptName = '', AIName = 'SmartAI' where entry = 73284;

delete from smart_scripts where entryorguid in (73284);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(73284, 0, 0, 1, 73, 0, 100, 1, 0, 0, 0, 0, 0, 0, 33, 73284, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'wolf - on spellclick - give credit'),
(73284, 0, 1, 2, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 83, 16777216, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'wolf - on link - remove npcflag'),
(73284, 0, 2, 3, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 89, 10, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'wolf - on link - random move'),
(73284, 0, 3, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 41, 6000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'wolf - on link - despawn'),
(73284, 0, 4, 0, 60, 0, 100, 1, 0, 0, 0, 0, 0, 0, 81, 16777216, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'wolf - on update - set npcflag');

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` in (73284);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 1, 73284, 0, 0, 9, 0, 33145, 0, 0, 0, 0, '', 'smart if quest incomplete');


-- https://forum.wowcircle.net/showthread.php?t=1064133
update quest_template set RewardSpellCast = 0, PrevQuestId = 0, NextQuestId = 0, NextQuestIdChain = 0 where id in (33132,33125);

update creature_template set AIName = 'SmartAI' where entry = 72976;

delete from smart_scripts where entryorguid in (72976);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(72976, 0, 0, 0, 20, 0, 100, 0, 0, 33132, 0, 0, 0, 0, 85, 159371, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'urukag - on quest reward - invoker cast'),
(72976, 0, 1, 0, 20, 0, 100, 0, 0, 33125, 0, 0, 0, 0, 85, 159371, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'urukag - on quest reward - invoker cast');

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` in (72976);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 1, 72976, 0, 0, 8, 0, 33125, 0, 0, 0, 0, '', 'smart if quest reward'),
(22, 2, 72976, 0, 0, 8, 0, 33132, 0, 0, 0, 0, '', 'smart if quest reward');


-- https://forum.wowcircle.net/showthread.php?t=1064135
update creature_template set npcflag = 16777216, AIName = 'SmartAI', faction = 35, unit_flags = 768 where entry in (72959,73467,73466);

UPDATE `creature_template_wdb` SET `KillCredit2`='0' WHERE (`Entry`='72959');

delete from npc_spellclick_spells where npc_entry in (72959,73467,73466);
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES 
(72959, 145949, 1, 0),
(73467, 145949, 1, 0),
(73466, 145949, 1, 0);

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 18 AND `SourceEntry` in (145949);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(18, 72959, 145949, 0, 0, 9, 0, 33125, 0, 0, 0, 0, '', 'spellclick if quest incomplete'),
(18, 73467, 145949, 0, 0, 9, 0, 33125, 0, 0, 0, 0, '', 'spellclick if quest incomplete'),
(18, 73466, 145949, 0, 0, 9, 0, 33125, 0, 0, 0, 0, '', 'spellclick if quest incomplete');

delete from smart_scripts where entryorguid in (72959,73467,73466);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(72959, 0, 0, 1, 8, 0, 100, 0, 0, 145949, 0, 0, 0, 0, 33, 72959, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'body - on spell hit - give credit'),
(72959, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'body - on link - despawn'),

(73467, 0, 0, 1, 8, 0, 100, 0, 0, 145949, 0, 0, 0, 0, 33, 73467, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'body - on spell hit - give credit'),
(73467, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'body - on link - despawn'),

(73466, 0, 0, 1, 8, 0, 100, 0, 0, 145949, 0, 0, 0, 0, 33, 73466, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'body - on spell hit - give credit'),
(73466, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'body - on link - despawn');

delete from creature_template_addon where entry in (72959,73467,73466);
INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `bytes2`, `auras`) VALUES
(72959, 0, 0, 0, '29266'),
(73467, 0, 0, 0, '29266'),
(73466, 0, 0, 0, '29266');