//
// Created by erik_ on 02.11.2021.
//

#include "AnimatorInteractor.h"
#include "Viewer.h"
#include <GLFW/glfw3.h>

using namespace glm;
template <unsigned int D>
std::vector<glm::vec<D, float>> catmullRom(glm::vec<D, float> p0, glm::vec<D, float> p1, glm::vec<D, float> p2, glm::vec<D, float> p3, double delta) {
    glm::mat<D,4,f32> pointMatrix = glm::transpose(mat<4,D,f32>(p0, p1, p2, p3));
    glm::mat4 catmullRomMatrix = glm::transpose(mat4(vec4(-1.f, 3.f, -3.f, 1.f),
                                                     vec4(2.f, -5.f, 4.f, -1.f),
                                                     vec4(-1.f, 0.f, 1.f, 0.f),
                                                     vec4(0.f, 2.f, 0.f, 0.f)
    ) * 0.5f);
    std::vector<glm::vec<D, float>> spline = {};
    int nrOfSplines = int(1.0/delta * 2.0);
    for (int u = 0; u <= nrOfSplines; u++)
    {

        float uToUse = float(u) / nrOfSplines;
        vec4 uVector = vec4(uToUse * uToUse * uToUse, uToUse * uToUse, uToUse, 1.f);
        spline.push_back(uVector * catmullRomMatrix * pointMatrix);
    }
    return spline;
}

template <>
std::vector<glm::vec<1, float>> catmullRom<1>(glm::vec<1, float> p0, glm::vec<1, float> p1, glm::vec<1, float> p2, glm::vec<1, float> p3, double delta){
    glm::vec<4,float> pointMatrix = vec<4,float>(p0, p1, p2, p3);
    glm::mat4 catmullRomMatrix = glm::transpose(mat4(vec4(-1.f, 3.f, -3.f, 1.f),
                                                     vec4(2.f, -5.f, 4.f, -1.f),
                                                     vec4(-1.f, 0.f, 1.f, 0.f),
                                                     vec4(0.f, 2.f, 0.f, 0.f)
    ) * 0.5f);
    std::vector<glm::vec<1, float>> spline = {};
    int nrOfSplines = int(1.0/delta * 2.0);

    for (int u = 0; u <= nrOfSplines; u++)
    {
        float uToUse = float(u) / nrOfSplines;
        vec4 uVector = vec4(uToUse * uToUse * uToUse, uToUse * uToUse, uToUse, 1.f);
        float uP = glm::dot(uVector * catmullRomMatrix, pointMatrix);
        spline.push_back(glm::vec1(uP));
    }

    return spline;
}


using namespace minity;
AnimatorInteractor:: AnimatorInteractor(Viewer * viewer) : Interactor(viewer){
}

double lastTime = 0.0;
double deltaTime = 0.0;
static int indexOfAnimation = 0;
static bool play = false;

void AnimatorInteractor::keyEvent(int key, int scancode, int action, int mods) {

    if(key == GLFW_KEY_KP_ADD && action == GLFW_PRESS){
        if(!rayTracing){
        KeyFrame keyFrame = KeyFrame(viewer()->viewTransform(),
                                     viewer()->lightTransform(), viewer()->backgroundColor(), viewer()->getExplosionDist());
        keyFrames.push_back(keyFrame);
        }else{
            RayTraceInfo rayTraceInfo = viewer()->getRayTraceInfo();
            KeyFrame keyFrame = KeyFrame(viewer()->viewTransform(),
                                         viewer()->lightTransform(),
                                         viewer()->backgroundColor(), viewer()->getExplosionDist(),
                                         rayTraceInfo);
            keyFrames.push_back(keyFrame);
        }
        globjects::debug() << "Added frame nr: " << std::to_string(keyFrames.size());
    } else if(key == 333 && action == GLFW_PRESS){
        //Remove KeyFrame
        if(!keyFrames.empty()){
            keyFrames.pop_back();
            globjects::debug() << "Removed frame from animation queue, size: " << std::to_string(keyFrames.size());
        } else{
            globjects::debug() << "Animation queue already empty";
        }

    }else if(key == GLFW_KEY_P && action == GLFW_PRESS){
        //Play animation
        //Get splines
        //Change state for each intermediate value
        if (keyFrames.size() >= 4){
            //Do animation
            globjects::debug() << "Animating: " << std::to_string(keyFrames.size()) << " in queue.";
            keyFrames = doInterPolation(keyFrames);
            play = true;

        }else{
            globjects::debug() << "Not enough points to animate, only " << std::to_string(keyFrames.size()) << " in queue.";
        }
    }else if(key == GLFW_KEY_D && action == GLFW_PRESS){
        globjects::debug() << "Deleting queue";
        globjects::debug() << "detlaTime " << std::to_string(deltaTime);
        keyFrames = {};
    }else if(key == GLFW_KEY_S && action == GLFW_PRESS){
        globjects::debug() << "Stopping, or restarting animation";
        play = !play;
        indexOfAnimation = 0;
    }else if(key == GLFW_KEY_R && action == GLFW_PRESS){
        this->rayTracing = !rayTracing;
        globjects::debug() << "Raytracing = " << rayTracing;

    }
}


