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

#include "myqmlcontroller.h"

MyQmlController::MyQmlController(QObject *parent) : SingleScanner(parent)
{
}

bool MyQmlController::checkService(QList<QLowEnergyService*>& list){
    QLowEnergyService* service =  getService(SENSOR_UUID);
    if(service==NULL){
        qDebug()<<tr("No service found!");
        return false;
    }
    list.append(service);
    return true;
}

bool MyQmlController::checkServiceDetailDiscovered(QLowEnergyService *service){
    bool ret = true;
    if(!service){
        return false;
    }

    //get characteristic list
    QList<QLowEnergyCharacteristic> list =  service->characteristics();
    if(!list.size()){
        return false;
    }
    qDebug()<<tr("Digging characteristic");
    do {

        ret = false;
        QLowEnergyCharacteristic c;
        for(int i=0;i<list.size();i++){
            c = list.at(i);
            QString id = uuidToString(c.uuid());
            qDebug()<<tr("found char uuid:%1").arg(id);
            if(id==QString(SENSOR_IMU_UUID)){
                m_charImu = c;
                ret=true;
                continue;
            }
        }
        if(!ret){
            qDebug()<<tr("None of required characteristic is found.");
            break;
        }

        /*enable the notify*/
        enableCharacteristicNotify(service,m_charImu,true);
    }while(0);

    return ret;
}
void MyQmlController::postCharacteristicValueReceived(const QLowEnergyService *service,
                                                      const QLowEnergyCharacteristic &c,
                                                      const QByteArray &value)
{
    (void)service;
    do{
        if(c==m_charImu){/*accel data*/
            sensing_imu_t * ptr;
            ptr = (sensing_imu_t*)value.data();
            emit charAccelUpdated(ptr->stamp,ptr->ax,ptr->ay,ptr->az);
            emit charGyroUpdated(ptr->stamp,ptr->gx,ptr->gy,ptr->gz);
            break;
        }
    }while(0);

}
