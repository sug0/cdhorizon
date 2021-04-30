local width  = horizon.width - 1
local height = horizon.height - 1

local output = {0, 0, 0}

function lerp(x, y, w)
    return (1.0 - w)*x + w*y
end

local wght = math.random()
while wght > 0.2 do
    wght = math.random()
end

for x=0,width do
    for y=0,height do
        local x0 = math.abs(math.floor(width * math.sin(output[1] - output[2])))
        local y0 = math.abs(math.floor(height * math.cos(output[2] - output[3])))

        local this  = horizon.getpixel(x, y)
        local other = horizon.getpixel(x0, y0)

        output[1] = lerp(this[1], other[2], wght)
        output[2] = lerp(this[2], other[3], wght)
        output[3] = lerp(this[3], other[1], wght)

        horizon.setpixel(x, y, output)
    end
end
