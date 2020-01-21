/* -*- c++ -*- */

#define EQUALIZERS_API
#define DIGITAL_API

%include "gnuradio.i"           // the common stuff
%{
#include "gnuradio/digital/constellation.h"
%}
%include "gnuradio/digital/constellation.h"
// %include <gnuradio/swig/constellation.i>

//load generated python docstrings
%include "equalizers_swig_doc.i"



%{
#include "equalizers/linear_equalizer.h"
#include "equalizers/adaptive_algorithm.h"
#include "equalizers/adaptive_algorithm_rls.h"
#include "equalizers/adaptive_algorithm_lms.h"
#include "equalizers/adaptive_algorithm_nlms.h"
#include "equalizers/adaptive_algorithm_cma.h"
#include "equalizers/adaptive_algorithm_zf.h"
#include "equalizers/meas_evm_cc.h"
#include "equalizers/decision_feedback_equalizer.h"
%}

%include "equalizers/adaptive_algorithm.h"
%include "equalizers/adaptive_algorithm_rls.h"
%include "equalizers/adaptive_algorithm_lms.h"
%include "equalizers/adaptive_algorithm_nlms.h"
%include "equalizers/adaptive_algorithm_cma.h"
%include "equalizers/adaptive_algorithm_zf.h"
%include "equalizers/linear_equalizer.h"
GR_SWIG_BLOCK_MAGIC2(equalizers, linear_equalizer);

// %template(constellation_sptr) boost::shared_ptr<gr::digital::constellation>;
%template()     boost::shared_ptr<gr::digital::constellation>;

%template(adaptive_algorithm_sptr) boost::shared_ptr<gr::equalizers::adaptive_algorithm>;

%template(adaptive_algorithm_lms_sptr) boost::shared_ptr<gr::equalizers::adaptive_algorithm_lms>;
%pythoncode %{
adaptive_algorithm_lms_sptr.__repr__ = lambda self: "<adaptive_algorithm LMS>"
adaptive_algorithm_lms = adaptive_algorithm_lms.make;
%}
%template(adaptive_algorithm_nlms_sptr) boost::shared_ptr<gr::equalizers::adaptive_algorithm_nlms>;
%pythoncode %{
adaptive_algorithm_nlms_sptr.__repr__ = lambda self: "<adaptive_algorithm NLMS>"
adaptive_algorithm_nlms = adaptive_algorithm_nlms.make;
%}
%template(adaptive_algorithm_cma_sptr) boost::shared_ptr<gr::equalizers::adaptive_algorithm_cma>;
%pythoncode %{
adaptive_algorithm_cma_sptr.__repr__ = lambda self: "<adaptive_algorithm CMA>"
adaptive_algorithm_cma = adaptive_algorithm_cma.make;
%}
%template(adaptive_algorithm_rls_sptr) boost::shared_ptr<gr::equalizers::adaptive_algorithm_rls>;
%pythoncode %{
adaptive_algorithm_rls_sptr.__repr__ = lambda self: "<adaptive_algorithm RLS>"
adaptive_algorithm_rls = adaptive_algorithm_rls.make;
%}
%template(adaptive_algorithm_zf_sptr) boost::shared_ptr<gr::equalizers::adaptive_algorithm_zf>;
%pythoncode %{
adaptive_algorithm_zf_sptr.__repr__ = lambda self: "<adaptive_algorithm ZF>"
adaptive_algorithm_zf = adaptive_algorithm_zf.make;
%}

%include "equalizers/meas_evm_cc.h"
GR_SWIG_BLOCK_MAGIC2(equalizers, meas_evm_cc);
%include "equalizers/decision_feedback_equalizer.h"
GR_SWIG_BLOCK_MAGIC2(equalizers, decision_feedback_equalizer);
