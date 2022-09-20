import os
import shutil
import subprocess

if not os.getenv("SAUROS_HOME"):
   print("Please define `SAUROS_HOME` environment variable")
   exit(1)

sauros_home = os.getenv("SAUROS_HOME")
sauros_libs = os.path.join(sauros_home, "libs")

if not os.path.isdir(sauros_home):
   os.mkdir(sauros_home)

if not os.path.isdir(sauros_libs):
   os.mkdir(sauros_libs)


def build_and_copy(path):

   install_target = os.path.join(sauros_libs, path)
   if not os.path.isdir(install_target):
      os.mkdir(install_target)
   else:
      shutil.rmtree(install_target)
      os.mkdir(install_target)

   os.chdir(path)
   if not os.path.isdir("build"):
      os.mkdir("build")

   os.chdir("build")

   print("Building : ", path)

   result = subprocess.run(["cmake", "../"], stdout=subprocess.PIPE)
   print(result.stdout.decode("utf-8"))

   result = subprocess.run(["make", "-j5"], stdout=subprocess.PIPE)
   print(result.stdout.decode("utf-8"))

   target_file = "sauros." + path + ".lib"

   target_file_dest = os.path.join(install_target, target_file)

   shutil.copyfile(target_file, target_file_dest)

   os.chdir("../")

   target_file_dest = os.path.join(install_target, "manifest.sau")
   shutil.copyfile("manifest.sau", target_file_dest)

   shutil.rmtree("build")

   os.chdir("../")


dirs = [d for d in os.listdir('.') if os.path.isdir(d)]
for dir in dirs:
   build_and_copy(dir)