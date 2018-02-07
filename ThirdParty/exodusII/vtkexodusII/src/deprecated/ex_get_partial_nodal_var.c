/*
 * Copyright (c) 2005-2017 National Technology & Engineering Solutions
 * of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with
 * NTESS, the U.S. Government retains certain rights in this software.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *
 *     * Neither the name of NTESS nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/*****************************************************************************
 *
 * exgnnv - ex_get_partial_nodal_var
 *
 * environment - UNIX
 *
 * entry conditions -
 *   input parameters:
 *	int	exoid			exodus file id
 *	int	time_step		whole time step number
 *	int	nodeal_var_index	index of desired nodal variable
 *       int     start_node		starting location for read
 *	int	num_nodes		number of nodal points
 *
 * exit conditions -
 *	float*	var_vals		array of nodal variable values
 *
 * revision history -
 *
 *  $Id: ne_gnnv.c,v 1.16 2008/01/25 15:47:35 gdsjaar Exp $
 *
 *****************************************************************************/

#include <exodusII.h> // for ex_get_partial_var, etc
#include <stdint.h>   // for int64_t

/*!
\ingroup ResultsData

 * \deprecated use ex_get_partial_var()(exoid, time_step, EX_NODAL, nodal_var_index, 1, start_node,
 num_nodes, var_vals);

 */

/*
 * reads the values of a single nodal variable for a single time step from
 * the database; assume the first time step and nodal variable index is 1
 */

int ex_get_partial_nodal_var(int exoid, int time_step, int nodal_var_index, int64_t start_node,
                             int64_t num_nodes, void *var_vals)
{
  return ex_get_partial_var(exoid, time_step, EX_NODAL, nodal_var_index, 1, start_node, num_nodes,
                            var_vals);
}
