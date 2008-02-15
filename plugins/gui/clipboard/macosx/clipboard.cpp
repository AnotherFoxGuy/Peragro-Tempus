/*
    Copyright (C) 2001 by Jorrit Tyberghein

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <cssysdef.h>
#include "clipboard.h"
#include <iutil/objreg.h>
#include <iutil/plugin.h>

CS_IMPLEMENT_PLUGIN
SCF_IMPLEMENT_FACTORY (csClipboard)

csClipboard::csClipboard(iBase* parent) :
	scfImplementationType (this, parent),
	object_reg(0)
{
}

csClipboard::~csClipboard()
{
}

bool csClipboard::Initialize (iObjectRegistry* r)
{
  object_reg = r;
  return true;
}

//Opens the clipboard for examination and prevents other applications from modifying the clipboard content. 
bool csClipboard::Open() 
{
	if (clipboardRef != NULL)
		return true;
	// The pasteboard is used by Mac OS X 10.3 or higher
	if (PasteboardCreate == NULL)
		return false;
	OSStatus status = PasteboardCreate(kPasteboardClipboard, &clipboardRef);
	if (status != noErr)
	{
		clipboardRef = NULL;
        return false;
	}
    
	return true;
}

//Closes the clipboard. 
bool csClipboard::Close()
{
	clipboardRef = NULL;
    return true;
}

//Empties the clipboard and frees handles to data in the clipboard.
bool csClipboard::Empty()
{
	OSStatus status = PasteboardClear( clipboardRef );
	if (status != noErr)
	{
       clipboardRef = NULL;
       return false;
	}
	else
	   return true;
}

//Retrieves data from the clipboard in a specified format. The clipboard must have been opened previously. Returns NULL if empty.
void csClipboard::GetData(csString &text)
{
    OSStatus err = noErr;
    PasteboardSyncFlags syncFlags;
    ItemCount itemCount;
	text = "";

	if (!Open())
       return;

	//Get number of items in the pasteboard
	err = PasteboardGetItemCount( clipboardRef, &itemCount );
    require_noerr( err, CantGetPasteboardItemCount );
    
    PasteboardItemID itemID;
    CFArrayRef flavorTypeArray;
    CFIndex flavorCount;
		
    //Get an item ID for current item in the pasteboard
    err = PasteboardGetItemIdentifier( clipboardRef, 1, &itemID );
    require_noerr( err, CantGetPasteboardItemIdentifier );
        
    //Get array of flavors for current item in the pasteboard
    err = PasteboardCopyItemFlavors( clipboardRef, itemID, &flavorTypeArray);
    require_noerr( err, CantCopyPasteboardItemFlavors );
        
	//Get number of flavors in array
	flavorCount = CFArrayGetCount( flavorTypeArray );
    for( CFIndex flavorIndex = 0; flavorIndex < flavorCount; flavorIndex++)
    {
        CFStringRef flavorType;
        CFDataRef flavorData;
        CFIndex flavorDataSize;
        

	    //Get the flavor strings in the array of flavors
        flavorType = (CFStringRef)CFArrayGetValueAtIndex( flavorTypeArray, flavorIndex );
			
		char flavorText[256];
        //Compare flavor string to the needed flavor type
        if (UTTypeConformsTo(flavorType, CFSTR("public.utf8-plain-text")))
        {
            //Get flavor data from pasteboard if compatible
            err = PasteboardCopyItemFlavorData( clipboardRef, itemID, flavorType, &flavorData );
            require_noerr( err, CantCopyFlavorData );
            flavorDataSize = CFDataGetLength( flavorData );
            flavorDataSize = (flavorDataSize < 254) ? flavorDataSize : 254;
               
	 	    //Iterate through the bytes of data and copy them into an object
	 	    for( short dataIndex = 0; dataIndex <= flavorDataSize; dataIndex++)
            {
                char byte = *(CFDataGetBytePtr( flavorData ) + dataIndex);
                flavorText[dataIndex] = byte;
            }
            flavorText[flavorDataSize] = '\0';
            flavorText[flavorDataSize+1] = '\n';
				
    	    //Set data object for retrieval from pasteboard
            text = csString(flavorText);
            CFRelease (flavorData);
				
		    // Only return the first item found
		    if (err == noErr)
    		    break;
   		}
	}
    CantCopyFlavorData:
    ;

    CFRelease (flavorTypeArray);
    CantCopyPasteboardItemFlavors:
    CantGetPasteboardItemIdentifier:
    CantGetPasteboardItemCount:
    ;
}

//Sets the clipboard object using the specified format.
bool csClipboard::SetData(csString text)
{
    OSStatus err = noErr;
    CFDataRef textData = nil;
    PasteboardSyncFlags syncFlags;

	if (!Open())
       return "";

    if (!Empty())
	   return "";

	//Synchronize Pasteboard
    syncFlags = PasteboardSynchronize( clipboardRef );
	require_action( !(syncFlags&kPasteboardModified),
    PasteboardNotSynchedAfterClear, err = badPasteboardSyncErr );
    require_action( (syncFlags&kPasteboardClientIsOwner),
    ClientNotPasteboardOwner, err = notPasteboardOwnerErr );

	//Assign data to handle
	char* source = (char *) text.GetData();
	textData = CFDataCreate ( kCFAllocatorDefault,
	(UInt8*)(source), strlen(source) * sizeof(UInt8));
	require_action( textData != NULL, CantCreateTextData, err = memFullErr );
	require_action( textData != NULL, CantCreateTextData, err = memFullErr );

	// Place the handle on the clipboard. 
	err = PasteboardPutItemFlavor( clipboardRef, (PasteboardItemID)1,
	    CFSTR("public.utf8-plain-text"), textData, 0 );
	require_noerr( err, CantPutTextData );
	
	//Error message handling
    CantPutTextData:
    CantCreateTextData:
    CantGetDataFromTextObject:
    CantSetPromiseKeeper:
    ClientNotPasteboardOwner:
    PasteboardNotSynchedAfterClear:
    CantClearPasteboard:
    
	Close(); 

	return (err == noErr);
}

