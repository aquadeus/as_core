

-- https://ru.wowhead.com/achievement=10587
-- https://forum.wowcircle.net/showthread.php?t=1064683
UPDATE `spell_scene` SET `ScriptName`='sceneTrigger_jewelcraft_game' WHERE (`SceneScriptPackageID`='1407') AND (`MiscValue`='1126');


-- https://ru.wowhead.com/npc=97097 заскриптован нпц
-- https://forum.wowcircle.net/showthread.php?t=1062169
UPDATE `creature_template` SET `AIName`='', `mechanic_immune_mask`='617299967', `ScriptName`='npc_helarjar_champion_mos' WHERE (`entry`='97097');


-- https://ru.wowhead.com/npc=97185 Переписан нпц. Теперь во время каста дыхания нпц не будет поворачиваться за целью.
UPDATE `creature_template` SET `AIName`='', `ScriptName`='npc_grimewalker_mos' WHERE (`entry`='97185');

-- 
UPDATE `creature_template` SET `mechanic_immune_mask`='642462671' WHERE (`entry`='98761');

-- https://ru.wowhead.com/npc=97365 заскриптован нпц, реализованы касты
UPDATE `creature_template` SET `AIName`='', `ScriptName`='npc_seacursed_mistmender_mos' WHERE (`entry`='97365');

delete from spell_script_names where spell_id = 199514;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES ('199514', 'spell_mos_torment_of_souls');