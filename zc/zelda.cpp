#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <map>
#include <vector>

#include "zscriptversion.h"
#include "zdefs.h"
#include "zcmusic.h"
#include "zelda.h"
#include "tiles.h"
#include "pal.h"
#include "qst.h"
#include "particles.h"
#include "ffscript.h"
#include "zcarray.h"
#include "sprite.h"
#include "custom.h"
#include "link.h"
#include "maps.h"
#include "subscr.h"
#include "guys.h"
#include "title.h"
#include "ending.h"
#include "zcsys.h"

ZCMUSIC *zcmusic = NULL;
zinitdata zinit;
int lens_hint_item[MAXITEMS][2];                            //aclk, aframe
int lens_hint_weapon[MAXWPNS][5];                           //aclk, aframe, dir, x, y
int strike_hint_counter = 0;
int strike_hint_timer = 0;
int strike_hint;

CScriptDrawingCommands script_drawing_commands;

using std::string;
using std::pair;
extern std::map<int, pair<string, string> > ffcmap;

extern int directItem;
extern int directItemA;
extern int directItemB;

int favorite_combos[MAXFAVORITECOMBOS];
int favorite_comboaliases[MAXFAVORITECOMBOALIASES];

int dmap_list_size = MAXDMAPS;
bool dmap_list_zero = true;

/**********************************/
/******** Global Variables ********/
/**********************************/

/* pointers to bitmap and palette objects used by Libretro */
BITMAP *zc_canvas;
RGB *zc_palette;

RGB_MAP rgb_table;
COLOR_MAP trans_table, trans_table2;

BITMAP     *framebuf, *scrollbuf, *tempbuf, *msgdisplaybuf, *pricesdisplaybuf, *prim_bmp,
           *lens_scr = NULL;
DATAFILE   *data, *sfxdata, *fontsdata, *mididata;
FONT       *nfont, *zfont, *z3font, *z3smallfont, *deffont, *lfont, *lfont_l, *pfont, *ztfont, *sfont, *sfont2,
           *sfont3, *spfont, *ssfont1, *ssfont2, *ssfont3, *ssfont4, *gblafont,
           *goronfont, *zoranfont, *hylian1font, *hylian2font, *hylian3font, *hylian4font, *gboraclefont, *gboraclepfont,
           *dsphantomfont, *dsphantompfont;
PALETTE    RAMpal;
uint8_t       *colordata;
itemdata   *itemsbuf;
wpndata    *wpnsbuf;
comboclass *combo_class_buf;
guydata    *guysbuf;
item_drop_object    item_drop_sets[MAXITEMDROPSETS];

bool ewind_restart = false;

uint16_t     msgclk, msgstr,
         msgpos,  // screen position of the next character.
         msgptr,  // position within the string of the next character. <MSGSIZE.
         msgcolour,  // colour to use for the displayed text.
         msgspeed,   // delay between each character (5 = default).
         msg_w,
         msg_h,
         msg_count,
         cursor_x,
         cursor_y,
         msg_xpos = 0,
         msg_ypos = 0,
         msgorig = 0;
bool msg_onscreen = false, msg_active = false, msgspace = false;
BITMAP   *msgbmpbuf = NULL;
FONT   *msgfont;
uint16_t     door_combo_set_count;
uint16_t     introclk, intropos, dmapmsgclk, linkedmsgclk;
short    lensclk;
int     lensid; // Lens's item id. -1 if lens is off.
int    Bpos;
uint8_t screengrid[22];
uint8_t ffcgrid[4];
bool screenscrolling = false;
PALETTE tempbombpal;
bool usebombpal;

combo_alias combo_aliases[MAXCOMBOALIASES];

SAMPLE customsfxdata[SFX_COUNT];
bool use_sfxdat = true;

int homescr, currscr, frame = 0, currmap = 0, dlevel, warpscr, worldscr;
int newscr_clk = 0, opendoors = 0, currdmap = 0, fadeclk = -1, currgame = 0, listpos = 0;
int lastentrance = 0, lastentrance_dmap = 0, prices[3], loadside, Bwpn, Awpn;
int master_vol, music_vol, sfx_vol, mix_quality, sel_music, hasitem, whistleclk, pan_style;
int AKey, BKey, SelectKey, StartKey, Lkey, Rkey, MapKey, ModKey, Ex1key, Ex2key, Ex3key, Ex4key, zc_state = ZC_RUN;
int UpKey, DownKey, LeftKey, RightKey, MouseX, MouseY, MouseZ, MouseB;
int hs_startx, hs_starty, hs_xdist, hs_ydist, clockclk, clock_zoras[eMAXGUYS];
int cheat_goto_dmap = 0, cheat_goto_screen = 0, currcset;
int gfc, gfc2, pitx, pity, refill_what, refill_why, heart_beep_timer = 0, new_enemy_tile_start = 1580;
int nets = 1580, magicitem = -1, nayruitem = -1, magiccastclk, quakeclk = 0, wavy = 0, castx, casty, df_x, df_y, nl1_x,
    nl1_y, nl2_x, nl2_y;
int magicdrainclk = 0, conveyclk = 3;
int checkx, checky;
int skipcont = 0;

bool playing, trans_layers, heart_beep, allow_cheats;
bool zc_sync_pal, blockpath, loaded_guys, freeze_guys,
     loaded_enemies, drawguys, watch;
bool darkroom = false, naturaldark = false, BSZ;
bool Udown, Ddown, Ldown, Rdown, Adown, Bdown, Sdown, Mdown, LBdown, RBdown, Pdown, Ex1down, Ex2down, Ex3down, Ex4down,
     AUdown, ADdown, ALdown, ARdown,
     nes_quit, boughtsomething = false,
              fixed_door = false, hookshot_used = false, hookshot_frozen = false,
              pull_link = false, add_chainlink = false, del_chainlink = false, hs_fix = false,
              cheat_superman = false, checklink = true, didpit = false,
              castnext = false, add_df1asparkle, add_df1bsparkle, add_nl1asparkle, add_nl1bsparkle, add_nl2asparkle, add_nl2bsparkle,
              is_on_conveyor, activated_timed_warp = false;

uint8_t COOLSCROLL;

int  add_asparkle = 0, add_bsparkle = 0;

short  visited[6];
uint8_t   guygrid[176];
mapscr tmpscr[2];
mapscr tmpscr2[6];
mapscr tmpscr3[6];
gamedata *game = NULL;
ffscript *ffscripts[NUMSCRIPTFFC];
ffscript *itemscripts[NUMSCRIPTITEM];
ffscript *globalscripts[NUMSCRIPTGLOBAL];

//If only...
ffscript *guyscripts[NUMSCRIPTGUYS];
ffscript *wpnscripts[NUMSCRIPTWEAPONS];
ffscript *linkscripts[NUMSCRIPTLINK];
ffscript *screenscripts[NUMSCRIPTSCREEN];

extern refInfo globalScriptData;
extern uint16_t g_doscript;
extern bool global_wait;

//ZScript array storage
std::vector<ZScriptArray> globalRAM;
ZScriptArray localRAM[MAX_ZCARRAY_SIZE];
uint8_t arrayOwner[MAX_ZCARRAY_SIZE];

//script bitmap drawing
ZScriptDrawingRenderTarget *zscriptDrawingRenderTarget;

// quest file data
zquestheader QHeader;
uint8_t                quest_rules[QUESTRULES_SIZE];
uint8_t                midi_flags[MIDIFLAGS_SIZE];
uint16_t                map_count;
MsgStr              *MsgStrings;
int                 msg_strings_size;
DoorComboSet        *DoorComboSets;
dmap                *DMaps;
miscQdata           QMisc;
std::vector<mapscr> TheMaps;
zcmap               *ZCMaps;
uint32_t               quest_map_pos[MAPSCRS * MAXMAPS2];

char     qst_path[MAX_STRLEN];
gamedata *saves = NULL;

bool blockmoving;
movingblock mblock2;
sprite_list  guys, items, Ewpns, Lwpns, Sitems, chainlinks, decorations, particles;
LinkClass   Link;

/**********************************/
/*********** Misc Data ************/
/**********************************/

const char startguy[8] = {-13, -13, -13, -14, -15, -18, -21, -40};
const char gambledat[12 * 6] =
{
   20, -10, -10, 20, -10, -10, 20, -40, -10, 20, -10, -40,
   50, -10, -10, 50, -10, -10, 50, -40, -10, 50, -10, -40,
   -10, 20, -10, -10, 20, -10, -40, 20, -10, -10, 20, -40,
   -10, 50, -10, -10, 50, -10, -40, 50, -10, -10, 50, -40,
   -10, -10, 20, -10, -10, 20, -10, -40, 20, -40, -10, 20,
   -10, -10, 50, -10, -10, 50, -10, -40, 50, -40, -10, 50
};
const uint8_t stx[4][9] =
{
   { 48, 80, 80, 96, 112, 144, 160, 160, 192},
   { 48, 80, 80, 96, 128, 144, 160, 160, 192},
   { 80, 80, 128, 128, 160, 160, 192, 192, 208},
   { 32, 48, 48, 80, 80, 112, 112, 160, 160}
};
const uint8_t sty[4][9] =
{
   {112, 64, 128, 96, 80, 96, 64, 128, 112},
   { 48, 32, 96, 64, 80, 64, 32, 96, 48},
   { 32, 128, 64, 96, 64, 96, 48, 112, 80},
   { 80, 48, 112, 64, 96, 64, 96, 32, 128}
};

