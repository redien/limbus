
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include <limbus/audio_stream.h>
#include <alsa/asoundlib.h>
#include <alloca.h>
#include <string.h>
#include <assert.h>

typedef struct _AlsaAudioStream
{
    snd_pcm_t* pcm_handle;
    unsigned int rate, channels, bits;
    snd_pcm_uframes_t frames;
    unsigned char* buffer;
} AlsaAudioStream;

LBAudioStream lb_audio_stream_construct( int bits,
                                         int sample_rate,
                                         int channels,
                                         unsigned int buffer_size )
{
    AlsaAudioStream* self;
    unsigned int size;

    self = (AlsaAudioStream*)malloc( sizeof *self );
    if (!self)
    {
        return NULL;
    }
    
    self->rate = sample_rate;
    self->channels = channels;
    self->bits = bits;
    self->frames = buffer_size;
    size = self->frames * self->channels * (self->bits / 8);
    self->buffer = (unsigned char*)malloc( size );

    return (LBAudioStream)self;
}

void lb_audio_stream_destruct( LBAudioStream audio_stream )
{
    AlsaAudioStream* self = (AlsaAudioStream*)audio_stream;
    assert( self );

    free( self->buffer );
}

int lb_audio_stream_constructed( LBAudioStream audio_stream )
{
	return (audio_stream) ? 1 : 0;
}

LBAudioStreamError lb_audio_stream_open( LBAudioStream audio_stream )
{
    AlsaAudioStream* self = (AlsaAudioStream*)audio_stream;
    int status, format;
    snd_pcm_hw_params_t* pcm_hw_params;
    snd_pcm_sw_params_t* pcm_sw_params;

	if (!(self->bits == 32 || self->bits == 24 || self->bits == 16 || self->bits == 8))
		return LBAudioStreamUnsupportedSampleSize; 

    status = snd_pcm_open( &self->pcm_handle,
                           "default",
                           SND_PCM_STREAM_PLAYBACK,
                           SND_PCM_NONBLOCK );
    if (status < 0)
        return LBAudioStreamFailedToOpen;

    if (self->bits == 16)
    {
        format = SND_PCM_FORMAT_S16_LE;
    }
    else if (self->bits == 24)
    {
        format = SND_PCM_FORMAT_S24_3LE;
    }
    else if (self->bits == 32)
    {
        format = SND_PCM_FORMAT_S32_LE;
    }
    else
    {
        format = SND_PCM_FORMAT_S8;
    }

    snd_pcm_hw_params_alloca( &pcm_hw_params );
    snd_pcm_hw_params_any( self->pcm_handle, pcm_hw_params );
    snd_pcm_hw_params_set_access( self->pcm_handle,
                                  pcm_hw_params,
                                  SND_PCM_ACCESS_RW_INTERLEAVED );
    snd_pcm_hw_params_set_format( self->pcm_handle,
                                  pcm_hw_params,
                                  format );
    snd_pcm_hw_params_set_rate_near( self->pcm_handle,
                                     pcm_hw_params,
                                     &self->rate,
                                     NULL );
    snd_pcm_hw_params_set_channels_near( self->pcm_handle,
                                         pcm_hw_params,
                                         &self->channels );

    if (self->frames == LBAudioStreamDefaultBufferSize)
    {
        snd_pcm_hw_params_get_period_size_min( pcm_hw_params, &self->frames, NULL );
    }
    else
    {
    	snd_pcm_uframes_t max_frames;
        snd_pcm_hw_params_get_period_size_max( pcm_hw_params, &max_frames, NULL );
        if (self->frames > max_frames)
            self->frames = max_frames;
    }
    snd_pcm_hw_params_set_period_size_near( self->pcm_handle,
                                            pcm_hw_params,
                                            &self->frames,
                                            NULL );
    status = snd_pcm_hw_params( self->pcm_handle, pcm_hw_params );
    if (status < 0)
    {
        snd_pcm_close( self->pcm_handle );
        return LBAudioStreamFailedToOpen;
    }

    snd_pcm_sw_params_alloca( &pcm_sw_params );
    snd_pcm_sw_params_current( self->pcm_handle, pcm_sw_params );
    snd_pcm_sw_params_set_avail_min( self->pcm_handle, pcm_sw_params, self->frames );
    snd_pcm_sw_params_set_start_threshold( self->pcm_handle, pcm_sw_params, 0U );
    status = snd_pcm_sw_params( self->pcm_handle, pcm_sw_params );
	if (status < 0)
	{
		snd_pcm_close( self->pcm_handle );
		return LBAudioStreamFailedToOpen;
	}

	return LBAudioStreamNoError;
}

LBAudioStreamError lb_audio_stream_close( LBAudioStream audio_stream )
{
    AlsaAudioStream* self = (AlsaAudioStream*)audio_stream;
    snd_pcm_drop( self->pcm_handle );
    snd_pcm_close( self->pcm_handle );
	return LBAudioStreamNoError;
}

int lb_audio_stream_get_channels( LBAudioStream audio_stream )
{
    AlsaAudioStream* self = (AlsaAudioStream*)audio_stream;
    assert( self );
    return self->channels;
}

int lb_audio_stream_get_rate( LBAudioStream audio_stream )
{
    AlsaAudioStream* self = (AlsaAudioStream*)audio_stream;
    assert( self );
    return self->rate;
}

void* lb_audio_stream_get_buffer( LBAudioStream audio_stream )
{
    AlsaAudioStream* self = (AlsaAudioStream*)audio_stream;
    assert( self );
    return self->buffer;
}

unsigned int lb_audio_stream_get_buffer_size( LBAudioStream audio_stream )
{
    AlsaAudioStream* self = (AlsaAudioStream*)audio_stream;
    assert( self );
    return self->frames;
}

int lb_audio_stream_write( LBAudioStream audio_stream )
{
    AlsaAudioStream* self = (AlsaAudioStream*)audio_stream;
    int status;
    assert( self );
    
    while ((status = snd_pcm_writei( self->pcm_handle, self->buffer, self->frames )) < 0)
    {
        snd_pcm_prepare( self->pcm_handle );
    }

    return status;
}

void lb_audio_stream_drop( LBAudioStream audio_stream )
{
    AlsaAudioStream* self = (AlsaAudioStream*)audio_stream;
    snd_pcm_drop( self->pcm_handle );
}

LBAudioStreamError lb_audio_stream_wait_for_available_buffers( LBAudioStream audio_stream )
{
    AlsaAudioStream* self = (AlsaAudioStream*)audio_stream;
	
	if (snd_pcm_wait(self->pcm_handle, 1000) < 0) {
		return LBAudioStreamFailedToWait;
	}
	
	return LBAudioStreamNoError;
}
