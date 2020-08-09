#ifndef GREEPACKER_H
#define GREEPACKER_h

#include <Arduino.h>

extern "C" {
	#include "libb64/cdecode.h"
	#include "libb64/cencode.h"
	#include "aes.h"
}

namespace GreeControllerLib {

class GreePacker
{
	private:
		static char* b64_encode(const char* data, size_t length);
		static char* b64_decode(const char* data);
	public:
		static char* pack(const char* key, const char* data);
		static char* unpack(const char* key, const char* data, uint16_t len);
};

} // namespace GreeControllerLib
#endif