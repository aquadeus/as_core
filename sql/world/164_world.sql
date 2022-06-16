

-- https://forum.wowcircle.net/showthread.php?t=1065082

delete from creature_loot_template where item IN (select itemId from item_legendary_available);
delete from gameobject_loot_template where item IN (select itemId from item_legendary_available);
delete from item_loot_template where item IN (select itemId from item_legendary_available);
delete from spell_loot_template where item IN (select itemId from item_legendary_available);


-- Maw of Souls
UPDATE `creature_template` SET `unit_flags`='33588032' WHERE (`entry`='99800');
UPDATE `creature_template` SET `unit_flags`='33588032' WHERE (`entry`='97099');
UPDATE `creature_template` SET `unit_flags`='33588032' WHERE (`entry`='100361');

UPDATE `creature_model_info` SET `BoundingRadius`='26.1861', `CombatReach`='26.75' WHERE (`DisplayID`='66345');
UPDATE `creature_model_info` SET `BoundingRadius`='26.1861', `CombatReach`='26.75' WHERE (`DisplayID`='66619');
UPDATE `creature_model_info` SET `BoundingRadius`='26.1861', `CombatReach`='26.75' WHERE (`DisplayID`='65778');

