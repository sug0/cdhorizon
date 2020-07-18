#!/bin/sh

usage() {
    echo Usage: $0 '[build|clean]'
}

case $1 in
    build)
        if ! command -v tup; then
            echo Error: You need tup installed to build: http://gittup.org/tup/
            exit 1
        fi
        tup init 2>/dev/null
        tup
        ;;
    clean)
        rm -f *.o libglitch.a
        ;;
    *)
        usage
        ;;
esac
