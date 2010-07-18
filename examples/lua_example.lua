
--          Copyright Jesper Oskarsson 2009 - 2010.
-- Distributed under the Boost Software License, Version 1.0.
--    (See accompanying file LICENSE_1_0.txt or copy at
--          http://www.boost.org/LICENSE_1_0.txt)

require "limbus"
require "OpenglWindow"
require "Timer"

local window = OpenglWindow();
window.width = 800;
window.height = 600;
window.caption = "Window";
window.create();

local running = true

window.onWindowEvent(function( event )
	if event == LBWindowEventClose then
		running = false
	end
end)

while running do
	window.sendEvents()
	window.swapBuffers()
end

window.destruct()

