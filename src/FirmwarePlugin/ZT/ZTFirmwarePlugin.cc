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

#include "ZTFirmwarePlugin.h"
#include "ZTParameterMetaData.h"
#include "QGCApplication.h"

#include <QDebug>

#include "zt_custom_mode.h"

ZTFirmwarePluginInstanceData::ZTFirmwarePluginInstanceData(QObject* parent)
    : QObject(parent)
    , versionNotified(false)
{

}

ZTFirmwarePlugin::ZTFirmwarePlugin(void)
    : _manualFlightMode(tr("Manual"))
    , _acroFlightMode(tr("Acro"))
    , _stabilizedFlightMode(tr("Stabilized"))
    , _rattitudeFlightMode(tr("Rattitude"))
    , _altCtlFlightMode(tr("Altitude"))
    , _posCtlFlightMode(tr("Position"))
    , _offboardFlightMode(tr("Offboard"))
    , _readyFlightMode(tr("Ready"))
    , _takeoffFlightMode(tr("Takeoff"))
    , _holdFlightMode(tr("Hold"))
    , _missionFlightMode(tr("Mission"))
    , _rtlFlightMode(tr("Return"))
    , _landingFlightMode(tr("Land"))
    , _rtgsFlightMode(tr("Return to Groundstation"))
    , _followMeFlightMode(tr("Follow Me"))
    , _simpleFlightMode(tr("Simple"))
{

    struct Modes2Name {
        uint32_t    mode;
        bool        canBeSet;   ///< true: Vehicle can be set to this flight mode
        bool        fixedWing;  /// fixed wing compatible
        bool        multiRotor;  /// multi rotor compatible
    };

    static const struct Modes2Name rgModes2Name[] = {
        //main_mode                          canBeSet  FW      MC
        { ZT_CUSTOM_MAIN_MODE_MANUAL,       true,   true,   true },
        { ZT_CUSTOM_MAIN_MODE_STABILIZED,   true,   true,   true },
        { ZT_CUSTOM_MAIN_MODE_RATTITUDE,    true,   true,   true },
        { ZT_CUSTOM_MAIN_MODE_ALTCTL,        true,   true,   true },
        { ZT_CUSTOM_MAIN_MODE_POSCTL,          true,   true,   true },
        { ZT_CUSTOM_MAIN_MODE_SIMPLE,         true,   false,  true },
        { ZT_CUSTOM_MAIN_MODE_ACRO,         true,   false,  true },
    };
    {

    }

    // Must be in same order as above structure
    const QString* rgModeNames[] = {
        &_manualFlightMode,
        &_stabilizedFlightMode,
        &_rattitudeFlightMode,
        &_altCtlFlightMode,
        &_posCtlFlightMode,
        &_simpleFlightMode,
        &_acroFlightMode,
    };

    // Convert static information to dynamic list. This allows for plugin override class to manipulate list.
    for (size_t i=0; i<sizeof(rgModes2Name)/sizeof(rgModes2Name[0]); i++) {
        const struct Modes2Name* pModes2Name = &rgModes2Name[i];

        FlightModeInfo_t info;

        info.mode      =    pModes2Name->mode;
        info.name      =    rgModeNames[i];
        info.canBeSet  =    pModes2Name->canBeSet;
        info.fixedWing =    pModes2Name->fixedWing;
        info.multiRotor =   pModes2Name->multiRotor;

        _flightModeInfoList.append(info);
    }
}

QList<VehicleComponent*> ZTFirmwarePlugin::componentsForVehicle(AutoPilotPlugin* vehicle)
{
    Q_UNUSED(vehicle);

    return QList<VehicleComponent*>();
}

QStringList ZTFirmwarePlugin::flightModes(Vehicle* vehicle)
{
    QStringList flightModes;

    foreach (const FlightModeInfo_t& info, _flightModeInfoList) {
        if (info.canBeSet) {
            bool fw = (vehicle->fixedWing() && info.fixedWing);
            bool mc = (vehicle->multiRotor() && info.multiRotor);

            // show all modes for generic, vtol, etc
            bool other = !vehicle->fixedWing() && !vehicle->multiRotor();

            if (fw || mc || other) {
                flightModes += *info.name;
            }
        }
    }

    return flightModes;
}

QString ZTFirmwarePlugin::flightMode(uint8_t base_mode, uint32_t custom_mode) const
{
    QString flightMode = "Unknown";

    if (base_mode & MAV_MODE_FLAG_CUSTOM_MODE_ENABLED) {

        bool found = false;
        foreach (const FlightModeInfo_t& info, _flightModeInfoList) {
            if (info.mode == custom_mode) {
                flightMode = *info.name;
                found = true;
                break;
            }
        }

        if (!found) {
            qWarning() << "Unknown flight mode" << custom_mode;
            return tr("Unknown %1:%2").arg(base_mode).arg(custom_mode);
        }
    } else {
        qWarning() << "ZT Flight Stack flight mode without custom mode enabled?";
    }

    return flightMode;
}

