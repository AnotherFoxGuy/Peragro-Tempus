/*
    Copyright (C) 2009 Development Team of Peragro Tempus

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

#ifndef TABLEHELPER_H
#define TABLEHELPER_H

#include <vector>
#include <boost/shared_ptr.hpp>

#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/tuple/elem.hpp>

#include <boost/preprocessor/list/adt.hpp>

#include <boost/preprocessor/comparison/equal.hpp>
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/seq/filter.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>

#include "common/util/exception.h"

#include "fieldhelper.h"

/* Example
// In .h
//-----------------------------------------------------------------------------------
//| Name    | C++ type name     | Key Type      | Foreign Key
//-----------------------------------------------------------------------------------
#define PT_DB_TABLE_ENTITIES "entities"
#define PT_DB_TABLE_ENTITIES_FIELDS \
  ((id,       size_t,             PT_PrimaryKey,  0)) \
  ((type,     short,              0,              0))

class EntityTable : public Table
{
private:
  PT_DECLARE_CreateTable(EntityTable, PT_DB_TABLE_ENTITIES, PT_DB_TABLE_ENTITIES_FIELDS)
public:
  PT_DECLARE_Insert(EntityTable, PT_DB_TABLE_ENTITIES, PT_DB_TABLE_ENTITIES_FIELDS)
};

// in .cpp
PT_DEFINE_CreateTable(EntityTable, PT_DB_TABLE_ENTITIES, PT_DB_TABLE_ENTITIES_FIELDS)

PT_DEFINE_Insert(EntityTable, PT_DB_TABLE_ENTITIES, PT_DB_TABLE_ENTITIES_FIELDS)
*/

// Helpers
//-----------------------------------------------------------------------------------
#define STRINGIFY(x) "\""BOOST_PP_STRINGIZE(x)"\""

#define PT_GETNAME_(e) \
  BOOST_PP_TUPLE_ELEM(4, 0, e)

#define PT_GETTYPE_(e) \
  BOOST_PP_TUPLE_ELEM(4, 1, e)

#define PT_GETK_(e) \
  BOOST_PP_TUPLE_ELEM(4, 2, e)

#define PT_GETFK_(e) \
  BOOST_PP_TUPLE_ELEM(4, 3, e)

#define PT_GETFK_TB_(a) \
  BOOST_PP_TUPLE_ELEM(2, 0, a)

#define PT_GETFK_CL_(a) \
  BOOST_PP_TUPLE_ELEM(2, 1, a)

#define PT_IS_PK_(c) \
  BOOST_PP_EQUAL(c, PT_PrimaryKey)

#define PT_IS_SK_(c) \
  BOOST_PP_EQUAL(c, PT_PrimaryKeyS)

/*#define PT_IF_PK_(c) \
  BOOST_PP_IF(PT_IS_PK_(c), " PRIMARY KEY", "")*/

#define PT_IF_PK_(c) \
  BOOST_PP_IF(PT_IS_PK_(c), "", "")

#define PT_IF_FK_(a) \
  BOOST_PP_IF(BOOST_PP_LIST_IS_CONS(a), " REFERENCES " STRINGIFY(PT_GETFK_TB_(a)) " (" STRINGIFY(PT_GETFK_CL_(a)) ")", "")

#define PT_FIELD_ARGLIST_(r, d, i, e) \
  BOOST_PP_COMMA_IF(i) const PT_GETTYPE_(e)& \
    PT_GETNAME_(e)

#define PT_CREATE_FIELDS_(r, d, i, e) \
  BOOST_PP_IF(i, ", ", "") STRINGIFY(PT_GETNAME_(e)) " %s NOT NULL" PT_IF_PK_(PT_GETK_(e)) PT_IF_FK_(PT_GETFK_(e))

#define PT_CREATE_FIELDS_TYPES_(r, d, i, e) \
  BOOST_PP_COMMA_IF(i) DB::Helper::DBType<PT_GETTYPE_(e)>()

