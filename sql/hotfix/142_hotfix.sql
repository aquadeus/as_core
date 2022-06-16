update retail_item_sparse set AllowableRace=100000 where AllowableRace=18446744073709551615;

ALTER TABLE `retail_item_sparse`
MODIFY COLUMN `AllowableRace`  bigint(20) NOT NULL DEFAULT 0 ,
MODIFY COLUMN `name`  text CHARACTER SET utf8 COLLATE utf8_general_ci,
MODIFY COLUMN `name2`  text CHARACTER SET utf8 COLLATE utf8_general_ci,
MODIFY COLUMN `name3`  text CHARACTER SET utf8 COLLATE utf8_general_ci,
MODIFY COLUMN `name4`  text CHARACTER SET utf8 COLLATE utf8_general_ci,
MODIFY COLUMN `Description`  text CHARACTER SET utf8 COLLATE utf8_general_ci;

update retail_item_sparse set AllowableRace=-1 where AllowableRace=100000;

-- https://ru.wowhead.com/item=137295
delete from retail_item_sparse where ROW_ID = 137295;
insert INTO `retail_item_sparse` (`ROW_ID`, `AllowableRace`, `name`, `name2`, `name3`, `name4`, `Description`, `Flags`, `Flags2`, `Flags3`, `Flags4`, `PriceRandomValue`, `PriceVariance`, `BuyCount`, `BuyPrice`, `SellPrice`, `RequiredSpell`, `MaxCount`, `Stackable`, `ItemStatAllocation1`, `ItemStatAllocation2`, `ItemStatAllocation3`, `ItemStatAllocation4`, `ItemStatAllocation5`, `ItemStatAllocation6`, `ItemStatAllocation7`, `ItemStatAllocation8`, `ItemStatAllocation9`, `ItemStatAllocation10`, `ItemStatSocketCostMultiplier1`, `ItemStatSocketCostMultiplier2`, `ItemStatSocketCostMultiplier3`, `ItemStatSocketCostMultiplier4`, `ItemStatSocketCostMultiplier5`, `ItemStatSocketCostMultiplier6`, `ItemStatSocketCostMultiplier7`, `ItemStatSocketCostMultiplier8`, `ItemStatSocketCostMultiplier9`, `ItemStatSocketCostMultiplier10`, `RangedModRange`, `BagFamily`, `ArmorDamageModifier`, `Duration`, `StatScalingFactor`, `AllowableClass`, `ItemLevel`, `RequiredSkill`, `RequiredSkillRank`, `RequiredReputationFaction`, `ItemStatValue1`, `ItemStatValue2`, `ItemStatValue3`, `ItemStatValue4`, `ItemStatValue5`, `ItemStatValue6`, `ItemStatValue7`, `ItemStatValue8`, `ItemStatValue9`, `ItemStatValue10`, `ScalingStatDistribution`, `Delay`, `PageText`, `StartQuest`, `LockID`, `RandomProperty`, `RandomSuffix`, `ItemSet`, `Area`, `Map`, `TotemCategory`, `SocketBonus`, `GemProperties`, `ItemLimitCategory`, `HolidayId`, `RequiredTransmogHolidayID`, `ItemNameDescriptionID`, `Quality`, `InventoryType`, `RequiredLevel`, `RequiredHonorRank`, `RequiredCityRank`, `RequiredReputationRank`, `ContainerSlots`, `ItemStatType1`, `ItemStatType2`, `ItemStatType3`, `ItemStatType4`, `ItemStatType5`, `ItemStatType6`, `ItemStatType7`, `ItemStatType8`, `ItemStatType9`, `ItemStatType10`, `DamageType`, `Bonding`, `LanguageID`, `PageMaterial`, `Material`, `Sheath`, `SocketColor1`, `SocketColor2`, `SocketColor3`, `CurrencySubstitutionId`, `CurrencySubstitutionCount`, `ArtifactID`, `RequiredExpansion`, `VerifiedBuild`) VALUES
('137295', -1, 'Бутылка чародейского вина', '', '', '', '', '65536', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '20', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '65535', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '4', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0');

delete from _hotfixs where Entry=137295 and Hash = 2442913102;
INSERT INTO `_hotfixs` (`ID`, `Entry`, `Hash`, `Date`, `Comment`) VALUES
('13003', 137295, '2442913102', UNIX_TIMESTAMP(), '');