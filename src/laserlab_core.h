// laserlab_core.h — LaserLab CPU reference (the single source of truth for
// the algorithm). The GPU kernels (LaserLabMetalKernel, LaserLabOpenCLKernel)
// are line-by-line ports of this file; ANY change to the math must be applied
// to all three and verified with test_laserlab_metal (parity ~2e-5).
//
// This is a faithful port of the DaVinci Resolve DCTL "LaserLab.dctl".
// Working container is AWG4 (gamut index 1). Scene-referred: values >1 and
// negatives must survive. Never clamp RGB or OKLab L into [0,1]. Only the
// final NaN/Inf guard is allowed. Defaults mirror the DCTL exactly (several
// are NON-IDENTITY by design).
//
// MIT License.

#ifndef LASERLAB_CORE_H
#define LASERLAB_CORE_H

#include <cmath>
#include <vector>
#include "LaserLabParams.h"

namespace laserlabcore {


// ---------------------------------------------------------------------------
// Matrix helpers (float3 as 3-array). mmul(a,b,c,v) is the DCTL's mmul:
//   out.x = a.x*v.x + b.x*v.y + c.x*v.z
// where each *_c0/_c1/_c2 is a COLUMN of the matrix (colour-science column
// convention, matching the DCTL defines).
// ---------------------------------------------------------------------------
static inline void mmul3(const float a[3], const float b[3], const float c[3],
                         const float v[3], float o[3])
{
    o[0] = a[0]*v[0] + b[0]*v[1] + c[0]*v[2];
    o[1] = a[1]*v[0] + b[1]*v[1] + c[1]*v[2];
    o[2] = a[2]*v[0] + b[2]*v[1] + c[2]*v[2];
}

// ---- constants generated verbatim from LaserLab.dctl ----
// ---- Generated directly from LaserLab.dctl (source of truth) ----
// ---- CAT16I ----
static const float CAT16I_c0[3] = {1.86206786f,0.38752654f,-0.01584150f};
static const float CAT16I_c1[3] = {-1.01125463f,0.62144744f,-0.03412294f};
static const float CAT16I_c2[3] = {0.14918677f,-0.00897398f,1.04996444f};
// ---- CAT16 ----
static const float CAT16_c0[3] = {0.401288f,-0.250268f,-0.002079f};
static const float CAT16_c1[3] = {0.650173f,1.204414f,0.048952f};
static const float CAT16_c2[3] = {-0.051461f,0.045854f,0.953127f};
// ---- G0 ----
static const float G0_c0[3] = {3.2409699419f,-0.9692436363f,0.0556300797f};
static const float G0_c1[3] = {-1.5373831776f,1.8759675015f,-0.2039769589f};
static const float G0_c2[3] = {-0.4986107603f,0.0415550574f,1.0569715142f};
// ---- G10 ----
static const float G10_c0[3] = {1.7166511880f,-0.6666843518f,0.0176398574f};
static const float G10_c1[3] = {-0.3556707838f,1.6164812366f,-0.0427706133f};
static const float G10_c2[3] = {-0.2533662814f,0.0157685458f,0.9421031212f};
// ---- G1 ----
static const float G1_c0[3] = {1.5092154722f,-0.4915454517f,0.0000000000f};
static const float G1_c1[3] = {-0.2505973452f,1.3612455459f,0.0000000000f};
static const float G1_c2[3] = {-0.1688114753f,0.0972829420f,0.9182249512f};
// ---- G2 ----
static const float G2_c0[3] = {1.7890660000f,-0.6398490000f,-0.0415320000f};
static const float G2_c1[3] = {-0.4825340000f,1.3964000000f,0.0823350000f};
static const float G2_c2[3] = {-0.2000760000f,0.1944320000f,0.8788680000f};
// ---- G3 ----
static const float G3_c0[3] = {1.5073998991f,-0.5181517271f,0.0155116982f};
static const float G3_c1[3] = {-0.2458221374f,1.3553912409f,-0.0078727714f};
static const float G3_c2[3] = {-0.1716116808f,0.1258786682f,0.9119163656f};
// ---- G4 ----
static const float G4_c0[3] = {1.5890120000f,-0.5340530000f,0.0111790000f};
static const float G4_c1[3] = {-0.3132040000f,1.3960110000f,0.0031940000f};
static const float G4_c2[3] = {-0.1809650000f,0.1024580000f,0.9055350000f};
// ---- G5 ----
static const float G5_c0[3] = {1.4128066123f,-0.4862031858f,-0.0371387758f};
static const float G5_c1[3] = {-0.1775223662f,1.2906962108f,0.2863757596f};
static const float G5_c2[3] = {-0.1517703764f,0.1574002837f,0.6876796053f};
// ---- G6 ----
static const float G6_c0[3] = {1.8663823404f,-0.6003424925f,0.0024519937f};
static const float G6_c1[3] = {-0.5183973429f,1.3781489625f,0.0863996742f};
static const float G6_c2[3] = {-0.2346098094f,0.1767318303f,0.8369427073f};
// ---- G7 ----
static const float G7_c0[3] = {1.4898182749f,-0.4581665745f,-0.0703496677f};
static const float G7_c1[3] = {-0.2608959022f,1.2616277831f,0.2215576672f};
static const float G7_c2[3] = {-0.1424265218f,0.1596236316f,0.7761816036f};
// ---- G8 ----
static const float G8_c0[3] = {1.6410233797f,-0.6636628587f,0.0117218943f};
static const float G8_c1[3] = {-0.3248032942f,1.6153315917f,-0.0082844420f};
static const float G8_c2[3] = {-0.2364246952f,0.0167563477f,0.9883948585f};
// ---- G9 ----
static const float G9_c0[3] = {1.7257000000f,-0.6025000000f,-0.0156000000f};
static const float G9_c1[3] = {-0.4314000000f,1.3906000000f,0.0905000000f};
static const float G9_c2[3] = {-0.1917000000f,0.1671000000f,0.8489000000f};
// ---- I0 ----
static const float I0_c0[3] = {3.2409699419f,-0.9692436363f,0.0556300797f};
static const float I0_c1[3] = {-1.5373831776f,1.8759675015f,-0.2039769589f};
static const float I0_c2[3] = {-0.4986107603f,0.0415550574f,1.0569715142f};
// ---- L ----
static const float L_c0[3] = {0.4122214708f,0.2119034982f,0.0883024619f};
static const float L_c1[3] = {0.5363325363f,0.6806995451f,0.2817188376f};
static const float L_c2[3] = {0.0514459929f,0.1073969566f,0.6299787005f};
// ---- O ----
static const float O_c0[3] = {0.2104542553f,1.9779984951f,0.0259040371f};
static const float O_c1[3] = {0.793617785f,-2.428592205f,0.7827717662f};
static const float O_c2[3] = {-0.0040720468f,0.4505937099f,-0.808675766f};
// ---- P ----
static const float P_c0[3] = {1.0f,1.0f,1.0f};
static const float P_c1[3] = {0.3963377774f,-0.1055613458f,-0.0894841775f};
static const float P_c2[3] = {0.2158037573f,-0.0638541728f,-1.291485548f};
// ---- Q ----
static const float Q_c0[3] = {4.0767416621f,-1.2684380046f,-0.0041960866f};
static const float Q_c1[3] = {-3.3077115901f,2.6097574011f,-0.7034186145f};
static const float Q_c2[3] = {0.2309699292f,-0.3413193965f,1.7076147014f};
// ---- R0 ----
static const float R0_c0[3] = {0.4123907993f,0.2126390059f,0.0193308187f};
static const float R0_c1[3] = {0.3575843394f,0.7151686788f,0.1191947798f};
static const float R0_c2[3] = {0.1804807884f,0.0721923154f,0.9505321522f};
// ---- R10 ----
static const float R10_c0[3] = {0.6369580483f,0.2627002120f,0.0000000000f};
static const float R10_c1[3] = {0.1446169036f,0.6779980715f,0.0280726930f};
static const float R10_c2[3] = {0.1688809752f,0.0593017165f,1.0609850577f};
// ---- R1 ----
static const float R1_c0[3] = {0.7048583204f,0.2545241764f,0.0f};
static const float R1_c1[3] = {0.1297602952f,0.7814777327f,0.0f};
static const float R1_c2[3] = {0.1158373115f,-0.0360019091f,1.0890577508f};
// ---- R2 ----
static const float R2_c0[3] = {0.638008f,0.291954f,0.002798f};
static const float R2_c1[3] = {0.214704f,0.823841f,-0.067034f};
static const float R2_c2[3] = {0.097744f,-0.115795f,1.153294f};
// ---- R3 ----
static const float R3_c0[3] = {0.7064827132f,0.2709796708f,-0.0096778454f};
static const float R3_c1[3] = {0.1288010498f,0.7866064112f,0.0046000375f};
static const float R3_c2[3] = {0.1151721641f,-0.057586082f,1.0941355587f};
// ---- R4 ----
static const float R4_c0[3] = {0.679644f,0.260686f,-0.00931f};
static const float R4_c1[3] = {0.152211f,0.774894f,-0.004612f};
static const float R4_c2[3] = {0.1186f,-0.03558f,1.10298f};
// ---- R5 ----
static const float R5_c0[3] = {0.735275f,0.286694f,-0.079681f};
static const float R5_c1[3] = {0.068609f,0.842979f,-0.347343f};
static const float R5_c2[3] = {0.146571f,-0.129673f,1.516082f};
// ---- R6 ----
static const float R6_c0[3] = {0.6065303722f,0.2679894071f,-0.029442166f};
static const float R6_c1[3] = {0.2204080953f,0.8327308786f,-0.086610607f};
static const float R6_c2[3] = {0.1234790005f,-0.1007202857f,1.2048607644f};
// ---- R7 ----
static const float R7_c0[3] = {0.7160496466f,0.2612613575f,-0.0096763466f};
static const float R7_c1[3] = {0.1296834779f,0.8696421458f,-0.2364816361f};
static const float R7_c2[3] = {0.1047228026f,-0.1309035033f,1.3352157335f};
// ---- R8 ----
static const float R8_c0[3] = {0.6624541811f,0.2722287168f,-0.0055746495f};
static const float R8_c1[3] = {0.1340042065f,0.6740817658f,0.0040607335f};
static const float R8_c2[3] = {0.156187687f,0.0536895174f,1.0103391003f};
// ---- R9 ----
static const float R9_c0[3] = {0.6482f,0.283f,-0.0183f};
static const float R9_c1[3] = {0.194f,0.8132f,-0.0832f};
static const float R9_c2[3] = {0.1082f,-0.0962f,1.1903f};
// ---- S0 ----
static const float S0_c0[3] = {0.4123907993f,0.2126390059f,0.0193308187f};
static const float S0_c1[3] = {0.3575843394f,0.7151686788f,0.1191947798f};
static const float S0_c2[3] = {0.1804807884f,0.0721923154f,0.9505321522f};
static const float WHITE_D65[3] = {0.9504559271f,1.0f,1.0890577508f};
static const float WHITE_D60[3] = {0.9526460746f,1.0f,1.0088251844f};


// ---- TF scalar constants (from DCTL) ----
static const float LOGC3_CUT = 0.010591f, LOGC3_A = 5.555556f;
static const float LOGC3_B = 0.052272f, LOGC3_C = 0.24719f;
static const float LOGC3_D = 0.385537f, LOGC3_E = 5.367655f, LOGC3_F = 0.092809f;
static const float LOGC4_A = 2231.8263090676883f, LOGC4_B = 0.9071358748778103f;
static const float LOGC4_C = 0.09286412512218964f, LOGC4_S2 = 0.1135972086105891f;
static const float LOGC4_T = -0.01805699611991131f;
static const float APPLE_R0 = -0.05641088f, APPLE_RT = 0.01f;
static const float APPLE_SIGMA = 47.28711236f, APPLE_BETA = 0.00964052f;
static const float APPLE_GAMMA = 0.08550479f, APPLE_DELTA = 0.69336945f;
static const float EXPO_FLASH_SCALE = 0.1f;
static const float PI = 3.141592653589793238f;

// ---------------------------------------------------------------------------
// Vec3 + scalar helpers (float-precision, mirrors DCTL __DEVICE__ funcs).
// ---------------------------------------------------------------------------
struct Vec3 { float x, y, z; };
static inline Vec3 v3(float x, float y, float z) { Vec3 v = {x, y, z}; return v; }
static inline float clampf(float v, float lo, float hi) { return fminf(hi, fmaxf(lo, v)); }
static inline float chroma_lab(Vec3 lab) { return sqrtf(lab.y * lab.y + lab.z * lab.z); }
static inline Vec3 mmul(const float c0[3], const float c1[3], const float c2[3], const Vec3 v) {
    return v3(c0[0]*v.x + c1[0]*v.y + c2[0]*v.z,
              c0[1]*v.x + c1[1]*v.y + c2[1]*v.z,
              c0[2]*v.x + c1[2]*v.y + c2[2]*v.z);
}
static inline float cbrt_s(float x) { return (x < 0.0f ? -1.0f : 1.0f) * powf(fabsf(x), 1.0f/3.0f); }
static inline float spowf(float x, float p) { float y = powf(fabsf(x), p); return (x < 0.0f) ? -y : y; }
static inline Vec3 cat16_adapt(const Vec3 xyz, const float src_w[3], const float dst_w[3]) {
    const Vec3 lms = mmul(CAT16_c0, CAT16_c1, CAT16_c2, xyz);
    const Vec3 src = mmul(CAT16_c0, CAT16_c1, CAT16_c2, v3(src_w[0],src_w[1],src_w[2]));
    const Vec3 dst = mmul(CAT16_c0, CAT16_c1, CAT16_c2, v3(dst_w[0],dst_w[1],dst_w[2]));
    const Vec3 a = v3(lms.x*dst.x/src.x, lms.y*dst.y/src.y, lms.z*dst.z/src.z);
    return mmul(CAT16I_c0, CAT16I_c1, CAT16I_c2, a);
}
static inline Vec3 d60_to_d65(const Vec3 xyz) { return cat16_adapt(xyz, WHITE_D60, WHITE_D65); }
static inline Vec3 d65_to_d60(const Vec3 xyz) { return cat16_adapt(xyz, WHITE_D65, WHITE_D60); }
static inline Vec3 white_balance(const Vec3 xyz, float temp_value, float tint_value) {
    if (fabsf(temp_value) < 1e-6f && fabsf(tint_value) < 1e-6f) return xyz;
    float k = 6504.0f * expf(temp_value * 1.0f), k2 = k*k, k3 = k2*k;
    float x = k <= 4000.0f ? -0.2661239e9f/k3 - 0.2343580e6f/k2 + 0.8776956e3f/k + 0.179910f
                           : -3.0258469e9f/k3 + 2.1070379e6f/k2 + 0.2226347e3f/k + 0.240390f;
    float y = k <= 4000.0f ? -1.1063814f*x*x*x - 1.34811020f*x*x + 2.18555832f*x - 0.20219683f
                           : 3.0817580f*x*x*x - 5.8733867f*x*x + 3.75112997f*x - 0.37001483f;
    y = fmaxf(y + tint_value*0.025f, 0.05f);
    const float sw[3] = {x/y, 1.0f, (1.0f - x - y)/y};
    return cat16_adapt(xyz, sw, WHITE_D65);
}
static inline Vec3 printer_lights(const Vec3 rgb, float master, float red, float green, float blue) {
    float m = master - 25.0f;
    float rg = powf(10.0f, (m + red  - 25.0f) * 0.0125f);
    float gg = powf(10.0f, (m + green- 25.0f) * 0.0125f);
    float bg = powf(10.0f, (m + blue - 25.0f) * 0.0125f);
    return v3(rgb.x*rg, rgb.y*gg, rgb.z*bg);
}
// ---- Gamut -> XYZ (index g, mirrors DCTL g2x) ----
static inline Vec3 g2x(int g, const Vec3 v) {
    switch (g) {
    case  0: return mmul(R0_c0,R0_c1,R0_c2,v);
    case  1: return mmul(R1_c0,R1_c1,R1_c2,v);
    case  2: return mmul(R2_c0,R2_c1,R2_c2,v);
    case  3: return mmul(R3_c0,R3_c1,R3_c2,v);
    case  4: return mmul(R4_c0,R4_c1,R4_c2,v);
    case  5: return mmul(R5_c0,R5_c1,R5_c2,v);
    case  6: return mmul(R6_c0,R6_c1,R6_c2,v);
    case  7: return mmul(R7_c0,R7_c1,R7_c2,v);
    case  8: return mmul(R8_c0,R8_c1,R8_c2,v);
    case  9: return mmul(R9_c0,R9_c1,R9_c2,v);
    default: return mmul(R10_c0,R10_c1,R10_c2,v);
    }
}
// ---- XYZ -> Gamut (inverse, index g; DCTL x2g) ----
static inline Vec3 x2g(int g, const Vec3 v) {
    switch (g) {
        case  0: return mmul(G0_c0,G0_c1,G0_c2,v);
        case  1: return mmul(G1_c0,G1_c1,G1_c2,v);
        case  2: return mmul(G2_c0,G2_c1,G2_c2,v);
        case  3: return mmul(G3_c0,G3_c1,G3_c2,v);
        case  4: return mmul(G4_c0,G4_c1,G4_c2,v);
        case  5: return mmul(G5_c0,G5_c1,G5_c2,v);
        case  6: return mmul(G6_c0,G6_c1,G6_c2,v);
        case  7: return mmul(G7_c0,G7_c1,G7_c2,v);
        case  8: return mmul(G8_c0,G8_c1,G8_c2,v);
        case  9: return mmul(G9_c0,G9_c1,G9_c2,v);
        default: return mmul(G10_c0,G10_c1,G10_c2,v);
    }
}

// ---- TFs: decode (each mirrors the DCTL __DEVICE__ tf*d) ----
static inline Vec3 tf709d(const Vec3 v) {
    Vec3 o;
    o.x = v.x <= 0.081f ? v.x/4.5f : powf((v.x+0.099f)/1.099f, 1.0f/0.45f);
    o.y = v.y <= 0.081f ? v.y/4.5f : powf((v.y+0.099f)/1.099f, 1.0f/0.45f);
    o.z = v.z <= 0.081f ? v.z/4.5f : powf((v.z+0.099f)/1.099f, 1.0f/0.45f);
    return o;
}
static inline Vec3 tfl4d(const Vec3 v) {
    float a=LOGC4_A, b=LOGC4_B, c=LOGC4_C, s2=LOGC4_S2, t=LOGC4_T; Vec3 o;
    o.x = v.x >= 0.0f ? (powf(2.0f, 14.0f*((v.x-c)/b) + 6.0f) - 64.0f)/a : v.x*s2 + t;
    o.y = v.y >= 0.0f ? (powf(2.0f, 14.0f*((v.y-c)/b) + 6.0f) - 64.0f)/a : v.y*s2 + t;
    o.z = v.z >= 0.0f ? (powf(2.0f, 14.0f*((v.z-c)/b) + 6.0f) - 64.0f)/a : v.z*s2 + t;
    return o;
}
static inline Vec3 tfl3d(const Vec3 v) {
    float cv = LOGC3_E*LOGC3_CUT + LOGC3_F; Vec3 o;
    o.x = v.x > cv ? (powf(10.0f,(v.x-LOGC3_D)/LOGC3_C) - LOGC3_B)/LOGC3_A : (v.x-LOGC3_F)/LOGC3_E;
    o.y = v.y > cv ? (powf(10.0f,(v.y-LOGC3_D)/LOGC3_C) - LOGC3_B)/LOGC3_A : (v.y-LOGC3_F)/LOGC3_E;
    o.z = v.z > cv ? (powf(10.0f,(v.z-LOGC3_D)/LOGC3_C) - LOGC3_B)/LOGC3_A : (v.z-LOGC3_F)/LOGC3_E;
    return o;
}
static inline Vec3 tfs3d(const Vec3 v) {
    Vec3 o;
    o.x = v.x >= 171.2102946929f/1023.0f ? (powf(10.0f,(v.x*1023.0f-420.0f)/261.5f))*0.19f-0.01f : (v.x*1023.0f-95.0f)*0.01125f/(171.2102946929f-95.0f);
    o.y = v.y >= 171.2102946929f/1023.0f ? (powf(10.0f,(v.y*1023.0f-420.0f)/261.5f))*0.19f-0.01f : (v.y*1023.0f-95.0f)*0.01125f/(171.2102946929f-95.0f);
    o.z = v.z >= 171.2102946929f/1023.0f ? (powf(10.0f,(v.z*1023.0f-420.0f)/261.5f))*0.19f-0.01f : (v.z*1023.0f-95.0f)*0.01125f/(171.2102946929f-95.0f);
    return o;
}
static inline Vec3 tfvld(const Vec3 v) {
    float cut=0.181f, b=0.00873f, c=0.241514f, d=0.598206f; Vec3 o;
    o.x = v.x < cut ? (v.x-0.125f)/5.6f : powf(10.0f,(v.x-d)/c) - b;
    o.y = v.y < cut ? (v.y-0.125f)/5.6f : powf(10.0f,(v.y-d)/c) - b;
    o.z = v.z < cut ? (v.z-0.125f)/5.6f : powf(10.0f,(v.z-d)/c) - b;
    return o;
}
static inline Vec3 tflg10d(const Vec3 v) {
    float B = 25.0f*(sqrtf(4093.0f)-3.0f)/9.0f, A = 1.0f/log10f(B*184.32f+1.0f);
    return v3(v.x<0.0f ? v.x/(A*B*0.4342944819f) : (powf(10.0f,v.x/A)-1.0f)/B,
              v.y<0.0f ? v.y/(A*B*0.4342944819f) : (powf(10.0f,v.y/A)-1.0f)/B,
              v.z<0.0f ? v.z/(A*B*0.4342944819f) : (powf(10.0f,v.z/A)-1.0f)/B);
}
static inline Vec3 tfdwd(const Vec3 v) {
    float A=0.0075f, B=7.0f, C=0.07329248f, LC=0.02740668f, M=10.44426855f; Vec3 o;
    o.x = v.x <= LC ? v.x/M : powf(2.0f,(v.x/C)-B) - A;
    o.y = v.y <= LC ? v.y/M : powf(2.0f,(v.y/C)-B) - A;
    o.z = v.z <= LC ? v.z/M : powf(2.0f,(v.z/C)-B) - A;
    return o;
}
static inline Vec3 tfc2d(const Vec3 v) {
    float k=0.092864125f, m=0.24136077f, n=87.09937546f; Vec3 o;
    o.x = v.x < k ? -(powf(10.0f,(k-v.x)/m)-1.0f)/n : (powf(10.0f,(v.x-k)/m)-1.0f)/n;
    o.y = v.y < k ? -(powf(10.0f,(k-v.y)/m)-1.0f)/n : (powf(10.0f,(v.y-k)/m)-1.0f)/n;
    o.z = v.z < k ? -(powf(10.0f,(k-v.z)/m)-1.0f)/n : (powf(10.0f,(v.z-k)/m)-1.0f)/n;
    return v3(o.x*0.9f, o.y*0.9f, o.z*0.9f);
}
static inline Vec3 tfc3d(const Vec3 v) {
    Vec3 o;
    o.x = v.x < 0.097465473f ? -(powf(10.0f,(0.12783901f-v.x)/0.36726845f)-1.0f)/14.98325f
           : (v.x <= 0.15277891f ? (v.x-0.12512219f)/1.9754798f : (powf(10.0f,(v.x-0.12240537f)/0.36726845f)-1.0f)/14.98325f);
    o.y = v.y < 0.097465473f ? -(powf(10.0f,(0.12783901f-v.y)/0.36726845f)-1.0f)/14.98325f
           : (v.y <= 0.15277891f ? (v.y-0.12512219f)/1.9754798f : (powf(10.0f,(v.y-0.12240537f)/0.36726845f)-1.0f)/14.98325f);
    o.z = v.z < 0.097465473f ? -(powf(10.0f,(0.12783901f-v.z)/0.36726845f)-1.0f)/14.98325f
           : (v.z <= 0.15277891f ? (v.z-0.12512219f)/1.9754798f : (powf(10.0f,(v.z-0.12240537f)/0.36726845f)-1.0f)/14.98325f);
    return v3(o.x*0.9f, o.y*0.9f, o.z*0.9f);
}
static inline Vec3 tfdld(const Vec3 v) {
    Vec3 o;
    o.x = v.x <= 0.14f ? (v.x-0.0929f)/6.025f : (powf(10.0f,3.89616f*v.x-2.27752f)-0.0108f)/0.9892f;
    o.y = v.y <= 0.14f ? (v.y-0.0929f)/6.025f : (powf(10.0f,3.89616f*v.y-2.27752f)-0.0108f)/0.9892f;
    o.z = v.z <= 0.14f ? (v.z-0.0929f)/6.025f : (powf(10.0f,3.89616f*v.z-2.27752f)-0.0108f)/0.9892f;
    return o;
}
static inline Vec3 tfapl(const Vec3 v) {
    float Pt = APPLE_SIGMA*(APPLE_RT-APPLE_R0)*(APPLE_RT-APPLE_R0); Vec3 o;
    o.x = v.x >= Pt ? powf(2.0f,(v.x-APPLE_DELTA)/APPLE_GAMMA) - APPLE_BETA
         : (v.x >= 0.0f ? sqrtf(v.x/APPLE_SIGMA)+APPLE_R0 : APPLE_R0);
    o.y = v.y >= Pt ? powf(2.0f,(v.y-APPLE_DELTA)/APPLE_GAMMA) - APPLE_BETA
         : (v.y >= 0.0f ? sqrtf(v.y/APPLE_SIGMA)+APPLE_R0 : APPLE_R0);
    o.z = v.z >= Pt ? powf(2.0f,(v.z-APPLE_DELTA)/APPLE_GAMMA) - APPLE_BETA
         : (v.z >= 0.0f ? sqrtf(v.z/APPLE_SIGMA)+APPLE_R0 : APPLE_R0);
    return o;
}
static inline Vec3 tfcctd(const Vec3 v) {
    float A=10.5402377417f, B=0.0729055342f, YB=0.15525114f; Vec3 o;
    o.x = v.x <= YB ? (v.x-B)/A : powf(2.0f, v.x*17.52f-9.72f);
    o.y = v.y <= YB ? (v.y-B)/A : powf(2.0f, v.y*17.52f-9.72f);
    o.z = v.z <= YB ? (v.z-B)/A : powf(2.0f, v.z*17.52f-9.72f);
    return o;
}
static inline Vec3 tfd(const Vec3 r, int t) {
    if (t == 0)  return tf709d(r);
    if (t == 1)  return tfl4d(r);
    if (t == 2)  return tfl3d(r);
    if (t == 3)  return tfs3d(r);
    if (t == 4)  return tfvld(r);
    if (t == 5)  return tflg10d(r);
    if (t == 6)  return tfdwd(r);
    if (t == 7)  return r;
    if (t == 8)  return tfc2d(r);
    if (t == 9)  return tfc3d(r);
    if (t == 10) return tfdld(r);
    if (t == 11) return r;
    if (t == 12) return r;
    if (t == 13) return tfapl(r);
    return tfcctd(r);
}
// ---- TFs: encode ----
static inline Vec3 tf709e(const Vec3 v) {
    Vec3 o;
    o.x = v.x <= 0.018f ? 4.5f*v.x : 1.099f*powf(v.x,0.45f) - 0.099f;
    o.y = v.y <= 0.018f ? 4.5f*v.y : 1.099f*powf(v.y,0.45f) - 0.099f;
    o.z = v.z <= 0.018f ? 4.5f*v.z : 1.099f*powf(v.z,0.45f) - 0.099f;
    return o;
}
static inline Vec3 tfl4e(const Vec3 v) {
    float a=LOGC4_A, b=LOGC4_B, c=LOGC4_C, s2=LOGC4_S2, t=LOGC4_T; Vec3 o;
    o.x = v.x > t ? c + b*(log10f(a*v.x+64.0f)/log10f(2.0f) - 6.0f)/14.0f : (v.x-t)/s2;
    o.y = v.y > t ? c + b*(log10f(a*v.y+64.0f)/log10f(2.0f) - 6.0f)/14.0f : (v.y-t)/s2;
    o.z = v.z > t ? c + b*(log10f(a*v.z+64.0f)/log10f(2.0f) - 6.0f)/14.0f : (v.z-t)/s2;
    return o;
}
static inline Vec3 tfl3e(const Vec3 v) {
    Vec3 o;
    o.x = v.x > LOGC3_CUT ? LOGC3_C*log10f(LOGC3_A*v.x+LOGC3_B)+LOGC3_D : LOGC3_E*v.x+LOGC3_F;
    o.y = v.y > LOGC3_CUT ? LOGC3_C*log10f(LOGC3_A*v.y+LOGC3_B)+LOGC3_D : LOGC3_E*v.y+LOGC3_F;
    o.z = v.z > LOGC3_CUT ? LOGC3_C*log10f(LOGC3_A*v.z+LOGC3_B)+LOGC3_D : LOGC3_E*v.z+LOGC3_F;
    return o;
}
static inline Vec3 tfs3e(const Vec3 v) {
    float a=0.19f, b=0.01f, c=261.5f, d=420.0f, e=171.2102946929f, f=1023.0f; Vec3 o;
    o.x = v.x <= 0.01125f ? (v.x*(e-95.0f)/0.01125f+95.0f)/f : (log10f((v.x+b)/a)*c+d)/f;
    o.y = v.y <= 0.01125f ? (v.y*(e-95.0f)/0.01125f+95.0f)/f : (log10f((v.y+b)/a)*c+d)/f;
    o.z = v.z <= 0.01125f ? (v.z*(e-95.0f)/0.01125f+95.0f)/f : (log10f((v.z+b)/a)*c+d)/f;
    return o;
}
static inline Vec3 tfvle(const Vec3 v) {
    float cut=0.181f, b=0.00873f, c=0.241514f, d=0.598206f; Vec3 o;
    o.x = v.x <= (cut-0.125f)/5.6f ? v.x*5.6f+0.125f : c*log10f(v.x+b)+d;
    o.y = v.y <= (cut-0.125f)/5.6f ? v.y*5.6f+0.125f : c*log10f(v.y+b)+d;
    o.z = v.z <= (cut-0.125f)/5.6f ? v.z*5.6f+0.125f : c*log10f(v.z+b)+d;
    return o;
}
static inline Vec3 tflg10e(const Vec3 v) {
    float B = 25.0f*(sqrtf(4093.0f)-3.0f)/9.0f, A = 1.0f/log10f(B*184.32f+1.0f);
    return v3(v.x<0.0f ? v.x*(A*B*0.4342944819f) : A*log10f(v.x*B+1.0f),
              v.y<0.0f ? v.y*(A*B*0.4342944819f) : A*log10f(v.y*B+1.0f),
              v.z<0.0f ? v.z*(A*B*0.4342944819f) : A*log10f(v.z*B+1.0f));
}
static inline Vec3 tfdwe(const Vec3 v) {
    float A=0.0075f, B=7.0f, C=0.07329248f, LC=0.02740668f, M=10.44426855f; Vec3 o;
    o.x = v.x <= LC/M ? v.x*M : C*(log2f(v.x+A)+B);
    o.y = v.y <= LC/M ? v.y*M : C*(log2f(v.y+A)+B);
    o.z = v.z <= LC/M ? v.z*M : C*(log2f(v.z+A)+B);
    return o;
}
static inline Vec3 tfc2e(const Vec3 v) {
    float k=0.092864125f, m=0.24136077f, n=87.09937546f;
    Vec3 w = v3(v.x/0.9f, v.y/0.9f, v.z/0.9f); Vec3 o;
    o.x = w.x < 0.0f ? k - m*log10f(-w.x*n+1.0f) : k + m*log10f(w.x*n+1.0f);
    o.y = w.y < 0.0f ? k - m*log10f(-w.y*n+1.0f) : k + m*log10f(w.y*n+1.0f);
    o.z = w.z < 0.0f ? k - m*log10f(-w.z*n+1.0f) : k + m*log10f(w.z*n+1.0f);
    return o;
}
static inline Vec3 tfc3e(const Vec3 v) {
    Vec3 w = v3(v.x/0.9f, v.y/0.9f, v.z/0.9f); Vec3 o;
    o.x = w.x <= 0.0f ? 0.12783901f - 0.36726845f*log10f(-w.x*14.98325f+1.0f)
         : (w.x <= (0.15277891f-0.12512219f)/1.9754798f ? w.x*1.9754798f+0.12512219f
            : 0.12240537f + 0.36726845f*log10f(w.x*14.98325f+1.0f));
    o.y = w.y <= 0.0f ? 0.12783901f - 0.36726845f*log10f(-w.y*14.98325f+1.0f)
         : (w.y <= (0.15277891f-0.12512219f)/1.9754798f ? w.y*1.9754798f+0.12512219f
            : 0.12240537f + 0.36726845f*log10f(w.y*14.98325f+1.0f));
    o.z = w.z <= 0.0f ? 0.12783901f - 0.36726845f*log10f(-w.z*14.98325f+1.0f)
         : (w.z <= (0.15277891f-0.12512219f)/1.9754798f ? w.z*1.9754798f+0.12512219f
            : 0.12240537f + 0.36726845f*log10f(w.z*14.98325f+1.0f));
    return o;
}
static inline Vec3 tfdle(const Vec3 v) {
    Vec3 o;
    o.x = v.x <= (0.14f-0.0929f)/6.025f ? v.x*6.025f+0.0929f : (log10f(v.x*0.9892f+0.0108f)+2.27752f)/3.89616f;
    o.y = v.y <= (0.14f-0.0929f)/6.025f ? v.y*6.025f+0.0929f : (log10f(v.y*0.9892f+0.0108f)+2.27752f)/3.89616f;
    o.z = v.z <= (0.14f-0.0929f)/6.025f ? v.z*6.025f+0.0929f : (log10f(v.z*0.9892f+0.0108f)+2.27752f)/3.89616f;
    return o;
}
static inline Vec3 tfaple(const Vec3 v) {
    Vec3 o;
    o.x = v.x <= APPLE_R0 ? 0.0f : v.x < APPLE_RT ? APPLE_SIGMA*(v.x-APPLE_R0)*(v.x-APPLE_R0)
        : APPLE_DELTA + APPLE_GAMMA*log2f(v.x+APPLE_BETA);
    o.y = v.y <= APPLE_R0 ? 0.0f : v.y < APPLE_RT ? APPLE_SIGMA*(v.y-APPLE_R0)*(v.y-APPLE_R0)
        : APPLE_DELTA + APPLE_GAMMA*log2f(v.y+APPLE_BETA);
    o.z = v.z <= APPLE_R0 ? 0.0f : v.z < APPLE_RT ? APPLE_SIGMA*(v.z-APPLE_R0)*(v.z-APPLE_R0)
        : APPLE_DELTA + APPLE_GAMMA*log2f(v.z+APPLE_BETA);
    return o;
}
static inline Vec3 tfccte(const Vec3 v) {
    float A=10.5402377417f, B=0.0729055342f, XB=0.0078125f; Vec3 o;
    o.x = v.x <= XB ? A*v.x+B : (log2f(v.x)+9.72f)/17.52f;
    o.y = v.y <= XB ? A*v.y+B : (log2f(v.y)+9.72f)/17.52f;
    o.z = v.z <= XB ? A*v.z+B : (log2f(v.z)+9.72f)/17.52f;
    return o;
}
static inline Vec3 tfe(const Vec3 r, int t) {
    if (t == 0)  return tf709e(r);
    if (t == 1)  return tfl4e(r);
    if (t == 2)  return tfl3e(r);
    if (t == 3)  return tfs3e(r);
    if (t == 4)  return tfvle(r);
    if (t == 5)  return tflg10e(r);
    if (t == 6)  return tfdwe(r);
    if (t == 7)  return r;
    if (t == 8)  return tfc2e(r);
    if (t == 9)  return tfc3e(r);
    if (t == 10) return tfdle(r);
    if (t == 11) return r;
    if (t == 12) return r;
    if (t == 13) return tfaple(r);
    return tfccte(r);
}

// ---- OKLab (Ottosson) ----
static inline Vec3 l2o(const Vec3 c) {
    const Vec3 l = mmul(L_c0, L_c1, L_c2, c);
    return mmul(O_c0, O_c1, O_c2, v3(cbrt_s(l.x), cbrt_s(l.y), cbrt_s(l.z)));
}
static inline Vec3 o2l(const Vec3 c) {
    const Vec3 l = mmul(P_c0, P_c1, P_c2, c);
    return mmul(Q_c0, Q_c1, Q_c2, v3(l.x*l.x*l.x, l.y*l.y*l.y, l.z*l.z*l.z));
}
static inline Vec3 work_to_oklab(const Vec3 work) {
    const Vec3 xyz = g2x(1, work);
    return l2o(mmul(I0_c0, I0_c1, I0_c2, xyz));
}
static inline Vec3 oklab_to_work(const Vec3 lab) {
    const Vec3 srgb = o2l(lab);
    return x2g(1, mmul(S0_c0, S0_c1, S0_c2, srgb));
}
// ---- OKLab gamut-aware chroma compression (AWG4 boundary, scan + refine) ----
static inline bool awg4_ingamut(float L, float cosh, float sinh, float r) {
    float l0 = L + P_c1[0]*cosh*r + P_c2[0]*sinh*r;
    float l1 = L + P_c1[1]*cosh*r + P_c2[1]*sinh*r;
    float l2 = L + P_c1[2]*cosh*r + P_c2[2]*sinh*r;
    l0 = l0*l0*l0; l1 = l1*l1*l1; l2 = l2*l2*l2;
    float a0 =  1.8746868966f*l0 - 1.0494462399f*l1 + 0.1747593441f*l2;
    float a1 = -0.6657305832f*l0 + 1.7472828065f*l1 - 0.0815522231f*l2;
    float a2 = -0.0701275482f*l0 - 0.3870256867f*l1 + 1.4571532352f*l2;
    return a0 >= 0.0f && a1 >= 0.0f && a2 >= 0.0f;
}
static inline Vec3 compress_oklab_chroma(Vec3 lab, float amount) {
    if (amount <= 0.0f) return lab;
    const float L = lab.x;
    float c = sqrtf(lab.y*lab.y + lab.z*lab.z);
    if (c <= 0.04f) return lab;                       // protect near-neutral
    const float hue = atan2f(lab.z, lab.y);
    const float cosh = cosf(hue), sinh = sinf(hue);
    float cmax = 4.0f;
    bool prev = awg4_ingamut(L, cosh, sinh, 0.0f);
    for (int i = 1; i <= 32; i++) {
        float r = (float)i*4.0f/32.0f;
        bool cur = awg4_ingamut(L, cosh, sinh, r);
        if (prev && !cur) {
            float lo = (float)(i-1)*4.0f/32.0f, hi = r;
            for (int it = 0; it < 10; it++) {
                float mid = 0.5f*(lo + hi);
                if (awg4_ingamut(L, cosh, sinh, mid)) lo = mid; else hi = mid;
            }
            cmax = 0.5f*(lo + hi);
            break;
        }
        prev = cur;
    }
    cmax = fmaxf(cmax, c);
    const float guard = 0.72f;
    float g = guard*cmax;
    if (c <= g) return lab;
    float t = fminf((c - g)/(cmax - g + 1e-6f), 1.0f);
    float k = 1.0f/(1.0f + amount*4.0f*t);
    float comp = g + (c - g)*k;
    float scale = fminf(comp/c, 1.0f);
    lab.y *= scale; lab.z *= scale;
    return lab;
}
static inline Vec3 compress_to_gamut(const Vec3 rgb, float luminance) {
    if (luminance <= 0.0f) return rgb;
    float scale = 1.0f;
    const Vec3 delta = v3(rgb.x - luminance, rgb.y - luminance, rgb.z - luminance);
    if (rgb.x < 0.0f && delta.x < 0.0f) scale = fminf(scale, luminance/-delta.x);
    if (rgb.y < 0.0f && delta.y < 0.0f) scale = fminf(scale, luminance/-delta.y);
    if (rgb.z < 0.0f && delta.z < 0.0f) scale = fminf(scale, luminance/-delta.z);
    scale = fmaxf(fminf(scale, 1.0f), 0.0f);
    return v3(luminance + delta.x*scale, luminance + delta.y*scale, luminance + delta.z*scale);
}
// ---- Density/desat helpers ----
static inline float rgb_sat(const Vec3 c) {
    const float mn = fminf(c.x, fminf(c.y, c.z)), mx = fmaxf(c.x, fmaxf(c.y, c.z));
    return mx == 0.0f ? 0.0f : (mx - mn)/mx;
}
static inline float smootherstep(float e0, float e1, float x) {
    x = fmaxf(e0, fminf(e1, x));
    x = (x - e0)/(e1 - e0);
    return x*x*(3.0f - 2.0f*x);
}
static inline float limiter(float v, float l) { return smootherstep(0.0f, l, v); }
// Monotonic shadow + highlight shaping on OKLab lightness.
static inline float shape_tone_oklab(float v, float sh, float hl) {
    const float HINGE = 0.70f, KNEE = 0.18f, SH_SLOPE = 0.35f;
    float sw = 1.0f - smootherstep(0.0f, 0.45f, v);
    v *= 1.0f + sh*SH_SLOPE*sw;
    float hw = smootherstep(HINGE - KNEE, HINGE + KNEE, v);
    float gap = v - HINGE;
    float shp = hl > 0.0f ? HINGE + gap/(1.0f + hl*2.5f*fabsf(gap)) : HINGE + gap*(1.0f - hl*0.5f);
    v += (shp - v)*hw;
    return v;
}
// ---- Tetra skin helpers (thatcherfreeman/SmoothTetraSkin) ----
static inline float deg_to_rad(float d) { return d*(2.0f*PI)/360.0f; }
static inline float mod2(float x, float y) { return x < 0.0f ? y + fmodf(x, y) : fmodf(x, y); }

struct tp_t { float ri[7], hi[7], si[7], vi[7]; };
static inline tp_t mk_tp(const float ri[7], const float hi[7], const float si[7], const float vi[7]) {
    tp_t p; for (int i = 0; i < 7; i++) { p.ri[i] = ri[i]; p.hi[i] = hi[i]; p.si[i] = si[i]; p.vi[i] = vi[i]; }
    return p;
}
static inline tp_t lim_tp(tp_t p) {
    float pos[7]; for (int i = 0; i < 7; i++) pos[i] = p.ri[i] + p.hi[i];
    if (pos[0] > pos[6]) pos[0] -= 2.0f*PI;
    for (int j = 0; j < 12; j++) {
        int u = 0;
        for (int i = 0; i < 6; i++) {
            if (pos[i] > pos[i+1]) { float m = (pos[i]+pos[i+1])*0.5f; pos[i] = m; pos[i+1] = m; u = 1; }
        }
        if (pos[6] > pos[0] + 2.0f*PI) { float m = (pos[6]+pos[0]+2.0f*PI)*0.5f; pos[6] = m; pos[0] = m - 2.0f*PI; u = 1; }
        if (!u) break;
    }
    for (int i = 0; i < 7; i++) p.hi[i] = pos[i] - p.ri[i];
    return p;
}
struct Vec2f { float x, y; };
static inline float interp_cyl(float ir, const Vec2f cp[7], float sm) {
    float n = 0.0f, d = 0.0f;
    for (int i = 0; i < 7; i++) { float w = expf(sm*cosf(ir - cp[i].x)); n += w*cp[i].y; d += w; }
    return n/d;
}
static inline Vec3 ev_curves(float ir, const tp_t& p, float sm) {
    Vec2f hp[7], sp[7], vp[7];
    for (int i = 0; i < 7; i++) { hp[i] = {p.ri[i], p.hi[i]}; sp[i] = {p.ri[i], p.si[i]}; vp[i] = {p.ri[i], p.vi[i]}; }
    return v3(interp_cyl(ir, hp, sm), interp_cyl(ir, sp, sm), interp_cyl(ir, vp, sm));
}

// ---------------------------------------------------------------------------
// transform — the per-pixel grade. Faithful port of the DCTL transform().
// Params come from LaserLabParams; all are clamped to the DCTL's UI ranges
// (keyframes can bypass slider limits).
// ---------------------------------------------------------------------------
static inline Vec3 transform(const LaserLabParams& pr, float p_R, float p_G, float p_B) {
    // Clamp all parameters to safe ranges (mirror DEFINE_UI_PARAMS bounds).
    float c_temp = clampf(pr.temp, -2.0f, 2.0f), c_tint = clampf(pr.tint, -1.5f, 1.5f);
    float c_lift = clampf(pr.lift, -0.5f, 0.5f);
    float c_gamma = clampf(pr.gamma, 0.2f, 3.0f), c_gain = clampf(pr.gain, 0.0f, 4.0f);
    float c_contrast = clampf(pr.contrast, 0.5f, 1.5f);
    float c_shadows = clampf(pr.shadows, -1.0f, 1.0f), c_highlights = clampf(pr.highlights, -1.0f, 1.0f);
    float c_exp = clampf(pr.expGlobal, -5.0f, 5.0f), c_flash = clampf(pr.expFlash, -2.0f, 2.0f);
    float c_sat = clampf(pr.satAmount, 0.0f, 3.0f), c_hue = clampf(pr.hueDeg, -180.0f, 180.0f);
    float c_satcomp = clampf(pr.satCompression, 0.0f, 1.0f);
    float c_desat_hi = clampf(pr.desatHigh, 0.0f, 1.0f), c_mid = clampf(pr.midDetail, 0.0f, 1.0f);
    float c_skinH = clampf(pr.skinHueDeg, -45.0f, 45.0f), c_skinS = clampf(pr.skinSat, 0.0f, 2.0f);
    float c_skinV = clampf(pr.skinVal, -0.5f, 0.5f), c_skinE = clampf(pr.skinEvenness, 0.0f, 1.0f);
    float c_gsat = clampf(pr.globalSat, -1.0f, 1.0f);
    float c_limsat = clampf(pr.limSat, 0.0f, 1.0f), c_limdens = clampf(pr.limDens, 0.0f, 1.0f);
    float c_rd = clampf(pr.rDens, 0.0f, 1.0f), c_yd = clampf(pr.yDens, 0.0f, 1.0f);
    float c_gd = clampf(pr.gDens, 0.0f, 1.0f);
    float c_cd = clampf(pr.cDens, 0.0f, 1.0f), c_bd = clampf(pr.bDens, 0.0f, 1.0f);
    float c_md = clampf(pr.mDens, 0.0f, 1.0f);
    float c_plm = clampf(pr.plMaster, 0.0f, 50.0f), c_plr = clampf(pr.plRed, 0.0f, 50.0f);
    float c_plg = clampf(pr.plGreen, 0.0f, 50.0f), c_plb = clampf(pr.plBlue, 0.0f, 50.0f);

    int g = (int)fminf(fmaxf((float)(pr.inputGamut + 0.5f), 0.0f), 10.0f);
    int ti_tf = (int)fminf(fmaxf((float)(pr.inputTransfer + 0.5f), 0.0f), 14.0f);
    Vec3 r = v3(p_R, p_G, p_B);

    // Forward: native decode -> XYZ D65 -> CAT16 white balance -> linear AWG4.
    Vec3 lin = tfd(r, ti_tf);
    Vec3 xyz = g2x(g, lin);
    if (g == 8) xyz = d60_to_d65(xyz);
    xyz = white_balance(xyz, c_temp, c_tint);
    Vec3 work = x2g(1, xyz);
    work = printer_lights(work, c_plm, c_plr, c_plg, c_plb);
    // Scene-linear exposure -> flash -> master gain, all before the OKLab grade.
    work = v3(work.x*powf(2.0f,c_exp), work.y*powf(2.0f,c_exp), work.z*powf(2.0f,c_exp));
    work.x += c_flash*EXPO_FLASH_SCALE; work.y += c_flash*EXPO_FLASH_SCALE; work.z += c_flash*EXPO_FLASH_SCALE;
    work = v3(work.x*c_gain, work.y*c_gain, work.z*c_gain);

    // Grade in OKLab.
    Vec3 lab = work_to_oklab(work);
    float L = lab.x;

    // Global OKLab saturation/hue.
    float av = lab.y*c_sat, bv = lab.z*c_sat;
    float ang = c_hue*3.14159265359f/180.0f, ca = cosf(ang), sak = sinf(ang);
    lab.y = av*ca - bv*sak; lab.z = av*sak + bv*ca;
    lab = compress_oklab_chroma(lab, c_satcomp);

    // Tonal: lift (shadow-biased), gamma, contrast S-curve, all on OKLab L.
    const float OKLAB_MIDGRAY = 0.565f;
    float liftMask = 1.0f - smootherstep(0.0f, 0.7f, L);
    L += c_lift*liftMask;
    L = spowf(L, 1.0f/c_gamma);
    if (fabsf(c_contrast - 1.0f) > 1e-4f) {
        const float pivot = OKLAB_MIDGRAY;
        L = pivot*spowf(L/pivot, c_contrast);
    }
    L = shape_tone_oklab(L, c_shadows, c_highlights);
    lab.x = L;

    // Desat highlights.
    if (c_desat_hi > 0.001f) {
        float hiW = smootherstep(0.80f, 1.15f, L);
        float cs = 1.0f - c_desat_hi*hiW;
        lab.y *= cs; lab.z *= cs;
    }

    // Mid Chroma.
    if (c_mid > 0.001f) {
        float mdMask = smootherstep(0.2f, 0.4f, L)*(1.0f - smootherstep(0.6f, 0.8f, L));
        float md = 1.0f + c_mid*mdMask*0.3f;
        lab.y *= md; lab.z *= md;
    }

    // Skin/warm axis (tetra).
    float sk_dg = smootherstep(0.02f, 0.18f, L), sk_bg = 1.0f - smootherstep(1.0f, 1.5f, L);
    float sk_lw = sk_dg*sk_bg;
    float sk_ri[7] = {0.5102284f, 1.0685f, 1.9158353f, 2.4870127f, 3.3993594f, 4.6085770f, 5.7310226f};
    float sk_hi[7] = {0.0f, deg_to_rad(c_skinH), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    float sk_si[7] = {1.0f, c_skinS, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    float sk_vi[7] = {0.0f, c_skinV, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    tp_t sk_p = mk_tp(sk_ri, sk_hi, sk_si, sk_vi); sk_p = lim_tp(sk_p);
    float sk_c = sqrtf(lab.y*lab.y + lab.z*lab.z);
    float sk_h = sk_c > 0.0f ? mod2(atan2f(lab.z, lab.y), 2.0f*PI) : 0.0f;
    Vec3 sk_adj = ev_curves(sk_h, sk_p, 5.0f);
    Vec3 lab_skin = v3(lab.x + sk_adj.z, (sk_c*sk_adj.y)*cosf(sk_h + sk_adj.x), (sk_c*sk_adj.y)*sinf(sk_h + sk_adj.x));
    float sk_nm = smootherstep(0.0f, 0.1f, sk_c)*sk_lw;
    lab = v3(lab.x + (lab_skin.x - lab.x)*sk_nm, lab.y + (lab_skin.y - lab.y)*sk_nm, lab.z + (lab_skin.z - lab.z)*sk_nm);
    if (c_skinE > 0.0f) {
        float sk_ec = sqrtf(lab.y*lab.y + lab.z*lab.z);
        float sk_eh = sk_ec > 0.0f ? mod2(atan2f(lab.z, lab.y), 2.0f*PI) : 0.0f;
        float sk_ealign = cosf(sk_eh - 1.0685f);
        float sk_emask = smootherstep(0.70f, 0.95f, sk_ealign)*smootherstep(0.015f, 0.080f, sk_ec)*sk_lw;
        if (sk_emask > 0.0f) {
            float sk_edelta = atan2f(sinf(1.0685f - sk_eh), cosf(1.0685f - sk_eh));
            float sk_eout = sk_eh + sk_edelta*c_skinE*sk_emask;
            lab.y = sk_ec*cosf(sk_eout); lab.z = sk_ec*sinf(sk_eout);
        }
    }

    Vec3 out = oklab_to_work(lab);

    // Density: subtractive sat + per-hue density (MONONODES-style), in linear AWG4.
    float d_asat = (c_limsat > 0.0f) ? limiter(rgb_sat(out), c_limsat) : 1.0f;
    float d_aden = (c_limdens > 0.0f) ? limiter(rgb_sat(out), c_limdens) : 1.0f;
    float d_rd = -(c_rd*d_aden); float d_gd = -(c_gd*d_aden); float d_bd = -(c_bd*d_aden);
    float d_cd = -(c_cd*d_aden*0.5f); float d_md = -(c_md*d_aden*0.5f); float d_yd = -(c_yd*d_aden*0.5f);
    float d_gs = c_gsat*d_asat;
    Vec3 d_blk = v3(0.0f, 0.0f, 0.0f);
    Vec3 d_wht = v3(1.0f, 1.0f, 1.0f);
    Vec3 d_rv = v3(d_rd + 1.0f, d_rd - d_gs, d_rd - d_gs);
    Vec3 d_gv = v3(d_gd - d_gs, d_gd + 1.0f, d_gd - d_gs);
    Vec3 d_bv = v3(d_bd - d_gs, d_bd - d_gs, d_bd + 1.0f);
    Vec3 d_cv = v3(d_cd - d_gs, d_cd + 1.0f, d_cd + 1.0f);
    Vec3 d_mv = v3(d_md + 1.0f, d_md - d_gs, d_md + 1.0f);
    Vec3 d_yv = v3(d_yd + 1.0f, d_yd + 1.0f, d_yd - d_gs);
    float d_Rv = out.x; float d_Gv = out.y; float d_Bv = out.z;
    Vec3 d_nrgb;
    float d_r1x = d_rv.x - d_blk.x, d_r1y = d_rv.y - d_blk.y, d_r1z = d_rv.z - d_blk.z;
    if (d_Rv > d_Gv) {
        if (d_Gv > d_Bv) {
            float d_y2x = d_yv.x - d_rv.x, d_y2y = d_yv.y - d_rv.y, d_y2z = d_yv.z - d_rv.z;
            float d_w3x = d_wht.x - d_yv.x, d_w3y = d_wht.y - d_yv.y, d_w3z = d_wht.z - d_yv.z;
            d_nrgb = v3(d_Rv*d_r1x + d_blk.x + d_Gv*d_y2x + d_Bv*d_w3x,
                        d_Rv*d_r1y + d_blk.y + d_Gv*d_y2y + d_Bv*d_w3y,
                        d_Rv*d_r1z + d_blk.z + d_Gv*d_y2z + d_Bv*d_w3z);
        } else if (d_Rv > d_Bv) {
            float d_w2x = d_wht.x - d_mv.x, d_w2y = d_wht.y - d_mv.y, d_w2z = d_wht.z - d_mv.z;
            float d_m3x = d_mv.x - d_rv.x, d_m3y = d_mv.y - d_rv.y, d_m3z = d_mv.z - d_rv.z;
            d_nrgb = v3(d_Rv*d_r1x + d_blk.x + d_Gv*d_w2x + d_Bv*d_m3x,
                        d_Rv*d_r1y + d_blk.y + d_Gv*d_w2y + d_Bv*d_m3y,
                        d_Rv*d_r1z + d_blk.z + d_Gv*d_w2z + d_Bv*d_m3z);
        } else {
            float d_m4x = d_mv.x - d_bv.x, d_m4y = d_mv.y - d_bv.y, d_m4z = d_mv.z - d_bv.z;
            float d_w5x = d_wht.x - d_mv.x, d_w5y = d_wht.y - d_mv.y, d_w5z = d_wht.z - d_mv.z;
            float d_b6x = d_bv.x - d_blk.x, d_b6y = d_bv.y - d_blk.y, d_b6z = d_bv.z - d_blk.z;
            d_nrgb = v3(d_Rv*d_m4x + d_Gv*d_w5x + d_Bv*d_b6x + d_blk.x,
                        d_Rv*d_m4y + d_Gv*d_w5y + d_Bv*d_b6y + d_blk.y,
                        d_Rv*d_m4z + d_Gv*d_w5z + d_Bv*d_b6z + d_blk.z);
        }
    } else {
        float d_w7x = d_wht.x - d_cv.x, d_w7y = d_wht.y - d_cv.y, d_w7z = d_wht.z - d_cv.z;
        float d_c8x = d_cv.x - d_bv.x, d_c8y = d_cv.y - d_bv.y, d_c8z = d_cv.z - d_bv.z;
        float d_b9x = d_bv.x - d_blk.x, d_b9y = d_bv.y - d_blk.y, d_b9z = d_bv.z - d_blk.z;
        if (d_Bv > d_Gv) {
            d_nrgb = v3(d_Rv*d_w7x + d_Gv*d_c8x + d_Bv*d_b9x + d_blk.x,
                        d_Rv*d_w7y + d_Gv*d_c8y + d_Bv*d_b9y + d_blk.y,
                        d_Rv*d_w7z + d_Gv*d_c8z + d_Bv*d_b9z + d_blk.z);
        } else if (d_Bv > d_Rv) {
            float d_gAx = d_gv.x - d_blk.x, d_gAy = d_gv.y - d_blk.y, d_gAz = d_gv.z - d_blk.z;
            float d_cBx = d_cv.x - d_gv.x, d_cBy = d_cv.y - d_gv.y, d_cBz = d_cv.z - d_gv.z;
            d_nrgb = v3(d_Rv*d_w7x + d_Gv*d_gAx + d_blk.x + d_Bv*d_cBx,
                        d_Rv*d_w7y + d_Gv*d_gAy + d_blk.y + d_Bv*d_cBy,
                        d_Rv*d_w7z + d_Gv*d_gAz + d_blk.z + d_Bv*d_cBz);
        } else {
            float d_yCx = d_yv.x - d_gv.x, d_yCy = d_yv.y - d_gv.y, d_yCz = d_yv.z - d_gv.z;
            float d_gDx = d_gv.x - d_blk.x, d_gDy = d_gv.y - d_blk.y, d_gDz = d_gv.z - d_blk.z;
            float d_wEx = d_wht.x - d_yv.x, d_wEy = d_wht.y - d_yv.y, d_wEz = d_wht.z - d_yv.z;
            d_nrgb = v3(d_Rv*d_yCx + d_Gv*d_gDx + d_blk.x + d_Bv*d_wEx,
                        d_Rv*d_yCy + d_Gv*d_gDy + d_blk.y + d_Bv*d_wEy,
                        d_Rv*d_yCz + d_Gv*d_gDz + d_blk.z + d_Bv*d_wEz);
        }
    }
    out = d_nrgb;

    // Reverse: linear AWG4 -> destination XYZ/gamut -> native transfer.
    Vec3 rxyz = g2x(1, out);
    if (g == 8) rxyz = d65_to_d60(rxyz);
    Vec3 cam = x2g(g, rxyz);
    cam = compress_to_gamut(cam, rxyz.y);
    Vec3 result = tfe(cam, ti_tf);

    // Preserve finite extended values; only replace NaN/Inf.
    float nx = (result.x == result.x && fabsf(result.x) < 3.402823e38f) ? result.x : 0.0f;
    float ny = (result.y == result.y && fabsf(result.y) < 3.402823e38f) ? result.y : 0.0f;
    float nz = (result.z == result.z && fabsf(result.z) < 3.402823e38f) ? result.z : 0.0f;
    return v3(nx, ny, nz);
}

// ---------------------------------------------------------------------------
// Skin confidence: how likely is this OKLab color to be skin?
// Uses circular hue distance from the 1.0685 rad skin-tone anchor and
// chroma qualification (not too desaturated, not too chromatic).
// Returns 0..1.
// ---------------------------------------------------------------------------
static inline float skin_confidence(float L, float a, float b) {
    float c = sqrtf(a*a + b*b);
    float hue = (c > 0.0f) ? mod2(atan2f(b, a), 2.0f*PI) : 0.0f;
    float hue_align = cosf(hue - 1.0685f);                        // 1 at skin line
    float hue_w = smootherstep(0.5f, 0.95f, hue_align);          // broad hue band
    float chroma_w = smootherstep(0.008f, 0.04f, c)              // reject near-neutral
                   * (1.0f - smootherstep(0.35f, 0.60f, c));     // reject very chromatic
    float L_w = smootherstep(0.10f, 0.25f, L)                    // reject deep shadows
              * (1.0f - smootherstep(0.85f, 1.10f, L));          // reject super-whites
    return hue_w * chroma_w * L_w;
}

// ---------------------------------------------------------------------------
// Spatial skin color evening: edge-aware averaging of OKLab a/b among
// neighboring skin pixels.  Operates on an intermediate OKLab buffer
// (the point-processed result).  Preserves center L, rejects luminance
// edges and non-skin neighbors.  The source is the original (uncorrected)
// pixel buffer used for skin-confidence evaluation.
//
// labBuf:  interleaved OKLab L,a,b for the full frame (W*H*3, modified in place)
// srcRGB:  original source RGB for skin-confidence evaluation
// srcOKLab: OKLab of original source for skin-confidence
// params:  must have skinEvenness and skinRadius set
// W, H:    full frame dimensions
//
// This function is tile-safe: it reads from srcRGB/srcOKLab (never from
// previously written labBuf output) and writes only labBuf.
// ---------------------------------------------------------------------------
static inline void spatial_skin_even(
    float* labBuf,                  // [W*H*3] OKLab L,a,b (in/out)
    const float* srcRGB,            // [W*H*4] original source RGBA
    const float* srcOKLab,          // [W*H*3] OKLab of original source
    const LaserLabParams& pr,
    int W, int H)
{
    const int radius = (int)(pr.skinRadius + 0.5f);
    if (radius < 1 || pr.skinEvenness <= 0.0f) return;

    // Pre-compute skin confidence for every source pixel
    std::vector<float> conf(W * H);
    for (int i = 0; i < W * H; i++) {
        conf[i] = skin_confidence(srcOKLab[i*3+0], srcOKLab[i*3+1], srcOKLab[i*3+2]);
    }

    // For each output pixel, average neighbor a/b weighted by:
    //   spatial Gaussian * luminance-edge rejection * neighbor skin confidence
    const float sigma_s = (float)radius * 0.6f;     // spatial Gaussian width
    const float inv_sigma_s2 = 1.0f / (sigma_s * sigma_s);

    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            const size_t ci = y * W + x;
            const float centerL = labBuf[ci*3+0];
            const float centerConf = conf[ci];
            if (centerConf < 0.01f) continue;                  // not skin -> skip

            float sum_a = 0.0f, sum_b = 0.0f, sum_w = 0.0f;

            for (int dy = -radius; dy <= radius; dy++) {
                const int ny = y + dy;
                if (ny < 0 || ny >= H) continue;
                for (int dx = -radius; dx <= radius; dx++) {
                    const int nx = x + dx;
                    if (nx < 0 || nx >= W) continue;

                    const size_t ni = ny * W + nx;
                    if (ni == ci) continue;                     // skip center

                    // Spatial Gaussian weight
                    const float dist2 = (float)(dx*dx + dy*dy);
                    const float w_spatial = expf(-0.5f * dist2 * inv_sigma_s2);

                    // Luminance-edge rejection: penalize large L differences
                    const float neighborL = labBuf[ni*3+0];
                    const float L_diff = fabsf(centerL - neighborL);
                    const float w_luma = expf(-L_diff * L_diff * 8.0f);

                    // Neighbor skin confidence
                    const float nconf = conf[ni];

                    const float w = w_spatial * w_luma * nconf;
                    sum_a += labBuf[ni*3+1] * w;
                    sum_b += labBuf[ni*3+2] * w;
                    sum_w += w;
                }
            }

            if (sum_w < 0.01f) continue;                       // negligible weight

            const float avg_a = sum_a / sum_w;
            const float avg_b = sum_b / sum_w;

            // Blend by skinEvenness * center confidence
            const float blend = pr.skinEvenness * centerConf;
            labBuf[ci*3+1] += (avg_a - labBuf[ci*3+1]) * blend;
            labBuf[ci*3+2] += (avg_b - labBuf[ci*3+2]) * blend;
            // L is preserved (not averaged)
        }
    }
}

// ---------------------------------------------------------------------------
// renderFrame — whole-frame CPU entry point (the GPU reference).
// Interleaved RGBA float, row-major, y up (OFX-native buffer order).
// Alpha passes through untouched.
//
// When skinEvenness > 0, performs a two-pass approach:
//   1. Point-wise transform all pixels to intermediate OKLab.
//   2. Spatial skin evenness on OKLab a/b.
//   3. Continue with density/output encoding from the averaged OKLab.
// ---------------------------------------------------------------------------
static inline void renderFrame(const float* src, int W, int H, const LaserLabParams& pr, float* dst)
{
    // Fast path: no spatial skin evenness
    if (pr.skinEvenness <= 0.0f || pr.skinRadius < 1) {
        for (int y = 0; y < H; ++y) {
            for (int x = 0; x < W; ++x) {
                const size_t i = (static_cast<size_t>(y) * W + x) * 4;
                const Vec3 o = transform(pr, src[i + 0], src[i + 1], src[i + 2]);
                dst[i + 0] = o.x; dst[i + 1] = o.y; dst[i + 2] = o.z; dst[i + 3] = src[i + 3];
            }
        }
        return;
    }

    // ---- Spatial skin evenness path ----
    // Step 1: Compute OKLab of original source (for skin-confidence)
    std::vector<float> srcOKLab(W * H * 3);
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            const size_t i = (static_cast<size_t>(y) * W + x) * 4;
            Vec3 lin = tfd(v3(src[i], src[i+1], src[i+2]), (int)pr.inputTransfer);
            Vec3 xyz = g2x((int)pr.inputGamut, lin);
            if ((int)pr.inputGamut == 8) xyz = d60_to_d65(xyz);
            // Use the OKLab of the linear Rec.2020 source (no WB/creative)
            Vec3 work = x2g(1, xyz);
            Vec3 lab = work_to_oklab(work);
            const size_t j = (static_cast<size_t>(y) * W + x) * 3;
            srcOKLab[j+0] = lab.x; srcOKLab[j+1] = lab.y; srcOKLab[j+2] = lab.z;
        }
    }

