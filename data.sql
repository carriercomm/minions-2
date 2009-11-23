INSERT INTO rooms VALUES( 1, "Town Square", "Welcome to Minions Town Square. There is a sign that says bank pointing towards the northeast.",0,2,0,6,0,0,4,0,0,0);
INSERT INTO rooms VALUES( 2, "Main Street", "The bank is to the north and town square is southeast.",3,0,0,5,0,1,0,0,0,0);
INSERT INTO rooms VALUES( 3, "Minions Bank", "You see people busy with thier transactions here at the bank.",0,0,0,0,2,0,0,0,0,0);
INSERT INTO rooms VALUES( 4, "Dark Alley", "This alley provides the perfect place to beat and rob people.",0,0,1,0,0,0,0,0,0,0);
INSERT INTO rooms VALUES( 5, "Sanguine Street", "This street breaks from main street to the southeast.",0,0,0,0,0,0,0,2,0,0);
INSERT INTO rooms VALUES( 6, "Narrow Path", "This narrow path leads to town square northeast and it continues to the south as well.",0,0,0,0,7,0,0,1,0,0);
INSERT INTO rooms VALUES( 7, "Narrow Path", "The path continues to the north leading towards town, or up a hill",6,0,0,0,0,0,0,0,8,0);
INSERT INTO rooms VALUES( 8, "Blue Tower - Ground Floor", "This strange blue tower has a cold smooth floor.",0,0,9,0,0,0,0,0,0,7);
INSERT INTO rooms VALUES( 9, "Blue Tower - Ground Floor", "An erry glow eminates from the room above.",0,0,0,0,0,0,8,0,10,0);
INSERT INTO rooms VALUES( 10, "Halls of the Dead", "The stale air and dank stone make it difficult to breathe. Bones lay piled, who knows how deep they are. Many have died here and more come each day.",0,0,0,0,0,0,0,0,0,9);

INSERT INTO races VALUES( 1, "Human", "The Human is the most balanced race at this time.");
INSERT INTO races VALUES( 2, "Dark-Elf", "The Dark-Elves are a darker relative of the Elf.");
INSERT INTO races VALUES( 3, "Dwarf", "The Dwarves are a stout and healthy race, what they lack in height they make up for with sturdiness and brawn.");
INSERT INTO races VALUES( 4, "Coward", "These sonzo bitches run like a mofo.");
INSERT INTO races VALUES( 5, "Feline", "This race doesnt give a shit about anything but themselves.");
INSERT INTO races VALUES( 6, "Crap Lord", "These race really stinks.");


INSERT INTO classes VALUES( 1, "Fighter", "Physical Combat is thier forte.");
INSERT INTO classes VALUES( 2, "Ranger", "Drawn to the woodlands and thier protection, Rangers gain special powers from Nature.");
INSERT INTO classes VALUES( 3, "Cleric", "Physical Combat is thier holy calling, and healing thier forte.");
INSERT INTO classes VALUES( 4, "Mage", "Frail physically but able to concentrate the mysterious energies into deadly forces.");
INSERT INTO classes VALUES( 5, "Paladin", "Physical Combat is thier holy calling, and healing thier forte.");
INSERT INTO classes VALUES( 6, "Priest", "Physical Combat is thier holy calling, and healing thier forte.");
INSERT INTO classes VALUES( 7, "Night Owl", "Physical Combat is thier holy calling, and healing thier forte.");

INSERT INTO items VALUES( 1, 30, 1, 25, "turd-tipped crozier", "This fine crozier is of a high quality. An immensely sharpened, pertrified turd forms a deady tip.", 15, 5, 3, 10, "penetrate", 7, 0, 0, 0, 0, 0, 0, 0, 9);
INSERT INTO items VALUES( 2, 30, 1, 25, "flappy titties", "These appear to be discarded breast implants of very low quality.", 12, 5, 3, 10, "flap", 5, 0, 0, 0, 0, 0, 0, 0, 9);
INSERT INTO items VALUES( 3, 30, 1, 25, "cowflop", "This steaming flop is a humiliating deathblow.", 20, 5, 3, 10, "slop", 10, 0, 0, 0, 0, 0, 0, 0, 9);
INSERT INTO items VALUES( 4, 30, 1, 25, "holy avenger", "This fine golden sword is intricately carved, and enchanted to deal trememdous damage!", 25, 8, 3, 10, "penetrate", 12, 0, 0, 0, 0, 0, 0, 0, 9);
INSERT INTO items VALUES( 5, 30, 2, 25, "rusty chainmail armor", "This rusty chainmail looks like it had been stripped off of a dead goblin. The wearer is better off dead.", 0, 0, 0, 5, "None", 0, 0, 0, 0, 0, 0, 0, 0, 3);
INSERT INTO items VALUES( 6, 10, 3, 500, "black cauldron", "This gigantic black cauldron was once used by trolls to cook great feasts made up of humans caught in the night!", 0, 0, 0, 0, "None", 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO items VALUES( 7, 10, 2, 15, "combat boots", "These nasty mud covered combat boots stink like nobody's bussiness, but they are better than nothing!", 0, 0, 0, 2, "None", 0, 0, 0, 0, 0, 0, 0, 0, 6);
INSERT INTO items VALUES( 8, 10, 2, 2, "polished black ring", "This black ring is made of lava glass. It is not known who made it or where it is from.", 0, 0, 0, 1, "None", 0, 50, 0, 0, 0, 0, 0, 0, 5);