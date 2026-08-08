// AutoWbMetalKernel.mm — Metal harness for the GPU-first Auto WB plugin.
// Compiles the self-contained AutoWbKernelSource and exposes:
//   RunMetalWbScan       — scan current frame on GPU, accumulate skin a/b/L stats
//   RunMetalWbTransform  — apply WB-only CAT16 grade on GPU
//
// The scan grid is bounded (gridW x gridH cells); per-cell weighted sums are
// written to a host-read buffer and reduced on the CPU (hundreds of floats,
// not the frame). The expensive per-pixel decode runs on the GPU.
#import <Metal/Metal.h>
#import <Foundation/Foundation.h>
#include "AutoWbKernelSource.h"
#include "LaserLabParams.h"

@interface AutoWbMetal : NSObject
@property (nonatomic, strong) id<MTLDevice> device;
@property (nonatomic, strong) id<MTLComputePipelineState> reducePipeline;
@property (nonatomic, strong) id<MTLComputePipelineState> transformPipeline;
@end

@implementation AutoWbMetal
- (instancetype)initWithDevice:(id<MTLDevice>)dev {
    self = [super init];
    if (self) {
        _device = dev;
        NSError* err = nil;
        id<MTLLibrary> lib = [dev newLibraryWithSource:@(kAutoWbKernelSource) options:nil error:&err];
        if (!lib || err) {
            FILE* f = fopen("/tmp/laserlab_metal.log", "a");
            if (f) { fprintf(f, "AWB COMPILE FAIL: %s\n", err ? [[err localizedDescription] UTF8String] : "nil"); fclose(f); }
            return nil;
        }
        id<MTLFunction> fnR = [lib newFunctionWithName:@"AutoWbReduceKernel"];
        id<MTLFunction> fnT = [lib newFunctionWithName:@"AutoWbTransformKernel"];
        if (!fnR || !fnT) {
            FILE* f = fopen("/tmp/laserlab_metal.log", "a");
            if (f) { fprintf(f, "AWB FUNCTION NOT FOUND\n"); fclose(f); }
            return nil;
        }
        _reducePipeline = [dev newComputePipelineStateWithFunction:fnR error:&err];
        if (!_reducePipeline || err) { fprintf(stderr, "AWB reduce pipeline fail\n"); return nil; }
        _transformPipeline = [dev newComputePipelineStateWithFunction:fnT error:&err];
        if (!_transformPipeline || err) { fprintf(stderr, "AWB transform pipeline fail\n"); return nil; }
    }
    return self;
}
@end

static AutoWbMetal* g_awb = nil;