#define PT_CREATE_UNIQUE_(r, d, i, e) \
  BOOST_PP_IF(i, ", ", ", UNIQUE (") STRINGIFY(PT_GETNAME_(e))

#define PT_INSERT_VALUENAMES_(r, d, i, e) \
  BOOST_PP_IF(i, ", ", "") BOOST_PP_STRINGIZE(PT_GETNAME_(e))

#define PT_INSERT_VALUES_SPECIFIERS_(r, d, i, e) \
  BOOST_PP_IF(i, ", ", "") "%s"

#define PT_INSERT_VALUES_(r, d, i, e) \
  BOOST_PP_COMMA_IF(i) DB::Helper::Convert<PT_GETTYPE_(e)>(PT_GETNAME_(e)).c_str()

#define PT_VO_MEMBERLIST_(r, d, i, e) \
  PT_GETTYPE_(e) PT_GETNAME_(e);

#define PT_VO_ASSIGN_(r, d, i, e) \
  DB::Helper::Convert(vo->PT_GETNAME_(e), rs->GetData(row, i));

#define PRED_PK(s, d, e) PT_GETK_(e)
#define PT_PKS_GETPKS_(s) \
  BOOST_PP_SEQ_FILTER(PRED_PK, ~, s)

#define PRED_SK(s, d, e) PT_IS_SK_(PT_GETK_(e))

#define SIZE_SK(s) BOOST_PP_SEQ_SIZE( BOOST_PP_SEQ_FILTER(PRED_SK, ~, s) )

#define PT_PKS_GETSKS_(s) \
  BOOST_PP_IF(SIZE_SK(s), BOOST_PP_SEQ_FILTER(PRED_SK, ~, s), PT_PKS_GETPKS_(s))

#define PT_CKS_VALUENAMES_(r, d, i, e) \
  BOOST_PP_IF(i, " AND ", "") BOOST_PP_STRINGIZE(PT_GETNAME_(e)) "=%s"

// Key Types
//-----------------------------------------------------------------------------------
#define PT_PrimaryKeyS 1 // Surrogate key
#define PT_PrimaryKey 2

// Macros
//-----------------------------------------------------------------------------------
#define PT_GetTableName(t) \
  BOOST_PP_STRINGIZE(t)

// Create
//-----------------------------------------------------------------------------------

#define PT_DECLARE_CreateTable(Class, t, s) \
  void CreateTable();

#define PT_DEFINE_CreateTable(Class, t, s) \
  void Class::CreateTable() \
  { \
    printf("create table "BOOST_PP_STRINGIZE(t)); \
    db->update("create table "BOOST_PP_STRINGIZE(t)" (" \
    BOOST_PP_SEQ_FOR_EACH_I(PT_CREATE_FIELDS_, ~, s) \
    BOOST_PP_SEQ_FOR_EACH_I(PT_CREATE_UNIQUE_, ~, PT_PKS_GETPKS_(s)) \
    BOOST_PP_IF(BOOST_PP_SEQ_SIZE(PT_PKS_GETPKS_(s)), ")", "") \
    ");", \
    BOOST_PP_SEQ_FOR_EACH_I(PT_CREATE_FIELDS_TYPES_, ~, s) \
    ); \
  }

// Create
//-----------------------------------------------------------------------------------
#define PT_DECLARE_DropTable(Class, t, s) \
  void DropTable();

#define PT_DEFINE_DropTable(Class, t, s) \
  void Class::DropTable() \
  { \
    db->update("drop table " BOOST_PP_STRINGIZE(t) ";"); \
  }

// Insert
//-----------------------------------------------------------------------------------
#define PT_DECLARE_Insert(Class, t, s) \
  void Insert( BOOST_PP_SEQ_FOR_EACH_I(PT_FIELD_ARGLIST_, ~, s) );

