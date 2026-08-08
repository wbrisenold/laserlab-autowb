// LaserLabPlugin.h — LaserLab OFX plugin declaration
// Free grading node for DaVinci Resolve. Faithful port of LaserLab.dctl.

#ifndef LASERLAB_PLUGIN_H
#define LASERLAB_PLUGIN_H

#include "ofxsImageEffect.h"
#include "LaserLabParams.h"

class LaserLabPluginFactory : public OFX::PluginFactoryHelper<LaserLabPluginFactory>
{
public:
    LaserLabPluginFactory();
    virtual void load() {}
    virtual void unload() {}
    virtual void describe(OFX::ImageEffectDescriptor& p_Desc);
    virtual void describeInContext(OFX::ImageEffectDescriptor& p_Desc, OFX::ContextEnum p_Context);
    virtual OFX::ImageEffect* createInstance(OfxImageEffectHandle p_Handle, OFX::ContextEnum p_Context);
};

namespace laserlabofx {
    void registerLaserLab(OFX::PluginFactoryArray& p_FactoryArray);
}

#endif