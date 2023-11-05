
enum CameraType : uint8 {
    CameraType_Perspective,
    CameraType_Orthographic,
};

enum CameraControllerType
{
    ControllerType_FirstPerson,
    ControllerType_ThirdPerson,

    // Level Editor Camera Types,

    ControllerType_TopDown,

};

struct CameraFocusPoint
{


};

struct Camera
{
    CameraType type;
//    CameraControllerType controllerType;
    mat4 view;
    mat4 projection;
    mat4 viewProjection;

    // Orthographic
    real32 width;
    real32 height;

    real32 size;

};

void UpdateCamera(Camera *camera, vec3 position, quaternion rotation) {
    mat4 camWorld = TRS(position, rotation, V3(1));
    camera->view = OrthogonalInverse(camWorld);
    camera->viewProjection = camera->projection * camera->view;
}