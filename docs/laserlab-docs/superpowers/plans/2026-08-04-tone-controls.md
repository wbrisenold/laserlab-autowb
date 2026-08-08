# Tone Controls Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Make Shadows and Highlights visibly effective, tame Contrast, and add visual markers to major DCTL controls.

**Architecture:** Apply the existing `hdr_recover(float,float,float)` curve once to perceptual OKLab `L`, preserving hue and making the controls visible before conversion back to linear RGB. Keep neutral defaults unchanged and only alter UI labels/ranges requested by the spec.

**Tech Stack:** DaVinci DCTL, OpenCL syntax validation, Python standard library assertions.

## Global Constraints

- Shadows and Highlights default to `0.0` and remain neutral at zero.
- Contrast defaults to `1.0`, ranges from `0.5` to `1.5`, and steps by `0.001`.
- Add markers only to Temp, Contrast, Shadows, Highlights, Expo Global, and Sat Amount.
- Do not change the input/output color-space round trip or unrelated controls.
- This directory is not a Git repository; commit steps are intentionally omitted.

---

### Task 1: Perceptual Tone Controls And UI Labels

**Files:**
- Create: `test_tone_controls.py`
- Modify: `OKLabGrade-NoDRT.dctl:117-140,286-314`

**Interfaces:**
- Consumes: `hdr_recover(float v, float sh, float hl)` and OKLab lightness variable `L`.
- Produces: tone-adjusted `L` assigned to `lab.x`; unchanged neutral output when both controls are zero.

- [ ] **Step 1: Write the failing numeric/source test**

```python
from pathlib import Path

source = Path("OKLabGrade-NoDRT.dctl").read_text()

assert "DEFINE_UI_PARAMS(contrast, [TONE] Contrast, DCTLUI_SLIDER_FLOAT, 1.0, 0.5, 1.5, 0.001)" in source
assert "L=hdr_recover(L,shadows,highlights);lab.x=L;" in source
assert "hdr_recover(out.x,shadows,highlights)" not in source

def smootherstep(edge0, edge1, value):
    value = max(edge0, min(edge1, value))
    value = (value - edge0) / (edge1 - edge0)
    return value * value * value * (value * (value * 6.0 - 15.0) + 10.0)

def recover(value, shadows, highlights):
    shadow_mask = 1.0 - smootherstep(0.0, 0.4, value)
    highlight_mask = smootherstep(0.55, 0.95, value)
    value += shadows * 0.5 * shadow_mask * (0.2 - value)
    return 0.75 + (value - 0.75) / (1.0 + highlights * 3.0 * highlight_mask)

for value in (0.0, 0.18, 0.5, 0.75, 1.0):
    assert abs(recover(value, 0.0, 0.0) - value) < 1e-7
assert recover(0.05, 1.0, 0.0) > 0.05
assert recover(1.0, 0.0, 1.0) < 1.0
```

- [ ] **Step 2: Run the test and verify it fails on the missing design**

Run: `python3 test_tone_controls.py`

Expected: assertion failure because the new Contrast label/range and OKLab `L` application are absent.

- [ ] **Step 3: Implement the minimal DCTL change**

Change the six labels and Contrast range to:

```c
DEFINE_UI_PARAMS(temp, [WB] Temp, DCTLUI_SLIDER_FLOAT, 0.0, -0.3, 0.3, 0.001)
DEFINE_UI_PARAMS(contrast, [TONE] Contrast, DCTLUI_SLIDER_FLOAT, 1.0, 0.5, 1.5, 0.001)
DEFINE_UI_PARAMS(shadows, [SH] Shadows, DCTLUI_SLIDER_FLOAT, 0.0, 0.0, 1.0, 0.001)
DEFINE_UI_PARAMS(highlights, [HI] Highlights, DCTLUI_SLIDER_FLOAT, 0.0, 0.0, 1.0, 0.001)
DEFINE_UI_PARAMS(exp_global, [EXP] Expo Global, DCTLUI_SLIDER_FLOAT, 0.0, -6.0, 6.0, 0.001)
DEFINE_UI_PARAMS(sat_amount, [SAT] Sat Amount, DCTLUI_SLIDER_FLOAT, 1.0, 0.0, 2.0, 0.001)
```

Replace the tonal assignment with:

```c
L+=lift;L=_fmaxf(L,0.0f);L=spowf(L,1.0f/gamma);L=(L-0.5f)*contrast+0.5f;
L=hdr_recover(L,shadows,highlights);lab.x=L;
```

Delete the later per-channel call:

```c
out=make_float3(hdr_recover(out.x,shadows,highlights),hdr_recover(out.y,shadows,highlights),hdr_recover(out.z,shadows,highlights));
```

- [ ] **Step 4: Run the numeric/source test**

Run: `python3 test_tone_controls.py`

Expected: exit code `0` with no output.

- [ ] **Step 5: Run the OpenCL syntax check**

Run:

```bash
clang -x cl -cl-std=CL1.2 -fsyntax-only -Werror \
  -D'__DEVICE__=' \
  -D'DEFINE_UI_PARAMS(name,label,type,def,...)=__constant float name=def;' \
  -D'DCTLUI_COMBO_BOX=0' -D'DCTLUI_SLIDER_FLOAT=0' \
  -D'make_float3(a,b,c)=((float3)(a,b,c))' -D'make_float2(a,b)=((float2)(a,b))' \
  -D'_powf=pow' -D'_fabs=fabs' \
  -D'_fminf(a,b)=((a)<(b)?(a):(b))' -D'_fmaxf(a,b)=((a)>(b)?(a):(b))' \
  -D'_sqrtf=sqrt' -D'_expf=exp' -D'_log10f=log10' -D'_log2f=log2' \
  -D'_copysignf=copysign' -D'_cosf=cos' -D'_sinf=sin' \
  -D'_fmod=fmod' -D'_atan2f=atan2' OKLabGrade-NoDRT.dctl
```

Expected: exit code `0` with no output. If Clang rejects display-label emoji while Resolve accepts the DCTL macro syntax, verify through Resolve's LUT refresh and report that harness limitation explicitly.

- [ ] **Step 6: Refresh Resolve and verify runtime reload requirement**

Call `project_settings(action="refresh_luts")`.

Expected: `{ "success": true }`. Reselect `OKLabGrade-NoDRT` in node 2 once if the existing OFX DCTL instance retains cached code or controls.
