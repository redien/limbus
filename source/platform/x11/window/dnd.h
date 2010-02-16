
/*
          Copyright Jesper Oskarsson 2009 - 2010.
 Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

static void dnd_setup( X11Window* window, X11WindowData* window_data )
{
    window_data->selection_atom = XInternAtom( window->display, "PRIMARY", False );
    window_data->XdndEnter = XInternAtom( window->display, "XdndEnter", False );
    window_data->XdndPosition = XInternAtom( window->display, "XdndPosition", False );
    window_data->XdndStatus = XInternAtom( window->display, "XdndStatus", False );
    window_data->XdndDrop = XInternAtom( window->display, "XdndDrop", False );
    window_data->XdndActionCopy = XInternAtom( window->display, "XdndActionCopy", False );
    window_data->XdndFinished = XInternAtom( window->display, "XdndFinished", False );
    window_data->XdndSelection = XInternAtom( window->display, "XdndSelection", False );
    
    {
        Atom XdndAware = XInternAtom( window->display, "XdndAware", False );
        unsigned int version = 5;
        
        XChangeProperty( window->display,
                         window->window,
                         XdndAware,
                         XA_ATOM,
                         32,
                         PropModeReplace,
                         (unsigned char*)&version, 1 );
    }
    
    window_data->dnd_target_type = None;
    window_data->dnd_source_window = None;
    window_data->dnd_version = 0;
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

static void dnd_handle_message( X11Window* window,
                                X11WindowData* window_data,
                                XEvent event )
{
    if (event.type == ClientMessage)
    {
        if (event.xclient.message_type == window_data->XdndEnter)
        {
            window_data->dnd_version = event.xclient.data.l[1] >> 24;
            window_data->dnd_target_type = XA_STRING;
        }
        else if (event.xclient.message_type == window_data->XdndPosition)
        {
            XClientMessageEvent message;
            memset( &message, sizeof message, 0 );
            message.type = ClientMessage;
            message.display = event.xclient.display;
            message.window = event.xclient.data.l[0];
            message.message_type = window_data->XdndStatus;
            message.format = 32;
            message.data.l[0] = window->window;
            message.data.l[1] = (window_data->dnd_target_type != None);
            message.data.l[2] = message.data.l[3] = 0;
            message.data.l[4] = window_data->XdndActionCopy;
            
            window_data->dnd_x = (event.xclient.data.l[2] >> 16) - window_data->x;
            window_data->dnd_y = (event.xclient.data.l[2] & 0xFFFF) - window_data->y;
            
            XSendEvent( window->display,
                        event.xclient.data.l[0],
                        False,
                        NoEventMask,
                        (XEvent*)&message );
            XFlush( window->display );
        }
        else if (event.xclient.message_type == window_data->XdndDrop)
        {
            if (window_data->dnd_target_type == None)
            {
                XClientMessageEvent message;
                memset( &message, sizeof message, 0 );
                message.type = ClientMessage;
                message.display = event.xclient.display;
                message.window = event.xclient.data.l[0];
                message.message_type = window_data->XdndFinished;
                message.format = 32;
                message.data.l[0] = window->window;
                message.data.l[1] = 0;
                message.data.l[2] = None;
                
                XSendEvent( window->display,
                            event.xclient.data.l[0],
                            False,
                            NoEventMask,
                            (XEvent*)&message );
            }
            else
            {
                window_data->dnd_source_window = event.xclient.data.l[0];
                if (window_data->dnd_version >= 1)
                    XConvertSelection( window->display,
                                       window_data->XdndSelection,
                                       window_data->dnd_target_type,
                                       window_data->selection_atom,
                                       window->window,
                                       event.xclient.data.l[2] );
                else
                    XConvertSelection( window->display,
                                       window_data->XdndSelection,
                                       window_data->dnd_target_type,
                                       window_data->selection_atom,
                                       window->window,
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
                XGetWindowProperty( window->display,
                                    window->window,
                                    window_data->selection_atom,
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
            
            if (event.xselection.target == window_data->dnd_target_type)
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
                    event.x = window_data->dnd_x;
                    event.y = window_data->dnd_y;

                    vector_clear( &window_data->files );
                    do
                    {
                        translate_uri( tmp, uri );
                        vector_push_back( &window_data->files, tmp );
                        event.files++;
                    } while ((uri = find_next_line( uri )));
                    
                    vector_push_back( &window_data->events, &event );
                }


                memset( &message, sizeof message, 0 );
                message.type = ClientMessage;
                message.display = window->display;
                message.window = window_data->dnd_source_window;
                message.message_type = window_data->XdndFinished;
                message.format = 32;
                message.data.l[0] = window->window;
                message.data.l[1] = 1;
                message.data.l[2] = window_data->XdndActionCopy;
                
                XSendEvent( window->display,
                            window_data->dnd_source_window,
                            False,
                            NoEventMask,
                            (XEvent*)&message );
                
                XSync( window->display, False );
            }
            
            XFree( data );
        }
    }
}

