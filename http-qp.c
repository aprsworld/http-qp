






#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <glib.h>
#include <getopt.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netdb.h>

#include <sys/errno.h>

#include	<termio.h>
#include	<signal.h>
#include	<fcntl.h>
#include	<time.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <syslog.h>

char	paths[256][8];
int	pathsCount;

GString *CurlFlags;
int	TCPport = 8897;
int	Stratum;
int	TimeOut;
int	verbose;
int	hd;
fd_set	fdread;	/* used by select */
char SER_HOST_ADDR[256]="197.0.0.14";

/* nap is a sco library function sleeping for milliseconds //
 * // usleep is a posix function sleeping for microseconds */
long nap( long length )
{
/* //usleep doesn't return anythings so we will assume that
 * the actual number of microseconds was slept.
 */

usleep(length<<10);	//

return	length;
}
#include <time.h>
char *get_yyyymmddhhmmss(void)
{

static	char	buffer[16];
time_t	now;
struct	tm	notzulu;

(void) time(&now);

(void) localtime_r(&now,&notzulu);

snprintf(buffer,sizeof(buffer),"%04d%02d%02d%02d%02d%02d",
    1900+notzulu.tm_year,1+notzulu.tm_mon,notzulu.tm_mday,
	notzulu.tm_hour,notzulu.tm_min,notzulu.tm_sec);

return	buffer;
}
int select_setup(void)
/* returns the 1+max value of file descriptor */
{
int	max = 0;

FD_ZERO(&fdread);
verbose && printf("select_setup()\n");
FD_SET(hd,&fdread);
max = hd;

return	max+1;
}
GString *packetize(char *input,int len )
{
GString *output_packet;

output_packet = g_string_new("<Packet>");
g_string_append_printf(output_packet,"<DateTime>%s</DateTime><Data>",get_yyyymmddhhmmss());
g_string_append_len(output_packet,input,len);
g_string_append_printf(output_packet,"</Data></Packet>\n");

return	output_packet;
}
void process_this_request(int sock )
{
static	int	r_count = 0;
GString *packet,*output_packet;
char	buffer[256];
int	total_bytes = 0;
int	i,c;
FILE *out;
packet = g_string_new("");

do {
	c = read(sock,buffer,sizeof(buffer));
	if ( 0 >=  c )
		break;
	g_string_append_len(packet,buffer,c);
	total_bytes += c;
	}	while ( 0 < c );
	
/* packet data is total_bytes long */
verbose && write(1,packet->str,total_bytes);

output_packet = packetize(packet->str,total_bytes);
g_string_free(packet,1);
verbose && write(1,output_packet->str,strlen(output_packet->str));

snprintf(buffer,sizeof(buffer),"X%06d",r_count++);
out = fopen(buffer,"w");
fwrite(output_packet->str,sizeof(char),strlen(output_packet->str),out);
fclose(out);

/* let the operating system contact the children */
for ( i = 0; pathsCount > i; i++ )
	{
	char	newpath[256];
	snprintf(newpath,sizeof(newpath),"%s/%s",paths[i],buffer);
	link(buffer,newpath);
	}
unlink(buffer);

g_string_free(output_packet,1);
}
/*------------------------------------------------------------------*/

void five_sleep(void) //
{
struct timespec   request,remainder;


/* sleep 5 seconds */ //
memset(&request,'\0',sizeof(request));
request.tv_sec = 5;
(void) nanosleep(&request,&remainder); //
}
struct addrinfo *info;
struct addrinfo hints;

typedef struct LINKED_LIST {
	struct LINKED_LIST *left;	// root is the oldest then root->left
	struct LINKED_LIST *right;	// root->right is the newest then root->right->right
	char	*packet;
	int	size;
	}	LinkedList;
	
