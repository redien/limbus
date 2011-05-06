
//          Copyright Jesper Oskarsson 2009 - 2010.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

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
