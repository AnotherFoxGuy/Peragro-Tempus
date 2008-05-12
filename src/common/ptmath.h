/*
    Copyright (C) 2008 Development Team of Peragro Tempus

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

#ifndef PT_MATH_H
#define PT_MATH_H

/**
 * Used to get the largest value.
 * @param f1 The first value.
 * @param f2 The second value.
 * @return The largest of the two values.
 */
float ptmaxf(float f1, float f2) {
  return f1 > f2 ? f1 : f2;
}

/**
 * Used to get the smallest value.
 * @param f1 The first value.
 * @param f2 The second value.
 * @return The smallest of the two values.
 */
float ptminf(float f1, float f2) {
  return f1 < f2 ? f1 : f2;
}

#endif  // PT_MATH_H
