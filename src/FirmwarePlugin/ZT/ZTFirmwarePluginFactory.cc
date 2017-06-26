/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/


/// @file
///     @author Don Gagne <don@thegagnes.com>

#include "ZTFirmwarePluginFactory.h"
#include "ZT/ZTFirmwarePlugin.h"
ZTFirmwarePluginFactory ZTFirmwarePluginFactory;

ZTFirmwarePluginFactory::ZTFirmwarePluginFactory(void)
    : _pluginInstance(NULL)
{

}

QList<MAV_AUTOPILOT> ZTFirmwarePluginFactory::supportedFirmwareTypes(void) const
{
    QList<MAV_AUTOPILOT> list;

    list.append(MAV_AUTOPILOT_ENUM_END);
    return list;
}

FirmwarePlugin* ZTFirmwarePluginFactory::firmwarePluginForAutopilot(MAV_AUTOPILOT autopilotType, MAV_TYPE vehicleType)
{
    Q_UNUSED(vehicleType);

    if (autopilotType == MAV_AUTOPILOT_ENUM_END) {
        if (!_pluginInstance) {
            _pluginInstance = new ZTFirmwarePlugin;
        }
        return _pluginInstance;
    }

    return NULL;
}
