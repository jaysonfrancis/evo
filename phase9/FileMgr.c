// FileMgr.c
// simple "memory-based" file system with preloaded objects
// as "/www/" and "/bin/"

#include "spede.h"
#include "type.h"
#include "syscall.h"
#include "tool.h"    // need MyStrlen, MyMemcpy, etc.
#include "FileMgr.h" // types and data defined/declared here

char help_txt_data[] = {
   'I', '\'', 'm', ' ', 's', 'o', 'r', 'r', 'y', ',', ' ', 'm', 'y', ' ',
   'c', 'a', 'p', 'a', 'c', 'i', 't', 'y', ' ', 't', 'o', ' ', 'a', 'n',
   's', 'w', 'e', 'r', 'i', 'n', 'g', ' ', 'q', 'u', 'e', 's', 't', 'i',
   'o', 'n', 's', ' ', 'i', 's', ' ', 'l', 'i', 'm', 'i', 't', 'e', 'd',
   '.', ' ', 'Y', 'o', 'u', ' ', 'w', 'o', 'u', 'l', 'd', ' ', 'h', 'a',
   'v', 'e', ' ', 't', 'o', ' ', 'a', 's', 'k', ' ', 'a', ' ', 'r', 'i',
   'g', 'h', 't', ' ', 'q', 'u', 'e', 's', 't', 'i', 'o', 'n', '.', '\n',
   '\0'
};
#define HELP_TXT_SIZE ( sizeof( help_txt_data ) )

char note_txt_data[] = {
   'A', ' ', 'f', 'a', 'i', 'l', 'u', 'r', 'e', ' ', 'i', 's', ' ', 'o',
   'f', 't', 'e', 'n', ' ', 'p', 'r', 'e', 'c', 'u', 'r', 's', 'o', 'r',
   'e', 'd', ' ', 'b', 'y', ' ', 'e', 'x', 'c', 'u', 's', 'e', 's', ',',
   ' ', 's', 'u', 'c', 'c', 'e', 's', 's', ' ', 'n', 'o', 'n', 'e', '.',
   '\n', '\0'
};
#define NOTE_TXT_SIZE ( sizeof( note_txt_data ) )

// C compiler joins lines below and adds '\0' at the end (not for char arrays above).
char index_html_data[] = {
   "<HTML>\n"
   "<HEAD>\n"
   "<TITLE>159ers' Do's and Don'ts</TITLE>\n"
   "</HEAD>\n"
   "<BODY>\n"
   "The few, the pround, semper fidelis!\n"
   "Failure's best friends: excuses; Success', lone silent works.\n"
   "When given freedom/privileges/kindness, treasure it, not abuse it.\n"
   "Be able to identify curcial knowledge and attent to its details timely, and "
   "waste no time on blame games.\n"
//   "Don't be a pathological social engineer:\n"
//   "If given an inch, claim a mile.\n"
//   "Be intolerable to others' mistakes, forgiving to own.\n"
//   "Quick to nitpick, slow to praise. Quick to judge, slow to change.\n"
//   "Fraternize to get close, identify weaknesses, then abuse with it.\n"
//   "Remember not it, but the intricated details on how to complain about it.\n"
//   "Others are to blame for own failures. High standards for others, low for self.\n"
//   "Accuse heavily on trivial errors, raise guilty conscience, see a soft belly, attack!\n"
   "</BODY></HTML>\n"
};
#define INDEX_HTML_SIZE ( sizeof( index_html_data ) ) // null is added+counted

char hello_html_data[] = {
   "<HTML><HEAD>\n"
   "<TITLE>Monkeys and Leopard</TITLE>\n"
   "</HEAD>\n"
   "<BODY BgColor=#FFFFFF>"
   "Monkeys in the jungle, tauting and mobbing a leopard passing by. Yelps and smugs. "
   "The leopard hisses -- a warning, just once. The monkeys acerbate intimidations. "
   "The leopard strikes, lightningly fast. Monkeys disperse. Some slain or mauled.\n"
   "Primemates live in each others' eyes, testy social settings; a lone leopard, no one's. "
   "Be a leopard at work, the one that gets things done; not spewing monkey business -- "
   "disruptions of arguments and blames over self inabilities and social manipulations.\n"
   "Evolve!\n"
   "</BODY></HTML>\n"
};
#define HELLO_HTML_SIZE ( sizeof( hello_html_data ) ) // null is added+counted

