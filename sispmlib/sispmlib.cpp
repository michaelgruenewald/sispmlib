#include "stdafx.h"
#include "sispmlib.h"

using std::vector;

#define HWID _T("HID\\VID_04B4&PID_FD13")

vector<SisPmDevice> SisPmDevice::findDevices() {
    vector<SisPmDevice> result = vector<SisPmDevice>();

    GUID hidGuid;
    HidD_GetHidGuid(&hidGuid);

    HDEVINFO deviceInfoSet;
    deviceInfoSet = SetupDiGetClassDevs(&hidGuid, NULL, NULL, DIGCF_PRESENT|DIGCF_DEVICEINTERFACE);

    SP_DEVINFO_DATA deviceInfoData;
    deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    for (int i=0; SetupDiEnumDeviceInfo(deviceInfoSet, i, &deviceInfoData); i++) {

        DWORD propertySize = 0;
        DWORD propertyDataType;
        SetupDiGetDeviceRegistryProperty(deviceInfoSet, &deviceInfoData, SPDRP_HARDWAREID, NULL, NULL, NULL, &propertySize);
        PBYTE propertyBuffer = new BYTE[propertySize];
        SetupDiGetDeviceRegistryProperty(deviceInfoSet, &deviceInfoData, SPDRP_HARDWAREID, &propertyDataType, propertyBuffer, propertySize, NULL);
        assert(propertyDataType == REG_MULTI_SZ);

        LPTSTR hardwareId = (LPTSTR)propertyBuffer;
        while(_tcslen(hardwareId)) {
            if(_tcslen(hardwareId) == _tcslen(HWID) && _tccmp(hardwareId, HWID) == 0)
                break;
            hardwareId = _tcsninc(hardwareId, _tcslen(hardwareId)) + 1;
        }

        if(_tcslen(hardwareId)) {
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

            result.push_back(SisPmDevice(detailledDataBuffer->DevicePath));

            delete detailledDataBuffer;
        }

        delete propertyBuffer;
    }
    SetupDiDestroyDeviceInfoList(deviceInfoSet);

    return result;
};

SisPmDevice::SisPmDeviceHandle::SisPmDeviceHandle(HANDLE hDevice) {
    this->hDevice = hDevice;
    this->references = 1;
}

void SisPmDevice::SisPmDeviceHandle::AddRef() {
    this->references++;
}

void SisPmDevice::SisPmDeviceHandle::Release() {
    this->references--;
    if(this->references == 0) {
        CloseHandle(this->hDevice);
    }
}

SisPmDevice::SisPmDevice(const SisPmDevice& device) {
    this->handle = device.handle;
    this->handle->AddRef();
}

SisPmDevice::SisPmDevice(LPCTSTR devicePath) {
    this->handle = new SisPmDeviceHandle(CreateFile(devicePath, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL));
}

SisPmDevice::~SisPmDevice() {
    this->handle->Release();
}

DWORD SisPmDevice::serial() {
    BYTE feature[5] = {0x01, 0, 0, 0, 0};
    HidD_GetFeature(this->handle->hDevice, &feature, sizeof feature);
    return *((PDWORD)&feature[1]);
}

SisPmSocket SisPmDevice::socket(DWORD number) {
    return SisPmSocket(*this, number);
}

SisPmSocket::SisPmSocket(const SisPmSocket& socket) {
    this->handle = socket.handle;
    this->number = socket.number;
    this->handle->AddRef();
}

SisPmSocket::SisPmSocket(SisPmDevice device, DWORD number) {
    if(number<1||number>4) {
        throw;
    }
    this->handle = device.handle;
    this->handle->AddRef();
    this->number = number;
}

SisPmSocket::~SisPmSocket() {
    this->handle->Release();
}

BOOL SisPmSocket::isOn() {
    BYTE feature[2] = {(BYTE)this->number * 3, 0};
    HidD_GetFeature(this->handle->hDevice, &feature, sizeof feature);
    return feature[1] & 0x03;
}

void SisPmSocket::turn(BOOL on) {
    BYTE feature[2] = {(BYTE)this->number * 3, 0};
    HidD_GetFeature(this->handle->hDevice, &feature, sizeof feature);
    if(on) {
        feature[1] |= 1;
    } else {
        feature[1] &= ~1;
    }
    HidD_SetFeature(this->handle->hDevice, &feature, sizeof feature);
}
