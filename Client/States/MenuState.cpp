#include "MenuState.h"
using namespace NCL;
using namespace CSC8503;

MenuState::MenuState(GameTechRenderer* pRenderer, GameWorld* pGameworld, GameClient* pClient, Canvas* pCanvas, SoundManager* pSoundManager, std::atomic<bool> &serverStartFlag) : State(), shouldServerStart(serverStartFlag)
{
    soundManager = pSoundManager;
    renderer = pRenderer;
    world = pGameworld;
    menuFont = renderer->LoadFont("Ubuntu-Bold.ttf", 55);
    baseClient = pClient;
    tweenManager = std::make_unique<TweenManager>();
    canvas = pCanvas;
    hoverShader = renderer->LoadShader("defaultUI.vert", "hoverUI.frag");
    titleShader = renderer->LoadShader("defaultUI.vert", "fireUI.frag");
    activeText = -1;
    textLimit = 15;

}

void MenuState::SendLevelSelectPacket(int level) {
    FunctionData data{};
    DataHandler handler(&data);
    handler.Pack(level);
    baseClient->RemoteFunction(Replicated::SetLevel, &data);
}

void MenuState::InitMenuSounds() {
    soundManager->SM_AddSoundToLoad("se_select00.wav");
    soundManager->SM_AddSoundToLoad("the-longest-night-of-this-winter-158699.wav");
    soundManager->SM_LoadSoundList();
    soundManager->SM_PlaySound("the-longest-night-of-this-winter-158699.wav");
}

MenuState::~MenuState() {
    delete hoverShader;
}

void MenuState::OptionHover(Element& element) {

    if (selected == element.GetIndex()) return;
    soundManager->SM_PlaySound("se_select00.wav");
    canvas->GetElementByIndex(selected).SetColor(inactiveMenuText);

    selected = element.GetIndex();

    auto pos = element.GetAbsolutePosition().y;
    auto& boxElement = canvas->GetElementByIndex(hoverBox);
    boxElement.GetAbsolutePosition().y = pos - 35 - boxElement.GetAbsoluteSize().y + element.GetAbsoluteSize().y;

    tweenManager->CreateTween(
            TweenManager::EaseOutSine,
            0.0f,
            1.0f,
            &boxElement.tweenValue1,
            0.5f);

    element.SetColor(activeMenuText);
}

void MenuState::MultiplayerOptionHover(Element& element) {

    if (mSelected == element.GetIndex()) return;
    soundManager->SM_PlaySound("se_select00.wav");
    canvas->GetElementByIndex(mSelected, "multiplayer").SetColor(inactiveMenuText);

    mSelected = element.GetIndex();

    auto pos = element.GetAbsolutePosition().y;
    auto& boxElement = canvas->GetElementByIndex(mHoverBox, "multiplayer");
    boxElement.GetAbsolutePosition().y = pos - 35 - boxElement.GetAbsoluteSize().y + element.GetAbsoluteSize().y;

    tweenManager->CreateTween(
            TweenManager::EaseOutSine,
            0.0f,
            1.0f,
            &boxElement.tweenValue1,
            0.5f);

    element.SetColor(activeMenuText);
}

void MenuState::GoBack(Element& element) {
    canvas->PopActiveLayer();
}

void MenuState::InitLua() {
    L = luaL_newstate();
    luaL_openlibs(L);

    lua_pushnumber(L, Window::GetWindow()->GetScreenSize().x);
    lua_setglobal(L, "WindowWidth");
    lua_pushnumber(L, Window::GetWindow()->GetScreenSize().y);
    lua_setglobal(L, "WindowHeight");

    auto status = luaL_dofile(L, (Assets::DATADIR + "MainMenu.lua").c_str());

    if (status) {
        std::cerr << "LUA CRIES OUT IN PAIN: " << lua_tostring(L, -1);
    }
}

void MenuState::StartSingleplayer() {
    baseClient->RemoteFunction(Replicated::StartGame, nullptr);
}

void MenuState::BeginSingleplayer(Element& _) {
    shouldServerStart.store(true);
    baseClient->SetSinglePlayer(true);
    baseClient->OnServerConnected.connect<&MenuState::StartSingleplayer>(this);
    baseClient->Connect("127.0.0.1", NetworkBase::GetDefaultPort());
}

void MenuState::ShowMultiplayerOptions(Element& _) {
    baseClient->SetSinglePlayer(false);
    canvas->PushActiveLayer("multiplayer");
}

void MenuState::JoinGame(Element& _) {
    canvas->PushActiveLayer("joinGame");
}

void MenuState::UnsetActiveTextEntry(Element& element) {
    if (element.GetIndex() == activeText) {
        activeText = -1;
    }

    std::string textId = element.GetId() + "Text";
    auto& textElement = canvas->GetElementById(textId, "multiplayer");

    if (textElement.GetTextData().text.empty()) {
        textElement.GetTextData().text = textElement.GetTextData().defaultText;
    }
}

void MenuState::SetActiveTextEntry(Element& element) {

    std::string textId = element.GetId() + "Text";
    auto& textElement = canvas->GetElementById(textId, "multiplayer");
    activeText = textElement.GetIndex();
    auto& textElementData = textElement.GetTextData();
    if (textElementData.text == textElementData.defaultText) {
        textElementData.text = "";
    }
}

