#! /bin/sh
#
# Must be called after mta-build.sh the first time, can be called anytime after that
#
if [ ! -f "Makefile" ]; then
  echo "Error: Please run ./mta-build.sh first"
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

echo "Server ready in MTA10_Server/output"
echo "Additional step 1: Manually install resources into MTA10_Server/output/mods/deathmatch/resources from http://mirror.mtasa.com/mtasa/resources/"
echo "Additional step 2: Manually install MTA10_Server/output/net.so by extracting net.so from matching archive at https://nightly.mtasa.com/"
