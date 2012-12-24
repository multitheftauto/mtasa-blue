#! /bin/sh
#
# Must be called after initial-install.sh the first time, can be called anytime after that
#
if [ ! -f "Makefile" ]; then
  echo "Error: Please run ./initial-install.sh first"
  exit
fi

make -j4 -C MTA10_Server install
make -j4 -C Shared/XML install

data_files="acl.xml banlist.xml mtaserver.conf vehiclecolors.conf"
for i in $data_files ; do
  if [ ! -e "MTA10_Server/output/mods/deathmatch/$i" ] ; then
    cp MTA10_Server/mods/deathmatch/"$i" MTA10_Server/output/mods/deathmatch/"$i"
  fi
done

cd MTA10_Server/output
if [ ! -d  "mods/deathmatch/resources" ]; then
    svn export http://mtasa-resources.googlecode.com/svn/trunk/required mods/deathmatch/resources
else
    echo "Info: MTA10_Server/output/mods/deathmatch/resources already exists, delete it and call ./install.sh again if you want to grab the latest version of the default resources."
fi

if [ -f "mtasa-1.1-custom-net-linux.tar.gz" ]; then
  rm -f mtasa-1.1-custom-net-linux.tar.gz
fi

if [ "$(which wget)" ]; then
  wget http://nightly.mtasa.com/?multitheftauto_linux-1.1-latest -O multitheftauto_linux-1.1-latest.tar.gz
  tar -xzf multitheftauto_linux-1.1-latest.tar.gz --transform 's:[^/]*:latest_nightly:'
  mv latest_nightly/net.so .
  rm -rf latest_nightly multitheftauto_linux-1.1-latest.tar.gz
else
  echo "Warning: Please download net.so from http://mtasa-blue.googlecode.com/files/mtasa-1.1-custom-net-linux.tar.gz"
fi

echo "Server ready in MTA10_Server/output"

