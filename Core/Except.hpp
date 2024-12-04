/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef EXCEPT_HPP
#define EXCEPT_HPP

// any code related to the use of C++ exceptions
// should be kept here. Note that this project does NOT
// use exceptions as an error handling mechanism, but
// many common interfaces like the STL do, and thus there
// will still be times where I will need tools for them.

#define IGNORE_EXCEPT(EXPR) try{ EXPR } catch(...) {/*...*/}

#endif //EXCEPT_HPP
