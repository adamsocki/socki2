#include <iostream>
#include <fstream>
#include <string>





//void InitGlyphBuffers(int32 count) {
//    for (int i = 0; i < count; i++) {
//        GlyphBuffer *buffer = &Game->glyphBuffers[i];
//
//        buffer->capacity = GlyphBufferCapacity;
//        buffer->size = buffer->capacity * sizeof(GlyphData);
//        buffer->data = (GlyphData *)malloc(buffer->size);
//        memset(buffer->data, 0, buffer->size);
//
//        glGenBuffers(1, (GLuint *)&buffer->bufferID);
//        glBindBuffer(GL_ARRAY_BUFFER, buffer->bufferID);
//        glBufferData(GL_ARRAY_BUFFER, buffer->size, buffer->data, GL_STATIC_DRAW);
//    }
//}

inline bool glCheckError_(char *file, uint32 line) {
    GLenum _glError = glGetError();
    if (_glError != GL_NO_ERROR) {
        Print("OpenGL error (%s:%d): 0x%x (%d)\n", file, line, _glError, _glError);
        return true;
    }

    return false;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__);
void LoadShader(const char *vertPath, const char *fragPath, Shader *shader) {
    FILE *file = fopen(vertPath, "r");

    if (file != NULL) {
        fseek(file, 0, SEEK_END);
        shader->vertSize = ftell(file);
        fseek(file, 0, SEEK_SET);

        shader->vertSrc = (char *)malloc(shader->vertSize + 1);
        int index = 0;
        int c = fgetc(file);
        while (c != EOF) {
            shader->vertSrc[index++] = c;
            c = fgetc(file);
        }

        shader->vertSrc[index] = 0;

        fclose(file);
    }
    else {
        Print("Error opening file %s", vertPath);
    }

    file = fopen(fragPath, "r");

    if (file != NULL) {
        fseek(file, 0, SEEK_END);
        shader->fragSize = ftell(file);
        fseek(file, 0, SEEK_SET);

        shader->fragSrc = (char *)malloc(shader->fragSize + 1);
        int index = 0;
        int c = fgetc(file);
        while (c != EOF) {
            shader->fragSrc[index++] = c;
            c = fgetc(file);
        }

        shader->fragSrc[index] = 0;

        fclose(file);
    }
    else {
        Print("Error opening file %s", fragPath);
    }
}

bool ShaderCompiled(GLuint shader, char **infoLog) {
    int32 isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        if (infoLog != NULL) {
            GLint maxLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

            maxLength = 256;

            *infoLog = (GLchar *)malloc(sizeof(GLchar) * maxLength);
            glGetShaderInfoLog(shader, maxLength, &maxLength, (GLchar *)*infoLog);
        }

        Print(*infoLog);

        glDeleteShader(shader);
    }

    return isCompiled;
}

bool ShaderLinked(GLuint shader, char **infoLog) {
    int32 isLinked = 0;
    glGetProgramiv(shader, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE) {
        if (infoLog != NULL) {
            GLint maxLength = 0;
            glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

            maxLength = 256;

            *infoLog = (GLchar *)malloc(sizeof(GLchar) * maxLength);
            glGetProgramInfoLog(shader, maxLength, &maxLength, (GLchar *)*infoLog);
        }

        Print(*infoLog);

        glDeleteShader(shader);
    }

    return isLinked;
}


