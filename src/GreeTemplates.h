#ifndef GREETEMPLATES_H
#define GREETEMPLATES_H

namespace GreeControllerLib {

PROGMEM const char SCANNING_STR[] = "{\"t\":\"scan\"}";

PROGMEM const char BINDING_STR[] = "{\"mac\":\"%.12s\",\"t\":\"bind\",\"uid\":0}";

PROGMEM const char ONE_STATUS_STR[] = "{\"cols\":[\"%s\"],\"mac\":\"%s\",\"t\":\"status\"}";
PROGMEM const char REQUEST_STR[] = "{\"cid\":\"app\",\"i\":%c,\"pack\":\"%s\",\"t\":\"pack\",\"tcid\":\"%.12s\",\"uid\":0}";

PROGMEM const char SET_OPTION_STR[] = "{\"t\":\"cmd\",\"opt\":[\"%s\"],\"p\":[%d]}";
PROGMEM const char SET_OPTIONS_STR[] = "{\"t\":\"cmd\",\"opt\":[%s],\"p\":[%s]}";
} // namespace GreeControllerLib
#endif