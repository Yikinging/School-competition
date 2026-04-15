#ifndef __UISYNC_H__
#define __UISYNC_H__

#include <stdint.h>

// void UISync_Init(void);
// void UISync_DeInit(void);

typedef struct UISync_DeviceHandleTypedef UISync_DeviceHandleTypedef;

typedef enum{
    UISync_Undefined = 0U,
    UISync_Pending,
    UISync_Waiting,
    UISync_Disabled
} UISync_DeviceStatusTypedef;

typedef struct{
    void (*Init)(void);
    void (*DeInit)(void);
    uint8_t Rank;
    void* DataSheet;
    UISync_DeviceStatusTypedef (*Update)(void);
    void (*Process)(void);
} UISync_DeviceInitTypedef;

#define UISYNC_MaxDeviceNum 10U

UISync_DeviceHandleTypedef* UISync_RegisterDevice(const UISync_DeviceInitTypedef *init);
void UISync_UnregisterDevice(UISync_DeviceHandleTypedef *handle);
void UISync_SetDeviceStatus(UISync_DeviceHandleTypedef *handle, UISync_DeviceStatusTypedef status);

void UISync_Update(void);
void UISync_Process(void);

#endif /* __UISYNC_H__ */
