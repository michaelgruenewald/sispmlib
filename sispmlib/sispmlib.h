#include <vector>

using std::vector;

#ifdef SISPMLIB_EXPORTS
#define SISPMLIB_API __declspec(dllexport)
#else
#define SISPMLIB_API __declspec(dllimport)
#endif

class SISPMLIB_API SisPmSocket;

class SISPMLIB_API SisPmDevice {
public:
    SisPmDevice(const SisPmDevice& device);
    SisPmDevice(LPCTSTR devicePath, DWORD socketCount);
    ~SisPmDevice();

    DWORD serial();
    SisPmSocket socket(DWORD number);

    static vector<SisPmDevice> findDevices();

private:
    class SisPmDeviceHandle {
    public:
        HANDLE hDevice;
        DWORD references;

        SisPmDeviceHandle(HANDLE hDevice);
        
        void AddRef();
        void Release();

        friend class SisPmSocket;
    };

    SisPmDeviceHandle *handle;
    DWORD socketCount;

    friend class SisPmSocket;
};

class SISPMLIB_API SisPmSocket {
public:
    SisPmSocket(const SisPmSocket& socket);
    SisPmSocket(SisPmDevice device, DWORD number);
    ~SisPmSocket();

    BOOL isOn();
    void turn(BOOL on);

private:
    DWORD number;
    SisPmDevice::SisPmDeviceHandle *handle;

    friend class SisPmDevice;
};
