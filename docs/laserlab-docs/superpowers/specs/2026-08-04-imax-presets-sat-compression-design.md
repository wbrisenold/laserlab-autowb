# IMAX-Inspired Presets and Saturation Compression Design

## Goal

Add a small preset layer and global saturation compression to the existing `OKLabGrade-NoDRT.dctl`. A new instance should default to a clean, modern IMAX Laser-inspired starting point while preserving a Neutral mode and a restrained IMAX 15/70-inspired alternative.

These presets emulate tonal and color characteristics only. They do not claim to reproduce IMAX capture area, spatial resolution, optics, projection, screen geometry, brightness, or theater acoustics.

## Preset Controls

Add two controls near the input controls:

| Parameter | Default | Range | Purpose |
|---|---:|---|---|
| `look_preset` | `IMAX Laser` | Neutral, IMAX Laser, IMAX 15/70 | Selects preset base values. |
| `look_strength` | `1.0` | `0.0-1.0` | Blends the selected preset base toward Neutral. |

Input Gamut and Input Transfer remain Rec.709 by default as requested. Camera-specific decoding remains explicit.

## Slider Model

Existing sliders remain live adjustments rather than being ignored by a preset.

- Identity-centered controls multiply the blended preset base: Gamma, Gain, Contrast, and Sat Amount.
- Zero-centered controls add to the blended preset base: Lift, Shadows, Highlights, Desat Highlights, exposure adjustments, Hue, Sub Sat, and density controls.
- `look_strength=0.0` contributes no preset adjustment.
- Look Strength affects only preset contributions; manual sliders remain fully active in every preset and at every Look Strength value.
- Neutral contributes no preset adjustment at any strength.
- Neutral with identity slider values follows the existing neutral processing path.
- White balance, skin tuning, per-hue density, and camera decoding receive no preset bias because universal values would be scene- or subject-dependent.

## Preset Base Values

Only parameters that support the target rendering receive non-identity preset values.

| Parameter | Neutral | IMAX Laser | IMAX 15/70 |
|---|---:|---:|---:|
| Contrast | `1.00` | `1.08` | `1.05` |
| Shadows | `0.00` | `0.08` | `0.12` |
| Highlights | `0.00` | `0.18` | `0.28` |
| Desat Highlights | `0.00` | `0.10` | `0.15` |
| Sat Amount | `1.00` | `1.08` | `1.04` |
| Sat Compression | `0.00` | `0.35` | `0.45` |
| Sub Sat | `0.00` | `0.06` | `0.10` |
| Scanner Strength | `0.00` | `0.00` | `0.25` |

All unlisted preset bases remain at their existing identity values. The IMAX Laser preset stays clean and scanner-free. IMAX 15/70 uses the existing scanner parameters at a subtle effective strength.

## Saturation Compression

Add one slider:

```c
DEFINE_UI_PARAMS(sat_compression, Sat Compression, DCTLUI_SLIDER_FLOAT, 0.0, 0.0, 1.0, 0.001)
```

The effective amount is the user slider plus the blended preset contribution, clamped to `0.0-1.0`.

Processing runs in OKLab after saturation and hue rotation and before skin adjustment:

1. Compute chroma `c = sqrt(a*a + b*b)`.
2. Leave near-neutral chroma at or below the fixed `0.04` threshold unchanged.
3. Compress excess chroma with `excess / (1 + 12 * amount * excess)`, then add the threshold back.
4. Scale `a` and `b` by the same ratio to preserve hue angle.
5. Return the original `a` and `b` exactly when compression is zero or chroma is effectively zero.

The implementation adds no second DCTL, LUT, spatial operation, texture read, or dependency.

## Scanner Interaction

- Neutral and IMAX Laser do not activate scanner processing.
- IMAX 15/70 contributes `0.25` scanner strength through `look_strength`.
- The existing Enable Cineon Scan checkbox remains available for manual scanner use.
- Manual and preset scanner strengths add and clamp to `1.0`.
- Existing scanner parameter sliders continue to control shoulder, highlight chroma, desaturation, shadow lift, and shadow coolness.

## Safety

- Clamp effective bounded controls to their declared ranges.
- Avoid NaN or division by zero when chroma is zero.
- Preserve the existing final NaN and output clamps.
- Do not alter input transfer functions, gamut matrices, skin anchors, density interpolation, or FilmBox placement.

## Verification

- Source checks verify the new controls, preset ordering, default IMAX Laser selection, and processing placement.
- Numeric checks verify Neutral equivalence, `look_strength=0` equivalence, hue-angle preservation, non-increasing excessive chroma, low-chroma protection, and monotonic preset blending.
- Numeric checks verify that IMAX Laser does not activate the scanner and IMAX 15/70 does.
- Existing tone, Rec.2020, scanner, and pass-through checks continue to pass.
- The DCTL passes the OpenCL syntax harness and Resolve LUT refresh.

## Out of Scope

- Spatial sharpening, MTF shaping, grain, halation, bloom, gate weave, aspect-ratio changes, and output-display calibration.
- Automatic camera detection.
- Recreating or redistributing proprietary IMAX transforms.
