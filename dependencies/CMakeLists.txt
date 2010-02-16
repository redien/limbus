
#          Copyright Jesper Oskarsson 2009 - 2010.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

##
# Lua interpreter
##

set( LUA_SRC_PATH ${LUA_DEP_DIR}/src )
set(
	LUA_LIB_FILES
		${LUA_SRC_PATH}/lapi.c
		${LUA_SRC_PATH}/lcode.c
		${LUA_SRC_PATH}/ldebug.c
		${LUA_SRC_PATH}/ldo.c
		${LUA_SRC_PATH}/ldump.c
		${LUA_SRC_PATH}/lfunc.c
		${LUA_SRC_PATH}/lgc.c
		${LUA_SRC_PATH}/llex.c
		${LUA_SRC_PATH}/lmem.c
		${LUA_SRC_PATH}/lobject.c
		${LUA_SRC_PATH}/lopcodes.c
		${LUA_SRC_PATH}/lparser.c
		${LUA_SRC_PATH}/lstate.c
		${LUA_SRC_PATH}/lstring.c
		${LUA_SRC_PATH}/ltable.c
		${LUA_SRC_PATH}/ltm.c
		${LUA_SRC_PATH}/lundump.c
		${LUA_SRC_PATH}/lvm.c
		${LUA_SRC_PATH}/lzio.c
		${LUA_SRC_PATH}/lauxlib.c
		${LUA_SRC_PATH}/lbaselib.c
		${LUA_SRC_PATH}/ldblib.c
		${LUA_SRC_PATH}/liolib.c
		${LUA_SRC_PATH}/lmathlib.c
		${LUA_SRC_PATH}/loslib.c
		${LUA_SRC_PATH}/ltablib.c
		${LUA_SRC_PATH}/lstrlib.c
		${LUA_SRC_PATH}/loadlib.c
		${LUA_SRC_PATH}/linit.c
)

add_library(
	lua_lib
		${LUA_LIB_FILES}
)
set_target_properties(
	lua_lib PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY ${LIB_DIR}
		OUTPUT_NAME lua
		COMPILE_DEFINITIONS _CRT_SECURE_NO_WARNINGS
)

add_executable(
	lua
		${LUA_SRC_PATH}/lua.c
)
set_target_properties(
	lua PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
		COMPILE_DEFINITIONS _CRT_SECURE_NO_WARNINGS
)
target_link_libraries(
	lua
		lua_lib
)

