# IMAX-Inspired Presets and Saturation Compression Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add Neutral, IMAX Laser, and IMAX 15/70 preset bases plus global hue-preserving OKLab saturation compression to the existing DCTL.

**Architecture:** A preset selector contributes a small set of base values that blend toward Neutral through Look Strength. Existing controls remain live modifiers around those bases. Saturation compression is one local OKLab helper applied after saturation/hue and before skin processing; no additional DCTL or dependency is introduced.

**Tech Stack:** DaVinci Resolve DCTL/OpenCL C, Python assertion-based regression checks, `clang` OpenCL syntax validation.

## Global Constraints

- Modify the existing `OKLabGrade-NoDRT.dctl`; do not create another DCTL.
- Default preset is IMAX Laser with Look Strength `1.0`.
- Presets are Neutral, IMAX Laser, and IMAX 15/70 in that order.
- Input Gamut and Input Transfer remain Rec.709 by default.
- Look Strength affects preset contributions only; manual controls remain active at every Look Strength value.
- Neutral with identity controls follows the existing neutral path.
- Saturation compression preserves hue and leaves near-neutral chroma at or below `0.04` unchanged.
- IMAX Laser does not activate Cineon Scan; IMAX 15/70 contributes scanner strength `0.25`.
- No new dependencies, texture reads, loops, spatial effects, or proprietary transforms.
- This workspace is not a Git repository; commit steps are omitted because commits are unavailable.

---

## File Structure

- Modify `OKLabGrade-NoDRT.dctl`: UI declarations, preset bases, effective control values, saturation helper, and scanner composition.
- Modify `test_tone_controls.py`: source assertions and numerical regression checks for presets, saturation compression, and scanner interaction.
- Preserve `docs/superpowers/specs/2026-08-04-imax-presets-sat-compression-design.md` as the source of requirements.

### Task 1: Preset Controls and Effective Tone/Color Values

**Files:**
- Modify: `test_tone_controls.py:8-20, 22-42`
- Modify: `OKLabGrade-NoDRT.dctl:118-174, 299-371`

**Interfaces:**
- Produces UI parameters `look_preset` and `look_strength`.
- Produces transform locals `lp`, `ls`, `e_contrast`, `e_shadows`, `e_highlights`, `e_desat_high`, `e_sat_amount`, `e_global_sat`, and `e_preset_scan` for later processing.

- [ ] **Step 1: Add failing source and numerical preset checks**

Add these declarations to the source assertions:

```python
assert "DEFINE_UI_PARAMS(look_preset, [LOOK] Preset, DCTLUI_COMBO_BOX, 1, {look_neutral,look_imax_laser,look_imax_1570},{Neutral,IMAX Laser,IMAX 15-70})" in source
assert "DEFINE_UI_PARAMS(look_strength, Look Strength, DCTLUI_SLIDER_FLOAT, 1.0, 0.0, 1.0, 0.001)" in source
```

Add this small numerical model:

```python
PRESETS = {
    0: (1.00, 0.00, 0.00, 0.00, 1.00, 0.00, 0.00, 0.00),
    1: (1.08, 0.08, 0.18, 0.10, 1.08, 0.35, 0.06, 0.00),
    2: (1.05, 0.12, 0.28, 0.15, 1.04, 0.45, 0.10, 0.25),
}


def preset_values(preset, strength):
    contrast, shadows, highlights, desat, sat, compression, sub_sat, scanner = PRESETS[preset]
    return (
        1.0 + (contrast - 1.0) * strength,
        shadows * strength,
        highlights * strength,
        desat * strength,
        1.0 + (sat - 1.0) * strength,
        compression * strength,
        sub_sat * strength,
        scanner * strength,
    )


assert preset_values(0, 1.0) == PRESETS[0]
assert preset_values(1, 0.0) == PRESETS[0]
assert preset_values(1, 1.0) == PRESETS[1]
assert preset_values(2, 1.0)[-1] == 0.25
assert preset_values(1, 1.0)[-1] == 0.0
```

- [ ] **Step 2: Run the regression script and verify RED**

Run:

```bash
python3 test_tone_controls.py
```

Expected: `AssertionError` because `look_preset` is absent from the DCTL.

- [ ] **Step 3: Add the preset UI declarations**

Insert before Input Decode:

```c
// ---- LOOK PRESETS ----
DEFINE_UI_PARAMS(look_preset, [LOOK] Preset, DCTLUI_COMBO_BOX, 1, {look_neutral,look_imax_laser,look_imax_1570},{Neutral,IMAX Laser,IMAX 15-70})
DEFINE_UI_PARAMS(look_strength, Look Strength, DCTLUI_SLIDER_FLOAT, 1.0, 0.0, 1.0, 0.001)
```

