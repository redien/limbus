
#          Copyright Jesper Oskarsson 2009 - 2010.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

include( CTest )
enable_testing()

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/source
)

macro( add_placebo_test name )
	add_test( ${name} ${name}${CMAKE_EXECUTABLE_SUFFIX} )
	add_executable(
		${name}
			tests/${name}.c
	)
	add_dependencies(
		${name}
			limbus
	)
	target_link_libraries(
		${name}
			limbus
			${SHARED_LIBRARIES}
	)
endmacro( add_placebo_test )

add_placebo_test( window_properties )
add_placebo_test( opengl_window_test )
add_placebo_test( filesystem )

