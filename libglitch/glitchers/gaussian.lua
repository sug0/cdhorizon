local width  = horizon.width - 1
local height = horizon.height - 1

local kernel = {
    {1/16, 2/16, 1/16},
    {2/16, 4/16, 2/16},
    {1/16, 2/16, 1/16},
}
local output = {0, 0, 0}

local function convolve(x, y)
    output[1] = 0 ; output[2] = 0 ; output[3] = 0
    for ky=-1,1 do
        for kx=-1,1 do
            local input = horizon.getpixel(
                (x+kx) % horizon.width,
                (y+ky) % horizon.height
            )
            local k = kernel[ky+2][kx+2]
            output[1] = output[1] + input[1] * k
            output[2] = output[2] + input[2] * k
            output[3] = output[3] + input[3] * k
        end
    end
    return output
end

for y=0,height do
    for x=0,width do
        horizon.setpixel(x, y, convolve(x, y))
    end
end
