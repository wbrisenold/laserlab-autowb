#!/bin/bash
# Build and install LaserLab OFX plugin
set -euo pipefail
cd "$(dirname "$0")/.."

echo "=== LaserLab OFX Build & Install ==="
echo ""

echo "[1/3] Building..."
make clean all

echo ""
echo "[2/3] Installing..."
sudo mkdir -p /Library/OFX/Plugins
sudo rm -rf /Library/OFX/Plugins/LaserLab.ofx.bundle
sudo cp -fr LaserLab.ofx.bundle /Library/OFX/Plugins/
rm -rf "$HOME/Library/Application Support/Blackmagic Design/DaVinci Resolve/OFX Plugins/LaserLab.ofx.bundle" 2>/dev/null || true

echo ""
echo "=== Done ==="
echo "Installed to /Library/OFX/Plugins/LaserLab.ofx.bundle"
echo "Restart DaVinci Resolve to load."
