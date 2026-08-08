# LaserLab Combined Upgrade Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Rename the project to LaserLab and ship one coherent CAT16, D65-adapted, Rec.2020-working-gamut grading revision that preserves finite extended output.

**Architecture:** Decode into native camera RGB, convert through XYZ with D60/D65 adaptation where required, perform CAT16 white balance, and grade in linear Rec.2020 with explicit OKLab boundary conversions. Compress destination-gamut excursions along the achromatic axis, harden Expo Roll, expose IMAX Laser-inspired values directly as slider defaults, and return finite encoded values without `[0,1]` clipping.

**Tech Stack:** DaVinci Resolve DCTL/OpenCL C, Python 3 assert-based regression script, `clang` OpenCL syntax validation.

## Global Constraints

- Keep one canonical live DCTL and one byte-identical last-known-good backup.
- Do not add dependencies or PowerGrades.
- Preserve all eleven input gamut choices and fourteen transfer choices.
- IMAX Laser-inspired values are visible slider defaults, not hidden preset contributions.
- Saturation always operates in OKLab; remove the saturation-type dropdown and alternate branches.
- Temp/Tint is camera white-balance correction.
- Preserve finite superwhites and negative output values.
- Rename the folder, DCTL, backup, tests, documentation, headings, and internal references to `LaserLab`.
- This directory is not a Git repository, so commit steps are intentionally omitted.

---

### Task 1: Lock The New Pipeline Contract With Failing Tests

**Files:**
- Modify: `test_tone_controls.py`
- Test: `test_tone_controls.py`

**Interfaces:**
- Consumes: final source file `LaserLab.dctl` as text.
- Produces: source-contract and numerical checks used by every later task.

- [ ] **Step 1: Point the test at the final filename and replace obsolete preset assertions**

```python
source = Path(__file__).with_name("LaserLab.dctl").read_text()

assert "// LaserLab.dctl" in source
assert "DEFINE_UI_PARAMS(look_preset," not in source
assert "DEFINE_UI_PARAMS(look_strength," not in source
assert "DEFINE_UI_PARAMS(sat_type," not in source
assert "DEFINE_UI_PARAMS(contrast, [TONE] Contrast, DCTLUI_SLIDER_FLOAT, 1.08," in source
assert "DEFINE_UI_PARAMS(shadows, [SH] Shadows, DCTLUI_SLIDER_FLOAT, 0.08," in source
assert "DEFINE_UI_PARAMS(highlights, [HI] Highlights, DCTLUI_SLIDER_FLOAT, 0.18," in source
assert "DEFINE_UI_PARAMS(desat_high, Desat Highlights, DCTLUI_SLIDER_FLOAT, 0.10," in source
assert "DEFINE_UI_PARAMS(sat_amount, [SAT] Sat Amount, DCTLUI_SLIDER_FLOAT, 1.08," in source
assert "DEFINE_UI_PARAMS(sat_compression, Sat Compression, DCTLUI_SLIDER_FLOAT, 0.35," in source
assert "DEFINE_UI_PARAMS(global_sat, Sub Sat, DCTLUI_SLIDER_FLOAT, 0.06," in source
```

- [ ] **Step 2: Add structural assertions for CAT16, Rec.2020 working RGB, adaptation, gamut compression, and unclipped output**

```python
for required in (
    "__DEVICE__ float3 cat16_adapt(",
    "__DEVICE__ float3 white_balance(",
    "__DEVICE__ float3 d60_to_d65(",
    "__DEVICE__ float3 d65_to_d60(",
    "__DEVICE__ float3 work_to_oklab(",
    "__DEVICE__ float3 oklab_to_work(",
    "__DEVICE__ float3 compress_to_gamut(",
    "float3 work=x2g(10,xyz);",
):
    assert required in source

assert "srgb=make_float3(srgb.x*tg,srgb.y*ti,srgb.z/tg);" not in source
assert "_fmaxf(_fminf(result.x,1.0f),0.0f)" not in source
assert "float safe_esh=_fmaxf(esh,1e-4f);" in source
```

- [ ] **Step 3: Add compact CAT16 and gamut-compression reference checks**

```python
CAT16 = (
    (0.401288, 0.650173, -0.051461),
    (-0.250268, 1.204414, 0.045854),
    (-0.002079, 0.048952, 0.953127),
)


def mat_vec(matrix, vector):
    return tuple(sum(row[i] * vector[i] for i in range(3)) for row in matrix)


def compress_to_gamut(rgb, luminance):
    scale = 1.0
    for channel in rgb:
        delta = channel - luminance
        if channel < 0.0 and delta < 0.0:
            scale = min(scale, luminance / -delta)
    scale = max(0.0, min(1.0, scale))
    return tuple(luminance + (channel - luminance) * scale for channel in rgb)


compressed = compress_to_gamut((1.4, -0.2, 0.7), 0.5)
assert min(compressed) >= -1e-12
assert compressed[0] > compressed[2] > compressed[1]
assert compress_to_gamut((2.0, 1.2, 1.0), 1.3) == (2.0, 1.2, 1.0)
```

