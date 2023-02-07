## Performance Comparison on Set Implementations

### Guidelines

- General purposes with editing and enumeration
  - 1. If $n < 2^{12}$, use `ArrayBitset`.
  - 2. If $n \geq 2^{18}$, use `SortedVectorSet`.
  - 3. If $2^{12} \leq n < 2^{17}$, follow the table below.
    - $2^{12} \leq n < 2^{13}$: $d < 2^6$, use `SortedVectorSet`; otherwise, use `ArrayBitset`.
    - $2^{13} \leq n < 2^{14}$: $d < 2^6$, use `SortedVectorSet`; otherwise, use `ArrayBitset`.
    - $2^{14} \leq n < 2^{15}$: $d < 2^7$, use `SortedVectorSet`; otherwise, use `ArrayBitset`.
    - $2^{15} \leq n < 2^{16}$: $d < 2^8$, use `SortedVectorSet`; otherwise, use `ArrayBitset`.
    - $2^{16} \leq n < 2^{17}$: $d < 2^9$, use `SortedVectorSet`; otherwise, use `ArrayBitset`.
    - $2^{17} \leq n < 2^{17}$: $d < 2^{10}$, use `SortedVectorSet`; otherwise, use `ArrayBitset`.
- For temporary purposes without enumeration, use `FastSet`.
