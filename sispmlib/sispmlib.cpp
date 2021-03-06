#include "stdafx.h"
#include "sispmlib.h"
#include "sispmlib_internal.h"

struct deviceType {
    LPCTSTR hardwareId;
    unsigned int socketCount;
};

const struct deviceType deviceTypes[] = {
    {_T("HID\\VID_04B4&PID_FD11"), 4},
    {_T("HID\\VID_04B4&PID_FD13"), 4},
    {_T("HID\\VID_04B4&PID_FD10"), 1},
    {_T("HID\\VID_04B4&PID_FD12"), 1},
};

#define MASK_SWITCH 1
#define MASK_SENSE 2

vector<SisPmDevice> SisPmDevice::findDevices() {
    vector<SisPmDevice> result = vector<SisPmDevice>();

    GUID hidGuid;
    HidD_GetHidGuid(&hidGuid);

    HDEVINFO deviceInfoSet;
    deviceInfoSet = SetupDiGetClassDevs(&hidGuid, NULL, NULL, DIGCF_PRESENT|DIGCF_DEVICEINTERFACE);

    SP_DEVINFO_DATA deviceInfoData;
    deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    for (int i = 0; SetupDiEnumDeviceInfo(deviceInfoSet, i, &deviceInfoData); i++) {

        DWORD propertySize = 0;
        DWORD propertyDataType;
        SetupDiGetDeviceRegistryProperty(deviceInfoSet, &deviceInfoData, SPDRP_HARDWAREID, NULL, NULL, NULL, &propertySize);
        PBYTE propertyBuffer = new BYTE[propertySize];
        SetupDiGetDeviceRegistryProperty(deviceInfoSet, &deviceInfoData, SPDRP_HARDWAREID, &propertyDataType, propertyBuffer, propertySize, NULL);
        assert(propertyDataType == REG_MULTI_SZ);

        const struct deviceType *deviceType = NULL;
        LPTSTR hardwareId = (LPTSTR)propertyBuffer;
        while(_tcslen(hardwareId) && !deviceType) {
            for(int j = 0; j < sizeof(deviceTypes) / sizeof(struct deviceType); j++) {
                if(_tcslen(hardwareId) == _tcslen(deviceTypes[j].hardwareId) && _tccmp(hardwareId, deviceTypes[j].hardwareId) == 0)
                    deviceType = &deviceTypes[j];
                    break;
            }
            hardwareId = _tcsninc(hardwareId, _tcslen(hardwareId)) + 1;
        }

        delete propertyBuffer;
        
        if(deviceType) {
            // get device interface
            SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
            deviceInterfaceData.cbSize = sizeof deviceInterfaceData;
            SetupDiEnumDeviceInterfaces(deviceInfoSet, &deviceInfoData, &hidGuid, 0, &deviceInterfaceData);

            // get device detailled data
            DWORD detailledDataSize = 0;
            SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, NULL, 0, &detailledDataSize, NULL);
            PSP_DEVICE_INTERFACE_DETAIL_DATA detailledDataBuffer = (PSP_DEVICE_INTERFACE_DETAIL_DATA)new BYTE[detailledDataSize];
            detailledDataBuffer->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
            SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, detailledDataBuffer, detailledDataSize, NULL, NULL);

            result.push_back(SisPmDevice(detailledDataBuffer->DevicePath, deviceType->socketCount));

            delete detailledDataBuffer;
        }
    }
    SetupDiDestroyDeviceInfoList(deviceInfoSet);

    return result;
};

_SisPmDeviceHandle::_SisPmDeviceHandle(HANDLE hDevice) {
    this->hDevice = hDevice;
    this->references = 1;
}

void _SisPmDeviceHandle::AddRef() {
    this->references++;
}

void _SisPmDeviceHandle::Release() {
    this->references--;
    if(this->references == 0) {
        CloseHandle(this->hDevice);
    }
}

SisPmDevice::SisPmDevice(const SisPmDevice& device) {
    this->handle = device.handle;
    this->socketCount = device.socketCount;
    this->handle->AddRef();
}

SisPmDevice::SisPmDevice(LPCTSTR devicePath, unsigned int socketCount) {
    this->handle = new _SisPmDeviceHandle(CreateFile(devicePath, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL));
    this->socketCount = socketCount;
}

SisPmDevice::~SisPmDevice() {
    this->handle->Release();
}

unsigned long SisPmDevice::serial() {
    BYTE feature[5] = {0x01, 0, 0, 0, 0};
    HidD_GetFeature(this->handle->hDevice, &feature, sizeof feature);
    return *((unsigned long*)&feature[1]);
}

SisPmSocket SisPmDevice::socket(unsigned int number) {
    return SisPmSocket(*this, number);
}

SisPmSocket::SisPmSocket(const SisPmSocket& socket) {
    this->handle = socket.handle;
    this->number = socket.number;
    this->handle->AddRef();
}

SisPmSocket::SisPmSocket(SisPmDevice device, unsigned int number) {
    if(number<1||number>device.socketCount) {
        throw;
    }
    this->handle = device.handle;
    this->handle->AddRef();
    this->number = number;
}

SisPmSocket::~SisPmSocket() {
    this->handle->Release();
}

bool SisPmSocket::isActive() {
    BYTE feature[2] = {(BYTE)this->number * 3, 0};
    HidD_GetFeature(this->handle->hDevice, &feature, sizeof feature);
    return (feature[1] & MASK_SENSE) == MASK_SENSE;
}

bool SisPmSocket::isTurnedOn() {
    BYTE feature[2] = {(BYTE)this->number * 3, 0};
    HidD_GetFeature(this->handle->hDevice, &feature, sizeof feature);
    return (feature[1] & MASK_SWITCH) == MASK_SWITCH;
}

void SisPmSocket::turn(bool on) {
    BYTE feature[2] = {(BYTE)this->number * 3, 0};
    HidD_GetFeature(this->handle->hDevice, &feature, sizeof feature);
    if(on) {
        feature[1] |= MASK_SWITCH;
    } else {
        feature[1] &= ~MASK_SWITCH;
    }
    HidD_SetFeature(this->handle->hDevice, &feature, sizeof feature);
}
