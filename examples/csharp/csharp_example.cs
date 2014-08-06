
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
	static public void Main()
	{
		bool running = true;
		Limbus.OpenglWindow window = new Limbus.OpenglWindow();
		window.Caption = "C# Window";
		window.Width = 640;
		window.Height = 480;
		window.OnClose += delegate( Limbus.OpenglWindow sender ) { running = false; };
		window.Create();
		
		Limbus.Keyboard keyboard = new Limbus.Keyboard( window );
		keyboard.OnKeyEvent += delegate( Limbus.Keyboard sender, int key, bool pressed )
		{
			if (pressed && key == Limbus.Imported.KEY_ESCAPE)
				running = false;
		};

		Limbus.Timer timer = new Limbus.Timer();
		while (running)
		{
			window.PollEvents();
			keyboard.PollEvents();
			window.SwapBuffers();
			
			if (timer.Elapsed > 1.0f)
			{
				System.Console.WriteLine( "Second passed..." );
				timer.Reset();
			}
		}

		keyboard.Destruct();
		window.Destruct();
	}
}
