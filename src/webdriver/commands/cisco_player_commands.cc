#include "commands/cisco_player_commands.h"

namespace webdriver {

bool CISCO_VolumeCommand::DoesGet() const
{
    return true;
}

bool CISCO_VolumeCommand::DoesPost() const
{
    return true;
}

void CISCO_VolumeCommand::ExecuteGet(webdriver::Response * const response)
{
    double volume;
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetPlayerVolume,
            base::Unretained(executor_.get()),
            element,
            &volume,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }

    Value* doubleValue = Value::CreateDoubleValue(volume);
    response->SetValue(doubleValue);
}

void CISCO_VolumeCommand::ExecutePost(webdriver::Response * const response)
{
    double volume;
    if (!GetDoubleParameter("volume", &volume)) {
        response->SetError(new Error(
            kBadRequest, "'volume' is missing or invalid"));
        return;
    }

    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::SetPlayerVolume,
            base::Unretained(executor_.get()),
            element,
            volume,
            &error));

    if (error)
        response->SetError(error);

}


CISCO_VolumeCommand::CISCO_VolumeCommand(
        const std::vector<std::string> &path_segments,
        const base::DictionaryValue *parameters)
    : ElementCommand(path_segments, parameters)
{
}

CISCO_VolumeCommand::~CISCO_VolumeCommand()
{
}


bool CISCO_StateCommand::DoesGet() const
{
    return true;
}

bool CISCO_StateCommand::DoesPost() const
{
    return true;
}

void CISCO_StateCommand::ExecuteGet(webdriver::Response * const response)
{
    PlayerState state;
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetPlayerState,
            base::Unretained(executor_.get()),
            element,
            &state,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }
    base::Value* result = base::Value::CreateIntegerValue(state);
    response->SetValue(result);
}

void CISCO_StateCommand::ExecutePost(webdriver::Response * const response)
{
    PlayerState state;
    if (!GetIntegerParameter("state", (int*)&state)) {
        response->SetError(new Error(
            kBadRequest, "'state' is missing or invalid"));
        return;
    }

    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::SetPlayerState,
            base::Unretained(executor_.get()),
            element,
            state,
            &error));

    if (error)
        response->SetError(error);

}


CISCO_StateCommand::CISCO_StateCommand(
        const std::vector<std::string> &path_segments,
        const base::DictionaryValue *parameters)
    : ElementCommand(path_segments, parameters)
{
}

CISCO_StateCommand::~CISCO_StateCommand()
{
}


bool CISCO_SeekCommand::DoesGet() const
{
    return true;
}

bool CISCO_SeekCommand::DoesPost() const
{
    return true;
}

void CISCO_SeekCommand::ExecuteGet(webdriver::Response * const response)
{
    double position;
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetPlayingPosition,
            base::Unretained(executor_.get()),
            element,
            &position,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }

    Value* value = Value::CreateDoubleValue(position);
    response->SetValue(value);
}

void CISCO_SeekCommand::ExecutePost(webdriver::Response * const response)
{
    double position;
    if (!GetDoubleParameter("position", &position)) {
        response->SetError(new Error(
            kBadRequest, "'position' is missing or invalid"));
        return;
    }

    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::SetPlayingPosition,
            base::Unretained(executor_.get()),
            element,
            position,
            &error));

    if (error)
        response->SetError(error);

}


CISCO_SeekCommand::CISCO_SeekCommand(
        const std::vector<std::string> &path_segments,
        const base::DictionaryValue *parameters)
    : ElementCommand(path_segments, parameters)
{
}

CISCO_SeekCommand::~CISCO_SeekCommand()
{
}

CISCO_MuteCommand::CISCO_MuteCommand(
        const std::vector<std::string> &path_segments,
        const base::DictionaryValue *parameters)
        : ElementCommand(path_segments, parameters)
{
}

CISCO_MuteCommand::~CISCO_MuteCommand()
{
}

bool CISCO_MuteCommand::DoesGet() const
{
    return true;
}

bool CISCO_MuteCommand::DoesPost() const
{
    return true;
}

void CISCO_MuteCommand::ExecuteGet(Response * const response)
{
    bool mute;
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetMute,
            base::Unretained(executor_.get()),
            element,
            &mute,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }

    Value* value = Value::CreateBooleanValue(mute);
    response->SetValue(value);
}

void CISCO_MuteCommand::ExecutePost(Response * const response)
{
    bool mute;
    if (!GetBooleanParameter("mute", &mute)) {
        response->SetError(new Error(
            kBadRequest, "'mute' is missing or invalid"));
        return;
    }

    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::SetMute,
            base::Unretained(executor_.get()),
            element,
            mute,
            &error));

    if (error)
        response->SetError(error);
}

CISCO_PlaybackSpeedCommand::CISCO_PlaybackSpeedCommand(
        const std::vector<std::string> &path_segments,
        const base::DictionaryValue *parameters)
        : ElementCommand(path_segments, parameters)
{
}

CISCO_PlaybackSpeedCommand::~CISCO_PlaybackSpeedCommand()
{
}

bool CISCO_PlaybackSpeedCommand::DoesGet() const
{
    return true;
}

bool CISCO_PlaybackSpeedCommand::DoesPost() const
{
    return true;
}

void CISCO_PlaybackSpeedCommand::ExecuteGet(Response * const response)
{
    double speed;
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetPlaybackSpeed,
            base::Unretained(executor_.get()),
            element,
            &speed,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }

    Value* value = Value::CreateDoubleValue(speed);
    response->SetValue(value);
}

void CISCO_PlaybackSpeedCommand::ExecutePost(Response * const response)
{
    double speed;
    if (!GetDoubleParameter("speed", &speed)) {
        response->SetError(new Error(
            kBadRequest, "'speed' is missing or invalid"));
        return;
    }

    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::SetPlaybackSpeed,
            base::Unretained(executor_.get()),
            element,
            speed,
            &error));

    if (error)
        response->SetError(error);
}

}
