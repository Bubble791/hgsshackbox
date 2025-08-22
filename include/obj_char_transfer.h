#pragma once

typedef struct ObjCharTransferTemplate {
    int maxTasks;
    u32 sizeMain;
    u32 sizeSub;
    HeapID heapId;
} ObjCharTransferTemplate;