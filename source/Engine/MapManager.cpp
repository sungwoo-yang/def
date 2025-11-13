 #include "Engine/MapManager.h"
#include "Engine/Collision.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameObjectManager.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Logger.hpp"
#include "Engine/MapElement.h"
#include "Engine/Path.hpp"

namespace CS230
{
    // --- MapManager ---
    MapManager::~MapManager()
    {
        Unload();
    }

    void MapManager::AddMap(Map* new_map)
    {
        maps.push_back(new_map);
    }

    void MapManager::LoadMap()
    {
        if (maps.empty())
            return;
        maps[currentMapIndex]->OpenSVG();
    }

    void MapManager::Unload()
    {
        for (Map* map : maps)
        {
            delete map;
        }
        maps.clear();
    }

    Map* MapManager::GetCurrentMap()
    {
        if (maps.empty())
            return nullptr;
        return maps[currentMapIndex];
    }

    /*void MapManager::Update(double dt)
    {*/
    void MapManager::Update([[maybe_unused]] double dt)
    {
        Map* currentMap = GetCurrentMap();
        if (currentMap && !currentMap->IsLevelLoaded())
        {
            currentMap->ParseSVG(); // 프레임당 한 태그씩 파싱
        }
    }

    // --- Map ---
    Map::Map(const std::string& filename)
        : file_path(filename), level_loaded(false), currentCommand('\0'), pathRegex(R"(<path[^>]*\sd\s*=\s*"([^"]+))"), gIdRegex(R"(<g[^>]*\bid\s*=\s*"([^"]+))"),
          transformRegex(R"xxx(transform\s*=\s*"([^"]+)")xxx"), translateRegex(R"(translate\(([^,]+),\s*([^\)]+)\))"), rotateRegex(R"(rotate\(\s*([^\s,]+)\s*,\s*([^\s,]+)\s*,\s*([^\)]+)\s*\))"),
          matrixRegex(R"(matrix\(([^,]+),([^,]+),([^,]+),([^,]+),([^,]+),([^,]+)\))"), pathIdRegex(R"xxx(id="([^"]+)")xxx"), fillColorRegex(R"(fill:\s*(#[0-9a-fA-F]+);)"), gEndTagRegex(R"(</g>)"),
          svgEndTagRegex(R"(</svg>)"), scale({ 1.0f, 1.0f }), IsinG(false), IsTranslate(false), IsRotate(false), IsScale(false)
    {
        // file_path를 assets::locate_asset을 통과한 경로로 저장합니다.
        try
        {
            this->file_path = assets::locate_asset(filename).string();
            Engine::GetLogger().LogDebug("Map 객체 생성 (경로 확인): " + this->file_path);
        }
        catch (const std::exception& e)
        {
            Engine::GetLogger().LogError("Map 생성 실패, 파일을 찾을 수 없음: " + filename);
            Engine::GetLogger().LogError(e.what());
            this->file_path = filename; // <--- 실패해도 일단 저장
        }
    }

    Map::~Map()
    {
        if (map_file.is_open())
            map_file.close();
    }

    void Map::OpenSVG()
    {
        map_file.open(file_path);
        if (!map_file.is_open())
        {
            Engine::GetLogger().LogError(file_path + " SVG 파일 열기 오류.");
            return;
        }
        Engine::GetLogger().LogEvent(file_path + " SVG 파일 열기 성공.");
    }

    void Map::ParseSVG()
    {
        if (level_loaded || !map_file.is_open())
            return;

        std::string line;
        if (!std::getline(map_file, line))
        { // 파일 끝 도달
            level_loaded = true;
            map_file.close();
            Engine::GetLogger().LogEvent(file_path + " 파싱 완료 (파일 끝).");
            return;
        }
        currentTagBuffer += line; // 태그가 여러 줄일 수 있으므로 버퍼에 추가

        // 태그가 완성되었는지 ('>') 확인
        size_t tagEnd = currentTagBuffer.find('>');
        if (tagEnd == std::string::npos)
        {
            return; // 아직 태그 완성 안됨
        }

        std::string currentTag = currentTagBuffer.substr(0, tagEnd + 1);
        currentTagBuffer.erase(0, tagEnd + 1); // 처리된 태그는 버퍼에서 제거

        std::smatch match;

        if (std::regex_search(currentTag, match, svgEndTagRegex))
        {
            level_loaded = true;
            map_file.close();
            Engine::GetLogger().LogEvent(file_path + " SVG 파싱 완료.");
            return;
        }

        if (std::regex_search(currentTag, match, gEndTagRegex))
        {
            IsinG           = false;
            IsTranslate     = false;
            IsRotate        = false;
            IsScale         = false;
            translate       = { 0, 0 };
            rotateAngle     = 0;
            rotatetranslate = { 0, 0 };
            scale           = { 1.0f, 1.0f };
            return;
        }

        if (std::regex_search(currentTag, match, gIdRegex))
        {
            IsinG = true;
        }

        if (std::regex_search(currentTag, match, transformRegex))
        {
            std::string transformStr = match[1].str();
            if (std::regex_search(transformStr, match, matrixRegex))
            {
                IsScale = true;
                float a = std::stof(match[1].str());
                float b = std::stof(match[2].str());
                float c = std::stof(match[3].str());
                float d = std::stof(match[4].str());
                scale.x = std::sqrt(a * a + c * c);
                scale.y = std::sqrt(b * b + d * d);
            }
            else if (std::regex_search(transformStr, match, rotateRegex))
            {
                IsTranslate       = false;
                IsRotate          = true;
                rotateAngle       = -std::stof(match[1].str()) * static_cast<float>(M_PI) / 180.0f;
                rotatetranslate.x = std::stof(match[2].str());
                rotatetranslate.y = std::stof(match[3].str());
            }
            else if (std::regex_search(transformStr, match, translateRegex))
            {
                IsRotate    = false;
                IsTranslate = true;
                translate.x = std::stof(match[1].str());
                translate.y = std::stof(match[2].str());
            }
            return;
        }

        if (std::regex_search(currentTag, match, pathRegex))
        {
            Engine::GetLogger().LogDebug(">>> <path> 태그 발견! MapElement 객체 생성 시도...");
            std::string pathData = match[1].str();
            std::replace(pathData.begin(), pathData.end(), ' ', ',');
            std::vector<Math::vec2> positions = parsePathData(pathData);
            if (positions.empty())
                return;

            for (auto& vec : positions)
            {
                if (IsinG)
                {
                    if (IsScale)
                    {
                        vec.x *= scale.x;
                        vec.y *= scale.y;
                    }
                    if (IsRotate)
                    {
                        vec.x += rotatetranslate.x;
                        vec.y += rotatetranslate.y;
                        double rotatedX = vec.x * std::cos(rotateAngle) - vec.y * std::sin(rotateAngle);
                        double rotatedY = vec.x * std::sin(rotateAngle) + vec.y * std::cos(rotateAngle);
                        vec.x          = rotatedX;
                        vec.y          = rotatedY;
                    }
                    if (IsTranslate)
                    {
                        vec.x += translate.x;
                        vec.y += translate.y;
                    }
                }
                vec.y = -vec.y; // SVG Y축(아래=+) -> 엔진 Y축(위=+) 변환
            }

            if (std::regex_search(currentTag, match, fillColorRegex))
            {
                fillColor = match[1].str();
            }

            // --- ?? 4, 5단계에서 만든 클래스 사용! ?? ---
            Polygon poly;
            poly.vertices    = positions;
            poly.vertexCount = static_cast<int>(positions.size());

            Math::vec2 poly_center = poly.FindCenter();

            // (static bool을 사용해 딱 한 번만 로그가 찍히도록 함)
            static bool first_path_logged = false;
            if (!first_path_logged)
            {
                Engine::GetLogger().LogEvent("!!! 첫 번째 맵 좌표: " + std::to_string(poly_center.x) + ", " + std::to_string(poly_center.y));
                first_path_logged = true;
            }

            Polygon modified_poly = poly;
            for (auto& v : modified_poly.vertices)
            {
                v -= poly_center; // 중심점을 (0,0)으로 이동
            }

            // MapElement 객체 생성 (위치 = 폴리곤 중심)
            MapElement* map_obj = new MapElement(poly_center, modified_poly);

            // SATCollision 컴포넌트 추가 (로컬 폴리곤 기준)


            // GameObjectManager에 최종 객체 추가
            Engine::GetGameStateManager().GetGSComponent<GameObjectManager>()->Add(map_obj);
            // --- ?? 작업 완료 ?? ---

            return;
        }
    }

    std::vector<Math::vec2> Map::parsePathData(const std::string& pathData)
    {
        // (지인 파일 로직 포팅, Math::vec2 사용)
        std::istringstream      stream(pathData);
        std::string             data;
        float                   last_x = 0, last_y = 0;
        bool                    isRelative = false;
        std::vector<Math::vec2> positions;

        while (std::getline(stream, data, ','))
        {
            if (data.empty())
                continue;

            if (std::isalpha(data[0]))
            {
                currentCommand = data[0];
                isRelative     = std::islower(currentCommand);
                if (data.length() > 1)
                {
                    data = data.substr(1);
                }
                else
                {
                    continue;
                }
            }

            float x = 0.0f, y = 0.0f;

            try
            {
                if (currentCommand == 'm' || currentCommand == 'M' || currentCommand == 'l' || currentCommand == 'L')
                {
                    x = std::stof(data);
                    if (std::getline(stream, data, ','))
                    {
                        y      = std::stof(data);
                        last_x = isRelative ? last_x + x : x;
                        last_y = isRelative ? last_y + y : y;
                        positions.push_back({ last_x, last_y });
                        if (currentCommand == 'm' || currentCommand == 'M')
                        {
                            currentCommand = isRelative ? 'l' : 'L';
                        }
                    }
                }
                else if (currentCommand == 'v' || currentCommand == 'V')
                {
                    y      = std::stof(data);
                    last_y = isRelative ? last_y + y : y;
                    positions.push_back({ last_x, last_y });
                }
                else if (currentCommand == 'h' || currentCommand == 'H')
                {
                    x      = std::stof(data);
                    last_x = isRelative ? last_x + x : x;
                    positions.push_back({ last_x, last_y });
                }
                else if (currentCommand == 'z' || currentCommand == 'Z')
                {
                    if (!positions.empty())
                    {
                        positions.push_back(positions.front());
                    }
                }
            }
            catch (const std::exception& e)
            {
                Engine::GetLogger().LogError("SVG parsePathData 오류: " + std::string(e.what()));
                continue;
            }
        }
        return positions;
    }
}