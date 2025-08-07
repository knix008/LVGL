# YOLO Converter Test Program

## 개요

YOLO Converter Test Program은 변환된 ONNX 모델을 테스트하고 검증하기 위한 종합적인 테스트 도구입니다. 이 프로그램은 YOLO Converter로 생성된 ONNX 모델의 기능성, 성능, 정확성을 검증합니다.

## 주요 기능

### 🔍 **모델 검증**
- ONNX 모델 로딩 테스트
- 모델 구조 검증
- 입력/출력 레이어 분석

### ⚡ **성능 테스트**
- 추론 시간 측정
- 벤치마크 테스트
- 메모리 사용량 모니터링

### 🖼️ **이미지 처리**
- 실제 이미지로 테스트
- 다양한 입력 크기 지원
- 전처리 및 후처리 검증

### 📊 **통계 및 분석**
- 평균/최소/최대 추론 시간
- 성공/실패 테스트 통계
- 상세한 결과 보고서

## 빌드 방법

### 필수 요구사항
- OpenCV 4.x
- C++17 컴파일러
- CMake 3.16+

### 빌드 명령어
```bash
# 프로젝트 루트 디렉토리에서
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### 생성된 실행 파일
- `yolo_converter_test` - 메인 테스트 프로그램

## 사용법

### 기본 사용법

```bash
# 기본 테스트
./yolo_converter_test -m model.onnx

# 이미지와 함께 테스트
./yolo_converter_test -m model.onnx -i image.jpg

# 벤치마크 테스트
./yolo_converter_test -m model.onnx -b 100

# 모델 로딩만 테스트
./yolo_converter_test -m model.onnx -l

# 모델 구조 검증
./yolo_converter_test -m model.onnx -v
```

### 명령행 옵션

| 옵션 | 긴 옵션 | 설명 | 기본값 |
|------|---------|------|--------|
| `-m` | `--model` | 테스트할 ONNX 모델 경로 | 필수 |
| `-i` | `--image` | 테스트 이미지 경로 | 선택사항 |
| `-s` | `--size` | 입력 크기 (WIDTHxHEIGHT) | 640x640 |
| `-b` | `--benchmark` | 벤치마크 실행 횟수 | 선택사항 |
| `-l` | `--load-only` | 모델 로딩만 테스트 | false |
| `-v` | `--validate` | 모델 구조 검증 | false |
| `-h` | `--help` | 도움말 표시 | - |

### 사용 예제

#### 1. 기본 모델 테스트
```bash
./yolo_converter_test -m yolov8n.onnx
```

**출력 예시:**
```
YOLO Converter Test Program
===========================

Model: yolov8n.onnx
Input Size: 640x640

Comprehensive Model Test Results:
========================
✅ Status: SUCCESS
📝 Message: Model test completed successfully
⏱️  Load Time: 22.4595 ms
⚡ Inference Time: 184.975 ms

Inference Test Results:
========================
✅ Status: SUCCESS
📝 Message: Inference test completed successfully
⚡ Inference Time: 121.385 ms

Model Structure Validation Results:
========================
✅ Status: SUCCESS
📝 Message: Model structure validation completed - Layers: 239, Outputs: 1
```

#### 2. 이미지와 함께 테스트
```bash
./yolo_converter_test -m yolov8n.onnx -i bus.jpg
```

#### 3. 벤치마크 테스트
```bash
./yolo_converter_test -m yolov8n.onnx -b 50
```

**출력 예시:**
```
Model Benchmark Test Results:
========================
✅ Status: SUCCESS
📝 Message: Benchmark completed - Avg: 126.879ms, Min: 81.512ms, Max: 257.002ms
⚡ Inference Time: 126.879 ms

Benchmark Statistics:
====================
Average Inference Time: 126.879 ms
Minimum Inference Time: 81.512 ms
Maximum Inference Time: 257.002 ms
```

#### 4. 모델 로딩 테스트
```bash
./yolo_converter_test -m yolov8n.onnx -l
```

#### 5. 모델 구조 검증
```bash
./yolo_converter_test -m yolov8n.onnx -v
```

## API 참조

### TestResult 구조체

```cpp
struct TestResult
{
    bool success;                    // 테스트 성공 여부
    std::string message;             // 결과 메시지
    double loadTime;                 // 모델 로딩 시간 (ms)
    double inferenceTime;            // 추론 시간 (ms)
    int numDetections;               // 검출된 객체 수
    std::vector<cv::Rect> boundingBoxes;  // 바운딩 박스
    std::vector<float> confidences;  // 신뢰도 점수
    std::vector<int> classIds;       // 클래스 ID
};
```

### YOLOConverterTest 클래스

#### 주요 메서드

```cpp
class YOLOConverterTest
{
public:
    // 종합 모델 테스트
    TestResult testConvertedModel(const std::string& onnxPath, 
                                  const std::string& testImagePath = "",
                                  const cv::Size& inputSize = {640, 640});

    // 모델 로딩 테스트
    TestResult testModelLoading(const std::string& onnxPath);

    // 추론 테스트
    TestResult testInference(const std::string& onnxPath, 
                             const cv::Size& inputSize = {640, 640});

    // 이미지 테스트
    TestResult testWithImage(const std::string& onnxPath, 
                             const std::string& imagePath,
                             const cv::Size& inputSize = {640, 640});

