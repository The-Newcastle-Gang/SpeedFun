#include "Renderer.h"
#include "Renderer.h"
#include "RenderObject.h"
#include "TextureLoader.h"
#include "Resources.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader.h"

using namespace NCL;
using namespace Rendering;
using namespace CSC8503;

#define SHADOWSIZE 4096 * 2

Matrix4 biasMatrix = Matrix4::Translation(Vector3(0.5f, 0.5f, 0.5f)) * Matrix4::Scale(Vector3(0.5f, 0.5f, 0.5f));

GameTechRenderer::GameTechRenderer(GameWorld& world, Canvas& canvas) : OGLRenderer(*Window::GetWindow()), gameWorld(world), canvas(canvas)	{
    glEnable(GL_DEPTH_TEST);

    debugShader  = new OGLShader("debug.vert", "debug.frag");
    shadowShader = new OGLShader("shadow.vert", "shadow.frag");
    textShader = std::make_shared<OGLShader>("text.vert", "text.frag");
    defaultShader = new OGLShader("scene.vert", "Buffer.frag");
    defaultUIShader = new OGLShader("defaultUi.vert", "defaultUi.frag");
    combineShader = new OGLShader("screenQuad.vert", "CombineFrag.frag");
    pointLightShader = new OGLShader("PointLightVertex.vert", "PointLightFragment.frag");
    noiseTexture = (OGLTexture*)LoadTexture("noise.png");
    cheeseTexture = (OGLTexture*)LoadTexture("cheeseTex.jpg");
    postProcessBase = new OGLShader("post.vert", "post.frag");
    fxaaShader = new OGLShader("post.vert", "fxaa.frag");

	lineCount = 0;
	textCount = 0;

    GLenum buffers[2] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1};
    LoadOBJMesh("stone_tallA.obj");

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
    glGenFramebuffers(1, &lightFBO);
    glGenFramebuffers(1, &bufferFBO);
    glGenFramebuffers(1, &fxaaFBO);

    GenerateScreenTexture(bufferColourTex, false);
    GenerateScreenTexture(bufferNormalTex, false);
    GenerateScreenTexture(lightDiffuseTex, false);
    GenerateScreenTexture(lightSpecularTex, false);
    GenerateScreenTexture(bufferDepthTex, true);

    glGenTextures(1, &fxaaTexture);
    glBindTexture(GL_TEXTURE_2D, fxaaTexture);
    // we are downscaling an HDR color buffer, so we need a float texture format
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F,
                 (int)windowWidth, (int)windowHeight,
                 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
    glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, fxaaFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fxaaTexture, 0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bufferNormalTex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
    glDrawBuffers(2, buffers);

    glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightDiffuseTex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lightSpecularTex, 0);
    glDrawBuffers(2, buffers);


	glClearColor(1, 1, 1, 0);

	textCount = 0;
	lineCount = 0;

	//Set up the light properties
    sunlight.lightColour = Vector4(0.8f, 0.8f, 0.5f, 1.0f);
    sunlight.lightRadius = 1000.0f;
    sunlight.lightPosition = Vector3(-200.0f, 60.0f, -200.0f);

	//Skybox!
	skyboxShader = new OGLShader("skybox.vert", "skybox.frag");
	skyboxMesh = new OGLMesh();
	skyboxMesh->SetVertexPositions({Vector3(-1, 1,-1), Vector3(-1,-1,-1) , Vector3(1,-1,-1) , Vector3(1,1,-1) });
	skyboxMesh->SetVertexIndices({ 0,1,2,2,3,0 });
	skyboxMesh->UploadToGPU();

	textCount = 0;
	lineCount = 0;

    CreatePostProcessQuad();

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
    

    sceneColorTexture = CreateHDRTexture();
    sceneDepthTexture = CreateDepthTexture();

    hdrFramebuffer = CreateHDRFramebuffer(sceneColorTexture, sceneDepthTexture);

    // move to own function.
    InitUIQuad();
    u_time = 0.0f;

    //InitRayMarching();
    CreatePostProcessQuad();
    // SpeedLines
    uTime = 0.0f;
    isSpeedLinesActive = true;
    speedLineDir = 0;

}

