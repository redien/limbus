
//          Copyright Jesper Oskarsson 2009 - 2010.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

public class OpenglWindowTest
{
	static private bool running;
	
	static private void CloseHandler( object sender )
	{
		running = false;
	}
	
	static private void KeyDownHandler( object sender, int key, bool pressed )
	{
		if (pressed && key == Limbus.Imported.KeyEscape)
			running = false;
	}

	static public void Main()
	{
		Limbus.OpenglWindow window = new Limbus.OpenglWindow();
		window.Caption = "C# Window";
		window.Width = 640;
		window.Height = 480;
		window.OnClose += new Limbus.OnCloseDelegate( CloseHandler );
		window.Create();
		
		Limbus.Keyboard keyboard = new Limbus.Keyboard( window );
		keyboard.OnKeyEvent += new Limbus.OnKeyEventDelegate( KeyDownHandler );

		running = true;
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
