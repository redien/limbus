
# Limbus - A cross-platform multimedia library.
# Written in 2014 by Jesper Oskarsson jesosk@gmail.com
#
# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain worldwide.
# This software is distributed without any warranty.
#
# You should have received a copy of the CC0 Public Domain Dedication along with this software.
# If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

macro( add_c_example name )
	add_executable(
		${name}
			examples/c/${name}.c
	)
	set_target_properties(
		${name} PROPERTIES
			RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
	)
	target_link_libraries(
		${name}
			limbus
			${SHARED_LIBRARIES}
	)
endmacro( add_c_example )

macro( add_cpp_example name )
	if( GENERATE_CPP_BINDING )
		add_executable(
			${name}_cpp_example
				examples/cpp/${name}.cpp
		)
		set_target_properties(
			${name}_cpp_example PROPERTIES
				RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
		)
		target_link_libraries(
			${name}_cpp_example
				limbus_cpp
				limbus
				${SHARED_LIBRARIES}
		)
	endif( GENERATE_CPP_BINDING )
endmacro( add_cpp_example )

set(
	COMPILE_EXAMPLES
	ON
	CACHE BOOL "Compile examples."
)
if(COMPILE_EXAMPLES)
	add_c_example( screen_example )
    add_c_example( opengl_example )
    add_c_example( timing_example )

	if (ENABLE_TABLET_SUPPORT)
		add_c_example( tablet_example )
	endif (ENABLE_TABLET_SUPPORT)

	if (ENABLE_AUDIO_STREAM_SUPPORT)
		add_c_example( audio_stream_example )
	endif (ENABLE_AUDIO_STREAM_SUPPORT)

	if(ENABLE_MOUSE_SUPPORT AND ENABLE_KEYBOARD_SUPPORT)
		add_c_example( opengl_window )
		
		if (GENERATE_CPP_BINDING)
			add_cpp_example( opengl_window )
		endif(GENERATE_CPP_BINDING)
	endif(ENABLE_MOUSE_SUPPORT AND ENABLE_KEYBOARD_SUPPORT)
endif(COMPILE_EXAMPLES)
