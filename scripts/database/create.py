import os

DBNAME = "EntityChannels"
PATH = os.path.dirname(__file__) + '/'
TEMPLATENAME = "Reputations"
TEMPLATETYPE = "TABLE_REPUTATIONS" 

def CreateFiles(dbName):
	h = open(PATH+"template.h", 'r').read()
	c = open(PATH+"template.cpp", 'r').read()

	h = h.replace(TEMPLATENAME, dbName)
	h = h.replace(TEMPLATETYPE, "TABLE_"+dbName.upper())

	c = c.replace(TEMPLATENAME, DBNAME)
	c = c.replace(TEMPLATETYPE, "TABLE_"+dbName.upper())
	c = c.replace("table-reputations.h", "table-"+dbName.lower()+".h")

	nh = open(PATH+"table-"+dbName.lower()+".h", 'w')
	nc = open(PATH+"table-"+dbName.lower()+".cpp", 'w')

	nh.write(h)
	nh.close()

	nc.write(c)
	nc.close()
	
	
CreateFiles(DBNAME)