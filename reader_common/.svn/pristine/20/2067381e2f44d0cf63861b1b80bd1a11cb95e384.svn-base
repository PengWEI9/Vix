/* Copyright (c) 2011. All rights reserved. ERG Transit Systems (HK) Ltd.
**
** cmd.cpp - general io commands
**
** Version Date    Name       Reason
** 1.0     12/12/11  Gerry       Created
**
*/

#include <locale.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <cmd.h>

static char *fifo_name()
{
    static char buffer[40];
    uid_t uid;

    uid = getuid();
    sprintf(buffer, "/tmp/cmdfifo.%d", uid);

    return buffer;
}

char *make_fifo()
{
    struct stat buf;
    int res;
    char *buffer = fifo_name();

    res = stat(buffer, &buf);
    if (res < 0 && errno == ENOENT)
    {
        printf("creating fifo %s\n", buffer);
        mkfifo(buffer, S_IRUSR|S_IWUSR);
    }
    return buffer;
}

void write_cmd(int type, char *data, int len)
{
    static int fh = -1;
    cmd_t cmd;

    if (fh == -1)
    {
        fh = open(make_fifo(), O_WRONLY);
        if (fh < 0)
        {
            printf("fifo creation/open failed\n");
            return;
        }
    }

    memset(&cmd, 0, sizeof(cmd_t));

    cmd.magic1 = MAGIC1;
    cmd.magic2 = MAGIC2;
    cmd.type = type;
    len =  (len > CMD_LEN) ? CMD_LEN : len;
    cmd.length = len;
    memcpy(cmd.buffer, data, len);
    write(fh, &cmd, sizeof(cmd_t));
    fdatasync(fh);
}

int read_cmd(cmd_t *cmd)
{
    static int fh = -1;
    int res;
//  int magic1, magic2;

    if (fh == -1)
    {
        fh = open(make_fifo(), O_RDONLY);
        if (fh < 0)
        {
            printf("fifo %s doesn't exist yet\n", fifo_name());
            return -1;
        }
    }

    res = read(fh, cmd, sizeof(cmd_t));
    if (res == sizeof(cmd_t))
    {
        /* read the right amount display it */
        if (cmd->magic1 != MAGIC1 || cmd->magic2 != MAGIC2)
        {
            printf("ignored garbage\n");
        }
        else
        {
            // got a structure
            return 1;
        }
    }
    return 0;
}

/* end of file */

