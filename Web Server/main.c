#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<dirent.h>
#include<errno.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<semaphore.h>
#include<signal.h>
#include<sys/signal.h>
void *baglanti(void *soket);
char * http_baslik_istegi (const char * istek);
int dosya_arama(char * dosya);
void dosya_kontrol(char *dosya, int sock,char *server_cevap);
void html_handler(char *dosya, int sock,char *server_cevap);
void image_handler(char *dosya, int sock);
char *connection_kontrol(char *istek);
sem_t mutex;

char *connection_kontrol(char *istek)
{
	int i,istek_uzunluk = strlen(istek);
	int flag = 0,j = 0,k;
	char connection[10];
	char *keep;
	keep = (char *)malloc(20*sizeof(char));
	for(i = 0; i < istek_uzunluk; i++)
	{
		if(flag == 1)
		{
			connection[j] = istek[i];
			j++;
		}
		if(j == 10)
		{
			j = 0;
			if(strcasecmp(connection,"connection") == 0)
			{
				i += 3;
				for(k = i; istek[k] != '\r'; k++)
				{
					keep[j] = istek[k];
					j++;
				}
				break;
			}
			else{
				flag = 0;
			}
		}
		if(istek[i] == '\n')
		{
			flag = 1;
		}
	}
	return keep;
}

void image_handler(char *dosya, int sock)
{
	char *buffer;
	int img_file;
	if((img_file=open(dosya,O_RDONLY)) > 0)
	{
		int bytes;
		char buffer[1024];
		while((bytes = read(img_file,buffer,1024))>0)
			write(sock,buffer,bytes);
	}
	else
	{
		printf("\nDosya Acilamadi...\n");
	}
	free(buffer);
	close(img_file);
}

void html_handler(char *dosya, int sock,char *server_cevap)
{
	char *buffer;
	FILE *html_file;
	html_file = fopen(dosya,"r");
	char *cevap_devam;
	int boyut;
	cevap_devam = (char *)malloc(200*sizeof(char));
	if(html_file)
	{
		fseek(html_file,0,SEEK_END);
		long bytes_read = ftell(html_file);
		fseek(html_file,0,SEEK_SET);
		sprintf(cevap_devam,"%ld\r\n\r\n",bytes_read);
		boyut += strlen(cevap_devam);
		strcat(server_cevap,cevap_devam);
		buffer = (char *)malloc(bytes_read*sizeof(char));
		fread(buffer,bytes_read,1,html_file);
		strcat(server_cevap,buffer);
		printf("Cevap:\n%s",server_cevap);
		boyut = strlen(server_cevap);
		send(sock,server_cevap,boyut,0);
		write(sock,buffer,bytes_read);
		fflush(stdout);
		free(buffer);
		fclose(html_file);
	}
	else
	{
		printf("\nDosya Acilamadi...\n");
	}
	free(cevap_devam);
}

void dosya_kontrol(char *dosya, int sock,char *server_cevap)
{
	char dosya_sonu[5];
	int i,flag = 0,j = 0;
	int boyut;
	int dosya_uzunlugu = strlen(dosya);
	char *type;
	strcmp(dosya_sonu,"");
	type = (char *)malloc(40*sizeof(char));
	printf("uzunluk:%d",dosya_uzunlugu);
	for(i = 0; i < dosya_uzunlugu; i++)
	{
		if(flag == 1)
		{
			dosya_sonu[j] = dosya[i];
			j++;
		}
		if(dosya[i] == '.')
		{
			flag = 1;
		}
	}
	printf("Dosya Sonu: %s\n",dosya_sonu);
	if(strcmp(dosya_sonu,"jpg") == 0 || strcmp(dosya_sonu,"jpeg") == 0 || strcmp(dosya_sonu,"png") == 0)
	{
		sprintf(type," image/%s\r\n\r\n",dosya_sonu);
		strcat(server_cevap,type);
		printf("\nServerCevap:%s\n",server_cevap);
		boyut = strlen(server_cevap);
		sem_wait(&mutex);
		send(sock,server_cevap,boyut,0);
		image_handler(dosya,sock);
		sem_post(&mutex);
	}
	else if(strcmp(dosya_sonu,"html") == 0 || strcmp(dosya_sonu,"css") == 0)
	{
		sprintf(type," text/%s\r\nContent-Length: ",dosya_sonu);
		strcat(server_cevap,type);
		sem_wait(&mutex);
		html_handler(dosya,sock,server_cevap);
		sem_post(&mutex);
	}
	else if(strcmp(dosya_sonu,"ico") == 0)
	{
		
	}
	else
	{
		strcat(server_cevap," text/plain\r\nContent-Length: ");
		sem_wait(&mutex);
		html_handler(dosya,sock,server_cevap);
		sem_post(&mutex);
	}
	free(type);
}

int dosya_arama(char * dosya)
{
	char *dizin;
	dizin = (char *)malloc(100*sizeof(char));
	strcpy(dizin,"/home/fatih/Masaüstü/Proje/Dosyalar/");
	strcat(dizin,dosya);
	printf("\n%s\n",dizin);
	FILE *file;
	file = fopen(dizin,"r");
	free(dizin);
	if(strcmp(dosya,"") == 0)
	{
		return 501;
	}
	if(file)
	{
		fclose(file);
		return 200;
	}
	else if(ENOENT == errno)
	{
		return 404;
	}
	else
	{
		return 501;
	}
}

char * http_baslik_istegi (const char * istek)
{
	int i;
	char *dosya;
	dosya = (char *)malloc(20*sizeof(char));
	strcpy(dosya,"");
	for(i = 5; istek[i] != ' '; i++)
	{
		dosya[i-5] = istek[i];
	}
	return dosya;
}


