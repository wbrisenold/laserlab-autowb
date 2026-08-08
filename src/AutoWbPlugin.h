// AutoWbPlugin.h — GPU-first Auto White Balance OFX plugin factory.
#ifndef AUTO_WB_PLUGIN_H
#define AUTO_WB_PLUGIN_H
#include "ofxsImageEffect.h"

class AutoWbPluginFactory : public OFX::PluginFactoryHelper<AutoWbPluginFactory> {
public:
    AutoWbPluginFactory();
    virtual void load() {}
    virtual void unload() {}
    virtual void describe(OFX::ImageEffectDescriptor& p_Desc);
    virtual void describeInContext(OFX::ImageEffectDescriptor& p_Desc, OFX::ContextEnum p_Context);
    virtual OFX::ImageEffect* createInstance(OfxImageEffectHandle p_Handle, OFX::ContextEnum p_Context);
};

namespace autowb {
    void registerAutoWb(OFX::PluginFactoryArray& p_FactoryArray);
}
#endif