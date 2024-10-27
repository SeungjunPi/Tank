# 탱크 대전 게임

![탱크 게임 스크린샷](pictures/탱크%20게임%20스크린샷.png)

사용 언어: C/C++

VCS: TFVC -> git

실행: 프로젝트의 exe 폴더 내 Sentinel 실행 후 Tank 실행

## 서버

IOCP 기반 NetCore DLL

​	Receive 로직

![NetCore Receive 스크린샷](pictures/NetCore%20Receive%20스크린샷.png)

​	Send 로직

​![NetCore Send 1](pictures/NetCore%20Send%201.png)

![NetCore Send 2](pictures/NetCore%20Send%202.png)

메인 로직은 싱글스레드 기반

​	특징: 서버 주도 피격 판정, 서버와 클라이언트가 충돌 로직을 공유함

## 클라이언트

터미널의 텍스트 기반 게임

조작

​	W/S: 앞/뒤 이동

​	A/D: 좌/우 회전

​	space: 총알 발사

​	esc: 종료

## 서버-클라이언트 통신

### 유저

처음 접속 시 id와 snapshot을 전달하는것 외에는 대부분 broadcast 처리.

![메시지](pictures/메시지.png)

### 투사체 관련

이동 처리는 각자 알아서 하며, 피격 판정만 서버에서 수행한 후 클라이언트로 전달함.

![투사체 1](pictures/투사체%201.png)

![투사체 2](pictures/투사체%202.png)

![투사체 3](pictures/투사체%203.png)

![투사체 4](pictures/투사체%204.png)



# 수정/변경/추가 예정인 것들

## 변경

### 게임 로직

- snapshot 전달 시 모든 object를 전달하게 변경

- 네트워크 트래픽을 감소시키기 위한 이동 로직 변경
  - 이동 중 상태면 서버와 각 클라이언트에서 자체적으로 이동
  - 이동 중에 주기적으로 보내는 클라이언트의 보고와 서버의 업데이트를 바탕으로 변경

## 추가

- 장애물
  - 서버 주도 생성

- 점수
  - 장애물을 맞추면 1점, 적을 맞추면 1점, 적을 격추시키면 10점, 자신이 격추당하면 -10점
- 클라이언트 카메라 및 카메라 이동
- AOI
- 서버 주도로 탱크 리스폰

### DB

- DB 쓰레드 추가
- id/pw 추가
- 유저 로그인 및 점수 기록