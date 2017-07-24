#!/bin/bash
#
# clang-format-all: a tool to run clang-format on an entire project
# Copyright (C) 2016 Evan Klitzke <evan@eklitzke.org>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

function usage {
    echo "Usage: $0 DIR..."
    exit 1
}

if [ $# -eq 0 ]; then
    usage
fi

# Variable that will hold the name of the clang-format command
FMT=""

# Some distros just call it clang-format. Others (e.g. Ubuntu) are insistent
# that the version number be part of the command. We prefer clang-format if
# that's present, otherwise we work backwards from highest version to lowest
# version.
for clangfmt in clang-format{,-3.{7,6,5,4,3}}; do
    if which "$clangfmt" &>/dev/null; then
        FMT="$clangfmt"
        break
    fi
done

# Check if we found a working clang-format
if [ -z "$FMT" ]; then
    echo "failed to find clang-format"
    exit 1
fi

# Check all of the arguments first to make sure they're all directories
for dir in "$@"; do
    if [ ! -d "${dir}" ]; then
        echo "${dir} is not a directory"
        usage
    fi
done

if [ ! -r .clang-format ]; then
    echo "no .clang-format in pwd $PWD"
    exit 1
fi
# Run clang-format -i on all of the things
for dir in "$@"; do
    pushd "${dir}"

    for f in $(find . \
                  -name '*.c' \
                  -o -name '*.cc' \
                  -o -name '*.cpp' \
                  -o -name '*.h' \
                  -o -name '*.hh' \
                  -o -name '*.hpp'); do
        "${FMT}" -i "${f}"
		echo "Formatted ${f}"
    done
    popd
done