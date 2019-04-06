 #include <stdio.h>
          #include <regex.h>
          #include <stdlib.h>
          
          void match_print(const char *buf, int start, int end)
          {
                  int i,j;
                  fprintf(stderr,"|");
                  for( i=0;i<end;i++ )
                          if( i >= start )
                                  fprintf(stderr,"%c", buf[i] );
                  fprintf(stderr,"|\n");
          }
          
          int main()
          {
                  int i;
                  int result;
                  int start=0, end=0;
                  char *str = "A WikiWikiWeb is a collaborative hypertext environment, with an emphasis on easy access to and modification of information.";
                  regex_t *compiled;
                  regmatch_t *matchptr;
                  size_t nmatch;
          
                  if( (compiled = (regex_t*)malloc(sizeof(regex_t))) == NULL ) {
                          fprintf(stderr, "regex_t malloc error\n" );
                          exit(-1);
                  }
          
                  if( regcomp( compiled, "[^ ]*t", REG_EXTENDED | REG_ICASE ) != 0 ) {
                          fprintf(stderr, "regcomp error\n" );
                          exit(-1);
                  }
          
                  nmatch = compiled->re_nsub+1;
          
                  if( (matchptr = (regmatch_t*)malloc(sizeof(regmatch_t)*nmatch)) == NULL ) {
                          fprintf(stderr, "regmatch_t malloc error\n" );
                          exit(-1);
                  }
          
                  while( (result = regexec( compiled, str+start, nmatch, matchptr, 0)) == 0 ) {
                          match_print( str, start+matchptr->rm_so, start+matchptr->rm_eo );
                          start += matchptr->rm_eo;
                  }
          
                  regfree( compiled );
          }

