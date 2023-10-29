

struct RectRenderData {
    vec4 color;
   // mat4 model;
};

struct RectBuffer {
    int32 count;
    int32 capacity;
    RectRenderData *data;

    uint32 bufferID;
    uint32 bufferSize;
};
