local width  = horizon.width - 1
local height = horizon.height - 1

local gamma  = 0
local output = {0, 0, 0}

local a1   = 0.432 * math.pi
local a2   = 0.152 * math.pi
local xfix = horizon.width/2
local yfix = -horizon.height/2

local function transform(ag, x, y)
    local x0 = x*math.cos(ag) - y*math.sin(ag)
    local y0 = x*math.sin(ag) + y*math.cos(ag)
    local x1 = math.abs(x0 + xfix) % horizon.width
    local y1 = math.abs(y0 + yfix) % horizon.height
    return horizon.getpixel(x1, y1)
end

for y=0,height do
    for x=0,width do
        local input = horizon.getpixel(x, y)

        local t1 = transform(a1, x, y)
        local t2 = transform(a2, x, y)

        output[1] = bit.bxor(input[1]-x, t1[1], bit.bnot(bit.band(t2[1], gamma)))
        output[2] = bit.bxor(input[2]-x, t1[2], bit.bnot(bit.band(t2[2], gamma)))
        output[3] = bit.bxor(input[3]-x, t1[3], bit.bnot(bit.band(t2[3], gamma)))
        gamma     = bit.rshift(output[1]*19595 + output[2]*38470 + output[3]*7471 + 0x8000, 24)

        horizon.setpixel(x, y, output)
    end
end
