#import <Metal/Metal.h>
#import <Foundation/Foundation.h>
#include <cstdio>
#include <vector>
#include <cmath>
#include "LaserLabParams.h"
#include "laserlab_core.h"
extern void RunMetalWbScan(void* q, const LaserLabParams& p_Params, int W, int H,
                           const void* p_Src, float* p_OutStats, int gridW, int gridH);
extern void RunMetalWbTransform(void* q, const LaserLabParams& p, int W, int H,
                                const float* p_Src, float* p_Dst);
using namespace laserlabcore;
int main(){
    id<MTLDevice> dev = MTLCreateSystemDefaultDevice();
    if(!dev){ printf("NO METAL\n"); return 2; }
    id<MTLCommandQueue> q = [dev newCommandQueue];
    const int W=320,H=180,GW=128,GH=72;
    // make an OFF-LINE skin: back-code a color whose OKLab hue is +8deg from the line
    float hue = 1.0685f + 8.0f*(2.0f*M_PI)/360.0f;
    Vec3 lab=v3(0.75f, 0.15f*cosf(hue), 0.15f*sinf(hue));
    Vec3 work=oklab_to_work(lab);
    Vec3 enc=tfe(x2g(1,g2x(1,work)), 1); // LogC4
    printf("seed rgb=%.4f %.4f %.4f\n",enc.x,enc.y,enc.z);
    std::vector<float> src(W*H*4);
    for(int y=0;y<H;y++)for(int x=0;x<W;x++){
        src[(y*W+x)*4+0]=enc.x; src[(y*W+x)*4+1]=enc.y; src[(y*W+x)*4+2]=enc.z; src[(y*W+x)*4+3]=1.0f;
    }
    id<MTLBuffer> sb=[dev newBufferWithBytes:src.data() length:W*H*4*4 options:MTLResourceStorageModeShared];
    LaserLabParams p={}; p.inputGamut=1; p.inputTransfer=1;
    std::vector<float> st(GW*GH*4,0);
    RunMetalWbScan((__bridge void*)q,p,W,H,(__bridge void*)sb,st.data(),GW,GH);
    double sa=0,sbB=0,sL=0,sw=0; int n=0;
    for(int i=0;i<GW*GH;i++){const float* c=&st[(size_t)i*4]; if(c[2]>0){sa+=c[0];sbB+=c[1];sL+=c[3];sw+=c[2];n++;}}
    printf("scan meanLab=%.3f %.3f %.3f cells=%d\n", n? sL/sw:0, n? sa/sw:0, n? sbB/sw:0, n);
    float temp=0,tint=0;
    solveSkinTempTint((float)(sL/sw),(float)(sa/sw),(float)(sbB/sw),temp,tint);
    printf("solved temp=%+.3f tint=%+.3f\n", temp,tint);
    // apply to a probe of the mean and re-measure hue
    Vec3 wb=white_balance(g2x(1,oklab_to_work(v3((float)(sL/sw),(float)(sa/sw),(float)(sbB/sw)))),temp,tint);
    Vec3 resl=work_to_oklab(x2g(1,wb));
    float herr=fabsf(atan2f(sinf(1.0685f-atan2f(resl.z,resl.y)),cosf(1.0685f-atan2f(resl.z,resl.y))));
    printf("after-WB hue err=%.4f rad (want <0.03)\n", herr);
    bool ok = n>0 && (temp*temp+tint*tint>1e-6) && herr<0.05f;
    printf("RESULT: %s\n", ok?"ALL PASS":"FAIL");
    return ok?0:1;
}
