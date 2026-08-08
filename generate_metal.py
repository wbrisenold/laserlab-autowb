#!/usr/bin/env python3
"""Generate the COMPLETE LaserLab Metal kernel source from LaserLab.dctl.
No truncation, no 'ff' suffixes, no duplicate functions."""
import re

dctl = open("../LaserLab.dctl").read()

# --- matrices / whites / scalars ---
mat = {}
for m in re.finditer(r'#define\s+(\w+)\s+make_float3\(\s*([-\d.eEfF]+)\s*,\s*([-\d.eEfF]+)\s*,\s*([-\d.eEfF]+)\s*\)', dctl):
    mat[m.group(1)] = [m.group(2), m.group(3), m.group(4)]

from collections import OrderedDict
families = OrderedDict()
for name in sorted(mat):
    m2 = re.match(r'^(.*)_C([012])$', name)
    if m2:
        base, col = m2.group(1), int(m2.group(2))
        families.setdefault(base, {})[col] = mat[name]

scalars = {}
for m in re.finditer(r'#define\s+(\w+)\s+([-\d\.eEf]+f?)', dctl):
    scalars[m.group(1)] = m.group(2)

# Extract ALL functions, keeping bodies intact
def extract_funcs(text):
    funcs = {}
    lines = text.split('\n')
    i = 0
    while i < len(lines):
        line = lines[i].strip()
        if line.startswith('__DEVICE__'):
            m = re.match(r'__DEVICE__\s+\w+\s+(\w+)\s*\(', lines[i])
            if m:
                name = m.group(1)
                full = '\n'.join(lines[i:])
                bp = full.find('{')
                if bp >= 0:
                    depth = 0; end = bp
                    for ch in full[bp:]:
                        if ch == '{': depth += 1
                        elif ch == '}':
                            depth -= 1
                            if depth == 0: break
                        end += 1
                    funcs[name] = full[:end+1]
                    i += full[:end+1].count('\n') + 1
                    continue
        i += 1
    return funcs

funcs = extract_funcs(dctl)

def port(body):
    b = body.replace('__DEVICE__', 'inline')
    b = b.replace('make_float3', 'float3')
    b = b.replace('_powf','pow').replace('_fabsf','fabs').replace('_fabs','fabs')
    b = b.replace('_sqrtf','sqrt').replace('_expf','exp').replace('_log10f','log10')
    b = b.replace('_log2f','log2').replace('_atan2f','atan2').replace('_cosf','cos')
    b = b.replace('_sinf','sin').replace('_fmodf','fmod').replace('_fmaxf','fmax').replace('_fminf','fmin')
    b = b.replace('smootherstep','smoothstep')
    b = re.sub(r'(\d+\.\d+)ff', r'\1f', b)
    b = re.sub(r'(\d+)ff', r'\1f', b)
    return b

out = []
out.append("#include <metal_stdlib>\nusing namespace metal;\n")

out.append("""// ---- Params struct (identical to LaserLabParams.h) ----
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
""")

out.append("// ---- Matrix constants (constant address space) ----")
for fam, cols in families.items():
    out.append(f"// {fam}")
    for c in range(3):
        v = cols[c]
        v0 = v[0].rstrip('f'); v1 = v[1].rstrip('f'); v2 = v[2].rstrip('f')
        out.append(f"constant float3 {fam}_c{c} = float3({v0}f, {v1}f, {v2}f);")
    out.append("")

for w in ("WHITE_D65", "WHITE_D60"):
    v = mat[w]
    v0 = v[0].rstrip('f'); v1 = v[1].rstrip('f'); v2 = v[2].rstrip('f')
    out.append(f"constant float3 {w} = float3({v0}f, {v1}f, {v2}f);")

out.append("// ---- Scalar constants ----")
for k in ["LOGC3_CUT","LOGC3_A","LOGC3_B","LOGC3_C","LOGC3_D","LOGC3_E","LOGC3_F",
          "LOGC4_A","LOGC4_B","LOGC4_C","LOGC4_S2","LOGC4_T",
          "APPLE_R0","APPLE_RT","APPLE_SIGMA","APPLE_BETA","APPLE_GAMMA","APPLE_DELTA",
          "EXPO_FLASH_SCALE","PI"]:
    out.append(f"constant float {k} = {scalars[k]};")

