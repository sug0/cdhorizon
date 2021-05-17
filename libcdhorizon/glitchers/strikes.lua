-- ported from:
-- https://github.com/jpoz/glitch/blob/master/glitch.go

local width  = horizon.width - 1
local height = horizon.height - 1

local strikes = horizon.params.strikes or 1000
local length  = horizon.params.length or 100

-- first pass, just set the image
for y=0,height do
    for x=0,width do
        local output = horizon.getpixel(x, y)
        horizon.setpixel(x, y, output)
    end
end

-- second pass, strike pixels
while strikes > 0 do
    local x  = math.random() * horizon.width
    local y  = math.random() * horizon.height
    local o1 = horizon.getpixel(x, y)

    local streakend = length < 0
        and width
        or  math.min(x+length, width)

    while x < streakend do
        local o2 = horizon.getpixel(x, y)

        o2[1] = 3*o1[1]/4 + o2[1]/4
        o2[2] = 3*o1[2]/4 + o2[2]/4
        o2[3] = 3*o1[3]/4 + o2[2]/4

        horizon.setpixel(x, y, o2)
        x = x + 1
    end

    strikes = strikes - 1
end