void AnimatorInteractor::display() {
    Interactor::display();
    double currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    if(play){
        if(keyFrames.empty()){
            play = false;
        }else{
            if(rayTracing){
                KeyFrame keyFrame = keyFrames.at(indexOfAnimation);
                viewer()->setLightTransform(keyFrame.getModelLightTransform());
                viewer()->setViewTransform(keyFrame.getModelViewTransform());
                viewer()->setBackgroundColor(keyFrame.getBackgroundColor());
                viewer()->setExplosionDist(keyFrame.getExplosionDist());
                viewer()->setRayTraceInfo(keyFrame.rayTraceInfo);
                indexOfAnimation = (indexOfAnimation+1) % keyFrames.size();
            } else{
                KeyFrame keyFrame = keyFrames.at(indexOfAnimation);
                viewer()->setLightTransform(keyFrame.getModelLightTransform());
                viewer()->setViewTransform(keyFrame.getModelViewTransform());
                viewer()->setBackgroundColor(keyFrame.getBackgroundColor());
                viewer()->setExplosionDist(keyFrame.getExplosionDist());
                indexOfAnimation = (indexOfAnimation+1) % keyFrames.size();
            }
        }
    }
}
double dt = 0.0;
std::vector<mat4> AnimatorInteractor::matrixSplines(mat4 m0, mat4 m1, mat4 m2, mat4 m3){
    vec3 translation0 = vec3(0.f); vec3 translation1 = vec3(0.f); vec3 translation2 = vec3(0.f); vec3 translation3 = vec3(0.f);
    mat4 rotation0 = mat4(0.f); mat4 rotation1 = mat4(0.f); mat4 rotation2 = mat4(0.f); mat4 rotation3 = mat4(0.f);
    vec3 scale0 = vec3(0.f); vec3 scale1 = vec3(0.f); vec3 scale2 = vec3(0.f); vec3 scale3 = vec3(0.f);
    matrixDecompose(m0, translation0, rotation0, scale0, true); matrixDecompose(m1, translation1, rotation1, scale1, true);
    matrixDecompose(m2, translation2, rotation2, scale2, true); matrixDecompose(m3, translation3, rotation3, scale3, true);
    std::vector<vec3> translationSplines = catmullRom<3>(translation0, translation1, translation2, translation3, dt);
    std::vector<vec3> scaleSplines = catmullRom<3>(scale0, scale1, scale2, scale3, dt);
    std::vector<vec4> rotation0Splines = catmullRom<4>(rotation0[0], rotation1[0], rotation2[0], rotation3[0], dt);
    std::vector<vec4> rotation1Splines = catmullRom<4>(rotation0[1], rotation1[1], rotation2[1], rotation3[1], dt);
    std::vector<vec4> rotation2Splines = catmullRom<4>(rotation0[2], rotation1[2], rotation2[2], rotation3[2], dt);
    std::vector<vec4> rotation3Splines = catmullRom<4>(rotation0[3], rotation1[3], rotation2[3], rotation3[3], dt);
    std::vector<mat4> result = {};
    for(int i = 0; i < translationSplines.size(); i++){
        mat4 tMat = mat4(1.f);
        tMat[3] = vec4(translationSplines.at(i), 1.f);
        mat4 sMat = mat4(scaleSplines.at(i)[0], 0.f, 0.f, 0.f,
                         0.f, scaleSplines.at(i)[1], 0.f, 0.f,
                         0.f, 0.f, scaleSplines.at(i)[2], 0.f,
                         0.f, 0.f, 0.f, 1.f);

        result.emplace_back(sMat *
        mat4(rotation0Splines.at(i), rotation1Splines.at(i), rotation2Splines.at(i), rotation3Splines.at(i)) *
        tMat);
    }
    return result;
}