const uint8_t ten_rupies_x[10] = {120, 112, 128, 96, 112, 128, 144, 112, 128, 120};
const uint8_t ten_rupies_y[10] = {49, 65, 65, 81, 81, 81, 81, 97, 97, 113};

zctune tunes[MAXMIDIS] =
{
   // (title)                              (s) (ls) (le) (l) (vol) (midi)(fmt)
   zctune((char *)"Zelda - Dungeon",     0,  -1,  -1,  1,  176,  NULL, 0),
   zctune((char *)"Zelda - Ending",      0, 129, 225,  1,  160,  NULL, 0),
   zctune((char *)"Zelda - Game Over",   0,  -1,  -1,  1,  224,  NULL, 0),
   zctune((char *)"Zelda - Level 9",     0,  -1,  -1,  1,  255,  NULL, 0),
   zctune((char *)"Zelda - Overworld",   0,  17,  -1,  1,  208,  NULL, 0),
   zctune((char *)"Zelda - Title",       0,  -1,  -1,  0,  168,  NULL, 0),
   zctune((char *)"Zelda - Triforce",    0,  -1,  -1,  0,  168,  NULL, 0)
};

void setZScriptVersion(int s_version)
{
   ZScriptVersion::setVersion(s_version);
}

void initZScriptArrayRAM(bool firstplay)
{
   for (uint16_t i = 0; i < MAX_ZCARRAY_SIZE; i++)
   {
      localRAM[i].Clear();
      arrayOwner[i] = 255;
   }

   if (game->globalRAM.size() != 0)
      game->globalRAM.clear();

   if (firstplay)
   {
      //leave to global script ~Init to allocate global memory first time round
      game->globalRAM.resize(getNumGlobalArrays());
   }
   else
   {
      //allocate from save file
      game->globalRAM.resize(saves[currgame].globalRAM.size());

      for (uint32_t i = 0; i < game->globalRAM.size(); i++)
      {
         ZScriptArray &from = saves[currgame].globalRAM[i];
         ZScriptArray &to = game->globalRAM[i];
         to.Resize(from.Size());

         for (uint32_t j = 0; j < from.Size(); j++)
            to[j] = from[j];
      }
   }
}

void initZScriptGlobalRAM()
{
   g_doscript = 1;
   globalScriptData.Clear();
   clear_global_stack();
}

uint32_t getNumGlobalArrays()
{
   uint16_t scommand, pc = 0, ret = 0;

   do
   {
      scommand = globalscripts[GLOBAL_SCRIPT_INIT][pc].command;

      if (scommand == ALLOCATEGMEMV || scommand == ALLOCATEGMEMR)
         ret++;

      pc++;
   }
   while (scommand != 0xFFFF);

   return ret;
}

FONT *setmsgfont()
{
   switch (MsgStrings[msgstr].font)
   {
      default:
         return zfont;

      case font_z3font:
         return z3font;

      case font_z3smallfont:
         return z3smallfont;

      case font_deffont:
         return deffont;

      case font_lfont:
         return lfont;

      case font_lfont_l:
         return lfont_l;

      case font_pfont:
         return pfont;

      case font_mfont:
         return zfont;

      case font_ztfont:
         return ztfont;

      case font_sfont:
         return sfont;

      case font_sfont2:
         return sfont2;

      case font_spfont:
         return spfont;

      case font_ssfont1:
         return ssfont1;

      case font_ssfont2:
         return ssfont2;

      case font_ssfont3:
         return ssfont3;

      case font_ssfont4:
         return ssfont4;

      case font_gblafont:
         return gblafont;

      case font_goronfont:
         return goronfont;

      case font_zoranfont:
         return zoranfont;

      case font_hylian1font:
         return hylian1font;

      case font_hylian2font:
         return hylian2font;

      case font_hylian3font:
         return hylian3font;

      case font_hylian4font:
         return hylian4font;

      case font_gboraclefont:
         return gboraclefont;

      case font_gboraclepfont:
         return gboraclepfont;

      case font_dsphantomfont:
         return dsphantomfont;

      case font_dsphantompfont:
         return dsphantompfont;
   }
}

void donewmsg(int str)
{
   if (msg_onscreen || msg_active)
      dismissmsg();

   linkedmsgclk = 0;
   msg_active = true;
   // Don't set msg_onscreen - not onscreen just yet
   msgstr = str;
   msgorig = msgstr;
   msgfont = setmsgfont();
   msgcolour = QMisc.colors.msgtext;
   msgspeed = zinit.msg_speed;

   if (introclk == 0 || (introclk >= 72 && dmapmsgclk == 0))
      clear_bitmap(msgdisplaybuf);

   clear_bitmap(msgdisplaybuf);
   set_clip_state(msgdisplaybuf, 1);
   clear_bitmap(msgbmpbuf);

   if (MsgStrings[msgstr].tile != 0)
   {
      frame2x2(msgbmpbuf, &QMisc, 0, 0, MsgStrings[msgstr].tile, MsgStrings[msgstr].cset,
               (MsgStrings[msgstr].w >> 3) + 2, (MsgStrings[msgstr].h >> 3) + 2, 0, true, 0);
   }

   msgclk = msgpos = msgptr = 0;
   msgspace = true;
   msg_w = MsgStrings[msgstr].w;
   msg_h = MsgStrings[msgstr].h;
   msg_xpos = MsgStrings[msgstr].x;
   msg_ypos = MsgStrings[msgstr].y;
   cursor_x = 0;
   cursor_y = 0;
}

// Called to make a message disappear
void dismissmsg()
{
   linkedmsgclk = 0;
   msgstr = msgclk = msgpos = msgptr = 0;
   cursor_x = 0;
   cursor_y = 0;
   msg_onscreen = msg_active = false;
   //Link.finishedmsg(); //Not possible?
   clear_bitmap(msgdisplaybuf);
   set_clip_state(msgdisplaybuf, 1);
}

void dointro()
{
   if (game->visited[currdmap] != 1 || (DMaps[currdmap].flags & dmfALWAYSMSG) != 0)
   {
      dmapmsgclk = 0;
      game->visited[currdmap] = 1;
      introclk = intropos = 0;
   }
   else
      introclk = intropos = 72;
}

bool bad_version(int version)
{
   // minimum zquest version allowed for any quest file
   if (version < 0x183)
      return true;

   return false;
}

extern char *item_string[];
extern char *guy_string[];


/**********************************/
/******* Other Source Files *******/
/**********************************/

void Z_eventlog(const char *format, ...)
{
   if (get_bit(quest_rules, qr_LOG))
   {
      char buf[MAX_STRLEN];

      va_list ap;
      va_start(ap, format);
      vsprintf(buf, format, ap);
      va_end(ap);
      zc_message("%s", buf);
   }
}

void Z_scripterrlog(const char *const format, ...)
{
   if (get_bit(quest_rules, qr_SCRIPTERRLOG))
   {
      char buf[MAX_STRLEN];

      va_list ap;
      va_start(ap, format);
      vsprintf(buf, format, ap);
      va_end(ap);
      zc_error("%s", buf);
   }
}

void ALLOFF(bool messagesToo, bool decorationsToo)
{
   if (messagesToo)
   {
      clear_bitmap(msgdisplaybuf);
      set_clip_state(msgdisplaybuf, 1);
   }

   clear_bitmap(pricesdisplaybuf);
   set_clip_state(pricesdisplaybuf, 1);

   if (items.idCount(iPile))
      loadlvlpal(DMaps[currdmap].color);

   items.clear();
   guys.clear();
   Lwpns.clear();
   Ewpns.clear();
   chainlinks.clear();
   clearScriptHelperData();

   if (decorationsToo)
      decorations.clear();

   particles.clear();

   if (Link.getNayrusLoveShieldClk())
      Link.setNayrusLoveShieldClk(Link.getNayrusLoveShieldClk());

   Link.resetflags(false);
   Link.reset_hookshot();
   linkedmsgclk = 0;
   add_asparkle = 0;
   add_bsparkle = 0;
   add_df1asparkle = false;
   add_df1bsparkle = false;
   add_nl1asparkle = false;
   add_nl1bsparkle = false;
   add_nl2asparkle = false;
   add_nl2bsparkle = false;
   mblock2.clk = 0;
   dismissmsg();
   fadeclk = -1;
   introclk = intropos = 72;

   lensclk = 0;
   lensid = -1;
   drawguys = Udown = Ddown = Ldown = Rdown = Adown = Bdown = Sdown = true;

   if (watch && !cheat_superman)
      Link.setClock(false);

   watch = freeze_guys = loaded_guys = loaded_enemies = blockpath = false;
   stop_sfx(SFX_BRANG);

   for (int i = 0; i < 176; i++)
      guygrid[i] = 0;

   sle_clk = 0;
   blockmoving = false;
   fairy_cnt = 0;

   if (usebombpal)
   {
      memcpy(RAMpal, tempbombpal, PAL_SIZE * sizeof(RGB));
      zc_sync_pal = true;
      usebombpal = false;
   }
}

