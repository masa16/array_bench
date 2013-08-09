#! /bin/sh -v

rubys="
$HOME/local/ruby187p371/bin/ruby
$HOME/local/ruby193p392/bin/ruby
$HOME/local/ruby200p247/bin/ruby
"

for ruby in $rubys; do
  $ruby extconf.rb
  make clean
  make V=1
  $ruby -v bench.rb
done
