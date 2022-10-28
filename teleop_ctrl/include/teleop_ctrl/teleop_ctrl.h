//
// Created by sanket on 10/10/22.
//

#ifndef TELEOP_WS_TELEOP_CTRL_H
#define TELEOP_WS_TELEOP_CTRL_H


#ifdef contone
struct xButtons{
    int32_t button1;
    int32_t button2;
    int32_t button3;
    int32_t button4;
    int32_t button5;
    int32_t button6;
    int32_t button7;
    int32_t button8;
    int32_t button9;
    int32_t button10;
    int32_t lAxisClick;
    int32_t rAxisClick;
} ;

struct xAxis{
    float lAxisHorizontal;
    float lAxisVertical;
    float rAxisHorizontal;
    float rAxisVertical;
    float arrowHorizontal;
    float arrowVertical;
} ;
#endif

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

#endif //TELEOP_WS_TELEOP_CTRL_H