bool  GuySuperman(int j)
{
   if ((j >= guys.Count()) || (j < 0))
      return true;

   return ((enemy *)guys.spr(j))->superman != 0;
}

void StunGuy(int j, int stun)
{
   if (stun <= 0) return;

   if (((enemy *)guys.spr(j))->z == 0 && canfall(((enemy *)guys.spr(j))->id))
   {
      ((enemy *)guys.spr(j))->stunclk = zc_min(360, stun * 4);
      ((enemy *)guys.spr(j))->fall = -zc_min(FEATHERJUMP, (stun * 8) + rand() % 5);
   }
}

void add_grenade(int wx, int wy, int wz, int size, int parentid)
{
   if (size)
   {
      Lwpns.add(new weapon((fix)wx, (fix)wy, (fix)wz, wSBomb, 0, 16 * DAMAGE_MULTIPLIER, Link.getDir(),
                           -1, parentid));
      Lwpns.spr(Lwpns.Count() - 1)->id = wSBomb;
   }
   else
   {
      Lwpns.add(new weapon((fix)wx, (fix)wy, (fix)wz, wBomb, 0, 4 * DAMAGE_MULTIPLIER, Link.getDir(),
                           -1, parentid));
      Lwpns.spr(Lwpns.Count() - 1)->id = wBomb;
   }

   Lwpns.spr(Lwpns.Count() - 1)->clk = 48;
   Lwpns.spr(Lwpns.Count() - 1)->misc = 50;
}

fix distance(int x1, int y1, int x2, int y2)

{
   return (fix)sqrt(pow((double)abs(x1 - x2), 2) + pow((double)abs(y1 - y2), 2));
}

/**************************/
/***** Main Game Code *****/
/**************************/

void init_dmap()
{
   // readjust disabled items; could also do dmap-specific scripts here
   for (int i = 0; i < MAXITEMS; i++)
   {
      game->items_off[i] &= (~1); // disable last bit - this one is set by dmap
      game->items_off[i] |= DMaps[currdmap].disableditems[i]; // and reset if required
   }

   flushItemCache();

   return;
}

// NOTE: This method has been severely hacked to fix an annoying problem at game start:
// items (ie the Small Wallet) which modify max counter values need to be processed after
// the values for those counters specified in init data, as the author expects these items
// to modify the max counter. BUT the counter value should NOT be updated, ie, starting with
// the bomb item does not give 8 free bombs at quest start.
// I don't like this solution one bit, but can't come up with anything better -DD

void resetItems(gamedata *game2, zinitdata *zinit2, bool lvlitems)
{
   game2->set_maxlife(zinit2->hc * HP_PER_HEART);
   game2->set_maxbombs(zinit2->max_bombs);
   game2->set_maxcounter(zinit2->max_bombs / zc_max(1, zinit2->bomb_ratio), 6);
   game2->set_maxmagic(zinit2->max_magic);
   game2->set_maxarrows(zinit2->max_arrows);
   game2->set_maxcounter(zinit2->max_rupees, 1);
   game2->set_maxcounter(zinit2->max_keys, 5);

   //set up the items
   for (int i = 0; i < MAXITEMS; i++)
   {
      if (zinit2->items[i] && (itemsbuf[i].flags & ITEM_GAMEDATA))
      {
         if (!game2->get_item(i))
            getitem(i, true);
      }
      else
         game2->set_item_no_flush(i, false);

      game2->items_off[i] = 0;

      // Fix them DMap items
      // Since resetItems() gets called before AND after init_dmap()...
      if (get_currdmap() > -1)
         game2->items_off[i] |= DMaps[get_currdmap()].disableditems[i];
   }

   flushItemCache();

   //Then set up the counters
   game2->set_bombs(zinit2->bombs);

   if (zinit2->bombs > 0 && zinit2->max_bombs > 0) game2->set_item(iBombs, true);

   game2->set_keys(zinit2->keys);
   game2->set_sbombs(zinit2->super_bombs);

   if (zinit2->super_bombs > 0 && (zinit2->max_bombs / zc_max(1, zinit2->bomb_ratio)) > 0) game2->set_item(iSBomb, true);

   game2->set_HCpieces(zinit2->hcp);
   game2->set_rupies(zinit2->rupies);
   game2->set_hcp_per_hc(zinit2->hcp_per_hc);
   game2->set_cont_hearts(zinit2->cont_heart);
   game2->set_cont_percent(get_bit(zinit2->misc, idM_CONTPERCENT) != 0);

   for (int i = 0; i < MAXLEVELS; i++)
   {
      // Kludge to prevent two bits (liTRIFORCE and liBOSS) which aren't
      // completely stored in Init Data, from being erased when this is run in-game.
      if (lvlitems)
         game2->lvlitems[i] = 0;
      else
         game2->lvlitems[i] &= ~(liMAP | liCOMPASS | liBOSSKEY | (i > 0 && i <= 8 ? liTRIFORCE : 0));

      game2->lvlitems[i] |= get_bit(zinit2->map, i) ? liMAP : 0;
      game2->lvlitems[i] |= get_bit(zinit2->compass, i) ? liCOMPASS : 0;
      game2->lvlitems[i] |= get_bit(zinit2->boss_key, i) ? liBOSSKEY : 0;
      game2->lvlkeys[i] = zinit2->level_keys[i];
   }

   for (int i = 0; i < 8; i++)
      game2->lvlitems[i + 1] |= get_bit(&zinit2->triforce, i) ? liTRIFORCE : 0;

   game2->set_magic(zc_min(zinit2->magic, zinit2->max_magic));
   game2->set_magicdrainrate(get_bit(zinit2->misc, idM_DOUBLEMAGIC) ? 1 : 2);
   game2->set_canslash(get_bit(zinit2->misc, idM_CANSLASH) ? 1 : 0);

   game2->set_arrows(zinit2->arrows);

   //flush the cache again (in case bombs became illegal to use by setting bombs to 0)
   flushItemCache();
}

