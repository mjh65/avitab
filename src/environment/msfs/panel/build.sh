#!/usr/bin/env bash

# script to build and package the MSFS ingame panel
# requires a number of tools not easily available from windows cmd files

# usage build.sh -b builddir -i installdir

# defaults before parsing options

builddir="$(readlink -fm "$(pwd)")"
installdir="$(readlink -fm "$(pwd)")"

die() {
    printf '%s\n' "$1" >&2
    exit 1
}
while :; do
    case $1 in
        -b|--builddir)
            if [ "$2" ]; then
                builddir=$2
                shift
            else
                die 'ERROR: "--builddir" requires a non-empty option argument.'
            fi
            ;;
        --buildir==?*)
            buildir=${1#*=}
            ;;
        -i|--installdir)
            if [ "$2" ]; then
                installdir=$2
                shift
            else
                die 'ERROR: "--installdir" requires a non-empty option argument.'
            fi
            ;;
        --buildir==?*)
            buildir=${1#*=}
            ;;
        -?*)
            printf 'WARN: Unknown option (ignored): %s\n' "$1" >&2
            ;;
        *)
            break
    esac
    shift
done

#

pkgname=avitab-ingamepanels-avitab
basedir="$(dirname "$(readlink -fm "${BASH_SOURCE[0]}")")"
workdir="$(mktemp -p . -d -t workXXXX)"
pkgdir="${installdir}/${pkgname}"

# create a build area in the workdir and populate from the sources

mkdir -p "${workdir}/build/Packages/${pkgname}"
touch "${workdir}/build/Packages/${pkgname}/layout.json"
cp -r "${basedir}/spb"/* "${workdir}/build"
cp -r "${basedir}/html_ui" "${workdir}/build/PackageSources"
mkdir -p "${workdir}/build/PackageSources/html_ui/Textures/Menu/toolbar"
cp "${workdir}/build/PackageSources/html_ui/icons/toolbar"/*.svg "${workdir}/build/PackageSources/html_ui/Textures/Menu/toolbar"

# use the MSFS SDK package tool to build the SPB and json files

echo "Building SPB"

"$(cygpath -u ${MSFS_SDK})Tools/bin/fspackagetool.exe" "${workdir}/build/${pkgname}.xml" -nopause
fsptx=$?

if [ ! -f ${workdir}/build/Packages/${pkgname}/Build/${pkgname}.spb ]; then
    echo "SPB file not found, looks like fspackagetool failed."
    echo "-fspackagetool completed with code $fsptx"
    echo "-try (in cmd): ${MSFS_SDK}Tools\\bin\\fspackagetool.exe ${workdir}/build/${pkgname}.xml"
    exit 1
fi

# Assemble the installable package by copying files and modifying json

echo "Assembling package"

mkdir -p "${pkgdir}/InGamePanels"
cp "${workdir}/build/Packages/${pkgname}/Build/${pkgname}.spb" "${pkgdir}/InGamePanels"
cp -r "${workdir}/build/Packages/${pkgname}/Build/html_ui" "${pkgdir}"
cp "${workdir}/build/Packages/${pkgname}"/*.json "${pkgdir}"
sed -i -e '/spb/s/Build/InGamePanels/' "${pkgdir}/layout.json"
sed -i -e 's/Build\///' "${pkgdir}/layout.json"

# Install into community folder if PANELINSTALLDIR env var is set

if [[ -n "$PANELINSTALLDIR" ]] && [[ -d "$PANELINSTALLDIR" ]]; then
    echo "Installing package to $PANELINSTALLDIR"
    rm -rf "$PANELINSTALLDIR/${pkgname}"
    cp -r "${pkgdir}" "$PANELINSTALLDIR"
fi

echo "Cleaning up"

rm -rf $workdir
