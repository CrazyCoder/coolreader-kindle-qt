#!/bin/sh
# full suspend
#if [ "$1" -eq "0" ]; then
./wmctrl -l | awk '{ print $1 }' | while read id ; do ./wmctrl -i -r $id -b add,hidden ; done
#fi

killall -STOP awesome