// Scan: fills out with gridW*gridH float4(a*w, b*w, w, L*w) cells.
// Runs synchronously so the caller reads valid data immediately.
void RunMetalWbScan(void* p_CmdQ, const LaserLabParams& p_Params, int W, int H,
                    const void* p_Src, float* p_OutStats, int gridW, int gridH)
{
    @autoreleasepool {
        id<MTLCommandQueue> cmdQ = (__bridge id<MTLCommandQueue>)p_CmdQ;
        if (!g_awb) {
            id<MTLDevice> dev = cmdQ.device;
            g_awb = [[AutoWbMetal alloc] initWithDevice:dev];
            if (!g_awb || !g_awb.reducePipeline) return;
        }

        id<MTLBuffer> srcBuf = (__bridge id<MTLBuffer>)(const_cast<void*>(p_Src));
        id<MTLBuffer> paramsBuf = [g_awb.device newBufferWithBytes:&p_Params length:sizeof(LaserLabParams) options:MTLResourceStorageModeShared];
        id<MTLBuffer> wBuf = [g_awb.device newBufferWithBytes:&W length:sizeof(int) options:MTLResourceStorageModeShared];
        id<MTLBuffer> hBuf = [g_awb.device newBufferWithBytes:&H length:sizeof(int) options:MTLResourceStorageModeShared];
        id<MTLBuffer> gwBuf = [g_awb.device newBufferWithBytes:&gridW length:sizeof(int) options:MTLResourceStorageModeShared];
        id<MTLBuffer> ghBuf = [g_awb.device newBufferWithBytes:&gridH length:sizeof(int) options:MTLResourceStorageModeShared];
        id<MTLBuffer> outBuf = [g_awb.device newBufferWithLength:(NSUInteger)gridW * gridH * 4 * sizeof(float) options:MTLResourceStorageModeShared];

        id<MTLCommandBuffer> cmdBuf = [cmdQ commandBuffer];
        id<MTLComputeCommandEncoder> enc = [cmdBuf computeCommandEncoder];
        [enc setComputePipelineState:g_awb.reducePipeline];
        [enc setBuffer:paramsBuf offset:0 atIndex:0];
        [enc setBuffer:wBuf offset:0 atIndex:1];
        [enc setBuffer:hBuf offset:0 atIndex:2];
        [enc setBuffer:gwBuf offset:0 atIndex:3];
        [enc setBuffer:ghBuf offset:0 atIndex:4];
        [enc setBuffer:srcBuf offset:0 atIndex:5];
        [enc setBuffer:outBuf offset:0 atIndex:6];
        MTLSize tg = MTLSizeMake(8, 8, 1);
        MTLSize grid = MTLSizeMake((gridW + 7) / 8, (gridH + 7) / 8, 1);
        [enc dispatchThreadgroups:grid threadsPerThreadgroup:tg];
        [enc endEncoding];
        [cmdBuf commit];
        [cmdBuf waitUntilCompleted];

        memcpy(p_OutStats, outBuf.contents, (NSUInteger)gridW * gridH * 4 * sizeof(float));
    }
}

// Applies the WB-only per-pixel grade. p_Src/p_Dst are Metal buffer handles
// (Resolve passes them via the GPU render path; kernel runs on GPU).
void RunMetalWbTransform(void* p_CmdQ, const LaserLabParams& p_Params, int W, int H,
                         const float* p_Src, float* p_Dst)
{
    @autoreleasepool {
        id<MTLCommandQueue> cmdQ = (__bridge id<MTLCommandQueue>)p_CmdQ;
        if (!g_awb) {
            id<MTLDevice> dev = cmdQ.device;
            g_awb = [[AutoWbMetal alloc] initWithDevice:dev];
            if (!g_awb || !g_awb.transformPipeline) return;
        }

        id<MTLBuffer> srcBuf = (__bridge id<MTLBuffer>)(const_cast<float*>(p_Src));
        id<MTLBuffer> dstBuf = (__bridge id<MTLBuffer>)(p_Dst);
        id<MTLBuffer> paramsBuf = [g_awb.device newBufferWithBytes:&p_Params length:sizeof(LaserLabParams) options:MTLResourceStorageModeShared];
        id<MTLBuffer> wBuf = [g_awb.device newBufferWithBytes:&W length:sizeof(int) options:MTLResourceStorageModeShared];
        id<MTLBuffer> hBuf = [g_awb.device newBufferWithBytes:&H length:sizeof(int) options:MTLResourceStorageModeShared];

        id<MTLCommandBuffer> cmd = [cmdQ commandBuffer];
        id<MTLComputeCommandEncoder> enc = [cmd computeCommandEncoder];
        [enc setComputePipelineState:g_awb.transformPipeline];
        [enc setBuffer:paramsBuf offset:0 atIndex:0];
        [enc setBuffer:wBuf offset:0 atIndex:1];
        [enc setBuffer:hBuf offset:0 atIndex:2];
        [enc setBuffer:srcBuf offset:0 atIndex:3];
        [enc setBuffer:dstBuf offset:0 atIndex:4];
        MTLSize tg = MTLSizeMake(16, 16, 1);
        MTLSize grid = MTLSizeMake((W + 15) / 16, (H + 15) / 16, 1);
        [enc dispatchThreadgroups:grid threadsPerThreadgroup:tg];
        [enc endEncoding];
        [cmd commit];
        [cmd waitUntilCompleted];
    }
}