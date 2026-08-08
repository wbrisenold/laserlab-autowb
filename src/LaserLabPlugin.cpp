// LaserLabPlugin.cpp — BaldavengerOFX pattern
#include "LaserLabPlugin.h"
#include "LaserLabParams.h"
#include "laserlab_core.h"
#include "ofxsProcessing.h"
#include <cmath>
#include <memory>

extern void RunMetalLaserLab(void* p_CmdQ, int p_Width, int p_Height,
                             const LaserLabParams& p_Params,
                             const float* p_Src, float* p_Dst);

class LaserLabProcessor : public OFX::ImageProcessor {
public:
    OFX::Image* _srcImg;
    LaserLabParams _params;
    explicit LaserLabProcessor(OFX::ImageEffect& e) : OFX::ImageProcessor(e), _srcImg(nullptr) {}
    void setSrcImg(OFX::Image* p) { _srcImg = p; }
    virtual void processImagesMetal() override {
#ifdef __APPLE__
        if (!_srcImg || !_dstImg) return;
        const OfxRectI& bounds = _srcImg->getBounds();
        int width = bounds.x2 - bounds.x1;
        int height = bounds.y2 - bounds.y1;
        float* input = static_cast<float*>(_srcImg->getPixelData());
        float* output = static_cast<float*>(_dstImg->getPixelData());
        if (input && output && width > 0 && height > 0)
            RunMetalLaserLab(_pMetalCmdQ, width, height, _params, input, output);
#endif
    }
    virtual void multiThreadProcessImages(OfxRectI) override {}
};

class LaserLabPlugin : public OFX::ImageEffect {
public:
    LaserLabPlugin(OfxImageEffectHandle h);
    virtual ~LaserLabPlugin() {}
    virtual void render(const OFX::RenderArguments& a);
    virtual bool isIdentity(const OFX::IsIdentityArguments&, OFX::Clip*&, double&);
    virtual void changedParam(const OFX::InstanceChangedArgs&, const std::string&);
    void setupAndProcess(LaserLabProcessor& proc, const OFX::RenderArguments& a);
private:
    OFX::Clip* m_Src; OFX::Clip* m_Dst;
    OFX::ChoiceParam* m_InputGamut; OFX::ChoiceParam* m_InputTransfer;
    OFX::DoubleParam* m_Temp; OFX::DoubleParam* m_Tint;
    OFX::DoubleParam* m_PlMaster; OFX::DoubleParam* m_PlRed; OFX::DoubleParam* m_PlGreen; OFX::DoubleParam* m_PlBlue;
    OFX::DoubleParam* m_Lift; OFX::DoubleParam* m_Gamma; OFX::DoubleParam* m_Gain; OFX::DoubleParam* m_Contrast;
    OFX::DoubleParam* m_Shadows; OFX::DoubleParam* m_Highlights; OFX::DoubleParam* m_DesatHigh; OFX::DoubleParam* m_MidDetail;
    OFX::DoubleParam* m_ExpGlobal; OFX::DoubleParam* m_ExpFlash;
    OFX::DoubleParam* m_SatAmount; OFX::DoubleParam* m_SatCompression; OFX::DoubleParam* m_HueDeg;
    OFX::DoubleParam* m_SkinHueDeg; OFX::DoubleParam* m_SkinSat; OFX::DoubleParam* m_SkinVal; OFX::DoubleParam* m_SkinEvenness;
    OFX::IntParam* m_SkinRadius;
    OFX::DoubleParam* m_GlobalSat; OFX::DoubleParam* m_LimSat; OFX::DoubleParam* m_LimDens;
    OFX::DoubleParam* m_RDens; OFX::DoubleParam* m_YDens; OFX::DoubleParam* m_GDens;
    OFX::DoubleParam* m_CDens; OFX::DoubleParam* m_BDens; OFX::DoubleParam* m_MDens;
    LaserLabParams gatherParams(double t);
};

