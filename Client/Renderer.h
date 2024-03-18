#pragma once
#include "OGLRenderer.h"
#include "OGLShader.h"
#include "OGLTexture.h"
#include "OGLMesh.h"

#include "GameWorld.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Vector2.h"
#include "Font.h"
#include "Frustum.h"
#include "GameWorld.h"

#include "Assets.h"
#include "Element.h"
#include "Canvas.h"
#include "CollisionDetection.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace NCL {
    namespace CSC8503 {
        class RenderObject;

        using namespace NCL;
        using namespace Maths;
        class GameTechRenderer : public OGLRenderer	{
        public:
            GameTechRenderer(GameWorld& world, Canvas& canvas);
            ~GameTechRenderer();

            void SetPointLights(std::vector<PointLightInfo>* lights) { pointLights = lights; }
            void SetPointLightMesh(MeshGeometry* mesh) { pointLightSphereMesh = mesh; }

            MeshGeometry*	LoadMesh(const string& name);
            TextureBase*	LoadTexture(const string& name);
            ShaderBase*		LoadShader(const string& vertex, const string& fragment);
            std::unique_ptr<Font> LoadFont(const string& fontName, int size = 48);

            void RenderText(string text, Font* font, float x, float y, float scale, Vector3 color);
            void RenderUI();
            void CreatePostProcessQuad();
            void CreateQuad();
            void RenderQuad();
            void SetDeferred(bool shouldUseDeferred) { doDeferred = shouldUseDeferred; }

            void SetSpeedLines(bool isActive) { isSpeedLinesActive = isActive; }
            void SetSpeedLineAmount(float percent) { speedLinePercent = percent; }

            OGLMesh* GetUIMesh() {return UIMesh;}

            MeshGeometry *LoadOBJMesh(const string &name);

            void SetSpeedActive(bool x){ isSpeedLinesActive = x; }

        protected:
            void NewRenderLines();
            void NewRenderText();

            void RenderFrame()	override;

            OGLShader*	defaultShader;
            OGLShader* defaultUIShader;

            GameWorld&	gameWorld;
            Canvas& canvas;

            void BuildObjectList();
            void SortObjectList();
            void RenderShadowMap();
            void RenderCamera();
            void ApplyPostProcessing();
            void RenderSkybox();
            void FillDiffuseBuffer();
            void RenderDeferredLighting();
            void CombineBuffers();
            void GenerateScreenTexture(GLuint& into, bool depth);

            void LoadSkybox();

            void SetDebugStringBufferSizes(size_t newVertCount);
            void SetDebugLineBufferSizes(size_t newVertCount);

            vector<const RenderObject*> activeObjects;


            bool doDeferred = false;

            GLuint bufferColourTex;
            GLuint bufferNormalTex;
            GLuint bufferDepthTex;
            GLuint lightDiffuseTex;
            GLuint lightSpecularTex;
            GLuint fxaaTexture;

            GLuint bufferFBO;
            GLuint lightFBO;
            GLuint fxaaFBO;

            OGLShader* combineShader;
            OGLShader* pointLightShader;
            OGLShader* fxaaShader;

            OGLShader*  debugShader;
            OGLShader*  skyboxShader;
            OGLShader*  postProcessBase;
            OGLMesh*	skyboxMesh;
            GLuint		skyboxTex;

            OGLTexture* noiseTexture;
            OGLTexture* cheeseTexture;

            //shadow mapping things
            OGLShader*	shadowShader;
            GLuint		shadowTex;
            GLuint		shadowFBO;
            Matrix4     shadowMatrix;

            std::unique_ptr<Font> debugFont;
            
            std::shared_ptr<OGLShader> textShader;

            // Ortho for UI
            Matrix4 uiOrthoView;


            PointLightInfo sunlight;
            std::vector<PointLightInfo>* pointLights;

            //Debug data storage things
            vector<Vector3> debugLineData;

            vector<Vector3> debugTextPos;
            vector<Vector4> debugTextColours;
            vector<Vector2> debugTextUVs;

            //debug
            std::vector<OGLMesh*> UIQuads;
            OGLMesh* UIMesh;
            OGLShader* uiShader;

            MeshGeometry* pointLightSphereMesh;

            GLuint lineVAO;
            GLuint lineVertVBO;
            size_t lineCount = 0;

            GLuint textVAO;
            GLuint textVertVBO;
            GLuint textColourVBO;
            GLuint textTexVBO;
            size_t textCount = 0;

            GLuint quadVBO;
            GLuint quadVAO;

            GLuint uiVAO;
            GLuint uiVBO;

            GLuint rayMarchFBO;
            GLuint rayMarchTexture;


            GLuint sceneColorTexture;
            GLuint sceneDepthTexture;

            GLuint hdrFramebuffer;

            Frustum frameFrustum;

            void InitUIQuad();


            OGLMesh*    LQuad;
            OGLShader*  LShader;
            float u_time;


            void RenderRayMap();

            void InitRayMarching();

            GLuint CreateHDRFramebuffer(GLuint colorBuffer, GLuint depthTexture);

            GLuint CreateDepthTexture();

            GLuint CreateHDRTexture();

            float uTime;
            int isSpeedLinesActive;
            float speedLinePercent = 0;
            int speedLineDir;

            void ApplyFXAA();
        };
    }
}

