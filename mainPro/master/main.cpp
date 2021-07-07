#include<iostream>
#include<unistd.h>
#include<fstream>
#include<vector>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<cstring>
#include<map>

using namespace std;

class splitFile
{
	int blockSize=1024;
	int option;
	int slave_count;
	const char *fname;
	string name;
	public:

	int sock=0;

	splitFile(int op,int sc,string fname)
	{
		option=op-1;
		slave_count=sc;
		name=fname;
		this->fname=fname.c_str();
	}

	splitFile()
	{}

	void split() // function to split the file into blocks and store it into  a string vector
	{
		
		struct stat st;
		//stat("mainfile.dat",&st);
		stat(fname,&st);
		
		int size=st.st_size;  // gets the file size

		ifstream fin(fname,ifstream::binary);  

		int blockSize=1024;

		vector<char> buffer(blockSize,0);
		char *buf=new char[size];  // creating a character array to read the file

		int fd=open(fname,O_RDONLY);

		read(fd,buf,size);  // storing data in array
		close(fd);
		
		int no_block=size/1024;
		if(size%1024>0)
		{
			no_block++;
		}
		cout<<no_block<<endl;

           	vector<string> block;
  		int n=1024,t=0;

  		for(int i=1;i<=no_block;i++)
  		{
         	string s1;
         	if(i!=no_block)
		{
                	for(int j=t;j<n;j++)
                	s1=s1+buf[j];
         	}

        	if(i==no_block)
        	{
                for(int j=t;j<size;j++)
                        s1=s1+buf[j];
        	}
        	block.push_back(s1);
        	t=n;
        	n=n+1024;
  		}
                

		sendBlocks(block,no_block);
		
		/*  for(auto i=0;i<block.size();i++)
  		{
  		cout<<block[i];
  		}*/

 		cout<<endl;
	}



	// Function to get the blocks and distribute them evenly to the clients
	// Uses different client port numbers to send the data
	void sendBlocks(vector<string> block,int no_of_blocks)
	{

		map<int,int> ip_ports;
		ip_ports[1]=8080;
		ip_ports[2]=8888;
		ip_ports[3]=7777;
		ip_ports[4]=7070;

		int no_of_slaves=slave_count;

		int no_blocks_per_node = no_of_blocks/no_of_slaves;

		int i=1;

		int st_block_no=0;
		int end_block_no=no_blocks_per_node;
		/*
		while(end_block_no<=no_of_blocks)
		{
		connection(block,st_block_no,end_block_no,ip_ports[i],0);
		st_block_no=end_block_no;
		end_block_no+=no_blocks_per_node;
		i++;
		}*/
		
		//connection(block,0,78,8080,1);
		
		 while(end_block_no<=no_of_blocks)
                {
                connection(block,st_block_no,end_block_no,ip_ports[i],option);
                st_block_no=end_block_no;
                end_block_no+=no_blocks_per_node;
                i++;
                }

	
	}

	void connection(vector<string> block,int st_block_no,int fn_block_no,int port,int send_or_recieve) // function to connect to slave machines and send blocks
	{
		int valread;
		struct  sockaddr_in serv_addr;
		if((sock=socket(AF_INET,SOCK_STREAM,0))<0)
		{
			cout<<"Socket error!!..."<<endl;
		}
		serv_addr.sin_family=AF_INET;
		serv_addr.sin_port=htons(port);

		if(inet_pton(AF_INET, "127.0.0.1" , &serv_addr.sin_addr)<=0) 
    		{
        		printf("\nInvalid address/ Address not supported \n");
    		}
   
  		if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    		{
        		printf("\nConnection Failed \n");
    		}
		cout<<"connected..."<<endl;
		
		send(sock,name.data(),name.size(),0);
		send(sock,&send_or_recieve,4,0); // telling client what needs to be done

		
		
		int blocks_count=fn_block_no-st_block_no;
		cout<<blocks_count;

		if(send_or_recieve==0) // if 0 then send data
		{
		send(sock,&blocks_count,4,0);
                send(sock,&st_block_no,4,0);
		
		for(int i=st_block_no;i<fn_block_no;i++)
		{
			const char *buff;
			string temp=block[i];
			buff=temp.c_str();

			send(sock,buff,strlen(buff),0);
                	//send(sock,&i,4,0);
		}
		}
		else // if not 0 then recieve data
		{
			send(sock,&blocks_count,4,0);
                	send(sock,&st_block_no,4,0);
			string s;

			ofstream fout("recieveFile",ios::app);

			/*
				while(recv(sock,&s,sizeof(s),0))
				{
					cout<<s<<endl;
				}
			*/
			char buffer[1024];

				while (1) 
				{

 					int n = recv(sock, buffer, sizeof(buffer), 0);
    					if (n <= 0){
      					break;
      					//return;
    					}
    					//fprintf(fp, "%s", buffer);
    					cout<<buffer<<endl;
					fout<<buffer;
					bzero(buffer, 1024);
				}
		}
		//close(sock);

	}

	void closeConn()
	{
		close(sock);
	}


};

int main()
{
	int op;
	int slave_count;
	string fname;

	cout<<"------------------------------------------welcome---------------------------------------"<<endl;
	cout<<"Hello ";
	while(op!=0)
	{
	cout<<"Please slect an option from the following :"<<endl;
	cout<<"0) exit"<<endl;
	cout<<"1) send file"<<endl;
	cout<<"2) recieve file"<<endl;
	cout<<"3) close connection"<<endl;
	cin>>op;

	//cout<<"enter the filename you want to send:"<<endl;
	//cin>>fname;
	if(op==1)
	{
		cout<<"enter number of slaves :"<<endl;
		cin>>slave_count;
		
		cout<<"enter the filename you want to send:"<<endl;
		cin>>fname;
	
		splitFile sf(1,slave_count,fname); // creating class object
		sf.split(); // calling function
	}
	else if(op==2)
	{
		cout<<"enter number of slaves :"<<endl;
		cin>>slave_count;
		
		cout<<"enter the filename you want to send:"<<endl;
		cin>>fname;
		
		splitFile sf(2,slave_count,fname);
		sf.split();
	}
	else if(op==3)
	{
		splitFile sf;
		sf.closeConn();
	}
	}
	return 0;
}
