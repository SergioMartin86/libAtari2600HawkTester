#!/bin/bash

# Stop if anything fails
set -e

# Getting executable paths
executable=${1}

# Getting script name
script=${2}

# Getting additional arguments
testerArgs=${@:3}

# Getting current folder (game name)
folder=`basename $PWD`

# Getting pid (for uniqueness)
pid=$$

# Hash files
hashFile="/tmp/baseA2600Hawk.${folder}.${script}.${pid}.hash"

# Removing them if already present
rm -f ${hashFile}.simple
rm -f ${hashFile}.rerecord

set -x
# Running script (Simple)
${executable} ${script} --hashOutputFile ${hashFile}.simple ${testerArgs} --cycleType Simple

# Running script (Rerecord)
${executable} ${script} --hashOutputFile ${hashFile}.rerecord ${testerArgs} --cycleType Rerecord
set +x

# Comparing hashes
hashSimple=`cat ${hashFile}.simple`
hashRerecord=`cat ${hashFile}.rerecord`

# Removing temporary files
rm -f ${hashFile}.simple ${hashFile}.rerecord

# Compare hashes
if [ "${hashSimple}" = "${hashRerecord}" ]; then
 echo "[] Test Passed"
 exit 0
else
 echo "[] Test Failed"
 exit -1
fi
