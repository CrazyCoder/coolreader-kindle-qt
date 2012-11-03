#!/bin/sh
killall -CONT awesome
# full resume
if [ "$1" -eq "0" ]; then
  ./wmctrl -l | awk '{ print $1 }' | while read id ; do ./wmctrl -i -r $id -b remove,hidden ; done
fi
