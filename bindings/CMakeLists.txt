
#          Copyright Jesper Oskarsson 2009 - 2010.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

option(
	GENERATE_CPP_BINDING
	"Generate C++ class files"
)
if(GENERATE_CPP_BINDING)
	add_custom_target(
		cpp_binding ALL
		COMMAND ${LUA_EXE} ${CBIND_DIR}/generate_cpp_class.lua bindings/opengl_window.cdef OpenglWindow ${CPP_DIR}/ ${CMAKE_CURRENT_SOURCE_DIR}/generated/config.lua ${GENERATED_CONFIG}
		COMMAND ${LUA_EXE} ${CBIND_DIR}/generate_cpp_class.lua bindings/keyboard.cdef Keyboard ${CPP_DIR}/ ${CMAKE_CURRENT_SOURCE_DIR}/generated/config.lua ${GENERATED_CONFIG}
		COMMAND ${LUA_EXE} ${CBIND_DIR}/generate_cpp_class.lua bindings/mouse.cdef Mouse ${CPP_DIR}/ ${CMAKE_CURRENT_SOURCE_DIR}/generated/config.lua ${GENERATED_CONFIG}
		COMMAND ${LUA_EXE} ${CBIND_DIR}/generate_cpp_class.lua bindings/tablet.cdef Tablet ${CPP_DIR}/ ${CMAKE_CURRENT_SOURCE_DIR}/generated/config.lua ${GENERATED_CONFIG}
		COMMAND ${LUA_EXE} ${CBIND_DIR}/generate_cpp_class.lua bindings/timer.cdef Timer ${CPP_DIR}/ ${CMAKE_CURRENT_SOURCE_DIR}/generated/config.lua ${GENERATED_CONFIG}
		COMMAND ${LUA_EXE} ${CBIND_DIR}/generate_cpp_class.lua bindings/joystick.cdef Joystick ${CPP_DIR}/ ${CMAKE_CURRENT_SOURCE_DIR}/generated/config.lua ${GENERATED_CONFIG}
		COMMAND ${LUA_EXE} ${CBIND_DIR}/generate_cpp_class.lua bindings/filesystem.cdef Filesystem ${CPP_DIR}/ ${CMAKE_CURRENT_SOURCE_DIR}/generated/config.lua ${GENERATED_CONFIG}
		COMMENT "Generating C++ classes..."
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	)
	add_dependencies(
		cpp_binding
			limbus
			lua
	)

	include_directories( ${INCLUDE_DIR} )
	include_directories( ${CPP_DIR} )
	
	set_source_files_properties(
		${CPP_DIR}/OpenglWindow.cpp
		${CPP_DIR}/Keyboard.cpp
		${CPP_DIR}/Mouse.cpp
		${CPP_DIR}/Tablet.cpp
		${CPP_DIR}/Timer.cpp
		${CPP_DIR}/Joystick.cpp
		${CPP_DIR}/Filesystem.cpp
		PROPERTIES GENERATED TRUE
	)

	add_library(
		limbus_cpp
			${CPP_DIR}/OpenglWindow.cpp
		    ${CPP_DIR}/Keyboard.cpp
		    ${CPP_DIR}/Mouse.cpp
		    ${CPP_DIR}/Tablet.cpp
			${CPP_DIR}/Timer.cpp
		    ${CPP_DIR}/Joystick.cpp
		    ${CPP_DIR}/Filesystem.cpp
	)
	set_target_properties(
		limbus_cpp PROPERTIES
			ARCHIVE_OUTPUT_DIRECTORY ${LIB_DIR}
			LIBRARY_OUTPUT_DIRECTORY ${BIN_DIR}
			RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
	)
	add_dependencies(
		limbus_cpp
			cpp_binding
	)
endif(GENERATE_CPP_BINDING)



option(
	GENERATE_PYTHON_BINDING
	"Generate Python binding and class files"
)
if(GENERATE_PYTHON_BINDING)
	add_custom_target(
		python_binding ALL
		COMMAND ${LUA_EXE} ${CBIND_DIR}/generate_python_binding.lua bindings/python_binding.lua
		COMMENT "Generating Python binding..."
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	)
	add_dependencies(
		python_binding
		lua
	)

	add_custom_target(
		python_classes ALL
		COMMAND ${LUA_EXE} ${CBIND_DIR}/generate_python_class.lua bindings/opengl_window.cdef ${PYTHON_DIR}/openglwindow ${PYTHON_DIR}/limbus_import.lua ${GENERATED_CONFIG}
		COMMAND ${LUA_EXE} ${CBIND_DIR}/generate_python_class.lua bindings/timer.cdef ${PYTHON_DIR}/timer ${PYTHON_DIR}/limbus_import.lua ${GENERATED_CONFIG}
		COMMAND ${LUA_EXE} ${CBIND_DIR}/generate_python_class.lua bindings/joystick.cdef ${PYTHON_DIR}/joystick ${PYTHON_DIR}/limbus_import.lua ${GENERATED_CONFIG}
		COMMAND ${LUA_EXE} ${CBIND_DIR}/generate_python_class.lua bindings/filesystem.cdef ${PYTHON_DIR}/filesystem ${PYTHON_DIR}/limbus_import.lua ${GENERATED_CONFIG}
		COMMENT "Generating Python classes..."
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	)
	add_dependencies(
		python_classes
		python_binding
	)
endif(GENERATE_PYTHON_BINDING)



