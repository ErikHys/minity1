//
// Created by erik_ on 02.11.2021.
//

#include "AnimatorInteractor.h"
#include "Viewer.h"

using namespace minity;
AnimatorInteractor:: AnimatorInteractor(Viewer * viewer) : Interactor(viewer){



}

using namespace glm;
template <unsigned int D>
std::vector<glm::vec<D, float>> catmullRom(glm::vec<D, float> p0, glm::vec<D, float> p1, glm::vec<D, float> p2, glm::vec<D, float> p3) {
    glm::mat<D,4,f32> pointMatrix = glm::transpose(mat<4,D,f32>(p0, p1, p2, p3));
    glm::mat4 catmullRomMatrix = glm::transpose(mat4(vec4(-1.f, 3.f, -3.f, 1.f),
                                                     vec4(2.f, -5.f, 4.f, -1.f),
                                                     vec4(-1.f, 0.f, 1.f, 0.f),
                                                     vec4(0.f, 2.f, 0.f, 0.f)
    ) * 0.5f);
    std::vector<glm::vec<D, float>> spline = {};
    for (int u = 0; u <= 10; u++)
    {

        float uToUse = float(u) / 10;
        vec4 uVector = vec4(uToUse * uToUse * uToUse, uToUse * uToUse, uToUse, 1.f);
        spline.push_back(uVector * catmullRomMatrix * pointMatrix);
    }
    return spline;
}

template <>
std::vector<glm::vec<1, float>> catmullRom<1>(glm::vec<1, float> p0, glm::vec<1, float> p1, glm::vec<1, float> p2, glm::vec<1, float> p3){
    glm::vec<4,float> pointMatrix = vec<4,float>(p0, p1, p2, p3);
    glm::mat4 catmullRomMatrix = glm::transpose(mat4(vec4(-1.f, 3.f, -3.f, 1.f),
                                                     vec4(2.f, -5.f, 4.f, -1.f),
                                                     vec4(-1.f, 0.f, 1.f, 0.f),
                                                     vec4(0.f, 2.f, 0.f, 0.f)
    ) * 0.5f);
    std::vector<glm::vec<1, float>> spline = {};
    for (int u = 0; u <= 10; u++)
    {
        float uToUse = float(u) / 10;
        vec4 uVector = vec4(uToUse * uToUse * uToUse, uToUse * uToUse, uToUse, 1.f);
        float uP = glm::dot(uVector * catmullRomMatrix, pointMatrix);
        spline.push_back(glm::vec1(uP));
    }

    return spline;
}
