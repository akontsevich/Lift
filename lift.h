#ifndef LIFT_H
#define LIFT_H

#include <thread>
#include <mutex>
#include <set>

typedef std::pair<uint, bool> Command;

struct CommCompare {
    bool operator() (const Command& a, const Command& b) const
    {
        return (a.second && !b.second) ||
               (a.second && b.second && a.first < b.first) ||   // go from lowest to higher
               (!a.second && !b.second && a.first > b.first);   // go from higher to lowest
    }
};

class Lift
{
    enum class LiftState {
        MovingUp, MovingDown,
        DoorsOpenned,
        Stopped
    };

public:
    ///
    /// \brief Lift default constructor
    /// \param number floors number
    /// \param height floor height m
    /// \param speed lift speed m/sec
    /// \param delay lift doors open/close time in sec
    ///
    Lift(uint number, double height, double speed, double delay);

    ///
    /// \brief SendCommand sends command to the lift
    /// \param floor floor number lift was called to/from
    /// \param inside command sent inside or outside lift
    ///
    void SendCommand(uint floor, bool inside = false);
    void Exit() { running_ = false; }

private:
    void Work();
    const Command & PopCurrentCommand();
    std::set<Command>::iterator NeedToStop(uint floor);
    void EraseCommand(uint floor);
    void setState(LiftState state);

    /// Action custom functions
    void MoveUp();
    void MoveDown();
    void OpenDoors();
    void Stop();

    /// Lift event cycle period in msec
    const uint period = 250;
    const double epsilon = 0.001;

    void (Lift::*action)() = &Lift::Stop;

    uint number_;
    double height_, speed_, delay_;

    // lift state
    uint current_floor_ = 1;
    LiftState state_ = LiftState::Stopped;           // Lift state
    Command current_command_ = Command(0, false);
    double command_time_ = 0.0;
    bool running_ = true;

    std::set<Command, CommCompare> commands;  // commands queue
    std::mutex command_mutex_;
};

#endif // LIFT_H
