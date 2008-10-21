/*
    Copyright (C) 2007-2008 Development Team of Peragro Tempus

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
/**
 * @file flags.h
 * @brief flags class.
 */

#ifndef PT_FLAGS_H
#define PT_FLAGS_H

namespace PT
{
  namespace Common
  {
    namespace Utils
    {
      /**
      * Set of flags which can be accessed through masks.
      */
      class Flags
      {
      typedef unsigned int uint32;

      private:
        /// Set of flags
        uint32 flags;

      public:
        /// Constructor. All flags are set to false by default.
        Flags (uint32 value = 0) : flags (value) { }

        /**
        * Initialize all flags to the given mask. You can
        * use this function to set and clear all flags at once.
        * If you only want to set flags (and not clear others)
        * then use 'Set(mask)'.
        */
        void SetAll (uint32 value)
        { flags = value; }

        /**
        * Set all given flags. This function will set all flags
        * which are '1' in the mask. All other flags are untouched.
        * This contrasts with SetAll() which will set all flags that
        * are '1' in the input and clear the flags that are '0'.
        */
        void Set (uint32 mask)
        { flags = (flags & ~mask) | mask; }

        /**
        * Reset all given flags.
        * This will clear all flags which you specify in the mask.
        * i.e. if a mask bit is 1 then the corresponding flag will be cleared.
        * All other flags are untouched.
        */
        void Reset (uint32 mask)
        { flags = (flags & ~mask); }

        /**
        * Set all flags with the given mask.
        * With this function you can set and clear a series of flags
        * but only the flags you specify in the mask will be affected.
        */
        void Set (uint32 mask, uint32 value)
        { flags = (flags & ~mask) | (value & mask); }

        /**
        * Set all flags with the given value.
        * This function will set all flags given in the mask with either
        * '1' or '0' depending on the boolean input value.
        */
        void SetBool (uint32 mask, bool value)
        {
          if (value)
            flags |= mask;
          else
            flags &= ~mask;
        }

        /// Get flags.
        uint32 Get () const
        { return flags; }

        /// Check if any of the given flags are set.
        bool Check (uint32 mask) const
        { return (flags & mask) != 0; }

        /// Check if all the given flags are set.
        bool CheckAll (uint32 mask) const
        { return (flags & mask) == mask; }


        /// Check if two sets of flags are equal
        bool operator== (const Flags& other) const
        { return flags == other.flags; }
        /// Check if two sets of flags are not equal
        bool operator!= (const Flags& other) const
        { return flags != other.flags; }
        /// AND-combine two sets of flags
        Flags operator& (const Flags& other) const
        { return Flags (flags & other.flags); }

        Flags operator~() const
        { return Flags (~flags); }
      };

    } // Utils namespace
  } // Common namespace
} // PT namespace

#endif // PT_FLAGS_H

