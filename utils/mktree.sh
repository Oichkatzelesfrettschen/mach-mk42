#!/bin/sh
#
# Mach Operating System
# Copyright (c) 1990 Carnegie-Mellon University
# Copyright (c) 1989 Carnegie-Mellon University
# Copyright (c) 1988 Carnegie-Mellon University
# All rights reserved.  The CMU software License Agreement specifies
# the terms and conditions for use and redistribution.
#
#
# HISTORY
# $Log:	mktree.sh,v $
# Revision 2.2  90/06/02  15:14:35  rpd
# 	Added i386 cases.
# 	[90/05/03            dbg]
# 
# 	Updated for new IPC.
# 
# 	Added pmax cases.
# 	[89/11/29            af]
# 
# 	Created from rpd's machtree
# 	[89/10/10            rwd]
# 
# Revision 2.12  89/04/08  23:33:20  rpd
# 	Put back code to link sys to h, for pre-X77 kernels.
# 	Removed a bunch of progress messages from MACHINE creation;
# 	made it create MACHINE/mach and MACHINE/mach_debug.
# 	Create mach/machine and mach_debug/machine links.
# 	[89/03/14  15:00:25  rpd]
# 

#
# A script to generate a complete set of kernel sources for any RCS
# configuration.  The result is a ready-to-compile directory, populated
# with RCS links, sources, and symbolic links for "machine" and "sys" (or "h").
#

# Our name, for warnings.
ProgName="`expr \"$0\" : '.*/\(.*\)' '|' \"$0\"`"

# Location of kernel RCS tree
KernelRCS=/afs/cs.cmu.edu/project/mach-7/mk/rcs

# Location of kernel latest tree
KernelLatest=/afs/cs.cmu.edu/project/mach-7/mk/src/latest

# Location of kernel merge tree
KernelMerge=/afs/cs.cmu.edu/project/mach-7/mk/src/merge

# Location of VERSIONS file
Versions="$KernelMerge/VERSIONS"

# Directories that we will process.
# -dir adds to this list.
# Normals args are sets of dirs from Directories/
# Defaults to "."
Dirs=

# -co sets CheckOut on; -noco sets CheckOut off
# -co is the default
CheckOut=yes

# -q sets Quiet on; -v sets Quiet off
# -v is the default
Quiet=

# -i specifies interactive prompting; -f forces the default
# the default is -i
Force=

# -b <version> specifies a base version
# the default is to prompt the user (unless Force)
BaseVersion=

# -n <name> specifies a branch name
# the default is to prompt the user (unless Force)
BranchName=

while [ "$#" -gt 0 ]; do
	case "$1" in
	  -help) echo "$ProgName: Summary of switches:";
		 echo "  -dir <dir>: specify a dir to process";
		 echo "  -co: check out subdirectories (default)";
		 echo "  -noco: don't check out subdirectories";
		 echo "  -q: be quiet";
		 echo "  -v: be verbose (default)";
		 echo "  -i: ask questions (default)";
		 echo "  -f: don't ask questions; use default answer";
		 echo "  -b <version>: specify a base version";
		 echo "  -n <name>: specify a branch name";
		 echo "  -help: this summary";
		 exit 0;;
	  -dir) shift;
		if [ "$1" = "." ]; then
			Dirs="$Dirs $1";
		elif expr "$1" : '\./' > /dev/null; then
			Dirs="$Dirs $1";
		else
			Dirs="$Dirs ./$1";
		fi
		shift;;
	  -co) CheckOut=yes; shift;;
	  -noco) CheckOut=; shift;;
	  -q) Quiet=yes; shift;;
	  -v) Quiet=; shift;;
	  -i) Force=; shift;;
	  -f) Force=yes; shift;;
	  -b) shift; BaseVersion="$1"; shift;;
	  -n) shift; BranchName="$1"; shift;;
	  -*) echo "$ProgName: unrecognized switch $1 (try -help)" 1>&2; exit 1;;
	  *) break;;
	esac
done

#
# The b-style rcs cover program doesn't work properly
# unless the last component of the directory name is "kernel".
#

