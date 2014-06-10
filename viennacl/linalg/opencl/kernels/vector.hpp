#ifndef VIENNACL_LINALG_OPENCL_KERNELS_VECTOR_HPP
#define VIENNACL_LINALG_OPENCL_KERNELS_VECTOR_HPP

#include "viennacl/tools/tools.hpp"

#include "viennacl/device_specific/database.hpp"
#include "viennacl/device_specific/generate.hpp"

#include "viennacl/scheduler/forwards.h"
#include "viennacl/scheduler/io.hpp"
#include "viennacl/scheduler/preset.hpp"

#include "viennacl/ocl/kernel.hpp"
#include "viennacl/ocl/platform.hpp"
#include "viennacl/ocl/utils.hpp"

/** @file viennacl/linalg/opencl/kernels/vector.hpp
 *  @brief OpenCL kernel file for vector operations */
namespace viennacl
{
  namespace linalg
  {
    namespace opencl
    {
      namespace kernels
      {

        using namespace viennacl::device_specific;

        //////////////////////////// Part 1: Kernel generation routines ////////////////////////////////////

        template<class T>
        void generate_plane_rotation(std::string & source, vector_axpy_template::parameters const & parameters)
        {
          viennacl::vector<T> x; viennacl::vector<T> y;
          T a; T b;
          source.append(generate::opencl_source<vector_axpy_template>(parameters, scheduler::preset::plane_rotation(&x, &y, &a, &b)));
        }

        template<class T>
        void generate_vector_swap(std::string & source, vector_axpy_template::parameters const & parameters)
        {
          viennacl::vector<T> x; viennacl::vector<T> y;
          source.append(generate::opencl_source<vector_axpy_template>(parameters, scheduler::preset::swap(&x, &y)));
        }

        template<class T>
        void generate_assign_cpu(std::string & source, vector_axpy_template::parameters const & parameters)
        {
          viennacl::vector<T> x; viennacl::scalar_vector<T> y(0,0);
          source.append(generate::opencl_source<vector_axpy_template>(parameters, scheduler::preset::assign_cpu(&x, &y)));
        }

        template<typename T>
        void generate_inner_prod(std::string & source, reduction_template::parameters const & parameters, vcl_size_t vector_num)
        {
          viennacl::vector<T> x;
          viennacl::vector<T> y;
          viennacl::scalar<T> s;

          statements_container::data_type statements;
          for(unsigned int i = 0 ; i < vector_num ; ++i)
            statements.push_back(scheduler::preset::inner_prod(&s, &x, &y));

          source.append(generate::opencl_source<reduction_template>(parameters, statements_container(statements,statements_container::INDEPENDENT),BIND_ALL_UNIQUE));
        }


        template <typename T>
        void generate_norms_sum(std::string & source, reduction_template::parameters const & parameters)
        {
          viennacl::vector<T> x;
          viennacl::scalar<T> s;

          source.append(generate::opencl_source<reduction_template>(parameters, scheduler::preset::norm_1(&s, &x)));
          source.append(generate::opencl_source<reduction_template>(parameters, scheduler::preset::norm_2(&s, &x)));
          source.append(generate::opencl_source<reduction_template>(parameters, scheduler::preset::norm_inf(&s, &x)));
          source.append(generate::opencl_source<reduction_template>(parameters, scheduler::preset::index_norm_inf(&s, &x)));
          source.append(generate::opencl_source<reduction_template>(parameters, scheduler::preset::sum(&s, &x)));
        }


