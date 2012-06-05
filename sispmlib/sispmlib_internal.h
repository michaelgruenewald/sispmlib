class _SisPmDeviceHandle {
public:
    HANDLE hDevice;
    DWORD references;

    _SisPmDeviceHandle(HANDLE hDevice);
        
    void AddRef();
    void Release();

    friend class SisPmSocket;
};
