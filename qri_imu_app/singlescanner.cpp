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
#include <qbluetoothaddress.h>
#include <qbluetoothdevicediscoveryagent.h>
#include <qbluetoothlocaldevice.h>
#include <qbluetoothdeviceinfo.h>
#include <qbluetoothservicediscoveryagent.h>
#include <QDebug>
#include <QList>
#include <QTimer>

#include "singlescanner.h"

SingleScanner::SingleScanner(QObject *parent) : QObject(parent)
{
    //init the bluetooth agent.
    m_pstAgent = new QBluetoothDeviceDiscoveryAgent();
    connect(m_pstAgent, SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo&)),
            this, SLOT(onScanDeviceDiscovered(const QBluetoothDeviceInfo&)));
    connect(m_pstAgent, SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)),
            this, SLOT(onScanError(QBluetoothDeviceDiscoveryAgent::Error)));
    connect(m_pstAgent, SIGNAL(finished()), this, SLOT(onScanFinished()));

    m_pstController = NULL;
    resetDeviceList();
    resetServiceList();
}
SingleScanner::~SingleScanner()
{
    resetServiceList();
    resetDeviceList();
    if(m_pstAgent){
        delete m_pstAgent;
    }
}

void SingleScanner::resetDeviceList(){
    qDeleteAll(m_deviceList);
    m_deviceList.clear();
    m_pstCurrentDevice=NULL;
}
void SingleScanner::resetServiceList(){
    qDeleteAll(m_serviceList);
    m_serviceList.clear();
    m_isServiceDiscovered = false;
}

bool SingleScanner::start()
{
    bool ret = false;

    //release the resources
    resetDeviceList();

    //start the scanner.
    m_pstAgent->start();
    ret = m_pstAgent->isActive();
    if(ret){
        //syp debug
        qDebug()<<tr("Start scanning BLE devices.\n");
        emit stateDeviceDiscoverStarted();
    }
    return ret;
}
void SingleScanner::stop()
{
    m_pstAgent->stop();
    onScanFinished();
}
void SingleScanner::onScanDeviceDiscovered(const QBluetoothDeviceInfo & info)
{
    if (info.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) {
        DeviceInfo *d = new DeviceInfo(info);
        m_deviceList.append(d);
        emit deviceListUpdated();
    }
}
void SingleScanner::onScanError(QBluetoothDeviceDiscoveryAgent::Error)
{
    qDebug()<<tr("Error happens at scanning phase.");
    /**************************************************
     *TODO: add some scan error processing code here.
     **************************************************/
}
void SingleScanner::onScanFinished()
{
    emit stateDeviceDiscoverDone();//emit a scan finished signal
}

bool SingleScanner::connectToDevice(const QString &address)
{
    DeviceInfo * dev = getDevice(address);
    if(dev==NULL)
        return false;

    // We need the current device for service discovery.
    if (!dev->device().isValid()) {
        qDebug()<<tr("Not a valid device\n");
        return false;
    }

    resetServiceList();
    if (m_pstController) {
        m_pstController->disconnectFromDevice();
        delete m_pstController;
        m_pstController = 0;
    }

    if (!m_pstController) {
        // Connecting signals and slots for connecting to LE services.
        m_pstController = new QLowEnergyController(dev->device());
        connect(m_pstController, SIGNAL(connected()),
                this, SLOT(onDeviceConnected()));
        connect(m_pstController, SIGNAL(error(QLowEnergyController::Error)),
                this, SLOT(onServiceErrorReceived(QLowEnergyController::Error)));
        connect(m_pstController, SIGNAL(disconnected()),
                this, SLOT(onDeviceDisconnected()));

        connect(m_pstController, SIGNAL(serviceDiscovered(const QBluetoothUuid&)),
                this, SLOT(onServiceDiscovered(const QBluetoothUuid&)));
        connect(m_pstController, SIGNAL(discoveryFinished()),
                this, SLOT(onServiceDiscoveryFinished()));
    }
    m_pstCurrentDevice=dev;//set device to current device;
    m_pstController->setRemoteAddressType(QLowEnergyController::PublicAddress);
    m_pstController->connectToDevice();
    return true;
}

void SingleScanner::disconnectFromDevice()
{
    resetServiceList();
    if(m_pstController){
        m_pstController->disconnectFromDevice();
        delete m_pstController;
        m_pstController = NULL;
    }

    //clear the current device
    if(m_pstCurrentDevice){
        emit stateDeviceDisconnected(m_pstCurrentDevice->address());
    }else{
        emit stateDeviceDisconnected("Unknown Address");
    }
    m_pstCurrentDevice=NULL;
}

void SingleScanner::onDeviceConnected(){
    if(m_pstCurrentDevice){
        emit stateDeviceConnected(m_pstCurrentDevice->address());
    }else{
        emit stateDeviceConnected("Unkown Address");
    }
    //if is conneccted, scan all services
    if(m_pstController)
        m_pstController->discoverServices();
    emit stateServiceDiscoverStarted();
}

void SingleScanner::onDeviceDisconnected()
{
    disconnectFromDevice();
}

