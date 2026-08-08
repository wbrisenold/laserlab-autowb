# LaserLab OFX Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build an Intel macOS DaVinci Resolve OFX plugin that retains every LaserLab control, recalibrates correction behavior, and replaces per-pixel skin hue contraction with edge-aware spatial skin-color evening.

**Architecture:** A host-independent C++ color core owns all point and neighborhood math. A small raw OpenFX C API adapter follows the installed HomeBase Core lifecycle and bundle layout, exposing float RGBA clips and parameter pages without a framework dependency.

**Tech Stack:** C++17, OpenFX 1.5 C headers, Apple Clang 17, Make, CPU float RGBA, DaVinci Resolve Studio 21.0.2.4

## Global Constraints

- Target only `x86_64` macOS and Resolve Studio 21.0.2.4 in the first release.
- Keep the existing `LaserLab.dctl` installed and unchanged as fallback.
- Install the plugin under `~/Library/Application Support/Blackmagic Design/DaVinci Resolve/OFX Plugins/`.
- Use plugin identifier `com.laserlab.fx`, name `LaserLab`, and grouping `Color/LaserLab`.
- Support only filter context and float RGBA source/output images.
- Preserve source alpha, finite negative RGB, and finite superwhites.
- All correction controls default to mathematical identity.
- Skin Evenness changes spatial skin color only; it must not smooth OKLab `L`.
- Auto WB uses skin only as a measurement and applies one held CAT16 correction globally to every pixel.
- Auto WB must pass through when fewer than 32 reliable skin samples are available.
- OFX page labels may use emoji; all internal parameter keys and script names remain ASCII.
- No Metal, temporal processing, face detection, ML, installer, signing, Apple Silicon, or cross-platform work.
- The workspace is not a Git repository, so tasks have no commit step.

---

### Task 1: OpenFX Headers, Build, And Bundle Skeleton

**Files:**
- Create: `LaserLabOFX/Makefile`
- Create: `LaserLabOFX/include/openfx/ofxCore.h`
- Create: `LaserLabOFX/include/openfx/ofxImageEffect.h`
- Create: `LaserLabOFX/include/openfx/ofxParam.h`
- Create: `LaserLabOFX/include/openfx/ofxProperty.h`
- Create: `LaserLabOFX/include/openfx/ofxMemory.h`
- Create: `LaserLabOFX/src/LaserLabPlugin.cpp`
- Create: `LaserLabOFX/bundle/Info.plist`
- Create: `LaserLabOFX/bundle/LaserLab.xml`
- Create: `LaserLabOFX/scripts/build-install.sh`

**Interfaces:**
- Consumes: OpenFX headers pinned to `AcademySoftwareFoundation/openfx@3de640d6f645fe6e346acd57e568d8b0a5ae4574`
- Produces: exported `OfxGetNumberOfPlugins()` and `OfxGetPlugin(int)` plus a loadable `LaserLab.ofx.bundle`

- [ ] **Step 1: Vendor the exact OpenFX headers**

Create `LaserLabOFX/include/openfx`, then download these files from the pinned commit:

```bash
base="https://raw.githubusercontent.com/AcademySoftwareFoundation/openfx/3de640d6f645fe6e346acd57e568d8b0a5ae4574/include"
for file in ofxCore.h ofxImageEffect.h ofxParam.h ofxProperty.h ofxMemory.h; do
  curl -fL "$base/$file" -o "LaserLabOFX/include/openfx/$file"
done
```

Expected: all five files exist and are non-empty.

- [ ] **Step 2: Add a minimal plugin entry-point source**

Create `LaserLabOFX/src/LaserLabPlugin.cpp` with suite storage, `setHost`, a dispatcher returning `kOfxStatOK` for `OfxActionLoad`/`OfxActionUnload`, and one static `OfxPlugin`:

```cpp
#include "openfx/ofxCore.h"
#include "openfx/ofxImageEffect.h"

#include <cstring>

namespace {
OfxHost* gHost = nullptr;

void setHost(OfxHost* host) { gHost = host; }

OfxStatus mainEntry(const char* action, const void*, OfxPropertySetHandle,
                    OfxPropertySetHandle) {
  if (std::strcmp(action, kOfxActionLoad) == 0 ||
      std::strcmp(action, kOfxActionUnload) == 0) {
    return kOfxStatOK;
  }
  return kOfxStatReplyDefault;
}

OfxPlugin plugin = {
    kOfxImageEffectPluginApi,
    1,
    "com.laserlab.fx",
    1,
    0,
    setHost,
    mainEntry,
};
}  // namespace

extern "C" {
int OfxGetNumberOfPlugins() { return 1; }
OfxPlugin* OfxGetPlugin(int index) { return index == 0 ? &plugin : nullptr; }
}
```

