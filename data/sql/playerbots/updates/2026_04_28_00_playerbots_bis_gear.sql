-- BiS gear table for the /p bis party-chat command. Unified across tiers.
-- Lookup keys: (class, tab, slot, faction, auto_gear_score_limit).
-- faction: 0=Both, 1=Alliance, 2=Horde. Faction-specific overrides Both.
-- auto_gear_score_limit: matched exactly against AiPlayerbot.AutoGearScoreLimit.
-- phase: free-text label for human readability (e.g. 'ICC', 'Pre-Raid').
-- Slot uses AzerothCore EquipmentSlots enum:
-- head=0, neck=1, shoulders=2, chest=4, waist=5, legs=6, feet=7, wrists=8, hands=9,
-- finger1=10, finger2=11, trinket1=12, trinket2=13, back=14, mainhand=15, offhand=16, ranged=17.
-- Druid Bear lives under sentinel tab=10 (resolved at runtime when bot has Tank strategy).

DROP TABLE IF EXISTS `playerbots_bis_gear`;
DROP TABLE IF EXISTS `playerbots_bis`;
CREATE TABLE `playerbots_bis` (
    `class`      TINYINT UNSIGNED NOT NULL,
    `tab`        TINYINT UNSIGNED NOT NULL,
    `slot`       TINYINT UNSIGNED NOT NULL,
    `faction`    TINYINT UNSIGNED NOT NULL DEFAULT 0,
    `auto_gear_score_limit` SMALLINT UNSIGNED NOT NULL,
    `item_id`       INT UNSIGNED NOT NULL,
    `phase`      VARCHAR(32) NOT NULL DEFAULT '',
    `class_name`   VARCHAR(16) NOT NULL,
    `spec_name`    VARCHAR(32) NOT NULL,
    `slot_name`    VARCHAR(16) NOT NULL,
    `faction_name` VARCHAR(8)  NOT NULL DEFAULT 'Both',
    `item_name`    VARCHAR(96) NOT NULL,
    PRIMARY KEY (`class`, `tab`, `slot`, `faction`, `auto_gear_score_limit`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ============================================================
-- Warrior (1)
-- ============================================================
-- Arms (tab 0) - 2H Shadowmourne
INSERT INTO `playerbots_bis` VALUES
(1, 0, 0, 0, 290, 51227, 'ICC', 'Warrior', 'Arms', 'Head', 'Both', 'Sanctified Ymirjar Lord''s Helmet'),
(1, 0, 1, 0, 290, 54581, 'ICC', 'Warrior', 'Arms', 'Neck', 'Both', 'Penumbra Pendant'),
(1, 0, 2, 0, 290, 51229, 'ICC', 'Warrior', 'Arms', 'Shoulders', 'Both', 'Sanctified Ymirjar Lord''s Shoulderplates'),
(1, 0, 4, 0, 290, 51225, 'ICC', 'Warrior', 'Arms', 'Chest', 'Both', 'Sanctified Ymirjar Lord''s Battleplate'),
(1, 0, 5, 0, 290, 50620, 'ICC', 'Warrior', 'Arms', 'Waist', 'Both', 'Coldwraith Links'),
(1, 0, 6, 0, 290, 51228, 'ICC', 'Warrior', 'Arms', 'Legs', 'Both', 'Sanctified Ymirjar Lord''s Legplates'),
(1, 0, 7, 0, 290, 54578, 'ICC', 'Warrior', 'Arms', 'Feet', 'Both', 'Apocalypse''s Advance'),
(1, 0, 8, 0, 290, 50670, 'ICC', 'Warrior', 'Arms', 'Wrists', 'Both', 'Toskk''s Maximized Wristguards'),
(1, 0, 9, 0, 290, 50675, 'ICC', 'Warrior', 'Arms', 'Hands', 'Both', 'Aldriana''s Gloves of Secrecy'),
(1, 0, 10, 0, 290, 50657, 'ICC', 'Warrior', 'Arms', 'Finger1', 'Both', 'Skeleton Lord''s Circle'),
(1, 0, 11, 0, 290, 52572, 'ICC', 'Warrior', 'Arms', 'Finger2', 'Both', 'Ashen Band of Endless Might'),
(1, 0, 12, 0, 290, 50363, 'ICC', 'Warrior', 'Arms', 'Trinket1', 'Both', 'Deathbringer''s Will'),
(1, 0, 13, 0, 290, 54590, 'ICC', 'Warrior', 'Arms', 'Trinket2', 'Both', 'Sharpened Twilight Scale'),
(1, 0, 14, 0, 290, 47545, 'ICC', 'Warrior', 'Arms', 'Back', 'Both', 'Vereesa''s Dexterity'),
(1, 0, 15, 0, 290, 49623, 'ICC', 'Warrior', 'Arms', 'MainHand', 'Both', 'Shadowmourne'),
(1, 0, 17, 0, 290, 50733, 'ICC', 'Warrior', 'Arms', 'Ranged', 'Both', 'Fal''inrush, Defender of Quel''thalas');

-- Fury (tab 1) - DW Cryptmaker
INSERT INTO `playerbots_bis` VALUES
(1, 1, 0, 0, 290, 51227, 'ICC', 'Warrior', 'Fury', 'Head', 'Both', 'Sanctified Ymirjar Lord''s Helmet'),
(1, 1, 1, 0, 290, 54581, 'ICC', 'Warrior', 'Fury', 'Neck', 'Both', 'Penumbra Pendant'),
(1, 1, 2, 0, 290, 51229, 'ICC', 'Warrior', 'Fury', 'Shoulders', 'Both', 'Sanctified Ymirjar Lord''s Shoulderplates'),
(1, 1, 4, 0, 290, 51225, 'ICC', 'Warrior', 'Fury', 'Chest', 'Both', 'Sanctified Ymirjar Lord''s Battleplate'),
(1, 1, 5, 0, 290, 50620, 'ICC', 'Warrior', 'Fury', 'Waist', 'Both', 'Coldwraith Links'),
(1, 1, 6, 0, 290, 51228, 'ICC', 'Warrior', 'Fury', 'Legs', 'Both', 'Sanctified Ymirjar Lord''s Legplates'),
(1, 1, 7, 0, 290, 54578, 'ICC', 'Warrior', 'Fury', 'Feet', 'Both', 'Apocalypse''s Advance'),
(1, 1, 8, 0, 290, 50670, 'ICC', 'Warrior', 'Fury', 'Wrists', 'Both', 'Toskk''s Maximized Wristguards'),
(1, 1, 9, 0, 290, 50675, 'ICC', 'Warrior', 'Fury', 'Hands', 'Both', 'Aldriana''s Gloves of Secrecy'),
(1, 1, 10, 0, 290, 50657, 'ICC', 'Warrior', 'Fury', 'Finger1', 'Both', 'Skeleton Lord''s Circle'),
(1, 1, 11, 0, 290, 52572, 'ICC', 'Warrior', 'Fury', 'Finger2', 'Both', 'Ashen Band of Endless Might'),
(1, 1, 12, 0, 290, 50363, 'ICC', 'Warrior', 'Fury', 'Trinket1', 'Both', 'Deathbringer''s Will'),
(1, 1, 13, 0, 290, 54590, 'ICC', 'Warrior', 'Fury', 'Trinket2', 'Both', 'Sharpened Twilight Scale'),
(1, 1, 14, 0, 290, 47545, 'ICC', 'Warrior', 'Fury', 'Back', 'Both', 'Vereesa''s Dexterity'),
(1, 1, 15, 0, 290, 50603, 'ICC', 'Warrior', 'Fury', 'MainHand', 'Both', 'Cryptmaker'),
(1, 1, 16, 0, 290, 50603, 'ICC', 'Warrior', 'Fury', 'OffHand', 'Both', 'Cryptmaker'),
(1, 1, 17, 0, 290, 50733, 'ICC', 'Warrior', 'Fury', 'Ranged', 'Both', 'Fal''inrush, Defender of Quel''thalas');

-- Protection (tab 2)
INSERT INTO `playerbots_bis` VALUES
(1, 2, 0, 0, 290, 50640, 'ICC', 'Warrior', 'Protection', 'Head', 'Both', 'Broken Ram Skull Helm'),
(1, 2, 1, 0, 290, 50682, 'ICC', 'Warrior', 'Protection', 'Neck', 'Both', 'Bile-Encrusted Medallion'),
(1, 2, 2, 0, 290, 51847, 'ICC', 'Warrior', 'Protection', 'Shoulders', 'Both', 'Spaulders of the Blood Princes'),
(1, 2, 4, 0, 290, 51220, 'ICC', 'Warrior', 'Protection', 'Chest', 'Both', 'Sanctified Ymirjar Lord''s Breastplate'),
(1, 2, 5, 0, 290, 50691, 'ICC', 'Warrior', 'Protection', 'Waist', 'Both', 'Belt of Broken Bones'),
(1, 2, 6, 0, 290, 50612, 'ICC', 'Warrior', 'Protection', 'Legs', 'Both', 'Legguards of Lost Hope'),
(1, 2, 7, 0, 290, 54579, 'ICC', 'Warrior', 'Protection', 'Feet', 'Both', 'Treads of Impending Resurrection'),
(1, 2, 8, 0, 290, 51901, 'ICC', 'Warrior', 'Protection', 'Wrists', 'Both', 'Gargoyle Spit Bracers'),
(1, 2, 9, 0, 290, 51222, 'ICC', 'Warrior', 'Protection', 'Hands', 'Both', 'Sanctified Ymirjar Lord''s Handguards'),
(1, 2, 10, 0, 290, 50622, 'ICC', 'Warrior', 'Protection', 'Finger1', 'Both', 'Devium''s Eternally Cold Ring'),
(1, 2, 11, 0, 290, 50404, 'ICC', 'Warrior', 'Protection', 'Finger2', 'Both', 'Ashen Band of Endless Courage'),
(1, 2, 12, 0, 290, 54591, 'ICC', 'Warrior', 'Protection', 'Trinket1', 'Both', 'Petrified Twilight Scale'),
(1, 2, 13, 0, 290, 50364, 'ICC', 'Warrior', 'Protection', 'Trinket2', 'Both', 'Sindragosa''s Flawless Fang'),
(1, 2, 14, 0, 290, 50466, 'ICC', 'Warrior', 'Protection', 'Back', 'Both', 'Sentinel''s Winter Cloak'),
(1, 2, 15, 0, 290, 50738, 'ICC', 'Warrior', 'Protection', 'MainHand', 'Both', 'Mithrios, Bronzebeard''s Legacy'),
(1, 2, 16, 0, 290, 50729, 'ICC', 'Warrior', 'Protection', 'OffHand', 'Both', 'Icecrown Glacial Wall'),
(1, 2, 17, 0, 290, 51834, 'ICC', 'Warrior', 'Protection', 'Ranged', 'Both', 'Dreamhunter''s Carbine');

-- ============================================================
-- Paladin (2)
-- ============================================================
-- Holy (tab 0)
INSERT INTO `playerbots_bis` VALUES
(2, 0, 0, 0, 290, 51272, 'ICC', 'Paladin', 'Holy', 'Head', 'Both', 'Sanctified Lightsworn Headpiece'),
(2, 0, 1, 0, 290, 50182, 'ICC', 'Paladin', 'Holy', 'Neck', 'Both', 'Blood Queen''s Crimson Choker'),
(2, 0, 2, 0, 290, 51273, 'ICC', 'Paladin', 'Holy', 'Shoulders', 'Both', 'Sanctified Lightsworn Spaulders'),
(2, 0, 4, 0, 290, 50680, 'ICC', 'Paladin', 'Holy', 'Chest', 'Both', 'Rot-Resistant Breastplate'),
(2, 0, 5, 0, 290, 54587, 'ICC', 'Paladin', 'Holy', 'Waist', 'Both', 'Split Shape Belt'),
(2, 0, 6, 0, 290, 50694, 'ICC', 'Paladin', 'Holy', 'Legs', 'Both', 'Plaguebringer''s Stained Pants'),
(2, 0, 7, 0, 290, 54586, 'ICC', 'Paladin', 'Holy', 'Feet', 'Both', 'Foreshadow Steps'),
(2, 0, 8, 0, 290, 54582, 'ICC', 'Paladin', 'Holy', 'Wrists', 'Both', 'Bracers of Fiery Night'),
(2, 0, 9, 0, 290, 50650, 'ICC', 'Paladin', 'Holy', 'Hands', 'Both', 'Fallen Lord''s Handguards'),
(2, 0, 10, 0, 290, 50664, 'ICC', 'Paladin', 'Holy', 'Finger1', 'Both', 'Ring of Rapid Ascent'),
(2, 0, 11, 0, 290, 50400, 'ICC', 'Paladin', 'Holy', 'Finger2', 'Both', 'Ashen Band of Endless Wisdom'),
(2, 0, 12, 0, 290, 46051, 'ICC', 'Paladin', 'Holy', 'Trinket1', 'Both', 'Meteorite Crystal'),
(2, 0, 13, 0, 290, 48724, 'ICC', 'Paladin', 'Holy', 'Trinket2', 'Both', 'Talisman of Resurgence'),
(2, 0, 14, 0, 290, 54583, 'ICC', 'Paladin', 'Holy', 'Back', 'Both', 'Cloak of Burning Dusk'),
(2, 0, 15, 0, 290, 46017, 'ICC', 'Paladin', 'Holy', 'MainHand', 'Both', 'Val''anyr, Hammer of Ancient Kings'),
(2, 0, 16, 0, 290, 50616, 'ICC', 'Paladin', 'Holy', 'OffHand', 'Both', 'Bulwark of Smouldering Steel'),
(2, 0, 17, 0, 290, 40705, 'ICC', 'Paladin', 'Holy', 'Ranged', 'Both', 'Libram of Renewal');

-- Protection (tab 1)
INSERT INTO `playerbots_bis` VALUES
(2, 1, 0, 0, 290, 50640, 'ICC', 'Paladin', 'Protection', 'Head', 'Both', 'Broken Ram Skull Helm'),
(2, 1, 1, 0, 290, 50682, 'ICC', 'Paladin', 'Protection', 'Neck', 'Both', 'Bile-Encrusted Medallion'),
(2, 1, 2, 0, 290, 50660, 'ICC', 'Paladin', 'Protection', 'Shoulders', 'Both', 'Boneguard Commander''s Pauldrons'),
(2, 1, 4, 0, 290, 51265, 'ICC', 'Paladin', 'Protection', 'Chest', 'Both', 'Sanctified Lightsworn Chestguard'),
(2, 1, 5, 0, 290, 50991, 'ICC', 'Paladin', 'Protection', 'Waist', 'Both', 'Verdigris Chain Belt'),
(2, 1, 6, 0, 290, 49904, 'ICC', 'Paladin', 'Protection', 'Legs', 'Both', 'Pillars of Might'),
(2, 1, 7, 0, 290, 54579, 'ICC', 'Paladin', 'Protection', 'Feet', 'Both', 'Treads of Impending Resurrection'),
(2, 1, 8, 0, 290, 51901, 'ICC', 'Paladin', 'Protection', 'Wrists', 'Both', 'Gargoyle Spit Bracers'),
(2, 1, 9, 0, 290, 51267, 'ICC', 'Paladin', 'Protection', 'Hands', 'Both', 'Sanctified Lightsworn Handguards'),
(2, 1, 10, 0, 290, 50622, 'ICC', 'Paladin', 'Protection', 'Finger1', 'Both', 'Devium''s Eternally Cold Ring'),
(2, 1, 11, 0, 290, 50404, 'ICC', 'Paladin', 'Protection', 'Finger2', 'Both', 'Ashen Band of Endless Courage'),
(2, 1, 12, 0, 290, 54591, 'ICC', 'Paladin', 'Protection', 'Trinket1', 'Both', 'Petrified Twilight Scale'),
(2, 1, 13, 0, 290, 50364, 'ICC', 'Paladin', 'Protection', 'Trinket2', 'Both', 'Sindragosa''s Flawless Fang'),
(2, 1, 14, 0, 290, 50466, 'ICC', 'Paladin', 'Protection', 'Back', 'Both', 'Sentinel''s Winter Cloak'),
(2, 1, 15, 0, 290, 50738, 'ICC', 'Paladin', 'Protection', 'MainHand', 'Both', 'Mithrios, Bronzebeard''s Legacy'),
(2, 1, 16, 0, 290, 50729, 'ICC', 'Paladin', 'Protection', 'OffHand', 'Both', 'Icecrown Glacial Wall'),
(2, 1, 17, 0, 290, 50461, 'ICC', 'Paladin', 'Protection', 'Ranged', 'Both', 'Libram of the Eternal Tower');

-- Retribution (tab 2)
INSERT INTO `playerbots_bis` VALUES
(2, 2, 0, 0, 290, 51277, 'ICC', 'Paladin', 'Retribution', 'Head', 'Both', 'Sanctified Lightsworn Helmet'),
(2, 2, 1, 0, 290, 54581, 'ICC', 'Paladin', 'Retribution', 'Neck', 'Both', 'Penumbra Pendant'),
(2, 2, 2, 0, 290, 51279, 'ICC', 'Paladin', 'Retribution', 'Shoulders', 'Both', 'Sanctified Lightsworn Shoulderplates'),
(2, 2, 4, 0, 290, 51275, 'ICC', 'Paladin', 'Retribution', 'Chest', 'Both', 'Sanctified Lightsworn Battleplate'),
(2, 2, 5, 0, 290, 50707, 'ICC', 'Paladin', 'Retribution', 'Waist', 'Both', 'Astrylian''s Sutured Cinch'),
(2, 2, 6, 0, 290, 51278, 'ICC', 'Paladin', 'Retribution', 'Legs', 'Both', 'Sanctified Lightsworn Legplates'),
(2, 2, 7, 0, 290, 54578, 'ICC', 'Paladin', 'Retribution', 'Feet', 'Both', 'Apocalypse''s Advance'),
(2, 2, 8, 0, 290, 54580, 'ICC', 'Paladin', 'Retribution', 'Wrists', 'Both', 'Umbrage Armbands'),
(2, 2, 9, 0, 290, 50690, 'ICC', 'Paladin', 'Retribution', 'Hands', 'Both', 'Fleshrending Gauntlets'),
(2, 2, 10, 0, 290, 50402, 'ICC', 'Paladin', 'Retribution', 'Finger1', 'Both', 'Ashen Band of Endless Vengeance'),
(2, 2, 11, 0, 290, 54576, 'ICC', 'Paladin', 'Retribution', 'Finger2', 'Both', 'Signet of Twilight'),
(2, 2, 12, 0, 290, 54590, 'ICC', 'Paladin', 'Retribution', 'Trinket1', 'Both', 'Sharpened Twilight Scale'),
(2, 2, 13, 0, 290, 50706, 'ICC', 'Paladin', 'Retribution', 'Trinket2', 'Both', 'Tiny Abomination in a Jar'),
(2, 2, 14, 0, 290, 50653, 'ICC', 'Paladin', 'Retribution', 'Back', 'Both', 'Shadowvault Slayer''s Cloak'),
(2, 2, 15, 0, 290, 49623, 'ICC', 'Paladin', 'Retribution', 'MainHand', 'Both', 'Shadowmourne'),
(2, 2, 17, 0, 290, 50455, 'ICC', 'Paladin', 'Retribution', 'Ranged', 'Both', 'Libram of Three Truths');

-- ============================================================
-- Hunter (3) - Marksmanship (tab 1)
-- ============================================================
INSERT INTO `playerbots_bis` VALUES
(3, 1, 0, 0, 290, 51286, 'ICC', 'Hunter', 'Marksmanship', 'Head', 'Both', 'Sanctified Ahn''Kahar Blood Hunter''s Headpiece'),
(3, 1, 1, 0, 290, 50633, 'ICC', 'Hunter', 'Marksmanship', 'Neck', 'Both', 'Sindragosa''s Cruel Claw'),
(3, 1, 2, 0, 290, 51288, 'ICC', 'Hunter', 'Marksmanship', 'Shoulders', 'Both', 'Sanctified Ahn''Kahar Blood Hunter''s Spaulders'),
(3, 1, 4, 0, 290, 51289, 'ICC', 'Hunter', 'Marksmanship', 'Chest', 'Both', 'Sanctified Ahn''Kahar Blood Hunter''s Tunic'),
(3, 1, 5, 0, 290, 50688, 'ICC', 'Hunter', 'Marksmanship', 'Waist', 'Both', 'Nerub''ar Stalker''s Cord'),
(3, 1, 6, 0, 290, 50645, 'ICC', 'Hunter', 'Marksmanship', 'Legs', 'Both', 'Leggings of Northern Lights'),
(3, 1, 7, 0, 290, 54577, 'ICC', 'Hunter', 'Marksmanship', 'Feet', 'Both', 'Returning Footfalls'),
(3, 1, 8, 0, 290, 50655, 'ICC', 'Hunter', 'Marksmanship', 'Wrists', 'Both', 'Scourge Hunter''s Vambraces'),
(3, 1, 9, 0, 290, 51285, 'ICC', 'Hunter', 'Marksmanship', 'Hands', 'Both', 'Sanctified Ahn''Kahar Blood Hunter''s Handguards'),
(3, 1, 10, 0, 290, 54576, 'ICC', 'Hunter', 'Marksmanship', 'Finger1', 'Both', 'Signet of Twilight'),
(3, 1, 11, 0, 290, 50402, 'ICC', 'Hunter', 'Marksmanship', 'Finger2', 'Both', 'Ashen Band of Endless Vengeance'),
(3, 1, 12, 0, 290, 50363, 'ICC', 'Hunter', 'Marksmanship', 'Trinket1', 'Both', 'Deathbringer''s Will'),
(3, 1, 13, 0, 290, 54590, 'ICC', 'Hunter', 'Marksmanship', 'Trinket2', 'Both', 'Sharpened Twilight Scale'),
(3, 1, 14, 0, 290, 47545, 'ICC', 'Hunter', 'Marksmanship', 'Back', 'Both', 'Vereesa''s Dexterity'),
(3, 1, 15, 0, 290, 50735, 'ICC', 'Hunter', 'Marksmanship', 'MainHand', 'Both', 'Oathbinder, Charge of the Ranger-General'),
(3, 1, 17, 0, 290, 50733, 'ICC', 'Hunter', 'Marksmanship', 'Ranged', 'Both', 'Fal''inrush, Defender of Quel''thalas');

-- ============================================================
-- Rogue (4)
-- ============================================================
-- Assassination (tab 0)
INSERT INTO `playerbots_bis` VALUES
(4, 0, 0, 0, 290, 50713, 'ICC', 'Rogue', 'Assassination', 'Head', 'Both', 'Geistlord''s Punishment Sack'),
(4, 0, 1, 0, 290, 50633, 'ICC', 'Rogue', 'Assassination', 'Neck', 'Both', 'Sindragosa''s Cruel Claw'),
(4, 0, 2, 0, 290, 50646, 'ICC', 'Rogue', 'Assassination', 'Shoulders', 'Both', 'Cultist''s Bloodsoaked Spaulders'),
(4, 0, 4, 0, 290, 50656, 'ICC', 'Rogue', 'Assassination', 'Chest', 'Both', 'Ikfirus''s Sack of Wonder'),
(4, 0, 5, 0, 290, 47112, 'ICC', 'Rogue', 'Assassination', 'Waist', 'Both', 'Belt of the Merciless Killer'),
(4, 0, 6, 0, 290, 51253, 'ICC', 'Rogue', 'Assassination', 'Legs', 'Both', 'Sanctified Shadowblade Legplates'),
(4, 0, 7, 0, 290, 50607, 'ICC', 'Rogue', 'Assassination', 'Feet', 'Both', 'Frostbitten Fur Boots'),
(4, 0, 8, 0, 290, 54580, 'ICC', 'Rogue', 'Assassination', 'Wrists', 'Both', 'Umbrage Armbands'),
(4, 0, 9, 0, 290, 51251, 'ICC', 'Rogue', 'Assassination', 'Hands', 'Both', 'Sanctified Shadowblade Gauntlets'),
(4, 0, 10, 0, 290, 50402, 'ICC', 'Rogue', 'Assassination', 'Finger1', 'Both', 'Ashen Band of Endless Vengeance'),
(4, 0, 11, 0, 290, 54576, 'ICC', 'Rogue', 'Assassination', 'Finger2', 'Both', 'Signet of Twilight'),
(4, 0, 12, 0, 290, 54590, 'ICC', 'Rogue', 'Assassination', 'Trinket1', 'Both', 'Sharpened Twilight Scale'),
(4, 0, 13, 0, 290, 50706, 'ICC', 'Rogue', 'Assassination', 'Trinket2', 'Both', 'Tiny Abomination in a Jar'),
(4, 0, 14, 0, 290, 50653, 'ICC', 'Rogue', 'Assassination', 'Back', 'Both', 'Shadowvault Slayer''s Cloak'),
(4, 0, 15, 0, 290, 50736, 'ICC', 'Rogue', 'Assassination', 'MainHand', 'Both', 'Heaven''s Fall, Kryss of a Thousand Lies'),
(4, 0, 16, 0, 290, 50621, 'ICC', 'Rogue', 'Assassination', 'OffHand', 'Both', 'Lungbreaker'),
(4, 0, 17, 0, 290, 50733, 'ICC', 'Rogue', 'Assassination', 'Ranged', 'Both', 'Fal''inrush, Defender of Quel''thalas');

-- Combat (tab 1)
INSERT INTO `playerbots_bis` VALUES
(4, 1, 0, 0, 290, 51252, 'ICC', 'Rogue', 'Combat', 'Head', 'Both', 'Sanctified Shadowblade Helmet'),
(4, 1, 1, 0, 290, 50633, 'ICC', 'Rogue', 'Combat', 'Neck', 'Both', 'Sindragosa''s Cruel Claw'),
(4, 1, 2, 0, 290, 51254, 'ICC', 'Rogue', 'Combat', 'Shoulders', 'Both', 'Sanctified Shadowblade Pauldrons'),
(4, 1, 4, 0, 290, 51250, 'ICC', 'Rogue', 'Combat', 'Chest', 'Both', 'Sanctified Shadowblade Breastplate'),
(4, 1, 5, 0, 290, 50707, 'ICC', 'Rogue', 'Combat', 'Waist', 'Both', 'Astrylian''s Sutured Cinch'),
(4, 1, 6, 0, 290, 51253, 'ICC', 'Rogue', 'Combat', 'Legs', 'Both', 'Sanctified Shadowblade Legplates'),
(4, 1, 7, 0, 290, 50607, 'ICC', 'Rogue', 'Combat', 'Feet', 'Both', 'Frostbitten Fur Boots'),
(4, 1, 8, 0, 290, 50670, 'ICC', 'Rogue', 'Combat', 'Wrists', 'Both', 'Toskk''s Maximized Wristguards'),
(4, 1, 9, 0, 290, 50675, 'ICC', 'Rogue', 'Combat', 'Hands', 'Both', 'Aldriana''s Gloves of Secrecy'),
(4, 1, 10, 0, 290, 50402, 'ICC', 'Rogue', 'Combat', 'Finger1', 'Both', 'Ashen Band of Endless Vengeance'),
(4, 1, 11, 0, 290, 50618, 'ICC', 'Rogue', 'Combat', 'Finger2', 'Both', 'Frostbrood Sapphire Ring'),
(4, 1, 12, 0, 290, 50363, 'ICC', 'Rogue', 'Combat', 'Trinket1', 'Both', 'Deathbringer''s Will'),
(4, 1, 13, 0, 290, 54590, 'ICC', 'Rogue', 'Combat', 'Trinket2', 'Both', 'Sharpened Twilight Scale'),
(4, 1, 14, 0, 290, 47545, 'ICC', 'Rogue', 'Combat', 'Back', 'Both', 'Vereesa''s Dexterity'),
(4, 1, 15, 0, 290, 50737, 'ICC', 'Rogue', 'Combat', 'MainHand', 'Both', 'Havoc''s Call, Blade of Lordaeron Kings'),
(4, 1, 16, 0, 290, 50654, 'ICC', 'Rogue', 'Combat', 'OffHand', 'Both', 'Scourgeborne Waraxe'),
(4, 1, 17, 0, 290, 50733, 'ICC', 'Rogue', 'Combat', 'Ranged', 'Both', 'Fal''inrush, Defender of Quel''thalas');

-- ============================================================
-- Priest (5)
-- ============================================================
-- Discipline (tab 0)
INSERT INTO `playerbots_bis` VALUES
(5, 0, 0, 0, 290, 51261, 'ICC', 'Priest', 'Discipline', 'Head', 'Both', 'Sanctified Crimson Acolyte Hood'),
(5, 0, 1, 0, 290, 50182, 'ICC', 'Priest', 'Discipline', 'Neck', 'Both', 'Blood Queen''s Crimson Choker'),
(5, 0, 2, 0, 290, 51264, 'ICC', 'Priest', 'Discipline', 'Shoulders', 'Both', 'Sanctified Crimson Acolyte Shoulderpads'),
(5, 0, 4, 0, 290, 51263, 'ICC', 'Priest', 'Discipline', 'Chest', 'Both', 'Sanctified Crimson Acolyte Robe'),
(5, 0, 5, 0, 290, 50613, 'ICC', 'Priest', 'Discipline', 'Waist', 'Both', 'Crushing Coldwraith Belt'),
(5, 0, 6, 0, 290, 51262, 'ICC', 'Priest', 'Discipline', 'Legs', 'Both', 'Sanctified Crimson Acolyte Leggings'),
(5, 0, 7, 0, 290, 50699, 'ICC', 'Priest', 'Discipline', 'Feet', 'Both', 'Plague Scientist''s Boots'),
(5, 0, 8, 0, 290, 54582, 'ICC', 'Priest', 'Discipline', 'Wrists', 'Both', 'Bracers of Fiery Night'),
(5, 0, 9, 0, 290, 50722, 'ICC', 'Priest', 'Discipline', 'Hands', 'Both', 'San''layn Ritualist Gloves'),
(5, 0, 10, 0, 290, 50644, 'ICC', 'Priest', 'Discipline', 'Finger1', 'Both', 'Ring of Maddening Whispers'),
(5, 0, 11, 0, 290, 50636, 'ICC', 'Priest', 'Discipline', 'Finger2', 'Both', 'Memory of Malygos'),
(5, 0, 12, 0, 290, 50366, 'ICC', 'Priest', 'Discipline', 'Trinket1', 'Both', 'Althor''s Abacus'),
(5, 0, 13, 0, 290, 54589, 'ICC', 'Priest', 'Discipline', 'Trinket2', 'Both', 'Glowing Twilight Scale'),
(5, 0, 14, 0, 290, 54583, 'ICC', 'Priest', 'Discipline', 'Back', 'Both', 'Cloak of Burning Dusk'),
(5, 0, 15, 0, 290, 50734, 'ICC', 'Priest', 'Discipline', 'MainHand', 'Both', 'Royal Scepter of Terenas II'),
(5, 0, 16, 0, 290, 50719, 'ICC', 'Priest', 'Discipline', 'OffHand', 'Both', 'Shadow Silk Spindle'),
(5, 0, 17, 0, 290, 50631, 'ICC', 'Priest', 'Discipline', 'Ranged', 'Both', 'Nightmare Ender');

-- Holy (tab 1)
INSERT INTO `playerbots_bis` VALUES
(5, 1, 0, 0, 290, 51261, 'ICC', 'Priest', 'Holy', 'Head', 'Both', 'Sanctified Crimson Acolyte Hood'),
(5, 1, 1, 0, 290, 50609, 'ICC', 'Priest', 'Holy', 'Neck', 'Both', 'Bone Sentinel''s Amulet'),
(5, 1, 2, 0, 290, 51264, 'ICC', 'Priest', 'Holy', 'Shoulders', 'Both', 'Sanctified Crimson Acolyte Shoulderpads'),
(5, 1, 4, 0, 290, 50717, 'ICC', 'Priest', 'Holy', 'Chest', 'Both', 'Sanguine Silk Robes'),
(5, 1, 5, 0, 290, 50702, 'ICC', 'Priest', 'Holy', 'Waist', 'Both', 'Lingering Illness'),
(5, 1, 6, 0, 290, 51262, 'ICC', 'Priest', 'Holy', 'Legs', 'Both', 'Sanctified Crimson Acolyte Leggings'),
(5, 1, 7, 0, 290, 50699, 'ICC', 'Priest', 'Holy', 'Feet', 'Both', 'Plague Scientist''s Boots'),
(5, 1, 8, 0, 290, 54582, 'ICC', 'Priest', 'Holy', 'Wrists', 'Both', 'Bracers of Fiery Night'),
(5, 1, 9, 0, 290, 51260, 'ICC', 'Priest', 'Holy', 'Hands', 'Both', 'Sanctified Crimson Acolyte Gloves'),
(5, 1, 10, 0, 290, 50400, 'ICC', 'Priest', 'Holy', 'Finger1', 'Both', 'Ashen Band of Endless Wisdom'),
(5, 1, 11, 0, 290, 50636, 'ICC', 'Priest', 'Holy', 'Finger2', 'Both', 'Memory of Malygos'),
(5, 1, 12, 0, 290, 50366, 'ICC', 'Priest', 'Holy', 'Trinket1', 'Both', 'Althor''s Abacus'),
(5, 1, 13, 0, 290, 54589, 'ICC', 'Priest', 'Holy', 'Trinket2', 'Both', 'Glowing Twilight Scale'),
(5, 1, 14, 0, 290, 50668, 'ICC', 'Priest', 'Holy', 'Back', 'Both', 'Greatcloak of the Turned Champion'),
(5, 1, 15, 0, 290, 50731, 'ICC', 'Priest', 'Holy', 'MainHand', 'Both', 'Archus, Greatstaff of Antonidas'),
(5, 1, 17, 0, 290, 50631, 'ICC', 'Priest', 'Holy', 'Ranged', 'Both', 'Nightmare Ender');

-- Shadow (tab 2)
INSERT INTO `playerbots_bis` VALUES
(5, 2, 0, 0, 290, 51255, 'ICC', 'Priest', 'Shadow', 'Head', 'Both', 'Sanctified Crimson Acolyte Cowl'),
(5, 2, 1, 0, 290, 50182, 'ICC', 'Priest', 'Shadow', 'Neck', 'Both', 'Blood Queen''s Crimson Choker'),
(5, 2, 2, 0, 290, 51257, 'ICC', 'Priest', 'Shadow', 'Shoulders', 'Both', 'Sanctified Crimson Acolyte Mantle'),
(5, 2, 4, 0, 290, 51259, 'ICC', 'Priest', 'Shadow', 'Chest', 'Both', 'Sanctified Crimson Acolyte Raiments'),
(5, 2, 5, 0, 290, 50613, 'ICC', 'Priest', 'Shadow', 'Waist', 'Both', 'Crushing Coldwraith Belt'),
(5, 2, 6, 0, 290, 50694, 'ICC', 'Priest', 'Shadow', 'Legs', 'Both', 'Plaguebringer''s Stained Pants'),
(5, 2, 7, 0, 290, 50699, 'ICC', 'Priest', 'Shadow', 'Feet', 'Both', 'Plague Scientist''s Boots'),
(5, 2, 8, 0, 290, 54582, 'ICC', 'Priest', 'Shadow', 'Wrists', 'Both', 'Bracers of Fiery Night'),
(5, 2, 9, 0, 290, 51256, 'ICC', 'Priest', 'Shadow', 'Hands', 'Both', 'Sanctified Crimson Acolyte Handwraps'),
(5, 2, 10, 0, 290, 50664, 'ICC', 'Priest', 'Shadow', 'Finger1', 'Both', 'Ring of Rapid Ascent'),
(5, 2, 11, 0, 290, 50398, 'ICC', 'Priest', 'Shadow', 'Finger2', 'Both', 'Ashen Band of Endless Destruction'),
(5, 2, 12, 0, 290, 54588, 'ICC', 'Priest', 'Shadow', 'Trinket1', 'Both', 'Charred Twilight Scale'),
(5, 2, 13, 0, 290, 50348, 'ICC', 'Priest', 'Shadow', 'Trinket2', 'Both', 'Dislodged Foreign Object'),
(5, 2, 14, 0, 290, 54583, 'ICC', 'Priest', 'Shadow', 'Back', 'Both', 'Cloak of Burning Dusk'),
(5, 2, 15, 0, 290, 50734, 'ICC', 'Priest', 'Shadow', 'MainHand', 'Both', 'Royal Scepter of Terenas II'),
(5, 2, 16, 0, 290, 50719, 'ICC', 'Priest', 'Shadow', 'OffHand', 'Both', 'Shadow Silk Spindle'),
(5, 2, 17, 0, 290, 50684, 'ICC', 'Priest', 'Shadow', 'Ranged', 'Both', 'Corpse-Impaling Spike');

-- ============================================================
-- Death Knight (6)
-- ============================================================
-- Blood Tank (tab 0)
INSERT INTO `playerbots_bis` VALUES
(6, 0, 0, 0, 290, 51306, 'ICC', 'DeathKnight', 'BloodTank', 'Head', 'Both', 'Sanctified Scourgelord Faceguard'),
(6, 0, 1, 0, 290, 50682, 'ICC', 'DeathKnight', 'BloodTank', 'Neck', 'Both', 'Bile-Encrusted Medallion'),
(6, 0, 2, 0, 290, 51309, 'ICC', 'DeathKnight', 'BloodTank', 'Shoulders', 'Both', 'Sanctified Scourgelord Pauldrons'),
(6, 0, 4, 0, 290, 51305, 'ICC', 'DeathKnight', 'BloodTank', 'Chest', 'Both', 'Sanctified Scourgelord Chestguard'),
(6, 0, 5, 0, 290, 50991, 'ICC', 'DeathKnight', 'BloodTank', 'Waist', 'Both', 'Verdigris Chain Belt'),
(6, 0, 6, 0, 290, 50612, 'ICC', 'DeathKnight', 'BloodTank', 'Legs', 'Both', 'Legguards of Lost Hope'),
(6, 0, 7, 0, 290, 54579, 'ICC', 'DeathKnight', 'BloodTank', 'Feet', 'Both', 'Treads of Impending Resurrection'),
(6, 0, 8, 0, 290, 51901, 'ICC', 'DeathKnight', 'BloodTank', 'Wrists', 'Both', 'Gargoyle Spit Bracers'),
(6, 0, 9, 0, 290, 51307, 'ICC', 'DeathKnight', 'BloodTank', 'Hands', 'Both', 'Sanctified Scourgelord Handguards'),
(6, 0, 10, 0, 290, 50622, 'ICC', 'DeathKnight', 'BloodTank', 'Finger1', 'Both', 'Devium''s Eternally Cold Ring'),
(6, 0, 11, 0, 290, 50404, 'ICC', 'DeathKnight', 'BloodTank', 'Finger2', 'Both', 'Ashen Band of Endless Courage'),
(6, 0, 12, 0, 290, 54591, 'ICC', 'DeathKnight', 'BloodTank', 'Trinket1', 'Both', 'Petrified Twilight Scale'),
(6, 0, 13, 0, 290, 50364, 'ICC', 'DeathKnight', 'BloodTank', 'Trinket2', 'Both', 'Sindragosa''s Flawless Fang'),
(6, 0, 14, 0, 290, 50466, 'ICC', 'DeathKnight', 'BloodTank', 'Back', 'Both', 'Sentinel''s Winter Cloak'),
(6, 0, 15, 0, 290, 49623, 'ICC', 'DeathKnight', 'BloodTank', 'MainHand', 'Both', 'Shadowmourne'),
(6, 0, 17, 0, 290, 50462, 'ICC', 'DeathKnight', 'BloodTank', 'Ranged', 'Both', 'Sigil of the Bone Gryphon');

-- Frost (tab 1) - DW Havoc's Call
INSERT INTO `playerbots_bis` VALUES
(6, 1, 0, 0, 290, 51312, 'ICC', 'DeathKnight', 'Frost', 'Head', 'Both', 'Sanctified Scourgelord Helmet'),
(6, 1, 1, 0, 290, 54581, 'ICC', 'DeathKnight', 'Frost', 'Neck', 'Both', 'Penumbra Pendant'),
(6, 1, 2, 0, 290, 51314, 'ICC', 'DeathKnight', 'Frost', 'Shoulders', 'Both', 'Sanctified Scourgelord Shoulderplates'),
(6, 1, 4, 0, 290, 51310, 'ICC', 'DeathKnight', 'Frost', 'Chest', 'Both', 'Sanctified Scourgelord Battleplate'),
(6, 1, 5, 0, 290, 50620, 'ICC', 'DeathKnight', 'Frost', 'Waist', 'Both', 'Coldwraith Links'),
(6, 1, 6, 0, 290, 51313, 'ICC', 'DeathKnight', 'Frost', 'Legs', 'Both', 'Sanctified Scourgelord Legplates'),
(6, 1, 7, 0, 290, 54578, 'ICC', 'DeathKnight', 'Frost', 'Feet', 'Both', 'Apocalypse''s Advance'),
(6, 1, 8, 0, 290, 50670, 'ICC', 'DeathKnight', 'Frost', 'Wrists', 'Both', 'Toskk''s Maximized Wristguards'),
(6, 1, 9, 0, 290, 50690, 'ICC', 'DeathKnight', 'Frost', 'Hands', 'Both', 'Fleshrending Gauntlets'),
(6, 1, 10, 0, 290, 50693, 'ICC', 'DeathKnight', 'Frost', 'Finger1', 'Both', 'Might of Blight'),
(6, 1, 11, 0, 290, 52572, 'ICC', 'DeathKnight', 'Frost', 'Finger2', 'Both', 'Ashen Band of Endless Might'),
(6, 1, 12, 0, 290, 54590, 'ICC', 'DeathKnight', 'Frost', 'Trinket1', 'Both', 'Sharpened Twilight Scale'),
(6, 1, 13, 0, 290, 50363, 'ICC', 'DeathKnight', 'Frost', 'Trinket2', 'Both', 'Deathbringer''s Will'),
(6, 1, 14, 0, 290, 50677, 'ICC', 'DeathKnight', 'Frost', 'Back', 'Both', 'Winding Sheet'),
(6, 1, 15, 0, 290, 50737, 'ICC', 'DeathKnight', 'Frost', 'MainHand', 'Both', 'Havoc''s Call, Blade of Lordaeron Kings'),
(6, 1, 16, 0, 290, 50737, 'ICC', 'DeathKnight', 'Frost', 'OffHand', 'Both', 'Havoc''s Call, Blade of Lordaeron Kings'),
(6, 1, 17, 0, 290, 50459, 'ICC', 'DeathKnight', 'Frost', 'Ranged', 'Both', 'Sigil of the Hanged Man');

-- Unholy (tab 2) - 2H Shadowmourne
INSERT INTO `playerbots_bis` VALUES
(6, 2, 0, 0, 290, 51312, 'ICC', 'DeathKnight', 'Unholy', 'Head', 'Both', 'Sanctified Scourgelord Helmet'),
(6, 2, 1, 0, 290, 54581, 'ICC', 'DeathKnight', 'Unholy', 'Neck', 'Both', 'Penumbra Pendant'),
(6, 2, 2, 0, 290, 51314, 'ICC', 'DeathKnight', 'Unholy', 'Shoulders', 'Both', 'Sanctified Scourgelord Shoulderplates'),
(6, 2, 4, 0, 290, 51310, 'ICC', 'DeathKnight', 'Unholy', 'Chest', 'Both', 'Sanctified Scourgelord Battleplate'),
(6, 2, 5, 0, 290, 50620, 'ICC', 'DeathKnight', 'Unholy', 'Waist', 'Both', 'Coldwraith Links'),
(6, 2, 6, 0, 290, 51313, 'ICC', 'DeathKnight', 'Unholy', 'Legs', 'Both', 'Sanctified Scourgelord Legplates'),
(6, 2, 7, 0, 290, 54578, 'ICC', 'DeathKnight', 'Unholy', 'Feet', 'Both', 'Apocalypse''s Advance'),
(6, 2, 8, 0, 290, 50670, 'ICC', 'DeathKnight', 'Unholy', 'Wrists', 'Both', 'Toskk''s Maximized Wristguards'),
(6, 2, 9, 0, 290, 50690, 'ICC', 'DeathKnight', 'Unholy', 'Hands', 'Both', 'Fleshrending Gauntlets'),
(6, 2, 10, 0, 290, 50693, 'ICC', 'DeathKnight', 'Unholy', 'Finger1', 'Both', 'Might of Blight'),
(6, 2, 11, 0, 290, 52572, 'ICC', 'DeathKnight', 'Unholy', 'Finger2', 'Both', 'Ashen Band of Endless Might'),
(6, 2, 12, 0, 290, 54590, 'ICC', 'DeathKnight', 'Unholy', 'Trinket1', 'Both', 'Sharpened Twilight Scale'),
(6, 2, 13, 0, 290, 50363, 'ICC', 'DeathKnight', 'Unholy', 'Trinket2', 'Both', 'Deathbringer''s Will'),
(6, 2, 14, 0, 290, 50677, 'ICC', 'DeathKnight', 'Unholy', 'Back', 'Both', 'Winding Sheet'),
(6, 2, 15, 0, 290, 49623, 'ICC', 'DeathKnight', 'Unholy', 'MainHand', 'Both', 'Shadowmourne'),
(6, 2, 17, 0, 290, 50459, 'ICC', 'DeathKnight', 'Unholy', 'Ranged', 'Both', 'Sigil of the Hanged Man');

-- ============================================================
-- Shaman (7)
-- ============================================================
-- Elemental (tab 0)
INSERT INTO `playerbots_bis` VALUES
(7, 0, 0, 0, 290, 51237, 'ICC', 'Shaman', 'Elemental', 'Head', 'Both', 'Sanctified Frost Witch''s Helm'),
(7, 0, 1, 0, 290, 50182, 'ICC', 'Shaman', 'Elemental', 'Neck', 'Both', 'Blood Queen''s Crimson Choker'),
(7, 0, 2, 0, 290, 51235, 'ICC', 'Shaman', 'Elemental', 'Shoulders', 'Both', 'Sanctified Frost Witch''s Shoulderpads'),
(7, 0, 4, 0, 290, 51239, 'ICC', 'Shaman', 'Elemental', 'Chest', 'Both', 'Sanctified Frost Witch''s Hauberk'),
(7, 0, 5, 0, 290, 54587, 'ICC', 'Shaman', 'Elemental', 'Waist', 'Both', 'Split Shape Belt'),
(7, 0, 6, 0, 290, 50694, 'ICC', 'Shaman', 'Elemental', 'Legs', 'Both', 'Plaguebringer''s Stained Pants'),
(7, 0, 7, 0, 290, 50699, 'ICC', 'Shaman', 'Elemental', 'Feet', 'Both', 'Plague Scientist''s Boots'),
(7, 0, 8, 0, 290, 54582, 'ICC', 'Shaman', 'Elemental', 'Wrists', 'Both', 'Bracers of Fiery Night'),
(7, 0, 9, 0, 290, 51238, 'ICC', 'Shaman', 'Elemental', 'Hands', 'Both', 'Sanctified Frost Witch''s Gloves'),
(7, 0, 10, 0, 290, 50664, 'ICC', 'Shaman', 'Elemental', 'Finger1', 'Both', 'Ring of Rapid Ascent'),
(7, 0, 11, 0, 290, 50398, 'ICC', 'Shaman', 'Elemental', 'Finger2', 'Both', 'Ashen Band of Endless Destruction'),
(7, 0, 12, 0, 290, 54588, 'ICC', 'Shaman', 'Elemental', 'Trinket1', 'Both', 'Charred Twilight Scale'),
(7, 0, 13, 0, 290, 50365, 'ICC', 'Shaman', 'Elemental', 'Trinket2', 'Both', 'Phylactery of the Nameless Lich'),
(7, 0, 14, 0, 290, 54583, 'ICC', 'Shaman', 'Elemental', 'Back', 'Both', 'Cloak of Burning Dusk'),
(7, 0, 15, 0, 290, 50734, 'ICC', 'Shaman', 'Elemental', 'MainHand', 'Both', 'Royal Scepter of Terenas II'),
(7, 0, 16, 0, 290, 50616, 'ICC', 'Shaman', 'Elemental', 'OffHand', 'Both', 'Bulwark of Smouldering Steel'),
(7, 0, 17, 0, 290, 50458, 'ICC', 'Shaman', 'Elemental', 'Ranged', 'Both', 'Bizuri''s Totem of Shattered Ice');

-- Enhancement (tab 1)
INSERT INTO `playerbots_bis` VALUES
(7, 1, 0, 0, 290, 51242, 'ICC', 'Shaman', 'Enhancement', 'Head', 'Both', 'Sanctified Frost Witch''s Faceguard'),
(7, 1, 1, 0, 290, 50633, 'ICC', 'Shaman', 'Enhancement', 'Neck', 'Both', 'Sindragosa''s Cruel Claw'),
(7, 1, 2, 0, 290, 51240, 'ICC', 'Shaman', 'Enhancement', 'Shoulders', 'Both', 'Sanctified Frost Witch''s Shoulderguards'),
(7, 1, 4, 0, 290, 50689, 'ICC', 'Shaman', 'Enhancement', 'Chest', 'Both', 'Carapace of Forgotten Kings'),
(7, 1, 5, 0, 290, 50688, 'ICC', 'Shaman', 'Enhancement', 'Waist', 'Both', 'Nerub''ar Stalker''s Cord'),
(7, 1, 6, 0, 290, 51241, 'ICC', 'Shaman', 'Enhancement', 'Legs', 'Both', 'Sanctified Frost Witch''s War-Kilt'),
(7, 1, 7, 0, 290, 54577, 'ICC', 'Shaman', 'Enhancement', 'Feet', 'Both', 'Returning Footfalls'),
(7, 1, 8, 0, 290, 50655, 'ICC', 'Shaman', 'Enhancement', 'Wrists', 'Both', 'Scourge Hunter''s Vambraces'),
(7, 1, 9, 0, 290, 51243, 'ICC', 'Shaman', 'Enhancement', 'Hands', 'Both', 'Sanctified Frost Witch''s Grips'),
(7, 1, 10, 0, 290, 50678, 'ICC', 'Shaman', 'Enhancement', 'Finger1', 'Both', 'Seal of Many Mouths'),
(7, 1, 11, 0, 290, 50402, 'ICC', 'Shaman', 'Enhancement', 'Finger2', 'Both', 'Ashen Band of Endless Vengeance'),
(7, 1, 12, 0, 290, 50706, 'ICC', 'Shaman', 'Enhancement', 'Trinket1', 'Both', 'Tiny Abomination in a Jar'),
(7, 1, 13, 0, 290, 54590, 'ICC', 'Shaman', 'Enhancement', 'Trinket2', 'Both', 'Sharpened Twilight Scale'),
(7, 1, 14, 0, 290, 50653, 'ICC', 'Shaman', 'Enhancement', 'Back', 'Both', 'Shadowvault Slayer''s Cloak'),
(7, 1, 15, 0, 290, 50737, 'ICC', 'Shaman', 'Enhancement', 'MainHand', 'Both', 'Havoc''s Call, Blade of Lordaeron Kings'),
(7, 1, 16, 0, 290, 50737, 'ICC', 'Shaman', 'Enhancement', 'OffHand', 'Both', 'Havoc''s Call, Blade of Lordaeron Kings'),
(7, 1, 17, 0, 290, 50463, 'ICC', 'Shaman', 'Enhancement', 'Ranged', 'Both', 'Totem of the Avalanche');

-- Restoration (tab 2)
INSERT INTO `playerbots_bis` VALUES
(7, 2, 0, 0, 290, 51247, 'ICC', 'Shaman', 'Restoration', 'Head', 'Both', 'Sanctified Frost Witch''s Headpiece'),
(7, 2, 1, 0, 290, 50182, 'ICC', 'Shaman', 'Restoration', 'Neck', 'Both', 'Blood Queen''s Crimson Choker'),
(7, 2, 2, 0, 290, 51245, 'ICC', 'Shaman', 'Restoration', 'Shoulders', 'Both', 'Sanctified Frost Witch''s Spaulders'),
(7, 2, 4, 0, 290, 51249, 'ICC', 'Shaman', 'Restoration', 'Chest', 'Both', 'Sanctified Frost Witch''s Tunic'),
(7, 2, 5, 0, 290, 54587, 'ICC', 'Shaman', 'Restoration', 'Waist', 'Both', 'Split Shape Belt'),
(7, 2, 6, 0, 290, 51246, 'ICC', 'Shaman', 'Restoration', 'Legs', 'Both', 'Sanctified Frost Witch''s Legguards'),
(7, 2, 7, 0, 290, 50699, 'ICC', 'Shaman', 'Restoration', 'Feet', 'Both', 'Plague Scientist''s Boots'),
(7, 2, 8, 0, 290, 50687, 'ICC', 'Shaman', 'Restoration', 'Wrists', 'Both', 'Bloodsunder''s Bracers'),
(7, 2, 9, 0, 290, 50703, 'ICC', 'Shaman', 'Restoration', 'Hands', 'Both', 'Unclean Surgical Gloves'),
(7, 2, 10, 0, 290, 50664, 'ICC', 'Shaman', 'Restoration', 'Finger1', 'Both', 'Ring of Rapid Ascent'),
(7, 2, 11, 0, 290, 50400, 'ICC', 'Shaman', 'Restoration', 'Finger2', 'Both', 'Ashen Band of Endless Wisdom'),
(7, 2, 12, 0, 290, 50366, 'ICC', 'Shaman', 'Restoration', 'Trinket1', 'Both', 'Althor''s Abacus'),
(7, 2, 13, 0, 290, 54589, 'ICC', 'Shaman', 'Restoration', 'Trinket2', 'Both', 'Glowing Twilight Scale'),
(7, 2, 14, 0, 290, 54583, 'ICC', 'Shaman', 'Restoration', 'Back', 'Both', 'Cloak of Burning Dusk'),
(7, 2, 15, 0, 290, 46017, 'ICC', 'Shaman', 'Restoration', 'MainHand', 'Both', 'Val''anyr, Hammer of Ancient Kings'),
(7, 2, 16, 0, 290, 50616, 'ICC', 'Shaman', 'Restoration', 'OffHand', 'Both', 'Bulwark of Smouldering Steel'),
(7, 2, 17, 0, 290, 50458, 'ICC', 'Shaman', 'Restoration', 'Ranged', 'Both', 'Bizuri''s Totem of Shattered Ice');

-- ============================================================
-- Mage (8) - source has Fire and Arcane only. Frost mirrors Arcane.
-- ============================================================
-- Arcane (tab 0)
INSERT INTO `playerbots_bis` VALUES
(8, 0, 0, 0, 290, 51281, 'ICC', 'Mage', 'Arcane', 'Head', 'Both', 'Sanctified Bloodmage Hood'),
(8, 0, 1, 0, 290, 50182, 'ICC', 'Mage', 'Arcane', 'Neck', 'Both', 'Blood Queen''s Crimson Choker'),
(8, 0, 2, 0, 290, 51284, 'ICC', 'Mage', 'Arcane', 'Shoulders', 'Both', 'Sanctified Bloodmage Shoulderpads'),
(8, 0, 4, 0, 290, 51283, 'ICC', 'Mage', 'Arcane', 'Chest', 'Both', 'Sanctified Bloodmage Robe'),
(8, 0, 5, 0, 290, 50613, 'ICC', 'Mage', 'Arcane', 'Waist', 'Both', 'Crushing Coldwraith Belt'),
(8, 0, 6, 0, 290, 50694, 'ICC', 'Mage', 'Arcane', 'Legs', 'Both', 'Plaguebringer''s Stained Pants'),
(8, 0, 7, 0, 290, 50699, 'ICC', 'Mage', 'Arcane', 'Feet', 'Both', 'Plague Scientist''s Boots'),
(8, 0, 8, 0, 290, 54582, 'ICC', 'Mage', 'Arcane', 'Wrists', 'Both', 'Bracers of Fiery Night'),
(8, 0, 9, 0, 290, 51280, 'ICC', 'Mage', 'Arcane', 'Hands', 'Both', 'Sanctified Bloodmage Gloves'),
(8, 0, 10, 0, 290, 50614, 'ICC', 'Mage', 'Arcane', 'Finger1', 'Both', 'Loop of the Endless Labyrinth'),
(8, 0, 11, 0, 290, 50398, 'ICC', 'Mage', 'Arcane', 'Finger2', 'Both', 'Ashen Band of Endless Destruction'),
(8, 0, 12, 0, 290, 54588, 'ICC', 'Mage', 'Arcane', 'Trinket1', 'Both', 'Charred Twilight Scale'),
(8, 0, 13, 0, 290, 50348, 'ICC', 'Mage', 'Arcane', 'Trinket2', 'Both', 'Dislodged Foreign Object'),
(8, 0, 14, 0, 290, 54583, 'ICC', 'Mage', 'Arcane', 'Back', 'Both', 'Cloak of Burning Dusk'),
(8, 0, 15, 0, 290, 50732, 'ICC', 'Mage', 'Arcane', 'MainHand', 'Both', 'Bloodsurge, Kel''Thuzad''s Blade of Agony'),
(8, 0, 16, 0, 290, 50719, 'ICC', 'Mage', 'Arcane', 'OffHand', 'Both', 'Shadow Silk Spindle'),
(8, 0, 17, 0, 290, 50684, 'ICC', 'Mage', 'Arcane', 'Ranged', 'Both', 'Corpse-Impaling Spike');

-- Fire (tab 1)
INSERT INTO `playerbots_bis` VALUES
(8, 1, 0, 0, 290, 51281, 'ICC', 'Mage', 'Fire', 'Head', 'Both', 'Sanctified Bloodmage Hood'),
(8, 1, 1, 0, 290, 50182, 'ICC', 'Mage', 'Fire', 'Neck', 'Both', 'Blood Queen''s Crimson Choker'),
(8, 1, 2, 0, 290, 51284, 'ICC', 'Mage', 'Fire', 'Shoulders', 'Both', 'Sanctified Bloodmage Shoulderpads'),
(8, 1, 4, 0, 290, 51283, 'ICC', 'Mage', 'Fire', 'Chest', 'Both', 'Sanctified Bloodmage Robe'),
(8, 1, 5, 0, 290, 50613, 'ICC', 'Mage', 'Fire', 'Waist', 'Both', 'Crushing Coldwraith Belt'),
(8, 1, 6, 0, 290, 50694, 'ICC', 'Mage', 'Fire', 'Legs', 'Both', 'Plaguebringer''s Stained Pants'),
(8, 1, 7, 0, 290, 50699, 'ICC', 'Mage', 'Fire', 'Feet', 'Both', 'Plague Scientist''s Boots'),
(8, 1, 8, 0, 290, 54582, 'ICC', 'Mage', 'Fire', 'Wrists', 'Both', 'Bracers of Fiery Night'),
(8, 1, 9, 0, 290, 51280, 'ICC', 'Mage', 'Fire', 'Hands', 'Both', 'Sanctified Bloodmage Gloves'),
(8, 1, 10, 0, 290, 50614, 'ICC', 'Mage', 'Fire', 'Finger1', 'Both', 'Loop of the Endless Labyrinth'),
(8, 1, 11, 0, 290, 50398, 'ICC', 'Mage', 'Fire', 'Finger2', 'Both', 'Ashen Band of Endless Destruction'),
(8, 1, 12, 0, 290, 54588, 'ICC', 'Mage', 'Fire', 'Trinket1', 'Both', 'Charred Twilight Scale'),
(8, 1, 13, 0, 290, 50365, 'ICC', 'Mage', 'Fire', 'Trinket2', 'Both', 'Phylactery of the Nameless Lich'),
(8, 1, 14, 0, 290, 54583, 'ICC', 'Mage', 'Fire', 'Back', 'Both', 'Cloak of Burning Dusk'),
(8, 1, 15, 0, 290, 50732, 'ICC', 'Mage', 'Fire', 'MainHand', 'Both', 'Bloodsurge, Kel''Thuzad''s Blade of Agony'),
(8, 1, 16, 0, 290, 50719, 'ICC', 'Mage', 'Fire', 'OffHand', 'Both', 'Shadow Silk Spindle'),
(8, 1, 17, 0, 290, 50684, 'ICC', 'Mage', 'Fire', 'Ranged', 'Both', 'Corpse-Impaling Spike');

-- Frost (tab 2) - mirrors Arcane
INSERT INTO `playerbots_bis` VALUES
(8, 2, 0, 0, 290, 51281, 'ICC', 'Mage', 'Frost', 'Head', 'Both', 'Sanctified Bloodmage Hood'),
(8, 2, 1, 0, 290, 50182, 'ICC', 'Mage', 'Frost', 'Neck', 'Both', 'Blood Queen''s Crimson Choker'),
(8, 2, 2, 0, 290, 51284, 'ICC', 'Mage', 'Frost', 'Shoulders', 'Both', 'Sanctified Bloodmage Shoulderpads'),
(8, 2, 4, 0, 290, 51283, 'ICC', 'Mage', 'Frost', 'Chest', 'Both', 'Sanctified Bloodmage Robe'),
(8, 2, 5, 0, 290, 50613, 'ICC', 'Mage', 'Frost', 'Waist', 'Both', 'Crushing Coldwraith Belt'),
(8, 2, 6, 0, 290, 50694, 'ICC', 'Mage', 'Frost', 'Legs', 'Both', 'Plaguebringer''s Stained Pants'),
(8, 2, 7, 0, 290, 50699, 'ICC', 'Mage', 'Frost', 'Feet', 'Both', 'Plague Scientist''s Boots'),
(8, 2, 8, 0, 290, 54582, 'ICC', 'Mage', 'Frost', 'Wrists', 'Both', 'Bracers of Fiery Night'),
(8, 2, 9, 0, 290, 51280, 'ICC', 'Mage', 'Frost', 'Hands', 'Both', 'Sanctified Bloodmage Gloves'),
(8, 2, 10, 0, 290, 50614, 'ICC', 'Mage', 'Frost', 'Finger1', 'Both', 'Loop of the Endless Labyrinth'),
(8, 2, 11, 0, 290, 50398, 'ICC', 'Mage', 'Frost', 'Finger2', 'Both', 'Ashen Band of Endless Destruction'),
(8, 2, 12, 0, 290, 54588, 'ICC', 'Mage', 'Frost', 'Trinket1', 'Both', 'Charred Twilight Scale'),
(8, 2, 13, 0, 290, 50348, 'ICC', 'Mage', 'Frost', 'Trinket2', 'Both', 'Dislodged Foreign Object'),
(8, 2, 14, 0, 290, 54583, 'ICC', 'Mage', 'Frost', 'Back', 'Both', 'Cloak of Burning Dusk'),
(8, 2, 15, 0, 290, 50732, 'ICC', 'Mage', 'Frost', 'MainHand', 'Both', 'Bloodsurge, Kel''Thuzad''s Blade of Agony'),
(8, 2, 16, 0, 290, 50719, 'ICC', 'Mage', 'Frost', 'OffHand', 'Both', 'Shadow Silk Spindle'),
(8, 2, 17, 0, 290, 50684, 'ICC', 'Mage', 'Frost', 'Ranged', 'Both', 'Corpse-Impaling Spike');

-- ============================================================
-- Warlock (9) - one shared list across all 3 specs.
-- ============================================================
-- Affliction (tab 0)
INSERT INTO `playerbots_bis` VALUES
(9, 0, 0, 0, 290, 51231, 'ICC', 'Warlock', 'Affliction', 'Head', 'Both', 'Sanctified Dark Coven Hood'),
(9, 0, 1, 0, 290, 50182, 'ICC', 'Warlock', 'Affliction', 'Neck', 'Both', 'Blood Queen''s Crimson Choker'),
(9, 0, 2, 0, 290, 51234, 'ICC', 'Warlock', 'Affliction', 'Shoulders', 'Both', 'Sanctified Dark Coven Shoulderpads'),
(9, 0, 4, 0, 290, 51233, 'ICC', 'Warlock', 'Affliction', 'Chest', 'Both', 'Sanctified Dark Coven Robe'),
(9, 0, 5, 0, 290, 50613, 'ICC', 'Warlock', 'Affliction', 'Waist', 'Both', 'Crushing Coldwraith Belt'),
(9, 0, 6, 0, 290, 50694, 'ICC', 'Warlock', 'Affliction', 'Legs', 'Both', 'Plaguebringer''s Stained Pants'),
(9, 0, 7, 0, 290, 50699, 'ICC', 'Warlock', 'Affliction', 'Feet', 'Both', 'Plague Scientist''s Boots'),
(9, 0, 8, 0, 290, 54582, 'ICC', 'Warlock', 'Affliction', 'Wrists', 'Both', 'Bracers of Fiery Night'),
(9, 0, 9, 0, 290, 51230, 'ICC', 'Warlock', 'Affliction', 'Hands', 'Both', 'Sanctified Dark Coven Gloves'),
(9, 0, 10, 0, 290, 50614, 'ICC', 'Warlock', 'Affliction', 'Finger1', 'Both', 'Loop of the Endless Labyrinth'),
(9, 0, 11, 0, 290, 50664, 'ICC', 'Warlock', 'Affliction', 'Finger2', 'Both', 'Ring of Rapid Ascent'),
(9, 0, 12, 0, 290, 54588, 'ICC', 'Warlock', 'Affliction', 'Trinket1', 'Both', 'Charred Twilight Scale'),
(9, 0, 13, 0, 290, 50365, 'ICC', 'Warlock', 'Affliction', 'Trinket2', 'Both', 'Phylactery of the Nameless Lich'),
(9, 0, 14, 0, 290, 54583, 'ICC', 'Warlock', 'Affliction', 'Back', 'Both', 'Cloak of Burning Dusk'),
(9, 0, 15, 0, 290, 50732, 'ICC', 'Warlock', 'Affliction', 'MainHand', 'Both', 'Bloodsurge, Kel''Thuzad''s Blade of Agony'),
(9, 0, 16, 0, 290, 50719, 'ICC', 'Warlock', 'Affliction', 'OffHand', 'Both', 'Shadow Silk Spindle'),
(9, 0, 17, 0, 290, 50684, 'ICC', 'Warlock', 'Affliction', 'Ranged', 'Both', 'Corpse-Impaling Spike');

-- Demonology (tab 1)
INSERT INTO `playerbots_bis` VALUES
(9, 1, 0, 0, 290, 51231, 'ICC', 'Warlock', 'Demonology', 'Head', 'Both', 'Sanctified Dark Coven Hood'),
(9, 1, 1, 0, 290, 50182, 'ICC', 'Warlock', 'Demonology', 'Neck', 'Both', 'Blood Queen''s Crimson Choker'),
(9, 1, 2, 0, 290, 51234, 'ICC', 'Warlock', 'Demonology', 'Shoulders', 'Both', 'Sanctified Dark Coven Shoulderpads'),
(9, 1, 4, 0, 290, 51233, 'ICC', 'Warlock', 'Demonology', 'Chest', 'Both', 'Sanctified Dark Coven Robe'),
(9, 1, 5, 0, 290, 50613, 'ICC', 'Warlock', 'Demonology', 'Waist', 'Both', 'Crushing Coldwraith Belt'),
(9, 1, 6, 0, 290, 50694, 'ICC', 'Warlock', 'Demonology', 'Legs', 'Both', 'Plaguebringer''s Stained Pants'),
(9, 1, 7, 0, 290, 50699, 'ICC', 'Warlock', 'Demonology', 'Feet', 'Both', 'Plague Scientist''s Boots'),
(9, 1, 8, 0, 290, 54582, 'ICC', 'Warlock', 'Demonology', 'Wrists', 'Both', 'Bracers of Fiery Night'),
(9, 1, 9, 0, 290, 51230, 'ICC', 'Warlock', 'Demonology', 'Hands', 'Both', 'Sanctified Dark Coven Gloves'),
(9, 1, 10, 0, 290, 50614, 'ICC', 'Warlock', 'Demonology', 'Finger1', 'Both', 'Loop of the Endless Labyrinth'),
(9, 1, 11, 0, 290, 50664, 'ICC', 'Warlock', 'Demonology', 'Finger2', 'Both', 'Ring of Rapid Ascent'),
(9, 1, 12, 0, 290, 54588, 'ICC', 'Warlock', 'Demonology', 'Trinket1', 'Both', 'Charred Twilight Scale'),
(9, 1, 13, 0, 290, 50365, 'ICC', 'Warlock', 'Demonology', 'Trinket2', 'Both', 'Phylactery of the Nameless Lich'),
(9, 1, 14, 0, 290, 54583, 'ICC', 'Warlock', 'Demonology', 'Back', 'Both', 'Cloak of Burning Dusk'),
(9, 1, 15, 0, 290, 50732, 'ICC', 'Warlock', 'Demonology', 'MainHand', 'Both', 'Bloodsurge, Kel''Thuzad''s Blade of Agony'),
(9, 1, 16, 0, 290, 50719, 'ICC', 'Warlock', 'Demonology', 'OffHand', 'Both', 'Shadow Silk Spindle'),
(9, 1, 17, 0, 290, 50684, 'ICC', 'Warlock', 'Demonology', 'Ranged', 'Both', 'Corpse-Impaling Spike');

-- Destruction (tab 2)
INSERT INTO `playerbots_bis` VALUES
(9, 2, 0, 0, 290, 51231, 'ICC', 'Warlock', 'Destruction', 'Head', 'Both', 'Sanctified Dark Coven Hood'),
(9, 2, 1, 0, 290, 50182, 'ICC', 'Warlock', 'Destruction', 'Neck', 'Both', 'Blood Queen''s Crimson Choker'),
(9, 2, 2, 0, 290, 51234, 'ICC', 'Warlock', 'Destruction', 'Shoulders', 'Both', 'Sanctified Dark Coven Shoulderpads'),
(9, 2, 4, 0, 290, 51233, 'ICC', 'Warlock', 'Destruction', 'Chest', 'Both', 'Sanctified Dark Coven Robe'),
(9, 2, 5, 0, 290, 50613, 'ICC', 'Warlock', 'Destruction', 'Waist', 'Both', 'Crushing Coldwraith Belt'),
(9, 2, 6, 0, 290, 50694, 'ICC', 'Warlock', 'Destruction', 'Legs', 'Both', 'Plaguebringer''s Stained Pants'),
(9, 2, 7, 0, 290, 50699, 'ICC', 'Warlock', 'Destruction', 'Feet', 'Both', 'Plague Scientist''s Boots'),
(9, 2, 8, 0, 290, 54582, 'ICC', 'Warlock', 'Destruction', 'Wrists', 'Both', 'Bracers of Fiery Night'),
(9, 2, 9, 0, 290, 51230, 'ICC', 'Warlock', 'Destruction', 'Hands', 'Both', 'Sanctified Dark Coven Gloves'),
(9, 2, 10, 0, 290, 50614, 'ICC', 'Warlock', 'Destruction', 'Finger1', 'Both', 'Loop of the Endless Labyrinth'),
(9, 2, 11, 0, 290, 50664, 'ICC', 'Warlock', 'Destruction', 'Finger2', 'Both', 'Ring of Rapid Ascent'),
(9, 2, 12, 0, 290, 54588, 'ICC', 'Warlock', 'Destruction', 'Trinket1', 'Both', 'Charred Twilight Scale'),
(9, 2, 13, 0, 290, 50365, 'ICC', 'Warlock', 'Destruction', 'Trinket2', 'Both', 'Phylactery of the Nameless Lich'),
(9, 2, 14, 0, 290, 54583, 'ICC', 'Warlock', 'Destruction', 'Back', 'Both', 'Cloak of Burning Dusk'),
(9, 2, 15, 0, 290, 50732, 'ICC', 'Warlock', 'Destruction', 'MainHand', 'Both', 'Bloodsurge, Kel''Thuzad''s Blade of Agony'),
(9, 2, 16, 0, 290, 50719, 'ICC', 'Warlock', 'Destruction', 'OffHand', 'Both', 'Shadow Silk Spindle'),
(9, 2, 17, 0, 290, 50684, 'ICC', 'Warlock', 'Destruction', 'Ranged', 'Both', 'Corpse-Impaling Spike');

-- ============================================================
-- Druid (11) - Balance=0, Feral Cat=1, Restoration=2, Feral Bear=10 (sentinel, runtime tank check).
-- ============================================================
-- Balance (tab 0)
INSERT INTO `playerbots_bis` VALUES
(11, 0, 0, 0, 290, 51290, 'ICC', 'Druid', 'Balance', 'Head', 'Both', 'Sanctified Lasherweave Cover'),
(11, 0, 1, 0, 290, 50182, 'ICC', 'Druid', 'Balance', 'Neck', 'Both', 'Blood Queen''s Crimson Choker'),
(11, 0, 2, 0, 290, 51292, 'ICC', 'Druid', 'Balance', 'Shoulders', 'Both', 'Sanctified Lasherweave Mantle'),
(11, 0, 4, 0, 290, 51294, 'ICC', 'Druid', 'Balance', 'Chest', 'Both', 'Sanctified Lasherweave Vestment'),
(11, 0, 5, 0, 290, 50613, 'ICC', 'Druid', 'Balance', 'Waist', 'Both', 'Crushing Coldwraith Belt'),
(11, 0, 6, 0, 290, 50694, 'ICC', 'Druid', 'Balance', 'Legs', 'Both', 'Plaguebringer''s Stained Pants'),
(11, 0, 7, 0, 290, 50699, 'ICC', 'Druid', 'Balance', 'Feet', 'Both', 'Plague Scientist''s Boots'),
(11, 0, 8, 0, 290, 54582, 'ICC', 'Druid', 'Balance', 'Wrists', 'Both', 'Bracers of Fiery Night'),
(11, 0, 9, 0, 290, 51291, 'ICC', 'Druid', 'Balance', 'Hands', 'Both', 'Sanctified Lasherweave Gloves'),
(11, 0, 10, 0, 290, 50398, 'ICC', 'Druid', 'Balance', 'Finger1', 'Both', 'Ashen Band of Endless Destruction'),
(11, 0, 11, 0, 290, 50714, 'ICC', 'Druid', 'Balance', 'Finger2', 'Both', 'Valanar''s Other Signet Ring'),
(11, 0, 12, 0, 290, 54588, 'ICC', 'Druid', 'Balance', 'Trinket1', 'Both', 'Charred Twilight Scale'),
(11, 0, 13, 0, 290, 50365, 'ICC', 'Druid', 'Balance', 'Trinket2', 'Both', 'Phylactery of the Nameless Lich'),
(11, 0, 14, 0, 290, 54583, 'ICC', 'Druid', 'Balance', 'Back', 'Both', 'Cloak of Burning Dusk'),
(11, 0, 15, 0, 290, 50734, 'ICC', 'Druid', 'Balance', 'MainHand', 'Both', 'Royal Scepter of Terenas II'),
(11, 0, 16, 0, 290, 50719, 'ICC', 'Druid', 'Balance', 'OffHand', 'Both', 'Shadow Silk Spindle');

-- Feral Cat (tab 1)
INSERT INTO `playerbots_bis` VALUES
(11, 1, 0, 0, 290, 51296, 'ICC', 'Druid', 'FeralCat', 'Head', 'Both', 'Sanctified Lasherweave Headguard'),
(11, 1, 1, 0, 290, 50633, 'ICC', 'Druid', 'FeralCat', 'Neck', 'Both', 'Sindragosa''s Cruel Claw'),
(11, 1, 2, 0, 290, 51299, 'ICC', 'Druid', 'FeralCat', 'Shoulders', 'Both', 'Sanctified Lasherweave Shoulderpads'),
(11, 1, 4, 0, 290, 51298, 'ICC', 'Druid', 'FeralCat', 'Chest', 'Both', 'Sanctified Lasherweave Raiment'),
(11, 1, 5, 0, 290, 50707, 'ICC', 'Druid', 'FeralCat', 'Waist', 'Both', 'Astrylian''s Sutured Cinch'),
(11, 1, 6, 0, 290, 51297, 'ICC', 'Druid', 'FeralCat', 'Legs', 'Both', 'Sanctified Lasherweave Legguards'),
(11, 1, 7, 0, 290, 50607, 'ICC', 'Druid', 'FeralCat', 'Feet', 'Both', 'Frostbitten Fur Boots'),
(11, 1, 8, 0, 290, 50670, 'ICC', 'Druid', 'FeralCat', 'Wrists', 'Both', 'Toskk''s Maximized Wristguards'),
(11, 1, 9, 0, 290, 50675, 'ICC', 'Druid', 'FeralCat', 'Hands', 'Both', 'Aldriana''s Gloves of Secrecy'),
(11, 1, 10, 0, 290, 50618, 'ICC', 'Druid', 'FeralCat', 'Finger1', 'Both', 'Frostbrood Sapphire Ring'),
(11, 1, 11, 0, 290, 50402, 'ICC', 'Druid', 'FeralCat', 'Finger2', 'Both', 'Ashen Band of Endless Vengeance'),
(11, 1, 12, 0, 290, 50363, 'ICC', 'Druid', 'FeralCat', 'Trinket1', 'Both', 'Deathbringer''s Will'),
(11, 1, 13, 0, 290, 54590, 'ICC', 'Druid', 'FeralCat', 'Trinket2', 'Both', 'Sharpened Twilight Scale'),
(11, 1, 14, 0, 290, 50653, 'ICC', 'Druid', 'FeralCat', 'Back', 'Both', 'Shadowvault Slayer''s Cloak'),
(11, 1, 15, 0, 290, 50735, 'ICC', 'Druid', 'FeralCat', 'MainHand', 'Both', 'Oathbinder, Charge of the Ranger-General'),
(11, 1, 17, 0, 290, 50456, 'ICC', 'Druid', 'FeralCat', 'Ranged', 'Both', 'Idol of the Crying Moon');

-- Restoration (tab 2)
INSERT INTO `playerbots_bis` VALUES
(11, 2, 0, 0, 290, 51302, 'ICC', 'Druid', 'Restoration', 'Head', 'Both', 'Sanctified Lasherweave Helmet'),
(11, 2, 1, 0, 290, 50609, 'ICC', 'Druid', 'Restoration', 'Neck', 'Both', 'Bone Sentinel''s Amulet'),
(11, 2, 2, 0, 290, 51304, 'ICC', 'Druid', 'Restoration', 'Shoulders', 'Both', 'Sanctified Lasherweave Pauldrons'),
(11, 2, 4, 0, 290, 50717, 'ICC', 'Druid', 'Restoration', 'Chest', 'Both', 'Sanguine Silk Robes'),
(11, 2, 5, 0, 290, 50705, 'ICC', 'Druid', 'Restoration', 'Waist', 'Both', 'Professor''s Bloodied Smock'),
(11, 2, 6, 0, 290, 51303, 'ICC', 'Druid', 'Restoration', 'Legs', 'Both', 'Sanctified Lasherweave Legplates'),
(11, 2, 7, 0, 290, 50665, 'ICC', 'Druid', 'Restoration', 'Feet', 'Both', 'Boots Of Unnatural Growth'),
(11, 2, 8, 0, 290, 54584, 'ICC', 'Druid', 'Restoration', 'Wrists', 'Both', 'Phaseshifter Bracers'),
(11, 2, 9, 0, 290, 51301, 'ICC', 'Druid', 'Restoration', 'Hands', 'Both', 'Sanctified Lasherweave Gauntlets'),
(11, 2, 10, 0, 290, 50400, 'ICC', 'Druid', 'Restoration', 'Finger1', 'Both', 'Ashen Band of Endless Wisdom'),
(11, 2, 11, 0, 290, 50636, 'ICC', 'Druid', 'Restoration', 'Finger2', 'Both', 'Memory of Malygos'),
(11, 2, 12, 0, 290, 47059, 'ICC', 'Druid', 'Restoration', 'Trinket1', 'Both', 'Solace of the Defeated'),
(11, 2, 13, 0, 290, 54589, 'ICC', 'Druid', 'Restoration', 'Trinket2', 'Both', 'Glowing Twilight Scale'),
(11, 2, 14, 0, 290, 50668, 'ICC', 'Druid', 'Restoration', 'Back', 'Both', 'Greatcloak of the Turned Champion'),
(11, 2, 15, 0, 290, 46017, 'ICC', 'Druid', 'Restoration', 'MainHand', 'Both', 'Val''anyr, Hammer of Ancient Kings'),
(11, 2, 16, 0, 290, 50635, 'ICC', 'Druid', 'Restoration', 'OffHand', 'Both', 'Sundial of Eternal Dusk'),
(11, 2, 17, 0, 290, 50454, 'ICC', 'Druid', 'Restoration', 'Ranged', 'Both', 'Idol of the Black Willow');

-- Feral Bear / Feral Tank (sentinel tab 10) - resolved at runtime when bot has Tank strategy
INSERT INTO `playerbots_bis` VALUES
(11, 10, 0, 0, 290, 51296, 'ICC', 'Druid', 'FeralBear', 'Head', 'Both', 'Sanctified Lasherweave Headguard'),
(11, 10, 1, 0, 290, 50682, 'ICC', 'Druid', 'FeralBear', 'Neck', 'Both', 'Bile-Encrusted Medallion'),
(11, 10, 2, 0, 290, 51299, 'ICC', 'Druid', 'FeralBear', 'Shoulders', 'Both', 'Sanctified Lasherweave Shoulderpads'),
(11, 10, 4, 0, 290, 50656, 'ICC', 'Druid', 'FeralBear', 'Chest', 'Both', 'Ikfirus''s Sack of Wonder'),
(11, 10, 5, 0, 290, 50707, 'ICC', 'Druid', 'FeralBear', 'Waist', 'Both', 'Astrylian''s Sutured Cinch'),
(11, 10, 6, 0, 290, 51297, 'ICC', 'Druid', 'FeralBear', 'Legs', 'Both', 'Sanctified Lasherweave Legguards'),
(11, 10, 7, 0, 290, 50607, 'ICC', 'Druid', 'FeralBear', 'Feet', 'Both', 'Frostbitten Fur Boots'),
(11, 10, 8, 0, 290, 54580, 'ICC', 'Druid', 'FeralBear', 'Wrists', 'Both', 'Umbrage Armbands'),
(11, 10, 9, 0, 290, 51295, 'ICC', 'Druid', 'FeralBear', 'Hands', 'Both', 'Sanctified Lasherweave Handgrips'),
(11, 10, 10, 0, 290, 50622, 'ICC', 'Druid', 'FeralBear', 'Finger1', 'Both', 'Devium''s Eternally Cold Ring'),
(11, 10, 11, 0, 290, 50404, 'ICC', 'Druid', 'FeralBear', 'Finger2', 'Both', 'Ashen Band of Endless Courage'),
(11, 10, 12, 0, 290, 50364, 'ICC', 'Druid', 'FeralBear', 'Trinket1', 'Both', 'Sindragosa''s Flawless Fang'),
(11, 10, 13, 0, 290, 50356, 'ICC', 'Druid', 'FeralBear', 'Trinket2', 'Both', 'Corroded Skeleton Key'),
(11, 10, 14, 0, 290, 50466, 'ICC', 'Druid', 'FeralBear', 'Back', 'Both', 'Sentinel''s Winter Cloak'),
(11, 10, 15, 0, 290, 50735, 'ICC', 'Druid', 'FeralBear', 'MainHand', 'Both', 'Oathbinder, Charge of the Ranger-General'),
(11, 10, 17, 0, 290, 50456, 'ICC', 'Druid', 'FeralBear', 'Ranged', 'Both', 'Idol of the Crying Moon');
