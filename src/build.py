import os
import shutil
import subprocess
import sys

do_clean = False
build_options = "-DCMAKE_BUILD_TYPE=Release"

def build(destination):
   os.chdir(destination)

   if os.path.isdir("build") and do_clean:
      subprocess.run(["rm", "-rf", "build"], stdout=subprocess.PIPE)

   if not os.path.isdir("build"):
      os.mkdir("build")

   os.chdir("build")

   result = subprocess.run(["cmake", "../", build_options], stdout=subprocess.PIPE)
   print(result.stdout.decode("utf-8"))

   result = subprocess.run(["make", "-j5"], stdout=subprocess.PIPE)
   print(result.stdout.decode("utf-8"))

   result = subprocess.run(["sudo", "make", "install"], stdout=subprocess.PIPE)
   print(result.stdout.decode("utf-8"))

   os.chdir("../../")

def show_help():
   print("Description: builds and installs `libsauros` and the sauros `app`")
   print("Args:")
   print("clean      -- Clean the build directories out before building")
   print("debug      -- Create a debug build")
   print("release    -- Create a release build")
   print("help       -- Show this message")
   exit(0)

for arg in sys.argv[1:]:
   if "clean" == arg:
      do_clean = True
   if "debug" == arg:
      build_options = "-DCMAKE_BUILD_TYPE=Debug"
   if "release" == arg:
      build_options = "-DCMAKE_BUILD_TYPE=Release"
   if "help" == arg or "-h" == arg:
      show_help()

print(">>> Building library")
build("libsauros")

print(">>> Building application")
build("app")