
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
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
    int status, format;
    unsigned int size;
    snd_pcm_hw_params_t* pcm_hw_params;
    AlsaAudioStream* self;
    
    self = (AlsaAudioStream*)malloc( sizeof *self );
    if (!self)
    {
        return NULL;
    }

    status = snd_pcm_open( &self->pcm_handle,
                           "default",
                           SND_PCM_STREAM_PLAYBACK,
                           0 );
    if (status < 0)
    {
        free( self );
        return NULL;
    }

    self->rate = sample_rate;
    self->channels = channels;
    self->bits = bits;
    assert( bits == 32 || bits == 24 || bits == 16 || bits == 8 );
    if (bits == 16)
    {
        format = SND_PCM_FORMAT_S16_LE;
    }
    else if (bits == 24)
    {
        format = SND_PCM_FORMAT_S24_3LE;
    }
    else if (bits == 32)
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

    if (buffer_size == LBAudioStreamDefaultBufferSize)
    {
        snd_pcm_hw_params_get_period_size_min( pcm_hw_params, &self->frames, NULL );
        buffer_size = self->frames;
    }
    else
    {
        snd_pcm_hw_params_get_period_size_max( pcm_hw_params, &self->frames, NULL );
        if (buffer_size > self->frames)
            buffer_size = self->frames;
    }
    self->frames = buffer_size;
    snd_pcm_hw_params_set_period_size_near( self->pcm_handle,
                                            pcm_hw_params,
                                            &self->frames,
                                            NULL );
    status = snd_pcm_hw_params( self->pcm_handle, pcm_hw_params );
    if (status < 0)
    {
        snd_pcm_close( self->pcm_handle );
        free( self );
        return NULL;
    }
    
    size = lb_audio_stream_get_buffer_size( self );
    self->buffer = (unsigned char*)malloc( size );

    return (LBAudioStream)self;
}

LBAudioStream lb_audio_stream_destruct( LBAudioStream audio_stream )
{
    AlsaAudioStream* self = (AlsaAudioStream*)audio_stream;
    assert( self );

    free( self->buffer );
    snd_pcm_drop( self->pcm_handle );
    snd_pcm_close( self->pcm_handle );

    return self;
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
    return self->frames * (self->bits / 8) * self->channels;
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