helper_order = ['clampf','chroma','mmul','cbrt_s','spowf','cat16_adapt','d60_to_d65','d65_to_d60',
                'white_balance','printer_lights','g2x','x2g',
                'tf709d','tfl4d','tfl3d','tfs3d','tfvld','tflg10d','tfdwd','tfc2d','tfc3d','tfdld','tfapl','tfcctd','tfd',
                'tf709e','tfl4e','tfl3e','tfs3e','tfvle','tflg10e','tfdwe','tfc2e','tfc3e','tfdle','tfaple','tfccte','tfe',
                'l2o','o2l','work_to_oklab','oklab_to_work','awg4_ingamut','compress_oklab_chroma',
                'compress_to_gamut','rgb_sat','smootherstep','limiter','shape_tone_oklab',
                'deg_to_rad','mod2','mk_tp','lim_tp','interp_cyl','ev_curves']
for name in helper_order:
    if name in funcs:
        out.append("\n// " + name + "\n" + port(funcs[name]))

# transform -> llTransform
m = re.search(r'__DEVICE__ float3 transform\s*\([^)]*\)\s*\{(.*)\}', funcs['transform'], re.DOTALL)
body = m.group(1).rstrip().rstrip('}') if m else funcs['transform']

param_map = {
    'temp':'pr.temp','tint':'pr.tint','lift':'pr.lift','gamma':'pr.gamma','gain':'pr.gain','contrast':'pr.contrast',
    'shadows':'pr.shadows','highlights':'pr.highlights','desat_high':'pr.desatHigh','mid_detail':'pr.midDetail',
    'exp_global':'pr.expGlobal','exp_flash':'pr.expFlash','sat_amount':'pr.satAmount',
    'sat_compression':'pr.satCompression','hue':'pr.hueDeg','skin_hue':'pr.skinHueDeg',
    'skin_sat':'pr.skinSat','skin_val':'pr.skinVal','skin_evenness':'pr.skinEvenness',
    'global_sat':'pr.globalSat','lim_sat':'pr.limSat','lim_dens':'pr.limDens',
    'r_dens':'pr.rDens','y_dens':'pr.yDens','g_dens':'pr.gDens','c_dens':'pr.cDens',
    'b_dens':'pr.bDens','m_dens':'pr.mDens','pl_master':'pr.plMaster','pl_red':'pr.plRed',
    'pl_green':'pr.plGreen','pl_blue':'pr.plBlue','in_gamut':'pr.inputGamut','in_transfer':'pr.inputTransfer'
}
for k, v in param_map.items():
    body = re.sub(rf'\b{k}\b', v, body)

body = body.replace('make_float3','float3')
body = body.replace('_powf','pow').replace('_fabsf','fabs').replace('_fabs','fabs')
body = body.replace('_sqrtf','sqrt').replace('_expf','exp').replace('_log10f','log10')
body = body.replace('_log2f','log2').replace('_atan2f','atan2').replace('_cosf','cos')
body = body.replace('_sinf','sin').replace('_fmodf','fmod').replace('_fmaxf','fmax').replace('_fminf','fmin')
body = body.replace('smootherstep','smoothstep')

body = re.sub(r'int g=', 'int gamut_idx=', body)
body = re.sub(r'int ti_tf=', 'int transfer_idx=', body)
body = body.replace('ti_tf','transfer_idx')
body = body.replace(' g2x(g,','g2x(gamut_idx,')
body = body.replace('if(g==8)','if(gamut_idx==8)')
body = body.replace('tfe(cam,ti_tf)','tfe(cam,transfer_idx)')
body = body.replace('if(g==8)rxyz','if(gamut_idx==8)rxyz')
body = re.sub(r'(\d+\.\d+)ff', r'\1f', body)
body = re.sub(r'(\d+)ff', r'\1f', body)

lltransform = f"""
// -- llTransform: per-pixel grade (ported from DCTL transform()) --
inline float3 llTransform(constant LaserLabParams& pr, float r_in, float g_in, float b_in) {{
{body}
}}
"""
out.append(lltransform)

kernel = """
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
"""
out.append(kernel)

metal_src = '\n'.join(out)
open("src/LaserLabKernelSourceMetal.h", "w").write(metal_src)
print(f"Wrote Metal source: {len(metal_src)} chars, {metal_src.count(chr(10))} lines")