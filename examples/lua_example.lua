
--          Copyright Jesper Oskarsson 2009 - 2010.
-- Distributed under the Boost Software License, Version 1.0.
--    (See accompanying file LICENSE_1_0.txt or copy at
--          http://www.boost.org/LICENSE_1_0.txt)

require "placebo"
require "p2dc"
require "OpenglWindow"
require "Timer"

local window = OpenglWindow();
window.width( 800 );
window.height( 600 );
window.caption( "Window" );
window.create();

local running = true

window.onWindowEvent[#window.onWindowEvent + 1] = function( event )
	if event == WindowEventClose then
		running = false
	end
end

while running do
	window.sendEvents()
	window.swapBuffers()
end

window.destruct()

