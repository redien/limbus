
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include <limbus/window.h>
#include <limbus/opengl_context.h>
#include <limbus/opengl.h>

#include <stdio.h>
#include <string.h>

typedef struct VertexTag
{
    GLfloat position[2];
    GLfloat color[4];
} Vertex;

int main( int argc, char** argv )
{
	LBWindow window = NULL;
	LBOpenglContext context = NULL;
	int running;

	window = lb_window_construct();
	if (!lb_window_constructed( window ))
		return -1;

	lb_window_set_width( window, 640 );
	lb_window_set_height( window, 480 );
	lb_window_set_caption( window, "Window name" );

	context = lb_opengl_context_construct( window, LBOpenglContextCreateNew );
	if (!lb_opengl_context_constructed( context ))
	{
		lb_window_destruct( window );
		return -1;
	}
	lb_opengl_context_bind( context,  LBOpenglContextDefaultPixelformat );
	lb_opengl_context_make_current( context );

	glClearColor( 0.5f, 0.5f, 1.0f, 1.0f );
	
	#define BUFFER_OFFSET( x ) (((char*)0) + x)

	Vertex vertices[4];
	memset( vertices, 0, sizeof vertices );
	vertices[0].position[0] = -.1;
	vertices[0].position[1] = -.1;
	vertices[1].position[0] = .1;
	vertices[1].position[1] = -.1;
	vertices[2].position[0] = .1;
	vertices[2].position[1] = .1;
	vertices[3].position[0] = -.1;
	vertices[3].position[1] = .1;

	vertices[0].color[0] = 1;
	vertices[3].color[1] = 1;
	vertices[1].color[2] = 1;
	vertices[2].color[3] = 1;

	GLuint buffer;
	glGenBuffers( 1, &buffer );
	glBindBuffer( GL_ARRAY_BUFFER, buffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof vertices, NULL, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof vertices, vertices );

	glEnableClientState( GL_COLOR_ARRAY );
	glEnableClientState( GL_VERTEX_ARRAY );

	running = 1;
	while (running)
	{
		while (lb_window_next_event( window ))
		{
			int type = lb_window_get_event_type( window );
			if (type == LBWindowEventClose)
				running = 0;
		}

		glClear( GL_COLOR_BUFFER_BIT );

        glColorPointer( 4, GL_FLOAT, sizeof( Vertex ), BUFFER_OFFSET( 8 ) );
        glVertexPointer( 2, GL_FLOAT, sizeof( Vertex ), BUFFER_OFFSET( 0 ) );

        glDrawArrays( GL_QUADS, 0, sizeof vertices / sizeof vertices[0] );

		lb_opengl_context_swap_buffers( context );
	}

	lb_opengl_context_destruct( context );
	lb_window_destruct( window );
	return 0;
}
