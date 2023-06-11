#!/bin/sh -e

waifu2x="/usr/share/waifu2x/waifu2x.lua"

if [ "$1" ]
then
	exec luajit "$waifu2x" "$@"
else
	exec luajit "$waifu2x" -h
fi
