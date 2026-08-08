# Tone Controls Design

## Goal

Make Shadows and Highlights visibly effective, reduce Contrast sensitivity, and make the main UI controls easier to scan.

## Design

- Apply Shadows and Highlights to perceptual OKLab `L` before converting back to linear RGB. Keep both defaults at `0.0` for neutral output.
- Limit Contrast to `0.5-1.5` with a `1.0` neutral default and `0.001` step.
- Prefix only major controls with parser-safe visual tags: `[WB] Temp`, `[TONE] Contrast`, `[SH] Shadows`, `[HI] Highlights`, `[EXP] Expo Global`, and `[SAT] Sat Amount`. Emoji labels were rejected by the OpenCL compiler.
- Keep the existing input/output color-space round trip and all unrelated controls unchanged.

## Verification

- Add a small numeric check proving neutral tone controls are identity and nonzero Shadows/Highlights alter representative low/high OKLab `L` values.
- Compile the DCTL with the existing OpenCL syntax harness.
- Refresh Resolve's LUT list. Existing OFX DCTL instances may require reselecting the DCTL once to rebuild cached parameters.
