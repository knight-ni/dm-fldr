#/bin/sh

DM_HOME=/dm8
gcc -o dm_fldr_with_thread dm_fldr_with_thread.c -I ${DM_HOME}/drivers/fldr/include -L ${DM_HOME}/drivers/fldr -ldmfldr -lpthread
