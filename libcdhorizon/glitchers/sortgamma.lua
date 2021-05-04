local width  = horizon.width - 1
local height = horizon.height - 1

local row = {}

local function clear(tab)
    for k,v in pairs(tab) do
        tab[k] = nil
    end
end

local function cmppixel(p, q)
    local yp = bit.rshift(p[1]*19595 + p[2]*38470 + p[3]*7471 + 0x8000, 24)
    local yq = bit.rshift(q[1]*19595 + q[2]*38470 + q[3]*7471 + 0x8000, 24)
    return yp < yp
end

for y=0,height do
    for x=0,width do
        local input = horizon.getpixel(x, y)
        table.insert(row, input)
    end
    table.sort(row, cmppixel)
    for x=0,width do
        horizon.setpixel(x, y, row[x+1])
    end
    clear(row)
end
