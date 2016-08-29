#ifndef SPIDERCENTER_H
#define SPIDERCENTER_H

#include <QObject>
#include "singlescanner.h"

#define SENSOR_UUID       "00ab0001-b5a3-0f93-e0a9-e50e24dc3993"
#define SENSOR_IMU_UUID "20ab0001-b5a3-0f93-e0a9-e50e24dc3993"
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
