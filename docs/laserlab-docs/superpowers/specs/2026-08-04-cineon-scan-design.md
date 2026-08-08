# Cineon Scan Module Design

## Goal

Add an optional, non-proprietary scanner-response stage to the existing DCTL. It should soften and neutralize scanned-negative highlights, establish a subtle black floor, and render shadows gently without adding grain, halation, bloom, stock response, orange mask, print response, or lens behavior.

## Placement

`CineonScan()` runs in linear sRGB immediately after the existing density model assigns `out=d_nrgb` and before master gain, camera-gamut conversion, and transfer re-encoding:

```text
Input decode and gamut conversion
→ existing OKLab grade
→ existing Expo Roll
→ existing density model
→ CineonScan()
→ master gain
→ camera-gamut conversion and transfer encode
→ output
```

The downstream Filmbox/LUT/CST remains the final display transform. The scanner module does not duplicate it.

## Controls

All scalar controls use range `0.0–1.0` with step `0.001`.

| Parameter | Default | Purpose |
|---|---:|---|
| `enable_cineon_scan` | `0` | Keeps the module pass-through until explicitly enabled. |
| `scan_strength` | `0.50` | Global blend, ready to produce a visible result when enabled. |
| `shoulder_strength` | `0.50` | Tonal highlight-shoulder amount. |
| `highlight_compression` | `0.50` | Nonlinear compression of excessive highlight chroma. |
| `highlight_desaturation` | `0.35` | Smooth radial chroma reduction near white. |
| `shadow_lift` | `0.15` | Scanner black-floor lift. |
| `shadow_coolness` | `0.10` | Subtle blue-gray shadow bias with neutral and skin protection. |

The checkbox defaults off for pass-through. Enabling it immediately applies the prepared `scan_strength=0.50` scanner blend; setting Scan Strength to zero remains mathematically neutral.

## Module Interface

```c
__DEVICE__ float3 CineonScan(
    float3 rgb,
    float strength,
    float shoulder,
    float chroma_compression,
    float highlight_desat,
    float black_lift,
    float shadow_coolness
);
```

The enable checkbox is evaluated at the call site to provide a cheap bypass. The function uses existing `l2o`, `o2l`, `smootherstep`, and `PI` helpers.

## Processing

1. Convert the post-density linear-sRGB value to OKLab and retain the original Lab value.
2. Build a smooth highlight mask from OKLab `L`; no binary threshold controls the output.
3. Produce a rational shoulder target from positive highlight excess. Blend toward it with `shoulder_strength` and the highlight mask. The curve remains continuous, monotonic, and does not hard-clip values above one.
4. Compute chroma as `sqrt(a*a+b*b)`. Soft-limit only excessive highlight chroma using `highlight_compression`, then apply gradual radial reduction using `highlight_desaturation`. Scaling `a` and `b` together preserves hue angle.
5. Build a smooth deep-shadow mask. Raise `L` by a small bounded amount controlled by `shadow_lift`, while retaining perceived contrast through a fading mask rather than a global offset.
6. Slightly reduce chroma in shadows as a fixed scanner-response characteristic; never increase shadow saturation.
7. Apply `shadow_coolness` only along negative OKLab `b`. Gate it by the shadow mask, a chroma mask that excludes neutral gray/black, and a skin-protection mask centered on the existing skin anchor near `1.0685` radians. The module does not add green or teal because it does not shift the `a` axis.
8. Blend original and processed OKLab by `strength`, then convert back to linear sRGB.

## Protection Rules

- Tone operations alter only `L`, preserving hue.
- Highlight chroma operations scale `a/b` radially, preserving hue angle.
- Neutral chroma protection keeps white, gray, and black from acquiring color.
- Skin protection suppresses scanner chroma/coolness changes around the existing skin-hue anchor.
- Exact pass-through occurs when disabled or when `scan_strength=0.0`.
- Existing highlight, Expo Roll, density, and output behavior are unchanged when the scanner stage is neutral.

## Performance

The module adds one OKLab round trip and scalar masks/arithmetic. It adds no loops, texture reads, external LUTs, neighborhood operations, or per-pixel dynamic allocation.

## Verification

- Source assertions verify all controls, the `CineonScan()` signature, and placement after `out=d_nrgb` but before gain.
- Numeric checks verify pass-through, monotonic shoulder behavior, no hard white clipping, black-floor lift, radial hue preservation, neutral protection, skin protection, and non-increasing shadow chroma.
- The full DCTL passes the existing OpenCL syntax harness.
- Resolve refreshes the LUT list successfully. Existing DCTL instances may need the DCTL reselected once to expose the new controls.
