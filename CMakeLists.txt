
# Limbus - A cross-platform multimedia library.
# Written in 2014 by Jesper Oskarsson jesosk@gmail.com
#
# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain worldwide.
# This software is distributed without any warranty.
#
# You should have received a copy of the CC0 Public Domain Dedication along with this software.
# If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

cmake_minimum_required (VERSION 2.6)
project (limbus)

if(CMAKE_SYSTEM_NAME STREQUAL "Haiku")
	set(HAIKU ON)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	set(LINUX ON)
elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
	set(FREEBSD ON)
endif(CMAKE_SYSTEM_NAME STREQUAL "Haiku")

if(CMAKE_C_COMPILER MATCHES gcc)
	set(GCC ON)
	set(
		ANSI_STRICT
		OFF
		CACHE BOOL "Compile source in strict ANSI C90 mode."
	)
endif(CMAKE_C_COMPILER MATCHES gcc)

set( INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include )
set( INCLUDE_SUB_NAME limbus )
set( INCLUDE_SUB_DIR ${INCLUDE_DIR}/${INCLUDE_SUB_NAME} )
include_directories( ${INCLUDE_DIR} )

set( BINDINGS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bindings )

set( BIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin )
set( LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib )
set( FINAL_BIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_CFG_INTDIR} )
set( FINAL_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/${CMAKE_CFG_INTDIR} )

set( GENERATED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/generated )
set( LUA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/generated/lua )
set( PYTHON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/generated/python )
set( RUBY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/generated/ruby )
set( CSHARP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/generated/csharp )
set( GO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/generated/go )
set( CPP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/generated/cpp )

set( TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests )

set(
	LUA_VERSION
	"5.1.4"
	CACHE STRING "Lua version"
)

set( LUA_DEP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/lua-${LUA_VERSION} )

set( LUA_SRC_PATH ${LUA_DEP_DIR}/src )
set( LUA_INCLUDE_DIR ${LUA_SRC_PATH} )
set( LUA_EXE ${FINAL_BIN_DIR}/lua )

set( CBIND_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/cbind/ )

set( GENERATED_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/generated/config.lua )

##
#	Core dependencies
##
set(
	SOURCE_FILES
		source/util/unicode.c
		source/util/vector.c

		source/generic/static_buffer.c
)

set(
	STATIC_LIB_SOURCE_FILES
)

set(
	BIND_HEADERS
		${INCLUDE_SUB_NAME}/timing.h
		${INCLUDE_SUB_NAME}/window.h
		${INCLUDE_SUB_NAME}/display.h
		${INCLUDE_SUB_NAME}/opengl_context.h
		${INCLUDE_SUB_NAME}/static_buffer.h
)

set(
	BIND_SOURCES
		timer.cdef
		input_device.cdef
		input_dispatcher.cdef
		display.cdef
		window.cdef
		opengl_context.cdef
		opengl_window.cdef
)

set(
	GENERATED_CPP_SOURCES
		${CPP_DIR}/Timer.cpp
		${CPP_DIR}/Display.cpp
		${CPP_DIR}/Window.cpp
		${CPP_DIR}/OpenglContext.cpp
		${CPP_DIR}/OpenglWindow.cpp
)

include( dependencies/CMakeLists.txt )

##
#	Optional dependencies
##
set(
	ENABLE_FILESYSTEM_SUPPORT
	ON
	CACHE BOOL "Enable support for querying the filesystem."
)
if(ENABLE_FILESYSTEM_SUPPORT)
	set( BIND_HEADERS ${BIND_HEADERS} ${INCLUDE_SUB_NAME}/filesystem.h )
	set(
		BIND_SOURCES
			${BIND_SOURCES}
			filesystem.cdef
	)
	set(
		GENERATED_CPP_SOURCES
			${GENERATED_CPP_SOURCES}
			${CPP_DIR}/Filesystem.cpp
	)
endif(ENABLE_FILESYSTEM_SUPPORT)

