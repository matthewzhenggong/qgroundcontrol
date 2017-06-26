/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#ifndef ZTFirmwarePluginFactory_H
#define ZTFirmwarePluginFactory_H

#include "FirmwarePlugin.h"

class ZTFirmwarePlugin;

class ZTFirmwarePluginFactory : public FirmwarePluginFactory
{
    Q_OBJECT

public:
    ZTFirmwarePluginFactory(void);

    QList<MAV_AUTOPILOT>    supportedFirmwareTypes      (void) const final;
    FirmwarePlugin*         firmwarePluginForAutopilot  (MAV_AUTOPILOT autopilotType, MAV_TYPE vehicleType) final;

private:
    ZTFirmwarePlugin*  _pluginInstance;
};

#endif
