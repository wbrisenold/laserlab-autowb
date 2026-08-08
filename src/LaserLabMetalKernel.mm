// LaserLabMetalKernel.mm — Metal implementation (BaldavengerOFX pattern)
#import <Metal/Metal.h>
#import <Foundation/Foundation.h>
#include "LaserLabParams.h"
#include "LaserLabKernelSourceStr.h"

@interface LaserLabMetal : NSObject
@property (nonatomic, strong) id<MTLDevice> device;
@property (nonatomic, strong) id<MTLComputePipelineState> pipeline;
@end

@implementation LaserLabMetal
- (instancetype)initWithDevice:(id<MTLDevice>)dev {
    self = [super init];
    if (self) {
        _device = dev;
        NSError* err = nil;
        id<MTLLibrary> lib = [dev newLibraryWithSource:@(kLaserLabKernelSource) options:nil error:&err];
        if (!lib || err) {
            FILE* f = fopen("/tmp/laserlab_metal.log", "a");
            if (f) { fprintf(f, "COMPILE FAIL: %s\n", err ? [[err localizedDescription] UTF8String] : "nil"); fclose(f); }
            return nil;
        }
        id<MTLFunction> fn = [lib newFunctionWithName:@"LaserLabKernel"];
        if (!fn) {
            FILE* f = fopen("/tmp/laserlab_metal.log", "a");
            if (f) { fprintf(f, "FUNCTION NOT FOUND\n"); fclose(f); }
            return nil;
        }
        _pipeline = [dev newComputePipelineStateWithFunction:fn error:&err];
        if (!_pipeline || err) {
            FILE* f = fopen("/tmp/laserlab_metal.log", "a");
            if (f) { fprintf(f, "PIPELINE FAIL: %s\n", err ? [[err localizedDescription] UTF8String] : "nil"); fclose(f); }
            return nil;
        }
        FILE* f = fopen("/tmp/laserlab_metal.log", "a");
        if (f) { fprintf(f, "PIPELINE OK maxThreads=%lu\n", _pipeline.maxTotalThreadsPerThreadgroup); fclose(f); }
    }
    return self;
}
@end

static LaserLabMetal* g_metal = nil;

void RunMetalLaserLab(void* p_CmdQ, int p_Width, int p_Height,
                      const LaserLabParams& p_Params,
                      const float* p_Src, float* p_Dst) {
    @autoreleasepool {
        id<MTLCommandQueue> cmdQ = (__bridge id<MTLCommandQueue>)p_CmdQ;
        if (!g_metal) {
            id<MTLDevice> dev = cmdQ.device;
            g_metal = [[LaserLabMetal alloc] initWithDevice:dev];
            if (!g_metal) return;
        }
        if (!g_metal.pipeline) return;

        // OFX host passes Metal buffer handles, not CPU pointers
        id<MTLBuffer> srcBuf = (__bridge id<MTLBuffer>)(const_cast<float*>(p_Src));
        id<MTLBuffer> dstBuf = (__bridge id<MTLBuffer>)(p_Dst);
        id<MTLBuffer> paramsBuf = [g_metal.device newBufferWithBytes:&p_Params length:sizeof(LaserLabParams) options:MTLResourceStorageModeShared];
        int W = p_Width, H = p_Height;
        id<MTLBuffer> wBuf = [g_metal.device newBufferWithBytes:&W length:sizeof(int) options:MTLResourceStorageModeShared];
        id<MTLBuffer> hBuf = [g_metal.device newBufferWithBytes:&H length:sizeof(int) options:MTLResourceStorageModeShared];

        id<MTLCommandBuffer> cmdBuf = [cmdQ commandBuffer];
        id<MTLComputeCommandEncoder> enc = [cmdBuf computeCommandEncoder];
        [enc setComputePipelineState:g_metal.pipeline];
        [enc setBuffer:paramsBuf offset:0 atIndex:0];
        [enc setBuffer:wBuf offset:0 atIndex:1];
        [enc setBuffer:hBuf offset:0 atIndex:2];
        [enc setBuffer:srcBuf offset:0 atIndex:3];
        [enc setBuffer:dstBuf offset:0 atIndex:4];

        MTLSize tg = MTLSizeMake(16, 16, 1);
        MTLSize grid = MTLSizeMake((p_Width + 15) / 16, (p_Height + 15) / 16, 1);
        [enc dispatchThreadgroups:grid threadsPerThreadgroup:tg];
        [enc endEncoding];

        [cmdBuf commit];
        [cmdBuf waitUntilCompleted];
    }
}
