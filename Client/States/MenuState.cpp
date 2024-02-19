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
    curvyShader = renderer->LoadShader("defaultUI.vert", "curvyUi.frag");

}

MenuState::~MenuState() {
    delete curvyShader;
}

void MenuState::OptionHover(Element& element) {

    canvas->GetElementByIndex(selected).GetTextData().color = inactiveMenuText;
    selected = element.GetIndex();

    auto pos = element.GetAbsolutePosition().y;
    auto& boxElement = canvas->GetElementByIndex(hoverBox);

    tweenManager->CreateTween(
            TweenManager::EaseOutElastic,
            boxElement.GetAbsolutePosition().y,
            pos - 33,
            &boxElement.GetAbsolutePosition().y,
            0.4f);

    element.GetTextData().color = activeMenuText;
}

void MenuState::MultiplayerOptionHover(Element& element) {
    canvas->GetElementByIndex(mSelected, "multiplayer").GetTextData().color = inactiveMenuText;
    mSelected = element.GetIndex();

    auto pos = element.GetAbsolutePosition().y;
    auto& boxElement = canvas->GetElementByIndex(mHoverBox, "multiplayer");

    tweenManager->CreateTween(
            TweenManager::EaseOutElastic,
            boxElement.GetAbsolutePosition().y,
            pos - 33,
            &boxElement.GetAbsolutePosition().y,
            0.4f);

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

void MenuState::BeginSingleplayer(Element& _) {
    ConnectToGame("127.0.0.1");
}

void MenuState::ShowMultiplayerOptions(Element& _) {
    canvas->PushActiveLayer("multiplayer");
}

void MenuState::JoinGame(Element& _) {
    canvas->PushActiveLayer("joinGame");
}

void MenuState::AttachSignals(Element& element, const std::unordered_set<std::string>& tags, const std::string& id) {
    if (tags.find("option") != tags.end()) {
        element.OnMouseEnter.connect<&MenuState::OptionHover>(this);
    } if (tags.find("mOption") != tags.end()) {
        element.OnMouseEnter.connect<&MenuState::MultiplayerOptionHover>(this);
    }
    if (id == "Singleplayer") {
        selected = element.GetIndex();
        element.OnMouseUp.connect<&MenuState::BeginSingleplayer>(this);
        canvas->GetElementByIndex(hoverBox).SetAbsolutePosition({0, element.GetAbsolutePosition().y - 33});
        canvas->GetElementByIndex(hoverBox).AlignLeft(115);
    } else if (id == "Multiplayer") {
        element.OnMouseUp.connect<&MenuState::ShowMultiplayerOptions>(this);
    } else if (id == "HoverBox") {
        hoverBox = element.GetIndex();
    } else if (id == "mHoverBox") {
        mHoverBox = element.GetIndex();
    } else if (id == "JoinGame") {
        mSelected = element.GetIndex();
        element.OnMouseUp.connect<&MenuState::JoinGame>(this);
    } else if (id == "PlayerName") {
        element.SetShader(curvyShader);
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
            element.AlignMiddle(padding);
        } else if (!strcmp(direction, "bottom")) {
            element.AlignBottom(padding);
        } else if (!strcmp(direction, "left")) {
            element.AlignLeft(padding);
        } else if (!strcmp(direction, "center")) {
            element.AlignCenter(padding);
        } else if (!strcmp(direction, "right")) {
            element.AlignRight(padding);
        }
        lua_pop(L, 1);
    }

    lua_pop(L, 1); // align
}

void MenuState::AddCanvasElement(const std::string& layerName, bool blocking) {

    if (layerName != "main" && !canvas->DoesLayerExist(layerName)) {
        canvas->CreateNewLayer(layerName, blocking);
    }

    std::string image = getStringField(L, "image");

    auto& element = image.empty() ? canvas->AddElement(layerName)
            : canvas->AddImageElement(image, layerName);

    element.SetAbsoluteSize(getVec2Field(L, "aSize"))
        .SetRelativeSize(getVec2Field(L, "rSize"))
        .SetAbsolutePosition(getVec2Field(L, "aPos"))
        .SetRelativePosition(getVec2Field(L, "rPos"))
        .SetColor(getVec4Field(L, "color"))
        .SetZIndex(getIntField(L, "zIndex"));

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
        auto layerName = lua_tostring(L, -2);
        lua_pushnil(L);
        while(lua_next(L, -2)) {
            AddCanvasElement(layerName);
            lua_pop(L, 1);
        }
        lua_pop(L, 1);
    }

    lua_getglobal(L, "blockingCanvas");
    lua_pushnil(L);
    while (lua_next(L, -2)) {
        auto layerName = lua_tostring(L, -2);
        lua_pushnil(L);
        while (lua_next(L, -2)) {
            AddCanvasElement(layerName, true);
            lua_pop(L, 1);
        }
        lua_pop(L, 1);
    }
}

void MenuState::OnEnter() {
    isGameStarted = false;

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
    StartGame();
}

void MenuState::ConnectToGame(const std::string& address) {
    baseClient->OnServerConnected.connect<&MenuState::ConnectedToServer>(this);
    baseClient->Connect(address, NetworkBase::GetDefaultPort());
}

void MenuState::StartGame() {
    baseClient->RemoteFunction(Replicated::StartGame, nullptr);
}

void MenuState::Update(float dt) {

    baseClient->UpdateClient();

    tweenManager->Update(dt);

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
