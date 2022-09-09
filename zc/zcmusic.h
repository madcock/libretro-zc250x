#ifndef _ZCMUSIC_H_
#define _ZCMUSIC_H_

bool zcmusic_init(float delta);
bool zcmusic_poll(void);
void zcmusic_exit(void);

typedef struct ZCMUSIC
{
   void *stream;                                           // actual pointer to the stream data
   int type;                                               // uses STREAM_TYPE enums
   int position;                                           // Only needed to sync Triforce jingle
   char filename[256];
   int track;
} ZCMUSIC;

ZCMUSIC *zcmusic_load_file(char *filename);
bool zcmusic_play(ZCMUSIC *zcm);
void zcmusic_pause(int pause);
void zcmusic_stop(void);
void zcmusic_unload_file(ZCMUSIC *zcm);
int zcmusic_get_tracks(ZCMUSIC *zcm);
int zcmusic_change_track(int tracknum);

#endif
