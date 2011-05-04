
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

/* Experimental */

typedef void* LBAudioStream;

/* An enumeration of the API-specific errors. */
enum LBAudioStreamError
{
    /* No error occured */
    LBAudioStreamNoError,
    /* The stream could not be opened */
    LBAudioStreamFailedToOpenStream,
    /* The stream has already been opened */
    LBAudioStreamAlreadyOpen,
    /* The stream has not been opened */
    LBAudioStreamNotOpen
};
typedef enum LBAudioStreamError LBAudioStreamError;

#define LBAudioStreamDefaultBufferSize 0

LBAudioStream lb_audio_stream_construct( int bits,
                                         int sample_rate,
                                         int channels,
                                         unsigned int buffer_size );
void lb_audio_stream_destruct( LBAudioStream audio_stream );

void lb_audio_stream_set_callback( LBAudioStream audio_stream, int (*callback)(void* data), void* data );

LBAudioStreamError lb_audio_stream_open( LBAudioStream audio_stream );
LBAudioStreamError lb_audio_stream_close( LBAudioStream audio_stream );
int lb_audio_stream_write( LBAudioStream audio_stream );
void lb_audio_stream_drop( LBAudioStream audio_stream );

int lb_audio_stream_get_channels( LBAudioStream audio_stream );
int lb_audio_stream_get_rate( LBAudioStream audio_stream );
void* lb_audio_stream_get_buffer( LBAudioStream audio_stream );
unsigned int lb_audio_stream_get_buffer_size( LBAudioStream audio_stream );

#ifdef __cplusplus
}
#endif
#endif

