#include "GreePacker.h"

using namespace GreeControllerLib;

char* GreePacker::b64_encode(char* buffer, uint16_t length, uint16_t offset) {
	char* data = buffer + offset;

	base64_encodestate _state;
	#ifdef ESP8266
	base64_init_encodestate_nonewlines(&_state);
	#elif LIBRETINY
	base64_init_encodestate(&_state);
	#else
		#error "Unsupported platform"
	#endif
	uint16_t len = base64_encode_block(data, length, buffer, &_state);
	base64_encode_blockend((buffer + len), &_state);

	return buffer;
}

char* GreePacker::b64_decode(const char* data) {
	uint16_t size = base64_decode_expected_len(strlen(data)) + 1;

	char* buf = (char*)malloc(size);
	base64_decode_chars(data, strlen(data), buf);
	return buf;
}

char* GreePacker::pack(const char* key, const char* data) {
	uint16_t size = strlen(data) + 1;
	char pkcs7_padding_length = 16 - (size % 16);
	size += pkcs7_padding_length;

	#ifdef ESP8266
	uint16_t b64_size = base64_encode_expected_len_nonewlines(size) + 1;
	#elif LIBRETINY
	uint16_t b64_size = base64_encode_expected_len(size) + 1;
	#else
		#error "Unsupported platform"
	#endif

	char* buf = (char*)malloc(b64_size);
	memset(buf + b64_size - size + strlen(data), pkcs7_padding_length, pkcs7_padding_length);
	memcpy(buf + b64_size - size, data, strlen(data));

	struct AES_ctx ctx;
	AES_init_ctx(&ctx, (uint8_t*)key);
	for(uint16_t i = b64_size - size; i<b64_size; i+=16) {
		AES_ECB_encrypt(&ctx, (uint8_t*)buf + i);
	}

	b64_encode(buf, size, b64_size - size);

	return buf;
}

char* GreePacker::unpack(const char* key, const char* data, uint16_t len) {
	char* buf = b64_decode(data);
	uint16_t size = base64_decode_expected_len(len);
	size -= size % 16;

	struct AES_ctx ctx;
	AES_init_ctx(&ctx, (uint8_t*)key);
	for(uint16_t i = 0; i<size; i+=16) {
		AES_ECB_decrypt(&ctx, (uint8_t*)buf + i);
	}
	buf[size - buf[size-1]] = 0; // pkcs7 unpad: 0 terminate the string

	return buf;
}