# 디지털 영상처리 설계 중간고사 (2022년)

## 1. (10점) 컬러 프린터의 C, M, Y 잉크를 2:3:5로 섞어서 종이의 한 면을 프린트하였다. 종이에 백색광을 투사하였을 때 R, G, B가 반사되는 비율을 %로 각각 제시하시오. 중간과정은 단순 공식을 쓰지 말고, 빛의 흡수와 반사의 관점에서 설명하시오.

<br>
<br>

## 2. (20점) Histogram stretch와 histogram equalization 연산에 대해 다음 물음에 답하시오.

### (1) (5점) 두 연산 중에서 일반적으로 영상의 명암대비를 더 잘 개선할 수 있는 방법은 무엇인가?

<br>

### (2) (5점) 어떤 경우에 두 연산이 동일한 효과를 가질 것인가?

<br>

### (3) (5점) Histogram stretch를 두 번 연속 수행하면 결과가 어떻게 될 것인가? 그 이유는 무엇인가?

<br>

### (4) (5점) Histogram equalization를 두 번 연속 수행하면 결과가 어떻게 될 것인가? 그 이유는 무엇인가?

<br>
<br>

## 3. (10점) DSLR의 초점 거리가 56mm이고 현재 F값이 2.8, 5.6일 때 조리개의 면적을 구하여 비교하시오.

<br>
<br>

## 4. (10점) Bilateral filtering과 Gaussian filtering의 공통점, 차이점을 설명하고, 연산량을 비교하시오.

<br>
<br>

## 5. (20점) 아래 우측 영상은 좌측 영상을 gray scale 영상으로 바꾼 후 Fourier transform 수행한 결과이다.

![5](https://user-images.githubusercontent.com/73745836/230255464-ce7c0c2f-2951-43c8-892a-013fe0aa4724.jpg)

### (1) (4점) 일반적으로 주파수 공간에서느 원점에 많은 에너지가 모여 있는데, 그 이유는 무엇인가?

<br>

### (2) (16점) 주파수 공간에서 (a)~(d)와 같은 다양한 필터의 모양과 inverse Fourier transform 결과 영상을 (1)~(4)에서 골라 짝짓기 하시오. (a)~(d)에서 검은 영역은 해당 주파수에서 magnitude를 0으로 만든 부분이다.

![5-2](https://user-images.githubusercontent.com/73745836/230255484-4e66b1d3-d226-4d72-a84b-c8ccff259683.jpg)

<br>
<br>

## 6. (20점) 다음 Sobel 필터에 대해 질문에 답하시오.

![6](https://user-images.githubusercontent.com/73745836/230255516-18dbfb53-ae8d-4db4-9d48-f07838d148a7.jpg)

### (1) (4점) 왼쪽 필터를 오른쪽 영상 *I*에 적용하면 생성되는 결과를 아래 셋 중에 고르시오.

![6-1](https://user-images.githubusercontent.com/73745836/230255538-73c6277c-3731-4795-b4a0-6e915feb8f2b.jpg)

<br>

### (2) (4점) 이 필터가 수행하는 수학적인 연산을 기호로 표시하시오.

<br>

### (3) (4점) 필터는 separable한 필터인지 구체적으로 설명하시오.

<br>

### (4) (8점) 원래의 필터와 seperable한 필터로 필터링을 수행할 때, 정수 덧셈과 곱셈의 측면에서 계산량을 비교하고, 계산량이 더 효율적인 방법이 무엇인지 찾으시오.

<br>
<br>

## 7. (10점) 다음 2차원 warping 변환의 DoF (Degree of Freedom)을 쓰시오.

### (1) (3점) RTS (rotation-translation-scaling)

<br>

### (2) (4점) Affine

<br>

### (3) (4점) Projective
