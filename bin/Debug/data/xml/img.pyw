import sys

f = open('img.xml','r+')

d = f.readlines()

f.seek(0)

name = 'placeholder'
path = 'placeholder.png'

if len(sys.argv)==1 :
    sys.exit()
elif len(sys.argv)>1 :
    name = sys.argv[1]
    path = name+'.png'

if len(sys.argv)>2 :
    path = sys.argv[2]

for i in d:
    if i != "</root>":
        f.write(i)
    else:
        f.write("\t<img>\n")
        f.write("\t\t<name>"+name+"</name>\n")
        f.write("\t\t<path>"+path+"</path>\n")
        f.write("\t</img>\n")
        f.write(i)
f.truncate()
f.close()
