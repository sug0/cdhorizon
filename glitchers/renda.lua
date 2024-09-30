local width  = horizon.width - 1
local height = horizon.height - 1

local o = {0, 0, 0}

local function bw(c)
    return bit.ror(c[1]*19595 + c[2]*38470 + c[3]*7471 + 0x8000, 16)
end

local function transform(x, y)
    local x0 = math.floor(math.abs(x * math.sin(x - y)))
    local y0 = math.floor(math.abs(y * math.sin(x + y)))
    return horizon.getpixel(x0, y0)
end

local function operate(output, x, y, f)
    output[1] = f(x[1], y[1])
    output[2] = f(x[2], y[2])
    output[3] = f(x[3], y[3])
    return output
end

local min = math.huge
local max = 0

for y=0,height do
    for x=0,width do
        local i1 = horizon.getpixel(x, y)
        local i2 = transform(x, y)
        local g  = bw(operate(o, i1, i2, bit.bxor))

        min = math.min(min, g)
        max = math.max(max, g)
        dif = max - min

        o[1] = bit.band(g, dif)
        o[2] = bit.band(g, dif)
        o[3] = bit.band(g, dif)

        horizon.setpixel(x, y, o)
    end
end
