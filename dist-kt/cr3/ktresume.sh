#!/bin/sh
killall -CONT awesome
# full resume
if [ "$1" -eq "0" ]; then
  /var/tmp/wmctrl -l | awk '{ print $1 }' | while read id ; do /var/tmp/wmctrl -i -r $id -b remove,hidden ; done
fi
