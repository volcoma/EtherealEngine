#include "GUI.h"
#include "embedded/roboto_regular.ttf.h"
#include "embedded/robotomono_regular.ttf.h"
#include "embedded/vs_ocornut_imgui.bin.h"
#include "embedded/fs_ocornut_imgui.bin.h"

#include "Runtime/Rendering/VertexBuffer.h"
#include "Runtime/Rendering/IndexBuffer.h"
#include "Runtime/Rendering/Program.h"
#include "Runtime/Rendering/Texture.h"
#include "Runtime/Rendering/FrameBuffer.h"
#include "Runtime/Rendering/Uniform.h"
#include "Runtime/Rendering/RenderWindow.h"
#include "Runtime/Assets/AssetManager.h"
#include "Runtime/System/Application.h"
#include <unordered_map>

// -------------------------------------------------------------------

class UIContexts
{
private:
	ImGuiContext* mInitialContext = nullptr;
public:
	void setInitialContext(ImGuiContext* context)
	{
		mInitialContext = context;
	}

	void restoreInitialContext()
	{
		if(mInitialContext)
			gui::SetCurrentContext(mInitialContext);
	}
};


static GUIStyle sGUIStyle;
static UIContexts sContexts;

static gfx::VertexDecl				sDecl;
static std::unique_ptr<Program>		sProgram;
static AssetHandle<Texture>			sFontTexture;
static std::vector<std::shared_ptr<ITexture>> sTextures;

void renderFunc(ImDrawData *_drawData)
{
	// Render command lists
	for (int32_t ii = 0, num = _drawData->CmdListsCount; ii < num; ++ii)
	{
		gfx::TransientVertexBuffer tvb;
		gfx::TransientIndexBuffer tib;

		const ImDrawList* drawList = _drawData->CmdLists[ii];
		std::uint32_t numVertices = (std::uint32_t)drawList->VtxBuffer.size();
		std::uint32_t numIndices = (std::uint32_t)drawList->IdxBuffer.size();

		if (!gfx::checkAvailTransientVertexBuffer(numVertices, sDecl)
			|| !gfx::checkAvailTransientIndexBuffer(numIndices))
		{
			// not enough space in transient buffer just quit drawing the rest...
			break;
		}

		gfx::allocTransientVertexBuffer(&tvb, numVertices, sDecl);
		gfx::allocTransientIndexBuffer(&tib, numIndices);

		ImDrawVert* verts = (ImDrawVert*)tvb.data;
		std::memcpy(verts, drawList->VtxBuffer.begin(), numVertices * sizeof(ImDrawVert));

		ImDrawIdx* indices = (ImDrawIdx*)tib.data;
		std::memcpy(indices, drawList->IdxBuffer.begin(), numIndices * sizeof(ImDrawIdx));

		std::uint32_t offset = 0;
		for (const ImDrawCmd* cmd = drawList->CmdBuffer.begin(), *cmdEnd = drawList->CmdBuffer.end(); cmd != cmdEnd; ++cmd)
		{
			if (cmd->UserCallback)
			{
				cmd->UserCallback(drawList, cmd);
			}
			else if (0 != cmd->ElemCount)
			{
				std::uint64_t state = 0
					| BGFX_STATE_RGB_WRITE
					| BGFX_STATE_ALPHA_WRITE
					| BGFX_STATE_MSAA
					| BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
					;

				Texture* texture = sFontTexture.get();
				FrameBuffer* fbo = nullptr;
				Program* program = sProgram.get();
				if (!program)
					return;

				if (nullptr != cmd->TextureId)
				{
					ITexture* bind = (ITexture*)cmd->TextureId;
					if (rttr::type::get(*bind) == rttr::type::get<Texture>())
						texture = (Texture*)bind;
					else if (rttr::type::get(*bind) == rttr::type::get<FrameBuffer>())
						fbo = (FrameBuffer*)bind;
				}

				const std::uint16_t xx = std::uint16_t(std::max(cmd->ClipRect.x, 0.0f));
				const std::uint16_t yy = std::uint16_t(std::max(cmd->ClipRect.y, 0.0f));
				gfx::setScissor(xx, yy
					, std::uint16_t(std::min(cmd->ClipRect.z, 65535.0f) - xx)
					, std::uint16_t(std::min(cmd->ClipRect.w, 65535.0f) - yy)
				);

				
				if(fbo)
					program->setTexture(0, "s_tex", fbo);
				else
					program->setTexture(0, "s_tex", texture);


				gfx::setVertexBuffer(&tvb, 0, numVertices);
				gfx::setIndexBuffer(&tib, offset, cmd->ElemCount);
				gfx::setState(state);
				gfx::submit(cmd->ViewId, program->handle);
			}

			offset += cmd->ElemCount;
		}
	}
}