int init_game()
{
   srand(time(0));

   //Some initialising globals
   didpit = false;
   Link.unfreeze();
   Link.reset_hookshot();
   Link.reset_ladder();
   linkedmsgclk = 0;
   blockmoving = false;
   add_asparkle = 0;
   add_bsparkle = 0;
   add_df1asparkle = false;
   add_df1bsparkle = false;
   add_nl1asparkle = false;
   add_nl1bsparkle = false;
   add_nl2asparkle = false;
   add_nl2bsparkle = false;
   cheat_superman = false;
   wavy = quakeclk = 0;

   for (int x = 0; x < MAXITEMS; x++)
   {
      lens_hint_item[x][0] = 0;
      lens_hint_item[x][1] = 0;
   }

   for (int x = 0; x < MAXWPNS; x++)
   {
      lens_hint_weapon[x][0] = 0;
      lens_hint_weapon[x][1] = 0;
   }

   /* Point game to the current save game slot */
   game->Clear();
   game->Copy(saves[currgame]);
   flushItemCache();

   bool firstplay = (game->get_hasplayed() == 0);

   BSZ = get_bit(quest_rules, qr_BSZELDA) != 0;
   //setuplinktiles(zinit.linkanimationstyle);

   COOLSCROLL = (get_bit(quest_rules, qr_COOLSCROLL) != 0 ? 1 : 0) |
                (get_bit(quest_rules, qr_OVALWIPE) != 0 ? 2 : 0) |
                (get_bit(quest_rules, qr_TRIANGLEWIPE) != 0 ? 4 : 0) |
                (get_bit(quest_rules, qr_SMASWIPE) != 0 ? 8 : 0);
   identifyCFEnemies();

   if (firstplay)
   {
      game->set_continue_dmap(zinit.start_dmap);
      resetItems(game, &zinit, true);
   }

   currdmap = warpscr = worldscr = game->get_continue_dmap();
   init_dmap();

   if (game->get_continue_scrn() >= 0x80)
   {
      //if ((DMaps[currdmap].type&dmfTYPE)==dmOVERW || QHeader.zelda_version <= 0x190)
      if ((DMaps[currdmap].type & dmfTYPE) == dmOVERW)
         homescr = currscr = DMaps[currdmap].cont;
      else
         homescr = currscr = DMaps[currdmap].cont + DMaps[currdmap].xoff;
   }
   else
      homescr = currscr = game->get_continue_scrn();

   lastentrance = currscr;
   game->set_continue_scrn(lastentrance);
   lastentrance_dmap = currdmap;
   currmap = DMaps[currdmap].map;
   dlevel = DMaps[currdmap].level;
   sle_x = sle_y = newscr_clk = opendoors = Bwpn = Bpos = 0;
   fadeclk = -1;

   if (DMaps[currdmap].flags & dmfVIEWMAP)
   {
      game->maps[(currmap * MAPSCRSNORMAL) + currscr] |= mVISITED;          // mark as visited
   }

   for (int i = 0; i < 6; i++)
      visited[i] = -1;

   game->lvlitems[9] &= ~liBOSS;

   ALLOFF();
   whistleclk = -1;
   clockclk = 0;
   currcset = DMaps[currdmap].color;
   darkroom = naturaldark = false;

   tmpscr[0].zero_memory();
   tmpscr[1].zero_memory();

   //Script-related nonsense
   script_drawing_commands.Clear();

   initZScriptArrayRAM(firstplay);
   initZScriptGlobalRAM();
   global_wait = false;

   //loadscr(0,currscr,up);
   loadscr(0, currdmap, currscr, -1, false);
   putscr(scrollbuf, 0, 0, &tmpscr[0]);
   putscrdoors(scrollbuf, 0, 0, &tmpscr[0]);

   //preloaded freeform combos
   //ffscript_engine(true); Can't do this here! Global arrays haven't been allocated yet... ~Joe

   Link.init();
   Link.resetflags(true);
   Link.setEntryPoints(Link.getX(), Link.getY());

   loadfullpal();
   ringcolor(false);
   loadlvlpal(DMaps[currdmap].color);
   lighting(false, true);

   if (firstplay)
      game->set_life(zinit.start_heart * HP_PER_HEART);
   else
   {
      if (game->get_cont_percent())
      {
         if (game->get_maxlife() % HP_PER_HEART == 0)
            game->set_life(((game->get_maxlife()*game->get_cont_hearts() / 100) / HP_PER_HEART)*HP_PER_HEART);
         else
            game->set_life(game->get_maxlife()*game->get_cont_hearts() / 100);
      }
      else
         game->set_life(game->get_cont_hearts()*HP_PER_HEART);
   }

   if (firstplay)
      game->set_hasplayed(1);

   update_subscreens();

   load_Sitems(&QMisc);

   //load the previous weapons -DD
   bool usesaved = (game->get_quest() == 0xFF); //What was wrong with firstplay?
   int apos = 0;
   int bpos = 0;

   if (!get_bit(quest_rules, qr_SELECTAWPN))
   {
      Awpn = selectSword();
      apos = -1;
      bpos = selectWpn_new(SEL_VERIFY_RIGHT, usesaved ? game->bwpn : 0xFF, -1);
   }
   else
   {
      apos = selectWpn_new(SEL_VERIFY_RIGHT, usesaved ? game->awpn : 0xFF);
      bpos = selectWpn_new(SEL_VERIFY_RIGHT, usesaved ? game->bwpn : 0xFF, apos);

      if (bpos == 0xFF)
      {
         bpos = apos;
         apos = 0;
      }

      Awpn = Bweapon(apos);
      directItemA = directItem;
   }

   game->awpn = apos;
   game->bwpn = bpos;
   Bwpn = Bweapon(bpos);
   directItemB = directItem;
   update_subscr_items();

   reset_subscr_items();

   Link.setDontDraw(false);
   show_subscreen_dmap_dots = true;
   show_subscreen_items = true;
   show_subscreen_numbers = true;
   show_subscreen_life = true;

   playing = true;

   map_bkgsfx(true);
   openscreen();
   show_subscreen_numbers = true;
   show_subscreen_life = true;
   dointro();
   loadguys();

   activated_timed_warp = false;
   newscr_clk = frame;

   if (isdungeon() && currdmap > 0) // currdmap>0 is weird, but at least one quest (Mario's Insane Rampage) depends on it
      Link.stepforward(get_bit(quest_rules, qr_LTTPWALK) ? 11 : 12, false);

   if (!zc_state)
      playLevelMusic();

   if (firstplay)
   {
      memset(game->screen_d, 0, MAXDMAPS * 64 * 8 * sizeof(int32_t));
      ZScriptVersion::RunScript(SCRIPT_GLOBAL, GLOBAL_SCRIPT_INIT);
   }
   else
   {
      ZScriptVersion::RunScript(SCRIPT_GLOBAL, GLOBAL_SCRIPT_CONTINUE); //Do this after global arrays have been loaded
   }

   initZScriptGlobalRAM(); //Call again so we're set up for GLOBAL_SCRIPT_GAME
   ffscript_engine(true);  //Here is a much safer place...

   return 0;
}

int cont_game()
{
   //  introclk=intropos=msgclk=msgpos=dmapmsgclk=0;
   didpit = false;
   Link.unfreeze();
   Link.reset_hookshot();
   Link.reset_ladder();
   linkedmsgclk = 0;
   blockmoving = 0;
   add_asparkle = 0;
   add_bsparkle = 0;
   add_df1asparkle = false;
   add_df1bsparkle = false;
   add_nl1asparkle = false;
   add_nl1bsparkle = false;
   add_nl2asparkle = false;
   add_nl2bsparkle = false;
   /*
     if(DMaps[currdmap].cont >= 0x80)
     {
     homescr = currscr = DMaps[0].cont;
     currdmap = warpscr = worldscr=0;
     currmap = DMaps[0].map;
     dlevel = DMaps[0].level;
     }
     */
   currdmap = lastentrance_dmap;
   homescr = currscr = lastentrance;
   currmap = DMaps[currdmap].map;
   dlevel = DMaps[currdmap].level;
   init_dmap();

   for (int i = 0; i < 6; i++)
      visited[i] = -1;

   if (dlevel == 0)
      game->lvlitems[9] &= ~liBOSS;

   ALLOFF();
   whistleclk = -1;
   currcset = DMaps[currdmap].color;
   darkroom = naturaldark = false;
   tmpscr[0].zero_memory();
   tmpscr[1].zero_memory();

   //loadscr(0,currscr,up);
   loadscr(0, currdmap, currscr, -1, false);
   putscr(scrollbuf, 0, 0, &tmpscr[0]);
   putscrdoors(scrollbuf, 0, 0, &tmpscr[0]);
   script_drawing_commands.Clear();

   //preloaded freeform combos
   ffscript_engine(true);

   loadfullpal();
   ringcolor(false);
   loadlvlpal(DMaps[currdmap].color);
   lighting(false, true);
   Link.init();
   wavy = quakeclk = 0;

   //if(get_bit(zinit.misc,idM_CONTPERCENT))
   if (game->get_cont_percent())
   {
      if (game->get_maxlife() % HP_PER_HEART == 0)
         game->set_life(((game->get_maxlife()*game->get_cont_hearts() / 100) / HP_PER_HEART)*HP_PER_HEART);
      else
         game->set_life(game->get_maxlife()*game->get_cont_hearts() / 100);
   }
   else
      game->set_life(game->get_cont_hearts()*HP_PER_HEART);

   update_subscreens();
   playing = true;
   map_bkgsfx(true);
   openscreen();
   show_subscreen_numbers = true;
   show_subscreen_life = true;
   dointro();
   loadguys();

   if (!zc_state)
   {
      playLevelMusic();

      if (isdungeon())
         Link.stepforward(get_bit(quest_rules, qr_LTTPWALK) ? 11 : 12, false);

      newscr_clk = frame;
      activated_timed_warp = false;
   }

   initZScriptGlobalRAM();
   return 0;
}

void restart_level()
{
   blackscr(16, true);
   map_bkgsfx(false);

   if (dlevel)
   {
      currdmap = lastentrance_dmap;
      homescr = currscr = lastentrance;
      init_dmap();
   }
   else
   {
      if ((DMaps[currdmap].type & dmfTYPE) == dmOVERW)
         homescr = currscr = DMaps[currdmap].cont;
      else
         homescr = currscr = DMaps[currdmap].cont + DMaps[currdmap].xoff;
   }

   currmap = DMaps[currdmap].map;
   dlevel = DMaps[currdmap].level;

   for (int i = 0; i < 6; i++)
      visited[i] = -1;

   ALLOFF();
   whistleclk = -1;
   darkroom = naturaldark = false;
   tmpscr[0].zero_memory();
   tmpscr[1].zero_memory();

   loadscr(0, currdmap, currscr, -1, false);
   putscr(scrollbuf, 0, 0, &tmpscr[0]);
   putscrdoors(scrollbuf, 0, 0, &tmpscr[0]);
   //preloaded freeform combos
   ffscript_engine(true);

   loadfullpal();
   ringcolor(false);
   loadlvlpal(DMaps[currdmap].color);
   lighting(false, true);
   Link.init();
   currcset = DMaps[currdmap].color;
   openscreen();
   map_bkgsfx(true);
   Link.setEntryPoints(Link.getX(), Link.getY());
   show_subscreen_numbers = true;
   show_subscreen_life = true;
   loadguys();

   if (!zc_state)
   {
      playLevelMusic();

      if (isdungeon())
         Link.stepforward(get_bit(quest_rules, qr_LTTPWALK) ? 11 : 12, false);

      newscr_clk = frame;
      activated_timed_warp = false;
   }
}


