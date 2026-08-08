#!/bin/bash
# Build script for LaserLab OFX plugin
set -euo pipefail
cd "$(dirname "$0")/.."

echo "=== LaserLab OFX Build ==="
echo "[1/2] Building..."
make clean all
echo "[2/2] Installing..."
sudo mkdir -p /Library/OFX/Plugins
sudo rm -rf /Library/OFX/Plugins/LaserLab.ofx.bundle
sudo cp -fr LaserLab.ofx.bundle /Library/OFX/Plugins/
rm -rf "$HOME/Library/Application Support/Blackmagic Design/DaVinci Resolve/OFX Plugins/LaserLab.ofx.bundle" 2>/dev/null || true
echo "=== Done. Restart Resolve. ==="
