

alter table achievement_reward 
add column learnSpell int(10) unsigned not null default 0 after mailTemplate,
add column castSpell int(10) unsigned not null default 0 after learnSpell;

-- https://ru.wowhead.com/achievement=12103
delete from achievement_reward where entry = 12103;
INSERT INTO `achievement_reward` (`entry`, `title_A`, `title_H`, `item`, `sender`, `subject`, `text`, `mailTemplate`, `learnSpell`, `castSpell`) VALUES
('12103', '0', '0', '0', '0', NULL, NULL, '0', '254260', '0');

-- Void Bolt Auras Update Time
delete from spell_script_names where spell_id=234746;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES 
('234746', 'spell_pri_void_bolt_missile');

-- Antorus: Kin'garoth: Sphere activation delay
UPDATE `areatrigger_data` SET `waitTime`='1500', `activationDelay`='1500' WHERE (`entry`='10886') AND (`spellId`='246746') AND (`customEntry`='0');


-- ToT: Teleport to Tortos
delete from creature where id = 8445001;
delete from creature_template where entry = 8445001;
delete from creature_template_wdb where entry = 8445001;

delete from areatrigger_scripts where entry = 8937;
INSERT INTO `areatrigger_scripts` (`entry`, `ScriptName`) VALUES
('8937', 'AreaTrigger_at_tot_tortos_lair_teleport');

-- Reward for acheive = 12083
delete from achievement_reward where entry = 12083;
INSERT INTO `achievement_reward` (`entry`, `title_A`, `title_H`, `item`, `sender`, `subject`, `text`, `mailTemplate`) VALUES
('12083', '515', '515', '0', '0', NULL, NULL, '0');
