require './narray.so'

params = [
  [100,10000000,1],  [100,1000000,10],
  [1000,1000000,1],  [1000,100000,10],
  [10000,100000,1],  [10000,10000,10],
  [100000,10000,1],  [100000,1000,10],
  [1000000,1000,1],  [1000000,100,10],
  [10000000,100,1],  [10000000,10,10],
  [10000000,100,1],  [10000000,10,10],
  [1000000,1000,1],  [1000000,100,10],
  [100000,10000,1],  [100000,1000,10],
  [10000,100000,1],  [10000,10000,10],
  [1000,1000000,1],  [1000,100000,10],
  [100,10000000,1],  [100,1000000,10],
]

params = [
  [100,10000000,1],
  [1000,1000000,1],
  [10000,100000,1],
  [100000,10000,1],
  [1000000,1000,1],
  [10000000,100,1],
  [10000000,100,1],
  [1000000,1000,1],
  [100000,10000,1],
  [10000,100000,1],
  [1000,1000000,1],
  [100,10000000,1],
]

cond = ARGV[0]
puts ""

def print_bench(tag,n,m,l,t)
  ops = n * m * l * 1e-9 / t
  printf "%s size=%d rept=%d loop=%d; %7.3f sec, %5.3f GFLOPS, %5.3f GB/s\n",
    tag, n, m, l, t, ops, ops*24
end

params.each do |n,m,l|
  a = NArray.new(n)
  b = NArray.new(n)
  c = nil

  s = Time.now
  m.times do
    c = a.add(b,l)
  end
  t = Time.now-s
  print_bench("basic:    ",n,m,l,t)

  if cond=="sse2" || cond=="all"
    s = Time.now
    m.times do
      c = a.add_sse2(b,l)
    end
    t = Time.now-s
    print_bench("sse2:     ",n,m,l,t)
  end

  if cond=="all"
    s = Time.now
    m.times do
      c = a.add_unroll(b,l)
    end
    t = Time.now-s
    print_bench("unrl:     ",n,m,l,t)
  #
    s = Time.now
    m.times do
      c = a.add_sse2(b,l)
    end
    t = Time.now-s
    print_bench("sse2_unrl:",n,m,l,t)
  end
end

puts ""
