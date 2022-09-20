#include <string.h>
#include "zcsys.h"
#include "zcmusic.h"

static ZCMUSIC *cur_zcm;

bool zcmusic_init(float delta)
{
   return stream_init(delta);
}

bool zcmusic_poll(void)
{
   if (!cur_zcm)
      return false;

   if (stream_isplaying())
   {
      cur_zcm->position++;
      stream_fill_buffer();
   }

   return true;
}

void zcmusic_exit(void)
{
   if (!cur_zcm)
      return;

   zcmusic_unload_file(cur_zcm);
   cur_zcm = NULL;

   stream_deinit();
}

ZCMUSIC *zcmusic_load_file(char *filename)
{
   if (filename == NULL)
      return NULL;

   if (strlen(filename) > 255)
   {
      zc_message("Music file '%s' not loaded: filename too long\n", filename);
      return NULL;
   }

   char *ext = get_extension(filename);

   if (strcmp(ext, "ogg") == 0 || strcmp(ext, "OGG") == 0)
   {
      VORBIS *p = vorbis_load(filename);
      if (!p)
      {
         zc_message("OGG file '%s' not found.\n", filename);
         goto error;
      }

      ZCMUSIC *music = (ZCMUSIC *)malloc(sizeof(ZCMUSIC));
      music->type = STREAM_VORBIS;
      music->stream = p;
      strncpy(music->filename, get_filename(filename), sizeof(music->filename) - 1);
      music->track = 0;

      return music;
   }
   else if (strcmp(ext, "mp3") == 0 || strcmp(ext, "MP3") == 0)
   {
      MP3 *p = mp3_load(filename);

      if (!p)
      {
         zc_message("MP3 file '%s' not found.\n", filename);
         goto error;
      }

      ZCMUSIC *music = (ZCMUSIC *)malloc(sizeof(ZCMUSIC));
      music->type = STREAM_MP3;
      music->stream = p;
      strncpy(music->filename, get_filename(filename), sizeof(music->filename) - 1);
      music->track = 0;
      return music;
   }
   else if ((strcmp(ext, "spc") == 0) || (strcmp(ext, "SPC") == 0) ||
            (strcmp(ext, "gbs") == 0) || (strcmp(ext, "GBS") == 0) ||
            (strcmp(ext, "nsf") == 0) || (strcmp(ext, "NSF") == 0))
   {

      GME *p = gme_load(filename);
      if (!p)
      {
         zc_message("GME file '%s' not found.\n", filename);
         goto error;
      }

      ZCMUSIC *music = (ZCMUSIC *)malloc(sizeof(ZCMUSIC));
      music->type = STREAM_GME;
      music->stream = p;
      strncpy(music->filename, get_filename(filename), sizeof(music->filename) - 1);
      music->track = 0;

      return music;
   }

error:
   return NULL;
}

bool zcmusic_play(ZCMUSIC *zcm)
{

   if (zcm == NULL) return FALSE;

   int ret = TRUE;

   switch (zcm->type)
   {
      case STREAM_VORBIS:
         ret = stream_play_vorbis((VORBIS *)zcm->stream, TRUE);

         break;

      case STREAM_MP3:
         ret = stream_play_mp3((MP3 *)zcm->stream, TRUE);

         break;

      case STREAM_GME:
         ret = stream_play_gme((GME *)zcm->stream);
   }

   if (ret)
   {
      zcm->position = 0;
      cur_zcm = zcm;
   }

   return ret != 0;
}

void zcmusic_pause(int pause)
{
   if (!cur_zcm)
      return;

   if (pause)
      stream_pause();
   else
      stream_resume();
}

void zcmusic_stop(void)
{
   stream_stop();
   cur_zcm = NULL;
}

void zcmusic_unload_file(ZCMUSIC *zcm)
{
   // this will unload and destroy all of the data/etc.
   // associated with 'zcm'.

   if (zcm == NULL) return;

   switch (zcm->type)
   {
      case STREAM_VORBIS:
         vorbis_destroy((VORBIS *)zcm->stream, TRUE);
         break;

      case STREAM_MP3:
         mp3_destroy((MP3 *)zcm->stream, TRUE);
         break;

      case STREAM_GME:
         gme_destroy((GME *)zcm->stream);
         break;
   }

   // free up the ZCMUSIC object
   free(zcm);
}

int zcmusic_get_tracks(ZCMUSIC *zcm)
{
   if (zcm == NULL) return 0;

   switch (zcm->type)
   {
      case STREAM_VORBIS:
      case STREAM_MP3:
         return 0;
         break;

      case STREAM_GME:
         return gme_track_count((GME *)zcm->stream);

         break;
   }

   return 0;
}

int zcmusic_change_track(int tracknum)
{
   if (cur_zcm == NULL) return -1;

   switch (cur_zcm->type)
   {
      case STREAM_VORBIS:
      case STREAM_MP3:
         return -1;
         break;

      case STREAM_GME:
         gme_change_track(tracknum);
         cur_zcm->track = tracknum;
         break;
   }

   return 0;
}