- [ ] **Step 3: Add exact bundle metadata**

`Info.plist` must set `CFBundleExecutable=LaserLab.ofx`, `CFBundleIdentifier=com.laserlab.fx`, `CFBundleName=LaserLab`, `CFBundlePackageType=BNDL`, and `CFBundleVersion=0.1.0`.

`LaserLab.xml` must identify `com.laserlab.fx`, name `LaserLab`, grouping `Color/LaserLab`, category `Filter`, plugin version `0.1`, and API version `1.0`.

- [ ] **Step 4: Add the build**

Use this `Makefile` contract:

```make
CXX := clang++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -Werror -fPIC -arch x86_64 -Iinclude
LDFLAGS := -bundle -arch x86_64
BUILD := build
BUNDLE := $(BUILD)/LaserLab.ofx.bundle
BINARY := $(BUNDLE)/Contents/MacOS/LaserLab.ofx
SOURCES := src/LaserLabPlugin.cpp src/LaserLabColor.cpp

.PHONY: all test install clean
all: $(BINARY)

$(BINARY): $(SOURCES)
	mkdir -p "$(BUNDLE)/Contents/MacOS"
	$(CXX) $(CXXFLAGS) $(SOURCES) $(LDFLAGS) -o "$@"
	cp bundle/Info.plist "$(BUNDLE)/Contents/Info.plist"
	cp bundle/LaserLab.xml "$(BUNDLE)/Contents/LaserLab.xml"

test: build/test_color
	./build/test_color

build/test_color: tests/test_color.cpp src/LaserLabColor.cpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) $^ -o $@

install: all
	mkdir -p "$(HOME)/Library/Application Support/Blackmagic Design/DaVinci Resolve/OFX Plugins"
	rm -rf "$(HOME)/Library/Application Support/Blackmagic Design/DaVinci Resolve/OFX Plugins/LaserLab.ofx.bundle"
	cp -R "$(BUNDLE)" "$(HOME)/Library/Application Support/Blackmagic Design/DaVinci Resolve/OFX Plugins/"

clean:
	rm -rf build
```

Create an empty `src/LaserLabColor.cpp` so the skeleton links.

- [ ] **Step 5: Build and inspect exports**

Run:

```bash
make -C LaserLabOFX clean all
file LaserLabOFX/build/LaserLab.ofx.bundle/Contents/MacOS/LaserLab.ofx
nm -gU LaserLabOFX/build/LaserLab.ofx.bundle/Contents/MacOS/LaserLab.ofx
otool -L LaserLabOFX/build/LaserLab.ofx.bundle/Contents/MacOS/LaserLab.ofx
```

Expected: Mach-O `x86_64` bundle; exports include `_OfxGetNumberOfPlugins` and `_OfxGetPlugin`; dependencies are only libc++ and libSystem.

---

### Task 2: Host-Independent Types And Neutral Point Pipeline

**Files:**
- Create: `LaserLabOFX/src/LaserLabColor.h`
- Modify: `LaserLabOFX/src/LaserLabColor.cpp`
- Create: `LaserLabOFX/tests/test_color.cpp`

**Interfaces:**
- Consumes: constants and transfer/gamut functions from `LaserLab.dctl`
- Produces: `laserlab::Params`, `laserlab::Pixel`, `laserlab::processPoint()`, `laserlab::toWorkingOklab()`, and `laserlab::fromWorkingOklab()`

- [ ] **Step 1: Define the complete public color-core interface**

Create `LaserLabColor.h`:

