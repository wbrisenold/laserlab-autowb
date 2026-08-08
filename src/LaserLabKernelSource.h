typedef struct LaserLabParams {
    int   inputGamut;
    int   inputTransfer;
    float temp;
    float tint;
    float plMaster;
    float plRed;
    float plGreen;
    float plBlue;
    float lift;
    float gamma;
    float gain;
    float contrast;
    float shadows;
    float highlights;
    float desatHigh;
    float midDetail;
    float expGlobal;
    float expFlash;
    float satAmount;
    float satCompression;
    float hueDeg;
    float skinHueDeg;
    float skinSat;
    float skinVal;
    float skinEvenness;
    float globalSat;
    float limSat;
    float limDens;
    float rDens;
    float yDens;
    float gDens;
    float cDens;
    float bDens;
    float mDens;
    int   _pad0;
} LaserLabParams;


// ---- Matrix constants (float3, column-major like DCTL) ----

// CAT16I
static const float3 CAT16I_c0 = float3(1.86206786f, 0.38752654f, -0.01584150f);
static const float3 CAT16I_c1 = float3(-1.01125463f, 0.62144744f, -0.03412294f);
static const float3 CAT16I_c2 = float3(0.14918677f, -0.00897398f, 1.04996444f);

// CAT16
static const float3 CAT16_c0 = float3(0.401288f, -0.250268f, -0.002079f);
static const float3 CAT16_c1 = float3(0.650173f, 1.204414f, 0.048952f);
static const float3 CAT16_c2 = float3(-0.051461f, 0.045854f, 0.953127f);

// G0
static const float3 G0_c0 = float3(3.2409699419f, -0.9692436363f, 0.0556300797f);
static const float3 G0_c1 = float3(-1.5373831776f, 1.8759675015f, -0.2039769589f);
static const float3 G0_c2 = float3(-0.4986107603f, 0.0415550574f, 1.0569715142f);

// G10
static const float3 G10_c0 = float3(1.7166511880f, -0.6666843518f, 0.0176398574f);
static const float3 G10_c1 = float3(-0.3556707838f, 1.6164812366f, -0.0427706133f);
static const float3 G10_c2 = float3(-0.2533662814f, 0.0157685458f, 0.9421031212f);

// G1
static const float3 G1_c0 = float3(1.5092154722f, -0.4915454517f, 0.0000000000f);
static const float3 G1_c1 = float3(-0.2505973452f, 1.3612455459f, 0.0000000000f);
static const float3 G1_c2 = float3(-0.1688114753f, 0.0972829420f, 0.9182249512f);

// G2
static const float3 G2_c0 = float3(1.7890660000f, -0.6398490000f, -0.0415320000f);
static const float3 G2_c1 = float3(-0.4825340000f, 1.3964000000f, 0.0823350000f);
static const float3 G2_c2 = float3(-0.2000760000f, 0.1944320000f, 0.8788680000f);

// G3
static const float3 G3_c0 = float3(1.5073998991f, -0.5181517271f, 0.0155116982f);
static const float3 G3_c1 = float3(-0.2458221374f, 1.3553912409f, -0.0078727714f);
static const float3 G3_c2 = float3(-0.1716116808f, 0.1258786682f, 0.9119163656f);

// G4
static const float3 G4_c0 = float3(1.5890120000f, -0.5340530000f, 0.0111790000f);
static const float3 G4_c1 = float3(-0.3132040000f, 1.3960110000f, 0.0031940000f);
static const float3 G4_c2 = float3(-0.1809650000f, 0.1024580000f, 0.9055350000f);

// G5
static const float3 G5_c0 = float3(1.4128066123f, -0.4862031858f, -0.0371387758f);
static const float3 G5_c1 = float3(-0.1775223662f, 1.2906962108f, 0.2863757596f);
static const float3 G5_c2 = float3(-0.1517703764f, 0.1574002837f, 0.6876796053f);

// G6
static const float3 G6_c0 = float3(1.8663823404f, -0.6003424925f, 0.0024519937f);
static const float3 G6_c1 = float3(-0.5183973429f, 1.3781489625f, 0.0863996742f);
static const float3 G6_c2 = float3(-0.2346098094f, 0.1767318303f, 0.8369427073f);

