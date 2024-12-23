# 탱크 대전 게임

![탱크 게임 스크린샷](pictures/탱크게임스크린샷.png)

사용 언어: C/C++

VCS: TFVC -> git

저장소 주소: https://github.com/SeungjunPi/Tank

~~실행: 프로젝트의 exe 폴더 내 Sentinel 실행 후 Tank 실행~~ (DB 추가로 인해 실행 불가)



# 클라이언트

터미널의 텍스트 기반 게임

조작

W/S: 앞/뒤 이동

A/D: 좌/우 회전

space: 총알 발사

esc: 종료



# 서버
### 서버 전체 아키텍처
![전체구조](pictures/전체구조.png)

특징: 서버 주도 피격 판정, 서버와 클라이언트가 충돌 로직을 공유함, 상태기반 업데이트를 통한 네트워크 부하 감소 및 클라이언트 수용량 증가

## NetCore

- IOCP 기반 네트워크 라이브러리

- DLL로 작성
- main에 NetCore 내부의 리소스를 직접 노출하는 것은 Buffer와 SessionID 외엔 없음. 
- NetCore에서는 특정 SessionID로 부터 받은 패킷을 NetMessage로 변경 후 공통 Buffer에 쌓음, 처리는 main의 몫
- main에서 SessionID를 통해 특정 세션에 대한 동작을 요청

### 세션 연결

새 Session 수립 시 NetCore에 SessionID만 전달

![NetCoreCreateSesssion2](pictures/세션연결2.png)




### Receive, Send 로직

Receive 로직

![NetCore Receive](pictures/NetCoreReceive.png)

Send 로직

![NetCore Send 1](pictures/NetCoreSend1.png)

![NetCore Send 2](pictures/NetCoreSend2.png)

메인 로직은 싱글스레드 기반

## 메시지 처리

### 유저 접속

처음 접속 시 id와 snapshot을 전달하는것 외에는 대부분 broadcast 처리.

![로그인과정](pictures/로그인과정.png)

![인증요청](pictures/인증요청.png)

![인증결과](pictures/인증결과.png)

index를 유효하게 받았을 때 인증 성공.

### 인증 성공 시 

로그인 과정에 이어 즉시 아래 과정이 실행됨. 

![점수로드](pictures/점수로드.png)

### 인증 실패 시

클라이언트에 로그인 실패 메시지 전달, 세션 연결 해제.

(작성 중)

### 이동 처리

이동 시작과 종료만 클라이언트가 서버로 전달하는 방식으로 동작

![이동 시작](pictures/이동시작.png)
클라이언트는 이벤트 발생 시 이동 시작에 앞/뒤/좌/우가 담긴 플래그를 전달

![이동 중](pictures/이동중.png)
이동 처리는 클라이언트에서 각자 처리함

![이동 종료](pictures/이동종료.png)
종료 시 클라이언트는 앞/뒤/좌/우가 담긴 플래그를 자신의 위치와 함께 보내며, (임시)서버는 이를 받아서 업데이트하고 다른 유저에게 broadcast.

### 투사체

이동 처리는 각자 알아서 하며, 피격 판정만 서버에서 수행한 후 클라이언트로 전달함.

![투사체 1](pictures/투사체1.png)

![투사체 2](pictures/투사체2.png)

![투사체 3](pictures/투사체3.png)

![투사체 4](pictures/투사체4.png)

## JunDB

접속 시 유저 확인 단계를 예로 들어 설명함.

### 시작

![DB시작](pictures/DBStart.png)

### DB 함수 호출

![DB 함수 호출](pictures/DBCallFunction.png)

- 쿼리문은 DB 라이브러리 내부에서 작성되며, 외부에선 함수만 호출함
- 메인 쓰레드에서 Validation 함수를 호출하며 ID와 PW를 넘겨줌
- 함수에서 쿼리를 생성, InQueue에 삽입
- 워커를 깨우는 이벤트 발생

### 워커 동작

이 부분은 순차적으로 설명함. 

1. 워커가 메인의 신호에 의해 깨어남

![DB 워커 1](pictures/DBWorker1.png)

2. 쿼리를 꺼냄

![DB 워커 2](pictures/DBWorker2.png)

3. 꺼낸 쿼리를 DB에 전달

![DB 워커 3](pictures/DBWorker3.png)

4. 받은 쿼리를 결과로 변환

![DB 워커 4](pictures/DBWorker4.png)

5. 변환한 결과를 Back Out Queue에 삽입

![DB 워커 5](pictures/DBWorker5.png)

6. In Queue가 비었다면 다시 Wait, 아니면 InQueue에서 꺼내는 동작부터 다시 수행

### 메인의 쿼리 결과 처리

1. 결과를 처리하기위해 BeginHandleResults() 호출
   1. (동시에 처리하는 현상 방지용) 처리중 플래그를 on으로 변경
   2. main에서 결과를 끊임없이 처리하기 위해 Front, Back Queue를 Swap

![워커](pictures/DBHandleResult0.png)

![워커결과처리1](pictures/DBHandleResult1.png)

2. Front Out Queue에서 결과를 꺼내서 처리

![워커 결과 처리2](pictures/DBHandleResult2.png)

3. 빌 때까지 처리 후, 처리 중 flag를 off로 변경. 

![워커 결과 처리3](pictures/DBHandleResult3.png)

## 충돌 처리 (tobe)






# 수정/변경/추가 예정인 것들

## 변경

### 게임 로직

- snapshot 전달 시 모든 object를 전달하게 변경


## 추가

### 컨텐츠

- 장애물
  - 서버 주도 생성

- 클라이언트 카메라 및 카메라 이동
- AOI
- 서버 주도로 탱크 리스폰

### DB

- 유저 로그인 기록

### 배포 프로세스

- 서버가 다운될 시 자동으로 재실행 되는 등의 프로세스 수립
  - .NET, Docker 사용 예상
