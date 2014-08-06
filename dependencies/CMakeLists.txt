
# Limbus - A cross-platform multimedia library.
# Written in 2014 by Jesper Oskarsson jesosk@gmail.com
#
# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain worldwide.
# This software is distributed without any warranty.
#
# You should have received a copy of the CC0 Public Domain Dedication along with this software.
# If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

##
# Lua interpreter
##

set(
	USE_LUA_5_2
	OFF
	CACHE BOOL "Assume Lua version 5.2."
)

set(
	USE_BUNDLED_LUA
	ON
	CACHE BOOL "Compile and use the bundled lua version."
)
if (USE_BUNDLED_LUA)
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

	if (USE_LUA_5_2)
		set(
			LUA_LIB_FILES
				${LUA_LIB_FILES}
				${LUA_SRC_PATH}/lbitlib.c
				${LUA_SRC_PATH}/lcorolib.c
				${LUA_SRC_PATH}/lctype.c
		)
	endif(USE_LUA_5_2)
	
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
else (USE_BUNDLED_LUA)
    
endif (USE_BUNDLED_LUA)

