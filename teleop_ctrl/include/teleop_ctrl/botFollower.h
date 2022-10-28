//
// Created by sanket on 10/10/22.
//

#ifndef TELEOP_WS_BOTFOLLOWER_H
#define TELEOP_WS_BOTFOLLOWER_H
struct xButtons{
    int buttonA;
    int buttonB;
    int buttonX;
    int buttonY;
    int buttonLB;
    int buttonRB;
    int buttonBack;
    int buttonStart;
    int buttonUnNammed;
    int buttonStickLeft;
    int buttonStickRight;
} ;

struct xAxis{
    float leftStickHorizontal;
    float leftStickVertical;
    float leftLT;
    float rightStickHorizontal;
    float rightStickVertical;
    float rightRT;
    float axisUPDown;
    float axisRightLeft;
} ;

struct xBoxController{
    struct xAxis axis;
    struct xButtons buttons;
};



#endif //TELEOP_WS_BOTFOLLOWER_H
