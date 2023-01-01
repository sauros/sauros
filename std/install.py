import os
import subprocess

if not os.getenv("SAUROS_HOME"):
   print("Please define `SAUROS_HOME` environment variable")
   exit(1)

sauros_home = os.getenv("SAUROS_HOME")
sauros_std = os.path.join(sauros_home, "std")

if not os.path.isdir(sauros_home):
   os.mkdir(sauros_home)
if not os.path.isdir(sauros_std):
   os.mkdir(sauros_std)

for file in [d for d in os.listdir('.') if os.path.isfile(d)]:
   abs_file = os.path.abspath(file)
   root, extension = os.path.splitext(abs_file)
   if extension != ".py":
      subprocess.run(["cp", file, sauros_std], stdout=subprocess.PIPE)