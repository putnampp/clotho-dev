#ifndef INITIALIZER_HPP_
#define INITIALIZER_HPP_

namespace initializer {

template < class A >
class InitWorker {
public:
    static void init( A * a ) {}
};

}   // namespace initializer

#endif  // INITIALIZER_HPP_