if (LINUX OR WIN32)
    set(
	    ENABLE_JOYSTICK_SUPPORT
	    ON
	    CACHE BOOL "Enable support for joysticks and gamepads."
    )
    if(ENABLE_JOYSTICK_SUPPORT)
	    set( BIND_HEADERS ${BIND_HEADERS} ${INCLUDE_SUB_NAME}/joystick.h )
		set(
			BIND_SOURCES
				${BIND_SOURCES}
				joystick.cdef
		)
		set(
			GENERATED_CPP_SOURCES
				${GENERATED_CPP_SOURCES}
				${CPP_DIR}/Joystick.cpp
		)
    endif(ENABLE_JOYSTICK_SUPPORT)
endif (LINUX OR WIN32)

set(
	ENABLE_MOUSE_SUPPORT
	ON
	CACHE BOOL "Enable support for standard mice-like pointing devices."
)
if(ENABLE_MOUSE_SUPPORT)
	set( BIND_HEADERS ${BIND_HEADERS} ${INCLUDE_SUB_NAME}/mouse.h )
	set(
		BIND_SOURCES
			${BIND_SOURCES}
			mouse.cdef
	)
	set(
		GENERATED_CPP_SOURCES
			${GENERATED_CPP_SOURCES}
			${CPP_DIR}/Mouse.cpp
	)
endif(ENABLE_MOUSE_SUPPORT)

set(
	ENABLE_KEYBOARD_SUPPORT
	ON
	CACHE BOOL "Enable support for keyboard devices."
)
if(ENABLE_KEYBOARD_SUPPORT)
	set( BIND_HEADERS ${BIND_HEADERS} ${INCLUDE_SUB_NAME}/keyboard.h )
	set(
		BIND_SOURCES
			${BIND_SOURCES}
			keyboard.cdef
	)
	set(
		GENERATED_CPP_SOURCES
			${GENERATED_CPP_SOURCES}
			${CPP_DIR}/Keyboard.cpp
	)
endif(ENABLE_KEYBOARD_SUPPORT)

set(
	ENABLE_AUDIO_STREAM_SUPPORT
	OFF
	CACHE BOOL "Enable support for audio streams."
)
if(ENABLE_AUDIO_STREAM_SUPPORT)
	set( BIND_HEADERS ${BIND_HEADERS} ${INCLUDE_SUB_NAME}/audio_stream.h )
	set(
		BIND_SOURCES
			${BIND_SOURCES}
			audio_stream.cdef
	)
	set(
		GENERATED_CPP_SOURCES
			${GENERATED_CPP_SOURCES}
			${CPP_DIR}/AudioStream.cpp
	)
endif(ENABLE_AUDIO_STREAM_SUPPORT)


##
#	Platform specific files
##
if(WIN32)
	set(
		SHARED_LIBRARIES
			opengl32
			winmm
			gdi32
	)

	set(
		SOURCE_FILES
			${SOURCE_FILES}
			source/platform/winapi/opengl_context/opengl_context_wgl.c
			source/platform/winapi/custom_context/custom_context_winapi.c
			source/platform/winapi/screen/screen.c
			source/platform/winapi/timing/timing_windows.c
			source/platform/winapi/window/win_window.c
	)

	set(
		REDIRECT_WINMAIN
		ON
		CACHE BOOL "Creates a WinMain() implementation that calls main()"
	)
	if (REDIRECT_WINMAIN)
		set( STATIC_LIB_SOURCE_FILES ${STATIC_LIB_SOURCE_FILES} source/platform/winapi/winapi_entrypoint.c )
	endif (REDIRECT_WINMAIN)

	if(ENABLE_FILESYSTEM_SUPPORT)
		set( SOURCE_FILES ${SOURCE_FILES} source/platform/winapi/filesystem/filesystem_winapi.c )
	endif(ENABLE_FILESYSTEM_SUPPORT)

	if(ENABLE_JOYSTICK_SUPPORT)
		set( SOURCE_FILES ${SOURCE_FILES} source/platform/winapi/joystick/joystick_winmm.c )
	endif(ENABLE_JOYSTICK_SUPPORT)

	if(ENABLE_MOUSE_SUPPORT)
		set( SOURCE_FILES ${SOURCE_FILES} source/platform/winapi/mouse/win_mouse.c )
	endif(ENABLE_MOUSE_SUPPORT)

	if(ENABLE_KEYBOARD_SUPPORT)
		set( SOURCE_FILES ${SOURCE_FILES} source/platform/winapi/keyboard/winapi_keyboard.c )
	endif(ENABLE_KEYBOARD_SUPPORT)

	if(ENABLE_AUDIO_STREAM_SUPPORT)
		set( SOURCE_FILES ${SOURCE_FILES} source/platform/winapi/audio_stream/winapi_audio_stream.c )
	endif(ENABLE_AUDIO_STREAM_SUPPORT)

