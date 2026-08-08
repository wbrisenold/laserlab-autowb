// LaserLabOfxMain.cpp — OFX bundle entry point
#include "LaserLabPlugin.h"
#include "AutoWbPlugin.h"

void OFX::Plugin::getPluginIDs(OFX::PluginFactoryArray& p_FactoryArray)
{
    laserlabofx::registerLaserLab(p_FactoryArray);
    autowb::registerAutoWb(p_FactoryArray);
}
