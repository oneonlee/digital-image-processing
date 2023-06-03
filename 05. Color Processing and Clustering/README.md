# Color Processing

## 색의 3속성

- 색상 (색의 종류)
  - Hue
- 명도 (색이 얼마나 밝은지)
  - Brightness, value
- 채도 (색이 얼마나 진하고 순수한지)
  - Purity, saturation, chroma

## CIE XYZ Color Model

### CIE primary colors

![](https://upload.wikimedia.org/wikipedia/commons/thumb/8/8f/CIE_1931_XYZ_Color_Matching_Functions.svg/325px-CIE_1931_XYZ_Color_Matching_Functions.svg.png)

![](https://wikimedia.org/api/rest_v1/media/math/render/svg/f62d8f94207fbbf4909ab58734898c6fcae324d5)

### Normalized (by total energy) XYZ values

![](https://wikimedia.org/api/rest_v1/media/math/render/svg/873561ef50950673969394936959aced38d64188)<br>
![](https://wikimedia.org/api/rest_v1/media/math/render/svg/9f438636eedf7bdbe729e512052e76e1f6addaca)

- $x$, $y$ : chromaticity values (색을 결정하는 근본적인 값)
- $Y$ : luminance (빛)

### Chromaticity Diagram

![](https://en.wikipedia.org/wiki/File:CIE1931xy_blank.svg)

## CMYK Color Model

- Three primary colors
  - Cyan
    - Red 흡수 = Green, Blue 반사
  - Magenta
    - Green 흡수 = Red, Blue 반사
  - Yellow
    - Blue 흡수 = Red, Green 반사
- 혼합 사례
  - Cyan + Magenta
    - Red, Green 흡수 = Blue 반사
  - Cyan + Yellow
    - Red, Blue 흡수 = Green 반사

## YIQ Color Model

![](https://upload.wikimedia.org/wikipedia/commons/thumb/8/82/YIQ_IQ_plane.svg/300px-YIQ_IQ_plane.svg.png)

- Y 신호만을 사용하던 과거 흑백 TV와의 호환성을 유지하면서 컬러 신호를 표현하기 위해 I, Q 추가
  - I: Orange-Cyan (Flesh tone; 살색)
  - Q: Green-Magenta

## HSV Color Model

![](https://upload.wikimedia.org/wikipedia/commons/thumb/0/00/HSV_color_solid_cone_chroma_gray.png/197px-HSV_color_solid_cone_chroma_gray.png)

- 원색의 조합이 아닌 인간 직관에 보다 유사한 특성 이용
  - Hue, Saturation, Value (색의 3요소)

## HSI Color Model

![](img/HSI.png)

- 사람의 인지 모델과 유사
  - Hue, Saturation, Intensity(Lightness)
- HSV 모델과 유사하며, HSV 모델과 함께 computer vision에서 많이 이용됨

# References

1. 디지털영상처리설계 (ICE4027), 인하대학교 정보통신공학과 박인규
2. Digital Image Processing 4th Edition, Rafael C. Gonzalez and Richard E. Woods
3. [Computer Vision: Algorithms and Applications 2nd Edition, Richard Szeliski](https://szeliski.org/Book/)
4. [CIE 1931 color space — Wikipedia](https://en.wikipedia.org/wiki/CIE_1931_color_space)
5. [Color Model (RGB vs CMYK vs HSI ) — Suyeon's Blog](https://suyeon96.tistory.com/3#HSI)
6. [YIQ — Wikipedia](https://en.wikipedia.org/wiki/YIQ)
