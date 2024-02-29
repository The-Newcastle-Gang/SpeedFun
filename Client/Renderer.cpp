#include "Renderer.h"
#include "RenderObject.h"
#include "TextureLoader.h"

using namespace NCL;
using namespace Rendering;
using namespace CSC8503;

#define SHADOWSIZE 4096

Matrix4 biasMatrix = Matrix4::Translation(Vector3(0.5f, 0.5f, 0.5f)) * Matrix4::Scale(Vector3(0.5f, 0.5f, 0.5f));

GameTechRenderer::GameTechRenderer(GameWorld& world, Canvas& canvas) : OGLRenderer(*Window::GetWindow()), gameWorld(world), canvas(canvas)	{
    glEnable(GL_DEPTH_TEST);

    debugShader  = new OGLShader("debug.vert", "debug.frag");
    shadowShader = new OGLShader("shadow.vert", "shadow.frag");
    textShader = std::make_shared<OGLShader>("text.vert", "text.frag");
    defaultShader = new OGLShader("scene.vert", "scene.frag");
    defaultUIShader = new OGLShader("defaultUi.vert", "defaultUi.frag");

	lineCount = 0;
	textCount = 0;

	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			     SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(1, 1, 1, 1);

	textCount = 0;
	lineCount = 0;

	//Set up the light properties
	lightColour = Vector4(0.8f, 0.8f, 0.5f, 1.0f);
	lightRadius = 1000.0f;
	lightPosition = Vector3(-200.0f, 60.0f, -200.0f);

	//Skybox!
	skyboxShader = new OGLShader("skybox.vert", "skybox.frag");
	skyboxMesh = new OGLMesh();
	skyboxMesh->SetVertexPositions({Vector3(-1, 1,-1), Vector3(-1,-1,-1) , Vector3(1,-1,-1) , Vector3(1,1,-1) });
	skyboxMesh->SetVertexIndices({ 0,1,2,2,3,0 });
	skyboxMesh->UploadToGPU();

	textCount = 0;
	lineCount = 0;

	LoadSkybox();

	glGenVertexArrays(1, &lineVAO);
	glGenVertexArrays(1, &textVAO);

	glGenBuffers(1, &lineVertVBO);
	glGenBuffers(1, &textVertVBO);
	glGenBuffers(1, &textColourVBO);
	glGenBuffers(1, &textTexVBO);

	SetDebugStringBufferSizes(10000);
	SetDebugLineBufferSizes(1000);

    uiOrthoView = Matrix4::Orthographic(0.0, windowWidth, 0, windowHeight, -1.0f, 1.0f);
    debugFont = std::unique_ptr(LoadFont("CascadiaMono.ttf"));

    // move to own function.
    InitUIQuad();
    u_time = 0.0f;

}

GameTechRenderer::~GameTechRenderer()	{
    glDeleteTextures(1, &shadowTex);
    glDeleteFramebuffers(1, &shadowFBO);
    delete defaultShader;
}

