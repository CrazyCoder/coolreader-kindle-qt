#!/bin/sh
./wmctrl -l | awk '{ print $1 }' | while read id ; do ./wmctrl -i -r $id -b remove,hidden ; done
