
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
    LBAudioStreamFailedToOpen,
    /* The stream has already been opened */
    LBAudioStreamAlreadyOpen,
    /* The stream has not been opened */
    LBAudioStreamNotOpen,
    /* The stream could not wait for a new buffer to become available */
    LBAudioStreamFailedToWait,
    /* The sample size specified is not supported */
    LBAudioStreamUnsupportedSampleSize
};
typedef enum LBAudioStreamError LBAudioStreamError;

#define LBAudioStreamDefaultBufferSize 0

LBAudioStream lb_audio_stream_construct( int sample_size_in_bits,
                                         int sample_rate_in_hz,
                                         int channels_per_sample,
                                         unsigned int buffer_size_in_samples );
void lb_audio_stream_destruct( LBAudioStream audio_stream );
int lb_audio_stream_constructed( LBAudioStream audio_stream );

LBAudioStreamError lb_audio_stream_open( LBAudioStream audio_stream );
LBAudioStreamError lb_audio_stream_close( LBAudioStream audio_stream );
int lb_audio_stream_write( LBAudioStream audio_stream );
void lb_audio_stream_drop( LBAudioStream audio_stream );

LBAudioStreamError lb_audio_stream_wait_for_available_buffers( LBAudioStream audio_stream );

int lb_audio_stream_get_channels( LBAudioStream audio_stream );
int lb_audio_stream_get_rate( LBAudioStream audio_stream );
void* lb_audio_stream_get_buffer( LBAudioStream audio_stream );
unsigned int lb_audio_stream_get_buffer_size( LBAudioStream audio_stream );

#ifdef __cplusplus
}
#endif
#endif

