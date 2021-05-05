local width  = horizon.width - 1
local height = horizon.height - 1

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

-- god sort
local function cmppixel_4(p, q)
    local p3 = p[3] * math.pi
    local q3 = q[3] * math.pi

    local p1 = p[1] * math.pi + math.cos(p3)
    local q1 = q[1] * math.pi + math.cos(q3)

    local a = p[2] * math.sin(p1)
    local b = q[2] * math.sin(q1)

    return a < b
end

-- params
local chan = bit.band(horizon.params.chan or 1, 3)
local cmp = nil
    or ((horizon.params.sort == 1) and cmppixel_1)
    or ((horizon.params.sort == 2) and cmppixel_2)
    or ((horizon.params.sort == 3) and cmppixel_3(chan))
    or ((horizon.params.sort == 4) and cmppixel_4)
    or cmppixel_2 -- default

-- main
local function lerp(x, y, w)
    return (1.0 - w)*x + w*y
end

local function pixand(p, q)
    local blend = horizon.params.blend
    if blend then
        p[1] = lerp(bit.band(p[1], q[1]), p[1], blend)
        p[2] = lerp(bit.band(p[2], q[2]), p[2], blend)
        p[3] = lerp(bit.band(p[3], q[3]), p[3], blend)
    else
        p[1] = bit.band(p[1], q[1])
        p[2] = bit.band(p[2], q[2])
        p[3] = bit.band(p[3], q[3])
    end
end

for y=0,height do
    local row = {}
    for x=0,width do
        local input = horizon.getpixel(x, y)
        table.insert(row, input)
    end
    table.sort(row, cmp)
    for x=0,width do
        local output = nil
        if horizon.params['and'] == 1 then
            output = horizon.getpixel(x, y)
            pixand(output, row[x+1])
        else
            output = row[x+1]
        end
        horizon.setpixel(x, y, output)
    end
end