elseif (APPLE)
	
    set(
        CMAKE_MACOSX_RPATH
        ON
    )
    
    FIND_LIBRARY(OPENGL_LIBRARY OpenGL)
    FIND_LIBRARY(FORCEFEEDBACK_LIBRARY ForceFeedback)
    FIND_LIBRARY(COCOA_LIBRARY Cocoa)
    FIND_LIBRARY(CARBON_LIBRARY Carbon)
    FIND_LIBRARY(IOKIT_LIBRARY IOKit)
    FIND_LIBRARY(COREAUDIO_LIBRARY CoreAudio)
    FIND_LIBRARY(AUDIOTOOLBOX_LIBRARY AudioToolbox)
    FIND_LIBRARY(AUDIOUNIT_LIBRARY AudioUnit)
    FIND_LIBRARY(SDL2_LIBRARY SDL2)
    
    set(
		SHARED_LIBRARIES
            ${SDL2_LIBRARY}
            iconv
            ${OPENGL_LIBRARY}
            ${FORCEFEEDBACK_LIBRARY}
            ${COCOA_LIBRARY}
            ${CARBON_LIBRARY}
            ${IOKIT_LIBRARY}
            ${COREAUDIO_LIBRARY}
            ${AUDIOTOOLBOX_LIBRARY}
            ${AUDIOUNIT_LIBRARY}
	)
    
    include_directories(
        ${SDL2_LIBRARY}/Headers
    )

	set(
		SOURCE_FILES
			${SOURCE_FILES}
			source/platform/sdl/opengl_context/opengl_context_sdl.c
			source/platform/sdl/screen/screen_sdl.c
			source/platform/sdl/window/window_sdl.c
	)

	if(ENABLE_MOUSE_SUPPORT)
		set( SOURCE_FILES ${SOURCE_FILES} source/platform/sdl/mouse/sdl_mouse.c )
	endif(ENABLE_MOUSE_SUPPORT)

	if(ENABLE_KEYBOARD_SUPPORT)
		set( SOURCE_FILES ${SOURCE_FILES} source/platform/sdl/keyboard/sdl_keyboard.c )
	endif(ENABLE_KEYBOARD_SUPPORT)

elseif(LINUX)
	set(
		SHARED_LIBRARIES
			X11
			Xrandr
			GL
	)

	set(
		SOURCE_FILES
			${SOURCE_FILES}
			source/platform/x11/opengl_context/opengl_context_glx.c
			source/platform/x11/screen/screen_x11.c
			source/platform/x11/window/window_x11.c
	)

	if(ENABLE_JOYSTICK_SUPPORT)
		if(LINUX)
			set( SOURCE_FILES ${SOURCE_FILES} source/platform/linux/joystick/joystick_linux.c )
		endif(LINUX)
	endif(ENABLE_JOYSTICK_SUPPORT)

	if(ENABLE_MOUSE_SUPPORT)
		set( SOURCE_FILES ${SOURCE_FILES} source/platform/x11/mouse/x11_mouse.c )
	endif(ENABLE_MOUSE_SUPPORT)

	if(ENABLE_KEYBOARD_SUPPORT)
		set( SOURCE_FILES ${SOURCE_FILES} source/platform/x11/keyboard/x11_keyboard.c )
	endif(ENABLE_KEYBOARD_SUPPORT)

	if(ENABLE_AUDIO_STREAM_SUPPORT)
		set(
			SHARED_LIBRARIES
				${SHARED_LIBRARIES}
				asound
		)
		set( SOURCE_FILES ${SOURCE_FILES} source/platform/alsa/audio_stream/audio_stream_alsa.c )
	endif(ENABLE_AUDIO_STREAM_SUPPORT)