if [ "`expr \"\`pwd\`\" : '.*/\(.*\)'`" != latest ]; then
	echo "$ProgName: WARNING: directory name not 'latest'" 1>&2
fi

if [ -d RCS ]; then
	if [ ! "$Quiet" ]; then echo "[ RCS already exists ]"; fi
else
	default=yes;
	if [ "$Force" ]; then answer="$default"; else
		echo -n "Link RCS to $KernelRCS? [$default] "
		read answer
		if [ -z "$answer" ]; then answer="$default"; fi
	fi

	if [ "$answer" = "yes" ] || [ "$answer" = "y" ]; then
		if [ ! "$Quiet" ]; then echo "[ linking RCS to $KernelRCS ]"; fi
		ln -s "$KernelRCS" RCS
	fi

	if [ ! -d RCS ]; then
		echo "$ProgName: You must have an RCS directory" 1>&2
		exit 1
	fi
fi

if [ ! -d RCS/Directories ]; then
	echo "$ProgName: You must have an RCS/Directories directory" 1>&2
	exit 1
fi

if [ -r RCSconfig ]; then
	if [ ! "$Quiet" ]; then echo "[ RCSconfig already exists ]"; fi
else
	if [ ! "$BaseVersion" ]; then
		default="`awk '{ v = $1; } END { print v; }' \"$Versions\"`"
		if [ "$Force" ]; then answer="$default"; else
			echo -n "Base version? [$default] "
			read answer
			if [ -z "$answer" ]; then answer="$default"; fi
		fi
		BaseVersion="$answer"
	fi

	# for some reason, can't put "" around `awk ...`

	date=`awk 'BEGIN { mon["Jan"] = "01"; mon["Feb"] = "02"; mon["Mar"] = "03";
                           mon["Apr"] = "04"; mon["May"] = "05"; mon["Jun"] = "06";
                           mon["Jul"] = "07"; mon["Aug"] = "08"; mon["Sep"] = "09";
                           mon["Oct"] = "10"; mon["Nov"] = "11"; mon["Dec"] = "12"; }
                    $1 == "'"$BaseVersion"'" {
printf("%s/%s/%02d,%s\n", substr($8, 3, 2), mon[$4], $5, $6); }' "$Versions"`

	if [ -z "$date" ]; then
		echo "$ProgName: Unknown version $answer specified" 1>&2
		exit 1
	fi

	if [ ! "$BranchName" ]; then
		default="${USER}_${BaseVersion}"
		if [ "$Force" ]; then answer="$default"; else
			echo -n "Branch name? [$default] "
			read answer
			if [ -z "$answer" ]; then answer="$default"; fi
		fi
		BranchName="$answer"
	fi

	if [ ! "$Quiet" ]; then
		echo "[ creating RCSconfig ]"
		echo "[   branch: $BranchName ]"
		echo "[   date:   $date ($BaseVersion) ]"
		echo "[   vbase:  $KernelRCS ]"
	fi
	rm -f RCSconfig
	echo "$BranchName" >> RCSconfig
	echo "<$date" >> RCSconfig
	echo "-relative_to $KernelRCS" >> RCSconfig

	if [ ! "$Quiet" ]; then echo "[ linking RCSCONFIG to RCSconfig ]"; fi
	rm -f RCSCONFIG
	ln -s RCSconfig RCSCONFIG

	if [ ! "$Quiet" ]; then echo "[ creating .BCSVBASE ]"; fi
	rm -f .BCSVBASE
	echo "$KernelRCS" > .BCSVBASE

	if [ ! "$Quiet" ]; then echo "[ creating .BCSBRANCH ]"; fi
	rm -f .BCSBRANCH
	echo "$BranchName" > .BCSBRANCH

	if [ ! "$Quiet" ]; then echo "[ linking .BCSconfig-$BranchName to RCSconfig ]"; fi
	rm -f ".BCSconfig-$BranchName"
	ln -s RCSconfig ".BCSconfig-$BranchName"
fi

if [ -d Directories ]; then
	if [ ! "$Quiet" ]; then echo "[ Directories already exists ]"; fi
