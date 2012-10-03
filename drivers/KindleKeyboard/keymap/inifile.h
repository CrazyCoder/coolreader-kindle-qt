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

#ifndef _INIFILE_H_
#define _INIFILE_H_

#define DEFAULT_SECTION_NAME	"Default"
#define DEFAULT_EMPTY_STRING	"<empty>"

// DB elements

// The data structures

struct key_row ;

typedef struct
{
	char *psname ;				/* pointer to section name */
	struct key_row *ppairs ;    /* pointer to the beginning of key/value ptr array for a section */
	int npairs ;                /* number of key/value pairs for a section */
} section_row_t ;

typedef struct key_row
{
	char *pkname ;                	/* pointer to key name */
	char *pkvalue ;					/* pointer to key value */
	section_row_t *psect ;			/* pointer to the container section */
} key_row_t ;


typedef struct
{
	int fd ;
	char *pbuf ;                         /* pointer to the ini-file buffer */
	int bufsize ;                        /* size of ini-file buffer */
	section_row_t	*psections ;         /* pointer to the beginning of section name ptr arrays */
	int	nsections ;                      /* number of sections */
	key_row_t		*pkeys ;         	 /* pointer to the beginning of key pair definitions */
	int	nkeys ;		                     /* number of keys defined in file */
} dbdesc_t ;


/*
* Database handling functions
*/

dbdesc_t * inidb_open(char *path) ;
void inidb_close(dbdesc_t *pdb) ;
void inidb_clear(dbdesc_t *pdb) ;
int inidb_get_number_of_sections(dbdesc_t *pdb) ;

int inidb_get_section_name( dbdesc_t *pdb, int idx, char *psn) ;
int inidb_get_number_of_keys(dbdesc_t *pdb, int idx) ;
int inidb_get_key_name(dbdesc_t *pdb, int nSectIndex, int nStrIndex, char *pKeyName) ;
int inidb_get_key_value(dbdesc_t *pdb, int sidx, int kidx, char *pval) ;

int inidb_find_section(dbdesc_t *pdb, char *pname) ;
int inidb_find_key(dbdesc_t *pdb, int sidx, char *pname) ;

int inidb_get_section_name_length(dbdesc_t *pdb, int sidx) ;
int inidb_get_key_value_length(dbdesc_t *pdb, int sidx, int kidx) ;
int inidb_get_key_name_length(dbdesc_t *pdb, int sidx, int kidx) ;




#endif

