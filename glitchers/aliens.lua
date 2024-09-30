local width  = horizon.width - 1
local height = horizon.height - 1

local output = {0, 0, 0}

for y=0,height do
    local yoff = math.abs(y * math.sin(math.pi * y/2))
    for x=0,width do
        local xoff = math.abs(x * math.cos(math.sqrt(x)))
        local input = horizon.getpixel(x, y)
        local transform = horizon.getpixel(xoff, yoff)
        for k=1,3 do
            local i = input[k]
            local t = transform[k]
            output[k] = (t == 0) and i or t
        end
        horizon.setpixel(x, y, output)
    end
end