int add_to_link_list(LinkedList **root,char *filename , struct stat *buf )
{
FILE	*in;
LinkedList *p,*q;
in = fopen(filename,"r");
if ( 0 == *root )
	{
	*root = calloc(1,sizeof(LinkedList));
	(*root)->left = (*root)->right = *root;
	(*root)->packet = calloc(1 + buf->st_size,sizeof(char));
	(*root)->size = buf->st_size;
	if ( buf->st_size != fread((*root)->packet,sizeof(char),buf->st_size,in))
		{
		 openlog("http_qp",LOG_PID,LOG_USER);
		 syslog(LOG_NOTICE,
		    "http_qp unable to fread  \"%s\"  %s\n",filename,strerror(errno));
		 closelog();
		kill( -1,SIGKILL);
		return	1;
		}
	return	0;
	}

/*    root--p--q--r--s--t--u--root */

p = calloc(1,sizeof(LinkedList));
q = (*root)->right;

q->left = p;

p->right = q;
p->left = *root;

(*root)->right = p;

p->packet = calloc(1 + buf->st_size,sizeof(char));
p->size = buf->st_size;
if ( buf->st_size != fread(p->packet,sizeof(char),buf->st_size,in))
	{
	 openlog("http_qp",LOG_PID,LOG_USER);
	 syslog(LOG_NOTICE,
	    "http_qp unable to fread  \"%s\"  %s\n",filename,strerror(errno));
	 closelog();
	kill( -1,SIGKILL);
	return	1;
	}
return	0;
}
int	dump_nodes( LinkedList **root , char *url )
{
LinkedList *p;
if ( 0 == *root )	return	0;	// nothing to dump
puts(url);
puts("right");
p = (*root)->right;
while ( p != *root )
	{
	write(1,p->packet,p->size);
	p = p->right;
	}
write(1,p->packet,p->size);

puts("left");
p = (*root)->left;
while ( p != *root )
	{
	write(1,p->packet,p->size);
	p = p->left;
	}
write(1,p->packet,p->size);
puts(url);
puts("*************************************");



return	0;
}
int send_from_linked_list(LinkedList **root, char *url )
{
/* send the newst packets first */
LinkedList *p,*q,*r;

if ( 0 == *root )
	return	1;	/* empty list and nothing to send */

q = (*root)->right;
p = q->left;
r = q->right;

p->right = r;
r->left = p;
/* q has been dropped */

if ( p == q && q == r )
	*root = 0;	/* the root node is being sent */


/* we do not yet have a method for sending so lets just write it to a temporary file */
char	fname[64];
FILE *out;

snprintf(fname,sizeof(fname),"/tmp/output_%s",url);
out = fopen(fname,"a");
fwrite(q->packet,sizeof(char),strlen(q->packet),out);
fclose(out);


free(q->packet);
free(q);


return	0;
}

