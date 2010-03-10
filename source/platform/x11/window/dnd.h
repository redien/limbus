
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

static void dnd_setup( X11WindowImpl* window )
{
    window->selection_atom = XInternAtom( window->base.display, "PRIMARY", False );
    window->XdndEnter =      XInternAtom( window->base.display, "XdndEnter", False );
    window->XdndPosition =   XInternAtom( window->base.display, "XdndPosition", False );
    window->XdndStatus =     XInternAtom( window->base.display, "XdndStatus", False );
    window->XdndDrop =       XInternAtom( window->base.display, "XdndDrop", False );
    window->XdndActionCopy = XInternAtom( window->base.display, "XdndActionCopy", False );
    window->XdndFinished =   XInternAtom( window->base.display, "XdndFinished", False );
    window->XdndSelection =  XInternAtom( window->base.display, "XdndSelection", False );

    {
        Atom XdndAware = XInternAtom( window->base.display, "XdndAware", False );
        unsigned int version = 5;
        
        XChangeProperty( window->base.display,
                         window->base.window,
                         XdndAware,
                         XA_ATOM,
                         32,
                         PropModeReplace,
                         (unsigned char*)&version, 1 );
    }
    
    window->dnd_target_type = None;
    window->dnd_source_window = None;
    window->dnd_version = 0;
}

static char* find_next_line( char* str )
{
    while (*str != '\n'
        && *str != '\r'
        && *str != '\0')
        str++;

    while (*str == '\n' || *str == '\r')
        str++;
    
    if (*str == '\0')
        return NULL;
    else
        return str;
}

#define DND_MAX_PATH_SIZE 256

static void translate_uri( char* dest, char* str )
{
    size_t size = 0;
    str += 7; /* jump over 'file://' */
    while (*str != '\n'
        && *str != '\r'
        && *str != '\0'
        && size < DND_MAX_PATH_SIZE - 1)
    {
        if (*str == '%')
        {
            unsigned int c;
            char tmp[3];
            tmp[0] = *(str + 1);
            tmp[1] = *(str + 2);
            tmp[2] = 0;
            
            sscanf( tmp, "%x", &c );
            
            dest[size] = c;
            str += 2;
        }
        else
        {
            dest[size] = *str;
        }
        
        size++;
        str++;
    }
    
    dest[size] = '\0';
}

static void dnd_handle_event( X11WindowImpl* window,
                              XEvent event )
{
    if (event.type == ClientMessage)
    {
        if (event.xclient.message_type == window->XdndEnter)
        {
            window->dnd_version = event.xclient.data.l[1] >> 24;
            window->dnd_target_type = XA_STRING;
        }
        else if (event.xclient.message_type == window->XdndPosition)
        {
            XClientMessageEvent message;
            memset( &message, sizeof message, 0 );
            message.type = ClientMessage;
            message.display = event.xclient.display;
            message.window = event.xclient.data.l[0];
            message.message_type = window->XdndStatus;
            message.format = 32;
            message.data.l[0] = window->base.window;
            message.data.l[1] = (window->dnd_target_type != None);
            message.data.l[2] = message.data.l[3] = 0;
            message.data.l[4] = window->XdndActionCopy;
            
            window->dnd_x = (event.xclient.data.l[2] >> 16) - window->x;
            window->dnd_y = (event.xclient.data.l[2] & 0xFFFF) - window->y;
            
            XSendEvent( window->base.display,
                        event.xclient.data.l[0],
                        False,
                        NoEventMask,
                        (XEvent*)&message );
            XFlush( window->base.display );
        }
        else if (event.xclient.message_type == window->XdndDrop)
        {
            if (window->dnd_target_type == None)
            {
                XClientMessageEvent message;
                memset( &message, sizeof message, 0 );
                message.type = ClientMessage;
                message.display = event.xclient.display;
                message.window = event.xclient.data.l[0];
                message.message_type = window->XdndFinished;
                message.format = 32;
                message.data.l[0] = window->base.window;
                message.data.l[1] = 0;
                message.data.l[2] = None;
                
                XSendEvent( window->base.display,
                            event.xclient.data.l[0],
                            False,
                            NoEventMask,
                            (XEvent*)&message );
            }
            else
            {
                window->dnd_source_window = event.xclient.data.l[0];
                if (window->dnd_version >= 1)
                    XConvertSelection( window->base.display,
                                       window->XdndSelection,
                                       window->dnd_target_type,
                                       window->selection_atom,
                                       window->base.window,
                                       event.xclient.data.l[2] );
                else
                    XConvertSelection( window->base.display,
                                       window->XdndSelection,
                                       window->dnd_target_type,
                                       window->selection_atom,
                                       window->base.window,
                                       CurrentTime );
            }
        }
    }
    
    if (event.type == SelectionNotify)
    {
        if (event.xselection.property != None)
        {
            Atom actual_type;
            int actual_format;
            unsigned long items, bytes_left, bytes_read;
            unsigned char* data = NULL;

            bytes_read = 1024;
            while (!data)
            {
                XGetWindowProperty( window->base.display,
                                    window->base.window,
                                    window->selection_atom,
                                    0,
                                    bytes_read,
                                    False,
                                    AnyPropertyType,
                                    &actual_type,
                                    &actual_format,
                                    &items,
                                    &bytes_left,
                                    &data );
                
                if (bytes_left > 0)
                {
                    XFree( data );
                    data = NULL;
                }

                bytes_read *= 2;
            }
            
            if (event.xselection.target == window->dnd_target_type)
            {
                XClientMessageEvent message;
                WindowEvent event;
                char* uri;
                char tmp[DND_MAX_PATH_SIZE];

                uri = (char*)data;
                
                /* As we're only using STRING as our type
                   we need to check for file URIs */
                if (strlen( uri ) > 7 &&
                    strncmp( uri, "file://", 7 ) == 0)
                {
                    event.type = LBWindowEventFileDrop;
                    event.files = 0;
                    event.x = window->dnd_x;
                    event.y = window->dnd_y;

                    vector_clear( &window->files );
                    do
                    {
                        translate_uri( tmp, uri );
                        vector_push_back( &window->files, tmp );
                        event.files++;
                    } while ((uri = find_next_line( uri )));
                    
                    vector_push_back( &window->events, &event );
                }

                memset( &message, sizeof message, 0 );
                message.type = ClientMessage;
                message.display = window->base.display;
                message.window = window->dnd_source_window;
                message.message_type = window->XdndFinished;
                message.format = 32;
                message.data.l[0] = window->base.window;
                message.data.l[1] = 1;
                message.data.l[2] = window->XdndActionCopy;
                
                XSendEvent( window->base.display,
                            window->dnd_source_window,
                            False,
                            NoEventMask,
                            (XEvent*)&message );
                
                XSync( window->base.display, False );
            }
            
            XFree( data );
        }
    }
}

