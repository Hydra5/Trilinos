// ***********************************************************************
// 
//                           Stokhos Package
//                 Copyright (2008) Sandia Corporation
// 
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
// 
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1 of the
// License, or (at your option) any later version.
//  
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//  
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
// Questions? Contact Christopher W. Miller (cmiller@math.umd.edu).
// 
// ***********************************************************************
// @HEADER

#include "Stokhos_ConfigDefs.h"
#ifdef HAVE_STOKHOS_DAKOTA
#include "sandia_rules.H"
#elif HAVE_STOKHOS_FORUQTK
#include "Stokhos_gaussq.h"
#else
#include "Teuchos_TestForException.hpp"
#endif

#include "Teuchos_LAPACK.hpp"
#include <Teuchos_SerialDenseMatrix.hpp>
#include <Teuchos_SerialDenseVector.hpp>

#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

template <typename ordinal_type, typename value_type>
Stokhos::RysBasis<ordinal_type,value_type>::
RysBasis(ordinal_type p , value_type c, bool normalize) :
  OneDOrthogPolyBasisBase<ordinal_type,value_type>("Rys",p)
{ 
//The Rys polynomials are defined by a recurrance relation,
//P_n+1 = \gamma_n+1[(x-\alpha_n) P_n - \beta_n P_n-1].
//The alpha and beta coefficients are generated first using the
//discritized stilges procidure described in "On the Calculation of Rys Polynomials and Quadratures",
//Robin P. Sagar, Vedene H. Smith.  The gamma coefficients are then optionally set so that each
//polynomial has norm 1.  If normalization is not enabled then the gammas are set to 1.
  this->scaleFactor = 1;
  this->cutoff = c;
  std::vector<value_type> alpha(p+1,0);
  std::vector<value_type> beta(p+1,0);
  std::vector<value_type> gamma(p+1,1);
  this->alpha = alpha;
  this->beta = beta;
  this->gamma = gamma;
  this->normalize = normalize;
 
  //First renormalize the weight function so that it has measure 1.
  value_type cut_gauss_norm;
  //\int_{-c}^c exp(-x^2/2) dx
  cut_gauss_norm = expectedValue_J_nsquared(0);
  //future evaluations of the weight function will scale it by this factor.
  this->scaleFactor = 1/cut_gauss_norm;
  
  
  value_type integral2;
  //NOTE!! This evaluation of 'expectedValue_J_nsquared(0)' is different
  //from the one above since we rescaled the weight.  Don't combine
  //the two!!!

  value_type past_integral = expectedValue_J_nsquared(0);
  this->alpha[0] = expectedValue_tJ_nsquared(0)/past_integral;
  //beta[0] := \int_-c^c w(x) dx.
  this->beta[0] = 1;
  //These formulas are from the above reference.
  for(ordinal_type n = 1; n<=p; n++){
    integral2 = expectedValue_J_nsquared(n);
    this->alpha[n] = expectedValue_tJ_nsquared(n)/integral2;
    this->beta[n] = integral2/past_integral;
    past_integral = integral2;
  }
 
  
  // Compute norms: ||P_n||^2 = beta[0]*beta[1]*...*beta[n]
  this->norms[0] = this->beta[0];
  for (ordinal_type k=1; k<=this->p; k++){
    this->norms[k] = this->beta[k]*(this->norms[k-1]);
  } 

  //If you want normalized polynomials, set gamma and reset the norms to 1.
  if( normalize ){
    for (ordinal_type k=1; k<=this->p; k++){
        this->gamma[k] = 1/sqrt(this->norms[k]);
        this->norms[k] = 1;
      }
  }
  
  
  // Fill in basis coefficients using the recurrance relation.
  this->basis[0].coeff(0) = value_type(1.0);
  if (this->p >= 1){
    this->basis[1].coeff(1) = value_type(1);
    this->basis[1].coeff(0) = -this->alpha[0];
  }
  
  for (ordinal_type k=2; k<=this->p; k++) {
    this->basis[k].coeff(0) = (-this->alpha[k-1]*(this->basis[k-1].coeff(0)) - value_type(this->beta[k-1])*(this->basis[k-2].coeff(0)));
    for (ordinal_type i=1; i<=k; i++)
      this->basis[k].coeff(i) = 
	(this->basis[k-1].coeff(i-1) - value_type(this->alpha[k-1])*(this->basis[k-1].coeff(i)) - value_type(this->beta[k-1])*(this->basis[k-2].coeff(i)));
  }
  

  for (ordinal_type k = 0; k<=this->p; k++) {
    for( ordinal_type i=0; i<=k; i++) this->basis[k].coeff(i) = this->basis[k].coeff(i) * this->gamma[k];
  }
  
}

