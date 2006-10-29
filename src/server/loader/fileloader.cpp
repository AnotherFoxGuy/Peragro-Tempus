#include "fileloader.h"
#include "file-doors.h"
#include "file-items.h"
#include "file-sectors.h"

FileLoader::FileLoader() 
{
  doorsfile = new DoorsFile();
  itemsfile = new ItemsFile();
  sectorsfile = new SectorsFile();
}

FileLoader::~FileLoader() 
{
  delete doorsfile;
  delete itemsfile;
  delete sectorsfile;
}
