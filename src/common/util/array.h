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

#ifndef ARRAY_H
#define ARRAY_H

#include <assert.h>
#include <new>
#include <iostream>

/**
  * Array
  * Simple array, please note that the order of elements is not given.
  * If you delete an entry, the last will replace it.
  */
template<typename typ>
class Array
{
private:
  /// Pointer to the array containing the object.
  typ* typarray;

  /// Reference Counter.
  size_t* refcount;

  /// Maximum object the array can store.
  size_t capacity;

  /// Number of currently stored object.
  size_t count;

  /// Number of new objects which will be added by #increase().
  size_t growrate;

  /** Increase size of array
    *  The Array will be extended by #growrate. Should the allocation of more memory fail,
    *  so it will return the small array. Error tolerance.
    */
  void increase()
  {
    increase(growrate);
  }

  /** Increase size of array
    *  The Array will be extended by #grow. Should the allocation of more memory fail,
    *  so it will return the small array. Error tolerance.
    */
  void increase(size_t grow)
  {
    typ* newtyparray = new (std::nothrow) typ[capacity+grow];
    if (!typarray)
    {
      typarray = newtyparray;
      capacity += grow;
    }
    else if (newtyparray)
    {
      for (size_t i = 0; i != capacity; i++)
      {
        newtyparray[i] = typarray[i];
      }
      delete [] typarray;
      typarray = newtyparray;
      capacity += grow;
    }
    else
    {
      std::cerr << "Array expansion from: " << (unsigned int)capacity << " to "
        << (unsigned int)capacity+grow << "failed\n";
    }
  }

public:
  /** Creates a new, empty array
    *  \param capacity Initial length of the array.
    *  \param growrate Number of new items added when #capacity limit is reached.
    */
  Array(size_t capacity = 0, size_t growrate = 10)
  : typarray(0), capacity(capacity), count(0), growrate(growrate)
  {
    refcount = new size_t;
    *refcount = 1;
    if (capacity)
    {
      typarray = new typ[capacity];
    }
  }

  Array(const Array& other)
  : typarray(other.typarray), refcount(other.refcount), capacity(other.capacity),
    count(other.count), growrate(other.growrate)
  {
    (*refcount)++;
  }

  /** Deletes the array
    *  If using pointers as #typ then those will not be deleted.
    */
  ~Array()
  {
    (*refcount)--;
    if (*refcount == 0)
    {
      delete refcount;
      delete [] typarray;
    }
  }

  /** Returns an Object
    *  If using pointers as #typ then those will not be deleted.
    *  \param index Position of the object inside the array.
    *  \return Object set by #add at the specified position.
    */
  const typ& get(size_t index) const
  {
    assert(index < count);
    return typarray[index];
  }

  /** Returns an Object
    *  If using pointers as #typ then those will not be deleted.
    *  \param index Position of the object inside the array.
    *  \return Object set by #add at the specified position.
    */
  typ& get(size_t index)
  {
    assert(index < count);
    return typarray[index];
  }

  ///Same as #get.
  typ operator[](size_t index) const
  {
    if (index < count)
      return get(index);

    if (index == count && index < capacity)
    {
      return typarray[index];
    }
    return typarray[count];
  }

  ///Returns the number of objects inside the array.
  size_t getCount() const
  {
    return count;
  }

  /** Adds a new item to the end of the array
    *  If the end of the array has been reached, it will increase it automatically
    *  \param object item which shall be added.
    */
  void add(typ& object)
  {
    if (count >= capacity)
    {
      increase();
      if (count >= capacity) //in case increase() failed... error tolerance.
      {
        assert("Array::add : failed to add object");
        return;
      }
    }
    typarray[count] = object;
    count++;
  }

  /** Adds all items to the end of the array
    *  \param object array of items which shall be added.
    */
  void add(Array<typ>& object)
  {
    // Increase the array if needed
    if (capacity-count < object.count)
    {
      increase(object.count - (capacity-count));
    }
    // Copy the objects from the other array
    for (size_t i=0; i<object.count; i++)
    {
      typarray[count+i] = object.typarray[i];
    }
    // Adjust count variable
    count += object.count;
  }

  /** Adds all items to the end of the array
    *  \param objects array of items which shall be added.
    *  \param objects_count The amount of objects in the array.
    */
  void add(typ* objects, size_t objects_count)
  {
    // Increase the array if needed
    if (capacity-count < objects_count)
    {
      increase(objects_count - (capacity-count));
    }
    // Copy the objects.
    for (size_t i=0; i<objects_count; i++)
    {
      typarray[count+i] = objects[i];
    }
    // Adjust count variable
    count += objects_count;
  }

  /** Finds an object inside the array
    * Please not that the object needs to implement the operator==
    * Returns #count if object wasn't found.
    * \return position of the object inside the array or #count if not found
    */
  size_t find(typ object) const
  {
    for(size_t i=0; i<count; i++)
    {
      if (typarray[i] == object)
      {
        return i;
      }
    }
    return count;
  }

  /// Sets a new #growrate.
  void setGrowRate(size_t grow)
  {
    growrate = grow;
  }

  /// Returns the current #growrate
  size_t getGrowRate() const
  {
    return growrate;
  }

  /** Removes an item
    *  The removed item will be replaced it with the last item in the array.
    *  Therefore the item order inside the array is not given!
    *  \param i index of the object which shall be removed.
    */
  void remove(size_t i)
  {
    if (i >= count) assert("Array::get : out of bound access");
    count--;
    typarray[i] = typarray[count];
  }

  /** Removes an item and deletes it
    *  The removed item will be replaced it with the last item in the array.
    *  Only use that if you have deletable types in the array such as pointers!
    *  \param i index of the object which shall be removed.
    *  \param isArray Indicates if its an array or single element that
    *  should be deleted.
    */
  void del(size_t i, bool isArray = false)
  {
    if (i >= count) assert("Array::del : out of bound access");
    count--;
    if (isArray) delete [] typarray[i];
    else delete typarray[i];
    typarray[i] = typarray[count];
  }

  /** Increase size of array
    *  The Array will be extended by #grow. Should the allocation of more memory fail,
    *  so it will return the small array. Error tolerance.
    */
  void forceIncrement(size_t grow)
  {
    if (grow > 0)
      increase(grow);
  }

  /** Removes all items
    */
  void removeAll()
  {
    count=0;
  }

  /** Removes all items and deletes it
    *  Only use that if you have deletable types in the array such as pointers!
    */
  void delAll()
  {
    for (size_t i = 0; i< count; i++)
    {
      delete typarray[i];
    }
    count = 0;
  }

  const typ* getRawArray() const
  {
    return typarray;
  }

};

#endif //ARRAY_H
