local width  = horizon.width - 1
local height = horizon.height - 1

local row = {}

local function clear(tab)
    for k,v in pairs(tab) do
        tab[k] = nil
    end
end

-- sort based on pixel int values
local function cmppixel_1(p, q)
    local pr = p[1]
    local pg = bit.lshift(p[2], 8)
    local pb = bit.lshift(p[3], 16)

    local qr = q[1]
    local qg = bit.lshift(q[2], 8)
    local qb = bit.lshift(q[3], 16)

    local pp = bit.bor(pr, pg, pb)
    local qp = bit.bor(qr, qg, qb)

    return pp < qp
end

-- sort based on pixel gamma values
local function cmppixel_2(p, q)
    local yp = bit.rshift(p[1]*19595 + p[2]*38470 + p[3]*7471 + 0x8000, 24)
    local yq = bit.rshift(q[1]*19595 + q[2]*38470 + q[3]*7471 + 0x8000, 24)
    return yp < yp
end

-- sort based on a color channel
local function cmppixel_3(chan)
    return function(p, q)
        return p[chan] < q[chan]
    end
end

local chan = bit.band(horizon.params.chan or 1, 3)
local cmp =
    ((horizon.params.sort == 1) and cmppixel_1)
    or ((horizon.params.sort == 2) and cmppixel_2)
    or ((horizon.params.sort == 3) and cmppixel_3(chan))
    or cmppixel_1 -- default

for y=0,height do
    for x=0,width do
        local input = horizon.getpixel(x, y)
        table.insert(row, input)
    end
    table.sort(row, cmp)
    for x=0,width do
        horizon.setpixel(x, y, row[x+1])
    end
    clear(row)
end
