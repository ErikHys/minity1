//
// Created by erik_ on 02.11.2021.
//

#ifndef MINITY_ANIMATORINTERACTOR_H
#define MINITY_ANIMATORINTERACTOR_H
#include "Interactor.h"
#include "KeyFrame.h"
#include "RaytraceRenderer.h"

namespace minity {
    class Viewer;
    class AnimatorInteractor : public Interactor {
        public:
            AnimatorInteractor(Viewer * viewer);
            virtual void keyEvent(int key, int scancode, int action, int mods);
            virtual void display();


        private:
            std::vector<KeyFrame> keyFrames = {};
            std::vector<mat4> matrixSplines(mat4 m0, mat4 m1, mat4 m2, mat4 m3);
            std::vector<KeyFrame> doInterPolation(std::vector<KeyFrame> vector);
            bool rayTracing = true;

        std::vector<RayTraceInfo> rayTracingSpline(RayTraceInfo p0, RayTraceInfo p1, RayTraceInfo p2, RayTraceInfo p3, double dt);
    };

}

#endif //MINITY_ANIMATORINTERACTOR_H
