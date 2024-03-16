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
    black = Vector4:new(0.0, 0.0, 0.0, 1.0),
    red = Vector4:new(1.0,0.0,0.0,1.0),
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
        {
            image = "hellMain/Singleplayer.png",
            color = COLORS.active,
            aSize = Vector2:new(157, 25),
            align = {
                AlignTo("center", 0),
                AlignTo("top", 285),
            },
            tags = {
                "option",
            },
            id = "Singleplayer",
        },
        {
            image = "hellMain/Multiplayer.png",
            color = COLORS.inactive,
            aSize = Vector2:new(146, 25),
            align = {
                AlignTo("center", 0),
                AlignTo("top", 285 + mainSpace),
            },
            tags = {
                "option",
            },
            id = "Multiplayer",
        },
        {
            image = "hellMain/Options.png",
            color = COLORS.inactive,
            aSize = Vector2:new(99, 25),
            align = {
                AlignTo("top", 285 + mainSpace * 2),
                AlignTo("center", 0),
            },
            tags = {
                "option",
            },
            id = "Options",
        },
        {
            image = "hellMain/Exit.png",
            color = COLORS.inactive,
            aSize = Vector2:new(48, 20),
            align = {
                AlignTo("top", 285 + mainSpace * 3),
                AlignTo("center", 0),
            },
            tags = {
                "option",
            },
            id = "Exit",
        },
        {
            color = COLORS.white,
            image = "hellMain/HoverBar.png",
            aSize = Vector2:new(170, 34),
            align = {
                AlignTo("top", 285 + 35),
                AlignTo("center", 0)
            },
            id = "HoverBox",
        },
    },
    lobby = {
        {
          rSize = Vector2:new(1,1),
          color = Vector4:new(COLORS.menuBackground.x, COLORS.menuBackground.y, COLORS.menuBackground.z, 0.8),
          aPos = Vector2:new(0,0),
        },
        {
            aSize = Vector2:new(530, 450),
            color = COLORS.menuBackground,
            align = {
                AlignTo("right", 615 - 530),
                AlignTo("top", 117),
            },
        },
        {
            color = COLORS.white,
            image = "hellMain/Players.png",
            aSize = Vector2:new(91, 25),
            align = {
              AlignTo("top", 157),
              AlignTo("right", 1280 - 886 - 91)
            },
        },
        {
            color = COLORS.white,
            image = "hellMain/Start.png",
            aSize = Vector2:new(62, 19),
            align = {
                AlignTo("top", 520),
                AlignTo("right", 1280 - 792 - 62)
            },
            tags = { "uiTextHover" },
            id = "StartLobby",
        },
        {
            color = COLORS.white,
            image = "hellMain/Disconnect.png",
            aSize = Vector2:new(138, 20),
            align = {
                AlignTo("top", 520),
                AlignTo("right", 1280 - 943 - 138)
            },
            tags = { "uiTextHover" },
            id = "DisconnectLobby"
        },
    }
}

blockingCanvas = {
    splash = {
        {
            color = COLORS.menuBackground,
            rSize =Vector2:new(1, 1),
        },
        {
            color = COLORS.menuBackground,
            aSize = Vector2:new(100,50),
            align = {
                AlignTo("middle", 0),
                AlignTo("center", 0)
            },
            tags = {"splash"},
            id= "splash"
        },
    },
    multiplayer = {
        {
            color = COLORS.menuBackground,
            rSize = Vector2:new(1, 1),

        },
        {
            image = "hellMain/Hell.png",
            color = COLORS.main,
            aSize = Vector2:new(309, 68),
            align = {
                AlignTo("left", 174),
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
                AlignTo("left", 102),
                AlignTo("top", 155),
            }
        },
        {
            image = "hellMain/Trident.png",
            color = COLORS.main,
            aSize = Vector2:new(353, 65),
            align = {
                AlignTo("left", 98),
                AlignTo("top", 161),
            }
        },
        {
          image = "hellMain/CreateGame.png",
          aSize = Vector2:new(161, 19),
          color = COLORS.inactive,
          align = {
              AlignTo("top", 371),
              AlignTo("left", 249),
          },
          tags = { "mOption" },
          id = "CreateGame",
        },
        {
            image = "hellMain/Back.png",
            aSize = Vector2:new(59, 21),
            color = COLORS.inactive,
            align = {
                AlignTo("top", 448),
                AlignTo("left", 300)
            },
            tags = { "mOption" },
            id = "Back",
        },
        {
            aSize = Vector2:new(540, 460),
            color = COLORS.white,
            align = {
                AlignTo("right", 620 - 540),
                AlignTo("top", 112),
            },
        },
        {
            aSize = Vector2:new(530, 450),
            color = COLORS.menuBackground,
            align = {
                AlignTo("right", 615 - 530),
                AlignTo("top", 117),
            },
        },
        {
            image = "hellMain/JoinGame.png",
            aSize = Vector2:new(137, 20),
            color = COLORS.inactive,
            align = {
                AlignTo("top", 407),
                AlignTo("right", 420 - 137),
            },
            id = "JoinGame",
        },
        {
            aSize = Vector2:new(240, 60),
            color = COLORS.lightgray,
            align = {
                AlignTo("top", 240),
                AlignTo("right", 467 - 240)
            },
            id = "PlayerName",
            tags = { "textEntry" }
        },
        {
            text = {
                text = "Player Name",
                color = COLORS.inactive,
                size = 0.45,
            },
            align = {
                AlignTo("top", 378 - 99), AlignTo("right", 305 + 155)
            },
            id = "PlayerNameText"
        },

        {
            aSize = Vector2:new(240, 60),
            color = COLORS.lightgray,
            align = {
                AlignTo("top", 240 + 77),
                AlignTo("right", 467 - 240)
            },
            id = "IpAddress",
            tags = { "textEntry" }
        },
        {
            text = {
                text = "IP Address",
                color = COLORS.inactive,
                size = 0.45,
            },
            align = {
                AlignTo("top", 378 - 99 + 77), AlignTo("right", 305 + 155)
            },
            id = "IpAddressText"
        },
        {
            color = COLORS.white,
            image = "hellMain/HoverBar.png",
            aSize = Vector2:new(170, 34),
            align = {
                AlignTo("top", 371 + 23),
                AlignTo("left", 249 - 5)
            },
            id = "mHoverBox",
        },
        --{
        --    color = COLORS.main,
        --    aSize = Vector2:new(415, 115),
        --    align = {
        --        AlignTo("top", 180 + 33 - 40),
        --        AlignTo("left", 115 + 415)
        --    },
        --    id = "mHoverBox",
        --},
        --{
        --    aSize = Vector2:new(315, 80),
        --    aPos = Vector2:new(160 + 415, 0),
        --    align = {
        --        AlignTo("top", 180)
        --    },
        --    text = {
        --        text = "Join Game",
        --        color = COLORS.active,
        --        size = 1.0
        --    },
        --    tags = {
        --        "mOption"
        --    },
        --    id = "JoinGame",
        --},
        --{
        --    aSize = Vector2:new(315, 80),
        --    aPos = Vector2:new(160 + 415, 0),
        --    align = {
        --        AlignTo("top", 290)
        --    },
        --    text = {
        --        text = "Create Game",
        --        color = COLORS.inactive,
        --        size = 1.0
        --    },
        --    tags = {
        --        "mOption"
        --    },
        --    id = "CreateGame",
        --},
    },
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