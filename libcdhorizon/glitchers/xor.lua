local width  = horizon.width - 1
local height = horizon.height - 1

local output = {0, 0, 0}

for y=0,height do
    for x=0,width do
        local input = horizon.getpixel(x, y)
        output[1] = bit.bxor(output[1], input[1])
        output[2] = bit.bxor(output[2], input[2])
        output[3] = bit.bxor(output[3], input[3])
        horizon.setpixel(x, y, output)
    end
end
