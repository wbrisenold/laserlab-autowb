# LaserLab Combined Upgrade Design

## Scope

Rename `OKLabGrade-NoDRT` to `LaserLab` and replace the narrow, clipping-prone internal pipeline with one coherent wide-gamut revision. The DCTL remains an unmanaged, scene-linear grading transform with no display rendering transform.

## Signal Flow

1. Decode the selected input transfer function without clipping extended values.
2. Convert the selected input gamut to XYZ using its native white point.
3. Adapt D60 sources, currently ACEScg, to D65.
4. Apply camera-style Temp/Tint white-balance correction with CAT16 in XYZ.
5. Convert XYZ D65 to linear Rec.2020 for wide-gamut grading operations.
6. Convert Rec.2020 to linear sRGB only at the OKLab boundary because standard OKLab is D65/sRGB-defined.
7. Apply perceptual saturation, hue, tone, highlight, and skin operations in OKLab.
8. Return to linear Rec.2020 for exposure, density, scanner, and gain operations.
9. Apply hue-preserving gamut compression relative to the selected output camera gamut.
10. Adapt D65 back to D60 when the selected gamut is ACEScg, convert through XYZ, and encode with the original transfer function.
11. Guard non-finite output while preserving finite extended and negative values.

## White Balance

Temp and Tint are camera white-balance correction controls, not RGB creative gains. They map their existing normalized UI ranges to bounded mired and green-magenta offsets, derive a target white, and apply a CAT16 diagonal adaptation in LMS. Neutral values produce an identity transform.

## Presets

Remove the hidden preset selector and Look Strength. Set the visible control defaults directly to the current IMAX Laser-inspired values. The DCTL makes no certification claim and includes no PowerGrades.

## Saturation

Remove the saturation-type dropdown and its Vibrance, HDR, and Natural branches. `Sat Amount` always scales OKLab `a` and `b`, followed by the existing hue-preserving OKLab saturation compression.

## Numerical Safety

- Do not clamp decoded input, intermediate linear RGB, or final finite output to `[0,1]`.
- Preserve superwhites and recoverable negative channels.
- Bound divisors and powers in Expo Roll so valid UI combinations cannot create poles or NaNs.
- Replace NaN and infinity with zero only at the final boundary.

## Gamut Handling

Use linear Rec.2020 as the main working RGB space. Before conversion back to the selected camera gamut, compress chroma toward the achromatic axis when a channel would leave the destination gamut. Preserve luminance and hue as closely as the compact DCTL implementation allows; do not independently clip channels.

## Rename

Rename the project folder and primary DCTL to `LaserLab`. Update the backup filename, test references, documentation paths, headings, comments, and generated artifacts. Preserve a single canonical live DCTL and one last-known-good backup.

## Verification

- Source-contract tests for the renamed files, removed hidden presets, CAT16 path, Rec.2020 working path, finite guards, and absence of final `[0,1]` clamps.
- Numerical tests for CAT16 identity, D60/D65 round trip, gamut-compression hue/luminance behavior, Expo Roll edge settings, and finite extended output.
- OpenCL syntax compilation with warnings treated as errors.
- Byte comparison between the verified live DCTL and backup.
- Resolve LUT refresh when the scripting API is responsive; otherwise report it as an external verification gap.
