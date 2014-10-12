function testfunc(x, y)
    d = x * x + y
    return d
end

function recursive(x)
    if (x > 1000) then
        return x
    else
        return recursive(x * 2)
    end
end

x = 5
a = x + 13

herp = testfunc(x, a)
recursive(1)
