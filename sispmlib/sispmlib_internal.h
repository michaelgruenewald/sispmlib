class _SisPmDeviceHandle {
public:
    HANDLE hDevice;
    unsigned int references;

    _SisPmDeviceHandle(HANDLE hDevice);
        
    void AddRef();
    void Release();

    friend class SisPmSocket;
};
