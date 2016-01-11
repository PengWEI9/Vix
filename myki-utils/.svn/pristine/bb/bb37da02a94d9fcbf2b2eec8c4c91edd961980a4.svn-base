
RELEASE_BASE=/afc/release
CURRENT_RELEASE_VERSION=""
if [ -f ${RELEASE_BASE}/installed_release_version ]; then
	CURRENT_RELEASE_VERSION=`cat ${RELEASE_BASE}/installed_release_version|sed 's/^[^.]\+\.\([^ ]\+\) .*$/\1/'`
fi

MANIFEST_FILE=${RELEASE_BASE}/manifests/manifest_${CURRENT_RELEASE_VERSION}


HAVE_MISMATCHES=0
while read line
do
	linetype=`echo $line|sed 's/:.*$//'`
	if echo $line|egrep -q '^ *#'; then
		continue
	fi
	if [ "$linetype" = "header" ]; then
		RELEASE_VERSION=`echo $line|sed 's/.*VIX_RELEASE_\([^ ]\+\) .*$/\1/'`
		if [ "$RELEASE_VERSION" != "$CURRENT_RELEASE_VERSION" ]; then
			echo "FATAL: release version in manifest header differs from the one in ${RELEASE_BASE}/available_release_version"
			exit 128
		fi
		continue
	fi
	PKG_NAME=`echo $line|cut -d , -f 2`
	MANIFEST_VER=`echo $line|cut -d , -f 3`
	INSTALLED_VER=`dpkg -s $PKG_NAME|egrep '^Version:'|sed 's/Version:  *//'`
	PLATFORM=`echo $line|cut -d , -f 4`
	PKG_FILE=`echo $line|cut -d , -f 6`
	INSTALLED_VER=`dpkg -s $PKG_NAME|egrep '^Version:'|sed 's/Version:  *//'`
	if [ "$MANIFEST_VER" != "$INSTALLED_VER" ]; then
		echo "ERROR: $PKG_NAME installed version is $INSTALLED_VER, manifest listed $MANIFEST_VER"
		HAVE_MISMATCHES=$((MISMATCHES+1))
	fi
done < $MANIFEST_FILE
echo "Current installed release is $CURRENT_RELEASE_VERSION, $HAVE_MISMATCHES mismatched versions found"
