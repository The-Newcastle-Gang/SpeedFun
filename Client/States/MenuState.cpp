#include "MenuState.h"
using namespace NCL;
using namespace CSC8503;

MenuState::MenuState(GameTechRenderer* pRenderer, GameWorld* pGameworld, GameClient* pClient, Canvas* pCanvas) : State()
{
    renderer = pRenderer;
    world = pGameworld;
    menuFont = renderer->LoadFont("IndigoRegular.otf", 55);
    baseClient = pClient;
    tweenManager = std::make_unique<TweenManager>();
    canvas = pCanvas;

}

void MenuState::OptionHover(Element& element) {

    canvas->GetElementByIndex(selected).GetTextData().color = inactiveMenuText;
    selected = element.GetIndex();

    auto pos = element.GetAbsolutePosition().y;
    auto& boxElement = canvas->GetElementByIndex(hoverBox);

    tweenManager->CreateTween(
            TweenManager::EaseOutSine,
            boxElement.GetAbsolutePosition().y,
            pos - 33,
            &boxElement.GetAbsolutePosition().y,
            0.1f);

    boxElement.AlignLeft(95);
    element.GetTextData().color = activeMenuText;
}

void MenuState::InitLua() {
    L = luaL_newstate();
    luaL_openlibs(L);

    //Register c functions here

    auto status = luaL_dofile(L, (Assets::DATADIR + "MainMenu.lua").c_str());

    if (status) {
        std::cerr << "LUA CRIES OUT IN PAIN: " << lua_tostring(L, -1);
    }
}

void MenuState::AttachSignals(Element& element, const std::unordered_set<std::string>& tags, const std::string& id) {
    if (tags.find("option") != tags.end()) {
        element.OnMouseEnter.connect<&MenuState::OptionHover>(this);
    }

    if (id == "Singleplayer") {
        selected = element.GetIndex();
    }

    if (id == "HoverBox") {
        hoverBox = element.GetIndex();
    }
}

void MenuState::AlignCanvasElement(Element& element) {
    getTableField(L, "align");
    lua_pushnil(L);

    while (lua_next(L, -2)) {
        auto direction = getStringField(L, "to");
        auto padding = getIntField(L, "padding");
        if (!strcmp(direction, "top")) {
            element.AlignTop(padding);
        } else if (!strcmp(direction, "middle")) {
            element.AlignMiddle();
        } else if (!strcmp(direction, "bottom")) {
            element.AlignBottom(padding);
        } else if (!strcmp(direction, "left")) {
            element.AlignLeft(padding);
        } else if (!strcmp(direction, "center")) {
            element.AlignCenter();
        } else if (!strcmp(direction, "right")) {
            element.AlignRight(padding);
        }
        lua_pop(L, 1);
    }

    lua_pop(L, 1); // align
}

void MenuState::AddCanvasElement() {

    std::string image = getStringField(L, "image");

    auto& element = image.empty() ? canvas->AddElement() : canvas->AddImageElement(image);

    element.SetAbsoluteSize(getVec2Field(L, "aSize"))
        .SetRelativeSize(getVec2Field(L, "rSize"))
        .SetAbsolutePosition(getVec2Field(L, "aPos"))
        .SetRelativePosition(getVec2Field(L, "rPos"))
        .SetColor(getVec4Field(L, "color"));

    AlignCanvasElement(element);

    TextData text;
    getTableField(L, "text");

    text.text = getStringField(L, "text");
    text.color = getVec4Field(L, "color");
    text.SetFont(menuFont.get());
    text.fontSize = (float)getNumberField(L, "size");

    element.SetText(text);
    lua_pop(L, 1); // text

    std::unordered_set<std::string> tags;
    getTableField(L, "tags");
    lua_pushnil(L);
    while (lua_next(L, -2)) {
        tags.insert(lua_tostring(L, -1));
        lua_pop(L, 1);
    }

    lua_pop(L, 1); // tags

    std::string id = getStringField(L, "id");

    element.AddTags(tags);
    element.SetId(id);

    AttachSignals(element, tags, id);
}

