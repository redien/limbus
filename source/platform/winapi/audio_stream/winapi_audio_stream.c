
#include <limbus/audio_stream.h>
#include <stdlib.h>
#include <windows.h>
#include <assert.h>

#define NUMBER_OF_BUFFERS 20

typedef struct AudioStreamWinAPI
{
	HWAVEOUT handle;
    WAVEFORMATEX format;
	unsigned int buffer_size;
	int opened;
	size_t buffers_available;
	CRITICAL_SECTION buffers_available_mutex;
	
	WAVEHDR headers[NUMBER_OF_BUFFERS];
	size_t current_buffer;
} AudioStreamWinAPI;

static void CALLBACK audio_stream_callback(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
	AudioStreamWinAPI* self = (AudioStreamWinAPI*)dwInstance;

	if (uMsg != WOM_DONE)
		return;

	EnterCriticalSection(&self->buffers_available_mutex);
	self->buffers_available += 1;
	LeaveCriticalSection(&self->buffers_available_mutex);
}

LBAudioStream lb_audio_stream_construct( int bits,
                                         int sample_rate,
                                         int channels,
                                         unsigned int buffer_size )
{
	AudioStreamWinAPI* self = (AudioStreamWinAPI*)malloc(sizeof(AudioStreamWinAPI));

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

int lb_audio_stream_constructed( LBAudioStream audio_stream )
{
	return (audio_stream) ? 1 : 0;
}

LBAudioStreamError lb_audio_stream_open( LBAudioStream audio_stream )
{
	int result;
	size_t i;
	AudioStreamWinAPI* self = (AudioStreamWinAPI*)audio_stream;

	if (!(self->format.wBitsPerSample == 8 || self->format.wBitsPerSample == 16))
		return LBAudioStreamUnsupportedSampleSize;

	if (self->opened)
		return LBAudioStreamAlreadyOpen;

	for (i = 0; i < NUMBER_OF_BUFFERS; ++i) {
		self->headers[i].dwBufferLength = self->format.nBlockAlign * self->buffer_size;
		self->headers[i].lpData = (LPSTR)malloc(self->headers[i].dwBufferLength);
		self->headers[i].dwFlags = 0;
	}

	InitializeCriticalSection(&self->buffers_available_mutex);
	self->buffers_available = NUMBER_OF_BUFFERS;
	self->current_buffer = 0;

	result = waveOutOpen(&self->handle, WAVE_MAPPER, &self->format, (DWORD_PTR)&audio_stream_callback, (DWORD_PTR)self, CALLBACK_FUNCTION);
	if (result != MMSYSERR_NOERROR) {
		return LBAudioStreamFailedToOpen;
	}

	for (i = 0; i < NUMBER_OF_BUFFERS; ++i) {
		result = waveOutPrepareHeader(self->handle, &self->headers[i], sizeof(WAVEHDR));
		if (result != MMSYSERR_NOERROR)
			return LBAudioStreamFailedToOpen;
	}

	return LBAudioStreamNoError;
}

LBAudioStreamError lb_audio_stream_close( LBAudioStream audio_stream )
{
	size_t i;
	AudioStreamWinAPI* self = (AudioStreamWinAPI*)audio_stream;
	if (!self->opened)
		return LBAudioStreamNotOpen;

	for (i = 0; i < NUMBER_OF_BUFFERS; ++i) {
		waveOutUnprepareHeader(self->handle, &self->headers[i], sizeof(WAVEHDR));
		free(self->headers[i].lpData);
	}

	waveOutClose(self->handle);

	DeleteCriticalSection(&self->buffers_available_mutex);

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
	return self->headers[self->current_buffer].lpData;
}

unsigned int lb_audio_stream_get_buffer_size( LBAudioStream audio_stream )
{
	AudioStreamWinAPI* self = (AudioStreamWinAPI*)audio_stream;
	return self->buffer_size;
}

int lb_audio_stream_write( LBAudioStream audio_stream )
{
	AudioStreamWinAPI* self = (AudioStreamWinAPI*)audio_stream;
	int result = waveOutWrite(self->handle, &self->headers[self->current_buffer], sizeof(WAVEHDR));

	EnterCriticalSection(&self->buffers_available_mutex);
	self->buffers_available -= 1;
	LeaveCriticalSection(&self->buffers_available_mutex);

	self->current_buffer = (self->current_buffer + 1) % NUMBER_OF_BUFFERS;

	return 0;
}

void lb_audio_stream_drop( LBAudioStream audio_stream )
{
	AudioStreamWinAPI* self = (AudioStreamWinAPI*)audio_stream;
	int result = waveOutReset(self->handle);
}

LBAudioStreamError lb_audio_stream_wait_for_available_buffers( LBAudioStream audio_stream )
{
	AudioStreamWinAPI* self = (AudioStreamWinAPI*)audio_stream;
	while (self->buffers_available == 0) {}
	return LBAudioStreamNoError;
}
