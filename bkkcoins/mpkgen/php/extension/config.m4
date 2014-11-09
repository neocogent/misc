PHP_ARG_ENABLE(mpkgen, whether to enable mpkgen,
[ --enable-mpkgen   Enable mpkgen])
 
if test "$PHP_MPKGEN" = "yes"; then
  AC_DEFINE(HAVE_MPKGEN, 1, [Whether you have mpkgen])
  PHP_SUBST(MPKGEN_SHARED_LIBADD)
  PHP_ADD_LIBRARY(crypto, 1, MPKGEN_SHARED_LIBADD)
  PHP_NEW_EXTENSION(mpkgen, mpk.c, $ext_shared)
fi
