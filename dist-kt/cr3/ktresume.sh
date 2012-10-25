#!/bin/sh
export LD_LIBRARY_PATH=/mnt/us/cr3/lib

for i in $(xwininfo -tree -root | grep "com.lab126.booklet.kindlet_M" | awk '{ print $1 }' ); do 
  ./xdotool windowmap $i
done 

lipc-set-prop com.lab126.appmgrd start app://com.lab126.booklet.home
./xdotool windowactivate `xwininfo -tree -root | grep "blankBackground_WS" | awk '{ print $1 }'`
./xdotool windowactivate `xwininfo -tree -root | grep "searchBar_ID" | awk '{ print $1 }'`
./xdotool windowmap      `xwininfo -tree -root | grep "com.lab126.booklet.home_M" | awk '{ print $1 }'`
./xdotool windowmap      `xwininfo -tree -root | grep "titleBar_ID" | awk '{ print $1 }'`
