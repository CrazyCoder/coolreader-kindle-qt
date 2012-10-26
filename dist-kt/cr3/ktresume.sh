#!/bin/sh
export LD_LIBRARY_PATH=/mnt/us/cr3/lib

if [ -s ".unmapped" ]
then
  cat .unmapped | while read id ; do echo $id >> .unmapped; ./xdotool windowmap $id ; done
  rm -f .unmapped
else
  for name in blankBackground_WS searchBar_ID com.lab126.booklet.home_M titleBar_ID keyboard; do
    ./xdotool windowmap `xwininfo -tree -root | grep "$name" | awk '{ print $1 }'`
  done
fi

lipc-set-prop com.lab126.appmgrd start app://com.lab126.booklet.home
