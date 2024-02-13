//
// Created by c3042750 on 13/02/2024.
//

#ifndef SPEEDFUN_UTILS_H
#define SPEEDFUN_UTILS_H

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "lua.hpp"

using namespace NCL;
using namespace Maths;

inline int getIntField(lua_State* L, const char* key) {
    lua_pushstring(L, key);
    lua_gettable(L, -2);
    if (!lua_isnumber(L, -1)) {
        luaL_error(L, "field is not an integer");
    }
    auto result = (int)lua_tonumber(L, -1);
    lua_pop(L, 1);
    return result;
}

inline double getNumberField(lua_State* L, const char* key) {
    lua_pushstring(L, key);
    lua_gettable(L, -2);
    if (!lua_isnumber(L, -1)) {
        luaL_error(L, "field is not an number");
    }
    auto result = lua_tonumber(L, -1);
    lua_pop(L, 1);
    return result;
}

inline bool getBool(lua_State* L, const char* key) {
    lua_pushstring(L, key);
    lua_gettable(L, -2);
    if (!lua_isboolean(L, -1)) {
        luaL_error(L, "field is not a boolean");
    }
    auto result = lua_toboolean(L, -1);
    lua_pop(L, 1);
    return result;
}

inline const char* getStringField(lua_State* L, const char* key) {
    lua_pushstring(L, key);
    lua_gettable(L, -2);
    if (!lua_isstring(L, -1)) {
        luaL_error(L, "field is not a string");
    }
    auto result = lua_tostring(L, -1);
    lua_pop(L, 1);
    return result;
}

inline Vector3 getVec3Field(lua_State* L, const char* key) {
    Vector3 v;
    lua_pushstring(L, key);
    lua_gettable(L, -2);
    // -1 table -2 table

    v.x = (float)getNumberField(L, "x");
    v.y = (float)getNumberField(L, "y");
    v.z = (float)getNumberField(L, "z");

    // Pop the vector table
    lua_pop(L, 1);

    return v;
}

inline Vector3 getVec3Field(lua_State* L, int key) {
    Vector3 v;
    lua_pushinteger(L, key);
    lua_gettable(L, -2);
    // -1 table -2 table

    v.x = (float)getNumberField(L, "x");
    v.y = (float)getNumberField(L, "y");
    v.z = (float)getNumberField(L, "z");

    // Pop the vector table
    lua_pop(L, 1);

    return v;
}

inline Vector4 getVec4Field(lua_State* L, const char* key) {
    Vector4 v;
    lua_pushstring(L, key);
    lua_gettable(L, -2);

    v.x = (float)getNumberField(L, "x");
    v.y = (float)getNumberField(L, "y");
    v.z = (float)getNumberField(L, "z");
    v.w = (float)getNumberField(L, "w");

    lua_pop(L, 1);

    return v;
}



inline void setIntField(lua_State* L,  const char* key, int value) {
    if (!lua_istable(L, -1)) {
        luaL_error(L, "element on stack is not a table!");
    }
    lua_pushstring(L, key);
    lua_pushnumber(L, value);
    lua_settable(L, -3);
}

inline void setFloatField(lua_State* L, const char* key, float value) {
    if (!lua_istable(L, -1)) {
        luaL_error(L, "element on stack is not a table!");
    }
    lua_pushstring(L, key);
    lua_pushnumber(L, value);
    lua_settable(L, -3);
}

inline int setVec3Field(lua_State* L, const char* key, const Vector3& value) {
    if (!lua_istable(L ,-1)) {
        luaL_error(L, "element on stack not a table");
    }
    lua_getglobal(L, "Vector3");
    lua_getfield(L, -1, "new");
    lua_getglobal(L, "Vector3");
    lua_pushnumber(L, value.x);
    lua_pushnumber(L, value.y);
    lua_pushnumber(L, value.z);

    auto result = lua_pcall(L, 4, 1, 0);
    if (result) {
        std::cerr << lua_tostring(L, 1) << std::endl;
    }


}

inline int getTableSize(lua_State* L) {
    if (!lua_istable(L, -1)) {
        luaL_error(L, "Aint a table on top of the stack");
    }

    lua_len(L, -1);
    auto length = (int)lua_tonumber(L, -1);
    lua_pop(L, 1);
    return length;
}


#endif //SPEEDFUN_UTILS_H