GameTechRenderer::~GameTechRenderer()	{
    glDeleteTextures(1, &shadowTex);
    glDeleteTextures(1, &bufferColourTex);
    glDeleteTextures(1, &bufferNormalTex);
    glDeleteTextures(1, &lightDiffuseTex);
    glDeleteTextures(1, &lightSpecularTex);
    glDeleteFramebuffers(1, &shadowFBO);
    glDeleteFramebuffers(1, &lightFBO);
    glDeleteFramebuffers(1, &bufferFBO);
    delete shadowShader;
    delete defaultShader;
    delete combineShader;
    delete pointLightShader;
    delete skyboxShader;
}

GLuint GameTechRenderer::CreateHDRTexture() {
    GLuint t;
    glGenTextures(1, &t);
    glBindTexture(GL_TEXTURE_2D, t);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return t;
}

GLuint GameTechRenderer::CreateHDRFramebuffer(GLuint colorBuffer, GLuint depthTexture) {
    GLuint f;
    glGenFramebuffers(1, &f);
    glBindFramebuffer(GL_FRAMEBUFFER, f);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

    if (depthTexture != 0) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer is jover" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return f;
}

GLuint GameTechRenderer::CreateDepthTexture() {
    GLuint t;
    glGenTextures(1, &t);
    glBindTexture(GL_TEXTURE_2D, t);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    return t;
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

void GameTechRenderer::LoadSkybox() {
    // THE TEMP SKYBOX IS FROM HERE: https://assetstore.unity.com/packages/2d/textures-materials/sky/8k-skybox-pack-free-150926
    string filenames[6] = {
        "/Cubemap/nx.png",
        "/Cubemap/px.png",
        "/Cubemap/py.png",
        "/Cubemap/ny.png",
        "/Cubemap/nz.png",
        "/Cubemap/pz.png"
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
    uTime += 0.01f;
    uiOrthoView = Matrix4::Orthographic(0.0, windowWidth, 0, windowHeight, -1.0f, 1.0f);
    glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glClearColor(1, 1, 1, 1);

	BuildObjectList();
	SortObjectList();
	//RenderShadowMap();
    FillDiffuseBuffer();
    RenderDeferredLighting();
    CombineBuffers();
    ApplyPostProcessing();
    ApplyFXAA();
	glDisable(GL_CULL_FACE); //Todo - text indices are going the wrong way...
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //RenderRayMap();
	NewRenderLines();
	NewRenderText();
    RenderUI();
}

void GameTechRenderer::FillDiffuseBuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(true);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    RenderSkybox();
    RenderCamera();
}

void GameTechRenderer::RenderDeferredLighting() {

    float screenAspect = (float)windowWidth / (float)windowHeight;
    Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
    Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

    glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
    BindShader(pointLightShader);
    GLuint shaderProg = pointLightShader->GetProgramID();

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    if (!doDeferred)return;
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_ONE, GL_ONE);
    glCullFace(GL_FRONT);
    glDepthFunc(GL_ALWAYS);

    glUniform1i(glGetUniformLocation(shaderProg, "depthTex"), 6);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

    glUniform1i(glGetUniformLocation(shaderProg, "normTex"), 7);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

    const Vector3 &cameraPosition = gameWorld.GetMainCamera()->GetPosition();
    float* camPos = (float*)&cameraPosition.array[0];

    glUniform3fv(glGetUniformLocation(shaderProg, "cameraPos"), 1, camPos);
    glUniform2f(glGetUniformLocation(shaderProg, "pixelSize"), 1.0f / windowWidth, 1.0f / windowHeight);

    Matrix4 invViewProj = (projMatrix * viewMatrix).Inverse();
    glUniformMatrix4fv(glGetUniformLocation(shaderProg, "inverseProjView"), 1, false, (float*)invViewProj.array);
    glUniformMatrix4fv(glGetUniformLocation(shaderProg, "viewMatrix"), 1, false, (float*)viewMatrix.array);
    glUniformMatrix4fv(glGetUniformLocation(shaderProg, "projMatrix"), 1, false, (float*)projMatrix.array);

    BindMesh(pointLightSphereMesh);
    for (int i = 0; i < (*pointLights).size(); i++) {
        PointLightInfo& l = (*pointLights)[i];
        glUniform3fv(glGetUniformLocation(shaderProg, "lightPos"), 1, (float*)&l.lightPosition);
        glUniform4fv(glGetUniformLocation(shaderProg, "lightColour"), 1, (float*)&l.lightColour);
        glUniform1f(glGetUniformLocation(shaderProg, "lightRadius"), l.lightRadius);
        DrawBoundMesh();
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glCullFace(GL_BACK);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);

    glDepthMask(GL_TRUE);
    glClearColor(0, 0, 0, 1);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GameTechRenderer::CombineBuffers() {
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFramebuffer);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glDisable(GL_CULL_FACE);

    BindShader(combineShader);
    Matrix4 modelTemp;
    Matrix4 viewTemp;
    Matrix4 projTemp;
    modelTemp.ToIdentity();
    viewTemp.ToIdentity();
    projTemp.ToIdentity();



    glDepthMask(false);
    GLuint shaderID = combineShader->GetProgramID();
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "modelMatrix"), 1, false, (float*)modelTemp.array);
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "viewMatrix"), 1, false, (float*)viewTemp.array);
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "projMatrix"), 1, false, (float*)projTemp.array);

    glUniform1i(glGetUniformLocation(shaderID, "diffuseTex"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bufferColourTex);
    glUniform1i(glGetUniformLocation(shaderID, "diffuseNorm"), 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

    glUniform1i(glGetUniformLocation(shaderID, "diffuseLight"), 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, lightDiffuseTex);

    glUniform1i(glGetUniformLocation(shaderID, "specularLight"), 3);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, lightSpecularTex);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP,0, 6);
    glEnable(GL_CULL_FACE);

    glDepthMask(true);
}