void MenuState::InitCanvas() {

    InitLua();

    lua_getglobal(L, "canvas");
    lua_pushnil(L);
    while (lua_next(L, -2)) {
        AddCanvasElement();
        lua_pop(L, 1);
    }
//    canvas->AddElement()
//            .SetColor({1.0f, 244.0f/255.0f, 1.0f, 0.8f})
//            .SetRelativeSize({0.0f, 1.0f})
//            .SetAbsoluteSize({415, 0})
//            .AlignLeft(115);
//
//    canvas->AddElement()
//           .SetColor({150.0f/255.0f, 0.0f, 210.0f/255.0f, 1.0f})
//           .SetAbsoluteSize({550, 220})
//           .AlignTop()
//           .AlignLeft(50);
//
//    canvas->AddImageElement("Menu/TitleSpeed.png")
//            .SetAbsoluteSize({290, 97})
//            .SetAbsolutePosition({280, 0})
//            .AlignTop(25);
//
//    canvas->AddImageElement("Menu/TitleFun.png")
//            .SetAbsoluteSize({151, 77})
//            .SetAbsolutePosition({407, 0})
//            .AlignTop(122);
//
//    canvas->AddImageElement("Menu/Dashes.png")
//            .SetAbsoluteSize({109, 70})
//            .SetAbsolutePosition({285, 0})
//            .AlignTop(128);
//
//    hoverBox = canvas->AddElement()
//            .SetAbsoluteSize({460, 115})
//            .SetColor({160.0f/255.0f, 20.0f / 255.0f, 220.0f/255.0f, 1.0f})
//            .AlignTop(247)
//            .AlignLeft(95)
//            .GetIndex();
//
//    // References are fun, change the order of the connections and it'll blow up.
//    // the extend upper bounds method below is a bit hacky, just lets the 'hitbox' of the text extend pass the image mesh.
//    // I don't even need this any more since I'm actually rendering text and can just make the absolute size bigger
//    // Owen I bet you're reviewing this don't delete my comments please.
//    // let my words ring throughout the future of github.
//    // let this be a reminder to all future generations
//    // kids
//    // don't drink and code.
//    // bad.
//    TextData text;
//    text.SetFont(menuFont.get());
//    text.color = activeMenuText;
//    text.text = "Singleplayer";
//
//    auto& singleplayer = canvas->AddElement()
//            .SetColor({0.0, 0.0, 0.0, 0.0})
//            .SetAbsoluteSize({315, 54})
//            .SetAbsolutePosition({160, 0})
//            .AlignTop(280)
//            .SetText(text);
//
//    selected = singleplayer.GetIndex();
//
//    text.color = inactiveMenuText;
//    text.text = "Multiplayer";
//
//    singleplayer.OnMouseEnter.connect<&MenuState::OptionHover>(this);
//
//    auto& multiplayer = canvas->AddElement()
//            .SetColor({0.0, 0.0, 0.0, 0.0})
//            .SetAbsoluteSize({290, 54})
//            .SetAbsolutePosition({160, 0})
//            .AlignTop(390)
//            .ExtendUpperBounds(20, 0)
//            .SetText(text);
//
//    text.text = "Exit";
//
//    multiplayer.OnMouseEnter.connect<&MenuState::OptionHover>(this);
//    auto& exit = canvas->AddElement()
//            .SetColor({0.0, 0.0, 0.0, 0.0})
//            .SetAbsoluteSize({97, 43})
//            .SetAbsolutePosition({160, 0})
//            .AlignTop(610)
//            .ExtendUpperBounds(215, 0)
//            .SetText(text);
//
//    exit.OnMouseEnter.connect<&MenuState::OptionHover>(this);
//
//    text.text = "Options";
//
//    auto& options = canvas->AddElement()
//            .SetColor({0.0, 0.0, 0.0, 0.0})
//            .SetAbsoluteSize({201, 55})
//            .SetAbsolutePosition({160, 0})
//            .AlignTop(500)
//            .ExtendUpperBounds(115, 0)
//            .SetText(text);
//
//    options.OnMouseEnter.connect<&MenuState::OptionHover>(this);
}

void MenuState::OnEnter() {
    isGameStarted = false;
    statusText = "Press L to connect to localhost";
    connectState = 0;

    InitCanvas();

    RegisterPackets();
}

void MenuState::RegisterPackets() {
    baseClient->RegisterPacketHandler(Function, this);
}

bool MenuState::CheckConnected() const {
    return isGameStarted;
}

void MenuState::ConnectedToServer() {
    statusText = "Connected! Press R to start";
    connectState = 2;
}

void MenuState::ConnectToGame(const std::string& address) {
    baseClient->OnServerConnected.connect<&MenuState::ConnectedToServer>(this);
    baseClient->Connect(address, NetworkBase::GetDefaultPort());
    statusText = "Attempting to connect to localhost";
    connectState = 1;
}

void MenuState::StartGame() {
    baseClient->RemoteFunction(Replicated::StartGame, nullptr);
}

void MenuState::Update(float dt) {

    baseClient->UpdateClient();

    tweenManager->Update(dt);

    if (Window::GetKeyboard()->KeyDown(KeyboardKeys::L) && connectState == 0) {
        ConnectToGame("127.0.0.1");
    }

    if (Window::GetKeyboard()->KeyDown(KeyboardKeys::R) && connectState == 2) {
        StartGame();
    }

    renderer->Render();
    Debug::UpdateRenderables(dt);
}

void MenuState::ReceivePacket(int type, GamePacket *payload, int source) {
    switch(type) {
        case Function: {
            auto packet = reinterpret_cast<FunctionPacket*>(payload);
            if (packet->functionId == Replicated::RemoteClientCalls::LoadGame) {
                isGameStarted = true;
            }

        } break;
    }
}

void MenuState::OnExit() {
    baseClient->OnServerConnected.disconnect(this);
    baseClient->ClearPacketHandlers();
    canvas->Reset();
    lua_close(L);
}

MenuState::~MenuState() {

}
