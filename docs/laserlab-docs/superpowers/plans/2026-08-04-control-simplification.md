# Inactive Control Simplification Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Remove inert subordinate skin/scanner controls while preserving their useful behavior through fixed internal values and one direct Cineon Scan slider.

**Architecture:** Skin interpolation keeps fixed smoothness constants at the existing call sites. Cineon scanning exposes one direct strength parameter that adds to the IMAX 15/70 preset contribution; all other scanner characteristics become fixed arguments to the existing `CineonScan()` function.

**Tech Stack:** DaVinci Resolve DCTL/OpenCL C, Python assertion-based regressions, `clang` OpenCL syntax validation.

## Global Constraints

- Modify only the existing DCTL implementation; do not create another DCTL.
- Remove `polar_smoothness`, `neutral_smoothness`, `enable_cineon_scan`, `shoulder_strength`, `highlight_compression`, `highlight_desaturation`, `shadow_lift`, and `shadow_coolness` UI parameters.
- Keep skin smoothness internally fixed at `5.0` and `0.1`.
- Expose `scan_strength` as `[SCAN] Cineon Scan`, range `0.0-1.0`, default `0.0`.
- Fix scanner shoulder/compression/desaturation/lift/coolness at `0.50/0.50/0.35/0.15/0.10`.
- Neutral and IMAX Laser contribute no scanner strength; IMAX 15/70 contributes `0.25 * Look Strength`.
- Synchronize the verified live source to `backup/OKLabGrade-NoDRT-LKG.dctl`.
- Update `HANDOFF-NODRT.md` to match the implemented UI.
- This workspace is not a Git repository; commit steps are unavailable.

---

### Task 1: Remove Subordinate Controls

**Files:**
- Modify: `test_tone_controls.py`
- Modify: `OKLabGrade-NoDRT.dctl`

**Interfaces:**
- Keeps `CineonScan(float3, float, float, float, float, float, float)` unchanged.
- Produces one UI scanner parameter: `scan_strength`.

- [ ] **Step 1: Write failing source checks**

Replace scanner declaration/call assertions and add removal checks:

```python
assert "DEFINE_UI_PARAMS(scan_strength, [SCAN] Cineon Scan, DCTLUI_SLIDER_FLOAT, 0.0, 0.0, 1.0, 0.001)" in source

for removed in (
    "DEFINE_UI_PARAMS(polar_smoothness,",
    "DEFINE_UI_PARAMS(neutral_smoothness,",
    "DEFINE_UI_PARAMS(enable_cineon_scan,",
    "DEFINE_UI_PARAMS(shoulder_strength,",
    "DEFINE_UI_PARAMS(highlight_compression,",
    "DEFINE_UI_PARAMS(highlight_desaturation,",
    "DEFINE_UI_PARAMS(shadow_lift,",
    "DEFINE_UI_PARAMS(shadow_coolness,",
):
    assert removed not in source

assert "float3 sk_adj=ev_curves(sk_h,sk_p,5.0f);" in source
assert "float sk_nm=smootherstep(0.0f,0.1f,sk_c);" in source
assert "float e_scan_strength=_fminf(scan_strength+e_preset_scan,1.0f);" in source
call = "out=CineonScan(out,e_scan_strength,0.50f,0.50f,0.35f,0.15f,0.10f);"
assert call in source
```

- [ ] **Step 2: Run tests and verify RED**

Run `python3 test_tone_controls.py`.

Expected: `AssertionError` because the old subordinate declarations still exist.

- [ ] **Step 3: Simplify the skin UI and call sites**

Delete the `polar_smoothness` and `neutral_smoothness` declarations. Replace their uses with:

```c
  float3 sk_adj=ev_curves(sk_h,sk_p,5.0f);
  float sk_nm=smootherstep(0.0f,0.1f,sk_c);
```

- [ ] **Step 4: Simplify the scanner UI and call site**

Replace the scanner declarations with:

```c
// --- CINEON SCAN — post-negative scanner response ---
DEFINE_UI_PARAMS(scan_strength, [SCAN] Cineon Scan, DCTLUI_SLIDER_FLOAT, 0.0, 0.0, 1.0, 0.001)
```

Replace scanner strength composition and invocation with:

```c
  float e_scan_strength=_fminf(scan_strength+e_preset_scan,1.0f);
  if(e_scan_strength>0.0f)
    out=CineonScan(out,e_scan_strength,0.50f,0.50f,0.35f,0.15f,0.10f);
```

- [ ] **Step 5: Run tests and verify GREEN**

Run `python3 test_tone_controls.py`.

Expected: PASS with no output.

### Task 2: Documentation, Backup, and Integration Verification

**Files:**
- Modify: `HANDOFF-NODRT.md`
- Replace: `backup/OKLabGrade-NoDRT-LKG.dctl`

**Interfaces:**
- Consumes the verified live DCTL from Task 1.
- Produces a byte-identical LKG backup and current operator documentation.

- [ ] **Step 1: Update the handoff**

Change the documented control count from `44` to `36`. Remove the two skin smoothness controls and replace the seven scanner controls with `scan_strength`. Document its visible default as `0.0` and the fixed scanner internals as `0.50/0.50/0.35/0.15/0.10`.

- [ ] **Step 2: Compile and run regressions**

Run `python3 test_tone_controls.py` and the existing `clang -x cl -cl-std=CL1.2 -fsyntax-only -Werror ... OKLabGrade-NoDRT.dctl` harness.

Expected: both exit `0` with no output.

- [ ] **Step 3: Synchronize the backup**

Copy `OKLabGrade-NoDRT.dctl` to `backup/OKLabGrade-NoDRT-LKG.dctl` after verification.

- [ ] **Step 4: Verify backup identity**

Run `cmp -s OKLabGrade-NoDRT.dctl backup/OKLabGrade-NoDRT-LKG.dctl`.

Expected: exit `0` with no output.

- [ ] **Step 5: Refresh Resolve LUTs**

Call Resolve LUT refresh and verify it returns success. Reselect the DCTL once in Resolve to discard stale controls from the existing instance.
