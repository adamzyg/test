#!/bin/bash - 
#===============================================================================
#
#          FILE: test.sh
# 
#         USAGE: ./test.sh 
# 
#   DESCRIPTION: 
# 
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: YOUR NAME (), 
#  ORGANIZATION: 
#       CREATED: 02/21/2020 12:06
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error
sleep 10
read -p "enter number:" no
read -p "enter name:" name
echo you have entered $no, $name

