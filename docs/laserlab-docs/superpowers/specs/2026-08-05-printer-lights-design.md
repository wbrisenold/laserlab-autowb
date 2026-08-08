# LaserLab Printer Lights Design

## Goal

Add traditional RGB printer-light timing to LaserLab without changing its default image.

## Controls

Add four sliders:

- `[PL] Master`
- `Red`
- `Green`
- `Blue`

Each control ranges from `0` to `50`, defaults to `25`, and moves in whole printer points.

## Mathematics

One printer point equals `0.025` log10 exposure, approximately `1/12.04` stop. For each channel:

```text
channel_points = (master - 25) + (channel - 25)
channel_gain = 10^(channel_points * 0.025)
output_channel = input_channel * channel_gain
```

Positive point changes increase exposure in that channel. Multiplication preserves the sign of negative scene-linear values. Master contributes equally to all three channels.

## Placement

Apply printer lights in linear Rec.2020 immediately after CAT16 white balance and before conversion to OKLab. This makes printer lights an early primary timing operation; all existing saturation, tone, skin, exposure-roll, density, and scanner processing follows it.

## Defaults And Compatibility

All controls default to `25`, producing exact identity gains of `1.0`. Existing LaserLab defaults and the IMAX Laser-inspired look remain unchanged. No print-stock model, lamp spectral model, new gamut, or dependency is added.

## Verification

- Source checks verify four controls, ranges, defaults, placement, and the `0.025` coefficient.
- Numerical checks verify neutral identity, one-point gain `10^0.025`, approximately 12.04 points per stop, master/channel composition, channel isolation, and sign preservation.
- Run the full Python regression script and OpenCL warning-as-error syntax harness.
- Synchronize the verified source to `backup/LaserLab-LKG.dctl`.
- Refresh Resolve LUTs when its scripting API responds.
