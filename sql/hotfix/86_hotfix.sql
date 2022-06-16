-- https://forum.wowcircle.com/showthread.php?t=848033
-- https://forum.wowcircle.com/showthread.php?t=848039
-- https://forum.wowcircle.com/showthread.php?t=880773
-- https://forum.wowcircle.com/showthread.php?t=848040
delete from retail_item_sparse where ROW_ID in (104312,104289,136851,101538);
INSERT INTO `retail_item_sparse` (`ROW_ID`, `AllowableRace`, `Name`, `Name2`, `Name3`, `Name4`, `Description`, `Flags`, `Flags2`, `Flags3`, `Flags4`, `PriceRandomValue`, `PriceVariance`, `BuyCount`, `BuyPrice`, `SellPrice`, `RequiredSpell`, `MaxCount`, `Stackable`, `ItemStatAllocation1`, `ItemStatAllocation2`, `ItemStatAllocation3`, `ItemStatAllocation4`, `ItemStatAllocation5`, `ItemStatAllocation6`, `ItemStatAllocation7`, `ItemStatAllocation8`, `ItemStatAllocation9`, `ItemStatAllocation10`, `ItemStatSocketCostMultiplier1`, `ItemStatSocketCostMultiplier2`, `ItemStatSocketCostMultiplier3`, `ItemStatSocketCostMultiplier4`, `ItemStatSocketCostMultiplier5`, `ItemStatSocketCostMultiplier6`, `ItemStatSocketCostMultiplier7`, `ItemStatSocketCostMultiplier8`, `ItemStatSocketCostMultiplier9`, `ItemStatSocketCostMultiplier10`, `RangedModRange`, `BagFamily`, `ArmorDamageModifier`, `Duration`, `StatScalingFactor`, `AllowableClass`, `ItemLevel`, `RequiredSkill`, `RequiredSkillRank`, `RequiredReputationFaction`, `ItemStatValue1`, `ItemStatValue2`, `ItemStatValue3`, `ItemStatValue4`, `ItemStatValue5`, `ItemStatValue6`, `ItemStatValue7`, `ItemStatValue8`, `ItemStatValue9`, `ItemStatValue10`, `ScalingStatDistribution`, `Delay`, `PageText`, `StartQuest`, `LockID`, `RandomProperty`, `RandomSuffix`, `ItemSet`, `Area`, `Map`, `TotemCategory`, `SocketBonus`, `GemProperties`, `ItemLimitCategory`, `HolidayId`, `RequiredTransmogHolidayID`, `ItemNameDescriptionID`, `Quality`, `InventoryType`, `RequiredLevel`, `RequiredHonorRank`, `RequiredCityRank`, `RequiredReputationRank`, `ContainerSlots`, `ItemStatType1`, `ItemStatType2`, `ItemStatType3`, `ItemStatType4`, `ItemStatType5`, `ItemStatType6`, `ItemStatType7`, `ItemStatType8`, `ItemStatType9`, `ItemStatType10`, `DamageType`, `Bonding`, `LanguageID`, `PageMaterial`, `Material`, `Sheath`, `SocketColor1`, `SocketColor2`, `SocketColor3`, `CurrencySubstitutionId`, `CurrencySubstitutionCount`, `ArtifactID`, `RequiredExpansion`) VALUES 
(101538, 18446744073709551615, 'Kukuru\'s Cache Key', '', '', '', 'This herb appears to be infused with a natural enchantment.', 65536, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 65535, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6),
(104289, 18446744073709551615, 'Faintly-Glowing Herb', '', '', '', 'This herb appears to be infused with a natural enchantment.', 65536, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 65535, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6),
(104312, 18446744073709551615, 'Strange Glowing Mushroom', '', '', '', 'An odd mushroom found deep within the Cavern of Lost Spirits.', 65536, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 65535, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6),
(136851, 18446744073709551615, 'Commander Domitille\'s Helm', '', '', '', '', 65536, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 65535, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 40762, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6);