namespace gui
{


//-----------------------------------------------------------------------------
//  Name : initialize ()
/// <summary>
/// Initialize the interface ready for processing.
/// </summary>
//-----------------------------------------------------------------------------
bool init()
{
	sContexts.setInitialContext(ImGui::GetCurrentContext());
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr;
	io.RenderDrawListsFn = renderFunc;

	auto& app = Singleton<Application>::getInstance();
	auto& manager = app.getAssetManager();

 	switch (gfx::getRendererType())
	{
	case gfx::RendererType::Direct3D9:
		manager.createAssetFromMemory<Shader>("vs_ocornut_imgui_embedded", &vs_ocornut_imgui_dx9[0], sizeof(vs_ocornut_imgui_dx9));
		manager.createAssetFromMemory<Shader>("fs_ocornut_imgui_embedded", &fs_ocornut_imgui_dx9[0], sizeof(fs_ocornut_imgui_dx9));
		break;

	case gfx::RendererType::Direct3D11:
	case gfx::RendererType::Direct3D12:
		manager.createAssetFromMemory<Shader>("vs_ocornut_imgui_embedded", &vs_ocornut_imgui_dx11[0], sizeof(vs_ocornut_imgui_dx11));
		manager.createAssetFromMemory<Shader>("fs_ocornut_imgui_embedded", &fs_ocornut_imgui_dx11[0], sizeof(fs_ocornut_imgui_dx11));

		break;
	case gfx::RendererType::Metal:
		manager.createAssetFromMemory<Shader>("vs_ocornut_imgui_embedded", &vs_ocornut_imgui_mtl[0], sizeof(vs_ocornut_imgui_mtl));
		manager.createAssetFromMemory<Shader>("fs_ocornut_imgui_embedded", &fs_ocornut_imgui_mtl[0], sizeof(fs_ocornut_imgui_mtl));

		break;
	case gfx::RendererType::OpenGL:
	case gfx::RendererType::OpenGLES:
		manager.createAssetFromMemory<Shader>("vs_ocornut_imgui_embedded", &vs_ocornut_imgui_glsl[0], sizeof(vs_ocornut_imgui_glsl));
		manager.createAssetFromMemory<Shader>("fs_ocornut_imgui_embedded", &fs_ocornut_imgui_glsl[0], sizeof(fs_ocornut_imgui_glsl));

		break;
	default:
	
		break;
	}
	manager.load<Shader>("vs_ocornut_imgui_embedded", false)
		.then([&manager](auto vs)
	{
		manager.load<Shader>("fs_ocornut_imgui_embedded", false)
			.then([vs](auto fs)
		{
			sProgram = std::make_unique<Program>(vs, fs);
		});
	});

	sDecl
		.begin()
		.add(gfx::Attrib::Position, 2, gfx::AttribType::Float)
		.add(gfx::Attrib::TexCoord0, 2, gfx::AttribType::Float)
		.add(gfx::Attrib::Color0, 4, gfx::AttribType::Uint8, true)
		.end();

	// init keyboard mapping
	io.KeyMap[ImGuiKey_Tab] = sf::Keyboard::Tab;
	io.KeyMap[ImGuiKey_LeftArrow] = sf::Keyboard::Left;
	io.KeyMap[ImGuiKey_RightArrow] = sf::Keyboard::Right;
	io.KeyMap[ImGuiKey_UpArrow] = sf::Keyboard::Up;
	io.KeyMap[ImGuiKey_DownArrow] = sf::Keyboard::Down;
	io.KeyMap[ImGuiKey_Home] = sf::Keyboard::Home;
	io.KeyMap[ImGuiKey_End] = sf::Keyboard::End;
	io.KeyMap[ImGuiKey_Delete] = sf::Keyboard::Delete;
	io.KeyMap[ImGuiKey_Backspace] = sf::Keyboard::BackSpace;
	io.KeyMap[ImGuiKey_Enter] = sf::Keyboard::Return;
	io.KeyMap[ImGuiKey_Escape] = sf::Keyboard::Escape;
	io.KeyMap[ImGuiKey_A] = sf::Keyboard::A;
	io.KeyMap[ImGuiKey_C] = sf::Keyboard::C;
	io.KeyMap[ImGuiKey_V] = sf::Keyboard::V;
	io.KeyMap[ImGuiKey_X] = sf::Keyboard::X;
	io.KeyMap[ImGuiKey_Y] = sf::Keyboard::Y;
	io.KeyMap[ImGuiKey_Z] = sf::Keyboard::Z;

	std::uint8_t* data;
	std::int32_t width;
	std::int32_t height;
	float _fontSize = 15;

	ImFontConfig config;
	config.FontDataOwnedByAtlas = false;
	config.MergeMode = false;
	config.MergeGlyphCenterV = true;

	io.Fonts->AddFontDefault(&config);
	io.Fonts->AddFontFromMemoryTTF((void*)s_robotoRegularTtf, sizeof(s_robotoRegularTtf), _fontSize, &config);
	io.Fonts->AddFontFromMemoryTTF((void*)s_robotoMonoRegularTtf, sizeof(s_robotoMonoRegularTtf), _fontSize - 3.0f, &config);
	io.Fonts->GetTexDataAsRGBA32(&data, &width, &height);

	sFontTexture = std::make_shared<Texture>(
		static_cast<std::uint16_t>(width)
		, static_cast<std::uint16_t>(height)
		, false
		, 1
		, gfx::TextureFormat::BGRA8
		, 0
		, gfx::copy(data, width*height * 4)
		);

	// Store our identifier
	io.Fonts->TexID = sFontTexture.get();

	sGUIStyle.resetStyle();

	return true;
}

void shutdown()
{
	sTextures.clear();
	sContexts.restoreInitialContext();
	sProgram.reset();
	sFontTexture.reset();

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->TexID = nullptr;
	ImGui::Shutdown();
}

void begin()
{
	sTextures.clear();
}

void Image(std::shared_ptr<ITexture> texture, const ImVec2& _size, const ImVec2& _uv0 /*= ImVec2(0.0f, 0.0f) */, const ImVec2& _uv1 /*= ImVec2(1.0f, 1.0f) */, const ImVec4& _tintCol /*= ImVec4(1.0f, 1.0f, 1.0f, 1.0f) */, const ImVec4& _borderCol /*= ImVec4(0.0f, 0.0f, 0.0f, 0.0f) */)
{
	sTextures.push_back(texture);
	ImGui::Image(texture.get(), _size, _uv0, _uv1, _tintCol, _borderCol);
}

bool ImageButton(std::shared_ptr<ITexture> texture, const ImVec2& _size, const ImVec2& _uv0 /*= ImVec2(0.0f, 0.0f) */, const ImVec2& _uv1 /*= ImVec2(1.0f, 1.0f) */, int _framePadding /*= -1 */, const ImVec4& _bgCol /*= ImVec4(0.0f, 0.0f, 0.0f, 0.0f) */, const ImVec4& _tintCol /*= ImVec4(1.0f, 1.0f, 1.0f, 1.0f) */)
{
	sTextures.push_back(texture);
	return ImGui::ImageButton(texture.get(), _size, _uv0, _uv1, _framePadding, _bgCol, _tintCol);
}

GUIStyle& getGUIStyle()
{
	return sGUIStyle;
}

}

