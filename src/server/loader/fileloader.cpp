#include "fileloader.h"
#include "file-doors.h"
#include "file-items.h"

FileLoader::FileLoader() 
{
  doorsfile = new DoorsFile();
  itemsfile = new ItemsFile();
}

FileLoader::~FileLoader() 
{
  delete doorsfile;
  delete itemsfile;
}
