#pragma once

typedef struct OverlayManager OverlayManager;

typedef int (*OverlayFunction)(OverlayManager *manager, int *state);

typedef struct OverlayManagerTemplate {
    OverlayFunction init;
    OverlayFunction exec;
    OverlayFunction exit;
    FSOverlayID ovy_id;
} OverlayManagerTemplate;

struct OverlayManager {
    OverlayManagerTemplate template;
    int exec_state;
    int proc_state;
    void *args;
    void *data;
    u32 unk_20;
    u32 unk_24;
};

OverlayManager *OverlayManager_New(const OverlayManagerTemplate *template, void *parentWork, HeapID heapId);
void OverlayManager_Delete(OverlayManager *man);
void *OverlayManager_CreateAndGetData(OverlayManager *man, u32 size, HeapID heapId);
void *OverlayManager_GetData(OverlayManager *man);
void OverlayManager_FreeData(OverlayManager *man);
void *OverlayManager_GetArgs(OverlayManager *man);
BOOL OverlayManager_Run(OverlayManager *man);