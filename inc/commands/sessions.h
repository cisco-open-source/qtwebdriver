#ifndef SESSIONS_H
#define SESSIONS_H

#include "commands/command.h"

namespace webdriver {

class Response;

class Sessions : public Command
{
public:
    Sessions(const std::vector<std::string>& path_segments,
             const DictionaryValue* const parameters);
	virtual ~Sessions();

	virtual bool DoesGet() const OVERRIDE;

	virtual void ExecuteGet(Response* const response) OVERRIDE;

private:
  	DISALLOW_COPY_AND_ASSIGN(Sessions);
};

}

#endif // SESSIONS_H
