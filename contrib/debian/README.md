
Debian
====================
This directory contains files used to package nativecoind/nativecoin-qt
for Debian-based Linux systems. If you compile nativecoind/nativecoin-qt yourself, there are some useful files here.

## nativecoin: URI support ##


nativecoin-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install nativecoin-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your nativecoinqt binary to `/usr/bin`
and the `../../share/pixmaps/nativecoin128.png` to `/usr/share/pixmaps`

nativecoin-qt.protocol (KDE)