void GameTechRenderer::InitRayMarching() {

    glGenTextures(1, &rayMarchTexture);
    glBindTexture(GL_TEXTURE_2D, rayMarchTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    glGenFramebuffers(1, &rayMarchFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, rayMarchFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rayMarchTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer is jover" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GameTechRenderer::RenderRayMap() {

}

void GameTechRenderer::RenderUI() {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto& layers = canvas.GetActiveLayers();

    int blockingLayer = layers.size() - 1;

    for (int i=blockingLayer; i >= 0; i--) {
        if (layers[blockingLayer]->CheckBlocking()) {
            break;
        }

        blockingLayer = i;
    }

    for (auto i = layers.begin() + blockingLayer; i != layers.end(); i++) {
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
            auto transformation = e.GetTransform();
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
            glUniformMatrix4fv(glGetUniformLocation(activeShader->GetProgramID(), "model"), 1, false, (float*)transformation.GetMatrix().array);
            glUniform4fv(glGetUniformLocation(activeShader->GetProgramID(), "uiColor"), 1, colorAddress);
            glUniform2f(glGetUniformLocation(activeShader->GetProgramID(), "positionRel"), relPos.x * windowWidth, relPos.y * windowHeight);
            glUniform2f(glGetUniformLocation(activeShader->GetProgramID(), "positionAbs"), absPos.x, absPos.y);
            glUniform2f(glGetUniformLocation(activeShader->GetProgramID(), "sizeRel"), relSize.x * windowWidth, relSize.y * windowHeight);
            glUniform2f(glGetUniformLocation(activeShader->GetProgramID(), "sizeAbs"), absSize.x, absSize.y);
            glUniform1f(glGetUniformLocation(activeShader->GetProgramID(), "tweenValue1"), e.tweenValue1);
            glUniform1i(glGetUniformLocation(activeShader->GetProgramID(), "noiseTexture"), 1);
            glUniform1i(glGetUniformLocation(activeShader->GetProgramID(), "cheeseTexture"), 2);
            glUniform1f(glGetUniformLocation(activeShader->GetProgramID(), "uTime"), (float)Window::GetTimer()->GetTotalTimeSeconds());

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, noiseTexture->GetObjectID());


            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, cheeseTexture->GetObjectID());

            glBindVertexArray(uiVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);

            if (!e.textData.text.empty()) {
                auto fontToUse = e.textData.font;
                if (!fontToUse) fontToUse = debugFont.get();
                RenderText(e.textData.text, fontToUse, textX, textY, e.textData.fontSize, e.textData.color);
            }

        }
    }
}

