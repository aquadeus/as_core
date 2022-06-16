ALTER TABLE `challenge_member`
ADD COLUMN `GuildId`  int(10) NOT NULL DEFAULT 0 AFTER `ChestID`;