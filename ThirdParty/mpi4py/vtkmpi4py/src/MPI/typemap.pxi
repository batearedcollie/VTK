# -----------------------------------------------------------------------------

cdef inline int AddTypeMap(dict TD, object key, Datatype dataype) except -1:
    global TypeDict
    if dataype.ob_mpi != MPI_DATATYPE_NULL:
        TD[key] = dataype
        return 1
    return 0

# -----------------------------------------------------------------------------

cdef dict TypeDict = { }
__TypeDict__ = TypeDict

# boolean (C99)
AddTypeMap(TypeDict, "?"  , __C_BOOL__ ) # PEP-3118 & NumPy
# character
AddTypeMap(TypeDict, "c"  , __CHAR__   ) # PEP-3118 & NumPy
## XXX this requires special handling
## AddTypeMap(TypeDict, "u"  , __????__   ) # PEP-3118
## AddTypeMap(TypeDict, "w"  , __????__   ) # PEP-3118
# (signed) integer
AddTypeMap(TypeDict, "b" , __SIGNED_CHAR__ ) # MPI-2
AddTypeMap(TypeDict, "h" , __SHORT__       )
AddTypeMap(TypeDict, "i" , __INT__         )
AddTypeMap(TypeDict, "l" , __LONG__        )
AddTypeMap(TypeDict, "q" , __LONG_LONG__   )
# unsigned integer
AddTypeMap(TypeDict, "B" , __UNSIGNED_CHAR__     )
AddTypeMap(TypeDict, "H" , __UNSIGNED_SHORT__    )
AddTypeMap(TypeDict, "I" , __UNSIGNED__          )
AddTypeMap(TypeDict, "L" , __UNSIGNED_LONG__     )
AddTypeMap(TypeDict, "Q" , __UNSIGNED_LONG_LONG__)
# (real) floating
AddTypeMap(TypeDict, "f" , __FLOAT__       )
AddTypeMap(TypeDict, "d" , __DOUBLE__      )
AddTypeMap(TypeDict, "g" , __LONG_DOUBLE__ ) # PEP-3118 & NumPy
# complex floating (F77)
AddTypeMap(TypeDict, "Zf" , __COMPLEX__        ) # PEP-3118
AddTypeMap(TypeDict, "Zd" , __DOUBLE_COMPLEX__ ) # PEP-3118
AddTypeMap(TypeDict, "F"  , __COMPLEX__        ) # NumPy
AddTypeMap(TypeDict, "D"  , __DOUBLE_COMPLEX__ ) # NumPy
# complex floating (F90)
AddTypeMap(TypeDict, "Zf" , __COMPLEX8__       ) # PEP-3118
AddTypeMap(TypeDict, "Zd" , __COMPLEX16__      ) # PEP-3118
AddTypeMap(TypeDict, "F"  , __COMPLEX8__       ) # NumPy
AddTypeMap(TypeDict, "D"  , __COMPLEX16__      ) # NumPy
# complex floating (C99)
AddTypeMap(TypeDict, "Zf" , __C_FLOAT_COMPLEX__       ) # PEP-3118
AddTypeMap(TypeDict, "Zd" , __C_DOUBLE_COMPLEX__      ) # PEP-3118
AddTypeMap(TypeDict, "Zg" , __C_LONG_DOUBLE_COMPLEX__ ) # PEP-3118
AddTypeMap(TypeDict, "F"  , __C_FLOAT_COMPLEX__       ) # NumPy
AddTypeMap(TypeDict, "D"  , __C_DOUBLE_COMPLEX__      ) # NumPy
AddTypeMap(TypeDict, "G"  , __C_LONG_DOUBLE_COMPLEX__ ) # NumPy

# -----------------------------------------------------------------------------

cdef dict CTypeDict = { }
__CTypeDict__ = CTypeDict

AddTypeMap(CTypeDict, "?"  , __C_BOOL__ )
AddTypeMap(CTypeDict, "c"  , __CHAR__   )

AddTypeMap(CTypeDict, "b" , __SIGNED_CHAR__ )
AddTypeMap(CTypeDict, "h" , __SHORT__       )
AddTypeMap(CTypeDict, "i" , __INT__         )
AddTypeMap(CTypeDict, "l" , __LONG__        )
AddTypeMap(CTypeDict, "q" , __LONG_LONG__   )

AddTypeMap(CTypeDict, "B" , __UNSIGNED_CHAR__      )
AddTypeMap(CTypeDict, "H" , __UNSIGNED_SHORT__     )
AddTypeMap(CTypeDict, "I" , __UNSIGNED__           )
AddTypeMap(CTypeDict, "L" , __UNSIGNED_LONG__      )
AddTypeMap(CTypeDict, "Q" , __UNSIGNED_LONG_LONG__ )

AddTypeMap(CTypeDict, "f" , __FLOAT__       )
AddTypeMap(CTypeDict, "d" , __DOUBLE__      )
AddTypeMap(CTypeDict, "g" , __LONG_DOUBLE__ )

AddTypeMap(CTypeDict, "F"  , __C_FLOAT_COMPLEX__       )
AddTypeMap(CTypeDict, "D"  , __C_DOUBLE_COMPLEX__      )
AddTypeMap(CTypeDict, "G"  , __C_LONG_DOUBLE_COMPLEX__ )

AddTypeMap(CTypeDict, "i1" , __INT8_T__   )
AddTypeMap(CTypeDict, "i2" , __INT16_T__  )
AddTypeMap(CTypeDict, "i4" , __INT32_T__  )
AddTypeMap(CTypeDict, "i8" , __INT64_T__  )

AddTypeMap(CTypeDict, "u1" , __UINT8_T__  )
AddTypeMap(CTypeDict, "u2" , __UINT16_T__ )
AddTypeMap(CTypeDict, "u4" , __UINT32_T__ )
AddTypeMap(CTypeDict, "u8" , __UINT64_T__ )

# -----------------------------------------------------------------------------

cdef dict FTypeDict = { }
__FTypeDict__ = FTypeDict

AddTypeMap(FTypeDict, "?"   , __LOGICAL__          )
AddTypeMap(FTypeDict, "c"   , __CHARACTER__        )
AddTypeMap(FTypeDict, "i"   , __INTEGER__          )
AddTypeMap(FTypeDict, "f"   , __REAL__             )
AddTypeMap(FTypeDict, "d"   , __DOUBLE_PRECISION__ )
AddTypeMap(FTypeDict, "F"   , __COMPLEX__          )
AddTypeMap(FTypeDict, "D"   , __DOUBLE_COMPLEX__   )

AddTypeMap(FTypeDict, "i1"  , __INTEGER1__  )
AddTypeMap(FTypeDict, "i2"  , __INTEGER2__  )
AddTypeMap(FTypeDict, "i4"  , __INTEGER4__  )
AddTypeMap(FTypeDict, "i8"  , __INTEGER8__  )
AddTypeMap(FTypeDict, "i16" , __INTEGER16__ )

AddTypeMap(FTypeDict, "f2"  , __REAL2__     )
AddTypeMap(FTypeDict, "f4"  , __REAL4__     )
AddTypeMap(FTypeDict, "f8"  , __REAL8__     )
AddTypeMap(FTypeDict, "f16" , __REAL16__    )

AddTypeMap(FTypeDict, "c4"  , __COMPLEX4__  )
AddTypeMap(FTypeDict, "c8"  , __COMPLEX8__  )
AddTypeMap(FTypeDict, "c16" , __COMPLEX16__ )
AddTypeMap(FTypeDict, "c32" , __COMPLEX32__ )

# -----------------------------------------------------------------------------