void CompileShader(Shader *shader, uint32 uniformCount, const char **uniformNames) {
    char *infoLog = NULL;

    if (shader->vertSrc != NULL) {
        shader->vertID = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(shader->vertID, 1, (const GLchar**)&shader->vertSrc, (GLint *)&shader->vertSize);
        glCheckError();
        glCompileShader(shader->vertID);
        glCheckError();

        Print("checking vert shader");
        ShaderCompiled(shader->vertID, &infoLog);
    }
    if (shader->fragSrc != NULL) {
        shader->fragID = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(shader->fragID, 1, (const GLchar**)&shader->fragSrc, (GLint *)&shader->fragSize);
        glCheckError();
        glCompileShader(shader->fragID);
        glCheckError();

        Print("checking frag shader");
        ShaderCompiled(shader->vertID, &infoLog);
    }


    shader->programID = glCreateProgram();

    if (!glIsProgram(shader->programID)) {
        Print("NOT A SHADER!");
    }

    if (shader->vertID != 0) {
        glAttachShader(shader->programID, shader->vertID);
        glCheckError();
    }
    if (shader->fragID != 0) {
        glAttachShader(shader->programID, shader->fragID);
        glCheckError();
    }

    glLinkProgram(shader->programID);
    glCheckError();

    ShaderLinked(shader->programID, &infoLog);


    shader->uniformCount = uniformCount;

    shader->uniforms = (ShaderUniform *)malloc(sizeof(ShaderUniform) * uniformCount);

    for (int i = 0; i < uniformCount; i++) {
        ShaderUniform *uniform = &shader->uniforms[i];
        const char *uniformName = uniformNames[i];

        uint32 nameLen = strlen(uniformName);
        uniform->name = (char *)malloc(nameLen + 1);
        memcpy(uniform->name, uniformName, nameLen);
        uniform->name[nameLen] = 0;

        uniform->id = glGetUniformLocation(shader->programID, uniform->name);
        glCheckError();

        if (uniform->id >= 0) {
            Print("Setting uniform %s", uniform->name);
        }
        else {
            Print("failed to set %s", uniform->name);
        }
        ShaderCompiled(shader->vertID, &infoLog);
    }
}
void AllocateRectBuffer(int32 capacity, RectBuffer *buffer) {
    buffer->count = 0;
    buffer->capacity = capacity;
    buffer->bufferSize = sizeof(RectRenderData) * buffer->capacity;

    buffer->data = (RectRenderData *)malloc(buffer->bufferSize);
    memset(buffer->data, 0, buffer->bufferSize);

    glGenBuffers(1, &buffer->bufferID);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->bufferID);
    glBufferData(GL_ARRAY_BUFFER, buffer->bufferSize, buffer->data, GL_STREAM_DRAW);
}



void SetShader(Shader *shader) {
    glUseProgram(shader->programID);
    //glCheckError();
}

void RenderRectBuffer(RectBuffer *buffer) {
    Mesh *mesh = &Game->quad;

    Shader *shader = &Game->instancedQuadShader;
    SetShader(shader);

    glUniformMatrix4fv(shader->uniforms[0].id, 1, GL_FALSE, Game->camera.viewProjection.data);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBufferID);

    // Position
    int vert = glGetAttribLocation(shader->programID, "vertexPosition_modelspace");
    glEnableVertexAttribArray(vert);
    glVertexAttribPointer(vert, 3, GL_FLOAT, GL_FALSE, 0, (uint8 *)0);

    int32 stride = sizeof(RectRenderData);

    glBindBuffer(GL_ARRAY_BUFFER, buffer->bufferID);
    glBufferData(GL_ARRAY_BUFFER, buffer->bufferSize, buffer->data, GL_STREAM_DRAW);

    int model = glGetAttribLocation(shader->programID, "instance_model");
    int color = glGetAttribLocation(shader->programID, "instance_color");

    // color
    glEnableVertexAttribArray(color);
    glVertexAttribPointer(color, 4, GL_FLOAT, GL_FALSE, stride, (uint8 *)0);
    glVertexAttribDivisor(color, 1);

    // model column 0
    glEnableVertexAttribArray(model);
    glVertexAttribPointer(model, 4, GL_FLOAT, GL_FALSE, stride, (uint8 *)0 + sizeof(vec4));
    glVertexAttribDivisor(model, 1);

    // model column 1
    glEnableVertexAttribArray(model + 1);
    glVertexAttribPointer(model + 1, 4, GL_FLOAT, GL_FALSE, stride, (uint8 *)0 + sizeof(vec4) * 2);
    glVertexAttribDivisor(model + 1, 1);

    // model column 2
    glEnableVertexAttribArray(model + 2);
    glVertexAttribPointer(model + 2, 4, GL_FLOAT, GL_FALSE, stride, (uint8 *)0 + sizeof(vec4) * 3);
    glVertexAttribDivisor(model + 2, 1);

    // model column 3
    glEnableVertexAttribArray(model + 3);
    glVertexAttribPointer(model + 3, 4, GL_FLOAT, GL_FALSE, stride, (uint8 *)0 + sizeof(vec4) * 4);
    glVertexAttribDivisor(model + 3, 1);

    glDrawElementsInstanced(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (uint8 *)NULL + 0, buffer->count);

    glDisableVertexAttribArray(vert);
    glDisableVertexAttribArray(color);
    glDisableVertexAttribArray(model);
    glDisableVertexAttribArray(model + 1);
    glDisableVertexAttribArray(model + 2);
    glDisableVertexAttribArray(model + 3);

    glVertexAttribDivisor(vert, 0);
    glVertexAttribDivisor(color, 0);
    glVertexAttribDivisor(model, 0);
    glVertexAttribDivisor(model + 1, 0);
    glVertexAttribDivisor(model + 2, 0);
    glVertexAttribDivisor(model + 3, 0);
}
