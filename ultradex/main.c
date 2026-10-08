#include <gint/display.h>
#include <gint/keyboard.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stddef.h>
#include "dex_data.h"
#include "ability_data.h"

typedef enum {
  T_NORMAL,T_FIRE,T_WATER,T_ELECTRIC,T_GRASS,T_ICE,T_FIGHTING,T_POISON,
  T_GROUND,T_FLYING,T_PSYCHIC,T_BUG,T_ROCK,T_GHOST,T_DRAGON,T_DARK,
  T_STEEL,T_FAIRY,T_NONE
} TypeId;

typedef struct {
  uint16_t id;
  const char *name;
  TypeId type;
  uint8_t cat;
  uint16_t power, acc;
  uint8_t pp;
  int8_t priority;
  const char *effect;
} Move;

typedef struct {
  uint16_t move_id;
  uint8_t level;
  uint8_t method;
} LearnMove;

typedef struct {
  uint16_t dex;
  const char *name;
  uint8_t gen;
  TypeId t1,t2;
  uint16_t hp,atk,def,spa,spd,spe;
  const char *abilities;
  const char *evo;
  const LearnMove *learn;
  uint8_t learn_n;
} Pokemon;

static const char *type_name(TypeId t) {
  static const char *n[]={"Normal","Fire","Water","Electric","Grass","Ice","Fighting","Poison",
    "Ground","Flying","Psychic","Bug","Rock","Ghost","Dragon","Dark","Steel","Fairy","-"};
  return ((unsigned)t<19)?n[t]:"-";
}

