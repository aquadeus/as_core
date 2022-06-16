
-- https://ru.wowhead.com/item=133579
UPDATE `gameobject_template` SET `data0`='201600' WHERE (`entry`='246434');

delete from spell_script_names where spell_id=201644;
delete from spell_script_names where spell_id=201600;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES 
('201600', 'spell_item_lavish_suramar_feast_use'),
('201644', 'spell_item_lavish_suramar_feast');

-- Quest NPC fix
UPDATE `creature_template_wdb` SET `QuestItem1`='135511' WHERE (`Entry`='103786');
UPDATE `creature_template_wdb` SET `QuestItem1`='135511' WHERE (`Entry`='103784');
UPDATE `creature_template_wdb` SET `QuestItem1`='135511' WHERE (`Entry`='103787');
UPDATE `creature_template_wdb` SET `QuestItem1`='135511' WHERE (`Entry`='103681');
UPDATE `creature_template_wdb` SET `QuestItem1`='135511' WHERE (`Entry`='103785');