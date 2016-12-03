#!/bin/bash -x


cd bin/client

PORT=12345
TFILE="tfile4.txt"

tclient nunki.usc.edu:12345 filetype /usr/bin
#/usr/bin: directory
tclient nunki.usc.edu:12345 filetype /usr/bin/ls
#/usr/bin/ls: ELF 32-bit MSB executable SPARC Version 1, dynamically linked, stripped
tclient nunki.usc.edu:12345 filetype /home/scf-22/csci551b/public/eecs450/pa2/logo-viterbi.png
#/home/scf-22/csci551b/public/eecs450/pa2/logo-viterbi.png: PNG image data
tclient nunki.usc.edu:12345 checksum /home/scf-22/csci551b/public/eecs450/pa2/logo-viterbi.png
#29fa9fc4d619fe576b57972b8c5fb9e4
tclient nunki.usc.edu:12345 download /home/scf-22/csci551b/public/eecs450/pa2/logo-viterbi.png
#...Downloaded data have been successfully written into 'logo-viterbi.png' (MD5=29fa9fc4d619fe576b57972b8c5fb9e4)
tclient nunki.usc.edu:12345 checksum -o 1 /home/scf-22/csci551b/public/eecs450/pa2/logo-viterbi.png
#9dc0314a5213a3a9e67375af872e8463
tclient nunki.usc.edu:12345 checksum -l 20 /home/scf-22/csci551b/public/eecs450/pa2/logo-viterbi.png
#0a19b5803ef78555d9c8554d5d747113
tclient nunki.usc.edu:12345 checksum -o 1 -l 20 /home/scf-22/csci551b/public/eecs450/pa2/logo-viterbi.png
#2d33ec017797006baf52fa21c3c82b13
tclient nunki.usc.edu:12345 filetype xyzzy
tclient nunki.usc.edu:12345 filetype /home/scf-22/csci551b/xyzzy2/x
#/home/scf-22/csci551b/xyzzy2/x: cannot open: Permission denied
tclient nunki.usc.edu:12345 filetype '%@#^'
#FILETYPE_ERR received from the server
tclient nunki.usc.edu:12345 checksum -o 9106 /home/scf-22/csci551b/public/eecs450/pa2/logo-viterbi.png
#CHECKSUM_ERR received from the server
tclient nunki.usc.edu:12345 checksum -l 9107 /home/scf-22/csci551b/public/eecs450/pa2/logo-viterbi.png
#CHECKSUM_ERR received from the server




