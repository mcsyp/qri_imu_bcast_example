/****************************************************************************
** Author: Song Yunpeng
** Copyright (c) 2016 M.C.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
**
****************************************************************************/
#ifndef DEVICEINFO_H
#define DEVICEINFO_H

#include <QObject>
#include <qbluetoothdeviceinfo.h>
#include <qbluetoothaddress.h>
#include <QLowEnergyController>
#include <QLowEnergyCharacteristic>
#include <QList>

#include <QString>
#include <QObject>
#include <qbluetoothdeviceinfo.h>
#include <qbluetoothaddress.h>

class DeviceInfo: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString deviceName READ name NOTIFY deviceUpdated)
    Q_PROPERTY(QString deviceAddress READ address NOTIFY deviceUpdated)
    Q_PROPERTY(int deviceRSSI READ rssi NOTIFY deviceUpdated)

public:
    DeviceInfo(const QBluetoothDeviceInfo &device);
    QString address() const;
    QString name() const{return m_device.name();}
    int rssi() const{return m_device.rssi();}

    QBluetoothDeviceInfo& device() {return m_device;}

signals:
    void deviceUpdated();

private:
    QBluetoothDeviceInfo m_device;
};


#endif // BLEDEVICEINFO_H
