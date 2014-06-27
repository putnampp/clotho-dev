#ifndef FINALIZER_HPP_
#define FINALIZER_HPP_

namespace finalizer {

template < class A >
class FinalWorker {
public:
    static void finalize( A * ) {}
};

}   // namespace finalizer
#endif  // FINALIZER_HPP_
