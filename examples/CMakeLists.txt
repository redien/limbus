
#          Copyright Jesper Oskarsson 2009 - 2010.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

macro( add_placebo_example name )
	add_executable(
		${name}
			examples/${name}.c
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
endmacro( add_placebo_example )

set(
	COMPILE_EXAMPLES
	ON
	CACHE BOOL "Compile examples."
)
if(COMPILE_EXAMPLES)
	if(ENABLE_MOUSE_SUPPORT AND ENABLE_KEYBOARD_SUPPORT)
		add_placebo_example( opengl_window )
	endif(ENABLE_MOUSE_SUPPORT AND ENABLE_KEYBOARD_SUPPORT)

	add_placebo_example( screen_example )
endif(COMPILE_EXAMPLES)