// We'll define "root_dir[]" later. Here is a forward declare.
extern dir_t root_dir[];                         // prototype it in advance

char MySleep_data[]= {
   #include "bin/MySleep.x"   // this must be a separate line
};
#define MYSLEEP_SIZE (sizeof(MySleep_data))

char MyHello_data[]= {
   #include "bin/MyHello.x"   // this must be a separate line
};
#define MYHELLO_SIZE (sizeof(MyHello_data))


dir_t bin_dir[] = {
   { 16, MODE_DIR, ~0, ".", (char *)bin_dir },   // current dir
   { 17, MODE_DIR, ~0, "..", (char *)root_dir }, // parent dir, forward declared
   { 18, MODE_EXEC, MYSLEEP_SIZE, "MySleep", (char *)MySleep_data }, // MySleep here
   { 19, MODE_EXEC, MYSLEEP_SIZE, "8", (char *)MySleep_data }, 
   { 20, MODE_EXEC, MYHELLO_SIZE, "MyHello", (char *)MyHello_data }, // MySleep here
   { 21, MODE_EXEC, MYHELLO_SIZE, "9", (char *)MyHello_data },
   {  0, 0, 0, NULL, NULL },                      // no entries in dir
   { END_DIR_INODE, 0, 0, NULL, NULL }           // end of bin_dir[]
};

dir_t www_dir[] = {
   { 10, MODE_DIR, ~0, ".", (char *)www_dir },
   { 11, MODE_DIR, ~0, "..", (char *)root_dir },
   { 12, MODE_FILE, HELLO_HTML_SIZE, "5", (char *)hello_html_data },
   { 13, MODE_FILE, INDEX_HTML_SIZE, "6", (char *)index_html_data },
   { 14, MODE_FILE, HELLO_HTML_SIZE, "hello.html", (char *)hello_html_data },
   { 15, MODE_FILE, INDEX_HTML_SIZE, "index.html", (char *)index_html_data },
   {  0, 0, 0, NULL, NULL },          
   { END_DIR_INODE, 0, 0, NULL, NULL }
};

dir_t root_dir[] = {
   { 1, MODE_DIR, ~0, ".", (char *)root_dir },
   { 2, MODE_DIR, sizeof(bin_dir), "1", (char *)bin_dir },
   { 3, MODE_DIR, sizeof(www_dir), "2", (char *)www_dir },
   { 4, MODE_FILE, HELP_TXT_SIZE, "3", (char *)help_txt_data },
   { 5, MODE_FILE, NOTE_TXT_SIZE, "4", (char *)note_txt_data },
   { 6, MODE_DIR, sizeof(bin_dir), "bin", (char *)bin_dir },
   { 7, MODE_DIR, sizeof(www_dir), "www", (char *)www_dir },
   { 8, MODE_FILE, HELP_TXT_SIZE, "help.txt", (char *)help_txt_data },
   { 9, MODE_FILE, NOTE_TXT_SIZE, "note.txt", (char *)note_txt_data },
   { 0, 0, 0, NULL, NULL },
   { END_DIR_INODE, 0, 0, NULL, NULL }
};

// *********************************************************************
fd_t fd_array[MAX_FD];  // one file descriptor for every OPEN_OBJ call
// *********************************************************************

