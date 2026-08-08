#include <metal_stdlib>
using namespace metal;

// ---- Params struct (identical to LaserLabParams.h) ----
typedef struct LaserLabParams
{
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

// ---- Matrix constants (constant address space) ----
// CAT16I
constant float3 CAT16I_c0 = float3(1.86206786f, 0.38752654f, -0.01584150f);
constant float3 CAT16I_c1 = float3(-1.01125463f, 0.62144744f, -0.03412294f);
constant float3 CAT16I_c2 = float3(0.14918677f, -0.00897398f, 1.04996444f);

// CAT16
constant float3 CAT16_c0 = float3(0.401288f, -0.250268f, -0.002079f);
constant float3 CAT16_c1 = float3(0.650173f, 1.204414f, 0.048952f);
constant float3 CAT16_c2 = float3(-0.051461f, 0.045854f, 0.953127f);

// G0
constant float3 G0_c0 = float3(3.2409699419f, -0.9692436363f, 0.0556300797f);
constant float3 G0_c1 = float3(-1.5373831776f, 1.8759675015f, -0.2039769589f);
constant float3 G0_c2 = float3(-0.4986107603f, 0.0415550574f, 1.0569715142f);

// G10
constant float3 G10_c0 = float3(1.7166511880f, -0.6666843518f, 0.0176398574f);
constant float3 G10_c1 = float3(-0.3556707838f, 1.6164812366f, -0.0427706133f);
constant float3 G10_c2 = float3(-0.2533662814f, 0.0157685458f, 0.9421031212f);

// G1
constant float3 G1_c0 = float3(1.5092154722f, -0.4915454517f, 0.0000000000f);
constant float3 G1_c1 = float3(-0.2505973452f, 1.3612455459f, 0.0000000000f);
constant float3 G1_c2 = float3(-0.1688114753f, 0.0972829420f, 0.9182249512f);

// G2
constant float3 G2_c0 = float3(1.7890660000f, -0.6398490000f, -0.0415320000f);
constant float3 G2_c1 = float3(-0.4825340000f, 1.3964000000f, 0.0823350000f);
constant float3 G2_c2 = float3(-0.2000760000f, 0.1944320000f, 0.8788680000f);

// G3
constant float3 G3_c0 = float3(1.5073998991f, -0.5181517271f, 0.0155116982f);
constant float3 G3_c1 = float3(-0.2458221374f, 1.3553912409f, -0.0078727714f);
constant float3 G3_c2 = float3(-0.1716116808f, 0.1258786682f, 0.9119163656f);

// G4
constant float3 G4_c0 = float3(1.5890120000f, -0.5340530000f, 0.0111790000f);
constant float3 G4_c1 = float3(-0.3132040000f, 1.3960110000f, 0.0031940000f);
constant float3 G4_c2 = float3(-0.1809650000f, 0.1024580000f, 0.9055350000f);

// G5
constant float3 G5_c0 = float3(1.4128066123f, -0.4862031858f, -0.0371387758f);
constant float3 G5_c1 = float3(-0.1775223662f, 1.2906962108f, 0.2863757596f);
constant float3 G5_c2 = float3(-0.1517703764f, 0.1574002837f, 0.6876796053f);

// G6
constant float3 G6_c0 = float3(1.8663823404f, -0.6003424925f, 0.0024519937f);
constant float3 G6_c1 = float3(-0.5183973429f, 1.3781489625f, 0.0863996742f);
constant float3 G6_c2 = float3(-0.2346098094f, 0.1767318303f, 0.8369427073f);

// G7
constant float3 G7_c0 = float3(1.4898182749f, -0.4581665745f, -0.0703496677f);
constant float3 G7_c1 = float3(-0.2608959022f, 1.2616277831f, 0.2215576672f);
constant float3 G7_c2 = float3(-0.1424265218f, 0.1596236316f, 0.7761816036f);

// G8
constant float3 G8_c0 = float3(1.6410233797f, -0.6636628587f, 0.0117218943f);
constant float3 G8_c1 = float3(-0.3248032942f, 1.6153315917f, -0.0082844420f);
constant float3 G8_c2 = float3(-0.2364246952f, 0.0167563477f, 0.9883948585f);

// G9
constant float3 G9_c0 = float3(1.7257000000f, -0.6025000000f, -0.0156000000f);
constant float3 G9_c1 = float3(-0.4314000000f, 1.3906000000f, 0.0905000000f);
constant float3 G9_c2 = float3(-0.1917000000f, 0.1671000000f, 0.8489000000f);

// I0
constant float3 I0_c0 = float3(3.2409699419f, -0.9692436363f, 0.0556300797f);
constant float3 I0_c1 = float3(-1.5373831776f, 1.8759675015f, -0.2039769589f);
constant float3 I0_c2 = float3(-0.4986107603f, 0.0415550574f, 1.0569715142f);

// L
constant float3 L_c0 = float3(0.4122214708f, 0.2119034982f, 0.0883024619f);
constant float3 L_c1 = float3(0.5363325363f, 0.6806995451f, 0.2817188376f);
constant float3 L_c2 = float3(0.0514459929f, 0.1073969566f, 0.6299787005f);

// O
constant float3 O_c0 = float3(0.2104542553f, 1.9779984951f, 0.0259040371f);
constant float3 O_c1 = float3(0.793617785f, -2.428592205f, 0.7827717662f);
constant float3 O_c2 = float3(-0.0040720468f, 0.4505937099f, -0.808675766f);

// P
constant float3 P_c0 = float3(1.0f, 1.0f, 1.0f);
constant float3 P_c1 = float3(0.3963377774f, -0.1055613458f, -0.0894841775f);
constant float3 P_c2 = float3(0.2158037573f, -0.0638541728f, -1.291485548f);

// Q
constant float3 Q_c0 = float3(4.0767416621f, -1.2684380046f, -0.0041960866f);
constant float3 Q_c1 = float3(-3.3077115901f, 2.6097574011f, -0.7034186145f);
constant float3 Q_c2 = float3(0.2309699292f, -0.3413193965f, 1.7076147014f);

// R0
constant float3 R0_c0 = float3(0.4123907993f, 0.2126390059f, 0.0193308187f);
constant float3 R0_c1 = float3(0.3575843394f, 0.7151686788f, 0.1191947798f);
constant float3 R0_c2 = float3(0.1804807884f, 0.0721923154f, 0.9505321522f);

// R10
constant float3 R10_c0 = float3(0.6369580483f, 0.2627002120f, 0.0000000000f);
constant float3 R10_c1 = float3(0.1446169036f, 0.6779980715f, 0.0280726930f);
constant float3 R10_c2 = float3(0.1688809752f, 0.0593017165f, 1.0609850577f);

// R1
constant float3 R1_c0 = float3(0.7048583204f, 0.2545241764f, 0.0f);
constant float3 R1_c1 = float3(0.1297602952f, 0.7814777327f, 0.0f);
constant float3 R1_c2 = float3(0.1158373115f, -0.0360019091f, 1.0890577508f);

// R2
constant float3 R2_c0 = float3(0.638008f, 0.291954f, 0.002798f);
constant float3 R2_c1 = float3(0.214704f, 0.823841f, -0.067034f);
constant float3 R2_c2 = float3(0.097744f, -0.115795f, 1.153294f);

// R3
constant float3 R3_c0 = float3(0.7064827132f, 0.2709796708f, -0.0096778454f);
constant float3 R3_c1 = float3(0.1288010498f, 0.7866064112f, 0.0046000375f);
constant float3 R3_c2 = float3(0.1151721641f, -0.057586082f, 1.0941355587f);

// R4
constant float3 R4_c0 = float3(0.679644f, 0.260686f, -0.00931f);
constant float3 R4_c1 = float3(0.152211f, 0.774894f, -0.004612f);
constant float3 R4_c2 = float3(0.1186f, -0.03558f, 1.10298f);

// R5
constant float3 R5_c0 = float3(0.735275f, 0.286694f, -0.079681f);
constant float3 R5_c1 = float3(0.068609f, 0.842979f, -0.347343f);
constant float3 R5_c2 = float3(0.146571f, -0.129673f, 1.516082f);

// R6
constant float3 R6_c0 = float3(0.6065303722f, 0.2679894071f, -0.029442166f);
constant float3 R6_c1 = float3(0.2204080953f, 0.8327308786f, -0.086610607f);
constant float3 R6_c2 = float3(0.1234790005f, -0.1007202857f, 1.2048607644f);

// R7
constant float3 R7_c0 = float3(0.7160496466f, 0.2612613575f, -0.0096763466f);
constant float3 R7_c1 = float3(0.1296834779f, 0.8696421458f, -0.2364816361f);
constant float3 R7_c2 = float3(0.1047228026f, -0.1309035033f, 1.3352157335f);

// R8
constant float3 R8_c0 = float3(0.6624541811f, 0.2722287168f, -0.0055746495f);
constant float3 R8_c1 = float3(0.1340042065f, 0.6740817658f, 0.0040607335f);
constant float3 R8_c2 = float3(0.156187687f, 0.0536895174f, 1.0103391003f);

// R9
constant float3 R9_c0 = float3(0.6482f, 0.283f, -0.0183f);
constant float3 R9_c1 = float3(0.194f, 0.8132f, -0.0832f);
constant float3 R9_c2 = float3(0.1082f, -0.0962f, 1.1903f);

// S0
constant float3 S0_c0 = float3(0.4123907993f, 0.2126390059f, 0.0193308187f);
constant float3 S0_c1 = float3(0.3575843394f, 0.7151686788f, 0.1191947798f);
constant float3 S0_c2 = float3(0.1804807884f, 0.0721923154f, 0.9505321522f);

constant float3 WHITE_D65 = float3(0.9504559271f, 1.0f, 1.0890577508f);
constant float3 WHITE_D60 = float3(0.9526460746f, 1.0f, 1.0088251844f);
// ---- Scalar constants ----
constant float LOGC3_CUT = 0.010591f;
constant float LOGC3_A = 5.555556f;
constant float LOGC3_B = 0.052272f;
constant float LOGC3_C = 0.24719f;
constant float LOGC3_D = 0.385537f;
constant float LOGC3_E = 5.367655f;
constant float LOGC3_F = 0.092809f;
constant float LOGC4_A = 2231.8263090676883f;
constant float LOGC4_B = 0.9071358748778103f;
constant float LOGC4_C = 0.09286412512218964f;
constant float LOGC4_S2 = 0.1135972086105891f;
constant float LOGC4_T = -0.01805699611991131f;
constant float APPLE_R0 = -0.05641088f;
constant float APPLE_RT = 0.01f;
constant float APPLE_SIGMA = 47.28711236f;
constant float APPLE_BETA = 0.00964052f;
constant float APPLE_GAMMA = 0.08550479f;
constant float APPLE_DELTA = 0.69336945f;
constant float EXPO_FLASH_SCALE = 0.1f;
constant float PI = 3.141592653589793238f;

// clampf
inline float clampf(float v, float lo, float hi) { return fmax(lo, fmin(hi, v)); }

// chroma
inline float chroma(float3 lab){return sqrt(lab.y*lab.y+lab.z*lab.z);}

// mmul
inline float3 mmul(float3 c0,float3 c1,float3 c2,float3 v){return float3(c0.x*v.x+c1.x*v.y+c2.x*v.z,c0.y*v.x+c1.y*v.y+c2.y*v.z,c0.z*v.x+c1.z*v.y+c2.z*v.z);}

// cbrt_s
inline float cbrt_s(float x){return(x<0.0f?-1.0f:1.0f)*pow(fabs(x),1.0f/3.0f);}

// spowf
inline float spowf(float x,float p){float y=pow(fabs(x),p);return(x<0.0f)?-y:y;}

// cat16_adapt
inline float3 cat16_adapt(float3 xyz,float3 src_white,float3 dst_white){
  float3 lms=mmul(CAT16_C0,CAT16_C1,CAT16_C2,xyz);
  float3 src=mmul(CAT16_C0,CAT16_C1,CAT16_C2,src_white);
  float3 dst=mmul(CAT16_C0,CAT16_C1,CAT16_C2,dst_white);
  lms=float3(lms.x*dst.x/src.x,lms.y*dst.y/src.y,lms.z*dst.z/src.z);
  return mmul(CAT16I_C0,CAT16I_C1,CAT16I_C2,lms);
}

// d60_to_d65
inline float3 d60_to_d65(float3 xyz){return cat16_adapt(xyz,WHITE_D60,WHITE_D65);}

// d65_to_d60
inline float3 d65_to_d60(float3 xyz){return cat16_adapt(xyz,WHITE_D65,WHITE_D60);}

// white_balance
inline float3 white_balance(float3 xyz,float temp_value,float tint_value){
  if(fabs(temp_value)<1e-6f&&fabs(tint_value)<1e-6f)return xyz;
  float k=6504.0f*exp(temp_value*1.0f),k2=k*k,k3=k2*k;
  float x=k<=4000.0f?-0.2661239e9f/k3-0.2343580e6f/k2+0.8776956e3f/k+0.179910f:-3.0258469e9f/k3+2.1070379e6f/k2+0.2226347e3f/k+0.240390f;
  float y=k<=4000.0f?-1.1063814f*x*x*x-1.34811020f*x*x+2.18555832f*x-0.20219683f:3.0817580f*x*x*x-5.8733867f*x*x+3.75112997f*x-0.37001483f;
  y=fmax(y+tint_value*0.025f,0.05f);
  float3 source_white=float3(x/y,1.0f,(1.0f-x-y)/y);
  return cat16_adapt(xyz,source_white,WHITE_D65);
}

// printer_lights
inline float3 printer_lights(float3 rgb,float master,float red,float green,float blue){
  float m=master-25.0f;
  float rg=pow(10.0f,(m+red-25.0f)*0.0125f);
  float gg=pow(10.0f,(m+green-25.0f)*0.0125f);
  float bg=pow(10.0f,(m+blue-25.0f)*0.0125f);
  return float3(rgb.x*rg,rgb.y*gg,rgb.z*bg);
}

// g2x
inline float3 g2x(int g,float3 v){
  if(g==0)return mmul(R0_C0,R0_C1,R0_C2,v);if(g==1)return mmul(R1_C0,R1_C1,R1_C2,v);
  if(g==2)return mmul(R2_C0,R2_C1,R2_C2,v);if(g==3)return mmul(R3_C0,R3_C1,R3_C2,v);
  if(g==4)return mmul(R4_C0,R4_C1,R4_C2,v);if(g==5)return mmul(R5_C0,R5_C1,R5_C2,v);
  if(g==6)return mmul(R6_C0,R6_C1,R6_C2,v);if(g==7)return mmul(R7_C0,R7_C1,R7_C2,v);
  if(g==8)return mmul(R8_C0,R8_C1,R8_C2,v);if(g==9)return mmul(R9_C0,R9_C1,R9_C2,v);
  return mmul(R10_C0,R10_C1,R10_C2,v);}

// x2g
inline float3 x2g(int g,float3 v){
  if(g==0)return mmul(G0_C0,G0_C1,G0_C2,v);if(g==1)return mmul(G1_C0,G1_C1,G1_C2,v);
  if(g==2)return mmul(G2_C0,G2_C1,G2_C2,v);if(g==3)return mmul(G3_C0,G3_C1,G3_C2,v);
  if(g==4)return mmul(G4_C0,G4_C1,G4_C2,v);if(g==5)return mmul(G5_C0,G5_C1,G5_C2,v);
  if(g==6)return mmul(G6_C0,G6_C1,G6_C2,v);if(g==7)return mmul(G7_C0,G7_C1,G7_C2,v);
  if(g==8)return mmul(G8_C0,G8_C1,G8_C2,v);if(g==9)return mmul(G9_C0,G9_C1,G9_C2,v);
  return mmul(G10_C0,G10_C1,G10_C2,v);}

// tf709d
inline float3 tf709d(float3 v){float3 o;o.x=v.x<=0.081f?v.x/4.5f:pow((v.x+0.099f)/1.099f,1.0f/0.45f);o.y=v.y<=0.081f?v.y/4.5f:pow((v.y+0.099f)/1.099f,1.0f/0.45f);o.z=v.z<=0.081f?v.z/4.5f:pow((v.z+0.099f)/1.099f,1.0f/0.45f);return o;}

// tfl4d
inline float3 tfl4d(float3 v){float a=LOGC4_A,b=LOGC4_B,c=LOGC4_C,s2=LOGC4_S2,t=LOGC4_T;float3 o;o.x=v.x>=0.0f?(pow(2.0f,14.0f*((v.x-c)/b)+6.0f)-64.0f)/a:v.x*s2+t;o.y=v.y>=0.0f?(pow(2.0f,14.0f*((v.y-c)/b)+6.0f)-64.0f)/a:v.y*s2+t;o.z=v.z>=0.0f?(pow(2.0f,14.0f*((v.z-c)/b)+6.0f)-64.0f)/a:v.z*s2+t;return o;}

// tfl3d
inline float3 tfl3d(float3 v){float cv=LOGC3_E*LOGC3_CUT+LOGC3_F;float3 o;o.x=v.x>cv?(pow(10.0f,(v.x-LOGC3_D)/LOGC3_C)-LOGC3_B)/LOGC3_A:(v.x-LOGC3_F)/LOGC3_E;o.y=v.y>cv?(pow(10.0f,(v.y-LOGC3_D)/LOGC3_C)-LOGC3_B)/LOGC3_A:(v.y-LOGC3_F)/LOGC3_E;o.z=v.z>cv?(pow(10.0f,(v.z-LOGC3_D)/LOGC3_C)-LOGC3_B)/LOGC3_A:(v.z-LOGC3_F)/LOGC3_E;return o;}

// tfs3d
inline float3 tfs3d(float3 v){float3 o;o.x=v.x>=171.2102946929f/1023.0f?(pow(10.0f,(v.x*1023.0f-420.0f)/261.5f))*0.19f-0.01f:(v.x*1023.0f-95.0f)*0.01125f/(171.2102946929f-95.0f);o.y=v.y>=171.2102946929f/1023.0f?(pow(10.0f,(v.y*1023.0f-420.0f)/261.5f))*0.19f-0.01f:(v.y*1023.0f-95.0f)*0.01125f/(171.2102946929f-95.0f);o.z=v.z>=171.2102946929f/1023.0f?(pow(10.0f,(v.z*1023.0f-420.0f)/261.5f))*0.19f-0.01f:(v.z*1023.0f-95.0f)*0.01125f/(171.2102946929f-95.0f);return o;}

// tfvld
inline float3 tfvld(float3 v){float cut=0.181f,b=0.00873f,c=0.241514f,d=0.598206f;float3 o;o.x=v.x<cut?(v.x-0.125f)/5.6f:pow(10.0f,(v.x-d)/c)-b;o.y=v.y<cut?(v.y-0.125f)/5.6f:pow(10.0f,(v.y-d)/c)-b;o.z=v.z<cut?(v.z-0.125f)/5.6f:pow(10.0f,(v.z-d)/c)-b;return o;}

// tflg10d
inline float3 tflg10d(float3 v){float B=25.0f*(sqrt(4093.0f)-3.0f)/9.0f,A=1.0f/log10(B*184.32f+1.0f);return float3(v.x<0.0f?v.x/(A*B*0.4342944819f):(pow(10.0f,v.x/A)-1.0f)/B,v.y<0.0f?v.y/(A*B*0.4342944819f):(pow(10.0f,v.y/A)-1.0f)/B,v.z<0.0f?v.z/(A*B*0.4342944819f):(pow(10.0f,v.z/A)-1.0f)/B);}

// tfdwd
inline float3 tfdwd(float3 v){float A=0.0075f,B=7.0f,C=0.07329248f,LC=0.02740668f,M=10.44426855f;float3 o;o.x=v.x<=LC?v.x/M:pow(2.0f,(v.x/C)-B)-A;o.y=v.y<=LC?v.y/M:pow(2.0f,(v.y/C)-B)-A;o.z=v.z<=LC?v.z/M:pow(2.0f,(v.z/C)-B)-A;return o;}

// tfc2d
inline float3 tfc2d(float3 v){float k=0.092864125f,m=0.24136077f,n=87.09937546f;float3 o;o.x=v.x<k?-(pow(10.0f,(k-v.x)/m)-1.0f)/n:(pow(10.0f,(v.x-k)/m)-1.0f)/n;o.y=v.y<k?-(pow(10.0f,(k-v.y)/m)-1.0f)/n:(pow(10.0f,(v.y-k)/m)-1.0f)/n;o.z=v.z<k?-(pow(10.0f,(k-v.z)/m)-1.0f)/n:(pow(10.0f,(v.z-k)/m)-1.0f)/n;return o*0.9f;}

// tfc3d
inline float3 tfc3d(float3 v){float3 o;o.x=v.x<0.097465473f?-(pow(10.0f,(0.12783901f-v.x)/0.36726845f)-1.0f)/14.98325f:(v.x<=0.15277891f?(v.x-0.12512219f)/1.9754798f:(pow(10.0f,(v.x-0.12240537f)/0.36726845f)-1.0f)/14.98325f);o.y=v.y<0.097465473f?-(pow(10.0f,(0.12783901f-v.y)/0.36726845f)-1.0f)/14.98325f:(v.y<=0.15277891f?(v.y-0.12512219f)/1.9754798f:(pow(10.0f,(v.y-0.12240537f)/0.36726845f)-1.0f)/14.98325f);o.z=v.z<0.097465473f?-(pow(10.0f,(0.12783901f-v.z)/0.36726845f)-1.0f)/14.98325f:(v.z<=0.15277891f?(v.z-0.12512219f)/1.9754798f:(pow(10.0f,(v.z-0.12240537f)/0.36726845f)-1.0f)/14.98325f);return o*0.9f;}

// tfdld
inline float3 tfdld(float3 v){float3 o;o.x=v.x<=0.14f?(v.x-0.0929f)/6.025f:(pow(10.0f,3.89616f*v.x-2.27752f)-0.0108f)/0.9892f;o.y=v.y<=0.14f?(v.y-0.0929f)/6.025f:(pow(10.0f,3.89616f*v.y-2.27752f)-0.0108f)/0.9892f;o.z=v.z<=0.14f?(v.z-0.0929f)/6.025f:(pow(10.0f,3.89616f*v.z-2.27752f)-0.0108f)/0.9892f;return o;}

// tfapl
inline float3 tfapl(float3 v){float Pt=APPLE_SIGMA*(APPLE_RT-APPLE_R0)*(APPLE_RT-APPLE_R0);float3 o;o.x=v.x>=Pt?pow(2.0f,(v.x-APPLE_DELTA)/APPLE_GAMMA)-APPLE_BETA:(v.x>=0.0f?sqrt(v.x/APPLE_SIGMA)+APPLE_R0:APPLE_R0);o.y=v.y>=Pt?pow(2.0f,(v.y-APPLE_DELTA)/APPLE_GAMMA)-APPLE_BETA:(v.y>=0.0f?sqrt(v.y/APPLE_SIGMA)+APPLE_R0:APPLE_R0);o.z=v.z>=Pt?pow(2.0f,(v.z-APPLE_DELTA)/APPLE_GAMMA)-APPLE_BETA:(v.z>=0.0f?sqrt(v.z/APPLE_SIGMA)+APPLE_R0:APPLE_R0);return o;}

// tfcctd
inline float3 tfcctd(float3 v){float A=10.5402377417f,B=0.0729055342f,YB=0.15525114f;float3 o;o.x=v.x<=YB?(v.x-B)/A:pow(2.0f,v.x*17.52f-9.72f);o.y=v.y<=YB?(v.y-B)/A:pow(2.0f,v.y*17.52f-9.72f);o.z=v.z<=YB?(v.z-B)/A:pow(2.0f,v.z*17.52f-9.72f);return o;}

// tfd
inline float3 tfd(float3 r,int t){if(t==0)return tf709d(r);if(t==1)return tfl4d(r);if(t==2)return tfl3d(r);if(t==3)return tfs3d(r);if(t==4)return tfvld(r);if(t==5)return tflg10d(r);if(t==6)return tfdwd(r);if(t==7)return r;if(t==8)return tfc2d(r);if(t==9)return tfc3d(r);if(t==10)return tfdld(r);if(t==11)return r;if(t==12)return r;if(t==13)return tfapl(r);return tfcctd(r);}

// tf709e
inline float3 tf709e(float3 v){float3 o;o.x=v.x<=0.018f?4.5f*v.x:1.099f*pow(v.x,0.45f)-0.099f;o.y=v.y<=0.018f?4.5f*v.y:1.099f*pow(v.y,0.45f)-0.099f;o.z=v.z<=0.018f?4.5f*v.z:1.099f*pow(v.z,0.45f)-0.099f;return o;}

// tfl4e
inline float3 tfl4e(float3 v){float a=LOGC4_A,b=LOGC4_B,c=LOGC4_C,s2=LOGC4_S2,t=LOGC4_T;float3 o;o.x=v.x>t?c+b*(log10(a*v.x+64.0f)/log10(2.0f)-6.0f)/14.0f:(v.x-t)/s2;o.y=v.y>t?c+b*(log10(a*v.y+64.0f)/log10(2.0f)-6.0f)/14.0f:(v.y-t)/s2;o.z=v.z>t?c+b*(log10(a*v.z+64.0f)/log10(2.0f)-6.0f)/14.0f:(v.z-t)/s2;return o;}

// tfl3e
inline float3 tfl3e(float3 v){float3 o;o.x=v.x>LOGC3_CUT?LOGC3_C*log10(LOGC3_A*v.x+LOGC3_B)+LOGC3_D:LOGC3_E*v.x+LOGC3_F;o.y=v.y>LOGC3_CUT?LOGC3_C*log10(LOGC3_A*v.y+LOGC3_B)+LOGC3_D:LOGC3_E*v.y+LOGC3_F;o.z=v.z>LOGC3_CUT?LOGC3_C*log10(LOGC3_A*v.z+LOGC3_B)+LOGC3_D:LOGC3_E*v.z+LOGC3_F;return o;}

// tfs3e
inline float3 tfs3e(float3 v){float a=0.19f,b=0.01f,c=261.5f,d=420.0f,e=171.2102946929f,f=1023.0f;float3 o;o.x=v.x<=0.01125f?(v.x*(e-95.0f)/0.01125f+95.0f)/f:(log10((v.x+b)/a)*c+d)/f;o.y=v.y<=0.01125f?(v.y*(e-95.0f)/0.01125f+95.0f)/f:(log10((v.y+b)/a)*c+d)/f;o.z=v.z<=0.01125f?(v.z*(e-95.0f)/0.01125f+95.0f)/f:(log10((v.z+b)/a)*c+d)/f;return o;}

// tfvle
inline float3 tfvle(float3 v){float cut=0.181f,b=0.00873f,c=0.241514f,d=0.598206f;float3 o;o.x=v.x<=(cut-0.125f)/5.6f?v.x*5.6f+0.125f:c*log10(v.x+b)+d;o.y=v.y<=(cut-0.125f)/5.6f?v.y*5.6f+0.125f:c*log10(v.y+b)+d;o.z=v.z<=(cut-0.125f)/5.6f?v.z*5.6f+0.125f:c*log10(v.z+b)+d;return o;}

// tflg10e
inline float3 tflg10e(float3 v){float B=25.0f*(sqrt(4093.0f)-3.0f)/9.0f,A=1.0f/log10(B*184.32f+1.0f);return float3(v.x<0.0f?v.x*(A*B*0.4342944819f):A*log10(v.x*B+1.0f),v.y<0.0f?v.y*(A*B*0.4342944819f):A*log10(v.y*B+1.0f),v.z<0.0f?v.z*(A*B*0.4342944819f):A*log10(v.z*B+1.0f));}

// tfdwe
inline float3 tfdwe(float3 v){float A=0.0075f,B=7.0f,C=0.07329248f,LC=0.02740668f,M=10.44426855f;float3 o;o.x=v.x<=LC/M?v.x*M:C*(log2(v.x+A)+B);o.y=v.y<=LC/M?v.y*M:C*(log2(v.y+A)+B);o.z=v.z<=LC/M?v.z*M:C*(log2(v.z+A)+B);return o;}

// tfc2e
inline float3 tfc2e(float3 v){float k=0.092864125f,m=0.24136077f,n=87.09937546f;v=float3(v.x/0.9f,v.y/0.9f,v.z/0.9f);float3 o;o.x=v.x<0.0f?k-m*log10(-v.x*n+1.0f):k+m*log10(v.x*n+1.0f);o.y=v.y<0.0f?k-m*log10(-v.y*n+1.0f):k+m*log10(v.y*n+1.0f);o.z=v.z<0.0f?k-m*log10(-v.z*n+1.0f):k+m*log10(v.z*n+1.0f);return o;}

// tfc3e
inline float3 tfc3e(float3 v){v=float3(v.x/0.9f,v.y/0.9f,v.z/0.9f);float3 o;o.x=v.x<=0.0f?0.12783901f-0.36726845f*log10(-v.x*14.98325f+1.0f):v.x<=(0.15277891f-0.12512219f)/1.9754798f?v.x*1.9754798f+0.12512219f:0.12240537f+0.36726845f*log10(v.x*14.98325f+1.0f);o.y=v.y<=0.0f?0.12783901f-0.36726845f*log10(-v.y*14.98325f+1.0f):v.y<=(0.15277891f-0.12512219f)/1.9754798f?v.y*1.9754798f+0.12512219f:0.12240537f+0.36726845f*log10(v.y*14.98325f+1.0f);o.z=v.z<=0.0f?0.12783901f-0.36726845f*log10(-v.z*14.98325f+1.0f):v.z<=(0.15277891f-0.12512219f)/1.9754798f?v.z*1.9754798f+0.12512219f:0.12240537f+0.36726845f*log10(v.z*14.98325f+1.0f);return o;}

// tfdle
inline float3 tfdle(float3 v){float3 o;o.x=v.x<=(0.14f-0.0929f)/6.025f?v.x*6.025f+0.0929f:(log10(v.x*0.9892f+0.0108f)+2.27752f)/3.89616f;o.y=v.y<=(0.14f-0.0929f)/6.025f?v.y*6.025f+0.0929f:(log10(v.y*0.9892f+0.0108f)+2.27752f)/3.89616f;o.z=v.z<=(0.14f-0.0929f)/6.025f?v.z*6.025f+0.0929f:(log10(v.z*0.9892f+0.0108f)+2.27752f)/3.89616f;return o;}

// tfaple
inline float3 tfaple(float3 v){float3 o;o.x=v.x<=APPLE_R0?0.0f:v.x<APPLE_RT?APPLE_SIGMA*(v.x-APPLE_R0)*(v.x-APPLE_R0):APPLE_DELTA+APPLE_GAMMA*log2(v.x+APPLE_BETA);o.y=v.y<=APPLE_R0?0.0f:v.y<APPLE_RT?APPLE_SIGMA*(v.y-APPLE_R0)*(v.y-APPLE_R0):APPLE_DELTA+APPLE_GAMMA*log2(v.y+APPLE_BETA);o.z=v.z<=APPLE_R0?0.0f:v.z<APPLE_RT?APPLE_SIGMA*(v.z-APPLE_R0)*(v.z-APPLE_R0):APPLE_DELTA+APPLE_GAMMA*log2(v.z+APPLE_BETA);return o;}

// tfccte
inline float3 tfccte(float3 v){float A=10.5402377417f,B=0.0729055342f,XB=0.0078125f;float3 o;o.x=v.x<=XB?A*v.x+B:(log2(v.x)+9.72f)/17.52f;o.y=v.y<=XB?A*v.y+B:(log2(v.y)+9.72f)/17.52f;o.z=v.z<=XB?A*v.z+B:(log2(v.z)+9.72f)/17.52f;return o;}

// tfe
inline float3 tfe(float3 r,int t){if(t==0)return tf709e(r);if(t==1)return tfl4e(r);if(t==2)return tfl3e(r);if(t==3)return tfs3e(r);if(t==4)return tfvle(r);if(t==5)return tflg10e(r);if(t==6)return tfdwe(r);if(t==7)return r;if(t==8)return tfc2e(r);if(t==9)return tfc3e(r);if(t==10)return tfdle(r);if(t==11)return r;if(t==12)return r;if(t==13)return tfaple(r);return tfccte(r);}

// l2o
inline float3 l2o(float3 c){float3 l=mmul(L_C0,L_C1,L_C2,c);l=float3(cbrt_s(l.x),cbrt_s(l.y),cbrt_s(l.z));return mmul(O_C0,O_C1,O_C2,l);}

// o2l
inline float3 o2l(float3 c){float3 l=mmul(P_C0,P_C1,P_C2,c);l=float3(l.x*l.x*l.x,l.y*l.y*l.y,l.z*l.z*l.z);return mmul(Q_C0,Q_C1,Q_C2,l);}

// work_to_oklab
inline float3 work_to_oklab(float3 work){float3 xyz=g2x(1,work);return l2o(mmul(I0_C0,I0_C1,I0_C2,xyz));}

// oklab_to_work
inline float3 oklab_to_work(float3 lab){float3 srgb=o2l(lab);return x2g(1,mmul(S0_C0,S0_C1,S0_C2,srgb));}

// awg4_ingamut
inline bool awg4_ingamut(float L,float cosh,float sinh,float r){
  float l0=L+P_C1[0]*cosh*r+P_C2[0]*sinh*r;
  float l1=L+P_C1[1]*cosh*r+P_C2[1]*sinh*r;
  float l2=L+P_C1[2]*cosh*r+P_C2[2]*sinh*r;
  l0=l0*l0*l0;l1=l1*l1*l1;l2=l2*l2*l2;
  float a0=1.8746868966f*l0-1.0494462399f*l1+0.1747593441f*l2;
  float a1=-0.6657305832f*l0+1.7472828065f*l1-0.0815522231f*l2;
  float a2=-0.0701275482f*l0-0.3870256867f*l1+1.4571532352f*l2;
  return a0>=0.0f&&a1>=0.0f&&a2>=0.0f;
}

// compress_oklab_chroma
inline float3 compress_oklab_chroma(float3 lab,float amount){
  if(amount<=0.0f)return lab;
  float L=lab.x;
  float c=sqrt(lab.y*lab.y+lab.z*lab.z);
  if(c<=0.04f)return lab;                       // protect near-neutral
  float hue=atan2(lab.z,lab.y);
  float cosh=cos(hue),sinh=sin(hue);
  // Coarse scan for the first (in->out) exit along r in [0,4], then refine the bracket.
  float cmax=4.0f;
  bool prev=awg4_ingamut(L,cosh,sinh,0.0f);
  for(int i=1;i<=32;i++){
    float r=(float)i*4.0f/32.0f;
    bool cur=awg4_ingamut(L,cosh,sinh,r);
    if(prev&&!cur){
      float lo=(float)(i-1)*4.0f/32.0f,hi=r;
      for(int it=0;it<10;it++){
        float mid=0.5f*(lo+hi);
        if(awg4_ingamut(L,cosh,sinh,mid))lo=mid;else hi=mid;
      }
      cmax=0.5f*(lo+hi);
      break;
    }
    prev=cur;
  }
  cmax=fmax(cmax,c);
  const float guard=0.72f;
  float g=guard*cmax;
  if(c<=g)return lab;
  float t=fmin((c-g)/(cmax-g+1e-6f),1.0f);
  float k=1.0f/(1.0f+amount*4.0f*t);
  float comp=g+(c-g)*k;
  float scale=fmin(comp/c,1.0f);
  lab.y*=scale;lab.z*=scale;
  return lab;
}

// compress_to_gamut
inline float3 compress_to_gamut(float3 rgb,float luminance){
  if(luminance<=0.0f)return rgb;
  float scale=1.0f;
  float3 delta=rgb-float3(luminance,luminance,luminance);
  if(rgb.x<0.0f&&delta.x<0.0f)scale=fmin(scale,luminance/-delta.x);
  if(rgb.y<0.0f&&delta.y<0.0f)scale=fmin(scale,luminance/-delta.y);
  if(rgb.z<0.0f&&delta.z<0.0f)scale=fmin(scale,luminance/-delta.z);
  scale=fmax(fmin(scale,1.0f),0.0f);
  return float3(luminance+delta.x*scale,luminance+delta.y*scale,luminance+delta.z*scale);
}

// rgb_sat
inline float rgb_sat(float3 c){float mn=fmin(c.x,fmin(c.y,c.z)),mx=fmax(c.x,fmax(c.y,c.z));return mx==0.0f?0.0f:(mx-mn)/mx;}

// smootherstep
inline float smoothstep(float e0,float e1,float x){x=fmax(e0,fmin(e1,x));x=(x-e0)/(e1-e0);return x*x*(3.0f-2.0f*x);}

// limiter
inline float limiter(float v,float l){return smoothstep(0.0f,l,v);}

// shape_tone_oklab
inline float shape_tone_oklab(float v,float sh,float hl){
  const float HINGE=0.70f,KNEE=0.18f,SH_SLOPE=0.35f;
  // Shadow region weight: 1 at black, fades out by the low-mids.
  float sw=1.0f-smoothstep(0.0f,0.45f,v);
  // Lift/deepen anchored at black (multiplicative near-black so v=0 stays 0).
  v*=1.0f+sh*SH_SLOPE*sw;
  // Highlight region weight around a soft hinge; open above, close below for compress.
  float hw=smoothstep(HINGE-KNEE,HINGE+KNEE,v);
  // Signed soft-knee: hl>0 compresses the gap above HINGE, hl<0 expands it.
  float gap=v-HINGE;
  float shp=hl>0.0f?HINGE+gap/(1.0f+hl*2.5f*fabs(gap)):HINGE+gap*(1.0f-hl*0.5f);
  v+=(shp-v)*hw;
  return v;
}

// deg_to_rad
inline float deg_to_rad(float d){return d*(2.0f*PI)/360.0f;}

// mod2
inline float mod2(float x,float y){return x<0.0f?y+_fmod(x,y):_fmod(x,y);}

// mk_tp
inline tp_t mk_tp(float ri[7],float hi[7],float si[7],float vi[7]){
  tp_t p;for(int i=0;i<7;i++){p.ri[i]=ri[i];p.hi[i]=hi[i];p.si[i]=si[i];p.vi[i]=vi[i];}return p;
}

// lim_tp
inline tp_t lim_tp(tp_t p){
  float pos[7];for(int i=0;i<7;i++)pos[i]=p.ri[i]+p.hi[i];
  if(pos[0]>pos[6])pos[0]-=2.0f*PI;
  for(int j=0;j<12;j++){
    int u=0;
    for(int i=0;i<6;i++){if(pos[i]>pos[i+1]){float m=(pos[i]+pos[i+1])*0.5f;pos[i]=m;pos[i+1]=m;u=1;}}
    if(pos[6]>pos[0]+2.0f*PI){float m=(pos[6]+pos[0]+2.0f*PI)*0.5f;pos[6]=m;pos[0]=m-2.0f*PI;u=1;}
    if(!u)break;
  }
  for(int i=0;i<7;i++)p.hi[i]=pos[i]-p.ri[i];
  return p;
}

// interp_cyl
inline float interp_cyl(float ir,float2 cp[7],float sm){
  float n=0.0f,d=0.0f;for(int i=0;i<7;i++){float w=exp(sm*cos(ir-cp[i].x));n+=w*cp[i].y;d+=w;}return n/d;
}

// ev_curves
inline float3 ev_curves(float ir,tp_t p,float sm){
  float2 hp[7],sp[7],vp[7];for(int i=0;i<7;i++){hp[i]=make_float2(p.ri[i],p.hi[i]);sp[i]=make_float2(p.ri[i],p.si[i]);vp[i]=make_float2(p.ri[i],p.vi[i]);}
  return float3(interp_cyl(ir,hp,sm),interp_cyl(ir,sp,sm),interp_cyl(ir,vp,sm));
}

// -- llTransform: per-pixel grade (ported from DCTL transform()) --
inline float3 llTransform(constant LaserLabParams& pr, float r_in, float g_in, float b_in) {

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

  int gamut_idx=fmin(fmax((int)(pr.inputGamut+0.5f),0),10);
  int transfer_idx=fmin(fmax((int)(pr.inputTransfer+0.5f),0),14);
  float3 r=float3(p_R,p_G,p_B);

  // Forward: native decode -> XYZ D65 -> CAT16 white balance -> linear AWG4.
  float3 lin=tfd(r,transfer_idx);
  float3 xyz=g2x(g,lin);
  if(gamut_idx==8)xyz=d60_to_d65(xyz);
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
  float liftMask=1.0f-smoothstep(0.0f,0.7f,L);
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
    float hiW=smoothstep(0.80f,1.15f,L);
    float cs=1.0f-c_desat_hi*hiW;
    lab.y*=cs;lab.z*=cs;
  }

  // Mid Chroma: boost OKLab chroma in a smooth midtone lightness band.
  if(c_mid>0.001f){
    float mdMask=smoothstep(0.2f,0.4f,L)*(1.0f-smoothstep(0.6f,0.8f,L));
    float md=1.0f+c_mid*mdMask*0.3f;
    lab.y*=md;lab.z*=md;
  }

  // Skin/warm axis (tetra, from thatcherfreeman/SmoothTetraSkin) in OKLab a/b pr.hueDeg space.
  // Broad lightness weighting so very dark/bright objects are less affected.
  // Skin/warm lightness gate: dark fades in->full mids->fades out bright.
  float sk_dg=smoothstep(0.02f,0.18f,L),sk_bg=1.0f-smoothstep(1.0f,1.5f,L);
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
  float sk_nm=smoothstep(0.0f,0.1f,sk_c)*sk_lw;
  lab=lab+(lab_skin-lab)*sk_nm;
  if(c_skinE>0.0f){
    float sk_ec=sqrt(lab.y*lab.y+lab.z*lab.z);
    float sk_eh=sk_ec>0.0f?mod2(atan2(lab.z,lab.y),2.0f*PI):0.0f;
    float sk_ealign=cos(sk_eh-1.0685f);
    float sk_emask=smoothstep(0.70f,0.95f,sk_ealign)*smoothstep(0.015f,0.080f,sk_ec)*sk_lw;
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
  if(gamut_idx==8)rxyz=d65_to_d60(rxyz);
  float3 cam=x2g(g,rxyz);
  cam=compress_to_gamut(cam,rxyz.y);
  float3 result=tfe(cam,transfer_idx);

  // Preserve finite extended values; only replace NaN/Inf.
  float nx=(result.x==result.x&&fabs(result.x)<3.402823e38f)?result.x:0.0f;
  float ny=(result.y==result.y&&fabs(result.y)<3.402823e38f)?result.y:0.0f;
  float nz=(result.z==result.z&&fabs(result.z)<3.402823e38f)?result.z:0.0f;
  return float3(nx,ny,nz);
}


// ---- Kernel entry ----
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
