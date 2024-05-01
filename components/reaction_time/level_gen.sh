#!/usr/bin/env bash

buttons=("BUTTON_UP" "BUTTON_DOWN" "BUTTON_LEFT" "BUTTON_RIGHT")
prev_index=-1
output=""

for _ in {1..16}; do
    while :; do
        index=$((RANDOM % 4))
        if [[ "$index" -ne "$prev_index" ]]; then
            output+="${output:+, }${buttons[$index]}"
            prev_index=$index
            break
        fi
    done
done

echo "{ $output }"
