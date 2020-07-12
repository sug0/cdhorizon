local width  = horizon.width - 1
local height = horizon.height - 1

local output = {0, 0, 0}

for y=0,height do
    for x=0,width do
        local param = math.log(math.sin((x-y)*math.pi) + y)
        local x0    = math.min(x*math.random(), math.abs(math.floor(x*math.cos(param))))
        local y0    = math.max(y*math.random(), math.abs(math.floor(y * math.sin(param))))
        local input = horizon.getpixel(x0, y0)

        output[1] = input[1]
        output[2] = input[2]
        output[3] = input[3]

        horizon.setpixel(x, y, output)
    end
end