```cpp
#pragma once

#include <cstdint>

namespace laserlab {
struct Vec3 { float x, y, z; };
struct Pixel { float r, g, b, a; };

struct Params {
  int inputGamut = 0;
  int inputTransfer = 0;
  float temp = 0.0f, tint = 0.0f;
  bool autoWb = false;
  float autoWbStrength = 1.0f;
  Vec3 autoWbLmsGain{1.0f, 1.0f, 1.0f};
  float printerMaster = 25.0f, printerRed = 25.0f;
  float printerGreen = 25.0f, printerBlue = 25.0f;
  float lift = 0.0f, gamma = 0.0f, gain = 1.0f, contrast = 1.0f;
  float shadows = 0.0f, highlights = 0.0f, desatHighlights = 0.0f;
  float exposure = 0.0f, flash = 0.0f, expoKnee = 0.75f;
  float expoWidth = 1.0f, expoShoulder = 0.0f, expoGamma = 0.0f;
  float saturation = 1.0f, satCompression = 0.0f, hueDegrees = 0.0f;
  float skinHueDegrees = 0.0f, skinSaturation = 1.0f, skinValue = 0.0f;
  float skinEvenness = 0.0f, skinRadius = 4.0f;
  float subtractiveSat = 0.0f, deepSat = 0.0f, deepDensity = 0.0f;
  float redDensity = 0.0f, yellowDensity = 0.0f, greenDensity = 0.0f;
  float cyanDensity = 0.0f, blueDensity = 0.0f, magentaDensity = 0.0f;
  float filmResponse = 0.0f;
};

Vec3 toWorkingOklab(Vec3 encoded, const Params& params);
Vec3 fromWorkingOklab(Vec3 lab, const Params& params);
Pixel processPoint(Pixel source, const Params& params);
bool finite(Pixel pixel);
}  // namespace laserlab
```

- [ ] **Step 2: Write the failing identity tests**

Create `tests/test_color.cpp` with a minimal `require()` helper and these cases:

```cpp
#include "LaserLabColor.h"
#include <cmath>
#include <cstdlib>

void require(bool value) { if (!value) std::abort(); }
bool near(float a, float b, float e = 2e-4f) { return std::fabs(a - b) <= e; }

int main() {
  const laserlab::Params p{};
  for (laserlab::Pixel v : {
           laserlab::Pixel{0.0f, 0.0f, 0.0f, 0.25f},
           laserlab::Pixel{0.18f, 0.18f, 0.18f, 0.5f},
           laserlab::Pixel{0.8f, 0.2f, 0.1f, 0.75f},
           laserlab::Pixel{-0.1f, 1.4f, 0.3f, 1.0f}}) {
    const auto out = laserlab::processPoint(v, p);
    require(near(out.r, v.r) && near(out.g, v.g) && near(out.b, v.b));
    require(out.a == v.a);
  }
}
```

- [ ] **Step 3: Run and observe the link failure**

Run: `make -C LaserLabOFX test`

Expected: link failure because `processPoint` is not implemented.

- [ ] **Step 4: Port the exact point-conversion foundation**

Move the DCTL's matrix constants, signed cube root, matrix multiply, gamut-to-XYZ, XYZ-to-gamut, transfer decode/encode, D60/D65 adaptation, Rec.2020-to-OKLab, OKLab-to-Rec.2020, and destination-gamut compression into `namespace laserlab` using `Vec3`.

Implement `processPoint` as decode -> gamut -> Rec.2020 -> OKLab -> Rec.2020 -> gamut -> encode, with alpha copied unchanged. Do not port creative controls yet. Return zero only for non-finite RGB channels.

- [ ] **Step 5: Run the identity tests**

Run: `make -C LaserLabOFX test`

Expected: exit code `0` with no output.

---

### Task 3: Calibrated White Balance, Primaries, Tone, And Exposure

**Files:**
- Modify: `LaserLabOFX/src/LaserLabColor.cpp`
- Modify: `LaserLabOFX/tests/test_color.cpp`

**Interfaces:**
- Consumes: `Params`, point conversion, OKLab helpers
- Produces: continuous white balance and calibrated correction stages used by `processPoint`

- [ ] **Step 1: Add failing white-balance continuity and authority checks**

Append tests that process D65-neutral gray with Temp/Tint at `0`, `+0.001`, `-0.001`, and endpoints. Require one-step output distance below `0.005`, opposite directions for opposite signs, and endpoint distance above `0.08` for Tint and `0.15` for Temp.

- [ ] **Step 2: Add failing direction checks for every correction control**

Use one parameter at a time and assert:

```cpp
// Exposure is stops.
p.exposure = 1.0f;
require(near(process(0.18f, p), 0.36f, 1e-3f));
p.exposure = -1.0f;
require(near(process(0.18f, p), 0.09f, 1e-3f));

// Positive shadows raise a dark sample; negative shadows lower it.
// Positive highlights raise a bright sample; negative highlights lower it.
// Positive gamma raises middle gray; negative gamma lowers it.
// Gain 1 and contrast 1 are neutral.
// Expo Shoulder 0 is neutral; 1 compresses values above Expo Knee monotonically.
```

The test helper `process(float gray, const Params&)` returns output red for an equal-RGB input.

