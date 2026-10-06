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
 {1,"Pound",T_NORMAL,1,40,100,35,0,"Deals damage."},
 {22,"Vine Whip",T_GRASS,1,45,100,25,0,"Strikes the target with vines."},
 {33,"Tackle",T_NORMAL,1,40,100,35,0,"A basic physical attack."},
 {39,"Tail Whip",T_NORMAL,0,0,100,30,0,"Lowers the target's Defense."},
 {45,"Growl",T_NORMAL,0,0,100,40,0,"Lowers the target's Attack."},
 {52,"Ember",T_FIRE,2,40,100,25,0,"May burn the target."},
 {55,"Water Gun",T_WATER,2,40,100,25,0,"Blasts the target with water."},
 {84,"Thunder Shock",T_ELECTRIC,2,40,100,30,0,"May paralyze the target."},
 {85,"Thunderbolt",T_ELECTRIC,2,90,100,15,0,"May paralyze the target."},
 {89,"Earthquake",T_GROUND,1,100,100,10,0,"Powerful Ground attack."},
 {94,"Psychic",T_PSYCHIC,2,90,100,10,0,"May lower Special Defense."},
 {98,"Quick Attack",T_NORMAL,1,40,100,30,1,"Attacks with increased priority."},
 {126,"Fire Blast",T_FIRE,2,110,85,5,0,"Heavy Fire damage; may burn."},
 {129,"Swift",T_NORMAL,2,60,0,20,0,"Normally does not miss."},
 {237,"Hidden Power",T_NORMAL,2,60,100,15,0,"Type varies in older games."},
 {247,"Shadow Ball",T_GHOST,2,80,100,15,0,"May lower Special Defense."},
 {249,"Rock Smash",T_FIGHTING,1,40,100,15,0,"May lower Defense."},
 {331,"Bullet Seed",T_GRASS,1,25,100,30,0,"Hits two to five times."},
 {337,"Dragon Claw",T_DRAGON,1,80,100,15,0,"Physical Dragon-type damage."},
 {344,"Volt Tackle",T_ELECTRIC,1,120,100,15,0,"Strong recoil attack; may paralyze."},
 {394,"Flare Blitz",T_FIRE,1,120,100,15,0,"Strong recoil attack; may burn."},
 {399,"Dark Pulse",T_DARK,2,80,100,15,0,"May make the target flinch."},
 {404,"X-Scissor",T_BUG,1,80,100,15,0,"Physical Bug-type attack."},
 {412,"Energy Ball",T_GRASS,2,90,100,10,0,"May lower Special Defense."},
 {430,"Flash Cannon",T_STEEL,2,80,100,10,0,"May lower Special Defense."}
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

