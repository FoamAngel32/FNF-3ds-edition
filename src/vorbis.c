#ifndef VORBIS_C
#define VORBIS_C
#include "vorbis.h"
static const int THREAD_AFFINITY = -1;        // Execute thread on any core
static const int THREAD_STACK_SZ = 32 * 1024; // 32kB stack for audio thread
OggVorbis_File vorbis_file[128];
u8 temp0_audio;
Thread threadId_audio;
Thread threadId_ms_counter;
OggVorbis oggs[23];
LightEvent s_event;
volatile bool s_quit = false; // Quit flag
void vorbis_init(void)
{
    int temp;
    for(temp = 0;temp < 23;++temp)
    {
        oggs[temp].s_audioBuffer = NULL;
    }
}
bool audioInit(OggVorbis *vorbisFile_, int chn, int id, float pitch)
{
    // ov_pcm_seek(&vorbis_file[id], 0);
    vorbisFile_->done = 0;
    vorbisFile_->chn = chn;
    vorbisFile_->vorbis_id = id;
    vorbisFile_->pitch = pitch;
    // Setup NDSP
    ndspChnReset(chn);
    ndspChnSetInterp(chn, NDSP_INTERP_LINEAR);
    ndspChnSetRate(chn, 32000 * pitch);
    ndspChnSetFormat(chn, NDSP_FORMAT_MONO_PCM16);

    // Allocate audio buffer
    // 120ms buffer
    if(vorbisFile_->s_audioBuffer)
        linearFree(vorbisFile_->s_audioBuffer);

    const size_t SAMPLES_PER_BUF = (32000 * pitch) * 120 / 1000;
    // mono (1) or stereo (2)
    const size_t CHANNELS_PER_SAMPLE = 1;
    // s16 buffer
    const size_t WAVEBUF_SIZE = SAMPLES_PER_BUF * CHANNELS_PER_SAMPLE * sizeof(s16);
    const size_t bufferSize = WAVEBUF_SIZE * ARRAY_SIZE(vorbisFile_->s_waveBufs);
    vorbisFile_->s_audioBuffer = (int16_t *)linearAlloc(bufferSize);
    // Setup waveBufs for NDSP
    memset(&vorbisFile_->s_waveBufs, 0, sizeof(vorbisFile_->s_waveBufs));
    int16_t *buffer = vorbisFile_->s_audioBuffer;

    for (size_t i = 0; i < ARRAY_SIZE(vorbisFile_->s_waveBufs); ++i)
    {
        vorbisFile_->s_waveBufs[i].data_vaddr = buffer;
        vorbisFile_->s_waveBufs[i].nsamples = WAVEBUF_SIZE / sizeof(buffer[0]);
        vorbisFile_->s_waveBufs[i].status = NDSP_WBUF_DONE;

        buffer += WAVEBUF_SIZE / sizeof(buffer[0]);
    }

    return true;
}
void audioExit(void)
{
    for (temp0_audio = 0; temp0_audio <= 23; ++temp0_audio)
    {
        stop_vorbis(temp0_audio);
        ndspChnReset(temp0_audio);
    }
    for (temp0_audio = 0; temp0_audio < ARRAY_SIZE(oggs); ++temp0_audio)
    {
        ov_clear(&vorbis_file[oggs[temp0_audio].vorbis_id]);
        linearFree(oggs[temp0_audio].s_audioBuffer);
    }
}
bool fillBuffer(OggVorbis_File *vorbisFile_, ndspWaveBuf *waveBuf_, int chn)
{
    int totalBytes = 0;
    bool eof = false;

    // printf("FIlled buffer\n");
    while (totalBytes < waveBuf_->nsamples * sizeof(s16))
    {
        int16_t *buffer = waveBuf_->data_pcm16 + (totalBytes / sizeof(s16));
        size_t bufferSize = waveBuf_->nsamples * sizeof(s16) - totalBytes;

        int bytesRead = ov_read(vorbisFile_, (char *)buffer, bufferSize, NULL);
        if (bytesRead == 0)
        { // EOF
            eof = true;
            break;
        }
        if (bytesRead < 0)
        {
            eof = true;
            break;
        }

        totalBytes += bytesRead;
    }

    if (totalBytes == 0 && eof)
    {
        // printf("FIll buffer eof\n");
        // ov_pcm_seek();
        return false;
    }

    waveBuf_->nsamples = totalBytes / sizeof(s16);
    ndspChnWaveBufAdd(chn, waveBuf_);
    DSP_FlushDataCache(waveBuf_->data_pcm16, totalBytes);
    return true;
}
void stop_vorbis(int id)
{
    oggs[id].done = 1;
}
void load_vorbis(char *restrict song_path, int id)
{
    stop_vorbis(id);
    FILE *fh = fopen(song_path, "rb");
    ov_clear(&vorbis_file[id]);
    ov_open(fh, &vorbis_file[id], NULL, 0);
}
void audioCallback_ogg(void *const nul_)
{
    // printf("Audio Callback!\n");
    (void)nul_; // Unused
    // ov_raw_seek(&vorbis_file[0], 0);
    if (s_quit)
    { // Quit flag
        return;
    }

    LightEvent_Signal(&s_event);
}
void audioThread(void *const unused)
{
    while (!s_quit)
    { // Whilst the quit flag is unset,
      // search our waveBufs and fill any that aren't currently
      // queued for playback (i.e, those that are 'done')
        for (temp0_audio = 0; temp0_audio < ARRAY_SIZE(oggs); ++temp0_audio)
        {
            if (oggs[temp0_audio].done == 1)
                continue;
            // printf("doing for channel %d               \n", temp0_audio);
            for (size_t i = 0; i < ARRAY_SIZE(oggs[temp0_audio].s_waveBufs); ++i)
            {
                // printf("%d ", oggs[temp0_audio].s_waveBufs[i].status);
                if (oggs[temp0_audio].s_waveBufs[i].status != NDSP_WBUF_DONE)
                    continue;
                else if (oggs[temp0_audio].s_waveBufs[i].status == NDSP_WBUF_DONE)
                {
                    if (!oggs[temp0_audio].pause)
                    {
                        if (!fillBuffer(&vorbis_file[oggs[temp0_audio].vorbis_id], &oggs[temp0_audio].s_waveBufs[i], oggs[temp0_audio].chn))
                        { // Playback complete
                            // printf("");
                            if (oggs[temp0_audio].done == -1)
                            {
                                play_vorbis(temp0_audio, true, oggs[temp0_audio].vorbis_id, oggs[temp0_audio].pitch);
                                // audioInit(&oggs[temp0_audio], oggs[temp0_audio].chn, oggs[temp0_audio].vorbis_id, oggs[temp0_audio].pitch);
                            }
                            else if (oggs[temp0_audio].done == 0)
                                oggs[temp0_audio].done = 1;
                                
                            break;
                        }
                    }
                }
                // else if(oggs[temp0_audio].s_waveBufs[i].status == NDSP_WBUF_DONE){
                //     memset(&oggs[temp0_audio].s_waveBufs[i], 0, sizeof(oggs[temp0_audio].s_waveBufs[i]));
                // }
            }
            // printf("\n");
        }
        // Wait for a signal that we're needed again before continuing,
        // so that we can yield to other things that want to run
        // (Note that the 3DS uses cooperative threading)
        LightEvent_Wait(&s_event);
    }
}
void play_vorbis(int chn, bool loop, int id, float pitch)
{
    // Attempt audioInit
    // if (audioInit(&oggs[chn], chn, id, pitch))
    audioInit(&oggs[chn], chn, id, pitch);
    if (loop)
        oggs[chn].done = -1;
    else
        oggs[chn].done = 0;
    // Set the ndsp sound frame callback which signals our audioThread
}
void pause_vorbis(int id, bool pause)
{
    oggs[id].pause = pause;
    ndspChnSetPaused(oggs[id].chn, pause); 
}
void create_audio_thread(void)
{
    LightEvent_Init(&s_event, RESET_ONESHOT);
    // Spawn audio thread

    // Set the thread priority to the main thread's priority ...
    int32_t priority = 0x30;
    svcGetThreadPriority(&priority, CUR_THREAD_HANDLE);
    // ... then subtract 1, as lower number => higher actual priority ...
    priority -= 2;
    // ... finally, clamp it between 0x18 and 0x3F to guarantee that it's valid.
    priority = priority < 0x18 ? 0x18 : priority;
    priority = priority > 0x3F ? 0x3F : priority;

    // Start the thread, passing the address of our vorbisFile as an argument.
    ndspSetCallback(audioCallback_ogg, NULL);
    for (temp0_audio = 0; temp0_audio <= 23; ++temp0_audio)
    {
        oggs[temp0_audio].done = 1;
    }
    threadId_audio = threadCreate(audioThread, NULL,
                                  THREAD_STACK_SZ, priority,
                                  THREAD_AFFINITY, true);
}
#endif