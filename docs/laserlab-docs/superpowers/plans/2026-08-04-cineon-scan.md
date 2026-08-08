# Cineon Scan Module Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add an optional, self-contained OKLab scanner-response stage to the existing DCTL after density and before output re-encoding.

**Architecture:** `CineonScan()` performs one OKLab round trip on post-density linear sRGB. Smooth tonal masks modify only lightness, radial chroma scaling preserves hue, and neutral/skin masks gate coolness and chroma changes; a zero global strength is exact pass-through.

**Tech Stack:** DaVinci DCTL, Python standard-library assertions, Clang OpenCL syntax validation, Resolve scripting API.

## Global Constraints

- Modify the existing `OKLabGrade-NoDRT.dctl`; do not create another DCTL.
- Run after `out=d_nrgb` and before gain, camera-gamut conversion, and transfer encoding.
- Add no grain, halation, bloom, film/print stock response, orange mask, or lens effects.
- All scalar controls range `0.0–1.0` with step `0.001`.
- `enable_cineon_scan=0` preserves existing output; `scan_strength=0.50` makes enabling immediately visible.
- Reuse `l2o`, `o2l`, `smootherstep`, and existing skin-hue conventions.
- This directory is not a Git repository; commit steps do not apply.

---

### Task 1: CineonScan Controls, Math, And Placement

**Files:**
- Modify: `test_tone_controls.py`
- Modify: `OKLabGrade-NoDRT.dctl:155-264,379-384`

**Interfaces:**
- Consumes: post-density linear-sRGB `out`, `l2o(float3)`, `o2l(float3)`, and `smootherstep(float,float,float)`.
- Produces: `CineonScan(float3,float,float,float,float,float,float) -> float3` and seven DCTL controls.

- [ ] **Step 1: Add failing source and numerical assertions**

Append this test code to `test_tone_controls.py`:

