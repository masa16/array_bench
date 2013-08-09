require "~/git/narray-devel/ext/narray"

params = [
[100,10000000],
[1000,1000000],
[10000,100000],
[100000,10000],
[1000000,1000],
[10000000,100],
[50000000,20],
[10000000,100],
[1000000,1000],
[100000,10000],
[10000,100000],
[1000,1000000],
[100,10000000],
]

params.each do |n,m|
  x = NArray::DFloat.new([n]).seq
  y = NArray::DFloat.new([n]).seq
  s = Time.now
  m.times do
    z = x+y
  end
  t = Time.now-s
  #p GC.stat
  ops = n * m * 1e-9 / t
  printf "size=%d repeat=%d: %7.3f sec, %5.3f GFLOPS, %5.3f GB/s\n", n, m, t, ops, ops*24
end