bool ZTFirmwarePlugin::setFlightMode(const QString& flightMode, uint8_t* base_mode, uint32_t* custom_mode)
{
    *base_mode = 0;
    *custom_mode = 0;

    bool found = false;
    foreach (const FlightModeInfo_t& info, _flightModeInfoList) {
        if (flightMode.compare(info.name, Qt::CaseInsensitive) == 0) {
            *base_mode = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED;
            *custom_mode = info.mode;

            found = true;
            break;
        }
    }

    if (!found) {
        qWarning() << "Unknown flight Mode" << flightMode;
    }

    return found;
}

int ZTFirmwarePlugin::manualControlReservedButtonCount(void)
{
    return 0;   // 0 buttons reserved for rc switch simulation
}

bool ZTFirmwarePlugin::supportsManualControl(void)
{
    return true;
}

bool ZTFirmwarePlugin::isCapable(const Vehicle *vehicle, FirmwareCapabilities capabilities)
{
    if (vehicle->multiRotor()) {
        return (capabilities & (MavCmdPreflightStorageCapability | GuidedModeCapability | SetFlightModeCapability | PauseVehicleCapability /*| OrbitModeCapability still NYI*/)) == capabilities;
    } else {
        return (capabilities & (MavCmdPreflightStorageCapability | GuidedModeCapability | SetFlightModeCapability | PauseVehicleCapability)) == capabilities;
    }
}

void ZTFirmwarePlugin::initializeVehicle(Vehicle* vehicle)
{
    vehicle->setFirmwarePluginInstanceData(new ZTFirmwarePluginInstanceData);
}

bool ZTFirmwarePlugin::sendHomePositionToVehicle(void)
{
    // ZT stack does not want home position sent in the first position.
    // Subsequent sequence numbers must be adjusted.
    return false;
}

void ZTFirmwarePlugin::addMetaDataToFact(QObject* parameterMetaData, Fact* fact, MAV_TYPE vehicleType)
{
    ZTParameterMetaData* ztMetaData = qobject_cast<ZTParameterMetaData*>(parameterMetaData);

    if (ztMetaData) {
        ztMetaData->addMetaDataToFact(fact, vehicleType);
    } else {
        qWarning() << "Internal error: pointer passed to ZTFirmwarePlugin::addMetaDataToFact not ZTParameterMetaData";
    }
}

QList<MAV_CMD> ZTFirmwarePlugin::supportedMissionCommands(void)
{
    QList<MAV_CMD> list;

    list << MAV_CMD_NAV_WAYPOINT
         //<< MAV_CMD_NAV_LOITER_UNLIM << MAV_CMD_NAV_LOITER_TIME << MAV_CMD_NAV_LOITER_TO_ALT
         //<< MAV_CMD_NAV_LAND << MAV_CMD_NAV_TAKEOFF
         << MAV_CMD_DO_JUMP
         << MAV_CMD_DO_VTOL_TRANSITION << MAV_CMD_NAV_VTOL_TAKEOFF << MAV_CMD_NAV_VTOL_LAND
         //<< MAV_CMD_DO_DIGICAM_CONTROL
         //<< MAV_CMD_DO_SET_CAM_TRIGG_DIST
         << MAV_CMD_DO_SET_SERVO
         << MAV_CMD_DO_CHANGE_SPEED
         << MAV_CMD_DO_LAND_START
         << MAV_CMD_DO_MOUNT_CONFIGURE
         << MAV_CMD_DO_MOUNT_CONTROL
         //<< MAV_CMD_SET_CAMERA_MODE
         //<< MAV_CMD_IMAGE_START_CAPTURE << MAV_CMD_IMAGE_STOP_CAPTURE << MAV_CMD_VIDEO_START_CAPTURE << MAV_CMD_VIDEO_STOP_CAPTURE
         //<< MAV_CMD_NAV_DELAY
         ;

    return list;
}

QString ZTFirmwarePlugin::missionCommandOverrides(MAV_TYPE vehicleType) const
{
   return QStringLiteral(":/json/ZT/MavCmdInfoCommon.json");
}

QObject* ZTFirmwarePlugin::loadParameterMetaData(const QString& metaDataFile)
{
    ZTParameterMetaData* metaData = new ZTParameterMetaData;
    if (!metaDataFile.isEmpty()) {
        metaData->loadParameterFactMetaDataFile(metaDataFile);
    }
    return metaData;
}

