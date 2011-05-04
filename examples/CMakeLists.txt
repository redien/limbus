
#          Copyright Jesper Oskarsson 2009 - 2010.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

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
			${SHARED_LIBRARIES}
			limbus
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
				${SHARED_LIBRARIES}
				limbus_cpp
				limbus
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
	
	if (ENABLE_AUDIO_STREAM_SUPPORT)
		add_c_example( audio_stream_example )
	endif (ENABLE_AUDIO_STREAM_SUPPORT)

	if(ENABLE_MOUSE_SUPPORT AND ENABLE_KEYBOARD_SUPPORT)
		add_c_example( opengl_window )
	
		add_cpp_example( opengl_window )
	endif(ENABLE_MOUSE_SUPPORT AND ENABLE_KEYBOARD_SUPPORT)
endif(COMPILE_EXAMPLES)

