///////////////////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2021.
//  Distributed under the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt
//  or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef TEST_UINTWIDE_T_N_BINARY_OPS_MUL_DIV_4_BY_4_TEMPLATE_2021_03_04_H_
  #define TEST_UINTWIDE_T_N_BINARY_OPS_MUL_DIV_4_BY_4_TEMPLATE_2021_03_04_H_

  #include <atomic>

  #include <test/test_uintwide_t_n_base.h>
  #include <wide_integer/generic_template_uintwide_t.h>

  template<const std::size_t MyDigits2,
           typename MyLimbType>
  class test_uintwide_t_n_binary_ops_mul_div_4_by_4_template : public test_uintwide_t_n_base
  {
  private:
    static constexpr std::size_t digits2 = MyDigits2;

    virtual std::size_t get_digits2 () const { return digits2; }

    using native_uint_control_type =
      typename wide_integer::generic_template::detail::int_type_helper<digits2>::exact_unsigned_type;

    using local_limb_type = MyLimbType;

    using local_uint_ab_type = wide_integer::generic_template::uintwide_t<digits2, local_limb_type>;

  public:
    test_uintwide_t_n_binary_ops_mul_div_4_by_4_template(const std::size_t count)
      : test_uintwide_t_n_base(count),
        a_local  (),
        b_local  (),
        a_control(),
        b_control() { }

    virtual ~test_uintwide_t_n_binary_ops_mul_div_4_by_4_template() = default;

    virtual bool do_test(const std::size_t rounds)
    {
      bool result_is_ok = true;

      for(std::size_t i = 0U; i < rounds; ++i)
      {
        std::cout << "initialize()       boost compare with uintwide_t: round " << i << ",  digits2: " << this->get_digits2() << std::endl;
        this->initialize();

        std::cout << "test_binary_mul()  boost compare with uintwide_t: round " << i << ",  digits2: " << this->get_digits2() << std::endl;
        result_is_ok &= this->test_binary_mul();
      }

      return result_is_ok;
    }

    virtual void initialize()
    {
      a_local.clear();
      b_local.clear();

      a_control.clear();
      b_control.clear();

      a_local.resize(size());
      b_local.resize(size());

      a_control.resize(size());
      b_control.resize(size());

      get_equal_random_test_values_control_and_local_n(a_local.data(), a_control.data(), size());
      get_equal_random_test_values_control_and_local_n(b_local.data(), b_control.data(), size());
    }

    virtual bool test_binary_mul() const
    {
      bool result_is_ok = true;

      std::atomic_flag test_lock = ATOMIC_FLAG_INIT;

      my_concurrency::parallel_for
      (
        std::size_t(0U),
        size(),
        [&test_lock, &result_is_ok, this](std::size_t i)
        {
          const native_uint_control_type c_control =   native_uint_control_type(a_control[i])
                                                     * b_control[i];

          const local_uint_ab_type c_local =   static_cast<local_uint_ab_type>(a_local[i])
                                             * static_cast<local_uint_ab_type>(b_local[i]);

          const std::string str_boost = hexlexical_cast(c_control);
          const std::string str_local = hexlexical_cast(c_local);

          while(test_lock.test_and_set()) { ; }
          result_is_ok &= (str_boost == str_local);
          test_lock.clear();
        }
      );

      return result_is_ok;
    }

  private:
    std::vector<local_uint_ab_type> a_local;
    std::vector<local_uint_ab_type> b_local;

    std::vector<native_uint_control_type> a_control;
    std::vector<native_uint_control_type> b_control;

    template<typename OtherLocalUintType,
             typename OtherControlUintType>
    static void get_equal_random_test_values_control_and_local_n(OtherLocalUintType*   u_local,
                                                                 OtherControlUintType* u_control,
                                                                 const std::size_t count)
    {
      using other_local_uint_type   = OtherLocalUintType;
      using other_control_uint_type = OtherControlUintType;

      test_uintwide_t_n_base::my_random_generator.seed(std::clock());

      using distribution_type =
        wide_integer::generic_template::uniform_int_distribution<other_local_uint_type::my_digits, typename other_local_uint_type::limb_type>;

      distribution_type distribution;

      std::atomic_flag rnd_lock = ATOMIC_FLAG_INIT;

      my_concurrency::parallel_for
      (
        std::size_t(0U),
        count,
        [&u_local, &u_control, &distribution, &rnd_lock](std::size_t i)
        {
          while(rnd_lock.test_and_set()) { ; }
          const other_local_uint_type a = distribution(my_random_generator);
          rnd_lock.clear();

          u_local  [i] = a;
          u_control[i] = static_cast<other_control_uint_type>(a);
        }
      );
    }
  };

#endif // TEST_UINTWIDE_T_N_BINARY_OPS_MUL_DIV_4_BY_4_TEMPLATE_2021_03_04_H_