#define PT_DEFINE_Insert(Class, t, s) \
  void Class::Insert( BOOST_PP_SEQ_FOR_EACH_I(PT_FIELD_ARGLIST_, ~, s) ) \
  { \
    db->update("insert or replace into "BOOST_PP_STRINGIZE(t)" (" \
    BOOST_PP_SEQ_FOR_EACH_I(PT_INSERT_VALUENAMES_, ~, s) \
    ") values (" \
    BOOST_PP_SEQ_FOR_EACH_I(PT_INSERT_VALUES_SPECIFIERS_, ~, s) \
    ");",\
    BOOST_PP_SEQ_FOR_EACH_I(PT_INSERT_VALUES_, ~, s) \
    );\
  }

// Delete
//-----------------------------------------------------------------------------------
#define PT_DECLARE_Delete(Class, t, s) \
  void Delete( BOOST_PP_SEQ_FOR_EACH_I(PT_FIELD_ARGLIST_, ~, PT_PKS_GETPKS_(s)) );

#define PT_DEFINE_Delete(Class, t, s) \
  void Class::Delete( BOOST_PP_SEQ_FOR_EACH_I(PT_FIELD_ARGLIST_, ~, PT_PKS_GETPKS_(s)) ) \
  { \
    db->update("delete from " BOOST_PP_STRINGIZE(t) " " \
    "where " \
    BOOST_PP_SEQ_FOR_EACH_I(PT_CKS_VALUENAMES_, ~, PT_PKS_GETPKS_(s)) \
    ";", \
    BOOST_PP_SEQ_FOR_EACH_I(PT_INSERT_VALUES_, ~, PT_PKS_GETPKS_(s)) \
    ); \
  }

// VO
//-----------------------------------------------------------------------------------
#define PT_DECLARE_VO(Class, t, s) \
  struct BOOST_PP_CAT(t, TableVO) \
  { \
    BOOST_PP_SEQ_FOR_EACH_I(PT_VO_MEMBERLIST_, ~, s) \
  }; \
  typedef boost::shared_ptr<BOOST_PP_CAT(t, TableVO)> BOOST_PP_CAT(t, TableVOp); \
  typedef std::vector<BOOST_PP_CAT(t, TableVOp)> BOOST_PP_CAT(t, TableVOArray);

#define PT_DECLARE_ParseSingleResultSet(Class, t, s) \
  boost::shared_ptr<BOOST_PP_CAT(t, TableVO)> ParseSingleResultSet(ResultSet* rs, size_t row = 0);

#define PT_DEFINE_ParseSingleResultSet(Class, t, s) \
  BOOST_PP_CAT(t, TableVOp) Class::ParseSingleResultSet(ResultSet* rs, size_t row) \
  { \
    BOOST_PP_CAT(t, TableVOp) vo(new BOOST_PP_CAT(t, TableVO)()); \
    BOOST_PP_SEQ_FOR_EACH_I(PT_VO_ASSIGN_, ~, s) \
    return vo; \
  }

#define PT_DECLARE_ParseMultiResultSet(Class, t, s) \
  BOOST_PP_CAT(t, TableVOArray) ParseMultiResultSet(ResultSet* rs);

#define PT_DEFINE_ParseMultiResultSet(Class, t, s) \
  BOOST_PP_CAT(t, TableVOArray) Class::ParseMultiResultSet(ResultSet* rs) \
  { \
    BOOST_PP_CAT(t, TableVOArray) arr; \
    for (size_t i = 0; rs && i < rs->GetRowCount(); i++) \
    { \
      BOOST_PP_CAT(t, TableVOp) obj = ParseSingleResultSet(rs, i); \
      arr.push_back(obj); \
    } \
    return arr; \
  }

// GetAll
//-----------------------------------------------------------------------------------
#define PT_DECLARE_GetAll(Class, t, s) \
  BOOST_PP_CAT(t, TableVOArray) GetAll();

