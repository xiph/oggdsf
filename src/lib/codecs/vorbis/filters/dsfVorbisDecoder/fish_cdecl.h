#include <fishsound/constants.h>
typedef struct {
  int samplerate;
  int channels;
  int format;
} FishSoundInfo;
typedef struct {
  int format;
  const char * name;     
  const char * extension;
} FishSoundFormat;

typedef void * FishSound;
typedef int (__cdecl *FishSoundDecoded) (FishSound * fsound, float ** pcm,
				 long frames, void * user_data);
typedef int (__cdecl *FishSoundEncoded) (FishSound * fsound, unsigned char * buf,
				 long bytes, void * user_data);
int __cdecl fish_sound_identify (unsigned char * buf, long bytes);

FishSound * __cdecl fish_sound_new (int mode, FishSoundInfo * fsinfo);

int __cdecl fish_sound_set_decoded_callback (FishSound * fsound,
				     FishSoundDecoded decoded,
				     void * user_data);

int __cdecl fish_sound_set_encoded_callback (FishSound * fsound,
				     FishSoundEncoded encoded,
				     void * user_data);

long __cdecl fish_sound_decode (FishSound * fsound, unsigned char * buf, long bytes);

long __cdecl fish_sound_encode (FishSound * fsound, float ** pcm, long frames);

long __cdecl fish_sound_flush (FishSound * fsound);

int __cdecl fish_sound_reset (FishSound * fsound);

int __cdecl fish_sound_delete (FishSound * fsound);

int __cdecl fish_sound_command (FishSound * fsound, int command, void * data,
			int datasize);

int __cdecl fish_sound_get_interleave (FishSound * fsound);

int __cdecl fish_sound_set_interleave (FishSound * fsound, int interleave);
