# Concurrent-Stock-Server

## Purpose
> 주식 서버는 다수의 client들이 접속했을 때, 각 client의 요청을 동시에 처리해줄 수 있어야 한다. 만약 주식 서버가 iterative하게 구현되어 있다면, 주식 서버는 한 번에 한 client와의 connection에 대해서만 request를 처리할 수 있다. 따라서 다수의 clients와 connection을 형성한 채로 여러 request를 동시에 처리하기 위해서는 주식 서버를 concurrent하게 구현할 필요가 있다. <br><br>
> 서버를 concurrent하게 운영하기 위해서는 다음과 같이 3가지 방법을 적용해볼 수 있다.<br>
> 1. Process-based approach <br>
> 2. Event-based approach <br>
> 3. Thread-based approach <br><br>
> 본 프로젝트에서는 미니 주식서버인 stockserver가 concurrent하게 운영되도록 하기 위해 2, 3번 방법을 이용하였다. Event-based approach에서는 select 함수를 이용하였고, Thread-based approach에서는 pthread 계열의 함수를 이용하여 구현하였다. <br>

## About Server
stockserver.c <br>
><br>
echo.c <br>
><br>

## About Client
multiclient.c <br>
><br>
stockclient.c <br>
><br>

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
1. show <br>
   : show the stock table <br>
   <br>
2. buy [stock ID] [# of stocks] <br>
   : buy the stocks <br>
   <br>
3. sell [stock ID] [# of stocks] <br>
   : sell the stocks <br>
   <br>
4. exit <br>
   : close the client connection <br>

## Author
Kim Minseon <br>
2021.06.03 - 2021.06.24 <br>
<br>
##### Sogang Univ. System Programming - Concurrent Stock Server project

