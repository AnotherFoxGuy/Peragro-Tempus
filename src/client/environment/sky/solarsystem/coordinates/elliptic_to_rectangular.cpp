

#include "elliptic_to_rectangular.h"

#include "client/environment/sky/utils/time.h"

#include <math.h>

static void
EllipticToRectangular(const double mu,const double a,const double n,
                      const double elem[6],const double dt,double xyz[]) {
  const double L = fmod(elem[1]+n*dt,2.0*M_PI);
    /* solve Keplers equation
        x = L - elem[2]*sin(x) + elem[3]*cos(x)
      not by trivially iterating
        x_0 = L
        x_{j+1} = L - elem[2]*sin(x_j) + elem[3]*cos(x_j)
      but instead by Newton approximation:
        0 = f(x) = x - L - elem[2]*sin(x) + elem[3]*cos(x)
        f'(x) = 1 - elem[2]*cos(x) - elem[3]*sin(x)
        x_0 = L or whatever, perhaps first step of trivial iteration
        x_{j+1} = x_j - f(x_j)/f'(x_j)
    */
  double Le = L - elem[2]*sin(L) + elem[3]*cos(L);
  for (;;) {
    const double cLe = cos(Le);
    const double sLe = sin(Le);
      /* for excenticity < 1 we have denominator > 0 */
    const double dLe = (L - Le + elem[2]*sLe - elem[3]*cLe)
                     / (1.0    - elem[2]*cLe - elem[3]*sLe);
    Le += dLe;
    if (fabs(dLe) <= 1e-14) break; /* L1: <1e-12 */
  }

  const double cLe = cos(Le);
  const double sLe = sin(Le);

  const double dlf = -elem[2]*sLe + elem[3]*cLe;
  const double phi = sqrt(1.0 - elem[2]*elem[2] - elem[3]*elem[3]);
  const double psi = 1.0 / (1.0 + phi);

  const double x1 = a * (cLe - elem[2] - psi*dlf*elem[3]);
  const double y1 = a * (sLe - elem[3] + psi*dlf*elem[2]);

  const double elem_4q = elem[4] * elem[4];
  const double elem_5q = elem[5] * elem[5];
  const double dwho = 2.0 * sqrt(1.0 - elem_4q - elem_5q);
  const double rtp = 1.0 - elem_5q - elem_5q;
  const double rtq = 1.0 - elem_4q - elem_4q;
  const double rdg = 2.0 * elem[5] * elem[4];

  xyz[0] = x1 * rtp + y1 * rdg;
  xyz[1] = x1 * rdg + y1 * rtq;
  xyz[2] = (-x1 * elem[5] + y1 * elem[4]) * dwho;

/*
  const double rsam1 = -elem[2]*cLe - elem[3]*sLe;
  const double h = a*n / (1.0 + rsam1);
  const double vx1 = h * (-sLe - psi*rsam1*elem[3]);
  const double vy1 = h * ( cLe + psi*rsam1*elem[2]);

  xyz[3] = vx1 * rtp + vy1 * rdg;
  xyz[4] = vx1 * rdg + vy1 * rtq;
  xyz[5] = (-vx1 * elem[5] + vy1 * elem[4]) * dwho;
*/
}

void EllipticToRectangularN(double mu,const double elem[6],double dt,
                            double xyz[]) {
  const double n = elem[0];
  const double a = pow((mu/(n*n)), 1/3 );
  EllipticToRectangular(mu,a,n,elem,dt,xyz);
}

void EllipticToRectangularA(double mu,const double elem[6],double dt,
                            double xyz[]) {
  const double a = elem[0];
  const double n = sqrt(mu/(a*a*a));
  EllipticToRectangular(mu,a,n,elem,dt,xyz);
}

