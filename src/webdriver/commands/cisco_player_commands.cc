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
    int volume;
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
}

void CISCO_VolumeCommand::ExecutePost(webdriver::Response * const response)
{
    int level;
    if (!GetIntegerParameter("level", &level)) {
        response->SetError(new Error(
            kBadRequest, "'level' is missing or invalid"));
        return;
    }

    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::SetPlayerVolume,
            base::Unretained(executor_.get()),
            element,
            level,
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
}

void CISCO_StateCommand::ExecutePost(webdriver::Response * const response)
{
    PlayerState state;
    if (!GetIntegerParameter("state", (int*)&state)) {
        response->SetError(new Error(
            kBadRequest, "'level' is missing or invalid"));
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
}

void CISCO_SeekCommand::ExecutePost(webdriver::Response * const response)
{
    double position;
    if (!GetDoubleParameter("position", &position)) {
        response->SetError(new Error(
            kBadRequest, "'level' is missing or invalid"));
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

}
