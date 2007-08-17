/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 1999-2007 Soeren Sonnenburg
 * Written (W) 1999-2007 Gunnar Raetsch
 * Copyright (C) 1999-2007 Fraunhofer Institute FIRST and Max-Planck-Society
 */

#ifndef _COMMWORDSTRINGKERNEL_H___
#define _COMMWORDSTRINGKERNEL_H___

#include "lib/common.h"
#include "lib/Mathematics.h"
#include "kernel/StringKernel.h"

class CCommWordStringKernel: public CStringKernel<WORD>
{
 public:
  CCommWordStringKernel(LONG size, bool use_sign, ENormalizationType normalization_=FULL_NORMALIZATION );
  CCommWordStringKernel(CStringFeatures<WORD>* l, CStringFeatures<WORD>* r, bool use_sign=false, ENormalizationType normalization_=FULL_NORMALIZATION, INT size=10);
  ~CCommWordStringKernel();
  
  virtual bool init(CFeatures* l, CFeatures* r);
  virtual void cleanup();

  /// load and save kernel init_data
  bool load_init(FILE* src);
  bool save_init(FILE* dest);

  // return what type of kernel we are Linear,Polynomial, Gaussian,...
  virtual EKernelType get_kernel_type() { return K_COMMWORDSTRING; }

  // return the name of a kernel
  virtual const CHAR* get_name() { return "CommWordString"; }

  virtual bool init_dictionary(INT size);
  virtual bool init_optimization(INT count, INT *IDX, DREAL * weights);
  virtual bool delete_optimization();
  virtual DREAL compute_optimized(INT idx);

  virtual void add_to_normal(INT idx, DREAL weight);
  virtual void clear_normal();

  virtual void remove_lhs();
  virtual void remove_rhs();

  inline virtual EFeatureType get_feature_type() { return F_WORD; }
  
  void get_dictionary(INT& dsize, DREAL*& dweights) 
  {
	  dsize=dictionary_size;
	  dweights = dictionary_weights;
  }

  virtual DREAL* compute_scoring(INT max_degree, INT& num_feat, INT& num_sym, 
		  DREAL* target, INT num_suppvec, INT* IDX, DREAL* alphas, bool do_init=true);

  CHAR* compute_consensus(INT &num_feat, INT num_suppvec, INT* IDX, DREAL* alphas);

 protected:
  /// compute kernel function for features a and b
  /// idx_{a,b} denote the index of the feature vectors
  /// in the corresponding feature object
  inline DREAL compute(INT idx_a, INT idx_b)
  {
	  return compute_helper(idx_a, idx_b, false);
  }

  DREAL compute_helper(INT idx_a, INT idx_b, bool do_sort);

  inline DREAL normalize_weight(DREAL* weights, DREAL value, INT seq_num, INT seq_len, ENormalizationType p_normalization)
  {
	  switch (p_normalization)
	  {
		  case NO_NORMALIZATION:
			  return value;
			  break;
		  case SQRT_NORMALIZATION:
			  return value/sqrt(weights[seq_num]);
			  break;
		  case FULL_NORMALIZATION:
			  return value/weights[seq_num];
			  break;
		  case SQRTLEN_NORMALIZATION:
			  return value/sqrt(sqrt((double) seq_len));
			  break;
		  case LEN_NORMALIZATION:
			  return value/sqrt((double) seq_len);
			  break;
		  case SQLEN_NORMALIZATION:
			  return value/seq_len;
			  break;
		  default:
			  ASSERT(0);
	  }

	  return -CMath::INFTY;
  }

 protected:
  DREAL* sqrtdiag_lhs;
  DREAL* sqrtdiag_rhs;

  bool initialized;

  INT dictionary_size;
  DREAL* dictionary_weights;
  bool use_sign;

  ENormalizationType normalization;
};
#endif
