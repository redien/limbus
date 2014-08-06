
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include "../../../util/vector.h"
#include "../x11_window.h"
#include "../x11_input_device.h"

#include <limbus/tablet.h>

#include <assert.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/extensions/XInput.h>

typedef struct TabletEventTag
{
	int type;
	int button;
	int pressure;
	int x, y;
} TabletEvent;

typedef struct X11TabletTag
{
	X11InputDevice base;

	Vector events;
	TabletEvent event;

	int stylus_id;
	XDevice* stylus_device;

	int context_width,
		context_height;
	
	float context_scale_x,
		  context_scale_y;

	XDeviceInfo* list;

	int button_press,
		button_release,
		proximity_in,
		proximity_out,
		valuator_motion;

	int added_to_window;
} X11Tablet;

#define CAST_TABLET()\
	X11Tablet* tablet = (X11Tablet*)t;\
	assert( tablet != NULL );

static void construct( void* t )
{
	int i,
		ndevices,
		stylus_index,
		event_class_count = 0;

	Display* display;
	Window window;

	XDeviceInfo* list;
	XDevice* stylus_device;

	XEventClass event_classes[122];

	CAST_TABLET()

	display = tablet->base.display;
	window = tablet->base.window;

	tablet->list = list = (XDeviceInfo*)XListInputDevices( display, &ndevices );

	tablet->stylus_device = stylus_device = NULL;

	for (i = 0; i < ndevices; ++i)
	{
		void* ptr = list[i].inputclassinfo;
		int j, found = 0;
		for (j = 0; j < list[i].num_classes; ++j)
		{
			XAnyClassPtr any_ptr = (XAnyClassPtr)ptr;
			if (any_ptr->class == ValuatorClass)
			{
				XValuatorInfo* valuator_info = (XValuatorInfo*)any_ptr;
				if (valuator_info->num_axes > 2)
				{
					stylus_index = i;
					tablet->stylus_id = list[i].id;
					stylus_device = tablet->stylus_device = XOpenDevice( display, tablet->stylus_id );
					assert( stylus_device );
					found = 1; /* TODO: Handle more than just the stylus */
				}
			}

			ptr = (char*)ptr + any_ptr->length;
		}

		if (found)
			break;
	}

	if (stylus_device)
	{
		void* ptr = list[stylus_index].inputclassinfo;
		tablet->context_scale_x = tablet->context_scale_y = 1;

		for (i = 0; i < list[stylus_index].num_classes; ++i)
		{
			XAnyClassPtr any_ptr = (XAnyClassPtr)ptr;

			if (any_ptr->class == ValuatorClass)
			{
				XValuatorInfo* valuator_info = (XValuatorInfo*)any_ptr;
				XAxisInfo* axes = valuator_info->axes;

				assert( axes[0].min_value == 0 );
				tablet->context_width = axes[0].max_value;

				assert( axes[1].min_value == 0 );
				tablet->context_height = axes[1].max_value;
			}
			
			ptr = (char*)ptr + any_ptr->length;
		}

		for (i = 0; i < stylus_device->num_classes; i++)
		{
			XInputClassInfo* info = &stylus_device->classes[i];

			if (info->input_class == ButtonClass)
			{
				DeviceButtonPress( stylus_device, tablet->button_press, event_classes[event_class_count++] );
				DeviceButtonRelease( stylus_device, tablet->button_release, event_classes[event_class_count++] );
			}
			else if (info->input_class == ProximityClass)
			{
				ProximityIn( stylus_device, tablet->proximity_in, event_classes[event_class_count++] );
				ProximityOut( stylus_device, tablet->proximity_out, event_classes[event_class_count++] );
			}
			else if (info->input_class == ValuatorClass)
			{
				DeviceMotionNotify( stylus_device, tablet->valuator_motion, event_classes[event_class_count++] );
			}
		
			assert( event_class_count < 120 );
		}
	}

	XSelectExtensionEvent( display, tablet->base.root_window, event_classes, event_class_count );

	{
		int count1, count2;
		XEventClass *list1, *list2;
		
		XGetSelectedExtensionEvents( display, window, &count1, &list1, &count2, &list2 );
	}

	tablet->added_to_window = 1;
}

