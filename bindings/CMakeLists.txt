
#          Copyright Jesper Oskarsson 2009 - 2010.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

add_custom_target(
	binding_make_directories ALL
	COMMAND cmake -E make_directory ${PYTHON_DIR}
	COMMAND cmake -E make_directory ${RUBY_DIR}
	COMMAND cmake -E make_directory ${CPP_DIR}
	COMMAND cmake -E make_directory ${LUA_DIR}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
	generate_binding ALL
	COMMAND ${LUA_EXE} generator.lua ${GENERATED_CONFIG}
	COMMENT "Generating bindings..."
	WORKING_DIRECTORY ${CBIND_DIR}
)
add_dependencies(
	generate_binding
		lua
		binding_make_directories
)

option(
	GENERATE_CPP_BINDING
	"Generate C++ class files"
)
if(GENERATE_CPP_BINDING)
	set(
		BIND_LANGUAGES
			${BIND_LANGUAGES}
			cpp
	)

	include_directories( ${INCLUDE_DIR} )
	include_directories( ${CPP_DIR} )

	set_source_files_properties(
		${GENERATED_CPP_SOURCES}
		PROPERTIES GENERATED TRUE
	)

	add_library(
		limbus_cpp
			${GENERATED_CPP_SOURCES}
	)
	set_target_properties(
		limbus_cpp PROPERTIES
			ARCHIVE_OUTPUT_DIRECTORY ${LIB_DIR}
			LIBRARY_OUTPUT_DIRECTORY ${BIN_DIR}
			RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
	)
	add_dependencies(
		limbus_cpp
			generate_binding
	)
endif(GENERATE_CPP_BINDING)

option(
	GENERATE_PYTHON_BINDING
	"Generate Python binding and class files"
)
if(GENERATE_PYTHON_BINDING)
	set( BUILD_SHARED ON CACHE BOOL ${BUILD_SHARED_DESC} FORCE )
	set(
		BIND_LANGUAGES
			${BIND_LANGUAGES}
			python
	)
	add_custom_target(
		python_binding_copy_lib ALL
		COMMAND cmake -E copy ${BIN_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}limbus${CMAKE_SHARED_LIBRARY_SUFFIX} ${PYTHON_DIR}/
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	)
	add_dependencies(
		python_binding_copy_lib
			limbus_shared
	)
endif(GENERATE_PYTHON_BINDING)

option(
	GENERATE_RUBY_BINDING
	"Generate Ruby binding and class files"
)
if(GENERATE_RUBY_BINDING)
	set( BUILD_SHARED ON CACHE BOOL ${BUILD_SHARED_DESC} FORCE )
	set(
		BIND_LANGUAGES
			${BIND_LANGUAGES}
			ruby
	)
	add_custom_target(
		ruby_binding_copy_lib ALL
		COMMAND cmake -E copy ${BIN_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}limbus${CMAKE_SHARED_LIBRARY_SUFFIX} ${RUBY_DIR}/
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	)
	add_dependencies(
		ruby_binding_copy_lib
			limbus_shared
	)
endif(GENERATE_RUBY_BINDING)

option(
	GENERATE_LUA_BINDING
	"Generate Lua bindings and classes"
)
if(GENERATE_LUA_BINDING)
	set(
		BIND_LANGUAGES
			${BIND_LANGUAGES}
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
		generate_binding
	)

	if(MSVC)
		set_target_properties(
			limbus_lua PROPERTIES
			OUTPUT_NAME limbus
			LINK_FLAGS /DEF:${LUA_DIR}/limbus.def
			ARCHIVE_OUTPUT_DIRECTORY ${LUA_DIR}
			LIBRARY_OUTPUT_DIRECTORY ${LUA_DIR}
			RUNTIME_OUTPUT_DIRECTORY ${LUA_DIR}
			PREFIX ""
		)
	else(MSVC)
		set_target_properties(
			limbus_lua PROPERTIES
			OUTPUT_NAME limbus
			ARCHIVE_OUTPUT_DIRECTORY ${LUA_DIR}
			LIBRARY_OUTPUT_DIRECTORY ${LUA_DIR}
			RUNTIME_OUTPUT_DIRECTORY ${LUA_DIR}
			PREFIX ""
		)
	endif(MSVC)
endif(GENERATE_LUA_BINDING)