static void head(const char *s) {
  dclear(C_WHITE);
  drect(0,0,395,22,C_BLACK);
  dtext(8,5,C_WHITE,s);
}
static void foot(const char *s) {
  drect(0,205,395,223,C_BLACK);
  dtext(6,209,C_WHITE,s);
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
    dtext(8,31,C_BLACK,"Search:");
    dtext(72,31,C_BLACK,buf);
    dtext(8,51,C_BLACK,"Name or Dex #: 001 / #001 / Mewtwo");
    int cols=7, len=(int)strlen(keys);
    for(int i=0;i<len;i++) {
      int x=18+(i%cols)*50, y=82+(i/cols)*24;
      if(i==cur) drect(x-3,y-3,x+28,y+14,C_BLACK);
      char q[2]={keys[i],0};
      dtext(x,y,i==cur?C_WHITE:C_BLACK,q);
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
  snprintf(b,sizeof b,"%s (#%u)",m->name,m->id); dtext(8,31,C_BLACK,b);
  snprintf(b,sizeof b,"Type: %s",type_name(m->type)); dtext(8,52,C_BLACK,b);
  snprintf(b,sizeof b,"Class: %s",m->cat==1?"Physical":m->cat==2?"Special":"Status"); dtext(8,72,C_BLACK,b);
  if(m->power) snprintf(b,sizeof b,"Power: %u",m->power); else snprintf(b,sizeof b,"Power: --");
  dtext(8,92,C_BLACK,b);
  if(m->acc) snprintf(b,sizeof b,"Accuracy: %u%%",m->acc); else snprintf(b,sizeof b,"Accuracy: --");
  dtext(8,112,C_BLACK,b);
  snprintf(b,sizeof b,"PP: %u   Priority: %d",m->pp,m->priority); dtext(8,132,C_BLACK,b);
  dtext(8,157,C_BLACK,"Effect:");
  dtext(8,176,C_BLACK,m->effect);
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
    char b[64]; snprintf(b,sizeof b,"Move: %s%s",q,q[0]?"":"_"); dtext(8,27,C_BLACK,b);
    snprintf(b,sizeof b,"Matches: %d",n); dtext(285,27,C_BLACK,b);
    for(int r=0;r<8 && top+r<n;r++) {
      const Move *m=&moves[idx[top+r]]; int y=48+r*18;
      if(top+r==sel)drect(4,y-2,391,y+14,C_BLACK);
      snprintf(b,sizeof b,"%-17s %s",m->name,type_name(m->type));
      dtext(10,y,top+r==sel?C_WHITE:C_BLACK,b);
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
    snprintf(b,sizeof b,"#%03u %s  Gen %u",p->dex,p->name,p->gen); dtext(8,30,C_BLACK,b);
    snprintf(b,sizeof b,"%s%s%s",type_name(p->t1),p->t2!=T_NONE?" / ":"",p->t2!=T_NONE?type_name(p->t2):"");
    dtext(8,49,C_BLACK,b);
    if(page==0) {
      snprintf(b,sizeof b,"HP %u  ATK %u  DEF %u",p->hp,p->atk,p->def); dtext(8,79,C_BLACK,b);
      snprintf(b,sizeof b,"SPA %u  SPD %u  SPE %u",p->spa,p->spd,p->spe); dtext(8,99,C_BLACK,b);
      dtext(8,125,C_BLACK,"Abilities:"); dtext(8,144,C_BLACK,p->abilities);
      dtext(8,169,C_BLACK,"Evolution:"); dtext(8,188,C_BLACK,p->evo);
    } else {
      dtext(8,76,C_BLACK,"Learnset sample:");
      for(int i=0;i<p->learn_n && i<6;i++) {
        const Move *m=move_by_id(p->learn[i].move_id); if(!m)continue;
        if(p->learn[i].method==0) snprintf(b,sizeof b,"Lv %02u  %s",p->learn[i].level,m->name);
        else if(p->learn[i].method==1) snprintf(b,sizeof b,"TM/HM  %s",m->name);
        else snprintf(b,sizeof b,"Other  %s",m->name);
        dtext(14,98+i*17,C_BLACK,b);
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
      if(i==sel)drect(4,y-2,391,y+14,C_BLACK);
      snprintf(b,sizeof b,"Gen %d  %-11s #%03u-%04u",i+1,regions[i],first[i],last[i]);
      dtext(10,y,i==sel?C_WHITE:C_BLACK,b);
    }
    foot("UP/DN Browse   EXIT Back");
    dupdate();
    key_event_t e=getkey();
    if(e.key==KEY_UP)sel=(sel+8)%9;
    else if(e.key==KEY_DOWN)sel=(sel+1)%9;
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
    char b[64]; snprintf(b,sizeof b,"Name: %s%s",q,q[0]?"":"_"); dtext(8,27,C_BLACK,b);
    snprintf(b,sizeof b,"Matches: %d",n); dtext(285,27,C_BLACK,b);
    for(int r=0;r<8 && top+r<n;r++) {
      const DexEntry *p=&national_dex[idx[top+r]]; int y=48+r*18;
      if(top+r==sel)drect(4,y-2,391,y+14,C_BLACK);
      snprintf(b,sizeof b,"#%03d %-20s G%u",idx[top+r]+1,p->name,p->gen);
      dtext(10,y,top+r==sel?C_WHITE:C_BLACK,b);
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
  snprintf(b,sizeof b,"#%03d %s   Gen %u",dex,p->name,p->gen); dtext(8,32,C_BLACK,b);
  snprintf(b,sizeof b,"Type: %s%s%s",type_name(pokeapi_type(p->t1)),p->t2?" / ":"",p->t2?type_name(pokeapi_type(p->t2)):""); dtext(8,56,C_BLACK,b);
  dtext(8,84,C_BLACK,"Base Stats");
  snprintf(b,sizeof b,"HP  %3u     ATK %3u     DEF %3u",p->hp,p->atk,p->def); dtext(8,108,C_BLACK,b);
  snprintf(b,sizeof b,"SPA %3u     SPD %3u     SPE %3u",p->spa,p->spd,p->spe); dtext(8,132,C_BLACK,b);
  snprintf(b,sizeof b,"Base Stat Total: %u",(unsigned)(p->hp+p->atk+p->def+p->spa+p->spd+p->spe)); dtext(8,154,C_BLACK,b);
  const DexAbilities *a=&dex_abilities[dex-1];
  snprintf(b,sizeof b,"Ability: %s%s%s",ability_names[a->a0],a->a1?" / ":"",a->a1?ability_names[a->a1]:""); dtext(8,176,C_BLACK,b);
  if(a->hidden){snprintf(b,sizeof b,"Hidden: %s",ability_names[a->hidden]); dtext(8,194,C_BLACK,b);}
  foot("EXE/EXIT Back");
  dupdate(); wait_back();
}

static void dex_number_search(void) {
  char buf[5]="";
  for(;;) {
    head("UltraDex - Dex Number");
    dtext(8,38,C_BLACK,"Type # using calculator number keys:");
    dtext(8,70,C_BLACK,buf[0]?buf:"_");
    dtext(8,102,C_BLACK,"Valid range: 1 - 1025");
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
    dtext(8,29,C_BLACK,"National Dex Gen 1-9 (#001-1025)");
    for(int i=0;i<5;i++) {
      int y=65+i*28;
      if(i==sel)drect(8,y-4,386,y+17,C_BLACK);
      dtext(18,y,sel==i?C_WHITE:C_BLACK,menu[i]);
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
        dtext(8,43,C_BLACK,"Text-first Pokedex for fx-CG50.");
        dtext(8,66,C_BLACK,"National Dex target: #001-1025 (Gen 1-9).");
        dtext(8,89,C_BLACK,"Moves, evolutions, stats and learnsets.");
        dtext(8,112,C_BLACK,"Physical-key Dex search enabled.");
        foot("EXE/EXIT Back"); dupdate(); wait_back();
      }
    }
  }
  return 1;
}
