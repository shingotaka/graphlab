/**  
 * Copyright (c) 2009 Carnegie Mellon University. 
 *     All rights reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an "AS
 *  IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 *  express or implied.  See the License for the specific language
 *  governing permissions and limitations under the License.
 *
 * For more about this software visit:
 *
 *      http://www.graphlab.ml.cmu.edu
 *  
 *  Code written by Danny Bickson, CMU
 *  Any changes to the code must include this original license notice in full.
 *  This file implements the shooting algorithm for solving Lasso problem
 */


#ifndef _COSAMP_HPP
#define _COSAMP_HPP

#include <itpp/base/sort.h>

extern bool debug;

ivec sort_union(ivec a, ivec b){
   ivec ab = concat(a,b);
   Sort<int> sorter;
   sorter.sort(0, ab.size()-1, ab);
   for (int i=1; i< ab.size(); i++){
      if (ab[i] == ab[i-1])
        ab.del(i);
   }
   return ab;
}


vec CoSaMP(mat Phi, vec u, int K, int max_iter, double tol1, int D){

  assert(K<= 2*D);
  assert(K>=1);

  assert(Phi.rows() == Phi.cols());
  assert(Phi.rows() == D);
  assert(u.size() == D);
  

  vec Sest = zeros(D);
  vec utrue = Sest;
  vec v = u;
  int t=1;
  ivec T2;
  Sort<double> sorter;

  while (t<max_iter){
    ivec z = sorter.sort_index(0, u.size()-1, abs(Phi.transpose() * v));
    z = reverse(z);
    ivec Omega = z(0,2*K-1);
    ivec T=sort_union(Omega,T2);
    mat phit=Phi.get_cols(T);
    vec b;
    bool ret = itpp::backslash(phit, u, b);
    assert(ret);
    b= abs(b);
    ivec z3 = sorter.sort_index(0, b.size()-1, b);
    z3 = reverse(z3);
    Sest=zeros(D);
    for (int i=0; i< K; i++)
       Sest.set(z3[i], b[z3[i]]);
    ivec z2 = sorter.sort_index(0,Sest.size()-1, abs(Sest));
    z2 = reverse(z2);
    T2 = z2(0,K-1);
    v=u-Phi*Sest;
    double n2 = max(abs(v));
    if (n2 < tol1)
        break;
    t++;
  }
  return Sest;

}



void test_cosamp(){

   mat A="0.9528    0.5982    0.8368 ; 0.7041    0.8407    0.5187; 0.9539    0.4428    0.0222";
   vec b=" 0.3759 0.8986 0.4290";
   int K=1;
   double epsilon =1e-3;
   vec ret = CoSaMP(A,b,K,10, epsilon,3);
   double diff = norm(ret - vec("0 1.2032 0"));
   assert(diff <1e-4);

}



#endif