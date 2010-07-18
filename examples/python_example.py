
#          Copyright Jesper Oskarsson 2009 - 2010.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

import openglwindow, timer

window = openglwindow.OpenglWindow()
window.caption = "Hello from Python!"
window.width = 800
window.height = 600
window.create()

running = True

def onWindowEvent( event ):
	global running
	if event == openglwindow.WindowEventClose:
		running = False

def onKeyEvent( key, pressed ):
	global running
	if key == openglwindow.KeyEscape and pressed:
		running = False

window.onWindowEvent.append( onWindowEvent )
window.onKeyEvent.append( onKeyEvent )

while running:
	window.swapBuffers()
	window.sendEvents()

window.destruct()