void ZTFirmwarePlugin::pauseVehicle(Vehicle* vehicle)
{
    vehicle->sendMavCommand(vehicle->defaultComponentId(),
                            MAV_CMD_DO_REPOSITION,
                            true,   // show error if failed
                            -1.0f,
                            MAV_DO_REPOSITION_FLAGS_CHANGE_MODE,
                            0.0f,
                            NAN,
                            NAN,
                            NAN,
                            NAN);
}

void ZTFirmwarePlugin::guidedModeRTL(Vehicle* vehicle)
{
    _setFlightModeAndValidate(vehicle, _rtlFlightMode);
}

void ZTFirmwarePlugin::guidedModeLand(Vehicle* vehicle)
{
    _setFlightModeAndValidate(vehicle, _landingFlightMode);
}

void ZTFirmwarePlugin::guidedModeOrbit(Vehicle* vehicle, const QGeoCoordinate& centerCoord, double radius, double velocity, double altitude)
{
    if (!isGuidedMode(vehicle)) {
        setGuidedMode(vehicle, true);
    }

    vehicle->sendMavCommand(vehicle->defaultComponentId(),
                            MAV_CMD_SET_GUIDED_SUBMODE_CIRCLE,
                            true,   // show error if fails
                            radius,
                            velocity,
                            altitude,
                            NAN,
                            centerCoord.isValid() ? centerCoord.latitude()  : NAN,
                            centerCoord.isValid() ? centerCoord.longitude() : NAN,
                            centerCoord.isValid() ? centerCoord.altitude()  : NAN);
}

void ZTFirmwarePlugin::_mavCommandResult(int vehicleId, int component, int command, int result, bool noReponseFromVehicle)
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(component);
    Q_UNUSED(noReponseFromVehicle);

    Vehicle* vehicle = dynamic_cast<Vehicle*>(sender());
    if (!vehicle) {
        qWarning() << "Dynamic cast failed!";
        return;
    }

    if (command == MAV_CMD_NAV_TAKEOFF && result == MAV_RESULT_ACCEPTED) {
        // Now that we are in takeoff mode we can arm the vehicle which will cause it to takeoff.
        // We specifically don't retry arming if it fails. This way we don't fight with the user if
        // They are trying to disarm.
        disconnect(vehicle, &Vehicle::mavCommandResult, this, &ZTFirmwarePlugin::_mavCommandResult);
        if (!vehicle->armed()) {
            vehicle->setArmed(true);
        }
    }
}

void ZTFirmwarePlugin::guidedModeTakeoff(Vehicle* vehicle)
{
    QString takeoffAltParam("MIS_TAKEOFF_ALT");

    if (qIsNaN(vehicle->altitudeAMSL()->rawValue().toDouble())) {
        qgcApp()->showMessage(tr("Unable to takeoff, vehicle position not known."));
        return;
    }

    if (!vehicle->parameterManager()->parameterExists(FactSystem::defaultComponentId, takeoffAltParam)) {
        qgcApp()->showMessage(tr("Unable to takeoff, MIS_TAKEOFF_ALT parameter missing."));
        return;
    }
    Fact* takeoffAlt = vehicle->parameterManager()->getParameter(FactSystem::defaultComponentId, takeoffAltParam);

    connect(vehicle, &Vehicle::mavCommandResult, this, &ZTFirmwarePlugin::_mavCommandResult);
    vehicle->sendMavCommand(vehicle->defaultComponentId(),
                            MAV_CMD_NAV_TAKEOFF,
                            true,                           // show error is fails
                            -1,                             // No pitch requested
                            0, 0,                           // param 2-4 unused
                            NAN, NAN, NAN,                  // No yaw, lat, lon
                            vehicle->altitudeAMSL()->rawValue().toDouble() + takeoffAlt->rawValue().toDouble());
}

void ZTFirmwarePlugin::guidedModeGotoLocation(Vehicle* vehicle, const QGeoCoordinate& gotoCoord)
{
    if (qIsNaN(vehicle->altitudeAMSL()->rawValue().toDouble())) {
        qgcApp()->showMessage(tr("Unable to go to location, vehicle position not known."));
        return;
    }

    vehicle->sendMavCommand(vehicle->defaultComponentId(),
                            MAV_CMD_DO_REPOSITION,
                            true,   // show error is fails
                            -1.0f,
                            MAV_DO_REPOSITION_FLAGS_CHANGE_MODE,
                            0.0f,
                            NAN,
                            gotoCoord.latitude(),
                            gotoCoord.longitude(),
                            vehicle->altitudeAMSL()->rawValue().toFloat());
}

