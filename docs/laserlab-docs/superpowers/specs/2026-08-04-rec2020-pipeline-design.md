# Rec.2020 Pipeline Design

## Goal

Preserve Apple Log saturation through the OKLab grading round trip and align the project viewer with the Filmbox LUT's Rec.709 Gamma 2.4 output.

## Design

- Append Rec.2020 to the DCTL input-gamut combo so existing stored indices remain unchanged.
- Add standard BT.2020/D65 RGB-to-XYZ and XYZ-to-RGB matrices and route gamut index `10` through them.
- Keep the DCTL default gamut unchanged; the current Apple Log node must be set to Rec2020 manually because Resolve scripting cannot write OFX parameters.
- Set project output to Rec.709 Gamma 2.4, timeline working luminance to SDR 100 nits, and inverse DRT off.
- Leave global macOS display-profile and QuickTime-matching preferences unchanged.

## Verification

- Extend the source regression check for the Rec.2020 combo, matrices, and routing.
- Run the OpenCL syntax check and refresh Resolve's LUT list.
- Preview project-setting changes before applying them and read the settings back afterward.
