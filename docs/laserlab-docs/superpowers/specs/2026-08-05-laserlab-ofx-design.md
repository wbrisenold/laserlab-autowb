# LaserLab OFX Design

## Goal

Replace the LaserLab DCTL with a native DaVinci Resolve OFX plugin that retains every current control, gives correction sliders predictable studio-style behavior, and performs artifact-resistant spatial skin color evening.

The first release targets this Intel macOS system and DaVinci Resolve Studio 21.0.2.4. The existing DCTL remains installed as a fallback.

## Proven Template

Use the installed HomeBase Core plugin as the host-integration template:

- Raw OpenFX C API with exported `OfxGetNumberOfPlugins` and `OfxGetPlugin` entry points.
- Filter context, float RGBA source/output clips, render-window processing, instance data, and OFX parameter pages.
- Minimal dependencies: libc++, libSystem, and vendored OpenFX headers.
- macOS `.ofx.bundle` containing `Contents/MacOS/LaserLab.ofx`, `Info.plist`, and `LaserLab.xml`.
- Install first to the user Resolve OFX directory, not `/Library/OFX/Plugins`.

HomeBase Core's source is no longer present, so its installed binary, metadata, exported symbols, parameter strings, and known working bundle layout are the template. BaldavengerOFX supplies readable OpenFX source patterns and headers where the binary cannot show implementation details.

## Plugin Identity

- Bundle: `LaserLab.ofx.bundle`
- Identifier: `com.laserlab.fx`
- Name: `LaserLab`
- Grouping: `Color/LaserLab`
- Context: Filter
- Components: float RGBA
- Architecture: `x86_64`

## Pipeline

1. Decode the selected input transfer function.
2. Convert the selected input gamut through XYZ into linear Rec.2020.
3. Apply the held global Auto WB chromatic adaptation when enabled.
4. Apply continuous manual white balance, printer lights, and primary corrections.
5. Convert to OKLab for tone, saturation, hue, and skin operations.
6. Apply the existing broad Skin Hue, Skin Sat, and Skin Val interpolation.
7. Build a smooth skin-confidence mask from OKLab hue and chroma.
8. Spatially average only OKLab `a/b` among neighboring skin pixels, rejecting luminance edges and non-skin neighbors.
9. Blend spatially averaged chroma by Skin Evenness while preserving the center pixel's OKLab `L` and strong edges.
10. Return to linear Rec.2020 for exposure, density, Film Response, and gain.
11. Compress destination-gamut chroma, convert to the selected input gamut, and re-encode with the selected transfer function.
12. Preserve source alpha and finite extended-range RGB values.

## Auto White Balance

Auto WB uses detected skin only as a measurement reference. It solves one global CAT16 chromatic adaptation and applies that correction to every pixel in the image. It never isolates the correction to skin and does not use a global hue rotation.

| Control | Default | Range | Purpose |
|---|---:|---:|---|
| Auto WB | `Off` | Boolean | Enables the held global chromatic adaptation. |
| Analyze Current Frame | - | Push button | Stores the current OFX time as the reference frame and invalidates cached analysis. |
| Auto WB Strength | `1` | `0-1` | Blends from no automatic correction to the held CAT16 correction. |

The plugin stores a hidden persistent `Auto WB Reference Time` double parameter. Pressing Analyze records `kOfxPropTime` from the instance-change action. After project reopen, an empty in-memory cache re-analyzes this persisted time, making the held result deterministic.

Reference analysis:

1. Fetch the source clip at Auto WB Reference Time.
2. Sample a bounded grid of at most `128x72` positions across the full frame.
3. Decode samples into linear Rec.2020 and convert to OKLab before automatic or manual white balance and creative corrections.
4. Keep plausible midtone skin measurements using broad lightness, chroma, and circular hue limits.
5. Compute a chroma-weighted robust skin cluster after rejecting low-confidence outliers.
6. Solve the minimum-strength global CAT16 Temp/Tint adaptation whose corrected skin measurement lies on the existing `1.0685` radian skin-tone line.
7. Clamp automatic Temp and Tint to the same safe ranges as their manual counterparts.
8. Cache the solved adaptation with reference time, input gamut, and input transfer.

The skin-tone line supplies the chromatic constraint; minimum correction magnitude regularizes the otherwise underdetermined two-axis solution. Auto WB Strength interpolates CAT16 LMS gains toward identity before applying them globally. Manual Temp/Tint remain downstream for final adjustment.

