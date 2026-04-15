#ifndef __UISYNC_H__
#define __UISYNC_H__

void UISync_Init(void);
void UISync_DeInit(void);

typedef enum{
    UISync_Error = 0U,
    UISync_Pending,
    UISync_Waiting
} UISync_DeviceStatusTypedef;

typedef struct{
    void (*Init)(void);
    void (*DeInit)(void);
    UISync_DeviceStatusTypedef (*GetStatus)(void);
    void (*Process)(void);
} UISync_DeviceInitTypedef;

void UISync_Update(void);
void UISync_Process(void);

#endif /* __UISYNC_H__ */