// G7
static const float3 G7_c0 = float3(1.4898182749f, -0.4581665745f, -0.0703496677f);
static const float3 G7_c1 = float3(-0.2608959022f, 1.2616277831f, 0.2215576672f);
static const float3 G7_c2 = float3(-0.1424265218f, 0.1596236316f, 0.7761816036f);

// G8
static const float3 G8_c0 = float3(1.6410233797f, -0.6636628587f, 0.0117218943f);
static const float3 G8_c1 = float3(-0.3248032942f, 1.6153315917f, -0.0082844420f);
static const float3 G8_c2 = float3(-0.2364246952f, 0.0167563477f, 0.9883948585f);

// G9
static const float3 G9_c0 = float3(1.7257000000f, -0.6025000000f, -0.0156000000f);
static const float3 G9_c1 = float3(-0.4314000000f, 1.3906000000f, 0.0905000000f);
static const float3 G9_c2 = float3(-0.1917000000f, 0.1671000000f, 0.8489000000f);

// I0
static const float3 I0_c0 = float3(3.2409699419f, -0.9692436363f, 0.0556300797f);
static const float3 I0_c1 = float3(-1.5373831776f, 1.8759675015f, -0.2039769589f);
static const float3 I0_c2 = float3(-0.4986107603f, 0.0415550574f, 1.0569715142f);

// L
static const float3 L_c0 = float3(0.4122214708f, 0.2119034982f, 0.0883024619f);
static const float3 L_c1 = float3(0.5363325363f, 0.6806995451f, 0.2817188376f);
static const float3 L_c2 = float3(0.0514459929f, 0.1073969566f, 0.6299787005f);

// O
static const float3 O_c0 = float3(0.2104542553f, 1.9779984951f, 0.0259040371f);
static const float3 O_c1 = float3(0.793617785f, -2.428592205f, 0.7827717662f);
static const float3 O_c2 = float3(-0.0040720468f, 0.4505937099f, -0.808675766f);

// P
static const float3 P_c0 = float3(1.0f, 1.0f, 1.0f);
static const float3 P_c1 = float3(0.3963377774f, -0.1055613458f, -0.0894841775f);
static const float3 P_c2 = float3(0.2158037573f, -0.0638541728f, -1.291485548f);

// Q
static const float3 Q_c0 = float3(4.0767416621f, -1.2684380046f, -0.0041960866f);
static const float3 Q_c1 = float3(-3.3077115901f, 2.6097574011f, -0.7034186145f);
static const float3 Q_c2 = float3(0.2309699292f, -0.3413193965f, 1.7076147014f);

// R0
static const float3 R0_c0 = float3(0.4123907993f, 0.2126390059f, 0.0193308187f);
static const float3 R0_c1 = float3(0.3575843394f, 0.7151686788f, 0.1191947798f);
static const float3 R0_c2 = float3(0.1804807884f, 0.0721923154f, 0.9505321522f);

// R10
static const float3 R10_c0 = float3(0.6369580483f, 0.2627002120f, 0.0000000000f);
static const float3 R10_c1 = float3(0.1446169036f, 0.6779980715f, 0.0280726930f);
static const float3 R10_c2 = float3(0.1688809752f, 0.0593017165f, 1.0609850577f);

// R1
static const float3 R1_c0 = float3(0.7048583204f, 0.2545241764f, 0.0f);
static const float3 R1_c1 = float3(0.1297602952f, 0.7814777327f, 0.0f);
static const float3 R1_c2 = float3(0.1158373115f, -0.0360019091f, 1.0890577508f);

// R2
static const float3 R2_c0 = float3(0.638008f, 0.291954f, 0.002798f);
static const float3 R2_c1 = float3(0.214704f, 0.823841f, -0.067034f);
static const float3 R2_c2 = float3(0.097744f, -0.115795f, 1.153294f);

// R3
static const float3 R3_c0 = float3(0.7064827132f, 0.2709796708f, -0.0096778454f);
static const float3 R3_c1 = float3(0.1288010498f, 0.7866064112f, 0.0046000375f);
static const float3 R3_c2 = float3(0.1151721641f, -0.057586082f, 1.0941355587f);

// R4
static const float3 R4_c0 = float3(0.679644f, 0.260686f, -0.00931f);
static const float3 R4_c1 = float3(0.152211f, 0.774894f, -0.004612f);
static const float3 R4_c2 = float3(0.1186f, -0.03558f, 1.10298f);

