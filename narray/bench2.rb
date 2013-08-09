require "~/git/narray-devel/ext/narray"

params = [100000,10000]

procs = [
  proc{},
  proc{puts ([0]*100000000).size},
  proc{NArray::DFloat.new([100000000]).fill(0)},
]

#x = [0]*10000000
#x = nil

procs.each do |prc|
  prc.call
  #GC.start
  #sleep 2
  n, m = params
  x = NArray::DFloat.new([n]).seq
  y = NArray::DFloat.new([n]).seq
  t = Time.now
  m.times do
    z = x+y
  end
  t1 = Time.now-t
  ops = n * m * 1e-9 / t1
  printf "size=%d repeat=%d: %7.3f sec, %5.3f GFLOPS, %5.3f GB/s\n", n, m, t1, ops, ops*24
end
