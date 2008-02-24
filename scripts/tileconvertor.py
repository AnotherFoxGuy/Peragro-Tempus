import time
import random
import os

#import xml.dom.ext
import xml.dom.minidom
from  Ft.Xml import Domlette

#==== [ XML ] =======================================================
def WriteToScreen(doc):
    Domlette.PrettyPrint(doc)
    
def WriteToFile(document_node, filename="export/tile.xml"):
	XmlFile = file(filename,"w")
	Domlette.PrettyPrint(document_node, stream=XmlFile)
	XmlFile.close()
    
def CreateDocument():
    doc = Domlette.implementation.createDocument(None,None,None)
    return doc
    
def OpenDocument(filename):
	newdocument = Domlette.NonvalidatingReader.parseUri("file:"+filename)
	return newdocument.rootNode
    
def FindElement(doc, elementName):
    return doc.getElementsByTagName(elementName)

#==== [ CS ] =========================================================
def GetNode(parentNode, elementName):
	node = parentNode.xpath(elementName)[0]
	return node
	
def GetNodeByName(parentNode, elementName, name):
	for node in parentNode.xpath(elementName):
		if node.getAttributeNS(None, "name") == name:
			return node
	return None
	
def ParseNode(parentNode, elementName):
	list = []
	for node in parentNode.xpath(elementName):
		list.append( node )
	return list
	
def GetName(node):
	if node.getAttributeNS(None, "name") == "":
		key = node.firstChild.nodeValue
	else:
		key = node.getAttributeNS(None, "name")
	return key
	
def FindPlugins(parentNode):
	dict = ()
	for node in parentNode.xpath("meshfact"):
		plugin = node.xpath("plugin")[0]
		pluginname = plugin.firstChild.nodeValue
		dict += pluginname,
		if pluginname.endswith("fact"):
			dict += pluginname.split("fact")[0],
	
	dict = set(dict)
	return dict

def FindMaterials(parentNode):
	dict = ()
	for node in parentNode.xpath("meshfact"):
		params = node.xpath("params")[0]
		if params and params.xpath("material"):
			material = params.xpath("material")[0]
			materialname = material.firstChild.nodeValue
			dict += materialname,
			
	dict = set(dict)
	return dict

def FindTextures(parentNode):
	dict = ()
	for mat in parentNode.xpath("material"):
		for node in mat.xpath("texture | shadervar"):
			textureName = node.firstChild.nodeValue
			dict += textureName,
		
	dict = set(dict)
	return dict
	
#==== [ Main ] =======================================================		
def Main():
	
	PATH = "D:\Temp\world/"
	EXPORT = "export"
	FACTORY_PATH = "factories/keep/"
	EXPORT_LIGHTS = False
	
	os.chdir(PATH)
	for root, dirs, files in os.walk(PATH):
		if not EXPORT in dirs:
			os.chdir(PATH)
			print "Creating export dir."
			os.mkdir(EXPORT)
		break
	
	for root, dirs, files in os.walk(PATH+EXPORT):
		if not "factories" in dirs:
			os.chdir(PATH+EXPORT)
			print "Creating factories dir."
			os.mkdir("factories")
		break
	
	print "Opening world file."
	os.chdir(PATH)
	worldNode = OpenDocument("world").documentElement

	print "Creating tile file."
	tile = CreateDocument()
	tilenode = tile.createElementNS(None, "tile")
	tile.appendChild(tilenode)
	
	print "Parsing plugins"
	plugins = []
	pluginNode = GetNode(worldNode, "plugins")
	plugins = ParseNode(pluginNode, "plugin")
		
	print "Parsing textures"
	textures = []
	textureNode = GetNode(worldNode, "textures")
	textures = ParseNode(textureNode, "texture")
	
	print "Parsing materials"
	materials = []
	materialNode = GetNode(worldNode, "materials")
	materials = ParseNode(materialNode, "material")

	print "Parsing libraries"
	libraries = []
	libraries = ParseNode(worldNode, "library")	
	
	sectorNode = GetNodeByName(worldNode, "sector", "room")
	
	print "Parsing lights"
	if EXPORT_LIGHTS:
		lights = []
		meshobjects = ParseNode(sectorNode, "light")
		
	print "Parsing portals"
	portals = []
	portals = ParseNode(sectorNode, "portals")

	print "Parsing meshobjects"
	meshobjects = []
	meshobjects = ParseNode(sectorNode, "meshobj")	
	
	print "Sort meshobjects on loadPriority"
	def GetValue(node):
		val = 0
		keys = node.xpath("key")
		for key in keys:
			if GetName(key) == "loadPriority":
				prior = key.getAttributeNS(None, "value").lower()
				if prior == "high":
					val = 3
				elif prior == "normal":
					val = 2
				elif prior == "low":
					val = 1
		return val
		
	def cmp(x, y):
		xval = GetValue(x)
		yval = GetValue(y)
		if xval>yval:
			return -1
		elif xval==yval:
			return 0
		else: # x<y
			return 1
		
	meshobjects = sorted(meshobjects, cmp)
	
	#===[ Tile ]==================================
	print "Copying", len(libraries), "library references"
	for library in libraries:
		newnode = tile.importNode(library, True)
		# Convert to lowercase filenames.
		newnode.firstChild.nodeValue = FACTORY_PATH + newnode.firstChild.nodeValue.lower()
		tilenode.appendChild(newnode)
		
	print "Copying", len(meshobjects), "meshobjects"
	for obj in meshobjects:
		newnode = tile.importNode(obj, True)
		tilenode.appendChild(newnode)
		
	print "Copying", len(portals), "portals"
	for portal in portals:
		newnode = tile.importNode(portal, True)
		tilenode.appendChild(newnode)
		
	if EXPORT_LIGHTS:
		print "Copying lights"
		for light in lights:
			newnode = tile.importNode(light, True)
			tilenode.appendChild(newnode)
		
	print "Writing tile file."
	WriteToFile(tile, EXPORT+"/"+"tile.xml")

	#===[ Library ]==================================
	print "Updating", len(libraries), "libraries"
	for library in libraries:
		fileName = library.firstChild.nodeValue
		doc = OpenDocument(fileName)
		node = doc.documentElement
		first = node.firstChild
		#=[ Plugins ]========
		pluginsnode = doc.createElementNS(None, "plugins")
		node.insertBefore(pluginsnode, first)
		
		pluginNames = FindPlugins(node)
		for plug in plugins:
			if GetName(plug) in pluginNames:
				newnode = tile.importNode(plug, True)
				pluginsnode.appendChild(newnode)
		
		#=[ Textures ]========
		texturesnode = doc.createElementNS(None, "textures")
		node.insertBefore(texturesnode, first)
		
		#=[ Materials ]========
		materialsnode = doc.createElementNS(None, "materials")
		node.insertBefore(materialsnode, first)
		
		materialNames = FindMaterials(node)
		for mat in materials:
			if GetName(mat) in materialNames:
				newnode = tile.importNode(mat, True)
				materialsnode.appendChild(newnode)
				
		textureNames = FindTextures(materialsnode)
		for tex in textures:
			if GetName(tex) in textureNames:
				newnode = tile.importNode(tex, True)
				texturesnode.appendChild(newnode)
		
		os.chdir(PATH)
		# Convert to lowercase filenames.
		WriteToFile(doc, EXPORT+"/"+fileName.lower())

Main()