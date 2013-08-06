#ifndef WEBDRIVER_CISCO_PLAYER_COMMANDS_INTERFACE_H
#define WEBDRIVER_CISCO_PLAYER_COMMANDS_INTERFACE_H

namespace webdriver {

class Error;

class CiscoPlayerCommandsInterface
{
public:
    enum PlayerState{
        Stopped = 0,
        Playing,
        Paused
    };

    virtual void GetPlayerState(PlayerState *state, Error** error) const = 0;
    virtual void SetPlayerState(const PlayerState state, Error** error) = 0;
    virtual void GetPlayerVolume(int *level, Error** error) const = 0;
    virtual void SetPlayerVolume(const int level, Error** error) = 0;
    virtual void GetPlayingPosition(double* reletivePos, Error** error) const =0;
    virtual void SetPlayingPosition(const double reletivePos, Error** error) = 0;

    virtual ~CiscoPlayerCommandsInterface() {}
};

}

#endif // WEBDRIVER_CISCO_PLAYER_COMMANDS_INTERFACE_H