- [ ] **Step 4: Run the test and verify the rename contract fails**

Run: `python3 test_tone_controls.py`

Expected: failure because `LaserLab.dctl` does not exist yet.

---

### Task 2: Rename The Primary Files And Expose Laser Defaults

**Files:**
- Rename: `OKLabGrade-NoDRT.dctl` to `LaserLab.dctl`
- Modify: `LaserLab.dctl:1-170,305-324`
- Rename: `HANDOFF-NODRT.md` to `HANDOFF-LASERLAB.md`
- Modify: `generate_matrices.py:1-3`
- Test: `test_tone_controls.py`

**Interfaces:**
- Consumes: existing DCTL controls and current IMAX Laser internal constants.
- Produces: `LaserLab.dctl` with direct defaults and no runtime preset layer.

- [ ] **Step 1: Rename the DCTL and handoff files**

Rename `OKLabGrade-NoDRT.dctl` to `LaserLab.dctl` and `HANDOFF-NODRT.md` to `HANDOFF-LASERLAB.md` without retaining duplicate live files.

- [ ] **Step 2: Replace the source heading and delete hidden preset controls**

```c
// LaserLab.dctl
// Wide-gamut CAT16/OKLab grading node with native-space roundtrip.
// Decodes input -> grades in Rec.2020/OKLab -> re-encodes to input space.
```

Delete both `look_preset` and `look_strength` declarations.

- [ ] **Step 3: Move the IMAX Laser-inspired values into visible defaults**

```c
DEFINE_UI_PARAMS(contrast, [TONE] Contrast, DCTLUI_SLIDER_FLOAT, 1.08, 0.5, 1.5, 0.001)
DEFINE_UI_PARAMS(shadows, [SH] Shadows, DCTLUI_SLIDER_FLOAT, 0.08, 0.0, 1.0, 0.001)
DEFINE_UI_PARAMS(highlights, [HI] Highlights, DCTLUI_SLIDER_FLOAT, 0.18, 0.0, 1.0, 0.001)
DEFINE_UI_PARAMS(desat_high, Desat Highlights, DCTLUI_SLIDER_FLOAT, 0.10, 0.0, 1.0, 0.001)
DEFINE_UI_PARAMS(sat_amount, [SAT] Sat Amount, DCTLUI_SLIDER_FLOAT, 1.08, 0.0, 2.0, 0.001)
DEFINE_UI_PARAMS(sat_compression, Sat Compression, DCTLUI_SLIDER_FLOAT, 0.35, 0.0, 1.0, 0.001)
DEFINE_UI_PARAMS(global_sat, Sub Sat, DCTLUI_SLIDER_FLOAT, 0.06, -1.0, 1.0, 0.001)
```

- [ ] **Step 4: Delete preset calculations from `transform` and use direct controls**

Remove `lp`, `ls`, every `p_*`, every `e_*` preset blend, and `e_preset_scan`. Use `contrast`, `shadows`, `highlights`, `desat_high`, `sat_amount`, `sat_compression`, `global_sat`, and `scan_strength` directly.

- [ ] **Step 5: Remove saturation modes and keep direct OKLab chroma scaling**

Delete the `sat_type` UI declaration, `st`, and all Vibrance/HDR/Natural branches. Replace them with:

```c
float av=lab.y*sat_amount,bv=lab.z*sat_amount;
```

- [ ] **Step 6: Update the generator identity**

```python
"""Generate C #define constants for LaserLab.dctl using colour-science 0.4.7."""
```

- [ ] **Step 7: Run the tests and confirm the remaining pipeline assertions fail**

Run: `python3 test_tone_controls.py`

Expected: failure on the first missing CAT16 or wide-gamut helper assertion, not on the filename or visible defaults.

---

### Task 3: Add CAT16, White-Point Adaptation, And Rec.2020 Boundaries

**Files:**
- Modify: `LaserLab.dctl:40-96,172-232,305-380`
- Test: `test_tone_controls.py`

**Interfaces:**
- Produces: `cat16_adapt(float3,float3,float3)`, `d60_to_d65(float3)`, `d65_to_d60(float3)`, `white_balance(float3,float,float)`, `work_to_oklab(float3)`, and `oklab_to_work(float3)`.
- Consumes: existing `mmul`, `g2x`, `x2g`, `l2o`, and `o2l` helpers.

