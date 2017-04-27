require 'colorize'

def hello(name)
  puts "hello #{name}"
end

class Hello
  def initialize(name)
    @my_name = name
  end
  def world
    puts "Hello world, #{@my_name}.".red
  end
end

p name = Dir.pwd.split('/')[2]
#bob = Hello.new(name)
bob = Hello.new(name)
bob.world
