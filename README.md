Luabinds [![Build Status](https://travis-ci.org/baabelfish/luabinds.svg?branch=master)](https://travis-ci.org/baabelfish/luabinds)
========

Simple and easy bindings for Lua that try to leverage from C++14.


# Features
- Execute Lua scripts
- Get values from Lua tables and globals
- Get generic function objects from Lua functions
- Use C++ functions from Lua
- Header only

# Installation
- Simply include ``luabinds.hpp`` and compile with c++14 support and lua linking

## Running the tests
- You need moonscript compiler to run the tests

```bash
sudo pacman -S luarocks
# Ubuntu/Debian:
# sudo apt-get -qq install luarocks

sudo luarocks install moonscript

git clone https://github.com/baabelfish/luabinds
cd luabinds
cmake .
make
./ytest
```

# Usage

## Basics
### Introduce a lua script to be used in the following examples
```lua
-- Example: somefile.lua

-- Table
player = {
    health = 100,
    mana = 25,
    stamina = 125,

    minion = {
        health = 10
    }
}

-- Global variable
global = "string"

-- Void function
function singlePrint(var)
    print(var)
end

-- Variadic parameter function with single return value
function sum(...)
    result = 0;
    for _, v in ipairs({...}) do
        result = result + v
    end
    return result
end

-- Multi return value function
function multire()
    return 1, "something", 42
end

-- These will work only with the last example
print(cppSum(1, 2, 5)) -- Prints: 8
print(cppHello("you")) -- Prints: Hello you!

print("All done")

```

### Evaluating the script

```cpp
lua::Lua lua("somefile.lua"); // Evals the file and prints "All done" doing so
```

### Accessing data

```cpp
auto some_global = lua.get("global"); // "get" defaults to std::string without template args
assert(some_global == "string");

auto minion_health = lua.get<int>("player.minion.health");
assert(minion_health == 10);
```

## Calling Lua functions from C++

### Functions with no return values
```cpp
lua.call("singlePrint")("Hello and print me!"); // will call function "singlePrint" with single string argument
```

### Store Lua functions as objects
```cpp
auto f = lua.call("singlePrint"); // Gets the function
f("Hello and print me!"); // Call it
```

### Functions with one return value and variadic functions
```cpp
auto result = lua.call<int>("sum")(1, 2); // result type: int
auto sum = lua.call<int>("sum");
sum(1, 2, 3); // == 6
sum(1, 2, "3", 4); // == 10
sum(1, 2, 3, 4, 5); // == 15
```

### Functions with multiple return values
```cpp
auto result = lua.call<int, std::string, int>("multire")(); // result type is: std::tuple<int, std::string, int>
assert(std::get<1>(result), "something");
```

## Calling C++ functions from Lua
- Warning! This will change in the future.

```cpp
// Exposes cppHello and cppSum functions 
std::string lastVisitor;
lua::Lua lua("tests/luatest.lua",
    "cppHello", [&](lua::State& s) {
        lastVisitor = s.get(0);
        s.push("Hello " + lastVisitor + "!");
    },
    "cppSum", [](lua::State& s) {
        int sum = 0;
        for (int i = 1; i < s.sizeParameters() + 1; ++i) { sum += s.get<int>(i); }
        s.push(sum);
    });
```


# Caveats
- Does not support all functionality lua provides for c++
- There are some ugly hacks that might not work in the future

# TODO
- Change script while running it