void ZTFirmwarePlugin::guidedModeChangeAltitude(Vehicle* vehicle, double altitudeChange)
{
    if (!vehicle->homePosition().isValid()) {
        qgcApp()->showMessage(tr("Unable to change altitude, home position unknown."));
        return;
    }
    if (qIsNaN(vehicle->homePosition().altitude())) {
        qgcApp()->showMessage(tr("Unable to change altitude, home position altitude unknown."));
        return;
    }

    double currentAltRel = vehicle->altitudeRelative()->rawValue().toDouble();
    double newAltRel = currentAltRel + altitudeChange;

    vehicle->sendMavCommand(vehicle->defaultComponentId(),
                            MAV_CMD_DO_REPOSITION,
                            true,   // show error is fails
                            -1.0f,
                            MAV_DO_REPOSITION_FLAGS_CHANGE_MODE,
                            0.0f,
                            NAN,
                            NAN,
                            NAN,
                            vehicle->homePosition().altitude() + newAltRel);
}

void ZTFirmwarePlugin::startMission(Vehicle* vehicle)
{
    if (!_armVehicleAndValidate(vehicle)) {
        qgcApp()->showMessage(tr("Unable to start mission: Vehicle failed to arm."));
        return;
    }

    _setFlightModeAndValidate(vehicle, missionFlightMode());
}

void ZTFirmwarePlugin::setGuidedMode(Vehicle* vehicle, bool guidedMode)
{
    if (guidedMode) {
        _setFlightModeAndValidate(vehicle, _holdFlightMode);
    } else {
        pauseVehicle(vehicle);
    }
}

bool ZTFirmwarePlugin::isGuidedMode(const Vehicle* vehicle) const
{
    // Not supported by generic vehicle
    return (vehicle->flightMode() == _holdFlightMode || vehicle->flightMode() == _takeoffFlightMode
            || vehicle->flightMode() == _landingFlightMode);
}

bool ZTFirmwarePlugin::adjustIncomingMavlinkMessage(Vehicle* vehicle, mavlink_message_t* message)
{
    //-- Don't process messages to/from UDP Bridge. It doesn't suffer from these issues
    if (message->compid == MAV_COMP_ID_UDP_BRIDGE) {
        return true;
    }

    switch (message->msgid) {
    case MAVLINK_MSG_ID_AUTOPILOT_VERSION:
        _handleAutopilotVersion(vehicle, message);
        break;
    }

    return true;
}

void ZTFirmwarePlugin::_handleAutopilotVersion(Vehicle* vehicle, mavlink_message_t* message)
{
    Q_UNUSED(vehicle);

    ZTFirmwarePluginInstanceData* instanceData = qobject_cast<ZTFirmwarePluginInstanceData*>(vehicle->firmwarePluginInstanceData());
    if (!instanceData->versionNotified) {
        bool notifyUser = false;
        int supportedMajorVersion = 1;
        int supportedMinorVersion = 4;
        int supportedPatchVersion = 1;

        mavlink_autopilot_version_t version;
        mavlink_msg_autopilot_version_decode(message, &version);

        if (version.flight_sw_version != 0) {
            int majorVersion, minorVersion, patchVersion;

            majorVersion = (version.flight_sw_version >> (8*3)) & 0xFF;
            minorVersion = (version.flight_sw_version >> (8*2)) & 0xFF;
            patchVersion = (version.flight_sw_version >> (8*1)) & 0xFF;

            if (majorVersion < supportedMajorVersion) {
                notifyUser = true;
            } else if (majorVersion == supportedMajorVersion) {
                if (minorVersion < supportedMinorVersion) {
                    notifyUser = true;
                } else if (minorVersion == supportedMinorVersion) {
                    notifyUser = patchVersion < supportedPatchVersion;
                }
            }
        } else {
            notifyUser = true;
        }

        if (notifyUser) {
            instanceData->versionNotified = true;
            qgcApp()->showMessage(QString("QGroundControl supports ZT Pro firmware Version %1.%2.%3 and above. You are using a version prior to that which will lead to unpredictable results. Please upgrade your firmware.").arg(supportedMajorVersion).arg(supportedMinorVersion).arg(supportedPatchVersion));
        }
    }
}

bool ZTFirmwarePlugin::vehicleYawsToNextWaypointInMission(const Vehicle* vehicle) const
{
    if (vehicle->isOfflineEditingVehicle()) {
        return FirmwarePlugin::vehicleYawsToNextWaypointInMission(vehicle);
    } else {
        if (vehicle->multiRotor() && vehicle->parameterManager()->parameterExists(FactSystem::defaultComponentId, QStringLiteral("MIS_YAWMODE"))) {
            Fact* yawMode = vehicle->parameterManager()->getParameter(FactSystem::defaultComponentId, QStringLiteral("MIS_YAWMODE"));
            return yawMode && yawMode->rawValue().toInt() == 1;
        }
    }
    return true;
}
