// 연결 포트트
let port;

// led 시간 슬라이더
let rSlider, ySlider, gSlider;
const sliderSize = 120;

// led 밝기 값 표현
let span;

// 시리얼 데이터 수신
let receivedData = '';

// 연결 버튼
let connectBtn;

// 캔버스에 led 그리기
function firstCircle() {
  circle(100, 80, 50);
}
function secondCircle() {
  circle(180, 80, 50);
}
function thirdCircle() {
  circle(260, 80, 50);
}

function redCircle() {
  fill(255, 0, 0);
  firstCircle();
}

function yellowCircle() {
  fill(255, 255, 0);
  secondCircle();}

function greenCircle() {
  fill(0, 255, 0);
  thirdCircle();
}

function fillWhite() {
  fill(255, 255, 255);
}

// setup 함수
function setup() {
  createCanvas(1000, 1000);

  // led 밝기 표현현
  span = createSpan('Light Brightness : ');
  span.position(250, 200);

  // led 밝기 조절 슬라이더 생성
  rSlider = createSlider(0, 5000, 2000);
  rSlider.position(50, 200);
  rSlider.size(sliderSize);
  rSlider.mouseReleased(() => {
    let rSliderVal = rSlider.value();
    writeSerialData('r' + rSliderVal + '\n');
  });

  ySlider = createSlider(0, 5000, 500);
  ySlider.position(50, 250);
  ySlider.size(sliderSize);
  ySlider.mouseReleased(() => {
    let ySliderVal = ySlider.value();
    writeSerialData('y' + ySliderVal + '\n');
  });

  gSlider = createSlider(0, 5000, 2000);
  gSlider.position(50, 300);
  gSlider.size(sliderSize);
  gSlider.mouseReleased(() => {
    let gSliderVal = gSlider.value();
    console.log(gSliderVal);
    writeSerialData('g' + gSliderVal + '\n');
  });

  // 연결 버튼 생성
  connectBtn = createButton('Connect');
  connectBtn.position(50, 350);
  connectBtn.mousePressed(portConnect);

  // 초기 led 상태 
  fillWhite();
  firstCircle();
  secondCircle();
  thirdCircle();
}



function draw() {

}

// 시리얼 포트 연결
// 포트 연결 후 데이터 수신 
async function portConnect() {
  try {
    port = await navigator.serial.requestPort();
    await port.open({ baudRate: 9600 });

    console.log('Connected to serial port');

    readSerialData();

  } catch (error) {
    console.error('There was an error opening the serial port:', error);
  }
}

// 시리얼 데이터 쓰기
// 인코딩 후 데이터 전송
async function writeSerialData(data) {
  const encoder = new TextEncoder();
  const writer = port.writable.getWriter();
  await writer.write(encoder.encode(data));
  writer.releaseLock();
}

// 시리얼 데이터 읽기 함수
async function readSerialData() {
  const decoder = new TextDecoder();

  while (port.readable) {
    const reader = port.readable.getReader();
    try {
      while (true) {
        const { value, done } = await reader.read();
        if (done) {
          break;
        }
        // 데이터 수신
        // 개행문자로 데이터 구분
        // 데이터 수신 후 캔버스 led 색상 변경
        if (value) {
          receivedData += decoder.decode(value);

          if (receivedData.indexOf('\n') !== -1) {
            receivedData = receivedData.trim();
            if (receivedData.split('\n').length > 1) {
              receivedData = receivedData.split('\n');
              paintColor(receivedData[0]);
              console.log(receivedData[0]);
              paintColor(receivedData[2]);
              console.log(receivedData[2]);
            } else {
              paintColor(receivedData);
              console.log(receivedData);
            }
            receivedData = '';
          }
        }
      }
    } catch (error) {
      console.error('Error reading:', error);
  }  finally {
      reader.releaseLock();
    }
  }
}

// 캔버스 led 색상 변경 함수
// 데이터에 따라 led 색상 변경
// 데이터가 숫자일 경우 밝기 변경
async function paintColor(str) {
  if (str == 'r' || str == 'e') {
    redCircle();
    fillWhite();
    secondCircle();
    thirdCircle();
  }
  else if (str == 'y') {
    yellowCircle();
    fillWhite();
    firstCircle();
    thirdCircle();
  }
  else if (str == 'g') {
    greenCircle();
    fillWhite();
    firstCircle();
    secondCircle();
  }
  else if (str == 'o') {
    fillWhite();
    firstCircle();
    secondCircle();
    thirdCircle();
  }
  else if (str == 'a') {
    redCircle();
    yellowCircle();
    greenCircle();
  }
  // 숫자일 경우 밝기 변경
  else if (Number(str) > 0 && Number(str) <= 1023) {
    span.html('Light Brightness : ' + str);
  }
}