local Vector4 = {}

function Vector4:new(x,y,z,w)
    local v = {}
    setmetatable(v, self)
    v.x = x
    v.y = y
    v.z = z
    v.w = w
    return v
end

function Vector4:newRGB(x,y,z,w)
    return Vector4:new(x / 255.0, y / 255.0, z / 255.0, w / 255.0)
end

Vector4.__tostring = function(vec)
    return vec.x .. vec.y .. vec.z .. vec.w
end

Vector4.__mul = function(vec, scale)
    return Vector4:new(vec.x * scale, vec.y * scale, vec.z * scale, vec.w * scale)
end


local Vector3 = {}

function Vector3:new(x, y, z)
    local v = {}
    setmetatable(v, self)
    v.x = x
    v.y = y
    v.z = z
    return v
end

Vector3.__tostring = function(vec)
    return vec.x .. vec.y .. vec.z
end

Vector3.__mul = function(vec, scale)
    local new = Vector3:new(vec.x * scale, vec.y * scale, vec.z * scale)
    return new
end

Vector3.__add = function(vec1, vec2)
    return Vector3:new(vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z)
end

local Vector2 = {}

function Vector2:new(x,y)
    local v = {}
    setmetatable(v, self)
    v.x = x
    v.y = y
    return v
end

Vector2.__mul = function(vec, scale)
    local new = Vector2:new(vec.x * scale, vec.y * scale)
    return new
end

Vector2.__add = function(vec1, vec2)
    return Vector2:new(vec1.x + vec2.x, vec1.y + vec2.y)
end


Vector2.__tostring = function(vec)
    return vec.x .. vec.y
end

local COLORS = {
    translucent = Vector4:newRGB(255, 244, 255, 200);
    inactive = Vector4:newRGB(40, 40, 40, 255),
    active = Vector4:new(1.0, 1.0, 1.0, 1.0),
    transparent = Vector4:new(0.0, 0.0, 0.0, 0.0),
    main = Vector4:newRGB(120, 10, 10, 255),
    white = Vector4:new(1.0, 1.0, 1.0, 1.0),
    lightgray = Vector4:newRGB(100, 100, 100, 255),
    black = Vector4:new(0.0, 0.0, 0.0, 1.0);
    menuBackground = Vector4:new(0.01, 0.01, 0.01, 1.0);
}

local elementBase = {
    image = "",
    color = COLORS.transparent,
    aSize = Vector2:new(0, 0),
    rSize = Vector2:new(0,0),
    aPos = Vector2:new(0,0),
    rPos = Vector2:new(0,0),
    align = {},
    tags = {},
    text = {
        text = "",
        color = COLORS.transparent,
        size = 1.0;
    },
    id = "",
    zIndex = 0
}

local function MakeList(padding, ...)
    local listedElements = {}
    local last
    for i,v in ipairs({...}) do
        if i == 1 then goto continue end
        v.aPos.y = last.aPos.y - last.aSize.y - padding
        table.insert(listedElements, v);
        ::continue::
        last = v
    end
    --
    return table.unpack(listedElements)
end

local alignBase = {
    to = "middle",
    padding = 0,
}

local function AlignTo(direction, padding)
    local a = {}
    a.to = direction
    a.padding = padding
    return a
end

local mainSpace = 80;

canvas = {
    main = {
        {
            color = COLORS.menuBackground,
            rSize = Vector2:new(1, 1),

        },
        {
            image = "hellMain/Hell.png",
            color = COLORS.main,
            aSize = Vector2:new(309, 68),
            align = {
                AlignTo("center", 0),
                AlignTo("top", 76),
            },
            tags = {
                "fireEffect",
            }
        },
        {
            image = "hellMain/Runners.png",
            color = COLORS.lightgray,
            aSize = Vector2:new(453, 68),
            align = {
                AlignTo("center", 0),
                AlignTo("top", 155),
            }
        },
        {
            image = "hellMain/Trident.png",
            color = COLORS.main,
            aSize = Vector2:new(353, 65),
            align = {
                AlignTo("center", -53),
                AlignTo("top", 161),
            }
        },
    }
}

for _, v in pairs(canvas) do
    for _, nv in pairs(v) do
        nv.__index = elementBase;
        setmetatable(nv, nv)
    end
end

for _, v in pairs(blockingCanvas) do
    for _, nv in pairs(v) do
        nv.__index = elementBase;
        setmetatable(nv, nv)
    end
end