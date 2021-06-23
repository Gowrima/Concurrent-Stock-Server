# Concurrent-Stock-Server

## 프로젝트 개요
프로젝트의 개요입니다. <br>
<br>
<br>

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
> ex) $ ./stockserver 9999

## How to run stockclient
> $ ./stockclient [server's IP address] [port number] <br>
> ex) $ ./stockclient 172.10.22.13 9999 

## How to run multiclients
> $ ./multiclient [server's IP address] [port number] [# of clients] <br>
> ex) $ ./multiclient 172.10.22.13 9999 4

## Allowed Commands
1. show
  - show the stock table
2. buy [stock ID] [# of stocks] <br>
  - buy the stocks
3. sell [stock ID] [# of stocks]
  - sell the stocks
4. exit
  - close the client connection

## Author
Kim Minseon <br>
2021.06.03 - 2021.06.24 <br>
<br>
##### Sogang Univ. System Programming - Concurrent Stock Server project

