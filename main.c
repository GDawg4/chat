#include <stdio.h>
#include <stdlib.h>
#include "new.pb-c.h"

int main (int argc, const char *argv[])
{
	Chat__UserRegistration msg = CHAT__USER_REGISTRATION__INIT;
	void *buf;
	unsigned len;
	msg.username = "hola";
	msg.ip = "5";
	len = chat__user_registration__get_packed_size(&msg);
	buf = malloc(len);
	chat__user_registration__pack(&msg,buf);
	fwrite(buf,len,1,stdout);
	return 0;
}

