#!/usr/bin/env bash

set -e
set -x
shopt -s dotglob

readonly name="metaio"
readonly ownership="MetaIO Upstream <kwrobot@kitware.com>"
readonly subtree="Utilities/MetaIO/vtk$name"
readonly update="Utilities/MetaIO/update.sh"
readonly repo="https://github.com/Kitware/MetaIO.git"
readonly tag="master"
readonly paths="
src/*.*
src/doc
License.txt
"

readonly basehash='fc3537673c92fe781cb9ab0242ee95cba2f87b0e' # NEWHASH

extract_source () {
    git_archive
    pushd "$extractdir/$name-reduced"
    mv -v src/* .
    rmdir src/
    popd
}

. "${BASH_SOURCE%/*}/../../ThirdParty/update-common.sh"
