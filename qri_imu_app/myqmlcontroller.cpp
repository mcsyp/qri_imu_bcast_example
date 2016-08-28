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
            if(id==QString(SENSOR_PVP_UUID)){
                m_charPVP = c;
                ret = true;
                continue;
            }
            if(id==QString(SENSOR_ACCEL_UUID)){
                m_charAcc = c;
                ret=true;
                continue;
            }
            if(id==QString(SENSOR_GYRO_UUID)){
                m_charGyro =c;
                ret=true;
                continue;
            }
        }
        if(!ret){
            qDebug()<<tr("None of required characteristic is found.");
            break;
        }

        /*enable the notify*/
        enableCharacteristicNotify(service,m_charAcc,true);
        enableCharacteristicNotify(service,m_charGyro,true);
    }while(0);

    return ret;
}
void MyQmlController::postCharacteristicValueReceived(const QLowEnergyService *service,
                                                      const QLowEnergyCharacteristic &c,
                                                      const QByteArray &value)
{
    do{
        if(c==m_charAcc){/*accel data*/
            sensing_accel_t * ptr;
            ptr = (sensing_accel_t*)value.data();
            emit charAccelUpdated(ptr->stamp,ptr->x,ptr->y,ptr->z);
            break;
        }
        if(c==m_charGyro){/*gyro data*/
            sensing_gyro_t* ptr;
            ptr = (sensing_gyro_t*)value.data();
            emit charGyroUpdated(ptr->stamp,ptr->x,ptr->y,ptr->z);
            break;
        }
    }while(0);

}
