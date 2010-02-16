
//          Copyright Jesper Oskarsson 2009 - 2010.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

using Placebo;
using Opengl;

public class OpenglWindowTest
{
	static public void Main()
	{
		var window = new OpenglWindow();
		window.caption = "C# Window";
		window.width = 640;
		window.height = 480;
		window.create();

		bool running = true;
		window.onWindowEvent += e => { if (e == PlaceboC.WindowEventClose) running = false; };
		window.onKeyEvent += ( key, pressed ) => { if (pressed && key == PlaceboC.KeyEscape) running = false; };

		GL.glClearColor( 0.390625f, 0.58203125f, 0.92578125f, 1.0f );

		var timer = new Timer();
		while (running)
		{
			window.sendEvents();

			GL.glClear( GL.GL_COLOR_BUFFER_BIT );
			window.swapBuffers();

			if (timer.elapsed() > 1.0f)
			{
				System.Console.WriteLine( "Second passed..." );
				timer.reset();
			}
		}

		window.destruct();
	}
}
