
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include <limbus/OpenglWindow.hpp>
#include <limbus/Keyboard.hpp>
#include <limbus/Mouse.hpp>

#include <limbus/opengl.h>

#include <iostream>
#include <cstring>

#define GL_ARRAY_BUFFER 0x8892
#define GL_STATIC_DRAW 0x88E4

typedef void (APIENTRY *GLGENBUFFERSPROC)( GLsizei, GLuint* );
typedef void (APIENTRY *GLDELETEBUFFERSPROC)( GLsizei, const GLuint* );
typedef void (APIENTRY *GLBINDBUFFERPROC)( GLenum, GLuint );
typedef void (APIENTRY *GLBUFFERDATAPROC)( GLenum, ptrdiff_t, const GLvoid*, GLenum );
typedef void (APIENTRY *GLBUFFERSUBDATAPROC)( GLenum, ptrdiff_t, ptrdiff_t, const GLvoid* );

#define BUFFER_OFFSET( x ) (((char*)0) + x)

class Game :
	public Limbus::OpenglWindow::EventHandler,
	public Limbus::Keyboard::EventHandler,
	public Limbus::Mouse::EventHandler
{
	struct Vertex
	{
		GLfloat position[2];
		GLfloat color[4];
	};

	Limbus::OpenglWindow window;

public:
	void run()
	{
		running = true;
		
		window.setCaption( "OpenGL window" );
		window.setWidth( 640 );
		window.setHeight( 480 );
		window.addEventHandler( this );
		window.create();

		Limbus::Keyboard keyboard;
		keyboard.addEventHandler( this );
		window.addInputDevice(&keyboard);
		
		Limbus::Mouse mouse;
		mouse.addEventHandler( this );
		window.addInputDevice(&mouse);

		glClearColor( 0.5f, 0.5f, 1.0f, 1.0f );

		Vertex vertices[3];
		memset( vertices, 0, sizeof vertices );
		vertices[0].position[0] = -.5f;
		vertices[0].position[1] = -.5f;
		vertices[1].position[0] = .5f;
		vertices[1].position[1] = -.5f;
		vertices[2].position[0] = 0.0f;
		vertices[2].position[1] = .5f;

		vertices[0].color[0] = 1.0f;
		vertices[1].color[2] = 1.0f;
		vertices[2].color[1] = 1.0f;

		GLGENBUFFERSPROC glGenBuffers = (GLGENBUFFERSPROC)glGetProcAddress( "glGenBuffers" );
		GLDELETEBUFFERSPROC glDeleteBuffers = (GLDELETEBUFFERSPROC)glGetProcAddress( "glDeleteBuffers" );
		GLBINDBUFFERPROC glBindBuffer = (GLBINDBUFFERPROC)glGetProcAddress( "glBindBuffer" );
		GLBUFFERDATAPROC glBufferData = (GLBUFFERDATAPROC)glGetProcAddress( "glBufferData" );
		GLBUFFERSUBDATAPROC glBufferSubData = (GLBUFFERSUBDATAPROC)glGetProcAddress( "glBufferSubData" );

		GLuint buffer;
		glGenBuffers( 1, &buffer );
		glBindBuffer( GL_ARRAY_BUFFER, buffer );
		glBufferData( GL_ARRAY_BUFFER, sizeof vertices, NULL, GL_STATIC_DRAW );
		glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof vertices, vertices );

		glEnableClientState( GL_COLOR_ARRAY );
		glEnableClientState( GL_VERTEX_ARRAY );

		while (running)
		{
			window.pollEvents();
			keyboard.pollEvents();
			mouse.pollEvents();

			glClear( GL_COLOR_BUFFER_BIT );

			glColorPointer( 4, GL_FLOAT, sizeof( Vertex ), BUFFER_OFFSET( 8 ) );
			glVertexPointer( 2, GL_FLOAT, sizeof( Vertex ), BUFFER_OFFSET( 0 ) );

			glDrawArrays( GL_TRIANGLES, 0, sizeof vertices / sizeof vertices[0] );

			window.swapBuffers();
		}

		glDeleteBuffers( 1, &buffer );
	}

private:
	void onClose( Limbus::OpenglWindow& sender )
	{
		running = false;
	}

	void onKeyEvent( Limbus::Keyboard& sender, int key, bool pressed )
	{
		if (key == LBKeyEscape && pressed)
			running = false;
	}
	
	void onMotionEvent( Limbus::Mouse& sender, int x, int y )
	{
		std::cout << " " << x << " " << y << std::endl;
	}

	bool running;
};

int main( int argc, char** argv )
{
	Game game;
	game.run();
	return 0;
}