template <typename ordinal_type, typename value_type>
Stokhos::RysBasis<ordinal_type,value_type>::
RysBasis(ordinal_type p , value_type c, const std::vector< value_type >& alpha, const std::vector< value_type >& beta, bool normalize) :
  OneDOrthogPolyBasisBase<ordinal_type,value_type>("Rys",p)
{ 
//Alternative constructor for when we already know the first n recurrance coefficients.
  this->scaleFactor = 1;
  this->cutoff = c;
  //std::vector<value_type> alpha(p+1,0);
  //std::vector<value_type> beta(p+1,0);
  std::vector<value_type> gamma(p+1,1);
  ordinal_type known_recurrance = alpha.size();
  this->normalize = normalize;
  this->alpha = alpha;
  this->beta = beta;
  this->gamma = gamma;
  
  this->alpha.resize(p+1);
  this->beta.resize(p+1);
  
  //First renormalize the weight function so that it has measure 1.
  value_type cut_gauss_norm;
  //\int_{-c}^c exp(-x^2/2) dx
  cut_gauss_norm = expectedValue_J_nsquared(0);
  //future evaluations of the weight function will scale it by this factor.
  this->scaleFactor = 1/cut_gauss_norm;
  
  value_type integral2;
  //NOTE!! This evaluation of 'expectedValue_J_nsquared(0)' is different
  //from the one above since we rescaled the weight.  Don't combine
  //the two!!!
  value_type past_integral = expectedValue_J_nsquared(known_recurrance-2); 
  //These formulas are from the above reference.
  
  for(ordinal_type n = known_recurrance-1; n<=p; n++){
    integral2 = expectedValue_J_nsquared(n);
    this->alpha[n] = expectedValue_tJ_nsquared(n)/integral2;
    this->beta[n] = integral2/past_integral;
    past_integral = integral2;
  }
 
  
  // Compute norms: ||P_n||^2 = beta[0]*beta[1]*...*beta[n]
  this->norms[0] = this->beta[0];
  for (ordinal_type k=1; k<=this->p; k++){
    this->norms[k] = this->beta[k]*(this->norms[k-1]);
  } 

  //If you want normalized polynomials, set gamma and reset the norms to 1.
  if( normalize ){
    for (ordinal_type k=1; k<=this->p; k++){
        this->gamma[k] = 1/sqrt(this->norms[k]);
        this->norms[k] = 1;
      }
  }
  
  
  // Fill in basis coefficients using the recurrance relation.
  this->basis[0].coeff(0) = value_type(1.0);
  if (this->p >= 1){
    this->basis[1].coeff(1) = value_type(1);
    this->basis[1].coeff(0) = -this->alpha[0];
  }
  
  for (ordinal_type k=2; k<=this->p; k++) {
    this->basis[k].coeff(0) = (-this->alpha[k-1]*(this->basis[k-1].coeff(0)) - value_type(this->beta[k-1])*(this->basis[k-2].coeff(0)));
    for (ordinal_type i=1; i<=k; i++)
      this->basis[k].coeff(i) = 
	(this->basis[k-1].coeff(i-1) - value_type(this->alpha[k-1])*(this->basis[k-1].coeff(i)) - value_type(this->beta[k-1])*(this->basis[k-2].coeff(i)));
  }
  

  for (ordinal_type k = 0; k<=this->p; k++) {
    for( ordinal_type i=0; i<=k; i++) this->basis[k].coeff(i) = this->basis[k].coeff(i) * this->gamma[k];
  }
  
}

template <typename ordinal_type, typename value_type>
Stokhos::RysBasis<ordinal_type,value_type>::
~RysBasis()
{
}

template <typename ordinal_type, typename value_type>
value_type 
Stokhos::RysBasis<ordinal_type,value_type>::
evaluateWeight(value_type x) const
{
  value_type result;

  //w(x) = K \chi_[-c,c] exp(-x^2/2).  With K chosen so that
  //\int_-inf^inf w(x) = 1.
  if (x > this->cutoff || -x > this->cutoff) {
    result = 0;
  }else{
    result = this->scaleFactor*exp(-pow(x,2)/2);
  }
  return result;
} 

template <typename ordinal_type, typename value_type>
void
Stokhos::RysBasis<ordinal_type,value_type>::
projectPoly(const Stokhos::Polynomial<value_type>& x, std::vector<value_type>& coeffs) const
{
  //This method not yet implimented.
}