void GameTechRenderer::InitUIQuad() {
    glGenVertexArrays(1, &uiVAO);
    glGenBuffers(1, &uiVBO);
    glBindVertexArray(uiVAO);
    glBindBuffer(GL_ARRAY_BUFFER, uiVBO);

    float vertices[6][4] = {
            { 0,  1,0.0f,0.0f },
            { 0,  0,0.0f,1.0f },
            { 1 , 0,1.0f,1.0f },

            { 0, 1,0.0f,0.0f },
            { 1, 0,1.0f,1.0f },
            { 1, 1,1.0f,0.0f }
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void GameTechRenderer::CreatePostProcessQuad() {

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);

    float vertices[] = {
            1.0f,  1.0f, 0.0f, 1.0, 1.0,
            1.0f, -1.0f, 0.0f, 1.0, 0.0,
            -1.0f,  1.0f, 0.0f, 0.0, 1.0,

            1.0f, -1.0f, 0.0f, 1.0, 0.0,
            -1.0f, -1.0f, 0.0f, 0.0, 0.0,
            -1.0f,  1.0f, 0.0f, 0.0, 1.0
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void GameTechRenderer::LoadSkybox() {
    string filenames[6] = {
        "/Cubemap/skyrender0004.png",
        "/Cubemap/skyrender0001.png",
        "/Cubemap/skyrender0003.png",
        "/Cubemap/skyrender0006.png",
        "/Cubemap/skyrender0002.png",
        "/Cubemap/skyrender0005.png"
    };

    int width[6]	= { 0 };
    int height[6]	= { 0 };
    int channels[6] = { 0 };
    int flags[6]	= { 0 };

    vector<char*> texData(6, nullptr);

    for (int i = 0; i < 6; ++i) {
        TextureLoader::LoadTexture(filenames[i], texData[i], width[i], height[i], channels[i], flags[i]);
        if (i > 0 && (width[i] != width[0] || height[0] != height[0])) {
            std::cout << __FUNCTION__ << " cubemap input textures don't match in size?\n";
            return;
        }
    }
    glGenTextures(1, &skyboxTex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

    GLenum type = channels[0] == 4 ? GL_RGBA : GL_RGB;

    for (int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width[i], height[i], 0, type, GL_UNSIGNED_BYTE, texData[i]);
    }

    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void GameTechRenderer::RenderFrame() {

    uiOrthoView = Matrix4::Orthographic(0.0, windowWidth, 0, windowHeight, -1.0f, 1.0f);

	glEnable(GL_CULL_FACE);
	glClearColor(1, 1, 1, 1);
	BuildObjectList();
	SortObjectList();
	RenderShadowMap();
	RenderSkybox();
	RenderCamera();
	glDisable(GL_CULL_FACE); //Todo - text indices are going the wrong way...
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	NewRenderLines();
	NewRenderText();
    RenderUI();

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    u_time += 0.01;
}

void GameTechRenderer::RenderUI() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    auto& layers = canvas.GetActiveLayers();
    for (auto i = layers.rbegin(); i != layers.rend(); i++) {
        auto& elements = (*i)->GetElements();
        for (auto& e : elements) {
            auto activeShader = defaultUIShader;
            if (!e.GetShader()) {
                BindShader(defaultUIShader);
            }
            else {
                BindShader(e.GetShader());
                activeShader = (OGLShader*)(e.GetShader());
            }

            auto color = e.GetColor();
            auto colorAddress = color.array;
            auto relPos = e.GetRelativePosition();
            auto absPos = e.GetAbsolutePosition();
            auto relSize = e.GetRelativeSize();
            auto absSize = e.GetAbsoluteSize();

            TextureBase* tex = e.GetTexture();
            if (tex) {
                auto glTex = (OGLTexture*)tex;
                glUniform1i(glGetUniformLocation(activeShader->GetProgramID(), "hasTexture"), 1);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, glTex->GetObjectID());
                glUniform1i(glGetUniformLocation(activeShader->GetProgramID(), "tex"), 0);
            } else {
                glUniform1i(glGetUniformLocation(activeShader->GetProgramID(), "hasTexture"), 0);
            }

            auto textX = (relPos.x + (float)absPos.x / (float)windowWidth) * 100;
            auto textY = (relPos.y + (float)absPos.y / (float)windowHeight) * 100;

            glUniformMatrix4fv(glGetUniformLocation(activeShader->GetProgramID(), "projection"), 1, false, (float*)uiOrthoView.array);
            glUniform4fv(glGetUniformLocation(activeShader->GetProgramID(), "uiColor"), 1, colorAddress);
            glUniform2f(glGetUniformLocation(activeShader->GetProgramID(), "positionRel"), relPos.x * windowWidth, relPos.y * windowHeight);
            glUniform2f(glGetUniformLocation(activeShader->GetProgramID(), "positionAbs"), absPos.x, absPos.y);
            glUniform2f(glGetUniformLocation(activeShader->GetProgramID(), "sizeRel"), relSize.x * windowWidth, relSize.y * windowHeight);
            glUniform2f(glGetUniformLocation(activeShader->GetProgramID(), "sizeAbs"), absSize.x, absSize.y);

            glBindVertexArray(uiVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);

            if (!e.textData.text.empty()) {
                auto fontToUse = e.textData.font;
                if (!fontToUse) fontToUse = debugFont.get();
                RenderText(e.textData.text, fontToUse, textX, textY, e.textData.fontSize, e.textData.color);
            }

        }
        if ((*i)->CheckBlocking()) {
            break;
        }
    }
}

void GameTechRenderer::BuildObjectList() {
    activeObjects.clear();

    gameWorld.OperateOnContents([&](GameObject* o) {
        if (o->IsActive()) {
            const RenderObject* g = o->GetRenderObject();
            if (g) {
                activeObjects.emplace_back(g);
            }
        }
    });
}

void GameTechRenderer::SortObjectList() {

}

void GameTechRenderer::RenderShadowMap() {
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

    glCullFace(GL_FRONT);

    BindShader(shadowShader);
    int mvpLocation = glGetUniformLocation(shadowShader->GetProgramID(), "mvpMatrix");

    Matrix4 shadowViewMatrix = Matrix4::BuildViewMatrix(lightPosition, Vector3(0, 0, 0), Vector3(0,1,0));
    Matrix4 shadowProjMatrix = Matrix4::Perspective(100.0f, 500.0f, 1, 45.0f);

    Matrix4 mvMatrix = shadowProjMatrix * shadowViewMatrix;

    shadowMatrix = biasMatrix * mvMatrix; //we'll use this one later on

    for (const auto&i : activeObjects) {
        Matrix4 modelMatrix = (*i).GetTransform()->GetMatrix();
        Matrix4 mvpMatrix	= mvMatrix * modelMatrix;
        glUniformMatrix4fv(mvpLocation, 1, false, (float*)&mvpMatrix);
        BindMesh((*i).GetMesh());
        int layerCount = (*i).GetMesh()->GetSubMeshCount();
        for (int i = 0; i < layerCount; ++i) {
            DrawBoundMesh(i);
        }
    }

    glViewport(0, 0, windowWidth, windowHeight);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glCullFace(GL_BACK);
}

void GameTechRenderer::RenderSkybox() {
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    float screenAspect = (float)windowWidth / (float)windowHeight;
    Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
    Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

    BindShader(skyboxShader);

    int projLocation = glGetUniformLocation(skyboxShader->GetProgramID(), "projMatrix");
    int viewLocation = glGetUniformLocation(skyboxShader->GetProgramID(), "viewMatrix");
    int texLocation  = glGetUniformLocation(skyboxShader->GetProgramID(), "cubeTex");

    glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);
    glUniformMatrix4fv(viewLocation, 1, false, (float*)&viewMatrix);

    glUniform1i(texLocation, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

    BindMesh(skyboxMesh);
    DrawBoundMesh();

    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void GameTechRenderer::RenderCamera() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    float screenAspect = (float)windowWidth / (float)windowHeight;
    Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
    Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

    OGLShader* activeShader = nullptr;
    int projLocation	= 0;
    int viewLocation	= 0;
    int modelLocation	= 0;
    int colourLocation  = 0;
    int hasVColLocation = 0;
    int hasTexLocation  = 0;
    int shadowLocation  = 0;

    int lightPosLocation	= 0;
    int lightColourLocation = 0;
    int lightRadiusLocation = 0;

    int cameraLocation = 0;

    float uTimeLocation = 0;

    //TODO - PUT IN FUNCTION
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, shadowTex);

    for (const RenderObject* i : activeObjects) {
        OGLShader* shader = (OGLShader*)(*i).GetShader();
        if (!shader) {
            shader = defaultShader;
        }


        BindShader(shader);

        if (i->GetDefaultTexture()) {
            BindTextureToShader((OGLTexture*)(*i).GetDefaultTexture(), "mainTex", 0);
        }

        if (activeShader != shader) {
            projLocation	= glGetUniformLocation(shader->GetProgramID(), "projMatrix");
            viewLocation	= glGetUniformLocation(shader->GetProgramID(), "viewMatrix");
            modelLocation	= glGetUniformLocation(shader->GetProgramID(), "modelMatrix");
            shadowLocation  = glGetUniformLocation(shader->GetProgramID(), "shadowMatrix");
            colourLocation  = glGetUniformLocation(shader->GetProgramID(), "objectColour");
            hasVColLocation = glGetUniformLocation(shader->GetProgramID(), "hasVertexColours");
            hasTexLocation  = glGetUniformLocation(shader->GetProgramID(), "hasTexture");

            lightPosLocation	= glGetUniformLocation(shader->GetProgramID(), "lightPos");
            lightColourLocation = glGetUniformLocation(shader->GetProgramID(), "lightColour");
            lightRadiusLocation = glGetUniformLocation(shader->GetProgramID(), "lightRadius");

            cameraLocation = glGetUniformLocation(shader->GetProgramID(), "cameraPos");
            uTimeLocation = glGetUniformLocation(shader->GetProgramID(), "u_time");

            Vector3 camPos = gameWorld.GetMainCamera()->GetPosition();
            glUniform3fv(cameraLocation, 1, camPos.array);

            glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);
            glUniformMatrix4fv(viewLocation, 1, false, (float*)&viewMatrix);

            glUniform3fv(lightPosLocation	, 1, (float*)&lightPosition);
            glUniform4fv(lightColourLocation, 1, (float*)&lightColour);
            glUniform1f(lightRadiusLocation , lightRadius);
            glUniform1f(uTimeLocation, u_time);

            int shadowTexLocation = glGetUniformLocation(shader->GetProgramID(), "shadowTex");
            glUniform1i(shadowTexLocation, 1);

            activeShader = shader;
        }

        Matrix4 modelMatrix = (*i).GetTransform()->GetMatrix();
        glUniformMatrix4fv(modelLocation, 1, false, (float*)&modelMatrix);

        Matrix4 fullShadowMat = shadowMatrix * modelMatrix;
        glUniformMatrix4fv(shadowLocation, 1, false, (float*)&fullShadowMat);

        Vector4 colour = i->GetColour();
        glUniform4fv(colourLocation, 1, colour.array);

        glUniform1i(hasVColLocation, !(*i).GetMesh()->GetColourData().empty());

        glUniform1i(hasTexLocation, (OGLTexture*)(*i).GetDefaultTexture() || (*i).GetMeshMaterial()  ? 1:0);

        BindMesh((*i).GetMesh());

        int layerCount = (*i).GetMesh()->GetSubMeshCount();

        if (MeshMaterial* m = (*i).GetMeshMaterial())BindMeshMaterial(m);
        else(BindMeshMaterial(nullptr));

        if ((*i).GetAnimatorObject()) {
            BindAnimation(i->GetAnimatorObject());
            DrawBoundAnimation(layerCount);
            BindAnimation(nullptr);
            continue;
        }

        for (int i = 0; i < layerCount; ++i) {
            DrawBoundMesh(i);
        }
    }
    glDisable(GL_BLEND);
}