```python
import math


for declaration in (
    "DEFINE_UI_PARAMS(enable_cineon_scan, [SCAN] Enable Cineon Scan, DCTLUI_CHECK_BOX, 0)",
    "DEFINE_UI_PARAMS(scan_strength, Scan Strength, DCTLUI_SLIDER_FLOAT, 0.50, 0.0, 1.0, 0.001)",
    "DEFINE_UI_PARAMS(shoulder_strength, Shoulder Strength, DCTLUI_SLIDER_FLOAT, 0.50, 0.0, 1.0, 0.001)",
    "DEFINE_UI_PARAMS(highlight_compression, Highlight Compression, DCTLUI_SLIDER_FLOAT, 0.50, 0.0, 1.0, 0.001)",
    "DEFINE_UI_PARAMS(highlight_desaturation, Highlight Desaturation, DCTLUI_SLIDER_FLOAT, 0.35, 0.0, 1.0, 0.001)",
    "DEFINE_UI_PARAMS(shadow_lift, Shadow Lift, DCTLUI_SLIDER_FLOAT, 0.15, 0.0, 1.0, 0.001)",
    "DEFINE_UI_PARAMS(shadow_coolness, Shadow Coolness, DCTLUI_SLIDER_FLOAT, 0.10, 0.0, 1.0, 0.001)",
):
    assert declaration in source

signature = "__DEVICE__ float3 CineonScan(float3 rgb,float strength,float shoulder,float chroma_compression,float highlight_desat,float black_lift,float shadow_cool)"
assert signature in source
call = "out=CineonScan(out,scan_strength,shoulder_strength,highlight_compression,highlight_desaturation,shadow_lift,shadow_coolness);"
assert call in source
assert source.index("out=d_nrgb;") < source.index(call) < source.index("out.x*=gain")


def cineon_scan(lab, strength, shoulder, compression, desaturation, lift, coolness):
    original = lab
    lightness, a_axis, b_axis = lab
    highlight_mask = smootherstep(0.55, 1.10, lightness)
    highlight_excess = max(lightness - 0.60, 0.0)
    rolled = 0.60 + highlight_excess / (1.0 + 2.5 * highlight_excess)
    lightness += (rolled - lightness) * shoulder * highlight_mask

    chroma = math.hypot(a_axis, b_axis)
    neutral_mask = smootherstep(0.015, 0.080, chroma)
    skin_alignment = (a_axis * 0.4815 + b_axis * 0.8764) / (chroma + 1e-6)
    skin_protection = smootherstep(0.70, 0.95, skin_alignment) * neutral_mask
    chroma_permission = 1.0 - skin_protection

    chroma_excess = max(chroma - 0.16, 0.0)
    compressed_chroma = chroma - chroma_excess + chroma_excess / (
        1.0 + 6.0 * compression * highlight_mask * chroma_excess
    )
    highlight_scale = (compressed_chroma / chroma if chroma > 1e-6 else 1.0)
    highlight_scale *= 1.0 - 0.45 * desaturation * highlight_mask
    chroma_scale = 1.0 + (highlight_scale - 1.0) * chroma_permission

    shadow_mask = 1.0 - smootherstep(0.08, 0.50, original[0])
    chroma_scale *= 1.0 - 0.08 * shadow_mask * neutral_mask * chroma_permission
    a_axis *= chroma_scale
    b_axis *= chroma_scale
    lightness += 0.04 * lift * shadow_mask
    b_axis -= 0.012 * coolness * shadow_mask * neutral_mask * chroma_permission

    processed = (lightness, a_axis, b_axis)
    return tuple(before + (after - before) * strength for before, after in zip(original, processed))


sample = (0.72, 0.12, 0.08)
assert cineon_scan(sample, 0.0, 0.5, 0.5, 0.35, 0.15, 0.10) == sample

shoulder_values = [
    cineon_scan((value, 0.0, 0.0), 1.0, 1.0, 0.5, 0.35, 0.15, 0.10)[0]
    for value in (0.55, 0.75, 1.0, 1.5, 2.0)
]
assert all(left < right for left, right in zip(shoulder_values, shoulder_values[1:]))
assert shoulder_values[-1] < 2.0
assert shoulder_values[-1] != shoulder_values[-2]

neutral = cineon_scan((0.2, 0.0, 0.0), 1.0, 0.5, 0.5, 0.35, 1.0, 1.0)
assert neutral[0] > 0.2
assert neutral[1:] == (0.0, 0.0)

colored_shadow = (0.25, -0.08, 0.04)
scanned_shadow = cineon_scan(colored_shadow, 1.0, 0.5, 0.5, 0.35, 0.15, 1.0)
assert math.hypot(scanned_shadow[1], scanned_shadow[2]) <= math.hypot(colored_shadow[1], colored_shadow[2])
assert scanned_shadow[2] < colored_shadow[2]

highlight = (1.0, -0.12, 0.16)
scanned_highlight = cineon_scan(highlight, 1.0, 1.0, 1.0, 1.0, 0.15, 0.0)
assert math.hypot(scanned_highlight[1], scanned_highlight[2]) < math.hypot(highlight[1], highlight[2])
assert abs(math.atan2(scanned_highlight[2], scanned_highlight[1]) - math.atan2(highlight[2], highlight[1])) < 1e-7

skin = (0.35, 0.06 * 0.4815, 0.06 * 0.8764)
skin_scanned = cineon_scan(skin, 1.0, 0.5, 1.0, 1.0, 0.15, 1.0)
non_skin = (0.35, -0.06, 0.0)
non_skin_scanned = cineon_scan(non_skin, 1.0, 0.5, 1.0, 1.0, 0.15, 1.0)
assert abs(skin_scanned[2] - skin[2]) < abs(non_skin_scanned[2] - non_skin[2])
```

- [ ] **Step 2: Run the test and verify RED**

Run: `python3 test_tone_controls.py`

Expected: `AssertionError` on the first missing Cineon control declaration.

- [ ] **Step 3: Add the seven controls**

Insert after the density controls:

