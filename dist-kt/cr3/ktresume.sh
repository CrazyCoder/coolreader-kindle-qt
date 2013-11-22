#!/bin/sh
killall -CONT awesome
# full resume
if [ "$1" -eq "0" ]; then
  /var/tmp/wmctrl -r blankBackground_WS -b remove,fullscreen
fi
