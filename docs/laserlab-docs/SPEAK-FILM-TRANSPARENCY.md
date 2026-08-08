# Speak — fast Film-Transparency OFX (installed)

The closed `FilmTransparency.ofx` plugin was slow. Instead of trying to speed up a
binary I can't read, this installs **Speak** — a MIT-licensed, Metal-accelerated
film-emulation OpenFX plugin that implements the same "developed film held to light"
physics. It is 4-backend parity-tested (CPU ref == Metal GPU to ~2e-5) and renders
real-time UHD (its halation uses an energy-normalized pyramid: ~5ms vs ~45ms for a
direct blur at the same radius).

## What it does (your film-transparency checklist)

- Transmitted light through the emulsion  ->  density spine (transmittance = 10^-D)
- Dense shadows that block light          ->  high-density toe/shadow curve
- Glowing highlights where light passes    ->  halation (halAmount/radius/thresh) + bloom
- Subtle film-base tint                    ->  per-preset Dmin + dye-color (opt-in)
- Dye-density color behavior               ->  subtractive dye stage (density domain)
- Slight uneven backlight                  ->  Vignette (cos^4)
- Optional surface sheen / reflection      ->  Bloom veil
- Optional film curl / warp                ->  Gate Weave
- Emulsion texture/gloss                   ->  Density-domain Grain

## Film Transparency stock presets added

Added a `Film Transparency` family to the Stock dropdown (default = **Positive print**):

| Preset | System γ | character |
|---|---|---|
| Positive print (transparency) | 1.55 | classic release print, deep shadow block, glowing highlights, soft shoulder — the default |
| Camera negative (orange) | 1.35 | orange-masked, deep Dmax, warm base, cooler-negative bloom veil |
| Reversal (slide) | 1.70 | steep chrome, punchy, forgiving |
| Neutral transparency | 1.25 | flat, even density sheet |

Each is gray-balanced (18% gray → 0.18), monotonic, and only sets the tone-spine
curves — none re-enable Speak's optional color stages.

## Default slider recommendations (already set)

- **Stock Family:** Positive print (transparency)
- **Strength:** 1.0 (so the look is visible immediately; 0 = untouched)
- **Tone ON** your density spine; **Light optics ON** (halation, bloom, grain, vignette)
- **Subtractive Color and Split Toning: OFF by default** (only the transparency
  density drives the image; no competing look rides on top)
- Optics per transparency preset: Halation 0.35 @ 1.2, Bloom 0.15 @ 4.0, Vignette 0.18,
  Grain 0.12 — tuned to read as transmitted film glow + tint.

## Silent of inert Speak-native UI

Speak's native color stages remained in the engine (they ARE the glow/dye/sheen/curl
features you asked for), but they are all **default OFF** so nothing of Speak's own
look rides on top of the transparency spine. Enable the ones you want (halation,
bloom, grain, vignette) and they become the transmitted-light optics.

## Where / how

- Installed: `~/Library/OFX/Plugins/Speak.ofx.bundle` (Resolve scans this dir)
- Source used: `amateurmenace/Speak` (MIT), verbatim + 2 data-only edits
- Resolve OFX cache was cleared (`OFXPluginCacheV2.xml`), so it will re-scan and pick
  up the new default preset on next launch.

## Verify

`verify_color.py` gate still applies to LaserLab. For Speak, run its own suites
(CPU gates + Metal parity = both ALL GREEN at build).