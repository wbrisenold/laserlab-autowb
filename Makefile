# LaserLab OFX build
# macOS:  make            (universal arm64 + x86_64, Metal + OpenCL)
# Install locally: make install

UNAME_SYSTEM := $(shell uname -s)

CXXFLAGS = -O2 -std=c++14 -fvisibility=hidden -Iofx/Support/include -Iofx/include -Isrc -Iplugin

ifeq ($(UNAME_SYSTEM), Linux)
	AMDAPP_PATH ?= /opt/AMDAPP
	CXXFLAGS += -I${AMDAPP_PATH}/include -fPIC
	CUDAPATH ?= /usr/local/cuda
	NVCC = ${CUDAPATH}/bin/nvcc
	NVCCFLAGS = --compiler-options="-fPIC" -Isrc
	LDFLAGS = -shared -fvisibility=hidden -L${CUDAPATH}/lib64 -lcuda -lcudart_static
	BUNDLE_DIR = LaserLab.ofx.bundle/Contents/Linux-x86-64/
	GPU_OBJ = LaserLabCudaKernel.o LaserLabOpenCLKernel.o
else
	MACOS_MIN = -mmacosx-version-min=11.0
	ARCH_FLAGS = -arch arm64 -arch x86_64 ${MACOS_MIN}
	CXXFLAGS += ${ARCH_FLAGS}
	LDFLAGS = -bundle -fvisibility=hidden -F/Library/Frameworks -framework OpenCL -framework Metal -framework AppKit
	LDFLAGS += ${ARCH_FLAGS}
	BUNDLE_DIR = LaserLab.ofx.bundle/Contents/MacOS/
	GPU_OBJ = LaserLabMetalKernel.o LaserLabOpenCLKernel.o
endif

.PHONY: all test test_metal install clean
all: LaserLab.ofx

LaserLab.ofx: LaserLabOfxMain.o LaserLabPlugin.o AutoWbPlugin.o LaserLabMetalKernel.o AutoWbMetalKernel.o LaserLabOpenCLKernel.o \
              ofxsCore.o ofxsImageEffect.o ofxsInteract.o ofxsLog.o ofxsMultiThread.o ofxsParams.o ofxsProperty.o ofxsPropertyValidation.o
	$(CXX) $^ -o $@ $(LDFLAGS)
	mkdir -p $(BUNDLE_DIR)
	cp LaserLab.ofx $(BUNDLE_DIR)
	cp Info.plist LaserLab.ofx.bundle/Contents/ 2>/dev/null || true

LaserLabOfxMain.o: src/LaserLabOfxMain.cpp src/LaserLabPlugin.h plugin/LaserLabParams.h
	$(CXX) -c src/LaserLabOfxMain.cpp $(CXXFLAGS) -o $@

LaserLabPlugin.o: src/LaserLabPlugin.cpp src/LaserLabPlugin.h plugin/LaserLabParams.h
	$(CXX) -c src/LaserLabPlugin.cpp $(CXXFLAGS) -o $@

AutoWbPlugin.o: src/AutoWbPlugin.cpp src/AutoWbPlugin.h plugin/LaserLabParams.h src/laserlab_core.h
	$(CXX) -c src/AutoWbPlugin.cpp $(CXXFLAGS) -o $@

AutoWbMetalKernel.o: src/AutoWbMetalKernel.mm src/AutoWbKernelSource.h plugin/LaserLabParams.h
	$(CXX) -c src/AutoWbMetalKernel.mm $(CXXFLAGS) -x objective-c++ -o $@

LaserLabMetalKernel.o: src/LaserLabMetalKernel.mm src/LaserLabKernelSourceStr.h plugin/LaserLabParams.h
	$(CXX) -c src/LaserLabMetalKernel.mm $(CXXFLAGS) -x objective-c++ -o $@

LaserLabOpenCLKernel.o: src/LaserLabOpenCLKernel.cpp src/LaserLabKernelSourceStr.h plugin/LaserLabParams.h
	$(CXX) -c src/LaserLabOpenCLKernel.cpp $(CXXFLAGS) -o $@

%.o: ofx/Support/Library/%.cpp
	$(CXX) -c $< $(CXXFLAGS) -o $@

# Test build
test: build/test_laserlab
	./build/test_laserlab

build/test_laserlab: tests/test_laserlab.cpp src/laserlab_core.h plugin/LaserLabParams.h
	mkdir -p build
	$(CXX) $(CXXFLAGS) tests/test_laserlab.cpp -o $@

test_metal: build/test_laserlab_metal
	./build/test_laserlab_metal

build/test_laserlab_metal: tests/test_laserlab_metal.mm src/laserlab_core.h plugin/LaserLabParams.h src/LaserLabMetalKernel.mm
	mkdir -p build
	$(CXX) $(CXXFLAGS) -framework Metal -framework Foundation tests/test_laserlab_metal.mm src/LaserLabMetalKernel.mm -o $@

test_autowb: build/test_autowb
	./build/test_autowb

build/test_autowb: tests/test_autowb.mm src/AutoWbMetalKernel.mm src/AutoWbKernelSource.h src/laserlab_core.h plugin/LaserLabParams.h
	mkdir -p build
	$(CXX) $(CXXFLAGS) -framework Metal -framework Foundation tests/test_autowb.mm src/AutoWbMetalKernel.mm -o $@

install: LaserLab.ofx
	sudo mkdir -p /Library/OFX/Plugins
	sudo rm -rf /Library/OFX/Plugins/LaserLab.ofx.bundle
	sudo cp -fr LaserLab.ofx.bundle /Library/OFX/Plugins/
	rm -rf "$$HOME/Library/Application Support/Blackmagic Design/DaVinci Resolve/OFX Plugins/LaserLab.ofx.bundle"

clean:
	rm -f *.o *.ofx
	rm -fr LaserLab.ofx.bundle build