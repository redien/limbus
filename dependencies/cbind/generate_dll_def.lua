
--     Copyright (c) 2009-2010 Jesper Oskarsson
-- This software is distributed under the MIT license.
--         (See accompanying file LICENSE.txt)

require "dll_helper"

local arg = {...}

dofile( arg[3] )
generate_dll_def( arg[1], arg[2], c_source_path, c_source_files )
