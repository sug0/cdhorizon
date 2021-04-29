local width  = horizon.width - 1
local height = horizon.height - 1

for y=0,height do
    for x=0,width do
        local output = horizon.getpixel(x, y)
        horizon.setpixel(x, y, output)
    end
end
