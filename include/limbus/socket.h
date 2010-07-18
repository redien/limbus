
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

typedef void* LBSocket;

enum LBSocketType
{
    LBSocketTypeDatagram,
    LBSocketTypeStream,
    LBSocketTypeRaw
};
typedef enum LBSocketType LBSocketType;

LBSocket lb_socket_construct( LBSocketType type );
void lb_socket_destruct( LBSocket socket );
int lb_socket_constructed( LBSocket socket );

int lb_socket_bind( LBSocket socket, int port );
int lb_socket_listen( LBSocket socket, int max_connections );
int lb_socket_connect( LBSocket socket, char* address, int port );
LBSocket lb_socket_accept( LBSocket socket );

int lb_socket_send_string( LBSocket socket, char* string );
const char* lb_socket_recv_string( LBSocket socket, int len );

int lb_socket_close( LBSocket socket );
int lb_socket_closed( LBSocket socket );

#ifdef __cplusplus
}
#endif
#endif