option(
	GENERATE_RUBY_BINDING
	"Generate Ruby binding and class files"
)
if(GENERATE_RUBY_BINDING)
	add_custom_target(
		ruby_binding ALL
		COMMAND ${LUA_EXE} ${CBIND_DIR}/generate_ruby_binding.lua bindings/ruby_binding.lua
		COMMENT "Generating Ruby binding..."
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	)
	add_dependencies(
		ruby_binding
		lua
	)

	add_custom_target(
		ruby_classes ALL
		COMMAND ${LUA_EXE} ${CBIND_DIR}/generate_ruby_class.lua bindings/opengl_window.cdef ${RUBY_DIR}/opengl_window ${RUBY_DIR}/limbus_import.lua ${GENERATED_CONFIG}
		COMMAND ${LUA_EXE} ${CBIND_DIR}/generate_ruby_class.lua bindings/keyboard.cdef ${RUBY_DIR}/keyboard ${RUBY_DIR}/limbus_import.lua ${GENERATED_CONFIG}
		COMMAND ${LUA_EXE} ${CBIND_DIR}/generate_ruby_class.lua bindings/mouse.cdef ${RUBY_DIR}/mouse ${RUBY_DIR}/limbus_import.lua ${GENERATED_CONFIG}
		COMMAND ${LUA_EXE} ${CBIND_DIR}/generate_ruby_class.lua bindings/tablet.cdef ${RUBY_DIR}/tablet ${RUBY_DIR}/limbus_import.lua ${GENERATED_CONFIG}
		COMMAND ${LUA_EXE} ${CBIND_DIR}/generate_ruby_class.lua bindings/timer.cdef ${RUBY_DIR}/timer ${RUBY_DIR}/limbus_import.lua ${GENERATED_CONFIG}
		COMMAND ${LUA_EXE} ${CBIND_DIR}/generate_ruby_class.lua bindings/joystick.cdef ${RUBY_DIR}/joystick ${RUBY_DIR}/limbus_import.lua ${GENERATED_CONFIG}
		COMMAND ${LUA_EXE} ${CBIND_DIR}/generate_ruby_class.lua bindings/filesystem.cdef ${RUBY_DIR}/filesystem ${RUBY_DIR}/limbus_import.lua ${GENERATED_CONFIG}
		COMMENT "Generating Ruby classes..."
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	)
	add_dependencies(
		ruby_classes
		ruby_binding
	)
endif(GENERATE_RUBY_BINDING)



option(
	GENERATE_LUA_BINDING
	"Generate Lua bindings and classes"
)
if(GENERATE_LUA_BINDING)
	add_custom_target(
		lua_binding ALL
		COMMAND ${LUA_EXE} ${CBIND_DIR}/generate_lua_binding.lua bindings/lua_binding.lua
		COMMENT "Generating Lua binding..."
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	)
	add_dependencies(
		lua_binding
			lua
	)

	include_directories(
		${CMAKE_CURRENT_SOURCE_DIR}/
		${LUA_INCLUDE_DIR}
	)

	if(BUILD_STATIC)
		link_directories( ${LIB_DIR} )
	else(BUILD_STATIC)
		link_directories( ${BIN_DIR} )
	endif(BUILD_STATIC)

	set_source_files_properties(
		${LUA_DIR}/limbus_lua.c
		PROPERTIES GENERATED TRUE
	)
	add_library(
		limbus_lua SHARED
		${LUA_DIR}/limbus_lua.c
	)
	target_link_libraries(
		limbus_lua
		limbus
		lua_lib
		${SHARED_LIBRARIES}
	)
	add_dependencies(
		limbus_lua
		lua_binding
	)
	set_target_properties(
		limbus_lua PROPERTIES
		OUTPUT_NAME limbus
		ARCHIVE_OUTPUT_DIRECTORY ${LUA_DIR}
		LIBRARY_OUTPUT_DIRECTORY ${LUA_DIR}
		RUNTIME_OUTPUT_DIRECTORY ${LUA_DIR}
		PREFIX ""
	)

	add_custom_target(
		lua_classes ALL
		COMMAND ${LUA_EXE} ${CBIND_DIR}/generate_lua_class.lua bindings/opengl_window.cdef ${LUA_DIR}/OpenglWindow.lua ${LUA_DIR}/limbus_import.lua ${GENERATED_CONFIG}
		COMMAND ${LUA_EXE} ${CBIND_DIR}/generate_lua_class.lua bindings/keyboard.cdef ${LUA_DIR}/Keyboard.lua ${LUA_DIR}/limbus_import.lua ${GENERATED_CONFIG}
		COMMAND ${LUA_EXE} ${CBIND_DIR}/generate_lua_class.lua bindings/mouse.cdef ${LUA_DIR}/Mouse.lua ${LUA_DIR}/limbus_import.lua ${GENERATED_CONFIG}
		COMMAND ${LUA_EXE} ${CBIND_DIR}/generate_lua_class.lua bindings/tablet.cdef ${LUA_DIR}/Tablet.lua ${LUA_DIR}/limbus_import.lua ${GENERATED_CONFIG}
		COMMAND ${LUA_EXE} ${CBIND_DIR}/generate_lua_class.lua bindings/timer.cdef ${LUA_DIR}/Timer.lua ${LUA_DIR}/limbus_import.lua ${GENERATED_CONFIG}
		COMMAND ${LUA_EXE} ${CBIND_DIR}/generate_lua_class.lua bindings/filesystem.cdef ${LUA_DIR}/Filesystem.lua ${LUA_DIR}/limbus_import.lua ${GENERATED_CONFIG}
		COMMAND ${LUA_EXE} ${CBIND_DIR}/generate_lua_class.lua bindings/joystick.cdef ${LUA_DIR}/Joystick.lua ${LUA_DIR}/limbus_import.lua ${GENERATED_CONFIG}
		COMMENT "Generating Lua classes..."
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	)
	add_dependencies(
		lua_classes
			lua
	)
endif(GENERATE_LUA_BINDING)