void handle_x11_event( void* t, XEvent event )
{
	TabletEvent new_event;
	CAST_TABLET()

	if (event.type == tablet->button_press)
	{
		XDeviceButtonEvent* button_event = (XDeviceButtonEvent*)&event;
		new_event.type = LBTabletEventButtonPress;
		new_event.button = button_event->button;
		vector_push_back( &tablet->events, &new_event );
	}
	else if (event.type == tablet->button_release)
	{
		XDeviceButtonEvent* button_event = (XDeviceButtonEvent*)&event;
		new_event.type = LBTabletEventButtonRelease;
		new_event.button = button_event->button;
		vector_push_back( &tablet->events, &new_event );
	}
	else if (event.type == tablet->proximity_in)
	{
		new_event.type = LBTabletEventProximityIn;
		vector_push_back( &tablet->events, &new_event );
	}
	else if (event.type == tablet->proximity_out)
	{
		new_event.type = LBTabletEventProximityOut;
		vector_push_back( &tablet->events, &new_event );
	}
	else if (event.type == tablet->valuator_motion)
	{
		XDeviceMotionEvent* motion_event = (XDeviceMotionEvent*)&event;
		new_event.type = LBTabletEventMotion;
		new_event.x = motion_event->axis_data[0];
		new_event.y = motion_event->axis_data[1];
		new_event.pressure = motion_event->axis_data[2];
		vector_push_back( &tablet->events, &new_event );
	}
}

void* lb_tablet_construct()
{
	X11Tablet* tablet = (X11Tablet*)malloc( sizeof( X11Tablet ) );
	vector_construct( &tablet->events, sizeof( TabletEvent ) );

	tablet->base.construct = &construct;
	tablet->base.handle_x11_event = &handle_x11_event;

	tablet->added_to_window = 0;

	return tablet;
}

void lb_tablet_destruct( void* t )
{
	CAST_TABLET()

	vector_destruct( &tablet->events );

	if (tablet->added_to_window)
	{
		if (tablet->stylus_device)
			XCloseDevice( tablet->base.display, tablet->stylus_device );
		XFreeDeviceList( tablet->list );
	}

	free( tablet );
}

int lb_tablet_constructed( void* tablet )
{
	return (tablet != NULL) ? 1 : 0;
}

void lb_tablet_set_context_width( void* t, int width )
{
	CAST_TABLET()
	tablet->context_scale_x = (float)width / tablet->context_width;
}

void lb_tablet_set_context_height( void* t, int height )
{
	CAST_TABLET()
	tablet->context_scale_y = (float)height / tablet->context_height;
}

int lb_tablet_get_context_width( void* t )
{
	CAST_TABLET()
	return tablet->context_width * tablet->context_scale_x + .5;
}

int lb_tablet_get_context_height( void* t )
{
	CAST_TABLET()
	return tablet->context_height * tablet->context_scale_y + .5;
}

int lb_tablet_next_event( void* t )
{
	TabletEvent* iter;
	CAST_TABLET()

	iter = (TabletEvent*)vector_next( &tablet->events, NULL );
	if (iter != NULL)
	{
		tablet->event = *iter;
		vector_erase( &tablet->events, iter );
		return 1;
	}
}

enum LBTabletEvent lb_tablet_get_event_type( void* t )
{
	CAST_TABLET()
	return tablet->event.type;
}

int lb_tablet_get_event_button( void* t )
{
	CAST_TABLET()
	return tablet->event.button;
}

int lb_tablet_get_event_pressure( void* t )
{
	CAST_TABLET()
	return tablet->event.pressure;
}

int lb_tablet_get_event_x( void* t )
{
	CAST_TABLET()
	return tablet->event.x * tablet->context_scale_x;
}

int lb_tablet_get_event_y( void* t )
{
	CAST_TABLET()
	return tablet->event.y * tablet->context_scale_y;
}

