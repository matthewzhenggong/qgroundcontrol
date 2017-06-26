/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/


#ifndef ZTParameterMetaData_H
#define ZTParameterMetaData_H

#include <QObject>
#include <QMap>
#include <QXmlStreamReader>
#include <QLoggingCategory>

#include "FactSystem.h"
#include "AutoPilotPlugin.h"
#include "Vehicle.h"

/// @file
///     @author Don Gagne <don@thegagnes.com>

Q_DECLARE_LOGGING_CATEGORY(ZTParameterMetaDataLog)

/// Loads and holds parameter fact meta data for ZT stack
class ZTParameterMetaData : public QObject
{
    Q_OBJECT
    
public:
    ZTParameterMetaData(void);

    void loadParameterFactMetaDataFile  (const QString& metaDataFile);
    void addMetaDataToFact              (Fact* fact, MAV_TYPE vehicleType);

    static void getParameterMetaDataVersionInfo(const QString& metaDataFile, int& majorVersion, int& minorVersion);

private:
    enum {
        XmlStateNone,
        XmlStateFoundParameters,
        XmlStateFoundVersion,
        XmlStateFoundGroup,
        XmlStateFoundParameter,
        XmlStateDone
    };    

    QVariant _stringToTypedVariant(const QString& string, FactMetaData::ValueType_t type, bool* convertOk);

    bool _parameterMetaDataLoaded;   ///< true: parameter meta data already loaded
    QMap<QString, FactMetaData*> _mapParameterName2FactMetaData; ///< Maps from a parameter name to FactMetaData
};

#endif