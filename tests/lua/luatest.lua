player = {
    health = 100,
    mana = 25,
    stamina = 125,
    name = "foobar",

    minion = {
        health = 10
    }
}

global = "string"

data_str = "a string variable"
data_integer = 10
data_boolean = true
data_float = 7.62

function ping()
    return "pong"
end

function multireval()
    return 1, "hello"
end

variable = 0
function noreval()
    variable = 1
end

function sum(...)
    result = 0;
    for _, v in ipairs({...}) do
        result = result + v
    end
    return result
end

cppsumValue = 0

if (cppsum ~= nil) then
    cppsumValue  = cppsum(1,2,3,4,5)
end

if (helloFromCpp ~= nil) then
    helloFromCpp("someone")
end

if (other ~= nil) then
    other()
end

somefunc = function()
    return 17
end

-- Function returning a function
function refunc(a)
    return function()
        print(a)
    end
end
