#/bin/sh

DM_HOME=/dm8
gcc -o test test.c -I ${DM_HOME}/drivers/fldr/include -L ${DM_HOME}/drivers/fldr -ldmfldr