static const Move moves[] = {
 {1,"Pound",0,1,40,100,35,0,"Move details vary by game."},
 {2,"Karate Chop",6,1,50,100,25,0,"Move details vary by game."},
 {3,"Double Slap",0,1,15,85,10,0,"Move details vary by game."},
 {4,"Comet Punch",0,1,18,85,15,0,"Move details vary by game."},
 {5,"Mega Punch",0,1,80,85,20,0,"Move details vary by game."},
 {6,"Pay Day",0,1,40,100,20,0,"Move details vary by game."},
 {7,"Fire Punch",1,1,75,100,15,0,"Move details vary by game."},
 {8,"Ice Punch",5,1,75,100,15,0,"Move details vary by game."},
 {9,"Thunder Punch",3,1,75,100,15,0,"Move details vary by game."},
 {10,"Scratch",0,1,40,100,35,0,"Move details vary by game."},
 {11,"Vise Grip",0,1,55,100,30,0,"Move details vary by game."},
 {12,"Guillotine",0,1,0,30,5,0,"Move details vary by game."},
 {13,"Razor Wind",0,2,80,100,10,0,"Move details vary by game."},
 {14,"Swords Dance",0,0,0,0,20,0,"Move details vary by game."},
 {15,"Cut",0,1,50,95,30,0,"Move details vary by game."},
 {16,"Gust",9,2,40,100,35,0,"Move details vary by game."},
 {17,"Wing Attack",9,1,60,100,35,0,"Move details vary by game."},
 {18,"Whirlwind",0,0,0,0,20,0,"Move details vary by game."},
 {19,"Fly",9,1,90,95,15,0,"Move details vary by game."},
 {20,"Bind",0,1,15,85,20,0,"Move details vary by game."},
 {21,"Slam",0,1,80,75,20,0,"Move details vary by game."},
 {22,"Vine Whip",4,1,45,100,25,0,"Move details vary by game."},
 {23,"Stomp",0,1,65,100,20,0,"Move details vary by game."},
 {24,"Double Kick",6,1,30,100,30,0,"Move details vary by game."},
 {25,"Mega Kick",0,1,120,75,5,0,"Move details vary by game."},
 {26,"Jump Kick",6,1,100,95,10,0,"Move details vary by game."},
 {27,"Rolling Kick",6,1,60,85,15,0,"Move details vary by game."},
 {28,"Sand Attack",8,0,0,100,15,0,"Move details vary by game."},
 {29,"Headbutt",0,1,70,100,15,0,"Move details vary by game."},
 {30,"Horn Attack",0,1,65,100,25,0,"Move details vary by game."},
 {31,"Fury Attack",0,1,15,85,20,0,"Move details vary by game."},
 {32,"Horn Drill",0,1,0,30,5,0,"Move details vary by game."},
 {33,"Tackle",0,1,40,100,35,0,"Move details vary by game."},
 {34,"Body Slam",0,1,85,100,15,0,"Move details vary by game."},
 {35,"Wrap",0,1,15,90,20,0,"Move details vary by game."},
 {36,"Take Down",0,1,90,85,20,0,"Move details vary by game."},
 {37,"Thrash",0,1,120,100,10,0,"Move details vary by game."},
 {38,"Double-Edge",0,1,120,100,15,0,"Move details vary by game."},
 {39,"Tail Whip",0,0,0,100,30,0,"Move details vary by game."},
 {40,"Poison Sting",7,1,15,100,35,0,"Move details vary by game."},
 {41,"Twineedle",11,1,25,100,20,0,"Move details vary by game."},
 {42,"Pin Missile",11,1,25,95,20,0,"Move details vary by game."},
 {43,"Leer",0,0,0,100,30,0,"Move details vary by game."},
 {44,"Bite",15,1,60,100,25,0,"Move details vary by game."},
 {45,"Growl",0,0,0,100,40,0,"Move details vary by game."},
 {46,"Roar",0,0,0,0,20,0,"Move details vary by game."},
 {47,"Sing",0,0,0,55,15,0,"Move details vary by game."},
 {48,"Supersonic",0,0,0,55,20,0,"Move details vary by game."},
 {49,"Sonic Boom",0,2,0,90,20,0,"Move details vary by game."},
 {50,"Disable",0,0,0,100,20,0,"Move details vary by game."},
 {51,"Acid",7,2,40,100,30,0,"Move details vary by game."},
 {52,"Ember",1,2,40,100,25,0,"Move details vary by game."},
 {53,"Flamethrower",1,2,90,100,15,0,"Move details vary by game."},
 {54,"Mist",5,0,0,0,30,0,"Move details vary by game."},
 {55,"Water Gun",2,2,40,100,25,0,"Move details vary by game."},
 {56,"Hydro Pump",2,2,110,80,5,0,"Move details vary by game."},
 {57,"Surf",2,2,90,100,15,0,"Move details vary by game."},
 {58,"Ice Beam",5,2,90,100,10,0,"Move details vary by game."},
 {59,"Blizzard",5,2,110,70,5,0,"Move details vary by game."},
 {60,"Psybeam",10,2,65,100,20,0,"Move details vary by game."},
 {61,"Bubble Beam",2,2,65,100,20,0,"Move details vary by game."},
 {62,"Aurora Beam",5,2,65,100,20,0,"Move details vary by game."},
 {63,"Hyper Beam",0,2,150,90,5,0,"Move details vary by game."},
 {64,"Peck",9,1,35,100,35,0,"Move details vary by game."},
 {65,"Drill Peck",9,1,80,100,20,0,"Move details vary by game."},
 {66,"Submission",6,1,80,80,20,0,"Move details vary by game."},
 {67,"Low Kick",6,1,0,100,20,0,"Move details vary by game."},
 {68,"Counter",6,1,0,100,20,0,"Move details vary by game."},
 {69,"Seismic Toss",6,1,0,100,20,0,"Move details vary by game."},
 {70,"Strength",0,1,80,100,15,0,"Move details vary by game."},
 {71,"Absorb",4,2,20,100,25,0,"Move details vary by game."},
 {72,"Mega Drain",4,2,40,100,15,0,"Move details vary by game."},
 {73,"Leech Seed",4,0,0,90,10,0,"Move details vary by game."},
 {74,"Growth",0,0,0,0,20,0,"Move details vary by game."},
 {75,"Razor Leaf",4,1,55,95,25,0,"Move details vary by game."},
 {76,"Solar Beam",4,2,120,100,10,0,"Move details vary by game."},
 {77,"Poison Powder",7,0,0,75,35,0,"Move details vary by game."},
 {78,"Stun Spore",4,0,0,75,30,0,"Move details vary by game."},
 {79,"Sleep Powder",4,0,0,75,15,0,"Move details vary by game."},
 {80,"Petal Dance",4,2,120,100,10,0,"Move details vary by game."},
 {81,"String Shot",11,0,0,95,40,0,"Move details vary by game."},
 {82,"Dragon Rage",14,2,0,100,10,0,"Move details vary by game."},
 {83,"Fire Spin",1,2,35,85,15,0,"Move details vary by game."},
 {84,"Thunder Shock",3,2,40,100,30,0,"Move details vary by game."},
 {85,"Thunderbolt",3,2,90,100,15,0,"Move details vary by game."},
 {86,"Thunder Wave",3,0,0,90,20,0,"Move details vary by game."},
 {87,"Thunder",3,2,110,70,10,0,"Move details vary by game."},
 {88,"Rock Throw",12,1,50,90,15,0,"Move details vary by game."},
 {89,"Earthquake",8,1,100,100,10,0,"Move details vary by game."},
 {90,"Fissure",8,1,0,30,5,0,"Move details vary by game."},
 {91,"Dig",8,1,80,100,10,0,"Move details vary by game."},
 {92,"Toxic",7,0,0,90,10,0,"Move details vary by game."},
 {93,"Confusion",10,2,50,100,25,0,"Move details vary by game."},
 {94,"Psychic",10,2,90,100,10,0,"Move details vary by game."},
 {95,"Hypnosis",10,0,0,60,20,0,"Move details vary by game."},
 {96,"Meditate",10,0,0,0,40,0,"Move details vary by game."},
 {97,"Agility",10,0,0,0,30,0,"Move details vary by game."},
 {98,"Quick Attack",0,1,40,100,30,0,"Move details vary by game."},
 {99,"Rage",0,1,20,100,20,0,"Move details vary by game."},
 {100,"Teleport",10,0,0,0,20,0,"Move details vary by game."},
 {101,"Night Shade",13,2,0,100,15,0,"Move details vary by game."},
 {102,"Mimic",0,0,0,0,10,0,"Move details vary by game."},
 {103,"Screech",0,0,0,85,40,0,"Move details vary by game."},
 {104,"Double Team",0,0,0,0,15,0,"Move details vary by game."},
 {105,"Recover",0,0,0,0,5,0,"Move details vary by game."},
 {106,"Harden",0,0,0,0,30,0,"Move details vary by game."},
 {107,"Minimize",0,0,0,0,10,0,"Move details vary by game."},
 {108,"Smokescreen",0,0,0,100,20,0,"Move details vary by game."},
 {109,"Confuse Ray",13,0,0,100,10,0,"Move details vary by game."},
 {110,"Withdraw",2,0,0,0,40,0,"Move details vary by game."},
 {111,"Defense Curl",0,0,0,0,40,0,"Move details vary by game."},
 {112,"Barrier",10,0,0,0,20,0,"Move details vary by game."},
 {113,"Light Screen",10,0,0,0,30,0,"Move details vary by game."},
 {114,"Haze",5,0,0,0,30,0,"Move details vary by game."},
 {115,"Reflect",10,0,0,0,20,0,"Move details vary by game."},
 {116,"Focus Energy",0,0,0,0,30,0,"Move details vary by game."},
 {117,"Bide",0,1,0,0,10,0,"Move details vary by game."},
 {118,"Metronome",0,0,0,0,10,0,"Move details vary by game."},
 {119,"Mirror Move",9,0,0,0,20,0,"Move details vary by game."},
 {120,"Self-Destruct",0,1,200,100,5,0,"Move details vary by game."},
 {121,"Egg Bomb",0,1,100,75,10,0,"Move details vary by game."},
 {122,"Lick",13,1,30,100,30,0,"Move details vary by game."},
 {123,"Smog",7,2,30,70,20,0,"Move details vary by game."},
 {124,"Sludge",7,2,65,100,20,0,"Move details vary by game."},
 {125,"Bone Club",8,1,65,85,20,0,"Move details vary by game."},
 {126,"Fire Blast",1,2,110,85,5,0,"Move details vary by game."},
 {127,"Waterfall",2,1,80,100,15,0,"Move details vary by game."},
 {128,"Clamp",2,1,35,85,15,0,"Move details vary by game."},
 {129,"Swift",0,2,60,0,20,0,"Move details vary by game."},
 {130,"Skull Bash",0,1,130,100,10,0,"Move details vary by game."},
 {131,"Spike Cannon",0,1,20,100,15,0,"Move details vary by game."},
 {132,"Constrict",0,1,10,100,35,0,"Move details vary by game."},
 {133,"Amnesia",10,0,0,0,20,0,"Move details vary by game."},
 {134,"Kinesis",10,0,0,80,15,0,"Move details vary by game."},
 {135,"Soft-Boiled",0,0,0,0,5,0,"Move details vary by game."},
 {136,"High Jump Kick",6,1,130,90,10,0,"Move details vary by game."},
 {137,"Glare",0,0,0,100,30,0,"Move details vary by game."},
 {138,"Dream Eater",10,2,100,100,15,0,"Move details vary by game."},
 {139,"Poison Gas",7,0,0,90,40,0,"Move details vary by game."},
 {140,"Barrage",0,1,15,85,20,0,"Move details vary by game."},
 {141,"Leech Life",11,1,80,100,10,0,"Move details vary by game."},
 {142,"Lovely Kiss",0,0,0,75,10,0,"Move details vary by game."},
 {143,"Sky Attack",9,1,140,90,5,0,"Move details vary by game."},
 {144,"Transform",0,0,0,0,10,0,"Move details vary by game."},
 {145,"Bubble",2,2,40,100,30,0,"Move details vary by game."},
 {146,"Dizzy Punch",0,1,70,100,10,0,"Move details vary by game."},
 {147,"Spore",4,0,0,100,15,0,"Move details vary by game."},
 {148,"Flash",0,0,0,100,20,0,"Move details vary by game."},
 {149,"Psywave",10,2,0,100,15,0,"Move details vary by game."},
 {150,"Splash",0,0,0,0,40,0,"Move details vary by game."},
 {151,"Acid Armor",7,0,0,0,20,0,"Move details vary by game."},
 {152,"Crabhammer",2,1,100,90,10,0,"Move details vary by game."},
 {153,"Explosion",0,1,250,100,5,0,"Move details vary by game."},
 {154,"Fury Swipes",0,1,18,80,15,0,"Move details vary by game."},
 {155,"Bonemerang",8,1,50,90,10,0,"Move details vary by game."},
 {156,"Rest",10,0,0,0,5,0,"Move details vary by game."},
 {157,"Rock Slide",12,1,75,90,10,0,"Move details vary by game."},
 {158,"Hyper Fang",0,1,80,90,15,0,"Move details vary by game."},
 {159,"Sharpen",0,0,0,0,30,0,"Move details vary by game."},
 {160,"Conversion",0,0,0,0,30,0,"Move details vary by game."},
 {161,"Tri Attack",0,2,80,100,10,0,"Move details vary by game."},
 {162,"Super Fang",0,1,0,90,10,0,"Move details vary by game."},
 {163,"Slash",0,1,70,100,20,0,"Move details vary by game."},
 {164,"Substitute",0,0,0,0,10,0,"Move details vary by game."},
 {165,"Struggle",0,1,50,0,1,0,"Move details vary by game."},
 {166,"Sketch",0,0,0,0,1,0,"Move details vary by game."},
 {167,"Triple Kick",6,1,10,90,10,0,"Move details vary by game."},
 {168,"Thief",15,1,60,100,25,0,"Move details vary by game."},
 {169,"Spider Web",11,0,0,0,10,0,"Move details vary by game."},
 {170,"Mind Reader",0,0,0,0,5,0,"Move details vary by game."},
 {171,"Nightmare",13,0,0,100,15,0,"Move details vary by game."},
 {172,"Flame Wheel",1,1,60,100,25,0,"Move details vary by game."},
 {173,"Snore",0,2,50,100,15,0,"Move details vary by game."},
 {174,"Curse",13,0,0,0,10,0,"Move details vary by game."},
 {175,"Flail",0,1,0,100,15,0,"Move details vary by game."},
 {176,"Conversion 2",0,0,0,0,30,0,"Move details vary by game."},
 {177,"Aeroblast",9,2,100,95,5,0,"Move details vary by game."},
 {178,"Cotton Spore",4,0,0,100,40,0,"Move details vary by game."},
 {179,"Reversal",6,1,0,100,15,0,"Move details vary by game."},
 {180,"Spite",13,0,0,100,10,0,"Move details vary by game."},
 {181,"Powder Snow",5,2,40,100,25,0,"Move details vary by game."},
 {182,"Protect",0,0,0,0,10,0,"Move details vary by game."},
 {183,"Mach Punch",6,1,40,100,30,0,"Move details vary by game."},
 {184,"Scary Face",0,0,0,100,10,0,"Move details vary by game."},
 {185,"Feint Attack",15,1,60,0,20,0,"Move details vary by game."},
 {186,"Sweet Kiss",17,0,0,75,10,0,"Move details vary by game."},
 {187,"Belly Drum",0,0,0,0,10,0,"Move details vary by game."},
 {188,"Sludge Bomb",7,2,90,100,10,0,"Move details vary by game."},
 {189,"Mud-Slap",8,2,20,100,10,0,"Move details vary by game."},
 {190,"Octazooka",2,2,65,85,10,0,"Move details vary by game."},
 {191,"Spikes",8,0,0,0,20,0,"Move details vary by game."},
 {192,"Zap Cannon",3,2,120,50,5,0,"Move details vary by game."},
 {193,"Foresight",0,0,0,0,40,0,"Move details vary by game."},
 {194,"Destiny Bond",13,0,0,0,5,0,"Move details vary by game."},
 {195,"Perish Song",0,0,0,0,5,0,"Move details vary by game."},
 {196,"Icy Wind",5,2,55,95,15,0,"Move details vary by game."},
 {197,"Detect",6,0,0,0,5,0,"Move details vary by game."},
 {198,"Bone Rush",8,1,25,90,10,0,"Move details vary by game."},
 {199,"Lock-On",0,0,0,0,5,0,"Move details vary by game."},
 {200,"Outrage",14,1,120,100,10,0,"Move details vary by game."},
 {201,"Sandstorm",12,0,0,0,10,0,"Move details vary by game."},
 {202,"Giga Drain",4,2,75,100,10,0,"Move details vary by game."},
 {203,"Endure",0,0,0,0,10,0,"Move details vary by game."},
 {204,"Charm",17,0,0,100,20,0,"Move details vary by game."},
 {205,"Rollout",12,1,30,90,20,0,"Move details vary by game."},
 {206,"False Swipe",0,1,40,100,40,0,"Move details vary by game."},
 {207,"Swagger",0,0,0,85,15,0,"Move details vary by game."},
 {208,"Milk Drink",0,0,0,0,5,0,"Move details vary by game."},
 {209,"Spark",3,1,65,100,20,0,"Move details vary by game."},
 {210,"Fury Cutter",11,1,40,95,20,0,"Move details vary by game."},
 {211,"Steel Wing",16,1,70,90,25,0,"Move details vary by game."},
 {212,"Mean Look",0,0,0,0,5,0,"Move details vary by game."},
 {213,"Attract",0,0,0,100,15,0,"Move details vary by game."},
 {214,"Sleep Talk",0,0,0,0,10,0,"Move details vary by game."},
 {215,"Heal Bell",0,0,0,0,5,0,"Move details vary by game."},
 {216,"Return",0,1,0,100,20,0,"Move details vary by game."},
 {217,"Present",0,1,0,90,15,0,"Move details vary by game."},
 {218,"Frustration",0,1,0,100,20,0,"Move details vary by game."},
 {219,"Safeguard",0,0,0,0,25,0,"Move details vary by game."},
 {220,"Pain Split",0,0,0,0,20,0,"Move details vary by game."},
 {221,"Sacred Fire",1,1,100,95,5,0,"Move details vary by game."},
 {222,"Magnitude",8,1,0,100,30,0,"Move details vary by game."},
 {223,"Dynamic Punch",6,1,100,50,5,0,"Move details vary by game."},
 {224,"Megahorn",11,1,120,85,10,0,"Move details vary by game."},
 {225,"Dragon Breath",14,2,60,100,20,0,"Move details vary by game."},
 {226,"Baton Pass",0,0,0,0,40,0,"Move details vary by game."},
 {227,"Encore",0,0,0,100,5,0,"Move details vary by game."},
 {228,"Pursuit",15,1,40,100,20,0,"Move details vary by game."},
 {229,"Rapid Spin",0,1,50,100,40,0,"Move details vary by game."},
 {230,"Sweet Scent",0,0,0,100,20,0,"Move details vary by game."},
 {231,"Iron Tail",16,1,100,75,15,0,"Move details vary by game."},
 {232,"Metal Claw",16,1,50,95,35,0,"Move details vary by game."},
 {233,"Vital Throw",6,1,70,0,10,0,"Move details vary by game."},
 {234,"Morning Sun",0,0,0,0,5,0,"Move details vary by game."},
 {235,"Synthesis",4,0,0,0,5,0,"Move details vary by game."},
 {236,"Moonlight",17,0,0,0,5,0,"Move details vary by game."},
 {237,"Hidden Power Water",2,2,60,100,15,0,"Move details vary by game."},
 {238,"Cross Chop",6,1,100,80,5,0,"Move details vary by game."},
 {239,"Twister",14,2,40,100,20,0,"Move details vary by game."},
 {240,"Rain Dance",2,0,0,0,5,0,"Move details vary by game."},
 {241,"Sunny Day",1,0,0,0,5,0,"Move details vary by game."},
 {242,"Crunch",15,1,80,100,15,0,"Move details vary by game."},
 {243,"Mirror Coat",10,2,0,100,20,0,"Move details vary by game."},
 {244,"Psych Up",0,0,0,0,10,0,"Move details vary by game."},
 {245,"Extreme Speed",0,1,80,100,5,0,"Move details vary by game."},
 {246,"Ancient Power",12,2,60,100,5,0,"Move details vary by game."},
 {247,"Shadow Ball",13,2,80,100,15,0,"Move details vary by game."},
 {248,"Future Sight",10,2,120,100,10,0,"Move details vary by game."},
 {249,"Rock Smash",6,1,40,100,15,0,"Move details vary by game."},
 {250,"Whirlpool",2,2,35,85,15,0,"Move details vary by game."},
 {251,"Beat Up",15,1,0,100,10,0,"Move details vary by game."},
 {252,"Fake Out",0,1,40,100,10,0,"Move details vary by game."},
 {253,"Uproar",0,2,90,100,10,0,"Move details vary by game."},
 {254,"Stockpile",0,0,0,0,20,0,"Move details vary by game."},
 {255,"Spit Up",0,2,0,100,10,0,"Move details vary by game."},
 {256,"Swallow",0,0,0,0,10,0,"Move details vary by game."},
 {257,"Heat Wave",1,2,95,90,10,0,"Move details vary by game."},
 {258,"Hail",5,0,0,0,10,0,"Move details vary by game."},
 {259,"Torment",15,0,0,100,15,0,"Move details vary by game."},
 {260,"Flatter",15,0,0,100,15,0,"Move details vary by game."},
 {261,"Will-O-Wisp",1,0,0,85,15,0,"Move details vary by game."},
 {262,"Memento",15,0,0,100,10,0,"Move details vary by game."},
 {263,"Facade",0,1,70,100,20,0,"Move details vary by game."},
 {264,"Focus Punch",6,1,150,100,20,0,"Move details vary by game."},
 {265,"Smelling Salts",0,1,70,100,10,0,"Move details vary by game."},
 {266,"Follow Me",0,0,0,0,20,0,"Move details vary by game."},
 {267,"Nature Power",0,0,0,0,20,0,"Move details vary by game."},
 {268,"Charge",3,0,0,0,20,0,"Move details vary by game."},
 {269,"Taunt",15,0,0,100,20,0,"Move details vary by game."},
 {270,"Helping Hand",0,0,0,0,20,0,"Move details vary by game."},
 {271,"Trick",10,0,0,100,10,0,"Move details vary by game."},
 {272,"Role Play",10,0,0,0,10,0,"Move details vary by game."},
 {273,"Wish",0,0,0,0,10,0,"Move details vary by game."},
 {274,"Assist",0,0,0,0,20,0,"Move details vary by game."},
 {275,"Ingrain",4,0,0,0,20,0,"Move details vary by game."},
 {276,"Superpower",6,1,120,100,5,0,"Move details vary by game."},
 {277,"Magic Coat",10,0,0,0,15,0,"Move details vary by game."},
 {278,"Recycle",0,0,0,0,10,0,"Move details vary by game."},
 {279,"Revenge",6,1,60,100,10,0,"Move details vary by game."},
 {280,"Brick Break",6,1,75,100,15,0,"Move details vary by game."},
 {281,"Yawn",0,0,0,0,10,0,"Move details vary by game."},
 {282,"Knock Off",15,1,65,100,20,0,"Move details vary by game."},
 {283,"Endeavor",0,1,0,100,5,0,"Move details vary by game."},
 {284,"Eruption",1,2,150,100,5,0,"Move details vary by game."},
 {285,"Skill Swap",10,0,0,0,10,0,"Move details vary by game."},
 {286,"Imprison",10,0,0,0,10,0,"Move details vary by game."},
 {287,"Refresh",0,0,0,0,20,0,"Move details vary by game."},
 {288,"Grudge",13,0,0,0,5,0,"Move details vary by game."},
 {289,"Snatch",15,0,0,0,10,0,"Move details vary by game."},
 {290,"Secret Power",0,1,70,100,20,0,"Move details vary by game."},
 {291,"Dive",2,1,80,100,10,0,"Move details vary by game."},
 {292,"Arm Thrust",6,1,15,100,20,0,"Move details vary by game."},
 {293,"Camouflage",0,0,0,0,20,0,"Move details vary by game."},
 {294,"Tail Glow",11,0,0,0,20,0,"Move details vary by game."},
 {295,"Luster Purge",10,2,95,100,5,0,"Move details vary by game."},
 {296,"Mist Ball",10,2,95,100,5,0,"Move details vary by game."},
 {297,"Feather Dance",9,0,0,100,15,0,"Move details vary by game."},
 {298,"Teeter Dance",0,0,0,100,20,0,"Move details vary by game."},
 {299,"Blaze Kick",1,1,85,90,10,0,"Move details vary by game."},
 {300,"Mud Sport",8,0,0,0,15,0,"Move details vary by game."},
 {301,"Ice Ball",5,1,30,90,20,0,"Move details vary by game."},
 {302,"Needle Arm",4,1,60,100,15,0,"Move details vary by game."},
 {303,"Slack Off",0,0,0,0,5,0,"Move details vary by game."},
 {304,"Hyper Voice",0,2,90,100,10,0,"Move details vary by game."},
 {305,"Poison Fang",7,1,50,100,15,0,"Move details vary by game."},
 {306,"Crush Claw",0,1,75,95,10,0,"Move details vary by game."},
 {307,"Blast Burn",1,2,150,90,5,0,"Move details vary by game."},
 {308,"Hydro Cannon",2,2,150,90,5,0,"Move details vary by game."},
 {309,"Meteor Mash",16,1,90,90,10,0,"Move details vary by game."},
 {310,"Astonish",13,1,30,100,15,0,"Move details vary by game."},
 {311,"Weather Ball",0,2,50,100,10,0,"Move details vary by game."},
 {312,"Aromatherapy",4,0,0,0,5,0,"Move details vary by game."},
 {313,"Fake Tears",15,0,0,100,20,0,"Move details vary by game."},
 {314,"Air Cutter",9,2,60,95,25,0,"Move details vary by game."},
 {315,"Overheat",1,2,130,90,5,0,"Move details vary by game."},
 {316,"Odor Sleuth",0,0,0,0,40,0,"Move details vary by game."},
 {317,"Rock Tomb",12,1,60,95,15,0,"Move details vary by game."},
 {318,"Silver Wind",11,2,60,100,5,0,"Move details vary by game."},
 {319,"Metal Sound",16,0,0,85,40,0,"Move details vary by game."},
 {320,"Grass Whistle",4,0,0,55,15,0,"Move details vary by game."},
 {321,"Tickle",0,0,0,100,20,0,"Move details vary by game."},
 {322,"Cosmic Power",10,0,0,0,20,0,"Move details vary by game."},
 {323,"Water Spout",2,2,150,100,5,0,"Move details vary by game."},
 {324,"Signal Beam",11,2,75,100,15,0,"Move details vary by game."},
 {325,"Shadow Punch",13,1,60,0,20,0,"Move details vary by game."},
 {326,"Extrasensory",10,2,80,100,20,0,"Move details vary by game."},
 {327,"Sky Uppercut",6,1,85,90,15,0,"Move details vary by game."},
 {328,"Sand Tomb",8,1,35,85,15,0,"Move details vary by game."},
 {329,"Sheer Cold",5,2,0,30,5,0,"Move details vary by game."},
 {330,"Muddy Water",2,2,90,85,10,0,"Move details vary by game."},
 {331,"Bullet Seed",4,1,25,100,30,0,"Move details vary by game."},
 {332,"Aerial Ace",9,1,60,0,20,0,"Move details vary by game."},
 {333,"Icicle Spear",5,1,25,100,30,0,"Move details vary by game."},
 {334,"Iron Defense",16,0,0,0,15,0,"Move details vary by game."},
 {335,"Block",0,0,0,0,5,0,"Move details vary by game."},
 {336,"Howl",0,0,0,0,40,0,"Move details vary by game."},
 {337,"Dragon Claw",14,1,80,100,15,0,"Move details vary by game."},
 {338,"Frenzy Plant",4,2,150,90,5,0,"Move details vary by game."},
 {339,"Bulk Up",6,0,0,0,20,0,"Move details vary by game."},
 {340,"Bounce",9,1,85,85,5,0,"Move details vary by game."},
 {341,"Mud Shot",8,2,55,95,15,0,"Move details vary by game."},
 {342,"Poison Tail",7,1,50,100,25,0,"Move details vary by game."},
 {343,"Covet",0,1,60,100,25,0,"Move details vary by game."},
 {344,"Volt Tackle",3,1,120,100,15,0,"Move details vary by game."},
 {345,"Magical Leaf",4,2,60,0,20,0,"Move details vary by game."},
 {346,"Water Sport",2,0,0,0,15,0,"Move details vary by game."},
 {347,"Calm Mind",10,0,0,0,20,0,"Move details vary by game."},
 {348,"Leaf Blade",4,1,90,100,15,0,"Move details vary by game."},
 {349,"Dragon Dance",14,0,0,0,20,0,"Move details vary by game."},
 {350,"Rock Blast",12,1,25,90,10,0,"Move details vary by game."},
 {351,"Shock Wave",3,2,60,0,20,0,"Move details vary by game."},
 {352,"Water Pulse",2,2,60,100,20,0,"Move details vary by game."},
 {353,"Doom Desire",16,2,140,100,5,0,"Move details vary by game."},
 {354,"Psycho Boost",10,2,140,90,5,0,"Move details vary by game."},
 {355,"Roost",9,0,0,0,5,0,"Move details vary by game."},
 {356,"Gravity",10,0,0,0,5,0,"Move details vary by game."},
 {357,"Miracle Eye",10,0,0,0,40,0,"Move details vary by game."},
 {358,"Wake-Up Slap",6,1,70,100,10,0,"Move details vary by game."},
 {359,"Hammer Arm",6,1,100,90,10,0,"Move details vary by game."},
 {360,"Gyro Ball",16,1,0,100,5,0,"Move details vary by game."},
 {361,"Healing Wish",10,0,0,0,10,0,"Move details vary by game."},
 {362,"Brine",2,2,65,100,10,0,"Move details vary by game."},
 {363,"Natural Gift",0,1,0,100,15,0,"Move details vary by game."},
 {364,"Feint",0,1,30,100,10,0,"Move details vary by game."},
 {365,"Pluck",9,1,60,100,20,0,"Move details vary by game."},
 {366,"Tailwind",9,0,0,0,15,0,"Move details vary by game."},
 {367,"Acupressure",0,0,0,0,30,0,"Move details vary by game."},
 {368,"Metal Burst",16,1,0,100,10,0,"Move details vary by game."},
 {369,"U-turn",11,1,70,100,20,0,"Move details vary by game."},
 {370,"Close Combat",6,1,120,100,5,0,"Move details vary by game."},
 {371,"Payback",15,1,50,100,10,0,"Move details vary by game."},
 {372,"Assurance",15,1,60,100,10,0,"Move details vary by game."},
 {373,"Embargo",15,0,0,100,15,0,"Move details vary by game."},
 {374,"Fling",15,1,0,100,10,0,"Move details vary by game."},
 {375,"Psycho Shift",10,0,0,100,10,0,"Move details vary by game."},
 {376,"Trump Card",0,2,0,0,5,0,"Move details vary by game."},
 {377,"Heal Block",10,0,0,100,15,0,"Move details vary by game."},
 {378,"Wring Out",0,2,0,100,5,0,"Move details vary by game."},
 {379,"Power Trick",10,0,0,0,10,0,"Move details vary by game."},
 {380,"Gastro Acid",7,0,0,100,10,0,"Move details vary by game."},
 {381,"Lucky Chant",0,0,0,0,30,0,"Move details vary by game."},
 {382,"Me First",0,0,0,0,20,0,"Move details vary by game."},
 {383,"Copycat",0,0,0,0,20,0,"Move details vary by game."},
 {384,"Power Swap",10,0,0,0,10,0,"Move details vary by game."},
 {385,"Guard Swap",10,0,0,0,10,0,"Move details vary by game."},
 {386,"Punishment",15,1,0,100,5,0,"Move details vary by game."},
 {387,"Last Resort",0,1,140,100,5,0,"Move details vary by game."},
 {388,"Worry Seed",4,0,0,100,10,0,"Move details vary by game."},
 {389,"Sucker Punch",15,1,70,100,5,0,"Move details vary by game."},
 {390,"Toxic Spikes",7,0,0,0,20,0,"Move details vary by game."},
 {391,"Heart Swap",10,0,0,0,10,0,"Move details vary by game."},
 {392,"Aqua Ring",2,0,0,0,20,0,"Move details vary by game."},
 {393,"Magnet Rise",3,0,0,0,10,0,"Move details vary by game."},
 {394,"Flare Blitz",1,1,120,100,15,0,"Move details vary by game."},
 {395,"Force Palm",6,1,60,100,10,0,"Move details vary by game."},
 {396,"Aura Sphere",6,2,80,0,20,0,"Move details vary by game."},
 {397,"Rock Polish",12,0,0,0,20,0,"Move details vary by game."},
 {398,"Poison Jab",7,1,80,100,20,0,"Move details vary by game."},
 {399,"Dark Pulse",15,2,80,100,15,0,"Move details vary by game."},
 {400,"Night Slash",15,1,70,100,15,0,"Move details vary by game."},
 {401,"Aqua Tail",2,1,90,90,10,0,"Move details vary by game."},
 {402,"Seed Bomb",4,1,80,100,15,0,"Move details vary by game."},
 {403,"Air Slash",9,2,75,95,15,0,"Move details vary by game."},
 {404,"X-Scissor",11,1,80,100,15,0,"Move details vary by game."},
 {405,"Bug Buzz",11,2,90,100,10,0,"Move details vary by game."},
 {406,"Dragon Pulse",14,2,85,100,10,0,"Move details vary by game."},
 {407,"Dragon Rush",14,1,100,75,10,0,"Move details vary by game."},
 {408,"Power Gem",12,2,80,100,20,0,"Move details vary by game."},
 {409,"Drain Punch",6,1,75,100,10,0,"Move details vary by game."},
 {410,"Vacuum Wave",6,2,40,100,30,0,"Move details vary by game."},
 {411,"Focus Blast",6,2,120,70,5,0,"Move details vary by game."},
 {412,"Energy Ball",4,2,90,100,10,0,"Move details vary by game."},
 {413,"Brave Bird",9,1,120,100,15,0,"Move details vary by game."},
 {414,"Earth Power",8,2,90,100,10,0,"Move details vary by game."},
 {415,"Switcheroo",15,0,0,100,10,0,"Move details vary by game."},
 {416,"Giga Impact",0,1,150,90,5,0,"Move details vary by game."},
 {417,"Nasty Plot",15,0,0,0,20,0,"Move details vary by game."},
 {418,"Bullet Punch",16,1,40,100,30,0,"Move details vary by game."},
 {419,"Avalanche",5,1,60,100,10,0,"Move details vary by game."},
 {420,"Ice Shard",5,1,40,100,30,0,"Move details vary by game."},
 {421,"Shadow Claw",13,1,70,100,15,0,"Move details vary by game."},
 {422,"Thunder Fang",3,1,65,95,15,0,"Move details vary by game."},
 {423,"Ice Fang",5,1,65,95,15,0,"Move details vary by game."},
 {424,"Fire Fang",1,1,65,95,15,0,"Move details vary by game."},
 {425,"Shadow Sneak",13,1,40,100,30,0,"Move details vary by game."},
 {426,"Mud Bomb",8,2,65,85,10,0,"Move details vary by game."},
 {427,"Psycho Cut",10,1,70,100,20,0,"Move details vary by game."},
 {428,"Zen Headbutt",10,1,80,90,15,0,"Move details vary by game."},
 {429,"Mirror Shot",16,2,65,85,10,0,"Move details vary by game."},
 {430,"Flash Cannon",16,2,80,100,10,0,"Move details vary by game."},
 {431,"Rock Climb",0,1,90,85,20,0,"Move details vary by game."},
 {432,"Defog",9,0,0,0,15,0,"Move details vary by game."},
 {433,"Trick Room",10,0,0,0,5,0,"Move details vary by game."},
 {434,"Draco Meteor",14,2,130,90,5,0,"Move details vary by game."},
 {435,"Discharge",3,2,80,100,15,0,"Move details vary by game."},
 {436,"Lava Plume",1,2,80,100,15,0,"Move details vary by game."},
 {437,"Leaf Storm",4,2,130,90,5,0,"Move details vary by game."},
 {438,"Power Whip",4,1,120,85,10,0,"Move details vary by game."},
 {439,"Rock Wrecker",12,1,150,90,5,0,"Move details vary by game."},
 {440,"Cross Poison",7,1,70,100,20,0,"Move details vary by game."},
 {441,"Gunk Shot",7,1,120,80,5,0,"Move details vary by game."},
 {442,"Iron Head",16,1,80,100,15,0,"Move details vary by game."},
 {443,"Magnet Bomb",16,1,60,0,20,0,"Move details vary by game."},
 {444,"Stone Edge",12,1,100,80,5,0,"Move details vary by game."},
 {445,"Captivate",0,0,0,100,20,0,"Move details vary by game."},
 {446,"Stealth Rock",12,0,0,0,20,0,"Move details vary by game."},
 {447,"Grass Knot",4,2,0,100,20,0,"Move details vary by game."},
 {448,"Chatter",9,2,65,100,20,0,"Move details vary by game."},
 {449,"Judgment",0,2,100,100,10,0,"Move details vary by game."},
 {450,"Bug Bite",11,1,60,100,20,0,"Move details vary by game."},
 {451,"Charge Beam",3,2,50,90,10,0,"Move details vary by game."},
 {452,"Wood Hammer",4,1,120,100,15,0,"Move details vary by game."},
 {453,"Aqua Jet",2,1,40,100,20,0,"Move details vary by game."},
 {454,"Attack Order",11,1,90,100,15,0,"Move details vary by game."},
 {455,"Defend Order",11,0,0,0,10,0,"Move details vary by game."},
 {456,"Heal Order",11,0,0,0,10,0,"Move details vary by game."},
 {457,"Head Smash",12,1,150,80,5,0,"Move details vary by game."},
 {458,"Double Hit",0,1,35,90,10,0,"Move details vary by game."},
 {459,"Roar of Time",14,2,150,90,5,0,"Move details vary by game."},
 {460,"Spacial Rend",14,2,100,95,5,0,"Move details vary by game."},
 {461,"Lunar Dance",10,0,0,0,10,0,"Move details vary by game."},
 {462,"Crush Grip",0,1,0,100,5,0,"Move details vary by game."},
 {463,"Magma Storm",1,2,100,75,5,0,"Move details vary by game."},
 {464,"Dark Void",15,0,0,50,10,0,"Move details vary by game."},
 {465,"Seed Flare",4,2,120,85,5,0,"Move details vary by game."},
 {466,"Ominous Wind",13,2,60,100,5,0,"Move details vary by game."},
 {467,"Shadow Force",13,1,120,100,5,0,"Move details vary by game."},
 {468,"Hone Claws",15,0,0,0,15,0,"Move details vary by game."},
 {469,"Wide Guard",12,0,0,0,10,0,"Move details vary by game."},
 {470,"Guard Split",10,0,0,0,10,0,"Move details vary by game."},
 {471,"Power Split",10,0,0,0,10,0,"Move details vary by game."},
 {472,"Wonder Room",10,0,0,0,10,0,"Move details vary by game."},
 {473,"Psyshock",10,2,80,100,10,0,"Move details vary by game."},
 {474,"Venoshock",7,2,65,100,10,0,"Move details vary by game."},
 {475,"Autotomize",16,0,0,0,15,0,"Move details vary by game."},
 {476,"Rage Powder",11,0,0,0,20,0,"Move details vary by game."},
 {477,"Telekinesis",10,0,0,0,15,0,"Move details vary by game."},
 {478,"Magic Room",10,0,0,0,10,0,"Move details vary by game."},
 {479,"Smack Down",12,1,50,100,15,0,"Move details vary by game."},
 {480,"Storm Throw",6,1,60,100,10,0,"Move details vary by game."},
 {481,"Flame Burst",1,2,70,100,15,0,"Move details vary by game."},
 {482,"Sludge Wave",7,2,95,100,10,0,"Move details vary by game."},
 {483,"Quiver Dance",11,0,0,0,20,0,"Move details vary by game."},
 {484,"Heavy Slam",16,1,0,100,10,0,"Move details vary by game."},
 {485,"Synchronoise",10,2,120,100,10,0,"Move details vary by game."},
 {486,"Electro Ball",3,2,0,100,10,0,"Move details vary by game."},
 {487,"Soak",2,0,0,100,20,0,"Move details vary by game."},
 {488,"Flame Charge",1,1,50,100,20,0,"Move details vary by game."},
 {489,"Coil",7,0,0,0,20,0,"Move details vary by game."},
 {490,"Low Sweep",6,1,65,100,20,0,"Move details vary by game."},
 {491,"Acid Spray",7,2,40,100,20,0,"Move details vary by game."},
 {492,"Foul Play",15,1,95,100,15,0,"Move details vary by game."},
 {493,"Simple Beam",0,0,0,100,15,0,"Move details vary by game."},
 {494,"Entrainment",0,0,0,100,15,0,"Move details vary by game."},
 {495,"After You",0,0,0,0,15,0,"Move details vary by game."},
 {496,"Round",0,2,60,100,15,0,"Move details vary by game."},
 {497,"Echoed Voice",0,2,40,100,15,0,"Move details vary by game."},
 {498,"Chip Away",0,1,70,100,20,0,"Move details vary by game."},
 {499,"Clear Smog",7,2,50,0,15,0,"Move details vary by game."},
 {500,"Stored Power",10,2,20,100,10,0,"Move details vary by game."},
 {501,"Quick Guard",6,0,0,0,15,0,"Move details vary by game."},
 {502,"Ally Switch",10,0,0,0,15,0,"Move details vary by game."},
 {503,"Scald",2,2,80,100,15,0,"Move details vary by game."},
 {504,"Shell Smash",0,0,0,0,15,0,"Move details vary by game."},
 {505,"Heal Pulse",10,0,0,0,10,0,"Move details vary by game."},
 {506,"Hex",13,2,65,100,10,0,"Move details vary by game."},
 {507,"Sky Drop",9,1,60,100,10,0,"Move details vary by game."},
 {508,"Shift Gear",16,0,0,0,10,0,"Move details vary by game."},
 {509,"Circle Throw",6,1,60,90,10,0,"Move details vary by game."},
 {510,"Incinerate",1,2,60,100,15,0,"Move details vary by game."},
 {511,"Quash",15,0,0,100,15,0,"Move details vary by game."},
 {512,"Acrobatics",9,1,55,100,15,0,"Move details vary by game."},
 {513,"Reflect Type",0,0,0,0,15,0,"Move details vary by game."},
 {514,"Retaliate",0,1,70,100,5,0,"Move details vary by game."},
 {515,"Final Gambit",6,2,0,100,5,0,"Move details vary by game."},
 {516,"Bestow",0,0,0,0,15,0,"Move details vary by game."},
 {517,"Inferno",1,2,100,50,5,0,"Move details vary by game."},
 {518,"Water Pledge",2,2,80,100,10,0,"Move details vary by game."},
 {519,"Fire Pledge",1,2,80,100,10,0,"Move details vary by game."},
 {520,"Grass Pledge",4,2,80,100,10,0,"Move details vary by game."},
 {521,"Volt Switch",3,2,70,100,20,0,"Move details vary by game."},
 {522,"Struggle Bug",11,2,50,100,20,0,"Move details vary by game."},
 {523,"Bulldoze",8,1,60,100,20,0,"Move details vary by game."},
 {524,"Frost Breath",5,2,60,90,10,0,"Move details vary by game."},
 {525,"Dragon Tail",14,1,60,90,10,0,"Move details vary by game."},
 {526,"Work Up",0,0,0,0,30,0,"Move details vary by game."},
 {527,"Electroweb",3,2,55,95,15,0,"Move details vary by game."},
 {528,"Wild Charge",3,1,90,100,15,0,"Move details vary by game."},
 {529,"Drill Run",8,1,80,95,10,0,"Move details vary by game."},
 {530,"Dual Chop",14,1,40,90,15,0,"Move details vary by game."},
 {531,"Heart Stamp",10,1,60,100,25,0,"Move details vary by game."},
 {532,"Horn Leech",4,1,75,100,10,0,"Move details vary by game."},
 {533,"Sacred Sword",6,1,90,100,15,0,"Move details vary by game."},
 {534,"Razor Shell",2,1,75,95,10,0,"Move details vary by game."},
 {535,"Heat Crash",1,1,0,100,10,0,"Move details vary by game."},
 {536,"Leaf Tornado",4,2,65,90,10,0,"Move details vary by game."},
 {537,"Steamroller",11,1,65,100,20,0,"Move details vary by game."},
 {538,"Cotton Guard",4,0,0,0,10,0,"Move details vary by game."},
 {539,"Night Daze",15,2,85,95,10,0,"Move details vary by game."},
 {540,"Psystrike",10,2,100,100,10,0,"Move details vary by game."},
 {541,"Tail Slap",0,1,25,85,10,0,"Move details vary by game."},
 {542,"Hurricane",9,2,110,70,10,0,"Move details vary by game."},
 {543,"Head Charge",0,1,120,100,15,0,"Move details vary by game."},
 {544,"Gear Grind",16,1,50,85,15,0,"Move details vary by game."},
 {545,"Searing Shot",1,2,100,100,5,0,"Move details vary by game."},
 {546,"Techno Blast",0,2,120,100,5,0,"Move details vary by game."},
 {547,"Relic Song",0,2,75,100,10,0,"Move details vary by game."},
 {548,"Secret Sword",6,2,85,100,10,0,"Move details vary by game."},
 {549,"Glaciate",5,2,65,95,10,0,"Move details vary by game."},
 {550,"Bolt Strike",3,1,130,85,5,0,"Move details vary by game."},
 {551,"Blue Flare",1,2,130,85,5,0,"Move details vary by game."},
 {552,"Fiery Dance",1,2,80,100,10,0,"Move details vary by game."},
 {553,"Freeze Shock",5,1,140,90,5,0,"Move details vary by game."},
 {554,"Ice Burn",5,2,140,90,5,0,"Move details vary by game."},
 {555,"Snarl",15,2,55,95,15,0,"Move details vary by game."},
 {556,"Icicle Crash",5,1,85,90,10,0,"Move details vary by game."},
 {557,"V-create",1,1,180,95,5,0,"Move details vary by game."},
 {558,"Fusion Flare",1,2,100,100,5,0,"Move details vary by game."},
 {559,"Fusion Bolt",3,1,100,100,5,0,"Move details vary by game."},
 {560,"Flying Press",6,1,100,95,10,0,"Move details vary by game."},
 {561,"Mat Block",6,0,0,0,10,0,"Move details vary by game."},
 {562,"Belch",7,2,120,90,10,0,"Move details vary by game."},
 {563,"Rototiller",8,0,0,0,10,0,"Move details vary by game."},
 {564,"Sticky Web",11,0,0,0,20,0,"Move details vary by game."},
 {565,"Fell Stinger",11,1,50,100,25,0,"Move details vary by game."},
 {566,"Phantom Force",13,1,90,100,10,0,"Move details vary by game."},
 {567,"Trick-or-Treat",13,0,0,100,20,0,"Move details vary by game."},
 {568,"Noble Roar",0,0,0,100,30,0,"Move details vary by game."},
 {569,"Ion Deluge",3,0,0,0,25,0,"Move details vary by game."},
 {570,"Parabolic Charge",3,2,65,100,20,0,"Move details vary by game."},
 {571,"Forest's Curse",4,0,0,100,20,0,"Move details vary by game."},
 {572,"Petal Blizzard",4,1,90,100,15,0,"Move details vary by game."},
 {573,"Freeze-Dry",5,2,70,100,20,0,"Move details vary by game."},
 {574,"Disarming Voice",17,2,40,0,15,0,"Move details vary by game."},
 {575,"Parting Shot",15,0,0,100,20,0,"Move details vary by game."},
 {576,"Topsy-Turvy",15,0,0,0,20,0,"Move details vary by game."},
 {577,"Draining Kiss",17,2,50,100,10,0,"Move details vary by game."},
 {578,"Crafty Shield",17,0,0,0,10,0,"Move details vary by game."},
 {579,"Flower Shield",17,0,0,0,10,0,"Move details vary by game."},
 {580,"Grassy Terrain",4,0,0,0,10,0,"Move details vary by game."},
 {581,"Misty Terrain",17,0,0,0,10,0,"Move details vary by game."},
 {582,"Electrify",3,0,0,0,20,0,"Move details vary by game."},
 {583,"Play Rough",17,1,90,90,10,0,"Move details vary by game."},
 {584,"Fairy Wind",17,2,40,100,30,0,"Move details vary by game."},
 {585,"Moonblast",17,2,95,100,15,0,"Move details vary by game."},
 {586,"Boomburst",0,2,140,100,10,0,"Move details vary by game."},
 {587,"Fairy Lock",17,0,0,0,10,0,"Move details vary by game."},
 {588,"King's Shield",16,0,0,0,10,0,"Move details vary by game."},
 {589,"Play Nice",0,0,0,0,20,0,"Move details vary by game."},
 {590,"Confide",0,0,0,0,20,0,"Move details vary by game."},
 {591,"Diamond Storm",12,1,100,95,5,0,"Move details vary by game."},
 {592,"Steam Eruption",2,2,110,95,5,0,"Move details vary by game."},
 {593,"Hyperspace Hole",10,2,80,0,5,0,"Move details vary by game."},
 {594,"Water Shuriken",2,2,15,100,20,0,"Move details vary by game."},
 {595,"Mystical Fire",1,2,75,100,10,0,"Move details vary by game."},
 {596,"Spiky Shield",4,0,0,0,10,0,"Move details vary by game."},
 {597,"Aromatic Mist",17,0,0,0,20,0,"Move details vary by game."},
 {598,"Eerie Impulse",3,0,0,100,15,0,"Move details vary by game."},
 {599,"Venom Drench",7,0,0,100,20,0,"Move details vary by game."},
 {600,"Powder",11,0,0,100,20,0,"Move details vary by game."},
 {601,"Geomancy",17,0,0,0,10,0,"Move details vary by game."},
 {602,"Magnetic Flux",3,0,0,0,20,0,"Move details vary by game."},
 {603,"Happy Hour",0,0,0,0,30,0,"Move details vary by game."},
 {604,"Electric Terrain",3,0,0,0,10,0,"Move details vary by game."},
 {605,"Dazzling Gleam",17,2,80,100,10,0,"Move details vary by game."},
 {606,"Celebrate",0,0,0,0,40,0,"Move details vary by game."},
 {607,"Hold Hands",0,0,0,0,40,0,"Move details vary by game."},
 {608,"Baby-Doll Eyes",17,0,0,100,30,0,"Move details vary by game."},
 {609,"Nuzzle",3,1,20,100,20,0,"Move details vary by game."},
 {610,"Hold Back",0,1,40,100,40,0,"Move details vary by game."},
 {611,"Infestation",11,2,20,100,20,0,"Move details vary by game."},
 {612,"Power-Up Punch",6,1,40,100,20,0,"Move details vary by game."},
 {613,"Oblivion Wing",9,2,80,100,10,0,"Move details vary by game."},
 {614,"Thousand Arrows",8,1,90,100,10,0,"Move details vary by game."},
 {615,"Thousand Waves",8,1,90,100,10,0,"Move details vary by game."},
 {616,"Land's Wrath",8,1,90,100,10,0,"Move details vary by game."},
 {617,"Light of Ruin",17,2,140,90,5,0,"Move details vary by game."},
 {618,"Origin Pulse",2,2,110,85,10,0,"Move details vary by game."},
 {619,"Precipice Blades",8,1,120,85,10,0,"Move details vary by game."},
 {620,"Dragon Ascent",9,1,120,100,5,0,"Move details vary by game."},
 {621,"Hyperspace Fury",15,1,100,0,5,0,"Move details vary by game."},
 {622,"Breakneck Blitz",0,1,1,0,1,0,"Move details vary by game."},
 {624,"All-Out Pummeling",6,1,1,0,1,0,"Move details vary by game."},
 {626,"Supersonic Skystrike",9,1,1,0,1,0,"Move details vary by game."},
 {628,"Acid Downpour",7,1,1,0,1,0,"Move details vary by game."},
 {630,"Tectonic Rage",8,1,1,0,1,0,"Move details vary by game."},
 {632,"Continental Crush",12,1,1,0,1,0,"Move details vary by game."},
 {634,"Savage Spin-Out",11,1,1,0,1,0,"Move details vary by game."},
 {636,"Never-Ending Nightmare",13,1,1,0,1,0,"Move details vary by game."},
 {638,"Corkscrew Crash",16,1,1,0,1,0,"Move details vary by game."},
 {640,"Inferno Overdrive",1,1,1,0,1,0,"Move details vary by game."},
 {642,"Hydro Vortex",2,1,1,0,1,0,"Move details vary by game."},
 {644,"Bloom Doom",4,1,1,0,1,0,"Move details vary by game."},
 {646,"Gigavolt Havoc",3,1,1,0,1,0,"Move details vary by game."},
 {648,"Shattered Psyche",10,1,1,0,1,0,"Move details vary by game."},
 {650,"Subzero Slammer",5,1,1,0,1,0,"Move details vary by game."},
 {652,"Devastating Drake",14,1,1,0,1,0,"Move details vary by game."},
 {654,"Black Hole Eclipse",15,1,1,0,1,0,"Move details vary by game."},
 {656,"Twinkle Tackle",17,1,1,0,1,0,"Move details vary by game."},
 {658,"Catastropika",3,1,210,0,1,0,"Move details vary by game."},
 {659,"Shore Up",8,0,0,0,5,0,"Move details vary by game."},
 {660,"First Impression",11,1,90,100,10,0,"Move details vary by game."},
 {661,"Baneful Bunker",7,0,0,0,10,0,"Move details vary by game."},
 {662,"Spirit Shackle",13,1,80,100,10,0,"Move details vary by game."},
 {663,"Darkest Lariat",15,1,85,100,10,0,"Move details vary by game."},
 {664,"Sparkling Aria",2,2,90,100,10,0,"Move details vary by game."},
 {665,"Ice Hammer",5,1,100,90,10,0,"Move details vary by game."},
 {666,"Floral Healing",17,0,0,0,10,0,"Move details vary by game."},
 {667,"High Horsepower",8,1,95,95,10,0,"Move details vary by game."},
 {668,"Strength Sap",4,0,0,100,10,0,"Move details vary by game."},
 {669,"Solar Blade",4,1,125,100,10,0,"Move details vary by game."},
 {670,"Leafage",4,1,40,100,40,0,"Move details vary by game."},
 {671,"Spotlight",0,0,0,0,15,0,"Move details vary by game."},
 {672,"Toxic Thread",7,0,0,100,20,0,"Move details vary by game."},
 {673,"Laser Focus",0,0,0,0,30,0,"Move details vary by game."},
 {674,"Gear Up",16,0,0,0,20,0,"Move details vary by game."},
 {675,"Throat Chop",15,1,80,100,15,0,"Move details vary by game."},
 {676,"Pollen Puff",11,2,90,100,15,0,"Move details vary by game."},
 {677,"Anchor Shot",16,1,80,100,20,0,"Move details vary by game."},
 {678,"Psychic Terrain",10,0,0,0,10,0,"Move details vary by game."},
 {679,"Lunge",11,1,80,100,15,0,"Move details vary by game."},
 {680,"Fire Lash",1,1,80,100,15,0,"Move details vary by game."},
 {681,"Power Trip",15,1,20,100,10,0,"Move details vary by game."},
 {682,"Burn Up",1,2,130,100,5,0,"Move details vary by game."},
 {683,"Speed Swap",10,0,0,0,10,0,"Move details vary by game."},
 {684,"Smart Strike",16,1,70,0,10,0,"Move details vary by game."},
 {685,"Purify",7,0,0,0,20,0,"Move details vary by game."},
 {686,"Revelation Dance",0,2,90,100,15,0,"Move details vary by game."},
 {687,"Core Enforcer",14,2,100,100,10,0,"Move details vary by game."},
 {688,"Trop Kick",4,1,70,100,15,0,"Move details vary by game."},
 {689,"Instruct",10,0,0,0,15,0,"Move details vary by game."},
 {690,"Beak Blast",9,1,100,100,15,0,"Move details vary by game."},
 {691,"Clanging Scales",14,2,110,100,5,0,"Move details vary by game."},
 {692,"Dragon Hammer",14,1,90,100,15,0,"Move details vary by game."},
 {693,"Brutal Swing",15,1,60,100,20,0,"Move details vary by game."},
 {694,"Aurora Veil",5,0,0,0,20,0,"Move details vary by game."},
 {695,"Sinister Arrow Raid",13,1,180,0,1,0,"Move details vary by game."},
 {696,"Malicious Moonsault",15,1,180,0,1,0,"Move details vary by game."},
 {697,"Oceanic Operetta",2,2,195,0,1,0,"Move details vary by game."},
 {698,"Guardian of Alola",17,2,0,0,1,0,"Move details vary by game."},
 {699,"Soul-Stealing 7-Star Strike",13,1,195,0,1,0,"Move details vary by game."},
 {700,"Stoked Sparksurfer",3,2,175,0,1,0,"Move details vary by game."},
 {701,"Pulverizing Pancake",0,1,210,0,1,0,"Move details vary by game."},
 {702,"Extreme Evoboost",0,0,0,0,1,0,"Move details vary by game."},
 {703,"Genesis Supernova",10,2,185,0,1,0,"Move details vary by game."},
 {704,"Shell Trap",1,2,150,100,5,0,"Move details vary by game."},
 {705,"Fleur Cannon",17,2,130,90,5,0,"Move details vary by game."},
 {706,"Psychic Fangs",10,1,85,100,10,0,"Move details vary by game."},
 {707,"Stomping Tantrum",8,1,75,100,10,0,"Move details vary by game."},
 {708,"Shadow Bone",13,1,85,100,10,0,"Move details vary by game."},
 {709,"Accelerock",12,1,40,100,20,0,"Move details vary by game."},
 {710,"Liquidation",2,1,85,100,10,0,"Move details vary by game."},
 {711,"Prismatic Laser",10,2,160,100,10,0,"Move details vary by game."},
 {712,"Spectral Thief",13,1,90,100,10,0,"Move details vary by game."},
 {713,"Sunsteel Strike",16,1,100,100,5,0,"Move details vary by game."},
 {714,"Moongeist Beam",13,2,100,100,5,0,"Move details vary by game."},
 {715,"Tearful Look",0,0,0,0,20,0,"Move details vary by game."},
 {716,"Zing Zap",3,1,80,100,10,0,"Move details vary by game."},
 {717,"Nature's Madness",17,2,0,90,10,0,"Move details vary by game."},
 {718,"Multi-Attack",0,1,120,100,10,0,"Move details vary by game."},
 {720,"Mind Blown",1,2,150,100,5,0,"Move details vary by game."},
 {721,"Plasma Fists",3,1,100,100,15,0,"Move details vary by game."},
 {722,"Photon Geyser",10,2,100,100,5,0,"Move details vary by game."},
 {723,"Light That Burns the Sky",10,2,200,0,1,0,"Move details vary by game."},
 {724,"Searing Sunraze Smash",16,1,200,0,1,0,"Move details vary by game."},
 {725,"Menacing Moonraze Maelstrom",13,2,200,0,1,0,"Move details vary by game."},
 {726,"Let's Snuggle Forever",17,1,190,0,1,0,"Move details vary by game."},
 {727,"Splintered Stormshards",12,1,190,0,1,0,"Move details vary by game."},
 {728,"Clangorous Soulblaze",14,2,185,0,1,0,"Move details vary by game."},
 {729,"Zippy Zap",3,1,80,100,10,0,"Move details vary by game."},
 {730,"Splishy Splash",2,2,90,100,15,0,"Move details vary by game."},
 {731,"Floaty Fall",9,1,90,95,15,0,"Move details vary by game."},
 {732,"Pika Papow",3,2,0,0,20,0,"Move details vary by game."},
 {733,"Bouncy Bubble",2,2,60,100,20,0,"Move details vary by game."},
 {734,"Buzzy Buzz",3,2,60,100,20,0,"Move details vary by game."},
 {735,"Sizzly Slide",1,1,60,100,20,0,"Move details vary by game."},
 {736,"Glitzy Glow",10,2,80,95,15,0,"Move details vary by game."},
 {737,"Baddy Bad",15,2,80,95,15,0,"Move details vary by game."},
 {738,"Sappy Seed",4,1,100,90,10,0,"Move details vary by game."},
 {739,"Freezy Frost",5,2,100,90,10,0,"Move details vary by game."},
 {740,"Sparkly Swirl",17,2,120,85,5,0,"Move details vary by game."},
 {741,"Veevee Volley",0,1,0,0,20,0,"Move details vary by game."},
 {742,"Double Iron Bash",16,1,60,100,5,0,"Move details vary by game."},
 {743,"Max Guard",0,0,0,0,10,0,"Move details vary by game."},
 {744,"Dynamax Cannon",14,2,100,100,5,0,"Move details vary by game."},
 {745,"Snipe Shot",2,2,80,100,15,0,"Move details vary by game."},
 {746,"Jaw Lock",15,1,80,100,10,0,"Move details vary by game."},
 {747,"Stuff Cheeks",0,0,0,0,10,0,"Move details vary by game."},
 {748,"No Retreat",6,0,0,0,5,0,"Move details vary by game."},
 {749,"Tar Shot",12,0,0,100,15,0,"Move details vary by game."},
 {750,"Magic Powder",10,0,0,100,20,0,"Move details vary by game."},
 {751,"Dragon Darts",14,1,50,100,10,0,"Move details vary by game."},
 {752,"Teatime",0,0,0,0,10,0,"Move details vary by game."},
 {753,"Octolock",6,0,0,100,15,0,"Move details vary by game."},
 {754,"Bolt Beak",3,1,85,100,10,0,"Move details vary by game."},
 {755,"Fishious Rend",2,1,85,100,10,0,"Move details vary by game."},
 {756,"Court Change",0,0,0,100,10,0,"Move details vary by game."},
 {757,"Max Flare",1,1,100,0,10,0,"Move details vary by game."},
 {758,"Max Flutterby",11,1,10,0,10,0,"Move details vary by game."},
 {759,"Max Lightning",3,1,10,0,10,0,"Move details vary by game."},
 {760,"Max Strike",0,1,10,0,10,0,"Move details vary by game."},
 {761,"Max Knuckle",6,1,10,0,10,0,"Move details vary by game."},
 {762,"Max Phantasm",13,1,10,0,10,0,"Move details vary by game."},
 {763,"Max Hailstorm",5,1,10,0,10,0,"Move details vary by game."},
 {764,"Max Ooze",7,1,10,0,10,0,"Move details vary by game."},
 {765,"Max Geyser",2,1,10,0,10,0,"Move details vary by game."},
 {766,"Max Airstream",9,1,10,0,10,0,"Move details vary by game."},
 {767,"Max Starfall",17,1,10,0,10,0,"Move details vary by game."},
 {768,"Max Wyrmwind",14,1,10,0,10,0,"Move details vary by game."},
 {769,"Max Mindstorm",10,1,10,0,10,0,"Move details vary by game."},
 {770,"Max Rockfall",12,1,10,0,10,0,"Move details vary by game."},
 {771,"Max Quake",8,1,10,0,10,0,"Move details vary by game."},
 {772,"Max Darkness",15,1,10,0,10,0,"Move details vary by game."},
 {773,"Max Overgrowth",4,1,10,0,10,0,"Move details vary by game."},
 {774,"Max Steelspike",16,1,10,0,10,0,"Move details vary by game."},
 {775,"Clangorous Soul",14,0,0,100,5,0,"Move details vary by game."},
 {776,"Body Press",6,1,80,100,10,0,"Move details vary by game."},
 {777,"Decorate",17,0,0,0,15,0,"Move details vary by game."},
 {778,"Drum Beating",4,1,80,100,10,0,"Move details vary by game."},
 {779,"Snap Trap",4,1,35,100,15,0,"Move details vary by game."},
 {780,"Pyro Ball",1,1,120,90,5,0,"Move details vary by game."},
 {781,"Behemoth Blade",16,1,100,100,5,0,"Move details vary by game."},
 {782,"Behemoth Bash",16,1,100,100,5,0,"Move details vary by game."},
 {783,"Aura Wheel",3,1,110,100,10,0,"Move details vary by game."},
 {784,"Breaking Swipe",14,1,60,100,15,0,"Move details vary by game."},
 {785,"Branch Poke",4,1,40,100,40,0,"Move details vary by game."},
 {786,"Overdrive",3,2,80,100,10,0,"Move details vary by game."},
 {787,"Apple Acid",4,2,80,100,10,0,"Move details vary by game."},
 {788,"Grav Apple",4,1,80,100,10,0,"Move details vary by game."},
 {789,"Spirit Break",17,1,75,100,15,0,"Move details vary by game."},
 {790,"Strange Steam",17,2,90,95,10,0,"Move details vary by game."},
 {791,"Life Dew",2,0,0,0,10,0,"Move details vary by game."},
 {792,"Obstruct",15,0,0,100,10,0,"Move details vary by game."},
 {793,"False Surrender",15,1,80,0,10,0,"Move details vary by game."},
 {794,"Meteor Assault",6,1,150,100,5,0,"Move details vary by game."},
 {795,"Eternabeam",14,2,160,90,5,0,"Move details vary by game."},
 {796,"Steel Beam",16,2,140,95,5,0,"Move details vary by game."},
 {797,"Expanding Force",10,2,80,100,10,0,"Move details vary by game."},
 {798,"Steel Roller",16,1,130,100,5,0,"Move details vary by game."},
 {799,"Scale Shot",14,1,25,90,20,0,"Move details vary by game."},
 {800,"Meteor Beam",12,2,120,90,10,0,"Move details vary by game."},
 {801,"Shell Side Arm",7,2,90,100,10,0,"Move details vary by game."},
 {802,"Misty Explosion",17,2,100,100,5,0,"Move details vary by game."},
 {803,"Grassy Glide",4,1,55,100,20,0,"Move details vary by game."},
 {804,"Rising Voltage",3,2,70,100,20,0,"Move details vary by game."},
 {805,"Terrain Pulse",0,2,50,100,10,0,"Move details vary by game."},
 {806,"Skitter Smack",11,1,70,90,10,0,"Move details vary by game."},
 {807,"Burning Jealousy",1,2,70,100,5,0,"Move details vary by game."},
 {808,"Lash Out",15,1,75,100,5,0,"Move details vary by game."},
 {809,"Poltergeist",13,1,110,90,5,0,"Move details vary by game."},
 {810,"Corrosive Gas",7,0,0,100,40,0,"Move details vary by game."},
 {811,"Coaching",6,0,0,0,10,0,"Move details vary by game."},
 {812,"Flip Turn",2,1,60,100,20,0,"Move details vary by game."},
 {813,"Triple Axel",5,1,20,90,10,0,"Move details vary by game."},
 {814,"Dual Wingbeat",9,1,40,90,10,0,"Move details vary by game."},
 {815,"Scorching Sands",8,2,70,100,10,0,"Move details vary by game."},
 {816,"Jungle Healing",4,0,0,0,10,0,"Move details vary by game."},
 {817,"Wicked Blow",15,1,75,100,5,0,"Move details vary by game."},
 {818,"Surging Strikes",2,1,25,100,5,0,"Move details vary by game."},
 {819,"Thunder Cage",3,2,80,90,15,0,"Move details vary by game."},
 {820,"Dragon Energy",14,2,150,100,5,0,"Move details vary by game."},
 {821,"Freezing Glare",10,2,90,100,10,0,"Move details vary by game."},
 {822,"Fiery Wrath",15,2,90,100,10,0,"Move details vary by game."},
 {823,"Thunderous Kick",6,1,90,100,10,0,"Move details vary by game."},
 {824,"Glacial Lance",5,1,120,100,5,0,"Move details vary by game."},
 {825,"Astral Barrage",13,2,120,100,5,0,"Move details vary by game."},
 {826,"Eerie Spell",10,2,80,100,5,0,"Move details vary by game."},
 {827,"Dire Claw",7,1,80,100,15,0,"Move details vary by game."},
 {828,"Psyshield Bash",10,1,70,90,10,0,"Move details vary by game."},
 {829,"Power Shift",0,0,0,0,10,0,"Move details vary by game."},
 {830,"Stone Axe",12,1,65,90,15,0,"Move details vary by game."},
 {831,"Springtide Storm",17,2,100,80,5,0,"Move details vary by game."},
 {832,"Mystical Power",10,2,70,90,10,0,"Move details vary by game."},
 {833,"Raging Fury",1,1,120,100,10,0,"Move details vary by game."},
 {834,"Wave Crash",2,1,120,100,10,0,"Move details vary by game."},
 {835,"Chloroblast",4,2,150,95,5,0,"Move details vary by game."},
 {836,"Mountain Gale",5,1,100,85,10,0,"Move details vary by game."},
 {837,"Victory Dance",6,0,0,0,10,0,"Move details vary by game."},
 {838,"Headlong Rush",8,1,120,100,5,0,"Move details vary by game."},
 {839,"Barb Barrage",7,1,60,100,10,0,"Move details vary by game."},
 {840,"Esper Wing",10,2,80,100,10,0,"Move details vary by game."},
 {841,"Bitter Malice",13,2,75,100,10,0,"Move details vary by game."},
 {842,"Shelter",16,0,0,0,10,0,"Move details vary by game."},
 {843,"Triple Arrows",6,1,90,100,10,0,"Move details vary by game."},
 {844,"Infernal Parade",13,2,60,100,15,0,"Move details vary by game."},
 {845,"Ceaseless Edge",15,1,65,90,15,0,"Move details vary by game."},
 {846,"Bleakwind Storm",9,2,100,80,10,0,"Move details vary by game."},
 {847,"Wildbolt Storm",3,2,100,80,10,0,"Move details vary by game."},
 {848,"Sandsear Storm",8,2,100,80,10,0,"Move details vary by game."},
 {849,"Lunar Blessing",10,0,0,0,5,0,"Move details vary by game."},
 {850,"Take Heart",10,0,0,0,15,0,"Move details vary by game."},
 {851,"Tera Blast",0,2,80,100,10,0,"Move details vary by game."},
 {852,"Silk Trap",11,0,0,0,10,0,"Move details vary by game."},
 {853,"Axe Kick",6,1,120,90,10,0,"Move details vary by game."},
 {854,"Last Respects",13,1,50,100,10,0,"Move details vary by game."},
 {855,"Lumina Crash",10,2,80,100,10,0,"Move details vary by game."},
 {856,"Order Up",14,1,80,100,10,0,"Move details vary by game."},
 {857,"Jet Punch",2,1,60,100,15,0,"Move details vary by game."},
 {858,"Spicy Extract",4,0,0,0,15,0,"Move details vary by game."},
 {859,"Spin Out",16,1,100,100,5,0,"Move details vary by game."},
 {860,"Population Bomb",0,1,20,90,10,0,"Move details vary by game."},
 {861,"Ice Spinner",5,1,80,100,15,0,"Move details vary by game."},
 {862,"Glaive Rush",14,1,120,100,5,0,"Move details vary by game."},
 {863,"Revival Blessing",0,0,0,0,1,0,"Move details vary by game."},
 {864,"Salt Cure",12,1,40,100,15,0,"Move details vary by game."},
 {865,"Triple Dive",2,1,30,95,10,0,"Move details vary by game."},
 {866,"Mortal Spin",7,1,30,100,15,0,"Move details vary by game."},
 {867,"Doodle",0,0,0,100,10,0,"Move details vary by game."},
 {868,"Fillet Away",0,0,0,0,10,0,"Move details vary by game."},
 {869,"Kowtow Cleave",15,1,85,0,10,0,"Move details vary by game."},
 {870,"Flower Trick",4,1,70,0,10,0,"Move details vary by game."},
 {871,"Torch Song",1,2,80,100,10,0,"Move details vary by game."},
 {872,"Aqua Step",2,1,80,100,10,0,"Move details vary by game."},
 {873,"Raging Bull",0,1,90,100,10,0,"Move details vary by game."},
 {874,"Make It Rain",16,2,120,100,5,0,"Move details vary by game."},
 {875,"Psyblade",10,1,80,100,15,0,"Move details vary by game."},
 {876,"Hydro Steam",2,2,80,100,15,0,"Move details vary by game."},
 {877,"Ruination",15,2,0,90,10,0,"Move details vary by game."},
 {878,"Collision Course",6,1,100,100,5,0,"Move details vary by game."},
 {879,"Electro Drift",3,2,100,100,5,0,"Move details vary by game."},
 {880,"Shed Tail",0,0,0,0,10,0,"Move details vary by game."},
 {881,"Chilly Reception",5,0,0,0,10,0,"Move details vary by game."},
 {882,"Tidy Up",0,0,0,0,10,0,"Move details vary by game."},
 {883,"Snowscape",5,0,0,0,10,0,"Move details vary by game."},
 {884,"Pounce",11,1,50,100,20,0,"Move details vary by game."},
 {885,"Trailblaze",4,1,50,100,20,0,"Move details vary by game."},
 {886,"Chilling Water",2,2,50,100,20,0,"Move details vary by game."},
 {887,"Hyper Drill",0,1,100,100,5,0,"Move details vary by game."},
 {888,"Twin Beam",10,2,40,100,10,0,"Move details vary by game."},
 {889,"Rage Fist",13,1,50,100,10,0,"Move details vary by game."},
 {890,"Armor Cannon",1,2,120,100,5,0,"Move details vary by game."},
 {891,"Bitter Blade",1,1,90,100,10,0,"Move details vary by game."},
 {892,"Double Shock",3,1,120,100,5,0,"Move details vary by game."},
 {893,"Gigaton Hammer",16,1,160,100,5,0,"Move details vary by game."},
 {894,"Comeuppance",15,1,0,100,10,0,"Move details vary by game."},
 {895,"Aqua Cutter",2,1,70,100,20,0,"Move details vary by game."},
 {896,"Blazing Torque",1,1,80,100,10,0,"Move details vary by game."},
 {897,"Wicked Torque",15,1,80,100,10,0,"Move details vary by game."},
 {898,"Noxious Torque",7,1,100,100,10,0,"Move details vary by game."},
 {899,"Combat Torque",6,1,100,100,10,0,"Move details vary by game."},
 {900,"Magical Torque",17,1,100,100,10,0,"Move details vary by game."},
 {901,"Blood Moon",0,2,140,100,5,0,"Move details vary by game."},
 {902,"Matcha Gotcha",4,2,80,90,15,0,"Move details vary by game."},
 {903,"Syrup Bomb",4,2,60,85,10,0,"Move details vary by game."},
 {904,"Ivy Cudgel",4,1,100,100,10,0,"Move details vary by game."},
 {905,"Electro Shot",3,2,130,100,10,0,"Move details vary by game."},
 {906,"Tera Starstorm",0,2,120,100,5,0,"Move details vary by game."},
 {907,"Fickle Beam",14,2,80,100,5,0,"Move details vary by game."},
 {908,"Burning Bulwark",1,0,0,0,10,0,"Move details vary by game."},
 {909,"Thunderclap",3,2,70,100,5,0,"Move details vary by game."},
 {910,"Mighty Cleave",12,1,95,100,5,0,"Move details vary by game."},
 {911,"Tachyon Cutter",16,2,50,0,10,0,"Move details vary by game."},
 {912,"Hard Press",16,1,0,100,10,0,"Move details vary by game."},
 {913,"Dragon Cheer",14,0,0,0,15,0,"Move details vary by game."},
 {914,"Alluring Voice",17,2,80,100,10,0,"Move details vary by game."},
 {915,"Temper Flare",1,1,75,100,10,0,"Move details vary by game."},
 {916,"Supercell Slam",3,1,100,95,15,0,"Move details vary by game."},
 {917,"Psychic Noise",10,2,75,100,10,0,"Move details vary by game."},
 {918,"Upper Hand",6,1,65,100,15,0,"Move details vary by game."},
 {919,"Malignant Chain",7,2,100,100,5,0,"Move details vary by game."},
 {920,"Nihil Light",14,2,100,100,10,0,"Move details vary by game."},
 {1000,"G-Max Wind Rage",9,1,10,0,10,0,"Move details vary by game."}
};
#define MOVE_N ((int)(sizeof(moves)/sizeof(moves[0])))