#define PT_DEFINE_GetAll(Class, t, s) \
  BOOST_PP_CAT(t, TableVOArray) Class::GetAll() \
  { \
    ResultSet* rs = db->query("select * from " BOOST_PP_STRINGIZE(t) ";"); \
    std::vector<boost::shared_ptr<BOOST_PP_CAT(t, TableVO)> > arr; \
    if (!rs) return arr; \
    arr = ParseMultiResultSet(rs); \
    delete rs; \
    return arr; \
  }

// DeleteAll
//-----------------------------------------------------------------------------------
#define PT_DECLARE_DeleteAll(Class, t, s) \
  void DeleteAll();

#define PT_DEFINE_DeleteAll(Class, t, s) \
  void Class::DeleteAll() \
  { \
    db->update("delete from " BOOST_PP_STRINGIZE(t) ";"); \
  }

// Get
//-----------------------------------------------------------------------------------
#define PT_DECLARE_Get(Class, t, s) \
  BOOST_PP_CAT(t, TableVOArray) Get(BOOST_PP_SEQ_FOR_EACH_I(PT_FIELD_ARGLIST_, ~, PT_PKS_GETSKS_(s)));

#define PT_DEFINE_Get(Class, t, s) \
  BOOST_PP_CAT(t, TableVOArray) Class::Get(BOOST_PP_SEQ_FOR_EACH_I(PT_FIELD_ARGLIST_, ~, PT_PKS_GETSKS_(s))) \
  { \
    ResultSet* rs = db->query("select * from " BOOST_PP_STRINGIZE(t) " " \
      "where " \
      BOOST_PP_SEQ_FOR_EACH_I(PT_CKS_VALUENAMES_, ~, PT_PKS_GETSKS_(s)) \
      ";", \
      BOOST_PP_SEQ_FOR_EACH_I(PT_INSERT_VALUES_, ~, PT_PKS_GETSKS_(s)) \
      ); \
    std::vector<boost::shared_ptr<BOOST_PP_CAT(t, TableVO)> > arr; \
    if (!rs) return arr; \
    arr = ParseMultiResultSet(rs); \
    delete rs; \
    return arr; \
  }

// GetSingle
//-----------------------------------------------------------------------------------
#define PT_DECLARE_GetSingle(Class, t, s) \
  BOOST_PP_CAT(t, TableVOp) GetSingle(BOOST_PP_SEQ_FOR_EACH_I(PT_FIELD_ARGLIST_, ~, PT_PKS_GETPKS_(s)));

PT_DEFINE_EXCEPTION(MultipleRowsGetSingle);

#define PT_DEFINE_GetSingle(Class, t, s) \
  BOOST_PP_CAT(t, TableVOp) Class::GetSingle(BOOST_PP_SEQ_FOR_EACH_I(PT_FIELD_ARGLIST_, ~, PT_PKS_GETPKS_(s))) \
  { \
    ResultSet* rs = db->query("select * from " BOOST_PP_STRINGIZE(t) " " \
      "where " \
      BOOST_PP_SEQ_FOR_EACH_I(PT_CKS_VALUENAMES_, ~, PT_PKS_GETPKS_(s)) \
      ";", \
      BOOST_PP_SEQ_FOR_EACH_I(PT_INSERT_VALUES_, ~, PT_PKS_GETPKS_(s)) \
      ); \
    if (!rs) return BOOST_PP_CAT(t, TableVOp)(); \
    std::vector<boost::shared_ptr<BOOST_PP_CAT(t, TableVO)> > arr; \
    arr = ParseMultiResultSet(rs); \
    delete rs; \
    if (arr.size() == 0) return BOOST_PP_CAT(t, TableVOp)(); \
    else if (arr.size() != 1) \
    { \
      throw PT_EX(MultipleRowsGetSingle("Multiple rows for " BOOST_PP_STRINGIZE(t) " calling GetSingle")); \
    } \
    return arr[0]; \
  }

#endif // TABLEHELPER_H
