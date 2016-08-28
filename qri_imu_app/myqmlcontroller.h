#ifndef SPIDERCENTER_H
#define SPIDERCENTER_H

#include <QObject>
#include "singlescanner.h"

#define SENSOR_UUID       "00ab0001-b5a3-0f93-e0a9-e50e24dc3993"
#define SENSOR_ACCEL_UUID "20ab0001-b5a3-0f93-e0a9-e50e24dc3993"
#define SENSOR_GYRO_UUID  "30ab0001-b5a3-0f93-e0a9-e50e24dc3993"
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
    QLowEnergyCharacteristic m_charPVP;//read|notify acc data
    QLowEnergyCharacteristic m_charAcc;//read|notify acc data
    QLowEnergyCharacteristic m_charGyro;//read|notify acc data
};

typedef struct sensing_accel {
    uint32_t stamp;
    uint16_t x;
    uint16_t y;
    uint16_t z;
} sensing_accel_t;

typedef struct sensing_gyro {
    uint32_t stamp;
    uint16_t x;
    uint16_t y;
    uint16_t z;
} sensing_gyro_t;

#endif // SPIDERCENTER_H
