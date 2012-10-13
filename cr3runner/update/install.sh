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
INST_LOG="/mnt/us/cr3runner_install.log"

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

logmsg "I" "backup" "Backup existing developer.keystore..."
if [ -f /var/local/java/keystore/developer.keystore ]; then
    mv -f /var/local/java/keystore/developer.keystore /var/local/java/keystore/developer.keystore.bak 2>> ${INST_LOG}
    logmsg "I" "backup" "Success."
else
    logmsg "I" "check" "Skip (developer.keystore not found)."
fi

update_percent_complete 30

logmsg "I" "copy" "Installing combined developer.keystore..."
cp -f developer.keystore /var/local/java/keystore/developer.keystore 2>> ${INST_LOG}
logmsg "I" "copy" "Success."

update_percent_complete 50

logmsg "I" "backup" "Backup original external.policy..."
mv -f /opt/amazon/ebook/security/external.policy /opt/amazon/ebook/security/external.policy.bak 2>> ${INST_LOG}
logmsg "I" "backup" "Success."

logmsg "I" "update" "Installing new external.policy..."
cp -f external.policy /opt/amazon/ebook/security/external.policy 2>> ${INST_LOG}
logmsg "I" "copy" "Success."

update_percent_complete 70

logmsg "I" "copy" "Installing cr3runner..."
cp -f cr3runner-2.7.azw2 /mnt/us/documents/cr3runner-2.7.azw2 2>> ${INST_LOG}
logmsg "I" "copy" "Success."

update_percent_complete 90

echo "All done!" >> ${INST_LOG}

update_progressbar 100

return 0
