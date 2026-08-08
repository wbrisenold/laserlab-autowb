// LaserLabParams.h — shared per-render parameter block passed from the OFX
// plugin to the GPU kernels (Metal + OpenCL) and the CPU reference.
//
// LaserLab is a faithful port of the DaVinci Resolve DCTL
// "LaserLab.dctl". The DCTL is the single source of truth; this struct and
// the three implementations (laserlab_core.h CPU, LaserLabMetalKernel,
// LaserLabOpenCLKernel) are line-by-line ports of it, verified by a CPU/GPU
// parity test (~2e-5). Keep every constant and formula textually parallel.
//
// EVERY member is 4 bytes, so the layout is identical in C++, Metal and
// OpenCL with no padding. A field added here MUST also be added to the struct
// declarations inside the Metal and OpenCL kernel sources, and the layout
// parity check in test_laserlab.cpp must be updated.
//
// Defaults mirror the DCTL's DEFINE_UI_PARAMS values EXACTLY — several are
// intentionally NON-IDENTITY (the DCTL bakes in a look): Contrast 1.06,
// Shadows 0.05, Highlights 0.05, Desat Highlights 0.05, Sat Amount 1.10,
// Sat Compression 0.15, Sub Sat 0.06. These are not neutral.
//
// MIT License.

#ifndef LASERLAB_PARAMS_H
#define LASERLAB_PARAMS_H

// inputGamut values (match DCTL Definer in_gamut, gamut index used in g2x/x2g)
#define LASERLAB_GAMUT_REC709   0
#define LASERLAB_GAMUT_AWG4     1   // working container
#define LASERLAB_GAMUT_AWG3     2
#define LASERLAB_GAMUT_SGAMUT3  3
#define LASERLAB_GAMUT_VGAMUT   4
#define LASERLAB_GAMUT_REDWG    5
#define LASERLAB_GAMUT_BMWG     6
#define LASERLAB_GAMUT_CINEMA   7
#define LASERLAB_GAMUT_ACESCG   8   // D60 white
#define LASERLAB_GAMUT_DGAMUT   9
#define LASERLAB_GAMUT_REC2020 10

// inputTransfer values (matches DCTL in_transfer, index used in tfd/tfe)
#define LASERLAB_TF_REC709   0
#define LASERLAB_TF_LOGC4    1
#define LASERLAB_TF_LOGC3    2
#define LASERLAB_TF_SLOG3    3
#define LASERLAB_TF_VLOG     4
#define LASERLAB_TF_LOG3G10  5
#define LASERLAB_TF_DWG      6
#define LASERLAB_TF_CINEMA   7   // linear
#define LASERLAB_TF_CLOG2    8
#define LASERLAB_TF_CLOG3    9
#define LASERLAB_TF_DLOG    10
#define LASERLAB_TF_LINEAR  11
#define LASERLAB_TF_ACESCG  12
#define LASERLAB_TF_APPLELOG 13
#define LASERLAB_TF_ACESCCT 14

typedef struct LaserLabParams
{
    // ---- 📥 Input ----
    int   inputGamut;                 // LASERLAB_GAMUT_*
    int   inputTransfer;              // LASERLAB_TF_*

    // ---- ⚖️ White Balance ----
    float temp;
    float tint;

    // ---- 🎞 Printer Lights ----
    float plMaster;
    float plRed;
    float plGreen;
    float plBlue;

    // ---- 🎚 Primaries ----
    float lift;
    float gamma;
    float gain;
    float contrast;

    // ---- 🌗 Tone ----
    float shadows;
    float highlights;
    float desatHigh;
    float midDetail;

    // ---- ☀️ Exposure ----
    float expGlobal;
    float expFlash;

    // ---- 🌈 Color ----
    float satAmount;
    float satCompression;
    float hueDeg;                    // degrees

    // ---- 🧑 Skin ----
    float skinHueDeg;
    float skinSat;
    float skinVal;
    float skinEvenness;
    float skinRadius;              // spatial skin averaging radius (pixels)

    // ---- 🧪 Density ----
    float globalSat;
    float limSat;
    float limDens;
    float rDens;
    float yDens;
    float gDens;
    float cDens;
    float bDens;
    float mDens;

    // ---- ☀️ Exposure (redesign) ----
    float expoKnee;                // scene-linear shoulder start
    float expoWidth;               // roll-off width
    float expoShoulder;            // blend into rational compression
    float expoGamma;               // signed post-exposure midtone power

    // ---- ⚖️ Auto WB ----
    int   autoWb;                  // 0=off, 1=on
    float autoWbStrength;          // 0-1 blend
    float autoWbLmsGainX;         // held CAT16 LMS gain (R)
    float autoWbLmsGainY;         // held CAT16 LMS gain (G)
    float autoWbLmsGainZ;         // held CAT16 LMS gain (B)

    // ---- meta ----
    int   _pad0;
    int   _pad1;
    int   _pad2;
} LaserLabParams;

#endif // LASERCLAB_PARAMS_H