else
	if [ ! "$Quiet" ]; then echo "[ creating Directories ]"; fi
	rm -rf Directories
	mkdir Directories

	if [ ! "$Quiet" ]; then echo "[ linking Directories/RCSconfig to ../RCSCONFIG ]"; fi
	ln -s ../RCSCONFIG Directories/RCSconfig

	if [ ! "$Quiet" ]; then echo "[ linking Directories/RCS to ../RCS/Directories ]"; fi
	ln -s ../RCS/Directories Directories/RCS

	if [ ! "$Quiet" ]; then echo "[ checking out Directories ]"; fi
	(cd Directories; rcsco -q RCS/*,v)
fi

if [ "$*" ]; then
	Dirs="$Dirs `(cd Directories; cat $@) | egrep -v '^#'`"
fi

if [ ! "$Dirs" ]; then
	Dirs="."
fi

echo "$Dirs" | tr -s ' ' '\12' | sort -u | awk '
{
  if (NF == 0) next;

  dots=""; dir = $1;
  for (i=0; i < length(dir); i++)
    if (substr(dir,i,1) == "/")
      dots = "../" dots;
  if (dots == "")
    dots = "/";
  if (substr(dir,1,2) == "./")
    dir = substr(dir,3,length(dir)-2);

  printf("%s %s\n", dots, dir);
}' | while read dots dir; do
	if [ -d "$dir" ]; then
		if [ "$dir" != "." ]; then
			if [ ! "$Quiet" ]; then echo "[ $dir already exists ]"; fi
		fi
	else
		if [ ! "$Quiet" ]; then echo "[ creating $dir ]"; fi
		rm -rf "$dir"
		mkdir "$dir"

		if [ ! "$Quiet" ]; then echo "[ linking $dir/RCSconfig to ${dots}RCSCONFIG ]"; fi
		ln -s "${dots}RCSCONFIG" "$dir/RCSconfig"

		if [ ! "$Quiet" ]; then echo "[ linking $dir/RCS to ${dots}RCS/$dir ]"; fi
		ln -s "${dots}RCS/$dir" "$dir/RCS"
	fi

	if [ "$CheckOut" ]; then
		(cd "$dir" &&
		 if [ ! -d RCS ]; then
			echo "$ProgName: WARNING: no access to $dir" 1>&2
			if [ ! "$Quiet" ]; then echo "[ skipping check out of $dir ]"; fi
		 elif [ "`echo RCS/*,v`" = "RCS/*,v" ]; then
			if [ ! "$Quiet" ]; then echo "[ no *,v files in $dir ]"; fi
		 else
			if [ ! "$Quiet" ]; then echo "[ checking out $dir ]"; fi
			rcsco -q RCS/*,v
		 fi)
	else
		if [ ! "$Quiet" ]; then echo "[ skipping check out of $dir ]"; fi
	fi
done

if [ -f RCSlog ]; then
	if [ ! "$Quiet" ]; then echo "[ removing RCSlog ]"; fi
	rm -f RCSlog
fi
if [ -f RCSset ]; then
	if [ ! "$Quiet" ]; then echo "[ removing RCSset ]"; fi
	rm -f RCSset
fi

OBJS=../../obj
if [ ! -d "$OBJS" ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$OBJS" ]"; fi
	rm -rf "$OBJS"
	mkdir "$OBJS"
fi

if [ ! -d "$OBJS"/vax_mach ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$OBJS"/vax_mach ]"; fi
	rm -rf "$OBJS"/vax_mach
	mkdir "$OBJS"/vax_mach
fi
if [ ! -d "$OBJS"/sun3_mach ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$OBJS"/sun3_mach ]"; fi
	rm -rf "$OBJS"/sun3_mach
	mkdir "$OBJS"/sun3_mach
fi
if [ ! -d "$OBJS"/pmax_mach ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$OBJS"/pmax_mach ]"; fi
	rm -rf "$OBJS"/pmax_mach
	mkdir "$OBJS"/pmax_mach
fi
if [ ! -d "$OBJS"/i386_mach ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$OBJS"/i386_mach ]"; fi
	rm -rf "$OBJS"/i386_mach
	mkdir "$OBJS"/i386_mach
fi
if [ ! -d "$OBJS"/vax_mach/latest ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$OBJS"/vax_mach/latest ]"; fi
	rm -rf "$OBJS"/vax_mach/latest
	mkdir "$OBJS"/vax_mach/latest
fi
if [ ! -d "$OBJS"/sun3_mach/latest ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$OBJS"/sun3_mach/latest ]"; fi
	rm -rf "$OBJS"/sun3_mach/latest
	mkdir "$OBJS"/sun3_mach/latest
fi
if [ ! -d "$OBJS"/pmax_mach/latest ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$OBJS"/pmax_mach/latest ]"; fi
	rm -rf "$OBJS"/pmax_mach/latest
	mkdir "$OBJS"/pmax_mach/latest
fi
if [ ! -d "$OBJS"/i386_mach/latest ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$OBJS"/i386_mach/latest ]"; fi
	rm -rf "$OBJS"/i386_mach/latest
	mkdir "$OBJS"/i386_mach/latest
fi
if [ ! -d "$OBJS"/vax_mach/latest/kernel ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$OBJS"/vax_mach/latest/kernel ]"; fi
	rm -rf "$OBJS"/vax_mach/latest/kernel
	mkdir "$OBJS"/vax_mach/latest/kernel
fi
if [ ! -d "$OBJS"/sun3_mach/latest/kernel ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$OBJS"/sun3_mach/latest/kernel ]"; fi
	rm -rf "$OBJS"/sun3_mach/latest/kernel
	mkdir "$OBJS"/sun3_mach/latest/kernel
fi
if [ ! -d "$OBJS"/pmax_mach/latest/kernel ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$OBJS"/pmax_mach/latest/kernel ]"; fi
	rm -rf "$OBJS"/pmax_mach/latest/kernel
	mkdir "$OBJS"/pmax_mach/latest/kernel
fi
if [ ! -d "$OBJS"/i386_mach/latest/kernel ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$OBJS"/i386_mach/latest/kernel ]"; fi
	rm -rf "$OBJS"/i386_mach/latest/kernel
	mkdir "$OBJS"/i386_mach/latest/kernel
fi
if [ ! -d "$OBJS"/vax_mach/latest/user ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$OBJS"/vax_mach/latest/user ]"; fi
	rm -rf "$OBJS"/vax_mach/latest/user
	mkdir "$OBJS"/vax_mach/latest/user
fi
if [ ! -d "$OBJS"/sun3_mach/latest/user ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$OBJS"/sun3_mach/latest/user ]"; fi
	rm -rf "$OBJS"/sun3_mach/latest/user
	mkdir "$OBJS"/sun3_mach/latest/user
fi
if [ ! -d "$OBJS"/pmax_mach/latest/user ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$OBJS"/pmax_mach/latest/user ]"; fi
	rm -rf "$OBJS"/pmax_mach/latest/user
	mkdir "$OBJS"/pmax_mach/latest/user
fi
if [ ! -d "$OBJS"/i386_mach/latest/user ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$OBJS"/i386_mach/latest/user ]"; fi
	rm -rf "$OBJS"/i386_mach/latest/user
	mkdir "$OBJS"/i386_mach/latest/user
fi
if [ ! -d "$OBJS"/vax_mach/latest/user/libmach ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$OBJS"/vax_mach/latest/user/libmach ]"; fi
	rm -rf "$OBJS"/vax_mach/latest/user/libmach
	mkdir "$OBJS"/vax_mach/latest/user/libmach
fi
if [ ! -d "$OBJS"/sun3_mach/latest/user/libmach ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$OBJS"/sun3_mach/latest/user/libmach ]"; fi
	rm -rf "$OBJS"/sun3_mach/latest/user/libmach
	mkdir "$OBJS"/sun3_mach/latest/user/libmach
fi
if [ ! -d "$OBJS"/pmax_mach/latest/user/libmach ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$OBJS"/pmax_mach/latest/user/libmach ]"; fi
	rm -rf "$OBJS"/pmax_mach/latest/user/libmach
	mkdir "$OBJS"/pmax_mach/latest/user/libmach
fi
if [ ! -d "$OBJS"/i386_mach/latest/user/libmach ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$OBJS"/i386_mach/latest/user/libmach ]"; fi
	rm -rf "$OBJS"/i386_mach/latest/user/libmach
	mkdir "$OBJS"/i386_mach/latest/user/libmach
fi
if [ ! -d "$OBJS"/vax_mach/latest/user/threads ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$OBJS"/vax_mach/latest/user/threads ]"; fi
	rm -rf "$OBJS"/vax_mach/latest/user/threads
	mkdir "$OBJS"/vax_mach/latest/user/threads
fi
if [ ! -d "$OBJS"/sun3_mach/latest/user/threads ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$OBJS"/sun3_mach/latest/user/threads ]"; fi
	rm -rf "$OBJS"/sun3_mach/latest/user/threads
	mkdir "$OBJS"/sun3_mach/latest/user/threads
fi
if [ ! -d "$OBJS"/pmax_mach/latest/user/threads ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$OBJS"/pmax_mach/latest/user/threads ]"; fi
	rm -rf "$OBJS"/pmax_mach/latest/user/threads
	mkdir "$OBJS"/pmax_mach/latest/user/threads
fi
if [ ! -d "$OBJS"/i386_mach/latest/user/threads ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$OBJS"/i386_mach/latest/user/threads ]"; fi
	rm -rf "$OBJS"/i386_mach/latest/user/threads
	mkdir "$OBJS"/i386_mach/latest/user/threads
fi

RELS=../../release
if [ ! -d "$RELS" ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$RELS" ]"; fi
	rm -rf "$RELS"
	mkdir "$RELS"
fi

if [ ! -d "$RELS"/vax_mach ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$RELS"/vax_mach ]"; fi
	rm -rf "$RELS"/vax_mach
	mkdir "$RELS"/vax_mach
fi
if [ ! -d "$RELS"/sun3_mach ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$RELS"/sun3_mach ]"; fi
	rm -rf "$RELS"/sun3_mach
	mkdir "$RELS"/sun3_mach
fi
if [ ! -d "$RELS"/pmax_mach ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$RELS"/pmax_mach ]"; fi
	rm -rf "$RELS"/pmax_mach
	mkdir "$RELS"/pmax_mach
fi
if [ ! -d "$RELS"/i386_mach ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$RELS"/i386_mach ]"; fi
	rm -rf "$RELS"/i386_mach
	mkdir "$RELS"/i386_mach
fi
if [ ! -d "$RELS"/vax_mach/latest ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$RELS"/vax_mach/latest ]"; fi
	rm -rf "$RELS"/vax_mach/latest
	mkdir "$RELS"/vax_mach/latest
fi
if [ ! -d "$RELS"/sun3_mach/latest ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$RELS"/sun3_mach/latest ]"; fi
	rm -rf "$RELS"/sun3_mach/latest
	mkdir "$RELS"/sun3_mach/latest
fi
if [ ! -d "$RELS"/pmax_mach/latest ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$RELS"/pmax_mach/latest ]"; fi
	rm -rf "$RELS"/pmax_mach/latest
	mkdir "$RELS"/pmax_mach/latest
fi
if [ ! -d "$RELS"/i386_mach/latest ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$RELS"/i386_mach/latest ]"; fi
	rm -rf "$RELS"/i386_mach/latest
	mkdir "$RELS"/i386_mach/latest
fi
if [ ! -d "$RELS"/vax_mach/latest/lib ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$RELS"/vax_mach/latest/lib ]"; fi
	rm -rf "$RELS"/vax_mach/latest/lib
	mkdir "$RELS"/vax_mach/latest/lib
fi
if [ ! -d "$RELS"/sun3_mach/latest/lib ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$RELS"/sun3_mach/latest/lib ]"; fi
	rm -rf "$RELS"/sun3_mach/latest/lib
	mkdir "$RELS"/sun3_mach/latest/lib
fi
if [ ! -d "$RELS"/pmax_mach/latest/lib ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$RELS"/pmax_mach/latest/lib ]"; fi
	rm -rf "$RELS"/pmax_mach/latest/lib
	mkdir "$RELS"/pmax_mach/latest/lib
fi
if [ ! -d "$RELS"/i386_mach/latest/lib ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$RELS"/i386_mach/latest/lib ]"; fi
	rm -rf "$RELS"/i386_mach/latest/lib
	mkdir "$RELS"/i386_mach/latest/lib
fi
if [ ! -d "$RELS"/vax_mach/latest/bin ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$RELS"/vax_mach/latest/bin ]"; fi
	rm -rf "$RELS"/vax_mach/latest/bin
	mkdir "$RELS"/vax_mach/latest/bin
fi
if [ ! -d "$RELS"/sun3_mach/latest/bin ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$RELS"/sun3_mach/latest/bin ]"; fi
	rm -rf "$RELS"/sun3_mach/latest/bin
	mkdir "$RELS"/sun3_mach/latest/bin
fi
if [ ! -d "$RELS"/pmax_mach/latest/bin ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$RELS"/pmax_mach/latest/bin ]"; fi
	rm -rf "$RELS"/pmax_mach/latest/bin
	mkdir "$RELS"/pmax_mach/latest/bin
fi
if [ ! -d "$RELS"/i386_mach/latest/bin ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$RELS"/i386_mach/latest/bin ]"; fi
	rm -rf "$RELS"/i386_mach/latest/bin
	mkdir "$RELS"/i386_mach/latest/bin
fi
if [ ! -d "$RELS"/vax_mach/latest/include ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$RELS"/vax_mach/latest/include ]"; fi
	rm -rf "$RELS"/vax_mach/latest/include
	mkdir "$RELS"/vax_mach/latest/include
fi
if [ ! -d "$RELS"/sun3_mach/latest/include ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$RELS"/sun3_mach/latest/include ]"; fi
	rm -rf "$RELS"/sun3_mach/latest/include
	mkdir "$RELS"/sun3_mach/latest/include
fi
if [ ! -d "$RELS"/pmax_mach/latest/include ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$RELS"/pmax_mach/latest/include ]"; fi
	rm -rf "$RELS"/pmax_mach/latest/include
	mkdir "$RELS"/pmax_mach/latest/include
fi
if [ ! -d "$RELS"/i386_mach/latest/include ]; then
	if [ ! "$Quiet" ]; then echo "[ creating "$RELS"/i386_mach/latest/include ]"; fi
	rm -rf "$RELS"/i386_mach/latest/include
	mkdir "$RELS"/i386_mach/latest/include
fi
if [ ! -d RCSTMP ]; then
	if [ ! "$Quiet" ]; then echo "[ creating RCSTMP ]"; fi
	rm -rf RCSTMP
	mkdir RCSTMP
	chmod a+w RCSTMP
fi

cd kernel
if [ ! "$Quiet" ]; then echo "[ creating kernel/machine ]"; fi
rm -rf machine
ln -s @sys machine
ln -s vax vax_mach
ln -s sun3 sun3_mach
ln -s mips pmax_mach
ln -s i386 i386_mach

cd mach
if [ ! "$Quiet" ]; then echo "[ creating kernel/mach/machine ]"; fi
rm -rf machine
ln -s @sys machine
ln -s vax vax_mach
ln -s sun3 sun3_mach
ln -s mips pmax_mach
ln -s i386 i386_mach

cd ../../user/libmach
if [ ! "$Quiet" ]; then echo "[ creating user/libmach/machine ]"; fi
rm -rf machine
ln -s @sys machine
ln -s vax vax_mach
ln -s sun3 sun3_mach
ln -s mips pmax_mach
ln -s i386 i386_mach

cd ../threads
if [ ! "$Quiet" ]; then echo "[ creating user/threads/machine ]"; fi
rm -rf machine
ln -s @sys machine
ln -s vax vax_mach
ln -s sun3 sun3_mach
ln -s mips pmax_mach
ln -s i386 i386_mach

cd ../..

exit 0