LaserLabPlugin::LaserLabPlugin(OfxImageEffectHandle h) : OFX::ImageEffect(h) {
    m_Src = fetchClip("Source"); m_Dst = fetchClip("Output");
    m_InputGamut = fetchChoiceParam("inputGamut"); m_InputTransfer = fetchChoiceParam("inputTransfer");
    m_Temp = fetchDoubleParam("temp"); m_Tint = fetchDoubleParam("tint");
    m_PlMaster = fetchDoubleParam("plMaster"); m_PlRed = fetchDoubleParam("plRed");
    m_PlGreen = fetchDoubleParam("plGreen"); m_PlBlue = fetchDoubleParam("plBlue");
    m_Lift = fetchDoubleParam("lift"); m_Gamma = fetchDoubleParam("gamma");
    m_Gain = fetchDoubleParam("gain"); m_Contrast = fetchDoubleParam("contrast");
    m_Shadows = fetchDoubleParam("shadows"); m_Highlights = fetchDoubleParam("highlights");
    m_DesatHigh = fetchDoubleParam("desatHigh"); m_MidDetail = fetchDoubleParam("midDetail");
    m_ExpGlobal = fetchDoubleParam("expGlobal"); m_ExpFlash = fetchDoubleParam("expFlash");
    m_SatAmount = fetchDoubleParam("satAmount"); m_SatCompression = fetchDoubleParam("satCompression");
    m_HueDeg = fetchDoubleParam("hueDeg");
    m_SkinHueDeg = fetchDoubleParam("skinHueDeg"); m_SkinSat = fetchDoubleParam("skinSat");
    m_SkinVal = fetchDoubleParam("skinVal"); m_SkinEvenness = fetchDoubleParam("skinEvenness");
    m_SkinRadius = fetchIntParam("skinRadius");
    m_GlobalSat = fetchDoubleParam("globalSat"); m_LimSat = fetchDoubleParam("limSat");
    m_LimDens = fetchDoubleParam("limDens");
    m_RDens = fetchDoubleParam("rDens"); m_YDens = fetchDoubleParam("yDens");
    m_GDens = fetchDoubleParam("gDens"); m_CDens = fetchDoubleParam("cDens");
    m_BDens = fetchDoubleParam("bDens"); m_MDens = fetchDoubleParam("mDens");
}

LaserLabParams LaserLabPlugin::gatherParams(double t) {
    LaserLabParams p = {};
    int ig, it; m_InputGamut->getValueAtTime(t, ig); m_InputTransfer->getValueAtTime(t, it);
    p.inputGamut = ig; p.inputTransfer = it;
    p.temp = m_Temp->getValueAtTime(t); p.tint = m_Tint->getValueAtTime(t);
    p.plMaster = m_PlMaster->getValueAtTime(t); p.plRed = m_PlRed->getValueAtTime(t);
    p.plGreen = m_PlGreen->getValueAtTime(t); p.plBlue = m_PlBlue->getValueAtTime(t);
    p.lift = m_Lift->getValueAtTime(t); p.gamma = m_Gamma->getValueAtTime(t);
    p.gain = m_Gain->getValueAtTime(t); p.contrast = m_Contrast->getValueAtTime(t);
    p.shadows = m_Shadows->getValueAtTime(t); p.highlights = m_Highlights->getValueAtTime(t);
    p.desatHigh = m_DesatHigh->getValueAtTime(t); p.midDetail = m_MidDetail->getValueAtTime(t);
    p.expGlobal = m_ExpGlobal->getValueAtTime(t); p.expFlash = m_ExpFlash->getValueAtTime(t);
    p.satAmount = m_SatAmount->getValueAtTime(t); p.satCompression = m_SatCompression->getValueAtTime(t);
    p.hueDeg = m_HueDeg->getValueAtTime(t);
    p.skinHueDeg = m_SkinHueDeg->getValueAtTime(t); p.skinSat = m_SkinSat->getValueAtTime(t);
    p.skinVal = m_SkinVal->getValueAtTime(t); p.skinEvenness = m_SkinEvenness->getValueAtTime(t);
    p.skinRadius = (float)m_SkinRadius->getValueAtTime(t);
    p.autoWb = 0; p.autoWbStrength = 1.0f;
    p.autoWbLmsGainX = 1.0f; p.autoWbLmsGainY = 1.0f; p.autoWbLmsGainZ = 1.0f;
    p.globalSat = m_GlobalSat->getValueAtTime(t); p.limSat = m_LimSat->getValueAtTime(t);
    p.limDens = m_LimDens->getValueAtTime(t);
    p.rDens = m_RDens->getValueAtTime(t); p.yDens = m_YDens->getValueAtTime(t);
    p.gDens = m_GDens->getValueAtTime(t); p.cDens = m_CDens->getValueAtTime(t);
    p.bDens = m_BDens->getValueAtTime(t); p.mDens = m_MDens->getValueAtTime(t);
    return p;
}

