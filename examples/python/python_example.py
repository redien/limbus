
# Limbus - A cross-platform multimedia library.
# Written in 2014 by Jesper Oskarsson jesosk@gmail.com
#
# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain worldwide.
# This software is distributed without any warranty.
#
# You should have received a copy of the CC0 Public Domain Dedication along with this software.
# If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

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
