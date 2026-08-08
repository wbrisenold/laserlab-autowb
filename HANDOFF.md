# LaserLab Auto WB — Handoff / Recreation Guide

> **This is a vibe-coded prototype.** Built through conversational AI iteration. It works for the intended use case but has not been battle-tested in production. Read this before extending.

---

## What this plugin is

A **GPU-first Auto White Balance OFX plugin** for DaVinci Resolve (Studio) that:
- Scans the **current frame** on GPU for skin tones
- Solves **CAT16 chromatic adaptation** Temp/Tint so skin lands on the vector-scope skin-tone line (1.0685 rad)
- **Writes the solved Temp/Tint into the plugin's sliders** — values hold across the timeline until you click **Analyze This Frame** again
- **No temporal tracking** — one frame, one analysis, done
- **Colorspace aware** — user-selectable Input Gamut / Transfer so it decodes camera footage correctly

---

## Architecture

### High-level flow

```
User clicks "Analyze This Frame"
         ↓
Next render() call on that frame
         ↓
GPU: AutoWbReduceKernel (128×72 grid)
    - Decode pixel: Transfer → linear → Gamut → XYZ(D65) → AWG4 → OKLab
    - Skin weight: L_w × C_w × H_w (hue alignment to 1.0685 rad)
    - Output per cell: (a·w, b·w, w, L·w) into float4 buffer
         ↓
CPU: Host reads back grid (128×72 × float4 ≈ 150 KB)
    - Chroma-weighted mean (a,b,L)
    - solveSkinTempTint(L, a, b) → temp, tint
         ↓
CPU: setValue() on Temp/Tint params (NO keyframe — value holds forever)
         ↓
GPU: AutoWbTransformKernel (full frame)
    - Decode → Gamut→XYZ(D65) → CAT16 white_balance(temp,tint) → back → encode
```

### Key files

| File | Purpose |
|------|---------|
| `src/AutoWbPlugin.cpp/.h` | OFX plugin entry, UI params, render/analyze logic |
| `src/AutoWbMetalKernel.mm` | Metal harness: compiles `AutoWbKernelSource.h`, runs scan/transform |
| `src/AutoWbKernelSource.h` | **Self-contained** Metal source: CAT16, all TFs, gamuts, reduce + transform kernels |
| `src/laserlab_core.h` | CPU reference: `solveSkinTempTint()`, `analyzeAutoWb()`, all math |
| `src/LaserLabParams.h` | 47-field POD struct (matches Metal/CPU/Metal exactly) |
| `tests/test_autowb.mm` | GPU chain test: synthetic off-line skin → scan → solve → transform → verify on-line |
| `Makefile` | Universal build, test targets, install |

---

## Key design decisions

### 1. Analysis runs in `render()`, not `changedParam()`
**Reason:** OFX only gives a Metal command queue inside `RenderArguments` during `render()`. The button sets `m_NeedAnalyze = true`; the next `render()` on that frame runs the scan, solves, writes sliders via `setValue()` (no keyframe), then proceeds to render with the new WB.

### 2. `setValue()` not `setValueAtTime()`
`setValueAtTime(t, v)` drops a keyframe at `t` only. Scrubbing away → param interpolates to default (0). `setValue(v)` writes a flat value that holds across all frames until the next Analyze click.

### 3. Solver uses the **real WB path**
`solveSkinTempTint(L, a, b)` reconstructs the mean skin as linear AWG4, then for each candidate (temp, tint):
- `xyz_src = g2x(1, oklab_to_work(L, a, b))`
- `xyz = white_balance(xyz_src, temp, tint)`  ← CAT16
- `lab = work_to_oklab(x2g(1, xyz))`
- `herr = angular_distance(atan2(lab.b, lab.a), 1.0685)`
- `score = herr + 0.0025*(temp²+tint²)`

This is **identical** to the plugin's own per-pixel transform (`AutoWbTransformKernel`), so the solved WB actually puts skin on the line when rendered.

### 4. Colorspace awareness via dropdowns
The plugin exposes **Input → Gamut** and **Input → Transfer** dropdowns (same options as main LaserLab). The kernels already consume `pr.inputGamut` / `pr.inputTransfer`; `spaceParams()` just threads them through. The user selects their camera's capture space, and the scan/transform decode correctly.

### 5. Self-contained Metal kernel source
`AutoWbKernelSource.h` is **completely independent** of the WIP LaserLab Metal port. It contains only what AutoWB needs:
- CAT16 matrices + `cat16_adapt`, `white_balance`, `d60_to_d65`/`d65_to_d60`
- All 15 transfer functions (decode/encode)
- All 11 gamut matrices (R*/G* for g2x/x2g)
- OKLab (I0, L, O) → `work_to_oklab`
- Skin weight helpers (`ssstep`, `mod2`)
- Two kernels: `AutoWbReduceKernel` + `AutoWbTransformKernel`

