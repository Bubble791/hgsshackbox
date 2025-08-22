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