void LaserLabPlugin::render(const OFX::RenderArguments& a) {
    if ((m_Dst->getPixelDepth() == OFX::eBitDepthFloat) && (m_Dst->getPixelComponents() == OFX::ePixelComponentRGBA)) {
        LaserLabProcessor proc(*this);
        setupAndProcess(proc, a);
    } else {
        OFX::throwSuiteStatusException(kOfxStatErrUnsupported);
    }
}

void LaserLabPlugin::setupAndProcess(LaserLabProcessor& proc, const OFX::RenderArguments& a) {
    std::unique_ptr<OFX::Image> dst(m_Dst->fetchImage(a.time));
    std::unique_ptr<OFX::Image> src(m_Src->fetchImage(a.time));
    if (!dst || !src) OFX::throwSuiteStatusException(kOfxStatFailed);
    if (src->getPixelDepth() != dst->getPixelDepth() || src->getPixelComponents() != dst->getPixelComponents())
        OFX::throwSuiteStatusException(kOfxStatErrValue);
    proc.setDstImg(dst.get());
    proc.setSrcImg(src.get());
    proc.setGPURenderArgs(a);
    proc.setRenderWindow(a.renderWindow);
    proc._params = gatherParams(a.time);
    proc.process();
}

bool LaserLabPlugin::isIdentity(const OFX::IsIdentityArguments&, OFX::Clip*&, double&) {
    return false;
}

void LaserLabPlugin::changedParam(const OFX::InstanceChangedArgs&, const std::string&) {}

// ---- Describe ----
LaserLabPluginFactory::LaserLabPluginFactory() : OFX::PluginFactoryHelper<LaserLabPluginFactory>("com.laserlab.fx", 1, 0) {}

void LaserLabPluginFactory::describe(OFX::ImageEffectDescriptor& d) {
    d.setLabels("LaserLab", "LaserLab", "LaserLab");
    d.setPluginGrouping("Color/LaserLab");
    d.addSupportedContext(OFX::eContextFilter);
    d.addSupportedBitDepth(OFX::eBitDepthFloat);
    d.setSupportsMultipleClipPARs(false);
    d.setSupportsTiles(false);
    d.setTemporalClipAccess(true);
    d.setRenderTwiceAlways(false);
    d.setSupportsMultipleClipDepths(false);
    d.setSupportsMetalRender(true);
}