// R5
static const float3 R5_c0 = float3(0.735275f, 0.286694f, -0.079681f);
static const float3 R5_c1 = float3(0.068609f, 0.842979f, -0.347343f);
static const float3 R5_c2 = float3(0.146571f, -0.129673f, 1.516082f);

// R6
static const float3 R6_c0 = float3(0.6065303722f, 0.2679894071f, -0.029442166f);
static const float3 R6_c1 = float3(0.2204080953f, 0.8327308786f, -0.086610607f);
static const float3 R6_c2 = float3(0.1234790005f, -0.1007202857f, 1.2048607644f);

// R7
static const float3 R7_c0 = float3(0.7160496466f, 0.2612613575f, -0.0096763466f);
static const float3 R7_c1 = float3(0.1296834779f, 0.8696421458f, -0.2364816361f);
static const float3 R7_c2 = float3(0.1047228026f, -0.1309035033f, 1.3352157335f);

// R8
static const float3 R8_c0 = float3(0.6624541811f, 0.2722287168f, -0.0055746495f);
static const float3 R8_c1 = float3(0.1340042065f, 0.6740817658f, 0.0040607335f);
static const float3 R8_c2 = float3(0.156187687f, 0.0536895174f, 1.0103391003f);

// R9
static const float3 R9_c0 = float3(0.6482f, 0.283f, -0.0183f);
static const float3 R9_c1 = float3(0.194f, 0.8132f, -0.0832f);
static const float3 R9_c2 = float3(0.1082f, -0.0962f, 1.1903f);

// S0
static const float3 S0_c0 = float3(0.4123907993f, 0.2126390059f, 0.0193308187f);
static const float3 S0_c1 = float3(0.3575843394f, 0.7151686788f, 0.1191947798f);
static const float3 S0_c2 = float3(0.1804807884f, 0.0721923154f, 0.9505321522f);

static const float3 WHITE_D65 = float3(0.9504559271ff, 1.0ff, 1.0890577508ff);
static const float3 WHITE_D60 = float3(0.9526460746ff, 1.0ff, 1.0088251844ff);

// ---- Scalar constants ----

static const float LOGC3_CUT = 0.010591f;
static const float LOGC3_A = 5.555556f;
static const float LOGC3_B = 0.052272f;
static const float LOGC3_C = 0.24719f;
static const float LOGC3_D = 0.385537f;
static const float LOGC3_E = 5.367655f;
static const float LOGC3_F = 0.092809f;
static const float LOGC4_A = 2231.8263090676883f;
static const float LOGC4_B = 0.9071358748778103f;
static const float LOGC4_C = 0.09286412512218964f;
static const float LOGC4_S2 = 0.1135972086105891f;
static const float LOGC4_T = -0.01805699611991131f;
static const float APPLE_R0 = -0.05641088f;
static const float APPLE_RT = 0.01f;
static const float APPLE_SIGMA = 47.28711236f;
static const float APPLE_BETA = 0.00964052f;
static const float APPLE_GAMMA = 0.08550479f;
static const float APPLE_DELTA = 0.69336945f;
static const float EXPO_FLASH_SCALE = 0.1f;
static const float PI = 3.141592653589793238f;

