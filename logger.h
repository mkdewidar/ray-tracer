#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <string>

extern bool LOGGER_ENABLED = false;

#define LOG(cmds) if (LOGGER_ENABLED) { cmds }

#endif