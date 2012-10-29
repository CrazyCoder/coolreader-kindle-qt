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
   # wake up, Neo
   kill -SIGUSR1 `pidof $1`
   exit 0
fi

export LD_LIBRARY_PATH=/mnt/us/qtKindle/lib:`pwd`/lib

source /etc/upstart/functions
source /etc/upstart/blanket_functions
f_blanket_unload_module ad_screensaver
f_blanket_unload_module ad_screensaver_active
f_blanket_load_module screensaver

# Close Xorg windows
./ktsuspend.sh

export QT_PLUGIN_PATH=/mnt/us/qtKindle/plugins
export QT_QWS_FONTDIR=/mnt/us/qtKindle/lib/fonts

export QWS_MOUSE_PROTO=KindleTS
export QWS_KEYBOARD=none
export QWS_DISPLAY=QKindleFb

echo "./$1 -qws"
./"$1" -qws

cd $SAVE_DIR

# return to home, comment for returning back to runner if you have other commands there
lipc-set-prop com.lab126.appmgrd start app://com.lab126.booklet.home
# restore Xorg windows
./ktresume.sh
