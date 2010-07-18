
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef LB_AUDIO_STREAM_H
#define LB_AUDIO_STREAM_H
#ifdef __cplusplus
extern "C"
{
#endif

typedef void* LBAudioStream;

LBAudioStream lb_audio_stream_construct( int bits,
                                         int sample_rate,
                                         int channels );
LBAudioStream lb_audio_stream_destruct( LBAudioStream audio_stream );
int lb_audio_stream_get_channels( LBAudioStream audio_stream );
int lb_audio_stream_get_rate( LBAudioStream audio_stream );
void* lb_audio_stream_get_buffer( LBAudioStream audio_stream );
unsigned int lb_audio_stream_get_buffer_size( LBAudioStream audio_stream );
int lb_audio_stream_write( LBAudioStream audio_stream );

#ifdef __cplusplus
}
#endif
#endif