        template<typename T, typename ScalarType1, typename ScalarType2>
        inline void generate_avbv_impl(std::string & source, vector_axpy_template::parameters const & parameters, scheduler::operation_node_type ASSIGN_OP,
                                       viennacl::vector_base<T> const * x, viennacl::vector_base<T> const * y, ScalarType1 const * a,
                                       viennacl::vector_base<T> const * z, ScalarType2 const * b)
        {
          source.append(generate::opencl_source<vector_axpy_template>(parameters, scheduler::preset::avbv(ASSIGN_OP, x, y, a, false, false, z, b, false, false), BIND_ALL_UNIQUE));
          source.append(generate::opencl_source<vector_axpy_template>(parameters, scheduler::preset::avbv(ASSIGN_OP, x, y, a, true, false, z, b, false, false), BIND_ALL_UNIQUE));
          source.append(generate::opencl_source<vector_axpy_template>(parameters, scheduler::preset::avbv(ASSIGN_OP, x, y, a, false, true, z, b, false, false), BIND_ALL_UNIQUE));
          source.append(generate::opencl_source<vector_axpy_template>(parameters, scheduler::preset::avbv(ASSIGN_OP, x, y, a, true, true, z, b, false, false), BIND_ALL_UNIQUE));
          if(b)
          {
            source.append(generate::opencl_source<vector_axpy_template>(parameters, scheduler::preset::avbv(ASSIGN_OP, x, y, a, false, false, z, b, true, false), BIND_ALL_UNIQUE));
            source.append(generate::opencl_source<vector_axpy_template>(parameters, scheduler::preset::avbv(ASSIGN_OP, x, y, a, true, false, z, b, true, false), BIND_ALL_UNIQUE));
            source.append(generate::opencl_source<vector_axpy_template>(parameters, scheduler::preset::avbv(ASSIGN_OP, x, y, a, false, true, z, b, true, false), BIND_ALL_UNIQUE));
            source.append(generate::opencl_source<vector_axpy_template>(parameters, scheduler::preset::avbv(ASSIGN_OP, x, y, a, true, true, z, b, true, false), BIND_ALL_UNIQUE));

            source.append(generate::opencl_source<vector_axpy_template>(parameters, scheduler::preset::avbv(ASSIGN_OP, x, y, a, false, false, z, b, false, true), BIND_ALL_UNIQUE));
            source.append(generate::opencl_source<vector_axpy_template>(parameters, scheduler::preset::avbv(ASSIGN_OP, x, y, a, true, false, z, b, false, true), BIND_ALL_UNIQUE));
            source.append(generate::opencl_source<vector_axpy_template>(parameters, scheduler::preset::avbv(ASSIGN_OP, x, y, a, false, true, z, b, false, true), BIND_ALL_UNIQUE));
            source.append(generate::opencl_source<vector_axpy_template>(parameters, scheduler::preset::avbv(ASSIGN_OP, x, y, a, true, true, z, b, false, true), BIND_ALL_UNIQUE));

            source.append(generate::opencl_source<vector_axpy_template>(parameters, scheduler::preset::avbv(ASSIGN_OP, x, y, a, false, false, z, b, true, true), BIND_ALL_UNIQUE));
            source.append(generate::opencl_source<vector_axpy_template>(parameters, scheduler::preset::avbv(ASSIGN_OP, x, y, a, true, false, z, b, true, true), BIND_ALL_UNIQUE));
            source.append(generate::opencl_source<vector_axpy_template>(parameters, scheduler::preset::avbv(ASSIGN_OP, x, y, a, false, true, z, b, true, true), BIND_ALL_UNIQUE));
            source.append(generate::opencl_source<vector_axpy_template>(parameters, scheduler::preset::avbv(ASSIGN_OP, x, y, a, true, true, z, b, true, true), BIND_ALL_UNIQUE));
          }
        }

        template<class T>
        inline void generate_avbv(std::string & source, vector_axpy_template::parameters const & parameters, scheduler::operation_node_type ASSIGN_TYPE)
        {
          viennacl::vector<T> x;
          viennacl::vector<T> y;
          viennacl::vector<T> z;

          viennacl::scalar<T> da;
          viennacl::scalar<T> db;

          T ha;
          T hb;

          //x = a*y
          generate_avbv_impl(source, parameters, ASSIGN_TYPE, &x, &y, &ha, (viennacl::vector<T>*)NULL, (T*)NULL);
          generate_avbv_impl(source, parameters, ASSIGN_TYPE, &x, &y, &da, (viennacl::vector<T>*)NULL, (T*)NULL);

          //x = a*y + b*z
          generate_avbv_impl(source, parameters, ASSIGN_TYPE, &x, &y, &ha, &z, &hb);
          generate_avbv_impl(source, parameters, ASSIGN_TYPE, &x, &y, &da, &z, &hb);
          generate_avbv_impl(source, parameters, ASSIGN_TYPE, &x, &y, &ha, &z, &db);
          generate_avbv_impl(source, parameters, ASSIGN_TYPE, &x, &y, &da, &z, &db);
        }

