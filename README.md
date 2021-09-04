# Concurrent-Stock-Server

## 1. Introduction
- 주식 서버는 다수의 client들이 접속했을 때, 각 client의 요청을 동시에 처리해줄 수 있어야 한다. 만약 주식 서버가 iterative하게 구현되어 있다면, 주식 서버는 한 번에 한 client와의 connection에 대해서만 request를 처리할 수 있다. 따라서 다수의 clients와 connection을 형성한 채로 여러 request를 동시에 처리하기 위해서는 주식 서버를 concurrent하게 구현할 필요가 있다. <br>

- 서버를 concurrent하게 운영하기 위해서는 다음과 같이 3가지 방법을 적용해볼 수 있다.<br>
  ```
  1. Process-based approach <br>
  2. Event-based approach <br>
  3. Thread-based approach <br>
  ```

- 본 프로젝트에서는 미니 주식서버인 stockserver가 concurrent하게 운영되도록 하기 위해 2, 3번 방법을 이용하였다. Event-based approach에서는 select 함수를 이용하였고, Thread-based approach에서는 pthread 계열의 함수를 이용하여 구현하였다. <br>
- 내가 구현한 파트는 **stockserver**와 관련한 부분이다. <br>
<br>

## 2. Requirements
- About Server
  >  
  >  서버가 실행되면 파일(stock.txt)로 저장되어 있는 주식 내역이 메모리 상에 올라가게 된다.
  >
  > 이후 서버는 다수의 클라이언트들로부터 connection request를 받게 된다. 이 때 서버에서는 각 클라이언트와 connection을 concurrent하게 형성해야 하며, 서버와 연결된 각 클라이언트들은 각자의 connection descriptor를 통해 서버로 원하는 request를 보낼 수 있다. 서버는 rio_readlineb()을 통해 클라이언트가 보낸 request를 받고, 이에 대해 처리를 한 뒤, rio_writen()을 통해 클라이언트로 결과 메시지를 전송해준다.
  >
  > 더이상 보낼 request가 없는 클라이언트들은 서버로 'exit'이라는 request를 보내 연결 해제를 요청한다. exit 메시지를 받은 서버는 해당 request를 보내온 클라이언트의 접속을 close시키고, 현재 메모리 상에 저장된 stock_table의 내용을 다시 파일로 저장해준다. 즉, stock.txt 파일은 각 클라이언트들이 접속을 종료할 때마다 update된다.
  > 

- About Client
  > 클라이언트 측에서는 rio_writen()을 통해 서버로 request를 보내고, rio_readnb()를 통해 서버로부터 response를 받는다.
  >
  > 1. multiclient.c 
  > 다수의 클라이언트가 접속하는 경우를 테스트하기 위한 소스코드이다. fork()를 이용해 여러 클라이언트 프로세스를 만들고, 각 클라이언트 프로세스들은 일정 개수만큼의 request를 자동으로 생성하여 서버로 보낸다. 일정 개수만큼 request를 생성한 프로세스는 자동으로 종료된다. 
  >
  > 2. stockclient.c 
  > 한 클라이언트가 접속하는 경우를 테스트하기 위한 소스코드이다. multiclient.c와 달리 request를 자동으로 생성하지 않으며, 사용자가 shell에 직접 show, buy, sell, exit 등의 command를 입력하여 테스트할 수 있다. 일정 개수만큼의 request를 생성하고 자동으로 종료되는 multiclient.c와는 달리, "exit"이라는 명령어를 직접 입력해야 클라이언트 접속을 종료할 수 있다.

<br><br>

## 3. How to run
- compile

  ```
  $ make
  ```

- stockserver 실행

  ```
  $ ./stockserver [port number]
  ```

- stockclient 실행

  ```
  $ ./stockclient [server's IP address] [port number]
  ```

- multiclient 실행

  ```
  $ ./multiclient [server's IP address] [port number] [# of clients]
  ```
<br><br>

## 4. Allowed Commands
- 주식 내역 보기 (주식 테이블 출력)

```powershell
>> show
```

- 주식 주문하기

```powershell
>> buy [stock ID] [# of stocks]
```

- 주식 판매하기

```powershell
>> sell [stock ID] [# of stocks]
```

- 주식장 나가기 (클라이언트 접속 종료)

```powershell
>> exit
```
<br>

## 5. Date
- 2021.06.03 - 2021.06.24
