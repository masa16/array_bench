require 'rbconfig.rb'
require 'mkmf'

$CFLAGS=" $(cflags) -O3 -m64 -msse -msse2 -funroll-loops"
$INCFLAGS = "-Itypes #$INCFLAGS"

srcs = %w(
narray
)

#have_func("rb_thread_call_without_gvl")

create_makefile('narray')
