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
 * exgvnm - ex_get_variable_name
 *
 * entry conditions -
 *   input parameters:
 *       int   exoid                   exodus file id
 *       int   obj_type                variable type
 *       int   var_num                 variable index to read 1..num_var
 *
 * exit conditions -
 *       char*   var_name                ptr to variable name
 *
 * revision history -
 *
 *
 *****************************************************************************/

#include "exodusII.h"     // for ex_err, etc
#include "exodusII_int.h" // for EX_FATAL, etc

/*!
 * \ingroup ResultsData
 *
 * reads the name of a particular results variable from the database
 */

int ex_get_variable_name(int exoid, ex_entity_type obj_type, int var_num, char *var_name)
{
  int         status;
  int         varid;
  char        errmsg[MAX_ERR_LENGTH];
  const char *vname = NULL;

  EX_FUNC_ENTER();
  ex_check_valid_file_id(exoid, __func__);

  /* inquire previously defined variables  */

  switch (obj_type) {
  case EX_GLOBAL: vname = VAR_NAME_GLO_VAR; break;
  case EX_NODAL: vname = VAR_NAME_NOD_VAR; break;
  case EX_EDGE_BLOCK: vname = VAR_NAME_EDG_VAR; break;
  case EX_FACE_BLOCK: vname = VAR_NAME_FAC_VAR; break;
  case EX_ELEM_BLOCK: vname = VAR_NAME_ELE_VAR; break;
  case EX_NODE_SET: vname = VAR_NAME_NSET_VAR; break;
  case EX_EDGE_SET: vname = VAR_NAME_ESET_VAR; break;
  case EX_FACE_SET: vname = VAR_NAME_FSET_VAR; break;
  case EX_SIDE_SET: vname = VAR_NAME_SSET_VAR; break;
  case EX_ELEM_SET: vname = VAR_NAME_ELSET_VAR; break;
  default:
    snprintf(errmsg, MAX_ERR_LENGTH, "ERROR: Invalid variable type (%d) given for file id %d",
             obj_type, exoid);
    ex_err_fn(exoid, __func__, errmsg, EX_BADPARAM);
    EX_FUNC_LEAVE(EX_FATAL);
  }

  if ((status = nc_inq_varid(exoid, vname, &varid)) != NC_NOERR) {
    snprintf(errmsg, MAX_ERR_LENGTH, "Warning: no %s variable names stored in file id %d",
             ex_name_of_object(obj_type), exoid);
    ex_err_fn(exoid, __func__, errmsg, status);
    EX_FUNC_LEAVE(EX_WARN);
  }

  /* read the variable name */
  {
    int db_name_size  = ex_inquire_int(exoid, EX_INQ_DB_MAX_ALLOWED_NAME_LENGTH);
    int api_name_size = ex_inquire_int(exoid, EX_INQ_MAX_READ_NAME_LENGTH);
    int name_size     = db_name_size < api_name_size ? db_name_size : api_name_size;

    status =
        ex_get_name_internal(exoid, varid, var_num - 1, var_name, name_size, obj_type, __func__);
    if (status != NC_NOERR) {
      EX_FUNC_LEAVE(EX_FATAL);
    }
  }
  EX_FUNC_LEAVE(EX_NOERR);
}