- [ ] **Step 4: Compute preset bases once per transform**

Insert after the input indices at the start of `transform`:

```c
  int lp=_fminf(_fmaxf((int)(look_preset+0.5f),0),2);
  float ls=_fminf(_fmaxf(look_strength,0.0f),1.0f);
  float p_contrast=lp==1?1.08f:(lp==2?1.05f:1.0f);
  float p_shadows=lp==1?0.08f:(lp==2?0.12f:0.0f);
  float p_highlights=lp==1?0.18f:(lp==2?0.28f:0.0f);
  float p_desat=lp==1?0.10f:(lp==2?0.15f:0.0f);
  float p_sat=lp==1?1.08f:(lp==2?1.04f:1.0f);
  float p_global_sat=lp==1?0.06f:(lp==2?0.10f:0.0f);
  float e_contrast=contrast*(1.0f+(p_contrast-1.0f)*ls);
  float e_shadows=_fminf(shadows+p_shadows*ls,1.0f);
  float e_highlights=_fminf(highlights+p_highlights*ls,1.0f);
  float e_desat_high=_fminf(desat_high+p_desat*ls,1.0f);
  float e_sat_amount=sat_amount*(1.0f+(p_sat-1.0f)*ls);
  float e_global_sat=_fminf(global_sat+p_global_sat*ls,1.0f);
  float e_preset_scan=lp==2?0.25f*ls:0.0f;
```

- [ ] **Step 5: Route existing processing through effective values**

Use `e_sat_amount` in place of `sat_amount`, `e_contrast` in place of `contrast`, `e_shadows/e_highlights` in `hdr_recover`, `e_desat_high` in the highlight desaturation block, and `e_global_sat` when computing `d_gs`:

```c
  float sa=e_sat_amount,av=lab.y,bv=lab.z;
  L=(L-0.5f)*e_contrast+0.5f;
  L=hdr_recover(L,e_shadows,e_highlights);lab.x=L;
  if(e_desat_high>0.001f){
    float invThresh=1.0f-e_desat_high;
```

```c
  float d_gs=e_global_sat*d_asat;
```

- [ ] **Step 6: Run the regression script**

Run:

```bash
python3 test_tone_controls.py
```

Expected: PASS with no output.

### Task 2: Global OKLab Saturation Compression

**Files:**
- Modify: `test_tone_controls.py:43-123`
- Modify: `OKLabGrade-NoDRT.dctl:143-147, 224-273, 319-340`

**Interfaces:**
- Consumes transform locals `lp` and `ls` from Task 1.
- Produces `compress_oklab_chroma(float3 lab, float amount) -> float3`.
- Produces transform local `e_sat_compression`.

- [ ] **Step 1: Add failing saturation-compression checks**

Add source assertions:

```python
assert "DEFINE_UI_PARAMS(sat_compression, Sat Compression, DCTLUI_SLIDER_FLOAT, 0.0, 0.0, 1.0, 0.001)" in source
assert "lab=compress_oklab_chroma(lab,e_sat_compression);" in source
```

Add the numerical model and behavior checks:

```python
def compress_chroma(lab, amount):
    lightness, a_axis, b_axis = lab
    chroma = math.hypot(a_axis, b_axis)
    if amount <= 0.0 or chroma <= 0.04:
        return lab
    excess = chroma - 0.04
    target = 0.04 + excess / (1.0 + 12.0 * amount * excess)
    scale = target / chroma
    return lightness, a_axis * scale, b_axis * scale


low = (0.5, 0.02, 0.01)
assert compress_chroma(low, 1.0) == low
ordinary = (0.5, 0.08, 0.06)
assert math.hypot(*compress_chroma(ordinary, 1.0)[1:]) < 0.08
sample = (0.5, -0.24, 0.32)
compressed = compress_chroma(sample, 0.35)
assert math.hypot(compressed[1], compressed[2]) < math.hypot(sample[1], sample[2])
assert abs(math.atan2(compressed[2], compressed[1]) - math.atan2(sample[2], sample[1])) < 1e-7
assert compress_chroma(sample, 0.0) == sample
```

- [ ] **Step 2: Run the regression script and verify RED**

Run:

```bash
python3 test_tone_controls.py
```

Expected: `AssertionError` because `sat_compression` and `compress_oklab_chroma` are absent.

- [ ] **Step 3: Add the slider and helper**

Place the slider beside Sat Amount:

```c
DEFINE_UI_PARAMS(sat_compression, Sat Compression, DCTLUI_SLIDER_FLOAT, 0.0, 0.0, 1.0, 0.001)
```

Place the helper after the OKLab conversion helpers:

