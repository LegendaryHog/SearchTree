# My solution of HWT and funny adventure with Red-Black trees

## How to build?

```
cmake -B build/ -DCMAKE_CXX_COMPILER=[compiler] -DCMAKE_BUILD_TYPE=[Release/Debug]
cmake --build build/
```

## How to run solution?
(There and everywhere path is from root of repository)
### Main solution by BoostSet:
My modification of Set to problem HWT
```
cd build
./task/boost/boost
```
### Solytion by Set:
My poor copy of std::set:
```
cd build
./task/my_set/my_set
```

### Solution by std::set:
```
cd build
./task/std_set/std_set
```

## How to run google tests?
```
cd build
ctest
```

## How to generate end-to-end test?
```
cd test
chmod "+x" requests_gen.py
./requests_gen.py [number of requests] [names of file to put tests]
```

## How to test answers of three solutions?
```
cd test
chmod "+x" cmp
./cmp -c [path from this directory to build/] [files with tests]
```

## How to test time of work?
```
cd test
chmod "+x" timecmp
./timecmp -c [path from this directory to build/] [files with tests]
```

On tests with 100.000 requests results is:

|Set:    | Time:      |
|--------|------------|
|boost   | ~ 0.284sec |
|my_set  | ~ 10.867sec|
|std_set | ~ 11.248sec|

You can see that BoostSet beat and destroy Set and std::set like childrens, but I'm very surprised that my own Set beat std::set by result of 10 random tests. All tests was run in Release version, in Debug std::set solve test in average of 14sec, my Set solve in average of 20sec.


## class RBSearchTree
It is a base for Container::Set and Container::BoostSet, it is a Red-Black self-balancing tree, all algorithm based on Introduction to Algorithms by Thomas H. Cormen, I did so much comments for all steps of algorithm (expect fixing invariants after erase) and made pictures for betted understanding. RBSearchTree locates in `include/search_tree.hpp`, Iterator locates in `search_tree_iterator.hpp`. I deligate work in rebalance invariants of given Node to Node (if this Node had any invariants). I do that to simplify writing other container basing on RBSearchTree. RBSearchTree has no public members and exist only to be base for other containers.

## class Set
It is poor (but strong) copy od std::set. Locates in `include/set.hpp`

## class BoostSet
It is Set made for problem HWT, this tree use not common R-B Node, Node of this tree keep size of own SubTree to speed up search of k-th smallest and count of elements smaller than given. It has two methods for this and overloaded operators[] and operator->*(for fun) to this tasks. But You can't erase from this set, because save size of sub tree while erase fix up is impossible (or to difficult for me).
Locates in `include/boost_set.hpp`, node for this tree in `include/boost_node.hpp`