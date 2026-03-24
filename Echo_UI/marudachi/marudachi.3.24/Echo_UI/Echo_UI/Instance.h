#pragma once

#include "UART.h"
#include "command.h"
#include "echo.h"
#include "button.h"
#include "UI.h"
#include "sound.h"

extern UI ui;
extern Echo echo;
extern Button button;    
extern Sound sound;

extern UART uart;
extern Data data;
extern Command command;