void MenuState::CreateLobby(Element& element) {
    shouldServerStart.store(true);
    canvas->PushActiveLayer("lobby");
    ConnectToGame("127.0.0.1");
}

void MenuState::JoinLobby() {
    canvas->PushActiveLayer("lobby");
}

void MenuState::LeaveLobby(Element& element) {
    canvas->PopActiveLayer();
}

void MenuState::ConnectWithIp(Element& element) {
    auto& textElement = canvas->GetElementById("IpAddressText", "multiplayer");
    ConnectToGame(textElement.GetTextData().text);
}

void MenuState::AttachSignals(Element& element, const std::unordered_set<std::string>& tags, const std::string& id) {


    if (tags.find("option") != tags.end()) {
        element.OnMouseEnter.connect<&MenuState::OptionHover>(this);
    } if (tags.find("mOption") != tags.end()) {
        element.OnMouseEnter.connect<&MenuState::MultiplayerOptionHover>(this);
    } if (tags.find("textEntry") != tags.end()) {
        element.OnFocusExit.connect<&MenuState::UnsetActiveTextEntry>(this);
        element.OnFocus.connect<&MenuState::SetActiveTextEntry>(this);
    } if (tags.find("fireEffect") != tags.end()) {
        element.SetShader(titleShader);
    }

    if (id == "Singleplayer") {
        selected = element.GetIndex();
        element.OnMouseUp.connect<&MenuState::BeginSingleplayer>(this);
    } else if (id == "Multiplayer") {
        element.OnMouseUp.connect<&MenuState::ShowMultiplayerOptions>(this);
    } else if (id == "HoverBox") {
        hoverBox = element.GetIndex();
        element.SetShader(hoverShader);
    } else if (id == "mHoverBox") {
        mHoverBox = element.GetIndex();
        element.SetShader(hoverShader);
    } else if (id == "JoinGame") {
        element.OnMouseUp.connect<&MenuState::ConnectWithIp>(this);
    } else if (id == "CreateGame") {
        element.OnMouseUp.connect<&MenuState::CreateLobby>(this);
        mSelected = element.GetIndex();
    } else if (id == "Back") {
        element.OnMouseUp.connect<&MenuState::GoBack>(this);
    } else if (id == "Disconnect") {
        element.OnMouseUp.connect<&MenuState::LeaveLobby>(this);
    } else if (id == "StartLobby") {

        element.OnMouseUp.connect<&MenuState::StartGame>(this);
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
    text.defaultText = text.text;
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
    renderer->SetDeferred(false);
    InitCanvas();
    InitMenuSounds();
    RegisterPackets();
}

void MenuState::RegisterPackets() {
    baseClient->RegisterPacketHandler(Function, this);
}

bool MenuState::CheckConnected() const {
    return isGameStarted;
}

void MenuState::ConnectedToServer() {
    JoinLobby();
}

void MenuState::ConnectToGame(const std::string& address) {
    baseClient->OnServerConnected.connect<&MenuState::ConnectedToServer>(this);
    baseClient->Connect(address, NetworkBase::GetDefaultPort());
}

void MenuState::StartGame(Element& _) {
    baseClient->SetSinglePlayer(false);
    baseClient->RemoteFunction(Replicated::StartGame, nullptr);
}

void MenuState::TextEntry() {
    if (activeText == -1) return;


    auto w = Window::GetKeyboard();
    auto& textElement = canvas->GetElementByIndex(activeText, "multiplayer");


    if (w->KeyHeld(KeyboardKeys::BACK)) {
        if (!textElement.GetTextData().text.empty()) {
            textElement.GetTextData().text.pop_back();
        }
    }

    if (textElement.GetTextData().text.size() >= textLimit) return;

    for (auto keyValue = (int)KeyboardKeys::A; keyValue <= (int)KeyboardKeys::Z; keyValue++) {
        if (w->KeyPressed((KeyboardKeys)keyValue)) {
            textElement.GetTextData().text += w->KeyDown(KeyboardKeys::SHIFT) ? (char)keyValue : (char)(keyValue + 32);
        }
    }

    for (auto keyValue = (int)KeyboardKeys::NUM0; keyValue <= (int)KeyboardKeys::NUM9; keyValue++) {
        if (!w->KeyPressed((KeyboardKeys)keyValue)) continue;
        textElement.GetTextData().text += (char)keyValue;
    }

    if (w->KeyPressed(KeyboardKeys::PERIOD)) {
        textElement.GetTextData().text += ".";
    }

    if (w->KeyPressed(KeyboardKeys::SPACE)) {
        textElement.GetTextData().text += " ";
    }
}

void MenuState::Update(float dt) {

    baseClient->UpdateClient();
    TextEntry();
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
                baseClient->RemoteFunction(Replicated::MenuToGameplay, nullptr);
            }
        } break;
    }
}

void MenuState::OnExit() {
    baseClient->OnServerConnected.disconnect(this);
    baseClient->ClearPacketHandlers();
    canvas->Reset();
    soundManager->SM_UnloadSoundList();
    lua_close(L);
}
