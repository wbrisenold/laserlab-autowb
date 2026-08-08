# LaserLab Auto WB — GPU-first Auto White Balance OFX Plugin

> **⚡ Vibe coded** — this plugin was built through conversational iteration with an AI assistant. It works, but treat it as a capable prototype, not production-hardened software.

## What it does

One-shot Auto White Balance for DaVinci Resolve (Studio) that:
- **Analyzes the current frame** on GPU (Metal) for skin tones
- **Solves CAT16 chromatic adaptation** temp/tint so skin lands on the vector-scope skin-tone line (1.0685 rad)
- **Writes the solved Temp/Tint into the plugin sliders** — holds until you click **Analyze This Frame** again
- **No temporal tracking** — one frame, one analysis, done
- **Colorspace aware** — dropdowns for Input Gamut / Transfer so it decodes your camera footage correctly

## Why it exists

Auto WB in Resolve is buried. This plugin gives you a single button: click it on a frame with visible skin, and it pushes the whole image so skin sits on the vector-scope skin-tone line using a proper CAT16 chromatic adaptation (not a naive gray-world). The WB sliders move, you see the result instantly, and it stays until you press again.

**Critical workflow note:** Place this **right after your CST** (camera → timeline space). Your look LUTs / film emulations expect a *corrected* image. This plugin corrects the white balance *before* your creative LUTs, so they behave as designed.

## Installation

### Quick (user folder, no sudo)
```bash
cp -R LaserLab.ofx.bundle ~/Library/OFX/Plugins/
```

### System-wide (needs sudo)
```bash
sudo make install
# or
sudo rm -rf /Library/OFX/Plugins/LaserLab.ofx.bundle
sudo cp -R LaserLab.ofx.bundle /Library/OFX/Plugins/
```

Restart Resolve. The plugin appears under **Effects > Color > Auto WB from Skin**.

## Usage

1. Add **Auto WB from Skin** to a node *after* your CST (camera → timeline space)
2. Set **Input → Gamut** and **Input → Transfer** to match your camera footage (e.g., Sony S-Log3 / S-Gamut3)
3. Park on a frame with visible skin
3. Click **Analyze This Frame**
4. Temp/Tint sliders move; skin locks to the vector-scope skin-tone line
5. Scrub freely — WB holds until you click **Analyze This Frame** again

## Parameters

| Page | Parameter | Description |
|------|-----------|-------------|
| **Input** | **Gamut** | Camera capture gamut (ARRI AWG4, Sony S-Gamut3, Rec709, etc.) |
| **Input** | **Transfer** | Camera log encoding (LogC4, S-Log3, V-Log, Rec709, etc.) |
| **White Balance** | **Analyze This Frame** | One-shot GPU skin scan → solves Temp/Tint |
| **White Balance** | **Temp** | CAT16 temperature (holds until next analyze) |
| **White Balance** | **Tint** | CAT16 tint (holds until next analyze) |
| **White Balance** | **Status** | Readout: sample count / solved values / warnings |

## Build

```bash
cd LaserLabOFX
make clean all          # builds LaserLab.ofx (universal arm64+x86_64)
make test               # CPU unit tests (layout, round-trip, solver)
make test_autowb        # GPU Metal scan → solve → transform chain test
make install            # installs to /Library/OFX/Plugins (sudo)
```

## How it works (brief)

1. **Analyze (GPU)** — `AutoWbReduceKernel` dispatches a 128×72 grid over the frame. Each cell:
   - Decodes source pixel via selected Transfer → linear → selected Gamut → XYZ(D65) → AWG4 linear → OKLab
   - Computes skin weight (Luma window × Chroma window × Hue alignment to 1.0685 rad)
   - Accumulates chroma-weighted OKLab `a,b` into a per-cell buffer
2. **Solve (CPU)** — Host reads back the grid, computes chroma-weighted mean OKLab `(L,a,b)`, then `solveSkinTempTint(L,a,b)` runs a coarse+fine grid search over Temp/Tint to minimize angular distance to the skin-tone line (1.0685 rad) using the real CAT16 `white_balance()` path.
3. **Apply (GPU)** — `AutoWbTransformKernel` applies the solved Temp/Tint via `white_balance()` (CAT16) per pixel. No creative grade, just pure WB.

The solver is faithful to the plugin's own transform: it reconstructs the mean skin as linear AWG4, applies `white_balance()` with candidate Temp/Tint, converts back to OKLab, and minimizes angular error to the skin line plus a tiny L2 regularizer on the sliders.

## Colorspace awareness

The plugin **does not auto-detect** colorspace — you must set **Input → Gamut** and **Input → Transfer** to match the footage *as it enters the plugin*. This is by design: Resolve feeds OFX plugins whatever space the node receives. If you place this **after your CST** (camera → timeline), set the dropdowns to your *camera's* native space. The plugin then decodes correctly, measures skin in OKLab, and writes WB that makes sense in that space.

## Testing

```bash
make test               # CPU: layout, round-trips, solver correctness
make test_autowb        # Metal: full GPU chain (scan → solve → transform)
```

`make test` passes all CPU gates (layout, transfer round-trips, direction, solver identity on-line / off-line). `make test_autowb` validates the full GPU chain: synthetic off-line skin → scan → solve → transform → skin lands on the line (herr < 0.05 rad).

## Requirements

- DaVinci Resolve Studio 18.5+ (Free edition has no OFX scripting)
- macOS (Metal GPU path)
- OFX host (Resolve, Nuke, etc. — tested on Resolve)

## License

MIT — do whatever, but it's vibe coded. No warranty.

## Handoff

See [HANDOFF.md](HANDOFF.md) for the full recreation guide, architecture notes, and future work.
