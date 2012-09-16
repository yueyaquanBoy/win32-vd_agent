#ifndef _H_VIRTIO_VDI_PORT
#define _H_VIRTIO_VDI_PORT

#include "vdi_port.h"

enum {
    VIRTIO_VDI_PORT_EVENT_WRITE=0,
    VIRTIO_VDI_PORT_EVENT_READ,
    VIRTIO_VDI_PORT_EVENT_COUNT
};

class VirtioVDIPort : public VDIPort {
public:
    VirtioVDIPort();
    ~VirtioVDIPort();
    virtual const char *name() { return "VirtioVDIPort"; }
    virtual bool init();
    virtual unsigned get_num_events() { return VIRTIO_VDI_PORT_EVENT_COUNT; }
    virtual void fill_events(HANDLE* handles);
    virtual bool handle_event(int event);
    virtual int write();
    virtual int read();

private:
    bool write_completion();
    bool read_completion();

private:
    static VirtioVDIPort* _singleton;
    HANDLE _handle;
};

#endif //_H_VIRTIO_VDI_PORT