// *********************************************************************
// File Mgr, a process to answer queries and return file contents
// via message passing
// *********************************************************************
void FileMgr() {
   int SHELL,     // the shell that's asking FileMgr for service
       result,    // the result to be included to return to shell
       i;

   msg_t msg;

// after all the directory entries have been assigned, then fill in the size
// of this "directory". _dir[1] is ".." which must point to parent dir
   root_dir[0].size = sizeof( root_dir );

   bin_dir[0].size = sizeof( bin_dir );
   bin_dir[1].size = root_dir[0].size;

   www_dir[0].size = sizeof( www_dir );
   www_dir[1].size = root_dir[0].size;

// mark all file descriptors as available, make sure UNUSED in FileMgr.h
   for(i=0; i<MAX_FD; i++) fd_array[i].owner = UNUSED;

   while(1) {                    // start serving requests for shells
      MsgRcv( &msg );

      SHELL = msg.sender;        // shell's pid to return query results
      
      switch( msg.code ) {       // depending on what's requested
// CHK_OBJ: Shell wants to "check" obj (name in msg.data),
// and attr of it will be returned via msg in msg.data as well
         case CHK_OBJ:
            result = ChkObj( msg.data, (attr_t *)msg.data );
            break;

// OPEN_OBJ: Shell wants to open obj, an FD will be returned
// in msg.number[1], the ownership (SHELL) will be recorded
         case OPEN_OBJ:
            result = OpenObj( msg.data, SHELL, &msg.number[1] );
            break;

// READ_OBJ: Shell wants content of obj FD (msg.number[1])
// return (msg.data) and # of bytes read (msg.number[2])
         case READ_OBJ:
            result = ReadObj( msg.number[1], msg.data, SHELL, &msg.number[2] );
            break;

// CLOSE_OBJ: Shell wants to close FD (msg.number[1])
// check if the owner of FD is Shell (SHELL)
         case CLOSE_OBJ:
            result = CloseObj( msg.number[1], SHELL );
            break;

         default: // unknown code received
            result = UNKNOWN;
            cons_printf( "FileMgr: Bad code %d in message from PID #%d\n",
                         msg.code, SHELL );
      }

      msg.recipient = SHELL; // reply msg to shell
      msg.code = result;
      MsgSnd( &msg );
   }
}

// get info about a specific obj. if not found, returns an error code.
int ChkObj( char *name, attr_t *attr_p ) {
   dir_t *dir_p = FindName( name );

   if( ! dir_p ) return UNFOUND;

   Dir2Attr( dir_p, attr_p ); // copy what dir_p points to to where attr_p points to

// this should be included to pass the filename (add 1 to length for null)
   MyMemcpy( (char *)( attr_p + 1 ), dir_p->name, MyStrlen( dir_p->name ) + 1 );

   return GOOD;
}

// access file or directory, returns file descriptor to use for
// READ_OBJ calls, or error code if fails
int OpenObj( char *name, int owner, int *fd_p ) {
   int fd;
   dir_t *dir_p;

   fd = AllocFD( owner );

   if( fd == NO_FD ) {
      cons_printf( "FileMgr: no more available File Descriptor!\n" );
      return NO_FD;
   }

   dir_p = FindName( name );
   if( ! dir_p ) return UNFOUND;
   *fd_p = fd;           // fd to return
   fd_array[fd].item = dir_p; // dir_p is the name

   return GOOD;
}

int CloseObj( int fd, int owner ) {
   int result;

   result = CanAccessFD( fd, owner ); // check if owner owns FD
   if( result == GOOD ) FreeFD( fd );

   return result;
}

// Copy bytes from file into user's buffer. Returns actual count of bytes
// transferred. Read from fd_array[fd].offset (initially given 0) for
// buff_size in bytes, and record the offset. may reach FM_EOF though...
int ReadObj( int fd, char *buff, int owner, int *lp_actual ) {
   int result;
   int remaining;
   dir_t *lp_dir;

   *lp_actual=0;

   result = CanAccessFD( fd, owner ); // check if owner owns the fd

   if( result != GOOD ) return result;

   lp_dir = fd_array[fd].item;

   if( A_ISDIR(lp_dir->mode ) ) {  // it's a dir
// if reading directory, return attr_t structure followed by an obj name.
// a chunk returned per read. `offset' is index into root_dir[] table.
      dir_t *this_dir = lp_dir;
      attr_t *attr_p = (attr_t *)buff;
      dir_t *dir_p;

      if( 101 < sizeof( *attr_p ) + 2) return BUF_SMALL;

// use current dir, advance to next dir for next time when called
      do {
         dir_p = ( (dir_t *)this_dir->data );
         dir_p += fd_array[fd].offset ;

         if( dir_p->inode == END_DIR_INODE ) return FM_EOF;

         fd_array[fd].offset++;   // advance

      } while( dir_p->name == NULL );

// MyBzero() fills buff with 0's, necessary to clean buff
// since Dir2Attr may not completely overwrite whole buff...
      MyBZero( buff, 101 );
      Dir2Attr( dir_p, attr_p );

// copy obj name after attr_t, add 1 to length for null
      MyMemcpy((char *)( attr_p + 1 ), dir_p->name, MyStrlen( dir_p->name ) + 1);

//     *lp_actual = sizeof(*dir_p) + MyStrlen((char *)(attr_p + 1)) + 1;
      *lp_actual = sizeof( attr_t ) + MyStrlen( dir_p->name ) + 1;

   } else {  // a file, not dir
// compute max # of bytes can transfer then MyMemcpy()
      remaining = lp_dir->size - fd_array[fd].offset;

      if( remaining == 0 ) return FM_EOF;

      MyBZero( buff, 101 );  // null termination for any part of file read

      result = remaining<100?remaining:100; // -1 saving is for last NULL

      MyMemcpy( buff, &lp_dir->data[ fd_array[ fd ].offset ], result );

      fd_array[fd].offset += result;  // advance our "current" ptr

      *lp_actual = result;
   }

   return GOOD;
}

