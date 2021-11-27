#ifndef heun_solver_h
#define heun_solver_h

#include "expression.h"

#include <cmath>
#include <vector>


template<typename type>
class HeunSolver{
private:
    unsigned int n;
    Exp<type>* expression;
    std::vector<type> P;
    std::vector<type> P_m;
    std::vector<type> P_f1;
    std::vector<type> P_f2;
    std::vector<std::vector<type>> P_w;
    std::vector<type> Force;
    std::vector<type> E;
    std::vector<type> T;
    type time;

public:
    HeunSolver() {}

    void take_exp(Exp<type>* exp, std::vector<type> P0)
    {
        n = exp->get_n();
        expression = exp;
        P = P0;
        std::vector<type> p_f1(n, 0.0);
        std::vector<type> p_f2(n, 0.0);
        std::vector<type> p_m(n, 0.0);

        P_f1 = p_f1;
        P_f2 = p_f2;
        P_m = p_m;
        time = 0.0;
    }

    inline
    void iter(type dt)
    {
        expression->f(P_f1, time, P);
        for(int i=0; i<n; ++i)
            P_m[i] = P[i] + P_f1[i]*dt;
        expression->f(P_f2, time+dt, P_m);
        //std::cout << P_f2[0] << ' ' << P_f2[1] << '\n';
        for(int i=0; i<n; ++i)
            P[i] += (P_f1[i] + P_f2[i])*dt*0.5;
    }

    void solve(type time0, type dt, type tau)
    {
        long long int iterations = static_cast<long long int> (time0/dt);
        long long int write = static_cast<long long int> (tau/dt);
        size_t size = static_cast<size_t> (time0/tau);
        std::vector<std::vector<type>> p_w(size, std::vector<type> (n, 0.0));
        std::vector<type> force(size, 0.0);
        std::vector<type> e(size, 0.0);
        std::vector<type> t(size, 0.0);
        int I = 0;
        int j = 1;
        p_w[0] = P;
        force[0] = expression->force(0.0, P);
        e[0] = expression->energy(P);
        t[0] = 0.0;
        for(long long int i=0; i<iterations; ++i){
            if (I == write){
                p_w[j] = P;
                //std::cout << P[0] << ' ' << P[1] << '\n';
                force[j] = expression->force(time, P);
                e[j] = expression->energy(P);
                t[j] = i*dt;
                ++j;
                I = 0;
            }
            iter(dt);
            time = (i+1)*dt;
            ++I;
        }
        P_w = p_w;
        Force = force;
        E = e;
        T = t;
    }

    std::vector<type> get_time() {return T;}

    std::vector<std::vector<type>> get_values() {return P_w;}

    std::vector<type> get_force() {return Force;}

    std::vector<type> get_energy() {return E;}
};

#endif // heun_solver_h
