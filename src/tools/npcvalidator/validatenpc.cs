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
	public static string schemaname = "npcs.xsd";
	public static string schema;

	static void Main(string[] args)
	{
		if (args.Length < 1)
		{
		Console.WriteLine("You idiot! You forgot to type out the name of the npc file you want to get.\nExample Usage: validatenpc.exe baby-dragonfly.xml\n");
		return;
		}
		else
		{

		string inputarg = (string)args.GetValue(0);

			if (File.Exists(inputarg))
			{

			filename = (string)args.GetValue(0);

			string dirname = (string)Path.GetDirectoryName(inputarg);
			schema = (string)Path.Combine(dirname,schemaname);
			
				if (File.Exists(schema))
				{
					try
					{			
					validateXML(inputarg);
					}

					catch(NullReferenceException)
					{
					Console.WriteLine("An error has occurred during parsing. An attribute is likely missing.");
					}

				}
				
				else
				{
					Console.WriteLine("The file you specified exists but no schema was found.  Make sure a valid schema file exists in the directory containing your xml files. It should be named npcs.xsd");
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

 	CheckLength(filename);

	// create XmlReader settings
	XmlReaderSettings settings = new XmlReaderSettings();

	settings.Schemas.Add(null, schema);	

	settings.ValidationEventHandler += new ValidationEventHandler(ValidationCallback);
	
	settings.ValidationType = ValidationType.Schema;

	// create an XML document
	XmlDocument doc = new XmlDocument();

		try
		{
			// Load the xml file
			doc.Load(XmlReader.Create(document, settings));
		}
	
		catch(XmlException e)
		{

		string message = e.Message;
		int ln = e.LineNumber;
		int lp = e.LinePosition;
		int lnn = e.LineNumber;
		int lnb = lnn - 1;
		
		// Lets generate some useful output.
		
		Console.WriteLine("\nExtra Error Output: ");
		Console.WriteLine("An error has occurred.\nLine Number: {0}\nPosition: {1}\nFull Error Message: {2}", ln.ToString(), lp.ToString(), message);

		Console.WriteLine("\nSince error checking counts where it hits the next element though, you might wish to check on line {0} instead for the source of your problem.\nNote: this is not always accurate, particularly for sub-elements.", lnb.ToString());

		return;		
		}

		catch(NullReferenceException)
		{
		Console.WriteLine("An error has occurred during parsing. An attribute is likely missing.");
		}
	
		//validate the XmlDocument
		try
		{
			doc.Validate(new ValidationEventHandler(ValidationCallback));
		}

		catch(XmlSchemaValidationException a)
		{
		string nodeobject = a.SourceObject.ToString();
		string message = a.Message;
		int lna = a.LineNumber;
		int lpa = a.LinePosition;

		Console.WriteLine("\nExtra Error Output: ");
		Console.WriteLine("An error has occurred.\nLine Number: {0}\nPosition: {1}\nFull Error Message: {2}", lna.ToString(), lpa.ToString(), message);
	
		Console.WriteLine("\nNode where error occurred: {0}\n", nodeobject);

		}

		catch(NullReferenceException)
		{
		Console.WriteLine("An error has occurred during parsing. An attribute is likely missing.");
		}
	
	}

	public static void ValidationCallback(object sender, ValidationEventArgs args)
	{
		if (args.Severity == XmlSeverityType.Warning)
		{
			Console.Write("WARNING: ");
		}
		else if (args.Severity == XmlSeverityType.Error)
		{
			Console.Write("ERROR: ");
		}
	///	else
	///	{
	///		Console.WriteLine("Validation was sucessful!");
	///	}

	// Print the error to the screen.
	Console.WriteLine(args.Message);

	}

	public static void CheckLength(string xmlfilename)
	{

	// Retrieve XML document  
	XmlTextReader reader = new XmlTextReader(xmlfilename);

	// Skip non-significant whitespace  
	reader.WhitespaceHandling = WhitespaceHandling.Significant;  
  
	// Read nodes one at a time  
	while (reader.Read())  
	{  

		switch (reader.NodeType) 
		{

		case XmlNodeType.Element:

			switch (reader.Name)
			{
				case "text":

				string teststring = reader.ReadString().ToString();

				if ( teststring.Length > 254 )
				{
				Console.WriteLine("\nString is too long.\nThe offending string is: {0}\n Length: {1}\nPlease keep the number of characters less than 255", teststring, teststring.Length);
				}
				else
				{
				}	
					
				break;
				case "answer":

				string teststringb = reader.ReadString().ToString();

				if ( teststringb.Length > 254 )
				{
				Console.WriteLine("\nString is too long.\nThe offending string is: {0}\n Length: {1}\nPlease keep the number of characters less than 255", teststringb, teststringb.Length);
				}
				else
				{
				}

				break;
			}
			break;
		}

	
	}
	
	}



} // end class NpcValidator
