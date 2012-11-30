#!/bin/sh
for f in *.ts
do
  lrelease -compress -silent $f
done
