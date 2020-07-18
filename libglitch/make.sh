#!/bin/sh

usage() {
    echo Usage: $0 '[build|clean]'
}

case $1 in
    build)
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
