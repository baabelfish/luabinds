#pragma once

#define LUA_PUSH_HELPER(FUNC, ...)\
    template<> inline void luaPush<__VA_ARGS__>(lua_State* state, __VA_ARGS__ param) {\
        FUNC(state, param); }

#define LUA_GET_HELPER(FUNC, TEST, ...)\
template<> inline __VA_ARGS__ luaGet<__VA_ARGS__>(lua_State* state, int index) {\
    if (!TEST(state, index)) { throw lua::exceptions::NotValidType("Given type is not a number"); }\
    return FUNC(state, index);\
};