template <typename ordinal_type, typename value_type>
void
Stokhos::RysBasis<ordinal_type,value_type>::
projectDerivative(ordinal_type i, std::vector<value_type>& coeffs) const
{
  //This method not yet implimented.
}

template <typename ordinal_type, typename value_type>
void
Stokhos::RysBasis<ordinal_type,value_type>::
evaluateBases(const value_type& x, std::vector<value_type>& basis_pts) const
{
  // Evaluate basis polynomials P(x) using 3 term recurrence
  // P_0(x) = 1 P_-1 = 0
  // P_i(x) = gamma[i]*[(x-alpha[i-1])*P_{i-1}(x) - beta[i-1]*P_{i-2}(x)], i=2,3,...
  basis_pts[0] = value_type(1.0);
  if (this->p >= 1){
    basis_pts[1] = (x-this->alpha[0])*basis_pts[0];
  }
  for (ordinal_type i=2; i<=this->p; i++){
    basis_pts[i] = (x-this->alpha[i-1])*basis_pts[i-1] - this->beta[i-1]*basis_pts[i-2];
  }
  for (ordinal_type i = 0; i<=this->p; i++){
    basis_pts[i] = this->gamma[i]*basis_pts[i];
  }
}

template <typename ordinal_type, typename value_type>
value_type
Stokhos::RysBasis<ordinal_type,value_type>::
evaluateBasesOrder_p(const value_type& x, const ordinal_type& order) const
{
  // Evaluate basis polynomials P(x) using 3 term recurrence
  // P_0(x) = 1, P_-1 = 0
  // P_i(x) = gamma[i]*[(x-alpha[i-1])*P_{i-1}(x) - beta[i-1]*P_{i-2}(x)], i=2,3,...
  value_type value;
  std::vector<value_type> basis_pts(order+1,0);
  basis_pts[0] = value_type(1.0);
  if (order >= 1){
    basis_pts[1] = (x-this->alpha[0])*basis_pts[0];
  }
  for (ordinal_type i=2; i<=order; i++){
    basis_pts[i] = (x-this->alpha[i-1])*basis_pts[i-1] - this->beta[i-1]*basis_pts[i-2];
  }
  value = this->gamma[order]*basis_pts[order];
  return value;
}


template <typename ordinal_type, typename value_type>
value_type
Stokhos::RysBasis<ordinal_type,value_type>::
expectedValue_tJ_nsquared(const ordinal_type& order) const
{
  //Impliments a clenshaw curtis quadrature routine to evaluate the integral,
  // \int_-c^c xJ_n(x^2)w(x)dx.  This is needed to compute the recurrance coefficients.
  //Note that for a symmetric measure this integral is zero but I wanted to keep things
  //general.
  ordinal_type p = (100<2*order)?order:100;
  value_type c = this->cutoff;
  value_type * cc_rule = new value_type[2*p+1];
  value_type value_type_p = (value_type) p;
  value_type N = 2*value_type_p;
  //Compute 1/(\int_{-c}^c exp(-x^2/2) dx)
  for (ordinal_type k=0; k<=2*p; k++) {
    cc_rule[k] = (1/N)*(1*pow(this->evaluateBasesOrder_p(c,order),2)*this->evaluateWeight(c) - 1*pow(this->evaluateBasesOrder_p(-c,order),2)*this->evaluateWeight(-c));
  }
  for (ordinal_type k=0; k<=2*p; k++) {
    for (ordinal_type n=1; n<p; n++) {
      cc_rule[k] = cc_rule[k] + (cos(n*PI/N)*pow(this->evaluateBasesOrder_p(c*cos(n*PI/N),order),2)*this->evaluateWeight(c*cos(n*PI/N)) - cos(n*PI/N)*pow(this->evaluateBasesOrder_p(-c*cos(n*PI/N),order),2)*this->evaluateWeight(-c*cos(n*PI/N))) * cos(2*n*k*PI/N);
    }
  }
  value_type integral = cc_rule[0];
  for (ordinal_type i=1; i<=2*p; i++){
    integral = integral + (2*cc_rule[i]/(1-4*pow(i,2)));
  }
  integral = pow(c,2)*integral;
  return integral;
}

