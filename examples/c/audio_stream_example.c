
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

#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <assert.h>

size_t t = 0;

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif

int callback(void* data)
{
	size_t i;
	short* buffer;
	unsigned int buffer_size;
	
	assert(sizeof(short) == 2);
	buffer = (short*)lb_audio_stream_get_buffer(data);
	buffer_size = lb_audio_stream_get_buffer_size(data);

	for (i = 0; i < buffer_size * 2; i += 2) {
		double sample = sin(t / M_PI / 2) * 0.25;
		buffer[i] = (short)(((sample + 1) / 2) * USHRT_MAX + SHRT_MIN);
		buffer[i + 1] = buffer[i];
		t += 1;
	}

	return 0;
}

int main( int argc, char** argv )
{
	LBAudioStream audio_stream;

	audio_stream = lb_audio_stream_construct(16, 44100, 2, 8192);
	
	if (lb_audio_stream_constructed(audio_stream) == 0)
		return EXIT_FAILURE;
	
	if (lb_audio_stream_open(audio_stream) != LBAudioStreamNoError)
	{
		lb_audio_stream_destruct(audio_stream);
		return EXIT_FAILURE;
	}

	while (1) {
		if (lb_audio_stream_wait_for_available_buffers(audio_stream) != LBAudioStreamNoError)
			break;

		callback(audio_stream);
		lb_audio_stream_write(audio_stream);
	}

	if (lb_audio_stream_close(audio_stream) != LBAudioStreamNoError)
	{
		lb_audio_stream_destruct(audio_stream);
		return EXIT_FAILURE;
	}
	lb_audio_stream_destruct(audio_stream);

	return EXIT_SUCCESS;
}
