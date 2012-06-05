#include <vector>

using std::vector;

#ifdef SISPMLIB_EXPORTS
#define SISPMLIB_API __declspec(dllexport)
#else
#define SISPMLIB_API __declspec(dllimport)
#endif

class SISPMLIB_API SisPmDevice;
class SISPMLIB_API SisPmSocket;
class _SisPmDeviceHandle;

class SISPMLIB_API SisPmDevice {
public:
    SisPmDevice(const SisPmDevice& device);
    SisPmDevice(LPCTSTR devicePath, unsigned int socketCount);
    ~SisPmDevice();

    unsigned long serial();
    SisPmSocket socket(unsigned int number);

    static vector<SisPmDevice> findDevices();

private:
    _SisPmDeviceHandle *handle;
    DWORD socketCount;

    friend class SisPmSocket;
};

class SISPMLIB_API SisPmSocket {
public:
    SisPmSocket(const SisPmSocket& socket);
    SisPmSocket(SisPmDevice device, unsigned int number);
    ~SisPmSocket();

    bool isActive();
    bool isTurnedOn();
    void turn(bool on);

private:
    unsigned int number;
    _SisPmDeviceHandle *handle;

    friend class SisPmDevice;
};
