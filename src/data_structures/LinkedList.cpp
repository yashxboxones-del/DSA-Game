// =============================================================
// LinkedList.cpp
// Implementation file for LinkedList data structure.
//
// In C++, template classes are usually header-defined, but this file
// provides explicit template instantiation for the game types
// and can host non-template helper functions.
// =============================================================

#include "data_structures/LinkedList.h"
#include "Enemy.h"

// Explicit instantiation for Enemy so the compiler generates
// the exact binary code for LinkedList<Enemy>.
template class LinkedList<Enemy>;
template class LinkedList<int>;
