#include "stdafx.h"
#include "sispmlib.h"

using std::vector;

vector<SisPmDevice*> SisPmDevice::findDevices() {
        GUID hidGuid;
        HDEVINFO deviceInfoSet  ;
        SP_DEVINFO_DATA deviceInfoData;

        vector<SisPmDevice*> result = vector<SisPmDevice*>();

        HidD_GetHidGuid(&hidGuid);
        deviceInfoSet = SetupDiGetClassDevs(&hidGuid, NULL, NULL, DIGCF_PRESENT|DIGCF_DEVICEINTERFACE);

        deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
        for (int i=0; SetupDiEnumDeviceInfo(deviceInfoSet, i, &deviceInfoData); i++) {
            DWORD bufferSize = 0;
            SetupDiGetDeviceRegistryProperty(deviceInfoSet, &deviceInfoData, SPDRP_HARDWAREID, NULL, NULL, NULL, &bufferSize);
            PBYTE buffer = new BYTE[bufferSize];
            SetupDiGetDeviceRegistryProperty(deviceInfoSet, &deviceInfoData, SPDRP_HARDWAREID, NULL, buffer, bufferSize, &bufferSize);

            if(wcsncmp((LPWSTR)buffer, L"HID\\VID_04B4&PID_FD13", 21) == 0) { // FIXME
                SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
                deviceInterfaceData.cbSize = sizeof deviceInterfaceData;
                SetupDiEnumDeviceInterfaces(deviceInfoSet, &deviceInfoData, &hidGuid, 0, &deviceInterfaceData);

                PSP_DEVICE_INTERFACE_DETAIL_DATA deviceInterfaceDetailedData;
                DWORD requiredSize = 0;
                SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, NULL, 0, &requiredSize, NULL);
                deviceInterfaceDetailedData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(requiredSize);
                deviceInterfaceDetailedData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
                SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, deviceInterfaceDetailedData, requiredSize, &requiredSize, NULL);
                
                SisPmDevice* device = new SisPmDevice(deviceInterfaceDetailedData->DevicePath);
                result.push_back(device);
            } else {
                delete buffer;
            }
        }
        SetupDiDestroyDeviceInfoList(deviceInfoSet);

        return result;
};

SisPmDevice::SisPmDevice(LPTSTR devicePath) {
    this->hDevice = CreateFile(devicePath, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
}

SisPmDevice::~SisPmDevice() {
    CloseHandle(this->hDevice);
}

DWORD SisPmDevice::serial() {
    BYTE feature[5] = {0x01, 0, 0, 0, 0};
    HidD_GetFeature(this->hDevice, &feature, sizeof feature);
    
    return *((PDWORD)&feature[1]);
}

BOOL SisPmDevice::isOn(DWORD socket) {
    if(socket<1||socket>4)
        throw;
    BYTE feature[2] = {3*socket, 0};
    HidD_GetFeature(this->hDevice, &feature, sizeof feature);
    return feature[1] & 0x03;
}

void SisPmDevice::turn(DWORD socket, BOOL on) {
    if(socket<1||socket>4)
        throw;
    BYTE feature[2] = {3*socket, 0};
    HidD_GetFeature(this->hDevice, &feature, sizeof feature);
    if(on) {
        feature[1] |= 1;
    } else {
        feature[1] &= ~1;
    }
    HidD_SetFeature(this->hDevice, &feature, sizeof feature);
}