```c
__DEVICE__ float3 compress_oklab_chroma(float3 lab,float amount){
  float c=_sqrtf(lab.y*lab.y+lab.z*lab.z);
  if(amount<=0.0f||c<=0.04f)return lab;
  float excess=c-0.04f;
  float target=0.04f+excess/(1.0f+12.0f*amount*excess);
  float scale=target/c;
  lab.y*=scale;lab.z*=scale;
  return lab;
}
```

- [ ] **Step 4: Apply compression at the specified boundary**

After Task 1's preset bases, add the effective compression value:

```c
  float p_sat_compression=lp==1?0.35f:(lp==2?0.45f:0.0f);
  float e_sat_compression=_fminf(sat_compression+p_sat_compression*ls,1.0f);
```

Immediately after saturation and hue rotation and before tonal/skin processing, add:

```c
  lab.y=av*ca-bv*sak;lab.z=av*sak+bv*ca;
  lab=compress_oklab_chroma(lab,e_sat_compression);
```

- [ ] **Step 5: Run the regression script and verify GREEN**

Run:

```bash
python3 test_tone_controls.py
```

Expected: PASS with no output.

### Task 3: IMAX 15/70 Scanner Composition and Full Verification

**Files:**
- Modify: `test_tone_controls.py:43-123`
- Modify: `OKLabGrade-NoDRT.dctl:416-418`

**Interfaces:**
- Consumes `e_preset_scan` from Task 1 and existing `enable_cineon_scan`, `scan_strength`, and scanner controls.
- Produces local `e_scan_strength` clamped to `0.0-1.0`.

- [ ] **Step 1: Add a failing source check for scanner composition**

Add:

```python
assert "float e_scan_strength=_fminf((enable_cineon_scan>0.5f?scan_strength:0.0f)+e_preset_scan,1.0f);" in source
assert "if(e_scan_strength>0.0f)" in source
assert "out=CineonScan(out,e_scan_strength,shoulder_strength,highlight_compression,highlight_desaturation,shadow_lift,shadow_coolness);" in source
```

- [ ] **Step 2: Run the regression script and verify RED**

Run:

```bash
python3 test_tone_controls.py
```

Expected: `AssertionError` because the old scanner call is still guarded only by `enable_cineon_scan`.

- [ ] **Step 3: Compose manual and preset scanner strengths**

Replace the old scanner guard and call with:

```c
  float e_scan_strength=_fminf((enable_cineon_scan>0.5f?scan_strength:0.0f)+e_preset_scan,1.0f);
  if(e_scan_strength>0.0f)
    out=CineonScan(out,e_scan_strength,shoulder_strength,highlight_compression,highlight_desaturation,shadow_lift,shadow_coolness);
```

- [ ] **Step 4: Run all numerical/source regressions**

Run:

```bash
python3 test_tone_controls.py
```

Expected: PASS with no output.

- [ ] **Step 5: Compile the complete DCTL as OpenCL C**

Run:

```bash
clang -x cl -cl-std=CL1.2 -fsyntax-only -Werror \
  -D'__DEVICE__=' \
  -D'DEFINE_UI_PARAMS(name,label,type,def,...)=__constant float name=def;' \
  -D'DCTLUI_COMBO_BOX=0' -D'DCTLUI_SLIDER_FLOAT=0' -D'DCTLUI_CHECK_BOX=0' \
  -D'make_float3(a,b,c)=((float3)(a,b,c))' -D'make_float2(a,b)=((float2)(a,b))' \
  -D'_powf=pow' -D'_fabs=fabs' \
  -D'_fminf(a,b)=((a)<(b)?(a):(b))' -D'_fmaxf(a,b)=((a)>(b)?(a):(b))' \
  -D'_sqrtf=sqrt' -D'_expf=exp' -D'_log10f=log10' -D'_log2f=log2' \
  -D'_copysignf=copysign' -D'_cosf=cos' -D'_sinf=sin' \
  -D'_fmod=fmod' -D'_atan2f=atan2' \
  OKLabGrade-NoDRT.dctl
```

Expected: exit code `0` with no output.

- [ ] **Step 6: Refresh Resolve and verify the installed source**

Call Resolve's LUT refresh, then reselect the DCTL on one test node so the new UI declarations instantiate. Verify:

- Preset defaults to IMAX Laser.
- Look Strength defaults to `1.0`.
- Neutral plus identity controls matches the prior neutral image.
- Look Strength changes only preset contribution; manual sliders still respond at zero.
- IMAX Laser has no scanner response.
- IMAX 15/70 adds a subtle scanner response.
- Sat Compression reduces excessive color without rotating hue.

Expected: Resolve refresh succeeds and the reselected DCTL exposes all three new controls without a compile error overlay.
