#data = File.open('mem.txt', 'r'){|file| file.read }.split("\n")[3..].map{|x| x[9..]}.join("").split("")

data = data.split("\n").join("").split("")
new_data = []
for i in (0..(data.size/2)) do
    new_elem = [data[i*2], data[i*2+1]].join("").to_i(16).to_s
    new_data << new_elem
end

new_data = new_data.join("\n")
File.open('output.txt', 'w'){|file| file.write(new_data)}
