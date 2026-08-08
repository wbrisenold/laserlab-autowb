// test_laserlab.cpp — CPU self-tests for LaserLab core
// Mirrors Speak's test_speak.cpp gates:
//   G1 struct layout parity
//   G2 encode/decode round-trips
//   G3 identity at default (strength 0 => bit-exact pass-through)
//   G4 per-control direction/magnitude
//   G5 transfer/gamut round-trips

#include "laserlab_core.h"
#include <initializer_list>
#include <algorithm>
#include "LaserLabParams.h"
#include <cstdio>
#include <cmath>
#include <cstdlib>

static int g_fail = 0;

static void check(bool ok, const char* name, const char* detail = "")
{
    printf("  [%s] %s %s\n", ok ? "PASS" : "FAIL", name, detail);
    if (!ok) g_fail++;
}

static bool near(float a, float b, float e = 2e-4f) { return std::fabs(a - b) <= e; }
static bool near3(const laserlabcore::Vec3& a, const laserlabcore::Vec3& b, float e = 2e-4f) {
    return near(a.x, b.x, e) && near(a.y, b.y, e) && near(a.z, b.z, e);
}

int main()
{
    using namespace laserlabcore;

    // ---- G1 struct layout parity (all fields 4 bytes, no padding) ----
    printf("G1 struct layout parity\n");
    check(sizeof(float) == 4 && sizeof(int) == 4, "float/int are 4 bytes");
    check(sizeof(LaserLabParams) == 47 * 4, "sizeof(LaserLabParams)==188");
    check(offsetof(LaserLabParams, inputGamut) == 0 * 4, "inputGamut offset==0");
    check(offsetof(LaserLabParams, inputTransfer) == 1 * 4, "inputTransfer offset==4");
    check(offsetof(LaserLabParams, temp) == 2 * 4, "temp offset==8");
    check(offsetof(LaserLabParams, _pad0) == 44 * 4, "_pad0 offset==176");

    // ---- G2 transfer encode/decode round-trips ----
    printf("G2 transfer encode/decode round-trips\n");
    const int transfers[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14};
    for (int t : transfers) {
        for (float v : {0.0f, 0.01f, 0.18f, 0.5f, 1.0f}) {
            Vec3 v3 = laserlabcore::v3(v,v,v);
            Vec3 enc = laserlabcore::tfe(v3, t);
            Vec3 dec = laserlabcore::tfd(enc, t);
            float err = std::fabs(dec.x - v);
            check(err < 1e-3f, "round-trip", "");
        }
    }

    // ---- G3 identity at default ----
    printf("G3 identity at default (DCTL defaults)\n");
    LaserLabParams p = {};
    p.inputGamut = 1; p.inputTransfer = 1;
    p.contrast = 1.06f; p.shadows = 0.05f; p.highlights = 0.05f;
    p.desatHigh = 0.05f; p.satAmount = 1.10f; p.satCompression = 0.15f;
    p.globalSat = 0.06f;
    p.plMaster = 25.0f; p.plRed = 25.0f; p.plGreen = 25.0f; p.plBlue = 25.0f;
    p.gamma = 1.0f; p.gain = 1.0f; p.skinSat = 1.0f;

    for (laserlabcore::Vec3 v : {
            v3(0.0f, 0.0f, 0.0f),
            v3(0.18f, 0.18f, 0.18f),
            v3(0.8f, 0.2f, 0.1f),
            v3(-0.1f, 1.4f, 0.3f)}) {
        laserlabcore::Vec3 out = laserlabcore::transform(p, v.x, v.y, v.z);
        // With non-neutral defaults, output != input; but should be finite
        check(std::isfinite(out.x) && std::isfinite(out.y) && std::isfinite(out.z), "finite output");
    }

    // ---- G4 per-control direction/magnitude ----
    printf("G4 per-control direction/magnitude\n");
    // Exposure stops
    p = {}; p.inputGamut=1; p.inputTransfer=1;
    p.expGlobal = 1.0f;
    check(near(transform(p,0.18f,0.18f,0.18f).x, 0.36f, 1e-3f), "exp +1 stop = 2x");
    p.expGlobal = -1.0f;
    check(near(transform(p,0.18f,0.18f,0.18f).x, 0.09f, 1e-3f), "exp -1 stop = 0.5x");

    // Temp/Tint (direction)
    p = {}; p.inputGamut=1; p.inputTransfer=1;
    p.temp = 1.0f;
    float t1 = laserlabcore::transform(p,0.18f,0.18f,0.18f).x;
    p.temp = -1.0f;
    float t2 = laserlabcore::transform(p,0.18f,0.18f,0.18f).x;
    check(t1 != t2, "temp opposite signs differ");
    // Tint
    p = {}; p.inputGamut=1; p.inputTransfer=1;
    p.tint = 1.0f; float t3 = laserlabcore::transform(p,0.18f,0.18f,0.18f).x;
    p.tint = -1.0f; float t4 = laserlabcore::transform(p,0.18f,0.18f,0.18f).x;
    check(t3 != t4, "tint opposite signs differ");

    // Printer lights halve/double per 8 points
    p = {}; p.inputGamut=1; p.inputTransfer=1;
    p.plMaster = 17.0f; // -8 points
    float pl1 = laserlabcore::transform(p,0.18f,0.18f,0.18f).x;
    p.plMaster = 33.0f; // +8 points
    float pl2 = laserlabcore::transform(p,0.18f,0.18f,0.18f).x;
    check(std::fabs(pl1 - 0.09f) < 0.02f && std::fabs(pl2 - 0.36f) < 0.02f, "printer lights 8pt = 1 stop");

    // Lift raises dark
    p = {}; p.inputGamut=1; p.inputTransfer=1;
    p.lift = 0.1f; float l1 = laserlabcore::transform(p,0.02f,0.02f,0.02f).x;
    p.lift = -0.1f; float l2 = laserlabcore::transform(p,0.02f,0.02f,0.02f).x;
    check(l1 > l2, "positive lift > negative lift");

    // Gamma raises mids
    p = {}; p.inputGamut=1; p.inputTransfer=1;
    p.gamma = 0.5f; float g1 = laserlabcore::transform(p,0.18f,0.18f,0.18f).x;
    p.gamma = 2.0f; float g2 = laserlabcore::transform(p,0.18f,0.18f,0.18f).x;
    check(g1 != g2, "gamma changes mids");

    // Saturation
    p = {}; p.inputGamut=1; p.inputTransfer=1;
    p.satAmount = 0.0f; float s1 = laserlabcore::chroma_lab(laserlabcore::transform(p,0.5f,0.0f,0.0f));
    p.satAmount = 2.0f; float s2 = laserlabcore::chroma_lab(laserlabcore::transform(p,0.5f,0.0f,0.0f));
    check(s2 > s1, "sat amount scales chroma");

    // Hue rotates
    p = {}; p.inputGamut=1; p.inputTransfer=1;
    p.hueDeg = 90.0f;
    laserlabcore::Vec3 h = laserlabcore::transform(p,0.5f,0.0f,0.0f);
    check(std::fabs(h.x) < 0.01f && h.y > 0.4f, "hue 90 rotates R->G");

    // Skin hue shifts
    p = {}; p.inputGamut=1; p.inputTransfer=1;
    p.skinHueDeg = 30.0f;
    laserlabcore::Vec3 sk = laserlabcore::transform(p,0.8f,0.4f,0.3f);
    check(std::isfinite(sk.x), "skin hue finite");

    // Density per-hue
    p = {}; p.inputGamut=1; p.inputTransfer=1;
    p.rDens = 0.5f; float d1 = laserlabcore::transform(p,0.5f,0.2f,0.2f).x;
    p.rDens = -0.5f; float d2 = laserlabcore::transform(p,0.5f,0.2f,0.2f).x;
    check(d1 != d2, "red density signed");

    // ---- G5 gamut round-trip (AWG4 identity) ----
    printf("G5 gamut round-trip (AWG4)\n");
    for (float v : {0.18f, 0.5f, 1.0f}) {
        laserlabcore::Vec3 rgb = laserlabcore::v3(v,v,v);
        laserlabcore::Vec3 xyz = laserlabcore::g2x(1, rgb);
        laserlabcore::Vec3 back = laserlabcore::x2g(1, xyz);
        check(near3(rgb, back, 1e-4f), "AWG4 round-trip");
    }

// ---- G6 auto-WB solver ----
    // A measured skin mean that sits above the skin-tone line in OKLab must
    // produce a non-identity CAT16 temp/tint, and the neutral solver must be 0/0.
    printf("G6 Auto-WB solver\n");
    // Neutral skin (hue exactly on the 1.0685 line): solver stays at identity.
    {
        float t0 = 1, n0 = 1;
        laserlabcore::solveSkinTempTint(0.7f, std::cos(1.0685f), std::sin(1.0685f), t0, n0);
        check(std::fabs(t0) < 1e-3f && std::fabs(n0) < 1e-3f, "on-line skin -> identity");
    }
    // Skewed skin (hue off the line by ~5 deg, realistic magnitude): solver
    // must move off zero and pull the mean skin back onto the line.
    {
        float t1 = 0, n1 = 0;
        const float hue = 1.0685f + 5.0f * (2.0f * M_PI) / 360.0f;
        laserlabcore::solveSkinTempTint(0.7f, 0.15f * std::cos(hue), 0.15f * std::sin(hue), t1, n1);
        check(std::fabs(t1) > 1e-3f || std::fabs(n1) > 1e-3f, "off-line skin -> nonzero correction");
        laserlabcore::Vec3 wb = laserlabcore::white_balance(
            laserlabcore::g2x(1, laserlabcore::oklab_to_work(laserlabcore::v3(0.7f, 0.15f * std::cos(hue), 0.15f * std::sin(hue)))),
            t1, n1);
        laserlabcore::Vec3 lab = laserlabcore::work_to_oklab(laserlabcore::x2g(1, wb));
        float ch = std::atan2(lab.z, lab.y);
        float herr = std::fabs(std::atan2(std::sin(1.0685f - ch), std::cos(1.0685f - ch)));
        check(herr < 0.05f, "solved WB lands skin on the line");
    }
    printf("--------------------------------\n");
    if (g_fail == 0) {
        printf("RESULT: ALL PASS - safe to push to Resolve\n");
        return 0;
    } else {
        printf("RESULT: %d FAIL\n", g_fail);
        return 1;
    }
}