MeshGeometry* GameTechRenderer::LoadMesh(const string& name) {
    OGLMesh* mesh = new OGLMesh(name);
    mesh->SetPrimitiveType(GeometryPrimitive::Triangles);
    mesh->UploadToGPU();
    return mesh;
}

void GameTechRenderer::NewRenderLines() {
    const std::vector<Debug::DebugLineEntry>& lines = Debug::GetDebugLines();
    if (lines.empty()) {
        return;
    }
    float screenAspect = (float)windowWidth / (float)windowHeight;
    Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
    Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

    Matrix4 viewProj  = projMatrix * viewMatrix;

    BindShader(debugShader);
    int matSlot = glGetUniformLocation(debugShader->GetProgramID(), "viewProjMatrix");
    GLuint texSlot = glGetUniformLocation(debugShader->GetProgramID(), "useTexture");
    glUniform1i(texSlot, 0);

    glUniformMatrix4fv(matSlot, 1, false, (float*)viewProj.array);

	  size_t frameLineCount = lines.size() * 2;
    debugLineData.clear();
  
    SetDebugLineBufferSizes(frameLineCount);

	glBindBuffer(GL_ARRAY_BUFFER, lineVertVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, lines.size() * sizeof(Debug::DebugLineEntry), lines.data());
	
	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, (GLsizei)frameLineCount);
	glBindVertexArray(0);
}

