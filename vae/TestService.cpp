//
// Created by protocol on 4/21/21.
//

#include "TestService.h"
void TestService::processGameLogic(double delta_t){
    double newX = node.getX() + (cos(vd) * delta_t);
    double newY = node.getY() + (sin(vd) * delta_t);

    //node.setX(node.getX() + 1);
    node.setX(newX);
    node.setY(newY);
}