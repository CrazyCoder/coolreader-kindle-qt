#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void keymap_init(char *path)
{
    char tmpbuf[256] ;


    strcpy(&tmpbuf[0], path) ;
    strcat(&tmpbuf[0], "/kbddrivers/keymaps") ;

    //// printf("result=%s\n", &tmpbuf[0]) ;
}
