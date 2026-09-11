#!/bin/sh

function add_extension {
    for f in *; do
        [ -f "$f" ] && mv "$f" "$f.bar"
    done
}

function add_extension_to_extensionless_files {
    for f in *; do
        if [ -f "$f" ] && [[ "$f" != *.* ]]; then
            mv "$f" "$f.log"
        fi
    done
}

# remove .foo and append .bar
function replace_extension {
    for f in *.foo; do
        [ -f "$f" ] && mv "$f" "${f%.foo}.bar"
    done
}