This isolates AutoWB from the WIP full LaserLab Metal port.

---

## Build & Test

```bash
cd LaserLabOFX
make clean all          # universal arm64+x86_64, Metal + OpenCL
make test               # CPU gates: layout, round-trips, direction, solver
make test_autowb        # GPU chain: scan → solve → transform → verify
make install            # sudo cp to /Library/OFX/Plugins/
```

### Test expectations

- `make test` — **G1 layout PASS**, **G6 Auto-WB solver PASS** (on-line → identity, off-line → nonzero + lands on line). G4 direction tests fail (pre-existing: test re-inits `p={0}` so gamma clamps to 0.2; unrelated to AutoWB).
- `make test_autowb` — GPU chain: synthetic 8° off-line skin → scan → solve → transform → skin lands on line (herr < 0.05 rad).

---

## Known issues / Future work

| Issue | Status |
|-------|--------|
| G4 direction tests fail | Pre-existing: test re-inits `p={}` so gamma=0 → clamp 0.2. Not AutoWB's fault. |
| No auto-detect from Resolve color manager | Requires proprietary `ResolveColorManager` suite (not in public OFX headers). Dropdowns are the pragmatic fix. |
| Metal scan only runs when `isEnabledMetalRender` | CPU fallback exists but is slow (full-frame getPixelAddress). Acceptable for Studio. |
| Status string truncated if long | 96-char buffer. Could expand. |
| No keyframe-aware “reset” button | Could add “Reset WB” button that sets Temp/Tint=0. |
| OpenCL kernel not implemented | Metal-only on macOS. OpenCL stub exists in Makefile but not implemented. |

---

## Extending the plugin

### Add a “Reset WB” button
1. Add `OFX::PushButtonParamDescriptor* btnReset` in `describeInContext`
2. Fetch in ctor: `m_ResetBtn = fetchPushButtonParam("resetWb")`
3. In `changedParam`: if `name=="resetWb"` → `m_Temp->setValue(0); m_Tint->setValue(0); m_Status->setValue("reset")`

### Add keyframe support for animated WB
Change `setSolvedWb` to `setValueAtTime(t, temp)` and `setValueAtTime(t, tint)`. Then each Analyze click drops a keyframe at that frame. Scrubbing would interpolate — decide if that's desired.

### Auto-detect from Resolve color manager
If Resolve ever exposes `kOfxImageEffectPropResolveColorManager` in public headers, fetch the input space in `changedParam` or `render` and auto-set the dropdowns.

---

## Building the OFX bundle (from scratch)

```bash
# 1. Clone repo
# 2. cd LaserLabOFX
# 3. make clean all
# 4. make test        # verify CPU
# 5. make test_autowb # verify Metal chain
# 6. sudo make install
```

The `Makefile` handles:
- Universal binary (arm64 + x86_64)
- Metal + OpenCL compilation
- Vendored OFX Support lib (BaldavengerOFX pattern)
- Test targets

---

## Installing in Resolve

1. **System folder** (recommended): `sudo make install` → `/Library/OFX/Plugins/LaserLab.ofx.bundle`
2. **User folder** (no sudo): `cp -R LaserLab.ofx.bundle ~/Library/OFX/Plugins/`
3. **Resolve OFX folder** (alternative): `cp -R LaserLab.ofx.bundle ~/Library/Application\ Support/Blackmagic\ Design/DaVinci\ Resolve/OFX\ Plugins/`

**Restart Resolve** after install. Plugin appears under **Effects → Color → Auto WB from Skin**.

---

## Verifying the install

```bash
# Check bundle
ls -la /Library/OFX/Plugins/LaserLab.ofx.bundle/Contents/MacOS/

# Verify symbols
nm /Library/OFX/Plugins/LaserLab.ofx.bundle/Contents/MacOS/LaserLab.ofx | grep -cE "MetalWbScan|MetalWbTransform|AutoWbPluginFactory"

# Run GPU test
make test_autowb
```

Expected: `AutoWbScan` + `AutoWbTransform` + factory symbols present; `make test_autowb` prints `RESULT: ALL PASS`.

---

## Logs / Debugging

Resolve OFX load errors appear in:
```
~/Library/Application Support/Blackmagic Design/DaVinci Resolve/Logs/davinci_resolve.log
```
Search for `LaserLab` or `AutoWb`. Metal compile errors log to `/tmp/laserlab_metal.log`.

---

## Credits

- **Vibe coded** with AI assistance (conversational iteration)
- CAT16 matrices & transfer functions from the LaserLab DCTL (single source of truth)
- OFX scaffolding from BaldavengerOFX pattern
- Metal harness adapted from LaserLabMetalKernel.mm

---

## Disclaimer

**Vibe coded prototype.** Works for the author's workflow (ARRI/Sony/DJI footage, placed after CST, before look LUTs). No warranty. No production hardening. Extend at your own risk.
