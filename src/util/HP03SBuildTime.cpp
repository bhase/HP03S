#include "HP03SBuildTime.h"

HP03SBuildTime::HP03SBuildTime()
: dateTime(__DATE__ " " __TIME__)
{
}

HP03SBuildTime::~HP03SBuildTime()
{
}

const char* HP03SBuildTime::GetDateTime()
{
    return dateTime;
}

