#include "Game/MiniMap.hpp"
#include "Engine/Camera.hpp"
#include "Engine/Engine.hpp"
#include "Engine/MapManager.h"
#include "Engine/Logger.hpp"
#include "Engine/Window.hpp"
#include "Player.hpp"
#include <algorithm>
#include <imgui.h>
#include <utility>

namespace
{
	constexpr double kEpsilon = 1e-5;
}

MiniMap::MiniMap()
	: worldBounds({ { 0.0, 0.0 }, { 1.0, 1.0 } }), windowTitle("Minimap"), player(nullptr), camera(nullptr), mapManager(nullptr), visible(true)
{
}

void MiniMap::SetWorldBounds(Math::rect bounds)
{
	worldBounds = bounds;
}

void MiniMap::AttachPlayer(Player* player_ptr)
{
	player = player_ptr;
}

void MiniMap::AttachCamera(CS230::Camera* camera_ptr)
{
	camera = camera_ptr;
}

void MiniMap::AttachMapManager(CS230::MapManager* map_manager_ptr)
{
	mapManager = map_manager_ptr;
}

void MiniMap::SetStyle(const MiniMapStyle& style_config)
{
	style = style_config;
}

void MiniMap::SetWindowTitle(std::string title_text)
{
	windowTitle = std::move(title_text);
}

void MiniMap::SetVisible(bool enabled)
{
	visible = enabled;
}

bool MiniMap::IsVisible() const
{
	return visible;
}

void MiniMap::DrawImGui()
{
	if (!visible || player == nullptr || camera == nullptr)
		return;

	const float width  = static_cast<float>(style.canvasSize.x);
	const float height = static_cast<float>(style.canvasSize.y);

	if (ImGui::Begin(windowTitle.c_str(), nullptr, ImGuiWindowFlags_NoScrollbar))
	{
		ImVec2 canvas_min = ImGui::GetCursorScreenPos();
		ImVec2 canvas_max = ImVec2(canvas_min.x + width, canvas_min.y + height);
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		draw_list->AddRectFilled(canvas_min, canvas_max, IM_COL32(18, 21, 29, 255));
		draw_list->AddRect(canvas_min, canvas_max, IM_COL32(255, 255, 255, 200), 4.0f, 0, 2.0f);

		if (style.showGrid)
		{
			DrawGrid(draw_list, canvas_min, canvas_max);
		}

		DrawLevelBounds(draw_list, canvas_min, canvas_max);
		DrawTerrainPolygons(draw_list, canvas_min, canvas_max);
		DrawPlayerMarker(draw_list, canvas_min, canvas_max);

		if (style.showCameraFrustum)
		{
			DrawCameraFrustum(draw_list, canvas_min, canvas_max);
		}

		ImGui::Dummy(ImVec2(width, height));
	}
	ImGui::End();
}

Math::vec2 MiniMap::Normalize(const Math::vec2& world_position) const
{
	const double level_width  = std::max(worldBounds.Right() - worldBounds.Left(), kEpsilon);
	const double level_height = std::max(worldBounds.Top() - worldBounds.Bottom(), kEpsilon);

	double u = (world_position.x - worldBounds.Left()) / level_width;
	double v = (world_position.y - worldBounds.Bottom()) / level_height;

	u = std::clamp(u, 0.0, 1.0);
	v = std::clamp(v, 0.0, 1.0);

	return { u, v };
}

void MiniMap::DrawGrid(ImDrawList* draw_list, const ImVec2& canvas_min, const ImVec2& canvas_max) const
{
	if (style.gridDivisions <= 1.0)
		return;

	const float width  = canvas_max.x - canvas_min.x;
	const float height = canvas_max.y - canvas_min.y;
	const int   lines  = static_cast<int>(style.gridDivisions);

	for (int i = 1; i < lines; ++i)
	{
		const float t = static_cast<float>(i) / static_cast<float>(lines);
		const float x = canvas_min.x + width * t;
		draw_list->AddLine(ImVec2(x, canvas_min.y), ImVec2(x, canvas_max.y), IM_COL32(70, 70, 70, 180));

		const float y = canvas_min.y + height * t;
		draw_list->AddLine(ImVec2(canvas_min.x, y), ImVec2(canvas_max.x, y), IM_COL32(70, 70, 70, 180));
	}
}

