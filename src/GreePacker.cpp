#include "GreePacker.h"

using namespace GreeControllerLib;

char* GreePacker::b64_encode(const char* data, size_t length) {
	size_t size = base64_encode_expected_len_nonewlines(length);
	char* buffer = (char*) malloc(size);
	base64_encodestate _state;
	base64_init_encodestate_nonewlines(&_state);
	int len = base64_encode_block(data, length, buffer, &_state);
	base64_encode_blockend((buffer + len), &_state);
	return buffer;
}

char* GreePacker::b64_decode(const char* data) {  
	size_t size = base64_decode_expected_len(strlen(data));

	char* buf = (char*)malloc(size);
	base64_decode_chars(data, strlen(data), buf);
	return buf;
}

char* GreePacker::pack(const char* key, const char* data) {
	size_t size = strlen(data) + 1;
	char pkcs7_padding_length = 16 - (size % 16);
	size += pkcs7_padding_length;

	char* buf = (char*)malloc(size);
	memset(buf + strlen(data), pkcs7_padding_length, pkcs7_padding_length);
	memcpy(buf, data, strlen(data));

	struct AES_ctx ctx;
	AES_init_ctx(&ctx, (uint8_t*)key);
	for(int i = 0; i<size; i+=16) {
		AES_ECB_encrypt(&ctx, (uint8_t*)buf + i);
	}

	char* packed = b64_encode(buf, size);
	free(buf);

	return packed;
}

char* GreePacker::unpack(const char* key, const char* data, uint16_t len) {
	char* buf = b64_decode(data);
	size_t size = base64_decode_expected_len(len);
	size -= size % 16;

	struct AES_ctx ctx;
	AES_init_ctx(&ctx, (uint8_t*)key);
	for(int i = 0; i<size; i+=16) {
		AES_ECB_decrypt(&ctx, (uint8_t*)buf + i);
	}
	buf[size - buf[size-1]] = 0; // pkcs7 unpad: 0 terminate the string

	return buf;
}