void *baglanti(void *soket)
{
	const char *HTTP_200_STRING = "OK";
	const char *HTTP_404_STRING = "Not Found";
	const char *HTTP_501_STRING = "Not Implemented";
	int istek_kontrol;
	char *client_cevap;
	char *temp;
	int buffer_size = 1024;
	int sock = *((int *)soket);
	int flag;
	int uzunluk;
	char end_of_file[5];
	const char *HTTP_404_CONTENT = "<html><head><title>404 Not Found</title></head><body><h1>404 Not Found</h1>The requested resource could not be found but may be available again in the future.<div style=\"color: #eeeeee; font-size: 8pt;\">Actually, it probably won't ever be available unless this is showing up because of a bug in your program. :(</div></html>";
const char *HTTP_501_CONTENT = "<html><head><title>501 Not Implemented</title></head><body><h1>501 Not Implemented</h1>The server either does not recognise the request method, or it lacks the ability to fulfill the request.</body></html>";
	temp = (char *)malloc(buffer_size*sizeof(char));
	client_cevap = (char *)malloc(buffer_size*sizeof(char));
	strcpy(client_cevap,"");
	do
	{
		flag = 0;
		//recv(int socket,void *buffer,size_t length,int flags)
		istek_kontrol = recv(sock,temp,1024,0);
		if(istek_kontrol < 0)
		{
			printf("İstek Yapilamadi...\n");
			perror("Recv");
			exit(0);
		}
		
		strcat(client_cevap,temp);
		uzunluk = strlen(client_cevap);
		strcpy(end_of_file,&client_cevap[uzunluk-4]);
		printf("%s",end_of_file);
		if(strcmp(end_of_file,"\r\n\r\n") != 0)
		{
			flag = 1;
			buffer_size *= 2;
			client_cevap = (char *)realloc(client_cevap,buffer_size*sizeof(char));
		}
		printf("%s",client_cevap);
		printf("\nİstek Alindi...");
	}while(flag);
	free(temp);
	char *baslik;
	baslik = (char *)malloc(100*sizeof(char));
	strcpy(baslik,"");
	int i;
	for(i = 0; client_cevap[i] != '\n'; i++)
	{
		baslik[i] = client_cevap[i];
	}
	char *connection_cevap = connection_kontrol(client_cevap);
	free(client_cevap);
	char *dosya = http_baslik_istegi(baslik);
	int response;
	response = dosya_arama(dosya);
	char *server_cevap;
	server_cevap = (char *)malloc(1024*sizeof(char));
	
	if(response == 200)
	{
		sprintf(server_cevap,"HTTP/1.1 %d %s\r\nConnection: %s\r\nContent-Type:",response,HTTP_200_STRING,connection_cevap);
		char *dosya_ismi;
		char buff[1024];
		dosya_ismi = (char *)malloc(50*sizeof(char));
		strcpy(dosya_ismi,"/home/fatih/Masaüstü/Proje/Dosyalar/");
		strcat(dosya_ismi,dosya);
		dosya_kontrol(dosya_ismi,sock,server_cevap);
	}
	if(response == 404)
	{
		sprintf(server_cevap,"HTTP/1.1 %d %s\r\nContent-Type: text/html\r\n\r\n",response,HTTP_404_STRING);
		send(sock,server_cevap,60,0);	
		write(sock,HTTP_404_CONTENT,strlen(HTTP_404_CONTENT));
	}
	else if(response == 501)
	{
	send(sock,"HTTP/1.1 501 Not Implemented\r\nContent-Type: text/html\r\n\r\n",58,0);
	      write(sock,HTTP_501_CONTENT,strlen(HTTP_501_CONTENT));	
	}
	
	close(sock);
	fflush(stdout);
	
}

int main()
{
	sem_init(&mutex,0,1);
	char localhost[] = "127.0.0.1";
	int port = 21546;
	int server_sock,client_sock;
	int *new_sock;
	struct sockaddr_in saddr_in;
	struct sockaddr_in client_saddr_in;
	saddr_in.sin_family = AF_INET;
	inet_aton(localhost,&saddr_in.sin_addr);
	saddr_in.sin_port = htons(port);
	socklen_t saddr_len = sizeof(struct sockaddr_in);
	
	if((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("Socket Acilamadi...\n");
		perror("Socket");
		exit(0);
	}
	
	if(bind(server_sock, (struct sockaddr *) &saddr_in, saddr_len) < 0)
	{
		printf("Bind Hatasi...\n");
		perror("Bind");
		exit(0);
	}
	//listen(int socket, int backlog)
	//backlog: Bağlantı kuyruğu için max uzunluk
	if(listen(server_sock, 10) < 0)
	{
		perror("Listen");
		exit(0);
	}
	

	
	while((client_sock = accept(server_sock, (struct sockaddr *) &client_saddr_in, &saddr_len)) > 0)
	{
		printf("Connection From: %s:%d (%d)\n",
			inet_ntoa(client_saddr_in.sin_addr),
			ntohs(client_saddr_in.sin_port),
			client_sock);
		pthread_t thre;
		new_sock = malloc(1);
		*new_sock = client_sock;
		if(pthread_create(&thre,NULL,baglanti,(void *)new_sock) < 0)
		{
			printf("Thread Olusturulamadi...\n");
			perror("Thread ");
			exit(0);
		}
		if(signal(SIGQUIT,NULL))
		{
			pthread_join(thre,NULL);
			close(server_sock);
			sem_destroy(&mutex);
			//exit(0);
		}
	}
	
}
