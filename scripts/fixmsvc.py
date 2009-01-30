import os
import re

projtofix = [["plgcomponent_networkmove", ["libclient-component", 
                                            "libclient-data", 
                                            "libeventmanager-cs", 
                                            "libreporter",
                                            "libwfmath"]],
             ["plgcomponent_playercontrols", ["libclient-component", 
                                              "libclient-cursor", 
                                              "libeventmanager-cs", 
                                              "libclient-network",
                                              "libreporter",
                                              "libnetwork",
                                              "libclient-tcp-network",
                                              "libutil",
                                              "libwfmath"]],
             ["plgcomponent_viewbob", ["libclient-component", "libeventmanager-cs", "libreporter"]],
             ["plgcomponentstats", ["libclient-component", "libeventmanager-cs", "libreporter"]],
             ["plgcomponent_viewcontrols", ["libclient-component", "libeventmanager-cs", "libreporter"]],
             ["plgcomponentmesh", ["libclient-component", "libeventmanager-cs", "libreporter"]],  
             ["plgworldloader", ["libworld", "libwfmath"]]                     
             ]

class Project:
	def __init__(self, data):
	  mob = re.compile("\(\"(.*?)\"\) = \"(.*?)\", \"(.*?)\", \"(.*?)\"", re.S).search(data)
	  self.projid = mob.group(1)
	  self.name = mob.group(2)
	  self.file = mob.group(3)
	  self.id = mob.group(4)
	  self.deps = []
	  self.depids = []

	  deps = re.compile("ProjectSection\(ProjectDependencies\) = postProject\n(.*?)EndProjectSection\n", re.S).finditer(data)
	  for dep in deps:
	    d = dep.group(1).strip().split(' =')[0]
	    self.depids.append(d)
	  
	def AddDep(self, dep):
	  self.deps.append(dep)
	  
	def __repr__(self):
	  return "Project: %s \t %s" % (self.name, self.id)
	  
	def Generate(self):
	  t = """Project("%s") = "%s", "%s", "%s" \n""" %(self.projid, self.name, self.file, self.id)
  	
	  if len(self.deps) > 0 or len(self.depids) > 0:
		t += "\tProjectSection(ProjectDependencies) = postProject\n"
		for dep in self.depids:
			t += "\t\t%s = %s\n" % (dep, dep)
		for dep in self.deps:
			d = GetDepProject(dep)
			t += "\t\t%s = %s\n" % (d.id, d.id)
		t += "\tEndProjectSection\n"

	  t += "EndProject\n"
	  return t

projects = {}	  
def GetDepProject(name):
  if projects.has_key(name):
    return projects[name]
  else:
    return None
	
def Fix(path, version):
	loc = os.path.join(os.getcwd(), path)

	print
	print "="*50
	print "Fixing solution: "+loc+'wksall.sln'
	print "="*50

	wksall = open(loc+'wksall.sln', 'r')
	text = wksall.read()
	wksall.close()

	projectssecs = re.compile("Project(.*?)EndProject\n", re.S).finditer(text)
	globalsec = re.compile("Global(.*?)EndGlobal\n", re.S).search(text)

	if version == 8:
		text = "Microsoft Visual Studio Solution File, Format Version 9.00\n"
		text += "# Visual C++ Express 2005\n"
	elif version == 9:
		text = "Microsoft Visual Studio Solution File, Format Version 10.00\n"
		text += "# Visual C++ Express 2008\n"
		
	for m in projectssecs:
		p = Project(m.group(1))
		projects[p.name] = p

	for proj, deps in projtofix:
		if projects.has_key(proj):
			p = projects[proj]
			for dep in deps:
				p.AddDep(dep)
				print "I: Added dep", dep, "to", proj

	for p in projects:  
		text += projects[p].Generate()

	text += globalsec.group()

	wksall = open(loc+'wksall.sln', 'w')
	wksall.write(text)
	wksall.close()
	
Fix("../msvc/8/", 8)
Fix("../msvc/9/", 9)