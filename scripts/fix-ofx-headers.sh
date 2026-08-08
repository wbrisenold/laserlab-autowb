#!/bin/bash
# Fix throw() specification mismatches in OFX Support headers for C++14
set -euo pipefail
cd "$(dirname "$0")/.."
echo "Fixing exception specifications in all headers..."
for f in ofx/Support/include/ofxsMemory.h ofx/Support/include/ofxsImageEffect.h ofx/Support/include/ofxsParam.h ofx/Support/include/ofxsProperty.h ofx/Support/include/ofxsPropertyValidation.h; do
    if [ -f "$f" ]; then
        sed -i '' 's/throw(std::invalid_argument)//g; s/throw(std::bad_alloc)//g; s/throw()//g' "$f"
        echo "  Fixed $f"
    fi
done
echo "Done."
