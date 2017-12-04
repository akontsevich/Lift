#include <iostream>
#include "lift.h"

int main(int argc, char **argv)
{
    uint number;
    double height, speed, delay;

    if (argc < 5) {
        std::cerr << "Usage: " << argv[0] << " <floors> <height> <speed> <delay>" << std::endl;
        return 1;
    }
    number = (uint)atoi(argv[1]);
    if(number <5 && number > 20)
        std::cerr << "Floors number should be between 5 and 20" << std::endl;
    height = atof(argv[2]);
    speed = atof(argv[3]);
    delay = atof(argv[4]);

    Lift lift(number, height, speed, delay);

    std::cout << "Enter 0 floor to exit" << std::endl;

    for(;;) {
        uint floor;
        bool inside = false;
        char type;

        std::cout << "Call lift from entrance hall (e) or push button inside (i) lift:";
        std::cin >> type;
        switch (type) {
        case 'i':
            std::cout << "Push button number:";
            inside = true;
            break;
        case 'e':
        default:
            std::cout << "Call lift from floor:";
            break;
        case '0':
            lift.Exit();
            return 0;
            break;
        }
        std::cin >> floor;
        if(floor != 0) {
            lift.SendCommand(floor, inside);
        } else {
            lift.Exit();
            return 0;
        }
    }
    return 0;
}
