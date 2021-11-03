//
// Created by erik_ on 02.11.2021.
//

#ifndef MINITY_ANIMATORINTERACTOR_H
#define MINITY_ANIMATORINTERACTOR_H
#include "Interactor.h"

namespace minity {
    class Viewer;
    class AnimatorInteractor : public Interactor {
        public:
            AnimatorInteractor(Viewer * viewer);
    };

}

#endif //MINITY_ANIMATORINTERACTOR_H
