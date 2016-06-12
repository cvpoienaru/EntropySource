#!/bin/bash

# Copyright (c) 2016, Codrin-Victor Poienaru <cvpoienaru@gmail.com>.
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
# 1. Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
# 
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
# 
# This software is provided by the copyright holders and contributors "as is"
# and any express or implied warranties, including, but not limited to, the
# implied warranties of merchantability and fitness for a particular purpose are
# disclaimed. In no event shall the copyright holder or contributors be liable
# for any direct, indirect, incidental, special, exemplary, or consequential
# damages (including, but not limited to, procurement of substitute goods or
# services; loss of use, data, or profits; or business interruption) however
# caused and on any theory of liability, whether in contract, strict liability,
# or tort (including negligence or otherwise) arising in any way out of the use
# of this software, even if advised of the possibility of such damage.

usage()
{
	echo -e "Usage:"
	echo -e "\t$0"
	echo -e "\t\t[-h|--help]"
	echo -e "\t\t[-b|--build]"
	echo -e "\t\t[-c|--clean]"
	exit 0
}

make_directory_structure()
{
	mkdir -p $ES_BIN
	mkdir -p $ES_LIB
}

clean_directory_structure()
{
	rm -rf $ES_BIN
	rm -rf $ES_LIB
}

make_build()
{
	declare -a current_build_list=("${!1}")

	for module in ${current_build_list[@]}; do
		find $2/$module -iname makefile | while read file; do
			make -f $file
		done
	done
}

clean_build()
{
	find $1 -iname makefile | while read file; do
		make -f $file clean
	done
}

# Check script arguments
if [[ $# = 0 ]]; then
	usage
fi

# Parse arguments
while [[ $# > 0 ]]; do
	case $1 in
		--help|-h)
			usage
		;;

		--build|-b)
			source $ES_SCRIPT/build_list.sh
			make_directory_structure
			make_build primary_build_list[@] $ES_SRC
			make_build test_build_list[@] $ES_SRC
		;;

		--clean|-c)
			clean_directory_structure
			clean_build $ES_SRC
		;;

		*)
			echo "Unknown option $1"
			usage
		;;
	esac
	shift
done
