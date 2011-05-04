
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
#endif

#include <limbus/audio_stream.h>

#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <assert.h>

float t = 0;
int buffers_done = 0;

int callback(void* data)
{
	size_t i;
	short* buffer;

	assert(sizeof(short) == 2);
	buffer = (short*)lb_audio_stream_get_buffer(data);
	for (i = 0; i < 100000 * 2; i += 2) {
		buffer[i] = (short)((sin(t) + 1.0) * USHRT_MAX + SHRT_MIN) * 0.1;
		buffer[i + 1] = buffer[i];
		t += 0.05f;
	}

	buffers_done = 1;
	printf("%f\n", t);

	return 0;
}

int main( int argc, char** argv )
{
	LBAudioStream audio_stream;

	audio_stream = lb_audio_stream_construct(16, 44100, 2, 100000);
	lb_audio_stream_set_callback(audio_stream, callback, audio_stream);

	lb_audio_stream_open(audio_stream);
	while (1) {
		if (buffers_done > 0)
		{
			lb_audio_stream_write(audio_stream);
			buffers_done = 0;
		}
	}
	lb_audio_stream_close(audio_stream);

	lb_audio_stream_destruct(audio_stream);

	return 0;
}
