#!/bin/sh

sqlite_shell=`echo "$1" | cut -d= -f2`/sqlite_shell
if [ ! -x "$sqlite_shell" ]; then
  echo Could not find sqlite
  exit 2
fi
if [ ! -f $2 ]; then
  echo "$2" does not exist
  exit 2
fi

cp "$2" "$3"
"$sqlite_shell" "$3" << EOS
INSERT INTO CDVersion (start_date, major_version, minor_version) VALUES ('2013-01-01 00:00:00', 0, 0);
.quit
EOS

