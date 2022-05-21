#!/usr/bin/env bash
if ! dpkg -s cmake >/dev/null 2>&1; then
  echo -e "We not found cmake on your system\nPlease run apt-get install cmake"
  exit 1;
fi

if ! dpkg -s g++ >/dev/null 2>&1; then
   echo -e "We not found g++ on you system\nPlease run apt-get install g++"
fi

rm -rf build
mkdir build
cd build
cmake ..
make
cp server.exe ../
cp client.exe ../
rm -rf ../build