template <typename ordinal_type, typename value_type>
value_type
Stokhos::RysBasis<ordinal_type,value_type>::
expectedValue_J_nsquared(const ordinal_type& order) const
{
  //Impliments a clenshaw curtis quadrature routine to evaluate the integral,
  // \int_-c^c J_n(x)^2w(x)dx.  This is needed to compute the recurrance coefficients.
  ordinal_type p = (100<2*order)?order:100;
  value_type c = this->cutoff;
  value_type * cc_rule = new value_type[2*p+1];
  value_type value_type_p = (value_type) p;
  value_type N = 2*value_type_p;
  for (ordinal_type k=0; k<=2*p; k++) {
    cc_rule[k] = (1/N)*(pow(this->evaluateBasesOrder_p(c,order),2)*this->evaluateWeight(c) + pow(this->evaluateBasesOrder_p(-c,order),2)*this->evaluateWeight(-c));
    cc_rule[k] = cc_rule[k] + (2/N)*pow(-1,k)*pow(this->evaluateBasesOrder_p(0,order),2)*this->evaluateWeight(0);
  }
  for (ordinal_type k=0; k<=2*p; k++) {
    for (ordinal_type n=1; n<p; n++) {
      cc_rule[k] = cc_rule[k] + (2/N)*(pow(this->evaluateBasesOrder_p(c*cos(n*PI/N),order),2)*this->evaluateWeight(c*cos(n*PI/N)) + pow(this->evaluateBasesOrder_p(-c*cos(n*PI/N),order),2)*this->evaluateWeight(-c*cos(n*PI/N))) * cos(2*n*k*PI/N);
    }
  }
  value_type integral = cc_rule[0];
  for (ordinal_type i=1; i<=2*p; i++){
    integral = integral + (2*cc_rule[i]/(1-4*pow(i,2)));
  }
  integral = c*integral;
  return integral;
}

template <typename ordinal_type, typename value_type>
value_type 
Stokhos::RysBasis<ordinal_type, value_type>::
eval_inner_product(const ordinal_type& order1, const ordinal_type& order2) const
{
  ordinal_type p = (100<2*order1)?order1:100;
  value_type c = this->cutoff;
  value_type * cc_rule = new value_type[2*p+1];
  value_type value_type_p = (value_type) p;
  value_type N = 2*value_type_p;
  for (ordinal_type k=0; k<=2*p; k++) {
    cc_rule[k] = (1/N)*(this->evaluateBasesOrder_p(c,order1)*this->evaluateBasesOrder_p(c,order2)*this->evaluateWeight(c) + this->evaluateBasesOrder_p(-c,order1)*this->evaluateBasesOrder_p(-c,order2)*this->evaluateWeight(-c));
    cc_rule[k] = cc_rule[k] + (2/N)*pow(-1,k)*this->evaluateBasesOrder_p(0,order1)*this->evaluateBasesOrder_p(0,order2)*this->evaluateWeight(0);
  }
  for (ordinal_type k=0; k<=2*p; k++) {
    for (ordinal_type n=1; n<p; n++) {
      cc_rule[k] = cc_rule[k] + (2/N)*(this->evaluateBasesOrder_p(c*cos(n*PI/N),order1)*this->evaluateBasesOrder_p(c*cos(n*PI/N),order2)*this->evaluateWeight(c*cos(n*PI/N)) + this->evaluateBasesOrder_p(-c*cos(n*PI/N),order1)*this->evaluateBasesOrder_p(-c*cos(n*PI/N),order2)*this->evaluateWeight(-c*cos(n*PI/N))) * cos(2*n*k*PI/N);
    }
  }
  value_type integral = cc_rule[0];
  for (ordinal_type i=1; i<=2*p; i++){
    integral = integral + (2*cc_rule[i]/(1-4*pow(i,2)));
  }
  integral = c*integral;
  return integral;
}

