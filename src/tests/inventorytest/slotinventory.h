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

void AddObjectsRef(Inventory* inv, const Objects& items)
{
  Objects::const_iterator it;
  unsigned int row = 0;
  unsigned int column = 0;
  for (it = items.begin(); it != items.end(); it++)
  {
    assert(inv->AddObjectAt(PositionRef(column, row), *it));

    if (column < inv->GetColumnCount()-1)
    {column++;}
    else
    {row++; column=0;}

    if (row >= inv->GetRowCount())
      break;
  }
}

void AddObjectsId(SlotInventory* inv, const Objects& items)
{
  Objects::const_iterator it;
  unsigned int id = 0;
  for (it = items.begin(); it != items.end(); it++)
  {
    assert(inv->AddObjectAt(id, *it));
    id++;
  }
}

void RemoveObjectsRef(Inventory* inv, const Objects& items)
{
  Objects::const_iterator it;
  unsigned int row = 0;
  unsigned int column = 0;
  for (it = items.begin(); it != items.end(); it++)
  {
    assert(inv->RemoveObjectAt(PositionRef(column, row)));

    if (column < inv->GetColumnCount()-1)
    {column++;}
    else
    {row++; column=0;}

    if (row >= inv->GetRowCount())
      break;
  }
}

void RemoveObjectsId(SlotInventory* inv, const Objects& items)
{
  Objects::const_iterator it;
  unsigned int id = 0;
  for (it = items.begin(); it != items.end(); it++)
  {
    assert(inv->RemoveObjectAt(id));
    id++;
  }
}

void MoveObjectsRef(Inventory* inv, const Objects& items)
{
  for (size_t i = 0; i < inv->GetRowCount() * inv->GetColumnCount(); i++)
  {
    unsigned int c1 = i%inv->GetColumnCount();
    unsigned int r1 = i/inv->GetColumnCount();
    unsigned int c2 = (i+2)%inv->GetColumnCount();
    unsigned int r2 = (i+2)/inv->GetColumnCount();
    r2 %= inv->GetRowCount();
    assert( inv->MoveObject(PositionRef(c1, r1), PositionRef(c2, r2)) );
    i++;i++;
  }
}

void MoveObjectsId(SlotInventory* inv, const Objects& items)
{
  for (size_t i = 0; i < inv->GetRowCount() * inv->GetColumnCount(); i++)
  {
    size_t i2 = (i+2)%(inv->GetRowCount()*inv->GetColumnCount());
    assert( inv->MoveObject(i, i2) );
    i++;i++;
  }
}

bool Equal(Inventory* i1, Inventory* i2)
{
  if (i1->GetRowCount() != i2->GetRowCount()) return false;
  if (i1->GetColumnCount() != i2->GetColumnCount()) return false;

  for (size_t i = 0; i < i1->GetRowCount() * i1->GetColumnCount(); i++)
  {
    unsigned int c = i%i1->GetColumnCount();
    unsigned int r = i/i1->GetColumnCount();
    boost::shared_ptr<Object> o1 = i1->GetObjectAt(PositionRef(c, r));
    boost::shared_ptr<Object> o2 = i2->GetObjectAt(PositionRef(c, r));
    if (o1 != o2) return false;
  }
  return true;
}

void TestSlotInventory()
{
  SlotInventory slotInvRef("SlotInvRef", ALLOW_ITEMS, 10, 5);
  SlotInventory* invRef = &slotInvRef;

  SlotInventory slotInvId("SlotInvId", ALLOW_ITEMS, 10, 5);
  SlotInventory* invId = &slotInvId;

  std::cout << "Slot Inventory: " << invRef->GetRowCount() <<" "<< invRef->GetColumnCount() << std::endl;

  // Add the callback.
  CallBack cb;
  invRef->AddInventoryCallBack(&cb);
  invId->AddInventoryCallBack(&cb);

  Objects items;
  for (size_t i = 0; i < (invRef->GetRowCount() * invRef->GetColumnCount()); i++)
  {
    std::stringstream name; name << i;
    boost::shared_ptr<Item> o(new Item());
    o->SetObjectName(name.str());
    items.push_back(o);
  }

  // Add items.
  AddObjectsRef(invRef, items);
  std::cout << *invRef << std::endl;
  AddObjectsId(invId, items);
  std::cout << *invId << std::endl;
  assert(Equal(invRef, invId));

  // Move items.
  MoveObjectsRef(invRef, items);
  std::cout << *invRef << std::endl;
  MoveObjectsId(invId, items);
  std::cout << *invId << std::endl;
  assert(Equal(invRef, invId));

  // Remove items.
  RemoveObjectsRef(invRef, items);
  std::cout << *invRef << std::endl;
  RemoveObjectsId(invId, items);
  std::cout << *invId << std::endl;
  assert(Equal(invRef, invId));

  // Remove the callback.
  invRef->RemoveInventoryCallBack(&cb);
  invId->RemoveInventoryCallBack(&cb);
}