#ifndef MHAPWEBDRIVER_H
#define MHAPWEBDRIVER_H

#include <viewfactory.h>

#ifdef __cplusplus
extern "C"
{
#endif

int main_server(int argc, char *argv[]);

#ifdef __cplusplus
}
#endif

template <class T>
void regitsterView(std::string name)
{
    ViewFactory::GetInstance()->add<T>(name);
}

#endif // MHAPWEBDRIVER_H
