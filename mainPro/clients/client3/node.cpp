#include<iostream>
#include<unistd.h>
#include<fstream>
#include<vector>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#define port 7777

using namespace std;

struct blockInfo
{
        int block_no;
        int block_size;
        string filename;
        string data;
}obj;

void createFile(int block_no,string data,string filename)
{
	//string fn(filename);
	string bno=to_string(block_no);
	string fname=filename+":blockFile-"+bno;
	ofstream fout(fname);
	cout<<fname<<" created"<<endl;
	fout<<data;
	cout<<"Data Entered..."<<endl;
}

int main()
	{
int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
       
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(0);
    }
       
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(0);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( port );
       
    if (bind(server_fd, (struct sockaddr *)&address, 
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(0);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(0);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,   
                       (socklen_t*)&addrlen))<0)  // master socket
    {
        perror("accept");
        exit(0);
    }

    char buf[1024];
    int block_no;
    int no_of_blocks;

    int starting_block;

    int send_or_recieve=0;
    
    char filename[15];

    recv(new_socket,&filename,sizeof(filename),0); // recieving name
	
    string filenm="";
    int k=0;
    while(filename[k]!='\0')
    {filenm=filenm+filename[k];k++;}

    cout<<filenm<<endl;
    cout<<filename<<endl;

    recv(new_socket,&send_or_recieve,4,0); // recieving option

    if(send_or_recieve==0)
    {
    recv(new_socket,&no_of_blocks,4,0);
    
    recv(new_socket,&starting_block,4,0);
	
    for(int i=0;i<no_of_blocks;i++)
    {
    	memset(buf,'\0',1024);
    	recv( new_socket , buf, 1024,0);
    	//recv(new_socket,&block_no,4,0);
    	//cout<<block_no<<endl;
    	cout<<starting_block;
	cout<<buf<<endl;
    	createFile(starting_block,buf,filenm);
	starting_block++;
    }
    }
    else
    {
	    cout<<"sending required blocks..."<<endl;
    	recv(new_socket,&no_of_blocks,4,0);
    	recv(new_socket,&starting_block,4,0);

	cout<<no_of_blocks<<" "<<starting_block<<endl;
	
	int end_block_no=starting_block+no_of_blocks;

	for(int i=starting_block;i<end_block_no;i++)
	{
		string fname=filenm+":blockFile-"+to_string(i);
		const char * fnamec = fname.c_str();
		ifstream fin;
		FILE *fp;
		
		fp=fopen(fnamec,"r");
		string s;
		/*while (getline(fin,s))
		{
			cout<<s<<endl;
			send(new_socket,&s,sizeof(s),0);
		}*/

		char data[1024] = {0};

		while(fgets(data, 1024, fp) != NULL) 
			{
    			if (send(new_socket, data, sizeof(data), 0) == -1) {
      			perror("[-]Error in sending file.");
    			exit(1);
			}
    bzero(data, 1024);
  }
	
	}
    }


	
    //recv(new_socket,&obj,sizeof(struct blockInfo),0);
    //cout<<obj.data<<endl;


return 0;
}
