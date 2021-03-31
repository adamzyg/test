#!/bin/bash - 
#===============================================================================
#
#          FILE: test_bash_rematch.sh
# 
#         USAGE: ./test_bash_rematch.sh 
# 
#   DESCRIPTION: 
# 
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: YOUR NAME (), 
#  ORGANIZATION: 
#       CREATED: 02/04/2021 10:46
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error

while read  line; do
    if [[ $line =~ id=\"([0-9]+).*name=\"([^\"]*) ]]; then
        echo "${BASH_REMATCH[2]} | ${BASH_REMATCH[1]}"
    fi
done < file