static const LearnMove l_bulba[]={{33,1,0},{45,3,0},{22,9,0},{331,15,0},{412,0,1}};
static const LearnMove l_char[]={{45,1,0},{52,7,0},{98,10,0},{337,0,1},{394,0,1}};
static const LearnMove l_squirt[]={{33,1,0},{39,4,0},{55,7,0},{129,0,1}};
static const LearnMove l_pika[]={{84,1,0},{45,1,0},{98,10,0},{85,0,1},{344,0,3}};
static const LearnMove l_mewtwo[]={{94,1,0},{129,1,0},{247,0,1}};
static const LearnMove l_chiko[]={{33,1,0},{45,1,0},{22,6,0},{412,0,1}};
static const LearnMove l_cynda[]={{33,1,0},{45,1,0},{52,10,0},{126,0,1}};
static const LearnMove l_toto[]={{33,1,0},{45,1,0},{55,6,0},{249,0,1}};
static const LearnMove l_tree[]={{1,1,0},{45,1,0},{98,11,0},{331,0,1},{412,0,1}};
static const LearnMove l_torch[]={{45,1,0},{52,10,0},{98,16,0},{394,0,1}};
static const LearnMove l_mud[]={{33,1,0},{45,1,0},{55,10,0},{89,0,1}};
static const LearnMove l_turt[]={{33,1,0},{45,1,0},{22,13,0},{412,0,1}};
static const LearnMove l_chim[]={{1,1,0},{45,1,0},{52,7,0},{394,0,1}};
static const LearnMove l_pip[]={{1,1,0},{45,4,0},{55,8,0},{430,0,1}};
static const LearnMove l_luca[]={{98,1,0},{399,0,1},{430,0,1},{89,0,1}};

