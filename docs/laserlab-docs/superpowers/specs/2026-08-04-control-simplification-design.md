# Inactive Control Simplification Design

## Goal

Remove reported subordinate controls that appear inert when their parent correction is neutral or bypassed. Preserve their useful internal behavior with fixed values and expose only controls that directly produce an image change.

## Skin Controls

Remove these UI parameters:

- `polar_smoothness`
- `neutral_smoothness`

Keep their current behavior through fixed internal constants:

- Polar interpolation smoothness: `5.0`
- Neutral chroma transition: `0.1`

Keep Skin Hue, Skin Sat, and Skin Value. The fixed constants apply whenever those skin corrections are used.

## Cineon Scanner Controls

Replace the seven existing scanner controls with one direct slider:

```c
DEFINE_UI_PARAMS(scan_strength, [SCAN] Cineon Scan, DCTLUI_SLIDER_FLOAT, 0.0, 0.0, 1.0, 0.001)
```

Remove:

- `enable_cineon_scan`
- `shoulder_strength`
- `highlight_compression`
- `highlight_desaturation`
- `shadow_lift`
- `shadow_coolness`

Call `CineonScan()` with fixed balanced parameters:

| Internal Parameter | Value |
|---|---:|
| Shoulder | `0.50` |
| Highlight chroma compression | `0.50` |
| Highlight desaturation | `0.35` |
| Shadow lift | `0.15` |
| Shadow coolness | `0.10` |

The effective scanner strength is:

```text
clamp(scan_strength + preset_scanner_strength, 0, 1)
```

- Neutral and IMAX Laser contribute `0.0` preset scanner strength.
- IMAX 15/70 contributes `0.25 * Look Strength`.
- The manual scanner slider always acts directly; no enable checkbox gates it.

## Compatibility

This intentionally removes UI parameters from newly instantiated DCTL nodes. Existing Resolve instances may retain stale controls until the DCTL is reselected. No compatibility shim is added because the project is not shipping a stable external parameter contract.

## Documentation and Backup

- Synchronize `backup/OKLabGrade-NoDRT-LKG.dctl` with the verified live DCTL.
- Update `HANDOFF-NODRT.md` control count, control table, defaults, and scanner description.

## Verification

- Source checks confirm all removed UI declarations are absent.
- Source checks confirm the fixed skin and scanner values at their call sites.
- Numeric checks confirm manual scanner strength is direct, IMAX Laser contributes zero, and IMAX 15/70 contributes `0.25` at full Look Strength.
- Existing preset, tone, saturation-compression, Rec.2020, and Cineon numerical checks continue to pass.
- The DCTL passes the OpenCL syntax harness.
- The live DCTL and LKG backup compare byte-for-byte.
- Resolve LUT refresh succeeds.