    // Step 2: Point-wise transform to intermediate OKLab (up to but not
    //         including density/output encoding). We use transform() for
    //         simplicity — the density/output encoding in transform() is
    //         cheap and we'll re-apply after spatial averaging.
    //         For correct spatial averaging, we need the OKLab BEFORE
    //         density.  Since transform() bakes density in, we call
    //         transform() on a version with zero density, then re-apply
    //         density separately.
    LaserLabParams pr_nodens = pr;
    pr_nodens.globalSat = 0.0f; pr_nodens.limSat = 0.0f; pr_nodens.limDens = 0.0f;
    pr_nodens.rDens = 0.0f; pr_nodens.yDens = 0.0f; pr_nodens.gDens = 0.0f;
    pr_nodens.cDens = 0.0f; pr_nodens.bDens = 0.0f; pr_nodens.mDens = 0.0f;

    std::vector<float> labBuf(W * H * 3);
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            const size_t i = (static_cast<size_t>(y) * W + x) * 4;
            const Vec3 o = transform(pr_nodens, src[i], src[i+1], src[i+2]);
            // o is linear AWG4 RGB (no density). Convert to OKLab for averaging.
            Vec3 lab = work_to_oklab(v3(o.x, o.y, o.z));
            const size_t j = (static_cast<size_t>(y) * W + x) * 3;
            labBuf[j+0] = lab.x; labBuf[j+1] = lab.y; labBuf[j+2] = lab.z;
        }
    }

    // Step 3: Spatial skin evenness
    spatial_skin_even(labBuf.data(), src, srcOKLab.data(), pr, W, H);

    // Step 4: Re-encode: OKLab -> linear AWG4 -> density -> output gamut/TF
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            const size_t i = (static_cast<size_t>(y) * W + x) * 4;
            const size_t j = (static_cast<size_t>(y) * W + x) * 3;
            Vec3 lab = v3(labBuf[j+0], labBuf[j+1], labBuf[j+2]);
            Vec3 out = oklab_to_work(lab);

            // Apply density (copied from transform, same as the density-only path)
            float c_gsat = clampf(pr.globalSat, -1.0f, 1.0f);
            float c_limsat = clampf(pr.limSat, 0.0f, 1.0f);
            float c_limdens = clampf(pr.limDens, 0.0f, 1.0f);
            float c_rd = clampf(pr.rDens, 0.0f, 1.0f);
            float c_yd = clampf(pr.yDens, 0.0f, 1.0f);
            float c_gd = clampf(pr.gDens, 0.0f, 1.0f);
            float c_cd = clampf(pr.cDens, 0.0f, 1.0f);
            float c_bd = clampf(pr.bDens, 0.0f, 1.0f);
            float c_md = clampf(pr.mDens, 0.0f, 1.0f);
            float d_asat = (c_limsat > 0.0f) ? limiter(rgb_sat(out), c_limsat) : 1.0f;
            float d_aden = (c_limdens > 0.0f) ? limiter(rgb_sat(out), c_limdens) : 1.0f;
            float d_rd2 = -(c_rd*d_aden); float d_gd2 = -(c_gd*d_aden); float d_bd2 = -(c_bd*d_aden);
            float d_cd2 = -(c_cd*d_aden*0.5f); float d_md2 = -(c_md*d_aden*0.5f); float d_yd2 = -(c_yd*d_aden*0.5f);
            float d_gs2 = c_gsat*d_asat;
            // Mononodes-style tetra density (same structure as transform)
            Vec3 d_blk = v3(0,0,0), d_wht = v3(1,1,1);
            Vec3 d_rv = v3(d_rd2+1, d_rd2-d_gs2, d_rd2-d_gs2);
            Vec3 d_gv = v3(d_gd2-d_gs2, d_gd2+1, d_gd2-d_gs2);
            Vec3 d_bv = v3(d_bd2-d_gs2, d_bd2-d_gs2, d_bd2+1);
            Vec3 d_cv = v3(d_cd2-d_gs2, d_cd2+1, d_cd2+1);
            Vec3 d_mv = v3(d_md2+1, d_md2-d_gs2, d_md2+1);
            Vec3 d_yv = v3(d_yd2+1, d_yd2+1, d_yd2-d_gs2);
            float d_Rv=out.x, d_Gv=out.y, d_Bv=out.z;
            Vec3 d_nrgb;
            if (d_Rv > d_Gv) {
                if (d_Gv > d_Bv) { d_nrgb=v3(d_Rv*(d_rv.x-d_blk.x)+d_blk.x+d_Gv*(d_yv.x-d_rv.x)+d_Bv*(d_wht.x-d_yv.x), d_Rv*(d_rv.y-d_blk.y)+d_blk.y+d_Gv*(d_yv.y-d_rv.y)+d_Bv*(d_wht.y-d_yv.y), d_Rv*(d_rv.z-d_blk.z)+d_blk.z+d_Gv*(d_yv.z-d_rv.z)+d_Bv*(d_wht.z-d_yv.z)); }
                else if (d_Rv > d_Bv) { d_nrgb=v3(d_Rv*(d_rv.x-d_blk.x)+d_blk.x+d_Gv*(d_wht.x-d_mv.x)+d_Bv*(d_mv.x-d_rv.x), d_Rv*(d_rv.y-d_blk.y)+d_blk.y+d_Gv*(d_wht.y-d_mv.y)+d_Bv*(d_mv.y-d_rv.y), d_Rv*(d_rv.z-d_blk.z)+d_blk.z+d_Gv*(d_wht.z-d_mv.z)+d_Bv*(d_mv.z-d_rv.z)); }
                else { d_nrgb=v3(d_Rv*(d_mv.x-d_bv.x)+d_Gv*(d_wht.x-d_mv.x)+d_Bv*(d_bv.x-d_blk.x), d_Rv*(d_mv.y-d_bv.y)+d_Gv*(d_wht.y-d_mv.y)+d_Bv*(d_bv.y-d_blk.y), d_Rv*(d_mv.z-d_bv.z)+d_Gv*(d_wht.z-d_mv.z)+d_Bv*(d_bv.z-d_blk.z)); }
            } else {
                if (d_Bv > d_Gv) { d_nrgb=v3(d_Rv*(d_wht.x-d_cv.x)+d_Gv*(d_cv.x-d_bv.x)+d_Bv*(d_bv.x-d_blk.x), d_Rv*(d_wht.y-d_cv.y)+d_Gv*(d_cv.y-d_bv.y)+d_Bv*(d_bv.y-d_blk.y), d_Rv*(d_wht.z-d_cv.z)+d_Gv*(d_cv.z-d_bv.z)+d_Bv*(d_bv.z-d_blk.z)); }
                else if (d_Bv > d_Rv) { d_nrgb=v3(d_Rv*(d_wht.x-d_cv.x)+d_Gv*(d_gv.x-d_blk.x)+d_Bv*(d_cv.x-d_gv.x), d_Rv*(d_wht.y-d_cv.y)+d_Gv*(d_gv.y-d_blk.y)+d_Bv*(d_cv.y-d_gv.y), d_Rv*(d_wht.z-d_cv.z)+d_Gv*(d_gv.z-d_blk.z)+d_Bv*(d_cv.z-d_gv.z)); }
                else { d_nrgb=v3(d_Rv*(d_yv.x-d_gv.x)+d_Gv*(d_gv.x-d_blk.x)+d_Bv*(d_wht.x-d_yv.x), d_Rv*(d_yv.y-d_gv.y)+d_Gv*(d_gv.y-d_blk.y)+d_Bv*(d_wht.y-d_yv.y), d_Rv*(d_yv.z-d_gv.z)+d_Gv*(d_gv.z-d_blk.z)+d_Bv*(d_wht.z-d_yv.z)); }
            }
            out = d_nrgb;

            // Reverse: linear AWG4 -> destination gamut -> native TF
            Vec3 rxyz = g2x(1, out);
            int g = (int)fminf(fmaxf((float)(pr.inputGamut + 0.5f), 0.0f), 10.0f);
            if (g == 8) rxyz = d65_to_d60(rxyz);
            Vec3 cam = x2g(g, rxyz);
            cam = compress_to_gamut(cam, rxyz.y);
            int ti_tf = (int)fminf(fmaxf((float)(pr.inputTransfer + 0.5f), 0.0f), 14.0f);
            Vec3 result = tfe(cam, ti_tf);
            float nx = (result.x == result.x && fabsf(result.x) < 3.402823e38f) ? result.x : 0.0f;
            float ny = (result.y == result.y && fabsf(result.y) < 3.402823e38f) ? result.y : 0.0f;
            float nz = (result.z == result.z && fabsf(result.z) < 3.402823e38f) ? result.z : 0.0f;
            dst[i+0] = nx; dst[i+1] = ny; dst[i+2] = nz; dst[i+3] = src[i+3];
        }
    }
}

