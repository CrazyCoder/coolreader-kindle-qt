#!/bin/sh
if [ -z "$1" ]        # No argument passed?
then
  echo "Usage: qoqt.sh 'executable_file_name'" ;
  return 1
fi

SAVE_DIR=`pwd`
cd /mnt/us/cr3

if [ ! -f "$1" ]      # Check if file exists and it is a regular one
then
   echo "goqt: "$1" -- regular file does not exist." ;
     exit 1
fi
if [ ! -x "$1" ]
then
   echo "goqt: "$1" -- not an executable file." ;
   exit 1
fi

if [ ! -z `pidof $1` ]
then
   echo "goqt: "$1" -- already running." ;
      exit 1
fi

# uncomment to shut down the Amazon framework
# /etc/init.d/framework stop

# stop cvm
killall -stop cvm

export LD_LIBRARY_PATH=/mnt/us/qtKindle/lib:`pwd`/lib
export QT_PLUGIN_PATH=/mnt/us/qtKindle/plugins
export QT_QWS_FONTDIR=/mnt/us/qtKindle/lib/fonts

export QWS_MOUSE_PROTO=none
export QWS_KEYBOARD=KindleKeyboard
export QWS_DISPLAY=QKindleFb

echo "./$1 -qws"
./"$1" -qws

cd $SAVE_DIR
# always try to continue cvm
killall -cont cvm || /etc/init.d/framework start
