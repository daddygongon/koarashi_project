class MyString < String
  attr_accessor :my_string
  def initialize(string)
    super()
    @my_string = string
  end

  def uppercase
    return "TEST"
  end
end

p tmp = String.new("test")
p tmp.uppercase

p "TEST".downcase
