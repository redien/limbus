
# Limbus - A cross-platform multimedia library.
# Written in 2014 by Jesper Oskarsson jesosk@gmail.com
#
# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain worldwide.
# This software is distributed without any warranty.
#
# You should have received a copy of the CC0 Public Domain Dedication along with this software.
# If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

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

