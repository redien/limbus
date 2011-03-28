
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#include <limbus/socket.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct
{
    int socket;
    char* return_string;
    int has_closed;
} UnixSocket;

static UnixSocket* construct()
{
    UnixSocket* sock = (UnixSocket*)malloc( sizeof( UnixSocket) );
    
    if (!sock)
        return NULL;
    
    sock->has_closed = 1;
    sock->return_string = NULL;
    
    return sock;
}

void* lb_socket_construct( enum LBSocketType type )
{
    UnixSocket* sock = construct();
    
    if (!sock)
        return NULL;
    
    sock->socket = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
    if (sock->socket == -1)
    {
        free( sock );
        return NULL;
    }
    
    return sock;
}

#define DEFINE_SOCKET()\
    UnixSocket* sock = (UnixSocket*)s;\
    assert( sock );

void lb_socket_destruct( void* s )
{
    DEFINE_SOCKET()
    
    lb_socket_close( sock );
    
    if (sock->return_string != NULL)
        free( sock->return_string );
    
    free( sock );
}

int lb_socket_constructed( void* sock )
{
    return (sock == NULL) ? 0 : 1;
}

int lb_socket_bind( void* s, int port )
{
    struct sockaddr_in sock_address;
    int result;
    DEFINE_SOCKET()
    
    memset( &sock_address, 0, sizeof sock_address );
    
    sock_address.sin_family = AF_INET;
    sock_address.sin_port = htons( port );
    sock_address.sin_addr.s_addr = INADDR_ANY;

    result = bind( sock->socket,
                   (const struct sockaddr*)&sock_address,
                   sizeof sock_address );
    if (result == -1)
        return 0;
    
    return 1;
}

int lb_socket_listen( void* s, int max_connections )
{
    int result;
    DEFINE_SOCKET()
    
    result = listen( sock->socket, max_connections );
    if (result == -1)
        return 0;

    return 1;
}

void* lb_socket_accept( void* s )
{
    int fd;
    UnixSocket* new_sock;
    DEFINE_SOCKET()

    fd = accept( sock->socket, NULL, NULL );
    new_sock = construct();
    
    if (!new_sock)
    {
        close( fd );
        return NULL;
    }

    new_sock->socket = fd;
    
    return new_sock;
}

int lb_socket_connect( void* s, char* address, int port )
{
    struct sockaddr_in sock_address;
    int result;
    DEFINE_SOCKET()
    
    memset( &sock_address, 0, sizeof sock_address );
    
    sock_address.sin_family = AF_INET;
    sock_address.sin_port = htons( port );
    
    result = inet_pton( AF_INET, address, &sock_address.sin_addr );
    if (result <= 0)
        return 0;

    result = connect( sock->socket,
                      (const struct sockaddr*)&sock_address,
                      sizeof sock_address );
    if (result == -1)
        return 0;
    
    return 1;
}

int lb_socket_send_string( void* s, char* string )
{
    int result;
    DEFINE_SOCKET()
    
    result = send( sock->socket, (const void*)string, strlen( string ), 0 );
    
    return result;
}

const char* lb_socket_recv_string( void* s, int len )
{
    ssize_t result;
    DEFINE_SOCKET()
    
    if (sock->return_string)
        free( sock->return_string );
    
    sock->return_string = (char*)malloc( len + 1 );
    result = recv( sock->socket, sock->return_string, len, 0 );
    
    if (result == -1)
        return "";
    
    if (result == 0)
    {
        sock->has_closed = 1;
        return "";
    }

    sock->return_string[result] = '\0';
    return sock->return_string;
}

int lb_socket_closed( void* s )
{
    DEFINE_SOCKET()
    return sock->has_closed;
}

int lb_socket_close( void* s )
{
    DEFINE_SOCKET()

    shutdown( sock->socket, SHUT_RDWR );
    close( sock->socket );
    
    return 1;
}

