#include "UI_Sync.h"
#include "main.h"

#include <stdlib.h>
#include <string.h>

typedef struct UISync_DeviceNodeTypeDef UISync_DeviceNodeTypeDef;

struct UISync_DeviceNodeTypeDef {
    UISync_DeviceNodeTypeDef *Prev;
    UISync_DeviceNodeTypeDef *Next;
    uint8_t Rank;
    uint8_t IsEnabled;
    void (*Update)(UISync_DeviceNodeTypeDef *node);
};

typedef struct {
    UISync_DeviceNodeTypeDef Head;
    UISync_DeviceNodeTypeDef Tail;
    uint8_t IsInitialized;
} UISync_DeviceManagerTypeDef;

struct UISync_KeyHandleTypeDef {
    UISync_DeviceNodeTypeDef DeviceNode;
    UISync_KeyInitTypeDef InitBackup;
    uint8_t StableLevel;
    uint8_t SampleLevel;
    uint8_t SampleCount;
};

static UISync_DeviceManagerTypeDef gUISyncDeviceManager;

/* 进入临界区，保护链表插入、删除和重排等写操作。 */
static uint32_t UISync_EnterCritical(void)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    return primask;
}

/* 退出临界区，恢复进入前的中断使能状态。 */
static void UISync_ExitCritical(uint32_t primask)
{
    if (primask == 0U)
    {
        __enable_irq();
    }
}

/* 确保模块内部的头尾哨兵链表已经完成初始化。 */
static void UISync_DeviceManagerEnsureInitialized(void)
{
    if (gUISyncDeviceManager.IsInitialized != 0U)
    {
        return;
    }

    (void)memset(&gUISyncDeviceManager, 0, sizeof(gUISyncDeviceManager));
    gUISyncDeviceManager.Head.Next = &gUISyncDeviceManager.Tail;
    gUISyncDeviceManager.Tail.Prev = &gUISyncDeviceManager.Head;
    gUISyncDeviceManager.IsInitialized = 1U;
}

/* 将设备节点插入到指定节点之前。 */
static void UISync_DeviceInsertBefore(UISync_DeviceNodeTypeDef *pos, UISync_DeviceNodeTypeDef *node)
{
    node->Prev = pos->Prev;
    node->Next = pos;
    pos->Prev->Next = node;
    pos->Prev = node;
}

/* 按 rank 升序将设备节点插入到统一设备表中。 */
static void UISync_DeviceInsertByRank(UISync_DeviceNodeTypeDef *node)
{
    UISync_DeviceNodeTypeDef *cursor = gUISyncDeviceManager.Head.Next;

    while (cursor != &gUISyncDeviceManager.Tail)
    {
        if (cursor->Rank > node->Rank)
        {
            break;
        }
        cursor = cursor->Next;
    }

    UISync_DeviceInsertBefore(cursor, node);
}

/* 将设备节点从统一设备表中摘除。 */
static void UISync_DeviceRemove(UISync_DeviceNodeTypeDef *node)
{
    if ((node == NULL) || (node->Prev == NULL) || (node->Next == NULL))
    {
        return;
    }

    node->Prev->Next = node->Next;
    node->Next->Prev = node->Prev;
    node->Prev = NULL;
    node->Next = NULL;
}

/* 更新单个 Key 的采样、滤波和上升沿事件触发状态。 */
static void UISync_KeyUpdate(UISync_DeviceNodeTypeDef *node)
{
    struct UISync_KeyHandleTypeDef *hKey = (struct UISync_KeyHandleTypeDef *)node;
    uint8_t sample;
    uint8_t threshold;

    if (hKey == NULL)
    {
        return;
    }

    sample = (hKey->InitBackup.KeyIsPressed() != 0U) ? 1U : 0U;
    threshold = hKey->InitBackup.FilterThreshold;

    if (sample != hKey->SampleLevel)
    {
        hKey->SampleLevel = sample;
        hKey->SampleCount = 1U;
        return;
    }

    if (hKey->SampleCount < threshold)
    {
        hKey->SampleCount++;
    }

    if ((hKey->SampleCount >= threshold) && (hKey->StableLevel != hKey->SampleLevel))
    {
        uint8_t oldStableLevel = hKey->StableLevel;
        hKey->StableLevel = hKey->SampleLevel;

        if ((oldStableLevel == 0U) && (hKey->StableLevel == 1U))
        {
            hKey->InitBackup.KeyEvent();
        }
    }
}

