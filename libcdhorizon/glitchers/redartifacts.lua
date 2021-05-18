local width  = horizon.width - 1
local height = horizon.height - 1

local output = {0, 0, 0}

for y=0,height do
    for x=0,width do
        local input = horizon.getpixel(x, y)

        output[1] = math.sin(math.exp(input[1] - input[2], input[3]) * math.pi) * horizon.width
        output[2] = bit.band(input[1], input[2], input[3])
        output[3] = output[2]

        horizon.setpixel(x, y, output)
    end
end