void LaserLabPluginFactory::describeInContext(OFX::ImageEffectDescriptor& d, OFX::ContextEnum) {
    OFX::ClipDescriptor* src = d.defineClip("Source");
    src->addSupportedComponent(OFX::ePixelComponentRGBA);
    src->setTemporalClipAccess(true);
    src->setSupportsTiles(false);
    OFX::ClipDescriptor* dst = d.defineClip("Output");
    dst->addSupportedComponent(OFX::ePixelComponentRGBA);
    dst->setSupportsTiles(false);

    OFX::PageParamDescriptor* p;

    p = d.definePageParam("pageInput"); p->setLabel("📥 Input");
    OFX::ChoiceParamDescriptor* pg = d.defineChoiceParam("inputGamut"); pg->setLabel("Gamut");
    const char* gamuts[] = {"Rec709","ARRI AWG4","ARRI AWG3","Sony S-Gamut3","Panasonic V-Gamut","RED Wide Gamut","BMD WG","Canon Cinema Gamut","ACEScg","DJI D-Gamut","Rec2020"};
    for (const char* g : gamuts) pg->appendOption(g); pg->setDefault(1);
    OFX::ChoiceParamDescriptor* pt = d.defineChoiceParam("inputTransfer"); pt->setLabel("Transfer");
    const char* tfs[] = {"Rec709","ARRI LogC4","ARRI LogC3","Sony S-Log3","Panasonic V-Log","RED Log3G10","BMD DWG","Canon Cinema (lin)","Canon Log2","Canon Log3","DJI D-Log","Linear","ACEScg","Apple Log","ACEScct"};
    for (const char* tf : tfs) pt->appendOption(tf); pt->setDefault(1);

    p = d.definePageParam("pageWB"); p->setLabel("⚖️ White Balance");
    OFX::DoubleParamDescriptor* tp = d.defineDoubleParam("temp"); tp->setLabel("Temp"); tp->setDefault(0.0); tp->setRange(-2.0, 2.0); tp->setIncrement(0.001); tp->setDisplayRange(-2.0, 2.0);
    OFX::DoubleParamDescriptor* ti = d.defineDoubleParam("tint"); ti->setLabel("Tint"); ti->setDefault(0.0); ti->setRange(-1.5, 1.5); ti->setIncrement(0.001); ti->setDisplayRange(-1.5, 1.5);

    p = d.definePageParam("pagePL"); p->setLabel("🎞 Printer Lights");
    OFX::DoubleParamDescriptor* plm = d.defineDoubleParam("plMaster"); plm->setLabel("Master"); plm->setDefault(25.0); plm->setRange(0.0, 50.0); plm->setIncrement(1.0); plm->setDisplayRange(0.0, 50.0);
    OFX::DoubleParamDescriptor* plr = d.defineDoubleParam("plRed"); plr->setLabel("Red"); plr->setDefault(25.0); plr->setRange(0.0, 50.0); plr->setIncrement(1.0); plr->setDisplayRange(0.0, 50.0);
    OFX::DoubleParamDescriptor* plg = d.defineDoubleParam("plGreen"); plg->setLabel("Green"); plg->setDefault(25.0); plg->setRange(0.0, 50.0); plg->setIncrement(1.0); plg->setDisplayRange(0.0, 50.0);
    OFX::DoubleParamDescriptor* plb = d.defineDoubleParam("plBlue"); plb->setLabel("Blue"); plb->setDefault(25.0); plb->setRange(0.0, 50.0); plb->setIncrement(1.0); plb->setDisplayRange(0.0, 50.0);

    p = d.definePageParam("pagePrim"); p->setLabel("🎚 Primaries");
    OFX::DoubleParamDescriptor* lf = d.defineDoubleParam("lift"); lf->setLabel("Lift"); lf->setDefault(0.0); lf->setRange(-0.5, 0.5); lf->setIncrement(0.001); lf->setDisplayRange(-0.5, 0.5);
    OFX::DoubleParamDescriptor* ga = d.defineDoubleParam("gamma"); ga->setLabel("Gamma"); ga->setDefault(1.0); ga->setRange(0.2, 3.0); ga->setIncrement(0.001); ga->setDisplayRange(0.2, 3.0);
    OFX::DoubleParamDescriptor* gn = d.defineDoubleParam("gain"); gn->setLabel("Gain"); gn->setDefault(1.0); gn->setRange(0.0, 4.0); gn->setIncrement(0.001); gn->setDisplayRange(0.0, 4.0);
    OFX::DoubleParamDescriptor* ct = d.defineDoubleParam("contrast"); ct->setLabel("Contrast"); ct->setDefault(1.06); ct->setRange(0.5, 1.5); ct->setIncrement(0.001); ct->setDisplayRange(0.5, 1.5);

    p = d.definePageParam("pageTone"); p->setLabel("🌗 Tone");
    OFX::DoubleParamDescriptor* sh = d.defineDoubleParam("shadows"); sh->setLabel("Shadows"); sh->setDefault(0.05); sh->setRange(-1.0, 1.0); sh->setIncrement(0.05); sh->setDisplayRange(-1.0, 1.0);
    OFX::DoubleParamDescriptor* hl = d.defineDoubleParam("highlights"); hl->setLabel("Highlights"); hl->setDefault(0.05); hl->setRange(-1.0, 1.0); hl->setIncrement(0.05); hl->setDisplayRange(-1.0, 1.0);
    OFX::DoubleParamDescriptor* dh = d.defineDoubleParam("desatHigh"); dh->setLabel("Desat Highlights"); dh->setDefault(0.05); dh->setRange(0.0, 1.0); dh->setIncrement(0.05); dh->setDisplayRange(0.0, 1.0);
    OFX::DoubleParamDescriptor* md = d.defineDoubleParam("midDetail"); md->setLabel("Mid Chroma"); md->setDefault(0.0); md->setRange(0.0, 1.0); md->setIncrement(0.05); md->setDisplayRange(0.0, 1.0);

    p = d.definePageParam("pageExp"); p->setLabel("☀️ Exposure");
    OFX::DoubleParamDescriptor* eg = d.defineDoubleParam("expGlobal"); eg->setLabel("Exposure"); eg->setDefault(0.0); eg->setRange(-5.0, 5.0); eg->setIncrement(0.05); eg->setDisplayRange(-5.0, 5.0);
    OFX::DoubleParamDescriptor* ef = d.defineDoubleParam("expFlash"); ef->setLabel("Flash"); ef->setDefault(0.0); ef->setRange(-2.0, 2.0); ef->setIncrement(0.1); ef->setDisplayRange(-2.0, 2.0);

    p = d.definePageParam("pageCol"); p->setLabel("🌈 Color");
    OFX::DoubleParamDescriptor* sa = d.defineDoubleParam("satAmount"); sa->setLabel("Sat Amount"); sa->setDefault(1.10); sa->setRange(0.0, 3.0); sa->setIncrement(0.05); sa->setDisplayRange(0.0, 3.0);
    OFX::DoubleParamDescriptor* sc = d.defineDoubleParam("satCompression"); sc->setLabel("Sat Compression"); sc->setDefault(0.15); sc->setRange(0.0, 1.0); sc->setIncrement(0.05); sc->setDisplayRange(0.0, 1.0);
    OFX::DoubleParamDescriptor* hu = d.defineDoubleParam("hueDeg"); hu->setLabel("Hue Rotate"); hu->setDefault(0.0); hu->setRange(-180.0, 180.0); hu->setIncrement(0.1); hu->setDisplayRange(-180.0, 180.0);

    p = d.definePageParam("pageSkin"); p->setLabel("🧑 Skin");
    OFX::DoubleParamDescriptor* skH = d.defineDoubleParam("skinHueDeg"); skH->setLabel("Skin Hue"); skH->setDefault(0.0); skH->setRange(-45.0, 45.0); skH->setIncrement(1.0); skH->setDisplayRange(-45.0, 45.0);
    OFX::DoubleParamDescriptor* skS = d.defineDoubleParam("skinSat"); skS->setLabel("Skin Sat"); skS->setDefault(1.0); skS->setRange(0.0, 2.0); skS->setIncrement(0.05); skS->setDisplayRange(0.0, 2.0);
    OFX::DoubleParamDescriptor* skV = d.defineDoubleParam("skinVal"); skV->setLabel("Skin Val"); skV->setDefault(0.0); skV->setRange(-0.5, 0.5); skV->setIncrement(0.05); skV->setDisplayRange(-0.5, 0.5);
    OFX::DoubleParamDescriptor* skE = d.defineDoubleParam("skinEvenness"); skE->setLabel("Skin Evenness"); skE->setDefault(0.0); skE->setRange(0.0, 1.0); skE->setIncrement(0.05); skE->setDisplayRange(0.0, 1.0);
    OFX::IntParamDescriptor* skR = d.defineIntParam("skinRadius"); skR->setLabel("Skin Radius"); skR->setDefault(4); skR->setRange(1, 12); skR->setDisplayRange(1, 12);

    p = d.definePageParam("pageDens"); p->setLabel("🧪 Density");
    OFX::DoubleParamDescriptor* gs = d.defineDoubleParam("globalSat"); gs->setLabel("Sub Sat"); gs->setDefault(0.06); gs->setRange(-1.0, 1.0); gs->setIncrement(0.001); gs->setDisplayRange(-1.0, 1.0);
    OFX::DoubleParamDescriptor* ls = d.defineDoubleParam("limSat"); ls->setLabel("Deep Sat"); ls->setDefault(0.0); ls->setRange(0.0, 1.0); ls->setIncrement(0.001); ls->setDisplayRange(0.0, 1.0);
    OFX::DoubleParamDescriptor* ld = d.defineDoubleParam("limDens"); ld->setLabel("Deep Dens"); ld->setDefault(0.0); ld->setRange(0.0, 1.0); ld->setIncrement(0.001); ld->setDisplayRange(0.0, 1.0);
    OFX::DoubleParamDescriptor* rd = d.defineDoubleParam("rDens"); rd->setLabel("Red Dens"); rd->setDefault(0.0); rd->setRange(0.0, 1.0); rd->setIncrement(0.001); rd->setDisplayRange(0.0, 1.0);
    OFX::DoubleParamDescriptor* yd = d.defineDoubleParam("yDens"); yd->setLabel("Yel Dens"); yd->setDefault(0.0); yd->setRange(0.0, 1.0); yd->setIncrement(0.001); yd->setDisplayRange(0.0, 1.0);
    OFX::DoubleParamDescriptor* gdn = d.defineDoubleParam("gDens"); gdn->setLabel("Grn Dens"); gdn->setDefault(0.0); gdn->setRange(0.0, 1.0); gdn->setIncrement(0.001); gdn->setDisplayRange(0.0, 1.0);
    OFX::DoubleParamDescriptor* cd = d.defineDoubleParam("cDens"); cd->setLabel("Cyn Dens"); cd->setDefault(0.0); cd->setRange(0.0, 1.0); cd->setIncrement(0.001); cd->setDisplayRange(0.0, 1.0);
    OFX::DoubleParamDescriptor* bd = d.defineDoubleParam("bDens"); bd->setLabel("Blu Dens"); bd->setDefault(0.0); bd->setRange(0.0, 1.0); bd->setIncrement(0.001); bd->setDisplayRange(0.0, 1.0);
    OFX::DoubleParamDescriptor* mdn = d.defineDoubleParam("mDens"); mdn->setLabel("Mag Dens"); mdn->setDefault(0.0); mdn->setRange(0.0, 1.0); mdn->setIncrement(0.001); mdn->setDisplayRange(0.0, 1.0);
}

OFX::ImageEffect* LaserLabPluginFactory::createInstance(OfxImageEffectHandle h, OFX::ContextEnum) {
    return new LaserLabPlugin(h);
}

namespace laserlabofx {
    void registerLaserLab(OFX::PluginFactoryArray& a) { a.push_back(new LaserLabPluginFactory()); }
}
