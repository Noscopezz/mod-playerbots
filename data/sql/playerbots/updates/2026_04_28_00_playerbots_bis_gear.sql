-- ICC BiS gear table for the bisicc party-chat command.
-- Keyed by (class, tab, slot). Slot uses AzerothCore EquipmentSlots enum:
-- head=0, neck=1, shoulders=2, chest=4, waist=5, legs=6, feet=7, wrists=8, hands=9,
-- finger1=10, finger2=11, trinket1=12, trinket2=13, back=14, mainhand=15, offhand=16, ranged=17.
-- Druid Bear lives under sentinel tab=10 (resolved at runtime when bot has Tank strategy).

DROP TABLE IF EXISTS `playerbots_bis_gear`;
CREATE TABLE `playerbots_bis_gear` (
    `class`      TINYINT UNSIGNED NOT NULL,
    `tab`        TINYINT UNSIGNED NOT NULL,
    `slot`       TINYINT UNSIGNED NOT NULL,
    `item`       INT UNSIGNED NOT NULL,
    `class_name` VARCHAR(16) NOT NULL,
    `spec_name`  VARCHAR(32) NOT NULL,
    `slot_name`  VARCHAR(16) NOT NULL,
    `item_name`  VARCHAR(96) NOT NULL,
    PRIMARY KEY (`class`, `tab`, `slot`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ============================================================
-- Warrior (1)
-- ============================================================
-- Arms (tab 0) - 2H Shadowmourne
INSERT INTO `playerbots_bis_gear` VALUES
(1, 0, 0, 51227, 'Warrior', 'Arms', 'Head', "Sanctified Ymirjar Lord's Helmet"),
(1, 0, 1, 54581, 'Warrior', 'Arms', 'Neck', 'Penumbra Pendant'),
(1, 0, 2, 51229, 'Warrior', 'Arms', 'Shoulders', "Sanctified Ymirjar Lord's Shoulderplates"),
(1, 0, 4, 51225, 'Warrior', 'Arms', 'Chest', "Sanctified Ymirjar Lord's Battleplate"),
(1, 0, 5, 50620, 'Warrior', 'Arms', 'Waist', 'Coldwraith Links'),
(1, 0, 6, 51228, 'Warrior', 'Arms', 'Legs', "Sanctified Ymirjar Lord's Legplates"),
(1, 0, 7, 54578, 'Warrior', 'Arms', 'Feet', "Apocalypse's Advance"),
(1, 0, 8, 50670, 'Warrior', 'Arms', 'Wrists', "Toskk's Maximized Wristguards"),
(1, 0, 9, 50675, 'Warrior', 'Arms', 'Hands', "Aldriana's Gloves of Secrecy"),
(1, 0, 10, 50657, 'Warrior', 'Arms', 'Finger1', "Skeleton Lord's Circle"),
(1, 0, 11, 52572, 'Warrior', 'Arms', 'Finger2', 'Ashen Band of Endless Might'),
(1, 0, 12, 50363, 'Warrior', 'Arms', 'Trinket1', "Deathbringer's Will"),
(1, 0, 13, 54590, 'Warrior', 'Arms', 'Trinket2', 'Sharpened Twilight Scale'),
(1, 0, 14, 47545, 'Warrior', 'Arms', 'Back', "Vereesa's Dexterity"),
(1, 0, 15, 49623, 'Warrior', 'Arms', 'MainHand', 'Shadowmourne'),
(1, 0, 17, 50733, 'Warrior', 'Arms', 'Ranged', "Fal'inrush, Defender of Quel'thalas");

-- Fury (tab 1) - DW Cryptmaker
INSERT INTO `playerbots_bis_gear` VALUES
(1, 1, 0, 51227, 'Warrior', 'Fury', 'Head', "Sanctified Ymirjar Lord's Helmet"),
(1, 1, 1, 54581, 'Warrior', 'Fury', 'Neck', 'Penumbra Pendant'),
(1, 1, 2, 51229, 'Warrior', 'Fury', 'Shoulders', "Sanctified Ymirjar Lord's Shoulderplates"),
(1, 1, 4, 51225, 'Warrior', 'Fury', 'Chest', "Sanctified Ymirjar Lord's Battleplate"),
(1, 1, 5, 50620, 'Warrior', 'Fury', 'Waist', 'Coldwraith Links'),
(1, 1, 6, 51228, 'Warrior', 'Fury', 'Legs', "Sanctified Ymirjar Lord's Legplates"),
(1, 1, 7, 54578, 'Warrior', 'Fury', 'Feet', "Apocalypse's Advance"),
(1, 1, 8, 50670, 'Warrior', 'Fury', 'Wrists', "Toskk's Maximized Wristguards"),
(1, 1, 9, 50675, 'Warrior', 'Fury', 'Hands', "Aldriana's Gloves of Secrecy"),
(1, 1, 10, 50657, 'Warrior', 'Fury', 'Finger1', "Skeleton Lord's Circle"),
(1, 1, 11, 52572, 'Warrior', 'Fury', 'Finger2', 'Ashen Band of Endless Might'),
(1, 1, 12, 50363, 'Warrior', 'Fury', 'Trinket1', "Deathbringer's Will"),
(1, 1, 13, 54590, 'Warrior', 'Fury', 'Trinket2', 'Sharpened Twilight Scale'),
(1, 1, 14, 47545, 'Warrior', 'Fury', 'Back', "Vereesa's Dexterity"),
(1, 1, 15, 50603, 'Warrior', 'Fury', 'MainHand', 'Cryptmaker'),
(1, 1, 16, 50603, 'Warrior', 'Fury', 'OffHand', 'Cryptmaker'),
(1, 1, 17, 50733, 'Warrior', 'Fury', 'Ranged', "Fal'inrush, Defender of Quel'thalas");

-- Protection (tab 2)
INSERT INTO `playerbots_bis_gear` VALUES
(1, 2, 0, 50640, 'Warrior', 'Protection', 'Head', 'Broken Ram Skull Helm'),
(1, 2, 1, 50682, 'Warrior', 'Protection', 'Neck', 'Bile-Encrusted Medallion'),
(1, 2, 2, 51847, 'Warrior', 'Protection', 'Shoulders', 'Spaulders of the Blood Princes'),
(1, 2, 4, 51220, 'Warrior', 'Protection', 'Chest', "Sanctified Ymirjar Lord's Breastplate"),
(1, 2, 5, 50691, 'Warrior', 'Protection', 'Waist', 'Belt of Broken Bones'),
(1, 2, 6, 50612, 'Warrior', 'Protection', 'Legs', 'Legguards of Lost Hope'),
(1, 2, 7, 54579, 'Warrior', 'Protection', 'Feet', 'Treads of Impending Resurrection'),
(1, 2, 8, 51901, 'Warrior', 'Protection', 'Wrists', 'Gargoyle Spit Bracers'),
(1, 2, 9, 51222, 'Warrior', 'Protection', 'Hands', "Sanctified Ymirjar Lord's Handguards"),
(1, 2, 10, 50622, 'Warrior', 'Protection', 'Finger1', "Devium's Eternally Cold Ring"),
(1, 2, 11, 50404, 'Warrior', 'Protection', 'Finger2', 'Ashen Band of Endless Courage'),
(1, 2, 12, 54591, 'Warrior', 'Protection', 'Trinket1', 'Petrified Twilight Scale'),
(1, 2, 13, 50364, 'Warrior', 'Protection', 'Trinket2', "Sindragosa's Flawless Fang"),
(1, 2, 14, 50466, 'Warrior', 'Protection', 'Back', "Sentinel's Winter Cloak"),
(1, 2, 15, 50738, 'Warrior', 'Protection', 'MainHand', "Mithrios, Bronzebeard's Legacy"),
(1, 2, 16, 50729, 'Warrior', 'Protection', 'OffHand', 'Icecrown Glacial Wall'),
(1, 2, 17, 51834, 'Warrior', 'Protection', 'Ranged', "Dreamhunter's Carbine");

-- ============================================================
-- Paladin (2)
-- ============================================================
-- Holy (tab 0)
INSERT INTO `playerbots_bis_gear` VALUES
(2, 0, 0, 51272, 'Paladin', 'Holy', 'Head', 'Sanctified Lightsworn Headpiece'),
(2, 0, 1, 50182, 'Paladin', 'Holy', 'Neck', "Blood Queen's Crimson Choker"),
(2, 0, 2, 51273, 'Paladin', 'Holy', 'Shoulders', 'Sanctified Lightsworn Spaulders'),
(2, 0, 4, 50680, 'Paladin', 'Holy', 'Chest', 'Rot-Resistant Breastplate'),
(2, 0, 5, 54587, 'Paladin', 'Holy', 'Waist', 'Split Shape Belt'),
(2, 0, 6, 50694, 'Paladin', 'Holy', 'Legs', "Plaguebringer's Stained Pants"),
(2, 0, 7, 54586, 'Paladin', 'Holy', 'Feet', 'Foreshadow Steps'),
(2, 0, 8, 54582, 'Paladin', 'Holy', 'Wrists', 'Bracers of Fiery Night'),
(2, 0, 9, 50650, 'Paladin', 'Holy', 'Hands', "Fallen Lord's Handguards"),
(2, 0, 10, 50664, 'Paladin', 'Holy', 'Finger1', 'Ring of Rapid Ascent'),
(2, 0, 11, 50400, 'Paladin', 'Holy', 'Finger2', 'Ashen Band of Endless Wisdom'),
(2, 0, 12, 46051, 'Paladin', 'Holy', 'Trinket1', 'Meteorite Crystal'),
(2, 0, 13, 48724, 'Paladin', 'Holy', 'Trinket2', 'Talisman of Resurgence'),
(2, 0, 14, 54583, 'Paladin', 'Holy', 'Back', 'Cloak of Burning Dusk'),
(2, 0, 15, 46017, 'Paladin', 'Holy', 'MainHand', "Val'anyr, Hammer of Ancient Kings"),
(2, 0, 16, 50616, 'Paladin', 'Holy', 'OffHand', 'Bulwark of Smouldering Steel'),
(2, 0, 17, 40705, 'Paladin', 'Holy', 'Ranged', 'Libram of Renewal');

-- Protection (tab 1)
INSERT INTO `playerbots_bis_gear` VALUES
(2, 1, 0, 50640, 'Paladin', 'Protection', 'Head', 'Broken Ram Skull Helm'),
(2, 1, 1, 50682, 'Paladin', 'Protection', 'Neck', 'Bile-Encrusted Medallion'),
(2, 1, 2, 50660, 'Paladin', 'Protection', 'Shoulders', "Boneguard Commander's Pauldrons"),
(2, 1, 4, 51265, 'Paladin', 'Protection', 'Chest', 'Sanctified Lightsworn Chestguard'),
(2, 1, 5, 50991, 'Paladin', 'Protection', 'Waist', 'Verdigris Chain Belt'),
(2, 1, 6, 49904, 'Paladin', 'Protection', 'Legs', 'Pillars of Might'),
(2, 1, 7, 54579, 'Paladin', 'Protection', 'Feet', 'Treads of Impending Resurrection'),
(2, 1, 8, 51901, 'Paladin', 'Protection', 'Wrists', 'Gargoyle Spit Bracers'),
(2, 1, 9, 51267, 'Paladin', 'Protection', 'Hands', 'Sanctified Lightsworn Handguards'),
(2, 1, 10, 50622, 'Paladin', 'Protection', 'Finger1', "Devium's Eternally Cold Ring"),
(2, 1, 11, 50404, 'Paladin', 'Protection', 'Finger2', 'Ashen Band of Endless Courage'),
(2, 1, 12, 54591, 'Paladin', 'Protection', 'Trinket1', 'Petrified Twilight Scale'),
(2, 1, 13, 50364, 'Paladin', 'Protection', 'Trinket2', "Sindragosa's Flawless Fang"),
(2, 1, 14, 50466, 'Paladin', 'Protection', 'Back', "Sentinel's Winter Cloak"),
(2, 1, 15, 50738, 'Paladin', 'Protection', 'MainHand', "Mithrios, Bronzebeard's Legacy"),
(2, 1, 16, 50729, 'Paladin', 'Protection', 'OffHand', 'Icecrown Glacial Wall'),
(2, 1, 17, 50461, 'Paladin', 'Protection', 'Ranged', 'Libram of the Eternal Tower');

-- Retribution (tab 2)
INSERT INTO `playerbots_bis_gear` VALUES
(2, 2, 0, 51277, 'Paladin', 'Retribution', 'Head', 'Sanctified Lightsworn Helmet'),
(2, 2, 1, 54581, 'Paladin', 'Retribution', 'Neck', 'Penumbra Pendant'),
(2, 2, 2, 51279, 'Paladin', 'Retribution', 'Shoulders', 'Sanctified Lightsworn Shoulderplates'),
(2, 2, 4, 51275, 'Paladin', 'Retribution', 'Chest', 'Sanctified Lightsworn Battleplate'),
(2, 2, 5, 50707, 'Paladin', 'Retribution', 'Waist', "Astrylian's Sutured Cinch"),
(2, 2, 6, 51278, 'Paladin', 'Retribution', 'Legs', 'Sanctified Lightsworn Legplates'),
(2, 2, 7, 54578, 'Paladin', 'Retribution', 'Feet', "Apocalypse's Advance"),
(2, 2, 8, 54580, 'Paladin', 'Retribution', 'Wrists', 'Umbrage Armbands'),
(2, 2, 9, 50690, 'Paladin', 'Retribution', 'Hands', 'Fleshrending Gauntlets'),
(2, 2, 10, 50402, 'Paladin', 'Retribution', 'Finger1', 'Ashen Band of Endless Vengeance'),
(2, 2, 11, 54576, 'Paladin', 'Retribution', 'Finger2', 'Signet of Twilight'),
(2, 2, 12, 54590, 'Paladin', 'Retribution', 'Trinket1', 'Sharpened Twilight Scale'),
(2, 2, 13, 50706, 'Paladin', 'Retribution', 'Trinket2', 'Tiny Abomination in a Jar'),
(2, 2, 14, 50653, 'Paladin', 'Retribution', 'Back', "Shadowvault Slayer's Cloak"),
(2, 2, 15, 49623, 'Paladin', 'Retribution', 'MainHand', 'Shadowmourne'),
(2, 2, 17, 50455, 'Paladin', 'Retribution', 'Ranged', 'Libram of Three Truths');

-- ============================================================
-- Hunter (3) - Marksmanship (tab 1)
-- ============================================================
INSERT INTO `playerbots_bis_gear` VALUES
(3, 1, 0, 51286, 'Hunter', 'Marksmanship', 'Head', "Sanctified Ahn'Kahar Blood Hunter's Headpiece"),
(3, 1, 1, 50633, 'Hunter', 'Marksmanship', 'Neck', "Sindragosa's Cruel Claw"),
(3, 1, 2, 51288, 'Hunter', 'Marksmanship', 'Shoulders', "Sanctified Ahn'Kahar Blood Hunter's Spaulders"),
(3, 1, 4, 51289, 'Hunter', 'Marksmanship', 'Chest', "Sanctified Ahn'Kahar Blood Hunter's Tunic"),
(3, 1, 5, 50688, 'Hunter', 'Marksmanship', 'Waist', "Nerub'ar Stalker's Cord"),
(3, 1, 6, 50645, 'Hunter', 'Marksmanship', 'Legs', 'Leggings of Northern Lights'),
(3, 1, 7, 54577, 'Hunter', 'Marksmanship', 'Feet', 'Returning Footfalls'),
(3, 1, 8, 50655, 'Hunter', 'Marksmanship', 'Wrists', "Scourge Hunter's Vambraces"),
(3, 1, 9, 51285, 'Hunter', 'Marksmanship', 'Hands', "Sanctified Ahn'Kahar Blood Hunter's Handguards"),
(3, 1, 10, 54576, 'Hunter', 'Marksmanship', 'Finger1', 'Signet of Twilight'),
(3, 1, 11, 50402, 'Hunter', 'Marksmanship', 'Finger2', 'Ashen Band of Endless Vengeance'),
(3, 1, 12, 50363, 'Hunter', 'Marksmanship', 'Trinket1', "Deathbringer's Will"),
(3, 1, 13, 54590, 'Hunter', 'Marksmanship', 'Trinket2', 'Sharpened Twilight Scale'),
(3, 1, 14, 47545, 'Hunter', 'Marksmanship', 'Back', "Vereesa's Dexterity"),
(3, 1, 15, 50735, 'Hunter', 'Marksmanship', 'MainHand', 'Oathbinder, Charge of the Ranger-General'),
(3, 1, 17, 50733, 'Hunter', 'Marksmanship', 'Ranged', "Fal'inrush, Defender of Quel'thalas");

-- ============================================================
-- Rogue (4)
-- ============================================================
-- Assassination (tab 0)
INSERT INTO `playerbots_bis_gear` VALUES
(4, 0, 0, 50713, 'Rogue', 'Assassination', 'Head', "Geistlord's Punishment Sack"),
(4, 0, 1, 50633, 'Rogue', 'Assassination', 'Neck', "Sindragosa's Cruel Claw"),
(4, 0, 2, 50646, 'Rogue', 'Assassination', 'Shoulders', "Cultist's Bloodsoaked Spaulders"),
(4, 0, 4, 50656, 'Rogue', 'Assassination', 'Chest', "Ikfirus's Sack of Wonder"),
(4, 0, 5, 47112, 'Rogue', 'Assassination', 'Waist', 'Belt of the Merciless Killer'),
(4, 0, 6, 51253, 'Rogue', 'Assassination', 'Legs', 'Sanctified Shadowblade Legplates'),
(4, 0, 7, 50607, 'Rogue', 'Assassination', 'Feet', 'Frostbitten Fur Boots'),
(4, 0, 8, 54580, 'Rogue', 'Assassination', 'Wrists', 'Umbrage Armbands'),
(4, 0, 9, 51251, 'Rogue', 'Assassination', 'Hands', 'Sanctified Shadowblade Gauntlets'),
(4, 0, 10, 50402, 'Rogue', 'Assassination', 'Finger1', 'Ashen Band of Endless Vengeance'),
(4, 0, 11, 54576, 'Rogue', 'Assassination', 'Finger2', 'Signet of Twilight'),
(4, 0, 12, 54590, 'Rogue', 'Assassination', 'Trinket1', 'Sharpened Twilight Scale'),
(4, 0, 13, 50706, 'Rogue', 'Assassination', 'Trinket2', 'Tiny Abomination in a Jar'),
(4, 0, 14, 50653, 'Rogue', 'Assassination', 'Back', "Shadowvault Slayer's Cloak"),
(4, 0, 15, 50736, 'Rogue', 'Assassination', 'MainHand', "Heaven's Fall, Kryss of a Thousand Lies"),
(4, 0, 16, 50621, 'Rogue', 'Assassination', 'OffHand', 'Lungbreaker'),
(4, 0, 17, 50733, 'Rogue', 'Assassination', 'Ranged', "Fal'inrush, Defender of Quel'thalas");

-- Combat (tab 1)
INSERT INTO `playerbots_bis_gear` VALUES
(4, 1, 0, 51252, 'Rogue', 'Combat', 'Head', 'Sanctified Shadowblade Helmet'),
(4, 1, 1, 50633, 'Rogue', 'Combat', 'Neck', "Sindragosa's Cruel Claw"),
(4, 1, 2, 51254, 'Rogue', 'Combat', 'Shoulders', 'Sanctified Shadowblade Pauldrons'),
(4, 1, 4, 51250, 'Rogue', 'Combat', 'Chest', 'Sanctified Shadowblade Breastplate'),
(4, 1, 5, 50707, 'Rogue', 'Combat', 'Waist', "Astrylian's Sutured Cinch"),
(4, 1, 6, 51253, 'Rogue', 'Combat', 'Legs', 'Sanctified Shadowblade Legplates'),
(4, 1, 7, 50607, 'Rogue', 'Combat', 'Feet', 'Frostbitten Fur Boots'),
(4, 1, 8, 50670, 'Rogue', 'Combat', 'Wrists', "Toskk's Maximized Wristguards"),
(4, 1, 9, 50675, 'Rogue', 'Combat', 'Hands', "Aldriana's Gloves of Secrecy"),
(4, 1, 10, 50402, 'Rogue', 'Combat', 'Finger1', 'Ashen Band of Endless Vengeance'),
(4, 1, 11, 50618, 'Rogue', 'Combat', 'Finger2', 'Frostbrood Sapphire Ring'),
(4, 1, 12, 50363, 'Rogue', 'Combat', 'Trinket1', "Deathbringer's Will"),
(4, 1, 13, 54590, 'Rogue', 'Combat', 'Trinket2', 'Sharpened Twilight Scale'),
(4, 1, 14, 47545, 'Rogue', 'Combat', 'Back', "Vereesa's Dexterity"),
(4, 1, 15, 50737, 'Rogue', 'Combat', 'MainHand', "Havoc's Call, Blade of Lordaeron Kings"),
(4, 1, 16, 50654, 'Rogue', 'Combat', 'OffHand', 'Scourgeborne Waraxe'),
(4, 1, 17, 50733, 'Rogue', 'Combat', 'Ranged', "Fal'inrush, Defender of Quel'thalas");

-- ============================================================
-- Priest (5)
-- ============================================================
-- Discipline (tab 0)
INSERT INTO `playerbots_bis_gear` VALUES
(5, 0, 0, 51261, 'Priest', 'Discipline', 'Head', 'Sanctified Crimson Acolyte Hood'),
(5, 0, 1, 50182, 'Priest', 'Discipline', 'Neck', "Blood Queen's Crimson Choker"),
(5, 0, 2, 51264, 'Priest', 'Discipline', 'Shoulders', 'Sanctified Crimson Acolyte Shoulderpads'),
(5, 0, 4, 51263, 'Priest', 'Discipline', 'Chest', 'Sanctified Crimson Acolyte Robe'),
(5, 0, 5, 50613, 'Priest', 'Discipline', 'Waist', 'Crushing Coldwraith Belt'),
(5, 0, 6, 51262, 'Priest', 'Discipline', 'Legs', 'Sanctified Crimson Acolyte Leggings'),
(5, 0, 7, 50699, 'Priest', 'Discipline', 'Feet', "Plague Scientist's Boots"),
(5, 0, 8, 54582, 'Priest', 'Discipline', 'Wrists', 'Bracers of Fiery Night'),
(5, 0, 9, 50722, 'Priest', 'Discipline', 'Hands', "San'layn Ritualist Gloves"),
(5, 0, 10, 50644, 'Priest', 'Discipline', 'Finger1', 'Ring of Maddening Whispers'),
(5, 0, 11, 50636, 'Priest', 'Discipline', 'Finger2', 'Memory of Malygos'),
(5, 0, 12, 50366, 'Priest', 'Discipline', 'Trinket1', "Althor's Abacus"),
(5, 0, 13, 54589, 'Priest', 'Discipline', 'Trinket2', 'Glowing Twilight Scale'),
(5, 0, 14, 54583, 'Priest', 'Discipline', 'Back', 'Cloak of Burning Dusk'),
(5, 0, 15, 50734, 'Priest', 'Discipline', 'MainHand', 'Royal Scepter of Terenas II'),
(5, 0, 16, 50719, 'Priest', 'Discipline', 'OffHand', 'Shadow Silk Spindle'),
(5, 0, 17, 50631, 'Priest', 'Discipline', 'Ranged', 'Nightmare Ender');

-- Holy (tab 1)
INSERT INTO `playerbots_bis_gear` VALUES
(5, 1, 0, 51261, 'Priest', 'Holy', 'Head', 'Sanctified Crimson Acolyte Hood'),
(5, 1, 1, 50609, 'Priest', 'Holy', 'Neck', "Bone Sentinel's Amulet"),
(5, 1, 2, 51264, 'Priest', 'Holy', 'Shoulders', 'Sanctified Crimson Acolyte Shoulderpads'),
(5, 1, 4, 50717, 'Priest', 'Holy', 'Chest', 'Sanguine Silk Robes'),
(5, 1, 5, 50702, 'Priest', 'Holy', 'Waist', 'Lingering Illness'),
(5, 1, 6, 51262, 'Priest', 'Holy', 'Legs', 'Sanctified Crimson Acolyte Leggings'),
(5, 1, 7, 50699, 'Priest', 'Holy', 'Feet', "Plague Scientist's Boots"),
(5, 1, 8, 54582, 'Priest', 'Holy', 'Wrists', 'Bracers of Fiery Night'),
(5, 1, 9, 51260, 'Priest', 'Holy', 'Hands', 'Sanctified Crimson Acolyte Gloves'),
(5, 1, 10, 50400, 'Priest', 'Holy', 'Finger1', 'Ashen Band of Endless Wisdom'),
(5, 1, 11, 50636, 'Priest', 'Holy', 'Finger2', 'Memory of Malygos'),
(5, 1, 12, 50366, 'Priest', 'Holy', 'Trinket1', "Althor's Abacus"),
(5, 1, 13, 54589, 'Priest', 'Holy', 'Trinket2', 'Glowing Twilight Scale'),
(5, 1, 14, 50668, 'Priest', 'Holy', 'Back', 'Greatcloak of the Turned Champion'),
(5, 1, 15, 50731, 'Priest', 'Holy', 'MainHand', 'Archus, Greatstaff of Antonidas'),
(5, 1, 17, 50631, 'Priest', 'Holy', 'Ranged', 'Nightmare Ender');

-- Shadow (tab 2)
INSERT INTO `playerbots_bis_gear` VALUES
(5, 2, 0, 51255, 'Priest', 'Shadow', 'Head', 'Sanctified Crimson Acolyte Cowl'),
(5, 2, 1, 50182, 'Priest', 'Shadow', 'Neck', "Blood Queen's Crimson Choker"),
(5, 2, 2, 51257, 'Priest', 'Shadow', 'Shoulders', 'Sanctified Crimson Acolyte Mantle'),
(5, 2, 4, 51259, 'Priest', 'Shadow', 'Chest', 'Sanctified Crimson Acolyte Raiments'),
(5, 2, 5, 50613, 'Priest', 'Shadow', 'Waist', 'Crushing Coldwraith Belt'),
(5, 2, 6, 50694, 'Priest', 'Shadow', 'Legs', "Plaguebringer's Stained Pants"),
(5, 2, 7, 50699, 'Priest', 'Shadow', 'Feet', "Plague Scientist's Boots"),
(5, 2, 8, 54582, 'Priest', 'Shadow', 'Wrists', 'Bracers of Fiery Night'),
(5, 2, 9, 51256, 'Priest', 'Shadow', 'Hands', 'Sanctified Crimson Acolyte Handwraps'),
(5, 2, 10, 50664, 'Priest', 'Shadow', 'Finger1', 'Ring of Rapid Ascent'),
(5, 2, 11, 50398, 'Priest', 'Shadow', 'Finger2', 'Ashen Band of Endless Destruction'),
(5, 2, 12, 54588, 'Priest', 'Shadow', 'Trinket1', 'Charred Twilight Scale'),
(5, 2, 13, 50348, 'Priest', 'Shadow', 'Trinket2', 'Dislodged Foreign Object'),
(5, 2, 14, 54583, 'Priest', 'Shadow', 'Back', 'Cloak of Burning Dusk'),
(5, 2, 15, 50734, 'Priest', 'Shadow', 'MainHand', 'Royal Scepter of Terenas II'),
(5, 2, 16, 50719, 'Priest', 'Shadow', 'OffHand', 'Shadow Silk Spindle'),
(5, 2, 17, 50684, 'Priest', 'Shadow', 'Ranged', 'Corpse-Impaling Spike');

-- ============================================================
-- Death Knight (6)
-- ============================================================
-- Blood Tank (tab 0)
INSERT INTO `playerbots_bis_gear` VALUES
(6, 0, 0, 51306, 'DeathKnight', 'BloodTank', 'Head', 'Sanctified Scourgelord Faceguard'),
(6, 0, 1, 50682, 'DeathKnight', 'BloodTank', 'Neck', 'Bile-Encrusted Medallion'),
(6, 0, 2, 51309, 'DeathKnight', 'BloodTank', 'Shoulders', 'Sanctified Scourgelord Pauldrons'),
(6, 0, 4, 51305, 'DeathKnight', 'BloodTank', 'Chest', 'Sanctified Scourgelord Chestguard'),
(6, 0, 5, 50991, 'DeathKnight', 'BloodTank', 'Waist', 'Verdigris Chain Belt'),
(6, 0, 6, 50612, 'DeathKnight', 'BloodTank', 'Legs', 'Legguards of Lost Hope'),
(6, 0, 7, 54579, 'DeathKnight', 'BloodTank', 'Feet', 'Treads of Impending Resurrection'),
(6, 0, 8, 51901, 'DeathKnight', 'BloodTank', 'Wrists', 'Gargoyle Spit Bracers'),
(6, 0, 9, 51307, 'DeathKnight', 'BloodTank', 'Hands', 'Sanctified Scourgelord Handguards'),
(6, 0, 10, 50622, 'DeathKnight', 'BloodTank', 'Finger1', "Devium's Eternally Cold Ring"),
(6, 0, 11, 50404, 'DeathKnight', 'BloodTank', 'Finger2', 'Ashen Band of Endless Courage'),
(6, 0, 12, 54591, 'DeathKnight', 'BloodTank', 'Trinket1', 'Petrified Twilight Scale'),
(6, 0, 13, 50364, 'DeathKnight', 'BloodTank', 'Trinket2', "Sindragosa's Flawless Fang"),
(6, 0, 14, 50466, 'DeathKnight', 'BloodTank', 'Back', "Sentinel's Winter Cloak"),
(6, 0, 15, 49623, 'DeathKnight', 'BloodTank', 'MainHand', 'Shadowmourne'),
(6, 0, 17, 50462, 'DeathKnight', 'BloodTank', 'Ranged', 'Sigil of the Bone Gryphon');

-- Frost (tab 1) - DW Havoc's Call
INSERT INTO `playerbots_bis_gear` VALUES
(6, 1, 0, 51312, 'DeathKnight', 'Frost', 'Head', 'Sanctified Scourgelord Helmet'),
(6, 1, 1, 54581, 'DeathKnight', 'Frost', 'Neck', 'Penumbra Pendant'),
(6, 1, 2, 51314, 'DeathKnight', 'Frost', 'Shoulders', 'Sanctified Scourgelord Shoulderplates'),
(6, 1, 4, 51310, 'DeathKnight', 'Frost', 'Chest', 'Sanctified Scourgelord Battleplate'),
(6, 1, 5, 50620, 'DeathKnight', 'Frost', 'Waist', 'Coldwraith Links'),
(6, 1, 6, 51313, 'DeathKnight', 'Frost', 'Legs', 'Sanctified Scourgelord Legplates'),
(6, 1, 7, 54578, 'DeathKnight', 'Frost', 'Feet', "Apocalypse's Advance"),
(6, 1, 8, 50670, 'DeathKnight', 'Frost', 'Wrists', "Toskk's Maximized Wristguards"),
(6, 1, 9, 50690, 'DeathKnight', 'Frost', 'Hands', 'Fleshrending Gauntlets'),
(6, 1, 10, 50693, 'DeathKnight', 'Frost', 'Finger1', 'Might of Blight'),
(6, 1, 11, 52572, 'DeathKnight', 'Frost', 'Finger2', 'Ashen Band of Endless Might'),
(6, 1, 12, 54590, 'DeathKnight', 'Frost', 'Trinket1', 'Sharpened Twilight Scale'),
(6, 1, 13, 50363, 'DeathKnight', 'Frost', 'Trinket2', "Deathbringer's Will"),
(6, 1, 14, 50677, 'DeathKnight', 'Frost', 'Back', 'Winding Sheet'),
(6, 1, 15, 50737, 'DeathKnight', 'Frost', 'MainHand', "Havoc's Call, Blade of Lordaeron Kings"),
(6, 1, 16, 50737, 'DeathKnight', 'Frost', 'OffHand', "Havoc's Call, Blade of Lordaeron Kings"),
(6, 1, 17, 50459, 'DeathKnight', 'Frost', 'Ranged', 'Sigil of the Hanged Man');

-- Unholy (tab 2) - 2H Shadowmourne
INSERT INTO `playerbots_bis_gear` VALUES
(6, 2, 0, 51312, 'DeathKnight', 'Unholy', 'Head', 'Sanctified Scourgelord Helmet'),
(6, 2, 1, 54581, 'DeathKnight', 'Unholy', 'Neck', 'Penumbra Pendant'),
(6, 2, 2, 51314, 'DeathKnight', 'Unholy', 'Shoulders', 'Sanctified Scourgelord Shoulderplates'),
(6, 2, 4, 51310, 'DeathKnight', 'Unholy', 'Chest', 'Sanctified Scourgelord Battleplate'),
(6, 2, 5, 50620, 'DeathKnight', 'Unholy', 'Waist', 'Coldwraith Links'),
(6, 2, 6, 51313, 'DeathKnight', 'Unholy', 'Legs', 'Sanctified Scourgelord Legplates'),
(6, 2, 7, 54578, 'DeathKnight', 'Unholy', 'Feet', "Apocalypse's Advance"),
(6, 2, 8, 50670, 'DeathKnight', 'Unholy', 'Wrists', "Toskk's Maximized Wristguards"),
(6, 2, 9, 50690, 'DeathKnight', 'Unholy', 'Hands', 'Fleshrending Gauntlets'),
(6, 2, 10, 50693, 'DeathKnight', 'Unholy', 'Finger1', 'Might of Blight'),
(6, 2, 11, 52572, 'DeathKnight', 'Unholy', 'Finger2', 'Ashen Band of Endless Might'),
(6, 2, 12, 54590, 'DeathKnight', 'Unholy', 'Trinket1', 'Sharpened Twilight Scale'),
(6, 2, 13, 50363, 'DeathKnight', 'Unholy', 'Trinket2', "Deathbringer's Will"),
(6, 2, 14, 50677, 'DeathKnight', 'Unholy', 'Back', 'Winding Sheet'),
(6, 2, 15, 49623, 'DeathKnight', 'Unholy', 'MainHand', 'Shadowmourne'),
(6, 2, 17, 50459, 'DeathKnight', 'Unholy', 'Ranged', 'Sigil of the Hanged Man');

-- ============================================================
-- Shaman (7)
-- ============================================================
-- Elemental (tab 0)
INSERT INTO `playerbots_bis_gear` VALUES
(7, 0, 0, 51237, 'Shaman', 'Elemental', 'Head', "Sanctified Frost Witch's Helm"),
(7, 0, 1, 50182, 'Shaman', 'Elemental', 'Neck', "Blood Queen's Crimson Choker"),
(7, 0, 2, 51235, 'Shaman', 'Elemental', 'Shoulders', "Sanctified Frost Witch's Shoulderpads"),
(7, 0, 4, 51239, 'Shaman', 'Elemental', 'Chest', "Sanctified Frost Witch's Hauberk"),
(7, 0, 5, 54587, 'Shaman', 'Elemental', 'Waist', 'Split Shape Belt'),
(7, 0, 6, 50694, 'Shaman', 'Elemental', 'Legs', "Plaguebringer's Stained Pants"),
(7, 0, 7, 50699, 'Shaman', 'Elemental', 'Feet', "Plague Scientist's Boots"),
(7, 0, 8, 54582, 'Shaman', 'Elemental', 'Wrists', 'Bracers of Fiery Night'),
(7, 0, 9, 51238, 'Shaman', 'Elemental', 'Hands', "Sanctified Frost Witch's Gloves"),
(7, 0, 10, 50664, 'Shaman', 'Elemental', 'Finger1', 'Ring of Rapid Ascent'),
(7, 0, 11, 50398, 'Shaman', 'Elemental', 'Finger2', 'Ashen Band of Endless Destruction'),
(7, 0, 12, 54588, 'Shaman', 'Elemental', 'Trinket1', 'Charred Twilight Scale'),
(7, 0, 13, 50365, 'Shaman', 'Elemental', 'Trinket2', 'Phylactery of the Nameless Lich'),
(7, 0, 14, 54583, 'Shaman', 'Elemental', 'Back', 'Cloak of Burning Dusk'),
(7, 0, 15, 50734, 'Shaman', 'Elemental', 'MainHand', 'Royal Scepter of Terenas II'),
(7, 0, 16, 50616, 'Shaman', 'Elemental', 'OffHand', 'Bulwark of Smouldering Steel'),
(7, 0, 17, 50458, 'Shaman', 'Elemental', 'Ranged', "Bizuri's Totem of Shattered Ice");

-- Enhancement (tab 1)
INSERT INTO `playerbots_bis_gear` VALUES
(7, 1, 0, 51242, 'Shaman', 'Enhancement', 'Head', "Sanctified Frost Witch's Faceguard"),
(7, 1, 1, 50633, 'Shaman', 'Enhancement', 'Neck', "Sindragosa's Cruel Claw"),
(7, 1, 2, 51240, 'Shaman', 'Enhancement', 'Shoulders', "Sanctified Frost Witch's Shoulderguards"),
(7, 1, 4, 50689, 'Shaman', 'Enhancement', 'Chest', 'Carapace of Forgotten Kings'),
(7, 1, 5, 50688, 'Shaman', 'Enhancement', 'Waist', "Nerub'ar Stalker's Cord"),
(7, 1, 6, 51241, 'Shaman', 'Enhancement', 'Legs', "Sanctified Frost Witch's War-Kilt"),
(7, 1, 7, 54577, 'Shaman', 'Enhancement', 'Feet', 'Returning Footfalls'),
(7, 1, 8, 50655, 'Shaman', 'Enhancement', 'Wrists', "Scourge Hunter's Vambraces"),
(7, 1, 9, 51243, 'Shaman', 'Enhancement', 'Hands', "Sanctified Frost Witch's Grips"),
(7, 1, 10, 50678, 'Shaman', 'Enhancement', 'Finger1', 'Seal of Many Mouths'),
(7, 1, 11, 50402, 'Shaman', 'Enhancement', 'Finger2', 'Ashen Band of Endless Vengeance'),
(7, 1, 12, 50706, 'Shaman', 'Enhancement', 'Trinket1', 'Tiny Abomination in a Jar'),
(7, 1, 13, 54590, 'Shaman', 'Enhancement', 'Trinket2', 'Sharpened Twilight Scale'),
(7, 1, 14, 50653, 'Shaman', 'Enhancement', 'Back', "Shadowvault Slayer's Cloak"),
(7, 1, 15, 50737, 'Shaman', 'Enhancement', 'MainHand', "Havoc's Call, Blade of Lordaeron Kings"),
(7, 1, 16, 50737, 'Shaman', 'Enhancement', 'OffHand', "Havoc's Call, Blade of Lordaeron Kings"),
(7, 1, 17, 50463, 'Shaman', 'Enhancement', 'Ranged', 'Totem of the Avalanche');

-- Restoration (tab 2)
INSERT INTO `playerbots_bis_gear` VALUES
(7, 2, 0, 51247, 'Shaman', 'Restoration', 'Head', "Sanctified Frost Witch's Headpiece"),
(7, 2, 1, 50182, 'Shaman', 'Restoration', 'Neck', "Blood Queen's Crimson Choker"),
(7, 2, 2, 51245, 'Shaman', 'Restoration', 'Shoulders', "Sanctified Frost Witch's Spaulders"),
(7, 2, 4, 51249, 'Shaman', 'Restoration', 'Chest', "Sanctified Frost Witch's Tunic"),
(7, 2, 5, 54587, 'Shaman', 'Restoration', 'Waist', 'Split Shape Belt'),
(7, 2, 6, 51246, 'Shaman', 'Restoration', 'Legs', "Sanctified Frost Witch's Legguards"),
(7, 2, 7, 50699, 'Shaman', 'Restoration', 'Feet', "Plague Scientist's Boots"),
(7, 2, 8, 50687, 'Shaman', 'Restoration', 'Wrists', "Bloodsunder's Bracers"),
(7, 2, 9, 50703, 'Shaman', 'Restoration', 'Hands', 'Unclean Surgical Gloves'),
(7, 2, 10, 50664, 'Shaman', 'Restoration', 'Finger1', 'Ring of Rapid Ascent'),
(7, 2, 11, 50400, 'Shaman', 'Restoration', 'Finger2', 'Ashen Band of Endless Wisdom'),
(7, 2, 12, 50366, 'Shaman', 'Restoration', 'Trinket1', "Althor's Abacus"),
(7, 2, 13, 54589, 'Shaman', 'Restoration', 'Trinket2', 'Glowing Twilight Scale'),
(7, 2, 14, 54583, 'Shaman', 'Restoration', 'Back', 'Cloak of Burning Dusk'),
(7, 2, 15, 46017, 'Shaman', 'Restoration', 'MainHand', "Val'anyr, Hammer of Ancient Kings"),
(7, 2, 16, 50616, 'Shaman', 'Restoration', 'OffHand', 'Bulwark of Smouldering Steel'),
(7, 2, 17, 50458, 'Shaman', 'Restoration', 'Ranged', "Bizuri's Totem of Shattered Ice");

-- ============================================================
-- Mage (8) - source has Fire and Arcane only. Frost mirrors Arcane.
-- ============================================================
-- Arcane (tab 0)
INSERT INTO `playerbots_bis_gear` VALUES
(8, 0, 0, 51281, 'Mage', 'Arcane', 'Head', 'Sanctified Bloodmage Hood'),
(8, 0, 1, 50182, 'Mage', 'Arcane', 'Neck', "Blood Queen's Crimson Choker"),
(8, 0, 2, 51284, 'Mage', 'Arcane', 'Shoulders', 'Sanctified Bloodmage Shoulderpads'),
(8, 0, 4, 51283, 'Mage', 'Arcane', 'Chest', 'Sanctified Bloodmage Robe'),
(8, 0, 5, 50613, 'Mage', 'Arcane', 'Waist', 'Crushing Coldwraith Belt'),
(8, 0, 6, 50694, 'Mage', 'Arcane', 'Legs', "Plaguebringer's Stained Pants"),
(8, 0, 7, 50699, 'Mage', 'Arcane', 'Feet', "Plague Scientist's Boots"),
(8, 0, 8, 54582, 'Mage', 'Arcane', 'Wrists', 'Bracers of Fiery Night'),
(8, 0, 9, 51280, 'Mage', 'Arcane', 'Hands', 'Sanctified Bloodmage Gloves'),
(8, 0, 10, 50614, 'Mage', 'Arcane', 'Finger1', 'Loop of the Endless Labyrinth'),
(8, 0, 11, 50398, 'Mage', 'Arcane', 'Finger2', 'Ashen Band of Endless Destruction'),
(8, 0, 12, 54588, 'Mage', 'Arcane', 'Trinket1', 'Charred Twilight Scale'),
(8, 0, 13, 50348, 'Mage', 'Arcane', 'Trinket2', 'Dislodged Foreign Object'),
(8, 0, 14, 54583, 'Mage', 'Arcane', 'Back', 'Cloak of Burning Dusk'),
(8, 0, 15, 50732, 'Mage', 'Arcane', 'MainHand', "Bloodsurge, Kel'Thuzad's Blade of Agony"),
(8, 0, 16, 50719, 'Mage', 'Arcane', 'OffHand', 'Shadow Silk Spindle'),
(8, 0, 17, 50684, 'Mage', 'Arcane', 'Ranged', 'Corpse-Impaling Spike');

-- Fire (tab 1)
INSERT INTO `playerbots_bis_gear` VALUES
(8, 1, 0, 51281, 'Mage', 'Fire', 'Head', 'Sanctified Bloodmage Hood'),
(8, 1, 1, 50182, 'Mage', 'Fire', 'Neck', "Blood Queen's Crimson Choker"),
(8, 1, 2, 51284, 'Mage', 'Fire', 'Shoulders', 'Sanctified Bloodmage Shoulderpads'),
(8, 1, 4, 51283, 'Mage', 'Fire', 'Chest', 'Sanctified Bloodmage Robe'),
(8, 1, 5, 50613, 'Mage', 'Fire', 'Waist', 'Crushing Coldwraith Belt'),
(8, 1, 6, 50694, 'Mage', 'Fire', 'Legs', "Plaguebringer's Stained Pants"),
(8, 1, 7, 50699, 'Mage', 'Fire', 'Feet', "Plague Scientist's Boots"),
(8, 1, 8, 54582, 'Mage', 'Fire', 'Wrists', 'Bracers of Fiery Night'),
(8, 1, 9, 51280, 'Mage', 'Fire', 'Hands', 'Sanctified Bloodmage Gloves'),
(8, 1, 10, 50614, 'Mage', 'Fire', 'Finger1', 'Loop of the Endless Labyrinth'),
(8, 1, 11, 50398, 'Mage', 'Fire', 'Finger2', 'Ashen Band of Endless Destruction'),
(8, 1, 12, 54588, 'Mage', 'Fire', 'Trinket1', 'Charred Twilight Scale'),
(8, 1, 13, 50365, 'Mage', 'Fire', 'Trinket2', 'Phylactery of the Nameless Lich'),
(8, 1, 14, 54583, 'Mage', 'Fire', 'Back', 'Cloak of Burning Dusk'),
(8, 1, 15, 50732, 'Mage', 'Fire', 'MainHand', "Bloodsurge, Kel'Thuzad's Blade of Agony"),
(8, 1, 16, 50719, 'Mage', 'Fire', 'OffHand', 'Shadow Silk Spindle'),
(8, 1, 17, 50684, 'Mage', 'Fire', 'Ranged', 'Corpse-Impaling Spike');

-- Frost (tab 2) - mirrors Arcane
INSERT INTO `playerbots_bis_gear` VALUES
(8, 2, 0, 51281, 'Mage', 'Frost', 'Head', 'Sanctified Bloodmage Hood'),
(8, 2, 1, 50182, 'Mage', 'Frost', 'Neck', "Blood Queen's Crimson Choker"),
(8, 2, 2, 51284, 'Mage', 'Frost', 'Shoulders', 'Sanctified Bloodmage Shoulderpads'),
(8, 2, 4, 51283, 'Mage', 'Frost', 'Chest', 'Sanctified Bloodmage Robe'),
(8, 2, 5, 50613, 'Mage', 'Frost', 'Waist', 'Crushing Coldwraith Belt'),
(8, 2, 6, 50694, 'Mage', 'Frost', 'Legs', "Plaguebringer's Stained Pants"),
(8, 2, 7, 50699, 'Mage', 'Frost', 'Feet', "Plague Scientist's Boots"),
(8, 2, 8, 54582, 'Mage', 'Frost', 'Wrists', 'Bracers of Fiery Night'),
(8, 2, 9, 51280, 'Mage', 'Frost', 'Hands', 'Sanctified Bloodmage Gloves'),
(8, 2, 10, 50614, 'Mage', 'Frost', 'Finger1', 'Loop of the Endless Labyrinth'),
(8, 2, 11, 50398, 'Mage', 'Frost', 'Finger2', 'Ashen Band of Endless Destruction'),
(8, 2, 12, 54588, 'Mage', 'Frost', 'Trinket1', 'Charred Twilight Scale'),
(8, 2, 13, 50348, 'Mage', 'Frost', 'Trinket2', 'Dislodged Foreign Object'),
(8, 2, 14, 54583, 'Mage', 'Frost', 'Back', 'Cloak of Burning Dusk'),
(8, 2, 15, 50732, 'Mage', 'Frost', 'MainHand', "Bloodsurge, Kel'Thuzad's Blade of Agony"),
(8, 2, 16, 50719, 'Mage', 'Frost', 'OffHand', 'Shadow Silk Spindle'),
(8, 2, 17, 50684, 'Mage', 'Frost', 'Ranged', 'Corpse-Impaling Spike');

-- ============================================================
-- Warlock (9) - one shared list across all 3 specs.
-- ============================================================
-- Affliction (tab 0)
INSERT INTO `playerbots_bis_gear` VALUES
(9, 0, 0, 51231, 'Warlock', 'Affliction', 'Head', 'Sanctified Dark Coven Hood'),
(9, 0, 1, 50182, 'Warlock', 'Affliction', 'Neck', "Blood Queen's Crimson Choker"),
(9, 0, 2, 51234, 'Warlock', 'Affliction', 'Shoulders', 'Sanctified Dark Coven Shoulderpads'),
(9, 0, 4, 51233, 'Warlock', 'Affliction', 'Chest', 'Sanctified Dark Coven Robe'),
(9, 0, 5, 50613, 'Warlock', 'Affliction', 'Waist', 'Crushing Coldwraith Belt'),
(9, 0, 6, 50694, 'Warlock', 'Affliction', 'Legs', "Plaguebringer's Stained Pants"),
(9, 0, 7, 50699, 'Warlock', 'Affliction', 'Feet', "Plague Scientist's Boots"),
(9, 0, 8, 54582, 'Warlock', 'Affliction', 'Wrists', 'Bracers of Fiery Night'),
(9, 0, 9, 51230, 'Warlock', 'Affliction', 'Hands', 'Sanctified Dark Coven Gloves'),
(9, 0, 10, 50614, 'Warlock', 'Affliction', 'Finger1', 'Loop of the Endless Labyrinth'),
(9, 0, 11, 50664, 'Warlock', 'Affliction', 'Finger2', 'Ring of Rapid Ascent'),
(9, 0, 12, 54588, 'Warlock', 'Affliction', 'Trinket1', 'Charred Twilight Scale'),
(9, 0, 13, 50365, 'Warlock', 'Affliction', 'Trinket2', 'Phylactery of the Nameless Lich'),
(9, 0, 14, 54583, 'Warlock', 'Affliction', 'Back', 'Cloak of Burning Dusk'),
(9, 0, 15, 50732, 'Warlock', 'Affliction', 'MainHand', "Bloodsurge, Kel'Thuzad's Blade of Agony"),
(9, 0, 16, 50719, 'Warlock', 'Affliction', 'OffHand', 'Shadow Silk Spindle'),
(9, 0, 17, 50684, 'Warlock', 'Affliction', 'Ranged', 'Corpse-Impaling Spike');

-- Demonology (tab 1)
INSERT INTO `playerbots_bis_gear` VALUES
(9, 1, 0, 51231, 'Warlock', 'Demonology', 'Head', 'Sanctified Dark Coven Hood'),
(9, 1, 1, 50182, 'Warlock', 'Demonology', 'Neck', "Blood Queen's Crimson Choker"),
(9, 1, 2, 51234, 'Warlock', 'Demonology', 'Shoulders', 'Sanctified Dark Coven Shoulderpads'),
(9, 1, 4, 51233, 'Warlock', 'Demonology', 'Chest', 'Sanctified Dark Coven Robe'),
(9, 1, 5, 50613, 'Warlock', 'Demonology', 'Waist', 'Crushing Coldwraith Belt'),
(9, 1, 6, 50694, 'Warlock', 'Demonology', 'Legs', "Plaguebringer's Stained Pants"),
(9, 1, 7, 50699, 'Warlock', 'Demonology', 'Feet', "Plague Scientist's Boots"),
(9, 1, 8, 54582, 'Warlock', 'Demonology', 'Wrists', 'Bracers of Fiery Night'),
(9, 1, 9, 51230, 'Warlock', 'Demonology', 'Hands', 'Sanctified Dark Coven Gloves'),
(9, 1, 10, 50614, 'Warlock', 'Demonology', 'Finger1', 'Loop of the Endless Labyrinth'),
(9, 1, 11, 50664, 'Warlock', 'Demonology', 'Finger2', 'Ring of Rapid Ascent'),
(9, 1, 12, 54588, 'Warlock', 'Demonology', 'Trinket1', 'Charred Twilight Scale'),
(9, 1, 13, 50365, 'Warlock', 'Demonology', 'Trinket2', 'Phylactery of the Nameless Lich'),
(9, 1, 14, 54583, 'Warlock', 'Demonology', 'Back', 'Cloak of Burning Dusk'),
(9, 1, 15, 50732, 'Warlock', 'Demonology', 'MainHand', "Bloodsurge, Kel'Thuzad's Blade of Agony"),
(9, 1, 16, 50719, 'Warlock', 'Demonology', 'OffHand', 'Shadow Silk Spindle'),
(9, 1, 17, 50684, 'Warlock', 'Demonology', 'Ranged', 'Corpse-Impaling Spike');

-- Destruction (tab 2)
INSERT INTO `playerbots_bis_gear` VALUES
(9, 2, 0, 51231, 'Warlock', 'Destruction', 'Head', 'Sanctified Dark Coven Hood'),
(9, 2, 1, 50182, 'Warlock', 'Destruction', 'Neck', "Blood Queen's Crimson Choker"),
(9, 2, 2, 51234, 'Warlock', 'Destruction', 'Shoulders', 'Sanctified Dark Coven Shoulderpads'),
(9, 2, 4, 51233, 'Warlock', 'Destruction', 'Chest', 'Sanctified Dark Coven Robe'),
(9, 2, 5, 50613, 'Warlock', 'Destruction', 'Waist', 'Crushing Coldwraith Belt'),
(9, 2, 6, 50694, 'Warlock', 'Destruction', 'Legs', "Plaguebringer's Stained Pants"),
(9, 2, 7, 50699, 'Warlock', 'Destruction', 'Feet', "Plague Scientist's Boots"),
(9, 2, 8, 54582, 'Warlock', 'Destruction', 'Wrists', 'Bracers of Fiery Night'),
(9, 2, 9, 51230, 'Warlock', 'Destruction', 'Hands', 'Sanctified Dark Coven Gloves'),
(9, 2, 10, 50614, 'Warlock', 'Destruction', 'Finger1', 'Loop of the Endless Labyrinth'),
(9, 2, 11, 50664, 'Warlock', 'Destruction', 'Finger2', 'Ring of Rapid Ascent'),
(9, 2, 12, 54588, 'Warlock', 'Destruction', 'Trinket1', 'Charred Twilight Scale'),
(9, 2, 13, 50365, 'Warlock', 'Destruction', 'Trinket2', 'Phylactery of the Nameless Lich'),
(9, 2, 14, 54583, 'Warlock', 'Destruction', 'Back', 'Cloak of Burning Dusk'),
(9, 2, 15, 50732, 'Warlock', 'Destruction', 'MainHand', "Bloodsurge, Kel'Thuzad's Blade of Agony"),
(9, 2, 16, 50719, 'Warlock', 'Destruction', 'OffHand', 'Shadow Silk Spindle'),
(9, 2, 17, 50684, 'Warlock', 'Destruction', 'Ranged', 'Corpse-Impaling Spike');

-- ============================================================
-- Druid (11) - Balance=0, Feral Cat=1, Restoration=2, Feral Bear=10 (sentinel, runtime tank check).
-- ============================================================
-- Balance (tab 0)
INSERT INTO `playerbots_bis_gear` VALUES
(11, 0, 0, 51290, 'Druid', 'Balance', 'Head', 'Sanctified Lasherweave Cover'),
(11, 0, 1, 50182, 'Druid', 'Balance', 'Neck', "Blood Queen's Crimson Choker"),
(11, 0, 2, 51292, 'Druid', 'Balance', 'Shoulders', 'Sanctified Lasherweave Mantle'),
(11, 0, 4, 51294, 'Druid', 'Balance', 'Chest', 'Sanctified Lasherweave Vestment'),
(11, 0, 5, 50613, 'Druid', 'Balance', 'Waist', 'Crushing Coldwraith Belt'),
(11, 0, 6, 50694, 'Druid', 'Balance', 'Legs', "Plaguebringer's Stained Pants"),
(11, 0, 7, 50699, 'Druid', 'Balance', 'Feet', "Plague Scientist's Boots"),
(11, 0, 8, 54582, 'Druid', 'Balance', 'Wrists', 'Bracers of Fiery Night'),
(11, 0, 9, 51291, 'Druid', 'Balance', 'Hands', 'Sanctified Lasherweave Gloves'),
(11, 0, 10, 50398, 'Druid', 'Balance', 'Finger1', 'Ashen Band of Endless Destruction'),
(11, 0, 11, 50714, 'Druid', 'Balance', 'Finger2', "Valanar's Other Signet Ring"),
(11, 0, 12, 54588, 'Druid', 'Balance', 'Trinket1', 'Charred Twilight Scale'),
(11, 0, 13, 50365, 'Druid', 'Balance', 'Trinket2', 'Phylactery of the Nameless Lich'),
(11, 0, 14, 54583, 'Druid', 'Balance', 'Back', 'Cloak of Burning Dusk'),
(11, 0, 15, 50734, 'Druid', 'Balance', 'MainHand', 'Royal Scepter of Terenas II'),
(11, 0, 16, 50719, 'Druid', 'Balance', 'OffHand', 'Shadow Silk Spindle');

-- Feral Cat (tab 1)
INSERT INTO `playerbots_bis_gear` VALUES
(11, 1, 0, 51296, 'Druid', 'FeralCat', 'Head', 'Sanctified Lasherweave Headguard'),
(11, 1, 1, 50633, 'Druid', 'FeralCat', 'Neck', "Sindragosa's Cruel Claw"),
(11, 1, 2, 51299, 'Druid', 'FeralCat', 'Shoulders', 'Sanctified Lasherweave Shoulderpads'),
(11, 1, 4, 51298, 'Druid', 'FeralCat', 'Chest', 'Sanctified Lasherweave Raiment'),
(11, 1, 5, 50707, 'Druid', 'FeralCat', 'Waist', "Astrylian's Sutured Cinch"),
(11, 1, 6, 51297, 'Druid', 'FeralCat', 'Legs', 'Sanctified Lasherweave Legguards'),
(11, 1, 7, 50607, 'Druid', 'FeralCat', 'Feet', 'Frostbitten Fur Boots'),
(11, 1, 8, 50670, 'Druid', 'FeralCat', 'Wrists', "Toskk's Maximized Wristguards"),
(11, 1, 9, 50675, 'Druid', 'FeralCat', 'Hands', "Aldriana's Gloves of Secrecy"),
(11, 1, 10, 50618, 'Druid', 'FeralCat', 'Finger1', 'Frostbrood Sapphire Ring'),
(11, 1, 11, 50402, 'Druid', 'FeralCat', 'Finger2', 'Ashen Band of Endless Vengeance'),
(11, 1, 12, 50363, 'Druid', 'FeralCat', 'Trinket1', "Deathbringer's Will"),
(11, 1, 13, 54590, 'Druid', 'FeralCat', 'Trinket2', 'Sharpened Twilight Scale'),
(11, 1, 14, 50653, 'Druid', 'FeralCat', 'Back', "Shadowvault Slayer's Cloak"),
(11, 1, 15, 50735, 'Druid', 'FeralCat', 'MainHand', 'Oathbinder, Charge of the Ranger-General'),
(11, 1, 17, 50456, 'Druid', 'FeralCat', 'Ranged', 'Idol of the Crying Moon');

-- Restoration (tab 2)
INSERT INTO `playerbots_bis_gear` VALUES
(11, 2, 0, 51302, 'Druid', 'Restoration', 'Head', 'Sanctified Lasherweave Helmet'),
(11, 2, 1, 50609, 'Druid', 'Restoration', 'Neck', "Bone Sentinel's Amulet"),
(11, 2, 2, 51304, 'Druid', 'Restoration', 'Shoulders', 'Sanctified Lasherweave Pauldrons'),
(11, 2, 4, 50717, 'Druid', 'Restoration', 'Chest', 'Sanguine Silk Robes'),
(11, 2, 5, 50705, 'Druid', 'Restoration', 'Waist', "Professor's Bloodied Smock"),
(11, 2, 6, 51303, 'Druid', 'Restoration', 'Legs', 'Sanctified Lasherweave Legplates'),
(11, 2, 7, 50665, 'Druid', 'Restoration', 'Feet', 'Boots Of Unnatural Growth'),
(11, 2, 8, 54584, 'Druid', 'Restoration', 'Wrists', 'Phaseshifter Bracers'),
(11, 2, 9, 51301, 'Druid', 'Restoration', 'Hands', 'Sanctified Lasherweave Gauntlets'),
(11, 2, 10, 50400, 'Druid', 'Restoration', 'Finger1', 'Ashen Band of Endless Wisdom'),
(11, 2, 11, 50636, 'Druid', 'Restoration', 'Finger2', 'Memory of Malygos'),
(11, 2, 12, 47059, 'Druid', 'Restoration', 'Trinket1', 'Solace of the Defeated'),
(11, 2, 13, 54589, 'Druid', 'Restoration', 'Trinket2', 'Glowing Twilight Scale'),
(11, 2, 14, 50668, 'Druid', 'Restoration', 'Back', 'Greatcloak of the Turned Champion'),
(11, 2, 15, 46017, 'Druid', 'Restoration', 'MainHand', "Val'anyr, Hammer of Ancient Kings"),
(11, 2, 16, 50635, 'Druid', 'Restoration', 'OffHand', 'Sundial of Eternal Dusk'),
(11, 2, 17, 50454, 'Druid', 'Restoration', 'Ranged', 'Idol of the Black Willow');

-- Feral Bear / Feral Tank (sentinel tab 10) - resolved at runtime when bot has Tank strategy
INSERT INTO `playerbots_bis_gear` VALUES
(11, 10, 0, 51296, 'Druid', 'FeralBear', 'Head', 'Sanctified Lasherweave Headguard'),
(11, 10, 1, 50682, 'Druid', 'FeralBear', 'Neck', 'Bile-Encrusted Medallion'),
(11, 10, 2, 51299, 'Druid', 'FeralBear', 'Shoulders', 'Sanctified Lasherweave Shoulderpads'),
(11, 10, 4, 50656, 'Druid', 'FeralBear', 'Chest', "Ikfirus's Sack of Wonder"),
(11, 10, 5, 50707, 'Druid', 'FeralBear', 'Waist', "Astrylian's Sutured Cinch"),
(11, 10, 6, 51297, 'Druid', 'FeralBear', 'Legs', 'Sanctified Lasherweave Legguards'),
(11, 10, 7, 50607, 'Druid', 'FeralBear', 'Feet', 'Frostbitten Fur Boots'),
(11, 10, 8, 54580, 'Druid', 'FeralBear', 'Wrists', 'Umbrage Armbands'),
(11, 10, 9, 51295, 'Druid', 'FeralBear', 'Hands', 'Sanctified Lasherweave Handgrips'),
(11, 10, 10, 50622, 'Druid', 'FeralBear', 'Finger1', "Devium's Eternally Cold Ring"),
(11, 10, 11, 50404, 'Druid', 'FeralBear', 'Finger2', 'Ashen Band of Endless Courage'),
(11, 10, 12, 50364, 'Druid', 'FeralBear', 'Trinket1', "Sindragosa's Flawless Fang"),
(11, 10, 13, 50356, 'Druid', 'FeralBear', 'Trinket2', 'Corroded Skeleton Key'),
(11, 10, 14, 50466, 'Druid', 'FeralBear', 'Back', "Sentinel's Winter Cloak"),
(11, 10, 15, 50735, 'Druid', 'FeralBear', 'MainHand', 'Oathbinder, Charge of the Ranger-General'),
(11, 10, 17, 50456, 'Druid', 'FeralBear', 'Ranged', 'Idol of the Crying Moon');
