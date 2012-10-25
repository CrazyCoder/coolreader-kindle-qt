#!/bin/sh
export LD_LIBRARY_PATH=/mnt/us/cr3/lib

./xdotool windowminimize   `xwininfo -tree -root | grep "searchBar_ID" | awk '{ print $1 }'`
./xdotool windowminimize   `xwininfo -tree -root | grep "blankBackground_WS" | awk '{ print $1 }'`
./xdotool windowunmap      `xwininfo -tree -root | grep "com.lab126.booklet.home_M" | awk '{ print $1 }'`
./xdotool windowunmap      `xwininfo -tree -root | grep "titleBar_ID" | awk '{ print $1 }'`

for i in $(xwininfo -tree -root | grep "com.lab126.booklet.kindlet_M" | awk '{ print $1 }' ); do 
  ./xdotool windowunmap $i
done

COUNTER=4; until [ $COUNTER -lt 1 ]; do ./xdotool search --classname --onlyvisible Pillowd windowminimize;let COUNTER-=1; done
COUNTER=4; until [ $COUNTER -lt 1 ]; do ./xdotool search --classname --onlyvisible Webreader windowminimize;let COUNTER-=1; done
