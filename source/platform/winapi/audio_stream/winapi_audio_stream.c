
#include <limbus/audio_stream.h>
#include <stdlib.h>
#include <windows.h>
#include <assert.h>

typedef struct AudioStreamWinAPI
{
	HWAVEOUT handle;
    WAVEFORMATEX format;
	WAVEHDR header;
	unsigned int buffer_size;
	int (*callback)(void*);
	void* callback_data;
	int opened;
} AudioStreamWinAPI;

static void CALLBACK audio_stream_callback(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
	AudioStreamWinAPI* self = (AudioStreamWinAPI*)dwInstance;
	self->callback(self->callback_data);
}

LBAudioStream lb_audio_stream_construct( int bits,
                                         int sample_rate,
                                         int channels,
                                         unsigned int buffer_size )
{
	AudioStreamWinAPI* self = (AudioStreamWinAPI*)malloc(sizeof(AudioStreamWinAPI));

	assert(bits == 8 || bits == 16);

	self->format.wFormatTag = 1; // PCM
	self->format.wBitsPerSample = bits;
	self->format.nChannels = channels;
	self->format.nSamplesPerSec = sample_rate;
	self->format.nBlockAlign = channels * (bits / 8);
	self->format.nAvgBytesPerSec = sample_rate * self->format.nBlockAlign;
	self->format.cbSize = 0;

	self->opened = 0;
	self->buffer_size = buffer_size;

	return self;
}

void lb_audio_stream_destruct( LBAudioStream audio_stream )
{
	AudioStreamWinAPI* self = (AudioStreamWinAPI*)audio_stream;
	if (self->opened)
	{
		lb_audio_stream_close( audio_stream );
	}
	free(self);
}

LBAudioStreamError lb_audio_stream_open( LBAudioStream audio_stream )
{
	int result;
	AudioStreamWinAPI* self = (AudioStreamWinAPI*)audio_stream;

	if (self->opened)
		return LBAudioStreamAlreadyOpen;

	self->header.dwBufferLength = self->format.nBlockAlign * self->buffer_size;
	self->header.lpData = (LPSTR)malloc(self->header.dwBufferLength);
	self->header.dwFlags = 0;

	result = waveOutOpen(&self->handle, WAVE_MAPPER, &self->format, (DWORD_PTR)&audio_stream_callback, (DWORD_PTR)self, CALLBACK_FUNCTION);
	if (result != MMSYSERR_NOERROR) {
		return LBAudioStreamFailedToOpenStream;
	}

	result = waveOutPrepareHeader(self->handle, &self->header, sizeof(WAVEHDR));

	return LBAudioStreamNoError;
}

LBAudioStreamError lb_audio_stream_close( LBAudioStream audio_stream )
{
	AudioStreamWinAPI* self = (AudioStreamWinAPI*)audio_stream;
	if (!self->opened)
		return LBAudioStreamNotOpen;

	waveOutUnprepareHeader(self->handle, &self->header, sizeof(WAVEHDR));
	free(self->header.lpData);
	waveOutClose(self->handle);

	return LBAudioStreamNoError;
}

int lb_audio_stream_get_channels( LBAudioStream audio_stream )
{
	AudioStreamWinAPI* self = (AudioStreamWinAPI*)audio_stream;
	return self->format.nChannels;
}

int lb_audio_stream_get_rate( LBAudioStream audio_stream )
{
	AudioStreamWinAPI* self = (AudioStreamWinAPI*)audio_stream;
	return self->format.nSamplesPerSec;
}

void* lb_audio_stream_get_buffer( LBAudioStream audio_stream )
{
	AudioStreamWinAPI* self = (AudioStreamWinAPI*)audio_stream;
	return self->header.lpData;
}

unsigned int lb_audio_stream_get_buffer_size( LBAudioStream audio_stream )
{
	AudioStreamWinAPI* self = (AudioStreamWinAPI*)audio_stream;
	return self->buffer_size;
}

int lb_audio_stream_write( LBAudioStream audio_stream )
{
	AudioStreamWinAPI* self = (AudioStreamWinAPI*)audio_stream;
	int result = waveOutWrite(self->handle, &self->header, sizeof(WAVEHDR));
	return 0;
}

void lb_audio_stream_drop( LBAudioStream audio_stream )
{
	AudioStreamWinAPI* self = (AudioStreamWinAPI*)audio_stream;
	int result = waveOutReset(self->handle);
}

void lb_audio_stream_set_callback( LBAudioStream audio_stream, int (*callback)(void* data), void* data )
{
	AudioStreamWinAPI* self = (AudioStreamWinAPI*)audio_stream;
	self->callback = callback;
	self->callback_data = data;
}
