Project for Programming in C MSc module. Create a Binary Sparse Array (BSA). 

The Binary Sparse Array (BSA) which aims to be memory efficient for small arrays, releasing new memory at an exponential rate as the array grows. Using a BSA ensures that no data is ever copied (e.g. via remalloc) nor too much unused memory created early. The price we pay for this memory efficient is a slightly more complex calculation for the address of the index required.
