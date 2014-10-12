
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
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

/** Constructs a new audio stream object.
  * @return a pointer to the newly created audio stream object if successful,
  * else returns a pointer identifying a non-constructed audio stream object.
  * Use lb_audio_stream_constructed to find out if an object was
  * constructed or not.
  */
LBAudioStream lb_audio_stream_construct( int sample_size_in_bits,
                                         int sample_rate_in_hz,
                                         int channels_per_sample,
                                         unsigned int buffer_size_in_samples );

/** Destructs a previously constructed audio stream object.
  * Destructing an audio stream object will deallocate all the resources
  * associated with it.
  * @param audio_stream a pointer to the audio stream object to destruct.
  */
void lb_audio_stream_destruct( LBAudioStream audio_stream );

/** Check for successfully constructed audio stream objects.
  * @param audio_stream a pointer returned by lb_audio_stream_construct
  * @return 1 if audio_stream points to a successfully constructed
  * audio stream object or 0 if it doesn't.
  */
int lb_audio_stream_constructed( LBAudioStream audio_stream );

/** Opens the audio stream for and starts streaming.
  * @param audio_stream a pointer to a successfully constructed audio stream object.
  * @return LBAudioStreamNoError if the stream was opened successfully.
  * LBAudioStreamFailedToOpen if the stream failed to open.
  * LBAudioStreamAlreadyOpen if the stream was already opened.
  * LBAudioStreamUnsupportedSampleSize if the specified sample size is not supported.
  */
LBAudioStreamError lb_audio_stream_open( LBAudioStream audio_stream );

/** Closes a previously opened audio stream.
  * @param audio_stream a pointer to a successfully constructed audio stream object.
  * @return LBAudioStreamNoError if the stream was closed successfully.
  * LBAudioStreamNotOpen if the stream was not opened to begin with.
  */
LBAudioStreamError lb_audio_stream_close( LBAudioStream audio_stream );

/** Writes the contents of the current buffer to the audio device.
  * @param audio_stream a pointer to a successfully constructed audio stream object.
  */
void lb_audio_stream_write( LBAudioStream audio_stream );

/** Drops the currently playing samples on the audio device.
  * @param audio_stream a pointer to a successfully constructed audio stream object.
  */
void lb_audio_stream_drop( LBAudioStream audio_stream );

/** Halts the system thread until another buffer is available for writing.
  * @param audio_stream a pointer to a successfully constructed audio stream object.
  */
LBAudioStreamError lb_audio_stream_wait_for_available_buffers( LBAudioStream audio_stream );

/** Returns the number of channels for the audio stream.
  * @param audio_stream a pointer to a successfully constructed audio stream object.
  * @return the number of channels for the audio stream.
  */
int lb_audio_stream_get_channels( LBAudioStream audio_stream );

/** Returns the sample rate for the audio stream in samples per second.
  * @param audio_stream a pointer to a successfully constructed audio stream object.
  * @return the sample rate for the audio stream in samples per second.
  */
int lb_audio_stream_get_sample_rate( LBAudioStream audio_stream );

/** Returns the buffer currently available for writing.
  * @param audio_stream a pointer to a successfully constructed audio stream object.
  * @return a pointer to the piece of memory used to write to the buffer.
  */
void* lb_audio_stream_get_buffer( LBAudioStream audio_stream );

/** Returns the size of the buffer currently available for writing.
  * @param audio_stream a pointer to a successfully constructed audio stream object.
  * @return an unsigned int with the size of the buffer in bytes.
  */
unsigned int lb_audio_stream_get_buffer_size( LBAudioStream audio_stream );

#ifdef __cplusplus
}
#endif
#endif

