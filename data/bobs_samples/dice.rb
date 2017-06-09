dice=Array.new(6,0)
1000.times{
  dice[rand(6)]+=1
}
p dice