/* 初始化 UI_Sync 的统一设备管理器。 */
void UISync_Init(void)
{
    uint32_t primask = UISync_EnterCritical();
    (void)memset(&gUISyncDeviceManager, 0, sizeof(gUISyncDeviceManager));
    gUISyncDeviceManager.Head.Next = &gUISyncDeviceManager.Tail;
    gUISyncDeviceManager.Tail.Prev = &gUISyncDeviceManager.Head;
    gUISyncDeviceManager.IsInitialized = 1U;
    UISync_ExitCritical(primask);
}

/* 遍历所有已注册设备，并按 rank 顺序执行其更新函数。 */
void UISync_Update(void)
{
    UISync_DeviceNodeTypeDef *cursor;
    UISync_DeviceManagerEnsureInitialized();

    cursor = gUISyncDeviceManager.Head.Next;
    while (cursor != &gUISyncDeviceManager.Tail)
    {
        UISync_DeviceNodeTypeDef *next = cursor->Next;
        if ((cursor->IsEnabled != 0U) && (cursor->Update != NULL))
        {
            cursor->Update(cursor);
        }
        cursor = next;
    }
}

/* 为一个 Key 创建设备句柄、备份初始化参数并注册到设备表。 */
UISync_KeyHandle UISync_KeyInit(const UISync_KeyInitTypeDef *pKeyInit)
{
    struct UISync_KeyHandleTypeDef *hKey;
    uint32_t primask;

    if ((pKeyInit == NULL) || (pKeyInit->KeyIsPressed == NULL) || (pKeyInit->KeyEvent == NULL))
    {
        return NULL;
    }

    UISync_DeviceManagerEnsureInitialized();

    hKey = (struct UISync_KeyHandleTypeDef *)malloc(sizeof(struct UISync_KeyHandleTypeDef));
    if (hKey == NULL)
    {
        return NULL;
    }

    (void)memset(hKey, 0, sizeof(*hKey));
    hKey->InitBackup = *pKeyInit;
    if (hKey->InitBackup.FilterThreshold == 0U)
    {
        hKey->InitBackup.FilterThreshold = 1U;
    }

    hKey->DeviceNode.Rank = hKey->InitBackup.Rank;
    hKey->DeviceNode.IsEnabled = 1U;
    hKey->DeviceNode.Update = UISync_KeyUpdate;

    primask = UISync_EnterCritical();
    UISync_DeviceInsertByRank(&hKey->DeviceNode);
    UISync_ExitCritical(primask);

    return (UISync_KeyHandle)hKey;
}

/* 从设备表中移除指定 Key，并释放对应的动态内存。 */
void UISync_KeyDeInit(UISync_KeyHandle hKey)
{
    struct UISync_KeyHandleTypeDef *key = (struct UISync_KeyHandleTypeDef *)hKey;
    uint32_t primask;

    if (key == NULL)
    {
        return;
    }

    primask = UISync_EnterCritical();
    UISync_DeviceRemove(&key->DeviceNode);
    UISync_ExitCritical(primask);

    free(key);
}

/* 使能一个已注册 Key。 */
void UISync_KeyEnable(UISync_KeyHandle hKey)
{
    struct UISync_KeyHandleTypeDef *key = (struct UISync_KeyHandleTypeDef *)hKey;
    if (key == NULL)
    {
        return;
    }
    key->DeviceNode.IsEnabled = 1U;
}

/* 失能一个已注册 Key。 */
void UISync_KeyDisable(UISync_KeyHandle hKey)
{
    struct UISync_KeyHandleTypeDef *key = (struct UISync_KeyHandleTypeDef *)hKey;
    if (key == NULL)
    {
        return;
    }
    key->DeviceNode.IsEnabled = 0U;
}

