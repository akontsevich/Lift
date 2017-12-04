#include <cmath>
#include <chrono>
#include <iostream>

#include "lift.h"

Lift::Lift(uint number, double height, double speed, double delay)
    : number_(number)
    , height_(height)
    , speed_(speed)
    , delay_(delay)
{
    // Start lift worker thread
    std::thread (&Lift::Work, this).detach();
}

void Lift::SendCommand(uint floor, bool inside)
{
    command_mutex_.lock();
    if(floor >= 1 && floor <=number_)
        commands.emplace(Command(floor, inside));
    else
        std::cerr << "Error: floor number must be between 1 and " << number_ << std::endl;
    command_mutex_.unlock();
}

void Lift::Work()
{
    while(running_) {
        // Perform pending lift action
        (this->*action)();

        std::this_thread::sleep_for(std::chrono::milliseconds(PERIOD));
    }
}

void Lift::setState(LiftState state)
{
    state_ = state;
    command_time_ = 0.0;

    switch (state_) {
    case LiftState::DoorsOpenned:
        action = &Lift::OpenDoors;
        break;
    case LiftState::MovingDown:
        action = &Lift::MoveDown;
        break;
    case LiftState::MovingUp:
        action = &Lift::MoveUp;
        break;
    default:
        action = &Lift::Stop;
        break;
    }
}

void Lift::Stop()
{
    if(commands.size() > 0) {
        command_mutex_.lock();
        current_command_ = *(commands.begin());
        if(current_command_.first == current_floor_) {
            EraseCommand(current_floor_);
            setState(LiftState::DoorsOpenned);
        } else {
            if(current_floor_ > current_command_.first)
                setState(LiftState::MovingDown);
            else
                setState(LiftState::MovingUp);
        }
        command_mutex_.unlock();
    }
}

void Lift::MoveUp()
{
    double floor = (double)current_floor_ + speed_ * command_time_;
    if(fabs(floor - current_command_.first) > EPSILON) {
        command_time_ += PERIOD / 1000.0;
        if(fabs(floor - (int)floor) <= EPSILON) {
            std::cout << (int)floor << " floor" << std::endl;
            // Check intermediate stop
            // (commented to skip intermediate stop on moving up as on real lift)
//            auto it = NeedToStop((int)floor);
//            if(it != commands.end()) {
//                current_floor_ = (int)floor;
//                EraseCommand(current_floor_);
//                setState(LiftState::DoorsOpenned);
//            }
        }
    } else {
        current_floor_ = (int)floor;
        std::cout << current_floor_ << " floor" << std::endl;
        EraseCommand(current_floor_);
        setState(LiftState::DoorsOpenned);
    }
}

void Lift::MoveDown()
{
    double floor = (double)current_floor_ - speed_ * command_time_;
    if(fabs(floor - current_command_.first) > EPSILON) {
        command_time_ += PERIOD / 1000.0;
        if(fabs(floor - (int)floor) <= EPSILON) {
            std::cout << (int)floor << " floor" << std::endl;
            // Check intermediate stop (gather all people on moving down)
            auto it = NeedToStop((int)floor);
            if(it != commands.end()) {
                current_floor_ = (int)floor;
                EraseCommand(current_floor_);
                setState(LiftState::DoorsOpenned);
            }
        }
    } else {
        current_floor_ = (int)floor;
        std::cout << current_floor_ << " floor" << std::endl;
        EraseCommand(current_floor_);
        setState(LiftState::DoorsOpenned);
    }
}

void Lift::OpenDoors()
{
    if(fabs(command_time_ - delay_) > EPSILON) {
        if(command_time_ == 0.0) {
            std::cout << "Doors openned" << std::endl;
        }
        command_time_ += PERIOD / 1000.0;
    } else {
        std::cout << "Doors closed" << std::endl;
        setState(LiftState::Stopped);
    }
}

std::set<Command>::iterator Lift::NeedToStop(uint floor)
{
    auto it = commands.find(Command(floor, true));
    if(it != commands.end()) return it;

    it = commands.find(Command(floor, false));
    if(it != commands.end()) return it;

    return commands.end();
}

void Lift::EraseCommand(uint floor)
{
    auto it = commands.find(Command(floor, true));
    if(it != commands.end()) commands.erase(it);

    it = commands.find(Command(floor, false));
    if(it != commands.end()) commands.erase(it);
}
