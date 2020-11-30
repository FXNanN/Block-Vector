The main idea of the ArrayList container: 

Based on STL vector implementation. 

In order to reduce the memory copying consumption when inserting or deleting in a vector containing a large amount of data, the new container split the data into several small vectors, and the address of those vectors are recorded in a single vector.
