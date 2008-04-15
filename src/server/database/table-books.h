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

#ifndef TABLE_BOOKS_H
#define TABLE_BOOKS_H

class Database;
class ResultSet;

#include "common/util/ptstring.h"
#include "table.h"

/**
 * Class holding data associated with a book.
 */
class BooksTableVO
{
public:
  /// The id of the book.
  int id;
  /// The item id, that identifies this as a book.
  int itemId;
  /// The name of the book.
  ptString name;
  /// The text in the book.
  ptString text;

  /**
   * Simple empty contstructor.
   */
  BooksTableVO() {}
  /**
   * More advanced construtor.
   * @param id The id of the book.
   * @param itemId The item id that identifies this as a book.
   * @param name The name of the book.
   * @param text The text in the book.
   */
  BooksTableVO(int id, int itemId, ptString name, ptString text)
  : id(id), itemId(itemId), name(name), text(text)
  {
  }
};

/**
 * Provides an interface to the database to handle storage of books.
 */
class BooksTable : public Table
{
private:
  /**
   * Parses a single database result set.
   * The caller is responsible for freeing the returned object.
   * @param rs The database result set.
   * @param row The row in the result table to use, default row is 0.
   * @return Pointer to BooksTableVO which hold all data associated with a book.
   */
  BooksTableVO* parseSingleResultSet(ResultSet* rs, size_t row = 0);
  /**
   * Will parse all data returned from the database query and insert it
   * into an array.
   * The caller is responsible for freeing the objects in the returned array.
   * @param rs The result set containing the database query result.
   * @return An array containing all BooksTableVO found in the result set.
   */
  Array<BooksTableVO*> parseMultiResultSet(ResultSet* rs);

public:
  /**
   * Construtor for BooksTable class.
   * If database is empty, the createTable function will be called.
   * @param db Pointer to the database.
   */
  BooksTable(Database* db);

  /**
   * Creates a table in the database that will store books table.
   */
  void createTable();
  /**
   * Removes all books table from the database.
   */
  void dropTable();

  /**
   * Insert a book table into the database.
   * @param vo The books table to insert.
   */
  void insert(BooksTableVO* vo);
  /**
   * Updates a book table entrie.
   */
  void update(BooksTableVO* vo);
  /**
   * Removes a book from the database.
   * Matches the book by its id and item id.
   * @param vo The BooksTableVO that should be removed.
   */
  void remove(BooksTableVO* vo);
  /**
   * Gives the total amount of books in the database.
   * @param itemID The book id.
   * @return The amount of books.
   */
  unsigned int getCount(unsigned int itemId);
  /**
   * Checks if a book exists based on its name.
   * @param name The name of the book to search for.
   * return True if the book where found, otherwise false.
   */
  bool existsByName(ptString name);
  /**
   * Checks if a book exists based on its id.
   * @param id The id of the book to search for.
   * return True if the book was found, otherwise false.
   */
  bool existsById(int id);
  /**
   * Returns a book table given its name.
   * The caller is responsible for freeing the returned door table.
   * @param name The name of the book to return.
   * @return The book VO class.
   */
  BooksTableVO* getByName(ptString name);
  /**
   * Returns a book table given its name.
   * The caller is responsible for freeing the returned door table.
   * @param id The id of the book to return.
   * @return The book VO class.
   */
  BooksTableVO* getById(int id);
  /**
   * This function will load all door tables from the database
   * and return an array containing them.
   * The caller is responsible for freeing all door tables in the array.
   * @return An array containing all available books.
   */
  Array<BooksTableVO*> getAll();
};

#endif // TABLE_BOOKS_H
