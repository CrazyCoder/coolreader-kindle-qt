/*
* Copyright (C) 2010 Andy M. aka h1uke	h1ukeguy @ gmail.com
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef _KEYMAP_H_
#define _KEYMAP_H_

/* there can be up to MAXKEY keys
 * each generating a code in range of 0..MAXCODE
 */
#define MAXKEYS	96
#define MAXCODE 255

#define MAXPATH 256
#define MAXNAME 16

/* longest hotkey sequence supported */
#define MAXSEQ	8

struct input_key
{
	int input_key_code ;
	char input_key_name[MAXNAME] ;
} ;

struct input_key_alias
{
	int input_key_code ;
	int substitute_code ;
} ;

struct action
{
	int type ;
	int len ;	/* number of key codes in sequence */
	short sequence[MAXSEQ] ;
	char command[MAXPATH] ;
	char dispname[MAXNAME] ;
};


int inroducer_key_code(void) ;
int trailer_key_code(void) ;
int hot_interval_msec(void) ;
int key_delay_msec(void) ;
char * input_key_name(int code) ;

struct action *find_sequence_action(short *pseq, int len) ;
int execute_action(struct action *pact, int fd_kbd, int fd_fw ) ;

//extern void msdelay(int) ;

extern "C" {

void keymap_init(char *path) ;

}
#endif

