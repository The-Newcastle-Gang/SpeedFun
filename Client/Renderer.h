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

#include "Assets.h"
#include "HUDElement.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace NCL {
	namespace CSC8503 {
		class RenderObject;

		class GameTechRenderer : public OGLRenderer	{
		public:
			GameTechRenderer(GameWorld& world);
			~GameTechRenderer();

			MeshGeometry*	LoadMesh(const string& name);
			TextureBase*	LoadTexture(const string& name);
			ShaderBase*		LoadShader(const string& vertex, const string& fragment);
            std::unique_ptr<Font> LoadFont(const string& fontName);

            void RenderText(string text, Font* font, float x, float y, float scale, Vector3 color);
            void GenerateUI();
            void RenderUI();

            OGLMesh* GetUIMesh() {return UIMesh;}

		protected:
			void NewRenderLines();
			void NewRenderText();

			void RenderFrame()	override;

			OGLShader*		defaultShader;

			GameWorld&	gameWorld;

			void BuildObjectList();
			void SortObjectList();
			void RenderShadowMap();
			void RenderCamera(); 
			void RenderSkybox();

			void LoadSkybox();

			void SetDebugStringBufferSizes(size_t newVertCount);
			void SetDebugLineBufferSizes(size_t newVertCount);

			vector<const RenderObject*> activeObjects;

			OGLShader*  debugShader;
			OGLShader*  skyboxShader;
			OGLMesh*	skyboxMesh;
			GLuint		skyboxTex;

			//shadow mapping things
			OGLShader*	shadowShader;
			GLuint		shadowTex;
			GLuint		shadowFBO;
			Matrix4     shadowMatrix;

            std::unique_ptr<Font> debugFont;
            std::shared_ptr<OGLShader> textShader;

            // Ortho for UI
            Matrix4 uiOrthoView;

			Vector4		lightColour;
			float		lightRadius;
			Vector3		lightPosition;

			//Debug data storage things
			vector<Vector3> debugLineData;

			vector<Vector3> debugTextPos;
			vector<Vector4> debugTextColours;
			vector<Vector2> debugTextUVs;

			GLuint lineVAO;
			GLuint lineVertVBO;
			size_t lineCount;

			GLuint textVAO;
			GLuint textVertVBO;
			GLuint textColourVBO;
			GLuint textTexVBO;
			size_t textCount;

            //debug
            std::vector<OGLMesh*> UIQuads;
            OGLMesh* UIMesh;
            OGLShader* uiShader;
        };
	}
}