- [ ] **Step 1: Add CAT16 and white constants after the RGB/XYZ matrices**

```c
#define CAT16_C0 make_float3(0.401288f,-0.250268f,-0.002079f)
#define CAT16_C1 make_float3(0.650173f,1.204414f,0.048952f)
#define CAT16_C2 make_float3(-0.051461f,0.045854f,0.953127f)
#define CAT16I_C0 make_float3(1.86206786f,0.38752654f,-0.01584150f)
#define CAT16I_C1 make_float3(-1.01125463f,0.62144744f,-0.03412294f)
#define CAT16I_C2 make_float3(0.14918677f,-0.00897398f,1.04996444f)
#define WHITE_D65 make_float3(0.9504559271f,1.0f,1.0890577508f)
#define WHITE_D60 make_float3(0.9526460746f,1.0f,1.0088251844f)
```

- [ ] **Step 2: Add CAT16 adaptation and CCT white-balance helpers after `mmul`**

```c
__DEVICE__ float3 cat16_adapt(float3 xyz,float3 src_white,float3 dst_white){
  float3 lms=mmul(CAT16_C0,CAT16_C1,CAT16_C2,xyz);
  float3 src=mmul(CAT16_C0,CAT16_C1,CAT16_C2,src_white);
  float3 dst=mmul(CAT16_C0,CAT16_C1,CAT16_C2,dst_white);
  lms=make_float3(lms.x*dst.x/src.x,lms.y*dst.y/src.y,lms.z*dst.z/src.z);
  return mmul(CAT16I_C0,CAT16I_C1,CAT16I_C2,lms);
}

__DEVICE__ float3 d60_to_d65(float3 xyz){return cat16_adapt(xyz,WHITE_D60,WHITE_D65);}
__DEVICE__ float3 d65_to_d60(float3 xyz){return cat16_adapt(xyz,WHITE_D65,WHITE_D60);}

__DEVICE__ float3 white_balance(float3 xyz,float temp_value,float tint_value){
  float k=6504.0f*_expf(temp_value*2.0f);
  float k2=k*k,k3=k2*k;
  float x=k<=4000.0f?-0.2661239e9f/k3-0.2343580e6f/k2+0.8776956e3f/k+0.179910f:-3.0258469e9f/k3+2.1070379e6f/k2+0.2226347e3f/k+0.240390f;
  float y=k<=4000.0f?-1.1063814f*x*x*x-1.34811020f*x*x+2.18555832f*x-0.20219683f:3.0817580f*x*x*x-5.8733867f*x*x+3.75112997f*x-0.37001483f;
  y=_fmaxf(y+tint_value*0.05f,0.05f);
  float3 source_white=make_float3(x/y,1.0f,(1.0f-x-y)/y);
  return cat16_adapt(xyz,source_white,WHITE_D65);
}
```

- [ ] **Step 3: Add explicit OKLab boundary helpers**

```c
__DEVICE__ float3 work_to_oklab(float3 work){
  float3 xyz=g2x(10,work);
  return l2o(mmul(I0_C0,I0_C1,I0_C2,xyz));
}

__DEVICE__ float3 oklab_to_work(float3 lab){
  float3 srgb=o2l(lab);
  return x2g(10,mmul(S0_C0,S0_C1,S0_C2,srgb));
}
```

- [ ] **Step 4: Replace the forward transform boundary**

```c
float3 lin=tfd(r,ti_tf);
float3 xyz=g2x(g,lin);
if(g==8)xyz=d60_to_d65(xyz);
xyz=white_balance(xyz,temp,tint);
float3 work=x2g(10,xyz);
float3 lab=work_to_oklab(work);
```

Remove the decoded-input clamp, linear-sRGB working variable, and RGB Temp/Tint gains.

- [ ] **Step 5: Return perceptual operations to Rec.2020**

Replace `float3 out=o2l(lab);` with:

```c
float3 out=oklab_to_work(lab);
```

Inside `CineonScan`, replace `l2o(rgb)` and `o2l(lab)` with `work_to_oklab(rgb)` and `oklab_to_work(lab)`.

- [ ] **Step 6: Run tests**

Run: `python3 test_tone_controls.py`

Expected: CAT16 and working-space assertions pass; failure remains at gamut compression or output safety.

---

### Task 4: Add Destination-Gamut Compression And Numerical Safety

**Files:**
- Modify: `LaserLab.dctl:232-249,378-459`
- Test: `test_tone_controls.py`

**Interfaces:**
- Produces: `compress_to_gamut(float3 rgb,float luminance)` and finite extended output.
- Consumes: Rec.2020 `out`, selected destination gamut `g`, and XYZ luminance.

