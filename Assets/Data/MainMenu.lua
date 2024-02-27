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
    white = Vector4:new(1.0, 1.0, 1.0, 1.0),
    lightgray = Vector4:newRGB(200, 200, 200, 255),
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

function elementBase:MakeList(previous, padding)
    self.aPos = Vector2:new(self.aPos.x, previous.aPos.y + previous.aSize.y + padding)
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
            aSize = Vector2:new(0, 150),
            rSize = Vector2:new(1.0, 0.0),
            align = {
                AlignTo("top", 0),
                AlignTo("left", 0),
            }
        },
        {
            image = "Menu/TitleSpeed.png",
            color = COLORS.white,
            aSize = Vector2:new(290, 97),
            aPos = Vector2:new(280, 0),
            align = {
                AlignTo("top", 35),
                AlignTo("right", 181)
            },
        },
        {
            image = "Menu/TitleFun.png",
            color = COLORS.white,
            aSize = Vector2:new(151, 77),
            aPos = Vector2:new(407, 0),
            align = {
                AlignTo("top", 35),
                AlignTo("right", 20)
            }
        },
        {
            image = "Menu/Dashes.png",
            color = COLORS.white,
            aSize = Vector2:new(109, 70),
            aPos = Vector2:new(285, 0),
            align = {
                AlignTo("top", 45),
                AlignTo("right", 181 + 290 + 10)
            }
        },
        {
            color = COLORS.main,
            aSize = Vector2:new(415, 115),
            align = {
                AlignTo("top", 180 + 33 - 40),
                AlignTo("left", 115)
            },
            id = "HoverBox",
        },
        {
            aSize = Vector2:new(315, 80),
            aPos = Vector2:new(160, 0),
            align = {
                AlignTo("top", 180)
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
                AlignTo("top", 290),
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
                AlignTo("top", 400),
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
                AlignTo("top", 510),
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
    },
    multiplayer = {
        {
            color = COLORS.translucent,
            rSize = Vector2:new(0, 1),
            aSize = Vector2:new(415, 0),
            align = {
                AlignTo("left", 115 + 415)
            },
        },
        {
            color = COLORS.main,
            aSize = Vector2:new(415, 115),
            align = {
                AlignTo("top", 180 + 33 - 40),
                AlignTo("left", 115 + 415)
            },
            id = "mHoverBox",
        },
        {
            aSize = Vector2:new(315, 80),
            aPos = Vector2:new(160 + 415, 0),
            align = {
                AlignTo("top", 180)
            },
            text = {
                text = "Join Game",
                color = COLORS.active,
                size = 1.0
            },
            tags = {
                "mOption"
            },
            id = "JoinGame",
        },
        {
            aSize = Vector2:new(315, 80),
            aPos = Vector2:new(160 + 415, 0),
            align = {
                AlignTo("top", 290)
            },
            text = {
                text = "Create Game",
                color = COLORS.inactive,
                size = 1.0
            },
            tags = {
                "mOption"
            },
            id = "CreateGame",
        },
    }
}

blockingCanvas = {
    joinGame = {
        {
            aSize = Vector2:new(350, 325),
            color = Vector4:newRGB(236, 236, 236, 255),
            align = {
                AlignTo("middle", 0), AlignTo("center", 0)
            }

        },
        {
            text = {
                text = "Join Game",
                color = COLORS.inactive,
                size = 0.56,
            },
            align = {
                AlignTo("middle", 100), AlignTo("center", -80)
            }
        },
        {
            aSize = Vector2:new(250, 70),
            color = COLORS.main,
            align = {
                AlignTo("middle", -100), AlignTo("center", 0)
            },
            id = "Connect",
        },
        {
            text = {
                text = "Connect",
                color = COLORS.white,
                size = 0.56,
            },
            align = {
                AlignTo("middle", -110), AlignTo("center", -60)
            },
        },
        {
            aSize = Vector2:new(255, 45),
            color = COLORS.white,
            align = {
                AlignTo("middle", 60), AlignTo("center", 0)
            },
            id = "PlayerName",
            tags = { "textEntry" }
        },
        {
            aSize = Vector2:new(255, 45),
            color = COLORS.white,
            align = {
                AlignTo("middle", 0), AlignTo("center", 0)
            },
            id = "IpAddress",
            tags = { "textEntry" }
        },
        {
            text = {
                text = "Player Name",
                color = COLORS.lightgray,
                size = 0.45,
            },
            align = {
                AlignTo("middle", 50), AlignTo("center", -110)
            },
            id = "PlayerNameText"
        },
        {
            text = {
                text = "IP Address",
                color = COLORS.lightgray,
                size = 0.45,
            },
            align = {
                AlignTo("middle", -10), AlignTo("center", -110)
            },
            id = "IpAddressText",
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