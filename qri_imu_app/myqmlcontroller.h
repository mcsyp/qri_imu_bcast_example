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

#ifndef SPIDERCENTER_H
#define SPIDERCENTER_H

#include <QObject>
#include "singlescanner.h"

#define SENSOR_UUID       "00ab0001-b5a3-0f93-e0a9-e50e24dc3993"
#define SENSOR_IMU_UUID "20ab0001-b5a3-0f93-e0a9-e50e24dc3993"
#define SENSOR_BEAT_UUID "30ab0001-b5a3-0f93-e0a9-e50e24dc3993"
class MyQmlController : public SingleScanner
{
    Q_OBJECT
public:
    explicit MyQmlController(QObject *parent = 0);

protected:
    virtual void postCharacteristicValueReceived(const QLowEnergyService *service, const QLowEnergyCharacteristic &c, const QByteArray &value);

    virtual bool checkService(QList<QLowEnergyService*> &list);
    virtual bool checkServiceDetailDiscovered(QLowEnergyService *service);
signals:
    void charGyroUpdated(int stamp,int gx, int gy, int gz);
    void charAccelUpdated(int stamp, int ax, int ay, int az);
private:
    QLowEnergyCharacteristic m_charImu;//read|notify imu data
    QLowEnergyCharacteristic m_charBeat;//read|notify imu data
};

typedef struct sensing_imu {
    uint32_t stamp;
    int16_t ax;
    int16_t ay;
    int16_t az;
    int16_t gx;
    int16_t gy;
    int16_t gz;
} sensing_imu_t;
#endif // SPIDERCENTER_H
