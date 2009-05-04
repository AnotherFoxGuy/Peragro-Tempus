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


void TestGridInventory()
{
  GridInventory slotInvRef("GridInvRef", ALLOW_ITEMS, 10, 5);
  GridInventory* inv = &slotInvRef;

  std::cout << "Grid Inventory: " << inv->GetRowCount() <<" "<< inv->GetColumnCount() << std::endl;

  // Add the callback.
  CallBack cb;
  inv->AddInventoryCallBack(&cb);

  Objects items;
  for (size_t i = 0; i < (inv->GetRowCount() * inv->GetColumnCount()); i++)
  {
    std::stringstream name; name << i;
    boost::shared_ptr<Item> o(new Item());
    o->SetObjectName(name.str());
    items.push_back(o);
  }

  // Add items.
  AddObjectsRef(inv, items);
  std::cout << *inv << std::endl;

  // Move items.
  MoveObjectsRef(inv, items);
  std::cout << *inv << std::endl;

  // Remove items.
  RemoveObjectsRef(inv, items);
  std::cout << *inv << std::endl;

  {
    boost::shared_ptr<Item> o(new Item());
    o->SetObjectName("0");
    o->SetSize(WFMath::AxisBox<2>(WFMath::Point<2>(0),  WFMath::Point<2>(3)));
    assert(inv->AddObjectAt(PositionRef(0, 0), o));
  }
  {
    boost::shared_ptr<Item> o(new Item());
    o->SetObjectName("1");
    o->SetSize(WFMath::AxisBox<2>(WFMath::Point<2>(0),  WFMath::Point<2>(2)));
    assert(inv->AddObjectAt(PositionRef(0, 4), o));
  }

  {
    boost::shared_ptr<Item> o(new Item());
    o->SetObjectName("2");
    o->SetSize(WFMath::AxisBox<2>(WFMath::Point<2>(0),  WFMath::Point<2>(2,3)));
    assert(inv->AddObjectAt(PositionRef(3, 4), o));
  }

  std::cout << *inv << std::endl;

  // Invalid move. not enough room at destination. (2 is in the way.)
  inv->MoveObject(PositionRef(0, 1), PositionRef(1, 4));

  std::cout << *inv << std::endl;

  // Move.
  inv->MoveObject(PositionRef(0, 1), PositionRef(0, 4));

  std::cout << *inv << std::endl;

  // Remove the callback.
  inv->RemoveInventoryCallBack(&cb);
}