static const Pokemon mons[] = {
 {1,"Bulbasaur",1,T_GRASS,T_POISON,45,49,49,65,65,45,"Overgrow / Chlorophyll","Lv16 -> Ivysaur",l_bulba,5},
 {4,"Charmander",1,T_FIRE,T_NONE,39,52,43,60,50,65,"Blaze / Solar Power","Lv16 -> Charmeleon",l_char,5},
 {7,"Squirtle",1,T_WATER,T_NONE,44,48,65,50,64,43,"Torrent / Rain Dish","Lv16 -> Wartortle",l_squirt,4},
 {25,"Pikachu",1,T_ELECTRIC,T_NONE,35,55,40,50,50,90,"Static / Lightning Rod","Thunder Stone -> Raichu",l_pika,5},
 {150,"Mewtwo",1,T_PSYCHIC,T_NONE,106,110,90,154,90,130,"Pressure / Unnerve","No evolution",l_mewtwo,3},
 {152,"Chikorita",2,T_GRASS,T_NONE,45,49,65,49,65,45,"Overgrow / Leaf Guard","Lv16 -> Bayleef",l_chiko,4},
 {155,"Cyndaquil",2,T_FIRE,T_NONE,39,52,43,60,50,65,"Blaze / Flash Fire","Lv14 -> Quilava",l_cynda,4},
 {158,"Totodile",2,T_WATER,T_NONE,50,65,64,44,48,43,"Torrent / Sheer Force","Lv18 -> Croconaw",l_toto,4},
 {252,"Treecko",3,T_GRASS,T_NONE,40,45,35,65,55,70,"Overgrow / Unburden","Lv16 -> Grovyle",l_tree,5},
 {255,"Torchic",3,T_FIRE,T_NONE,45,60,40,70,50,45,"Blaze / Speed Boost","Lv16 -> Combusken",l_torch,4},
 {258,"Mudkip",3,T_WATER,T_NONE,50,70,50,50,50,40,"Torrent / Damp","Lv16 -> Marshtomp",l_mud,4},
 {387,"Turtwig",4,T_GRASS,T_NONE,55,68,64,45,55,31,"Overgrow / Shell Armor","Lv18 -> Grotle",l_turt,4},
 {390,"Chimchar",4,T_FIRE,T_NONE,44,58,44,58,44,61,"Blaze / Iron Fist","Lv14 -> Monferno",l_chim,4},
 {393,"Piplup",4,T_WATER,T_NONE,53,51,53,61,56,40,"Torrent / Competitive","Lv16 -> Prinplup",l_pip,4},
 {448,"Lucario",4,T_FIGHTING,T_STEEL,70,110,70,115,70,90,"Steadfast / Inner Focus","Riolu: friendship daytime",l_luca,4}
};
#define MON_N ((int)(sizeof(mons)/sizeof(mons[0])))

