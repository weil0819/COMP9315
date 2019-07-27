// gendata.c ... a main program to generate random tuples
// gendata.c ... generate random tuples
// part of Multi-attribute linear-hashed files
// Generates a list of K random tuples with N attributes
// Usage:  ./insert  [-v]  #attributes  #tuples
// Last modified by John Shepherd, July 2019

#include "defs.h"

#define USAGE "./insert  #tuples  #attributes  [startID]  [seed]"

// Main ... process args, read/insert tuples

int main(int argc, char **argv)
{
	int  natts;    // number of attributes in each tuple
	int  ntups;    // number of tuples
	int  startID;  // starting ID
	char err[MAXERRMSG]; // buffer for error messages

	// process command-line args

	if (argc < 3) fatal(USAGE);

	// how many tuples
	ntups = atoi(argv[1]);
	if (ntups < 1 || ntups > 100000) {
		sprintf(err, "Invalid #tuples: %d (must be 0 < # < 10^6)", ntups);
		fatal(err);
	}

	// how many attributes in each tuple
	natts = atoi(argv[2]);
	if (natts < 2 || natts > 10) {
		sprintf(err, "Invalid #attrs: %d (must be 1 < # < 11)", natts);
		fatal(err);
	}

	// set starting ID
	if (argc < 4)
		startID = 1;
	else
		startID = atoi(argv[3]);

	// seed random # generator
	if (argc < 5)
		srand(0);
	else
		srand(atoi(argv[4]));

	// reflects distribution of letter usage in english ... somewhat
	// id ensures that all tuples are distinct
	int i, j, id=startID;
	char attr[MAXTUPLEN];
	char tuple[MAXTUPLEN];
	char *randWord();
	for (i = 0; i < ntups; i++) {
		sprintf(tuple,"%d",id++);
		for (j = 0; j < natts-1; j++) {
			sprintf(attr,",%s",randWord());
			strcat(tuple,attr);
		}
		printf("%s\n",tuple);
	}

	return OK;
}

// based on a word-list from
// http://members.optusnet.com.au/charles57/Creative/Techniques/random_words.htm

char *words[251] =
{
"adult", "aeroplane", "air", "aircraft", "airforce", "airport", "album",
"alphabet", "apple", "arm", "army", "baby", "baby", "backpack", "balloon",
"banana", "bank", "barbecue", "bathroom", "bathtub", "bed", "bed", "bee",
"bird", "bomb", "book", "boss", "bottle", "bowl", "box", "boy", "brain",
"bridge", "butterfly", "button", "cappuccino", "car", "car-race", "carpet",
"carrot", "cat", "cave", "chair", "chess-board", "chief", "child", "chisel",
"chocolates", "church", "circle", "circus", "circus", "clock", "clown",
"coffee", "coffee-shop", "comet", "compact-disc", "compass", "computer",
"crystal", "cup", "cycle", "database", "desk", "diamond", "dingbat", "dog",
"double", "dress", "drill", "drink", "drum", "dung", "ears", "earth", "egg",
"electricity", "elephant", "eraser", "explosive", "eyes", "family", "famine",
"fan", "feather", "festival", "film", "fin", "finger", "fire", "floodlight",
"flower", "foot", "fork", "freeway", "fruit", "fungus", "game", "garden",
"gas", "gasp", "gate", "gemstone", "girl", "gloves", "grapes", "guitar",
"hammer", "hat", "hieroglyph", "highway", "horoscope", "horse", "hose","hot",
"ice", "ice-cream", "insect", "jet-fighter", "junk", "kaleidoscope", "key",
"kitchen", "knife", "leather", "leg", "library", "liquid", "magnet", "man",
"map", "maze", "meat", "meteor", "microscope", "milk", "milkshake", "mist",
"money", "monster", "mosquito", "mouth", "mum", "nail", "navy", "necklace",
"needle", "onion", "oodle", "paintbrush", "pants", "parachute", "passport",
"pebble", "pendulum", "pepper", "perfume", "pillow", "pin", "pith", "plane",
"planet", "pocket", "post", "potato", "printer", "prison", "pyramid", "radar",
"rainbow", "record", "restaurant", "rib", "rifle", "ring", "robot", "rock",
"rocket", "roof", "room", "rope", "saddle", "salt", "sandpaper", "sandwich",
"satellite", "school", "set", "ship", "shoes", "shop", "shower", "signature",
"skeleton", "slave", "snail", "software", "solid", "space", "spectrum",
"sphere", "spice", "spiral", "spoon", "sports-car", "spotlight", "square",
"staircase", "star", "stomach", "sun", "sunglasses", "surveyor", "swim",
"sword", "table", "tapestry", "teeth", "telescope", "television", "tennis",
"thermometer", "tiger", "toilet", "tongue", "torch", "torpedo", "train",
"treadmill", "triangle", "tunnel", "typewriter", "umbrella", "vacuum",
"vampire", "videotape", "vulture", "water", "weapon", "web", "wheelchair",
"win", "window", "woman", "worm", "x-ray", "yawn", "yellow", "zebra", "zoo"
};

char *randWord()
{
	return words[rand()%251];
}
