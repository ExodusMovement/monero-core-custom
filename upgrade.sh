#!/usr/bin/env sh

# changes
# removed vtlogger (it got replaced by easylogging++)

# aliases
# epee -> contrib/epee

upstream=/Users/sekhmet/Workspace/monero-cpp

prepare_v0_15_0_0() {
    rm -rf vtlogger
    mv common/int-util.h epee/include
    touch epee/src/int-util.cpp
    mkdir epee/include/storages
    touch epee/include/storages/parserse_base_utils.h
    touch rpc/rpc_handler.h
}

list_missing() {
    for f in $(find . -name '*.c' -or -name '*.cc' -or -name '*.cpp' -or -name '*.h' -or -name '*.hpp'); do
        upstream_file=$upstream/src/$f

        if [[ $f == ./contrib* ]]; then
            continue
        fi

        if [[ $f == ./patches* ]]; then
            continue
        fi

        if [[ $f == ./epee* ]]; then
            base=$(echo $f | cut -d'/' -f3-)
            upstream_file=$upstream/contrib/epee/$base
        fi

        if [[ ! -e $upstream_file ]]; then
            echo "Not exists $f"
        fi
    done
}

list_identical() {
    for f in $(find . -name '*.c' -or -name '*.cc' -or -name '*.cpp' -or -name '*.h' -or -name '*.hpp'); do
        upstream_file=$upstream/src/$f

        if [[ $f == ./contrib* ]]; then
            continue
        fi

        if [[ $f == ./epee* ]]; then
            base=$(echo $f | cut -d'/' -f3-)
            upstream_file=$upstream/contrib/epee/$base
        fi

        if [[ -e $upstream_file ]]; then
            sum=$(shasum $f | awk '{ print $1 '})
            upstream_sum=$(shasum $upstream_file | awk '{ print $1 }')

            if [[ $sum == $upstream_sum ]]; then
                echo "File $f is identical. Skipping."
            fi
        fi
    done
}

upgrade() {
    for f in $(find . -name '*.c' -or -name '*.cc' -or -name '*.cpp' -or -name '*.h' -or -name '*.hpp'); do
        upstream_file=$upstream/src/$f

        if [[ $f == ./contrib* ]]; then
            continue
        fi

        if [[ $f == ./epee* ]]; then
            base=$(echo $f | cut -d'/' -f3-)
            upstream_file=$upstream/contrib/epee/$base
        fi

        if [[ -e $upstream_file ]]; then
            sum=$(shasum $f | awk '{ print $1 '})
            upstream_sum=$(shasum $upstream_file | awk '{ print $1 }')

            if [[ $sum != $upstream_sum ]]; then
                echo "File $f is different. Patching."
                cp $upstream_file $f
            fi
        fi
    done
}

patch() {
    cp -R patches/* .
}

upgrade_and_patch() {
    upgrade
    patch
}

"$@"
