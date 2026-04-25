#include "Clip.h"

// 렌더링 파이프라인의 Clip Functor 구현
bool Clip::operator()(std::vector<Hcoord>& vertices) {
    if (vertices.empty()) {
        return false;
    }

    // 각각의 HalfSpace 평면을 순회하며 다각형을 잘라냄
    for (size_t plane_idx = 0; plane_idx < half_spaces.size(); ++plane_idx) {
        if (vertices.empty()) {
            break;
        }

        const HalfSpace& current_plane = half_spaces[plane_idx];
        temp_vertices.clear();

        // 닫힌 다각형이므로 마지막 정점을 이전 정점(prev)으로 초기 설정
        Hcoord prev_vertex = vertices.back();
        float prev_dist = dot(current_plane, prev_vertex);
        bool is_prev_inside = (prev_dist <= 0.0f);

        for (size_t vert_idx = 0; vert_idx < vertices.size(); ++vert_idx) {
            const Hcoord& curr_vertex = vertices[vert_idx];
            float curr_dist = dot(current_plane, curr_vertex);
            bool is_curr_inside = (curr_dist <= 0.0f);

            // 한 점은 외부에, 한 점은 내부에 있어 경계를 교차하는 경우 교차점 계산
            if (is_prev_inside != is_curr_inside) {
                float t = static_cast<float>(prev_dist) / (prev_dist - curr_dist);
                
                // Affine.h의 Hcoord 연산자를 활용한 4D 동차 좌표 보간
                Hcoord intersect_vertex = prev_vertex + t * (curr_vertex - prev_vertex);
                temp_vertices.push_back(intersect_vertex);
            }

            // 현재 점이 평면 안쪽에 있다면 결과 목록에 유지
            if (is_curr_inside) {
                temp_vertices.push_back(curr_vertex);
            }

            prev_vertex = curr_vertex;
            prev_dist = curr_dist;
            is_prev_inside = is_curr_inside;
        }
        
        // 다음 평면 클리핑을 위해 리스트 갱신
        vertices = temp_vertices;
    }

    // 최종 결과가 비어있지 않으면 렌더링 유효 (true 반환)
    return !vertices.empty();
}