#!/bin/bash
KERNEL_MODULE_FILE=virtual_sensordev.ko
SERVICE_FILE=load-virtual-sensordev.service
RULE_FILE=vsensor.rules

PATH_SYSTEMD=/usr/lib/systemd

if [ ! -e ${KERNEL_MODULE_FILE} ]
then
	echo "Error: No kernel module (${KERNEL_MODULE_FILE})"
	exit -1
fi

if [ ! -e ${SERVICE_FILE} ]
then
	echo "Error: No service file (${SERVICE_FILE})"
	exit -2
fi

sdb root on
sdb shell mkdir -p /data/
sdb push ${KERNEL_MODULE_FILE} /data/
sdb push ${RULE_FILE} /etc/udev/${RULE_FILE} 
sdb shell rm -f /etc/udev/rules.d/010_${RULE_FILE}
sdb shell ln -s /etc/udev/${RULE_FILE} /etc/udev/rules.d/010_${RULE_FILE} 
sdb push ${SERVICE_FILE} ${PATH_SYSTEMD}/system/
#sdb shell ln -s ${PATH_SYSTEMD}/system/${SERVICE_FILE} ${PATH_SYSTEMD}/system/graphical.target.wants/${SERVICE_FILE}
sdb shell ln -s ${PATH_SYSTEMD}/system/${SERVICE_FILE} ${PATH_SYSTEMD}/system/multi-user.target.wants/${SERVICE_FILE}
echo "Install completed. Reboot target device to use this module."