If fewer than 32 reliable skin samples survive, the reference image cannot be fetched, or the cluster is unstable, Auto WB caches identity and leaves the entire image unchanged. It must not fall back to gray-world estimation or infer white balance from non-skin colors.

## Spatial Skin Processing

Skin Evenness corrects color blotchiness only. It must not smooth luminance texture, pores, grain, or facial edges.

For each center pixel:

- Compute skin confidence from circular hue distance to the existing `1.0685` radian anchor and a low-chroma rejection mask.
- Read a square neighborhood expanded by Skin Radius.
- Weight neighbors by spatial distance, center-to-neighbor OKLab `L` difference, and both pixels' skin confidence.
- Average neighbor `a/b`; retain center `L`.
- Preserve center chroma magnitude when the average would collapse saturation excessively.
- Blend center and averaged `a/b` by Skin Evenness and center confidence.
- Return the center unchanged when confidence or accumulated weight is negligible.

The render callback must request or access source pixels beyond the output render window by Skin Radius. Coordinates are clamped to source bounds. The algorithm must remain tile-safe.

Add one plugin-only control:

| Control | Default | Range | Unit |
|---|---:|---:|---|
| Skin Radius | `4` | `1-12` | pixels at full render scale |

## Control Calibration

All correction controls default to mathematical identity. Full-range values are strong but usable; ordinary corrections occupy the middle of each slider rather than its first few increments.

| Current Control | Default | Range | Behavior |
|---|---:|---:|---|
| Temp | `0` | `-1 to 1` | Continuous warm/cool correction; normalized CAT16-relative response with no zero discontinuity. |
| Tint | `0` | `-1 to 1` | Green/magenta correction with enough endpoint authority to replace routine offset-wheel compensation. |
| Auto WB | `Off` | Boolean | Held global CAT16 correction measured from reference-frame skin. |
| Analyze Current Frame | - | Push button | Captures the deterministic analysis reference time. |
| Auto WB Strength | `1` | `0-1` | Automatic global correction blend. |
| Printer Lights Master/R/G/B | `25` | `0-50` | Traditional printer points; one point remains `0.025 log10`. |
| Lift | `0` | `-0.25 to 0.25` | Signed OKLab lightness offset. |
| Gamma | `0` | `-1 to 1` | Signed midtone correction mapped symmetrically through an exponential power curve. |
| Gain | `1` | `0-2` | Linear output multiplier. |
| Contrast | `1` | `0.5-1.5` | Symmetric contrast around OKLab `L=0.5`. |
| Shadows | `0` | `-1 to 1` | Positive lifts shadows; negative deepens shadows; black remains anchored. |
| Highlights | `0` | `-1 to 1` | Positive raises highlights; negative lowers highlights; midtones remain protected. |
| Desat Highlights | `0` | `0-1` | Progressive radial OKLab chroma reduction in highlights. |
| Exposure | `0` | `-6 to 6` | Stops, using `2^value`. Public label replaces Expo Global. |
| Flash | `0` | `-1 to 1` | Signed scene-linear additive exposure, internally limited to a practical range. |
| Expo Knee Pivot | `0.75` | `0.18-1.25` | Scene-linear shoulder start. |
| Expo Width | `1` | `0.1-4` | Scene-linear roll-off width. |
| Expo Shoulder | `0` | `0-1` | Blend amount into monotonic rational highlight compression; zero is identity. |
| Expo Gamma | `0` | `-1 to 1` | Signed post-exposure midtone power with symmetric response. |
| Sat Amount | `1` | `0-2` | Direct OKLab chroma multiplier. |
| Sat Compression | `0` | `0-1` | Hue-preserving excessive-chroma compression. |
| Hue Rotate | `0` | `-180 to 180` | Degrees. |
| Skin Hue | `0` | `-60 to 60` | Degrees around the smooth skin interpolation. |
| Skin Sat | `1` | `0-2` | Skin chroma multiplier. |
| Skin Val | `0` | `-0.25 to 0.25` | Signed OKLab skin-lightness offset. |
| Skin Evenness | `0` | `0-1` | Edge-aware spatial skin-color averaging amount. |
| Sub Sat | `0` | `0-1` | Subtractive saturation amount; no negative/inverted mode. |
| Deep Sat | `0` | `0-1` | Smooth saturation qualifier for Sub Sat; zero disables qualification. |
| Deep Dens | `0` | `0-1` | Smooth saturation qualifier for per-hue density; zero disables qualification. |
| R/Y/G/C/B/M Dens | `0` | `-0.5 to 0.5` | Signed per-hue density correction; positive adds density, negative removes it. |
| Film Response | `0` | `0-1` | Blend into the existing scanner-style finishing response. |

