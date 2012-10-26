#!/bin/sh
export LD_LIBRARY_PATH=/mnt/us/cr3/lib

# unmap all viewable windows
rm -f .unmapped
xwininfo -tree -root | grep "MapState=IsViewable" | awk '{ print $1 }' | while read id ; do echo $id >> .unmapped; ./xdotool windowunmap $id ; done
