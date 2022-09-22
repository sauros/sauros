// Copyright (c) Leon Freist <freist@informatik.uni-freiburg.de>
// This software is part of HWBenchmark

#include <string>

#include "hwinfo/os.h"

namespace hwinfo {

// _____________________________________________________________________________________________________________________
OS::OS() {
   _32bit = getIs32bit();
   _64bit = getIs64bit();
   _bigEndian = getIsBigEndian();
   _littleEndian = !_bigEndian;
}

// _____________________________________________________________________________________________________________________
std::string OS::fullName() {
   if (_fullName.empty()) {
      _fullName = getFullName();
   }
   return _fullName;
}

// _____________________________________________________________________________________________________________________
std::string OS::name() {
   if (_name.empty()) {
      _name = getName();
   }
   return _name;
}

// _____________________________________________________________________________________________________________________
std::string OS::version() {
   if (_version.empty()) {
      _version = getVersion();
   }
   return _version;
}

// _____________________________________________________________________________________________________________________
std::string OS::kernel() {
   if (_kernel.empty()) {
      _kernel = getKernel();
   }
   return _kernel;
}

// _____________________________________________________________________________________________________________________
bool OS::is32bit() const { return _32bit; }

// _____________________________________________________________________________________________________________________
bool OS::is64bit() const { return _64bit; }

// _____________________________________________________________________________________________________________________
bool OS::isBigEndian() const { return _bigEndian; }

// _____________________________________________________________________________________________________________________
bool OS::isLittleEndian() const { return _littleEndian; }

// _____________________________________________________________________________________________________________________
bool OS::getIs32bit() { return !getIs64bit(); }

// _____________________________________________________________________________________________________________________
bool OS::getIsBigEndian() {
   char16_t dummy = 0x0102;
   return ((char *)&dummy)[0] == 0x01;
}

// _____________________________________________________________________________________________________________________
bool OS::getIsLittleEndian() {
   char16_t dummy = 0x0102;
   return ((char *)&dummy)[0] == 0x02;
}

} // namespace hwinfo