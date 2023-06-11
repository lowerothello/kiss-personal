#!/bin/sh -e

# "#pragma once" is non-standard
# fuck "#pragma once"
# all my homies hate "#pragma once"

# call with "./fuckpragmaonce.sh $(find . -type f)" or similar

for i in "$@"
do
	definename="_$(echo "$i" | tr "[:lower:]" "[:upper:]" | tr "/.-" "_")"

	# the U+FEFF is, if I understand it correctly, macos being literal hell to work with
	# apparently apple thinks big endian or middle endian is going into vogue soon
	# see <https://en.wikipedia.org/wiki/Byte_order_mark> for more info
	cat > _ <<-EOF
		#ifndef $definename
		#define $definename 1
		$(sed 's_#pragma once__;s_﻿__g' "$i")

		#endif /* #ifndef $definename */
	EOF

	mv _ "$i"
done
