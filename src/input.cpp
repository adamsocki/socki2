// void AllocateInputManager(InputManager *input, MemoryArena *arena, int32 capacity, int32 deviceCapacity) {
//     input->events = MakeDynamicArray<InputEvent>(arena, capacity);

//     input->deviceCount = deviceCapacity;
//     input->devices = (InputDevice *)malloc(sizeof(InputDevice) * deviceCapacity);

//     memset(input->devices, 0, input->deviceCount * sizeof(InputDevice));

//     input->charSize = 32;
//     input->charCount = 0;
//     input->inputChars = (char *)malloc(input->charSize);
// }

// template <typename T>
// inline 