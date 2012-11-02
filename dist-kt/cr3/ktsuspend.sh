#!/bin/sh
./wmctrl -l | awk '{ print $1 }' | while read id ; do ./wmctrl -i -r $id -b add,hidden ; done
killall -STOP awesome
