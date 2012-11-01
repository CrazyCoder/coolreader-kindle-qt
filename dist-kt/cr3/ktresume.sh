#!/bin/sh
export LD_LIBRARY_PATH=/mnt/us/cr3/lib

if [ -s ".unmapped" ]
then
  # cat .unmapped | while read id ; do echo $id >> .unmapped; ./xdotool windowmap $id ; done
  cat .unmapped | while read id ; do echo $id >> .unmapped; ./wmctrl -i -r $id -b remove,hidden; done
  rm -f .unmapped
else
  echo ... emergency resume ...
  for name in com.lab126.booklet.home titleBar lightDialog pillowAlert searchResults searchBar systemMenu blankBackground keyboard; do
    # ./xdotool windowmap `xwininfo -tree -root | grep "$name" | awk '{ print $1 }'`
    ./wmctrl -r $name -b remove,hidden
  done
fi