    // 벤치마크 테스트
    TestResult benchmarkModel(const std::string& onnxPath, 
                             int numRuns = 10,
                             const cv::Size& inputSize = {640, 640});

    // 모델 구조 검증
    TestResult validateModelStructure(const std::string& onnxPath);

    // 통계 조회
    double getAverageInferenceTime() const;
    double getMinInferenceTime() const;
    double getMaxInferenceTime() const;
    int getTotalTests() const;
    int getSuccessfulTests() const;
};
```

## 테스트 시나리오

### 1. 모델 검증 테스트
- **목적**: ONNX 모델이 올바르게 로드되는지 확인
- **검증 항목**: 파일 존재, 형식 유효성, 레이어 구조
- **실행**: `./yolo_converter_test -m model.onnx -l`

### 2. 추론 성능 테스트
- **목적**: 모델의 추론 성능 측정
- **검증 항목**: 추론 시간, 메모리 사용량, 처리량
- **실행**: `./yolo_converter_test -m model.onnx -b 100`

### 3. 이미지 처리 테스트
- **목적**: 실제 이미지에서의 객체 검출 성능 확인
- **검증 항목**: 검출 정확도, 바운딩 박스, 클래스 분류
- **실행**: `./yolo_converter_test -m model.onnx -i image.jpg`

### 4. 구조 검증 테스트
- **목적**: 모델의 내부 구조 분석
- **검증 항목**: 레이어 수, 출력 형태, 연결 관계
- **실행**: `./yolo_converter_test -m model.onnx -v`

## 오류 처리

### 일반적인 오류

#### 1. 모델 파일 없음
```
Error: Model file does not exist: model.onnx
```
**해결책**: 올바른 모델 파일 경로 확인

#### 2. 잘못된 ONNX 형식
```
Error: OpenCV error loading ONNX model: Failed to parse ONNX model
```
**해결책**: ONNX 모델이 올바르게 변환되었는지 확인

#### 3. 이미지 파일 없음
```
Error: Image file does not exist: image.jpg
```
**해결책**: 올바른 이미지 파일 경로 확인

#### 4. 잘못된 입력 크기
```
Error: Invalid size format. Use WIDTHxHEIGHT (e.g., 640x640)
```
**해결책**: 올바른 크기 형식 사용 (예: 640x640, 416x416)

### 오류 코드

| 오류 코드 | 설명 | 해결책 |
|-----------|------|--------|
| 1 | 명령행 인수 오류 | `-h` 옵션으로 사용법 확인 |
| 2 | 파일 없음 | 파일 경로 및 존재 여부 확인 |
| 3 | ONNX 파싱 오류 | 모델 변환 과정 재확인 |
| 4 | OpenCV 오류 | OpenCV 설치 및 버전 확인 |

## 성능 최적화

### 추론 시간 최적화
- **배치 처리**: 여러 이미지를 한 번에 처리
- **모델 양자화**: INT8 양자화로 속도 향상
- **GPU 가속**: CUDA 지원 활성화

### 메모리 최적화
- **모델 크기**: 불필요한 레이어 제거
- **배치 크기**: 메모리 사용량에 맞게 조정
- **메모리 풀**: 재사용 가능한 메모리 할당

## 테스트 스크립트

### 자동화된 테스트
```bash
# 전체 테스트 실행
./test_converter_test.sh
```

### 테스트 항목
1. **도움말 표시** - 명령행 인터페이스 검증
2. **인수 검증** - 필수 인수 누락 처리
3. **파일 검증** - 존재하지 않는 파일 처리
4. **모델 로딩** - ONNX 모델 로딩 테스트
5. **추론 테스트** - 기본 추론 기능 검증
6. **이미지 처리** - 실제 이미지 테스트
7. **벤치마크** - 성능 측정
8. **구조 검증** - 모델 구조 분석

## 제한사항

### 현재 제한사항
- **PyTorch 모델 직접 지원 없음**: ONNX 형식만 지원
- **GPU 가속 미지원**: CPU 추론만 지원
- **배치 처리 제한**: 단일 이미지 처리
- **후처리 제한**: 기본적인 바운딩 박스만 제공

### 향후 개선 계획
- **GPU 지원**: CUDA/OpenCL 가속 추가
- **배치 처리**: 다중 이미지 동시 처리
- **고급 후처리**: NMS, 앵커 박스 등 추가
- **모델 최적화**: 양자화, 프루닝 지원

## 문제 해결

### 빌드 문제
```bash
# OpenCV 경로 확인
pkg-config --cflags --libs opencv4

# 의존성 확인
ldd ./yolo_converter_test
```

### 실행 문제
```bash
# 권한 확인
chmod +x ./yolo_converter_test

# 라이브러리 경로 설정
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./lib
```

### 성능 문제
```bash
# CPU 사용률 확인
top -p $(pgrep yolo_converter_test)

# 메모리 사용량 확인
ps aux | grep yolo_converter_test
```

## 라이선스

이 프로젝트는 MIT 라이선스 하에 배포됩니다.

## 기여

버그 리포트, 기능 요청, 풀 리퀘스트를 환영합니다.

## 연락처

프로젝트 관련 문의사항이 있으시면 이슈를 생성해 주세요.
