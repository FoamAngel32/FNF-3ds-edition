#ifndef VORBIS_H
#define VORBIS_H
#include <tremor/ivorbisfile.h>
#include <tremor/ivorbiscodec.h>
#include <3ds.h>
#include <string.h>
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
extern OggVorbis_File vorbis_file[128];
extern u8 temp0_audio;
extern Thread threadId_audio;
extern Thread threadId_ms_counter;
typedef struct
{
    int vorbis_id;
    int chn;
    signed char done;
    bool pause;
    ndspWaveBuf s_waveBufs[3];
    int16_t *s_audioBuffer;
    float pitch;
} OggVorbis;
extern OggVorbis oggs[23];
static const int THREAD_AFFINITY = -1;        // Execute thread on any core
static const int THREAD_STACK_SZ = 32 * 1024; // 32kB stack for audio thread

// ---- END DEFINITIONS ----

extern LightEvent s_event;
extern volatile bool s_quit; // Quit flag

// Copied from audio example with a slight bit of change

// Pause until user presses a button

// ---- END HELPER FUNCTIONS ----

// Audio initialisation code
// This sets up NDSP and our primary audio buffer
bool audioInit(OggVorbis *vorbisFile_, int chn, int id, float pitch);
// Audio de-initialisation code
// Stops playback and frees the primary audio buffer
void audioExit(void);
bool fillBuffer(OggVorbis_File *vorbisFile_, ndspWaveBuf *waveBuf_, int chn);
// NDSP audio frame callback
// This signals the audioThread to decode more things
// once NDSP has played a sound frame, meaning that there should be
// one or more available waveBufs to fill with more data.
void audioCallback_ogg(void *const nul_);
// Audio thread
// This handles calling the decoder function to fill NDSP buffers as necessary
void audioThread(void *const unused);
void stop_vorbis(int id);
void load_vorbis(char *restrict song_path, int chn);
void pause_vorbis(int id, bool pause);
void play_vorbis(int chn, bool loop, int id, float pitch);
void create_audio_thread(void);
#endif