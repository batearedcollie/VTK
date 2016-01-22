#ifndef DIY_REDUCE_OPERATIONS_HPP
#define DIY_REDUCE_OPERATIONS_HPP

#include "reduce.hpp"
#include "partners/swap.hpp"
#include "detail/reduce/all-to-all.hpp"

namespace diy
{

/**
 * \ingroup Communication
 * \brief all to all reduction
 *
 */
template<class Op>
void
all_to_all(Master&              master,     //!< block owner
           const Assigner&      assigner,   //!< global block locator (maps gid to proc)
           const Op&            op,         //!< user-defined operation called to enqueue and dequeue items
           int                  k = 2       //!< reduction fanout
          )
{
  RegularSwapPartners  partners(1, assigner.nblocks(), k, false);
  reduce(master, assigner, partners, detail::AllToAllReduce<Op>(op, assigner), detail::SkipIntermediate(partners.rounds()));
}

}

#endif
