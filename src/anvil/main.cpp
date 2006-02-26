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

#include "engine.h"

CS_IMPLEMENT_APPLICATION

int main(int argc, char** argv)
{
  printf("Anvil version 0.0.2 by The Peragro Tempus Team.\n");

  // Create an application instance which will perform initialization and event
  // handling for the entire game.
  anvEngine anvil;

  // Start the application.  The underlying csApplicationFramework also
  // performs some core initialization.  It will set up the configuration
  // manager, event queue, object registry, and much more.  The object registry
  // is very important, and it is stored in your main application class (again,
  // by csApplicationFramework).
  return anvil.Main (argc, argv);
}