// check ownership of fd and the fd is valid within range
int CanAccessFD( int fd, int owner ) {
   if( VALID_FD_RANGE(fd) && fd_array[fd].owner == owner) return GOOD;
   return ILL_PARAM;
}

// Search our (fixed size) table of file descriptors. returns fd_array[] index
// if an unused entry is found, else -1 if all in use. if avail, then all
// fields are initialized.
int AllocFD( int owner ) {
   int i;

   for(i=0; i<MAX_FD; i++) {
      if( UNUSED == fd_array[i].owner ) {
         fd_array[i].owner = owner;
         fd_array[i].offset = 0;
         fd_array[i].item = NULL;     // NULL is (void *)0, spede/stdlib.h

         return i;
      }
   }

   return NO_FD;   // no free file descriptors
}

void FreeFD( int fd ) {  // mark a file descriptor as now free
   fd_array[fd].owner = UNUSED;
}

dir_t *FindName( char *name ) {
   dir_t *starting;

// assume every path relative to root directory. Eventually, the user
// context will contain a "current working directory" and we can possibly
// start our search there
   if( name[0] == '/' ) {
      starting = root_dir;

      while( name[0] == '/' ) name++;

      if( name[0] == 0 ) return root_dir; // client asked for "/"
   } else {
// path is relative, so start off at CWD for this process
// but we don't have env var CWD, so just use root as well
      starting = root_dir; // should be what env var CWD is
   }

   if( name[0] == 0 ) return NULL;

   return FindNameSub( name, starting );
}

// go searching through a single dir for a name match. use MyStrcmpSize()
// for case-insensitive compare. use '/' to separate directory components
// if more after '/' and we matched a dir, recurse down there
// RETURN: ptr to dir entry if found, else 0
// once any dir matched, don't return name which dir was matched
dir_t *FindNameSub( char *name, dir_t *this_dir ) {
   dir_t *dir_p = this_dir;
   int len = MyStrlen( name );
   char *p;

// if name ends in '/', chances are we need to decend into the dir
   if( ( p = strchr( name, '/' ) ) != NULL) len = p - name; 

   for( ; dir_p->name; dir_p++ ) {
      if( 1 == MyStrcmpSize( name, dir_p->name, len ) ) {
         if( p && p[1] != 0 ) {
// user is trying for a sub-dir. if there are more components, make sure this
// is a dir. if name ends with "/" we don't check. thus "hello.html/" is legal
             while( *p == '/' ) {
                p++;
                if( '\0' == *p ) return dir_p; // "/README/////" is OK
             }

             name = p;
             return FindNameSub( name, (dir_t *)dir_p->data );
         }
         return dir_p;
      }
   }

   return NULL;   // no match found
}

// copy what dir_p points to (dir_t) to what attr_p points to (attr_t)
void Dir2Attr( dir_t *dir_p, attr_t *attr_p ) {
   attr_p->dev = GetPid();            // FileMgr() manages this i-node

   attr_p->inode = dir_p->inode;
   attr_p->mode = dir_p->mode;
   attr_p->nlink = ( A_ISDIR( attr_p->mode ) ) + 1;
   attr_p->size = dir_p->size;
   attr_p->data = dir_p->data;
}
