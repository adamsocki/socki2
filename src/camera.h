
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
    CameraControllerType controllerType;
};