void putintro()
{
   if (!strcmp("                                                                        ", DMaps[currdmap].intro))
   {
      introclk = intropos = 72;
      return;
   }

   if ((cBbtn()) && (get_bit(quest_rules, qr_ALLOWMSGBYPASS)))
   {
      //finish writing out the string
      for (; intropos < 72; ++intropos)
      {
         textprintf_ex(msgdisplaybuf, zfont, ((intropos % 24) << 3) + 32, ((intropos / 24) << 3) + 40, QMisc.colors.msgtext, -1,
                       "%c", DMaps[currdmap].intro[intropos]);
      }
   }

   if (intropos >= 72)
   {
      if (dmapmsgclk >= 51)
         dmapmsgclk = 50;

      return;
   }

   if (((introclk++) % 6 < 5) && ((!cAbtn()) || (!get_bit(quest_rules, qr_ALLOWFASTMSG))))
      return;

   dmapmsgclk = 51;

   if (intropos == 0)
   {
      while (DMaps[currdmap].intro[intropos] == ' ')
         ++intropos;
   }

   sfx(SFX_MSG);


   //using the clip value to indicate the bitmap is "dirty"
   //rather than add yet another global variable
   set_clip_state(msgdisplaybuf, 0);
   textprintf_ex(msgdisplaybuf, zfont, ((intropos % 24) << 3) + 32, ((intropos / 24) << 3) + 40, QMisc.colors.msgtext, -1,
                 "%c", DMaps[currdmap].intro[intropos]);

   ++intropos;

   if (DMaps[currdmap].intro[intropos] == ' ' && DMaps[currdmap].intro[intropos + 1] == ' ')
      while (DMaps[currdmap].intro[intropos] == ' ')
         ++intropos;
}

void do_magic_casting()
{
   static int tempx, tempy;
   static uint8_t linktilebuf[256];
   int ltile = 0;
   int lflip = 0;
   bool shieldModify = true;

   if (magicitem == -1)
      return;

   switch (itemsbuf[magicitem].family)
   {
      case itype_dinsfire:
      {
         if (magiccastclk == 0)
         {
            Lwpns.add(new weapon(Link.getX(), Link.getY(), Link.getZ(), wPhantom, pDINSFIREROCKET, 0, up, magicitem,
                                 Link.getUID()));
            weapon *w1 = (weapon *)(Lwpns.spr(Lwpns.Count() - 1));
            w1->step = 4;
            //          Link.tile=(BSZ)?32:29;
            linktile(&Link.tile, &Link.flip, &Link.extend, ls_landhold2, Link.getDir(), zinit.linkanimationstyle);

            if (get_bit(quest_rules, qr_EXPANDEDLTM))
               Link.tile += item_tile_mod(shieldModify);

            casty = Link.getY();
         }

         if (magiccastclk == 64)
         {
            Lwpns.add(new weapon((fix)Link.getX(), (fix)(-32), (fix)Link.getZ(), wPhantom, pDINSFIREROCKETRETURN, 0, down,
                                 magicitem, Link.getUID()));
            weapon *w1 = (weapon *)(Lwpns.spr(Lwpns.Count() - 1));
            w1->step = 4;
            //          Link.tile=29;
            linktile(&Link.tile, &Link.flip, &Link.extend, ls_landhold2, Link.getDir(), zinit.linkanimationstyle);

            if (get_bit(quest_rules, qr_EXPANDEDLTM))
               Link.tile += item_tile_mod(shieldModify);

            castnext = false;
         }

         if (castnext)
         {
            linktile(&Link.tile, &Link.flip, &Link.extend, ls_cast, Link.getDir(), zinit.linkanimationstyle);

            if (get_bit(quest_rules, qr_EXPANDEDLTM))
               Link.tile += item_tile_mod(shieldModify);

            if (get_bit(quest_rules, qr_MORESOUNDS))
               sfx(itemsbuf[magicitem].usesound, pan(int(Link.getX())));

            int flamemax = itemsbuf[magicitem].misc1;

            for (int flamecounter = ((-1) * (flamemax / 2)) + 1; flamecounter <= ((flamemax / 2) + 1); flamecounter++)
            {
               Lwpns.add(new weapon((fix)Link.getX(), (fix)Link.getY(), (fix)Link.getZ(), wFire, 3,
                                    itemsbuf[magicitem].power * DAMAGE_MULTIPLIER,
                                    (tmpscr->flags7 & fSIDEVIEW) ? (flamecounter < flamemax ? left : right) : 0, magicitem, Link.getUID()));
               weapon *w = (weapon *)(Lwpns.spr(Lwpns.Count() - 1));
               w->step = (itemsbuf[magicitem].misc2 / 100.0);
               w->angular = true;
               w->angle = (flamecounter * PI / (flamemax / 2.0));
            }

            castnext = false;
            magiccastclk = 128;
         }

         if ((magiccastclk++) >= 226)
         {
            magicitem = -1;
            magiccastclk = 0;
         }
      }
      break;

      case itype_faroreswind:
      {
         if (magiccastclk == 0)
         {
            linktile(&ltile, &lflip, ls_stab, down, zinit.linkanimationstyle);

            if (get_bit(quest_rules, qr_EXPANDEDLTM))
               ltile += item_tile_mod(shieldModify);

            unpack_tile(newtilebuf, ltile, lflip, true);
            memcpy(linktilebuf, unpackbuf, 256);
            tempx = Link.getX();
            tempy = Link.getY();
            linktile(&Link.tile, &Link.flip, &Link.extend, ls_pound, down, zinit.linkanimationstyle);

            if (get_bit(quest_rules, qr_EXPANDEDLTM))
               Link.tile += item_tile_mod(shieldModify);
         }

         if (magiccastclk >= 0 && magiccastclk < 64)
         {
            Link.setX(tempx + ((rand() % 3) - 1));
            Link.setY(tempy + ((rand() % 3) - 1));
         }

         if (magiccastclk == 64)
         {
            Link.setX(tempx);
            Link.setY(tempy);
            linktile(&Link.tile, &Link.flip, &Link.extend, ls_stab, down, zinit.linkanimationstyle);

            if (get_bit(quest_rules, qr_EXPANDEDLTM))
               Link.tile += item_tile_mod(shieldModify);
         }

         if (magiccastclk == 96)
         {
            if (get_bit(quest_rules, qr_MORESOUNDS))
               sfx(itemsbuf[magicitem].usesound, pan(int(Link.getX())));

            Link.setDontDraw(true);

            for (int i = 0; i < 16; ++i)
            {
               for (int j = 0; j < 16; ++j)
               {
                  if (linktilebuf[i * 16 + j])
                  {
                     if (itemsbuf[magicitem].misc1 == 1) // Twilight
                     {
                        particles.add(new pTwilight(Link.getX() + j, Link.getY() - Link.getZ() + i, 5, 0, 0, (rand() % 8) + i * 4));
                        int k = particles.Count() - 1;
                        particle *p = (particle *)(particles.spr(k));
                        p->step = 3;
                     }
                     else if (itemsbuf[magicitem].misc1 == 2) // Sands of Hours
                     {
                        particles.add(new pTwilight(Link.getX() + j, Link.getY() - Link.getZ() + i, 5, 1, 2, (rand() % 16) + i * 2));
                        int k = particles.Count() - 1;
                        particle *p = (particle *)(particles.spr(k));
                        p->step = 4;

                        if (rand() % 10 < 2)
                        {
                           p->color = 1;
                           p->cset = 0;
                        }
                     }
                     else
                     {
                        particles.add(new pFaroresWindDust(Link.getX() + j, Link.getY() - Link.getZ() + i, 5, 6, linktilebuf[i * 16 + j],
                                                           rand() % 96));

                        int k = particles.Count() - 1;
                        particle *p = (particle *)(particles.spr(k));
                        p->angular = true;
                        p->angle = rand();
                        p->step = (((double)j) / 8);
                        p->yofs = Link.getYOfs();
                     }
                  }
               }
            }
         }

         if ((magiccastclk++) >= 226)
         {
            //attackclk=0;
            int nayrutemp = nayruitem;
            restart_level();
            nayruitem = nayrutemp;
            //xofs=0;
            //action=none;
            magicitem = -1;
            magiccastclk = 0;
            Link.setDontDraw(false);
         }
      }
      break;

      case itype_nayruslove:
      {
         // See also Link.cpp, LinkClass::checkhit().
         if (magiccastclk == 0)
         {
            Lwpns.add(new weapon(Link.getX(), Link.getY(), (fix)0, wPhantom, pNAYRUSLOVEROCKET1, 0, left, magicitem,
                                 Link.getUID()));
            weapon *w1 = (weapon *)(Lwpns.spr(Lwpns.Count() - 1));
            w1->step = 4;
            Lwpns.add(new weapon(Link.getX(), Link.getY(), (fix)0, wPhantom, pNAYRUSLOVEROCKET2, 0, right, magicitem,
                                 Link.getUID()));
            w1 = (weapon *)(Lwpns.spr(Lwpns.Count() - 1));
            w1->step = 4;
            //          Link.tile=(BSZ)?32:29;
            linktile(&Link.tile, &Link.flip, &Link.extend, ls_cast, Link.getDir(), zinit.linkanimationstyle);

            if (get_bit(quest_rules, qr_EXPANDEDLTM))
               Link.tile += item_tile_mod(shieldModify);

            castx = Link.getX();
         }

         if (magiccastclk == 64)
         {
            int d = zc_max(Link.getX(), 256 - Link.getX()) + 32;
            Lwpns.add(new weapon((fix)(Link.getX() - d), (fix)Link.getY(), (fix)Link.getZ(), wPhantom, pNAYRUSLOVEROCKETRETURN1, 0,
                                 right, magicitem, Link.getUID()));
            weapon *w1 = (weapon *)(Lwpns.spr(Lwpns.Count() - 1));
            w1->step = 4;
            Lwpns.add(new weapon((fix)(Link.getX() + d), (fix)Link.getY(), (fix)Link.getZ(), wPhantom, pNAYRUSLOVEROCKETRETURN2, 0,
                                 left, magicitem, Link.getUID()));
            w1 = (weapon *)(Lwpns.spr(Lwpns.Count() - 1));
            w1->step = 4;
            //          Link.tile=29;
            linktile(&Link.tile, &Link.flip, &Link.extend, ls_cast, Link.getDir(), zinit.linkanimationstyle);

            if (get_bit(quest_rules, qr_EXPANDEDLTM))
               Link.tile += item_tile_mod(shieldModify);

            castnext = false;
         }

         if (castnext)
         {
            //          Link.tile=4;
            linktile(&Link.tile, &Link.flip, &Link.extend, ls_landhold2, Link.getDir(), zinit.linkanimationstyle);

            if (get_bit(quest_rules, qr_EXPANDEDLTM))
               Link.tile += item_tile_mod(shieldModify);

            Link.setNayrusLoveShieldClk(itemsbuf[magicitem].misc1);

            if (get_bit(quest_rules, qr_MORESOUNDS))
            {
               if (nayruitem != -1)
               {
                  stop_sfx(itemsbuf[nayruitem].usesound + 1);
                  stop_sfx(itemsbuf[nayruitem].usesound);
               }

               cont_sfx(itemsbuf[magicitem].usesound);
            }

            castnext = false;
            magiccastclk = 128;
            nayruitem = magicitem;
         }

         // Finish the final spell pose
         if ((magiccastclk++) >= 160)
         {
            magicitem = -1;
            magiccastclk = 0;
         }
      }
      break;

      default:
         magiccastclk = 0;
         break;
   }
}