        template<class T>
        inline void generate_avbv(std::string & source, vector_axpy_template::parameters const & parameters)
        {
          generate_avbv<T>(source, parameters, scheduler::OPERATION_BINARY_ASSIGN_TYPE);
          generate_avbv<T>(source, parameters, scheduler::OPERATION_BINARY_INPLACE_ADD_TYPE);
        }


        //////////////////////////// Part 2: Main kernel class ////////////////////////////////////

        // main kernel class
        /** @brief Main kernel class for generating OpenCL kernels for operations on/with viennacl::vector<> without involving matrices, multiple inner products, or element-wise operations other than addition or subtraction. */
        template <class TYPE>
        struct vector
        {
          static std::string program_name()
          {
            return viennacl::ocl::type_to_string<TYPE>::apply() + "_vector";
          }

          static void init(viennacl::ocl::context & ctx)
          {
            viennacl::ocl::DOUBLE_PRECISION_CHECKER<TYPE>::apply(ctx);
            static std::map<cl_context, bool> init_done;
            if (!init_done[ctx.handle().get()])
            {
              std::string numeric_string = viennacl::ocl::type_to_string<TYPE>::apply();
              vector_axpy_template::parameters const & axpy = database::get<TYPE>(database::axpy);
              reduction_template::parameters const & reduction = database::get<TYPE>(database::reduction);

              std::string source;
              source.reserve(8192);

              viennacl::ocl::append_double_precision_pragma<TYPE>(ctx, source);

              generate_avbv<TYPE>(source, axpy);
              generate_plane_rotation<TYPE>(source, axpy);
              generate_vector_swap<TYPE>(source, axpy);
              generate_assign_cpu<TYPE>(source, axpy);
              generate_inner_prod<TYPE>(source, reduction, 1);
              generate_norms_sum<TYPE>(source, reduction);

              std::string prog_name = program_name();
              #ifdef VIENNACL_BUILD_INFO
              std::cout << "Creating program " << prog_name << std::endl;
              #endif
              ctx.add_program(source, prog_name);
              init_done[ctx.handle().get()] = true;
            } //if
          } //init
        };

        // class with kernels for multiple inner products.
        /** @brief Main kernel class for generating OpenCL kernels for multiple inner products on/with viennacl::vector<>. */
        template <class TYPE>
        struct vector_multi_inner_prod
        {
          static std::string program_name()
          {
            return viennacl::ocl::type_to_string<TYPE>::apply() + "_vector_multi";
          }

          static void init(viennacl::ocl::context & ctx)
          {
            viennacl::ocl::DOUBLE_PRECISION_CHECKER<TYPE>::apply(ctx);
            static std::map<cl_context, bool> init_done;
            if (!init_done[ctx.handle().get()])
            {
              template_base & reduction = database::get<TYPE>(database::reduction);

              std::string source;
              source.reserve(8192);

              viennacl::ocl::append_double_precision_pragma<TYPE>(ctx, source);

              generate_inner_prod<TYPE>(source, reduction, 2);
              generate_inner_prod<TYPE>(source, reduction, 3);
              generate_inner_prod<TYPE>(source, reduction, 4);
              generate_inner_prod<TYPE>(source, reduction, 8);

              std::string prog_name = program_name();
              #ifdef VIENNACL_BUILD_INFO
              std::cout << "Creating program " << prog_name << std::endl;
              #endif
              ctx.add_program(source, prog_name);
              init_done[ctx.handle().get()] = true;
            } //if
          } //init
        };

      }  // namespace kernels
    }  // namespace opencl
  }  // namespace linalg
}  // namespace viennacl
#endif