static const Move *move_by_id(uint16_t id) {
  for(int i=0;i<MOVE_N;i++) if(moves[i].id==id) return &moves[i];
  return NULL;
}
/* Verified move effects; fallback remains explicit until catalog is complete. */
static const char *move_effect(uint16_t id) {
  switch(id) {
    case 247: return "20% chance: target Sp. Def -1.";
    case 85: return "10% chance to paralyze target.";
    case 89: return "Hits all adjacent Pokemon.";
    case 94: return "10% chance: target Sp. Def -1.";
    case 98: return "Usually moves first (+1 priority).";
    case 52: return "10% chance to burn target.";
    case 55: return "Deals Water-type damage.";
    case 126: return "10% chance to burn target.";
    case 129: return "Ignores accuracy checks.";
    case 33: return "Deals Normal-type damage.";
    case 39: return "Lowers target Defense by 1.";
    case 45: return "Lowers target Attack by 1.";
    default: return "Effect details not added yet.";
  }
}

/* UltraDex electric theme */
#define UX_BG C_RGB(0,0,0)
#define UX_PANEL C_RGB(0,18,30)
#define UX_BLUE C_RGB(0,170,255)
#define UX_BLUE2 C_RGB(0,90,190)
#define UX_TEXT C_RGB(255,255,255)
#define UX_MUTED C_RGB(155,205,225)

