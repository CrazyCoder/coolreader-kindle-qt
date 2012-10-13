#!/bin/sh

# diff OTA patch script

_FUNCTIONS=/etc/rc.d/functions
[ -f ${_FUNCTIONS} ] && . ${_FUNCTIONS}

MSG_SLLVL_D="debug"
MSG_SLLVL_I="info"
MSG_SLLVL_W="warn"
MSG_SLLVL_E="err"
MSG_SLLVL_C="crit"
MSG_SLNUM_D=0
MSG_SLNUM_I=1
MSG_SLNUM_W=2
MSG_SLNUM_E=3
MSG_SLNUM_C=4
MSG_CUR_LVL=/var/local/system/syslog_level
INST_LOG="/mnt/us/cr3runner_uninstall.log"

logmsg()
{
    local _NVPAIRS
    local _FREETEXT
    local _MSG_SLLVL
    local _MSG_SLNUM

    _MSG_LEVEL=$1
    _MSG_COMP=$2

    { [ $# -ge 4 ] && _NVPAIRS=$3 && shift ; }

    _FREETEXT=$3

    eval _MSG_SLLVL=\${MSG_SLLVL_$_MSG_LEVEL}
    eval _MSG_SLNUM=\${MSG_SLNUM_$_MSG_LEVEL}

    local _CURLVL

    { [ -f $MSG_CUR_LVL ] && _CURLVL=`cat $MSG_CUR_LVL` ; } || _CURLVL=1

    if [ $_MSG_SLNUM -ge $_CURLVL ]; then
        /usr/bin/logger -p local4.$_MSG_SLLVL -t "ota_install" "$_MSG_LEVEL def:$_MSG_COMP:$_NVPAIRS:$_FREETEXT"
    fi

    [ "$_MSG_LEVEL" != "D" ] && echo "BIN_install: ($_MSG_COMP) $_FREETEXT" >> ${INST_LOG}
}

if [ -z "${_PERCENT_COMPLETE}" ]; then
    export _PERCENT_COMPLETE=0
fi

update_percent_complete()
{
    _PERCENT_COMPLETE=$((${_PERCENT_COMPLETE} + $1))
    update_progressbar ${_PERCENT_COMPLETE}
}

update_percent_complete 10

echo "Start update!" >> ${INST_LOG}

logmsg "I" "remove" "Removing combined developer.keystore..."
if [ -f /var/local/java/keystore/developer.keystore ]; then
    rm -f /var/local/java/keystore/developer.keystore 2>> ${INST_LOG}
    logmsg "I" "remove" "Success."
else
    logmsg "I" "warning" "Skip (developer.keystore not found)."
fi

update_percent_complete 30

logmsg "I" "restore" "Restoring old developer.keystore..."
if [ -f /var/local/java/keystore/developer.keystore.bak ]; then
    mv -f /var/local/java/keystore/developer.keystore.bak /var/local/java/keystore/developer.keystore 2>> ${INST_LOG}
    logmsg "I" "restore" "Success."
else
    logmsg "I" "warning" "Skip (developer.keystore.bak not found)."
fi

update_percent_complete 50

logmsg "I" "restore" "Restoring original external.policy..."
if [ -f /opt/amazon/ebook/security/external.policy.bak ]; then
    rm -f /opt/amazon/ebook/security/external.policy 2>> ${INST_LOG}
    mv -f /opt/amazon/ebook/security/external.policy.bak /opt/amazon/ebook/security/external.policy 2>> ${INST_LOG}
    logmsg "I" "restore" "Success."
else
    logmsg "I" "warning" "Skip (external.policy.bak not found)."
fi

update_percent_complete 70

logmsg "I" "remove" "Removing cr3runner..."
if [ -f /mnt/us/documents/cr3runner-2.7.azw2 ]; then
    rm -f /mnt/us/documents/cr3runner-2.7.azw2 2>> ${INST_LOG}
    logmsg "I" "remove" "Success (cr3runner-2.7.azw2)."
else
    logmsg "I" "warning" "Skip (cr3runner-2.7.azw2 not found)."
fi
if [ -d /mnt/us/developer ]; then
    rm -rf /mnt/us/developer 2>> ${INST_LOG}
    logmsg "I" "remove" "Success (developer)."
else
    logmsg "I" "warning" "Skip (directory not found)."
fi

update_percent_complete 90

echo "All done!" >> ${INST_LOG}

update_progressbar 100

return 0
