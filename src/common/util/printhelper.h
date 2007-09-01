/*
    Copyright (C) 2007 Development Team of Peragro Tempus

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef PRINTHELPER_H
#define PRINTHELPER_H

// Fix, in order to be able to use printf safe on win32 and linux 32/64

#ifdef __x86_64__
# define SIZET "zd"
# define PRIxPTR "lx"
#else
# define SIZET "ld"
# define PRIxPTR "x"
#endif

#endif

