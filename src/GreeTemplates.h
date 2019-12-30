#ifndef GREETEMPLATES_H
#define GREETEMPLATES_H

namespace GreeControllerLib {

PROGMEM const char SCANNING_STR[] = "{\"t\":\"scan\"}";

PROGMEM const char BINDING_STR[] = "{\"mac\":\"%s\",\"t\":\"bind\",\"uid\":0}";
PROGMEM const char BINDING_REQUEST_STR[] = "{\"cid\":\"app\",\"i\":1,\"pack\":\"%s\",\"t\":\"pack\",\"tcid\":\"%s\",\"uid\":0}";

PROGMEM const char ONE_STATUS_STR[] = "{\"cols\":[\"%s\"],\"mac\":\"%s\",\"t\":\"status\"}";
PROGMEM const char STATUS_STR[] = "{\"cols\":[\"Pow\",\"Mod\",\"SetTem\",\"WdSpd\",\"Air\",\"Blo\",\"Health\",\"SwhSlp\",\"Lig\",\"SwingLfRig\",\"SwUpDn\",\"Quiet\",\"Tur\",\"StHt\",\"TemUn\",\"HeatCoolType\",\"TemRec\",\"SvSt\"],\"mac\":\"%s\",\"t\":\"status\"}";
PROGMEM const char STATUS_REQUEST_STR[] = "{\"cid\":\"app\",\"i\":0,\"pack\":\"%s\",\"t\":\"pack\",\"tcid\":\"%s\",\"uid\":0}";

PROGMEM const char SET_OPTION_STR[] = "{\"t\":\"cmd\",\"opt\":[\"%s\"],\"p\":[%d]}";
PROGMEM const char SET_OPTIONS_STR[] = "{\"t\":\"cmd\",\"opt\":[%s],\"p\":[%s]}";
} // namespace GreeControllerLib
#endif