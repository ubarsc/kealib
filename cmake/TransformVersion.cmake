
function(TRANSFORM_VERSION numerical_result major_result minor_result patch_result version)
  # internal_version ignores everything in version after any character that
  # is not 0-9 or ".".  This should take care of the case when there is
  # some non-numerical data in the patch version.
  string(REGEX REPLACE "^([0-9.]+).*$" "\\1" internal_version ${version})

  # internal_version is normally a period-delimited triplet string of the form
  # "major.minor.patch", but patch and/or minor could be missing.
  # Transform internal_version into a numerical result that can be compared.
  string(REGEX REPLACE "^([0-9]*).+$" "\\1" major ${internal_version})
  string(REGEX REPLACE "^[0-9]*\\.([0-9]*).*$" "\\1" minor ${internal_version})
  string(REGEX REPLACE "^[0-9]*\\.[0-9]*\\.([0-9]*)$" "\\1" patch ${internal_version})

  if(NOT patch MATCHES "[0-9]+")
    set(patch 0)
  endif(NOT patch MATCHES "[0-9]+")

  if(NOT minor MATCHES "[0-9]+")
    set(minor 0)
  endif(NOT minor MATCHES "[0-9]+")

  if(NOT major MATCHES "[0-9]+")
    set(major 0)
  endif(NOT major MATCHES "[0-9]+")
  math(EXPR internal_numerical_result
    "${major}*1000000 + ${minor}*1000 + ${patch}"
    )
  set(${numerical_result} ${internal_numerical_result} PARENT_SCOPE)
  set(${major_result} ${major} PARENT_SCOPE)
  set(${minor_result} ${minor} PARENT_SCOPE)
  set(${patch_result} ${patch} PARENT_SCOPE)
endfunction()
