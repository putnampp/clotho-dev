#ifndef CHROMOSOME_H_
#define CHROMOSOME_H_

struct chromosome {
    typedef unsigned int  chrom_id_t;
    virtual chrom_id_t  getID() const = 0;
    virtual ~chromosome() {}
};

#endif  // CHROMOSOME_H_
