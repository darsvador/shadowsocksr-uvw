/*
 * utils.c - Misc utilities
 *
 * Copyright (C) 2013 - 2016, Max Lv <max.c.lv@gmail.com>
 *
 * This file is part of the shadowsocks-libev.
 *
 * shadowsocks-libev is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * shadowsocks-libev is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with shadowsocks-libev; see the file COPYING. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "utils.h"

#ifdef HAVE_SETRLIMIT
#include <sys/time.h>
#include <sys/resource.h>
#endif

#define INT_DIGITS 19           /* enough for 64 bit integer */



int use_tty = 1;

char *
ss_itoa(int i)
{
    /* Room for INT_DIGITS digits, - and '\0' */
    static char buf[INT_DIGITS + 2];
    char *p = buf + INT_DIGITS + 1;     /* points to terminating '\0' */
    if (i >= 0) {
        do {
            *--p = '0' + (i % 10);
            i   /= 10;
        } while (i != 0);
        return p;
    } else {                     /* i < 0 */
        do {
            *--p = '0' - (i % 10);
            i   /= 10;
        } while (i != 0);
        *--p = '-';
    }
    return p;
}

int
ss_isnumeric(const char *s) {
    if (!s || !*s)
        return 0;
    while (isdigit(*s))
        ++s;
    return *s == '\0';
}

char *
ss_strndup(const char *s, size_t n)
{
    size_t len = strlen(s);
    char *ret;

    if (len <= n) {
        return strdup(s);
    }

    ret = ss_malloc(n + 1);
    strncpy(ret, s, n);
    ret[n] = '\0';
    return ret;
}

char *
ss_strdup(const char *s) {
    if (!s) {
        return NULL;
    }

    return strdup(s);
}

void
FATAL(const char *msg)
{
    LOGE("%s", msg);
    exit(-1);
}

void *
ss_malloc(size_t size)
{
    void *tmp = malloc(size);
    if (tmp == NULL)
        exit(EXIT_FAILURE);
    return tmp;
}

void *
ss_realloc(void *ptr, size_t new_size)
{
    void *new = realloc(ptr, new_size);
    if (new == NULL) {
        free(ptr);
        ptr = NULL;
        exit(EXIT_FAILURE);
    }
    return new;
}

void
usage()
{
    printf("\n");
    printf("shadowsocks-libev %s with %s\n\n", VERSION, USING_CRYPTO);
    printf(
        "  maintained by Max Lv <max.c.lv@gmail.com> and Linus Yang <laokongzi@gmail.com>\n\n");
    printf("  usage:\n\n");
#ifdef MODULE_LOCAL
    printf("    ss-local\n");
#elif MODULE_REMOTE
    printf("    ss-server\n");
#elif MODULE_TUNNEL
    printf("    ss-tunnel\n");
#elif MODULE_REDIR
    printf("    ss-redir\n");
#elif MODULE_MANAGER
    printf("    ss-manager\n");
#endif
    printf("\n");
    printf(
        "       -s <server_host>           Host name or IP address of your remote server.\n");
    printf(
        "       -p <server_port>           Port number of your remote server.\n");
    printf(
        "       -l <local_port>            Port number of your local server.\n");
    printf(
        "       -k <password>              Password of your remote server.\n");
    printf(
        "       -m <encrypt_method>        Encrypt method: table, rc4, rc4-md5,\n");
    printf(
        "                                  aes-128-cfb, aes-192-cfb, aes-256-cfb,\n");
    printf(
        "                                  aes-128-ctr, aes-192-ctr, aes-256-ctr,\n");
    printf(
        "                                  bf-cfb, camellia-128-cfb, camellia-192-cfb,\n");
    printf(
        "                                  camellia-256-cfb, cast5-cfb, des-cfb,\n");
    printf(
        "                                  idea-cfb, rc2-cfb, seed-cfb, salsa20,\n");
    printf(
        "                                  chacha20 and chacha20-ietf.\n");
    printf(
        "                                  The default cipher is rc4-md5.\n");
    printf("\n");
    printf(
        "       [-a <user>]                Run as another user.\n");
    printf(
        "       [-f <pid_file>]            The file path to store pid.\n");
    printf(
        "       [-t <timeout>]             Socket timeout in seconds.\n");
    printf(
        "       [-c <config_file>]         The path to config file.\n");
#ifdef HAVE_SETRLIMIT
    printf(
        "       [-n <number>]              Max number of open files.\n");
#endif
#ifndef MODULE_REDIR
    printf(
        "       [-i <interface>]           Network interface to bind.\n");
#endif
    printf(
        "       [-b <local_address>]       Local address to bind.\n");
    printf("\n");
    printf(
        "       [-u]                       Enable UDP relay.\n");
#ifdef MODULE_REDIR
    printf(
        "                                  TPROXY is required in redir mode.\n");
#endif
    printf(
        "       [-U]                       Enable UDP relay and disable TCP relay.\n");
#ifdef MODULE_REMOTE
    printf(
        "       [-6]                       Resovle hostname to IPv6 address first.\n");
#endif
    printf("\n");
#ifdef MODULE_TUNNEL
    printf(
        "       [-L <addr>:<port>]         Destination server address and port\n");
    printf(
        "                                  for local port forwarding.\n");
#endif
#ifdef MODULE_REMOTE
    printf(
        "       [-d <addr>]                Name servers for internal DNS resolver.\n");
#endif
#if defined(MODULE_REMOTE) || defined(MODULE_LOCAL)
    printf(
        "       [--fast-open]              Enable TCP fast open.\n");
    printf(
        "                                  with Linux kernel > 3.7.0.\n");
    printf(
        "       [--acl <acl_file>]         Path to ACL (Access Control List).\n");
#endif
#if defined(MODULE_REMOTE) || defined(MODULE_MANAGER)
    printf(
        "       [--manager-address <addr>] UNIX domain socket address.\n");
#endif
#ifdef MODULE_MANAGER
    printf(
        "       [--executable <path>]      Path to the executable of ss-server.\n");
#endif
    printf(
        "       [--mtu <MTU>]              MTU of your network interface.\n");
#ifdef __linux__
    printf(
        "       [--mptcp]                  Enable Multipath TCP on MPTCP Kernel.\n");
#ifdef MODULE_REMOTE
    printf(
        "       [--firewall]               Setup firewall rules for auto blocking.\n");
#endif
#endif
    printf("\n");
    printf(
        "       [-v]                       Verbose mode.\n");
    printf(
        "       [-h, --help]               Print this message.\n");
    printf("\n");
}

#ifdef HAVE_SETRLIMIT
int
set_nofile(int nofile)
{
    struct rlimit limit = { nofile, nofile }; /* set both soft and hard limit */

    if (nofile <= 0) {
        FATAL("nofile must be greater than 0\n");
    }

    if (setrlimit(RLIMIT_NOFILE, &limit) < 0) {
        if (errno == EPERM) {
            LOGE(
                "insufficient permission to change NOFILE, not starting as root?");
            return -1;
        } else if (errno == EINVAL) {
            LOGE("invalid nofile, decrease nofile and try again");
            return -1;
        } else {
            LOGE("setrlimit failed: %s", strerror(errno));
            return -1;
        }
    }

    return 0;
}

#endif