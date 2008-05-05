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

using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.Xml.Schema;
using System.IO;

class NpcValidator
{

  public static string filename;
  public static string schema = "npcs.xsd";

  static void Main(string[] args)
  {
    if (args.Length < 1)
    {
      Console.WriteLine("You stupid idiot! You forgot to type out the name of the npc file you want to get.\nExample Usage: validatenpc.exe baby-dragonfly.xml\n");
      return;
    }
    else
    {
      string inputarg = (string)args.GetValue(0);

      if (File.Exists(inputarg))
      {
        if (File.Exists(schema))
        {
          filename = (string)args.GetValue(0);
          validateXML(inputarg);
          //Path.GetDirectoryName(testPath))
        }
        else
        {
          Console.WriteLine("Please run this tool from the npcs directory located in peragro/data/xml/npcs\nJust copy this exe there and run from within that directory.");
          return;
        }
      }
      else
      {
        Console.WriteLine("That file doesn't even exist you loser.\nTry using a valid filename next time!");
        return;
      }
    }
  }

  public static void validateXML(string document)
  {

    Console.WriteLine("\n\nPeragro Tempus NPC Validator\nValidating {0}.\n", filename);

    // create XmlReader settings
    XmlReaderSettings settings = new XmlReaderSettings();
    settings.Schemas.Add(null, schema);
    settings.ValidationEventHandler += new ValidationEventHandler(ValidationCallback);
    settings.ValidationType = ValidationType.Schema;

    // create an XML document
    XmlDocument doc = new XmlDocument();
    doc.Load(XmlReader.Create(document, settings));

    //validate the XmlDocument
    doc.Validate(new ValidationEventHandler(ValidationCallback));

    Console.WriteLine("It would appear that validation has succeeded.");
    // Console.ReadLine();
  }

  public static void ValidationCallback(object sender, ValidationEventArgs args)
  {
    if (args.Severity == XmlSeverityType.Warning)
      Console.Write("WARNING: ");
    else if (args.Severity == XmlSeverityType.Error)
      Console.Write("ERROR: ");

    // Print the error to the screen.
    Console.WriteLine(args.Message);

    Console.WriteLine("Validation Failed");
  }
}
