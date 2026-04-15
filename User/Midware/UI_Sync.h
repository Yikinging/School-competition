#ifndef __UISYNC_H
#define __UISYNC_H

#include <stdint.h>

typedef struct UISync_KeyHandleTypeDef *UISync_KeyHandle;

typedef struct {
    uint8_t Rank;
    uint8_t FilterThreshold;
    uint8_t (*KeyIsPressed)(void);
    void (*KeyEvent)(void);
} UISync_KeyInitTypeDef;

/* 初始化 UI_Sync 模块内部的设备管理器。 */
void UISync_Init(void);

/* 按 rank 顺序更新所有已注册且处于使能状态的设备。 */
void UISync_Update(void);

/* 注册一个 Key 设备，拷贝初始化参数并返回托管句柄。 */
UISync_KeyHandle UISync_KeyInit(const UISync_KeyInitTypeDef *pKeyInit);

/* 注销一个 Key 设备，并释放其句柄占用的动态内存。 */
void UISync_KeyDeInit(UISync_KeyHandle hKey);

/* 使能指定 Key，使其参与后续的统一更新。 */
void UISync_KeyEnable(UISync_KeyHandle hKey);

/* 失能指定 Key，使其暂时不参与后续的统一更新。 */
void UISync_KeyDisable(UISync_KeyHandle hKey);

/* 直接触发指定 Key 的已注册事件回调。 */
void UISync_KeyTriggerEvent(UISync_KeyHandle hKey);

/* 修改指定 Key 的 rank，并在设备表中按新优先级重排。 */
void UISync_KeySetRank(UISync_KeyHandle hKey, uint8_t rank);


#endif /* __UISYNC_H */
