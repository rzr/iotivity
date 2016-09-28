#!/bin/sh

set -f

[ ! -z ${EXEC_MODE} ] || EXEC_MODE=true

topdir="${PWD}"

do_()
{
    set +f
    if $EXEC_MODE; then
        echo "warning: fetching online resources may not be reproductible"
        printf "%s\n" "trying: \"$@\""
        eval "$@"
    else
        echo "# Please manually execute from shell:"
        printf "%s\n" "$@"
    fi
    sleep 5
    set -f
}


main_()
{
    cat<<EOF
#
# WARNING: This helper script is DISCOURAGED to be used
#
#  Instead please use the supported build tool "scons"
#
#  However in some specific cases it's not possible
#  so this script is providing a fallback option.
#
#  Reminder: it's not safe to access online resources at build time,
#  so by default the script will tell what to do (like scons),
#  to enable tasks executions it should be run with EXEC_MODE enabled:
#
#  EXEC_MODE=true ./prep.sh
#
#  More details at:
#  https://wiki.iotivity.org/build
#
EOF

    which git > /dev/null
    which unzip > /dev/null
    which wget > /dev/null

    echo "# Checking for tinycbor presence:"
    tinycbor_url='https://github.com/01org/tinycbor.git'
    tinycbor_rev='v0.3.1'
    if [ ! -e 'extlibs/tinycbor/tinycbor' ] ; then
        do_ "git clone --depth 1 -b "${tinycbor_rev}" "$tinycbor_url" extlibs/tinycbor/tinycbor"
    fi

    echo "# Checking for gtest presence:"
    gtest_url='http://pkgs.fedoraproject.org/repo/pkgs/gtest/gtest-1.7.0.zip/2d6ec8ccdf5c46b05ba54a9fd1d130d7/gtest-1.7.0.zip'
    gtest_dir="${topdir}/extlibs/gtest/"
    gtest_archive=$(basename -- "${gtest_url}")
    gtest_file="${gtest_dir}/gtest-1.7.0/CMakeLists.txt"
    if [ ! -e "${gtest_file}" ] ; then
        do_ "cd ${gtest_dir} && wget -nc -O ${gtest_archive} ${gtest_url} && unzip ${gtest_archive}"
        cd "${topdir}"
    fi

    echo "# Checking for sqlite presence:"
    sqlite_url='http://www.sqlite.org/2015/sqlite-amalgamation-3081101.zip'
    sqlite_dir="${topdir}/extlibs/sqlite3"
    sqlite_file="${sqlite_dir}/sqlite3.c"
    sqlite_archive=$(basename -- "${sqlite_url}")
    if [ ! -e 'extlibs/sqlite3/$sqlite_file' ] ; then
        do_ "cd ${sqlite_dir} && wget -nc $sqlite_url && unzip ${sqlite_archive} && mv */* ."
        cd "${topdir}"
    fi

    echo "# Checking for mbedtls presence:"
    mbedtls_url='https://github.com/ARMmbed/mbedtls.git'
    mbedtls_dir="${topdir}/extlibs/mbedtls/mbedtls"
    mbedtls_rev="ad249f509fd62a3bbea7ccd1fef605dbd482a7bd" # in "yotta-2.3.2"
    if [ ! -e "${mbedtls_dir}" ] ; then
        do_ "git clone ${mbedtls_url} ${mbedtls_dir}"
    fi
    cd "${mbedtls_dir}"
    do_ "git reset --hard ${mbedtls_rev}"
    echo "# Checking for mbedtls patches:"
    file="${mbedtls_dir}/../ocf.patch"
    do_ "git apply ${file}" ||:
    cd "${topdir}"
}


main_

