// test_laserlab_metal.mm — CPU↔Metal parity test for LaserLab
// Runs the REAL RunMetalLaserLab entry point (same one DaVinci Resolve calls)
// against the CPU reference (laserlab_core.h) and asserts they agree.
// Parity target: ~2e-5 mean absolute error.

#import <Metal/Metal.h>
#include <cstdio>
#include <cmath>
#include <vector>
#include <string>

#include "LaserLabParams.h"
#include "laserlab_core.h"

extern void RunMetalLaserLab(void* p_CmdQ, int p_Width, int p_Height,
                             const LaserLabParams& p_Params, const float* p_Src, float* p_Dst);

static int g_fail = 0;

static void check(bool ok, const char* name, const char* detail = "")
{
    printf("  [%s] %s %s\n", ok ? "PASS" : "FAIL", name, detail);
    if (!ok) g_fail++;
}

// Synthetic frame: varied gradients + extended values
static std::vector<float> makeFrame(int W, int H)
{
    std::vector<float> f(static_cast<size_t>(W) * H * 4);
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x) {
            const size_t i = (static_cast<size_t>(y) * W + x) * 4;
            // Per-channel different gradients spanning shadow to superwhite
            f[i + 0] = 0.02f + 1.5f * x / (W - 1);      // R: 0.02..1.52
            f[i + 1] = 0.01f + 1.2f * y / (H - 1);      // G: 0.01..1.21
            f[i + 2] = 0.005f + 0.9f * (x + y) / (W + H); // B: 0.005..0.91
            f[i + 3] = 1.0f; // opaque alpha
        }
    return f;
}

static void runParity(const LaserLabParams& params, const char* label)
{
    const int W = 64, H = 64;
    std::vector<float> src = makeFrame(W, H);
    std::vector<float> cpuDst(W * H * 4), gpuDst(W * H * 4);

    // CPU reference
    laserlabcore::renderFrame(src.data(), W, H, params, cpuDst.data());

    // Metal
    id<MTLDevice> dev = MTLCreateSystemDefaultDevice();
    if (!dev) { fprintf(stderr, "No Metal device\n"); return; }
    id<MTLCommandQueue> queue = [dev newCommandQueue];
    
    RunMetalLaserLab((__bridge void*)queue, W, H, params, src.data(), gpuDst.data());
    
    // Wait for GPU to complete
    [NSThread sleepForTimeInterval:1.0];

    // Compare
    double maxErr = 0.0, sumErr = 0.0;
    int n = W * H * 3; // RGB only
    for (int i = 0; i < n; ++i) {
        double err = std::fabs(cpuDst[i] - gpuDst[i]);
        maxErr = std::max(maxErr, err);
        sumErr += err;
    }
    double meanErr = sumErr / n;
    bool pass = maxErr < 1e-4 && meanErr < 2e-5;
    check(pass, label, (std::string("max=") + std::to_string(maxErr) + " mean=" + std::to_string(meanErr)).c_str());
}

int main()
{
    using namespace laserlabcore;

    // Test 1: DCTL defaults (non-neutral look)
    printf("Test 1: DCTL defaults\n");
    LaserLabParams p = {};
    p.inputGamut = 1; p.inputTransfer = 1;
    p.contrast = 1.06f; p.shadows = 0.05f; p.highlights = 0.05f;
    p.desatHigh = 0.05f; p.satAmount = 1.10f; p.satCompression = 0.15f;
    p.globalSat = 0.06f;
    p.plMaster = 25.0f; p.plRed = 25.0f; p.plGreen = 25.0f; p.plBlue = 25.0f;
    p.gamma = 1.0f; p.gain = 1.0f; p.skinSat = 1.0f;
    runParity(p, "defaults");

    // Test 2: All neutral
    printf("Test 2: All neutral\n");
    LaserLabParams p2 = {};
    p2.inputGamut = 1; p2.inputTransfer = 1;
    p2.plMaster = 25.0f; p2.plRed = 25.0f; p2.plGreen = 25.0f; p2.plBlue = 25.0f;
    p2.gamma = 1.0f; p2.gain = 1.0f; p2.skinSat = 1.0f;
    runParity(p2, "neutral");

    // Test 3: Heavy temp/tint
    printf("Test 3: Heavy temp/tint\n");
    LaserLabParams p3 = {};
    p3.inputGamut = 1; p3.inputTransfer = 1;
    p3.temp = 1.5f; p3.tint = -1.0f;
    p3.plMaster = 25.0f; p3.plRed = 25.0f; p3.plGreen = 25.0f; p3.plBlue = 25.0f;
    p3.gamma = 1.0f; p3.gain = 1.0f; p3.skinSat = 1.0f;
    runParity(p3, "temp/tint");

    // Test 4: Exposure extremes
    printf("Test 4: Exposure extremes\n");
    LaserLabParams p4 = {};
    p4.inputGamut = 1; p4.inputTransfer = 1;
    p4.expGlobal = 3.0f; p4.expFlash = 1.0f;
    p4.plMaster = 25.0f; p4.plRed = 25.0f; p4.plGreen = 25.0f; p4.plBlue = 25.0f;
    p4.gamma = 1.0f; p4.gain = 1.0f; p4.skinSat = 1.0f;
    runParity(p4, "exposure");

    // Test 5: Saturation + hue
    printf("Test 5: Saturation + hue\n");
    LaserLabParams p5 = {};
    p5.inputGamut = 1; p5.inputTransfer = 1;
    p5.satAmount = 2.5f; p5.satCompression = 0.8f; p5.hueDeg = 120.0f;
    p5.plMaster = 25.0f; p5.plRed = 25.0f; p5.plGreen = 25.0f; p5.plBlue = 25.0f;
    p5.gamma = 1.0f; p5.gain = 1.0f; p5.skinSat = 1.0f;
    runParity(p5, "sat/hue");

    // Test 6: Skin + density
    printf("Test 6: Skin + density\n");
    LaserLabParams p6 = {};
    p6.inputGamut = 1; p6.inputTransfer = 1;
    p6.skinHueDeg = 20.0f; p6.skinSat = 1.5f; p6.skinEvenness = 0.7f;
    p6.globalSat = 0.5f; p6.limSat = 0.3f; p6.rDens = 0.4f;
    p6.plMaster = 25.0f; p6.plRed = 25.0f; p6.plGreen = 25.0f; p6.plBlue = 25.0f;
    p6.gamma = 1.0f; p6.gain = 1.0f; p6.skinSat = 1.0f;
    runParity(p6, "skin/density");

    printf("--------------------------------\n");
    if (g_fail == 0) {
        printf("RESULT: ALL PASS - CPU/Metal parity within tolerance\n");
        return 0;
    } else {
        printf("RESULT: %d FAIL\n", g_fail);
        return 1;
    }
}