void GameTechRenderer::NewRenderText() {
    const std::vector<Debug::DebugStringEntry>& strings = Debug::GetDebugStrings();
    if (strings.empty()) {
        return;
    }

    for (const auto& s : strings) {
        float size = 0.5f;
        RenderText(s.data, debugFont.get(), s.position.x, s.position.y, size, s.colour);
    }
}



TextureBase* GameTechRenderer::LoadTexture(const string& name) {
    return TextureLoader::LoadAPITexture(name);
}

ShaderBase* GameTechRenderer::LoadShader(const string& vertex, const string& fragment) {
    return new OGLShader(vertex, fragment);
}

void GameTechRenderer::SetDebugStringBufferSizes(size_t newVertCount) {
    if (newVertCount > textCount) {
        textCount = newVertCount;

        glBindBuffer(GL_ARRAY_BUFFER, textVertVBO);
        glBufferData(GL_ARRAY_BUFFER, textCount * sizeof(Vector3), nullptr, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, textColourVBO);
        glBufferData(GL_ARRAY_BUFFER, textCount * sizeof(Vector4), nullptr, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, textTexVBO);
        glBufferData(GL_ARRAY_BUFFER, textCount * sizeof(Vector2), nullptr, GL_DYNAMIC_DRAW);

        debugTextPos.reserve(textCount);
        debugTextColours.reserve(textCount);
        debugTextUVs.reserve(textCount);

        glBindVertexArray(textVAO);

        glVertexAttribFormat(0, 3, GL_FLOAT, false, 0);
        glVertexAttribBinding(0, 0);
        glBindVertexBuffer(0, textVertVBO, 0, sizeof(Vector3));

        glVertexAttribFormat(1, 4, GL_FLOAT, false, 0);
        glVertexAttribBinding(1, 1);
        glBindVertexBuffer(1, textColourVBO, 0, sizeof(Vector4));

        glVertexAttribFormat(2, 2, GL_FLOAT, false, 0);
        glVertexAttribBinding(2, 2);
        glBindVertexBuffer(2, textTexVBO, 0, sizeof(Vector2));

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }
}