void SingleScanner::onServiceErrorReceived(QLowEnergyController::Error err)
{
    int id = (int)err;
    switch(id){
    case QLowEnergyController::UnknownRemoteDeviceError:
    case QLowEnergyController::InvalidBluetoothAdapterError:
        disconnectFromDevice();
        break;
    }
}
void SingleScanner::onServiceDiscovered(const QBluetoothUuid& serviceUuid)
{
    QLowEnergyService *service = m_pstController->createServiceObject(serviceUuid);
    if (!service) {
        qWarning() << "Cannot create service for uuid";
        return;
    }
    //qDebug()<<tr("found uuid:%1").arg(uuidToString(serviceUuid));
    m_serviceList.append(service);
}

void SingleScanner::onServiceDiscoveryFinished()
{
    bool ret=true;
    do{
        if(m_pstCurrentDevice==NULL){
            ret = false;
            break;
        }
        QList<QLowEnergyService*> list;
        list.clear();

        if(!checkService(list) || !list.size()){
            ret = false;
            qDebug()<<tr("Fail to find the required service list");
            break;
        }
        for(int i=0;i<list.size();i++){
            QLowEnergyService * service = (QLowEnergyService*)list.at(i);
            //clear all connected call back
            disconnect(service,0,0,0);
            //connect service
            connect(service,SIGNAL(characteristicChanged(const QLowEnergyCharacteristic &, const QByteArray &)),
                    this,SLOT(onCharacteristicValueUpdated(const QLowEnergyCharacteristic &, const QByteArray & )));
            connect(service, SIGNAL(stateChanged(QLowEnergyService::ServiceState)),
                    this, SLOT(onServiceStateChanged(QLowEnergyService::ServiceState)));
            service->discoverDetails();
        }
        ret = true;
        m_isServiceDiscovered=true;
    }while(0);
    if(!ret){//if fail
        disconnectFromDevice();
    }
   emit stateServiceDiscoverDone();
}
void SingleScanner::onServiceStateChanged(QLowEnergyService::ServiceState state)
{
    QLowEnergyService* service = qobject_cast<QLowEnergyService*>(sender());
    int index = (int)state;
    switch(index){
    case QLowEnergyService::DiscoveryRequired:
        if(service){
            service->discoverDetails();
        }
        break;
    case QLowEnergyService::DiscoveringServices:
        break;
    case QLowEnergyService::ServiceDiscovered:
        if(!service || !checkServiceDetailDiscovered(service)){
            //if service or detail is not found
            qDebug()<<tr("Service or characteristic not found.");
            disconnectFromDevice();//tell the system that the service is not found and disconnect the device.
        }
        break;
    }
}
void SingleScanner::onCharacteristicValueUpdated(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    QLowEnergyService* service = qobject_cast<QLowEnergyService*>(sender());
    if(service){
        postCharacteristicValueReceived(service,c,value);
    }
}
void SingleScanner::postCharacteristicValueReceived(const QLowEnergyService *service,
                                                    const QLowEnergyCharacteristic &c,
                                                    const QByteArray &value){
    (void)service;
    (void)c;
    (void)value;
}

void SingleScanner::enableCharacteristicNotify(QLowEnergyService* service,QLowEnergyCharacteristic &c, bool b){
    if(!c.isValid() || service==NULL)return;

    const QLowEnergyDescriptor notificationDesc = c.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
    if (!notificationDesc.isValid()) return;
    if(b){
        service->writeDescriptor(notificationDesc, QByteArray::fromHex("0100"));
    }else{
        service->writeDescriptor(notificationDesc, QByteArray::fromHex("0000"));
    }
}

QVariant SingleScanner::getDeviceList(){
    return QVariant::fromValue(m_deviceList);
}

DeviceInfo* SingleScanner::getDevice(const QString& address)
{
    //1.search for device
    for(int i=0;i<m_deviceList.size();i++){
        DeviceInfo* dev = (DeviceInfo*)m_deviceList.at(i);
        if(dev->address()==address){
            return dev;
        }
    }
    return NULL;
}

QLowEnergyService* SingleScanner::getService(const QString &uuid){
    for(int i=0;i<m_serviceList.size();i++){
        QLowEnergyService* s = qobject_cast<QLowEnergyService*>(m_serviceList.at(i));
        if(s && uuidToString(s->serviceUuid())==uuid){
            return s;
        }
    }
    return NULL;
}

QString SingleScanner::uuidToString(const QBluetoothUuid & id)
{
    bool success = false;
    quint16 result16 = id.toUInt16(&success);
    if (success)
        return QStringLiteral("0x") + QString::number(result16, 16);

    quint32 result32 = id.toUInt32(&success);
    if (success)
        return QStringLiteral("0x") + QString::number(result32, 16);

    return id.toString().remove(QLatin1Char('{')).remove(QLatin1Char('}'));
}

bool SingleScanner::isDeviceConnected(){
    return !(m_pstCurrentDevice==NULL);
}
bool SingleScanner::isServiceDiscovered(){
    return m_isServiceDiscovered;
}