// ---------------------------------------------------------------------------
// solveSkinTempTint — given the mean measured skin as OKLab (L,a,b), find
// CAT16 temp/tint (white_balance sliders) that rotate that skin's hue onto
// the skin-tone line (1.0685 rad). Faithful to the plugin's own transform:
// mean skin -> OKLab -> linear AWG4 -> XYZ(D65) -> white_balance(_) -> AWG4
// -> OKLab, minimising angular distance to the skin line plus a tiny L2
// regulariser on the sliders (picks the smallest correction).
// ---------------------------------------------------------------------------
static inline void solveSkinTempTint(float L, float a, float b,
                                     float& best_temp, float& best_tint) {
    // Reconstruct the mean skin as linear AWG4 (the working container).
    const Vec3 work = oklab_to_work(v3(L, a, b));
    const Vec3 xyz_src = g2x(1, work);   // XYZ D65, pre-WB

    float best_err = 1e10f;
    best_temp = 0.0f; best_tint = 0.0f;

    auto eval = [&](float temp, float tint) {
        const Vec3 xyz = white_balance(xyz_src, temp, tint);
        const Vec3 lab = work_to_oklab(x2g(1, xyz));
        const float c = sqrtf(lab.y * lab.y + lab.z * lab.z);
        if (c < 1e-6f) return 1e10f;
        const float ch = atan2f(lab.z, lab.y);
        const float herr = fabsf(atan2f(sinf(1.0685f - ch), cosf(1.0685f - ch)));
        return herr + 0.0025f * (temp * temp + tint * tint);
    };
    for (int ti = 0; ti < 33; ti++) {
        const float temp = -1.0f + 2.0f * ti / 32.0f;
        for (int tj = 0; tj < 33; tj++) {
            const float tint = -0.5f + 1.0f * tj / 32.0f;
            const float score = eval(temp, tint);
            if (score < best_err) { best_err = score; best_temp = temp; best_tint = tint; }
        }
    }
    const float fstep = 2.0f / 32.0f / 3.0f;
    const float ft_lo = best_temp - fstep * 3, ft_hi = best_temp + fstep * 3;
    const float fi_lo = best_tint - fstep * 3, fi_hi = best_tint + fstep * 3;
    for (int ti = 0; ti < 21; ti++) {
        const float temp = ft_lo + (ft_hi - ft_lo) * ti / 20.0f;
        for (int tj = 0; tj < 21; tj++) {
            const float tint = fi_lo + (fi_hi - fi_lo) * tj / 20.0f;
            const float score = eval(temp, tint);
            if (score < best_err) { best_err = score; best_temp = temp; best_tint = tint; }
        }
    }
    if (fabsf(best_temp) < 1e-4f && fabsf(best_tint) < 1e-4f) { best_temp = 0.0f; best_tint = 0.0f; }
}

