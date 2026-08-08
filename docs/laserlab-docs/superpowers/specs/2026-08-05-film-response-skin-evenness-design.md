# Film Response And Skin Evenness Design

## Goal

Rename the scanner-strength control to describe its visible finishing behavior and add a hue-only skin-evenness control for reducing red/yellow skin blotchiness.

## Controls

- Rename the visible `[SCAN] Cineon Scan` label to `[FINISH] Film Response`.
- Keep the existing `scan_strength` identifier, range, placement, and processing unchanged, but set its default to the recommended `0.25`.
- Add `skin_evenness` as `Skin Evenness`, range `0.0-1.0`, recommended default `0.25`, step `0.001`.

## Skin Processing

Skin Evenness runs inside the existing OKLab skin stage. It uses the existing skin anchor at `1.0685` radians and smooth hue/chroma masks to identify plausible skin colors. For selected pixels, it blends the OKLab hue angle toward the anchor by `skin_evenness` while preserving OKLab lightness and chroma magnitude.

The correction is per-pixel and does not blur texture, average neighboring pixels, alter luminance, or perform beauty smoothing. The default `0.25` gives a restrained correction; at `0.0`, it is an exact identity. Non-skin hues and near-neutral pixels remain unchanged.

Skin Evenness is applied after the existing Skin Hue, Skin Sat, and Skin Val adjustment so it can unify the final adjusted skin hue.

## Film Response

Only the public label changes. The existing response still combines highlight shoulder rolloff, highlight chroma softening, slight black lift, and cool-shadow bias after density and before output gain. Internal names such as `scan_strength` and `CineonScan()` remain unchanged to keep the implementation diff small.

## Verification

- Source assertions verify both UI labels and the neutral default.
- Numeric checks verify zero-strength identity, skin hues move toward the anchor, stronger values reduce angular error monotonically, and lightness/chroma remain unchanged.
- Numeric checks verify non-skin and near-neutral colors remain unchanged.
- Existing scanner and full regression checks continue to pass.
- The DCTL passes the existing OpenCL syntax harness.