```c
// --- CINEON SCAN — post-negative scanner response ---
DEFINE_UI_PARAMS(enable_cineon_scan, [SCAN] Enable Cineon Scan, DCTLUI_CHECK_BOX, 0)
DEFINE_UI_PARAMS(scan_strength, Scan Strength, DCTLUI_SLIDER_FLOAT, 0.50, 0.0, 1.0, 0.001)
DEFINE_UI_PARAMS(shoulder_strength, Shoulder Strength, DCTLUI_SLIDER_FLOAT, 0.50, 0.0, 1.0, 0.001)
DEFINE_UI_PARAMS(highlight_compression, Highlight Compression, DCTLUI_SLIDER_FLOAT, 0.50, 0.0, 1.0, 0.001)
DEFINE_UI_PARAMS(highlight_desaturation, Highlight Desaturation, DCTLUI_SLIDER_FLOAT, 0.35, 0.0, 1.0, 0.001)
DEFINE_UI_PARAMS(shadow_lift, Shadow Lift, DCTLUI_SLIDER_FLOAT, 0.15, 0.0, 1.0, 0.001)
DEFINE_UI_PARAMS(shadow_coolness, Shadow Coolness, DCTLUI_SLIDER_FLOAT, 0.10, 0.0, 1.0, 0.001)
```

- [ ] **Step 4: Add the self-contained module**

Insert after `mod2()` so all reused helpers are already defined:

```c
__DEVICE__ float3 CineonScan(float3 rgb,float strength,float shoulder,float chroma_compression,float highlight_desat,float black_lift,float shadow_cool){
  float3 src=l2o(rgb),lab=src;
  float L=lab.x,hi=smootherstep(0.55f,1.10f,L),hex=_fmaxf(L-0.60f,0.0f);
  float rolled=0.60f+hex/(1.0f+2.5f*hex);
  lab.x=L+(rolled-L)*shoulder*hi;

  float c=_sqrtf(lab.y*lab.y+lab.z*lab.z);
  float neutral=smootherstep(0.015f,0.080f,c);
  float skin_align=(lab.y*0.4815f+lab.z*0.8764f)/(c+1e-6f);
  float skin=smootherstep(0.70f,0.95f,skin_align)*neutral;
  float permission=1.0f-skin;
  float cex=_fmaxf(c-0.16f,0.0f);
  float csoft=c-cex+cex/(1.0f+6.0f*chroma_compression*hi*cex);
  float hi_scale=(c>1e-6f?csoft/c:1.0f)*(1.0f-0.45f*highlight_desat*hi);
  float cscale=1.0f+(hi_scale-1.0f)*permission;

  float sh=1.0f-smootherstep(0.08f,0.50f,L);
  cscale*=1.0f-0.08f*sh*neutral*permission;
  lab.y*=cscale;lab.z*=cscale;
  lab.x+=0.04f*black_lift*sh;
  lab.z-=0.012f*shadow_cool*sh*neutral*permission;

  lab=src+(lab-src)*strength;
  return o2l(lab);
}
```

- [ ] **Step 5: Insert the guarded call after density**

Immediately after `out=d_nrgb;`, add:

```c
  // Scanner response after negative density, before output gain/re-encode.
  if(enable_cineon_scan&&scan_strength>0.0f)
    out=CineonScan(out,scan_strength,shoulder_strength,highlight_compression,highlight_desaturation,shadow_lift,shadow_coolness);
```

- [ ] **Step 6: Run the numerical/source test and verify GREEN**

Run: `python3 test_tone_controls.py`

Expected: exit code `0` with no output.

- [ ] **Step 7: Run the OpenCL syntax check**

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
  -D'_fmod=fmod' -D'_atan2f=atan2' OKLabGrade-NoDRT.dctl
```

Expected: exit code `0` with no output.

- [ ] **Step 8: Refresh Resolve LUTs**

Call `project_settings(action="refresh_luts")`.

Expected: `{ "success": true }`. Reselect the DCTL once in an existing node to instantiate the new controls.