// ---------------------------------------------------------------------------
// analyzeAutoWb — measure skin from pixels, solve minimum CAT16
// correction to place measured skin on the skin-tone line (1.0685 rad).
// Returns the LMS gains to apply globally, or identity if < 32 samples.
// ---------------------------------------------------------------------------
struct AutoWbResult {
    bool valid;
    int sampleCount;
    float solvedTemp;
    float solvedTint;
    Vec3 lmsGain;
};

static inline AutoWbResult analyzeAutoWb(
    const float* src, int W, int H, const LaserLabParams& pr)
{
    AutoWbResult r = {false, 0, 0.0f, 0.0f, v3(1.0f, 1.0f, 1.0f)};

    // Sample at most 128x72 grid positions
    const int gridW = (W < 128) ? W : 128;
    const int gridH = (H < 72) ? H : 72;
    const float stepX = (float)W / (float)gridW;
    const float stepY = (float)H / (float)gridH;

    // Collect skin samples (OKLab of linear Rec.2020, no WB or creative)
    static const int MAX_SAMPLES = 128 * 72;
    std::vector<float> samples;     // interleaved L, a, b
    samples.reserve(MAX_SAMPLES * 3);

    int g = (int)fminf(fmaxf((float)(pr.inputGamut + 0.5f), 0.0f), 10.0f);
    int ti_tf = (int)fminf(fmaxf((float)(pr.inputTransfer + 0.5f), 0.0f), 14.0f);

    for (int gy = 0; gy < gridH; gy++) {
        for (int gx = 0; gx < gridW; gx++) {
            const int px = (int)((gx + 0.5f) * stepX);
            const int py = (int)((gy + 0.5f) * stepY);
            if (px >= W || py >= H) continue;
            const size_t i = (static_cast<size_t>(py) * W + px) * 4;

            Vec3 lin = tfd(v3(src[i], src[i+1], src[i+2]), ti_tf);
            Vec3 xyz = g2x(g, lin);
            if (g == 8) xyz = d60_to_d65(xyz);
            Vec3 work = x2g(1, xyz);
            Vec3 lab = work_to_oklab(work);

            float c = sqrtf(lab.y*lab.y + lab.z*lab.z);
            float hue = (c > 0.0f) ? mod2(atan2f(lab.z, lab.y), 2.0f*PI) : 0.0f;
            float hue_align = cosf(hue - 1.0685f);
            float L_w = smootherstep(0.15f, 0.30f, lab.x) * (1.0f - smootherstep(0.80f, 1.05f, lab.x));
            float c_w = smootherstep(0.02f, 0.06f, c) * (1.0f - smootherstep(0.30f, 0.55f, c));
            float h_w = smootherstep(0.4f, 0.9f, hue_align);
            float w = L_w * c_w * h_w;
            if (w > 0.05f) {
                samples.push_back(lab.x); samples.push_back(lab.y); samples.push_back(lab.z);
            }
        }
    }

    if ((int)samples.size() < 32 * 3) return r;   // not enough skin

    // Compute chroma-weighted mean (L included so the solver can reconstruct
    // the mean skin color and solve through the real WB path).
    float sum_a = 0, sum_b = 0, sum_L = 0, sum_w = 0;
    for (size_t i = 0; i < samples.size(); i += 3) {
        float c = sqrtf(samples[i+1]*samples[i+1] + samples[i+2]*samples[i+2]);
        sum_a += samples[i+1] * c;
        sum_b += samples[i+2] * c;
        sum_L += samples[i+0] * c;
        sum_w += c;
    }
    if (sum_w < 1e-6f) return r;
    float mean_a = sum_a / sum_w, mean_b = sum_b / sum_w;
    float mean_L = sum_L / sum_w;

    // Solve temp/tint (CAT16) that rotates skin hue onto the skin-tone line.
    float best_temp = 0, best_tint = 0;
    solveSkinTempTint(mean_L, mean_a, mean_b, best_temp, best_tint);

    // Compute the XYZ->CAT16 LMS gains for this temp/tint
    // The gain is the ratio of corrected D65 white to uncorrected D65 white in LMS
    Vec3 d65_xyz = v3(WHITE_D65[0], WHITE_D65[1], WHITE_D65[2]);
    Vec3 corrected_white = white_balance(d65_xyz, best_temp, best_tint);
    Vec3 lms_src = mmul(CAT16_c0, CAT16_c1, CAT16_c2, d65_xyz);
    Vec3 lms_dst = mmul(CAT16_c0, CAT16_c1, CAT16_c2, corrected_white);
    Vec3 gain = v3(lms_dst.x / lms_src.x, lms_dst.y / lms_src.y, lms_dst.z / lms_src.z);

    r.valid = true;
    r.sampleCount = (int)samples.size() / 3;
    r.solvedTemp = best_temp;
    r.solvedTint = best_tint;
    r.lmsGain = gain;
    return r;
}

} // namespace laserlabcore

#endif // LASERCLAB_CORE_H