void GUIStyle::resetStyle()
{
	col_main_hue = 145.0f / 255.0f;
	col_main_sat = 255.0f / 255.0f;
	col_main_val = 205.0f / 255.0f;

	col_area_hue = 145.0f / 255.0f;
	col_area_sat = 0.0f / 255.0f;
	col_area_val = 65.0f / 255.0f;

	col_back_hue = 145.0f / 255.0f;
	col_back_sat = 0.0f / 255.0f;
	col_back_val = 45.0f / 255.0f;

	col_text_hue = 0.0f / 255.0f;
	col_text_sat = 0.0f / 255.0f;
	col_text_val = 255.0f / 255.0f;
	frameRounding = 0.0f;

	ImVec4 col_text = ImColor::HSV(col_text_hue, col_text_sat, col_text_val);
	ImVec4 col_main = ImColor::HSV(col_main_hue, col_main_sat, col_main_val);
	ImVec4 col_back = ImColor::HSV(col_back_hue, col_back_sat, col_back_val);
	ImVec4 col_area = ImColor::HSV(col_area_hue, col_area_sat, col_area_val);
	setStyleColors(frameRounding, col_text, col_main, col_back, col_area);
}

void GUIStyle::setStyleColors(float rounding, const ImVec4& col_text, const ImVec4& col_main, const ImVec4& col_back, const ImVec4& col_area)
{
	ImGuiStyle& style = gui::GetStyle();
	style.FrameRounding = rounding;
	style.WindowRounding = rounding;
	style.Colors[ImGuiCol_Text] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(col_text.x, col_text.y, col_text.z, 0.58f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(col_back.x, col_back.y, col_back.z, 1.00f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(col_text.x, col_text.y, col_text.z, 0.30f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(col_back.x, col_back.y, col_back.z, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.68f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(col_main.x, col_main.y, col_main.z, 1.0f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(col_main.x, col_main.y, col_main.z, 1.0f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.0f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.0f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.31f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(col_text.x, col_text.y, col_text.z, 0.80f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.54f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(col_main.x, col_main.y, col_main.z, 0.44f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(col_main.x, col_main.y, col_main.z, 0.76f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(col_text.x, col_text.y, col_text.z, 0.32f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(col_text.x, col_text.y, col_text.z, 0.78f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(col_main.x, col_main.y, col_main.z, 0.20f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(col_text.x, col_text.y, col_text.z, 0.16f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(col_text.x, col_text.y, col_text.z, 0.39f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.43f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.10f, 0.10f, 0.10f, 0.55f);
}
