#!/bin/sh
# @(#$Id: makesrcdist.sh,v 1.1 2009-01-20 13:48:34 dockes Exp $  (C) 2005 J.F.Dockes
# A shell-script to make a pxattr source distribution

fatal()
{
	echo $*
	exit 1
}
#set -x

TAR=/usr/bin/tar
SVNREPOS=svn+ssh://y/home/subversion/pxattr/

version=`cat VERSION`
versionforcvs=`echo $version | sed -e 's/\./_/g'`

editedfiles=`svn status | egrep -v '^\?'`
if test ! -z "$editedfiles"; then
	fatal  "Edited files exist: " $editedfiles
fi

tagtop() {
  svn copy -m "Release $version tagged" . $SVNREPOS/tags/$1 || \
	    	fatal tag failed
}

targetdir=${targetdir-/tmp}
dotag=${dotag-yes}

if test ! -f pxattr.cpp ; then
    echo "Should be executed in the master pxattr directory"
    exit 1
fi

topdir=$targetdir/pxattr-$version
if test ! -d $topdir ; then
    mkdir $topdir || exit 1
else 
    echo "Removing everything under $topdir Ok ? (y/n)"
    read rep 
    if test $rep = 'y';then
    	rm -rf $topdir/*
    fi
fi

# Clean up this dir and copy the dist-specific files 
make clean
yes | clean.O

$TAR chfX - excludefile .  | (cd $topdir;$TAR xf -)

TAG="RECOLL_$versionforcvs"
[ $dotag = "yes" ] && tagtop $TAG

out=pxattr-$version.tar.gz
(cd $targetdir ; \
    $TAR chf - pxattr-$version | \
    	gzip > $out)
echo "$targetdir/$out created"
