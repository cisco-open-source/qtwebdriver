#ifndef CISCO_VOLUMECOMMAND_H
#define CISCO_VOLUMECOMMAND_H

#include "commands/webdriver_command.h"
#include "commands/element_commands.h"
#include "base/values.h"
#include "base/bind.h"
#include "commands/response.h"
#include "webdriver_error.h"
#include "webdriver_session.h"
#include "webdriver_view_executor.h"
#include "webdriver_util.h"

namespace base {
class DictionaryValue;
}

namespace webdriver {

class Response;

class CISCO_VolumeCommand : public ElementCommand
{
public:
    CISCO_VolumeCommand(const std::vector<std::string>& path_segments,
                        const base::DictionaryValue* parameters);
    virtual ~CISCO_VolumeCommand();

    virtual bool DoesGet() const OVERRIDE;
    virtual bool DoesPost() const OVERRIDE;
    virtual void ExecuteGet(Response* const response) OVERRIDE;
    virtual void ExecutePost(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(CISCO_VolumeCommand);
};

class CISCO_StateCommand : public ElementCommand
{
public:
    CISCO_StateCommand(const std::vector<std::string>& path_segments,
                        const base::DictionaryValue* parameters);
    virtual ~CISCO_StateCommand();

    virtual bool DoesGet() const OVERRIDE;
    virtual bool DoesPost() const OVERRIDE;
    virtual void ExecuteGet(Response* const response) OVERRIDE;
    virtual void ExecutePost(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(CISCO_StateCommand);
};

class CISCO_SeekCommand : public ElementCommand
{
public:
    CISCO_SeekCommand(const std::vector<std::string>& path_segments,
                        const base::DictionaryValue* parameters);
    virtual ~CISCO_SeekCommand();

    virtual bool DoesGet() const OVERRIDE;
    virtual bool DoesPost() const OVERRIDE;
    virtual void ExecuteGet(Response* const response) OVERRIDE;
    virtual void ExecutePost(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(CISCO_SeekCommand);
};

class CISCO_MuteCommand : public ElementCommand
{
public:
    CISCO_MuteCommand(const std::vector<std::string>& path_segments,
                        const base::DictionaryValue* parameters);
    virtual ~CISCO_MuteCommand();

    virtual bool DoesGet() const OVERRIDE;
    virtual bool DoesPost() const OVERRIDE;
    virtual void ExecuteGet(Response* const response) OVERRIDE;
    virtual void ExecutePost(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(CISCO_MuteCommand);
};


class CISCO_PlaybackSpeedCommand : public ElementCommand
{
public:
    CISCO_PlaybackSpeedCommand(const std::vector<std::string>& path_segments,
                        const base::DictionaryValue* parameters);
    virtual ~CISCO_PlaybackSpeedCommand();

    virtual bool DoesGet() const OVERRIDE;
    virtual bool DoesPost() const OVERRIDE;
    virtual void ExecuteGet(Response* const response) OVERRIDE;
    virtual void ExecutePost(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(CISCO_PlaybackSpeedCommand);
};

}

#endif // CISCO_VOLUMECOMMAND_H
