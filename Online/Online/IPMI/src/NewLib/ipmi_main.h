/*
 * ipmi_main.h
 *
 *  Created on: Jan 29, 2016
 *      Author: beat
 */

#ifndef ONLINE_IPMI_SRC_NEWLIB_IPMI_MAIN_H_
#define ONLINE_IPMI_SRC_NEWLIB_IPMI_MAIN_H_

/*
 * Copyright (c) 2003 Sun Microsystems, Inc.  All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistribution of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * Redistribution in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * Neither the name of Sun Microsystems, Inc. or the names of
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * This software is provided "AS IS," without a warranty of any kind.
 * ALL EXPRESS OR IMPLIED CONDITIONS, REPRESENTATIONS AND WARRANTIES,
 * INCLUDING ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE OR NON-INFRINGEMENT, ARE HEREBY EXCLUDED.
 * SUN MICROSYSTEMS, INC. ("SUN") AND ITS LICENSORS SHALL NOT BE LIABLE
 * FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING
 * OR DISTRIBUTING THIS SOFTWARE OR ITS DERIVATIVES.  IN NO EVENT WILL
 * SUN OR ITS LICENSORS BE LIABLE FOR ANY LOST REVENUE, PROFIT OR DATA,
 * OR FOR DIRECT, INDIRECT, SPECIAL, CONSEQUENTIAL, INCIDENTAL OR
 * PUNITIVE DAMAGES, HOWEVER CAUSED AND REGARDLESS OF THE THEORY OF
 * LIABILITY, ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE,
 * EVEN IF SUN HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef ENABLE_ALL_OPTIONS
# define OPTION_STRING  "I:hVvcgsEKYao:H:d:P:f:U:p:C:L:A:t:T:m:z:S:l:b:B:e:k:y:O:R:N:D:"
#else
# define OPTION_STRING  "I:hVvcH:f:U:p:d:S:D:"
#endif

extern int verbose;
extern int csv_output;
extern const struct valstr ipmi_privlvl_vals[];
extern const struct valstr ipmi_authtype_session_vals[];

class ipmi_intf;

class ipmi_main
{
  public:
    ipmi_intf *ipmi_main_intf;
    ipmi_main();
    ~ipmi_main();
    /* ipmi_password_file_read  -  Open file and read password from it
     *
     * @filename:   file name to read from
     *
     * returns pointer to allocated buffer containing password
     *   (caller is expected to free when finished)
     * returns NULL on error
     */
    char *ipmi_password_file_read(char * filename);

    /*
     * Print all the commands in the above table to stderr
     * used for help text on command line and shell
     */
    void ipmi_cmd_print(struct ipmi_cmd * cmdlist);

    /* ipmi_cmd_run - run a command from list based on parameters
     *                called from main()
     *
     *                1. iterate through ipmi_cmd_list matching on name
     *                2. call func() for that command
     *
     * @intf:   ipmi interface
     * @name:   command name
     * @argc:   command argument count
     * @argv:   command argument list
     *
     * returns value from func() of that commnad if found
     * returns -1 if command is not found
     */
    int ipmi_cmd_run( ipmi_intf * intf, char * name, int argc,
        char ** argv);

    void ipmi_option_usage(const char * progname, struct ipmi_cmd * cmdlist,
        struct ipmi_intf_support * intflist);
    /* ipmi_catch_sigint  -  Handle the interrupt signal (Ctrl-C), close the
     *                       interface, and exit ipmitool with error (-1)
     *
     *                       This insures that the IOL session gets freed
     *                       for other callers.
     *
     * returns -1
     */
    void ipmi_catch_sigint();

    /* ipmi_parse_hex - convert hexadecimal numbers to ascii string
     *                  Input string must be composed of two-characer hexadecimal numbers.
     *                  There is no separator between the numbers. Each number results in one character
     *                  of the converted string.
     *
     *                  Example: ipmi_parse_hex("50415353574F5244") returns 'PASSWORD'
     *
     * @param str:  input string. It must contain only even number of '0'-'9','a'-'f' and 'A-F' characters.
     * @returns converted ascii string
     * @returns NULL on error
     */
    unsigned char *ipmi_parse_hex(const char *str);

    /* ipmi_parse_options  -  helper function to handle parsing command line options
     *
     * @argc:   count of options
     * @argv:   list of options
     * @cmdlist:    list of supported commands
     * @intflist:   list of supported interfaces
     *
     * returns 0 on success
     * returns -1 on error
     */
    int i_ipmi_main(int argc, char ** argv, struct ipmi_cmd * cmdlist,
        struct ipmi_intf_support * intflist);

};

#endif /* ONLINE_IPMI_SRC_NEWLIB_IPMI_MAIN_H_ */
