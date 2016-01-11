/* Copyright (c) 2011. All rights reserved. ERG Transit Systems (HK) Ltd.
**
** cmd.h - general io commands
**
** Version Date    Name       Reason
** 1.0     12/12/11  Gerry       Created
**
*/


#ifndef __cmd_h__
#define __cmd_h__

#define CMD_LEN 240
#define MAGIC1  0x5a5a5a5a
#define MAGIC2  0xa5a5a5a5

typedef struct
{
    unsigned magic1;
    int type;
    int length;
    char buffer[CMD_LEN];
    unsigned magic2;
} cmd_t;

void write_cmd(int type, char *data, int len);
int read_cmd(cmd_t *cmd);


#endif

/* end of file */