void GameTechRenderer::SetDebugLineBufferSizes(size_t newVertCount) {
    if (newVertCount > lineCount) {
        lineCount = newVertCount;

        glBindBuffer(GL_ARRAY_BUFFER, lineVertVBO);
        glBufferData(GL_ARRAY_BUFFER, lineCount * sizeof(Debug::DebugLineEntry), nullptr, GL_DYNAMIC_DRAW);

        debugLineData.reserve(lineCount);

        glBindVertexArray(lineVAO);

        int realStride = sizeof(Debug::DebugLineEntry) / 2;

        glVertexAttribFormat(0, 3, GL_FLOAT, false, offsetof(Debug::DebugLineEntry, start));
        glVertexAttribBinding(0, 0);
        glBindVertexBuffer(0, lineVertVBO, 0, realStride);

        glVertexAttribFormat(1, 4, GL_FLOAT, false, offsetof(Debug::DebugLineEntry, colourA));
        glVertexAttribBinding(1, 0);
        glBindVertexBuffer(1, lineVertVBO, sizeof(Vector4), realStride);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }
}

void GameTechRenderer::RenderText(std::string text, Font* font, float x, float y, float scale, Vector3 color) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    BindShader(font->fontShader.get());

    glUniform3f(glGetUniformLocation(textShader->GetProgramID(), "textColor"), color.x, color.y, color.z);
    glUniformMatrix4fv(glGetUniformLocation(textShader->GetProgramID(), "projection"), 1, false, (float*)uiOrthoView.array);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(font->textVAO);

    // Converting from relative to absolute size
    x = x * 0.01f * windowWidth;
    y = y * 0.01f * windowHeight;

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        Font::Character ch = font->characters[*c];

        float xpos = x + (float)ch.baselineOffset.x * scale;
        float ypos = y - (float)(ch.glyphSize.y - ch.baselineOffset.y) * scale;

        float w = (float)ch.glyphSize.x * scale;
        float h = (float)ch.glyphSize.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        glBindTexture(GL_TEXTURE_2D, ch.textureId);
        glBindBuffer(GL_ARRAY_BUFFER, font->textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (float)(ch.advanceOffset >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

std::unique_ptr<Font> GameTechRenderer::LoadFont(const std::string& fontName, int size) {
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return nullptr;
    }

    std::string filePath = std::string(Assets::FONTSSDIR) + fontName;

    FT_Face face;
    if (FT_New_Face(ft, filePath.c_str(), 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        std::cout << filePath.c_str() << std::endl;
        return nullptr;
    }

    FT_Set_Pixel_Sizes(face, 0, size);


    if (FT_Load_Char(face, 'X', FT_LOAD_RENDER)) {
        std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
        return nullptr;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    auto font = std::make_unique<Font>();

    for (unsigned char c = 0; c < 128; c++) {

        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }

        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Font::Character character = {
                texture,
                Font::Vector2i(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                Font::Vector2i(face->glyph->bitmap_left, face->glyph->bitmap_top),
                (unsigned int) face->glyph->advance.x
        };
        font->characters.insert(std::pair<char, Font::Character>(c, character));
        font->fontShader = textShader;

        glGenVertexArrays(1, &font->textVAO);
        glGenBuffers(1, &font->textVBO);
        glBindVertexArray(font->textVAO);
        glBindBuffer(GL_ARRAY_BUFFER, font->textVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    return font;
}