/* 手动触发一个已注册 Key 的事件回调。 */
void UISync_KeyTriggerEvent(UISync_KeyHandle hKey)
{
    struct UISync_KeyHandleTypeDef *key = (struct UISync_KeyHandleTypeDef *)hKey;
    if ((key == NULL) || (key->InitBackup.KeyEvent == NULL))
    {
        return;
    }
    key->InitBackup.KeyEvent();
}

/* 修改一个已注册 Key 的 rank，并将其重新插回有序设备表。 */
void UISync_KeySetRank(UISync_KeyHandle hKey, uint8_t rank)
{
    struct UISync_KeyHandleTypeDef *key = (struct UISync_KeyHandleTypeDef *)hKey;
    uint32_t primask;

    if (key == NULL)
    {
        return;
    }

    primask = UISync_EnterCritical();
    UISync_DeviceRemove(&key->DeviceNode);
    key->DeviceNode.Rank = rank;
    key->InitBackup.Rank = rank;
    UISync_DeviceInsertByRank(&key->DeviceNode);
    UISync_ExitCritical(primask);
}

/*
使用方法示例

1. 在一个用户文件中先定义按键句柄。

    static UISync_KeyHandle gUserKeyHandle;

2. 提供“读取当前按键电平”的函数。
    该函数返回 0 或 1。
    返回 1 表示当前检测到按键按下。

    static uint8_t UserKey_IsPressed(void)
    {
         return (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET) ? 1U : 0U;
    }

3. 提供“按键事件触发”的函数。
    该函数不负责滤波，只负责在 UI_Sync 确认稳定上升沿后执行对应业务逻辑。

    static void UserKey_OnPressed(void)
    {
         // 在这里编写按键触发后的业务逻辑。
    }

4. 在初始化阶段填写 UISync_KeyInitTypeDef 并注册按键。
    Rank 越小，更新优先级越高。
    FilterThreshold 表示连续多少次采样一致后，才确认电平稳定。

    void UserInput_Init(void)
    {
         UISync_KeyInitTypeDef keyInit;

         UISync_Init();

         keyInit.Rank = 1U;
         keyInit.FilterThreshold = 3U;
         keyInit.KeyIsPressed = UserKey_IsPressed;
         keyInit.KeyEvent = UserKey_OnPressed;

         gUserKeyHandle = UISync_KeyInit(&keyInit);
    }

5. 在固定周期中断中调用 UISync_Update。
    例如每隔 1ms、5ms 或 10ms 调用一次。
    只要周期固定，FilterThreshold 就能对应成稳定的滤波时间。

    void TIMx_IRQHandler(void)
    {
         // 先清除定时器中断标志。
         UISync_Update();
    }

6. 如果要临时屏蔽某个按键，可调用：

    UISync_KeyDisable(gUserKeyHandle);

    恢复更新时调用：

    UISync_KeyEnable(gUserKeyHandle);

7. 如果要动态调整优先级，可调用：

    UISync_KeySetRank(gUserKeyHandle, 0U);

    调整后该按键会按新的 rank 重新插入到统一设备表中。

8. 如果要手动触发一次按键事件，可调用：

    UISync_KeyTriggerEvent(gUserKeyHandle);

    这会直接执行注册时提供的 KeyEvent，不经过滤波判定。

9. 如果不再需要该按键句柄，可调用：

    UISync_KeyDeInit(gUserKeyHandle);
    gUserKeyHandle = NULL;

10. 当前版本的事件语义。
     当前实现只在“滤波确认后的稳定上升沿”触发一次 KeyEvent。
     也就是说：
     - 按键从未按下变为稳定按下时，会触发一次事件。
     - 按键持续保持按下时，不会重复触发。
     - 按键释放时，当前版本不会触发事件。

11. 关于 FilterThreshold 的理解。
     假设中断周期为 5ms：
     - FilterThreshold = 1，约等于不做连续滤波确认。
     - FilterThreshold = 3，表示需要连续 3 次一致采样，即约 15ms 后确认稳定。
     - FilterThreshold = 5，表示需要连续 5 次一致采样，即约 25ms 后确认稳定。

12. 关于多设备更新顺序。
     如果注册多个设备，UI_Sync 会按 rank 从小到大顺序更新。
     如果 rank 相同，则保持它们被注册时的先后顺序。
*/
