#!/bin/bash
# Restore original OFX Support source files, keep only updated ofxsProcessing.h
set -euo pipefail
cd "$(dirname "$0")/.."

echo "=== Restoring original source files ==="
# Download original source files from BaldavengerOFX's fork of the standard OFX Support lib
# These are the files that match our original headers
BASE="https://raw.githubusercontent.com/AcademySoftwareFoundation/openfx/3de640d6f645fe6e346acd57e568d8b0a5ae4574/Support/Library"
for f in ofxsCore.cpp ofxsImageEffect.cpp ofxsInteract.cpp ofxsLog.cpp ofxsMultiThread.cpp ofxsParams.cpp ofxsProperty.cpp ofxsPropertyValidation.cpp; do
    curl -fsSL "$BASE/$f" -o "ofx/Support/Library/$f" 2>/dev/null && echo "  Restored $f" || echo "  Kept $f"
done

echo "=== Fixing exception specs ==="
sed -i '' 's/throw(std::invalid_argument)//g; s/throw(std::bad_alloc)//g; s/throw()//g' "ofx/Support/include/ofxsProcessing.h" 2>/dev/null || true
sed -i '' 's/throw(std::invalid_argument)//g; s/throw(std::bad_alloc)//g; s/throw()//g' "ofx/Support/include/ofxsMemory.h" 2>/dev/null || true
sed -i '' 's/throw(std::invalid_argument)//g; s/throw(std::bad_alloc)//g; s/throw()//g' "ofx/Support/include/ofxsImageEffect.h" 2>/dev/null || true

echo "=== Metal support check ==="
grep -c "processImagesMetal\|_pMetalCmdQ\|setGPURenderArgs" ofx/Support/include/ofxsProcessing.h
echo "Done."
