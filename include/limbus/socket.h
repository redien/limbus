
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
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