void GameTechRenderer::ClearActiveObjects() {
    activeObjects.clear();
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

    Matrix4 shadowViewMatrix = Matrix4::BuildViewMatrix(sunlight.lightPosition, Vector3(0, 0, 0), Vector3(0,1,0));
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
    glDepthMask(GL_FALSE);

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
    glDepthMask(GL_TRUE);
}

void GameTechRenderer::RenderCamera() {
    glEnable(GL_DEPTH_TEST);
    float screenAspect = (float)windowWidth / (float)windowHeight;
    Matrix4 viewMatrix = gameWorld.GetMainCamera()->BuildViewMatrix();
    Matrix4 projMatrix = gameWorld.GetMainCamera()->BuildProjectionMatrix(screenAspect);

    frameFrustum = Frustum::FromViewProjMatrix(projMatrix * viewMatrix);


    OGLShader* activeShader = nullptr;
    int projLocation	= 0;
    int viewLocation	= 0;
    int modelLocation	= 0;
    int colourLocation  = 0;
    int hasVColLocation = 0;
    int hasTexLocation  = 0;
    int shadowLocation  = 0;
    int isLitLocation   = 0;

    int lightPosLocation	= 0;
    int lightColourLocation = 0;
    int lightRadiusLocation = 0;

    int cameraLocation = 0;

    int lavaLocation = 0;
    float uTimeLocation = 0;

    //glBindFramebuffer(GL_FRAMEBUFFER, hdrFramebuffer);
    //glClearColor(0.0, 0.0, 0.0, 0.0);
    //glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


    //TODO - PUT IN FUNCTION
    glActiveTexture(GL_TEXTURE0+1);
    glBindTexture(GL_TEXTURE_2D, shadowTex);

    for (const RenderObject* i : activeObjects) {

        Vector3 scale = (*i).GetMeshScale();
        float maxTransform = std::max(std::max(scale.x, scale.y), scale.z);
        //if (!frameFrustum.SphereInsideFrustum(i->GetTransform()->GetPosition(), maxTransform * 0.5)) continue;

        OGLShader *shader = (OGLShader *) (*i).GetShader();
        if (!shader) {
            shader = defaultShader;
        }

        BindShader(shader);

        if (i->GetDefaultTexture()) {
            BindTextureToShader((OGLTexture *) (*i).GetDefaultTexture(), "mainTex", 0);
        }

        if (activeShader != shader) {
            projLocation    = glGetUniformLocation(shader->GetProgramID(), "projMatrix");
            viewLocation    = glGetUniformLocation(shader->GetProgramID(), "viewMatrix");
            modelLocation   = glGetUniformLocation(shader->GetProgramID(), "modelMatrix");
            shadowLocation  = glGetUniformLocation(shader->GetProgramID(), "shadowMatrix");
            colourLocation  = glGetUniformLocation(shader->GetProgramID(), "objectColour");
            hasVColLocation = glGetUniformLocation(shader->GetProgramID(), "hasVertexColours");

            hasTexLocation  = glGetUniformLocation(shader->GetProgramID(), "hasTexture");
            isLitLocation   = glGetUniformLocation(shader->GetProgramID(), "isLit");

            lightPosLocation    = glGetUniformLocation(shader->GetProgramID(), "lightPos");
            lightColourLocation = glGetUniformLocation(shader->GetProgramID(), "lightColour");
            lightRadiusLocation = glGetUniformLocation(shader->GetProgramID(), "lightRadius");

            cameraLocation = glGetUniformLocation(shader->GetProgramID(), "cameraPos");
            uTimeLocation = glGetUniformLocation(shader->GetProgramID(), "u_time");
            lavaLocation = glGetUniformLocation(shader->GetProgramID(), "lavaHeight");

            Vector3 camPos = gameWorld.GetMainCamera()->GetPosition();
            glUniform3fv(cameraLocation, 1, camPos.array);

            glUniformMatrix4fv(projLocation, 1, false, (float *) &projMatrix);
            glUniformMatrix4fv(viewLocation, 1, false, (float *) &viewMatrix);

            glUniform1f(uTimeLocation, uTime);
            glUniform3fv(lightPosLocation	, 1, (float*)&sunlight.lightPosition);
            glUniform4fv(lightColourLocation, 1, (float*)&sunlight.lightColour);
            glUniform1f(lightRadiusLocation, sunlight.lightRadius);
            glUniform1f(lavaLocation, lavaHeight);

            int shadowTexLocation = glGetUniformLocation(shader->GetProgramID(), "shadowTex");
            glUniform1i(shadowTexLocation, 1);

            activeShader = shader;
        }
        Vector3 originalScale = (*i).GetTransform()->GetScale();
        Vector3 originalPosition = (*i).GetTransform()->GetPosition();
        (*i).GetTransform()->SetScale(scale); //multiply scale before we get the matrix
        (*i).GetTransform()->SetPosition(originalPosition + (*i).GetMeshOffset()); //add offset before we get the matrix
        Matrix4 modelMatrix = (*i).GetTransform()->GetMatrix();
        (*i).GetTransform()->SetScale(originalScale); //set it back to normal
        (*i).GetTransform()->SetPosition(originalPosition); //set it back to normal

        glUniformMatrix4fv(modelLocation, 1, false, (float *) &modelMatrix);

        Matrix4 fullShadowMat = shadowMatrix * modelMatrix;
        glUniformMatrix4fv(shadowLocation, 1, false, (float *) &fullShadowMat);

        Vector4 colour = i->GetColour();
        glUniform4fv(colourLocation, 1, colour.array);

        glUniform1i(hasVColLocation, !(*i).GetMesh()->GetColourData().empty());

        glUniform1i(hasTexLocation, (OGLTexture *) (*i).GetDefaultTexture() || (*i).GetMeshMaterial() ? 1 : 0);

        glUniform1i(isLitLocation, 1);

        BindMesh((*i).GetMesh());

        int layerCount = (*i).GetMesh()->GetSubMeshCount();

        if (MeshMaterial *m = (*i).GetMeshMaterial())BindMeshMaterial(m);
        else(BindMeshMaterial(nullptr));

        if ((*i).GetAnimatorObject()) {
            BindAnimation(i->GetAnimatorObject());
            DrawBoundAnimation(layerCount);
            BindAnimation(nullptr);
            continue;
        }

        if (layerCount == 0) {
            DrawBoundMesh();
        } else {
            for (int i = 0; i < layerCount; ++i) {
                DrawBoundMesh(i);
            }
        }
    }
    //test if ogl error
    glDisable(GL_BLEND);
}

