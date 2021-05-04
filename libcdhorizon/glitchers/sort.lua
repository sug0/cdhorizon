local width  = horizon.width - 1
local height = horizon.height - 1

local row = {}

local function clear(tab)
    for k,v in pairs(tab) do
        tab[k] = nil
    end
end

local function cmppixel(p, q)
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
