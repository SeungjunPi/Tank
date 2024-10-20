# 탱크 대전 게임

사용 언어: C/C++

VCS: TFVC -> git

​실행: exe 폴더 내의 Sentinel 실행 후 Tank 실행

## 서버

IOCP 기반 NetCore DLL 포함

​	메인 쓰레드와 메시지 전달은 double buffering이용

​	Receive 로직

​		stream 조립 후 global back buffer에 추가

​			추가 시 atomic 변수를 이용해 offset을 이동하며 추가

​		사용자가 Handle 시 Doublebuffer를 교체

​	send message

​		session의 back buffer에 삽입

메인 게임 로직은 싱글스레드 기반

​	특징: 서버 주도 피격 판정, 서버와 클라이언트가 충돌 로직을 공유함

## 클라이언트

터미널의 텍스트 기반 게임ㅇ

wasd로 조작, space 로 총알 발사, 종료는 esc



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

### DB

- DB 쓰레드 추가
- id/pw 추가
- 유저 로그인 및 점수 기록