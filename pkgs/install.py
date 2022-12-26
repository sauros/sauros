import os
import shutil
import subprocess

if not os.getenv("SAUROS_HOME"):
   print("Please define `SAUROS_HOME` environment variable")
   exit(1)

sauros_home = os.getenv("SAUROS_HOME")
sauros_pckgs = os.path.join(sauros_home, "pkgs")

# A name of a directory
# and relevant files to a package (any .sau and .lib)
class target_item:

   def __init__(self, name):
      self.name = name
      
      self.abspath = os.path.abspath(name)
      self.files = []

   def add_item(self, item):
      self.files.append(item)

   def dump(self):
      print("Name: ", self.name)
      print("Contents: ")
      for item in self.files:
         print("\t", item)

# A package is a list of targets
class package:
   def __init__(self, name):
      self.name = name
      self.targets = []

   def dump(self):
      print("Package name: ", self.name)
      print("Contents:")
      for target in self.targets:
         target.dump()

# ENsure that the paths exist that we need
def pre_flight():
   if not os.path.isdir(sauros_home):
      os.mkdir(sauros_home)
   if not os.path.isdir(sauros_pckgs):
      os.mkdir(sauros_pckgs)

# Build an item that has a CMakeLists.txt
def build_item(item_name):
   if not os.path.isdir("build"):
      os.mkdir("build")
   os.chdir("build")

   result = subprocess.run(["cmake", "../"], stdout=subprocess.PIPE)
   print(result.stdout.decode("utf-8"))

   result = subprocess.run(["make", "-j5"], stdout=subprocess.PIPE)
   print(result.stdout.decode("utf-8"))

   target = item_name + ".lib"
   if not os.path.isfile(target):
      print("Unable to find library file: ", target)
      exit(1)

   result = subprocess.run(["cp", target, "../"], stdout=subprocess.PIPE)
   print(result.stdout.decode("utf-8"))

   os.chdir("../")
   shutil.rmtree("build")

# Scan a target directory for files we need to copy
# and check to see if we need to build any libs 
def scan_target(target):
   print("Target: ", target.name)

   files = [d for d in os.listdir('.') if os.path.isfile(d)]
   if "CMakeLists.txt" in files:
      build_item(target.name)
      files = [d for d in os.listdir('.') if os.path.isfile(d)]

   for file in files:
      abs_file = os.path.abspath(file)
      root, extension = os.path.splitext(abs_file)
      if extension == ".sau" or extension == ".lib":
         target.add_item(abs_file)

# Scan the directories and generate anything needed,
# then place into a series of targets for the package
def build_package(pkg):
   print("Package: ", pkg)
   os.chdir(pkg)
   dirs = [d for d in os.listdir('.') if os.path.isdir(d)]

   new_package = package(pkg)

   # Check the local directory
   top_level_pkg = target_item(pkg)
   scan_target(top_level_pkg)
   new_package.targets.append(top_level_pkg)

   os.chdir("../")
   return new_package

# Copy all of the files that we scanned and deemed relevant
# to the SAUROS_HOME/packages directory
def copy_packages(packages):
   os.chdir(sauros_pckgs)
   for pkg in packages:
      for target in pkg.targets:
         if not os.path.isdir(target.name):
            os.mkdir(target.name)
         os.chdir(target.name)
         for file in target.files:
           # print("Copying: ", file)
            subprocess.run(["cp", file, "./"], stdout=subprocess.PIPE)
         os.chdir("../")
         

pre_flight()

dirs = [d for d in os.listdir('.') if os.path.isdir(d)]
packages = []
for pkg in dirs:
   packages.append(build_package(pkg))

for pkg in packages:
   print("-------------")
   pkg.dump()

copy_packages(packages)