void update_hookshot()
{
   int hs_x, hs_y, hs_z, hs_dx, hs_dy;
   bool check_hs = false;
   int dist_bx, dist_by, hs_w;
   chainlinks.animate();
   //  char tempbuf[80];
   //  char tempbuf2[80];

   //find out where the head is and make it
   //easy to reference
   if (Lwpns.idFirst(wHookshot) > -1)
      check_hs = true;

   if (check_hs)
   {
      int parentitem = ((weapon *)Lwpns.spr(Lwpns.idFirst(wHookshot)))->parentitem;
      hs_x = Lwpns.spr(Lwpns.idFirst(wHookshot))->x;
      hs_y = Lwpns.spr(Lwpns.idFirst(wHookshot))->y;
      hs_z = Lwpns.spr(Lwpns.idFirst(wHookshot))->z;
      hs_dx = hs_x - hs_startx;
      hs_dy = hs_y - hs_starty;

      //extending
      if (((weapon *)Lwpns.spr(Lwpns.idFirst(wHookshot)))->misc == 0)
      {
         int maxchainlinks = itemsbuf[parentitem].misc2;

         if (chainlinks.Count() < maxchainlinks)        //extending chain
         {
            if (abs(hs_dx) >= hs_xdist + 8)
            {
               hs_xdist = abs(hs_x - hs_startx);
               chainlinks.add(new weapon((fix)hs_x, (fix)hs_y, (fix)hs_z, wHSChain, 0, 0, Link.getDir(), parentitem, Link.getUID()));
            }
            else if (abs(hs_dy) >= hs_ydist + 8)
            {
               hs_ydist = abs(hs_y - hs_starty);
               chainlinks.add(new weapon((fix)hs_x, (fix)hs_y, (fix)hs_z, wHSChain, 0, 0, Link.getDir(), parentitem, Link.getUID()));
            }
         }                                                     //stretching chain
         else
         {
            dist_bx = (abs(hs_dx) - (8 * chainlinks.Count())) / (chainlinks.Count() + 1);
            dist_by = (abs(hs_dy) - (8 * chainlinks.Count())) / (chainlinks.Count() + 1);
            hs_w = 8;

            if (hs_dx < 0)
            {
               dist_bx = 0 - dist_bx;
               hs_w = -8;
            }

            if (hs_dy < 0)
            {
               dist_by = 0 - dist_by;
               hs_w = -8;
            }

            for (int counter = 0; counter < chainlinks.Count(); counter++)
            {
               if (Link.getDir() > down)                         //chain is moving horizontally
                  chainlinks.spr(counter)->x = hs_startx + hs_w + dist_bx + (counter * (hs_w + dist_bx));
               else
                  chainlinks.spr(counter)->y = hs_starty + hs_w + dist_by + (counter * (hs_w + dist_by));
            }
         }
      }                                                       //retracting
      else if (((weapon *)Lwpns.spr(Lwpns.idFirst(wHookshot)))->misc == 1)
      {
         dist_bx = (abs(hs_dx) - (8 * chainlinks.Count())) / (chainlinks.Count() + 1);
         dist_by = (abs(hs_dy) - (8 * chainlinks.Count())) / (chainlinks.Count() + 1);
         hs_w = 8;

         if (hs_dx < 0)
         {
            dist_bx = 0 - dist_bx;
            hs_w = -8;
         }

         if (hs_dy < 0)
         {
            dist_by = 0 - dist_by;
            hs_w = -8;
         }

         /* With ZScript modification, chains can conceivably move diagonally.*/
         //if (Link.getDir()>down)                               //chain is moving horizontally
         {
            if (abs(hs_dx) - (8 * chainlinks.Count()) > 0)      //chain is stretched
            {
               for (int counter = 0; counter < chainlinks.Count(); counter++)
                  chainlinks.spr(counter)->x = hs_startx + hs_w + dist_bx + (counter * (hs_w + dist_bx));
            }
            else
            {
               if (abs(hs_x - hs_startx) <= hs_xdist - 8)
               {
                  hs_xdist = abs(hs_x - hs_startx);

                  if (pull_link == false)
                     chainlinks.del(chainlinks.idLast(wHSChain));
                  else
                     chainlinks.del(chainlinks.idFirst(wHSChain));
               }
            }
         }                                                     //chain is moving vertically
         //else
         {
            if (abs(hs_dy) - (8 * chainlinks.Count()) > 0)      //chain is stretched
            {
               for (int counter = 0; counter < chainlinks.Count(); counter++)
                  chainlinks.spr(counter)->y = hs_starty + hs_w + dist_by + (counter * (hs_w + dist_by));
            }
            else
            {
               if (abs(hs_y - hs_starty) <= hs_ydist - 8)
               {
                  hs_ydist = abs(hs_y - hs_starty);

                  if (pull_link == false)
                     chainlinks.del(chainlinks.idLast(wHSChain));
                  else
                     chainlinks.del(chainlinks.idFirst(wHSChain));
               }
            }
         }
      }
   }
}

