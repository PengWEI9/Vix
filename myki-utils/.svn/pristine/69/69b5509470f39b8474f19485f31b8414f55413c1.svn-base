#!/bin/sh

AFC_HOME=${AFC_HOME:=/afc}
PERSIST_DIR=$AFC_HOME/data/alarm/current_set

TMP_DIR=$AFC_HOME/var/tmp/persist-$$

test "$AFC_HOME" = '.' && echo "Please supply a full path." && exit 1

mkdir -p $TMP_DIR

cd $PERSIST_DIR
ls -1 | while read F ; do mv $F $TMP_DIR ; done

cd  $TMP_DIR
ls -1 | while read F ; do
	mv $F $PERSIST_DIR
	/afc/bin/alarm_command --clear
	/afc/bin/ipc_clean --flush
done

cd
rmdir $TMP_DIR
exit 0
