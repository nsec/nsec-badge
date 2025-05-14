#!/bin/bash

# Store the output of the command in a variable
output=$(JLC2KiCadLib -footprint_lib footprints -symbol_lib_dir symbols -model_dir models -dir . $1)

# Echo the output so you can still see it
echo "$output"

# Extract the footprint file path using grep and pattern matching
footprint_file=$(echo "$output" | grep -o "'./footprints/.*\.kicad_mod'" | tr -d "'")

# Check if footprint_file was found
if [ -n "$footprint_file" ]; then
    echo "Updating footprint file: $footprint_file"
#    sed -i '' 's|./footprints/|\$\{KIPRJMOD\}/library/footprints/|g' "$footprint_file"
    echo "Footprint file updated successfully."
else
    echo "Error: Could not find footprint file path in the output."
fi

