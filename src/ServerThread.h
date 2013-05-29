#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include <QtCore/QThread>

class ServerThread : public QThread
{
  Q_OBJECT

public:
  int   argc;
  char** argv;
  
protected:
  void run();
};

#endif // SERVERTHREAD_H