static void ux_clear(void) { dclear(UX_BG); }
static void ux_text(int x,int y,const char *s) { dtext(x,y,UX_TEXT,s); }
static void ux_select(int y) { drect(3,y-3,392,y+14,UX_BLUE2); drect(3,y-3,392,y-2,UX_BLUE); }
static void head(const char *s) {
  ux_clear();
  drect(0,0,395,22,UX_PANEL);
  drect(0,21,395,22,UX_BLUE);
  dtext(8,5,UX_TEXT,s);
}
static void foot(const char *s) {
  drect(0,204,395,223,UX_PANEL);
  drect(0,204,395,204,UX_BLUE);
  dtext(6,209,UX_TEXT,s);
}
static void wait_back(void) {
  while(1) {
    key_event_t e=getkey();
    if(e.key==KEY_EXIT || e.key==KEY_EXE) return;
  }
}
static int ci_contains(const char *h,const char *n) {
  if(!n[0]) return 1;
  for(int i=0;h[i];i++) {
    int j=0;
    while(n[j] && h[i+j] && tolower((unsigned char)n[j])==tolower((unsigned char)h[i+j])) j++;
    if(!n[j]) return 1;
  }
  return 0;
}

static void keyboard(char *buf,int cap,const char *label) {
  static const char keys[]="ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#- ";
  int cur=0; buf[0]=0;
  for(;;) {
    head(label);
    dtext(8,31,UX_TEXT,"Search:");
    dtext(72,31,UX_TEXT,buf);
    dtext(8,51,UX_TEXT,"Name or Dex #: 001 / #001 / Mewtwo");
    int cols=7, len=(int)strlen(keys);
    for(int i=0;i<len;i++) {
      int x=18+(i%cols)*50, y=82+(i/cols)*24;
      if(i==cur) drect(x-3,y-3,x+28,y+14,UX_BLUE2);
      char q[2]={keys[i],0};
      dtext(x,y,UX_TEXT,q);
    }
    foot("F6 Done   EXIT Cancel");
    dupdate();
    key_event_t e=getkey();
    if(e.key==KEY_LEFT) cur=(cur-1+len)%len;
    else if(e.key==KEY_RIGHT) cur=(cur+1)%len;
    else if(e.key==KEY_UP) cur=(cur-cols+len)%len;
    else if(e.key==KEY_DOWN) cur=(cur+cols)%len;
    else if(e.key==KEY_DEL) { int n=(int)strlen(buf); if(n) buf[n-1]=0; }
    else if(e.key==KEY_EXE) { int n=(int)strlen(buf); if(n+1<cap){buf[n]=keys[cur];buf[n+1]=0;} }
    else if(e.key==KEY_F6) return;
    else if(e.key==KEY_EXIT) { buf[0]=0; return; }
  }
}

