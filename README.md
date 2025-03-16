# 임베디드 통신시스템  - 신호등

## 영상 링크
https://github.com/EliseEgkart/ECS_traffic_light

## 회로 구성

![회로사진](https://github.com/user-attachments/assets/8b294105-8cc7-494f-9c41-e3191a931bcc)

|   부품   | 핀 |
|----------|----|
| 빨강 led | 11 |
| 노랑 led | 10 |
| 초록 led |  9 |
| 가변저항 | A0 |
|  버튼 1  |  6 |
|  버튼 2  |  5 |
|  버튼 3  |  4 |
|   power  | 5V |


## 코드 구성

### arduino
  
+ Taskscheduler 객체 생성
  ---
  + Task red, yellow, green, greenBlink, allBlink 생성
  ![image](https://github.com/user-attachments/assets/5d4799cd-dd30-44f8-959c-f63b12a1fe5a)
    

+ setup 함수
   ---
  + 핀 모드 설정
  + 인터럽트 설정
  ![image](https://github.com/user-attachments/assets/62f90cfb-622a-435b-9b38-ea96fd19f42a)
    

+ loop 함수
  ---
  + 버튼 입력 확인
  + 세 가지 버튼에 대해 더블 클릭 확인
  ![image](https://github.com/user-attachments/assets/5c8a5f2b-e0c6-41bc-a8e5-feccb7e3a963)

  + 가변저항 값 확인
    ![image](https://github.com/user-attachments/assets/439c3e8e-557d-46af-b124-58de94311d48)
    
    
  + 시리얼 데이터 수신
    ![image](https://github.com/user-attachments/assets/8d3ad12c-884d-4f2f-a3a6-3c74a09e89a3)





### p5.js

+ setup 함수
  ---
  + 밝기 표현을 위한 span 생성
  + led 밝기 조절 슬라이더 생성

    ![image](https://github.com/user-attachments/assets/d852bcbc-a4aa-44de-9fc3-7d7c327cd803)

  + connect 버튼 생성
    
    ![image](https://github.com/user-attachments/assets/b4311c41-6f24-4c12-88da-b017a51bcbe8)


  + 시리얼 포트 연결
    ---
    + 포트 연결 후 데이터 수신
      
      ![image](https://github.com/user-attachments/assets/40b3e07b-3a72-4e89-b6b5-64723ddb533b)

  + 시리얼 데이터 전송
    ---
    + 인코딩 후 데이터 전송

      ![image](https://github.com/user-attachments/assets/c0bedc90-7ff1-4c84-895f-62ee4417875b)

  + 시리얼 데이터 읽기
    ---
    + 데이터 수신
    + 개행문자로 데이터 구분
    + 수신 후 캔버스 led 색 변경

      ![image](https://github.com/user-attachments/assets/08e62c2a-f3be-4610-9e6e-6414d5d406e3)

  + 캔버스 led 색 변경
    ---
    + 데이터에 따라 led 색 변경
    + 데이터가 숫자일 경우 밝기 변경

      ![image](https://github.com/user-attachments/assets/998ddcdf-f15a-4312-8384-ec3be5510f0f)
      

