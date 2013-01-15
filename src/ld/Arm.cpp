#include "Arm.h"
#include "ArmDriver.h"

Arm::Arm( ) {
    for(int i = 0; i < ARM_SERVOS; ++i ) {
        oldTheta[ i ] = 0;
        newTheta[ i ] = 0;
    }
    armServosInit();
}

void Arm::setNewTheta( int newTheta[ARM_SERVOS], int time ) {
    int max = 0;
    for( int i = 0; i < ARM_SERVOS; ++i ) {
        this->newTheta[ i ] = newTheta[ i ];
        int diff = ABS( this->newTheta[ i ] - this->oldTheta[ i ] );
        if ( max < diff ) {
            max = diff;
        }
    }
    for( int i = 0; i < ARM_SERVOS; ++i ) {
        int diff = this->newTheta[ i ] - this->oldTheta[ i ];
        delay[ i ] = ARM_DELAYMS * max / diff;
    }
    startTime = time;
}
void Arm::moveAll( int time ) {
    int currentTheta[ARM_SERVOS];
    for( int i = 0; i < ARM_SERVOS; ++i ) {
        int diff = newTheta[ i ] - oldTheta[ i ];
        if ( diff != 0 ) {
            int change = ( time - startTime ) / delay[ i ];
            if ( change > 0 ) {
                if ( newTheta[ i ] - oldTheta[ i ] - change < diff ) {
                    currentTheta[ i ] = oldTheta[ i ] + change;
                } else {
                    currentTheta[ i ] = oldTheta[ i ] + newTheta[ i ];
                }
            }
        }
    }
    armServosWrite( currentTheta );
}