std::vector<RayTraceInfo> AnimatorInteractor::rayTracingSpline(RayTraceInfo p0, RayTraceInfo p1,
                                                               RayTraceInfo p2, RayTraceInfo p3, double dt){
    std::vector<vec3> sphereColors = catmullRom<3>(p0.sphereColor, p1.sphereColor, p2.sphereColor, p3.sphereColor, dt);
    std::vector<vec3> boxColors = catmullRom<3>(p0.boxColor, p1.boxColor, p2.boxColor, p3.boxColor, dt);
    std::vector<vec3> cylinderColors = catmullRom<3>(p0.cylinderColor, p1.cylinderColor, p2.cylinderColor, p3.cylinderColor, dt);
    std::vector<vec3> planeColors = catmullRom<3>(p0.planeColor, p1.planeColor, p2.planeColor, p3.planeColor, dt);
    std::vector<vec3> spherePositions = catmullRom<3>(p0.spherePosition, p1.spherePosition, p2.spherePosition, p3.sphereColor, dt);
    std::vector<vec3> boxPositions = catmullRom<3>(p0.boxPosition, p1.boxPosition, p2.boxPosition, p3.boxPosition, dt);
    std::vector<vec3> cylinderPositions = catmullRom<3>(p0.cylinderPosition, p1.cylinderPosition, p2.cylinderPosition, p3.cylinderPosition, dt);
    std::vector<vec3> planeNormals = catmullRom<3>(p0.planeNormal, p1.planeNormal, p2.planeNormal, p3.planeNormal, dt);
    std::vector<vec1> planePositions = catmullRom<1>(vec1(p0.planePosition), vec1(p1.planePosition),
                                                    vec1(p2.planePosition),vec1(p3.planePosition), dt);
    std::vector<vec1> cylinderHeights = catmullRom<1>(vec1(p0.cylinderHeight), vec1(p1.cylinderHeight),
                                                    vec1(p2.cylinderHeight),vec1(p3.cylinderHeight), dt);
    std::vector<vec1> boxScales = catmullRom<1>(vec1(p0.boxScale), vec1(p1.boxScale),
                                                    vec1(p2.boxScale),vec1(p3.boxScale), dt);
    std::vector<vec1> sphereScales = catmullRom<1>(vec1(p0.sphereScale), vec1(p1.sphereScale),
                                                    vec1(p2.sphereScale),vec1(p3.sphereScale), dt);
    std::vector<vec3> lightColors = catmullRom<3>(p0.lightColor, p1.lightColor, p2.lightColor, p3.lightColor, dt);
    std::vector<vec1> lightIntensityFronts = catmullRom<1>(vec1(p0.lightIntensityFront), vec1(p1.lightIntensityFront),
                                                   vec1(p2.lightIntensityFront),vec1(p3.lightIntensityFront), dt);
    std::vector<RayTraceInfo> result = {};
    for(int i = 0; i < sphereColors.size(); i++){
        RayTraceInfo r = RayTraceInfo(p0.sphere, p0.box, p0.cylinder, p0.plane, sphereColors.at(i), boxColors.at(i),
                                      cylinderColors.at(i), planeColors.at(i), spherePositions.at(i), boxPositions.at(i),
                                      cylinderPositions.at(i), planeNormals.at(i), planePositions.at(i).x, cylinderHeights.at(i).x,
                                      boxScales.at(i).x, sphereScales.at(i).x, lightColors.at(i), p0.celShading, p0.levelOfCelShading,
                                      lightIntensityFronts.at(i).x);
        result.emplace_back(r);
    }
    return result;

}

std::vector<KeyFrame> AnimatorInteractor::doInterPolation(std::vector<KeyFrame> frames) {
    std::vector<KeyFrame> result = {};
    dt = deltaTime;
    for(int i = 0; i < frames.size()-3; i++){
        std::vector<mat4> mwts = matrixSplines(frames.at(i).getModelViewTransform(), frames.at(i+1).getModelViewTransform(),
                                               frames.at(i+2).getModelViewTransform(), frames.at(i+3).getModelViewTransform());
        std::vector<mat4> mlts = matrixSplines(frames.at(i).getModelLightTransform(), frames.at(i+1).getModelLightTransform(),
                                               frames.at(i+2).getModelLightTransform(), frames.at(i+3).getModelLightTransform());
        std::vector<vec3> bgcs = catmullRom<3>(frames.at(i).getBackgroundColor(), frames.at(i+1).getBackgroundColor(),
                                               frames.at(i+2).getBackgroundColor(), frames.at(i+3).getBackgroundColor(), dt);
        std::vector<vec1> eds = catmullRom<1>(vec1(frames.at(i).getExplosionDist()), vec1(frames.at(i+1).getExplosionDist()),
                                             vec1(frames.at(i+2).getExplosionDist()), vec1(frames.at(i+3).getExplosionDist()), dt);
        std::vector<RayTraceInfo> rayTraceInfo;
        if(rayTracing){
        rayTraceInfo = rayTracingSpline(frames.at(i).rayTraceInfo, frames.at(i+1).rayTraceInfo,
                                                                  frames.at(i+2).rayTraceInfo, frames.at(i+3).rayTraceInfo, dt);
        }
        for(int j = 0; j < bgcs.size(); j++){
            if(rayTracing){
            result.emplace_back(mwts.at(j), mlts.at(j),
                                bgcs.at(j), eds.at(j).x, rayTraceInfo.at(j));

            }else{
                result.emplace_back(mwts.at(j), mlts.at(j),
                                    bgcs.at(j), eds.at(j).x);
            }
        }
    }
    globjects::debug() << "Animation starting...";

    return result;
}


