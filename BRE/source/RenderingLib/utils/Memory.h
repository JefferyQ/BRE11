#pragma once

//////////////////////////////////////////////////////////////////////////
//
// Memory related utility functions
//
//////////////////////////////////////////////////////////////////////////

#define DELETE_OBJECT(object) if((object) != nullptr) { delete object; object = nullptr; }
#define DELETE_OBJECTS(objects) if((objects) != nullptr) { delete[] objects; objects = nullptr; }
#define RELEASE_OBJECT(object) if((object) != nullptr) { object->Release(); object = nullptr; }