#!/bin/sh
# full suspend
/var/tmp/wmctrl -r blankBackground_WS -b add,fullscreen
killall -STOP awesome
