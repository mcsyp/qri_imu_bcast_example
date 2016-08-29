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

#ifndef SCANNER_H
#define SCANNER_H

#include <qbluetoothglobal.h>
#include <qbluetoothlocaldevice.h>
#include <QObject>
#include <QVariant>
#include <QList>
#include <QBluetoothServiceDiscoveryAgent>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QLowEnergyController>
#include <QBluetoothServiceInfo>

#include "deviceinfo.h"


class SingleScanner : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariant deviceList READ getDeviceList NOTIFY deviceListUpdated)
public:
    explicit SingleScanner(QObject *parent = 0);
    ~SingleScanner();
    /*************************************************
     * call this function to start BLE scan
     * @return true to be successfull.
     *************************************************/
    Q_INVOKABLE bool start();
    Q_INVOKABLE void stop();

    /***
     * connect to the device with specific address
     * @address the mac address string shown in the qml interface.
     * @return true, if connect first call is success. but it doesn't mean connectiong is done.
     */
    Q_INVOKABLE bool connectToDevice(const QString & address);
    Q_INVOKABLE void disconnectFromDevice();

    /**************************************************
     * return all scanned devices
     **************************************************/
    QVariant getDeviceList();
    DeviceInfo* getDevice(const QString& address);
    QLowEnergyService* getService(const QString & uuid);


    Q_INVOKABLE bool isDeviceConnected();
    Q_INVOKABLE bool isServiceDiscovered();

protected:
    void resetDeviceList();
    void resetServiceList();
    /************************************************
     * convert UUID class into readable uuid string.
     ************************************************/
    QString uuidToString(const QBluetoothUuid & id);
    void enableCharacteristicNotify(QLowEnergyService* service, QLowEnergyCharacteristic &c, bool b=true);

    /*************************************************************
     * check if the required service & characteristics exists.
     *************************************************************/
    /**
     * check if required services exist
     * @list, a empty list, save the required and found service in this list
     * @return true if all required services are found.
     */
    virtual bool checkService(QList<QLowEnergyService*> & list)=0;
    /**
     * process the discovered characteristics
     * @service, the detals discovered service.
     * @return true if the characterlist is OK.
     **/
    virtual bool checkServiceDetailDiscovered(QLowEnergyService* service)=0;

    /**
     * process the characteristic update/notified message.
     * @service, who posted this message
     * @c, the characteristic update this message.
     * @value, value of the message.
     */
    virtual void postCharacteristicValueReceived(const QLowEnergyService* service,
                                                 const QLowEnergyCharacteristic& c,
                                                 const QByteArray &value);
signals:
    //emit when device list content changed.
    void deviceListUpdated();

    /***********************************************
     * controller state related signal
     **********************************************/
    void stateDeviceDiscoverStarted();
    void stateDeviceDiscoverDone();
    void stateDeviceConnected(const QString & address);
    void stateDeviceDisconnected(const QString & address);
    void stateServiceDiscoverStarted();
    void stateServiceDiscoverDone();

    /*
     * update message to the signal receiver.
     * @message, the transmitting message.
     */
    void updateMessage(const QString & message);

protected slots:
    // QBluetoothDeviceDiscoveryAgent related
    void onScanDeviceDiscovered(const QBluetoothDeviceInfo&);
    void onScanError(QBluetoothDeviceDiscoveryAgent::Error);
    void onScanFinished();

    void onDeviceConnected();
    void onDeviceDisconnected();

    /************************************************
     * service related functions
     ***********************************************/
    void onServiceDiscovered(const QBluetoothUuid &newService);
    void onServiceStateChanged(QLowEnergyService::ServiceState);
    void onServiceDiscoveryFinished();
    void onServiceErrorReceived(QLowEnergyController::Error);

    /*
     * process your characteristic message here
     * TODO:inherit it and process it here.
     */
    void onCharacteristicValueUpdated(const QLowEnergyCharacteristic &c,const QByteArray &value);
private:
    /***************************************
     * local centeral device scan agent
     ***************************************/
    QBluetoothDeviceDiscoveryAgent *m_pstAgent;

    /***************************
     * device related members
     ***************************/
    QLowEnergyController* m_pstController;
    DeviceInfo* m_pstCurrentDevice;
    bool m_isServiceDiscovered;

    QList<QObject*> m_deviceList;
    QList<QObject*> m_serviceList;

};

#endif // SCANNER_H