int start_up_sender(int number,char *url)
{
int flag;
char buffer[16];
struct stat buf;
LinkedList *root = 0;
LinkedList *p;


snprintf(buffer,sizeof(buffer),"%03d/",number);
flag = mkdir(buffer,0777);

if ( 0 != flag && EEXIST != errno )
	{
	fprintf(stderr,"Error creating \"%s\"  erno = %d",buffer,errno );
         openlog("http_qp",LOG_PID,LOG_USER);
         syslog(LOG_NOTICE,
            "http_qp Error creating \"%s\"  %s\n",buffer,strerror(errno));
         closelog();
	return	-1;
	}
strcpy(paths[number],buffer);
pathsCount++;


if ( 0 == fork() )
	{
	/*   this is the child */
	if ( chdir(buffer))
		{
		/* unable to switch to senders folder so tell everybody to exit */
		 openlog("http_qp",LOG_PID,LOG_USER);
		 syslog(LOG_NOTICE,
		    "http_qp unable to chdir  \"%s\"  %s\n",buffer,strerror(errno));
		 closelog();
		kill( -1,SIGKILL);
		}
	for ( ;; )
		{
		DIR *dir;
		struct dirent *d;
		dir = opendir(".");
		while (  d = readdir(dir) )
			{
			/* d->d_name is type file then add it to linked list and 
				unlink it from the file system. */
			if ( stat(d->d_name,&buf))
				continue;	/* do not figure out why */
			   if ( 0 == S_ISREG(buf.st_mode) )
			      continue;   /* not a regular file */
			/* okay add it to the linked list */
			add_to_link_list(&root,d->d_name,&buf);
			verbose && dump_nodes(&root,url);
			unlink(d->d_name);
			}
		closedir(dir);
		nap(200L);
		/* when here we try sending patckets */
		send_from_linked_list(&root,url);
		verbose && dump_nodes(&root,url);
		}
	}


return	0;
}
int	main(int argc, char ** argv )
{
int	a,i,index;
struct sockaddr_in   cli_addr;
int	fdmax;
int	ec = 0;
int	clilen;
struct addrinfo *info;
struct addrinfo hints;
char	Port[16];
CurlFlags = g_string_new("");
snprintf(Port,sizeof(Port),"%d",TCPport);


while ( (a = getopt(argc, argv,"vt:s:p:c:d:")) != -1 )
	{
	switch ( a )
		{
		case	'd':
			if ( chdir(optarg) )
				{
				fprintf(stderr,"#Cannot chdir(\"%s\")\n",optarg);
				return	1;
				}
			break;
		case	'c':
			g_string_append_printf(CurlFlags,"%s ",optarg);
			break;
		case	'p':
			TCPport = atoi(optarg);
			strncpy(Port,optarg,sizeof(Port));
			break;
		case	's':
			Stratum = atoi(optarg);
			break;
		case	't':
			TimeOut = atoi(optarg);
			break;
		case	'v':
			verbose = 1;
			break;

		}
	}
   
i = 0;
for ( index = optind; argc > index ; index++,i++ )
	{ 
	verbose && printf("url = %s\n",argv[index]);
	if ( start_up_sender(i,argv[index]))
		return	-1;
	}

strncpy(SER_HOST_ADDR,"0.0.0.0",sizeof(SER_HOST_ADDR));

memset(&hints,'\0',sizeof(hints));

hints.ai_family =PF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;
hints.ai_protocol=0;


i = getaddrinfo(SER_HOST_ADDR,Port,  &hints, &info);

if ( 0 == i )
	{
	if ( PF_INET6 == info->ai_family )
		puts("IPv6");
	for ( ; 0 != info ; info = info->ai_next )
		{
		if (info->ai_family != AF_INET && info->ai_family != AF_INET6)
			continue;

		if ( (hd = socket(info->ai_family, info->ai_socktype,
			info->ai_protocol)) < 0)
			continue;
		if ( bind( hd,info->ai_addr, info->ai_addrlen ) < 0 )
			{
			close(hd);
			continue;
			}
		break;
		}
	}

if ( 0 == info  || 0 != i )
	{
	five_sleep(); //
	return	1;
	}


listen(hd,5);
for ( ;; )
   {
   int   i;
   /* setup for a call to select */
   fdmax=select_setup();
   i = select(fdmax,&fdread,(fd_set *) 0,(fd_set *) 0,(struct timeval *) 0);
   if ( 0 > i )
      {
      ec++;
      if ( 100 < ec )
         exit(1);
      continue;
      }
   ec = 0;
   /* check for connection request */
   while ( 0 != FD_ISSET(hd,&fdread) )
      {
      int   newsockfd;

     if ( verbose )
         {
         puts("Connection request.");
         fflush(stdout);
         }

      /* accept connection */
      clilen=sizeof(cli_addr);
      newsockfd = accept(hd,(struct sockaddr *) &cli_addr, &clilen);

      if ( newsockfd < 0 )
         {
         puts("server: accept error");
         openlog("http_qp",LOG_PID,LOG_USER);
         syslog(LOG_NOTICE,
            "http_qp %s\n",strerror(errno));
         closelog();
         five_sleep(); //
         return 1;
         }

        process_this_request(newsockfd);
      close(newsockfd);
      }
   }




return	0;
}