- [ ] **Step 3: Verify the tests fail**

Run: `make -C LaserLabOFX test`

Expected: abort because controls are not applied.

- [ ] **Step 4: Implement normalized relative CAT16 white balance**

Compute the target model white from Temp/Tint, convert it and the fixed 6504 K model white to CAT16 LMS, and multiply input LMS by `baselineLms / targetLms`. Map Temp `-1..1` through `6504 * exp(temp * 0.7)` and Tint `-1..1` through a signed `0.035` chromaticity-y offset. This makes zero continuous and gives full-range authority.

- [ ] **Step 5: Implement correction math**

- Printer lights: retain `10^((point-25)*0.025)` gains.
- Lift: add directly to OKLab `L`.
- Gamma: exponent `exp2(-gamma)` so positive values raise midtones symmetrically.
- Gain: linear Rec.2020 multiplier.
- Contrast: `(L-0.5)*contrast+0.5`.
- Shadows/Highlights: signed smooth masks that preserve black/midtones.
- Exposure: multiply by `exp2(exposure)`.
- Flash: add `flash*0.1` scene-linearly.
- Expo Shoulder: blend by `expoShoulder` into `knee + excess/(1+excess/width)`.
- Expo Gamma: exponent `exp2(-expoGamma)`.

- [ ] **Step 6: Run tests**

Run: `make -C LaserLabOFX test`

Expected: exit code `0` with no output.

---

### Task 4: Color, Skin Selection, Density, And Film Response

**Files:**
- Modify: `LaserLabOFX/src/LaserLabColor.cpp`
- Modify: `LaserLabOFX/tests/test_color.cpp`

**Interfaces:**
- Consumes: point correction stages
- Produces: broad skin controls, density controls, and finishing response without spatial averaging

- [ ] **Step 1: Add failing invariants**

Test saturation `1`, hue `0`, Skin Hue `0`, Skin Sat `1`, Skin Val `0`, density `0`, and Film Response `0` as identity. Test hue rotation and radial saturation preserve expected hue/chroma relationships. Test signed per-hue density produces opposite changes for `+0.25` and `-0.25`.

- [ ] **Step 2: Verify failure**

Run: `make -C LaserLabOFX test`

Expected: abort because color controls are not applied.

- [ ] **Step 3: Port and calibrate color controls**

Port `compress_oklab_chroma`, tetra skin interpolation, subtractive density, per-hue density, and `CineonScan` from the DCTL. Remove the DCTL's per-pixel Skin Evenness block. Use the ranges/defaults in the design spec and allow signed per-hue density `-0.5..0.5`.

- [ ] **Step 4: Run tests**

Run: `make -C LaserLabOFX test`

Expected: exit code `0` with no output.

---

### Task 5: Analyze-And-Hold Global Auto White Balance

**Files:**
- Modify: `LaserLabOFX/src/LaserLabColor.h`
- Modify: `LaserLabOFX/src/LaserLabColor.cpp`
- Modify: `LaserLabOFX/tests/test_color.cpp`

**Interfaces:**
- Consumes: input decode, Rec.2020/OKLab conversion, normalized CAT16 white balance
- Produces: `analyzeAutoWb()` and global `autoWbLmsGain`

- [ ] **Step 1: Add the analysis interface**

Add:

```cpp
struct AutoWbResult {
  bool valid = false;
  int sampleCount = 0;
  float solvedTemp = 0.0f;
  float solvedTint = 0.0f;
  Vec3 lmsGain{1.0f, 1.0f, 1.0f};
};

AutoWbResult analyzeAutoWb(const Pixel* source, int width, int height,
                           std::ptrdiff_t stride, const Params& params);
```

- [ ] **Step 2: Write failing analysis tests**

Build a synthetic reference frame containing at least 64 midtone skin samples whose hue is offset from `1.0685` radians by a known CAT16 cast. Require `analyzeAutoWb` to return valid, use at least 32 samples, and produce gains that place corrected skin on the line within `0.01` radians. Include non-skin patches and assert the same gains alter them globally. A frame with no skin must return invalid and identity gains.

- [ ] **Step 3: Verify failure**

Run: `make -C LaserLabOFX test`

Expected: link failure because `analyzeAutoWb` is absent.

- [ ] **Step 4: Implement bounded robust skin measurement**

Sample at most `128x72` positions. Decode to linear Rec.2020 and OKLab before all corrections. Keep broad midtone/chroma/hue candidates, reject outliers around the first circular mean, and require at least 32 survivors.

