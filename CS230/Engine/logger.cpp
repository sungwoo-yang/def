/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Logger.cpp
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    March 6, 2025
*/

#include <iostream>
#include "Logger.h"
#include <raylib.h>

CS230::Logger::Logger(Logger::Severity severity, bool use_console, std::chrono::system_clock::time_point engine_start_time) 
    : min_level(severity), out_stream("Trace.log"), start_time(engine_start_time) {
    SetTraceLogLevel(LOG_NONE);
    if (use_console == true) {
        out_stream.set_rdbuf(std::cout.rdbuf());
    }
}

CS230::Logger::~Logger() {
    out_stream.flush();
    out_stream.close();
}

void CS230::Logger::log(CS230::Logger::Severity severity, std::string message) {

    std::string SeverityNames[] = { "Verbose", "Debug", "Event", "Error" };

    if (severity >= min_level && static_cast<int>(severity) > 0 && static_cast<int>(severity) < 4)
    {
        out_stream.precision(4);
        out_stream << '[' << std::fixed << seconds_since_start() << "]\t";


        out_stream << SeverityNames[static_cast<int>(severity)] << "\t" << message << std::endl;
    }

    return;
}

double CS230::Logger::seconds_since_start() const
{
    return std::chrono::duration<double>(std::chrono::system_clock::now() - start_time).count();
}