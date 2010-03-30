
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef LB_SOCKET_H
#define LB_SOCKET_H
#ifdef __cplusplus
extern "C"
{
#endif

/* Experimental */

enum LBSocketType
{
    LBSocketTypeDatagram,
    LBSocketTypeStream,
    LBSocketTypeRaw
};

void* lb_socket_construct( enum LBSocketType type );
void lb_socket_destruct( void* socket );
int lb_socket_constructed( void* socket );

int lb_socket_bind( void* socket, int port );
int lb_socket_listen( void* socket, int max_connections );
int lb_socket_connect( void* socket, char* address, int port );
void* lb_socket_accept( void* socket );

int lb_socket_send_string( void* socket, char* string );
const char* lb_socket_recv_string( void* socket, int len );

int lb_socket_close( void* socket );
int lb_socket_closed( void* socket );

#ifdef __cplusplus
}
#endif
#endif

