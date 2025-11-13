#pragma once

#include "Engine/Component.hpp"
#include "Engine/Polygon.h" // Polygon.h 인클루드
#include "Engine/Vec2.hpp"
#include <fstream>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

// Pi 상수 정의
#ifndef M_PI
#    define M_PI 3.14159265358979323846
#endif

namespace CS230
{
    class Map; // 전방 선언

    class MapManager : public Component
    {
    public:
        MapManager() : currentMapIndex(0)
        {
        }

        ~MapManager();

        void AddMap(Map* new_map);
        void LoadMap(); // 현재 맵의 SVG 파일을 엽니다.
        void Unload();  // 모든 맵을 메모리에서 해제합니다.
        /*void Update(double dt) override;*/
        void Update([[maybe_unused]] double dt) override;
        Map* GetCurrentMap();

    private:
        std::vector<Map*> maps;
        int               currentMapIndex;
    };

    class Map : public Component
    {
    public:
        Map(const std::string& filename);
        ~Map();

        void OpenSVG();  // SVG 파일을 엽니다.
        void ParseSVG(); // SVG 파일 내용을 *프레임당 한 태그씩* 파싱합니다.

        bool IsLevelLoaded() const
        {
            return level_loaded;
        }

    private:
        std::vector<Math::vec2> parsePathData(const std::string& pathData);

        std::ifstream map_file;
        std::string   file_path;
        bool          level_loaded   = false;
        char          currentCommand = '\0';
        std::string   currentTagBuffer; // <g> 태그처럼 여러 줄일 경우를 대비한 버퍼

        // SVG 파싱용 정규식
        std::regex pathRegex;
        std::regex gIdRegex;
        std::regex transformRegex;
        std::regex translateRegex;
        std::regex rotateRegex;
        std::regex matrixRegex;
        std::regex pathIdRegex;
        std::regex fillColorRegex;
        std::regex gEndTagRegex;   // </g>
        std::regex svgEndTagRegex; // </svg>

        // SVG 파싱 상태 변수
        Math::vec2  translate       = { 0, 0 };
        float       rotateAngle     = 0;
        Math::vec2  rotatetranslate = { 0, 0 };
        Math::vec2  scale           = { 1.0f, 1.0f };
        std::string fillColor       = "#000000";
        bool        IsinG           = false;
        bool        IsTranslate     = false;
        bool        IsRotate        = false;
        bool        IsScale         = false;
    };
}