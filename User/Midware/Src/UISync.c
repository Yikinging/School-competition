#include "UISync.h"
#include <stddef.h>

struct UISync_DeviceHandleTypedef {
    UISync_DeviceInitTypedef Init;
    UISync_DeviceStatusTypedef Status;
};

static UISync_DeviceHandleTypedef UISync_DeviceList[UISYNC_MaxDeviceNum] = {0};

UISync_DeviceHandleTypedef* UISync_RegisterDevice(const UISync_DeviceInitTypedef* init){
    if(init == NULL
        || init->Init == NULL
        || init->DeInit == NULL
        || init->Update == NULL
        || init->Process == NULL){
        return NULL;
    }

    if (init->Rank >= UISYNC_MaxDeviceNum){
        return NULL;
    }
    if (UISync_DeviceList[init->Rank].Status != UISync_Undefined){
        return NULL;
    }

    UISync_DeviceHandleTypedef* handle = &UISync_DeviceList[init->Rank];
    handle->Init = *init;
    handle->Status = UISync_Waiting;

    init->Init();

    return handle;
}

void UISync_UnregisterDevice(UISync_DeviceHandleTypedef *handle){
    if(handle == NULL
        || handle->Init.DeInit == NULL){
        return;
    }

    handle->Init.DeInit();

    (*handle) = (UISync_DeviceHandleTypedef){0};
}

void UISync_SetDeviceStatus(UISync_DeviceHandleTypedef *handle, UISync_DeviceStatusTypedef status){
    if(handle == NULL){
        return;
    }
    handle->Status = status;
}

// placed at interrupt function
void UISync_Update(void){
    for(int i = 0; i < UISYNC_MaxDeviceNum; i++){
        UISync_DeviceHandleTypedef* handle = &UISync_DeviceList[i];
        if (handle->Status == UISync_Undefined){
            continue;
        }
        if (handle->Status == UISync_Disabled){
            continue;
        }
        handle->Status = handle->Init.Update();
    }
}

// placed at main loop
void UISync_Process(void){
    for(int i = 0; i < UISYNC_MaxDeviceNum; i++){
        UISync_DeviceHandleTypedef *handle = &UISync_DeviceList[i];
        if (handle->Status == UISync_Undefined){
            continue;
        }
        if (handle->Status == UISync_Pending){
            handle->Status = UISync_Waiting;
            handle->Init.Process();
        }
    }
}