void NCL::CSC8503::GameTechRenderer::ApplyPostProcessing()
{
    float screenAspect = (float)windowWidth / (float)windowHeight;
    glBindFramebuffer(GL_FRAMEBUFFER, fxaaFBO);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_CULL_FACE); //Todo - text indices are going the wrong way...
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    BindShader(postProcessBase);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sceneColorTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
    glBindVertexArray(quadVAO);

    auto& camera = *gameWorld.GetMainCamera();

    Matrix4 invVP = CollisionDetection::GenerateInverseView(camera) * CollisionDetection::GenerateInverseProjection(screenAspect, camera.GetFieldOfVision(), camera.GetNearPlane(), camera.GetFarPlane());
    int invLocation         = glGetUniformLocation(postProcessBase->GetProgramID(), "invViewPersp");
    int lightLoc            = glGetUniformLocation(postProcessBase->GetProgramID(), "lightPos");
    int depthLoc            = glGetUniformLocation(postProcessBase->GetProgramID(), "depthBuffer");
    int timeLoc             = glGetUniformLocation(postProcessBase->GetProgramID(), "u_time");
    int speedBoolLoc        = glGetUniformLocation(postProcessBase->GetProgramID(), "SpeedLinesActive");
    int speedLineDirLoc     = glGetUniformLocation(postProcessBase->GetProgramID(), "speedLineDir");
    int speedLineAmountLoc  = glGetUniformLocation(postProcessBase->GetProgramID(), "speedLineAmount");

    glUniformMatrix4fv(invLocation, 1, false, (float*)&invVP);
    glUniform3fv(lightLoc, 1, (float*)&sunlight.lightPosition);
    glUniform1i(depthLoc, 1);
    glUniform1f(timeLoc, uTime);
    glUniform1f(speedLineAmountLoc, speedLinePercent);
    glUniform1i(speedBoolLoc, isSpeedLinesActive);
    glUniform1i(speedLineDirLoc, speedLineDir);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
}