endif(WIN32)

if (UNIX)
	target_link_libraries( lua m )

	set(
		SOURCE_FILES
			${SOURCE_FILES}
			source/platform/unix/timing/timing_unix.c
	)

	if(ENABLE_FILESYSTEM_SUPPORT)
		set( SOURCE_FILES ${SOURCE_FILES} source/platform/unix/filesystem/unix_filesystem.c )
	endif(ENABLE_FILESYSTEM_SUPPORT)
endif (UNIX)


set( BUILD_SHARED_DESC "Build as a shared library" )
option(
	BUILD_SHARED
	${BUILD_SHARED_DESC}
)
if(BUILD_SHARED)
	if(MSVC)
		add_custom_target(
			msvc_dll_def ALL
			COMMAND ${LUA_EXE} generate_dll_def.lua limbus ${LIB_DIR}/limbus.def ${GENERATED_CONFIG}
			COMMENT "Generating DLL .def file..."
			WORKING_DIRECTORY ${CBIND_DIR}
		)
		add_dependencies(
			msvc_dll_def
				lua
		)

		add_library(
			limbus_shared SHARED
				${SOURCE_FILES}
		)
		target_link_libraries(
			limbus_shared
				${SHARED_LIBRARIES}
		)
		add_dependencies(
			limbus_shared
				msvc_dll_def
		)
		set_target_properties(
			limbus_shared PROPERTIES
				LINK_FLAGS /DEF:${LIB_DIR}/limbus.def
				OUTPUT_NAME limbus
				ARCHIVE_OUTPUT_DIRECTORY ${BIN_DIR}
				RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
		)
	else(MSVC)
		add_library(
			limbus_shared SHARED
				${SOURCE_FILES}
		)
		target_link_libraries(
			limbus_shared
				${SHARED_LIBRARIES}
		)
		set_target_properties(
			limbus_shared PROPERTIES
				OUTPUT_NAME limbus
				LIBRARY_OUTPUT_DIRECTORY ${BIN_DIR}
				RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
		)
	endif(MSVC)

	if(GCC AND ANSI_STRICT)
		set_target_properties(
			limbus_shared PROPERTIES
				COMPILE_FLAGS "-ansi -Wall -pedantic"
		)
	endif(GCC AND ANSI_STRICT)
endif(BUILD_SHARED)



set(
	BUILD_STATIC
	ON
	CACHE BOOL "Build as a static library"
)
if(BUILD_STATIC)
	add_library(
		limbus
			${SOURCE_FILES}
			${STATIC_LIB_SOURCE_FILES}
	)
	set_target_properties(
		limbus PROPERTIES
			ARCHIVE_OUTPUT_DIRECTORY ${LIB_DIR}
	)

	if(GCC AND ANSI_STRICT)
		set_target_properties(
			limbus PROPERTIES
				COMPILE_FLAGS "-ansi -Wall -pedantic"
		)
	endif(GCC AND ANSI_STRICT)
endif(BUILD_STATIC)



set(
	RUN_UNITTESTS
	ON
	CACHE BOOL "Compile and run unittests."
)
if(RUN_UNITTESTS)
	add_executable(
		unittest
			source/unittest.c
			source/util/unicode.c
			source/util/vector.c
	)
	set_target_properties(
		unittest PROPERTIES
			RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
	)

	add_custom_command(
		TARGET unittest
		POST_BUILD
		COMMAND unittest
		COMMENT "Unittesting..."
	)
endif(RUN_UNITTESTS)

##
#	Examples
##
include( examples/CMakeLists.txt )

##
#	Language bindings
##

include( bindings/CMakeLists.txt )
include( tests/CMakeLists.txt )


configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/source/config.lua
	${GENERATED_CONFIG}
)