Input Gamut and Input Transfer remain unchanged. Slider increments use `0.01` for perceptual amounts and stops, `0.1` for degrees, and `1` for printer points and pixel radius.

OFX parameter pages use restrained emoji prefixes: `📥 Input`, `⚖️ White Balance`, `🎞 Printer Lights`, `🎚 Primaries`, `🌗 Tone`, `☀️ Exposure`, `🌈 Color`, `🧑 Skin`, `🧪 Density`, and `🎬 Finish`. Parameter keys, script names, and bundle identifiers remain ASCII.

## White Balance

The existing function has a confirmed discontinuity: its calculated 6504 K white is not D65, while a special zero bypass hides that mismatch. Moving Temp or Tint by one step abruptly changes a neutral D65 input.

The plugin computes a relative CAT16 correction against the same model's neutral baseline. At zero, source and baseline LMS values are equal, producing continuous identity without a special-case jump. Temp and Tint use normalized `-1 to 1` UI ranges, with their internal scale chosen so the endpoints are corrective but finite.

## Expo Redesign

The existing Knee/Width/Shoulder controls interact through a formula where Width is nearly ineffective at the default knee and negative Shoulder values collapse into a safety clamp. Replace it with:

1. Compute positive excess above Knee Pivot.
2. Produce a monotonic rational roll-off target using Expo Width.
3. Blend the original highlight into that target by Expo Shoulder.
4. Apply signed Expo Gamma with a symmetric exponential mapping.

Every Expo control has an exact neutral state and a monotonic response.

## Source Layout

```text
LaserLabOFX/
  Makefile
  include/openfx/
  src/LaserLabPlugin.cpp
  src/LaserLabColor.h
  src/LaserLabColor.cpp
  tests/test_color.cpp
  bundle/Info.plist
  bundle/LaserLab.xml
  scripts/build-install.sh
```

`LaserLabColor` owns host-independent point and neighborhood math. `LaserLabPlugin` owns only OpenFX lifecycle, clips, parameters, render windows, instance data, and error translation.

The plugin instance owns a synchronized Auto WB cache containing validity, reference time, input gamut, input transfer, solved CAT16 LMS gains, and analysis status. Ordinary rendering reads only the held gains; reference-frame analysis updates them.

## Error Handling

- Unsupported contexts, components, or bit depths return the appropriate OFX error instead of rendering corrupt output.
- Missing source/output images fail the render cleanly.
- Invalid row bytes, bounds, or data pointers fail before pixel access.
- Non-finite computed RGB channels fall back to zero; finite negative values and superwhites remain available.
- Out-of-bounds neighborhood coordinates clamp to the source image bounds.
- Allocation failure returns an OFX memory error and leaves source media untouched.
- Auto WB analysis failure caches identity and continues rendering; it never fails the current frame.

## Verification

### Standalone Tests

- Neutral parameters produce identity within numeric tolerance across negative values, middle gray, saturated colors, and superwhites.
- Temp/Tint are continuous around zero, monotonic, correctly directed, and sufficiently strong at endpoints.
- Auto WB uses synthetic skin measurements to solve a global CAT16 correction that places corrected skin on the skin-tone line.
- Auto WB applies the same adaptation to skin and non-skin pixels across the entire image.
- Auto WB rejects frames without reliable skin and returns exact identity.
- Analyze-and-hold returns the same correction until reference time or relevant input settings change.
- Every slider's neutral, midpoint, direction, and endpoint behavior is asserted.
- Tone and shoulder curves remain finite and monotonic.
- Skin Evenness leaves non-skin, neutral, and hard-edge pixels unchanged.
- Synthetic blotchy skin patches show reduced `a/b` variance without reduced `L` variance.
- Tiled rendering matches whole-frame rendering at tile boundaries.

### Plugin Validation

- Build an `x86_64` bundle with Apple Clang and no unresolved non-system libraries.
- Install to the user Resolve OFX directory.
- Resolve discovers `com.laserlab.fx` under `Color/LaserLab`.
- Neutral plugin output matches source frames within float tolerance.
- Render representative HD and 4K frames with Skin Radius `1`, `4`, and `12`.
- Repeated add/remove, timeline scrubbing, project save/reopen, and Resolve relaunch do not crash.

## Scope Boundaries

- First release is CPU-only and Intel macOS-only.
- No temporal processing, face detection, machine learning, beauty smoothing, licensing, installer package, code signing, Apple Silicon build, or cross-platform build.
- Metal acceleration is added only after CPU output and spatial behavior are validated.