- [ ] **Step 1: Add hue-preserving destination-gamut compression**

```c
__DEVICE__ float3 compress_to_gamut(float3 rgb,float luminance){
  if(luminance<=0.0f)return rgb;
  float scale=1.0f;
  float3 delta=rgb-make_float3(luminance,luminance,luminance);
  if(rgb.x<0.0f&&delta.x<0.0f)scale=_fminf(scale,luminance/-delta.x);
  if(rgb.y<0.0f&&delta.y<0.0f)scale=_fminf(scale,luminance/-delta.y);
  if(rgb.z<0.0f&&delta.z<0.0f)scale=_fminf(scale,luminance/-delta.z);
  scale=_fmaxf(_fminf(scale,1.0f),0.0f);
  return make_float3(luminance+delta.x*scale,luminance+delta.y*scale,luminance+delta.z*scale);
}
```

- [ ] **Step 2: Harden Expo Roll against zero and negative shoulder widths**

```c
float esh=(1.0f-exp_knee)*exp_stops+exp_shoulder;
float safe_esh=_fmaxf(esh,1e-4f);
float3 ecomp=make_float3(eov.x/(1.0f+eov.x/safe_esh),eov.y/(1.0f+eov.y/safe_esh),eov.z/(1.0f+eov.z/safe_esh));
```

Use `safe_esh`; do not use `esh` as a divisor.

- [ ] **Step 3: Replace the reverse boundary and preserve finite extended output**

```c
float3 rxyz=g2x(10,out);
if(g==8)rxyz=d65_to_d60(rxyz);
float3 cam=x2g(g,rxyz);
cam=compress_to_gamut(cam,rxyz.y);
float3 result=tfe(cam,ti_tf);
float nx=(result.x==result.x&&_fabs(result.x)<3.402823e38f)?result.x:0.0f;
float ny=(result.y==result.y&&_fabs(result.y)<3.402823e38f)?result.y:0.0f;
float nz=(result.z==result.z&&_fabs(result.z)<3.402823e38f)?result.z:0.0f;
return make_float3(nx,ny,nz);
```

Remove the pre-encode non-negative clamp and final `[0,1]` clamp.

- [ ] **Step 4: Run all Python checks**

Run: `python3 test_tone_controls.py`

Expected: exit code `0` with no output.

- [ ] **Step 5: Compile as OpenCL with warnings treated as errors**

Run the existing OpenCL harness command used for this project against `LaserLab.dctl`.

Expected: exit code `0`, no diagnostics.

---

### Task 5: Rename The Project Tree, Documentation, And Backup

**Files:**
- Rename: project folder `OKLabGrade-NoDRT` to `LaserLab`
- Rename: `backup/OKLabGrade-NoDRT-LKG.dctl` to `backup/LaserLab-LKG.dctl`
- Modify: `HANDOFF-LASERLAB.md`
- Modify: `docs/superpowers/specs/*.md`
- Modify: `docs/superpowers/plans/*.md`
- Modify: `test_tone_controls.py`

**Interfaces:**
- Consumes: verified `LaserLab.dctl`.
- Produces: one consistently named project tree and byte-identical backup.

- [ ] **Step 1: Replace current-name references in maintained documentation**

Update headings, file trees, signal-flow descriptions, control count, defaults, white-balance description, working gamut, extended-output policy, and source references. Historical design documents may describe old behavior but must identify it as the former `OKLabGrade-NoDRT` implementation.

- [ ] **Step 2: Synchronize and rename the backup**

Copy the verified `LaserLab.dctl` bytes over the existing LKG contents, then rename the file to `backup/LaserLab-LKG.dctl`.

- [ ] **Step 3: Rename the project folder**

Rename:

```text
.../_Dev/OKLabGrade-NoDRT
```

to:

```text
.../_Dev/LaserLab
```

Do not leave an alias folder or duplicate DCTL.

- [ ] **Step 4: Verify no active project references use the old name**

Search the final `LaserLab` tree for `OKLabGrade-NoDRT` and `HANDOFF-NODRT`. Expected matches are limited to historical statements explicitly naming the former implementation; executable paths and current file inventories must have zero matches.

- [ ] **Step 5: Run final regression and backup checks from the renamed folder**

Run: `python3 test_tone_controls.py`

Expected: exit code `0` with no output.

Run: `cmp -s "LaserLab.dctl" "backup/LaserLab-LKG.dctl"`

Expected: exit code `0`.

- [ ] **Step 6: Refresh Resolve LUTs**

Call Resolve's LUT refresh. Expected: success and `LaserLab` visible after reselecting the DCTL. If the scripting API still times out, record that as the only external verification gap and instruct Resolve to refresh LUTs manually.