void do_dcounters()
{
   static bool sfxon = false;

   for (int i = 0; i < 32; i++)
   {
      if (game->get_dcounter(i) == 0)
      {
         sfxon = false;
         continue;
      }

      if (frame & 1)
      {
         sfxon = true;

         if (game->get_dcounter(i) > 0)
         {
            int drain = (i == 4 ? MAGICPERBLOCK / 4 : 1);
            drain = zc_min(game->get_dcounter(i), drain);

            if (game->get_counter(i) < game->get_maxcounter(i))
            {
               game->change_counter(drain, i);
               game->change_dcounter(-drain, i);

               if (game->get_dcounter(i) < 0) // Better safe
                  game->set_dcounter(0, i);
            }
            else
            {
               game->set_dcounter(0, i);
               game->set_counter(game->get_maxcounter(i), i);
            }
         }
         else
         {
            if (i != 1) // Only rupee drain is sounded
               sfxon = false;

            int drain = (i == 4 ? 2 * game->get_magicdrainrate() : 1);

            if (game->get_counter(i) > 0)
            {
               game->change_counter(-drain, i);
               game->change_dcounter(drain, i);

               if (game->get_dcounter(i) > 0) // Better safe
                  game->set_dcounter(0, i);
            }
            else
            {
               game->set_dcounter(0, i);
               game->set_counter(0, i);
            }
         }
      }

      if ((sfxon || i == 1) && !lensclk && (i < 2 || i == 4)) // Life, Rupees and Magic
         sfx(SFX_MSG);
   }
}

void game_loop()
{

   if (fadeclk >= 0)
   {
      if (fadeclk == 0 && currscr < 128)
         blockpath = false;

      --fadeclk;
   }

   // Three kinds of freezes: freeze, freezemsg, freezeff

   // freezemsg if message is being printed && qr_MSGFREEZE is on,
   // or if a message is being prepared && qr_MSGDISAPPEAR is on.
   bool freezemsg = ((msg_active || (intropos && intropos < 72) || (linkedmsgclk && get_bit(quest_rules, qr_MSGDISAPPEAR)))
                     && get_bit(quest_rules, qr_MSGFREEZE));

   // Messages also freeze FF combos.
   bool freezeff = freezemsg;

   bool freeze = false;

   for (int i = 0; i < 32; i++)
   {
      if (combobuf[tmpscr->ffdata[i]].type == cSCREENFREEZE) freeze = true;

      if (combobuf[tmpscr->ffdata[i]].type == cSCREENFREEZEFF) freezeff = true;
   }

   for (int i = 0; i < 176; i++)
   {
      if (combobuf[tmpscr->data[i]].type == cSCREENFREEZE) freeze = true;

      if (combobuf[tmpscr->data[i]].type == cSCREENFREEZEFF) freezeff = true;

      if (guygrid[i] > 0)
         --guygrid[i];
   }

   animate_combos();
   load_control_state();

   if (!freezemsg)
      script_drawing_commands.Clear();

   if (!freezeff)
      update_freeform_combos();

   // Arbitrary Rule 637: neither 'freeze' nor 'freezeff' freeze the global script.
   if (!freezemsg && g_doscript)
      ZScriptVersion::RunScript(SCRIPT_GLOBAL, GLOBAL_SCRIPT_GAME);

   if (!freeze && !freezemsg)
   {
      mblock2.animate(0);
      items.animate();
      items.check_conveyor();
      guys.animate();
      roaming_item();
      dragging_item();
      Ewpns.animate();
      checklink = true;

      for (int i = 0; i < 1; i++)
      {
         if (Link.animate(0))
         {
            if (!zc_state)
               zc_state = ZC_GAMEOVER;

            return;
         }

         checklink = false;
      }

      do_magic_casting();
      Lwpns.animate();
      decorations.animate();
      particles.animate();
      update_hookshot();

      if (conveyclk <= 0)
         conveyclk = 3;

      --conveyclk;
      check_collisions();
      dryuplake();
      cycle_palette();
   }
   else if (freezemsg)
   {
      for (int i = 0; i < guys.Count(); i++)
         if (((enemy *)guys.spr(i))->ignore_msg_freeze())
            guys.spr(i)->animate(i);
   }

   if (global_wait)
   {
      ZScriptVersion::RunScript(SCRIPT_GLOBAL, GLOBAL_SCRIPT_GAME);
      global_wait = false;
   }

   draw_screen(tmpscr);

   if (linkedmsgclk == 1)
   {
      if (wpnsbuf[iwMore].tile != 0)
         putweapon(framebuf, zinit.msg_more_x, message_more_y(), wPhantom, 4, up, lens_hint_weapon[wPhantom][0],
                   lens_hint_weapon[wPhantom][1], -1);
   }

   if (!freeze)
      putintro();

   if (dmapmsgclk > 0)
   {
      Link.Freeze();

      if (dmapmsgclk <= 50)
         --dmapmsgclk;
   }

   if (dmapmsgclk == 1)
   {
      Link.finishedmsg();
      dmapmsgclk = 0;
      introclk = 72;
      clear_bitmap(msgdisplaybuf);
      set_clip_state(msgdisplaybuf, 1);
      //    clear_bitmap(pricesdisplaybuf);
   }

   if (!freeze)
   {
      if (introclk == 0 || (introclk >= 72 && dmapmsgclk == 0))
      {
         putmsg();

         if (msgstr)
         {
            set_clip_state(msgdisplaybuf, 0);
            blit(msgbmpbuf, msgdisplaybuf, 0, 0, msg_xpos, msg_ypos, msg_w + 16, msg_h + 16);
         }
      }

      do_dcounters();

      if (!freezemsg && current_item(itype_heartring))
      {
         int itemid = current_item_id(itype_heartring);
         int fskip = itemsbuf[itemid].misc2;

         if (fskip == 0 || frame % fskip == 0)
            game->set_life(zc_min(game->get_life() + itemsbuf[itemid].misc1, game->get_maxlife()));
      }

      if (!freezemsg && current_item(itype_magicring))
      {
         int itemid = current_item_id(itype_magicring);
         int fskip = itemsbuf[itemid].misc2;

         if (fskip == 0 || frame % fskip == 0)
            game->set_magic(zc_min(game->get_magic() + itemsbuf[itemid].misc1, game->get_maxmagic()));
      }

      if (!freezemsg && current_item(itype_wallet))
      {
         int itemid = current_item_id(itype_wallet);
         int fskip = itemsbuf[itemid].misc2;

         if (fskip == 0 || frame % fskip == 0)
            game->set_rupies(zc_min(game->get_rupies() + itemsbuf[itemid].misc1, game->get_maxcounter(1)));
      }

      if (!freezemsg && current_item(itype_bombbag))
      {
         int itemid = current_item_id(itype_bombbag);

         if (itemsbuf[itemid].misc1)
         {
            int fskip = itemsbuf[itemid].misc2;

            if (fskip == 0 || frame % fskip == 0)
               game->set_bombs(zc_min(game->get_bombs() + itemsbuf[itemid].misc1, game->get_maxbombs()));

            if (itemsbuf[itemid].flags & ITEM_FLAG1)
            {
               int ratio = zinit.bomb_ratio;

               fskip = itemsbuf[itemid].misc2 * ratio;

               if (fskip == 0 || frame % fskip == 0)
                  game->set_sbombs(zc_min(game->get_sbombs() + zc_max(itemsbuf[itemid].misc1 / ratio, 1), game->get_maxbombs() / ratio));
            }
         }
      }

      if (!freezemsg && current_item(itype_quiver) && game->get_arrows() != game->get_maxarrows())
      {
         int itemid = current_item_id(itype_quiver);
         int fskip = itemsbuf[itemid].misc2;

         if (fskip == 0 || frame % fskip == 0)
            game->set_arrows(zc_min(game->get_arrows() + itemsbuf[itemid].misc1, game->get_maxarrows()));
      }

      if (lensclk)
      {
         draw_lens_over();
         --lensclk;
      }
   }
}

int get_currdmap()
{
   return currdmap;
}

int get_dlevel()
{
   return dlevel;
}

int get_currscr()
{
   return currscr;
}

int get_homescr()
{
   return homescr;
}

int get_bmaps(int si)
{
   return game->bmaps[si];
}

bool no_subscreen()
{
   return (tmpscr->flags3 & fNOSUBSCR) != 0;
}

#define FREE_BITMAP(a)  if (a) {              \
                           destroy_bitmap(a); \
                           a = NULL;          \
                        }

void free_bitmap_buffers(void)
{
   FREE_BITMAP(framebuf)
   FREE_BITMAP(scrollbuf)
   FREE_BITMAP(tempbuf)
   FREE_BITMAP(msgdisplaybuf)
   FREE_BITMAP(pricesdisplaybuf)
   FREE_BITMAP(msgbmpbuf)
   FREE_BITMAP(prim_bmp)
   FREE_BITMAP(lens_scr)
}

#undef FREE_BITMAP