// ---- Helpers (float3, MSL/OpenCL math) ----
static inline float clampf(float v, float lo, float hi) { return fmin(hi, fmax(lo, v)); }
static inline float chroma_lab(float3 lab) { return sqrt(lab.y*lab.y + lab.z*lab.z); }
static inline float3 mmul3(const float3 c0, const float3 c1, const float3 c2, const float3 v) {
    return float3(c0.x*v.x + c1.x*v.y + c2.x*v.z,
                  c0.y*v.x + c1.y*v.y + c2.y*v.z,
                  c0.z*v.x + c1.z*v.y + c2.z*v.z);
}
static inline float cbrt_s(float x) { return (x < 0.0f ? -1.0f : 1.0f) * pow(fabs(x), 1.0f/3.0f); }
static inline float spowf(float x, float p) { float y = pow(fabs(x), p); return (x < 0.0f) ? -y : y; }
static inline float3 cat16_adapt(const float3 xyz, const float3 src_w, const float3 dst_w) {
    float3 lms = mmul3(CAT16_c0, CAT16_c1, CAT16_c2, xyz);
    float3 src = mmul3(CAT16_c0, CAT16_c1, CAT16_c2, src_w);
    float3 dst = mmul3(CAT16_c0, CAT16_c1, CAT16_c2, dst_w);
    return mmul3(CAT16I_c0, CAT16I_c1, CAT16I_c2, float3(lms.x*dst.x/src.x, lms.y*dst.y/src.y, lms.z*dst.z/src.z));
}
static inline float3 d60_to_d65(const float3 xyz) { return cat16_adapt(xyz, WHITE_D60, WHITE_D65); }
static inline float3 d65_to_d60(const float3 xyz) { return cat16_adapt(xyz, WHITE_D65, WHITE_D60); }
static inline float3 white_balance(const float3 xyz, float temp_value, float tint_value) {
    if (fabs(temp_value) < 1e-6f && fabs(tint_value) < 1e-6f) return xyz;
    float k = 6504.0f * exp(temp_value * 1.0f);
    float k2 = k*k, k3 = k2*k;
    float x = k <= 4000.0f ? -0.2661239e9f/k3 - 0.2343580e6f/k2 + 0.8776956e3f/k + 0.179910f
                           : -3.0258469e9f/k3 + 2.1070379e6f/k2 + 0.2226347e3f/k + 0.240390f;
    float y = k <= 4000.0f ? -1.1063814f*x*x*x - 1.34811020f*x*x + 2.18555832f*x - 0.20219683f
                           : 3.0817580f*x*x*x - 5.8733867f*x*x + 3.75112997f*x - 0.37001483f;
    y = fmax(y + tint_value*0.025f, 0.05f);
    float3 sw = float3(x/y, 1.0f, (1.0f - x - y)/y);
    return cat16_adapt(xyz, sw, WHITE_D65);
}
static inline float3 printer_lights(const float3 rgb, float master, float red, float green, float blue) {
    float m = master - 25.0f;
    float rg = pow(10.0f, (m + red  - 25.0f) * 0.0125f);
    float gg = pow(10.0f, (m + green- 25.0f) * 0.0125f);
    float bg = pow(10.0f, (m + blue - 25.0f) * 0.0125f);
    return float3(rgb.x*rg, rgb.y*gg, rgb.z*bg);
}
static inline float3 g2x(int g, const float3 v) {
    if (g == 0)  return mmul3(R0_c0,R0_c1,R0_c2,v);
    if (g == 1)  return mmul3(R1_c0,R1_c1,R1_c2,v);
    if (g == 2)  return mmul3(R2_c0,R2_c1,R2_c2,v);
    if (g == 3)  return mmul3(R3_c0,R3_c1,R3_c2,v);
    if (g == 4)  return mmul3(R4_c0,R4_c1,R4_c2,v);
    if (g == 5)  return mmul3(R5_c0,R5_c1,R5_c2,v);
    if (g == 6)  return mmul3(R6_c0,R6_c1,R6_c2,v);
    if (g == 7)  return mmul3(R7_c0,R7_c1,R7_c2,v);
    if (g == 8)  return mmul3(R8_c0,R8_c1,R8_c2,v);
    if (g == 9)  return mmul3(R9_c0,R9_c1,R9_c2,v);
    return mmul3(R10_c0,R10_c1,R10_c2,v);
}
static inline float3 x2g(int g, const float3 v) {
    if (g == 0)  return mmul3(G0_c0,G0_c1,G0_c2,v);
    if (g == 1)  return mmul3(G1_c0,G1_c1,G1_c2,v);
    if (g == 2)  return mmul3(G2_c0,G2_c1,G2_c2,v);
    if (g == 3)  return mmul3(G3_c0,G3_c1,G3_c2,v);
    if (g == 4)  return mmul3(G4_c0,G4_c1,G4_c2,v);
    if (g == 5)  return mmul3(G5_c0,G5_c1,G5_c2,v);
    if (g == 6)  return mmul3(G6_c0,G6_c1,G6_c2,v);
    if (g == 7)  return mmul3(G7_c0,G7_c1,G7_c2,v);
    if (g == 8)  return mmul3(G8_c0,G8_c1,G8_c2,v);
    if (g == 9)  return mmul3(G9_c0,G9_c1,G9_c2,v);
    return mmul3(G10_c0,G10_c1,G10_c2,v);
}
// ---------------------------------------------------------------------------
// llTransform — per-pixel grade, faithful port of DCTL transform().
// ---------------------------------------------------------------------------
static inline float3 llTransform(const LaserLabParams& pr, float r, float g, float b) {

  // Clamp all parameters to safe ranges (keyframes can bypass slider limits).
  // Ranges mirror the DEFINE_UI_PARAMS UI bounds.
  float c_temp=clampf(pr.temp,-2.0f,2.0f), c_tint=clampf(pr.tint,-1.5f,1.5f);
  float c_lift=clampf(pr.lift,-0.5f,0.5f);
  float c_gamma=clampf(pr.gamma,0.2f,3.0f), c_gain=clampf(pr.gain,0.0f,4.0f), c_contrast=clampf(pr.contrast,0.5f,1.5f);
  float c_shadows=clampf(pr.shadows,-1.0f,1.0f), c_highlights=clampf(pr.highlights,-1.0f,1.0f);
  float c_exp=clampf(pr.expGlobal,-5.0f,5.0f), c_flash=clampf(pr.expFlash,-2.0f,2.0f);
  float c_sat=clampf(pr.satAmount,0.0f,3.0f), c_hue=clampf(pr.hueDeg,-180.0f,180.0f);
  float c_satcomp=clampf(pr.satCompression,0.0f,1.0f);
  float c_desat_hi=clampf(pr.desatHigh,0.0f,1.0f), c_mid=clampf(pr.midDetail,0.0f,1.0f);
  float c_skinH=clampf(pr.skinHueDeg,-45.0f,45.0f), c_skinS=clampf(pr.skinSat,0.0f,2.0f);
  float c_skinV=clampf(pr.skinVal,-0.5f,0.5f), c_skinE=clampf(pr.skinEvenness,0.0f,1.0f);
  float c_gsat=clampf(pr.globalSat,-1.0f,1.0f);
  float c_limsat=clampf(pr.limSat,0.0f,1.0f), c_limdens=clampf(pr.limDens,0.0f,1.0f);
  float c_rd=clampf(pr.rDens,0.0f,1.0f), c_yd=clampf(pr.yDens,0.0f,1.0f), c_gd=clampf(pr.gDens,0.0f,1.0f);
  float c_cd=clampf(pr.cDens,0.0f,1.0f), c_bd=clampf(pr.bDens,0.0f,1.0f), c_md=clampf(pr.mDens,0.0f,1.0f);
  float c_plm=clampf(pr.plMaster,0.0f,50.0f), c_plr=clampf(pr.plRed,0.0f,50.0f), c_plg=clampf(pr.plGreen,0.0f,50.0f), c_plb=clampf(pr.plBlue,0.0f,50.0f);

  int g=fmin(fmax((int)(pr.inputGamut+0.5f),0),10);
  int ti_tf=fmin(fmax((int)(pr.inputTransfer+0.5f),0),14);
  float3 r=float3(p_R,p_G,p_B);

  // Forward: native decode -> XYZ D65 -> CAT16 white balance -> linear AWG4.
  float3 lin=tfd(r,ti_tf);
  float3 xyz=g2x(g,lin);
  if(g==8)xyz=d60_to_d65(xyz);
  xyz=white_balance(xyz,c_temp,c_tint);
  float3 work=x2g(1,xyz);
  work=printer_lights(work,c_plm,c_plr,c_plg,c_plb);
  // Scene-linear exposure -> flash -> master pr.gain, all before the OKLab grade.
  work*=float3(pow(2.0f,c_exp),pow(2.0f,c_exp),pow(2.0f,c_exp));
  work.x+=c_flash*EXPO_FLASH_SCALE;work.y+=c_flash*EXPO_FLASH_SCALE;work.z+=c_flash*EXPO_FLASH_SCALE;
  work*=float3(c_gain,c_gain,c_gain);

  // Grade in OKLab
  float3 lab=work_to_oklab(work);
  float L=lab.x;

  // Global OKLab saturation/pr.hueDeg
  float av=lab.y*c_sat,bv=lab.z*c_sat;
  float ang=c_hue*3.14159265359f/180.0f,ca=cos(ang),sak=sin(ang);
  lab.y=av*ca-bv*sak;lab.z=av*sak+bv*ca;
  lab=compress_oklab_chroma(lab,c_satcomp);

  // Tonal: pr.lift (shadow-biased), pr.gamma, pr.contrast S-curve, all on perceptual OKLab L.
  const float OKLAB_MIDGRAY=0.565f;
  float liftMask=1.0f-smootherstep(0.0f,0.7f,L);
  L+=c_lift*liftMask;L=spowf(L,1.0f/c_gamma);
  // Pivot-centered pr.contrast: L = pivot * (L/pivot)^e. e>1 pushes both halves away from
  // middle gray (more pr.contrast); e<1 pulls toward it. No fixed point at L=1, so extended
  // pr.highlights keep spreading rather than collapsing back. spowf keeps signed values.
  if(fabs(c_contrast-1.0f)>1e-4f){
    float pivot=OKLAB_MIDGRAY;
    L=pivot*spowf(L/pivot,c_contrast);
  }
  L=shape_tone_oklab(L,c_shadows,c_highlights);  lab.x=L;

  // Desat pr.highlights: amount = max chroma reduction. fades in ~L 0.80->1.15.
  if(c_desat_hi>0.001f){
    float hiW=smootherstep(0.80f,1.15f,L);
    float cs=1.0f-c_desat_hi*hiW;
    lab.y*=cs;lab.z*=cs;
  }

  // Mid Chroma: boost OKLab chroma in a smooth midtone lightness band.
  if(c_mid>0.001f){
    float mdMask=smootherstep(0.2f,0.4f,L)*(1.0f-smootherstep(0.6f,0.8f,L));
    float md=1.0f+c_mid*mdMask*0.3f;
    lab.y*=md;lab.z*=md;
  }

  // Skin/warm axis (tetra, from thatcherfreeman/SmoothTetraSkin) in OKLab a/b pr.hueDeg space.
  // Broad lightness weighting so very dark/bright objects are less affected.
  // Skin/warm lightness gate: dark fades in->full mids->fades out bright.
  float sk_dg=smootherstep(0.02f,0.18f,L),sk_bg=1.0f-smootherstep(1.0f,1.5f,L);
  float sk_lw=sk_dg*sk_bg;
  float sk_ri[7]={0.5102284f,1.0685f,1.9158353f,2.4870127f,3.3993594f,4.6085770f,5.7310226f};
  float sk_hi[7]={0.0f,deg_to_rad(c_skinH),0.0f,0.0f,0.0f,0.0f,0.0f};
  float sk_si[7]={1.0f,c_skinS,1.0f,1.0f,1.0f,1.0f,1.0f};
  float sk_vi[7]={0.0f,c_skinV,0.0f,0.0f,0.0f,0.0f,0.0f};
  tp_t sk_p=mk_tp(sk_ri,sk_hi,sk_si,sk_vi);sk_p=lim_tp(sk_p);
  float sk_c=sqrt(lab.y*lab.y+lab.z*lab.z);
  float sk_h=sk_c>0.0f?mod2(atan2(lab.z,lab.y),2.0f*PI):0.0f;
  float3 sk_adj=ev_curves(sk_h,sk_p,5.0f);
  float3 lab_skin=float3(lab.x+sk_adj.z,(sk_c*sk_adj.y)*cos(sk_h+sk_adj.x),(sk_c*sk_adj.y)*sin(sk_h+sk_adj.x));
  float sk_nm=smootherstep(0.0f,0.1f,sk_c)*sk_lw;
  lab=lab+(lab_skin-lab)*sk_nm;
  if(c_skinE>0.0f){
    float sk_ec=sqrt(lab.y*lab.y+lab.z*lab.z);
    float sk_eh=sk_ec>0.0f?mod2(atan2(lab.z,lab.y),2.0f*PI):0.0f;
    float sk_ealign=cos(sk_eh-1.0685f);
    float sk_emask=smootherstep(0.70f,0.95f,sk_ealign)*smootherstep(0.015f,0.080f,sk_ec)*sk_lw;
    if(sk_emask>0.0f){
      float sk_edelta=atan2(sin(1.0685f-sk_eh),cos(1.0685f-sk_eh));
      float sk_eout=sk_eh+sk_edelta*c_skinE*sk_emask;
      lab.y=sk_ec*cos(sk_eout);lab.z=sk_ec*sin(sk_eout);
    }
  }

  float3 out=oklab_to_work(lab);

  // Density: subtractive sat + per-pr.hueDeg density (MONONODES-style), applied in linear AWG4.
  float d_asat = (c_limsat>0.0f)?limiter(rgb_sat(out),c_limsat):1.0f;
  float d_aden = (c_limdens>0.0f)?limiter(rgb_sat(out),c_limdens):1.0f;
  float d_rd=-(c_rd*d_aden); float d_gd=-(c_gd*d_aden); float d_bd=-(c_bd*d_aden);
  float d_cd=-(c_cd*d_aden*0.5f); float d_md=-(c_md*d_aden*0.5f); float d_yd=-(c_yd*d_aden*0.5f);
  float d_gs=c_gsat*d_asat;
  float3 d_blk = float3(0.0f,0.0f,0.0f);
  float3 d_wht = float3(1.0f,1.0f,1.0f);
  float3 d_rv = float3(d_rd+1.0f,d_rd-d_gs,d_rd-d_gs);
  float3 d_gv = float3(d_gd-d_gs,d_gd+1.0f,d_gd-d_gs);
  float3 d_bv = float3(d_bd-d_gs,d_bd-d_gs,d_bd+1.0f);
  float3 d_cv = float3(d_cd-d_gs,d_cd+1.0f,d_cd+1.0f);
  float3 d_mv = float3(d_md+1.0f,d_md-d_gs,d_md+1.0f);
  float3 d_yv = float3(d_yd+1.0f,d_yd+1.0f,d_yd-d_gs);
  float d_Rv=out.x; float d_Gv=out.y; float d_Bv=out.z;
  float3 d_nrgb;
  float d_r1x=d_rv.x-d_blk.x,d_r1y=d_rv.y-d_blk.y,d_r1z=d_rv.z-d_blk.z;
  if(d_Rv>d_Gv){
    if(d_Gv>d_Bv){
      float d_y2x=d_yv.x-d_rv.x,d_y2y=d_yv.y-d_rv.y,d_y2z=d_yv.z-d_rv.z;
      float d_w3x=d_wht.x-d_yv.x,d_w3y=d_wht.y-d_yv.y,d_w3z=d_wht.z-d_yv.z;
      d_nrgb=float3(d_Rv*d_r1x+d_blk.x+d_Gv*d_y2x+d_Bv*d_w3x,d_Rv*d_r1y+d_blk.y+d_Gv*d_y2y+d_Bv*d_w3y,d_Rv*d_r1z+d_blk.z+d_Gv*d_y2z+d_Bv*d_w3z);
    }else if(d_Rv>d_Bv){
      float d_w2x=d_wht.x-d_mv.x,d_w2y=d_wht.y-d_mv.y,d_w2z=d_wht.z-d_mv.z;
      float d_m3x=d_mv.x-d_rv.x,d_m3y=d_mv.y-d_rv.y,d_m3z=d_mv.z-d_rv.z;
      d_nrgb=float3(d_Rv*d_r1x+d_blk.x+d_Gv*d_w2x+d_Bv*d_m3x,d_Rv*d_r1y+d_blk.y+d_Gv*d_w2y+d_Bv*d_m3y,d_Rv*d_r1z+d_blk.z+d_Gv*d_w2z+d_Bv*d_m3z);
    }else{
      float d_m4x=d_mv.x-d_bv.x,d_m4y=d_mv.y-d_bv.y,d_m4z=d_mv.z-d_bv.z;
      float d_w5x=d_wht.x-d_mv.x,d_w5y=d_wht.y-d_mv.y,d_w5z=d_wht.z-d_mv.z;
      float d_b6x=d_bv.x-d_blk.x,d_b6y=d_bv.y-d_blk.y,d_b6z=d_bv.z-d_blk.z;
      d_nrgb=float3(d_Rv*d_m4x+d_Gv*d_w5x+d_Bv*d_b6x+d_blk.x,d_Rv*d_m4y+d_Gv*d_w5y+d_Bv*d_b6y+d_blk.y,d_Rv*d_m4z+d_Gv*d_w5z+d_Bv*d_b6z+d_blk.z);
    }
  }else{
    float d_w7x=d_wht.x-d_cv.x,d_w7y=d_wht.y-d_cv.y,d_w7z=d_wht.z-d_cv.z;
    float d_c8x=d_cv.x-d_bv.x,d_c8y=d_cv.y-d_bv.y,d_c8z=d_cv.z-d_bv.z;
    float d_b9x=d_bv.x-d_blk.x,d_b9y=d_bv.y-d_blk.y,d_b9z=d_bv.z-d_blk.z;
    if(d_Bv>d_Gv)d_nrgb=float3(d_Rv*d_w7x+d_Gv*d_c8x+d_Bv*d_b9x+d_blk.x,d_Rv*d_w7y+d_Gv*d_c8y+d_Bv*d_b9y+d_blk.y,d_Rv*d_w7z+d_Gv*d_c8z+d_Bv*d_b9z+d_blk.z);
    else if(d_Bv>d_Rv){
      float d_gAx=d_gv.x-d_blk.x,d_gAy=d_gv.y-d_blk.y,d_gAz=d_gv.z-d_blk.z;
      float d_cBx=d_cv.x-d_gv.x,d_cBy=d_cv.y-d_gv.y,d_cBz=d_cv.z-d_gv.z;
      d_nrgb=float3(d_Rv*d_w7x+d_Gv*d_gAx+d_blk.x+d_Bv*d_cBx,d_Rv*d_w7y+d_Gv*d_gAy+d_blk.y+d_Bv*d_cBy,d_Rv*d_w7z+d_Gv*d_gAz+d_blk.z+d_Bv*d_cBz);
    }else{
      float d_yCx=d_yv.x-d_gv.x,d_yCy=d_yv.y-d_gv.y,d_yCz=d_yv.z-d_gv.z;
      float d_gDx=d_gv.x-d_blk.x,d_gDy=d_gv.y-d_blk.y,d_gDz=d_gv.z-d_blk.z;
      float d_wEx=d_wht.x-d_yv.x,d_wEy=d_wht.y-d_yv.y,d_wEz=d_wht.z-d_yv.z;
      d_nrgb=float3(d_Rv*d_yCx+d_Gv*d_gDx+d_blk.x+d_Bv*d_wEx,d_Rv*d_yCy+d_Gv*d_gDy+d_blk.y+d_Bv*d_wEy,d_Rv*d_yCz+d_Gv*d_gDz+d_blk.z+d_Bv*d_wEz);
    }
  }
  out=d_nrgb;

  // Reverse: linear AWG4 -> destination XYZ/gamut -> native transfer.
  float3 rxyz=g2x(1,out);
  if(g==8)rxyz=d65_to_d60(rxyz);
  float3 cam=x2g(g,rxyz);
  cam=compress_to_gamut(cam,rxyz.y);
  float3 result=tfe(cam,ti_tf);

  // Preserve finite extended values; only replace NaN/Inf.
  float nx=(result.x==result.x&&fabs(result.x)<3.402823e38f)?result.x:0.0f;
  float ny=(result.y==result.y&&fabs(result.y)<3.402823e38f)?result.y:0.0f;
  float nz=(result.z==result.z&&fabs(result.z)<3.402823e38f)?result.z:0.0f;
  return float3(nx,ny,nz);
}
// ---- Metal kernel entry ----
kernel void LaserLabKernel(
    constant LaserLabParams& pr [[buffer(0)]],
    constant int& W [[buffer(1)]],
    constant int& H [[buffer(2)]],
    device const float* src [[buffer(3)]],
    device float* dst [[buffer(4)]],
    uint2 gid [[thread_position_in_grid]]
) {
    if ((int)gid.x >= W || (int)gid.y >= H) return;
    int x = (int)gid.x, y = (int)gid.y;
    int i = (y * W + x) * 4;
    float3 out = llTransform(pr, src[i + 0], src[i + 1], src[i + 2]);
    dst[i + 0] = out.x; dst[i + 1] = out.y; dst[i + 2] = out.z; dst[i + 3] = src[i + 3];
}

// ---- OpenCL kernel entry ----
__kernel void LaserLabKernel(
    __constant LaserLabParams* pr,
    __constant int* W,
    __constant int* H,
    __global const float* src,
    __global float* dst
) {
    int x = get_global_id(0);
    int y = get_global_id(1);
    int width = *W;
    int height = *H;
    if (x >= width || y >= height) return;
    int i = (y * width + x) * 4;
    float3 out = llTransform(*pr, src[i + 0], src[i + 1], src[i + 2]);
    dst[i + 0] = out.x; dst[i + 1] = out.y; dst[i + 2] = out.z; dst[i + 3] = src[i + 3];
}
