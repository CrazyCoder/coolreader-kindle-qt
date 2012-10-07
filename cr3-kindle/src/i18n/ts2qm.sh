#!/bin/sh
for f in *.ts
do
  echo "$f"
  lrelease -compress -silent -nounfinished $f
done

