#!/bin/sh
rm -rf /var/tmp/crfswin.running
/var/tmp/crfswin &
# wait for the flag file
x=0
while [ "$x" -lt 30 -a ! -f /var/tmp/crfswin.running ]
do
  x=$((x+1))
  /var/tmp/usleep 200000
done
/var/tmp/usleep 500000
