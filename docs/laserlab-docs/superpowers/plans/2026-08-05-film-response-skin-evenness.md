# Film Response And Skin Evenness Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Rename the scanner slider to Film Response and add a recommended-default, hue-only Skin Evenness control.

**Architecture:** Keep `scan_strength` and `CineonScan()` unchanged internally, changing only the UI label and default. Apply Skin Evenness after the existing OKLab skin adjustment by moving selected skin hue angles toward the existing `1.0685` radian anchor while retaining lightness and chroma.

**Tech Stack:** DaVinci DCTL/OpenCL C, Python assertion-based regression checks

## Global Constraints

- Film Response and Skin Evenness both default to `0.25`, range `0.0-1.0`, step `0.001`.
- Skin Evenness changes hue only; it must not change OKLab lightness or chroma.
- Zero Skin Evenness is exact identity; non-skin and near-neutral pixels are unchanged.
- Do not rename internal scanner symbols or add spatial processing.

---

### Task 1: Film Response And Skin Evenness

**Files:**
- Modify: `test_tone_controls.py:117-139,342`
- Modify: `LaserLab.dctl:164-181,388-401`
- Modify: `backup/LaserLab-LKG.dctl`
- Modify: `HANDOFF-LASERLAB.md:34-49,55-67`

**Interfaces:**
- Consumes: existing OKLab skin result in `lab`, existing `smootherstep()`, `mod2()`, and skin anchor `1.0685f`
- Produces: `skin_evenness` DCTL UI parameter and hue-only final skin adjustment

- [ ] **Step 1: Write failing source and numeric checks**

Replace the old scanner declaration assertion and add a Skin Evenness declaration assertion:

```python
assert "DEFINE_UI_PARAMS(skin_evenness, Skin Evenness, DCTLUI_SLIDER_FLOAT, 0.25, 0.0, 1.0, 0.001)" in source
assert "DEFINE_UI_PARAMS(scan_strength, [FINISH] Film Response, DCTLUI_SLIDER_FLOAT, 0.25, 0.0, 1.0, 0.001)" in source
```

Append this numeric model and checks:

```python
def skin_evenness(lab, amount):
    lightness, a_axis, b_axis = lab
    chroma = math.hypot(a_axis, b_axis)
    if amount <= 0.0 or chroma <= 0.0:
        return lab
    anchor = 1.0685
    hue = math.atan2(b_axis, a_axis) % (2.0 * math.pi)
    alignment = math.cos(hue - anchor)
    mask = smootherstep(0.70, 0.95, alignment) * smootherstep(0.015, 0.080, chroma)
    delta = math.atan2(math.sin(anchor - hue), math.cos(anchor - hue))
    even_hue = hue + delta * amount * mask
    return lightness, chroma * math.cos(even_hue), chroma * math.sin(even_hue)


skin_sample = (0.55, 0.10 * math.cos(1.30), 0.10 * math.sin(1.30))
assert skin_evenness(skin_sample, 0.0) == skin_sample
skin_default = skin_evenness(skin_sample, 0.25)
skin_full = skin_evenness(skin_sample, 1.0)
assert abs(math.atan2(skin_full[2], skin_full[1]) - 1.0685) < abs(math.atan2(skin_default[2], skin_default[1]) - 1.0685)
assert skin_default[0] == skin_sample[0]
assert abs(math.hypot(*skin_default[1:]) - math.hypot(*skin_sample[1:])) < 1e-12
assert skin_evenness((0.55, -0.10, 0.0), 1.0) == (0.55, -0.10, 0.0)
assert skin_evenness((0.55, 0.004, 0.003), 1.0) == (0.55, 0.004, 0.003)
```

- [ ] **Step 2: Run tests and verify the new assertions fail**

Run: `python3 test_tone_controls.py`

Expected: `AssertionError` at the missing `skin_evenness` declaration.

- [ ] **Step 3: Add the controls and hue-only correction**

Add the control after `Skin Val` and rename the finishing control:

```c
DEFINE_UI_PARAMS(skin_evenness, Skin Evenness, DCTLUI_SLIDER_FLOAT, 0.25, 0.0, 1.0, 0.001)
DEFINE_UI_PARAMS(scan_strength, [FINISH] Film Response, DCTLUI_SLIDER_FLOAT, 0.25, 0.0, 1.0, 0.001)
```

Immediately after `lab=lab+(lab_skin-lab)*sk_nm;`, add:

```c
  float sk_ec=_sqrtf(lab.y*lab.y+lab.z*lab.z);
  float sk_eh=sk_ec>0.0f?mod2(_atan2f(lab.z,lab.y),2.0f*PI):0.0f;
  float sk_ealign=_cosf(sk_eh-1.0685f);
  float sk_emask=smootherstep(0.70f,0.95f,sk_ealign)*smootherstep(0.015f,0.080f,sk_ec);
  float sk_edelta=_atan2f(_sinf(1.0685f-sk_eh),_cosf(1.0685f-sk_eh));
  float sk_eout=sk_eh+sk_edelta*skin_evenness*sk_emask;
  lab.y=sk_ec*_cosf(sk_eout);lab.z=sk_ec*_sinf(sk_eout);
```

- [ ] **Step 4: Run regression checks**

Run: `python3 test_tone_controls.py`

Expected: exit code `0` with no output.

- [ ] **Step 5: Synchronize the last-known-good copy and documentation**

Copy the verified `LaserLab.dctl` contents exactly to `backup/LaserLab-LKG.dctl`. Update `HANDOFF-LASERLAB.md` to list 38 controls, add Skin Evenness to Skin, rename Scan to Finish / Film Response, and include both `0.25` defaults.

- [ ] **Step 6: Run final verification**

Run: `python3 test_tone_controls.py`

Expected: exit code `0` with no output.

Run: `cmp -s LaserLab.dctl backup/LaserLab-LKG.dctl`

Expected: exit code `0` with no output.

Run the documented OpenCL syntax harness.

Expected: exit code `0` with no diagnostics.

This workspace is not a Git repository, so there is no commit step.