void GameTechRenderer::ApplyFXAA() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    BindShader(fxaaShader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fxaaTexture);
    glBindVertexArray(quadVAO);

    int widthLocation = glGetUniformLocation(fxaaShader->GetProgramID(), "width");
    int heightLocation = glGetUniformLocation(fxaaShader->GetProgramID(), "height");

    glUniform1f(widthLocation, windowWidth);
    glUniform1f(heightLocation, windowHeight);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glDisable(GL_FRAMEBUFFER_SRGB);
}

MeshGeometry* GameTechRenderer::LoadMesh(const string& name) {
    OGLMesh* mesh = new OGLMesh(name);
    mesh->SetPrimitiveType(GeometryPrimitive::Triangles);
    mesh->UploadToGPU();
    return mesh;
}

MeshGeometry* GameTechRenderer::LoadOBJMesh(const string& name) {
    OGLMesh* mesh = new OGLMesh();
    mesh->SetPrimitiveType(GeometryPrimitive::Triangles);

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, (Assets::MESHDIR + name).c_str(), (Assets::MESHDIR).c_str());

    if (!err.empty()) {
        std::cerr << err << std::endl;
    }

    if (!ret) {
        std::cerr << "Definitely didn't work" << std::endl;
    }

    const std::vector<tinyobj::index_t>& indices = shapes[0].mesh.indices;
    const std::vector<int>& material_ids = shapes[0].mesh.material_ids;

    std::vector<Vector3> vertexPositions;
    std::vector<Vector2> vertexTexcoords;
    std::vector<Vector4> vertexColors;
    std::vector<GLuint> vertexIndices;

    for (size_t index = 0; index < material_ids.size(); index++) {
        Vector3 original[] = {
                Vector3(attrib.vertices[indices[3 * index].vertex_index * 3],
                        attrib.vertices[indices[3 * index].vertex_index * 3 + 1],
                        attrib.vertices[indices[3 * index].vertex_index * 3 + 2]),
                Vector3(attrib.vertices[indices[3 * index + 1].vertex_index * 3],
                        attrib.vertices[indices[3 * index + 1].vertex_index * 3 + 1],
                        attrib.vertices[indices[3 * index + 1].vertex_index * 3 + 2]),
                Vector3(attrib.vertices[indices[3 * index + 2].vertex_index * 3],
                        attrib.vertices[indices[3 * index + 2].vertex_index * 3 + 1],
                        attrib.vertices[indices[3 * index + 2].vertex_index * 3 + 2])
        };

        vertexPositions.push_back(original[0]);
        vertexTexcoords.push_back(Vector2(
                attrib.texcoords[indices[3 * index].texcoord_index * 2],
                attrib.texcoords[indices[3 * index].texcoord_index * 2 + 1]
        ));

        vertexColors.push_back(Vector4(1.0, 1.0, 1.0, 1.0));
        vertexIndices.push_back(vertexPositions.size() - 1);

        vertexPositions.push_back(original[1]);
        vertexTexcoords.push_back(Vector2(
                attrib.texcoords[indices[3 * index + 1].texcoord_index * 2],
                attrib.texcoords[indices[3 * index + 1].texcoord_index * 2 + 1]
        ));

        vertexColors.push_back(Vector4(1.0, 1.0, 1.0, 1.0));
        vertexIndices.push_back(vertexPositions.size() - 1);

        vertexPositions.push_back(original[2]);
        vertexTexcoords.push_back(Vector2(
                attrib.texcoords[indices[3 * index + 2].texcoord_index * 2],
                attrib.texcoords[indices[3 * index + 2].texcoord_index * 2 + 1]
        ));

        vertexColors.push_back(Vector4(1.0, 1.0, 1.0, 1.0));
        vertexIndices.push_back(vertexPositions.size() - 1);
    }

    mesh->SetVertexPositions(vertexPositions);
    mesh->SetVertexTextureCoords(vertexTexcoords);
    mesh->SetVertexColours(vertexColors);
    mesh->SetVertexIndices(vertexIndices);

    mesh->RecalculateNormals();
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

void GameTechRenderer::GenerateScreenTexture(GLuint& into, bool depth) {
    glGenTextures(1, &into);
    glBindTexture(GL_TEXTURE_2D, into);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    GLuint format = depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8;
    GLuint type = depth ? GL_DEPTH_COMPONENT : GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, format, windowWidth, windowHeight, 0, type, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
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
