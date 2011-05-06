
//          Copyright Jesper Oskarsson 2009 - 2010.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

public class OpenglWindowTest
{
	private static uint t = 0;

	static public void callback(Limbus.AudioStream audio_stream)
	{
		unsafe {
			short* buffer = (short*)audio_stream.Buffer;
			uint buffer_size = audio_stream.BufferSize;
			for (uint i = 0; i < buffer_size * 2; i += 2)
			{
				double sample = System.Math.Sin(t / System.Math.PI / 2) * 0.25;
				buffer[i] = (short)(((sample + 1) / 2) * ushort.MaxValue + short.MinValue);
				buffer[i + 1] = buffer[i];
				t += 1;
			}
		}
	}
	
	static public void PlaySineWave()
	{
		Limbus.AudioStream audio_stream = new Limbus.AudioStream(16, 44100, 2, 8192);
		audio_stream.Open();
		
		for (;;) {
			audio_stream.WaitForAvailableBuffers();
			callback(audio_stream);
			audio_stream.Write();
		}
	}

	static public void Main()
	{
		System.Threading.Thread thread = new System.Threading.Thread(PlaySineWave);
		thread.IsBackground = true;
		thread.Start();
		
		Limbus.Timer timer = new Limbus.Timer();
		for (;;) {
			System.Console.WriteLine("time elapsed: " + timer.Elapsed);
		}
	}
}
