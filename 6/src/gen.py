src = """
        Love,
        Salt,
        Dream,
        Asparagus,
        Spinach,
        WillowBranches,
        SealedLetter,
        WatermelonSlices,
        Notebook,
        Sun,
        Moon,
        Earth,
        Heaven,
        Location,
        Logos,
        Kairos,
        Employment,
        Longing,
"""

process = """
(0) Love, Salt: Argument
(1) Love, Dream: Oedipus
(2) Love, Asparagus: Spear
(3) Love, Spinach: Leaf
(4) Love, WillowBranches: Tree
(5) Love, SealedLetter: Address
(6) Love, WatermelonSlices: Cucumber
(7) Love, Notebook: Journal
(8) Love, Sun: Star
(9) Love, Moon: Sattelite
(10) Love, Earth: 
(11) Love, Heaven: 
(12) Love, Location: 
(13) Love, Logos: 
(14) Love, Kairos: 
(15) Love, Employment: 
(16) Love, Longing: 
(17) Salt, Dream: 
(18) Salt, Asparagus: 
(19) Salt, Spinach: 
(20) Salt, WillowBranches: 
(21) Salt, SealedLetter: 
(22) Salt, WatermelonSlices: 
(23) Salt, Notebook: 
(24) Salt, Sun: 
(25) Salt, Moon: 
(26) Salt, Earth: 
(27) Salt, Heaven: 
(28) Salt, Location: 
(29) Salt, Logos: 
(30) Salt, Kairos: 
(31) Salt, Employment: 
(32) Salt, Longing: 
(33) Dream, Asparagus: 
(34) Dream, Spinach: 
(35) Dream, WillowBranches: 
(36) Dream, SealedLetter: 
(37) Dream, WatermelonSlices: 
(38) Dream, Notebook: 
(39) Dream, Sun: 
(40) Dream, Moon: 
(41) Dream, Earth: 
(42) Dream, Heaven: 
(43) Dream, Location: 
(44) Dream, Logos: 
(45) Dream, Kairos: 
(46) Dream, Employment: 
(47) Dream, Longing: 
(48) Asparagus, Spinach: 
(49) Asparagus, WillowBranches: 
(50) Asparagus, SealedLetter: 
(51) Asparagus, WatermelonSlices: 
(52) Asparagus, Notebook: 
(53) Asparagus, Sun: 
(54) Asparagus, Moon: 
(55) Asparagus, Earth: 
(56) Asparagus, Heaven: 
(57) Asparagus, Location: 
(58) Asparagus, Logos: 
(59) Asparagus, Kairos: 
(60) Asparagus, Employment: 
(61) Asparagus, Longing: 
(62) Spinach, WillowBranches: 
(63) Spinach, SealedLetter: 
(64) Spinach, WatermelonSlices: 
(65) Spinach, Notebook: 
(66) Spinach, Sun: 
(67) Spinach, Moon: 
(68) Spinach, Earth: 
(69) Spinach, Heaven: 
(70) Spinach, Location: 
(71) Spinach, Logos: 
(72) Spinach, Kairos: 
(73) Spinach, Employment: 
(74) Spinach, Longing: 
(75) WillowBranches, SealedLetter: 
(76) WillowBranches, WatermelonSlices: 
(77) WillowBranches, Notebook: 
(78) WillowBranches, Sun: 
(79) WillowBranches, Moon: 
(80) WillowBranches, Earth: 
(81) WillowBranches, Heaven: 
(82) WillowBranches, Location: 
(83) WillowBranches, Logos: 
(84) WillowBranches, Kairos: 
(85) WillowBranches, Employment: 
(86) WillowBranches, Longing: 
(87) SealedLetter, WatermelonSlices: 
(88) SealedLetter, Notebook: 
(89) SealedLetter, Sun: 
(90) SealedLetter, Moon: 
(91) SealedLetter, Earth: 
(92) SealedLetter, Heaven: 
(93) SealedLetter, Location: 
(94) SealedLetter, Logos: 
(95) SealedLetter, Kairos: 
(96) SealedLetter, Employment: 
(97) SealedLetter, Longing: 
(98) WatermelonSlices, Notebook: 
(99) WatermelonSlices, Sun: 
(100) WatermelonSlices, Moon: 
(101) WatermelonSlices, Earth: 
(102) WatermelonSlices, Heaven: 
(103) WatermelonSlices, Location: 
(104) WatermelonSlices, Logos: 
(105) WatermelonSlices, Kairos: 
(106) WatermelonSlices, Employment: 
(107) WatermelonSlices, Longing: 
(108) Notebook, Sun: 
(109) Notebook, Moon: 
(110) Notebook, Earth: 
(111) Notebook, Heaven: 
(112) Notebook, Location: 
(113) Notebook, Logos: 
(114) Notebook, Kairos: 
(115) Notebook, Employment: 
(116) Notebook, Longing: 
(117) Sun, Moon: 
(118) Sun, Earth: 
(119) Sun, Heaven: 
(120) Sun, Location: 
(121) Sun, Logos: 
(122) Sun, Kairos: 
(123) Sun, Employment: 
(124) Sun, Longing: 
(125) Moon, Earth: 
(126) Moon, Heaven: 
(127) Moon, Location: 
(128) Moon, Logos: 
(129) Moon, Kairos: 
(130) Moon, Employment: 
(131) Moon, Longing: 
(132) Earth, Heaven: 
(133) Earth, Location: 
(134) Earth, Logos: 
(135) Earth, Kairos: 
(136) Earth, Employment: 
(137) Earth, Longing: 
(138) Heaven, Location: 
(139) Heaven, Logos: 
(140) Heaven, Kairos: 
(141) Heaven, Employment: 
(142) Heaven, Longing: 
(143) Location, Logos: 
(144) Location, Kairos: 
(145) Location, Employment: 
(146) Location, Longing: 
(147) Logos, Kairos: 
(148) Logos, Employment: 
(149) Logos, Longing: 
(150) Kairos, Employment: 
(151) Kairos, Longing: 
(152) Employment, Longing: 
"""

def get_items(src):
    items = []
    for str in src.split(","):
        item = str.strip()
        if len(item):
            items.append(item)
    return items

def combinations_count(item_count):
    total_count = 0
    for i in range(item_count):
        total_count += i
    return total_count

def main():
    items = get_items(src)
    count = len(items)
    print(items)
    print(f"total combinations {combinations_count(count)}")
    combinations = []
    for i in range(count):
        item_a = items[i]
        for j in range(i + 1, count):
            item_b = items[j]
            combinations.append((item_a, item_b))
    for i in range(len(combinations)):
        print(f"({i}) {combinations[i][0]}, {combinations[i][1]}: ")



if __name__ == "__main__":
    main()