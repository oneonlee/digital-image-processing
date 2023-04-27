# Color Processing and Clustering

## Clustering

### K-Means Clustering

- Iteratively re-assign points to the nearest cluster center
- $\mathbf{c}^{*}, \mathbf{\delta}^{*} = \textup{argmin}_{c,\delta} \ \frac{1}{N}\sum_{j}^{N}\sum_{i}^{K}\delta_{ij}\mathit{\mathbf{\mathit{d}}\left (  \mathbf{c}_i, \mathbf{x}_j \right )^{2}}$
  - Goal: cluster to minimize variance in data given-clusters
- Steps
  - 1): Randomly select K centers
  - 2): [Expectation Step] Assign each point to nearest center
  - 3): [Minimize Step] Compute new center (mean) for each cluster
  - 4): Repeat from the [Expectation Step] to the [Minimize Step] until coverage

#### Hyperparater of K-Means Clustering

- K (군집의 수): 3, 5, ...
- Initial center: Randomly select, Greedily choose, ...
- Distance measures: L2 Norm, ..., ?
- Maximum of iterations

#### Pros & Cons

- Pros
  - 간단하고 빠르다.
  - 구현하기 쉽다.
- Cons
  - 수렴이 보장되지 않는다.
    - 수렴한다해도, global minimum이 아닌 local minimum으로 수렴할 가능성이 있다.
  - 초기 중심값 위치에 따라 원하는 결과가 나오지 않을 수도 있다.
  - 이상치(outlier) 데이터에 민감하다.
  - 정규화되지 않은 데이터들은 바로 사용할 수 없다.
  - 계산량이 많아 속도가 느리다. ($O(KNd)$ for $N$ $d$-dimensional points)