- [ ] **Step 5: Solve minimum-strength CAT16 correction**

Search bounded Temp/Tint values in two deterministic passes: a coarse `33x33` grid over `[-1,1]`, then a fine `21x21` grid around the best coarse point. Score each candidate as squared corrected skin-line angular error plus `0.0025*(temp*temp+tint*tint)`. Return the normalized relative CAT16 LMS gains for the lowest score. This regularizer selects the smallest global correction that places measured skin on the line.

- [ ] **Step 6: Apply globally in point processing**

When `autoWb` is true, interpolate each held LMS gain from `1` by `autoWbStrength`, transform every pixel's linear Rec.2020 through XYZ/CAT16 LMS, apply those gains, and continue with manual Temp/Tint. Do not use a skin mask during application.

- [ ] **Step 7: Run tests**

Run: `make -C LaserLabOFX test`

Expected: exit code `0` with no output.

---

### Task 6: Edge-Aware Spatial Skin Color Evening

**Files:**
- Modify: `LaserLabOFX/src/LaserLabColor.h`
- Modify: `LaserLabOFX/src/LaserLabColor.cpp`
- Modify: `LaserLabOFX/tests/test_color.cpp`

**Interfaces:**
- Consumes: pointwise `toWorkingOklab()` and broad skin controls
- Produces: `processFrame()` for tile-safe spatial rendering

- [ ] **Step 1: Add the frame interface**

Add:

```cpp
struct ImageView {
  const Pixel* source;
  Pixel* output;
  int width, height;
  std::ptrdiff_t sourceStride, outputStride;
};
struct Rect { int x1, y1, x2, y2; };
void processFrame(const ImageView& image, Rect renderWindow, const Params& params);
```

- [ ] **Step 2: Write failing synthetic spatial tests**

Create a `9x9` skin-colored image with alternating `a/b` blotches but constant `L`, plus a hard vertical `L` edge. Require Skin Evenness `0` to be exact identity. At amount `1` and radius `2`, require lower within-region `a/b` variance, unchanged `L` variance, and no averaging across the hard edge. Render the same frame as one full rectangle and four tiles; require equal outputs within `1e-5`.

- [ ] **Step 3: Verify failure**

Run: `make -C LaserLabOFX test`

Expected: link failure because `processFrame` is absent.

- [ ] **Step 4: Implement two-pass CPU processing**

Allocate an OKLab scratch buffer for the full source view. First apply point operations through broad skin controls. For each output pixel, compute skin confidence and average neighbor `a/b` within `round(skinRadius)`, using spatial Gaussian weight, luminance-edge rejection, and neighbor skin confidence. Preserve center `L`, reject negligible total weight, blend by `skinEvenness*centerConfidence`, then continue density/Film Response/output conversion.

Clamp neighborhood coordinates to image bounds. Derive results only from source/scratch data, never previously written output, so tile order cannot change results.

- [ ] **Step 5: Run tests**

Run: `make -C LaserLabOFX test`

Expected: exit code `0` with no output.

---

### Task 7: Raw OpenFX Lifecycle, Parameters, Auto WB State, And Render

**Files:**
- Modify: `LaserLabOFX/src/LaserLabPlugin.cpp`
- Modify: `LaserLabOFX/tests/test_color.cpp`

**Interfaces:**
- Consumes: `processFrame`, `analyzeAutoWb`, all `Params` fields
- Produces: complete OFX describe/create/destroy/render actions and parameter pages

- [ ] **Step 1: Acquire and validate suites on load**

Fetch `OfxImageEffectSuiteV1`, `OfxPropertySuiteV1`, `OfxParameterSuiteV1`, and `OfxMemorySuiteV1`. Return `kOfxStatErrMissingHostFeature` if any required suite is absent.

- [ ] **Step 2: Describe the plugin and clips**

Set label `LaserLab`, grouping `Color/LaserLab`, filter context, float pixel depth, RGBA components, no multiple clip PARs/depths, and thread safety. Define `Source` and `Output` clips as RGBA.

- [ ] **Step 3: Define all parameter pages and exact ranges**

Create pages `📥 Input`, `⚖️ White Balance`, `🎞 Printer Lights`, `🎚 Primaries`, `🌗 Tone`, `☀️ Exposure`, `🌈 Color`, `🧑 Skin`, `🧪 Density`, and `🎬 Finish`. Define every parameter from `Params`, matching names/defaults/ranges/increments in the design table. Internal keys remain ASCII. Define Skin Radius as integer `1..12`, default `4`.

