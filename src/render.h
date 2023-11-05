



struct ShaderUniform {
    char *name;
    int32 id;
};

struct Shader {
    uint32 vertSize;
    char *vertSrc;

    uint32 fragSize;
    char *fragSrc;

    int32 vertID;
    int32 fragID;
    int32 programID;

    int32 uniformCount;
    ShaderUniform *uniforms;
};


struct RectRenderData {
    vec4 color;
    mat4 model;
};

struct RectBuffer {
    int32 count;
    int32 capacity;
    RectRenderData *data;

    uint32 bufferID;
    uint32 bufferSize;
};





