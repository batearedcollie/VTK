#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void CreateInitFile(const char *libName, FILE *fout)
{
  int i;

  const char *prefix = "";
  const char* dllexp = "VTK_ABI_EXPORT ";

  fprintf(fout,"// Generated by vtkWrapPythonInit in VTK/Wrapping\n");
  fprintf(fout,"#include \"vtkSystemIncludes.h\"\n");
  fprintf(fout,"// Handle compiler warning messages, etc.\n"
          "#if defined( _MSC_VER ) && !defined(VTK_DISPLAY_WIN32_WARNINGS)\n"
          "#pragma warning ( disable : 4706 )\n"
          "#endif // Windows Warnings \n\n");

  fprintf(fout,"extern \"C\" void real_init%s(const char *modulename);\n\n", libName);

  for (;;)
    {
    fprintf(fout,"extern  \"C\" { %svoid init%s%s(); }\n\n", dllexp, prefix, libName);

    fprintf(fout,"void init%s%s()\n{\n", prefix, libName);
    fprintf(fout,"  static const char modulename[] = \"%s%s\";\n", prefix, libName);
    fprintf(fout,"  real_init%s(modulename);\n}\n\n", libName);

#if defined(__CYGWIN__) || !defined(_WIN32)
    /* add a "lib" prefix for compatibility with old python scripts */
    if (strcmp(prefix, "lib") != 0)
      {
      prefix = "lib";
      continue;
      }
#endif
    break;
    }
}


/* warning this code is also in getclasses.cxx under pcmaker */
/* this routine creates the init file */
static void CreateImplFile(const char *libName,
  int numFiles, char **files,
  FILE *fout)
{
  int i;

  const char *prefix = "";
  const char* dllexp = "VTK_ABI_EXPORT ";

  fprintf(fout,"// Generated by vtkWrapPythonInit in VTK/Wrapping\n");
  fprintf(fout,"#include \"vtkPython.h\"\n\n");
  fprintf(fout,"#include \"vtkSystemIncludes.h\"\n");
  fprintf(fout,"#include <string.h>\n");
  fprintf(fout,"// Handle compiler warning messages, etc.\n"
          "#if defined( _MSC_VER ) && !defined(VTK_DISPLAY_WIN32_WARNINGS)\n"
          "#pragma warning ( disable : 4706 )\n"
          "#endif // Windows Warnings \n\n");

  for (i = 0; i < numFiles; i++)
    {
    fprintf(fout,"extern  \"C\" {%sPyObject *PyVTKAddFile_%s(PyObject *, const char *); }\n", dllexp, files[i]);
    }

  fprintf(fout,"\nstatic PyMethodDef Py%s_ClassMethods[] = {\n", libName);
  fprintf(fout,"{NULL, NULL, 0, NULL}};\n\n");

  fprintf(fout,"extern  \"C\" { void real_init%s(const char *modulename); }\n\n", libName);

  fprintf(fout,"void real_init%s(const char *modulename)\n{\n", libName);

  /* module init function */
  fprintf(fout,"  PyObject *m, *d;\n\n");
  fprintf(fout,"  m = Py_InitModule((char*)modulename, Py%s_ClassMethods);\n",
    libName);

  fprintf(fout,"  d = PyModule_GetDict(m);\n");
  fprintf(fout,"  if (!d)\n");
  fprintf(fout,"    {\n");
  fprintf(fout,"    Py_FatalError((char*)\"can't get dictionary for module %s\");\n",
    libName);
  fprintf(fout,"    }\n");

  for (i = 0; i < numFiles; i++)
    {
    fprintf(fout,"  PyVTKAddFile_%s(d, modulename);\n",
      files[i]);
    }
  fprintf(fout,"}\n\n");
}


int main(int argc,char *argv[])
{
  FILE *file;
  FILE *fout_init;
  FILE *fout_impl;
  int numFiles = 0;
  char libName[250];
  char tmpVal[250];
  char *files[4000];

  if (argc < 4)
    {
    fprintf(stderr,"Usage: %s input_file init_file impl_file\n",argv[0]);
    return 1;
    }

  file = fopen(argv[1],"r");
  if (!file)
    {
    fprintf(stderr,"Input file %s could not be opened\n",argv[1]);
    return 1;
    }

  /* read the info from the file */
  if (fscanf(file,"%s",libName) != 1)
    {
    fprintf(stderr,"Error getting libName\n");
    fclose(file);
    return 1;
    }
  /* read in the classes */
  while (fscanf(file,"%s",tmpVal) != EOF)
    {
    files[numFiles] = strdup(tmpVal);
    numFiles++;
    }
  /* close the file */
  fclose(file);
  file = NULL;

  fout_init = fopen(argv[2],"w");
  if (!fout_init)
    {
    return 1;
    }

  fout_impl = fopen(argv[3],"w");
  if (!fout_impl)
    {
    fclose(fout_init);
    return 1;
    }

  /* extra object for the Common module */
  if (strcmp(libName, "vtkCommonCorePython") == 0)
    {
    /* the PyVTKMutableObject */
    files[numFiles] = strdup("mutable");
    numFiles++;
    }

  CreateInitFile(libName, fout_init);
  CreateImplFile(libName, numFiles, files, fout_impl);
  fclose(fout_init);
  fclose(fout_impl);

  return 0;
}
