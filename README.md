# Concurrent-Stock-Server

## Purpose
> 주식 서버는 다수의 client들이 접속했을 때, 각 client의 요청을 동시에 처리해줄 수 있어야 한다. 만약 주식 서버가 iterative하게 구현되어 있다면, 주식 서버는 한 번에 한 client와의 connection에 대해서만 request를 처리할 수 있다. 따라서 다수의 clients와 connection을 형성한 채로 여러 request를 동시에 처리하기 위해서는 주식 서버를 concurrent하게 구현할 필요가 있다. <br>
>
> 서버를 concurrent하게 운영하기 위해서는 다음과 같이 3가지 방법을 적용해볼 수 있다.<br>
>
> 1. Process-based approach <br>
> 2. Event-based approach <br>
> 3. Thread-based approach <br>
>
> 본 프로젝트에서는 미니 주식서버인 stockserver가 concurrent하게 운영되도록 하기 위해 2, 3번 방법을 이용하였다. Event-based approach에서는 select 함수를 이용하였고, Thread-based approach에서는 pthread 계열의 함수를 이용하여 구현하였다. <br>

## About Server
> 서버가 실행되면 파일로 저장되어있는 주식 내역이 메모리 상에 올라가게 된다. 즉, 서버는 stock.txt라는 파일에서 주식 내역을 읽어들이며, 주식 내역은 트리 구조로 stock_table에 저장된다. <br>
> 
> 이후 서버는 다수의 클라이언트들로부터 connection request를 받게 된다. 이 때 Event-based approach에서는 각 클라이언트와 connection을 형성하기 위해 connection descriptor를 배열로 저장할 수 있도록 한다. Thread-based approach에서는 여러 thread들이 서로 다른 클라이언트들과 각자 connection을 형성할 수 있도록 한다. 서버와 연결된 각 클라이언트들은 각자의 connection descriptor를 통해 서버로 원하는 request를 보낼 수 있다. 서버는 rio_readlineb()을 통해 클라이언트가 보낸 request를 받고, 이에 대해 처리를 한 뒤, rio_writen()을 통해 클라이언트로 결과 메시지를 전송해준다. <br>
> 
> 더이상 보낼 request가 없는 클라이언트들은 서버로 'exit'이라는 request를 보내 연결 해제를 요청한다. exit 메시지를 받은 서버는 해당 request를 보내온 클라이언트의 접속을 close시키고, 현재 메모리 상에 저장된 stock_table의 내용을 다시 파일로 저장해준다. 즉, stock.txt 파일은 각 클라이언트들이 접속을 종료할 때마다 update된다. <br>

## About Client
> 클라이언트 측에서는 rio_writen()을 통해 서버로 request를 보내고, rio_readnb()를 통해 서버로부터 response를 받는다. <br>
> 
> 1. multiclient.c <br>
> 다수의 클라이언트가 접속하는 경우를 테스트하기 위한 소스코드이다. fork()를 이용해 여러 클라이언트 프로세스를 만들고, 각 클라이언트 프로세스들은 일정 개수만큼의 request를 자동으로 생성하여 서버로 보낸다. 일정 개수만큼 request를 생성한 프로세스는 자동으로 종료된다. <br>
>
> 2. stockclient.c <br>
> 한 클라이언트가 접속하는 경우를 테스트하기 위한 소스코드이다. multiclient.c와 달리 request를 자동으로 생성하지 않으며, 사용자가 shell에 직접 show, buy, sell, exit 등의 command를 입력하여 테스트할 수 있다. 일정 개수만큼의 request를 생성하고 자동으로 종료되는 multiclient.c와는 달리, "exit"이라는 명령어를 직접 입력해야 클라이언트 접속을 종료할 수 있다.<br>

## How to compile
> $ make

## How to run stockserver
> $ ./stockserver [port number] <br>
> <br>
> ex) $ ./stockserver 9999

## How to run stockclient
> $ ./stockclient [server's IP address] [port number] <br>
> <br>
> ex) $ ./stockclient 172.10.22.13 9999 

## How to run multiclients
> $ ./multiclient [server's IP address] [port number] [# of clients] <br>
> <br>
> ex) $ ./multiclient 172.10.22.13 9999 4

## Allowed Commands
> 1. show <br>
>    : show the stock table <br>
>
> 2. buy [stock ID] [# of stocks] <br>
>    : buy the stocks <br>
>    
> 3. sell [stock ID] [# of stocks] <br>
>    : sell the stocks <br>
>
> 4. exit <br>
>   : close the client connection <br>
>   

## Author
Kim Minseon <br>
2021.06.03 - 2021.06.24 <br>
<br>
##### Sogang Univ. System Programming - Concurrent Stock Server project