static void show_move(const Move *m) {
  char b[96];
  head("UltraDex - Move");
  snprintf(b,sizeof b,"%s (#%u)",m->name,m->id); dtext(8,31,UX_TEXT,b);
  snprintf(b,sizeof b,"Type: %s",type_name(m->type)); dtext(8,52,UX_TEXT,b);
  snprintf(b,sizeof b,"Class: %s",m->cat==1?"Physical":m->cat==2?"Special":"Status"); dtext(8,72,UX_TEXT,b);
  if(m->power) snprintf(b,sizeof b,"Power: %u",m->power); else snprintf(b,sizeof b,"Power: --");
  dtext(8,92,UX_TEXT,b);
  if(m->acc) snprintf(b,sizeof b,"Accuracy: %u%%",m->acc); else snprintf(b,sizeof b,"Accuracy: --");
  dtext(8,112,UX_TEXT,b);
  snprintf(b,sizeof b,"PP: %u   Priority: %d",m->pp,m->priority); dtext(8,132,UX_TEXT,b);
  dtext(8,157,UX_TEXT,"Effect:");
  dtext(8,176,UX_TEXT,move_effect(m->id));
  foot("EXE/EXIT Back");
  dupdate(); wait_back();
}

static char alpha_letter(int key);

static void move_search(void) {
  char q[24]=""; int sel=0,top=0;
  for(;;) {
    int idx[MOVE_N],n=0;
    for(int i=0;i<MOVE_N;i++) if(ci_contains(moves[i].name,q)) idx[n++]=i;
    if(sel>=n)sel=n?n-1:0;
    if(sel<top)top=sel;
    if(sel>=top+8)top=sel-7;
    head("UltraDex - Move Search");
    char b[64]; snprintf(b,sizeof b,"Move: %s%s",q,q[0]?"":"_"); dtext(8,27,UX_TEXT,b);
    snprintf(b,sizeof b,"Matches: %d",n); dtext(285,27,UX_MUTED,b);
    for(int r=0;r<8 && top+r<n;r++) {
      const Move *m=&moves[idx[top+r]]; int y=48+r*18;
      if(top+r==sel)ux_select(y);
      snprintf(b,sizeof b,"%-17s %s",m->name,type_name(m->type));
      dtext(10,y,top+r==sel?UX_TEXT:UX_TEXT,b);
    }
    foot("ALPHA keys Type  DEL Erase  EXE Open");
    dupdate(); key_event_t e=getkey();
    if(e.key==KEY_DEL) { int z=(int)strlen(q); if(z)q[z-1]=0; sel=top=0; }
    else if(e.key==KEY_UP && sel>0)sel--;
    else if(e.key==KEY_DOWN && sel<n-1)sel++;
    else if(e.key==KEY_EXE && n)show_move(&moves[idx[sel]]);
    else if(e.key==KEY_EXIT)return;
    else {
      char ch=alpha_letter(e.key); int z=(int)strlen(q);
      if(ch && z+1<(int)sizeof q){q[z]=ch;q[z+1]=0;sel=top=0;}
    }
  }
}

