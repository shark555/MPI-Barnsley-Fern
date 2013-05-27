#ifndef BARNSLEY_H
#define	BARNSLEY_H

class Barnsley {
public:
    Barnsley();
    Barnsley(const Barnsley& orig);
    virtual ~Barnsley();
    double** generate();
private:
    double* f(double x,double y);
    double* g(double x,double y); 
    double* h(double x,double y);
    double* j(double x,double y);    
};

#endif	/* BARNSLEY_H */

