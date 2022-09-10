#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "zdefs.h"
#include <set>
#include <map>

using std::map;
// this code needs some patching for use in zquest.cc

extern itemdata *itemsbuf;
extern wpndata  *wpnsbuf;
extern bool     freeze_guys;
extern int fadeclk;
extern int frame;
extern bool BSZ;
extern int conveyclk;

/**********************************/
/******* Sprite Base Class ********/
/**********************************/

class sprite
{
private:
   static long getNextUID();
   //unique sprite ID
   //given upon construction
   long uid;

public:
   long getUID()
   {
      return uid;
   }
   fix x, y, z, fall;
   int tile, shadowtile, cs, flip, c_clk, clk, misc;

   fix xofs, yofs, zofs;
   // no hzofs - it's always equal to zofs.
   int hxofs, hyofs, hxsz, hysz, hzsz;
   int txsz, tysz;
   /*
    * Explanation of hzsz:
    * Hzsz is how "tall" a sprite is.
    * To jump over a sprite, you must be higher than its z+hzsz.
    * Enemies have hzsz of 0 so that Link can jump over them (see LA.)
    */
   int id, dir;
   bool angular, canfreeze;
   double angle;
   int lasthit, lasthitclk;
   int dummy_int[10];
   fix dummy_fix[10];
   float dummy_float[10];
   bool dummy_bool[10];
   int drawstyle;                                          //0=normal, 1=translucent, 2=cloaked
   int extend;
   // Scripting additions
   int32_t miscellaneous[16];
   uint8_t scriptcoldet;
   //long stack[256];
   //Are you kidding? Really? 256 * sizeof(long) = 2048 bytes = 2kb of wasted memory for every sprite, and it'll never
   //even get used because item scripts only run for one frame. Gah! Maybe when we have npc scripts, not not now...

   //refInfo scriptData; //For when we have npc scripts maybe
   /*long d[8];
   long a[2];
   uint8_t ffcref;
   uint32_t itemref;
   uint32_t guyref;
   uint32_t lwpnref;
   uint32_t ewpnref;
   uint8_t sp;
   uint16_t pc;
   uint32_t scriptflag;
   uint16_t doscript;
   uint8_t itemclass;*/
   //uint8_t guyclass; //Not implemented
   //uint8_t lwpnclass;
   //uint8_t ewpnclass;


   sprite();
   sprite(sprite const &other);
   sprite(fix X, fix Y, int T, int CS, int F, int Clk, int Yofs);
   virtual ~sprite();
   virtual void draw(BITMAP *dest);                        // main layer
   virtual void draw8(BITMAP *dest);                       // main layer
   virtual void drawcloaked(BITMAP *dest);                 // main layer
   virtual void drawshadow(BITMAP *dest, bool translucent);// main layer
   virtual void draw2(BITMAP *dest);                       // top layer for special needs
   virtual void drawcloaked2(BITMAP *dest);                // top layer for special needs
   virtual bool animate(int index);
   virtual void check_conveyor();
   int real_x(fix fx);
   int real_y(fix fy);
   int real_ground_y(fix fy);
   int real_z(fix fz);
   virtual bool hit(sprite *s);
   virtual bool hit(int tx, int ty, int tz, int txsz, int tysz, int tzsz);


   virtual int hitdir(int tx, int ty, int txsz, int tysz, int dir);
   virtual void move(fix dx, fix dy);
   virtual void move(fix s);
};

/***************************************************************************/

/**********************************/
/********** Sprite List ***********/
/**********************************/

#define SLMAX 255

class sprite_list
{
   sprite *sprites[SLMAX];
   int count;
   map<long, int> containedUIDs;

public:
   sprite_list();

   sprite *getByUID(long uid);
   void clear();
   sprite *spr(int index);
   bool swap(int a, int b);
   bool add(sprite *s);
   // removes pointer from list but doesn't delete it
   bool remove(sprite *s);
   fix getX(int j);
   fix getY(int j);
   int getID(int j);
   int getMisc(int j);
   bool del(int j);
   void draw(BITMAP *dest, bool lowfirst);
   void drawshadow(BITMAP *dest, bool translucent, bool lowfirst);
   void draw2(BITMAP *dest, bool lowfirst);
   void drawcloaked2(BITMAP *dest, bool lowfirst);
   void animate();
   void check_conveyor();
   int Count();
   int hit(sprite *s);
   int hit(int x, int y, int z, int xsize, int ysize, int zsize);
   // returns the number of sprites with matching id
   int idCount(int id, int mask);
   // returns index of first sprite with matching id, -1 if none found
   int idFirst(int id, int mask);
   // returns index of last sprite with matching id, -1 if none found
   int idLast(int id, int mask);
   // returns the number of sprites with matching id
   int idCount(int id);
   // returns index of first sprite with matching id, -1 if none found
   int idFirst(int id);
   // returns index of last sprite with matching id, -1 if none found
   int idLast(int id);
};

/**********************************/
/********** Moving Block **********/
/**********************************/

class movingblock : public sprite
{
public:
   int bcombo;
   int oldflag;
   int oldcset;
   int endx, endy;
   bool trigger;
   uint8_t undercset;

   movingblock();
   void push(fix bx, fix by, int d, int f);
   virtual bool animate(int index);
   virtual void draw(BITMAP *dest);
};
#endif
