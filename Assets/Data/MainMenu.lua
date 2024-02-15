Vector4 = {}

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


Vector3 = {}

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

Vector2 = {}

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
    main = Vector4:new(150, 0, 210, 255),
    white = Vector4:new(1.0, 1.0, 1.0, 1.0)
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
    id = ""
}

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

canvas = {
    main = {
        {
            color = COLORS.translucent,
            rSize = Vector2:new(0, 1),
            aSize = Vector2:new(415, 0),
            align = {
                AlignTo("left", 115)
            },
        },
        {
            color = COLORS.main,
            aSize = Vector2:new(550, 220),
            align = {
                AlignTo("top", 0),
                AlignTo("left", 50),
            }
        },
        {
            image = "Menu/TitleSpeed.png",
            color = COLORS.white,
            aSize = Vector2:new(290, 97),
            aPos = Vector2:new(280, 0),
            align = {
                AlignTo("top", 25)
            },
        },
        {
            image = "Menu/TitleFun.png",
            color = COLORS.white,
            aSize = Vector2:new(151, 77),
            aPos = Vector2:new(407, 0),
            align = {
                AlignTo("top", 128),
            }
        },
        {
            image = "Menu/Dashes.png",
            color = COLORS.white,
            aSize = Vector2:new(109, 70),
            aPos = Vector2:new(285, 0),
            align = {
                AlignTo("top", 128)
            }
        },
        {
            color = COLORS.main,
            aSize = Vector2:new(460, 115),
            align = {
                AlignTo("top", 247),
                AlignTo("left", 95)
            },
            id = "HoverBox",
        },
        {
            aSize = Vector2:new(315, 80),
            aPos = Vector2:new(160, 0),
            align = {
                AlignTo("top", 250)
            },
            text = {
                text = "Singleplayer",
                color = COLORS.active,
                size = 1.0
            },
            tags = {
                "option"
            },
            id = "Singleplayer",
        },
        {
            aSize = Vector2:new(315, 80),
            aPos = Vector2:new(160, 0),
            align = {
                AlignTo("top", 360),
            },
            text = {
                text = "Multiplayer",
                color = COLORS.inactive,
                size = 1.0,
            },
            tags = {
                "option",
            },
            id = "Multiplayer",
        },
        {
            aSize = Vector2:new(315, 80),
            aPos = Vector2:new(160, 0),
            align = {
                AlignTo("top", 470),
            },
            text = {
                text = "Options",
                color = COLORS.inactive,
                size = 1.0,
            },
            tags = {
                "option",
            },
            id = "Options",
        },
        {
            aSize = Vector2:new(315, 80),
            aPos = Vector2:new(160, 0),
            align = {
                AlignTo("top", 580),
            },
            text = {
                text = "Exit",
                color = COLORS.inactive,
                size = 1.0,
            },
            tags = {
                "option",
            },
            id = "Exit",
        }
    }
}

for _, v in pairs(canvas) do
    for _, nv in pairs(v) do
        nv.__index = elementBase;
        setmetatable(nv, nv)
    end
end