int alloc_bitmap_buffers(void)
{
   bool success = true;

   if(!(framebuf = create_bitmap(256, 224)))
      RETURN_ERROR;

   if(!(scrollbuf = create_bitmap(512, 406)))
      RETURN_ERROR;

   if(!(tempbuf = create_bitmap(320, 224)))
      RETURN_ERROR;

   if(!(prim_bmp = create_bitmap(512, 512)))
      RETURN_ERROR;

   if(!(msgdisplaybuf = create_bitmap(256, 176)))
      RETURN_ERROR;

   if(!(msgbmpbuf = create_bitmap(512 + 16, 512 + 16)))
      RETURN_ERROR;

   if(!(pricesdisplaybuf = create_bitmap(256, 176)))
      RETURN_ERROR;

   clear_bitmap(scrollbuf);
   clear_bitmap(framebuf);
   clear_bitmap(msgdisplaybuf);
   set_clip_state(msgdisplaybuf, 1);
   clear_bitmap(pricesdisplaybuf);
   set_clip_state(pricesdisplaybuf, 1);
   
error:
   if (!success)
      zc_error("Error allocating bitmap buffers memory.");

   return success;
}

bool zc_init(const char *qpath)
{
   bool success = true;
   int res;
   char temp[MAX_STRLEN];

   zc_message("Zelda Classic %s (Build %d)\n", VerStr(ZELDA_VERSION), VERSION_BUILD);
   zc_message("Armageddon Games web site: http://www.armageddongames.com");
   zc_message("Zelda Classic web site: http://www.zeldaclassic.com");

   // allocate quest data buffers
   if (!alloc_qst_buffers())
      RETURN_ERROR;

   if (!alloc_bitmap_buffers())
      RETURN_ERROR;

   sprintf(temp, "%s%c" ZC_SYS_DIR "%c" SYSTEM_FILE, system_path, OTHER_PATH_SEPARATOR, OTHER_PATH_SEPARATOR);

   if ((data = load_datafile(temp)) == NULL)
      RETURN_ERROR_M("Error loading " SYSTEM_FILE " system datafile.");

   sprintf(temp, "zcdata.dat v%s Build %d", VerStr(ZCDAT_VERSION), ZCDAT_BUILD);

   if (strncmp((char *)data[_SIGNATURE].dat, temp, 25))
      RETURN_ERROR_M("Not a valid " SYSTEM_FILE " file.");

   sfxdata =  (DATAFILE *)data[SFX].dat;
   fontsdata = (DATAFILE *)data[FON].dat;
   mididata = (DATAFILE *)data[MID].dat;

   deffont = (FONT *)fontsdata[FON_ALLEGRO].dat;
   nfont = (FONT *)fontsdata[FON_GUI_PROP].dat;
   pfont = (FONT *)fontsdata[FON_8xPROP_THIN].dat;
   lfont = (FONT *)fontsdata[FON_LARGEPROP].dat;
   lfont_l = (FONT *)fontsdata[FON_LARGEPROP_L].dat;
   zfont = (FONT *)fontsdata[FON_NES].dat;
   z3font = (FONT *)fontsdata[FON_Z3].dat;
   z3smallfont = (FONT *)fontsdata[FON_Z3SMALL].dat;
   ztfont = (FONT *)fontsdata[FON_ZTIME].dat;
   sfont = (FONT *)fontsdata[FON_6x6].dat;
   sfont2 = (FONT *)fontsdata[FON_6x4].dat;
   sfont3 = (FONT *)fontsdata[FON_12x8].dat;
   spfont = (FONT *)fontsdata[FON_6xPROP].dat;
   ssfont1 = (FONT *)fontsdata[FON_SUBSCREEN1].dat;
   ssfont2 = (FONT *)fontsdata[FON_SUBSCREEN2].dat;
   ssfont3 = (FONT *)fontsdata[FON_SUBSCREEN3].dat;
   ssfont4 = (FONT *)fontsdata[FON_SUBSCREEN4].dat;
   gblafont = (FONT *)fontsdata[FON_GB_LA].dat;
   goronfont = (FONT *)fontsdata[FON_GORON].dat;
   zoranfont = (FONT *)fontsdata[FON_ZORAN].dat;
   hylian1font = (FONT *)fontsdata[FON_HYLIAN1].dat;
   hylian2font = (FONT *)fontsdata[FON_HYLIAN2].dat;
   hylian3font = (FONT *)fontsdata[FON_HYLIAN3].dat;
   hylian4font = (FONT *)fontsdata[FON_HYLIAN4].dat;
   gboraclefont = (FONT *)fontsdata[FON_GB_ORACLE].dat;
   gboraclepfont = (FONT *)fontsdata[FON_GB_ORACLE_P].dat;
   dsphantomfont = (FONT *)fontsdata[FON_DS_PHANTOM].dat;
   dsphantompfont = (FONT *)fontsdata[FON_DS_PHANTOM_P].dat;

   for (int i = 0; i < 4; i++)
   {
      for (int j = 0; j < MAXSUBSCREENITEMS; j++)
         memset(&custom_subscreen[i].objects[j], 0, sizeof(subscreen_object));
   }

   for (int i = 0; i < SFX_COUNT; i++)
      customsfxdata[i].data = NULL;

   for (int i = 0; i < ITEMCNT; i++)
      item_string[i] = new char[64];

   for (int i = 0; i < eMAXGUYS; i++)
      guy_string[i] = new char[64];

   // Script initialization
   init_scripts();
   zscriptDrawingRenderTarget = new ZScriptDrawingRenderTarget();

   /* Load the qst file and confirm it is valid. */
   packfile_password(DATA_PASSWORD);
   if ((res = loadquest(qpath, &QHeader, &QMisc, tunes + MID_COUNT)))
   {
      zc_error("Error loading quest. %s.", qst_error[res]);
      RETURN_ERROR;
   }
   packfile_password(NULL);

   if (!zc_initsound())
      RETURN_ERROR;

   /* keep the qst path to use it during the gameplay */
   strcpy(qst_path, qpath);

   /* load saved games */
   if (load_savedgames() != 0)
      RETURN_ERROR_M("Error loading saved games.");

   zc_palette = RAMpal;
   rgb_map = &rgb_table;

   /* set up initial game save slot (for the list_saves function) */
   game = new gamedata;
   game->Clear();

error:
   if (!success)
      zc_deinit();

   return success;
}

void zc_gameloop(void *)
{
   slock_lock(mutex);
   
   /* wait the main thread's
    * signal to continue. */
   scond_wait(cond, mutex);

   while (zc_state != ZC_EXIT)
   {
      titlescreen();

      setup_combo_animations();
      setup_combo_animations2();

      while (!zc_state)
      {
         game_loop();
         advanceframe(true);
      }

      tmpscr->flags3 = 0;
      playing = false;

      switch (zc_state)
      {
         case ZC_QUIT:
         case ZC_GAMEOVER:
         {
            Link.setDontDraw(false);
            show_subscreen_dmap_dots = true;
            show_subscreen_numbers = true;
            show_subscreen_items = true;
            show_subscreen_life = true;
            introclk = intropos = 0;

            initZScriptGlobalRAM();
            ZScriptVersion::RunScript(SCRIPT_GLOBAL, GLOBAL_SCRIPT_END);

            if (!skipcont && !get_bit(quest_rules, qr_NOCONTINUE)) game_over(get_bit(quest_rules, qr_NOSAVE));

            skipcont = 0;
            reset_combo_animations();
            reset_combo_animations2();
         }
         break;

         case ZC_WON:
         {
            Link.setDontDraw(false);
            show_subscreen_dmap_dots = true;
            show_subscreen_numbers = true;
            show_subscreen_items = true;
            show_subscreen_life = true;

            initZScriptGlobalRAM();
            ZScriptVersion::RunScript(SCRIPT_GLOBAL, GLOBAL_SCRIPT_END);
            ending();
         }
         break;

      }

      kill_sfx();
      music_stop();
   }
}

void zc_deinit(void)
{
   zc_deinitsound();

   save_savedgames();
   if (saves) delete [] saves;
   if (game) delete game;

   script_drawing_commands.Dispose(); //for bitmaps

   delete_combo_aliases();
   reset_subscr_items();
   delete_selectors();
   Sitems.clear();

   if (data) unload_datafile(data);

   free_bitmap_buffers();

   for (int i = 0; i < 4; i++)
   {
      for (int j = 0; j < MAXSUBSCREENITEMS; j++)
      {
         switch (custom_subscreen[i].objects[j].type)
         {
            case ssoTEXT:
            case ssoTEXTBOX:
            case ssoCURRENTITEMTEXT:
            case ssoCURRENTITEMCLASSTEXT:
               if (custom_subscreen[i].objects[j].dp1 != NULL) delete[](char *)custom_subscreen[i].objects[j].dp1;
         }
      }
   }

   for (int i = 0; i < SFX_COUNT; i++)
   {
      if (customsfxdata[i].data != NULL)
         free(customsfxdata[i].data);
   }

   for (int i = 0; i < ITEMCNT; i++)
      if (item_string[i]) delete [] item_string[i];

   for (int i = 0; i < eMAXGUYS; i++)
      if (guy_string[i]) delete [] guy_string[i];

   delete_scripts();

   if (zscriptDrawingRenderTarget)
   {
      delete zscriptDrawingRenderTarget;
      zscriptDrawingRenderTarget = NULL;
   }

   free_qst_buffers();
}