template <typename ordinal_type, typename value_type>
void
Stokhos::RysBasis<ordinal_type,value_type>::
getQuadPoints(ordinal_type quad_order,
	      std::vector<value_type>& quad_points,
	      std::vector<value_type>& quad_weights,
	      std::vector< std::vector<value_type> >& quad_values) const
{
  //This is a transposition into C++ of Gautschi's code for taking the first N recurrance coefficients
  //and generating a N point quadrature rule.  The MATLAB version is available at
  // http://www.cs.purdue.edu/archives/2002/wxg/codes/gauss.m
  ordinal_type num_points = quad_order+1;
  std::vector<value_type> alpha(num_points,0);
  std::vector<value_type> beta(num_points,0);
  Teuchos::RCP<const Stokhos::RysBasis<int,double> > basis;
  //If we don't have enough recurrance coefficients, get some more.
  if(num_points > this->p+1){
    
    basis = Teuchos::rcp(new Stokhos::RysBasis<int,double>(quad_order,this->cutoff,this->alpha,this->beta,this->normalize));
    basis->getAlpha(alpha);
    basis->getBeta(beta);
  }else{  //else just take the ones we already have.
    for(ordinal_type n = 0; n<num_points; n++){
      alpha[n] = this->alpha[n];
      beta[n] = this->beta[n];
    }
  }
  
  //A is symmetric and tridiagonal with A[i][i] = alpha[i] and A[i-1][i] = A[i][i-1] = sqrt(beta[i]) 
  Teuchos::SerialDenseMatrix<ordinal_type, value_type> A(num_points,num_points);
  for( ordinal_type i=0; i<num_points; i++) A(i,i) = alpha[i];
  for( ordinal_type i=1; i<num_points; i++){ 
    A(i,i-1) = sqrt(beta[i]);
    A(i-1,i) = sqrt(beta[i]);
  }

  Teuchos::SerialDenseMatrix<ordinal_type,value_type> eig_vectors(num_points,num_points);
  
  Teuchos::SerialDenseVector<ordinal_type,value_type> workspace(8*num_points);
  ordinal_type info_flag;
  Teuchos::LAPACK<ordinal_type,value_type> my_lapack;
  Teuchos::SerialDenseVector<ordinal_type,value_type> eig_real(num_points);
  Teuchos::SerialDenseVector<ordinal_type,value_type> eig_imag(num_points);
  
  //compute the eigenvalues and right eigenvectors.
  my_lapack.GEEV('N', 'V', num_points, A.values(), num_points,eig_real.values(),eig_imag.values(), A.values(), num_points,eig_vectors.values(),num_points,workspace.values(),8*num_points,&info_flag);
  
  quad_points.resize(num_points);
  quad_weights.resize(num_points);
  
  //the eigenvalues are the quadrature points, sort these and keep track of the indices.
  std::vector< ordinal_type > idx(num_points,0);
  value_type temp1,temp2;
  for(ordinal_type i = 0; i< num_points; i++) idx[i] = i;
  for(ordinal_type i = 0; i< num_points; i++){
    for(ordinal_type j = 0; j< num_points; j++){
      if(eig_real[i]< eig_real[j]){
        temp1 = eig_real[j];
        temp2 = idx[j];
        eig_real[j] = eig_real[i];
        idx[j] = idx[i];
        eig_real[i] = temp1;
        idx[i] = temp2;
      }
    }
  }
  
  
  for(ordinal_type i = 0; i< num_points; i++){
    quad_points[i] = eig_real[i];
    quad_weights[i] = beta[0]*pow(eig_vectors[idx[i]][0],2);
  }

  // Evalute basis at gauss points
  quad_values.resize(num_points);
  for (ordinal_type i=0; i<num_points; i++) {
    quad_values[i].resize(num_points);
    if (num_points > this->p+1){
      basis->evaluateBases(quad_points[i], quad_values[i]);
    }else{
      evaluateBases(quad_points[i], quad_values[i]);
    }
  }

   /*std::cout << "values = " << std::endl;
   for (ordinal_type i=0; i<num_points; i++) {
     //std::cout << "\t" << quad_points[i] 
               //<< "\t" << quad_weights[i];
     for (ordinal_type j=0; j<num_points; j++)
       std::cout << "\t" << quad_values[i][j];
     std::cout << std::endl;
   }
*/
}

template <typename ordinal_type, typename value_type>
Teuchos::RCP< const Stokhos::Dense3Tensor<ordinal_type, value_type> >
Stokhos::RysBasis<ordinal_type, value_type>::
getTripleProductTensor() const
{
  
  ordinal_type sz = this->size();
  
  // Compute Cijk = < \Psi_i \Psi_j \Psi_k >
  if (this->Cijk == Teuchos::null) {
    std::vector<value_type> points, weights;
    std::vector< std::vector<value_type> > values;
    getQuadPoints(ceil((3*this->p+1)/2), points, weights, values);
    this->Cijk = Teuchos::rcp(new Dense3Tensor<ordinal_type, value_type>(sz));
    
    
    for (ordinal_type i=0; i<sz; i++) {
      for (ordinal_type j=0; j<sz; j++) {
	for (ordinal_type k=0; k<sz; k++) {
          value_type triple_product = 0;
	  for (ordinal_type l=0; l<points.size();l++){
             triple_product = triple_product + weights[l]*(values[l][i])*(values[l][j])*(values[l][k]);
             //if(k == 0) std::cout<< "values[0]["<<l<<"] = "<<values[l][k] <<"\n";
          }
          (*this->Cijk)(i,j,k) = triple_product;
          //if(i == 0 && j == 0 && k == 0) std::cout<< "C000 = " << (*Cijk)(i,j,k) << "\n";
          
	}
      }
    }
  }

  return this->Cijk;
}
