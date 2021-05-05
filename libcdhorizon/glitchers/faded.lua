local width  = horizon.width - 1
local height = horizon.height - 1

local output = {0, 0, 0}

local min = math.huge
local max = 0

local pmin = horizon.params.min or 0.3
local pmax = horizon.params.max or 0.3

for y=0,height do
    for x=0,width do
        local i = horizon.getpixel(x, y)
        local g = bit.bxor(max/32, bit.band(i[1], i[2], i[3]) + min/32)

        min = math.min(min + pmin * math.random(), g)
        max = math.max(max - pmax * math.random(), g)

        dif = math.abs(max + min)/6 + 30

        output[1] = dif
        output[2] = dif
        output[3] = dif

        horizon.setpixel(x, y, output)
    end
end
