#
# Regular cron jobs for the iotivity package
#
0 4	* * *	root	[ -x /usr/bin/iotivity_maintenance ] && /usr/bin/iotivity_maintenance
