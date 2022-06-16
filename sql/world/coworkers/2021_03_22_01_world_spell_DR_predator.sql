-- Predator
DELETE FROM `spell_script_names` WHERE `spell_id` IN (1079, 155722, 106830, 210723) AND `ScriptName` = "spell_dru_feral_predator";
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(1079, "spell_dru_feral_predator"),
(155722, "spell_dru_feral_predator"),
(106830, "spell_dru_feral_predator"),
(210723, "spell_dru_feral_predator");

DELETE FROM `spell_script_names` WHERE `ScriptName` = "PlayerScript_predator";