import re
import glob
import sys
import os
import subprocess
import threading
import time

runs_each = 50

if len(sys.argv) < 2:
   print("Expected path to sauros binary")
   exit(1)

binary = sys.argv[1]

print("\n<< SAUROS BENCH >>\n")
check_directory = os.getcwd()
print("CWD : ", check_directory)

test_directories = [
  check_directory + ""
]

def test_item(id, expected_result, item):

   cum_time = 0
   for x in range(0, runs_each):
      start = time.time()
      result = subprocess.run([binary, item], stdout=subprocess.PIPE)
      end = time.time()

      cum_time += end - start

      if result.returncode != int(expected_result):
         print("Error : Failed to execute")
         exit(1)

   avg = cum_time / runs_each
   sec_avg = str(round(avg, 4)) + "s"
   ms_avg = str(round(avg * 1000, 4)) + "ms"

   print(item, ">> averaged an execution time of ", sec_avg, "(", ms_avg, ")", " after ", str(runs_each), " executions")
   return

def retrieve_objects_from(directory):
   os.chdir(directory)
   items_in_dir = glob.glob("*.saur")
   results = []
   for item in items_in_dir:
      result = {}
      result["name"] = item
      result["path"] = directory + "/" + item
      results.append(result)
   return results

def build_exec_list(dirs):
   exec_list = []
   for dir in dirs:
      print("Scanning directory : ", dir)
      exec_list.append(retrieve_objects_from(dir))
   print("")
   return exec_list

def task(id, jobs):
   for item in jobs:
      test_item(id, 0, item["path"])

def run():
   exec_list = build_exec_list(test_directories)
   os.chdir(check_directory)
   for item in exec_list:
      task(0, item)

run_time_start = time.time()
run()
run_time_end = time.time()

total_time = run_time_end - run_time_start
secs_ran = str(round(total_time, 4)) 
mins_ran = str(round((total_time / 60), 4))

print("Total time was ", secs_ran, " seconds (", mins_ran, " minutes)")