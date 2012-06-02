#include <vector>

using std::vector;

#ifdef SISPMLIB_EXPORTS
#define SISPMLIB_API __declspec(dllexport)
#else
#define SISPMLIB_API __declspec(dllimport)
#endif

class SISPMLIB_API SisPmDevice {
private:
    HANDLE hDevice;

public:
    SisPmDevice(LPTSTR devicePath);
    ~SisPmDevice();

    DWORD serial();

    BOOL isOn(DWORD socket);
    void turn(DWORD socket, BOOL on);

    static vector<SisPmDevice*> findDevices();
};
