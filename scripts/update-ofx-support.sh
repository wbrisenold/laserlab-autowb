#!/bin/bash
# Update OFX Support library from BaldavengerOFX (has Metal GPU support)
set -euo pipefail
cd "$(dirname "$0")/.."

echo "=== Updating OFX Support Library ==="
REPO="https://raw.githubusercontent.com/baldavenger/BaldavengerOFX/refs/heads/master/openfx/Support"

# Download all files that exist
FILES=(
    "include/ofxsProcessing.h"
    "include/ofxsImageEffect.h"
    "include/ofxsMultiThread.h"
    "include/ofxsParam.h"
    "include/ofxsMemory.h"
    "include/ofxsLog.h"
    "include/ofxsMessage.h"
    "include/ofxsProperty.h"
    "include/ofxsPropertyValidation.h"
    "include/ofxsImageBlender.H"
    "include/ofxsHWNDInteract.h"
    "include/ofxsSupportPrivate.h"
    "Library/ofxsCore.cpp"
    "Library/ofxsImageEffect.cpp"
    "Library/ofxsInteract.cpp"
    "Library/ofxsLog.cpp"
    "Library/ofxsMultiThread.cpp"
    "Library/ofxsParams.cpp"
    "Library/ofxsProperty.cpp"
    "Library/ofxsPropertyValidation.cpp"
)

for f in "${FILES[@]}"; do
    url="$REPO/$f"
    dest="ofx/$f"
    mkdir -p "$(dirname "$dest")"
    if curl -fsSL --fail "$url" -o "$dest" 2>/dev/null; then
        echo "  OK: $f"
    else
        echo "  404: $f"
    fi
done

echo "=== Done ==="
echo "Metal support check:"
grep -c "processImagesMetal\|_pMetalCmdQ\|setGPURenderArgs" ofx/Support/include/ofxsProcessing.h || echo "  NOT FOUND"
