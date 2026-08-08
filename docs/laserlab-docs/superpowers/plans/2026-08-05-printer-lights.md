# LaserLab Printer Lights Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add traditional 25-centered Master/R/G/B printer lights to LaserLab without changing its neutral image.

**Architecture:** Four UI sliders feed one compact signed linear-RGB helper. The helper converts combined master/channel point offsets to exact `0.025` log10 exposure gains and runs in linear Rec.2020 immediately after CAT16 white balance.

**Tech Stack:** DaVinci Resolve DCTL/OpenCL C, Python 3 assert-based regressions, Clang OpenCL syntax checking.

## Global Constraints

- Controls are `pl_master`, `pl_red`, `pl_green`, and `pl_blue`.
- Every control ranges from `0` to `50`, defaults to `25`, and steps by `1` point.
- One printer point equals `0.025` log10 exposure.
- Master adds equally to each channel's point offset.
- Apply printer lights after CAT16 white balance and before OKLab conversion.
- Preserve negative channel signs through multiplication.
- Add no dependency, print-stock model, or spectral model.
- Keep `backup/LaserLab-LKG.dctl` byte-identical to the verified live source.
- This directory is not a Git repository, so commit steps are omitted.

---

### Task 1: Add And Verify Printer Lights

**Files:**
- Modify: `LaserLab.dctl:130-175,176-215,340-355`
- Modify: `test_tone_controls.py`
- Modify: `HANDOFF-LASERLAB.md`
- Replace: `backup/LaserLab-LKG.dctl`

**Interfaces:**
- Produces: `printer_lights(float3 rgb,float master,float red,float green,float blue) -> float3`.
- Consumes: linear Rec.2020 `work` immediately after `x2g(10,xyz)`.

- [ ] **Step 1: Add failing source and numerical tests**

```python
for declaration in (
    "DEFINE_UI_PARAMS(pl_master, [PL] Master, DCTLUI_SLIDER_FLOAT, 25.0, 0.0, 50.0, 1.0)",
    "DEFINE_UI_PARAMS(pl_red, Red, DCTLUI_SLIDER_FLOAT, 25.0, 0.0, 50.0, 1.0)",
    "DEFINE_UI_PARAMS(pl_green, Green, DCTLUI_SLIDER_FLOAT, 25.0, 0.0, 50.0, 1.0)",
    "DEFINE_UI_PARAMS(pl_blue, Blue, DCTLUI_SLIDER_FLOAT, 25.0, 0.0, 50.0, 1.0)",
):
    assert declaration in source

assert "__DEVICE__ float3 printer_lights(" in source
printer_call = "work=printer_lights(work,pl_master,pl_red,pl_green,pl_blue);"
assert printer_call in source
assert source.index("float3 work=x2g(10,xyz);") < source.index(printer_call) < source.index("float3 lab=work_to_oklab(work);")


def printer_lights(rgb, master, red, green, blue):
    gains = [10 ** (((master - 25.0) + (channel - 25.0)) * 0.025) for channel in (red, green, blue)]
    return tuple(value * gain for value, gain in zip(rgb, gains))


sample = (0.18, -0.05, 1.5)
assert printer_lights(sample, 25, 25, 25, 25) == sample
one_point = printer_lights((1.0, 1.0, 1.0), 26, 25, 25, 25)
assert all(abs(value - 10**0.025) < 1e-12 for value in one_point)
one_stop = printer_lights((1.0, 1.0, 1.0), 25, 25 + math.log10(2) / 0.025, 25, 25)
assert abs(one_stop[0] - 2.0) < 1e-12
assert one_stop[1:] == (1.0, 1.0)
assert printer_lights(sample, 30, 20, 20, 20) == sample
assert printer_lights(sample, 26, 25, 25, 25)[1] < 0.0
```

- [ ] **Step 2: Run the regression script and verify it fails**

Run: `python3 test_tone_controls.py`

Expected: `AssertionError` on the first missing printer-light declaration.

- [ ] **Step 3: Add the four UI controls after Temp/Tint**

```c
// ---- PRINTER LIGHTS ----
DEFINE_UI_PARAMS(pl_master, [PL] Master, DCTLUI_SLIDER_FLOAT, 25.0, 0.0, 50.0, 1.0)
DEFINE_UI_PARAMS(pl_red, Red, DCTLUI_SLIDER_FLOAT, 25.0, 0.0, 50.0, 1.0)
DEFINE_UI_PARAMS(pl_green, Green, DCTLUI_SLIDER_FLOAT, 25.0, 0.0, 50.0, 1.0)
DEFINE_UI_PARAMS(pl_blue, Blue, DCTLUI_SLIDER_FLOAT, 25.0, 0.0, 50.0, 1.0)
```

- [ ] **Step 4: Add the signed linear printer-light helper**

```c
__DEVICE__ float3 printer_lights(float3 rgb,float master,float red,float green,float blue){
  float m=master-25.0f;
  float rg=_powf(10.0f,(m+red-25.0f)*0.025f);
  float gg=_powf(10.0f,(m+green-25.0f)*0.025f);
  float bg=_powf(10.0f,(m+blue-25.0f)*0.025f);
  return make_float3(rgb.x*rg,rgb.y*gg,rgb.z*bg);
}
```

- [ ] **Step 5: Insert the helper at the Rec.2020 primary boundary**

```c
float3 work=x2g(10,xyz);
work=printer_lights(work,pl_master,pl_red,pl_green,pl_blue);

// Grade in OKLab
float3 lab=work_to_oklab(work);
```

- [ ] **Step 6: Run the regression script**

Run: `python3 test_tone_controls.py`

Expected: exit code `0` with no output.

- [ ] **Step 7: Update the handoff**

Change the control count from `33` to `37`, add `Printer Lights | Master, Red, Green, Blue`, and document the 25-centered `0.025` log10 point scale and placement after white balance.

- [ ] **Step 8: Compile the DCTL with the existing OpenCL harness**

Run the documented `clang -x cl -cl-std=CL1.2 -fsyntax-only -Werror` command against `LaserLab.dctl`.

Expected: exit code `0` with no diagnostics.

- [ ] **Step 9: Synchronize and verify the backup**

Copy `LaserLab.dctl` to `backup/LaserLab-LKG.dctl`, then run:

```bash
cmp -s LaserLab.dctl backup/LaserLab-LKG.dctl
```

Expected: exit code `0`.

- [ ] **Step 10: Refresh Resolve LUTs**

Call Resolve's LUT refresh. Expected: success. If both Resolve bridges remain unavailable, report manual LUT refresh as the only external verification gap.
