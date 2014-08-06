
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

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
