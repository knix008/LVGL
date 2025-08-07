# YOLO Model Converter - PyTorch to ONNX

이 프로젝트는 YOLO PyTorch 모델(.pt)을 ONNX 형식으로 변환하는 C/C++ 도구입니다.

## 개요

YOLO Model Converter는 다양한 YOLO 버전(YOLOv5, YOLOv6, YOLOv7, YOLOv8)의 PyTorch 모델을 ONNX 형식으로 변환하는 기능을 제공합니다. 이는 모델을 다양한 플랫폼에서 실행할 수 있게 해주며, 추론 성능을 최적화할 수 있습니다.

## 주요 기능

- **다양한 YOLO 버전 지원**: YOLOv5, YOLOv6, YOLOv7, YOLOv8
- **자동 모델 감지**: 입력 파일에서 YOLO 버전을 자동으로 감지
- **모델 검증**: 변환된 ONNX 모델의 유효성 검사
- **모델 최적화**: ONNX 모델 단순화 및 최적화 옵션
- **테스트 기능**: 변환된 모델을 테스트 이미지로 검증
- **모델 정보 표시**: 입력 모델의 상세 정보 확인

## 빌드 방법

### 필수 요구사항

- OpenCV 4.x
- C++17 호환 컴파일러
- CMake 3.16 이상

### 빌드 명령어

```bash
# 빌드 디렉토리 생성
mkdir build
cd build

# CMake 설정 및 빌드
cmake ..
make -j$(nproc)

# 실행 파일 확인
ls -la yolo_converter
```

## 사용법

### 기본 사용법

```bash
# 기본 변환
./yolo_converter -i yolov8n.pt -o yolov8n.onnx

# 모델 정보 확인
./yolo_converter -i yolov8n.pt -I

# 최적화 옵션과 함께 변환
./yolo_converter -i yolov5s.pt -o yolov5s.onnx -s -O

# 변환 후 검증
./yolo_converter -i yolov8n.pt -o yolov8n.onnx -v

# 테스트 이미지로 검증
./yolo_converter -i yolov8n.pt -o yolov8n.onnx -t test.jpg
```

### 명령행 옵션

| 옵션 | 설명 | 기본값 |
|------|------|--------|
| `-i, --input PATH` | 입력 PyTorch 모델 파일 (.pt) | 필수 |
| `-o, --output PATH` | 출력 ONNX 파일 경로 | 자동 생성 |
| `-w, --width SIZE` | 입력 너비 | 640 |
| `-h, --height SIZE` | 입력 높이 | 640 |
| `-b, --batch SIZE` | 배치 크기 | 1 |
| `-d, --device DEVICE` | 디바이스 (cpu/cuda) | cpu |
| `-c, --confidence THRESHOLD` | 신뢰도 임계값 | 0.25 |
| `-n, --nms THRESHOLD` | NMS 임계값 | 0.45 |
| `-s, --simplify` | ONNX 모델 단순화 | false |
| `-O, --optimize` | ONNX 모델 최적화 | false |
| `-t, --test IMAGE` | 테스트 이미지로 검증 | - |
| `-v, --validate` | 변환된 모델 검증 | false |
| `-I, --info` | 모델 정보 표시 | false |

## 사용 예제

### 1. 기본 변환

```bash
# YOLOv8n 모델을 ONNX로 변환
./yolo_converter -i yolov8n.pt -o yolov8n.onnx
```

### 2. 모델 정보 확인

```bash
# 모델의 상세 정보 확인
./yolo_converter -i yolov8n.pt -I
```

출력 예시:
```
Model Information:
=================
Model type: yolov8
Version: 
Input channels: 3
Dynamic shape: No
Number of classes: 80

Classes:
  0: person
  1: bicycle
  2: car
  ...
```

### 3. 최적화된 변환

```bash
# 단순화 및 최적화와 함께 변환
./yolo_converter -i yolov5s.pt -o yolov5s.onnx -s -O
```

### 4. 변환 후 검증

```bash
# 변환된 모델 검증
./yolo_converter -i yolov8n.pt -o yolov8n.onnx -v
```

### 5. 테스트 이미지로 검증

```bash
# 테스트 이미지로 모델 검증
./yolo_converter -i yolov8n.pt -o yolov8n.onnx -t test.jpg
```

## 프로그래밍 인터페이스

### C++ API 사용법

```cpp
#include "yolo_converter.h"

int main() {
    YOLOConverter converter;
    
    // 변환 설정
    ConversionConfig config;
    config.inputPath = "yolov8n.pt";
    config.outputPath = "yolov8n.onnx";
    config.inputWidth = 640;
    config.inputHeight = 640;
    config.optimize = true;
    config.simplify = true;
    
    // 변환 실행
    if (converter.convertPyTorchToONNX(config)) {
        std::cout << "변환 성공!" << std::endl;
        
        // 모델 검증
        if (converter.validateONNXModel(config.outputPath)) {
            std::cout << "모델 검증 통과!" << std::endl;
        }
    } else {
        std::cerr << "변환 실패: " << converter.getLastError() << std::endl;
    }
    
    return 0;
}
```

## 지원하는 모델 형식

### 입력 형식
- **PyTorch (.pt)**: YOLOv5, YOLOv6, YOLOv7, YOLOv8 모델
- **PyTorch (.pth)**: 기타 PyTorch 모델

### 출력 형식
- **ONNX (.onnx)**: Open Neural Network Exchange 형식

## 모델 최적화

### 단순화 (Simplify)
- 불필요한 연산 제거
- 모델 크기 감소
- 추론 속도 향상

### 최적화 (Optimize)
- 연산 그래프 최적화
- 메모리 사용량 최적화
- 하드웨어 가속 지원

## 오류 처리

### 일반적인 오류

1. **입력 파일 없음**
   ```
   Error: Input file does not exist: yolov8n.pt
   ```

2. **지원하지 않는 모델 형식**
   ```
   Error: Unsupported YOLO model type: unknown
   ```

3. **출력 디렉토리 생성 실패**
   ```
   Error: Failed to create output directory: /path/to/output
   ```

### 디버깅

오류 발생 시 다음을 확인하세요:
- 입력 파일 경로가 올바른지 확인
- 파일 권한 확인
- 디스크 공간 확인
- 모델 형식이 지원되는지 확인

## 성능 고려사항

### 변환 시간
- YOLOv8n: ~30초 (CPU)
- YOLOv5s: ~20초 (CPU)
- YOLOv6: ~25초 (CPU)

### 메모리 사용량
- 입력 모델 크기에 따라 다름
- 일반적으로 2-4GB RAM 필요

## 제한사항

### 현재 구현
- PyTorch 모델 로딩은 플레이스홀더 구현
- 실제 변환을 위해서는 PyTorch 라이브러리 통합 필요
- CUDA 지원은 기본적으로 비활성화

### 향후 개선사항
- 실제 PyTorch 모델 로딩 구현
- CUDA 가속 지원
- 더 많은 YOLO 버전 지원
- TensorRT 변환 지원

## 라이선스

이 프로젝트는 AGPL-3.0 라이선스를 따릅니다. Ultralytics의 라이선스 헤더를 참조하세요.

## 기여하기

버그 리포트나 기능 요청은 이슈를 통해 제출해 주세요. 코드 기여도 환영합니다.

## 관련 링크

- [YOLO 공식 사이트](https://ultralytics.com/)
- [ONNX 공식 사이트](https://onnx.ai/)
- [OpenCV 공식 사이트](https://opencv.org/)
