#include "HalfSpace.h"
#include <cmath>

// 법선 벡터(m)와 평면 위의 한 점(C)을 이용한 생성자
HalfSpace::HalfSpace(const Vector& m, const Point& C) {
    x = m.x;
    y = m.y;
    z = m.z;
    // 평면의 방정식: m.x*P.x + m.y*P.y + m.z*P.z + w = 0
    w = -(m.x * C.x + m.y * C.y + m.z * C.z);
}

// 평면 위의 세 점(A, B, C)과 평면 내부를 향하는 점(P)을 이용한 생성자
HalfSpace::HalfSpace(const Point& A, const Point& B, const Point& C, const Point& P) {
    // Affine.h의 Hcoord 연산자(-)를 통해 생성된 객체를 Vector로 변환 (w=0 자동 검증)
    Vector AB(B - A);
    Vector AC(C - A);
    Vector n = cross(AB, AC);

    x = n.x;
    y = n.y;
    z = n.z;
    w = -(n.x * A.x + n.y * A.y + n.z * A.z);

    // P가 평면의 외부에 있다면(dot > 0), 법선의 방향을 뒤집어 P가 내부(dot <= 0)에 오게 함
    if (dot(*this, P) > 0.0f) {
        x = -x;
        y = -y;
        z = -z;
        w = -w;
    }
}

// 정규화된 뷰 볼륨(Cube)을 위한 생성자
Polyhedron::Polyhedron(bool cube) {
    if (cube) {
        // -W <= X, Y, Z <= W 를 만족하는 6개의 Clip Space 평면
        half_spaces.push_back(HalfSpace(1.0f, 0.0f, 0.0f, -1.0f));   // Right: X <= W
        half_spaces.push_back(HalfSpace(-1.0f, 0.0f, 0.0f, -1.0f));  // Left: -X <= W
        half_spaces.push_back(HalfSpace(0.0f, 1.0f, 0.0f, -1.0f));   // Top: Y <= W
        half_spaces.push_back(HalfSpace(0.0f, -1.0f, 0.0f, -1.0f));  // Bottom: -Y <= W
        half_spaces.push_back(HalfSpace(0.0f, 0.0f, 1.0f, -1.0f));   // Z <= W
        half_spaces.push_back(HalfSpace(0.0f, 0.0f, -1.0f, -1.0f));  // -Z <= W
    }
}

// 카메라 뷰 프러스텀(Eye Space)을 위한 생성자
Polyhedron::Polyhedron(float fov, float a, float N, float F) {
    float e = 1.0f / std::tan(fov / 2.0f);
    
    half_spaces.push_back(HalfSpace(e, 0.0f, a, 0.0f));       // Right
    half_spaces.push_back(HalfSpace(-e, 0.0f, a, 0.0f));      // Left
    half_spaces.push_back(HalfSpace(0.0f, e, 1.0f, 0.0f));    // Top
    half_spaces.push_back(HalfSpace(0.0f, -e, 1.0f, 0.0f));   // Bottom
    half_spaces.push_back(HalfSpace(0.0f, 0.0f, 1.0f, N));    // Near: Z <= -N
    half_spaces.push_back(HalfSpace(0.0f, 0.0f, -1.0f, -F));  // Far: Z >= -F
}

// 점이 다면체 내부에 있는지 판별 (모든 평면의 내부에 있어야 함)
bool contains(const Polyhedron& polyhedron, const Hcoord& P) {
    for (size_t i = 0; i < polyhedron.half_spaces.size(); ++i) {
        if (dot(polyhedron.half_spaces[i], P) > 0.0f) {
            return false;
        }
    }
    return true;
}