void MiniMap::DrawLevelBounds(ImDrawList* draw_list, const ImVec2& canvas_min, const ImVec2& canvas_max) const
{
	draw_list->AddRect(canvas_min, canvas_max, IM_COL32(150, 150, 150, 255));
}

void MiniMap::DrawCameraFrustum(ImDrawList* draw_list, const ImVec2& canvas_min, const ImVec2& canvas_max) const
{
	if (camera == nullptr)
		return;

	const Math::vec2 camera_pos = camera->GetPosition();
	Math::vec2       camera_size;
	{
		const Math::ivec2 window_px = Engine::GetWindow().GetSize();
		camera_size = { static_cast<double>(window_px.x), static_cast<double>(window_px.y) };
	}
	Math::vec2 bottom_left  = camera_pos;
	Math::vec2 top_right    = camera_pos + camera_size;

	bottom_left.x = std::clamp(bottom_left.x, worldBounds.Left(), worldBounds.Right());
	bottom_left.y = std::clamp(bottom_left.y, worldBounds.Bottom(), worldBounds.Top());
	top_right.x   = std::clamp(top_right.x,   worldBounds.Left(), worldBounds.Right());
	top_right.y   = std::clamp(top_right.y,   worldBounds.Bottom(), worldBounds.Top());

	const Math::vec2 normalized_bl = Normalize(bottom_left);
	const Math::vec2 normalized_tr = Normalize(top_right);

	const float width  = canvas_max.x - canvas_min.x;
	const float height = canvas_max.y - canvas_min.y;

	ImVec2 rect_min(
		canvas_min.x + static_cast<float>(normalized_bl.x) * width,
		canvas_max.y - static_cast<float>(normalized_tr.y) * height);
	ImVec2 rect_max(
		canvas_min.x + static_cast<float>(normalized_tr.x) * width,
		canvas_max.y - static_cast<float>(normalized_bl.y) * height);

	draw_list->AddRect(rect_min, rect_max, IM_COL32(255, 255, 0, 220), 2.0f, 0, style.cameraLineWidth);
}

void MiniMap::DrawPlayerMarker(ImDrawList* draw_list, const ImVec2& canvas_min, const ImVec2& canvas_max) const
{
	if (player == nullptr)
		return;

	const Math::vec2 player_pos   = player->GetPosition();
	const Math::vec2 normalized   = Normalize(player_pos);
	const float      width        = canvas_max.x - canvas_min.x;
	const float      height       = canvas_max.y - canvas_min.y;
	const float      px           = canvas_min.x + static_cast<float>(normalized.x) * width;
	const float      py           = canvas_max.y - static_cast<float>(normalized.y) * height;

	draw_list->AddCircleFilled(ImVec2(px, py), style.playerMarkerRadius, IM_COL32(0, 220, 130, 255));
	draw_list->AddCircle(ImVec2(px, py), style.playerMarkerRadius + 1.5f, IM_COL32(0, 0, 0, 180));
}

void MiniMap::DrawTerrainPolygons(ImDrawList* draw_list, const ImVec2& canvas_min, const ImVec2& canvas_max) const
{
	if (!style.showTerrain || mapManager == nullptr)
		return;

	const auto& polygons = mapManager->GetMiniMapPolygons();
	if (polygons.empty())
		return;

	const float width  = canvas_max.x - canvas_min.x;
	const float height = canvas_max.y - canvas_min.y;

	for (const Polygon& polygon : polygons)
	{
		const size_t vertex_count = polygon.vertices.size();
		if (vertex_count < 2)
			continue;

		for (size_t i = 0; i < vertex_count; ++i)
		{
			const size_t next_index = (i + 1) % vertex_count;
			Math::vec2 p1_norm = Normalize(polygon.vertices[i]);
			Math::vec2 p2_norm = Normalize(polygon.vertices[next_index]);

			ImVec2 v1(
				canvas_min.x + static_cast<float>(p1_norm.x) * width,
				canvas_max.y - static_cast<float>(p1_norm.y) * height);
			ImVec2 v2(
				canvas_min.x + static_cast<float>(p2_norm.x) * width,
				canvas_max.y - static_cast<float>(p2_norm.y) * height);

			draw_list->AddLine(v1, v2, IM_COL32(120, 200, 255, 190), style.terrainLineWidth);
		}
	}
}
