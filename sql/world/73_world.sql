-- https://forum.wowcircle.com/showthread.php?t=1051189
delete from conditions where SourceGroup in (986500) and SourceTypeOrReferenceId = 15;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 986500, 0, 0, 0, 9, 0, 41070, 0, 0, 0, 0, '', 'Show gossip if 41070 quest incomplete'),
(15, 986500, 0, 0, 0, 48, 0, 281809, 0, 0, 1, 0, '', 'Phase if criteria not completed'),

(15, 986500, 0, 0, 1, 9, 0, 41062, 0, 0, 0, 0, '', 'Show gossip if 41062 quest incomplete'),
(15, 986500, 0, 0, 1, 48, 0, 281808, 0, 0, 1, 0, '', 'Phase if criteria not completed');


delete from smart_scripts where entryorguid = 98650;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(98650, 0, 0, 1, 62, 0, 100, 0, 986500, 0, 0, 0, 0, 12, 103047, 5, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Gaardun - on gossip select - summon creature'),
(98650, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 0, 33, 98650, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Gaardun - on link - give credit'),
(98650, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Gaardun - on link - close gossip');

delete from phase_definitions where zoneId = 8022 and entry in (41070);
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `flags`, `comment`) VALUES 
(8022, 41070, 0, 41070, 0, '41070 41062 quest');

delete from conditions where SourceGroup in (8022) and SourceTypeOrReferenceId = 26 and sourceentry in (41070);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 8022, 41070, 0, 0, 9, 0, 41062, 0, 0, 0, 0, '', 'Phase If quest 41062 incompleted'),
(26, 8022, 41070, 0, 1, 9, 0, 41070, 0, 0, 0, 0, '', 'Phase If quest 41070 incompleted');

update creature set phasemask = 0, phaseid = 41070 where id = 98650;


-- https://forum.wowcircle.com/showthread.php?t=1051195
update creature_template set ainame = 'SmartAI' where entry = 110482;

delete from smart_scripts where entryorguid = 110482;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(110482, 0, 0, 0, 19, 0, 100, 0, 43412, 0, 0, 0, 0, 75, 219567, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Kayn - on quest accept - apply aura');


-- https://forum.wowcircle.com/showthread.php?t=1051331
update quest_template set prevquestid = 40947 where id = 40745;
update quest_template set prevquestid = 40745 where id = 42722;
update quest_template set prevquestid = 42722 where id = 42486;
update quest_template set prevquestid = 42486 where id = 44051;
update quest_template set prevquestid = 42486 where id = 42487;


-- https://forum.wowcircle.com/showthread.php?t=1051335
DELETE FROM `gameobject_queststarter` WHERE quest in (42489);
INSERT INTO `gameobject_queststarter` (`id`, `quest`) VALUES
(251032, 42489);


-- https://forum.wowcircle.com/showthread.php?t=1051529
update quest_template set prevquestid = 26748 where id = 26749;


-- https://forum.wowcircle.com/showthread.php?t=1044093
update creature_template set unit_flags = 768, npcflag = 16777216 where entry = 75809;

delete from npc_spellclick_spells where npc_entry = 75809;
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES 
(75809, 152903, 1, 0);

delete from conditions where SourceGroup in (75809) and SourceTypeOrReferenceId = 18;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(18, 75809, 152903, 0, 0, 9, 0, 34091, 0, 0, 0, 0, '', 'Allow spellclick if player has Quest'),
(18, 75809, 152903, 0, 1, 9, 0, 33720, 0, 0, 0, 0, '', 'Allow spellclick if player has Quest');

delete from creature where id = 75869;

update creature_template set ainame = 'SmartAI', npcflag = 16777216 where entry = 75869;
update creature_template set ainame = 'SmartAI', flags_extra = 128 where entry = 88765;

delete from npc_spellclick_spells where npc_entry = 75869;
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES 
(75869, 46598, 1, 0);

delete from smart_scripts where entryorguid in (75869,88765);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(75869, 0, 0, 5, 54, 0, 100, 1, 0, 0, 0, 0, 0, 85, 46598, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Shreder - on just summoned - invoker cast'),
(75869, 0, 1, 0, 28, 0, 100, 1, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Shreder - on just summoned - invoker cast'),
(75869, 0, 2, 3, 38, 0, 100, 1, 0, 1, 0, 0, 0, 33, 88765, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Shreder - on set data - give credit'),
(75869, 0, 3, 4, 61, 0, 100, 1, 0, 0, 0, 0, 0, 28, 152903, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Shreder - on link - remove aura'),
(75869, 0, 4, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 28, 46598, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Shreder - on link - remove aura'),
(75869, 0, 5, 0, 61, 0, 100, 1, 0, 1, 0, 0, 0, 33, 75809, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Shreder - on link - give credit'),

(88765, 0, 0, 0, 60, 0, 100, 0, 0, 0, 5000, 5000, 0, 45, 0, 1, 0, 0, 0, 0, 19, 75869, 20, 0, 0, 0, 0, 0, 'Shreder - on set data - give credit');

delete from creature where id = 88765;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(88765, 1116, 6662, 7132, 1, 1, 0, 0, 0, 3398.39, 2066.44, 160.399, 0.992086, 300, 0, 0, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(88765, 1116, 6662, 7132, 1, 1, 0, 0, 0, 3613.3643, 2318.5715, 67.0907, 0.992086, 300, 0, 0, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

delete from creature where id = 75809;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(75809, 1116, 6662, 6662, 1, 1, 0, 0, 0, 3487.01, 2066.21, 194.505, 2.53792, 120, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(75809, 1116, 6662, 7136, 1, 1, 0, 0, 0, 3850.85, 2062.66, 16.4215, 2.57845, 300, 0, 0, 2718175, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

update creature_template set faction = 14 where entry = 76791;


-- https://forum.wowcircle.com/showthread.php?t=1051989
update quest_template set flags = 0 where id = 33735;

delete from gameobject where id in (225731);
INSERT INTO `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES 
(225731, 1116, 6662, 6950, 1, 1, 0, 3082.91, 2003.43, 109.922, 1.98413, 0.0232716, 0.000736237, 0.83707, 0.546601, 120, 255, 1),
(225731, 1116, 6662, 6950, 1, 1, 0, 3100.59, 2029.36, 111.279, 1.35758, 0.0150022, 0.00108433, 0.627862, 0.778179, 120, 255, 1),
(225731, 1116, 6662, 6950, 1, 1, 0, 3115.75, 2033.41, 110.252, 1.14191, 0.102168, 0.019227, 0.540688, 0.834775, 120, 255, 1),
(225731, 1116, 6662, 6950, 1, 1, 0, 3122.27, 1992.33, 111.926, 1.04163, 0, 0, 0.497588, 0.867413, 120, 255, 1),
(225731, 1116, 6662, 6950, 1, 1, 0, 3148.56, 1950.64, 118.475, 1.25883, -0.0140276, 0.0410347, 0.58769, 0.807923, 120, 255, 1),
(225731, 1116, 6662, 6950, 1, 1, 0, 3156.53, 1996.91, 116.028, 4.67605, 0, 0, -0.719837, 0.694143, 120, 255, 1),
(225731, 1116, 6662, 6950, 1, 1, 0, 3165.98, 1918.58, 127.449, 1.74393, 0, 0, 0.765594, 0.643323, 120, 255, 1),
(225731, 1116, 6662, 6950, 1, 1, 0, 3192.71, 2063.07, 109.095, 1.30144, 0.0121455, 0.0000038147, 0.605772, 0.795546, 120, 255, 1),
(225731, 1116, 6662, 6950, 1, 1, 0, 3195.89, 1976.29, 120.889, 2.20469, -0.0591283, 0.0525456, 0.888773, 0.45147, 120, 255, 1),
(225731, 1116, 6662, 6950, 1, 1, 0, 3226.19, 1915.48, 127.995, 0.723288, 0, 0, 0.353812, 0.935316, 120, 255, 1),
(225731, 1116, 6662, 6950, 1, 1, 0, 3234.91, 2104.93, 111.035, 4.78032, 0, 0, -0.682686, 0.730712, 120, 255, 1),
(225731, 1116, 6662, 6950, 1, 1, 0, 3253.81, 1990.71, 130.864, 1.15349, 0.090817, 0.0153475, 0.545587, 0.832977, 120, 255, 1),
(225731, 1116, 6662, 6950, 1, 1, 0, 3274.89, 1973.33, 134.018, 1.3208, 0, 0, 0.613431, 0.789748, 120, 255, 1),
(225731, 1116, 6662, 6950, 1, 1, 0, 3310.11, 1984.32, 138.047, 4.89729, 0.0255384, -0.0263233, -0.638268, 0.76894, 120, 255, 1),
(225731, 1116, 6662, 6950, 1, 1, 0, 3376.18, 1953.76, 160.488, 1.15349, 0.090817, 0.0153475, 0.545587, 0.832977, 120, 255, 1),
(225731, 1116, 6662, 6947, 1, 1, 0, 3717.32, 2604.33, 3.46591, 3.39327, -0.000636101, -0.0718498, -0.989442, 0.125865, 120, 255, 1),
(225731, 1116, 6662, 6947, 1, 1, 0, 3754.12, 2567.84, 9.20442, 4.45698, -0.0789061, -0.106105, -0.781641, 0.609551, 120, 255, 1),
(225731, 1116, 6662, 6947, 1, 1, 0, 3786.34, 2607.8, 2.96196, 4.39892, -0.0508251, -0.0937586, -0.801578, 0.588302, 120, 255, 1),
(225731, 1116, 6662, 6947, 1, 1, 0, 3814.42, 2524.9, 5.1911, 5.9321, -0.0281076, 0.0531616, -0.172595, 0.983155, 120, 255, 1),
(225731, 1116, 6662, 6947, 1, 1, 0, 3832.8, 2580.43, 2.90544, 6.2084, -0.0445185, 0.0881538, -0.0330563, 0.994562, 120, 255, 1),
(225731, 1116, 6662, 6947, 1, 1, 0, 3836.7, 2501.89, 3.31956, 3.49797, -0.070725, -0.019928, -0.981414, 0.177279, 120, 255, 1),
(225731, 1116, 6662, 6947, 1, 1, 0, 3884.68, 2481.07, 4.20775, 1.1857, -0.122736, 0.0610199, 0.560191, 0.816944, 120, 255, 1),
(225731, 1116, 6662, 6947, 1, 1, 0, 3917.32, 2431.89, 8.4828, 3.52188, -0.0767074, -0.114882, -0.970696, 0.196638, 120, 255, 1),
(225731, 1116, 6662, 6947, 1, 1, 0, 3936.2, 2465.32, 3.92548, 5.18443, -0.126393, -0.0509424, -0.524916, 0.840175, 120, 255, 1),
(225731, 1116, 6662, 6947, 1, 1, 0, 3958.29, 2496.06, -0.234878, 4.37873, -0.00451183, -0.0201807, -0.814412, 0.579918, 120, 255, 1),
(225731, 1116, 6662, 6947, 1, 1, 0, 3995.21, 2498.27, 4.01549, 1.88263, -0.0177026, 0.0150938, 0.808091, 0.588598, 120, 255, 1),
(225731, 1116, 6662, 6947, 1, 1, 0, 4019.94, 2506.92, 4.36285, 2.35428, -0.0383882, -0.0338688, 0.922699, 0.382107, 120, 255, 1),
(225731, 1116, 6662, 6947, 1, 1, 0, 4032.35, 2547.57, 1.85431, 1.72379, 0, 0, 0.759077, 0.651001, 120, 255, 1),
(225731, 1116, 6662, 6947, 1, 1, 0, 4054.05, 2574.14, 3.13013, 4.16321, 0, 0, -0.872351, 0.488881, 120, 255, 1);

delete from creature where id = 75808 and position_x like '3628%';
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(75808, 1116, 6662, 6662, 1, 1, 0, 0, 0, 3628.5940, 2318.4004, 67.1339, 3.8, 120, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);


-- https://forum.wowcircle.com/showthread.php?t=1052065
delete from world_quest_template where entry in (42023,41896);
INSERT INTO `world_quest_template` (`entry`, `overrideAreaGroupID`, `areaID`, `enabled`, `overrideVariableID`, `overrideVariableValue`) VALUES 
(41896, -1, 0, 1, 0, 0),
(42023, -1, 0, 1, 0, 0);

delete from world_quest_loot where quest_id in (42023,41896);
INSERT INTO `world_quest_loot` (`quest_id`, `group_id`, `chance`) VALUES 
(41896, 3, 0),
(42023, 3, 0);

update quest_template set flags = 35193088 where id in (42023,41896);