static void show_mon(const Pokemon *p) {
  int page=0;
  for(;;) {
    char b[120]; head("UltraDex - Pokemon");
    snprintf(b,sizeof b,"#%03u %s  Gen %u",p->dex,p->name,p->gen); dtext(8,30,UX_TEXT,b);
    snprintf(b,sizeof b,"%s%s%s",type_name(p->t1),p->t2!=T_NONE?" / ":"",p->t2!=T_NONE?type_name(p->t2):"");
    dtext(8,49,UX_TEXT,b);
    if(page==0) {
      snprintf(b,sizeof b,"HP %u  ATK %u  DEF %u",p->hp,p->atk,p->def); dtext(8,79,UX_TEXT,b);
      snprintf(b,sizeof b,"SPA %u  SPD %u  SPE %u",p->spa,p->spd,p->spe); dtext(8,99,UX_TEXT,b);
      dtext(8,125,UX_TEXT,"Abilities:"); dtext(8,144,UX_TEXT,p->abilities);
      dtext(8,169,UX_TEXT,"Evolution:"); dtext(8,188,UX_TEXT,p->evo);
    } else {
      dtext(8,76,UX_TEXT,"Learnset sample:");
      for(int i=0;i<p->learn_n && i<6;i++) {
        const Move *m=move_by_id(p->learn[i].move_id); if(!m)continue;
        if(p->learn[i].method==0) snprintf(b,sizeof b,"Lv %02u  %s",p->learn[i].level,m->name);
        else if(p->learn[i].method==1) snprintf(b,sizeof b,"TM/HM  %s",m->name);
        else snprintf(b,sizeof b,"Other  %s",m->name);
        dtext(14,98+i*17,UX_TEXT,b);
      }
    }
    foot("F1 Info  F2 Moves  EXIT");
    dupdate(); key_event_t e=getkey();
    if(e.key==KEY_F1)page=0;
    else if(e.key==KEY_F2)page=1;
    else if(e.key==KEY_EXIT)return;
  }
}

static uint8_t gen_for_dex(uint16_t dex) {
  if(dex<=151) return 1;
  if(dex<=251) return 2;
  if(dex<=386) return 3;
  if(dex<=493) return 4;
  if(dex<=649) return 5;
  if(dex<=721) return 6;
  if(dex<=809) return 7;
  if(dex<=905) return 8;
  return dex<=1025 ? 9 : 0;
}

static void show_dex_entry(int dex);

static void browse_generation(int gen, int first, int last, const char *region) {
  int sel=0,top=0;
  const int count=last-first+1;
  for(;;) {
    if(sel<top)top=sel;
    if(sel>=top+8)top=sel-7;
    char b[72];
    snprintf(b,sizeof b,"Gen %d - %s",gen,region);
    head(b);
    snprintf(b,sizeof b,"Pokemon #%03d - #%04d   (%d)",first,last,count);
    dtext(8,27,UX_MUTED,b);
    for(int r=0;r<8 && top+r<count;r++) {
      int dex=first+top+r, y=48+r*18;
      if(top+r==sel)ux_select(y);
      snprintf(b,sizeof b,"#%03d  %s",dex,national_dex[dex-1].name);
      dtext(10,y,UX_TEXT,b);
    }
    foot("UP/DN Browse  EXE Open  EXIT Back");
    dupdate();
    key_event_t e=getkey();
    if(e.key==KEY_UP && sel>0)sel--;
    else if(e.key==KEY_DOWN && sel<count-1)sel++;
    else if(e.key==KEY_EXE)show_dex_entry(first+sel);
    else if(e.key==KEY_EXIT)return;
  }
}

static void generation_info(void) {
  static const char *regions[]={"Kanto","Johto","Hoenn","Sinnoh","Unova","Kalos","Alola","Galar/Hisui","Paldea"};
  static const uint16_t first[]={1,152,252,387,494,650,722,810,906};
  static const uint16_t last[]={151,251,386,493,649,721,809,905,1025};
  int sel=0;
  for(;;) {
    head("UltraDex - Generations");
    for(int i=0;i<9;i++) {
      int y=29+i*18;
      char b[64];
      if(i==sel)ux_select(y);
      snprintf(b,sizeof b,"Gen %d  %-11s #%03u-%04u",i+1,regions[i],first[i],last[i]);
      dtext(10,y,UX_TEXT,b);
    }
    foot("UP/DN Select  EXE Open  EXIT Back");
    dupdate();
    key_event_t e=getkey();
    if(e.key==KEY_UP)sel=(sel+8)%9;
    else if(e.key==KEY_DOWN)sel=(sel+1)%9;
    else if(e.key==KEY_EXE)browse_generation(sel+1,first[sel],last[sel],regions[sel]);
    else if(e.key==KEY_EXIT)return;
  }
}

static int dex_query(const char *q) {
  const char *s=q;
  if(*s=='#') s++;
  if(!*s) return -1;
  int n=0;
  while(*s) {
    if(!isdigit((unsigned char)*s)) return -1;
    n=n*10+(*s-'0');
    if(n>1025) return -1;
    s++;
  }
  return (n>=1 && n<=1025) ? n : -1;
}

static char alpha_letter(int key) {
  if(key==KEY_XOT)return 'A'; if(key==KEY_LOG)return 'B'; if(key==KEY_LN)return 'C';
  if(key==KEY_SIN)return 'D'; if(key==KEY_COS)return 'E'; if(key==KEY_TAN)return 'F';
  if(key==KEY_FRAC)return 'G'; if(key==KEY_FD)return 'H'; if(key==KEY_LEFTP)return 'I';
  if(key==KEY_RIGHTP)return 'J'; if(key==KEY_COMMA)return 'K'; if(key==KEY_ARROW)return 'L';
  if(key==KEY_7)return 'M'; if(key==KEY_8)return 'N'; if(key==KEY_9)return 'O';
  if(key==KEY_4)return 'P'; if(key==KEY_5)return 'Q'; if(key==KEY_6)return 'R';
  if(key==KEY_MUL)return 'S'; if(key==KEY_DIV)return 'T'; if(key==KEY_1)return 'U';
  if(key==KEY_2)return 'V'; if(key==KEY_3)return 'W'; if(key==KEY_ADD)return 'X';
  if(key==KEY_SUB)return 'Y'; if(key==KEY_0)return 'Z'; return 0;
}

static void show_dex_entry(int dex);

static void mon_search(void) {
  char q[18]=""; int sel=0,top=0;
  for(;;) {
    int idx[1025],n=0;
    for(int i=0;i<1025;i++) if(ci_contains(national_dex[i].name,q)) idx[n++]=i;
    if(sel>=n)sel=n?n-1:0;
    if(sel<top)top=sel;
    if(sel>=top+8)top=sel-7;
    head("UltraDex - Name Search");
    char b[64]; snprintf(b,sizeof b,"Name: %s%s",q,q[0]?"":"_"); dtext(8,27,UX_TEXT,b);
    snprintf(b,sizeof b,"Matches: %d",n); dtext(285,27,UX_MUTED,b);
    for(int r=0;r<8 && top+r<n;r++) {
      const DexEntry *p=&national_dex[idx[top+r]]; int y=48+r*18;
      if(top+r==sel)ux_select(y);
      snprintf(b,sizeof b,"#%03d %-20s G%u",idx[top+r]+1,p->name,p->gen);
      dtext(10,y,top+r==sel?UX_TEXT:UX_TEXT,b);
    }
    foot("ALPHA keys Type  DEL Erase  EXE Open");
    dupdate(); key_event_t e=getkey();
    if(e.key==KEY_DEL) { int z=(int)strlen(q); if(z)q[z-1]=0; sel=top=0; }
    else if(e.key==KEY_UP && sel>0)sel--;
    else if(e.key==KEY_DOWN && sel<n-1)sel++;
    else if(e.key==KEY_EXE && n)show_dex_entry(idx[sel]+1);
    else if(e.key==KEY_EXIT)return;
    else {
      char ch=alpha_letter(e.key); int z=(int)strlen(q);
      if(ch && z+1<(int)sizeof q){q[z]=ch;q[z+1]=0;sel=top=0;}
    }
  }
}

static int digit_from_key(int key) {
  if(key==KEY_0) return 0;
  if(key==KEY_1) return 1;
  if(key==KEY_2) return 2;
  if(key==KEY_3) return 3;
  if(key==KEY_4) return 4;
  if(key==KEY_5) return 5;
  if(key==KEY_6) return 6;
  if(key==KEY_7) return 7;
  if(key==KEY_8) return 8;
  if(key==KEY_9) return 9;
  return -1;
}

static TypeId pokeapi_type(uint8_t t) {
  static const TypeId map[19]={T_NONE,T_NORMAL,T_FIGHTING,T_FLYING,T_POISON,T_GROUND,T_ROCK,T_BUG,T_GHOST,T_STEEL,T_FIRE,T_WATER,T_GRASS,T_ELECTRIC,T_PSYCHIC,T_ICE,T_DRAGON,T_DARK,T_FAIRY};
  return t<19?map[t]:T_NONE;
}

static void show_dex_entry(int dex) {
  char b[96];
  const DexEntry *p=&national_dex[dex-1];
  head("UltraDex - National Dex");
  snprintf(b,sizeof b,"#%03d %s   Gen %u",dex,p->name,p->gen); dtext(8,32,UX_TEXT,b);
  snprintf(b,sizeof b,"Type: %s%s%s",type_name(pokeapi_type(p->t1)),p->t2?" / ":"",p->t2?type_name(pokeapi_type(p->t2)):""); dtext(8,56,UX_TEXT,b);
  dtext(8,84,UX_TEXT,"Base Stats");
  snprintf(b,sizeof b,"HP  %3u     ATK %3u     DEF %3u",p->hp,p->atk,p->def); dtext(8,108,UX_TEXT,b);
  snprintf(b,sizeof b,"SPA %3u     SPD %3u     SPE %3u",p->spa,p->spd,p->spe); dtext(8,132,UX_TEXT,b);
  snprintf(b,sizeof b,"Base Stat Total: %u",(unsigned)(p->hp+p->atk+p->def+p->spa+p->spd+p->spe)); dtext(8,154,UX_TEXT,b);
  const DexAbilities *a=&dex_abilities[dex-1];
  snprintf(b,sizeof b,"Ability: %s%s%s",ability_names[a->a0],a->a1?" / ":"",a->a1?ability_names[a->a1]:""); dtext(8,176,UX_TEXT,b);
  if(a->hidden){snprintf(b,sizeof b,"Hidden: %s",ability_names[a->hidden]); dtext(8,194,UX_TEXT,b);}
  foot("EXE/EXIT Back");
  dupdate(); wait_back();
}

static void dex_number_search(void) {
  char buf[5]="";
  for(;;) {
    head("UltraDex - Dex Number");
    dtext(8,38,UX_TEXT,"Type # using calculator number keys:");
    dtext(8,70,UX_TEXT,buf[0]?buf:"_");
    dtext(8,102,UX_TEXT,"Valid range: 1 - 1025");
    foot("0-9 Type  DEL Erase  EXE Open  EXIT");
    dupdate();
    key_event_t e=getkey();
    int d=digit_from_key(e.key);
    if(d>=0) {
      int n=(int)strlen(buf);
      if(n<4) { buf[n]=(char)('0'+d); buf[n+1]=0; }
    } else if(e.key==KEY_DEL) {
      int n=(int)strlen(buf); if(n) buf[n-1]=0;
    } else if(e.key==KEY_EXE) {
      int n=0;
      for(int i=0;buf[i];i++) n=n*10+(buf[i]-'0');
      if(n>=1 && n<=1025) show_dex_entry(n);
    } else if(e.key==KEY_EXIT) return;
  }
}

int main(void) {
  int sel=0;
  const char *menu[]={"Dex # Search","Name Search","Move Search","Generations","About"};
  for(;;) {
    head("UltraDex-CG50"); /* Mewtwo icon build */
    dtext(8,29,UX_TEXT,"National Dex Gen 1-9 (#001-1025)");
    for(int i=0;i<5;i++) {
      int y=65+i*28;
      if(i==sel){ drect(8,y-4,386,y+17,UX_BLUE2); drect(8,y-4,386,y-3,UX_BLUE); }
      dtext(18,y,UX_TEXT,menu[i]);
    }
    foot("UP/DN Select  EXE Open  EXIT Quit");
    dupdate(); key_event_t e=getkey();
    if(e.key==KEY_UP)sel=(sel+4)%5;
    else if(e.key==KEY_DOWN)sel=(sel+1)%5;
    else if(e.key==KEY_EXIT)break;
    else if(e.key==KEY_EXE) {
      if(sel==0)dex_number_search();
      else if(sel==1)mon_search();
      else if(sel==2)move_search();
      else if(sel==3)generation_info();
      else {
        head("About UltraDex");
        dtext(8,43,UX_TEXT,"Text-first Pokedex for fx-CG50.");
        dtext(8,66,UX_TEXT,"National Dex target: #001-1025 (Gen 1-9).");
        dtext(8,89,UX_TEXT,"Moves, evolutions, stats and learnsets.");
        dtext(8,112,UX_TEXT,"Physical-key Dex search enabled.");
        foot("EXE/EXIT Back"); dupdate(); wait_back();
      }
    }
  }
  return 1;
}