On `⚖️ White Balance`, add `AutoWB` boolean default false, `AnalyzeCurrentFrame` push button, `AutoWBStrength` double `0..1` default `1`, and secret persistent `AutoWBReferenceTime` double.

- [ ] **Step 4: Create and destroy instance data**

Store source/output clip handles, every parameter handle, a mutex, and an Auto WB cache in one `InstanceData` allocated with `new (std::nothrow)`. The cache contains validity, reference time, input gamut/transfer, and `AutoWbResult`. Save it under `kOfxPropInstanceData`; delete it on destroy.

- [ ] **Step 5: Handle Auto WB analysis state**

On `OfxActionInstanceChanged` for `AnalyzeCurrentFrame`, read `kOfxPropTime`, write it into `AutoWBReferenceTime`, and invalidate the cache under the instance mutex. During render, when Auto WB is enabled and cache keys differ, fetch the source image at the persisted reference time, call `analyzeAutoWb`, cache identity on failure, and release the reference image. Subsequent frames reuse the held result.

- [ ] **Step 6: Implement render validation and parameter reading**

At render time, fetch time and render window, acquire source/output images, verify float RGBA, data pointers, bounds, and row bytes, read every parameter at time into `Params`, and call `processFrame`. Always release both images. Return `kOfxStatErrImageFormat` for unsupported images and `kOfxStatErrMemory` on allocation failure.

- [ ] **Step 7: Build and inspect**

Run:

```bash
make -C LaserLabOFX clean test all
file LaserLabOFX/build/LaserLab.ofx.bundle/Contents/MacOS/LaserLab.ofx
nm -gU LaserLabOFX/build/LaserLab.ofx.bundle/Contents/MacOS/LaserLab.ofx
otool -L LaserLabOFX/build/LaserLab.ofx.bundle/Contents/MacOS/LaserLab.ofx
```

Expected: tests pass; binary is x86_64; both OFX exports exist; only system libraries are linked.

---

### Task 8: Install And Validate In Resolve

**Files:**
- Modify: `LaserLabOFX/scripts/build-install.sh`
- Modify: `HANDOFF-LASERLAB.md`

**Interfaces:**
- Consumes: complete OFX bundle
- Produces: installed, discoverable Resolve plugin and operator documentation

- [ ] **Step 1: Make installation safe and repeatable**

`build-install.sh` must run `make clean test all`, copy the previous installed LaserLab bundle to `LaserLab.ofx.bundle.backup` when present, install the new bundle, and print the installed binary architecture and dependencies. It must not touch `LaserLab.dctl`.

- [ ] **Step 2: Install**

Run: `LaserLabOFX/scripts/build-install.sh`

Expected: bundle exists at `~/Library/Application Support/Blackmagic Design/DaVinci Resolve/OFX Plugins/LaserLab.ofx.bundle`.

- [ ] **Step 3: Relaunch and inspect Resolve discovery**

Close and reopen Resolve, then inspect `~/Library/Application Support/Blackmagic Design/DaVinci Resolve/logs/davinci_resolve.log` for `LaserLab`, `com.laserlab.fx`, OFX load errors, missing symbols, or crashes.

Expected: plugin loads without OFX errors and appears under `Color/LaserLab`.

- [ ] **Step 4: Live smoke test**

Apply LaserLab to a test clip. Verify neutral output, emoji parameter grouping, Temp/Tint direction and strength, Exposure stops, signed tone controls, and Skin Evenness at radii `1`, `4`, and `12`. Analyze Auto WB on a frame with visible skin, confirm the whole image receives one global correction, scrub other frames to confirm the correction is held, then save/reopen the project and confirm the persisted reference time reproduces it. Remove/re-add the plugin and render HD plus 4K frames.

- [ ] **Step 5: Update handoff documentation**

Document the plugin path, identifier, build/test/install commands, neutral defaults, CPU-only limitation, DCTL fallback, Resolve restart requirement, and rollback by restoring `LaserLab.ofx.bundle.backup`.

- [ ] **Step 6: Final verification**

Run:

```bash
make -C LaserLabOFX clean test all
cmp -s LaserLab.dctl backup/LaserLab-LKG.dctl
file "$HOME/Library/Application Support/Blackmagic Design/DaVinci Resolve/OFX Plugins/LaserLab.ofx.bundle/Contents/MacOS/LaserLab.ofx"
```

Expected: tests/build pass, fallback DCTL still matches its LKG backup, and installed plugin is an x86_64 Mach-O bundle.
