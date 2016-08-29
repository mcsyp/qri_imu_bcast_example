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
