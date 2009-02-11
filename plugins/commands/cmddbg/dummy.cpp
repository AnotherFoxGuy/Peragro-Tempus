/*
    Copyright (C) 2005 Development Team of Peragro Tempus

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

/* Dummy variable, used only to get the plugin to link successfully.

   The plugin uses calls from the EntityManager class, which
   calls PointerLibrary::getInstance() in the constructor.  The constructor
   isn't used by the plugin, but nevertheless causes a link problem
   if this isn't here.

   TODO: linking is beastly... linking the EntityManager for anything
   requires linking in most of the project.  EntityManager probably should 
   use an abstract interface, and derive from that.
*/

#include "client/pointer/pointer.h"

PointerLibrary* PointerLibrary::pointerlib;
